#! /bin/bash
# Process MC closure for a given Z->e+mu card

Help() {
    echo "Process Z->e+mu MC closure test:"
    echo " Usage: bemu_mc_bias.sh <card> [options]"
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
    echo "--nozmumu     : Use a MC template without Z->mumu for generation"
    echo "--dontclean   : Don't clean up temporary files"
    echo "--dryrun      : Only print commands"
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
NOZMUMUM=""
DONTCLEAN=""
DRYRUN=""

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
    elif [[ "${var}" == "--nozmumu" ]]
    then
        NOZMUMU="d"
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
GENCARD=`echo ${CARD} | sed 's/zemu_/zemu_mc_/'`
if [[ "${NOZMUMU}" != "" ]]; then
    GENCARD=`echo ${GENCARD} | sed 's/mc_/mc_no_zmumu_/'`
fi
echo ${GENCARD}

#Check that the gen/fit workspaces/cards exist
if [ ! -f ${CARD} ]; then
    echo "Fit card ${CARD} not found"
    exit
fi
if [ ! -f ${GENCARD} ]; then
    echo "Gen card ${GENCARD} not found"
    exit
fi

OUTNAME="${NAME}_mc_closure_test"

ARGS="${ARGS} --cminDefaultMinimizerStrategy=0 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGS="${ARGS} --X-rtd MINIMIZER_freezeDisassociatedParams"
ARGS="${ARGS} --X-rtd MINIMIZER_multiMin_hideConstants"
# ARGS="${ARGS}  --cminRunAllDiscreteCombinations"
ARGS="${ARGS} --stepSize 0.05 --setRobustFitTolerance 0.001 --setCrossingTolerance 5e-6 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1"

FITARG="${ARGS} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE}"
GENARG="${GENARG}"
ALGO="singles --cl=0.68"
if [[ "${GRID}" == "d" ]]; then
    ALGO="grid --points 3 --alignEdges 1"
fi

HEAD=""
if [[ "${DRYRUN}" != "" ]]; then
    HEAD="echo"
fi

#Do the fits in increments of N(toys) to avoid fit failures
if [[ "${SKIPFITS}" == "" ]]; then
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
            ${HEAD} combine -d ${CARD} ${FITARG} -M FitDiagnostics -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} fitDiagnostics.${OUTNAME}_${SEED}.root"
        else
            ${HEAD} combine -d ${CARD} ${FITARG} -M MultiDimFit --algo ${ALGO} --saveFitResult --saveNLL -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} higgsCombine.${OUTNAME}_${SEED}.MultiDimFit.mH120.${SEED}.root"
            #cleanup fit file not used
            if [[ "${DONTCLEAN}" == "" ]]; then
                ${HEAD} rm multidimfit.${OUTNAME}_${SEED}.root
            fi
        fi
        # combine -d ${CARD} ${ARGS} --rMin -${RRANGE} --rMax ${RRANGE} -M FitDiagnostics -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
        # OUTPUTLIST="${OUTPUTLIST} fitDiagnostics.${OUTNAME}_${SEED}.root"

        #Increment N(toys generated)
        GENERATED=$((GENERATED+NGEN))

        #Clean up the output
        if [[ "${DONTCLEAN}" == "" ]]; then
            ${HEAD} rm higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root
            ${HEAD} rm higgsCombine.${OUTNAME}_binned.GenerateOnly.mH120.${SEED}.root
            ${HEAD} rm higgsCombine.${OUTNAME}_${SEED}.FitDiagnostics.mH120.${SEED}.root
        fi
    done

    #Merge the output files
    if [[ "${MULTIDIM}" == "" ]]; then
        ${HEAD} ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "fitDiagnostics.${OUTNAME}${TAG}.root" ${OUTPUTLIST}
    else
        ${HEAD} ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py "higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root" ${OUTPUTLIST}
    fi

    #Clean up the output
    ${HEAD} rm ${OUTPUTLIST}
fi


echo "Creating bias plots..."

OUTFILE="fitDiagnostics.${OUTNAME}${TAG}.root"
if [[ "${MULTIDIM}" == "d" ]]; then
    OUTFILE="higgsCombine.${OUTNAME}${TAG}.MultiDimFit.root"
fi
${HEAD} ls -l ${OUTFILE}

if [[ "${MULTIDIM}" == "" ]]; then
    ${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fits.C(\"${OUTFILE}\", 0, \"bias_${OUTNAME}${TAG}\", 1, 0)"
else
    ${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_multidim_fits.C(\"${OUTFILE}\", 0., \"bias_${OUTNAME}${TAG}\")"
fi

echo "Creating plots of all fit params..."
${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fit_params.C(\"${OUTFILE}\", \"figures/bias_${OUTNAME}${TAG}\")"
