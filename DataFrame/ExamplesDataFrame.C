#include "DataFrame.h"
#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TLegend.h"


void PlotIntimePulseShapes()
{
  Pulse *pSignal = new Pulse(0);
  Pulse *pSpike  = new Pulse(1);

  TGraph *gr1 = new TGraph();
  TGraph *gr2 = new TGraph();
  for(int i=0; i<5000; i++){
    double t = -10. + 0.1 * i;
    double y1 = pSignal->Value(t);
    double y2 = pSpike->Value(t);
    gr1->SetPoint(i, t, y1);
    gr2->SetPoint(i, t, y2);
  }
  gr2->Draw("AL");
  gr1->Draw("L");
}



void PlotSignalDigisIdeal(double shift=0.)
{
  int    numberOfSamples = 20;
  double samplingStep    = 6.25;      // in nanoseconds
  double eta             = 1.4;
  
  Orbit *orbit   = new Orbit(eta);
  Pulse *pSignal = new Pulse(0);
  Frame *frame   = new Frame(numberOfSamples, samplingStep);
  frame->setLocal(14, -69.6);

  TGraph *waveform = new TGraph(); 
  for(int it = 0; it < frame->nsamples(); it++){
    for(int ip=0; ip<20; ip++){
      double tlocal = double(it) + 0.05 * ip;
      double t = shift + frame->timeLocal(0) + frame->timeStep() * tlocal;
      int np = 20 * it + ip;
      waveform->SetPoint(np, tlocal, pSignal->Value(t) );
    }
  }  

  TGraph *digis = new TGraph(); 
  for(int it = 0; it < frame->nsamples(); it++){
    double tlocal = double(it);
    double t = shift + frame->timeLocal(0) + frame->timeStep() * tlocal;
    int np = it;
    digis->SetPoint(np, tlocal, pSignal->Value(t) );
  }  

  waveform->Draw("AL");
  digis->Draw("P");
}



void PrintSignalDigisReal(double shift=0.)
{
  int    numberOfSamples = 20;
  double samplingStep    = 6.25;      // in nanoseconds
  double eta             = 1.4;

  double pu              = 200.0;
  double jitterCollision = 0.2;       // in nanoseconds
  double amplitudeSignal = 10.0;      // in GeV;
  double noiseRMS        = 0.2;       // in GeV;
  
  Orbit *orbit   = new Orbit(eta);
  Pulse *pSignal = new Pulse(0);
  Frame *frame   = new Frame(numberOfSamples, samplingStep);
  frame->setLocal(14, -69.6 + shift);

  int nevents = 10;
  for(int ievt = 0; ievt < nevents; ievt++){
    
    vector<double> amplitudes;
      
    // Pure signal
    for(int it = 0; it < frame->nsamples(); it++){    
      amplitudes.push_back( amplitudeSignal * pSignal->Value(frame->timeLocal(it) + rnd.Gaus(0,jitterCollision)) );
    }
    
    // Add noise  
    frame->newFrame();
    for(int it = 0; it < frame->nsamples(); it++){
      amplitudes[it] += noiseRMS * frame->noiseC(it);
    }
    
    //Pileup
    if(pu>1){
      orbit->newOrbit(pu,0,orbit->nbx());
      int ibx0 = int(orbit->nbx() * rnd.Rndm());
      for(unsigned int ibx=0; ibx<orbit->nbx(); ibx++){
	for(int it=0; it<frame->nsamples(); it++){
	  amplitudes[it] += orbit->Epu(ibx) * pSignal->Value(frame->timeLocal(it) + 25. * (ibx0 - ibx));
	}
      }
    }

    // Printout
    cout << " Event = " << ievt << endl;
    cout << " Amplitudes : " << endl;
    for(unsigned int it=0; it<amplitudes.size(); it++){
      cout << amplitudes[it] << "  ";
    }
    cout << endl;
    
  }
}
