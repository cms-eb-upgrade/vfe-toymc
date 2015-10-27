import os
import argparse
from sys import argv

""" Creates the data for multifit over a range of values quickly by submitting
    all jobs to lxbatch. Lists of the desired parameters can be passed in at
    the command line using flags: 
        -s NSAMPLES 
        -f NFREQ
        -p NPU
        -a Amplitude
        -e Number of events
    The create_data and fit_data vars are booleans to determine which of these
    actions to take. It is ensured that only one can be true at a time
        

    Examples:
    - Given no input a test file will be created and submitted to create data
        $ python createAllLxbatch.py

    - Create data for all combinations of the given parameters
        $ python createAllLxbatch.py -c -s 10 15 25 -f 25.00 -p 0.00 10.00 -a 5.00 10.00 -e 500

    - To fit data, a similar command as above can be used, but with the '-f'
      flag instead of the '-c' flag
        $ python createAllLxbatch.py -c -s 10 15 25 -f 25.00 -p 0.00 10.00 -a 5.00 10.00 -e 500

    Important: The flags can be specified in any order and can accept any 
                number of values, except '-e' which must be a single integer 
                representing the number of events per files to generate.

    Source: https://github.com/cms-eb-upgrade/vfe-toymc/
"""
def main(NEVENTS, NFREQS, NSAMPLES, NPUS, AMPLITUDES, create_data, fit_data):

    main_path   = os.getcwd()
    script_path = os.getcwd() + "/scripts/"
    input_path  = os.getcwd() + "/input/"
    output_path = os.getcwd() + "/outputfit/"

    for path in [script_path, input_path, output_path]:
        if not os.path.exists(path):
            os.makedirs(path)

    os.chdir(script_path)

    # For enumerating the data-creating scripts
    fnum = 0

    for nsample in NSAMPLES:
        for nfreq in NFREQS:
            for npu in NPUS:
                for amplitude in AMPLITUDES:
                    fnum += 1
                    if create_data:
                        script_name = "CreateData" + str(fnum) + ".sh"
                    if fit_data:
                        script_name = "FitData" + str(fnum) + ".sh"
                    script = open(script_name, 'w')
                    script.write(
                    "#!/bin/bash\n" +
                    "cd /afs/cern.ch/user/a/acoda/cmssw/CMSSW_7_4_7/src\n" +
                    "eval `scramv1 runtime -sh`\n" +
                    "cd " + main_path + "\n")
                    if create_data:
                        script.write(
                        "./CreateData.exe 0 " + 
                        str(NEVENTS) + " " + str(nsample) + " " + 
                        str(nfreq) + " " + str(npu) + " " + str(amplitude))
                    if fit_data:
                        print nfreq, nsample, npu, amplitude, NEVENTS
                        script.write(
                        "./Example07.multifit.exe  input/mysample_" +
                        str(NEVENTS) + "_0_" + str(nsample) + "_" +
                        "{0:.2f}".format(nfreq)  + "_" + 
                        "{0:.2f}".format(amplitude) + "_" + 
                        "{0:.2f}".format(npu) + ".root " + 
                        "outputfit/output_" + 
                        str(NEVENTS) + "_0_" + str(nsample) + "_" +  
                        "{0:.2f}".format(nfreq)  + "_" + 
                        "{0:.2f}".format(amplitude) + "_" + 
                        "{0:.2f}".format(npu) + ".root " + 
                        str(nsample) + " " + str(nfreq))

                    os.system("chmod u+x " + script_name)
                    #os.system("bsub -q 1nh " + script_name)

if __name__ == "__main__":

    p = argparse.ArgumentParser()

    p.add_argument('-s', '--nsamples', nargs="+", type=int)
    p.add_argument('-f', '--nfreq', nargs="+", type=float)
    p.add_argument('-p', '--npu', nargs="+", type=float)
    p.add_argument('-a', '--amplitude', nargs="+", type=float)
    p.add_argument('-e', '--nevents', type=int)
    p.add_argument('-c', '--create', action='store_true')
    p.add_argument('-m', '--multifit', action='store_true')

    args = p.parse_args()
    # default test data
    if len(argv) == 1:
        print "No arguments given, making default test script"
        main(20, [25.00], [10], [0.00], [10.00], True, False)
    # creating or fitting test data
    if len(argv) == 2 and args.create != args.multifit:
        if args.create print "Creating test data"
        if args.multifit print "Fitting test data"
        main(20, [25.00], [10], [0.00], [10.00], args.create, args.multifit)
    # warning if both -m and -c flags were passed
    if args.create == args.multifit:
        print "Specify either creating data or fitting it, both cannot be done at once"
    else:
        main(args.nevents, args.nfreq, args.nsamples, args.npu, 
             args.amplitude, args.create, args.multifit)
