#! /bin/bash

Help() {
    echo "Process combine impacts"
    echo "Options:"
    echo " 1: Card name"
    echo " 2: Don't clean flag"
    echo " 3: Do observed flag"
    echo " 4: r POI range"
    echo " 5: Approximate impacts"
    echo " 6: Argument for combineTool.py"
    echo " 7: Argument for plot tool"
    echo " 8: Mu2e"
}

CARD=$1
DONTCLEAN=$2
DOOBS=$3
RRANGE=$4
APPROX=$5
COMMAND=$6
PLOTARG=$7
MU2E=$8

if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]
then
    Help
    exit
fi

if [[ "${CARD}" == "" ]]
then
    echo "No combine card given!"
    Help
    exit
fi

if [ ! -f ${CARD} ]
then
    echo "Combine card ${CARD} not found"
    Help
    exit
fi

if [[ "${DOOBS}" == "" ]]
then
    DOOBS="-t -1"
else
    DOOBS=""
fi

if [[ "${RRANGE}" == "" ]]
then
    RRANGE=20
fi

echo "Performing impacts on combine card ${CARD}"
echo "Parameters: DONTCLEAN=${DONTCLEAN}; DOOBS=${DOOBS}; RRANGE=${RRANGE}"
if [[ "${MU2E}" != "" ]]; then
    echo "Using Mu2e parameters"
fi

WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
JSON=`echo ${CARD} | sed 's/.txt/.json/' | sed 's/combine_/impacts_/'`

ARGS=""
if [[ "${APPROX}" != "" ]]; then
    echo "-- Performing approximate impacts"
    ARGS="--approx robust"
fi
if [[ "${COMMAND}" != "" ]]; then
    echo "Adding argument ${COMMAND} to the evaluation"
    ARGS="${ARGS} ${COMMAND}"
fi

text2workspace.py ${CARD} -o ${WORKSPACE}
if [[ "${APPROX}" == "" ]]; then
    echo "Running: combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doInitialFit ${DOOBS}"
    combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doInitialFit ${DOOBS}
fi
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doFits ${DOOBS} ${ARGS}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --output ${JSON} ${DOOBS} ${ARGS}
if [[ "${MU2E}" != "" ]]; then
    PDF=`echo ${JSON} | sed 's/.json//'`
    plotImpacts.py -i ${JSON} -o ${PDF} ${PLOTARG} --experiment Mu2e --cms-label " Internal"
    pdftoppm ${PDF}.pdf ${PDF} -png
else
    plotImpacts.py -i ${JSON} -o `echo ${JSON} | sed 's/.json//'` ${PLOTARG}
fi

if [[ "${DONTCLEAN}" == "" ]]
then
    rm higgsCombine_*_Test*.MultiDimFit.mH0.root
fi
