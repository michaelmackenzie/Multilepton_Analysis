#! /bin/bash

Help() {
    echo "Process combine impacts"
    echo "Options:"
    echo " 1: Force card name"
    echo " 2: Don't clean flag"
    echo " 3: Do observed flag"
    echo " 4: r POI range"
    echo " 5: Approximate impacts"
}

CARD=$1
DONTCLEAN=$2
DOOBS=$3
RRANGE=$4
APPROX=$5

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

WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
JSON=`echo ${CARD} | sed 's/.txt/.json/' | sed 's/combine_/impacts_/'`

ARGS=""
if [[ "${APPROX}" != "" ]]; then
    echo "-- Performing approximate impacts"
    ARGS="--approx robust"
fi

text2workspace.py ${CARD} -o ${WORKSPACE}
if [[ "${APPROX}" == "" ]]; then
    echo "Running: combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doInitialFit ${DOOBS}"
    combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doInitialFit ${DOOBS}
fi
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doFits ${DOOBS} ${ARGS}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --output ${JSON} ${DOOBS} ${ARGS}
plotImpacts.py -i ${JSON} -o `echo ${JSON} | sed 's/.json//'`

if [[ "${DONTCLEAN}" == "" ]]
then
    rm higgsCombine_*_Test*.MultiDimFit.mH0.root
fi
