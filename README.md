# vfe-toymc

Example07.multifit
------------------

Multifit reconstruction
One need to make sure that Eigen is installed
New tests


Create data:

    g++ -o CreateData.exe  CreateData.C -std=c++11 `root-config --cflags --glibs`

    ./CreateData.exe     
    ./CreateData.exe    temporal_shift      number_of_events       NSAMPLES       NFREQ

e.g.

    ./CreateData.exe    0      1000    10    25   -->  10 samples sampled every 25ns
    ./CreateData.exe    0      1000    10     5   -->  10 samples sampled every 5ns
    ./CreateData.exe    0      1000    30     5   -->  30 samples sampled every 5ns
    ./CreateData.exe    0      1000    50     5   -->  50 samples sampled every 5ns
    ./CreateData.exe    0      1000    20     5   -->  20 samples sampled every 5ns
    ./CreateData.exe    0      1000    20    25   -->  20 samples sampled every 25ns
    ./CreateData.exe    0      1000    11    25   -->  11 samples sampled every 25ns
    ./CreateData.exe    0      1000    15    25   -->  15 samples sampled every 25ns
    ./CreateData.exe    0      1000    80    6.25 -->  80 samples sampled every 6.25ns

        

Analyse data:

    g++ -o Example07.multifit.exe Example07.multifit.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`

To run

    ./Example07.multifit.exe 

                               shift _ NSAMPLES _ NFREQ                NSAMPLES       NFREQ
    ./Example07.multifit.exe mysample_0_50_5.root    output.0.50.5.root       50       5
    ./Example07.multifit.exe mysample_0_30_5.root    output.0.30.5.root       30       5
    ./Example07.multifit.exe mysample_0_20_5.root    output.0.20.5.root       20       5
    ./Example07.multifit.exe mysample_0_10_25.root   output.0.10.25.root      10      25
    ./Example07.multifit.exe mysample_0_20_25.root   output.0.20.25.root      20      25
    ./Example07.multifit.exe mysample_0_11_25.root   output.0.11.25.root      11      25
    ./Example07.multifit.exe mysample_0_15_25.root   output.0.15.25.root      15      25
    ./Example07.multifit.exe mysample_0_80_6.25.root      output.0.80.6.25.root      80      6.25
    
    PS: don't be bothered by the error messages from ROOT at the end of the code run ...
    
Look at results:

    r99t plot/plot.C\(\"output.0.50.5.root\"\)
    r99t plot/plot.C\(\"output.0.30.5.root\"\)
    r99t plot/plot.C\(\"output.0.20.5.root\"\)
    r99t plot/plot.C\(\"output.0.20.25.root\"\)
    r99t plot/plot.C\(\"output.0.11.25.root\"\)
    r99t plot/plot.C\(\"output.0.15.25.root\"\)
    r99t plot/plot.C\(\"output.0.80.6.25.root\"\)


    r99t plot/plotPulse.C\(\"output.0.50.5.root\"\)
    r99t plot/plotPulse.C\(\"output.0.15.25.root\"\)
    r99t plot/plotPulse.C\(\"output.0.30.5.root\"\)
    r99t plot/plotPulse.C\(\"output.0.80.6.25.root\"\)




