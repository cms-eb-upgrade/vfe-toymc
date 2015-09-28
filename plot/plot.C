//---- plot output of multifit

void plot (std::string nameInputFile = "output.root"){
 
 TFile *file = new TFile(nameInputFile.c_str());
 
 TTree* tree = (TTree*) file->Get("RecoAndSim");
 
 TCanvas* cc = new TCanvas ("cc","",800,600);
 
 TH1F* histo[10];
 
 TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
 
 for (int i=0; i<10; i++) {
  TString toDraw = Form ("samplesReco[%d] >> hnew", i);
  tree->Draw(toDraw.Data());
  TString nameHisto = Form ("samplesReco_%d", i);
  TH1F *hnewmy = (TH1F*) gDirectory->Get("hnew");
  histo[i] = (TH1F*) hnewmy->Clone(nameHisto.Data());
  histo[i] -> SetLineColor(i+1);
  histo[i] -> SetLineWidth(i+1);
  TString nameHistoTitle = Form ("BX %d", i-5);
  histo[i]->SetTitle(nameHistoTitle.Data());
  leg->AddEntry(histo[i],nameHistoTitle.Data(),"l");
 }

 for (int i=0; i<10; i++) {
  if (i==0) {
   histo[i]->Draw();
  }
  else {
   if (histo[i]->GetMean() < 5) {
    histo[i]->Draw("same");   
   }
  }
 }
 
 leg->Draw();
 
 TCanvas* ccGood = new TCanvas ("ccGood","",800,600);
 for (int i=0; i<10; i++) {
  if (histo[i]->GetMean() > 5) {
   histo[i]->Draw();
  }
 }
 
 
}