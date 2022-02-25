# Plotting histograms

## Using the dataplotter
The `dataplotter_clfv.C` script is used to initialzie a DataPlotter* (as dataplotter_ in the anonymous namespace) with CLFVHistMaker histograms.
Example initialization (these values are all set to default ones otherwise):
```
$> root.exe dataplotter_clfv.C
root> selection_ = "[selection, e.g. emu]"
root> folder_ = "[directory to print to under figures/, e.g. clfv_decays]"
root> hist_dir_ = "[directory to find the histograms under, e.g. clfv]"
root> useNanoAODs_ = [true if using nano aod based histograms, false otherwise]
root> doStatsLegend_ = [true to add yields in the legend]
root> init_dataplotter()
```

After initializing, the dataplotter_ should be properly setup to plot relevant figures:
```
root> dataplotter_->plot_stack("[histogram name, e.g. lepm]", "[histogram type, e.g. event]", [set number, e.g. 47], [rebin histograms, default 1],
                                [x axis min, default 1], [x axis max, default -1 (if min > max doesn't change axis)]);
root> dataplotter_->plot_stack(DataPlotter::PlottingCard_t(...))
```

Print a standard set of plots, with binning and axis ranges set to commonly used, pre-defined values (recommended to use batch mode):
```
$> root.exe -b dataplotter_clfv.C
root> [initialize selection and dataplotter_]
( if already initialized but not in batch mode: root> gROOT->SetBatch(kTRUE) )
root> dataplotter_->print_standard_plots({[list of sets to print, e.g. 47, 48]}, {[list of signal scales to use, empty to use default, e.g. 250., 250.]},
                                         {[list of base rebinning of histograms for lower statistics, empty to not have extra rebinning]}) 
```

Get the yields and statistical uncertainty on the yields for a given set:
```
root> print_statistics("[histogram name]", "[histogram type]", [histogram set], [xmin to consider], [xmax to consider, xmin > xmax uses full range])
```
