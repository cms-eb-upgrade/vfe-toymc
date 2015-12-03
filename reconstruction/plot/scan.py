from ROOT import TH1F, TFile
from glob import glob
from math import sqrt, log
from sys import argv


def main(files):

    # The files to write histograms and raw text data too
    outputfile = open("results.txt", 'w')
    outrootfile = TFile("energy_hists.root", "RECREATE")

    # The number of bins to use in the histogram
    BINS = 4000
    start_bin = -2
    end_bin = 2
    bin_size = 1.0 * (end_bin - start_bin) / BINS

    # The in-time bunch crossing number. Note: This will need to be changed if
    # in the future, changing the location of the sampling window is explored
    in_time_bx = 4

    # Logging information headers, only used for writing to output file
    stat_names = ["NSAMPLE", "NFREQ", "Amplitude", "Pileup", "Noise"]
    dev_names  = ["sigma_eff", "sigma_eff_err"]
    for name in stat_names + dev_names:
        outputfile.write('{0:>17} '.format(name))
    outputfile.write('\n')
    for i in enumerate(stat_names + dev_names):
        outputfile.write('{0:->17} '.format(""))
    outputfile.write('\n')

    # Accessing each file and making the histograms
    print "\nProcessing ", len(files), "file(s)\n"
    for i, f in enumerate(files):

        # Accessing the file
        print "File", i+1, ":", f
        in_file = TFile(f, "Read")
        tree = in_file.Get("RecoAndSim")
        entries = tree.GetEntries()

        # Getting the stats for the file
        tree.GetEntry(0)
        stats = [tree.nSmpl, tree.nFreq, tree.signalTruth , 
                 tree.nPU, tree.sigmaNoise]

        # Making the energy distribution histogram
        dev_hist_name = "Hist"
        dev_hist = TH1F("", "", BINS, start_bin, end_bin)
        for event in range(0, entries):
            tree.GetEntry(event)
            event_amplitude = tree.amplitudeTruth
            reco_amplitude = tree.samplesReco.at(in_time_bx)
            dev_hist.Fill(reco_amplitude - event_amplitude)

        # Filling the array for determining sigma_eff
        # Note: the first and last bins are overflow and currently this loop
        # includes those two bins
        dev_array = []
        for bin in range(0, BINS + 1):
            dev_array.append(dev_hist.GetBinContent(bin))

        # calculate uncertainty
        sigma = sigma_eff(dev_array, bin_size, dev_hist.GetEntries())

        # Logging information and terminal print outs
        for stat, name in zip(stats, stat_names):
            dev_hist_name += ("_" + name + "_" + str(round(stat, 3)))
            print '{0:>17}: {1:>5}'.format(name, stat)
            outputfile.write('{0:>17.7f} '.format(stat))

        for dev, name in zip(sigma, dev_names):
            print '{0:>17}: {1:>11}'.format(name, dev)
            outputfile.write('{0:>17.7f} '.format(dev))
        outputfile.write("\n")

        outrootfile.cd()
        dev_hist.SetName(dev_hist_name)
        dev_hist.SetTitle(dev_hist_name)
        dev_hist.Write()
        in_file.Close()

    outputfile.close()
    outrootfile.Close()

# Takes in an array and calculates the effective sigma, the windowed
# range which contains 68% of the data (one sigma). Returns a tuple 
# containing (sigma, sigma_error).
def sigma_eff(arr, bin_size, entries):
    length = len(arr)

    window = [0, length]
    window_size = window[1] - window[0]

    for start_bin in range(0, length - 1):
        sum_in_range = 0
        for end_bin in range(start_bin, length - 1):
            sum_in_range += arr[end_bin]
            if (sum_in_range >= 0.68 * entries) and \
               (end_bin - start_bin < window_size):
                window[0] = start_bin
                window[1] = end_bin
                window_size = end_bin - start_bin
                break
        else:
            # No window was found so we can drop out of the loop
            # Note: this is supposed to be in-line with the for-loop,
            # not the if statement
            break

    real_start = window[0]*bin_size - 2
    real_end = window[1]*bin_size - 2

    print "Window size of:", window_size, "Between (" + \
    str(real_start) + ", " + str(real_end) + ")"


    sigma = (window[1] - window[0])*bin_size / 2.0
    sigma_err = sigma / sqrt(2.0*(entries - 1.0))

    return (sigma, sigma_err)

if __name__ == "__main__":
    
    args = []
    # for loop that converts arguments with a '*' to the corresponding files
    for arg in argv[1:]:
        args += glob(arg)
    main(args)
