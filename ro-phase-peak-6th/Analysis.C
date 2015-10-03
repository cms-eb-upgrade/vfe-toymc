#include "Analysis.h"
#include <TFile.h>
#include <TString.h>
#include <TGraph.h>
#include <TF1.h>
#include <TH1.h>
#include <TAxis.h>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;




void Debug()
{

  TString filename = "../data/EmptyFileCRRC43.root";
  double  tau  = 43.0;
  double  gate = 0.0;
  double  eta  = 0.0;

  psNow = new PulseShape(filename, tau, gate);
  puNow = new PileUp(filename, eta);
  wfNow = new WaveForm();

  int    nsamples     = 40;         // 10, 20, or 40
  double timeStep     = 6.25;       // time between samples in ns
  double peakPosition = 24.0;       // nominal position of the peak in sample units from 1 to nsamples
  
  dgNow = new Digi(nsamples, timeStep, peakPosition);

  double amplitude = 1.;
  double pileup    = 0.;
  wfNow->generate(amplitude, pileup);

  double noise   = 0.05;
  double offset  = 0.0;
  int    nevents = 1000;

  vector<double> dA;
  vector<double> dT;
  for(int ievt=0; ievt<nevents; ievt++){
    dgNow->generate(noise , offset);
    RecoFit  rf( 0.0, 1.0, (peakPosition - 1.) * timeStep );
    if( rf.status()==0 ){
      dA.push_back( rf.amplitude() - dgNow->energyTrue() );
      dT.push_back( rf.timing() - dgNow->timeTrue() );
    }
  }
  cout << dA.size() << endl;
}



void Analysis()
{
  Debug();
}
