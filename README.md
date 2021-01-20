# Multilepton_Analysis
Analysis work using NANOAOD based format.

## Overview of the analysis tools/strategies
This assumes ntuples have been created using a skimmer run on Monte Carlo and data samples in NANOAOD format.
See: https://github.com/michaelmackenzie/ZEMuAnalysis/tree/mmackenz_dev for an example skimmer.

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
the evaluation of these MVAs. The DataPlotter can then make approximate CDF and significicance/limit gain plots for the MVA 
score distributions.

## Directories
### src/interface
Contains the analyzers and tools.

### histograms
Used to plot the histograms from a given book in a HistMaker's output
adding together the processes with their cross section and generation values, primarily
using dataplotter_ztautau.C. This uses the DataPlotter data handler to plot, storing the object as a field of the script

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


## Compiling and loading

Libraries can be loaded by default with the `rootlogon.C` by using `root.exe` or ignored by using `root -l`.
Objects can be recompiled using:
```
make clean
make -j4
```

## Examples
in ROOT:
  
### ZTauTauHistMaker
```c++
.L lib/libCLFVAnalysis.so
TTree* tree = [Get ZTauTau Tree for selection S];
ZTauTauMaker* selec = new ZTauTauHistMaker();
selec->fFolderName = "[selection name, S]";
tree->Process(selec,"");
```

### DataPlotter
```c++
.L lib/libCLFVAnalysis.so
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


 

