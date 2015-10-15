//---- plot output of multifit

void plotPulse (std::string nameInputFile = "output.root", int nEvent = 10){
 
 TFile *file = new TFile(nameInputFile.c_str());
 
 TTree* tree = (TTree*) file->Get("RecoAndSim");
  
 int    nWF;
 std::vector<double>* waveform    = new std::vector<double>;
 std::vector<double>* samplesReco = new std::vector<double>;
 std::vector<double>* samples     = new std::vector<double>;
 std::vector<int>*    activeBXs   = new std::vector<int>;
 
 
 tree->SetBranchAddress("nWF",      &nWF);
 tree->SetBranchAddress("waveform", &waveform);
 tree->SetBranchAddress("samplesReco", &samplesReco);
 tree->SetBranchAddress("samples",   &samples);
 tree->SetBranchAddress("activeBXs", &activeBXs);
 
 
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
 
 
 
 TCanvas* ccReco = new TCanvas ("ccReco","",800,600);
 TGraph *grReco = new TGraph();
 for(int i=0; i<samplesReco->size(); i++){
  std::cout << " i, activeBXs->at(i), samplesReco->at(i) = " << i << " , " << activeBXs->at(i) << " , " << samplesReco->at(i) << std::endl;
  grReco->SetPoint(i, activeBXs->at(i), samplesReco->at(i));
 }
 grReco->SetMarkerSize(2);
 grReco->SetMarkerStyle(22);
 grReco->SetMarkerColor(kBlue);
 grReco->Draw("ALP");
 grReco->GetXaxis()->SetTitle("BX");
 
 
 TCanvas* ccPulse = new TCanvas ("ccPulse","",800,600);
 TGraph *grPulse = new TGraph();
 for(int i=0; i<samples->size(); i++){
  grPulse->SetPoint(i, activeBXs->at(i), samples->at(i));
 }
 grPulse->SetMarkerSize(2);
 grPulse->SetMarkerStyle(21);
 grPulse->SetMarkerColor(kRed);
 grPulse->Draw("ALP");
 grPulse->GetXaxis()->SetTitle("BX");
 
 
 
 
}


