#! /bin/bash
# Do significance tests

Help() {
    echo "Do significance tests"
    echo "Usage: significance.sh <card> [options]"
    echo "Options:"
    echo " --rrange    (-r ): POI range (default = 30)"
    echo " --fitarg         : Additional fit arguments"
    echo " --skipworkspace  : Don't recreate workspace for input data card"
    echo " --plotonly       : Only make plots"
    echo " --tag            : Tag for output results"
    echo " --help      (-h ): Print this information"
}

CARD=""
RRANGE="30"
FITARG=""
SKIPWORKSPACE=""
PLOTONLY=""
TAG="Test"

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]; then
        Help
        exit
    elif [[ "${var}" == "--rrange" ]] || [[ "${var}" == "-r" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        RRANGE=${var}
    elif [[ "${var}" == "--fitarg" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        FITARG=${var}
    elif [[ "${var}" == "--tag" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--skipworkspace" ]]; then
        SKIPWORKSPACE="d"
    elif [[ "${var}" == "--plotonly" ]]; then
        PLOTONLY="d"
    elif [[ "${CARD}" != "" ]]; then
        echo "Arguments aren't configured correctly! (at ${var}, CARD=${CARD})"
        Help
        exit
    else
        CARD=${var}
    fi
    iarg=$((iarg + 1))
done

if [[ "${CARD}" == "" ]]; then
    echo "No card given!"
    exit
fi

echo "Using card ${CARD} for the studies with TAG=${TAG}"
TAG="_${TAG}"

# Create a workspace if needed

WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
if [[ "${SKIPWORKSPACE}" == "" ]] && [[ "${CARD}" == *".txt" ]]; then
    text2workspace.py ${CARD} --channel-masks -o ${WORKSPACE}
    echo "Created workspace ${WORKSPACE}"
fi

# FITARG="${FITARG} --cminDefaultMinimizerStrategy 0 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --rMin -${RRANGE} --rMax ${RRANGE}"
FITARG="${FITARG} --cminDefaultMinimizerStrategy 0 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --rMin 0 --rMax ${RRANGE}"

if [[ "${PLOTONLY}" == "" ]]; then
    echo "Performing significance calculation"
    echo ">>> combine -M Significance -d ${WORKSPACE} ${FITARG} -n ${TAG}"
    combine -M Significance -d ${WORKSPACE} ${FITARG} -n ${TAG}
fi

FILE="higgsCombine${TAG}.Significance.mH120.root"
if [ ! -f ${FILE} ]; then
    echo "File ${FILE} not found!"
    exit
fi

echo "Finished running significance tests"
