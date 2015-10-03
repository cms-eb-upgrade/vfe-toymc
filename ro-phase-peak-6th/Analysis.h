#include <TRandom.h>
#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TTree.h>
#include <TGraph.h>
#include <iostream>



TRandom rnd;

class PulseShape;
class PileUp;
class WaveForm;
class Digi;
class RecoFit;

PulseShape *psNow;
PileUp     *puNow;
WaveForm   *wfNow;
Digi       *dgNow;

double funcPulseShape(double*, double*);

class PulseShape{

 public:

  PulseShape(TString filename = "data/EmptyFileCRRC43.root", double tau = 43.0, double gate = 0.);
  ~PulseShape();
  TString filename() const { return filename_; };
  double  tMin()     const { return tMin_; };
  double  fPar0()    const { return fPar0_; };
  double  fPar1()    const { return fPar1_; };
  double  tau()      const { return tau_; };
  double  gate()     const { return gate_; };
  double  fShape(double);
  
 private:
  
  TFile   *filePS_;
  TString  filename_;
  TGraph  *grPS_;
  double   tMin_;
  double   fPar0_;
  double   fPar1_;
  double   tau_;
  double   gate_;
};



PulseShape::PulseShape(TString filename, double tau, double gate)
{
  filename_  = filename;
  tau_       = tau;
  gate_      = gate;

  filePS_ = new TFile(filename_.Data());
  grPS_ = (TGraph*)filePS_->Get("PulseShape/grPulseShape");
  TTree *trPS = (TTree*)filePS_->Get("PulseShape/Tail");
  float tMin, fPar0, fPar1;
  trPS->SetBranchAddress("timeMin",      &tMin);
  trPS->SetBranchAddress("expAmplitude", &fPar0);
  trPS->SetBranchAddress("expTime",      &fPar1);
  trPS->GetEntry(0);
  tMin_  = tMin;
  fPar0_ = fPar0;
  fPar1_ = fPar1;
}



PulseShape::~PulseShape()
{
  delete grPS_;
  delete filePS_;
}



double PulseShape::fShape(double x)
{
  if( grPS_ !=0 && x > 0.){
    if(x<tMin_){
      return grPS_->Eval(x);
    }else{
      return fPar0_ * exp( -x * fPar1_ );
    }
  }else{
    return 0.;
  }
}




class PileUp{

 public:

  PileUp(TString filename =  "data/EmptyFileCRRC43.root", double eta = 0.0);
  ~PileUp();
  double  rndEnergy();
  TString filename()   const { return filename_; };
  double  eta()        const { return eta_; };

 private:

  TString filename_;
  double  eta_;
  TH1D*   pupdf_;
};


PileUp::PileUp(TString filename, double eta)
{
  eta_ = eta;
  filename_ = filename;

  int indx = 10 * fabs(eta) / 0.1;
  if( indx < 0 )  indx = 0;
  if( indx > 13 ) indx = 13;
  char hname[120];
  sprintf(hname,"PileupPDFs/pupdf_%d",indx);
  TFile *file = new TFile(filename.Data());
  pupdf_ = (TH1D*)file->Get(hname);
  pupdf_->SetDirectory(0);
  delete file;
}


PileUp::~PileUp()
{
  delete pupdf_;
}



double PileUp::rndEnergy()
{
   return pow(10., pupdf_->GetRandom());
}




class WaveForm{

 public:

  WaveForm();
  ~WaveForm();
  void       generate(double, double);
  double     wf(double);
  double     peak()      { return peak_; };
  double     amplitude() { return amplitude_; };
  
 private:
  
  TGraph     *grWF_;
  double      peak_;  
  double      amplitude_;
};


WaveForm::WaveForm()
{
  grWF_ = new TGraph();
  for(int iwf = 0; iwf < 501; iwf++){
    grWF_->SetPoint(iwf, iwf - 251, 0.);
  }

  double fmax = -1e+6;
  for(int iwf = 0; iwf < 501; iwf++){
    double x = iwf - 251;
    double y = psNow->fShape(x);
    if(y >= fmax){
      fmax = y;
      peak_ = x;
    }
  }      
  
};



WaveForm::~WaveForm()
{
  delete grWF_;
}



void WaveForm::generate(double signal, double pu)
{
  int    nMinBias[2800];
  double energyPU[2800];

  for(int ibx=0; ibx<2800; ibx++){
    energyPU[ibx] = 0.;	
    nMinBias[ibx] = 0;
  }

  if(pu>1e-3){
    for(int ibx = 0; ibx < 2800; ibx++){
      nMinBias[ibx] = rnd.Poisson(pu);
      for(int imb = 0; imb < nMinBias[ibx]; imb++){
	energyPU[ibx] += puNow->rndEnergy();
      }
    }
  }

  int bx0 = int(2800. * rnd.Rndm());

  double waveform[501];      
  for(int iwf = 0; iwf < 501; iwf++){
    waveform[iwf] = 0.;
  }
  int ibxMax = min( bx0+11, 2800 );
  for(int ibx = 0; ibx < ibxMax; ibx++){
    if(energyPU[ibx]>1e-9){
      for(int iwf = 0; iwf < 501; iwf++){
	double t = (bx0 - ibx) * 25. + double(iwf - 251);
	waveform[iwf] += energyPU[ibx] * psNow->fShape(t);
      }
    }
  }
  
  for(int iwf = 0; iwf < 501; iwf++){
    waveform[iwf] += signal * psNow->fShape(iwf - 251);
  }
  
  amplitude_ = signal + energyPU[bx0];
  
  for(int iwf = 0; iwf < 501; iwf++){
    grWF_->SetPoint(iwf, iwf - 251, waveform[iwf]);
  }      
  
}



double WaveForm::wf(double t)
{
  if(t<-249.9 || t>249.9){
    cout << " ERROR: waveform interpolation is outside of the range t=" << t << endl;
    return 0.;
  }else{
    return grWF_->Eval(t);
  }
}




class Digi{

 public:
  
  Digi(int nsamp = 10, double tstep = 25.0, double peakPos = 6.0);
  ~Digi();
  void generate(double, double);
  int    nsamples()      { return nsamples_; };
  double timeStep()      { return tstep_; };
  double sample(int i)   { return samples_[i]; };
  double timeTrue()      { return time_; };
  double energyTrue()    { return energy_; };
  
 private:
  
  int      nsamples_;
  double   tstep_;
  double   peakPos_;
  double   tstart_;
  double  *mC_;
  double **mL_;
  double  *samples_;
  double   time_;
  double   energy_;

};


Digi::Digi(int nsamp, double tstep, double peakPos)
{
  nsamples_ = nsamp;
  tstep_    = tstep;
  peakPos_  = peakPos;
  tstart_   = wfNow->peak() - (peakPos - 1.) * tstep;

  samples_ = new double [nsamples_];
  mC_ = new double [nsamples_];
  mL_ = new double *[nsamples_];
  for( int i = 0 ; i < nsamples_ ; i++ ) {
    mL_[i] = new double[nsamples_];
  }
  
  for(int i=0; i<nsamples_; i++){
    double y = 1. - exp( -double(tstep_ * i) / (sqrt(2.) * psNow->tau()));
    mC_[i] = 1. - y * y;
  }

  for(int i=0; i<nsamples_; ++i){
    for(int j=0; j<nsamples_; ++j){
      mL_[i][j]=0;
    }
  }

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



Digi::~Digi()
{
  free(samples_); samples_ = NULL;
  free(mC_); mC_ = NULL;

  for( int i = 0 ; i < nsamples_ ; i++ ) {
    delete [] mL_[i];
  }
  delete [] mL_;  
}


void Digi::generate(double noise, double offset)
{
  double samplesUncorrelated[nsamples_];
  for(int i=0; i<nsamples_; ++i){
    samplesUncorrelated[i] = rnd.Gaus(0,1);
  }
  for(int i=0; i<nsamples_; ++i){
    samples_[i]=0;
    for(int j=0; j<nsamples_; ++j){
      samples_[i] += mL_[i][j] * samplesUncorrelated[j];
    }
  }
  
  for(int i=0; i<nsamples_; ++i){
    samples_[i] *= noise;
    samples_[i] += wfNow->wf(tstart_ - offset + i * tstep_);
  }

  time_   = tstep_ * (peakPos_ - 1.0) + offset;
  energy_ = wfNow->amplitude();
}



double funcPulseShape( double *x, double *par )
{
  double t = x[0] - par[2];
  return par[0] + par[1] * psNow->fShape(t);
}



class RecoFit{

 public:
  
  RecoFit(double p = 0., double a = 1., double t = 100. );
  ~RecoFit();
  int status()            { return status_; };
  int statusFixed()       { return statusFixed_; };
  double pedestal()       { return pedestal_; };
  double amplitude()      { return amplitude_; };
  double timing()         { return timing_; };
  double pedestalFixed()  { return pedestalFixed_; };
  double amplitudeFixed() { return amplitudeFixed_; };

 private:

  int status_;
  int statusFixed_;
  double pedestal_;
  double amplitude_;
  double timing_;
  double pedestalFixed_;
  double amplitudeFixed_;
  
};


RecoFit::RecoFit(double p, double a, double t)
{
  TGraph *gr = new TGraph();
  for(int i=0; i<dgNow->nsamples(); i++){
    gr->SetPoint(i, i * dgNow->timeStep(), dgNow->sample(i) );
  }

  TF1 *funcFit = new TF1("funcFit",funcPulseShape, -1.*dgNow->timeStep(),
			 (dgNow->nsamples()+0.5)*dgNow->timeStep(), 3);
  funcFit->SetParameters(p, a, t - wfNow->peak());
  status_ = gr->Fit("funcFit","QW");
  pedestal_  = funcFit->GetParameter(0);
  amplitude_ = funcFit->GetParameter(1);
  timing_    = funcFit->GetParameter(2) + wfNow->peak();
  funcFit->FixParameter(2, t - wfNow->peak());
  statusFixed_ = gr->Fit("funcFit","QW");
  pedestalFixed_  = funcFit->GetParameter(0);
  amplitudeFixed_ = funcFit->GetParameter(1);  
}

RecoFit::~RecoFit()
{
}
