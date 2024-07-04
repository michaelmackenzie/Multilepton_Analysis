#! /bin/bash
# Create a likelihood scan of a single parameter

Help() {
    echo "Create a likelihood scan of a single parameter"
    echo " 1: Input card/workspace"
    echo " 2: Parameter to scan"
    echo " 3: Fit arguments"
    echo " 4: N(points) (default = 50)"
    echo " 5: Plot only"
    echo " 6: Random seed"
}

CARD=$1
PARA=$2
ARGS=$3
POINTS=$4
PLOTONLY=$5
SEED=$6

if [[ "${CARD}" == "-h" ]] || [[ "${CARD}" == "--help" ]] || [[ "${CARD}" == "" ]]; then
    Help
    exit
fi

if [[ "${PARA}" == "" ]]; then
    PARA="r"
fi

if [[ "${POINTS}" == "" ]]; then
    POINTS="50"
fi

if [ ! -f ${CARD} ]; then
    echo "Card ${CARD} not found!"
    exit
fi

if [[ "${PLOTONLY}" == "" ]]; then
    echo "Scanning parameter ${PARA} using card ${CARD}"
    if [[ "${SEED}" != "" ]]; then
        ARGS="${ARGS} -s ${SEED}"
    fi
    echo ">>> combine -d ${CARD} -M MultiDimFit --saveNLL --algo grid -P ${PARA} --redefineSignalPOIs r --floatOtherPOIs 1 --points ${POINTS} ${ARGS}"
    combine -d ${CARD} -M MultiDimFit --saveNLL --algo grid -P ${PARA} --redefineSignalPOIs r --floatOtherPOIs 1 --points ${POINTS} ${ARGS}
fi

FILE="higgsCombineTest.MultiDimFit.mH120.root"
if [[ "${SEED}" != "" ]]; then
    FILE="higgsCombineTest.MultiDimFit.mH120.${SEED}.root"
fi


if [ ! -f ${FILE} ]; then
    echo "File ${FILE} not found!"
    exit
fi

root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_scan.C(\"${FILE}\", \"${PARA}\", \"\", 10)"
