import os
import sys
from ROOT import *
import math


if __name__ == '__main__':

    print sys.argv
    
    dryrun = 0
    if len(sys.argv) > 1 :
       print " dry run option:",
       dryrun = sys.argv[1]
       print dryrun

    inputfile = open("results.txt", 'r')
    lines = inputfile.readlines()
    #print lines
    
    input_list = [ l.split() for l in lines]
    print input_list

    #input_dict_bias_gev = {}
    #input_dict_standard_dev_bias = {}
    #input_dict_standard_dev_bias_err = {}
    #input_dict_standard_dev_gev = {}
    #input_dict_standard_dev_gev_err = {}
    input_dict = {}
    for bla in input_list:
      a = ()
      b = ()
      for i in range(0, len(bla)) :
        if i < (len(bla)-5) :
          a = a + ( float(bla[i]), )
        else :
          b = b + ( float(bla[i]), )
      input_dict[ a ] = b
      
    print input_dict
 
    
             
    #NTOYS = 100
    #NFREQS = [6.25, 12.5, 25.0]
    #NSAMPLES = [10, 20, 40, 80, 160]
    #NPUS = [0.0]
    ##AMPLITUDES = [10]
    #AMPLITUDES = [10.0, 100.0]

    NTOYS = 100
    NFREQS = [6.25, 12.5, 25]
    #NSAMPLES = [10, 20, 40, 80, 160]
    NSAMPLES = [10, 11, 12, 13, 14, 15, 20, 30, 40, 50, 60, 70, 80, 90]
    NPUS = [0, 20, 40, 140]
    #AMPLITUDES = [10]
    AMPLITUDES = [1, 10, 100]

   
    
    grnum = 0
    leg = TLegend(0.7,0.2,0.9,0.9)
    all_multi_bias = TMultiGraph()
    all_multi_standard_dev = TMultiGraph()

    list_multi_bias_fix_amplitude = []
    list_multi_standard_dev_fix_amplitude = []
    list_multi_fix_amplitude_leg = []
    
    
    for amplitude in AMPLITUDES:
      
      multi_bias_fix_amplitude = TMultiGraph()
      multi_standard_dev_fix_amplitude = TMultiGraph()
      leg_fix_amplitude = TLegend(0.7,0.2,0.9,0.9)

      for npu in NPUS:
        for nfreq in NFREQS:
          graph_bias = TGraphErrors()
          graph_bias.SetName("histo_bias_" +  "{:.2f}".format(nfreq) + "_" + "{:.2f}".format(npu) + "_" + "{:.2f}".format(amplitude))

          graph_standard_dev = TGraphErrors()
          graph_standard_dev.SetName("histo_standard_dev_" +  "{:.2f}".format(nfreq) + "_" + "{:.2f}".format(npu) + "_" + "{:.2f}".format(amplitude))
          point = 0
          for nsample in NSAMPLES:
            #print (nsample, nfreq, amplitude, npu)
            if (nsample, nfreq, amplitude, npu) in input_dict.keys() :
              print " - " , point, " ", nsample, " = ", input_dict[ (nsample, nfreq, amplitude, npu) ]
              graph_bias.SetPoint(point, nsample, (input_dict[ (nsample, nfreq, amplitude, npu) ])[0] )
              graph_bias.SetPointError(point, 0, (input_dict[ (nsample, nfreq, amplitude, npu) ])[1] / math.sqrt(NTOYS) )

              graph_standard_dev.SetPoint(point, nsample, (input_dict[ (nsample, nfreq, amplitude, npu) ])[3] )
              graph_standard_dev.SetPointError(point, 0, (input_dict[ (nsample, nfreq, amplitude, npu) ])[4] )

              point += 1
            
          graph_bias.SetMarkerColor(grnum+1)
          graph_bias.SetLineColor(grnum+1)
          graph_bias.SetMarkerStyle(grnum+20)
          graph_bias.SetMarkerSize(2)
          all_multi_bias.Add(graph_bias)
          
          graph_standard_dev.SetMarkerColor(grnum+1)
          graph_standard_dev.SetLineColor(grnum+1)
          graph_standard_dev.SetMarkerStyle(grnum+20)
          graph_standard_dev.SetMarkerSize(2)
          all_multi_standard_dev.Add(graph_standard_dev)
          
          leg.AddEntry(graph_bias, "Time step: " + "{:.2f}".format(nfreq) + " nPU:" + "{:.0f}".format(npu) + " Amplitude" + "{:.0f}".format(amplitude) + " GeV ", "p")      
          leg_fix_amplitude.AddEntry(graph_bias, "Time step: " + "{:.2f}".format(nfreq) + " nPU:" + "{:.0f}".format(npu) + " Amplitude" + "{:.0f}".format(amplitude) + " GeV ", "p")

          grnum += 1
          
          multi_bias_fix_amplitude.Add(graph_bias.Clone())
          multi_standard_dev_fix_amplitude.Add(graph_standard_dev.Clone())
      
      list_multi_bias_fix_amplitude.append(multi_bias_fix_amplitude)
      list_multi_standard_dev_fix_amplitude.append(multi_standard_dev_fix_amplitude)
      list_multi_fix_amplitude_leg.append(leg_fix_amplitude)
      
    fileout = TFile("out.root","RECREATE")

    cc_bias = TCanvas("cc_bias","",800,600)
    all_multi_bias.Draw("APL")
    all_multi_bias.GetXaxis().SetTitle("n Samples")
    all_multi_bias.GetYaxis().SetTitle("(E - E_{true}) / E_{true}")
    leg.Draw()
    cc_bias.SaveAs("cc_bias.png")
 
    cc_standard_dev = TCanvas("cc_standard_dev","",800,600)
    all_multi_standard_dev.Draw("APL")
    all_multi_standard_dev.GetXaxis().SetTitle("n Samples")
    all_multi_standard_dev.GetYaxis().SetTitle("#sigma_{E} / E")
    leg.Draw()
    cc_standard_dev.SaveAs("cc_standard_dev.png")
    
    cc_bias.Write()
    cc_standard_dev.Write()
    
    
    for i in range(len(list_multi_bias_fix_amplitude)) :
      cc_bias_fix_amplitude = TCanvas("cc_bias_fix_amplitude"+str(AMPLITUDES[i]), "Energy = " + str(AMPLITUDES[i]) + "GeV" ,800,600)
      list_multi_bias_fix_amplitude[i].Draw("APL")
      list_multi_bias_fix_amplitude[i].GetXaxis().SetTitle("n Samples")
      list_multi_bias_fix_amplitude[i].GetYaxis().SetTitle("(E - E_{true}) / E_{true}")
      list_multi_fix_amplitude_leg[i].Draw()
      cc_bias_fix_amplitude.Write()

      cc_standard_dev_fix_amplitude = TCanvas("cc_standard_dev_fix_amplitude"+str(AMPLITUDES[i]), "Energy = " + str(AMPLITUDES[i]) + "GeV" ,800,600)
      list_multi_standard_dev_fix_amplitude[i].Draw("APL")
      list_multi_standard_dev_fix_amplitude[i].GetXaxis().SetTitle("n Samples")
      list_multi_standard_dev_fix_amplitude[i].GetYaxis().SetTitle("(E - E_{true}) / E_{true}")
      list_multi_fix_amplitude_leg[i].Draw()
      cc_standard_dev_fix_amplitude.Write()
    
    
    fileout.Close()
    print "done"
   
       

       