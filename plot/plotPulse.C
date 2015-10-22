//---- plot output of multifit

void plotPulse (std::string nameInputFile = "output.root", int nEvent = 10){
 
 Color_t* color = new Color_t [200];
 color[0] = kAzure; //kRed ;
 color[1] = kAzure + 10 ;
 color[2] = kYellow + 2 ;
 color[3] = kGreen ;
 color[4] = kGreen + 4 ;
 color[5] = kBlue ;
 color[6] = kCyan ;
 color[7] = kPink + 1 ;
 color[8] = kBlack ;
 color[9] = kYellow + 4 ;
 for (int i=0; i<30; i++) {
  color[i+10] = kBlue + i;
 }
 
 
 TFile *file = new TFile(nameInputFile.c_str());
 
 TTree* tree = (TTree*) file->Get("RecoAndSim");
  
 int    nWF;
 std::vector<double>* waveform    = new std::vector<double>;
 std::vector<double>* samplesReco = new std::vector<double>;
 std::vector<double>* samples     = new std::vector<double>;
 std::vector<int>*    activeBXs   = new std::vector<int>;
 std::vector<double>* pulseShapeTemplate     = new std::vector<double>;
 
 float NFREQ;
 
 tree->SetBranchAddress("nWF",      &nWF);
 tree->SetBranchAddress("waveform", &waveform);
 tree->SetBranchAddress("samplesReco", &samplesReco);
 tree->SetBranchAddress("samples",   &samples);
 tree->SetBranchAddress("activeBXs", &activeBXs);
 tree->SetBranchAddress("nFreq",   &NFREQ);
 tree->SetBranchAddress("pulseShapeTemplate",   &pulseShapeTemplate);
 
 
 tree->GetEntry(nEvent);
 std::cout << " NFREQ = " << NFREQ << std::endl;
 
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
  std::cout << " i, activeBXs->at(i), samplesReco->at(i) = " << i << "::" << samplesReco->size() << " -> " << activeBXs->at(i) << " , " << samplesReco->at(i) << std::endl;
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
  grPulse->SetPoint(i, i * NFREQ , samples->at(i));
 }
 grPulse->SetMarkerSize(2);
 grPulse->SetMarkerStyle(21);
 grPulse->SetMarkerColor(kRed);
 grPulse->Draw("ALP");
 grPulse->GetXaxis()->SetTitle("time [ns]");
 
 std::cout << " end " << std::endl;
 
 
 TCanvas* ccPulseAndReco = new TCanvas ("ccPulseAndReco","",800,600);
 TGraph *grPulseRecoAll = new TGraph();
 TGraph *grPulseReco[samplesReco->size()];
 std::cout << " samplesReco->size() = " << samplesReco->size() << std::endl;
 std::cout << " activeBXs->size() = " << activeBXs->size() << std::endl;
 std::cout << " pulseShapeTemplate->size() = " << pulseShapeTemplate->size() << std::endl;
 std::cout << " samples->size() = " << samples->size() << std::endl;
 
 TLegend* leg = new TLegend(0.7,0.2,0.9,0.9);
 
 std::vector<float> totalRecoSpectrum;
 for(int i=0; i<samples->size(); i++){
  totalRecoSpectrum.push_back(0);
 }
 
 
//  for(int iBx=0; iBx<3; iBx++){
 for(int iBx=0; iBx<samplesReco->size(); iBx++){
  grPulseReco[iBx] = new TGraph();
  for(int i=0; i<samples->size(); i++){
   grPulseReco[iBx]->SetPoint(i, i * NFREQ + activeBXs->at(iBx)*NFREQ + 2 * 25, pulseShapeTemplate->at(i) * samplesReco->at(iBx));
  
   int iReco = (i * NFREQ + activeBXs->at(iBx)*NFREQ + 2 * 25) / NFREQ;
   if ( iReco >= 0 && iReco <samples->size() ) {
    totalRecoSpectrum.at(iReco) += pulseShapeTemplate->at(i) * samplesReco->at(iBx);
   } 
   
  }
  grPulseReco[iBx]->SetMarkerColor(color[iBx]);
  grPulseReco[iBx]->SetLineColor(color[iBx]);
  grPulseReco[iBx]->SetMarkerSize(1);
  grPulseReco[iBx]->SetMarkerStyle(21+iBx);
  TString nameHistoTitle = Form ("BX %d", activeBXs->at(iBx));
  leg->AddEntry(grPulseReco[iBx],nameHistoTitle.Data(),"p");
 }
 
 grPulse->Draw("ALP");
//  for(int iBx=0; iBx<3; iBx++){
 for(int iBx=1; iBx<samplesReco->size(); iBx++){
  grPulseReco[iBx]->Draw("PL");
 }
 
 for(int i=0; i<samples->size(); i++){
  grPulseRecoAll->SetPoint(i, i * NFREQ, totalRecoSpectrum.at(i));
 }
 
 grPulseRecoAll->SetMarkerColor(kMagenta);
 grPulseRecoAll->SetLineColor(kMagenta);
 grPulseRecoAll->SetLineStyle(1);
 grPulseRecoAll->SetMarkerSize(2);
 grPulseRecoAll->SetMarkerStyle(24);
 grPulseRecoAll->Draw("PL");
 grPulse->GetXaxis()->SetTitle("time [ns]");
 
 
 leg->Draw();
 
 
 
 
}


