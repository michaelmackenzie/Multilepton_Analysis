#! /bin/bash
# Generate events with one card, then fit with another

Help() {
    echo "Process a generation vs. fitting model bias test:"
    echo " Usage: mva_gen_fit_test.sh --card_gen <gen_card> --card_fit <fit_card> [options]"
    echo "--card_fit    : Card for generation"
    echo "--card_gen    : Card for fitting"
    echo "--name    (-n): Test name (default taken from gen card)"
    echo "--toys    (-t): N(toys) (default = 100)"
    echo "--gentoys (-g): N(gen) per segment (default = 100)"
    echo "--fitarg      : Additional fit arguments"
    echo "--genarg      : Additional generator arguments"
    echo "--tag         : Name tag for output"
    echo "--rrange  (-r): r range (default = 100)"
    echo "--seed    (-s): Base random seed (default = 90)"
    echo "--skipfits    : Skip fit loops, only create plots"
    echo "--multidim    : Use the MultiDimFit method"
    echo "--dontclean   : Don't clean up temporary files"
    echo "--dryrun      : Don't execute commands"
}

CARD_GEN=""
CARD_FIT=""
NTOYS="100"
NGENPERTOY="100"
FITARG=""
GENARG=""
NAME=""
TAG=""
RRANGE="100"
SEED="90"
SKIPFITS=""
MULTIDIM=""
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
    elif [[ "${var}" == "--card_gen" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENCARD=${var}
    elif [[ "${var}" == "--card_fit" ]]
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
    NAME=`echo ${GENCARD} | sed 's/combine_mva_//' | sed 's/.txt//' | sed 's/_workspace.root//'`
fi

OUTNAME="${NAME}"
echo "Using output naming tag ${OUTNAME}"

# Create workspaces if needed

if [[ "${GENCARD}" == *".txt" ]]; then
    WORKSPACE=`echo ${GENCARD} | sed 's/.txt/_workspace.root/'`
    echo "Creating gen card workspace..."
    ${HEAD} text2workspace.py ${GENCARD} -o ${WORKSPACE}
    GENCARD=${WORKSPACE}
fi
if [[ "${FITCARD}" == *".txt" ]]; then
    WORKSPACE=`echo ${FITCARD} | sed 's/.txt/_workspace.root/'`
    echo "Creating fit card workspace..."
    ${HEAD} text2workspace.py ${FITCARD} -o ${WORKSPACE}
    FITCARD=${WORKSPACE}
fi

# Setup the Combine arguments

ARGS="${ARGS} --cminDefaultMinimizerStrategy=0 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1"

FITARG="${ARGS} ${FITARG}"
# GENARG="${ARGS} ${GENARG} --toysFreq"
GENARG="${ARGS} ${GENARG}"
ALGO="singles --cl=0.68"

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
        ${HEAD} combine -d ${GENCARD} -M GenerateOnly --saveToys -t ${NGEN} -n .${OUTNAME} -s ${SEED} ${GENARG}

        # Fit the toy data
        if [[ "${MULTIDIM}" == "" ]]; then
            ${HEAD} combine -d ${FITCARD} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE} -M FitDiagnostics -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
            OUTPUTLIST="${OUTPUTLIST} fitDiagnostics.${OUTNAME}_${SEED}.root"
            if [[ "${DONTCLEAN}" == "" ]]; then
                ${HEAD} rm higgsCombine.${OUTNAME}_${SEED}.FitDiagnostics.mH120.${SEED}.root
            fi
        else
            ${HEAD} combine -d ${FITCARD} ${FITARG} --rMin -${RRANGE} --rMax ${RRANGE} -M MultiDimFit --algo ${ALGO} --saveFitResult --saveNLL -t ${NGEN} --toysFile=higgsCombine.${OUTNAME}.GenerateOnly.mH120.${SEED}.root -n .${OUTNAME}_${SEED} -s ${SEED}
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

# echo "Creating plots of all fit params..."
# ${HEAD} root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_combine_fit_params.C(\"${OUTFILE}\", \"figures/bias_${OUTNAME}${TAG}\")"
