//
// Takes waveforms from the "file".
// Creates NSAMPLES starting with IDSTART time with the step of NFREQ ns
// Applies noise (correlated) for each sample
// Stores samples and true in-time amplitude
// To run:
// > root -l -q CreateData.C+
//


#include "Pulse.h"
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TProfile.h>
#include <TF1.h>
#include <TGraph.h>
#include <TRandom.h>
#include <TMath.h>
#include <iostream>



#include <TRandom.h>

//----                                                                                time shift in ns
// void CreateData(std::string nameInputFile = "data/waveform_signal_10GeV_pu_0.root", int shift = 0) {
void CreateData(int shift = 0) {
  
 TRandom rnd;
 
 Pulse pSh;
 
 TString filenameOutput = Form("mysample_%d.root", shift); 
 
 // Noise level (GeV)
 double sigmaNoise = 0.044;
 
 int NSAMPLES = 10;
 int WFLENGTH = 500;
 int NFREQ = 25;
 int IDSTART = 180;
 
 
 //
 // Takes existing "empty" file. Uses pileup PDFs from "empty"
 // file. Creates TTree "Events" and fills each of BX in a bunch train
 // with the following info:
 //  - number of minbias interactions
 //  - total energy from PU
 //  - MC truth for signal amplitude
 //  - waveform of PU + Signal
 //
 // Also, picks randomly the in-time BX
 //
 // suggestions to run:
 // - change inputs (see below)
 // - run command:
 //   > root -l -q Example02.C+
 // 
 //
  
  // make sure these inputs are what you really want
  
  const TString fileInput       = "data/EmptyFileCRRC43.root";
  const TString fileOutput      = "output.root";
  const int     nPU             = 0;
  const int     nEventsTotal    = 100;
  const float   eta             = 0.0;
  const float   signalAmplitude = 10.0;
  
  TFile *file = new TFile(fileInput.Data());
  
  // Get PDF for pileup
  int indx = 10 * fabs(eta) / 0.1;
  if( indx < 0 )  indx = 0;
  if( indx > 13 ) indx = 13;
  char hname[120];
  sprintf(hname,"PileupPDFs/pupdf_%d",indx);
  TH1D *pupdf = (TH1D*)file->Get(hname);
  pupdf->SetDirectory(0);
  
  // Get the Pulse Shape
  Pulse pSh;
  
  // Output file will be created
  TFile *fileOut = new TFile(fileOutput.Data(),"recreate");
  TTree *tree = new TTree("Waveforms", "");
  int    BX0;
  std::vector<double> energyPU; //---- along a complete LHC circle
  
  // output samples
  
  int nSmpl = NSAMPLES;
  int nFreq = NFREQ;
  //  double samples[NSAMPLES];
  std::vector<double> samples;
  double amplitudeTruth;
  TFile *fileOut = new TFile(filenameOutput.Data(),"recreate");
  TTree *treeOut = new TTree("Samples", "");
  
  treeOut->Branch("nSmpl",             &nSmpl,               "nSmpl/I");
  treeOut->Branch("nFreq",             &nFreq,               "nFreq/I");
  treeOut->Branch("amplitudeTruth",    &amplitudeTruth,      "amplitudeTruth/D");
  treeOut->Branch("samples",           &samples,             "samples/D");
  
  
  // total number of bunches in "LHC" bunch train
  int NBXTOTAL = 2800;
  
  
  // Add branches
  int    nBX    = NBXTOTAL;
  std::vector<int> nMinBias;
  std::vector<double> energyPU;
  int    nWF    = WFLENGTH;
  std::vector<double> waveform;
  double signalTruth;
  
  tree->Branch("BX0",         &BX0,         "BX0/I");
  tree->Branch("nBX",         &nBX,         "nBX/I");
  tree->Branch("nWF",         &nWF,         "nWF/I");
  tree->Branch("nMinBias",    &nMinBias,    "nMinBias");
  tree->Branch("energyPU",    &energyPU,    "energyPU");
  tree->Branch("waveform",    &waveform,    "waveform");
  tree->Branch("signalTruth", &signalTruth, "signalTruth/D");
  
  
  for(int ievt = 0; ievt < nEventsTotal; ievt++){
   nMinBias.clear();
   energyPU.clear();
   for(int ibx = 0; ibx < nBX; ibx++){
    
    // number of min-bias interactions in each bunch crossing
    
    nMinBias.push_back( rnd.Poisson(nPU) );
    
    // total energy per BX
    
    energyPU.push_back( 0. );
    for(int imb = 0; imb < nMinBias[ibx]; imb++){
     energyPU.at(ibx) += pow(10., pupdf->GetRandom());
    }
    
    // pick in-time BX
    
    BX0 = int(nBX * rnd.Rndm());
   }
   
   // waveform, initialize
   waveform.clear();
   for(int iwf = 0; iwf < nWF; iwf++){
    waveform.push_back(0.);
   }
   
   // add pileup to the waveform
   // time window is nWF ns wide and is centered at BX0
   
   int ibxMax = min( BX0+11, nBX );
   for(int ibx = 0; ibx < ibxMax; ibx++){
    for(int iwf = 0; iwf < nWF; iwf++){
     double t = (BX0 - ibx) * 25. + iwf - (nWF / 2);
     waveform.at(iwf) += energyPU.at(ibx) * pSh.fShape(t);
    }
   }
   
   // save MC truth for signal
   
   signalTruth = signalAmplitude;
   
   // add signal to the waveform
   
   for(int iwf = 0; iwf < nWF; iwf++){
    waveform.at(iwf) += signalTruth * pSh.fShape(iwf - (nWF / 2));
   }
   
   
   
   
   
   
   
   tree->Fill();
  }
  
  tree->Write();
  fileOut->Close();
  file->Close();
 }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 // input Waveforms
 
 TFile *file = new TFile(nameInputFile.c_str());
 int    BX0;
 int    nWF;
//  double waveform[WFLENGTH];
 std::vector<double> waveform;
//  double energyPU[NBXTOTAL]; //---- along a complete LHC circle
 std::vector<double> energyPU; //---- along a complete LHC circle
 double signalTruth;
 TTree *tree = (TTree*)file->Get("Waveforms");
 tree->SetBranchAddress("nWF",          &nWF);
 tree->SetBranchAddress("waveform",     &waveform);
 tree->SetBranchAddress("BX0",          &BX0);
 tree->SetBranchAddress("signalTruth",  &signalTruth);
 tree->SetBranchAddress("energyPU" ,    &energyPU);
 
 // output samples
 
 int nSmpl = NSAMPLES;
 int nFreq = NFREQ;
//  double samples[NSAMPLES];
 std::vector<double> samples;
 double amplitudeTruth;
 TFile *fileOut = new TFile(filenameOutput.Data(),"recreate");
 TTree *treeOut = new TTree("Samples", "");
 
 treeOut->Branch("nSmpl",             &nSmpl,               "nSmpl/I");
 treeOut->Branch("nFreq",             &nFreq,               "nFreq/I");
 treeOut->Branch("amplitudeTruth",    &amplitudeTruth,      "amplitudeTruth/D");
 treeOut->Branch("samples",           &samples,             "samples/D");
 
 int nentries = tree->GetEntries();
 
 std::cout << " nentries = " << nentries << std::endl;
 
 for(int ievt=0; ievt<nentries; ievt++){
  
  std::vector<double> samplesUncorrelated;
  
  for(int i=0; i<NSAMPLES; ++i){
   samplesUncorrelated.push_back(rnd.Gaus(0,1));
  }
  
  // Noise correlations
  for(int i=0; i<NSAMPLES; ++i){
   samples.push_back(0);
   for(int j=0; j<NSAMPLES; ++j){
    samples.at(i) += pSh.cholesky(i,j) * samplesUncorrelated.at(j);
   }
  }
  
  for(int i=0; i<NSAMPLES; ++i){
   samples.at(i)   *= sigmaNoise;
  }
  
  // add signal and pileup
  
  tree->GetEntry(ievt);
  for(int i=0; i<NSAMPLES; ++i){
   int index = IDSTART + i * NFREQ - shift; //---- time shift "-" on function
   samples.at(i)   += waveform.at(index);
  }    
  
  // true amplitude = in-time pileup + signal
  amplitudeTruth = signalTruth + energyPU.at(BX0);
  
  treeOut->Fill();
 }
 
 treeOut->Write();
 fileOut->Close();
 file->Close();
 
}
