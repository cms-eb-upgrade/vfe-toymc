//
// Initialize Event file
// Store Pulse shape as a TGraph and three parameters to describe the tail of the pulse shape
// Original graph is stored in data/crrc43ns.root
//
// Suggestion to run it:
// >root -l -q Example01.C
// >cp output.root filename.root
//

#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <TH1.h>
#include <iostream>


void Example01() {

  // Load the event graph
  TFile *fin = new TFile("data/crrc43ns.root");
  TGraph *grPulseShape = (TGraph*)fin->Get("grCRRC43");
  grPulseShape->SetName("grPulseShape");
  grPulseShape->SetTitle("Pulse shape");

  // Set up the output file and write the event graph
  TFile *fout = new TFile("output.root","recreate");
  TDirectory *cdPulseShape = fout->mkdir("PulseShape");
  TDirectory *cdPU = fout->mkdir("PileupPDFs");
  cdPulseShape->cd();
  grPulseShape->Write();

  // Write the tree containing information about the pulse shape
  TTree *tr = new TTree("Tail","Parameters for the tail of the pulse shape");
  float timeMin = 800;
  float expAmplitude = 0.0401833;
  float expTime = .01 ;
  tr->Branch("timeMin",     &timeMin,     "timeMin/F");
  tr->Branch("expAmplitude",&expAmplitude,"expAmplitude/F");
  tr->Branch("expTime",     &expTime,     "expTime/F");
  tr->Fill();
  tr->Write();
  
  // Load the pileup graphs and format them into new TH1D graphs
  TFile *fin2 = new TFile("data/minbias_01.root");
  TH1D *hpdf[14];
  fout->cd("PileupPDFs");
  for (int ih=0; ih<14; ih++) {
	  // Names for getting and writing the graphs
    char hnameInput[120];
    sprintf(hnameInput,"h%i",101+ih);
	  cout<<hnameInput<<endl;
    char hnameOutput[120];
    sprintf(hnameOutput,"pupdf_%i",ih);
    char htitleOutput[120];
    sprintf(htitleOutput,
  	"pdf of energies from minbias interaction at %3.2f < eta < %3.2f", 0.05+0.1*ih, 0.15+0.1*ih);
	  // Actually getting, formatting, and writing
    hpdf[ih] = (TH1D*)fin2->Get(hnameInput);
    hpdf[ih]->SetName(hnameOutput);
    hpdf[ih]->SetTitle(htitleOutput);
   	hpdf[ih]->SetAxisRange(-10.0, 2.0);
	  hpdf[ih]->Write();
  }

  fin->Close();
  fin2->Close();
  fout->Close();
}
