# Z pT re-weighting
The Drell-Yan Monte Carlo has imperfect modeling of the Z pT vs Mass spectrum.
To correct for this, Data/MC weights in the mumu selection are used to correct the
distrubution. These are evaluated first using reconstructed quantities, and then
the correction factors are derived by comparing the Generator level pT vs Mass spectrum
before and after the reconstructed level correction. These generator level corrections
are then used to correct the Z spectrum for all selections.

To measure this correction, histogramming of at least the DY MC has to be performed twice:
once to get the reconstruction-level correction factors, and the second time to get the
generator-level correction factors. The di-muon selection is used to perform this measurement
as the Z->mu+mu reconstruction is very accurate and most closely reconstructs the true
Z spectrum.

To estimate the systematic uncertainty on this correction, this measurement can be performed
using a di-electron selection instead of a di-muon selection. This involves setting the
ZPtWeight object, which reads the correction files and applies the event-by-event weight,
to use the `systematic` weight set. The same two histogramming stages are again required
to perform this measurement.

Creating a set of correction factors:
```
$> root.exe -q -b "scale_factors.C([true for mumu, false for ee], [histogram set, default is 8], [year], \"[histogram path, default is nanoaods_dev]\")
```
