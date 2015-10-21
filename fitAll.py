import os
import sys

if __name__ == '__main__':

    print sys.argv
    if len(sys.argv) <= 1 :
       print "no arguments?"
       print "Usage to submit:     python fillAll.py file.py"
       #exit()
       

    #samples = {}
    #SamplesFile = sys.argv[1]
    #print " SamplesFile = ", SamplesFile
    
    for npul in range(10):
      toExec = "./Example07.multifit.exe mysample_100_0_" + str(npul+10) + "_25.00_10.00_20.00.root    output_100_0_" + str(npul+10) + "_25.00_10.00_20.00.root   " + str(npul+10) + "   25   "
      print " ", toExec
      os.system(toExec)
      
      

       

       