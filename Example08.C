//
// Example of APD spike simulation. Rate, time, amplitude of spikes
// are based on CMSSW simulation of Phase2 ECAL barrel
//
// > root -l Example08.C+
//
// It takes couple of minutes
//
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include <iostream>


TH2D *hpdf;
double fracTotal;


void LoadPDF()
{
  TFile *f = new TFile("data/apd_spike_pdf.root");
  hpdf = (TH2D*)f->Get("hpdf2D");
  hpdf->SetDirectory(0);
  TH1D *hnorm = (TH1D*)f->Get("hpdf1D");
  fracTotal = (hnorm->GetEntries() - hnorm->GetBinContent(1)) / hnorm->GetEntries();
  cout << " Fraction of EB channels with non-zero APD energy per MinBias = " << fracTotal << endl;
  return;
}


void Example08()
{
  double energyMin   = 3.0;              // minimum energy of APD spike in GeV
  int nPU            = 140;              // number of MinBias interactions per BX
  int nEvents        = 20;               // to generate

  int nCrystalsEB    = 61200;            
  double energyScale = 3.0 / 4500.;      // specific to CMSSW config files
  double counter     = 0.;
  
  LoadPDF();

  TH1D *h1 = new TH1D("h1", "Time of spikes (mod 25ns)", 100, 0., 25.);
  h1->GetXaxis()->SetTitle("time (ns)");
  h1->GetYaxis()->SetTitle("events / ns");

  for(int ievt=0; ievt<nEvents; ievt++){
    for(int imb=0; imb<nPU; imb++){
      for(int ich=0; ich<nCrystalsEB; ich++){
	double logE, time25ns; 
	hpdf->GetRandom2(logE, time25ns);
	double energy = pow(10., logE) * energyScale; 
	if(energy > energyMin ){
	  counter += 1.;
	  h1->Fill( time25ns );
	}
      }
    }
  }
  
  cout << " Minimum energy of APD spikes = " << energyMin << " GeV " << endl;
  cout << " Rate of APD spikes in entire EB per BX = " << counter * fracTotal / double(nEvents) << endl;
  h1->Draw();
}
