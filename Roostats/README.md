# Evaluating the upper limit for Z->e+mu using an invariant mass fit
The UL calculation uses the Z->e+e and Z->mu+mu data to create a morphed signal template.
The background in the e+mu channel is then fit using MC estimates.
The background fit + signal template is then used to evaluate the expected upper limit, inluding
a few log-normal systematics.

## Create same flavor histograms for Z mass resonance
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
## Morph the Z resonance yields
creates a morphed signal PDF using the Z->ee and mumu data/fits
```
root.exe "morph_signal.C([set number], [year], [hist path])"
```
## Create e+mu background trees
runs the standard background tree making, without signals
```
root.exe "create_background_trees.C([set number], [year], [hist path])"
```
## Fit e+mu background distribution
Fits the background distribution with a Berstein poly, saves workspace
```
root.exe "fit_background([set number], [year], [add lum to weight])"
```
## Calculate UL
Runs a simple asymptotic CLs calculation of the upper limit
```
root.exe "calculate_UL.C([set number], [year])"
```
