#! /bin/bash

SELECTION=$1
HISTSTRING=$2
YEARSTRING=$3
TOTAL=$4
DONTCLEAN=$5
DOOBS=$6

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

WORKSPACE=combine_mva_${SELECTION}_${YEARSTRING}_workspace.root
text2workspace.py combine_mva_${SELECTION}_${HISTSTRING}_${YEARSTRING}.txt -o ${WORKSPACE}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doInitialFit ${DOOBS}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doFits ${DOOBS}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --output impacts_${SELECTION}_${HISTSTRING}_${YEARSTRING}.json ${DOOBS}
plotImpacts.py -i impacts_${SELECTION}_${HISTSTRING}_${YEARSTRING}.json -o impacts_${SELECTION}_${HISTSTRING}_${YEARSTRING}

# #check MC stats group impact
# combine -M MultiDimFit -n _paramFit_Test_mcstats --algo impact --redefineSignalPOIs r -P 'rgx{prop_bin*_*_bin*}' --floatOtherPOIs 1 --saveInactivePOI 1 --rMin -20 --rMax 20 --robustFit 1 -t -1 -m 0 -d ${WORKSPACE}

if [[ "${DONTCLEAN}" == "" ]]
then
    rm higgsCombine_*_Test*.MultiDimFit.mH0.root
fi
