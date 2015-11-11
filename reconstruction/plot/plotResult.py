import os
from ROOT import *
import tdrstyle
import copy

# Sets up some of the variables based off of the input file, results.txt
# then passes these to make_file_system where ther real work starts happening
def main():

    tdrstyle.setTDRStyle()
    gROOT.SetBatch(True)

    # Read and split lines from input file
    input_file = open("results.txt", 'r')
    input_list = [l.split() for l in input_file.readlines()[2:]]
    out_file = TFile("out.root", "Recreate")

    dir_name = "Dependent_Sigma"
    out_file.mkdir(dir_name)
    out_file.cd(dir_name)
    root_file_path = [dir_name, "/"]

    # List of parameters to be looped over in creating the graphs
    NSAMPLES = []
    NFREQ = []
    AMPLITUDE = []
    NPU = []
    param_list  = [NSAMPLES, NFREQ, AMPLITUDE, NPU]
    param_dict  = {'NSamples': NSAMPLES, 'NFreq': NFREQ, 
                   'Amplitude': AMPLITUDE, 'NPU': NPU}
    param_names = ['NSamples', 'NFreq', 'Amplitude', 'NPU']

    # Make a dict linking input params to the various sigmas
    input_dict = {}

    # Parse the input lines into the dictionaries
    for line in input_list:
        a = ()
        b = ()
        for i, stat in enumerate(line):
            if i == 2:
                # Round the amplitudes to the nearest integer
                a += (round(float(stat)),)
                if a[i] not in param_list[i]:
                    param_list[i].append(a[i])
            elif (i < len(line) - 5):
                # First four items are inputs, sharply rounded
                a += (round(float(stat), 2),)
                # Add items to the list of parameters
                if a[i] not in param_list[i]:
                    param_list[i].append(a[i])
            else:
                # After that are the errors, lots of digits kept
                b += (round(float(stat), 7),)
        input_dict[a] = b

    # Sort the parameters so the TGraph draws nice lines later on
    for param in param_list:
        param.sort()

    # Pass all this information and start building things
    make_file_system(param_dict, input_dict, param_names, root_file_path, out_file)


# Recursively makes the files system and fills it with graphs
def make_file_system(params, sigmas, names, path, out_file):

    for n in names:

        path += [n]
        out_file.mkdir(flatten_to_string(path))
        path += ["/"]

        new_names = names[:]
        new_names.remove(n)

        # Recur to make more files, else if at the last param, make some graphs
        if len(names) > 1:
            make_file_system(params, sigmas, new_names, path, out_file)
        else:
            values = params[n]
            for v in values:
                print path
                make_graphs(params, sigmas, path, v, out_file)

        path.pop()
        path.pop()

# Constructs multi-paneled multigraphs whose structure is based off
# the path variable, and whose contents is dictated by the params and
# sigma variables
def make_graphs(params, sigmas, path, canvas_var_val, out_file):

    out_file.cd(flatten_to_string(path))

    # Parsing the path to determine how to construct the graph
    x_axis    = path[2]
    lines     = path[4]
    graph_var = path[6]
    canvas_var     = path[8]

    # The canvas to be used 
    canvas = TCanvas("Sigma vs. " + x_axis +
                     " with fixed " + canvas_var + "=" + str(canvas_var_val),
                     canvas_var + "=" + str(canvas_var_val),
                     800, 1000)
    canvas.Divide(2, len(params[graph_var])/2)

    # Clones the graphs made into these lists otherwise there is difficulty when writing it
    graph_list = []
    leg_list = []
    canvas_panel_num = 1

    for panel_var in params[graph_var]:
        # Initialize the graph and legend
        graph  = TMultiGraph()
        legend = TLegend(0.8,0.2,1.0,0.95)
        title  = "#sigma_{E} / E vs. " + x_axis + " where " + \
                 graph_var + "=" + str(panel_var) + " and " + \
                 canvas_var + "=" + str(canvas_var_val)

        legend.SetName(title+"_leg")
        legend.SetBorderSize(1)
        graph.SetName(title)
        graph.SetTitle(title)

        g_lines = []
        line_num = 0
        for line_val in params[lines]:
            # Initialize the line with error bars
            graph_line = TGraphErrors()
            graph_line.SetName(lines + "=" + str(line_val))
            point_num = 0
            for x_val in params[x_axis]:
                # Construct a point
                point = ()
                for name in ['NSamples', 'NFreq', 'Amplitude', 'NPU']:
                    for var in [[graph_var, panel_var], [lines, line_val], 
                                [x_axis, x_val], [canvas_var, canvas_var_val]]:
                        if var[0] == name: point += (float(var[1]),)
                print point
                # The values that sigmas access determines what measure you are using
                # Change these from 3,4 once sigma_eff is updated and in
                if point in sigmas:
                    graph_line.SetPoint(point_num, x_val, sigmas[point][3])
                    graph_line.SetPointError(point_num, 0, sigmas[point][4])
                    point_num += 1

            # Formatting the style and then adding the line to the graph
            graph_line.SetMarkerColor(100 - (line_num*12)%50)
            graph_line.SetLineColor(100 - (line_num*12)%50)
            graph_line.SetMarkerStyle(line_num+20)
            graph_line.SetMarkerSize(2)

            g_lines.append([graph_line.Clone(), lines + "=" + str(line_val)])
            line_num += 1

        # Add the lines to the graph/legend
        for g in g_lines:
            graph.Add(g[0])
            legend.AddEntry(g[0], g[1], "p")

        # Formatting and drawing the graph on the canvas
        canvas.cd(canvas_panel_num)
        canvas_panel_num += 1

        graph_list.append(graph.Clone())
        leg_list.append(legend.Clone())

        index = len(graph_list) - 1
        graph_list[index].Draw("APL")
        graph_list[index].GetXaxis().SetTitle(x_axis)
        graph_list[index].GetYaxis().SetTitle("#sigma_{E} / E")
        leg_list[index].Draw()

    canvas.Write()

# Given a list of strings, will flatten them into a signle string
def flatten_to_string(l):
    return ''.join([item for sublist in l for item in sublist])

if __name__ == "__main__":
    main()
