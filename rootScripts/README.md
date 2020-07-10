# Running interactive ROOT scripts with compiled objects

## Histogramming

Histogramming is done using the ZTauTauHistMaker. 
To recompile this (or any *.cc object):
```
$> root -l
root> .L ZTauTauHistMaker.cc++g //+ = compile, ++ = force re-compile even if unchanged, g = debug compilation
```

TTrees are assumed to come from the ZTauTauAnalyzer, but TTrees from NanoAODs can be created using the NanoAODConversion
object.
```
$> root -l
root> .L ParticleCorrections.cc+g //Contains scale factors for electrons and muons
root> .L NanoAODConversion.cc+g //Will run over a NanoAOD and output ZTauTauAnalyzer-like trees
```
The script `process_nanoaods.C` will run over a standard list of NanoAODs and create the needed trees.

The script `process_ztautau.C` will process a standard list of ZTauTauAnalyzer trees with the ZTauTauHistMaker:
```
$> root -l process_ztautau.C # root -l to avoid loading to-be-recompiled ZTauTauHistMaker library
```

The `process_ztautau.C` script has many flags defined before its processing loop.
Some of the flags include:
 - doProcess[] = array of flags to process or not process a ZTauTauAnalyzer sample in the list
 - nanoaod_process[] = array of flags to process or not process a NanoAOD based sample in the list
 - useNanoAods = flag to use NanoAOD based samples, 0 = ignore, 1 = include, -1 = only do these samples
 - writeTrees = flag to write TTrees along with histograms for MVA training
 - onlyChannel = name of the ZTauTauAnalyzer selection to process
 - skipChannels = names of ZTauTauAnalyzer selections to ignore
 - reProcessMVAs = flag to reprocess MVA scores
 - signalTrainFraction = fraction of signal MC to dedicate to MVA training
 - backgroundTrainFraction = fraction of background MC to dedicate to MVA training
 - useTauFakeSF = flag to pass to the ZTauTauHistMaker to use pre-defined fake tau scale factors (1) 
 or to reprocess them (2), or don't apply at all (0)
 
 These histogram files can be then moved to a directory under `histograms/` where it can be plotted using the DataPlotter.
 
 To make a single tree with the relevant processes, use `make_ztautau_background.C`. This will generate a tree that
 can be passed to a TMVA training, where processes are weighted by `fulleventweight` (which includes cross-section)
 and can be differentiated by `eventcategory`.
 
 There are several flags in the background making, which turn on/off processes. The most relevant is `doHiggsDecays_` which is used to 
 switch between Higgs and Z0 CLFV signals.
 
```
$> root -l make_ztautau_background.C
root> doHiggsDecays_ = [true/false]
root> make_background([selection set number], "[selection]", "[path/to/histogram/files/")
```
