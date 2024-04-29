#! /bin/bash
# Test the MC bias measurement

SET=$1
if [[ "${SET}" == "" ]]; then
    SET=13
fi

# Create the setup
root.exe -q -b test_bemu_mc_bias.C\(${SET}\)
[ ! -d figures ] && mkdir figures

# Generate toys with the histogram
combine -d bemu_mc_bias_hist.txt -n .bemu_mc_bias_${SET} -M GenerateOnly -t 1000 --expectSignal 0.0 --saveToys --genBinnedChannels lepm_13 --freezeParameters cat

TOYFILE="higgsCombine.bemu_mc_bias_${SET}.GenerateOnly.mH120.123456.root"
root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/convert_unbinned_to_binned.C(\"${TOYFILE}\", \"higgsCombine.bemu_mc_bias_${SET}_binned.GenerateOnly.mH120.123456.root\")"
TOYFILE="higgsCombine.bemu_mc_bias_${SET}_binned.GenerateOnly.mH120.123456.root"

# Fit the toys with a PDF
FITARGS="--cminDefaultMinimizerStrategy=0 --cminRunAllDiscreteCombinations --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
FITARGS="${FITARGS} --X-rtd MINIMIZER_freezeDisassociatedParams"
FITARGS="${FITARGS} --X-rtd MINIMIZER_multiMin_hideConstants --freezeParameters cat,var{chb_.*}"

#FitDiagnostics test
combine -d bemu_mc_bias_pdf.txt -n .bemu_mc_bias_${SET} -M FitDiagnostics -t 1000 ${FITARGS} --rMin -30 --rMax 30 --toysFile=${TOYFILE}
OUTFILE="fitDiagnostics.bemu_mc_bias_${SET}.root"
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fits.C(\"${OUTFILE}\", 0, \"figures/bias_bemu_mc_bias_${SET}_fitdiag\", 1, 0)"

#MultiDimFit test
combine -d bemu_mc_bias_pdf.txt -n .bemu_mc_bias_${SET} -M MultiDimFit --algo singles --cl=0.68 --saveNLL -t 1000 ${FITARGS} --rMin -20 --rMax 20 --toysFile=${TOYFILE}
OUTFILE="higgsCombine.bemu_mc_bias_${SET}.MultiDimFit.mH120.123456.root"
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_multidim_fits.C(\"${OUTFILE}\", 0, \"figures/bias_bemu_mc_bias_${SET}_multidim\")"

rm higgsCombine.bemu_mc_bias_${SET}.GenerateOnly.mH120.123456.root
rm higgsCombine.bemu_mc_bias_${SET}_binned.GenerateOnly.mH120.123456.root
rm fitDiagnostics.bemu_mc_bias_${SET}.root
rm higgsCombine.bemu_mc_bias_${SET}.FitDiagnostics.mH120.123456.root
rm higgsCombine.bemu_mc_bias_${SET}.MultiDimFit.mH120.123456.root
