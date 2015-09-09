//
// MultiFit amplitude reconstruction
// To run:
// > g++ -o Example07.multifit.exe Example07.multifit.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
// > ./Example07.multifit.exe
//

#include <iostream>
#include "PulseChiSqSNNLS.h"
#include "Pulse.h"

#include "TTree.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2.h"
#include "TFile.h"
 
Pulse pSh;

FullSampleVector fullpulse(FullSampleVector::Zero());
FullSampleMatrix fullpulsecov(FullSampleMatrix::Zero());
SampleMatrix noisecor(SampleMatrix::Zero());
BXVector activeBX;
SampleVector amplitudes(SampleVector::Zero());

TFile *fout;
TH1D *h01;

std::vector<TH1F*> v_pulses;

//---- transform the pulse into an histogram
TH1F* CreateHistoShape( SampleVector& sam, int itime) {
 TString name = Form ("h_%d",itime);
 TH1F* temphist = new TH1F(name.Data(),"",sam.rows(),0,sam.rows());
 
 for (int i=0; i<sam.rows(); i++) {
  temphist->SetBinContent(i+1, sam[i]);
 }
 
 return temphist;
}


//---- initialize histograms results
void initHist() {
  fout = new TFile("output.root","recreate");
  h01 = new TH1D("h01", "dA", 1000, -5.0, 5.0);
}

void init() {
  initHist();

  // intime sample is [2]
  double pulseShapeTemplate[NSAMPLES+2];
  for(int i=0; i<(NSAMPLES+2); i++){
    double x = double( IDSTART + NFREQ * (i + 3) - WFLENGTH / 2);
    pulseShapeTemplate[i] = pSh.fShape(x);
  }
  //  for(int i=0; i<(NSAMPLES+2); i++) pulseShapeTemplate[i] /= pulseShapeTemplate[2];
  for (int i=0; i<(NSAMPLES+2); ++i) fullpulse(i+7) = pulseShapeTemplate[i];

  
  for (int i=0; i<NSAMPLES; ++i) {
    for (int j=0; j<NSAMPLES; ++j) {
      int vidx = std::abs(j-i);
      noisecor(i,j) = pSh.corr(vidx);
    }
  }

  
  
//   int activeBXs[] = { -5, -4, -3, -2, -1,  0,  1,  2,  3,  4 };
  int activeBXs[100];
  if (NSAMPLES == 10) {
   for (unsigned int ibx=0; ibx<NSAMPLES; ++ibx) {
    activeBXs[ibx] = ibx - NSAMPLES/2;
    std::cout << " activeBXs[" << ibx << "] = " << activeBXs[ibx] << std::endl;
   }
  }
  else {
   for (unsigned int ibx=0; ibx<10; ++ibx) {
    activeBXs[ibx] = 2*ibx - NSAMPLES/2;
    std::cout << " activeBXs[" << ibx << "] = " << activeBXs[ibx] << std::endl;
   }
  }
  
  
  //---- whatever the sampling, it is always 10!
  activeBX.resize(10);
  for (unsigned int ibx=0; ibx<10; ++ibx) {
    activeBX.coeffRef(ibx) = activeBXs[ibx];
  } 
  
  std::cout << " end init " << std::endl;
  //  activeBX.resize(1);
  //  activeBX.coeffRef(0) = 0;
}



void run(std::string inputFile) {
 
 TFile *file2 = new TFile(inputFile.c_str());
 //  TFile *file2 = new TFile("data/samples_signal_10GeV_eta_0.0_pu_140.root");
 
 double samples[NSAMPLES];
 double amplitudeTruth;
 TTree *tree = (TTree*)file2->Get("Samples");
 tree->SetBranchAddress("amplitudeTruth",      &amplitudeTruth);
 tree->SetBranchAddress("samples",             samples);
 int nentries = tree->GetEntries();
 
 std::cout << " nentries = " << nentries << std::endl;
 std::cout << " NSAMPLES = " << NSAMPLES << std::endl;
 
 for(int ievt=0; ievt<nentries; ++ievt){
  tree->GetEntry(ievt);
  for(int i=0; i<NSAMPLES; i++){
   amplitudes[i] = samples[i];
  }
  
  v_pulses.push_back(CreateHistoShape(amplitudes, ievt));
  
  double pedval = 0.;
  double pedrms = 1.0;
  PulseChiSqSNNLS pulsefunc;
  
  pulsefunc.disableErrorCalculation();
  bool status = pulsefunc.DoFit( amplitudes, noisecor, pedrms, activeBX, fullpulse, fullpulsecov );
  double chisq = pulsefunc.ChiSq();
  
  unsigned int ipulseintime = 0;
  for (unsigned int ipulse=0; ipulse<pulsefunc.BXs().rows(); ++ipulse) {
   if (pulsefunc.BXs().coeff(ipulse)==0) {
    ipulseintime = ipulse;
    break;
   }
  }
  std::cout << "  >> ipulseintime = " << ipulseintime << std::endl;
  std::cout << "  >> status =       " << status << std::endl;
  
  double aMax = status ? pulsefunc.X()[ipulseintime] : 0.;
  //  double aErr = status ? pulsefunc.Errors()[ipulseintime] : 0.;
  
  h01->Fill(aMax - amplitudeTruth);
 }
 
 
 //---- save pulses
 fout->cd();
 fout->mkdir("samples");
 fout->cd("samples"); 
 for (int ievt = 0; ievt < v_pulses.size(); ievt++) {
  v_pulses.at(ievt)->Write();
 }
 
 std::cout << "  Mean of REC-MC = " << h01->GetMean() << " GeV" << std::endl;
 std::cout << "   RMS of REC-MC = " << h01->GetRMS()  << " GeV" << std::endl;
}

void saveHist() {
  fout->cd();
  h01->Write();
  fout->Close();
}



# ifndef __CINT__

int main(int argc, char** argv) {
 
 std::string inputFile = "data/samples_signal_10GeV_pu_0.root";
 if (argc>=2) {
  inputFile = argv[1];
 }
 
 init();
 run(inputFile);
 saveHist();
 return 0;
}

# endif


