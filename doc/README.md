# Overall workflow

The analysis can be broken down into the following stages:

- Skimming

- Format conversion and selection filtering

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
cmsrel CMSSW_10_2_18
cd CMSSW_10_2_18/src
cmsenv
```

Dataset skimming:
```
git clone https://github.com/michaelmackenzie/ZEMuAnalysis.git StandardModel/ZEMuAnalysis #or use https://github.com/ --> git@github.com:
git clone https://github.com/cms-nanoAOD/nanoAOD-tools.git PhysicsTools/NanoAODTools
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
https://github.com/michaelmackenzie/ZEMuAnalysis/

The main dataset skimming is done by the `python/runSkimModule.py` which is called by the
processing script `python/LFVAnalyzer.py`.

LFVAnalyzer takes several command line arguments:
`python python/LFVAnalyzer.py [input file] isData=[data for Data, MC for MC, Embedded for Embedding sampled]  year=[year] saveZ=[True for DY or Z signal sample, False otherwise] [optional [nevents] [nstart event]]`

An example for local testing:
```
DATASET="/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16NanoAODv7-PUMoriond17_Nano02Apr2020_102X_mcRun2_asymptotic_v8_ext2-v1/NANOAODSIM"
voms-proxy-init --rfc --voms cms
xrdcp -f "root://cmseos.fnal.gov/"`das_client -query="file dataset=${DATASET}" | head -n 1` ./test/NANO.root
python LFVAnalyzer.py ./test/NANO.root isData=MC year=2016 saveZ=True
ls -l tree.root #results
```

Grid jobs are submitted using LPC condor in the scripts/submitBatch_Legacy.py, which uses the python/BatchMaster.py object.
The datasets are configured in a `samplesDict` dictionary and the `samplesToSubmit` list indicates which to submit.

An example for grid submission:
```
voms-proxy-init --rfc --voms cms
cd scripts
./submitBatch_Legacy.py
./query_grid.sh #monitor jobs
```

When the jobs are finished, failed jobs can be investigated and resubmitted by:
```
./check_batch_job.sh [path to job] ["d" to open each output root file or ""] ["d" to resubmit failed jobs or ""]
#e.g.
./check_batch_job.sh batch/LFVAnalyzer_20220516_125456/ "" d
```

Merging output datasets:

THIS NEEDS TO BE UPDATED TO USE XROOTD TO MERGE AND WRITE BACK

```
cp test/scripts/prepare_batch.py /eos/uscms/store/user/${USER}/
cd /eos/uscms/store/user/${USER}/
python prepare_batch.py [output directory, e.g. "nano_batchout/LFVAnalyzer_20220516_125456/"]
```

After this, merged output files should be in `/eos/uscms/store/user/${USER}/lfvanalysis_rootfiles/`

## Format conversion and selection filtering

From this point, all tools are in the repository (with a historical name for multiple two lepton selections):
https://github.com/michaelmackenzie/Multilepton_Analysis/

The analysis was originally based on the BLT ntuples with an analyzer written for the original Multilepton_Analysis
based analysis. Due to this, a conversion step is required between the NANOAOD format ntuples and performing the
analysis.

The conversion is done by the `src/NanoAODConversion.cc` tool, which reads in the events, counts the number of
objects (using a potentially different selection criteria for each considered final state), and applies some
corrections to the physics objects. Each final state (e+e, mu+mu, e+mu, e+tau, and mu+tau) is written to a separate
TTree.

The processing of the datasets is done using the script `rootScripts/process_nanoaods.C`. For example:
```
cd rootScripts
#set the dataset processing flags to true for each dataset of interest
#change path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/" to your area if skimming completed
root.exe -q -b "process_nanoaods.C"
```


## Histogramming and kinematic selections

The histogramming stage involves reading the ntuples from the NanoAODConversion stage for a given final
state (e.g. mu+tau), applying more corrections, transfer factors, etc., and filling different histogram
"sets" with the event variables. This is done by the `src/CLFVHistMaker.cc` object. A histogram set corresponds
to a certain selection (e.g. no b-jets), where each set has a base number (e.g. 8), an offset corresponding to
the final state (e.g. 0 for mu+tau, 200 for e+mu), and then an offset of 1000 if the event is a same-sign event,
and an offset of 2000 if the event is a loose ID lepton event. For example, a standard Z->e+mu signal event for base
set 8 would likely have an absolute set number of 208, and a loose ID same-sign event would have an absoluted set
number of 3008. This allows estimates of data-driven backgrounds in the selection corresponding to set number 8 by
using identical control regions with the same-sign or loose lepton ID. Histogram filling is done by the
`FillAllHistograms([set])` function.

Histogramming is performed using the script `rootScripts/process_clfv.C` where the histograms and final
states of interest are selected using the `rootScripts/histogramming_config.C` script. Due to issues in the
current code where there are likely memory leaks in the histogramming code, the `process_clfv.C` can submit
a new job calling `process_card.C` to histogram a given dataset so the memory is cleared at the end of
execution of the job.

To add a histogram selection, add an entry in `interface/CLFVHistMaker.hh:Init` with `fEventSets [<selection enum> + <base set>] = 1;`
and then add in the `interface/CLFVHistMaker.cc:Process` function the call `FillAllHistograms(set_offset + <base set>)`, with
the desired selection cuts.

To add a histogram to the standard histogramming, add a field for it to `interface/<EventHist/LepHist>.hh`, then iniitialize
the histogram for each selection set in `src/CLFVHistMaker.cc:Book<Event/Lep>Histograms`, and the corresponding `Fill` call
in `src/CLFVHistMaker.cc:Fill<Event/Lep>Histogram`.

For example:
```
cd rootScripts/
#set the dataset processing flags to true for each dataset of interest in histogramming_config.C
#set the config.onlyChannel_ flag to a specific channel to process (e.g. "emu") or "" to process all channels
#set the config.skipChannels_ list to specify channels to ignore when histogramming
# update   nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/clfv_nanoaod_test_trees/"; to your area if conversion completed
root.exe -q -b process_clfv.C
./move_histograms.sh [output directory, e.g. "nanoaods/"] "" d
rm *.hist
```

## Plotting

Plotting is done using pre-made histogram files from the CLFVHistMaker object, using the `src/DataPlotter.cc` object.

This is primarily done using the `histograms/dataplotter_clfv.C` script, which dataset configured in `histograms/datacards.C`.

For example:
```
cd histograms
root.exe dataplotter_clfv.C
root> years_=[vector of years to plot, e.g. {2016,2017,2018}];
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
root> print_standard_plots({8}/*no offset needed unless offsetSets_ = false*/); //may be slow due to xrootd reading and memory filling from histograms
root> .qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
```