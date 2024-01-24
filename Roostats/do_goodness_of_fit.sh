#! /bin/bash

# Do goodness of fit studies on a given signal

SIGNAL=$1
SKIPWORKSPACE=$2
YEAR=$3
HAD_SET=$4

if [[ "${SIGNAL}" == "" ]]; then
    echo "No signal given! Allowed are zmutau or zetau"
    exit
fi

if [[ "${SIGNAL}" != "zmutau" ]] && [[ "${SIGNAL}" != "zetau" ]]; then
    echo "Unknown signal ${SIGNAL} given! Allowed are zmutau or zetau"
    exit
fi

if [[ "${YEAR}" == "" ]]; then
    YEAR="2016_2017_2018"
fi

if [[ "${HAD_SET}" == "" ]]; then
    HAD_SET="25_26_27_28"
fi

echo "Using signal ${SIGNAL} with year ${YEAR}, hadronic set ${HAD_SET} for the studies"

# Create a workspace for the hadronic channel

CARD_HAD="combine_mva_${SIGNAL}_${HAD_SET}_${YEAR}.txt"
WORK_HAD="combine_mva_${SIGNAL}_${HAD_SET}_${YEAR}_workspace.root"
if [[ "${SKIPWORKSPACE}" == "" ]]; then
    text2workspace.py ${CARD_HAD} --channel-masks -o ${WORK_HAD}
fi

echo "Created workspace ${WORK_HAD}"

echo "Performing asymptotic limits"
combine -d ${WORK_HAD} --cminDefaultMinimizerStrategy 0 --rMin -500 --rMax 500

echo "Performing saturated goodness of fit calculation"
combine -M GoodnessOfFit --algo=saturated -d ${WORK_HAD} --cminDefaultMinimizerStrategy 0 --rMin -500 --rMax 500

echo "Performing KS goodness of fit calculation"
combine -M GoodnessOfFit --algo=KS -d ${WORK_HAD} --cminDefaultMinimizerStrategy 0 --rMin -500 --rMax 500

echo "Performing AD goodness of fit calculation"
combine -M GoodnessOfFit --algo=AD -d ${WORK_HAD} --cminDefaultMinimizerStrategy 0 --rMin -500 --rMax 500

echo "Creating approximate impacts"
../../impacts_mva.sh "${WORK_HAD}" "" "d" "500" "d"
