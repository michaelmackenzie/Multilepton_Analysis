#! /bin/bash
# Generate Z->emu events with one card, then fit with another

Help() {
    echo "Process Z->e+mu closure test:"
    echo " Usage: bemu_bias.sh --card_1 --card_2 [options]"
    echo "--card_1      : Card for generation"
    echo "--card_2      : Card for generation"
    echo "--name    (-n): Test name (default taken from gen card)"
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
    echo "--dryrun      : Don't execute commands"
}

CARD_GEN=""
CARD_FIT=""
NTOYS="1000"
NGENPERTOY="100"
FITARG=""
GENARG=""
NAME=""
TAG=""
RRANGE="100"
SEED="90"
SKIPFITS=""
MULTIDIM=""
GRID=""
DONTCLEAN=""
HEAD=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--card_1" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENCARD=${var}
    elif [[ "${var}" == "--card_2" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        FITCARD=${var}
    elif [[ "${var}" == "--name" ]] || [[ "${var}" == "-n" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NAME=${var}
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
        HEAD="echo"
    else
        echo "Arguments aren't configured correctly!"
        Help
        exit
    fi
    iarg=$((iarg + 1))
done

if [[ "${GENCARD}" == "" ]] || [[ "${FITCARD}" == "" ]]; then
    Help
    exit
fi

if [[ "${NAME}" == "" ]]; then
    echo "Creating test name from gen card ${GENCARD}..."
    NAME=`echo ${GENCARD} | sed 's/combine_bemu_//' | sed 's/.txt//' | sed 's/_workspace.root//'`
fi

OUTNAME="${NAME}_test"
echo "Using output naming tag ${OUTNAME}"

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
        ${HEAD} combine -d ${GENCARD} -M GenerateOnly --saveToys -t ${NGEN} -n .${OUTNAME} --genBinnedChannels lepm_13,lepm_12,lepm_11,lepm_10 -s ${SEED} ${GENARG}

        # Create binned data to fit
        ${HEAD} root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/convert_unbinned_to_binned.C(\"higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root\", \"higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root\")"

        # Fit the toy data
        if [[ "${MULTIDIM}" == "" ]]; then
            ${HEAD} combine -d ${FITCARD} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE} -M FitDiagnostics -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} fitDiagnostics.${OUTNAME}_${SEED}.root"
        else
            ${HEAD} combine -d ${FITCARD} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE} -M MultiDimFit --algo ${ALGO} --saveFitResult --saveNLL -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} higgsCombine.${OUTNAME}_${SEED}.MultiDimFit.mH120.${SEED}.root"
            #cleanup fit file not used
            if [[ "${DONTCLEAN}" == "" ]]; then
                ${HEAD} rm multidimfit.${OUTNAME}_${SEED}.root
            fi
        fi
        GENERATED=$((GENERATED+NGEN))

        #Clean up the output
        if [[ "${DONTCLEAN}" == "" ]]; then
            ${HEAD} rm higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root
            ${HEAD} rm higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root
            ${HEAD} rm higgsCombine.${OUTNAME}_${SEED}.FitDiagnostics.mH120.${SEED}.root
        fi
    done

    #Merge the output fitDiagnostics files
    if [[ "${MULTIDIM}" == "" ]]; then
        ${HEAD} ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "fitDiagnostics.${OUTNAME}${TAG}.root" ${OUTPUTLIST}
    else
        ${HEAD} ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root" ${OUTPUTLIST}
    fi

    #Clean up the output
    if [[ "${DONTCLEAN}" == "" ]]; then
        ${HEAD} rm ${OUTPUTLIST}
    fi
fi

#Create the bias plots
OUTFILE="fitDiagnostics.${OUTNAME}${TAG}.root"
if [[ "${MULTIDIM}" == "d" ]]; then
    OUTFILE="higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root"
fi
${HEAD} ls -l ${OUTFILE}

echo "Creating bias plots..."
if [[ "${MULTIDIM}" == "" ]]; then
    ${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fits.C(\"${OUTFILE}\", 0, \"bias_${OUTNAME}${TAG}\", 2, 0)"
else
    ${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_multidim_fits.C(\"${OUTFILE}\", 0., \"bias_${OUTNAME}${TAG}\")"
fi

echo "Creating plots of all fit params..."
${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fit_params.C(\"${OUTFILE}\", \"figures/bias_${OUTNAME}${TAG}\")"
