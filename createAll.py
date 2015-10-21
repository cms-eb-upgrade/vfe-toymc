import os
import sys

if __name__ == '__main__':

    NTOYS = 100
    NFREQS = [6.25, 12.5, 25]
    NSAMPLES = [10, 11, 12, 15, 20, 25, 30, 35, 40, 50]
    NPUS = [0, 1, 10, 20, 40, 50, 140]
    AMPLITUDES = [1, 2, 5, 10, 20, 50, 100]
    

    for nsample in NSAMPLES:
      for nfreq in NFREQS:
        for npu in NPUS:
          for amplitude in AMPLITUDES:
            toExec = "./CreateData.exe     0    " + str(NTOYS) + "    " + str(nsample) + "  " + str(nfreq) + "  " + str(npu) + "  " + str(amplitude) 
            print " ", toExec
            #os.system(toExec)
      
      

       