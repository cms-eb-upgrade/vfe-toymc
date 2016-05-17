//
// Creates waveforms for photo-current in APD as a histogram. One histogram
// per event. Horizontal axis is in ns. Vertical axis is in microAmps.
//
// Assumes EB light output of 4.5 pe/MeV and APD gain 50
//
// The waveform is an average shape without photo-statistics fluctuations
//
// Suggestions for inputs:
//
//  - Set nPU=0, nEventsTotal=1, signalAmplitude=1.0
//    You will get one histogram with signal shape of 0.036 pC total charge (1 GeV)
//
//  - Set nPU=200, nEventsTotal=5, signalAmplitude=0.0
//    After ~1 min, you will get 5 histograms with PU=200 events
//
//  - Adjust time window, time step, and other inputs as you wish
//
// suggestions to run:
// - change inputs (see above)
// - run command:
//   > root -l -q PhotoCurrent.C+
// 
//

#include "Pulse.h"
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TRandom.h>
#include <iostream>


void PhotoCurrent()
{
  //-------------------------------------------------------
  // make sure these inputs are what you really want
  //------------------------------------------------------
  
  const TString fileOutput      = "output.root";    // where waveforms will be saved
  const int     nPU             = 200;              // PileUp level
  const int     nEventsTotal    = 5;                // How many events to generate
  const float   eta             = 1.4;              // eta of EB channel
  const float   signalAmplitude = 0.0;              // in GeV
  const double  timeStep        = 0.05;             // in ns (no need to go below 0.02)
  const double  timeMin         = -2000.0;          // in ns 
  const double  timeMax         =  500.0;           // in ns

  //------------ end of user inputs ---------------------
  
  const TString fileInput       = "data/EmptyFilePhotoCurrent.root";
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

  Pulse pSh(fileInput);

   
  // Output file will be created

  TFile *fileOut = new TFile(fileOutput.Data(),"recreate");

  const int nBinsWF = int( fabs(timeMax - timeMin) / timeStep );
  TH1D *hWaveform[nEventsTotal];


  int    BX0;
  int    nBX    = NBXTOTAL;
  int    nMinBias[NBXTOTAL];
  double energyPU[NBXTOTAL];
  
  for(int ievt = 0; ievt < nEventsTotal; ievt++){

    for(int ibx = 0; ibx < nBX; ibx++){

      // number of min-bias interactions in each bunch crossing
      
      nMinBias[ibx] = rnd.Poisson(nPU);

      // total energy per BX

      energyPU[ibx] = 0.;
      for(int imb = 0; imb < nMinBias[ibx]; imb++){
	energyPU[ibx] += pow(10., pupdf->GetRandom());
      }

      // pick in-time BX

      BX0 = int(nBX * rnd.Rndm());
    }

    char histName[120];
    sprintf(histName,"hWaveform_%d",ievt);
    hWaveform[ievt] = new TH1D(histName,"waveform for photo-current", nBinsWF, timeMin, timeMax);


    // add pileup to the waveform
    
    int ibxMax = TMath::Min( BX0+int(timeMax/25.)+1, nBX );
    for(int ibx = 0; ibx < ibxMax; ibx++){
      for(int iwf = 1; iwf <= nBinsWF; iwf++){
	double t = (BX0 - ibx) * 25. + hWaveform[ievt]->GetBinCenter(iwf);
	hWaveform[ievt]->Fill( hWaveform[ievt]->GetBinCenter(iwf), energyPU[ibx] * pSh.fShape(t) );
      }
    }

    // add signal to the waveform

    for(int iwf = 1; iwf <= nBinsWF; iwf++){
      hWaveform[ievt]->Fill( hWaveform[ievt]->GetBinCenter(iwf),
			     signalAmplitude * pSh.fShape( hWaveform[ievt]->GetBinCenter(iwf) ) );
    }

    hWaveform[ievt]->Write();
  }

  fileOut->Close();
  file->Close();
}

