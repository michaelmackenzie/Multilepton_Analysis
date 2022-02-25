# Running interactive ROOT scripts with compiled objects

## Histogramming

Histogramming is done using the CLFVHistMaker, which inherits from TSelector. 

TTrees are assumed to come from the CLFVAnalyzer, but TTrees from NanoAODs can be created using the NanoAODConversion
object.

The script `process_nanoaods.C` will run over a standard list of NanoAODs and create the needed trees. It also gives an
example of how to configure the NanoAODConversion object.

The script `process_clfv.C` will process a standard list of CLFVAnalyzer trees with the CLFVHistMaker:
```
$> root.exe process_clfv.C
```

The `process_clfv.C` script has many flags defined before its processing loop.
Some of the flags are:
 - useNanoAods = flag to use NanoAOD based samples, 0 = ignore, 1 = include, -1 = only do these samples
 - writeTrees = flag to write TTrees along with histograms for MVA training
 - onlyChannel = name of the CLFVAnalyzer selection to process
 - skipChannels = names of CLFVAnalyzer selections to ignore
 - reProcessMVAs = flag to reprocess MVA scores
 - signalTrainFraction = fraction of signal MC to dedicate to MVA training
 - backgroundTrainFraction = fraction of background MC to dedicate to MVA training
 - useTauFakeSF = flag to pass to the CLFVHistMaker to use pre-defined fake tau scale factors (1) 
 or to reprocess them (2), or don't apply at all (0)
 
 These histogram files can be then moved to a directory under `histograms/` where it can be plotted using the DataPlotter.
 
 To make a single tree with the relevant processes, use `make_clfv_background.C`. This will generate a tree that
 can be passed to a TMVA training, where processes are weighted by `fulleventweight` (which includes cross-section)
 and can be differentiated by `eventcategory`.
 
 There are several flags in the background making, which turn on/off processes. The most relevant is `doHiggsDecays_` which is used to 
 switch between Higgs and Z0 CLFV signals. For NANOAODs, the `doNanoAODs_` flag should be set to true.
 
```
$> root.exe make_clfv_background.C
root> doHiggsDecays_ = [true/false]
root> make_background([selection set number], "[selection]", "[path/to/histogram/files/")
```
