//
// Given the output file from Example02.C, plots the waveforms for all events
//

#include "Pulse.h"

void TreeReader(const char* file = "output.root") {

  double waveform[WFLENGTH];

  TFile* fin  = new TFile(file, "read");
  TFile* fout = new TFile("waveforms.root", "recreate");
  TTree* tree = (TTree*) fin->Get("Waveforms");
  tree->SetBranchAddress("waveform", &waveform);
  int events = tree->GetEntries();

  for (int e = 0; e < events; e++) {
    // Create a new hist for the current event
    TH1D* wavehist = new TH1D("Event_" + TString::Itoa(e, 10), 
                              "Waveform with pileup; Time; Amplitude",
                              WFLENGTH, 0, WFLENGTH);
    tree->GetEntry(e);
    // Fill the histogram across the whole time range
    for (int t = 0; t < WFLENGTH; t++) {
      wavehist->Fill(t, waveform[t]);
      wavehist->SetBinError(t, 0.0);
    }
    fout->cd();
    wavehist->Write();
  }

  fout->Close();
  fin->Close();
}
