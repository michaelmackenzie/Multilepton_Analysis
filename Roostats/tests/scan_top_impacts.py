#! /bin/env python
import os
import sys
import json
import argparse
from math import sqrt

def impact_pull(x):
    fit_val  = x['fit']   [1]
    fit_up   = x['fit']   [2]
    fit_down = x['fit']   [0]
    pre_val  = x['prefit'][1]
    pre_up   = x['prefit'][2]
    pre_down = x['prefit'][0]
    min_diff = 1.e-2
    if fit_val > pre_val:
        sigma = sqrt(max(min_diff, (pre_val-pre_down)**2 - (fit_val-fit_down)**2))
    else:
        sigma = sqrt(max(min_diff, (pre_up-pre_val)**2 - (fit_up-fit_val)**2))
    return (fit_val - pre_val) / sigma

# Input arguments
p = argparse.ArgumentParser(description='Run likelihood scans for the top impacts of a given Combine JSON file and workspace')
p.add_argument('-d', '--card', help='Input combine workspace/data card', required=True)
p.add_argument('-j', '--json', help='Input combine impact json file', required=True)
p.add_argument('-t', '--tag', help='Tag', default="", required=False)
p.add_argument('-p', '--points', help='N(points)', default="50", required=False)
p.add_argument('-po', '--plot_only', help='Only make the plot', action='store_true', default=False, required=False)
p.add_argument('-n', '--nscan', help='N(parameters) to scan', default=10, type=int, required=False)
p.add_argument('--dryrun', help='Setup processing without running', action='store_true', required=False)
p.add_argument('--skip_impacts', help='Skip processing top impacts', action='store_true', default = False, required=False)
p.add_argument('--skip_pulls', help='Skip processing largest pulls', action='store_true', default = False, required=False)

args = p.parse_args()

card = args.card
j_file = args.json
tag = args.tag
points = args.points
nscan = args.nscan
plot_only = args.plot_only
dryrun = args.dryrun
skip_impacts = args.skip_impacts
skip_pulls = args.skip_pulls

# Open the impacts file
f = open(j_file)
if not f:
    print "JSON file %s not found" % (j_file)
    exit

# Retrieve the impacts
data = json.load(f)
params = data['params']

# # Print an example impact
# print params[0]

# Make a map of just the impact name and it's impact and pull
impacts = [[x['name'], x['impact_r'], x['fit'], abs(impact_pull(x))] for x in params]

# Sort by impact size and process the top impacts
sorted_list = sorted(impacts, key=lambda x : x[1], reverse=True)
top_impacts = sorted_list[0:min(len(sorted_list), nscan)]
print "Top impacts:"
for param in top_impacts:
    print " %-30s: impact = %7.5f, fit = %6.3f (+%.3f, -%.3f), pull = %.2f" % (param[0], param[1], param[2][1],
                                                                               param[2][2]-param[2][1], param[2][1]-param[2][0],
                                                                               param[3])

fit_args = '--cminDefaultMinimizerStrategy 0'
script = '${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/scan_parameter.sh'
script_flags = '--points %s' % (points)
if tag != "": script_flags = '%s --tag %s' % (script_flags, tag)
if plot_only: script_flags = '%s --plotonly' % (script_flags)
if not skip_impacts:
    for param in top_impacts:
        command = '%s -d %s -p %s --fitarg \"--setParameterRanges %s=%.3f,%.3f %s\" %s' % (script, card, param[0], param[0],
                                                                                            param[2][1] - 2.5*(param[2][1]-param[2][0]),
                                                                                            param[2][1] + 2.5*(param[2][2]-param[2][1]),
                                                                                            fit_args, script_flags)
        if not dryrun:
            os.system(command)
        else:
            print command

# Sort by pull size and process the largest pulls
sorted_list = sorted(impacts, key=lambda x : abs(x[3]), reverse=True)
top_pulls = sorted_list[0:min(len(sorted_list), nscan)]
print "Top pulls:"
for param in top_pulls:
    print " %-30s: impact = %7.5f, fit = %6.3f (+%.3f, -%.3f), pull = %.2f" % (param[0], param[1], param[2][1],
                                                                               param[2][2]-param[2][1], param[2][1]-param[2][0],
                                                                               param[3])
if not skip_pulls:
    print "Processing pulls"
    for param in top_pulls:
        command = '%s -d %s -p %s --fitarg \"--setParameterRanges %s=%.3f,%.3f %s\" %s' % (script, card, param[0], param[0],
                                                                                            param[2][1] - 2.5*(param[2][1]-param[2][0]),
                                                                                            param[2][1] + 2.5*(param[2][2]-param[2][1]),
                                                                                            fit_args, script_flags)
        if not dryrun:
            os.system(command)
        else:
            print command
