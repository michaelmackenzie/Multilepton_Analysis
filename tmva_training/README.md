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

Output of older trainings can be studied with:
```
$> root -l
root> TMVA::TMVAGui("[path/to/training/output/root/file.root]")
```
where the output file is under the directory created for the training.
