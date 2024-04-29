#! /bin/bash
# Test generating with a histogram and fitting with a PDF

# Create the setup
root.exe -q -b test_hist_v_pdf_bias.C

# Generate toys with the histogram
combine -d hist_v_pdf_bias_hist.txt -n .hist_v_pdf_bias -M GenerateOnly -t 1000 --expectSignal 0.0 --saveToys --genBinnedChannels mass

TOYFILE="higgsCombine.hist_v_pdf_bias.GenerateOnly.mH120.123456.root"
root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/convert_unbinned_to_binned.C(\"${TOYFILE}\", \"higgsCombine.hist_v_pdf_bias_binned.GenerateOnly.mH120.123456.root\")"
TOYFILE="higgsCombine.hist_v_pdf_bias_binned.GenerateOnly.mH120.123456.root"

# Fit the toys with a PDF
FITARGS="--cminDefaultMinimizerStrategy=0 --cminRunAllDiscreteCombinations --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
FITARGS="${FITARGS} --X-rtd MINIMIZER_freezeDisassociatedParams"
FITARGS="${FITARGS} --X-rtd MINIMIZER_multiMin_hideConstants"

#FitDiagnostics test
combine -d hist_v_pdf_bias_pdf.txt -n .hist_v_pdf_bias -M FitDiagnostics -t 1000 ${FITARGS} --rMin -20 --rMax 20 --toysFile=${TOYFILE}
OUTFILE="fitDiagnostics.hist_v_pdf_bias.root"
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fits.C(\"${OUTFILE}\", 0, \"bias_hist_v_pdf_bias_fitdiag\", 1, 0)"

#MultiDimFit test
combine -d hist_v_pdf_bias_pdf.txt -n .hist_v_pdf_bias -M MultiDimFit --algo singles --cl=0.68 --saveNLL -t 1000 ${FITARGS} --rMin -20 --rMax 20 --toysFile=${TOYFILE}
OUTFILE="higgsCombine.hist_v_pdf_bias.MultiDimFit.mH120.123456.root"
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_multidim_fits.C(\"${OUTFILE}\", 0, \"bias_hist_v_pdf_bias_multidim\")"
