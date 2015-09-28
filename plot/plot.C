//---- plot output of multifit

void plot (std::string nameInputFile = "output.root"){
 
 Color_t* color = new Color_t [20];
 color[0] = kRed ;
 color[1] = kAzure + 10 ;
 color[2] = kYellow + 2 ;
 color[3] = kGreen ;
 color[4] = kGreen + 4 ;
 color[5] = kBlue ;
 color[6] = kCyan ;
 color[7] = kPink + 1 ;
 color[8] = kBlack ;
 color[9] = kYellow + 4 ;
 
 
 
 TFile *file = new TFile(nameInputFile.c_str());
 
 TTree* tree = (TTree*) file->Get("RecoAndSim");
 
 TCanvas* cc = new TCanvas ("cc","",800,600);
 
 TH1F* histo[10];
 
 TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
 
 for (int i=0; i<10; i++) {
  TString nameHisto = Form ("samplesReco_%d", i);
  TString nameHistoTitle = Form ("BX %d", i-5);
  histo[i] = new TH1F (nameHisto.Data(), nameHistoTitle.Data(), 2000, 0, 20);
  
  TString toDraw = Form ("samplesReco[%d] >> %s", i, nameHisto.Data());
  tree->Draw(toDraw.Data());
  histo[i] -> SetLineColor(color[i]);
  histo[i] -> SetLineWidth(2);
  leg->AddEntry(histo[i],nameHistoTitle.Data(),"l");
 }

 for (int i=0; i<10; i++) {
  if (i==0) {
   histo[i]->Draw();
   histo[i]->GetXaxis()->SetTitle("Energy [GeV]");
  }
  else {
   histo[i]->Draw("same");   
  }
 }
 
 leg->Draw();
 
 TCanvas* ccGood = new TCanvas ("ccGood","",800,600);
 for (int i=0; i<10; i++) {
  if (histo[i]->GetMean() > 5) {
   histo[i]->Draw();
   histo[i]->GetXaxis()->SetTitle("Energy [GeV]");
  }
 }
 
 
}