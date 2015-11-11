//
// Fit one event with a pulse shape
// To run:
// > root -l Example07.C+
//

#include "Pulse.h"
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TRandom.h>
#include <iostream>


Pulse pSh;


pair<double,double> effectiveSigma(vector<double> a)
{
  std::sort(a.begin(),a.end());
  unsigned int nInInterval = int(0.68*a.size());
  double minInterval = 1e+9;
  double amin = 0;
  double amax = 0;
  for (unsigned int i=0; i<a.size(); i++) {
    double interval_size=minInterval;
    if ((i+nInInterval)<a.size()) interval_size = (a[i+nInInterval]-a[i]);
    if (interval_size < minInterval){
      minInterval = interval_size;
      amin = a[i];
      amax = amin + minInterval;
    }
  }
  pair<double,double> result;
  result.first  = amin;
  result.second = amax;
  return result;     
}



double funcPulseShape( double *x, double *par )
{
  double t = x[0] - par[0];
  return par[1] + par[2] * pSh.fShape(t);
}



void Example07()
{
  // Fit function
  
  TF1 *fPulseShape = new TF1("fPulseShape", funcPulseShape, -500, 500, 3);
  fPulseShape->SetNpx(1000);

  // Get one event with samples

  //  TFile *file2 = new TFile("data/samples_qie25_signal_10GeV_pu_0.root");
  //  TFile *file2 = new TFile("data/samples_signal_10GeV_pu_0.root");
  //  TFile *file2 = new TFile("data/samples_signal_10GeV_eta_0.0_pu_140.root");
  TFile *file2 = new TFile("data/samples_test.root");

  double samples[NSAMPLES];
  double amplitudeTruth;
  TTree *tree = (TTree*)file2->Get("Samples");
  tree->SetBranchAddress("amplitudeTruth",      &amplitudeTruth);
  tree->SetBranchAddress("samples",             samples);

  tree->GetEntry(10);

  // Create TGraphErrors with the pulse to fit
  
  TGraphErrors *gr = new TGraphErrors();
  for(int i=0; i<NSAMPLES; i++){
    double x = i * NFREQ;  
    gr->SetPoint(i, x, samples[i]);
    gr->SetPointError(i, 0., 0.044);  // 44 MeV for all samples
  }

  // Fit 
  
  fPulseShape->SetParameters(70., 0., amplitudeTruth);
  fPulseShape->SetLineColor(2);
  gr->Fit("fPulseShape","E");
  gr->Draw("AP");
  

  // Reconstructed amplitude and timing
  vector<double> resultsA;
  vector<double> resultsT;

  int nentries = tree->GetEntries();
  for(int ievt=0; ievt<nentries; ievt++){
    tree->GetEntry(ievt);
    TGraphErrors *gr = new TGraphErrors();
    for(int i=0; i<NSAMPLES; i++){
      double x = i * NFREQ;  
      gr->SetPoint(i, x, samples[i]);
      gr->SetPointError(i, 0., 0.044);  // 44 MeV for all samples
    }
    fPulseShape->SetParameters(70., 0., amplitudeTruth);
    gr->Fit("fPulseShape","QE");
    resultsA.push_back(fPulseShape->GetParameter(2)-amplitudeTruth);
    resultsT.push_back(fPulseShape->GetParameter(0));
  }

  if(resultsA.size()>0){
    pair<double,double> effA = effectiveSigma(resultsA);
    double avgValue = 0.5 * (effA.first + effA.second);
    double sigValue = 0.5 * fabs(effA.second - effA.first) ;
    double avgError = sigValue / sqrt(double(resultsA.size()));
    double sigError = sigValue / sqrt(2.0 * double(resultsA.size()));
    cout << " Mean of amplitude      " << avgValue << " +- " << avgError << " GeV" << endl;
    cout << " Sigma_eff of amplitude " << sigValue << " +- " << sigError << " GeV" << endl;
  }

  if(resultsT.size()>0){
    pair<double,double> effT = effectiveSigma(resultsT);
    double avgValue = 0.5 * (effT.first + effT.second);
    double sigValue = 0.5 * fabs(effT.second - effT.first) ;
    double avgError = sigValue / sqrt(double(resultsT.size()));
    double sigError = sigValue / sqrt(2.0 * double(resultsT.size()));
    cout << " Mean of timing      " << avgValue << " +- " << avgError << " ns" << endl;
    cout << " Sigma_eff of timing " << sigValue << " +- " << sigError << " ns" << endl;
  }

  
}
