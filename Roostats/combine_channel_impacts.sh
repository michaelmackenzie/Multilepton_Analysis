#! /bin/bash

SELECTION=$1
HISTSTRING=$2
DONTCLEAN=$3

WORKSPACE=combine_mva_total_${SELECTION}_workspace.root
# if [ ! -f ${WORKSPACE} ]
# then
#     echo "Workspace ${WORKSPACE} not found!"
#     pwd
#     ls
#     exit 1;
# fi

echo "Creating total search impacts"

text2workspace.py combine_mva_total_${SELECTION}_${HISTSTRING}.txt -o ${WORKSPACE}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doInitialFit -t -1
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doFits -t -1
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --output impacts_total_${SELECTION}_${HISTSTRING}.json -t -1
plotImpacts.py -i impacts_total_${SELECTION}_${HISTSTRING}.json -o impacts_total_${SELECTION}_${HISTSTRING}

echo "Creating channel specific impacts"
if [[ "${SELECTION}" == *"mutau*" ]]
then
    CHANNEL="${SELECTION}_e"
else
    CHANNEL="${SELECTION}_mu"
fi
echo "Running on channel ${CHANNEL}"
WORKSPACE=combine_mva_${CHANNEL}_workspace.root
text2workspace.py combine_mva_${CHANNEL}_${HISTSTRING}.txt -o ${WORKSPACE}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doInitialFit -t -1
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doFits -t -1
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --output impacts_${CHANNEL}_${HISTSTRING}.json -t -1
plotImpacts.py -i impacts_${CHANNEL}_${HISTSTRING}.json -o impacts_${CHANNEL}_${HISTSTRING}

CHANNEL=${SELECTION}
echo "Running on channel ${CHANNEL}"
WORKSPACE=combine_mva_${CHANNEL}_workspace.root
text2workspace.py combine_mva_${CHANNEL}_${HISTSTRING}.txt -o ${WORKSPACE}
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doInitialFit -t -1
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --doFits -t -1
combineTool.py -M Impacts -d ${WORKSPACE} -m 0 --rMin -20 --rMax 20 --robustFit 1 --output impacts_${CHANNEL}_${HISTSTRING}.json -t -1
plotImpacts.py -i impacts_${CHANNEL}_${HISTSTRING}.json -o impacts_${CHANNEL}_${HISTSTRING}

if [[ "${DONTCLEAN}" == "" ]]
then
    echo "Cleaning up work area of test files"
    rm higgsCombine*Test_*.root
fi
