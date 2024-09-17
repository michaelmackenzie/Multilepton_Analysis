#! /bin/bash
# Create a likelihood scan of two parameters

Help() {
    echo "Create a likelihood scan of two parameters"
    echo " 1: Input card/workspace"
    echo " 2: Parameter 1 to scan"
    echo " 3: Parameter 2 to scan"
    echo " 4: Fit arguments"
    echo " 5: N(points) (default = 50)"
    echo " 6: Plot only"
}

CARD=$1
PARA1=$2
PARA2=$3
ARGS=$4
POINTS=$5
PLOTONLY=$6

if [[ "${CARD}" == "-h" ]] || [[ "${CARD}" == "--help" ]] || [[ "${CARD}" == "" ]]; then
    Help
    exit
fi

if [[ "${PARA1}" == "" ]] || [[ "${PARA2}" == "" ]]; then
    echo "Two parameter inputs required (given ${PARA1} and ${PARA2})"
    Help
    exit
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
    echo ">>> combine -d ${CARD} -M MultiDimFit --saveNLL --algo grid -P ${PARA1} -P ${PARA2} --redefineSignalPOIs r --floatOtherPOIs 1 --points ${POINTS} --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --cminDefaultMinimizerStrategy 0 ${ARGS} -n .2d_scan"
    combine -d ${CARD} -M MultiDimFit --saveNLL --algo grid -P ${PARA1} -P ${PARA2} --redefineSignalPOIs r --floatOtherPOIs 1 --points ${POINTS} --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --cminDefaultMinimizerStrategy 0 --alignEdges 1 ${ARGS} -n .2d_scan
fi

FILE="higgsCombine.2d_scan.MultiDimFit.mH120.root"
if [[ "${SEED}" != "" ]]; then
    FILE="higgsCombine.2d_scan.MultiDimFit.mH120.${SEED}.root"
fi


if [ ! -f ${FILE} ]; then
    echo "File ${FILE} not found!"
    exit
fi

python ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_2D_scan.py ${FILE} ${PARA1} ${PARA2}
# root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_2d_scan.C(\"${FILE}\", \"${PARA1}\", \"${PARA2}\", \"\", 0)"
