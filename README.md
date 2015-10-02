# vfe-toymc
Toy MC to study VFE pulse shapes.


A set of examples for standalone.
It is a toy Monte Carlo for simulation and reconstruction of pulses.
All files here are for VFE scenario: CRRC 43ns pulse shaping
It is defined in Pulse.h

Example01
---------

Produces a file with information about pulse shape.  The output is a
root file with TGraph (pulse shape) and 3 parameters to describe the
tail of the pulse shape.

File data/EmptyFileCRRC43.root was produced with this example. It
corresponds to CRRC pulse shaper with tau=43ns. This file is a
starting point. This example is for info only. It does not need to be
repeated for CRRC 43ns shaper.

Similar files for other configurations has been produced:
data/EmptyFileCRRC10.root    CRRC tau=10 ns
data/EmptyFileCRRC20.root    CRRC tau=20 ns
data/EmptyFileCRRC30.root    CRRC tau=30 ns
data/EmptyFileCRRC43.root    CRRC tau=43 ns
data/EmptyFileCRRC60.root    CRRC tau=60 ns
data/EmptyFileCRRC90.root    CRRC tau=90 ns
data/EmptyFileQIE25.root    Charge integration with gate 25 ns
data/EmptyFileQIE12.root    Charge integration with gate 12 ns



Example02
---------

Produces waveforms and stores it in a separate file. Each waveform is
an array of amplitudes (GeV) as a function of time (ns) in 500 ns time
window. Pileup level and signal amplitude need to be defined in the
code.

Files

data/waveform_signal_10GeV_pu_0.root 
data/waveform_signal_10GeV_eta_0.0_pu_140.root

were produced with this example. It is a set of 100 waveforms for
a signal of 10 GeV and PU=0 and PU=140 respectively.


Example03
---------

Plots one waveform created in Example02


Example04
---------

Creates DIGIs: samples of amplitude with noise.
Noise is correlated.

Files

data/samples_signal_10GeV_pu_0.root 
data/samples_signal_10GeV_eta_0.0_pu_140.root

were created from

data/waveforms_signal_10GeV_pu_0.root 
data/waveforms_signal_10GeV_eta_0.0_pu_140.root

with average noise of 44 MeV and correlation matrix for CRRC 43ns



Example05
---------

Fit one event with a pulse shape and plot it
Compare reconstructed amplitude with MC truth.
Timing is also reconstructed.
Errors on amplitude and timing are estimated.


Example06
---------

Multifit reconstruction
One need to make sure that Eigen is installed
To compile:
> g++ -o Example06 Example06.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
To run
> ./Example06 



Example07.multifit
------------------

Multifit reconstruction
One need to make sure that Eigen is installed
New tests
To compile:

    g++ -o Example07.multifit.exe Example07.multifit.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`

To run

    ./Example07.multifit.exe 

    
    r99t -q CreateData.C
    
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_eta_0.0_pu_140.root\"\)
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\"\)
     
    ./Example07.multifit.exe mysample.root
    
    
    r99t plot/plot.C\(\"output.root\"\)

    
    g++ -o CreateData.exe  CreateData.C -std=c++11 `root-config --cflags --glibs`
    ./CreateData.exe    25
    ./CreateData.exe    25   1000
    
    
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\",25\)
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\",-25\)
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\",-50\)
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\",-75\)
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\",-100\)
    r99t -q CreateData.C\(\"data/waveform_signal_10GeV_pu_0.root\",-125\)
    
    ./Example07.multifit.exe mysample_0.root    output.0.root
    ./Example07.multifit.exe mysample_25.root   output.25.root
    ./Example07.multifit.exe mysample_-25.root  output.-25.root
    ./Example07.multifit.exe mysample_-50.root  output.-50.root
    ./Example07.multifit.exe mysample_-75.root  output.-75.root
    ./Example07.multifit.exe mysample_-100.root  output.-100.root
    ./Example07.multifit.exe mysample_-125.root  output.-125.root
    
    r99t plot/plot.C\(\"output.0.root\"\)
    r99t plot/plot.C\(\"output.25.root\"\)
    r99t plot/plot.C\(\"output.-25.root\"\)
    r99t plot/plot.C\(\"output.-50.root\"\)
    r99t plot/plot.C\(\"output.-75.root\"\)
    r99t plot/plot.C\(\"output.-100.root\"\)
    r99t plot/plot.C\(\"output.-125.root\"\)

    hadd output_all.root output.*.root
    r99t plot/plot.C\(\"output_all.root\"\)

    
25ns

   Mean of REC-MC = -0.00319768 GeV
   RMS of REC-MC = 0.0268833 GeV
        
12ns

   Mean of REC-MC = -0.00182634 GeV
   RMS of REC-MC = 0.026504 GeV
    