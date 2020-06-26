#include "TFile.h"
#include "TGraph.h"


class Pulse{
public:
    Pulse(int);
    ~Pulse();
    double Value(double t) { return grPulse_->Eval(t); };
private:
    TGraph *grPulse_;
};



Pulse::Pulse(int iopt)
{
  TFile *file = new TFile("200626_Signal_Spike_Pulse_Shapes.root");
  if(iopt==0){ // Signal Phase 1
    grPulse_ = (TGraph*)file->Get("grSignal_Legacy");
  }else if(iopt==1){ // Spike Phase 1
    grPulse_ = (TGraph*)file->Get("grSpike_Legacy");
  }else if(iopt==2){ // Signal Phase 2
    grPulse_ = (TGraph*)file->Get("grSignal_Phase2");
  }else if(iopt==3){ // Spike Phase 1
    grPulse_ = (TGraph*)file->Get("grSpike_Phase2");
  }
  file->Close();
}



Pulse::~Pulse()
{
  delete grPulse_;
}

