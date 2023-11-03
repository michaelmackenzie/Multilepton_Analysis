#! /bin/bash

Help() {
    echo "Process BDT uncertainty factors"
    echo " 1: Selection list"
    echo " 2: Year list"
}

SELECTIONS=$1
YEARS=$2

if [[ "${SELECTIONS}" == "-h" ]] || [[ "${SELECTIONS}" == "--help" ]]; then
    Help
    exit
fi

if [[ "${SELECTIONS}" == "" ]]; then
    SELECTIONS="zmutau zetau zmutau_e zetau_mu"
fi

if [[ "${YEARS}" == "" ]]; then
    YEARS="2016 2017 2018"
fi

for YEAR in ${YEARS}; do
    echo "Processing year ${YEAR}"
    for SELECTION in ${SELECTIONS}; do
        echo "Processing selection ${SELECTION}"
        root.exe -q -b "scale_factors.C(\"${SELECTION}\", ${YEAR})"
    done
done
