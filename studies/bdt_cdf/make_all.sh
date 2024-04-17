#! /bin/bash

Help() {
    echo "Process all CDF transformations"
    echo "1: Selections"
    echo "2: Histogram path"
    echo "3: Set base index"
}

if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
    Help
    exit
fi
SELECTIONS=$1
HIST=$2
SET=$3

if [[ "${SELECTIONS}" == "" ]]; then
    SELECTIONS="zmutau zmutau_e zetau zetau_mu"
fi

if [[ "${HIST}" == "" ]]; then
    HIST="nanoaods_cdf"
fi

if [[ "${SET}" == "" ]]; then
    SET=8
fi

for SELEC in ${SELECTIONS}
do
    root.exe -q -b "study.C(\"${SELEC}\", ${SET}, \"${HIST}\")"
done
