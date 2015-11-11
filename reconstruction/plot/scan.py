from ROOT import TH1F, TFile
from glob import glob
from math import sqrt, log
from sys import argv


def main(files):

    # The files to write histograms and raw text data too
    outputfile = open("results.txt", 'w')
    outrootfile = TFile("energy_hists.root", "RECREATE")

    # The number of bins to use in the histogram
    BINS = 10000

    # Misc. names only for printing out information as the code runs
    stat_names = ["NSAMPLE", "NFREQ", "Amplitude", "Pileup"]
    dev_names  = ["dev_mean", "std_dev", "std_dev_err",
                  "std_dev_ratio", "std_dev_ratio_err"]
    for name in stat_names + dev_names:
        outputfile.write('{0:>17} '.format(name))
    outputfile.write('\n')
    for i in enumerate(stat_names + dev_names):
        outputfile.write('{0:->17} '.format(""))
    outputfile.write('\n')

    # Histograms for standard deviations / event energy
    dev_ratio_hist = TH1F("Energy_Histogram_all_events","", BINS, -0.1, 0.1)

    # Accessing each file and making the histograms
    print "\nProcessing ", len(files), "files\n"
    for i, f in enumerate(files):

        # Accessing the file
        print "File", i+1, ":", f
        in_file = TFile(f, "Read")
        tree = in_file.Get("RecoAndSim")
        entries = tree.GetEntries()

        # Getting and checking the stats for the file
        #stats = [NSAMPLE, NFREQ, AMP, PU]
        tree.GetEntry(0)
        stats = [tree.nSmpl, tree.nFreq, tree.amplitudeTruth , tree.nPU]

        dev_ratio_hist.Reset()

        # Histogram for standard eviation
        dev_hist = TH1F("dev_hist", "", BINS, 
                        tree.amplitudeTruth + 5, tree.amplitudeTruth + 5)

        # Filling the energy distribution histograms
        for event in range(0, entries):
            tree.GetEntry(event)
            event_amplitude = tree.signalTruth
            for sample in tree.samplesReco:
                if (sample > tree.amplitudeTruth / 2) :
                    dev_hist.Fill(sample - event_amplitude)
                    if (event_amplitude != 0):
                        dev_ratio_hist.Fill((sample - event_amplitude) / event_amplitude)
                    else:
                        dev_ratio_hist.Fill(sample - event_amplitude)
        # calculate uncertainty
        std_dev_ratio = dev_ratio_hist.GetStdDev()
        std_dev       = dev_hist.GetStdDev()

        std_dev_ratio_err = 1.0 / sqrt(2.0*(entries - 1.0))*std_dev_ratio
        std_dev_err       = 1.0 / sqrt(2.0*(entries - 1.0))*std_dev

        dev_mean = dev_hist.GetMean()

        deviation_metrics = [dev_mean, std_dev, std_dev_err,
                             std_dev_ratio, std_dev_ratio_err]

        # Print out and write all the information to a text file
        for stat, name in zip(stats, stat_names):
            print '{0:>17}: {1:>5}'.format(name, stat)
            outputfile.write('{0:>17.7f} '.format(stat))

        for dev, name in zip(deviation_metrics, dev_names):
            print '{0:>17}: {1:>11}'.format(name, dev)
            outputfile.write('{0:>17.7f} '.format(dev))

        outputfile.write("\n")

        outrootfile.cd()
        dev_ratio_hist.Write()
        dev_hist.Write()

    outputfile.close()
    in_file.Close()
    outrootfile.Close()
    
if __name__ == "__main__":
    
    args = []
    # for loop that converts arguments with a '*' to the corresponding files
    for arg in argv[1:]:
        args += glob(arg)
    print args
    main(args)
    
    
