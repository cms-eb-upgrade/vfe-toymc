from ROOT import *
from glob import glob
from math import sqrt, log
from sys import argv
import re


def scan(files):
    # Output file
    #out_file = TFile("scan_plots.root", "Recreate")
    # The number of bins to use in the histogram
    #BINS = 200
    BINS = 10000

    # Histograms for standard deviations vs. vairous values
    hist = TH1F("hist","", BINS, -0.1, 0.1)

    outputfile = open("results.txt", 'a')

    outrootfile = TFile("outfit.root", "RECREATE")

    # Accessing each file and making the histograms
    for i, f in enumerate(files):
    
        # Accessing the file
        print "File", i+1, ":", f
        in_file = TFile(f, "Read")
        tree = in_file.Get("RecoAndSim")
        nentries = tree.GetEntries()

        # Getting and checking the stats for the file
        #stats = [NSAMPLE, NFREQ, AMP, PU]
        tree.GetEntry(0)
        stats = [ tree.nSmpl, tree.nFreq, tree.amplitudeTruth , tree.nPU ]

        hist.Reset()
        
        #hist_delta_energy = TH1F("hist_delta_energy","", BINS, -tree.amplitudeTruth/10 , tree.amplitudeTruth/10)
        hist_delta_energy = TH1F("hist_delta_energy","", BINS, -log(tree.amplitudeTruth+1)/tree.amplitudeTruth*2 , log(tree.amplitudeTruth+1)/tree.amplitudeTruth*2)


        # Filling the energy distribution histograms
        for event in range(0, nentries):
          tree.GetEntry(event)
          for sample in tree.samplesReco:
            if (sample > tree.amplitudeTruth / 2) :
               hist_delta_energy.Fill(sample - tree.amplitudeTruth)
               hist.Fill( (sample - tree.amplitudeTruth) / tree.amplitudeTruth)

        # calculate uncertainty
        standard_dev_gev = hist_delta_energy.GetStdDev()
        standard_dev_gev_err = 1.0 / sqrt(2.0*(nentries - 1.0))*standard_dev_gev
        
        bias_gev = hist_delta_energy.GetMean()
        standard_dev_bias = hist_delta_energy.GetStdDev()
        standard_dev_bias_err = 1.0 / sqrt(2.0*(nentries - 1.0))*standard_dev_bias
        
        for inputs in stats:
          print " ", inputs,
        print "  ", bias_gev,
        print "  ", standard_dev_bias,
        print "  ", standard_dev_bias_err,
        print "  ", standard_dev_gev,
        print "  ", standard_dev_gev_err

        for inputs in stats:
          outputfile.write(" " + str(inputs))
        outputfile.write(" " + str(bias_gev))
        outputfile.write(" " + str(standard_dev_bias))
        outputfile.write(" " + str(standard_dev_bias_err))
        outputfile.write(" " + str(standard_dev_gev))
        outputfile.write(" " + str(standard_dev_gev_err))
        outputfile.write("\n")

        outrootfile.cd()
        hist.Write()
        hist_delta_energy.Write()

    outputfile.close()
    in_file.Close()
    outrootfile.Close()
    

if __name__ == "__main__":
    
    args = []
    # for loop that converts arguments with a '*' to the corresponding files
    for arg in argv[1:]:
        args += glob(arg)
    print args
    scan(args)
    
    