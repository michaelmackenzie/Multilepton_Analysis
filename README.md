# Multilepton_Analysis
Analysis work using the MultileptonAnalyzer

## Directories
### rootScripts
Contains root scripts to process the MultileptonAnalyzer ntuples
MultileptonHistMaker.cc makes books of histograms when running over the ntuple
process_multilepton.C processes a list of ntuples through MultileptonHistMaker

MultileptonNTupleMaker.cc makes books of trees when running over the ntuples
process_multileptonNT.C processes a list of ntuples through MultileptonNTupleMaker

make_background.C adds the trees for the various processes from a given book in
MultileptonNTupleMaker's output trees, using the cross section and generation values

find_cuts.C loops over output from MultileptonHistMaker and identifies a rectangular
cut that has the greatest increase in signal/background for a fixed efficiency drop
in the signal

### histograms
Used to plot the histograms from a given book in MultileptonHistMaker's output
adding together the processes with their cross section and generation values, primarily
using plot_histograms.C

### tmva_training
Used to train MVAs to separate backgrounds from signal
CWoLa training uses variables not correlated with the dilepton mass to separate two mass ranges,
hopefully only succeeding when there is a resonance in one of the masses

## Examples

in ROOT:
### HistMaker

.L MultileptonHistMaker.cc++g //recompile in debug mode

TTree* tree = [Get MultileptonAnalyzer Tree]

MultileptonHistMaker* selec = new MultileptonHistMaker()

tree->Process(selec,"")

TFile* out = new TFile("[file name]", "[internal name]", "RECREATE")

out->Write()
  

### NTupleMaker

.L MultileptonNTupleMaker.cc++g //recompile in debug mode

TTree* tree = [Get MultileptonAnalyzer Tree]

MultileptonNTupleMaker* selec = new MultileptonNTupleMaker()

tree->Process(selec,"")

TFile* out = new TFile("[file name]", "[internal name]", "RECREATE")

out->Write()
  
### Train MVA

.L train_tmva.C

train_tmva("[make_background.C out file]", {[list of process IDs to ignore]}")
 

