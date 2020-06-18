# Multilepton_Analysis
Analysis work using the MultileptonAnalyzer as well as the ZTauTauAnalyzer.
See the NWU BLT Analyzers for more information on the creation and format of these trees.

## Overview of the analysis tools/strategies
This assumes ntuples have been created using a BLT based analyzer for Monte Carlo and data samples.
Currently, this workflow assumes all samples are for 2016.

The first step in the workflow is creating sparse trees and sets of histograms for specific analysis selections.
This is done using a *HistMaker* type object. This object is a TSelector that processes each event in the given trees.
Along with the histograms, the HistMaker will store the event count histogram that is used for normalization of 
Monte Carlo samples. The histograms are created with ranges and binnings such that they should only need to be created
once, and all future plots can be made using these generated histograms.

The next step is typically creating plots using these histograms, where the contribution from each distinct process
is of interest. This involves applying normalization and cross-section scales to each individual histogram.
One is also often interested in combing different samples together into a single contribution (such as all top
related backgrounds). This is done using the *DataPlotter* object, which takes in a *DataCard* that gives
the sample histogram file, the normalization, the plotting label, and a few flags such as whether or not the sample
is real data.
This object then will create plots based on the histogram set (which usually differ by different physics selections),
optionally printing them to disk.

One can also use sparse trees created by the HistMaker to train MVAs to separate events of interest from backgrounds.
This is done using the ROOT TMVA package, where the trainings take place in scripts named *TrkQual*. The results
of these trainings can then be passed to the HistMaker to apply to events or passed to a script to add branches with
the evaluation of these MVAs. The DataPlotter can then make approximate CDF and significicance plots for the MVA 
score distributions.

## Directories
### rootScripts
Contains ROOT scripts to process the analyzer produced ntuples

Descriptions:

- MultileptonHistMaker.cc makes books of histograms when running over the multilepton ntuple

- process_multilepton.C processes a list of ntuples through the MultileptonHistMaker

- MultileptonNTupleMaker.cc makes books of trees when running over the multilepton ntuples

- process_multileptonNT.C processes a list of ntuples through the MultileptonNTupleMaker

- make_background.C adds the trees for the various processes from a given book output in the
MultileptonNTupleMaker's output trees, using the cross section and generation values

- ZTauTauHistMaker.cc makes books of histograms when running over the ztautau ntuples

- process_ztautau.C processes a list of ntuples through the ZTauTauHistMaker

- find_cuts.C loops over output from MultileptonHistMaker and identifies a rectangular
cut that has the greatest increase in signal/background for a fixed efficiency drop
in the signal

- Fitter.cc is used to get legendre polynomials for background fits, made to match up
with Multilepton studies

- DataPlotter.cc holds datasets to be added together with their cross sections and plotted
against data. Plots 1D histograms and stacks as well as 2D histograms

### histograms
Used to plot the histograms from a given book in a HistMaker's output
adding together the processes with their cross section and generation values, primarily
using plot_histograms.C, plot_ztautau_histograms.C, and dataplotter_ztautau.C. The last of
these uses the DataPlotter data handler to plot, storing the object as a field of the script

### tmva_training
Used to train MVAs to separate backgrounds from signal

CWoLa training uses variables not correlated with the dilepton mass to separate two mass ranges,
hopefully only succeeding when there is a resonance in one of the masses

ztautau training uses signal/background labels to train the MVAs

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

Examples in examples directory:

`root.exe process_ntuples.C` runs the ZTauTauHistMaker over a few example MC samples

`root.exe plot_histograms.C` loads a DataPlotter with the resulting histograms

In ROOT:
```c++
  .x process_ntuples.C; //create histogram files
  .L plot_histograms.C;
  plot_histograms("[name of histogram]", [set number], "[set type]", [xmin], [xmax], [rebin number]); //returns a canvas
```

General examples:

in ROOT:
### MultileptonHistMaker
```c++
.L MultileptonHistMaker.cc++g; //recompile in debug mode
TTree* tree = [Get MultileptonAnalyzer Tree];
MultileptonHistMaker* selec = new MultileptonHistMaker();
tree->Process(selec,"");
```

### MultileptonNTupleMaker
```c++
.L MultileptonNTupleMaker.cc++g; //recompile in debug mode
TTree* tree = [Get MultileptonAnalyzer Tree];
MultileptonNTupleMaker* selec = new MultileptonNTupleMaker();
tree->Process(selec,"");
  ```
  
### ZTauTauHistMaker
```c++
.L ZTauTauHistMaker.cc++g; //recompile in debug mode
TTree* tree = [Get ZTauTau Tree for selection S];
ZTauTauMaker* selec = new ZTauTauHistMaker();
selec->fFolderName = "[selection name, S]";
tree->Process(selec,"");
```

### DataPlotter
```c++
.L DataPlotter.cc++g; //recompile in debug mode
DataPlotter* d = new DataPlotter();
d->add_dataset("[file path]", "[dataset name when generated e.g. zjets_m-50]", "[Label e.g. Z+Jets]", [0 if MC 1 if Data], [xsec in pb^-1], [true if signal false if background]);
TCanvas* c = d->plot_stack("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number]);
c = d->plot_hist("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number]);
c = d->plot_hist("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number], [xmin], [xmax]);
//print the figures
c = d->print_stack("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number]);
c = d->print_hist("[histogram name e.g. lepm]", "[histogram folder e.g. event]", [histogram set number]);
```

### Train MVA

#### Default version of ROOT (6.06/01 or 6.12/07)

##### Trains MVA specified in TrainTrkQual.C, default is a BDT 
```c++
.L train_tmva.C;
train_tmva("[make_background.C out file]", {[list of process IDs to ignore]}");
```

#### Newer version of ROOT (6.16/00)

##### Trains MVA specified in TrainTrkQualNew.C, default use is Random Forrest using K-Fold validation
```c++
.L train_tmva_new.C;
train_tmva("[make_background.C out file]", {[list of process IDs to ignore]}");
```


 

