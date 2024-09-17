# Process COMBINE FitDiagnostics trees to compare B-only and S+B fit nuisances
import os
import argparse
import ROOT as rt
from array import array

#----------------------------------------------
# Read in the input parameters
#----------------------------------------------

parser = argparse.ArgumentParser()
parser.add_argument("-d", dest="name",default="fitDiagnostics.root", type=str,help="Input FitDiagnostics tree")
parser.add_argument("--tag", dest="tag",default="test", type=str,help="Tag for output file")
parser.add_argument("--max", dest="max",default=-1, type=int,help="Maximum nuisances to process")
parser.add_argument("-v", dest="verbose",default=0, type=int,help="Verbose processing")
args, unknown = parser.parse_known_args()
if len(unknown)>0: 
   print "not found:",unknown,"exiting"
   exit()

#----------------------------------------------
# Retrieve the two fit trees
#----------------------------------------------

f = rt.TFile.Open(args.name, 'READ')
t_s = f.Get('tree_fit_sb')
t_b = f.Get('tree_fit_b')
t_s.GetEntry(0)
t_b.GetEntry(0)

#----------------------------------------------
# Create a list of nuisance parameters
#----------------------------------------------

names = [br.GetName() for br in t_s.GetListOfBranches() if '_In' not in br.GetName() and 'nll' not in br.GetName()]
names.sort()
print "Found %i branches to check" % (len(names))

#----------------------------------------------
# Check each nuisance and write out the results
#----------------------------------------------

f_out = open("nuisance_comparison_%s.log" % (args.tag), "w")
f_out.write("# Nuisance parameter comparison\n")
f_out.write("# Name                                B-only   S+B    Diff\n")
if args.max > 0 and args.max < len(names):
    names = names[:args.max]
for name in names:
    if not hasattr(t_s, name):
        print "Attribute", name, "not found"
        continue
    if not hasattr(t_b, name):
        print "Attribute", name, "not found (tree B-only)"
        continue
    val_s = getattr(t_s, name)
    val_b = getattr(t_b, name)
    if args.verbose:
        print "%-35s %7.3f %7.3f %7.3f" % (name, val_b, val_s, val_b-val_s)
    f_out.write("%-35s %7.3f %7.3f %7.3f\n" % (name, val_b, val_s, val_b-val_s))

f_out.close()
f.Close()
