#include "Signal_and_Spikes.h"
#include "TProfile.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "Riostream.h"

TRandom rnd;



void ExampleA(int iPhase=1)
{
    int idSignal = 0;
    int idSpike  = 1;
    int nSamples = 10;
    double tStep = 25;
    
    if(iPhase == 2){
        idSignal = 2;
        idSpike  = 3;
        nSamples = 20;
        tStep    = 6.25;
    }
    
    Pulse *pulseS = new Pulse(idSignal);
    Pulse *pulseA = new Pulse(idSpike);
    double spikeTime = 0.7;      // timing of prompt spikes
    double signalTime = 0.0;     // timing for prompt signal (by definition)
    
    TGraph *grSignalAnalog = new TGraph();
    TGraph *grSpikeAnalog  = new TGraph();
    for(int i=0; i<200; i++){
        double t = tStep * nSamples / 200 * i - 0.5 * tStep;
        grSignalAnalog->SetPoint(i, t, pulseS->Value(t - signalTime));
        grSpikeAnalog->SetPoint(i, t, pulseA->Value(t - spikeTime));
    }

    TGraph *grSignalDigital = new TGraph();
    TGraph *grSpikeDigital  = new TGraph();
    for(int i=0; i<nSamples; i++){
        double t = tStep * i;
        grSignalDigital->SetPoint(i, t, pulseS->Value(t - signalTime));
        grSpikeDigital->SetPoint(i, t, pulseA->Value(t - spikeTime));
    }
    
    grSignalAnalog->SetLineColor(4);
    grSpikeAnalog->SetLineColor(2);
    grSignalDigital->SetMarkerColor(4);
    grSpikeDigital->SetMarkerColor(2);
    
    grSpikeAnalog->GetXaxis()->SetTitle("recorded time (ns)");
    grSpikeAnalog->GetYaxis()->SetTitle("amplitude for 1 GeV hitb");
    
    grSpikeAnalog->Draw("AL");
    grSignalAnalog->Draw("L");
    grSpikeDigital->Draw("P");
    grSignalDigital->Draw("P");
}



void ExampleB(int ieta=85, int iPhase=1)
{
    double energyCut = 1.0;
    
    // define pulse shapes for Spikes
    
    int idSpike  = 1;
    int nSamples = 10;
    double tStep = 25;
    
    if(iPhase == 2){
        idSpike  = 3;
        nSamples = 20;
        tStep    = 6.25;
    }
    
    Pulse *pulse = new Pulse(idSpike);

    // Load PDFs for spike time and energy
    
    TFile *fin = new TFile("200626_apd_signal_pdf_v2.root");
    TProfile * hProbability = (TProfile*)fin->Get("h2");
    TH2D * hTvsE = (TH2D*)fin->Get("h4");
    hProbability->SetDirectory(0);
    hTvsE->SetDirectory(0);
    fin->Close();
    
    // Generate DIGI for good events
    
    int nGoodEvents = 0;
    double nTotalEvents = 0;
    while(nGoodEvents < 10 && nTotalEvents < 1e+7){
        
        nTotalEvents += 1.;
        
        std::vector<double> times;
        std::vector<double> amplitudes;
        for(int BX=-10; BX<5; BX++){
            double e,t;
            if(rnd.Rndm() < hProbability->GetBinContent(ieta)){
                hTvsE->GetRandom2(e,t);
                amplitudes.push_back(pow(10,e));
                times.push_back(t + 25. * BX);
            }
        }
        
        // check if this is an interesting event 
        
        double totalEnergy = 0;
        for(unsigned int i=0; i<amplitudes.size(); i++){
            totalEnergy += amplitudes[i];
        }
        if( totalEnergy > energyCut ){
             
            nGoodEvents++;

            // make DIGI frame
            std::vector<double> digis;
            
            for(int i=0; i<nSamples; i++){
                double tSample = tStep * i;
                double amplitude = 0;
                for(unsigned int ihit=0; ihit<amplitudes.size(); ihit++){
                    amplitude += amplitudes[ihit] * pulse->Value(tSample - times[ihit]);
                }
                digis.push_back(amplitude);
            }
            
            // print
            cout << " (T,A) = ";
            for(unsigned int i=0; i<times.size(); i++){
                cout << "( " << times[i] << "," << amplitudes[i] << " ) ";
            }
            cout << endl;
            for(unsigned int i=0; i<digis.size(); i++){
                cout << digis[i] << "  ";
            }
            cout << endl;
            cout << endl;
        }
    }
    cout << " Total events = " << nTotalEvents << "   Good events = " << nGoodEvents << endl;

}
