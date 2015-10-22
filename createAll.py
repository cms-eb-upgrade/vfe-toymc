import os
import sys

if __name__ == '__main__':

    NTOYS = 100
    NFREQS = [6.25, 12.5, 25]
    #NSAMPLES = [10, 20, 40, 80, 160]
    NSAMPLES = [10, 11, 12, 13, 14, 15, 20, 30, 40, 50, 60, 70, 80, 90]
    NPUS = [0, 20, 40, 140]
    #AMPLITUDES = [10]
    AMPLITUDES = [1, 10, 100]
    
    #NFREQS = [6.25, 12.5, 25]
    #NSAMPLES = [10, 11, 12, 15, 20, 25, 30, 35, 40, 50]
    #NPUS = [0, 1, 10, 20, 40, 50, 140]
    #AMPLITUDES = [1, 2, 5, 10, 20, 50, 100]

    dryrun = 0
    if len(sys.argv) > 1 :
       print " dry run option:",
       dryrun = sys.argv[1]
       print dryrun
 
 
 
    for nsample in NSAMPLES:
      for nfreq in NFREQS:
        for npu in NPUS:
          for amplitude in AMPLITUDES:
            toExec = "./CreateData.exe     0    " + str(NTOYS) + "    " + str(nsample) + "  " + str(nfreq) + "  " + str(npu) + "  " + str(amplitude) 
            print toExec
            if (dryrun == 0) :
              os.system(toExec)
      
      

       