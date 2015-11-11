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

  TRandom rnd;

  // Default variables
  // time shift in ns
  int shift = 0;
  // number of events to generate
  int nEventsTotal = 1000;
  // number of samples per impulse
  int NSAMPLES = 10;
  // number of samples per impulse
  float NFREQ = 25;
  // number of pile up
  float nPU = 0;
  // signal amplitude in GeV
  float signalAmplitude = 10.0;
  // Noise level (GeV)
  float sigmaNoise = 0.044;
  // PU Scale factor
  float puFactor = 1;
  // total number of bunches in "LHC" bunch train
  int NBXTOTAL = 2800;
  // ADD WHAT THIS IS
  const float eta = 0.0;

  // Changing variables if passed in on the command line
  if (argc>=2) shift = atoi(argv[1]);
  if (argc>=3) nEventsTotal = atoi(argv[2]);
  if (argc>=4) NSAMPLES = atoi(argv[3]);
  if (argc>=5) NFREQ = atof(argv[4]);
  if (argc>=6) nPU = atof(argv[5]);
  if (argc>=7) signalAmplitude = atof(argv[6]);
  if (argc>=8) sigmaNoise = atof(argv[7]);
  if (argc>=9) puFactor = atof(argv[8]);
 
  int IDSTART = 7*25;
  int WFLENGTH = 500*4; // step 1/4 ns in waveform
  if (( IDSTART + NSAMPLES * NFREQ ) > 500 ) {
    WFLENGTH = (IDSTART + NSAMPLES * NFREQ)*4 + 100;
  }

  std::cout << " NSAMPLES = " << NSAMPLES << std::endl;
  std::cout << " NFREQ = " << NFREQ << std::endl;
  std::cout << " nPU = " << nPU << std::endl;
  std::cout << " signalAmplitude = " << signalAmplitude << std::endl;
  std::cout << " Generation of digitized samples " << std::endl;
  std::cout << " WFLENGTH = " << WFLENGTH << std::endl;
  std::cout << " nEventsTotal = " << nEventsTotal << std::endl;
 
  Pulse pSh;
  pSh.SetNSAMPLES(NSAMPLES);
  pSh.SetNFREQ(NFREQ);
  pSh.SetIDSTART(IDSTART);
  pSh.SetWFLENGTH(WFLENGTH);
  pSh.Init();
 
  // make sure these inputs are what you really want
  TFile *file = new TFile("data/EmptyFileCRRC43.root");
  TString filenameOutput = 
          Form("input/mysample_%d_%d_%d_%.2f_%.2f_%.2f_%.2f_%.2f.root", 
          nEventsTotal, shift, NSAMPLES, NFREQ, signalAmplitude, nPU, sigmaNoise, puFactor);
  TFile *fileOut = new TFile(filenameOutput.Data(),"recreate");
 
  // Get PDF for pileup
  int indx = 10 * fabs(eta) / 0.1;
  if( indx < 0 )  indx = 0;
  if( indx > 13 ) indx = 13;
  char hname[120];
  sprintf(hname,"PileupPDFs/pupdf_%d",indx);
  TH1D *pupdf = (TH1D*)file->Get(hname);
  pupdf->SetDirectory(0);

  // Variables for filling the tree
  int nSmpl = NSAMPLES;
  float nFreq = NFREQ;
  double amplitudeTruth;
  std::vector<double> samples;
  int BX0;
  int nBX = NBXTOTAL;
  int nWF = WFLENGTH;
  std::vector<int> nMinBias;
  std::vector<double> energyPU; // along a complete LHC circle
  std::vector<double> waveform;
  double signalTruth = signalAmplitude;

  // Making the tree
  TTree *treeOut = new TTree("Samples", "");
  treeOut->Branch("shift",          &shift,           "shift/I");
  treeOut->Branch("nSmpl",          &nSmpl,           "nSmpl/I");
  treeOut->Branch("nFreq",          &nFreq,           "nFreq/F");
  treeOut->Branch("amplitudeTruth", &amplitudeTruth,  "amplitudeTruth/D");
  treeOut->Branch("samples",        &samples );
  treeOut->Branch("nPU",            &nPU,             "nPU/F");
  treeOut->Branch("BX0",            &BX0,             "BX0/I");
  treeOut->Branch("nBX",            &nBX,             "nBX/I");
  treeOut->Branch("nWF",            &nWF,             "nWF/I");
  treeOut->Branch("nMinBias",       &nMinBias );
  treeOut->Branch("energyPU",       &energyPU);
  treeOut->Branch("waveform",       &waveform );
  treeOut->Branch("signalTruth",    &signalTruth,     "signalTruth/D");
  treeOut->Branch("sigmaNoise",     &sigmaNoise,      "sigmaNoise/F");
  treeOut->Branch("puFactor",       &puFactor,        "puFactor/F");

  for (int ievt = 0; ievt < nEventsTotal; ievt++) {
    if (!(ievt%10)) {
      std::cout << " ievt = " << ievt << " :: " << nEventsTotal << std::endl;
    }

    nMinBias.clear();
    energyPU.clear();
    for (int ibx = 0; ibx < nBX; ibx++) {
      // number of min-bias interactions in each bunch crossing
      nMinBias.push_back(rnd.Poisson(nPU));
   
      // total energy per BX
      energyPU.push_back(0.);
      for (int imb = 0; imb < nMinBias.at(ibx); imb++) {
        energyPU.at(ibx) += pow(10., pupdf->GetRandom());
      }

      // pick in-time BX
      BX0 = int(nBX * rnd.Rndm());
      while (BX0 > (nBX-3*NSAMPLES) || BX0 < (3*NSAMPLES)) { // ---- 15 or 11 ?
        BX0 = int(nBX * rnd.Rndm());
      }
    }

    // Initialize the Waveform to be zero everwhere
    waveform.clear();
    for (int iwf = 0; iwf < nWF; iwf++) {
      waveform.push_back(0.);
    }

    // Add pileup to the waveform
    // time window is nWF ns wide and is centered at BX0
    for (int ibx = 0; ibx < nBX; ibx++) {
      for (int iwf = 0; iwf < nWF; iwf++) {
        double t = (BX0 - ibx) * 25. + iwf/4. - (500 / 2) + 25.;
        double temp = waveform.at(iwf);
        // adding the pu times the scale factor to the waveform
        waveform.at(iwf) = temp + energyPU.at(ibx) * pSh.fShape(t) * puFactor;
      }
    }

    // Add signal to the waveform
    for (int iwf = 0; iwf < nWF; iwf++) {
      waveform.at(iwf) += signalTruth * pSh.fShape(iwf/4. - (500 / 2) + 25.);
    }

    // Construct the digitized points
    std::vector<double> samplesUncorrelated;
    for (int i=0; i < NSAMPLES; ++i) {
      samplesUncorrelated.push_back(rnd.Gaus(0,1));
    }

    // Noise correlations
    samples.clear();
    for (int i=0; i < NSAMPLES; ++i) {
      samples.push_back(0);
      for(int j=0; j < NSAMPLES; ++j){
        samples.at(i) += pSh.cholesky(i,j) * samplesUncorrelated.at(j);
      }
    }

    for (int i=0; i < NSAMPLES; ++i) {
      samples.at(i) *= sigmaNoise;
    }

    // Add signal (that includes already the pileup!)
    for (int i=0; i < NSAMPLES; ++i) {
      int index = 4*(IDSTART + i * NFREQ - shift);
      samples.at(i) += waveform.at(index);
    }    

    // Adding energyPU to the true amplitude
    amplitudeTruth = signalTruth + energyPU.at(BX0);

    treeOut->Fill();
  }

  treeOut->Write();
  fileOut->Close();
  file->Close();

  std::cout << " output file = " << filenameOutput.Data() << std::endl;
}
