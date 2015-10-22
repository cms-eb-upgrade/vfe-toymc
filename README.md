# vfe-toymc

Example07.multifit
------------------

Multifit reconstruction
One need to make sure that Eigen is installed
New tests


Create data:

    g++ -o CreateData.exe  CreateData.C -std=c++11 `root-config --cflags --glibs`

    ./CreateData.exe     
    ./CreateData.exe    temporal_shift      number_of_events       NSAMPLES       NFREQ     nPU    signalAmplitude

e.g.

    ./CreateData.exe    0      1000     6    25   -->   6 samples sampled every 25ns
    ./CreateData.exe    0      1000    10    25   -->  10 samples sampled every 25ns
    ./CreateData.exe    0      1000    10     5   -->  10 samples sampled every 5ns
    ./CreateData.exe    0      1000    30     5   -->  30 samples sampled every 5ns
    ./CreateData.exe    0      1000    50     5   -->  50 samples sampled every 5ns
    ./CreateData.exe    0      1000    20     5   -->  20 samples sampled every 5ns
    ./CreateData.exe    0      1000    20    25   -->  20 samples sampled every 25ns
    ./CreateData.exe    0      1000    11    25   -->  11 samples sampled every 25ns
    ./CreateData.exe    0      1000    15    25   -->  15 samples sampled every 25ns
    ./CreateData.exe    0      300     80    6.25 -->  80 samples sampled every 6.25ns
    ./CreateData.exe    0      1000    40    6.25 -->  40 samples sampled every 6.25ns
    ./CreateData.exe    0      100     40    6.25 -->  40 samples sampled every 6.25ns
    ./CreateData.exe    0      300     40    6.25 -->  40 samples sampled every 6.25ns
    ./CreateData.exe    0      300     20   12.5  -->  20 samples sampled every 12.5ns

     ./CreateData.exe    0     100    10     25   
     ./CreateData.exe    0     100    11     25   
     ./CreateData.exe    0     100    12     25   
     ./CreateData.exe    0     100    13     25   
     ./CreateData.exe    0     100    14     25   
     ./CreateData.exe    0     100    15     25   
     ./CreateData.exe    0     100    20     25   
     ./CreateData.exe    0     100    50     25   
     ./CreateData.exe    0     100    40    6.25   
     ./CreateData.exe    0     100    20   12.5   
     
     
     ./CreateData.exe    0     100    10     25     19.5
     ./CreateData.exe    0     100    10     25     20
     
     ./CreateData.exe    0     100    40    6.25    20     0.1
     ./CreateData.exe    0     100    40    6.25    20     0.5
     ./CreateData.exe    0     100    40    6.25    20     1
     ./CreateData.exe    0     100    10     25     20     0.1
     ./CreateData.exe    0     100    10     25     20     0.5
     ./CreateData.exe    0     100    10     25     20     1
     ./CreateData.exe    0     100    10     25     20     2
     ./CreateData.exe    0     100    10     25     20     10
     ./CreateData.exe    0     100    10     25     20     50
     ./CreateData.exe    0     100    10     25     20     100
     
     ./CreateData.exe    0     100    15     25     20     10
     
     ./CreateData.exe    0     100    45      5      0     10
     ./CreateData.exe    0     100    50      5      0     10

     
     for i in `seq 1 10`;
        do
                let npul=$i+10
                echo "./CreateData.exe    0     100    $npul     25     20     10"
        done 
        
    python createAll.py 
     

Analyse data:

    g++ -o Example07.multifit.exe Example07.multifit.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`

To run

    ./Example07.multifit.exe 

                               shift _ NSAMPLES _ NFREQ                NSAMPLES       NFREQ
    ./Example07.multifit.exe mysample_0_50_5.00.root    output.0.50.5.root       50       5
    ./Example07.multifit.exe mysample_0_30_5.00.root    output.0.30.5.root       30       5
    ./Example07.multifit.exe mysample_0_20_5.00.root    output.0.20.5.root       20       5
    ./Example07.multifit.exe mysample_0_10_25.00.root   output.0.10.25.root      10      25
    ./Example07.multifit.exe mysample_0_20_25.00.root   output.0.20.25.root      20      25
    ./Example07.multifit.exe mysample_0_11_25.00.root   output.0.11.25.root      11      25
    ./Example07.multifit.exe mysample_0_15_25.00.root   output.0.15.25.root      15      25
    ./Example07.multifit.exe mysample_0_80_6.25.root      output.0.80.6.25.root      80      6.25
    ./Example07.multifit.exe mysample_0_40_6.25.root      output.0.40.6.25.root      40      6.25
    ./Example07.multifit.exe mysample_100_0_40_6.25.root      output.0.40.6.25.root      40      6.25
    ./Example07.multifit.exe mysample_300_0_40_6.25.root      output.0.40.6.25.root      40      6.25
    ./Example07.multifit.exe mysample_300_0_80_6.25.root      output.0.80.6.25.root      80      6.25
    ./Example07.multifit.exe mysample_300_0_20_12.50.root      output.0.20.12.5.root      20      12.5
    ./Example07.multifit.exe mysample_1000_0_6_25.00.root   output.0.6.25.root      6      25
    ./Example07.multifit.exe mysample_1000_0_10_25.00.root   output.0.10.25.root      10      25
    ./Example07.multifit.exe mysample_1000_0_20_25.00.root   output.0.20.25.root      20      25

    ./Example07.multifit.exe mysample_100_0_10_25.00.root      output.0.10.25.00.root     10      25
    ./Example07.multifit.exe mysample_100_0_11_25.00.root      output.0.11.25.00.root     11      25
    ./Example07.multifit.exe mysample_100_0_12_25.00.root      output.0.12.25.00.root     12      25
    ./Example07.multifit.exe mysample_100_0_13_25.00.root      output.0.13.25.00.root     13      25
    ./Example07.multifit.exe mysample_100_0_14_25.00.root      output.0.14.25.00.root     14      25
    ./Example07.multifit.exe mysample_100_0_15_25.00.root      output.0.15.25.00.root     15      25
    ./Example07.multifit.exe mysample_100_0_20_25.00.root      output.0.20.25.00.root     20      25
    ./Example07.multifit.exe mysample_100_0_20_12.50.root      output.0.20.12.5.root      20      12.5
    ./Example07.multifit.exe mysample_100_0_40_6.25.root       output.0.40.6.25.root      40      6.25
    
    ./Example07.multifit.exe mysample_100_0_10_25.00_20.00.root      output.0.10.25.00.20.00.root     10      25
    
    ./Example07.multifit.exe mysample_100_0_45_5.00_10.00.root      output.root         45      5
    ./Example07.multifit.exe mysample_100_0_50_5.00_10.00.root      output.root         50      5

    ./Example07.multifit.exe mysample_100_0_50_5.00_10.00_0.00.root      output.root         50      5
    
    
    ./Example07.multifit.exe mysample_100_0_40_6.25_0.10_20.00.root       output.0.40.6.25.20.00.0.1GeV.root        40      6.25
    ./Example07.multifit.exe mysample_100_0_40_6.25_0.50_20.00.root       output.0.40.6.25.20.00.0.5GeV.root        40      6.25
    ./Example07.multifit.exe mysample_100_0_40_6.25_1.00_20.00.root       output.0.40.6.25.20.00.1GeV.root        40      6.25
    ./Example07.multifit.exe mysample_100_0_10_25.00_0.10_20.00.root      output.0.10.25.00.20.00.0.1GeV.root       10      25
    ./Example07.multifit.exe mysample_100_0_10_25.00_0.50_20.00.root      output.0.10.25.00.20.00.0.5GeV.root       10      25

    ./Example07.multifit.exe mysample_100_0_10_25.00_1.00_20.00.root      output.0.10.25.00.20.00.1GeV.root       10      25
    ./Example07.multifit.exe mysample_100_0_10_25.00_2.00_20.00.root      output.0.10.25.00.20.00.2GeV.root       10      25
    ./Example07.multifit.exe mysample_100_0_10_25.00_5.00_20.00.root      output.0.10.25.00.20.00.5GeV.root       10      25
    ./Example07.multifit.exe mysample_100_0_10_25.00_10.00_20.00.root     output.0.10.25.00.20.00.10GeV.root      10      25
    ./Example07.multifit.exe mysample_100_0_10_25.00_50.00_20.00.root     output.0.10.25.00.20.00.50GeV.root      10      25
    ./Example07.multifit.exe mysample_100_0_10_25.00_100.00_20.00.root    output.0.10.25.00.20.00.100GeV.root     10      25
    
    ./Example07.multifit.exe mysample_100_0_15_25.00_10.00_20.00.root     output.0.15.25.00.20.00.10GeV.root      15      25


    
    ./Example07.multifit.exe mysample_100_0_10_25.00_10.00_0.00.root      output_100_0_10_25.00_10.00_0.00.root       10      25
    ./Example07.multifit.exe mysample_100_0_11_25.00_10.00_0.00.root      output_100_0_11_25.00_10.00_0.00.root       11      25
    ./Example07.multifit.exe mysample_100_0_12_25.00_10.00_0.00.root      output_100_0_12_25.00_10.00_0.00.root       12      25
    ./Example07.multifit.exe mysample_100_0_13_25.00_10.00_0.00.root      output_100_0_13_25.00_10.00_0.00.root       13      25
    ./Example07.multifit.exe mysample_100_0_14_25.00_10.00_0.00.root      output_100_0_14_25.00_10.00_0.00.root       14      25
    ./Example07.multifit.exe mysample_100_0_15_25.00_10.00_0.00.root      output_100_0_15_25.00_10.00_0.00.root       15      25

    
    python fitAll.py
    python fitAll.py  0
    
    PS: don't be bothered by the error messages from ROOT at the end of the code run ...
    
Look at results:

    r99t plot/plot.C\(\"output.0.50.5.root\"\)
    r99t plot/plot.C\(\"output.0.30.5.root\"\)
    r99t plot/plot.C\(\"output.0.20.5.root\"\)
    r99t plot/plot.C\(\"output.0.20.25.root\"\)
    r99t plot/plot.C\(\"output.0.11.25.root\"\)
    r99t plot/plot.C\(\"output.0.15.25.root\"\)
    r99t plot/plot.C\(\"output.0.80.6.25.root\"\)
    r99t plot/plot.C\(\"output.0.40.6.25.root\"\)
    r99t plot/plot.C\(\"output.0.80.6.25.root\"\)
    r99t plot/plot.C\(\"output.0.20.12.5.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.root\"\)

    r99t plot/plotPulse.C\(\"output.0.10.25.root\"\)
    r99t plot/plotPulse.C\(\"output.0.50.5.root\"\)
    r99t plot/plotPulse.C\(\"output.0.15.25.root\"\)
    r99t plot/plotPulse.C\(\"output.0.30.5.root\"\)
    r99t plot/plotPulse.C\(\"output.0.80.6.25.root\"\)
    r99t plot/plotPulse.C\(\"output.0.40.6.25.root\"\)
    r99t plot/plotPulse.C\(\"output.0.20.25.root\"\)
    r99t plot/plotPulse.C\(\"output.0.20.12.5.root\"\)

    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.11.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.12.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.13.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.14.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.15.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.20.25.00.root\"\)
    r99t plot/plotPulse.C\(\"output.0.20.12.5.root\"\)
    r99t plot/plotPulse.C\(\"output.0.40.6.25.root\"\)


    r99t plot/plot.C\(\"output.0.10.25.00.20.00.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.11.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.12.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.13.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.14.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.15.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.20.25.00.root\"\)
    r99t plot/plot.C\(\"output.0.20.12.5.root\"\)
    r99t plot/plot.C\(\"output.0.40.6.25.root\"\)

    
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.1GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.2GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.5GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.10GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.50GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.100GeV.root\"\)


    r99t plot/plot.C\(\"output.0.40.6.25.20.00.0.1GeV.root\"\)
    r99t plot/plot.C\(\"output.0.40.6.25.20.00.0.5GeV.root\"\)
    r99t plot/plot.C\(\"output.0.40.6.25.20.00.1GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.0.1GeV.root\"\)
    r99t plot/plot.C\(\"output.0.10.25.00.20.00.0.5GeV.root\"\)


    r99t plot/plotPulse.C\(\"output.0.40.6.25.20.00.0.1GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.40.6.25.20.00.0.5GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.40.6.25.20.00.1GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.0.1GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.0.5GeV.root\"\)

    
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.1GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.2GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.5GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.10GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.50GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.10.25.00.20.00.100GeV.root\"\)
        
        
    r99t plot/plot.C\(\"output.0.15.25.00.20.00.10GeV.root\"\)
    r99t plot/plotPulse.C\(\"output.0.15.25.00.20.00.10GeV.root\"\)
    r99t plot/plotPulseInput.C\(\"mysample_100_0_15_25.00_10.00_20.00.root\"\)
        
        
    r99t plot/plotPulse.C\(\"output.root\"\)
    
    
    r99t plot/plotPulseInput.C\(\"mysample_300_0_40_6.25.root\"\)

    r99t plot/plotPulseInput.C\(\"mysample_100_0_10_25.00.root\"\)
    r99t plot/plotPulseInput.C\(\"mysample_100_0_20_25.00.root\"\)
    r99t plot/plotPulseInput.C\(\"mysample_100_0_20_12.5.root\"\)
    r99t plot/plotPulseInput.C\(\"mysample_100_0_40_6.25.root\"\)

    
    


