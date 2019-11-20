# Multilepton_Analysis
Analysis work using the MultileptonAnalyzer as well as the ZTauTauAnalyzer

## Directories
### rootScripts
Contains root scripts to process the analyzer ntuples

MultileptonHistMaker.cc makes books of histograms when running over the multilepton ntuple

process_multilepton.C processes a list of ntuples through MultileptonHistMaker

MultileptonNTupleMaker.cc makes books of trees when running over the multilepton ntuples

process_multileptonNT.C processes a list of ntuples through MultileptonNTupleMaker

make_background.C adds the trees for the various processes from a given book in

MultileptonNTupleMaker's output trees, using the cross section and generation values

ZTauTauHistMaker.cc makes books of histograms when running over the ztautau ntuple

process_ztautau.C processes a list of ntuples through ZTauTauHistMaker

find_cuts.C loops over output from MultileptonHistMaker and identifies a rectangular
cut that has the greatest increase in signal/background for a fixed efficiency drop
in the signal

Fitter.cc is used to get legendre polynomials for background fits, made to match up
with Multilepton studies

DataPlotter.cc holds datasets to be added together with their cross sections and plotted
against data. Plots 1D histograms and stacks as well as 2D histograms

### histograms
Used to plot the histograms from a given book in MultileptonHistMaker's output
adding together the processes with their cross section and generation values, primarily
using plot_histograms.C, plot_ztautau_histograms.C, and dataplotter_ztautau.C. The last of
these uses the DataPlotter data handler to plot, storing the object as a field of the script

### tmva_training
Used to train MVAs to separate backgrounds from signal

CWoLa training uses variables not correlated with the dilepton mass to separate two mass ranges,
hopefully only succeeding when there is a resonance in one of the masses

#### MVA scripts/
##### make_mock_data.C

Makes a root file of events chosen at random from the given file, weighted by event weights.

Arguments: "[background file path]", [integrated luminosity], [seed], [a category to ignore],
[initial luminosity background generated using]

##### plot_tmva_tree.C

Script to plot the results of a CWoLa training.

##### fit_tmva_tree.C

Script to fit a signal + background vs background only hypothesis to a CWoLa trained tree vs MVA score.


## Examples

in ROOT:
### MultileptonHistMaker
```
.L MultileptonHistMaker.cc++g //recompile in debug mode
TTree* tree = [Get MultileptonAnalyzer Tree]
MultileptonHistMaker* selec = new MultileptonHistMaker()
tree->Process(selec,"")
```

### MultileptonNTupleMaker
```
.L MultileptonNTupleMaker.cc++g //recompile in debug mode
TTree* tree = [Get MultileptonAnalyzer Tree]
MultileptonNTupleMaker* selec = new MultileptonNTupleMaker()
tree->Process(selec,"")
  ```
  
### ZTauTauHistMaker
```
.L ZTauTauHistMaker.cc++g //recompile in debug mode
TTree* tree = [Get ZTauTau Tree for selection S]
ZTauTauMaker* selec = new ZTauTauHistMaker()
selec->fFolderName = "[selection name, S]"
tree->Process(selec,"")
```

### DataPlotter
```
.L DataPlotter.cc++g //recompile in debug mode
DataPlotter* d = new DataPlotter()
d->add_dataset("[file path]", "[dataset name when generated e.g. zjets_m-50]", "[Label e.g. Z+Jets]", [0 if MC 1 if Data], [xsec in pb^-1], [true if signal false if background])
TCanvas* c = d->plot_stack("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number])
c = d->plot_hist("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number])
TCanvas* c = d->print_stack("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number])
c = d->plot_hist("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number])
```

### Train MVA

#### Default version of ROOT (6.06/01 or 6.12/07)

##### Trains MVA specified in TrainTrkQual.C, default use is Random Forrest BDT
```
.L train_tmva.C
train_tmva("[make_background.C out file]", {[list of process IDs to ignore]}")
```

#### Newer version of ROOT (6.16/00)

##### Trains MVA specified in TrainTrkQualNew.C, default use is Random Forrest BDT using K-Fold validation
```
.L train_tmva_new.C
train_tmva("[make_background.C out file]", {[list of process IDs to ignore]}")
```


 

