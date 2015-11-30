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
 

  int IDSTART = 7*25;
 
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
 
 
  FullSampleVector fullpulse;
  FullSampleMatrix fullpulsecov;
  SampleMatrix noisecor;
  BXVector activeBX;
  SampleVector amplitudes;
 
  fullpulse.resize(2*NSAMPLES,1);   fullpulse.setZero(); 
  fullpulsecov.resize(2*NSAMPLES,2*NSAMPLES); fullpulsecov.setZero(); 
  noisecor.resize(NSAMPLES,NSAMPLES); noisecor.setZero(); 
  activeBX.resize(Eigen::NoChange,1);
  amplitudes.resize(NSAMPLES,1); amplitudes.setZero(); 
 
  std::cout << " default ready " << std::endl;
 
  std::vector<double> pulseShapeTemplate;
 

  for(int i=0; i<(NSAMPLES+7*int(25 /NFREQ)); i++) {
   

    double x;
 
    x = double( IDSTART + NFREQ * i + 3*25. - 500 / 2. );  //----> 500 ns is fixed!  
  
    pulseShapeTemplate.push_back( pSh.fShape(x));
  
    td::cout << " [" << i << "::" << (NSAMPLES+2*25 /NFREQ) << "] --> pSh.fShape(" << x << ") = " << pSh.fShape(x) << " ---> " << pSh.fShape(x) * NFREQ/25. << std::endl;
 
  }
 
  for (int i=1; i<(NSAMPLES + 2*int(25 /NFREQ)); i++) {
  
    fullpulse(i + 7 * int(25 /NFREQ)) = pulseShapeTemplate[i];
  }
 
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
  std::cout << " totalNumberOfBxActive = " << totalNumberOfBxActive << std::endl;
 
  std::vector<int> activeBXs;
  for (unsigned int ibx=0; ibx<totalNumberOfBxActive; ++ibx) {

    activeBXs.push_back( ibx * int(25 /NFREQ) - 4 * int(25 /NFREQ) ); //----> -5 BX are active w.r.t. 0 BX
  
    std::cout << " activeBXs[" << ibx << "] = " << activeBXs[ibx] << std::endl;
  }
 
  activeBX.resize(totalNumberOfBxActive);
  for (unsigned int ibx=0; ibx<totalNumberOfBxActive; ++ibx) {
    activeBX.coeffRef(ibx) = activeBXs[ibx];
  } 
 
  std::cout << " end init " << std::endl;
 
 
 
  std::cout << " inputFile = " << inputFile << std::endl;
  TFile *file2 = new TFile(inputFile.c_str());

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
  
    tree->GetEntry(ievt);
  
    for(int i=0; i<NSAMPLES; i++){
      amplitudes[i] = samples->at(i);
    }
  
    double pedval = 0.;
    double pedrms = 1.0;
  
   
    pulsefunc.disableErrorCalculation();
  
    bool status = pulsefunc.DoFit( amplitudes, noisecor, pedrms, activeBX, fullpulse, fullpulsecov );
    double chisq = pulsefunc.ChiSq();
  
    ipulseintime = 0;
    for (unsigned int ipulse=0; ipulse<pulsefunc.BXs()->rows(); ++ipulse) {
      if ( ((int(pulsefunc.BXs()->coeff(ipulse))) * NFREQ/25 + 5) == 0) {
        ipulseintime = ipulse;
        break;
      }
    }
  
    double aMax = status ? (*(pulsefunc.X()))[ipulseintime] : 0.;
  
    for (unsigned int ipulse=0; ipulse<pulsefunc.BXs()->rows(); ++ipulse) {
      if (status) {
        if (ievt == 0) std::cout << " ip = " << ipulse << " --> [" <<  (int(pulsefunc.BXs()->coeff(ipulse))) << "] --> " << (int(pulsefunc.BXs()->coeff(ipulse))) * NFREQ/25 + 5  << " == " << (*(pulsefunc.X()))[ ipulse ] << std::endl;

          //---- YES
          samplesReco[ (int(pulsefunc.BXs()->coeff(ipulse))) * NFREQ/25 + 5] = (*(pulsefunc.X()))[ ipulse ];
    
      }
      else {
        samplesReco[ipulse] = -1;
      }
    }
  
    h01->Fill(aMax - amplitudeTruth);
  
    newtree->Fill();
  
  }
  // CONTINUED HERE //
  std::cout << "  Mean of REC-MC = " << h01->GetMean() << " GeV" << std::endl;
  std::cout << "   RMS of REC-MC = " << h01->GetRMS()  << " GeV" << std::endl;
 
 
  fout->cd();
  std::cout << " done (1) " << std::endl;
  h01->Write();
  std::cout << " done (2) " << std::endl;

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
