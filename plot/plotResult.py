import os
import sys
from ROOT import *
import math

import tdrstyle


if __name__ == '__main__':

    #set the tdr style
    tdrstyle.setTDRStyle()
    
    
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
    #print input_list

    input_dict = {}
    for bla in input_list:
      a = ()
      b = ()
      for i in range(0, len(bla)) :
        if i < (len(bla)-5) :
          a = a + ( float( "{:.1f}".format(float(bla[i])) ), )
        else :
          b = b + ( float(bla[i]), )
      input_dict[ a ] = b
      
    #print input_dict
 
    
             
    #NTOYS = 100
    #NFREQS = [6.25, 12.5, 25.0]
    #NSAMPLES = [10, 20, 40, 80, 160]
    #NPUS = [0.0]
    ##AMPLITUDES = [10]
    #AMPLITUDES = [10.0, 100.0]

    #NTOYS = 100
    #NFREQS = [6.2, 12.5, 25.0]   #   6.2 instead of 6.25 only for matching requirements in python code
    ##NFREQS = [6.25, 12.5, 25.0]
    ##NSAMPLES = [10, 20, 40, 80, 160]
    #NSAMPLES = [10, 11, 12, 13, 14, 15, 20, 30, 40, 50, 60, 70, 80, 90]
    #NPUS = [0.0, 20.0, 40.0, 140.0]
    ##AMPLITUDES = [10]
    #AMPLITUDES = [1.0, 10.0, 100.0]

    NTOYS = 1000
    NFREQS = [6.2, 12.5, 25]
    NSAMPLES = [10, 15, 20, 40, 50, 60]
    NPUS = [0.0, 20.0, 40.0, 140.0]
    #AMPLITUDES = [10]
    AMPLITUDES = [1.0, 10.0, 100.0]
    
   
    
    grnum = 0
    leg = TLegend(0.8,0.2,1.0,0.95)
    all_multi_bias = TMultiGraph()
    all_multi_standard_dev = TMultiGraph()

    list_multi_bias_fix_amplitude = []
    list_multi_standard_dev_fix_amplitude = []
    list_multi_fix_amplitude_leg = []
    
    dict_multi_bias_fix_amplitude_fix_pu = {}
    dict_multi_standard_dev_fix_amplitude_fix_pu = {}
    dict_multi_fix_amplitude_leg_fix_pu = {}
    
    for amplitude in AMPLITUDES:
      
      multi_bias_fix_amplitude = TMultiGraph()
      multi_standard_dev_fix_amplitude = TMultiGraph()
      leg_fix_amplitude = TLegend(0.8,0.2,1.0,0.95)

      list_multi_bias_fix_amplitude_fix_pu = []
      list_multi_standard_dev_fix_amplitude_fix_pu = []
      list_multi_fix_amplitude_leg_fix_pu = []

      for npu in NPUS:

        multi_bias_fix_amplitude_fix_pu = TMultiGraph()
        multi_standard_dev_fix_amplitude_fix_pu = TMultiGraph()
        leg_fix_amplitude_fix_pu = TLegend(0.8,0.2,1.0,0.95)
        
        for nfreq in NFREQS:
          graph_bias = TGraphErrors()
          graph_bias.SetName("histo_bias_" +  "{:.2f}".format(nfreq) + "_" + "{:.2f}".format(npu) + "_" + "{:.2f}".format(amplitude))

          graph_standard_dev = TGraphErrors()
          graph_standard_dev.SetName("histo_standard_dev_" +  "{:.2f}".format(nfreq) + "_" + "{:.2f}".format(npu) + "_" + "{:.2f}".format(amplitude))
          point = 0
          for nsample in NSAMPLES:
            print (nsample, nfreq, amplitude, npu)
            if (nsample, nfreq, amplitude, npu) in input_dict.keys() :
              print " - " , point, " ", nsample, " = ", input_dict[ (nsample, nfreq, amplitude, npu) ]
              graph_bias.SetPoint(point, nsample, (input_dict[ (nsample, nfreq, amplitude, npu) ])[0] )
              graph_bias.SetPointError(point, 0, (input_dict[ (nsample, nfreq, amplitude, npu) ])[1] / math.sqrt(NTOYS) )

              graph_standard_dev.SetPoint(point, nsample, (input_dict[ (nsample, nfreq, amplitude, npu) ])[3] / amplitude )
              graph_standard_dev.SetPointError(point, 0, (input_dict[ (nsample, nfreq, amplitude, npu) ])[4] / amplitude )

              point += 1
            
          graph_bias.SetMarkerColor(100 - (grnum*9)%50 )
          graph_bias.SetLineColor(100 - (grnum*9)%50 )
          graph_bias.SetMarkerStyle(grnum+20)
          graph_bias.SetMarkerSize(2)
          all_multi_bias.Add(graph_bias)
          
          graph_standard_dev.SetMarkerColor(100 - (grnum*9)%50 )
          graph_standard_dev.SetLineColor(100 - (grnum*9)%50 )
          graph_standard_dev.SetMarkerStyle(grnum+20)
          graph_standard_dev.SetMarkerSize(2)
          all_multi_standard_dev.Add(graph_standard_dev)
          
          leg.AddEntry(graph_bias, "#Delta Time: " + "{:.2f}".format(nfreq) + " nPU:" + "{:.0f}".format(npu) + " Amplitude" + "{:.0f}".format(amplitude) + " GeV ", "p")      
          leg_fix_amplitude.AddEntry(graph_bias, "#Delta Time: " + "{:.1f}".format(nfreq) + " nPU:" + "{:.0f}".format(npu), "p")

          grnum += 1
          
          multi_bias_fix_amplitude.Add(graph_bias.Clone())
          multi_standard_dev_fix_amplitude.Add(graph_standard_dev.Clone())

          multi_bias_fix_amplitude_fix_pu.Add(graph_bias.Clone())
          multi_standard_dev_fix_amplitude_fix_pu.Add(graph_standard_dev.Clone())
          leg_fix_amplitude_fix_pu.AddEntry(graph_bias, "#Delta Time: " + "{:.1f}".format(nfreq), "p")
    
        multi_bias_fix_amplitude_fix_pu.SetTitle(" Amplitude" + "{:.0f}".format(amplitude) + " GeV " + " nPU:" + "{:.0f}".format(npu) )
        multi_standard_dev_fix_amplitude_fix_pu.SetTitle(" Amplitude" + "{:.0f}".format(amplitude) + " GeV " + " nPU:" + "{:.0f}".format(npu) )
        list_multi_bias_fix_amplitude_fix_pu.append(multi_bias_fix_amplitude_fix_pu)
        list_multi_standard_dev_fix_amplitude_fix_pu.append(multi_standard_dev_fix_amplitude_fix_pu)
        list_multi_fix_amplitude_leg_fix_pu.append(leg_fix_amplitude_fix_pu)

      list_multi_bias_fix_amplitude.append(multi_bias_fix_amplitude)
      list_multi_standard_dev_fix_amplitude.append(multi_standard_dev_fix_amplitude)
      list_multi_fix_amplitude_leg.append(leg_fix_amplitude)

      dict_multi_bias_fix_amplitude_fix_pu[amplitude] = list_multi_bias_fix_amplitude_fix_pu
      dict_multi_standard_dev_fix_amplitude_fix_pu[amplitude] = list_multi_standard_dev_fix_amplitude_fix_pu
      dict_multi_fix_amplitude_leg_fix_pu[amplitude] = list_multi_fix_amplitude_leg_fix_pu
      

      
 
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
      list_multi_standard_dev_fix_amplitude[i].GetYaxis().SetTitle("#sigma_{E} / E")
      list_multi_fix_amplitude_leg[i].Draw()
      cc_standard_dev_fix_amplitude.Write()
    
    for amplitude, list_multigraph in dict_multi_bias_fix_amplitude_fix_pu.iteritems() :
      print "amplitude = ", amplitude
      cc_bias_fix_amplitude_fix_pu = TCanvas("cc_bias_fix_amplitude"+str(amplitude)+"_fix_pu", "Energy = " + str(amplitude) + "GeV" ,800,1000)
      cc_bias_fix_amplitude_fix_pu.Divide(int(math.sqrt(len(list_multigraph))), int(math.sqrt(len(list_multigraph))+1))
      cc_standard_dev_fix_amplitude_fix_pu = TCanvas("cc_standard_dev_fix_amplitude"+str(amplitude)+"_fix_pu", "Energy = " + str(amplitude) + "GeV" ,800,1000)
      cc_standard_dev_fix_amplitude_fix_pu.Divide(int(math.sqrt(len(list_multigraph))), int(math.sqrt(len(list_multigraph))+1))

      for i in range(len(list_multigraph)) :
         #print " i = ", i, " :: ", len(list_multigraph)
         cc_bias_fix_amplitude_fix_pu.cd(i+1)
         list_multigraph[i].Draw("APL");
         list_multigraph[i].GetXaxis().SetTitle("n Samples");
         list_multigraph[i].GetYaxis().SetTitle("(E - E_{true}) / E_{true}")
         #print " len[" , amplitude, " = " , len(dict_multi_fix_amplitude_leg_fix_pu[amplitude])
         dict_multi_fix_amplitude_leg_fix_pu[amplitude][i].Draw()
         
         cc_standard_dev_fix_amplitude_fix_pu.cd(i+1)
         dict_multi_standard_dev_fix_amplitude_fix_pu[amplitude][i].Draw("APL");
         dict_multi_standard_dev_fix_amplitude_fix_pu[amplitude][i].GetXaxis().SetTitle("n Samples");
         dict_multi_standard_dev_fix_amplitude_fix_pu[amplitude][i].GetYaxis().SetTitle("#sigma_{E} / E")
         #print " len[" , amplitude, " = " , len(dict_multi_fix_amplitude_leg_fix_pu[amplitude])
         dict_multi_fix_amplitude_leg_fix_pu[amplitude][i].Draw()
                  
      cc_bias_fix_amplitude_fix_pu.Write()
      cc_standard_dev_fix_amplitude_fix_pu.Write()












    # plots vs PU

    dict_multi_bias_fix_amplitude_vs_pu = {}
    dict_multi_standard_dev_fix_amplitude_vs_pu = {}
    dict_multi_fix_amplitude_leg_vs_pu = {}

    grnum = 0
    
    for amplitude in AMPLITUDES:

      list_multi_bias_fix_amplitude_vs_pu = []
      list_multi_standard_dev_fix_amplitude_vs_pu = []
      list_multi_fix_amplitude_leg_vs_pu = []

      for nsample in NSAMPLES:

        multi_bias_fix_amplitude_vs_pu = TMultiGraph()
        multi_bias_fix_amplitude_vs_pu.SetName("mul_bias_" + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude))
        multi_standard_dev_fix_amplitude_vs_pu = TMultiGraph()
        multi_standard_dev_fix_amplitude_vs_pu.SetName("mul_RMS_" + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude))
        leg_fix_amplitude_vs_pu = TLegend(0.8,0.2,1.0,0.95)
        leg_fix_amplitude_vs_pu.SetName("leg_" + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude))
        
        for nfreq in NFREQS:
          graph_bias_vs_pu = TGraphErrors()
          #graph_bias_vs_pu.SetName("histo_bias_" +  "{:.2f}".format(nfreq) + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude))
          graph_bias_vs_pu.SetTitle("#Delta T: " + "{:.1f}".format(nfreq))
          graph_bias_vs_pu.SetFillColor(kWhite)

          graph_standard_dev_vs_pu = TGraphErrors()
          #graph_standard_dev_vs_pu.SetName("histo_standard_dev_" +  "{:.2f}".format(nfreq) + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude))
          graph_standard_dev_vs_pu.SetTitle("#Delta T: " + "{:.1f}".format(nfreq))
          graph_standard_dev_vs_pu.SetFillColor(kWhite)

          point = 0
          for npu in NPUS:
            print (nsample, nfreq, amplitude, npu)
            if (nsample, nfreq, amplitude, npu) in input_dict.keys() :
              print " - " , point, " ", npu, " = ", input_dict[ (nsample, nfreq, amplitude, npu) ]
              graph_bias_vs_pu.SetPoint(point, npu, (input_dict[ (nsample, nfreq, amplitude, npu) ])[0] )
              graph_bias_vs_pu.SetPointError(point, 0, (input_dict[ (nsample, nfreq, amplitude, npu) ])[1] / math.sqrt(NTOYS) )

              graph_standard_dev_vs_pu.SetPoint(point, npu, (input_dict[ (nsample, nfreq, amplitude, npu) ])[3] / amplitude )
              graph_standard_dev_vs_pu.SetPointError(point, 0, (input_dict[ (nsample, nfreq, amplitude, npu) ])[4] / amplitude )

              point += 1
            
          graph_bias_vs_pu.SetMarkerColor(100 - (grnum*9)%50 )
          graph_bias_vs_pu.SetLineColor(100 - (grnum*9)%50 )
          graph_bias_vs_pu.SetMarkerStyle(grnum+20)
          graph_bias_vs_pu.SetMarkerSize(2)
          
          graph_standard_dev_vs_pu.SetMarkerColor(100 - (grnum*9)%50 )
          graph_standard_dev_vs_pu.SetLineColor(100 - (grnum*9)%50 )
          graph_standard_dev_vs_pu.SetMarkerStyle(grnum+20)
          graph_standard_dev_vs_pu.SetMarkerSize(2)
                    
          #multi_bias_fix_amplitude_vs_pu.Add(graph_bias_vs_pu.Clone("histo_bias_" +  "{:.2f}".format(nfreq) + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude)))
          #multi_standard_dev_fix_amplitude_vs_pu.Add(graph_standard_dev_vs_pu.Clone("histo_standard_dev_" +  "{:.2f}".format(nfreq) + "_ns" + "{:.2f}".format(nsample) + "_" + "{:.2f}".format(amplitude)))
          multi_bias_fix_amplitude_vs_pu.Add(graph_bias_vs_pu.Clone())
          multi_standard_dev_fix_amplitude_vs_pu.Add(graph_standard_dev_vs_pu.Clone())
          leg_fix_amplitude_vs_pu.AddEntry(graph_bias_vs_pu, "#Delta T: " + "{:.2f}".format(nfreq), "p")
    
          grnum += 1

        multi_bias_fix_amplitude_vs_pu.SetTitle(" Amplitude" + "{:.0f}".format(amplitude) + " GeV " + " nSample:" + "{:.0f}".format(nsample) )
        multi_standard_dev_fix_amplitude_vs_pu.SetTitle(" Amplitude" + "{:.0f}".format(amplitude) + " GeV " + " nSample:" + "{:.0f}".format(nsample) )
      
        list_multi_bias_fix_amplitude_vs_pu.append(multi_bias_fix_amplitude_vs_pu)
        list_multi_standard_dev_fix_amplitude_vs_pu.append(multi_standard_dev_fix_amplitude_vs_pu)
        list_multi_fix_amplitude_leg_vs_pu.append(leg_fix_amplitude_vs_pu)

      dict_multi_bias_fix_amplitude_vs_pu[amplitude] = list_multi_bias_fix_amplitude_vs_pu
      dict_multi_standard_dev_fix_amplitude_vs_pu[amplitude] = list_multi_standard_dev_fix_amplitude_vs_pu
      dict_multi_fix_amplitude_leg_vs_pu[amplitude] = list_multi_fix_amplitude_leg_vs_pu







    # plot vs pile-up
    for amplitude, list_multigraph in dict_multi_bias_fix_amplitude_vs_pu.iteritems() :
      print "amplitude = ", amplitude
      cc_bias_fix_amplitude_vs_pu = TCanvas("cc_bias_fix_amplitude"+str(amplitude)+"_vs_pu", "Energy = " + str(amplitude) + "GeV" ,800,1000)
      cc_bias_fix_amplitude_vs_pu.Divide(int(math.sqrt(len(list_multigraph))), int(math.sqrt(len(list_multigraph))+1))
      cc_standard_dev_fix_amplitude_vs_pu = TCanvas("cc_standard_dev_fix_amplitude"+str(amplitude)+"_vs_pu", "Energy = " + str(amplitude) + "GeV" ,800,1000)
      cc_standard_dev_fix_amplitude_vs_pu.Divide(int(math.sqrt(len(list_multigraph))), int(math.sqrt(len(list_multigraph))+1))

      for i in range(len(list_multigraph)) :
         print " i = ", i, " : ", len(list_multigraph), " = ", len(dict_multi_fix_amplitude_leg_vs_pu[amplitude]), " = ", len(dict_multi_standard_dev_fix_amplitude_vs_pu[amplitude])
         cc_bias_fix_amplitude_vs_pu.cd(i+1)
         if list_multigraph[i].GetListOfGraphs().GetSize() != 0 :
           list_multigraph[i].Draw("APL");
           list_multigraph[i].GetXaxis().SetTitle("n PU");
           list_multigraph[i].GetYaxis().SetTitle("(E - E_{true}) / E_{true}")
           gPad.BuildLegend(0.8,0.2,1.0,0.95)
           #dict_multi_fix_amplitude_leg_vs_pu[amplitude][i].Draw()
         
         if dict_multi_standard_dev_fix_amplitude_vs_pu[amplitude][i].GetListOfGraphs().GetSize() != 0 :
           cc_standard_dev_fix_amplitude_vs_pu.cd(i+1)
           dict_multi_standard_dev_fix_amplitude_vs_pu[amplitude][i].Draw("APL");
           dict_multi_standard_dev_fix_amplitude_vs_pu[amplitude][i].GetXaxis().SetTitle("n PU");
           dict_multi_standard_dev_fix_amplitude_vs_pu[amplitude][i].GetYaxis().SetTitle("#sigma_{E} / E")
           gPad.BuildLegend(0.8,0.2,1.0,0.95)
           #dict_multi_fix_amplitude_leg_vs_pu[amplitude][i].Draw()
                  
      cc_bias_fix_amplitude_vs_pu.Write()
      cc_standard_dev_fix_amplitude_vs_pu.Write()

    
    fileout.Close()
    print "done"
   
       

       