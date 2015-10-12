/*
  Given the output file from Sasha's Example02.C or Andrea's CreateData.C, 
  plots the waveforms for all events

  Source:
  https://github.com/cms-eb-upgrade/vfe-toymc
mysample_100_0_10_25.00.root
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
  std::vector<double>* waveformV;  // Used if the source data type is double vector
  double waveformA[WFLENGTH];      // Used if the source data type is double array
  int events = tree->GetEntries(); // The total events stored in the tree to be iterated over
  bool isVector = (strcmp(tree->GetBranch("waveform")->GetClassName(), "vector<double>") == 0);

  // Determine the correct data type from the input file
  if (isVector) {
    tree->SetBranchAddress("waveform", &waveformV);
  } 
  else {
    tree->SetBranchAddress("waveform", &waveformA);
  }

  for (int e = 0; e < events; e++) {
    if ((e + 1) % 10 == 0) cout << "Event: " << e + 1 << " / " << events << endl;
    // Create a new hist for the current event
    TH1D wavehist("Event_" + TString::Itoa(e, 10), 
                  "Waveform with pileup; Time (ns); Amplitude (GeV)",
                  WFLENGTH, 0, WFLENGTH);

    tree->GetEntry(e);
    // Fill the histogram across the whole time range
    for (int t = 0; t < WFLENGTH; t++) {
      if (isVector) {
        wavehist.Fill(t, waveformV->at(t));
      }
      else {
        wavehist.Fill(t, waveformA[t]);
      }
      wavehist.SetBinError(t, 0.0);
    }
    fout->cd();
    wavehist.Write();
  }

  fout->Close();
  fin->Close();
}
