# Overall workflow

The analysis can be broken down into the following stages:

- Skimming

- Histogramming and kinematic selections

- Plotting

- Scale factor measurements

- BDT training and evaluation

- Signal search

## Initial setup

CMSSW:
```
cd ~/nobackup
mkdir CLFV
cd CLFV
cmsrel CMSSW_10_6_29
cd CMSSW_10_6_29/src
cmsenv
```

Dataset skimming:
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
cd -
scram b -j4
```

Remainder of the analysis:
```
git clone https://github.com/michaelmackenzie/Multilepton_Analysis.git CLFVAnalysis #no second level to avoid CMSSW scram compiling
cd CLFVAnalysis
make -j4
```

## Skimming

Dataset skimming is done using a separate repository, based on the NANOAOD Tools package:
`https://github.com/michaelmackenzie/Z_LFV_Analysis.git/`, under `src/PhyscisTools/NanoAODTools/`


The main dataset skimming is done by the `python/analyzers/LFVAnalyzer.py`

LFVAnalyzer takes several command line arguments:
`python python/LFVAnalyzer.py [comma separated input file list] [data for Data, MC for MC, Embedded for Embedding sampled] [year]

An example for local testing:
```
cd ${CMSSW_BASE}/src/PhysicsTools/NanoAODTools/
DATASET="/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16NanoAODv7-PUMoriond17_Nano02Apr2020_102X_mcRun2_asymptotic_v8_ext2-v1/NANOAODSIM"
voms-proxy-init --rfc --voms cms --hours 36
xrdcp -f "root://cmseos.fnal.gov/"`das_client -query="file dataset=${DATASET}" | head -n 1` ./NANO.root
python python/LFVAnalyzer.py ./NANO.root MC 2016
ls -lh tree.root #results
root.exe -q -b "condor/split_output_tree.C(\"tree.root\", \"tree-split.root\")" #split output ntuple into 5 ntuples
root.exe -q -b "condor/add_norm.C(\"NANO.root\", \"tree-split.root\")" #add normalization event count info
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
./check_batch_job.sh batch/LFVAnalyzer_20220516_125456/ --ignorerunning
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
"sets" with the event variables. This is done by the `src/CLFVHistMaker.cc` object.

A histogram set corresponds to a certain selection (e.g. no b-jets), where each set has a base number (e.g. 8), an offset corresponding to
the final state (e.g. 0 for mu+tau, 200 for e+mu), and then an offset of 1000 if the event is a same-sign event,
and an offset of 2000 if the event is a loose ID lepton event. For example, a standard Z->e+mu signal event for base
set 8 would likely have an absolute set number of 208, and a loose ID same-sign event would have an absoluted set
number of 3008. This allows estimates of data-driven backgrounds in the selection corresponding to set number 8 by
using identical control regions with the same-sign or loose lepton ID. Histogram filling is done by the
`FillAllHistograms([set])` function.

Currently, the histogramming object is being extended to an inheritence based model, where `interface/HistMaker.hh`
describes the general histogram maker, the typical event weight initialization, histogram definitions, etc., and
then a specific histogramming object can inherit from this and only implement the desired changes (e.g. the
`SparseHistMaker` redefines the histograms made to be only a small set of important distributions). The
`CLFVHistMaker` is being reimplemented in this form, currently as `CLFVTmpHistMaker`.

Histogramming is performed using the script `rootScripts/process_clfv.C` where the datasets and final
states ntuples of interest are selected using the `rootScripts/histogramming_config.C` script. Due to issues in the
current code where there are likely memory leaks in the histogramming code, the `process_clfv.C` can submit
a new job calling `process_card.C` to histogram a given dataset so the memory is cleared at the end of
execution of the job. The histogram object can be changed to the desired one by changing the `typedef` statement
at the top of the `histogramming_config.C` configuration file.

To add a histogram selection set to an existing histogrammer <HISTMAKER>, add an entry in `src/<HISTMAKER>.cc:InitHistogramFlags` with
 `fEventSets [<selection enum> + <base set>] = 1;` to tell the histogrammer to initialize these histograms.
Then add in the `src/<HISTMAKER>.cc:Process` function the call `FillAllHistograms(set_offset + <base set>)`, with
the desired selection cuts.

To add a histogram to the standard histogramming of an existing histogrammer HISTMAKER, add a field for it to
`interface/<EventHist/LepHist>.hh`, then iniitialize the histogram for each selection set in `src/<HISTMAKER>.cc:Book<Event/Lep>Histograms`,
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

Plotting is done using pre-made histogram files from an HistMaker object, using the `src/DataPlotter.cc` object.

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

- mu+tau: 0
- e+tau: 100
- e+mu: 200
- mu+tau_e: 300
- e+tau_mu: 400
- mu+mu: 500
- e+e: 600

They then additionally have an offset of 1000 for same-sign events and 2000 for loose ID events
(e.g. loose tau anti-jet ID or anti-isolated muon ID)
All histograms for a selection set are then stored in `Data/<event/lep>_<set number>/`

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
