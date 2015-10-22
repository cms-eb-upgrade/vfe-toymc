import os
import sys

if __name__ == '__main__':

    print sys.argv
    
    dryrun = 0
    if len(sys.argv) > 1 :
       print " dry run option:",
       dryrun = sys.argv[1]
       print dryrun
       
       

    ##samples = {}
    ##SamplesFile = sys.argv[1]
    ##print " SamplesFile = ", SamplesFile
    
    #for npul in range(10):
      #toExec = "./Example07.multifit.exe mysample_100_0_" + str(npul+10) + "_25.00_10.00_20.00.root    output_100_0_" + str(npul+10) + "_25.00_10.00_20.00.root   " + str(npul+10) + "   25   "
      #print " ", toExec
      #os.system(toExec)
      
      
    #NTOYS = 100
    #NFREQS = [6.25, 12.5, 25]
    #NSAMPLES = [10, 11, 12, 15, 20, 25, 30, 35, 40, 50]
    #NPUS = [0, 1, 10, 20, 40, 50, 140]
    #AMPLITUDES = [1, 2, 5, 10, 20, 50, 100]

    #NTOYS = 100
    #NFREQS = [6.25, 12.5, 25]
    #NSAMPLES = [10, 20, 40, 80, 160]
    #NPUS = [0]
    ##AMPLITUDES = [10]
    #AMPLITUDES = [10, 100]

    NTOYS = 100
    NFREQS = [6.25, 12.5, 25]
    #NSAMPLES = [10, 20, 40, 80, 160]
    NSAMPLES = [10, 11, 12, 13, 14, 15, 20, 30, 40, 50, 60, 70, 80, 90]
    NPUS = [0, 20, 40, 140]
    #AMPLITUDES = [10]
    AMPLITUDES = [1, 10, 100]
   

    for nsample in NSAMPLES:
      for nfreq in NFREQS:
        for npu in NPUS:
          for amplitude in AMPLITUDES:
            toExec = "./Example07.multifit.exe  input/mysample_" + str(NTOYS) + "_0_" + str(nsample) + "_" +  "{:.2f}".format(nfreq)  + "_" + "{:.2f}".format(amplitude) + "_" + "{:.2f}".format(npu) + ".root   " \
                      + "outputfit/output_" + str(NTOYS) + "_0_" + str(nsample) + "_" +  "{:.2f}".format(nfreq)  + "_" + "{:.2f}".format(amplitude) + "_" + "{:.2f}".format(npu) + ".root   " + str(nsample) + "   " + str(nfreq) 
            print toExec
            if (dryrun == 0) :
              os.system(toExec)
      
  




       

       