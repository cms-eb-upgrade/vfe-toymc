/*
  Given the output file from Sasha's Example02.C or Andrea's CreateData.C, 
  plots the waveforms for all events

  Source:
  https://github.com/cms-eb-upgrade/vfe-toymc

  To run:
  root -l -q TreeReader.C
*/

#include "Pulse.h"

void TreeReader(const char* file = "output.root") {

  TFile* fin  = new TFile(file, "read");
  TFile* fout = new TFile("waveforms.root", "recreate");

  // Opens the tree from the infile
  TTree* tree = (TTree*) fin->Get(fin->GetListOfKeys()->At(0)->GetName());

  // For actually filling the waveform
  std::vector<float>* waveformF;   // Used if the source data type is float vector
  double waveformD[WFLENGTH];      // Used if the source data type is double array
  double entry;                    // Helper for the float vector case
  int events = tree->GetEntries(); // The total events stored in the tree to be iterated over
  bool isFloatVector = (strcmp(tree->GetBranch("waveform")->GetClassName(), "vector<float>") == 0);

  // Determine the correct data type from the input file
  if (isFloatVector) {
    tree->SetBranchAddress("waveform", &waveformF);
  } 
  else {
    tree->SetBranchAddress("waveform", &waveformD);
  }

  for (int e = 0; e < events; e++) {
    if ((e + 1) % 10 == 0) cout << "Event: " << e + 1 << " / " << events << endl;
    // Create a new hist for the current event
    TH1D* wavehist = new TH1D("Event_" + TString::Itoa(e, 10), 
                              "Waveform with pileup; Time (ns); Amplitude (GeV)",
                              WFLENGTH, 0, WFLENGTH);
    tree->GetEntry(e);
    // Fill the histogram across the whole time range
    for (int t = 0; t < WFLENGTH; t++) {
      if (isFloatVector) {
        entry = static_cast<double>(waveformF->at(t));
        wavehist->Fill(t, entry);
        cout << "Un-cast: " << waveformF->at(t) << endl;
        cout << "Entry: " << entry << endl;
      }
      else {
        wavehist->Fill(t, waveformD[t]);
      }
      wavehist->SetBinError(t, 0.0);
    }
    fout->cd();
    wavehist->Write();
  }

  fout->Close();
  fin->Close();
}
