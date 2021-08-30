# Training MVAs using TMVA

TMVA is a ROOT package for training, evaluating, and applying multi-variate analysis (MVA) techiniques.
The most commonly used MVAs here are the boosted decision tree (BDT) and artificial neural networks (ANN).
The ANN used here is the multi-layer perceptron (MLP).

The most common method of training here uses `train_tmva.C`.
This is used to load a TTree that is a merged list of TTrees from the `ZTauTauHistMaker`.
This does some configuring, and then passes the tree to `TrainTrkQual.C` for the actual training.

There is a map of strings to integers, that turns on/off different MVAs to be trained.
By default, only the standard BDT is trained.

Depending on the name of the tree, the selection/boson of interest in the training is identified.
The script then adds variables to be trained using, or spectators to simply carry along, corresponding
to these selections/boson identified.

The fraction of events to train are either decided by the script, or are passed from the ZTauTauHistMaker pre-defined.

The structure of the MVAs can be edited by following to where they're initialized in the TMVA factory.

Example:
```
$> root -l
root> .L train_tmva.C
root> train_tmva("[path/to/tree.tree]", {[list of signal IDs]}, {[list of IDs to ignore]}, [integer flag of which bjet count to use])
```

## Testing

Variable initialization/configuration is controlled by `interface/TrkQualInit.hh`, with the version
set by the flag `trkqual_version_` in `tmva_training/TrainTrkQual.C`.

After running a training, output of the training can be studied with:
```
$> root -l
root> TMVA::TMVAGui("[path/to/training/output/root/file.root]")
```
where the output file is under the directory created for this training.


### Validate TMVAReader

To ensure the variables are being properly passed to the reader, the TrkQualInit object can be used to
setup the TMVAReader in the same way the TMVA training was initialized. This setup can be validated by
using `scripts/compare_reader.C` as follows:

```
cd scripts
root.exe "compare_reader.C(\"[selection]\", [vector of years], [hist set], [TrkQualInit version, defaults to Default])"
```
where `maxEntries_` can be set for faster testing.


### Plot variables and estimate limits

To study the distributions in the trees as well as the MVA variables, use the script `scripts/plot_ztautau_tmva_tree.C`.
The MVA variable of interest is set using the defined enums at the top and the variable `MVA_`.
The plotting information is defined in the script anonymous namespace at the top.

The process labels defined by the `get_label_from_category` function is a hack to try to make these distributions
more meaningful, so it needs updating at any point the `rootScripts/process_ztautau.C` data file lists are changed.

`get_file_name` is a short cut to define the training file name using the selection information given.

`plot_tmva_tree` is used to plot the variable defined in the namespace using the passed in file information. It also
allows one to cut on the MVA variable, to see how this affects the variable of interest (most useful for studying the
di-lepton mass shape change vs MVA score cut in the B->emu MVAs). `stack_tmva_tree` can similarly be used to make a stack
instead of histograms.

```
$> cd scripts
$> root.exe plot_ztautau_tree.C
root> plot_tmva_tree(get_file_name("zmutau", {2016,2017,2018}, 8), -1, -1)
root> stack_tmva_tree(get_file_name("zmutau", {2016,2017,2018}, 8), -1, 0)
```