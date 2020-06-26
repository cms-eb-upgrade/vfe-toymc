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
data/EmptyFileQIE6.root     Charge integration with gate 6 ns



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
Errors on amplitude and timing are estimated using RMS of histograms


Example06
---------

Multifit reconstruction
One need to make sure that Eigen is installed
To compile:
> g++ -o Example06 Example06.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
To run
> ./Example06 


Example07
---------

Same exercise as in Example05 except errors on amplitude and timing
are estimated using effective sigma: shortest interval that covers
68%. The advantages of this approach:
- no need to use histograms
- works for non-Gaussian distributions


Example08
----------

Simulation of APD spike energy and timing. Absolute rate is calculated
as well.  Timing is ToF adjusted and modulo 25ns. Hence spikes from
earlier BX are accounted for.  Based on CMSSW simulations for Phase2
ECAL. Represents an average EB channel (no eta dependence of APD spike
energy/time)


PhotoCurrent.C
---------------

Generate waveform of photo-current in APD in a presense of pileup
One histogram per event.
Horizontal axis is in ns.
Vertical axis is in microAmps.
Assumes EB light output of 4.5 pe/MeV and APD gain 50

suggestions to run:
 - change inputs (see more details in PhotoCurrent.C)
 - run command:
 > root -l -q PhotoCurrent.C+
 
 
DataFrame
------------

This subdirectory contains examples related to digitization of signal with TIA (or CATIA).

Pileup and noise is also simulated.  
Noise includes correlations.  
Number of samples and sampling frequency (sampling step) should be specified.

Instructions to run:  
`> cd DataFrame`  
`> root -l`  
`[] .L ExamplesDataFrame.C+`  
`[] PlotSignalDigisIdeal()`  
`[] PrintSignalDigisReal()`  

 
Signal_and_Spikes
------------

This subdirectory contains files with various spike PDFs as descibed in CMS DN-17/028
Two examples provided:

First, plots analog waveform and digital samples for prompt spike and signal. One can choose phase 1 or phase 2 configuration  

`> cd Signal_and_Spikes`  
`> root -l`  
`[] .L Signal_and_Spikes.C+`  
`[] ExampleA(1) or ExampleA(2)`  

Second, prints few digis for spikes above 1 GeV. One can choose ieta of a channel and phase 1/2 configuration.  
For example,  
`[] ExampleB(85,1)`  
