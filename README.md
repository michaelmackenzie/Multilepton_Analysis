# Multilepton_Analysis
Analysis work for the CLFV Z decay search using NANOAOD based datasets, processed using the Z_LFV_Analysis tools.

## Overview of the analysis tools/strategies
This assumes ntuples have been created using a skimmer run on Monte Carlo, Embedding, and data samples in the NANOAOD format.
See: https://github.com/michaelmackenzie/Z_LFV_Analysis/ for these tools

This repository is used to create sparse trees and sets of histograms for specific analysis selections, as well as perform
studies, fits, etc. on these results.
This is done using a *HistMaker* type object. This object is a TSelector that processes each event in the given trees.
Along with the histograms, the HistMaker will store the event count histogram that is used for normalization of
Monte Carlo samples. The histograms are generally created with ranges and binnings such that they should only need to be created
once, and all future plots can be made using these generated histograms.

Creating plots using these histograms is done using the *DataPlotter* object, which takes in a *DataCard* that gives
the sample histogram file, the normalization, the plotting label, and a few flags such as whether or not the sample
is data.
This object then will create plots based on the histogram set (which usually differ by different physics selections),
optionally printing them to disk.


## Overall workflow

The analysis can be broken down into the following stages:

- Skimming

- Histogramming and kinematic selections

- Plotting

- Scale factor measurements

- BDT training and evaluation

- Signal search

## Cloning and building the tools

Setup the CMSSW release:
```
cd ~/nobackup
mkdir CLFV
cd CLFV
cmsrel CMSSW_10_6_29
# optionally: CMSSW_11_3_4
cd CMSSW_10_6_29/src
cmsenv
```

Setup NANOAOD dataset skimming repository:
```
git clone https://github.com/michaelmackenzie/Z_LFV_Analysis.git PhysicsTools #or use https://github.com/ --> git@github.com:
scram b -j4
```

TauPOG resources:
```
git clone https://github.com/cms-tau-pog/TauIDSFs TauPOG/TauIDSFs
scram b -j4
```

Higgs Combine tools:
```
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
git checkout v8.1.0
# for use with CMSSW_11_3_4:
# git checkout v9.1.0
cd -
#Get CombineTool
bash <(curl -s https://raw.githubusercontent.com/cms-analysis/CombineHarvester/master/CombineTools/scripts/sparse-checkout-ssh.sh)
# for use with CMSSW_11_3_4:
# git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester
# cd CombineHarvester
# git checkout v2.0.0
# cd -
scram b -j4
```

Remainder of the analysis:
```
git clone https://github.com/michaelmackenzie/Multilepton_Analysis.git CLFVAnalysis #no second level to avoid CMSSW scram compiling
cd CLFVAnalysis
make -j4
```

Add a patch to improve fits in sidebands using Bernstein polynomials
```
patch ${CMSSW_BASE}/src/HiggsAnalysis/CombinedLimit/interface/RooBernsteinFast.h ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/RooBernsteinFast.h.patch
```

## Skimming

Dataset skimming is done using a separate repository, based on the NANOAOD Tools package:
`https://github.com/michaelmackenzie/Z_LFV_Analysis`, under `src/PhyscisTools/NanoAODTools/`


The main dataset skimming is done by the `python/analyzers/LFVAnalyzer.py`

LFVAnalyzer takes several command line arguments:
`python python/analyzers/LFVAnalyzer.py [comma separated input file list] [data for Data, MC for MC, Embedded for Embedding sampled] [year]`

An example for local testing:
```
cd ${CMSSW_BASE}/src/PhysicsTools/NanoAODTools/
DATASET="/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16NanoAODv7-PUMoriond17_Nano02Apr2020_102X_mcRun2_asymptotic_v8_ext2-v1/NANOAODSIM"
voms-proxy-init --rfc --voms cms --hours 36
xrdcp -f "root://cmseos.fnal.gov/"`das_client -query="file dataset=${DATASET}" | head -n 1` ./NANO.root
python python/analyzers/LFVAnalyzer.py ./NANO.root MC 2016
ls -lh tree.root #results
root.exe -q -b "condor/split_output_tree.C(\"tree.root\", \"tree-split.root\")" #split output ntuple into 5 ntuples
root.exe -q -b "condor/add_norm.C(\"NANO.root\", \"tree-split.root\")" #add event count info without trigger cuts for normalization
```

Grid jobs are submitted using LPC condor in the condor/submitBatch_Legacy.py, which uses the python/condor/BatchMaster.py object.
The datasets are configured in a `samplesDict` dictionary and the `samplesToSubmit` list indicates which to submit.

An example for grid submission:
```
voms-proxy-init --rfc --voms cms --hours 36
cd condor
./submitBatch_Legacy.py
./query_grid.sh [-s for just an overall summary] #monitor jobs
```

When the jobs are finished, failed jobs can be investigated and resubmitted by:
```
./check_batch_job.sh [path to job] [-h for options]
#e.g.
./check_batch_job.sh batch/LFVAnalyzer_20220516_125456/ [--ignorerunning] [--resubmit]
```

Merging output datasets:

Datasets are read over XROOTD, merged locally, and then copied back to eos:
```
cd condor
python prepare_batch.py [output directory, e.g. "nano_batchout/LFVAnalyzer_20220516_125456/"] [-h for options]
```

After this, merged output files should be in `/eos/uscms/store/user/${USER}/lfvanalysis_rootfiles/`

## Histogramming and kinematic selections

The histogramming stage involves reading the ntuples from the skimming stage for a given final
state (e.g. mu+tau), applying corrections, transfer factors, etc., and filling different histogram
"sets" with the event variables. This is done by the `src/CLFVHistMaker.cc` object typically.

A histogram set corresponds to a certain selection (e.g. no b-jets), where each set has a base number (e.g. 8), an offset corresponding to
the final state (e.g. 0 for mu+tau, 200 for e+mu), and then an offset of 1000 if the event is a same-sign event,
and an offset of 2000 if the event is a loose ID lepton event. For example, a standard Z->e+mu signal event for base
set 8 would likely have an absolute set number of 208, and a loose ID (+2000), same-sign (+1000) event would have an absoluted set
number of 3008. This allows estimates of data-driven backgrounds in the selection corresponding to set number 8 by
using identical control regions except with the same-sign or loose lepton ID requirement. Histogram filling is done by the
`FillAllHistograms([set])` function.

The histogramming object inherits from the base `HistMaker` object, `interface/HistMaker.hh`. This
describes the general histogram maker, the typical event weight initialization, histogram definitions, etc., and
then a specific histogramming object can inherit from this and only implement the desired changes (e.g. the
`SparseHistMaker` redefines the histograms made to be only a small set of important distributions). This can also be
used to implement specific measurements, e.g. `JTTHistMaker` creates histograms for the j-->tau transfer factor
measurements, used by `studies/jet_tau_scale/`.

Histogramming is performed using the script `rootScripts/process_clfv.C` where the datasets, final
states ntuples of interest, and `HistMaker` are selected using the `rootScripts/histogramming_config.C` script. The `process_clfv.C` can submit
a number of new jobs calling `process_card.C` to histogram datasets in parallel, also ensuring the memory is cleared at the end of
execution of a given job. The `HistMaker` object can be changed to the desired one by changing the `typedef` statement
at the top of the `histogramming_config.C` configuration file.

To add a histogram selection set to an existing histogrammer <HISTMAKER>, add an entry in `src/<HISTMAKER>.cc:InitHistogramFlags` with
 `fEventSets [<selection enum> + <base set>] = 1;` to tell the histogrammer to initialize these histograms (by default, the Loose ID and same-sign
 sets are also initialized).
Then add in the `src/<HISTMAKER>.cc:Process` function the call `FillAllHistograms(set_offset + <base set>)`, with
the desired selection cuts.

To add a histogram to the standard histogramming of an existing histogrammer <HISTMAKER>, add a field for it to
`interface/<EventHist/LepHist>_t.hh`, then initialize the histogram for each selection set in `src/<HISTMAKER>.cc:Book<Event/Lep>Histograms`,
and add the corresponding `Fill` call in `src/<HISTMAKER>.cc:Fill<Event/Lep>Histogram`.

To create a new histogrammer, simply add a new histogramming object extending HistMaker, implement any updates/overrides,
and then add the object to the class lists: `src/classes.h` and `src/classes_def.xml`. See `interface/SparseHistMaker.hh`
and `interface/CLFVHistMaker.hh` for examples of this.

Histogramming example:
```
cd rootScripts/
# set the dataset processing flags to true for each dataset of interest in histogramming_config.C
# set the config.onlyChannel_ flag to a specific channel to process (e.g. "emu") or "" to process all channels
# set the config.skipChannels_ list to specify channels to ignore when histogramming
# set typedef <HistMaker of interest> HISTOGRAMMER
# update   nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/clfv_nanoaod_test_trees/"; to your area if needed
root.exe -q -b process_clfv.C
./move_histograms.sh [output directory, e.g. "nanoaods/"] "" d
rm *.hist
```

## Plotting

Plotting is done using pre-made histogram files from a HistMaker object, using the `src/DataPlotter.cc` object.

This is primarily done using the `histograms/dataplotter_clfv.C` script, with dataset configured in `histograms/datacards.C`.

For example:
```
cd histograms
root.exe dataplotter_clfv.C
root> //years_=[vector of years to plot, e.g. {2016,2017,2018}];
root> years_={2016,2017,2018};
root> //hist_tag_ = [proper tag, e.g. "clfv" for CLFVHistMaker or "sparse" for SparseHistMaker]
root> //see more options in datacards.C, such as useEmbed_ = 0 or 1
root> //nanoaod_init([channel], [histogram dir], [output dir]);
root> //e.g.
root> nanoaod_init("emu", "nanoaods_dev", "nanoaods_dev");
root> //reads /eos/uscms/store/user/${USER}/histograms/nanoaods_dev/ and creates ./figures/nanoaods_dev/emu/2016_2017_2018/
root> // dataplotter_->plot_stack([hist], [type, "event" or "lep"], [absolute set] [optionally , [xmin], [xmax]])
root> //e.g.
root> dataplotter_->plot_stack("leppt", "event", 208, 0., 100);
root> dataplotter_->include_qcd_ = 1; //turn on/off same-sign QCD estimate
root> dataplotter_->include_misid_ = 0; //turn on/off loose ID j-->tau estimate
root> dataplotter_->rebinH_ = 2; //rebin histograms
root> dataplotter_->plot_stack("leppt", "event", 208, 0., 100);
root> dataplotter_->print_stack("leppt", "event", 208, 0., 100); //print plot to disk
root> dataplotter_->plot_data_ = 0; //turn off data
root> dataplotter_->data_over_mc_ = -1; //plot signal / background in the ratio plot
root> //PlottingCard_t card([name], [type], [set], [rebinning], [xmin], [xmax], [{blind min(s)}], [{blind max(es)}]);
root> PlottingCard_t card("lepm", "event", 208, 2, 50, 170, {84., 118.}, {98., 132.})
root> dataplotter_->print_stack(card);
root> //print many plots, ranges/rebinning may need updating as histograms change
root> print_basic_debug_plots(); //may be slow due to xrootd reading and memory filling from histograms
root> .qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
```


## Standard histogram sets:

Histogram sets have a base number of 1-99. They then have a final state selection offset (defined in `interface/HistMaker.hh`) of:

- mu+tau  :   0
- e+tau   : 100
- e+mu    : 200
- mu+tau_e: 300
- e+tau_mu: 400
- mu+mu   : 500
- e+e     : 600

They then additionally have an offset of 1000 for same-sign events and 2000 for Loose ID events
(e.g. loose tau anti-jet ID or anti-isolated muon/electron ID).
All histograms for a selection set are then stored in `Data/<event/lep>_<set number>/` in a given histogram file.

Standard selection set base numbers used by CLFVHistMaker:
- 1: All events
- 7: Preselection events without a b-jet veto
- 8: Standard preselection events
- 30: QCD j-->tau control region
- 31: W+Jets j-->tau control region
- 32: ttbar j-->tau control region
- 35: Standard preselection events, including MC fake taus
- 36: QCD j-->tau control region, including MC fake taus
- 37: W+Jets j-->tau control region, including MC fake taus
- 38: ttbar j-->tau control region, including MC fake taus
- 82: ttbar j-->tau weights in the nominal selection, including MC fake taus

## Directories
### src and interface
Contains the analyzers and tools.

### histograms
Used to plot the histograms from a given book in a HistMaker's output
adding together the processes with their cross section and generation values, primarily
using dataplotter_clfv.C. This uses the DataPlotter data handler to plot, storing the object as a field of the script

### tmva_training
Used to train MVAs to separate backgrounds from signal

CWoLa training uses variables not correlated with the dilepton mass to separate two mass ranges,
hopefully only succeeding when there is a resonance in one of the masses

Typical training uses signal/background labels to train the MVAs

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
  
### CLFVHistMaker
```c++
.L lib/libCLFVAnalysis.so
TTree* tree = [Get CLFV Tree for selection S];
auto selec = new CLFVHistMaker();
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

##### Trains MVA specified in TrainTrkQual.C, default is a BDT 
```c++
.L train_tmva.C;
train_tmva("[make_background.C out file]", {[list of process IDs to ignore]}");
```


### Studies (mostly scale factors)
Several scale factor are measured for this analysis, as well as inputs for POG derived scale factors.
The order of the scale factors given below is the order they should be evaluated in.

#### Jet PU ID
The Jet/MET POG gives scale factors for MC --> Data, but the MC efficiencies need to be evaluated
for the specific selection. This is done using `studies/jet_puid_eff/`.
This generates the histogram using the TTree rather than histograms produced by the CLFVHistMaker.

```
$> cd studies/jet_puid_eff
$> root.exe -q -b "scale_factors([year], \"[selection]\", \"[tree path]\", \"[MC file to use]\");
```

This should be done for all years needed.
The JetPUWeight object that uses this assumes the selection is "mumu".

#### B-Tag
The B-Tag POG gives scale factors for MC --> Data, but the MC efficiencies need to be evaluated
for the specific selection. This is done using `studies/btag_scale/`

```
$> cd studies/btag_scale
$> root.exe -q -b "scale_factors(\"[selection]\", [histogram set], [year], [b-tag WP], \"[histogram path]\");
```

This should be done for all years, selections, and working points needed.
This can be automated using:

```
$> ./make_all_scales.sh
```

#### Z pT re-weighting
The Drell-Yan Monte Carlo has imperfect modeling of the Z pT vs Mass spectrum.
To correct for this, Data/MC weights in the mumu selection are used to correct the
distrubution. These are evaluated first using reconstructed quantities, and then
the correction factors are derived by comparing the Generator level pT vs Mass before
and after the reconstructed level correction. These generator level corrections are
then used for all selections.

```
$> cd studies/z_pt_scale
$> root.exe -q -b "scale_factors.C(true, [histogram set], [year], \"[histogram path]\")
```

The current strategy for making these scale factors is to use the mumu selection,
create the scale factors for a given year, re-histogram the samples (mostly just DY50)
and then re-create the scale factors to have the proper gen-level scale factors.

#### Jet --> tau transfer factors
The jet --> tau background is estimated using a loose jet ID control region, where
the data is applied to the tight jet anti-ID region using transfer factors. These factors
are measured in the mumu and ee selection region, where the number of taus in bins
of pT, eta, and decay mode are counted in both the loose and tight anti-jet ID region.
The transfer factor is then N(tight) / N(loose) for the given bin, though the fraction
N(tight) / (N(tight) + N(loose)) is instead stored since it's more convenient to plot
given it's restricted between 0 and 1.

```
$> cd studies/fake_tau_scale
$> root.exe -q -b "scale_factors.C(\"[measurement selection]\", [selection set], [year])"
$> root.exe -q -b "test_data_scale.C("[measurement selection]\", [tight tau selection set], [loose tau selection set], [year])"
```

#### QCD transfer factors
The QCD background is estimated using a loose lepton ID control region, where
the data is applied to the tight ID region using transfer factors. These factors
are measured in the emu selection region, where the number of events in bins
of lepton delta R are counted in both the loose and tight ID region.
The transfer factor is then N(tight) / N(loose) for the given bin

```
$> cd studies/qcd_scale
$> root.exe -q -b "scale_factors.C(\"[measurement selection]\", [selection set], [year])"
```
