#! /bin/bash

Help() {
    echo "Process combine impacts"
    echo "Options:"
    echo " 1: Selection"
    echo " 2: Hist set string"
    echo " 3: Year string"
    echo " 4: Do total flag"
    echo " 5: Don't clean flag"
    echo " 6: Do observed flag"
    echo " 7: r POI range"
    echo " 8: Force card name"
    echo " 9: Approximate impacts"
}

SELECTION=$1
HISTSTRING=$2
YEARSTRING=$3
TOTAL=$4
DONTCLEAN=$5
DOOBS=$6
RRANGE=$7
CARD=$8
APPROX=$9

if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]
then
    Help
    exit
fi

if [[ "${TOTAL}" != "" ]]
then
    SELECTION="total_${SELECTION}"
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

echo "Performing impacts on ${SELECTION} set ${HISTSTRING} and year ${YEARSTRING}"
echo "Parameters: DONTCLEAN=${DONTCLEAN}; DOOBS=${DOOBS}; RRANGE=${RRANGE}"

if [[ "${CARD}" != "" ]]
then
    echo "Using input datacard ${CARD}"
    WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
    JSON=`echo ${CARD} | sed 's/.txt/.json/' | sed 's/combine_/impacts_/'`
else
    CARD=combine_mva_${SELECTION}_${HISTSTRING}_${YEARSTRING}.txt
    WORKSPACE=combine_mva_${SELECTION}_${YEARSTRING}_workspace.root
    JSON=impacts_${SELECTION}_${HISTSTRING}_${YEARSTRING}.json
fi

ARGS=""
if [[ "${APPROX}" != "" ]]; then
    echo "-- Performing approximate impacts"
    ARGS="--approx robust"
fi

text2workspace.py ${CARD} -o ${WORKSPACE}
echo "Running: combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doInitialFit ${DOOBS}"
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doInitialFit ${DOOBS}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --doFits ${DOOBS} ${ARGS}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -${RRANGE} --rMax ${RRANGE} --robustFit 1 --output ${JSON} ${DOOBS} ${ARGS}
plotImpacts.py -i ${JSON} -o `echo ${JSON} | sed 's/.json//'`

# #check MC stats group impact
# combine -M MultiDimFit -n _paramFit_Test_mcstats --algo impact --redefineSignalPOIs r -P 'rgx{prop_bin*_*_bin*}' --floatOtherPOIs 1 --saveInactivePOI 1 --rMin -20 --rMax 20 --robustFit 1 -t -1 -m 0 -d ${WORKSPACE}

if [[ "${DONTCLEAN}" == "" ]]
then
    rm higgsCombine_*_Test*.MultiDimFit.mH0.root
fi
