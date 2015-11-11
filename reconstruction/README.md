Reconstruction step
====

Two steps:

    - create data with different pu, nsamples, nfreq, ...
    - run multifit

In the create step we can decide:

    - nPU : average number of pu (float)
    - NSAMPLES : number of sampled points out of the pulse
    - NFREQ: temporary space among points, in ns. nfreq must be defined such that the "in time" bunch crossing is one of it. e.g. 25 ns, 12.5 ns, 6.25 ns, ...
    - nEventsTotal: number of events to simulate
    - signalAmplitude: true signal amplitude of the interesting event.
    - shift: temporal shift of the simulated sampling. Useful to study the dependence on un-tracked temporal shifts of the pulse
    
Multifit need to know
    
    - NSAMPLES: how many points have been sampled
    - NFREQ: temporary space among points, in ns

Multifit code will automatically assign the "active bunch crosses", under the assumption that there is a collision every 25 ns.


How to Create data
====

First compile:

    g++ -o CreateData.exe  CreateData.C -std=c++11 `root-config --cflags --glibs`
    mkdir input
    
Then

    python createAll.py 
    python createAll.py 0 -----> to have a dry run, only "cout" and no actual running
    
It is an interface to the code

    CreateData.C
    
and it will create a lot of files, one for each configuration:
    
    NTOYS = 100
    NFREQS = [6.25, 12.5, 25]
    NSAMPLES = [10, 11, 12, 13, 14, 15, 20, 30, 40, 50, 60, 70, 80, 90]
    NPUS = [0, 20, 40, 140]
    AMPLITUDES = [1, 10, 100]



How to Fit data with multifit
====

First compile:

    g++ -o Example07.multifit.exe Example07.multifit.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
    mkdir outputfit
    
Then

    python fitAll.py
    python fitAll.py 0 -----> to have a dry run, only "cout" and no actual running
    
It is an interface to the code
    
    Example07.multifit.exe
    
    
Plot the results
====

see the plot folder

    
    
    
    
    
    
    
    
    
    
    