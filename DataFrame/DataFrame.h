#include <TRandom3.h>
#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TTree.h>
#include <TGraph.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

using namespace std;

TRandom rnd;
const int nbx_ = 2800;



class Orbit{

 public:
  Orbit(double);
  ~Orbit();
  unsigned int nbx()      const { return Npu_.size(); };
  int Npu(int i)          const { return Npu_[i]; };
  double Epu(int i)       const { return Epu_[i]; };
  void newOrbit(double,int,int);
  TH1D *hpdf()            const { return pupdf_; };

 private:

  vector<int> Npu_;
  vector<double> Epu_;
  TH1D *pupdf_;
};



Orbit::Orbit(double eta)
{
  int indx = 10 * fabs(eta) / 0.1;
  if( indx < 0 )  indx = 0;
  if( indx > 13 ) indx = 13;

  TFile *file = new TFile("pileupPDFs.root");
  char hname[120];
  sprintf(hname,"pupdf_%d",indx);
  pupdf_ = (TH1D*)file->Get(hname);
  pupdf_->SetDirectory(0);
  file->Close();
  
  Npu_.clear();
  Epu_.clear();
  for(int i=0; i<nbx_; i++){
    Npu_.push_back(0);
    Epu_.push_back(0);
  }  
}



Orbit::~Orbit()
{
}



void Orbit::newOrbit(double pu, int bxmin, int bxmax)
{
  if(bxmin<0) bxmin=0;
  if(bxmax>=int(Npu_.size())) bxmax=Npu_.size();
  for(unsigned int ibx=0; ibx<Npu_.size(); ibx++){
    Npu_[ibx] = 0;
    Epu_[ibx] = 0;
  }  
  if(pu>0){
    for(int ibx=bxmin; ibx<bxmax; ibx++){
      Npu_[ibx] = rnd.Poisson(pu);
      Epu_[ibx] = 0;
      for(int imb = 0; imb < Npu_[ibx]; imb++){
        Epu_[ibx] += pow(10., pupdf_->GetRandom());
      }
    }      
  }
}


//---------------------------


class Pulse{

 public:
  Pulse(int);
  ~Pulse();
  double norm()  const { return norm_; };
  void SetNorm(double x)   { norm_ = x; };
  double Value(double);


 private:
  TGraph *grPulse_;
  TF1    *fPulse_;
  double tMin_;
  double tLimit_;
  double tMax_;
  double norm_;
  double offset_;
};



Pulse::Pulse(int iopt)
{
  TFile *file = new TFile("TIA_ASIC_signal.root");
  if(iopt==0){
    grPulse_ = (TGraph*)file->Get("grTIA_Signal");
    fPulse_ = new TF1("fPulse_","[0]*exp(-x/[1])",0.,1e+6);
    fPulse_->SetParameters(2.21312e-05,1e+2);
    tMin_ = -0.4;
    tLimit_ = 200.;
    tMax_ = 1e+5;
    norm_ = 164.844;
    offset_ = 0;
  }else{
    grPulse_ = (TGraph*)file->Get("grTIA_Spike");
    fPulse_ = new TF1("fPulse_","[0]*exp(-pow((x-[1])/[2],2))",0.,1e+6);
    fPulse_->SetParameters(3.79488e-01,5.44116e+01,9.62779);
    tMin_ = -0.4;
    tLimit_ = 51.3750;
    tMax_ = 1e+5;
    norm_ = 0.0235474;
    offset_ = +0.95;
  }
  file->Close();
}



Pulse::~Pulse()
{
  delete fPulse_;
}




double Pulse::Value(double t)
{
  if(t < tMin_){
    return 0.;
  }else if(t < tLimit_){
    return grPulse_->Eval(t + offset_) * norm_;
  }else if(t < tMax_){
    return fPulse_->Eval(t + offset_) * norm_;
  }else{
    return 0.;
  }
}



//-------------------------------------


class Frame{

 public:
  Frame(int, double);
  ~Frame();
  int    nsamples()       const { return nsamples_; };
  int    imax()           const { return iMax_; };
  double mC(int i)        const { return mC_[i];    };
  double mL(int i, int j) const { return mL_[i][j]; };
  double noiseU(int i)    const { return noiseU_[i]; };
  double noiseC(int i)    const { return noiseC_[i]; };
  double timeLocal(int i) const { return tLocal_[i]; };
  double timeStep()       const { return tStep_; };
  void newFrame();
  void setLocal(int, double);
  
 private:
  int nsamples_;
  int iMax_;
  double tStep_;
  double tOffset_;
  vector<double> mC_;
  vector< vector<double> > mL_;
  vector<double> noiseU_;
  vector<double> noiseC_;
  vector<double> tLocal_;
};



Frame::Frame(int n, double s)
{
  nsamples_ = 10;
  if(n>10) nsamples_ = n;
  tStep_    = s;
  tOffset_  = 0;

  mC_.clear();
  mL_.clear();
  for(int i=0; i<nsamples_; i++){
    mC_.push_back(0);
    noiseU_.push_back(0);
    noiseC_.push_back(0);
    tLocal_.push_back(tOffset_ + tStep_ * i);
  }
  for(int i=0; i<nsamples_; i++){
    mL_.push_back(mC_);
  }
  
  // Noise correlation matrix.
  // grNC_200 is for dark current = 200 uA
  // other available graphs are for 0, 5, 10, 50 etc uA.
  
  TFile *file = new TFile("TIA_ASIC_noise_matrix.root");
  TGraph *gr = (TGraph*)file->Get("grNC_200");
  for(int i=0; i<nsamples_; i++){
    mC_[i] = gr->Eval(s*i);
  }
  file->Close();

  // decomposition
  
  mL_[0][0] = sqrt(mC_[0]);
  for( int col=1; col<nsamples_; col++){
    mL_[0][col]=0;
  }
  for( int row=1; row<nsamples_; row++){
    for( int col=0; col<row; col++ ){
      double sum1 = 0;
      int m=abs(row-col);
      for( int k=0; k<col; ++k) sum1 += mL_[row][k]*mL_[col][k];
      mL_[row][col] = (mC_[m] - sum1)/mL_[col][col];
    }
    double sum2 = 0;
    for( int k=0; k<row; ++k) sum2 += mL_[row][k]*mL_[row][k];
    mL_[row][row] = sqrt( mC_[0] - sum2 );
    for( int col=row+1; col<nsamples_; col++ ) mL_[row][col] = 0;
  }
  
}


Frame::~Frame()
{
}



void Frame::newFrame()
{
  for(int i=0; i<nsamples_; ++i){
    noiseU_[i] = rnd.Gaus(0,1);
  }
    
  // Noise correlations
  for(int i=0; i<nsamples_; ++i){
    noiseC_[i] = 0;
    for(int j=0; j<nsamples_; ++j){
      noiseC_[i] += mL_[i][j] * noiseU_[j];
    }
  }

}



void Frame::setLocal(int im, double toff)
{
  iMax_    = im;
  tOffset_ = toff;
  for(int i=0; i<nsamples_; i++){
    tLocal_[i] = tOffset_ + i * tStep_;
  }
}

