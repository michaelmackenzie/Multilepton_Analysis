#! /bin/bash
# Process MC closure for a given Z->e+mu card

Help() {
    echo "Process Z->e+mu MC closure test:"
    echo "Assumes cards of the form \"combine_bemu_zemu_SETS.txt\" and \"combine_bemu_zemu_mc_SETS.txt\" exist"
    echo "1: Card name"
    echo "2: N(toys) (default = 1000)"
    echo "3: N(gen) per segment (default = 100)"
    echo "4: Additional arguments"
    echo "5: r range (default = 100)"
    echo "6: Base random seed (default = 90)"
}

CARD=$1
NTOYS=$2
NGENPERTOY=$3
ARGS=$4
RRANGE=$5
SEED=$6

if [[ "${CARD}" == "" ]] || [[ "${CARD}" == "--help" ]] || [[ "{CARD}" == "-h" ]]; then
    Help
    exit
fi

if [[ "${NTOYS}" == "" ]]; then
    NTOYS=1000
fi

if [[ "${NGENPERTOY}" == "" ]]; then
    NGENPERTOY=100
fi

if [[ "${RRANGE}" == "" ]]; then
    RRANGE="100"
fi

if [[ "${SEED}" == "" ]]; then
    SEED=90
fi

NAME=`echo ${CARD} | sed 's/combine_bemu_//' | sed 's/.txt//' | sed 's/_workspace.root//'`
echo ${NAME}
GENCARD=`echo ${CARD} | sed 's/zemu_/zemu_mc_/'`
echo ${GENCARD}

OUTNAME="${NAME}_mc_closure_test"

ARGS="${ARGS} --cminDefaultMinimizerStrategy=0 --cminRunAllDiscreteCombinations --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGS="${ARGS} --X-rtd MINIMIZER_freezeDisassociatedParams"
ARGS="${ARGS} --X-rtd MINIMIZER_multiMin_hideConstants --X-rtd MINIMIZER_multiMin_maskConstraints"

#Do the fits in increments of N(toys) to avoid fit failures
OUTPUTLIST=""
GENERATED=0
for ((IGEN=0; IGEN<=${NTOYS}; IGEN+=${NGENPERTOY}))
do
    SEED=$((SEED+NGENPERTOY))
    NGEN=${NGENPERTOY}
    # Generate toy MC data
    combine -d ${GENCARD} -M GenerateOnly --saveToys -t ${NGEN} -n .${OUTNAME} --genBinnedChannels lepm_13,lepm_12,lepm_11,lepm_10 -s ${SEED}

    # Fit the toy data
    combine -d ${CARD} ${ARGS} --rMin -${RRANGE} --rMax ${RRANGE} -M FitDiagnostics -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
    OUTPUTLIST="${OUTPUTLIST} fitDiagnostics.${OUTNAME}_${SEED}.root"
    GENERATED=$((GENERATED+NGEN))

    #Clean up the output
    rm higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root
    rm higgsCombine.${OUTNAME}_${SEED}.FitDiagnostics.mH120.${SEED}.root
done

#Merge the output fitDiagnostics files
${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "fitDiagnostics.${OUTNAME}.root" ${OUTPUTLIST}

#Clean up the output
rm ${OUTPUTLIST}

#Merge output files
echo "Creating bias plots..."
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/plot_combine_fits.C(\"fitDiagnostics.${OUTNAME}.root\", 0, \"bias_${OUTNAME}\", 0, 1)"
