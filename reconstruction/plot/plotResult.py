from scipy import array # For filling a TH2F with a vector
from copy import deepcopy
from math import ceil
from ROOT import *
import tdrstyle

# Sets up some of the variables based off of the input file, results.txt then
# passes these to dependent_sigma_files where ther real work starts happening
def main():

    tdrstyle.setTDRStyle()
    gROOT.SetBatch(True)
    gStyle.SetPalette(1)

    # Read and split lines from input file
    input_file = open("results.txt", 'r')
    input_lines = input_file.readlines()
    input_list = [l.split() for l in input_lines[2:]]

    # Generate a dictionary and name list of the parameters from the
    # input file
    param_names = input_lines[0].split()
    param_names.remove("sigma_eff")
    param_names.remove("sigma_eff_err")
    param_dict = {}
    param_list = [] # Just to help fill the dict
    for param in param_names:
        a = []
        param_list.append(a)
        param_dict[param] = a

    # Make a dictionary linking input params to the various sigmas
    sigma_dict = {}

    # Parsing the parameter and sigma values from the input file into
    # the sigma and param dictionaries
    for line in input_list:
        key = ()
        sigma = ()
        for i, stat in enumerate(line):
            #print line
            # Round and add the values to the two placeholder tuples
            if (i < len(line) - 2):
                key += (round(float(stat), 3),)
                if key[i] not in param_list[i]:
                    param_list[i].append(key[i])
                #print stat, i
            else:
                # Don't round the errors much
                sigma += (round(float(stat), 7),)
        sigma_dict[key] = sigma

    # Sort the parameters so the TGraph draws nice lines later on
    for param in param_list:
        param.sort()
    # Create a new list of names for making the main graphs
    new_names = param_names[:]
    [new_names.remove(n) for n in ["NSAMPLE", "NFREQ"] if n in new_names]

    # The file for everything to be written to
    out_file = TFile("out.root", "Recreate")

    # Build all the main graphs for standard sampling schemes
    for key in param_dict:
        print key, ":", param_dict[key]
    dir_name = "Main_Sampling_Graphs"
    out_file.mkdir(dir_name)
    path = [dir_name, "/"]
    main_sampling_files(new_names, path, param_dict, param_names, sigma_dict, out_file)

    # Build all the dependent sigma graphs
    dir_name = "Dependent_Sigma"
    out_file.mkdir(dir_name)
    path = [dir_name, "/"]
    dependent_sigma_files(param_names, path, param_dict, param_names, 
                           sigma_dict, out_file)

    out_file.Close()

''' Making the files where all variables vary between graphs/canvases '''

# Recursively makes the files system and fills it with graphs
# Note: only the names and path inputs are used to generate the files,
# the others are just passed along to the make_graphs function
def dependent_sigma_files(names, path, params, param_names, sigmas, out_file):

    for n in names:

        path += [n]
        out_file.mkdir(flatten_to_string(path))
        path += ["/"]

        new_names = names[:]
        new_names.remove(n)

        # Recur to make more files, else if at the last param, make some graphs
        if len(names) > len(param_names) - 3:
            dependent_sigma_files(new_names, path, 
                                    params, param_names, sigmas, out_file)
        elif len(names) > 1:
            values = params[n]
            for v in values:
                path += [str(v)]
                path += ["/"]
                dependent_sigma_files(new_names, path, 
                                        params, param_names, sigmas, out_file)
                path.pop()
                path.pop()
        else:
            values = params[n]
            for v in values:
                print path
                make_graphs(path, v, params, param_names, sigmas, out_file)

        path.pop()
        path.pop()
    return

# Constructs multi-paneled multigraphs whose structure is based off
# the path variable, and whose contents is dictated by the params and
# sigma variables
def make_graphs(path, canvas_var_val, params, names, sigmas, out_file):

    out_file.cd(flatten_to_string(path))

    # Parsing the path to determine how to construct the graph
    x_axis    = path[2]
    lines     = path[4]
    graph_var = path[6]
    canvas_var = path[len(path) -2]

    # If there are more than 4 variables used, parses the fixed vars in the path
    fixed_vars = determine_fixed_vars(8, path)

    # The canvas to be used 
    canvas = TCanvas("Sigma vs. " + x_axis +
                     " with fixed " + canvas_var + "=" + str(canvas_var_val),
                     canvas_var + "=" + str(canvas_var_val) + fixed_vars[1], 
                     800, 1000)
    canvas_2D = TCanvas("2D: Sigma vs. " + x_axis + " and " + lines +
                        " with fixed " + canvas_var + "=" + str(canvas_var_val),
                        "2D_" + canvas_var + "=" + str(canvas_var_val) + 
                        fixed_vars[1], 800, 1000)
    panels = len(params[graph_var])
    canvas.Divide(min(2, panels), panels/2)
    canvas_2D.Divide(min(2, panels), panels/2)
                        
    # Clones the graphs made into these lists otherwise there is difficulty 
    # when writing it
    graph_list = []
    hist_list_2D = []
    leg_list = []
    canvas_panel_num = 1
    max_2D = [0, 0]

    for panel_var in params[graph_var]:

        hist_vals_2D = [] 
        g_lines = []
        line_num = 0

        # First we iterate through all combos to make points/lines to fill
        for line_val in params[lines]:
            # Initialize the line with error bars
            graph_line = TGraphErrors()
            graph_line.SetName(lines + "=" + str(line_val))
            graph_line.SetMarkerColor(100 - (line_num*12)%50)
            graph_line.SetLineColor(100 - (line_num*12)%50)
            graph_line.SetMarkerStyle(line_num+20)
            graph_line.SetMarkerSize(2)

            point_num = 0

            for x_val in params[x_axis]:
                # Construct a point
                point = make_point(names, fixed_vars[0],
                                    [[graph_var, panel_var],
                                    [lines, line_val],
                                    [x_axis, x_val], 
                                    [canvas_var, canvas_var_val]])
                if point in sigmas:
                    print point
                    graph_line.SetPoint(point_num, x_val, sigmas[point][0])
                    graph_line.SetPointError(point_num, 0, sigmas[point][1])
                    hist_vals_2D.append([x_val, line_val, sigmas[point][0]])
                    if x_val    > max_2D[0]: max_2D[0] = x_val
                    if line_val > max_2D[1]: max_2D[1] = line_val
                    point_num += 1

            g_lines.append([deepcopy(graph_line), lines + "=" + str(line_val)])
            line_num += 1

        x_bins = [0.0]
        y_bins = [0.0]
        for p in hist_vals_2D:
            x_bins.append(p[0])
            y_bins.append(p[1])
        x_bins = array(remove_duplicates(x_bins))
        y_bins = array(remove_duplicates(y_bins))
        nx_bins = max(len(x_bins) - 1, 1)
        ny_bins = max(len(y_bins) - 1, 1)
        print " X:", x_bins, "Y:", y_bins

        # Initialize the graph and legend
        title  = "#sigma_{Eff} vs. " + x_axis + " where " + \
                 graph_var + "=" + str(panel_var) + " and " + \
                 canvas_var + "=" + str(canvas_var_val) + fixed_vars[1]
        title_2D = "#sigma_{Eff} vs. " + x_axis + " and " + lines + \
                    graph_var + "=" + str(panel_var) + " and " + \
                    canvas_var + "=" + str(canvas_var_val) + fixed_vars[1]

        graph  = TMultiGraph()
        hist_2D = TH2F(title_2D, title_2D, nx_bins, x_bins, ny_bins, y_bins)
        legend = TLegend(0.8,0.2,1.0,0.95)

        graph.SetName(title)
        graph.SetTitle(title)
        hist_2D.SetName(title_2D)
        hist_2D.SetTitle(title_2D)
        legend.SetName(title + "_leg")
        legend.SetBorderSize(1)

        # Add the lines to the graph/legend
        for g in g_lines:
            graph.Add(g[0])
            legend.AddEntry(g[0], g[1], "p")
        for i, p in enumerate(hist_vals_2D):
            hist_2D.Fill(i+1, i+1, p[2])

        # Formatting and drawing the graphs on the canvas
        graph_list.append(deepcopy(graph))
        hist_list_2D.append(deepcopy(hist_2D))
        leg_list.append(deepcopy(legend))
        index = len(graph_list) - 1

        canvas_2D.cd(canvas_panel_num)
        hist_list_2D[index].Draw("COLZ")
        hist_list_2D[index].GetXaxis().SetTitle(x_axis)
        #hist_list_2D[index].GetXaxis().SetRangeUser(0, int(ceil(max_2D[0]*1.1)))
        hist_list_2D[index].GetYaxis().SetTitle(lines)
        #hist_list_2D[index].GetYaxis().SetRangeUser(0, int(ceil(max_2D[1]*1.1)))
        hist_list_2D[index].GetZaxis().SetTitle("#sigma_{Eff}")
        canvas_2D.Update()

        canvas.cd(canvas_panel_num)
        graph_list[index].Draw("APL")
        graph_list[index].GetXaxis().SetTitle(x_axis)
        graph_list[index].GetYaxis().SetTitle("#sigma_{Eff}")
        leg_list[index].Draw()

        canvas_panel_num += 1

    canvas_2D.Write()
    canvas.Write()
    return

''' Making the files and graphs for the main sampling situations '''

# Recursively makes the files system and fills it with graphs
# This makes graphs where sigma depends on some variable, and the lines
# represent the three main samplings: (10, 25.), (20, 12.5), (40, 6.25)
def main_sampling_files(names, path, params, param_names, sigmas, out_file):
    
    for n in names:
        path += [n]
        out_file.mkdir(flatten_to_string(path))
        path += ["/"]

        new_names = names[:]
        new_names.remove(n)
        # Recur to make more files, else if at the last param, make some graphs
        if len(names) > len(param_names) - 4:
            main_sampling_files(new_names, path, 
                                    params, param_names, sigmas, out_file)
        elif len(names) > 1:
            values = params[n]
            for v in values:
                path += [str(v)]
                path += ["/"]
                main_sampling_files(new_names, path, 
                                        params, param_names, sigmas, out_file)
                path.pop()
                path.pop()
        else:
            values = params[n]
            for v in values:
                make_main_graphs(path, v, params, param_names, sigmas, out_file)

        path.pop()
        path.pop()
    return

def make_main_graphs(path, canvas_var_val, params, names, sigmas, out_file):

    out_file.cd(flatten_to_string(path))

    x_axis    = path[2]
    graph_var = path[4]
    lines = [[10.0, 25.0], [20.0, 12.5], [40., 6.25]]
    canvas_var = path[len(path) -2]
    print path
    fixed_vars = determine_fixed_vars(6, path)
    
    # The canvas to be used 
    canvas = TCanvas("Sigma vs. " + x_axis +
                     " with fixed " + canvas_var + "=" + str(canvas_var_val),
                     canvas_var + "=" + str(canvas_var_val) + fixed_vars[1], 
                     800, 1000)
    panels = len(params[graph_var])
    canvas.Divide(min(2, panels), panels/2)
                        
    # Clones the graphs made into these lists otherwise there is difficulty 
    # when writing it
    graph_list = []
    leg_list = []
    canvas_panel_num = 1

    for panel_var in params[graph_var]:

        g_lines = []
        line_num = 0

        # First we iterate through all combos to make points/lines to fill
        for line_val in lines:
            # Initialize the line with error bars
            graph_line = TGraphErrors()
            graph_line.SetName("NSAMPLE = " + str(line_val[0]) + 
                               " NFREQ = " + str(line_val[1]))
            graph_line.SetMarkerColor(100 - (line_num*12)%50)
            graph_line.SetLineColor(100 - (line_num*12)%50)
            graph_line.SetMarkerStyle(line_num+20)
            graph_line.SetMarkerSize(2)

            point_num = 0

            for x_val in params[x_axis]:
                # Construct a point
                point = make_point(names, fixed_vars[0],
                                    [[graph_var, panel_var],
                                    ["NSAMPLE", line_val[0]],
                                    ["NFREQ", line_val[1]],
                                    [x_axis, x_val], 
                                    [canvas_var, canvas_var_val]])
                # The values that sigmas access determines what measure you are 
                # using. Change these from 3,4 once sigma_eff is updated and in
                if point in sigmas:
                    print point
                    graph_line.SetPoint(point_num, x_val, sigmas[point][0])
                    graph_line.SetPointError(point_num, 0, sigmas[point][1])
                    point_num += 1

            g_lines.append([deepcopy(graph_line), 
                            "NSAMPLE = " + str(line_val[0]) + 
                            " NFREQ = " + str(line_val[1])])
            line_num += 1

        # Initialize the graph and legend
        title  = "#sigma_{Eff} vs. " + x_axis + " where " + \
                 graph_var + "=" + str(panel_var) + " and " + \
                 canvas_var + "=" + str(canvas_var_val) + fixed_vars[1]

        graph  = TMultiGraph()
        #legend = TLegend(0.2,0.75,0.6,0.9)
        legend = TLegend(0.4,0.8,0.8,0.95)

        graph.SetName(title)
        graph.SetTitle(title)
        legend.SetName(title + "_leg")
        legend.SetBorderSize(1)

        # Add the lines to the graph/legend
        for g in g_lines:
            graph.Add(g[0])
            legend.AddEntry(g[0], g[1], "p")

        # Formatting and drawing the graphs on the canvas
        graph_list.append(deepcopy(graph))
        leg_list.append(deepcopy(legend))
        index = len(graph_list) - 1

        canvas.cd(canvas_panel_num)
        graph_list[index].Draw("APL")
        graph_list[index].GetXaxis().SetTitle(x_axis)
        graph_list[index].GetYaxis().SetTitle("#sigma_{Eff}")
        #leg_list[index].Draw()

        canvas_panel_num += 1

    canvas.Write()
    return

# Makes a point based on the given parameters. Note that a 'point' here is an
# ordered tuple of all the parameters. It corresponds to a key in the sigma 
# dict, and thus can be used to retrieve the relevant sigma valye
def make_point(names, fixed_vars, var_list):
    point = ()
    for name in names:
        if len(fixed_vars) > 0:
            for i in range(0, len(fixed_vars), 2):
                whole_list = var_list[:]
                whole_list.append([fixed_vars[i], fixed_vars[i+1]])
                for var in whole_list:
                    if var[0] == name: point += (float(var[1]),)
        else:
            for var in var_list:
                if var[0] == name: point += (float(var[1]),)
    return point


# Given the path to a graph, determines the fixed variables for each canvas
def determine_fixed_vars(min_val, path):
    fixed_vars = []
    fixed_vars_str = ""
    if len(path) > min_val + 2:
        for i in range(min_val, len(path) - 2, 4):
            var = path[i]
            val = path[i+2]
            fixed_vars += [var, val]
            fixed_vars_str += " and " +str(var) + "=" + str(val)
    return (fixed_vars, fixed_vars_str)
        
# Given a list of strings, will flatten them into a signle string
def flatten_to_string(l):
    return ''.join([item for sublist in l for item in sublist])

# Removes duplicates from a given list and sorts it
def remove_duplicates(l):
    return sorted(list(set(l)))

if __name__ == "__main__":
    main()
