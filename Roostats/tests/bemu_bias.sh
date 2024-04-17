#! /bin/bash
# Process a closure for a given Z->e+mu card

Help() {
    echo "Process Z->e+mu closure test:"
    echo " Usage: bemu_bias.sh <card> [options]"
    echo "--toys    (-t): N(toys) (default = 1000)"
    echo "--gentoys (-g): N(gen) per segment (default = 100)"
    echo "--fitarg      : Additional fit arguments"
    echo "--genarg      : Additional generator arguments"
    echo "--tag         : Name tag for output"
    echo "--rrange  (-r): r range (default = 100)"
    echo "--seed    (-s): Base random seed (default = 90)"
    echo "--skipfits    : Skip fit loops, only create plots"
    echo "--multidim    : Use the MultiDimFit method"
    echo "--grid        : Use a grid scan with MultiDimFit"
    echo "--dontclean   : Don't clean up temporary files"
}

CARD=""
NTOYS="1000"
NGENPERTOY="100"
FITARG=""
GENARG=""
TAG=""
RRANGE="100"
SEED="90"
SKIPFITS=""
MULTIDIM=""
GRID=""
DONTCLEAN=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--rrange" ]] || [[ "${var}" == "-r" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        RRANGE=${var}
    elif [[ "${var}" == "--toys" ]] || [[ "${var}" == "-t" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NTOYS=${var}
    elif [[ "${var}" == "--gentoys" ]] || [[ "${var}" == "-g" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NGENPERTOY=${var}
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--fitarg" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        FITARG=${var}
    elif [[ "${var}" == "--genarg" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENARG=${var}
    elif [[ "${var}" == "--seed" ]] || [[ "${var}" == "-s" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SEED=${var}
    elif [[ "${var}" == "--skipfits" ]]
    then
        SKIPFITS="d"
    elif [[ "${var}" == "--multidim" ]]
    then
        MULTIDIM="d"
    elif [[ "${var}" == "--grid" ]]
    then
        GRID="d"
    elif [[ "${var}" == "--dontclean" ]]
    then
        DONTCLEAN="d"
    elif [[ "${var}" == "--dryrun" ]]
    then
        DRYRUN="d"
    elif [[ "${CARD}" != "" ]]
    then
        echo "Arguments aren't configured correctly!"
        Help
        exit
    else
        CARD=${var}
    fi
    iarg=$((iarg + 1))
done

if [[ "${CARD}" == "" ]]; then
    Help
    exit
fi

NAME=`echo ${CARD} | sed 's/combine_bemu_//' | sed 's/.txt//' | sed 's/_workspace.root//'`
echo ${NAME}
GENCARD=${CARD}
echo ${GENCARD}

OUTNAME="${NAME}_closure_test"

ARGS="${ARGS} --cminDefaultMinimizerStrategy=0 --cminRunAllDiscreteCombinations --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGS="${ARGS} --X-rtd MINIMIZER_freezeDisassociatedParams"
ARGS="${ARGS} --X-rtd MINIMIZER_multiMin_hideConstants"

FITARG="${ARGS} ${FITARG}"
GENARG="${ARGS} ${GENARG}"
ALGO="singles --cl=0.68"
if [[ "${GRID}" == "d" ]]; then
    ALGO="grid --points 3 --alignEdges 1"
fi

if [[ "${SKIPFITS}" == "" ]]; then
    #Do the fits in increments of N(toys) to avoid fit failures
    OUTPUTLIST=""
    GENERATED=0
    if [ ${NTOYS} -lt ${NGENPERTOY} ]; then
        echo "Setting N(gen per toy) to ${NTOYS}"
        NGENPERTOY=${NTOYS}
    fi
    for ((IGEN=0; IGEN<${NTOYS}; IGEN+=${NGENPERTOY}))
    do
        SEED=$((SEED+NGENPERTOY))
        NGEN=${NGENPERTOY}
        # Generate toy MC data
        combine -d ${GENCARD} -M GenerateOnly --saveToys -t ${NGEN} -n .${OUTNAME} --genBinnedChannels lepm_13,lepm_12,lepm_11,lepm_10 -s ${SEED} ${GENARG}

        # Create binned data to fit
        time root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/convert_unbinned_to_binned.C(\"higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root\", \"higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root\")"

        # Fit the toy data
        if [[ "${MULTIDIM}" == "" ]]; then
            combine -d ${CARD} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE} -M FitDiagnostics -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} fitDiagnostics.${OUTNAME}_${SEED}.root"
        else
            combine -d ${CARD} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE} -M MultiDimFit --algo ${ALGO} --saveFitResult --saveNLL -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} higgsCombine.${OUTNAME}_${SEED}.MultiDimFit.mH120.${SEED}.root"
            #cleanup fit file not used
            if [[ "${DONTCLEAN}" == "" ]]; then
                rm multidimfit.${OUTNAME}_${SEED}.root
            fi
        fi
        GENERATED=$((GENERATED+NGEN))

        #Clean up the output
        if [[ "${DONTCLEAN}" == "" ]]; then
            rm higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root
            rm higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root
            rm higgsCombine.${OUTNAME}_${SEED}.FitDiagnostics.mH120.${SEED}.root
        fi
    done

    #Merge the output fitDiagnostics files
    if [[ "${MULTIDIM}" == "" ]]; then
        echo ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "fitDiagnostics.${OUTNAME}${TAG}.root" ${OUTPUTLIST}
        ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "fitDiagnostics.${OUTNAME}${TAG}.root" ${OUTPUTLIST}
    else
        echo ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root" ${OUTPUTLIST}
        ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root" ${OUTPUTLIST}
    fi

    #Clean up the output
    if [[ "${DONTCLEAN}" == "" ]]; then
        rm ${OUTPUTLIST}
    fi
fi

#Create the bias plots
OUTFILE="fitDiagnostics.${OUTNAME}${TAG}.root"
if [[ "${MULTIDIM}" == "d" ]]; then
    OUTFILE="higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root"
fi
ls -l ${OUTFILE}

echo "Creating bias plots..."
if [[ "${MULTIDIM}" == "" ]]; then
    root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fits.C(\"${OUTFILE}\", 0, \"bias_${OUTNAME}${TAG}\", 2, 0)"
else
    root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_multidim_fits.C(\"${OUTFILE}\", 0., \"bias_${OUTNAME}${TAG}\")"
fi

echo "Creating plots of all fit params..."
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fit_params.C(\"${OUTFILE}\", \"figures/bias_${OUTNAME}${TAG}\")"
