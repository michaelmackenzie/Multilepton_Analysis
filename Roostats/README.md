# Description of RooStats and RooFit based upper limit evaluations

## Evaluating the upper limit for B->e+mu using an invariant mass fit
The UL calculation uses the Z->e+e and Z->mu+mu data to create a morphed signal template.
The background in the e+mu channel is then fit using MC estimates.
The background fit + signal template is then used to evaluate the expected upper limit, inluding
a few log-normal systematics.

### Create same flavor histograms for Z mass resonance
creates Z->ee and Z->mumu di-lepton mass histogram files
```
root.exe "create_same_flavor_histograms.C([set number], [hist path], [year])"
```
## Fit the same flavor mass distributions
fits the ee and mumu channels with a crystall ball function, saves the 
workspace to be accessed later
```
root.exe "fit_same_flavor.C([set number], [year], [do di-muon])" 
```
### Morph the Z resonance yields
creates a morphed signal PDF using the Z->ee and mumu data/fits
```
root.exe "morph_signal.C([set number], [year], [hist path])"
```
### Create e+mu background trees
runs the standard background tree making, without signals
```
root.exe "create_background_trees.C([set number], [year], [hist path])"
```
### Fit e+mu background distribution
Fits the background distribution with a Berstein poly, saves workspace
```
root.exe "fit_background([set number], [year], [add lum to weight])"
```
### Calculate UL
Runs a simple asymptotic CLs calculation of the upper limit
```
root.exe "calculate_UL.C([set number], [year])"
```

## B->e+mu upper limit using binned distributions

## Input parameters
HISTSET: Base histogram set number corresponding to the selection in ZTauTauHistMaker
default: 8
options: single set number or bracketed list of set numbers for separate categories

SELECTION: Signal decay performing the analysis for
default: zmutau
options: zmutau, zetau, hmutau, hetau

YEAR: List of years to perform the analysis on (in brackets)
default: {2016, 2017, 2018}

HISTPATH: Directory for the set of histograms to use, from /uscms/store/user/mmackenz/histograms
default: nanoaods_dev

### Retrieve the binned distributions
```
root.exe -q -b "get_bemu_histogram.C($HISTSET, \"$SELECTION\", $YEAR, \"$HISTPATH\")"
```

### Create background and signal PDFs
Read the histogram files for the background and signal PDFs, fit analytic functions to these.
Creates a RooSimultaneous PDF for the multiple categories of the selection.
Also creates PDFs for each systematically shifted histogram.
```
root.exe -q -b "fit_background_bemu_binned.C($HISTSET, \"${SELECTION}\", ${YEAR})"
```

### Perform upper limit test
Read the workspace for the background and signal PDFs, perform upper limit calculations.
```
root.exe -q -b "calculate_UL_bemu_binned.C(${HISTSET}, \"${SELECTION}\", ${YEAR})"
```

## Evaluating the upper limit for B->X+tau using an MVA score fit
The UL calculation uses MC and data-driven background estimated MVA score
histograms to create a background PDF and a histogram of the expected
signal MVA score distribution to create the signal PDF

### Input parameters
HISTSET: Base histogram set number corresponding to the selection in ZTauTauHistMaker
default: 8

SELECTION: Signal decay performing the analysis for
default: zmutau
options: zmutau, zetau, hmutau, hetau

YEAR: List of years to perform the analysis on (in brackets)
default: {2016, 2017, 2018}

HISTPATH: Directory for the set of histograms to use, from /uscms/store/user/mmackenz/histograms
default: nanoaods_dev

### Get MVA score histograms
Load the histogram sets using the DataPlotter defined in CLFVAnalysis/histograms/dataplotter_ztautau.C.
Writes these histograms locally to Roostats/histograms/.
Histograms using systematically shifted scale factors are also stored.
```
root.exe -q -b "get_MVA_histogram.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, \"${HISTPATH}\")"
```
### Create background and signal PDFs
Read the histogram files for the background and signal PDFs, create RooHistPdfs for these.
Creates a RooSimultaneous PDF for the multiple categories of the selection.
Also creates PDFs for each systematically shifted histogram.
```
root.exe -q -b "fit_background_MVA_binned.C(${HISTSET}, \"${SELECTION}\", ${YEAR})"
```
### Perform upper limit test
Read the workspace for the background and signal PDFs, perform upper limit calculations.
```
root.exe -q -b "calculate_UL_MVA_categories.C(${HISTSET}, \"${SELECTION}\", ${YEAR})"
```
### Perform systematic tests
Read the workspace for the background and signal PDFs, perform upper limit calculations.
Systematics are numbered based on the histogram set filled in the Systematic filling in ZTauTauHistMaker.
```
root.exe -q -b "toyMC_mva_systematics.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, ${SYSTEMATIC})"
```
Many systematics can be tested at once using a bash script
```
time ./do_toyMC_MVA_stats.sh "${HISTSET}" "${SELECTION}" "${YEAR}" [Number of fits] [Maximum systematic] [Minimum systematic] [seed]
```
Alternatively, one can test using a RooMCStudy
```
root.exe -q -b "toyMC_mva_roomcstudy.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, ${SYSTEMATIC})"
```

## Using Higgs Combine with MVA fits

Higgs Combine statistics:
https://cds.cern.ch/record/1379837/files/NOTE2011_005.pdf

Higgs Combine page:
https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/

Useful Higgs Combine tutorial:
https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/tutorial2020/exercise/

Useful variables:

HISTSTRING="8"

YEARSTRING="2016_2017_2018"

### Create the data cards and files
This assumes histograms for the search channel have already been retrieved.

This creates a data card and data file for each selection, and then the combined search:
```
root.exe -q -b "create_combine_cards.C(${HISTSET}, \"${SELECTION}\", ${YEAR})"
```
#### Card inputs
```
imax <number of categories>
jmax <number of background types>
kmax <number of nuisance parameters>

bin <cat name one> <cat name two> ...
observation <n in cat one> <n in cat two> ...

bin <cat name one> <cat name one> ..for each background/signal.. <cat name two> ...
process <bkg/sig name> <next bkg/sig name> ...
process  <process number> ...

rate <n exp in cat 1 process 1> ....

#shape based uncertainties
<systematic> shape <1/0> <1/0> ...
#log-normal uncertainty, also gmN = gamma, lnU = log-uniform
<systematic> lnN <value> <value_2> ...

#Use Barlow-Beeston-lite in all bins for given channel, * for all channels
[channel] autoMCStats 0
```

### Estimate an upper limit
Standard running:
```
cd datacards/${YEARSTRING}
combine -d combine_mva_total_${SELECTION}.txt
```
One can estimate the bin uncertainty impact by commenting out "* autoMCStats 0"

### Generate impacts
```
cd datacards/${YEARSTRING}
WORKSPACE=combine_mva_total_${SELECTION}_workspace.root
text2workspace.py combine_mva_total_${SELECTION}_${HISTSTRING}.txt -o ${WORKSPACE}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -1 --rMax 2 --robustFit 1 --doInitialFit
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -1 --rMax 2 --robustFit 1 --doFits
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -1 --rMax 2 --robustFit 1 --output impacts_${SELECTION}.json
plotImpacts.py -i impacts_${SELECTION}.json -o impacts_${SELECTION}
```

### Additional tests
Generate workspace:
```
text2workspace.py <card> -o <workspace>
```
Perform diagnostics:
```
combine -M FitDiagnostics <card>  --forceRecreateNLL
```
Perform upperlimits:
```
combine -M AsymptoticLimits <card> [--run <expected/blind>] [--noFitAsimov]
```
Without asymptotic limits:
```
combine -M HybridNew <datacard> --LHCmode LHC-limits -n <name> --saveHybridResult --fork 0 [ --expectedFromGrid <quantile, e.g. 0.500>]
```

Options:
--rule: CLsplusb, CLs
