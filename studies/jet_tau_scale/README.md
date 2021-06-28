# Jet to tau scale factor calculations
The scale factors are measured in DM, eta, and year bins as a function of tau pT.
These are measured using a data-driven method to estimate the tight tau ID region
using a loose region orthogonal to the tight region.

## Scale factors
root.exe -q -b "scale_factors.C(\"[selection]\", \"[process name]\", [tight set], [loose set], [year], \"[histogram path]\")"

Process: tight set, loose set
QCD  : 1030, 3030
WJets:   31, 2031 
Top  :   32, 2032

## Closure correction
The scale factors fail to accurately reproduce some event kinematics, so
a non-closure correction is applied to attempt to address this.

root.exe -q -b "test_data_scales.C(\"[selection]\", \"[process name]\", [tight set], [loose set], [year], \"[histogram path]\")"
Process: tight set, loose set
QCD  : 1030, 3030
WJets:   31, 2031
Top  :   32, 2032

## Composition
The control region has a mixture of the different background processes, which
have different scale factors due to the underlying processes generating the
tau candidate. This composition is broken down as a function of different
kinematic distributions, where the final j->tau scale is considered the
sum of each process' scale factor multiplied by the expected fraction of that
process in the control region.

root.exe -q -b "composition.C(\"[selection]\", [set with loose MC fake taus for non-QCD processes], [set with loose taus, true and MC estimated for QCD], [year], \"[histogram path]\")"
