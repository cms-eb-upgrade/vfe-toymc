//---- plot output of multifit

void plotPulseInput (std::string nameInputFile = "output.root", int nEvent = 10){
 
 TFile *file = new TFile(nameInputFile.c_str());
 
 TTree* tree = (TTree*) file->Get("Samples");
  
 int    nWF;
 std::vector<double>* waveform = new std::vector<double>;
 std::vector<double>* samples = new std::vector<double>;
 
 tree->SetBranchAddress("nWF",      &nWF);
 tree->SetBranchAddress("waveform", &waveform);
 tree->SetBranchAddress("samples", &samples);
 
 tree->GetEntry(nEvent);
 
 TCanvas* ccwaveform = new TCanvas ("ccwaveform","",800,600);
 TGraph *gr = new TGraph();
 for(int i=0; i<nWF; i++){
  gr->SetPoint(i, i, waveform->at(i));
 }
 gr->Draw("AL");
 gr->SetLineColor(kMagenta);
 gr->SetLineWidth(2);
 gr->GetXaxis()->SetTitle("time [ns]");
 
 
 TCanvas* ccPulse = new TCanvas ("ccPulse","",800,600);
 TGraph *grPulse = new TGraph();
 for(int i=0; i<samples->size(); i++){
  grPulse->SetPoint(i, i, samples->at(i));
 }
 grPulse->SetMarkerSize(2);
 grPulse->SetMarkerStyle(21);
 grPulse->SetMarkerColor(kRed);
 grPulse->Draw("ALP");
 grPulse->GetXaxis()->SetTitle("BX");
 
 
 
 
}


