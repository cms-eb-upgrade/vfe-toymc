//
// MultiFit amplitude reconstruction
// To run:
// > g++ -o Example07.multifit.exe Example07.multifit.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
// > ./Example07.multifit.exe
//

#include <iostream>
#include "PulseChiSqSNNLS.h"
#include "Pulse.h"

#include "TTree.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2.h"
#include "TFile.h"


//---- transform the pulse into an histogram              - type is "reco = 1" or "sim = 0"
TH1F* CreateHistoShape( SampleVector& sam, int itime, int type) {
 TString name = Form ("h_%d_%d",type, itime);
 TH1F* temphist = new TH1F(name.Data(),"",sam.rows(),0,sam.rows());
 
 for (int i=0; i<sam.rows(); i++) {
  temphist->SetBinContent(i+1, sam[i]);
 }
 
 return temphist;
}


//---- transform the pulse into an histogram              - type is "reco = 1" or "sim = 0"
TH1F* CreateHistoAmplitudes( const PulseVector& sam, int itime, int type) {
 TString name = Form ("hAmpl_%d_%d",type, itime);
 TH1F* temphist = new TH1F(name.Data(),"",sam.rows(),0,sam.rows());
 
 for (int i=0; i<sam.rows(); i++) {
  temphist->SetBinContent(i+1, sam[i]);
 }
 
 return temphist;
}




void run(std::string inputFile, std::string outFile, int NSAMPLES, float NFREQ) {
 
 std::cout << " run ..." << std::endl;
 
 //----
//  int NSAMPLES = 10;
//  int NFREQ = 25;
//  int WFLENGTH = 500;
//  int IDSTART = 180;
 int IDSTART = 7*25;
 
 //----
//  IDSTART = 7.2 * NFREQ;
 
 int WFLENGTH = 500*4; //---- step 1/4 ns in waveform
 
 if (( IDSTART + NSAMPLES * NFREQ ) > 500 ) {
  WFLENGTH = (IDSTART + NSAMPLES * NFREQ)*4 + 100;
 }
 
 std::cout << " WFLENGTH = " << WFLENGTH << std::endl;
 std::cout << " NFREQ    = " << NFREQ << std::endl;
 
 
 Pulse pSh;
 pSh.SetNSAMPLES (NSAMPLES);
 pSh.SetNFREQ (NFREQ);
 pSh.SetWFLENGTH(WFLENGTH);
 pSh.SetIDSTART(IDSTART);
 
 pSh.Init();
 
 std::cout << " pSh ready " << std::endl;
 
//  FullSampleVector fullpulse(FullSampleVector::Zero());
//  FullSampleMatrix fullpulsecov(FullSampleMatrix::Zero());
//  SampleMatrix noisecor(SampleMatrix::Zero());
//  BXVector activeBX;
//  SampleVector amplitudes(SampleVector::Zero());
 
 
 FullSampleVector fullpulse;
 FullSampleMatrix fullpulsecov;
 SampleMatrix noisecor;
 BXVector activeBX;
 SampleVector amplitudes;
 
 fullpulse.resize(2*NSAMPLES,1);   fullpulse.setZero(); // (Eigen::Matrix<double,2*NSAMPLES,1>::Zero());
 fullpulsecov.resize(2*NSAMPLES,2*NSAMPLES); fullpulsecov.setZero(); // (Eigen::Matrix<double,2*NSAMPLES,2*NSAMPLES>::Zero());
 noisecor.resize(NSAMPLES,NSAMPLES); noisecor.setZero(); // (Eigen::Matrix<double,NSAMPLES,NSAMPLES>::Zero());
 activeBX.resize(Eigen::NoChange,1);
 amplitudes.resize(NSAMPLES,1); amplitudes.setZero(); // (SampleVector::Zero());
 
 std::cout << " default ready " << std::endl;
 
 
 // intime sample is [2]
//  double pulseShapeTemplate[NSAMPLES+2];
 std::vector<double> pulseShapeTemplate;
 
//  for(int i=0; i<(NSAMPLES+2*int(25 /NFREQ)); i++) {
//  for(int i=0; i<(NSAMPLES+5*int(25 /NFREQ)); i++) {
  for(int i=0; i<(NSAMPLES+7*int(25 /NFREQ)); i++) {
   
  //   for(int i=0; i<(2*NSAMPLES+2); i++){
//  for(int i=0; i<(NSAMPLES+2); i++){
   //   double x = double( IDSTART + NFREQ * (i + 3) - WFLENGTH / 2 + 25); //----> + 25 or not?
//   double x = double( IDSTART + NFREQ * (i + 3) - WFLENGTH / 2 ); //----> + 25 or not?
//   double x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2 + 25.);
//   double x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2 );  // ----> best for 25 ns spacing??
//   double x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2 + 5  );  //--------------> this is the residual of the 180 ns shift ... arbitrary 180 ns!!! 
//   double x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2 + 5 + 25 * ( int( IDSTART / NFREQ ) - ( IDSTART / NFREQ ) ) );   

  double x;
//   if ( NFREQ == 25)    x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2 );   
//   if ( NFREQ == 6.25)  x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2  + 5);   
//   if ( NFREQ == 12.5)  x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2  + 10);   
  
//   x = double( IDSTART + NFREQ * i + 4*25. - 500 / 2. );  //----> 500 ns is fixed!  
  x = double( IDSTART + NFREQ * i + 3*25. - 500 / 2. );  //----> 500 ns is fixed!  
  
//   std::cout << " IDSTART + NFREQ * i + 3*25. - 500 / 2. = " << IDSTART + NFREQ * i + 3*25. - 500 / 2. << std::endl;
  
  //   double x = double( IDSTART + NFREQ * (i) + 3*25. - WFLENGTH / 2 - 25.); 
//   double x = double( NFREQ * (i) + 3*25. - WFLENGTH / 2 - 25.); 
  //   double x = double( IDSTART + NFREQ * (i) - WFLENGTH / 2 ); 
  //   pulseShapeTemplate[i] = pSh.fShape(x);
  
//   if (NFREQ == 12.5)      pulseShapeTemplate.push_back( pSh.fShape(x) / (2.5));
//   else if (NFREQ == 6.25) pulseShapeTemplate.push_back( pSh.fShape(x) / (10/0.3));
//   else                    pulseShapeTemplate.push_back( pSh.fShape(x));
  pulseShapeTemplate.push_back( pSh.fShape(x));
  
//   pulseShapeTemplate.push_back( pSh.fShape(x) * NFREQ/25.);
//   pulseShapeTemplate.push_back( pSh.fShape(x));
  
  std::cout << " [" << i << "::" << (NSAMPLES+2*25 /NFREQ) << "] --> pSh.fShape(" << x << ") = " << pSh.fShape(x) << " ---> " << pSh.fShape(x) * NFREQ/25. << std::endl;
 
 }
 
 
 //  for(int i=0; i<(NSAMPLES+2); i++) pulseShapeTemplate[i] /= pulseShapeTemplate[2];
//  for (int i=0; i<(NSAMPLES+2); i++) fullpulse(i+(NSAMPLES-3)) = pulseShapeTemplate[i];
//  for (int i=0; i<(NSAMPLES+2); i++) fullpulse(i+7) = pulseShapeTemplate[i];
 //  7 before was due to 
 //  IDSTART = 180 ns = 7.2 * 25 ns ;
 //  now it should be, in a more general way:
 //  180 ns = X * NFREQ --> X = 180 / NFREQ
//  for (int i=0; i<(NSAMPLES + 3*int(25 /NFREQ)); i++) {
  for (int i=1; i<(NSAMPLES + 2*int(25 /NFREQ)); i++) {
//    for (int i=0; i<(NSAMPLES + 2*int(25 /NFREQ)); i++) {
    //   fullpulse(i + NSAMPLES - 3*25 /NFREQ + int( IDSTART / NFREQ )) = pulseShapeTemplate[i]; 
//   std::cout << " i + NSAMPLES - 3*int(25 /NFREQ) = " << i << " + " << NSAMPLES << " - 3*int(25 / " << NFREQ << " ) = " << i + NSAMPLES - 3*int(25 /NFREQ) << std::endl;
//   fullpulse(i + NSAMPLES - 3*int(25 /NFREQ)) = pulseShapeTemplate[i];
  
   fullpulse(i + 7 * int(25 /NFREQ)) = pulseShapeTemplate[i];
//    fullpulse(i + 7 * int(25 /NFREQ)) = pulseShapeTemplate[i-1];
   
//   _NSAMPLES - 5 * int(25. /_NFREQ)
  
  
  //   fullpulse(i) = pulseShapeTemplate[i];
//   fullpulse(i + int( IDSTART / NFREQ )) = pulseShapeTemplate[i];
//   fullpulse(i + int(180/NFREQ)) = pulseShapeTemplate[i];
  //   fullpulse(i + NSAMPLES/2 + int(2 * 25 /NFREQ) ) = pulseShapeTemplate[i];
//   std::cout << " i + int(" << IDSTART << "/NFREQ) = " << i << " + " << int(IDSTART/NFREQ) << " = " << i + int(IDSTART/NFREQ) << " --> " << pulseShapeTemplate[i] << std::endl;
//   std::cout << " i = " << i << " --> pulseShapeTemplate = " << pulseShapeTemplate[i] << std::endl;
 }
 
 
//  for (int i=0; i<(NSAMPLES+5); i++) fullpulse(i+(NSAMPLES-3)) = pulseShapeTemplate[i];
//  for (int i=0; i<(NSAMPLES+2); i++) fullpulse(i+(NSAMPLES-3)) = pulseShapeTemplate[i];
//  for (int i=0; i<(NSAMPLES+2); i++) fullpulse(i+(NSAMPLES-3)) = pulseShapeTemplate[i];
 
 
 //---- correlation
 for (int i=0; i<NSAMPLES; ++i) {
  for (int j=0; j<NSAMPLES; ++j) {
   int vidx = std::abs(j-i);
   noisecor(i,j) = pSh.corr(vidx);
  }
 }
 
 std::cout << " noise ready " << std::endl;
 
 //----  collision every 25 ns -> this is fixed number
 //----                       number of sampls * frequence in ns / 25 ns
 if ( round((NSAMPLES * NFREQ) / 25.) != (NSAMPLES * NFREQ) / 25 ) {
  std::cout << " Attention please! How do you think multifit can fit a pulse in the middle between collisions!?!?!?!?" << std::endl;
 }
 
 int totalNumberOfBxActive = int(NSAMPLES * NFREQ) / 25; 
//  int totalNumberOfBxActive = int(NSAMPLES * NFREQ) / 25 - 1;
 std::cout << " totalNumberOfBxActive = " << totalNumberOfBxActive << std::endl;
 
//  totalNumberOfBxActive = 4;
//  int activeBXs[] = { -5, -1,  0,  1 };
 std::vector<int> activeBXs;
 for (unsigned int ibx=0; ibx<totalNumberOfBxActive; ++ibx) {

//   activeBXs.push_back( ibx * 25./NFREQ - 3 * 25 / NFREQ ); //----> -3 BX are active w.r.t. 0 BX
//   activeBXs.push_back( ibx * int(25 /NFREQ) - 5 * int(25 /NFREQ) ); //----> -5 BX are active w.r.t. 0 BX
  activeBXs.push_back( ibx * int(25 /NFREQ) - 4 * int(25 /NFREQ) ); //----> -5 BX are active w.r.t. 0 BX
  
//   activeBXs.push_back( ibx * 25./NFREQ - 3 * 25 / NFREQ );
  
//   if (NSAMPLES%2) {
//    //---- odd
//    activeBXs[ibx] = ibx * 25./NFREQ - (NSAMPLES+1)/2;
//   }
//   else {
//    //---- even
//   activeBXs[ibx] = ibx * 25./NFREQ - NSAMPLES/2;
//   }
//   activeBXs[ibx] = ibx * 25./NFREQ - NSAMPLES/2;
  std::cout << " activeBXs[" << ibx << "] = " << activeBXs[ibx] << std::endl;
 }
 
 
 
 
//  //   int activeBXs[] = { -5, -4, -3, -2, -1,  0,  1,  2,  3,  4 };
//  int activeBXs[500];
//  if (NSAMPLES == 10) {
//   for (unsigned int ibx=0; ibx<NSAMPLES; ++ibx) {
//    activeBXs[ibx] = ibx - NSAMPLES/2;
//    std::cout << " activeBXs[" << ibx << "] = " << activeBXs[ibx] << std::endl;
//   }
//  }
//  else {
//   for (unsigned int ibx=0; ibx<10; ++ibx) {
//    activeBXs[ibx] = 2*ibx - NSAMPLES/2;
//    std::cout << " activeBXs[" << ibx << "] = " << activeBXs[ibx] << std::endl;
//   }
//  }
 
 
 activeBX.resize(totalNumberOfBxActive);
 for (unsigned int ibx=0; ibx<totalNumberOfBxActive; ++ibx) {
  activeBX.coeffRef(ibx) = activeBXs[ibx];
 } 
 
 std::cout << " end init " << std::endl;
 //  activeBX.resize(1);
 //  activeBX.coeffRef(0) = 0;
 
 
 
 std::cout << " inputFile = " << inputFile << std::endl;
 TFile *file2 = new TFile(inputFile.c_str());
 //  TFile *file2 = new TFile("data/samples_signal_10GeV_eta_0.0_pu_140.root");
 
//  double samples[NSAMPLES];
 std::vector<double>* samples = new std::vector<double>;
 double amplitudeTruth;
 TTree *tree = (TTree*) file2->Get("Samples");
 tree->SetBranchAddress("amplitudeTruth",      &amplitudeTruth);
 tree->SetBranchAddress("samples",             &samples);
 
 int nentries = tree->GetEntries();
 
 std::cout << " nentries = " << nentries << std::endl;
 std::cout << " NSAMPLES = " << NSAMPLES << std::endl;
 
 TFile *fout;
 TH1D *h01;
 
 std::vector<TH1F*> v_pulses;
 std::vector<TH1F*> v_amplitudes_reco;
 
 std::cout << " outFile = " << outFile << std::endl;
 fout = new TFile(outFile.c_str(),"recreate");
 h01 = new TH1D("h01", "dA", 5000, -5.0, 5.0);
 
 fout->cd();
 TTree* newtree = (TTree*) tree->CloneTree(0); //("RecoAndSim");
 newtree->SetName("RecoAndSim");
 
 std::vector <double> samplesReco;
 int ipulseintime = 0;
 newtree->Branch("samplesReco",   &samplesReco);
 newtree->Branch("ipulseintime",  ipulseintime,  "ipulseintime/I");
 newtree->Branch("activeBXs",     &activeBXs);
 newtree->Branch("pulseShapeTemplate",   &pulseShapeTemplate);
 std::cout << " pulseShapeTemplate.size () = " << pulseShapeTemplate.size() << std::endl;
 
 
 for (unsigned int ibx=0; ibx<totalNumberOfBxActive; ++ibx) {
  samplesReco.push_back(0.);
 }
 
 
 v_amplitudes_reco.clear();
 
 //---- create the multifit
 PulseChiSqSNNLS pulsefunc;
 pulsefunc.setNSAMPLES(NSAMPLES);
 pulsefunc.setNFREQ(NFREQ);
 pulsefunc.Init(); //---- initialization, needed
 
 fout->cd();
 
 for(int ievt=0; ievt<nentries; ++ievt){
  
  if (!(ievt%10)) {
   std::cout << " ievt = " << ievt << " :: " << nentries << std::endl;
  }
  
//   std::cout << " here " << std::endl;
  
  tree->GetEntry(ievt);
  
//   std::cout << " here " << std::endl;
  
//   std::cout << " samples->size() = " << samples->size() << std::endl;
  
  
  for(int i=0; i<NSAMPLES; i++){
   amplitudes[i] = samples->at(i);
//    std::cout << " samples->at(" << i << ") = " << samples->at(i) << std::endl;
  }
  
//   v_pulses.push_back(CreateHistoShape(amplitudes, ievt, 0));
  
  double pedval = 0.;
  double pedrms = 1.0;
  
   
  pulsefunc.disableErrorCalculation();
  
//   std::cout << " amplitudes = " << amplitudes << std::endl;
//   std::cout << " noisecor = " << noisecor << std::endl;
//   std::cout << " pedrms = " << pedrms << std::endl;
// //   std::cout << " activeBX = " << activeBX << std::endl;
//   std::cout << " fullpulse = " << fullpulse << std::endl;
//   std::cout << " fullpulsecov = " << fullpulsecov << std::endl;
  
  bool status = pulsefunc.DoFit( amplitudes, noisecor, pedrms, activeBX, fullpulse, fullpulsecov );
  double chisq = pulsefunc.ChiSq();
  
  ipulseintime = 0;
  for (unsigned int ipulse=0; ipulse<pulsefunc.BXs()->rows(); ++ipulse) {
//    std::cout << " pulsefunc.BXs()->coeff(" << ipulse << ") = " << int(pulsefunc.BXs()->coeff(ipulse)) << std::endl;
//    if (pulsefunc.BXs()->coeff(ipulse)==0) {
   if ( ((int(pulsefunc.BXs()->coeff(ipulse))) * NFREQ/25 + 5) == 0) {
//     std::cout << " found intime!!! --> " << ipulse << std::endl;
    ipulseintime = ipulse;
    break;
   }
  }

//   std::cout << "  >> ipulseintime = " << ipulseintime << "    >> status =       " << status << std::endl;
//   std::cout << "  >> ipulseintime = " << ipulseintime << std::endl;
//   std::cout << "  >> status =       " << status << std::endl;
  
  double aMax = status ? (*(pulsefunc.X()))[ipulseintime] : 0.;
  //  double aErr = status ? pulsefunc.Errors()[ipulseintime] : 0.;
  
//   std::cout << " pulsefunc.BXs().rows() = " << pulsefunc.BXs()->rows() << std::endl;
//   std::cout << " pulsefunc.X() = " <<  pulsefunc.X() << std::endl;
  
  for (unsigned int ipulse=0; ipulse<pulsefunc.BXs()->rows(); ++ipulse) {
   if (status) {
//     std::cout << " ip = " << ipulse << " --> " << int(pulsefunc.BXs()->coeff(ipulse)) << " ----> " << (*(pulsefunc.X()))[ ipulse ] << std::endl;
//     std::cout << " ip = " << ipulse << " --> " <<  (int(pulsefunc.BXs()->coeff(ipulse)) + NSAMPLES / 2) * NFREQ/25  << " ----> " << (*(pulsefunc.X()))[ ipulse ] << std::endl;

//     std::cout << " ip = " << ipulse << " --> " <<  (int(pulsefunc.BXs()->coeff(ipulse)) + 3) * NFREQ/25  << " ----> " << (*(pulsefunc.X()))[ ipulse ] << std::endl;
    if (ievt == 0) std::cout << " ip = " << ipulse << " --> [" <<  (int(pulsefunc.BXs()->coeff(ipulse))) << "] --> " << (int(pulsefunc.BXs()->coeff(ipulse))) * NFREQ/25 + 5  << " == " << (*(pulsefunc.X()))[ ipulse ] << std::endl;

    //     samplesReco[ int(pulsefunc.BXs()->coeff(ipulse)) + NSAMPLES / 2 ] = (*(pulsefunc.X()))[ ipulse ];
//     samplesReco[ (int(pulsefunc.BXs()->coeff(ipulse)) + NSAMPLES / 2) * NFREQ/25 ] = (*(pulsefunc.X()))[ ipulse ];

//     samplesReco[ (int(pulsefunc.BXs()->coeff(ipulse)) + 3 ) * NFREQ/25 ] = (*(pulsefunc.X()))[ ipulse ];
//     samplesReco[ (int(pulsefunc.BXs()->coeff(ipulse)) + 5 ) * NFREQ/25 ] = (*(pulsefunc.X()))[ ipulse ];

    //---- YES
    samplesReco[ (int(pulsefunc.BXs()->coeff(ipulse))) * NFREQ/25 + 5] = (*(pulsefunc.X()))[ ipulse ];
    
    // 
//     ibx * 25./NFREQ - NSAMPLES/2
//     
    //     samplesReco[ int(pulsefunc.BXs()->coeff(ipulse)) + 5] = (*(pulsefunc.X()))[ ipulse ];
    //     samplesReco[ int(pulsefunc.BXs().coeff(ipulse)) + 5] = pulsefunc.X()[ int(pulsefunc.BXs().coeff(ipulse)) + 5 ];
    //     samplesReco[ipulse] = pulsefunc.X()[ ipulse ];
    //     samplesReco[ipulse] = pulsefunc.X()[ pulsefunc.BXs().coeff(ipulse) ];
   }
   else {
    samplesReco[ipulse] = -1;
   }
  }
  
  
  
  //   for (unsigned int ipulse=0; ipulse<pulsefunc.BXs().rows(); ++ipulse) {
  //    amplitudes[ipulse] = pulsefunc.X()[ipulse];
  //   }
  
  //   v_pulses_reco.push_back(CreateHistoShape(amplitudes, ievt, 1));
  
//   v_amplitudes_reco.push_back(CreateHistoAmplitudes(*(pulsefunc.X()), ievt, 1));  
  
  h01->Fill(aMax - amplitudeTruth);
//   std::cout << " aMax - amplitudeTruth = " << aMax << " - " << amplitudeTruth << std::endl;
  
  newtree->Fill();
  
 }
 
 
 //---- save pulses
//  fout->cd();
//  fout->mkdir("samples");
//  fout->cd("samples"); 
//  for (int ievt = 0; ievt < v_pulses.size(); ievt++) {
//   v_pulses.at(ievt)->Write();
//  }
//  
//  for (int ievt = 0; ievt < v_amplitudes_reco.size(); ievt++) {
//   v_amplitudes_reco.at(ievt)->Write();
//  }
 
 std::cout << "  Mean of REC-MC = " << h01->GetMean() << " GeV" << std::endl;
 std::cout << "   RMS of REC-MC = " << h01->GetRMS()  << " GeV" << std::endl;
 
 
 fout->cd();
 std::cout << " done (1) " << std::endl;
 h01->Write();
 std::cout << " done (2) " << std::endl;
//  newtree->AutoSave();
 newtree->Write();
 std::cout << " done (3) " << std::endl;
 fout->Close();
 
 std::cout << " done ... " << std::endl;
 
}






# ifndef __CINT__

int main(int argc, char** argv) {
 
 std::string inputFile = "data/samples_signal_10GeV_pu_0.root";
 if (argc>=2) {
  inputFile = argv[1];
 }
 
 std::string outFile = "output.root";
 if (argc>=3) {
  outFile = argv[2];
 }
 std::cout << " outFile = " << outFile << std::endl;
 
 //---- number of samples per impulse
 int NSAMPLES = 10;
 if (argc>=4) {
  NSAMPLES = atoi(argv[3]);
 }
 std::cout << " NSAMPLES = " << NSAMPLES << std::endl;
 
 //---- number of samples per impulse
 float NFREQ = 25;
 if (argc>=5) {
  NFREQ = atof(argv[4]);
 }
 std::cout << " NFREQ = " << NFREQ << std::endl;
 
 
 
 run(inputFile, outFile, NSAMPLES, NFREQ);
 
 std::cout << " outFile = " << outFile << std::endl;
 
 return 0;
}

# endif


