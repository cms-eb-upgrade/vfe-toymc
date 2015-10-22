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

int main(int argc, char** argv) {
  
 int shift = 0;
 //----  time shift in ns
 if (argc>=2) {
  shift = atoi(argv[1]);
 }
 
 //---- number of events to generate
 int nEventsTotal = 100;
 if (argc>=3) {
  nEventsTotal = atoi(argv[2]);
 }
 
 //---- number of samples per impulse
 int NSAMPLES = 10;
 if (argc>=4) {
  NSAMPLES = atoi(argv[3]);
 }
 std::cout << " NSAMPLES = " << NSAMPLES << std::endl;
 
 //---- number of samples per impulse
 float NFREQ = 25;
 if (argc>=5) {
  NFREQ = atof(argv[4]);
 }
 std::cout << " NFREQ = " << NFREQ << std::endl;
 
 
 //---- number of pile up
 float nPU = 0;
 if (argc>=6) {
  nPU = atof(argv[5]);
 }
 std::cout << " nPU = " << nPU << std::endl;
  
 
 //---- signal amplitude in GeV
 float signalAmplitude = 10.0;
 if (argc>=7) {
  signalAmplitude = atof(argv[6]);
 }
 std::cout << " signalAmplitude = " << signalAmplitude << std::endl;
 
 
 
 
 std::cout << " Generation of digitized samples " << std::endl;
 
 
 TRandom rnd;
  
 
 
 TString filenameOutput;
//  if (nPU == 0)  filenameOutput = Form("mysample_%d_%d_%d_%.2f_%.2f.root", nEventsTotal, shift, NSAMPLES, NFREQ, signalAmplitude); 
//  else           filenameOutput = Form("mysample_%d_%d_%d_%.2f_%.2f_%.2f.root", nEventsTotal, shift, NSAMPLES, NFREQ, signalAmplitude, nPU); 
//  filenameOutput = Form("mysample_%d_%d_%d_%.2f_%.2f_%.2f.root", nEventsTotal, shift, NSAMPLES, NFREQ, signalAmplitude, nPU); 
 filenameOutput = Form("input/mysample_%d_%d_%d_%.2f_%.2f_%.2f.root", nEventsTotal, shift, NSAMPLES, NFREQ, signalAmplitude, nPU); 
 
 Pulse pSh;
 pSh.SetNSAMPLES(NSAMPLES);
 pSh.SetNFREQ(NFREQ);
 
 
//  int IDSTART = 180;
 int IDSTART = 7*25;
 //  IDSTART = 180 +  NFREQ * 
 int WFLENGTH = 500*4; //---- step 1/4 ns in waveform
  
 if (( IDSTART + NSAMPLES * NFREQ ) > 500 ) {
  WFLENGTH = (IDSTART + NSAMPLES * NFREQ)*4 + 100;
 }
 
 std::cout << " WFLENGTH = " << WFLENGTH << std::endl;
 
 pSh.SetIDSTART(IDSTART);
 pSh.SetWFLENGTH(WFLENGTH);
 pSh.Init();
 
 
 
 // Noise level (GeV)
 float sigmaNoise = 0.044;
 
 
 // total number of bunches in "LHC" bunch train
 int NBXTOTAL = 2800;
//  int NBXTOTAL = 100;
 
 
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
//  const int     nPU             = 0;
 const float   eta             = 0.0;
//  const float   signalAmplitude = 10.0;
 
 TFile *file = new TFile(fileInput.Data());
 
 // Get PDF for pileup
 int indx = 10 * fabs(eta) / 0.1;
 if( indx < 0 )  indx = 0;
 if( indx > 13 ) indx = 13;
 char hname[120];
 sprintf(hname,"PileupPDFs/pupdf_%d",indx);
 TH1D *pupdf = (TH1D*)file->Get(hname);
 pupdf->SetDirectory(0);
 
 
 // Output file will be created
 int    BX0;
 std::vector<double> energyPU; //---- along a complete LHC circle
 
 // output samples
 
 int nSmpl = NSAMPLES;
 float nFreq = NFREQ;
 //  double samples[NSAMPLES];
 std::vector<double> samples;
 double amplitudeTruth;
 TFile *fileOut = new TFile(filenameOutput.Data(),"recreate");
 TTree *treeOut = new TTree("Samples", "");
 
 //---- to be given to reconstruction step
 treeOut->Branch("nSmpl",             &nSmpl,               "nSmpl/I");
 treeOut->Branch("nFreq",             &nFreq,               "nFreq/F");
 treeOut->Branch("amplitudeTruth",    &amplitudeTruth,      "amplitudeTruth/D");
 treeOut->Branch("samples",           &samples );
 treeOut->Branch("nPU",         &nPU,         "nPU/F");
 
  
 
 // Add branches
 int    nBX    = NBXTOTAL;
 std::vector<int> nMinBias;
 int    nWF    = WFLENGTH;
 std::vector<double> waveform;
 double signalTruth;
 
 //---- support, super-fine sampling
 treeOut->Branch("BX0",         &BX0,         "BX0/I");
 treeOut->Branch("nBX",         &nBX,         "nBX/I");
 treeOut->Branch("nWF",         &nWF,         "nWF/I");
 treeOut->Branch("nMinBias",    &nMinBias );
 treeOut->Branch("energyPU",    &energyPU);
 treeOut->Branch("waveform",    &waveform );
 treeOut->Branch("signalTruth", &signalTruth, "signalTruth/D");
 
 std::cout << " nEventsTotal = " << nEventsTotal << std::endl;
 
 for(int ievt = 0; ievt < nEventsTotal; ievt++) {
  if (!(ievt%10)) {
   std::cout << " ievt = " << ievt << " :: " << nEventsTotal << std::endl;
  }
  
  nMinBias.clear();
  energyPU.clear();
  for(int ibx = 0; ibx < nBX; ibx++) {
//    std::cout << " ibx = " << ibx << " :: " << nBX << std::endl;
   // number of min-bias interactions in each bunch crossing
   
   nMinBias.push_back( rnd.Poisson(nPU) );
   
   // total energy per BX
   
   energyPU.push_back(0.);
   for(int imb = 0; imb < nMinBias.at(ibx); imb++){
    energyPU.at(ibx) += pow(10., pupdf->GetRandom());
   }
   
   // pick in-time BX
   
   BX0 = int(nBX * rnd.Rndm());
   while (BX0 > (nBX-3*NSAMPLES) || BX0 < (3*NSAMPLES) ) { // ---- 15 or 11 ?
    BX0 = int(nBX * rnd.Rndm());
   }
  }
    
  // waveform, initialize
  waveform.clear();
  for(int iwf = 0; iwf < nWF; iwf++){
   waveform.push_back(0.);
  }
  
  // add pileup to the waveform
  // time window is nWF ns wide and is centered at BX0
  
//   int ibxMax = min( BX0+11, nBX );
//   for(int ibx = 0; ibx < ibxMax; ibx++){

  for(int ibx = 0; ibx < nBX; ibx++){
//    std::cout << " ibx = " << ibx << " :: " << nBX << " BX0 = " << BX0 << std::endl;
   for(int iwf = 0; iwf < nWF; iwf++){
    double t = (BX0 - ibx) * 25. + iwf/4. - (500 / 2) + 25.; //----> + 25 or not?   ---> 500 fixed here!
//     double t = (BX0 - ibx) * 25. + iwf - (nWF / 2) + 25.; //----> + 25 or not?
    //     double t = (BX0 - ibx) * 25. + iwf - (nWF / 2); //----> + 25 or not?
    
//     if (t<1000) std::cout << " ---> t = " << t << " iwf = " << iwf << " BX0 = " << BX0 << " ibx = " << ibx << " energyPU.size() = " << energyPU.size() << " energyPU.at(ibx) = " << energyPU.at(ibx) ;
    double temp = waveform.at(iwf);
//     if (t<1000) std::cout << " ciao " ;
//     if (t<1000) std::cout << " pSh.fShape(t) = " << pSh.fShape(t) ;
    
    waveform.at(iwf) = temp + energyPU.at(ibx) * pSh.fShape(t);
//     if (t<1000) std::cout << " -> done " << std::endl;
   }
  }
//   std::cout << " done " << std::endl;
  
  // save MC truth for signal
  
  signalTruth = signalAmplitude;
  

  // add signal to the waveform
  for(int iwf = 0; iwf < nWF; iwf++){
//    waveform.at(iwf) += signalTruth * pSh.fShape(iwf - 25. * 3);
//    waveform.at(iwf) += signalTruth * pSh.fShape(iwf - (nWF / 2) + 25.); //----> + 25 or not?
   waveform.at(iwf) += signalTruth * pSh.fShape(iwf/4. - (500 / 2) + 25.); //----> + 25 or not?  ---> 500 fixed here !!!
   //    waveform.at(iwf) += signalTruth * pSh.fShape(iwf - (nWF / 2) ); //----> + 25 or not?
  }
//   std::cout << " done " << std::endl;
  
  
  
  
  
  //---- construct the digitized points
  std::vector<double> samplesUncorrelated;
  
  for(int i=0; i<NSAMPLES; ++i){
   samplesUncorrelated.push_back(rnd.Gaus(0,1));
  }
  
  // Noise correlations
  samples.clear();
  for(int i=0; i<NSAMPLES; ++i){
   samples.push_back(0);
   for(int j=0; j<NSAMPLES; ++j){
//     std::cout << " pSh.cholesky(" << i << "," << j << ") = " << pSh.cholesky(i,j) << std::endl;
    samples.at(i) += pSh.cholesky(i,j) * samplesUncorrelated.at(j);
   }
  }
  
  
  for(int i=0; i<NSAMPLES; ++i){
   samples.at(i)   *= sigmaNoise;
  }
  
  // add signal (that includes already the pileup!)
  for(int i=0; i<NSAMPLES; ++i){
//    int index = IDSTART + int(i * NFREQ) - shift; //---- time shift "-" on function
   int index = 4*(IDSTART + i * NFREQ - shift); //---- time shift "-" on function
//    std::cout << " index = " << index << std::endl;
   samples.at(i)   += waveform.at(index);
  }    
  
  // true amplitude = in-time pileup + signal
  amplitudeTruth = signalTruth + energyPU.at(BX0);
  
//   std::cout << " done 3 " << std::endl;
  
//   for(int i=0; i<NSAMPLES; ++i){
//    std::cout << "  samples.at(" << i << ") = " <<  samples.at(i) << std::endl;   
//   }
  
  treeOut->Fill();
  
  
 }
 
 treeOut->Write();
 fileOut->Close();
 file->Close();
 
 std::cout << " output file = " << filenameOutput.Data() << std::endl;
 
}
