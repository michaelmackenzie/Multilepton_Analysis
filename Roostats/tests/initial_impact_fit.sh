#! /bin/bash
# Process the impact fit step for a single parameter

Help() {
    echo "Process the impact fit step for a single parameter"
    echo " 1: Input card/workspace"
    echo " 2: Name"
    echo " 3: Additional arguments"
    echo " 4: r range (default = 30)"
}

CARD=$1
NAME=$2
FITARG=$3
RRANGE=$4

if [[ "${CARD}" == "" ]] || [[ "${CARD}" == "-h" ]] || [[ "${CARD}" == "--help" ]]; then
    Help
    exit
fi

if [[ "${NAME}" == "" ]]; then
    NAME="Test"
fi

if [[ "${RRANGE}" == "" ]]; then
    RRANGE="30"
fi

ARGS="-M MultiDimFit --algo singles --redefineSignalPOIs r -m 0"
if [[ "${FITARG}" != *"robustFit"* ]]; then
    ARGS="${ARGS} --robustFit 1"
fi

echo ">>> combine -d ${CARD} ${ARGS} ${FITARG} -n _initialFit_${NAME} --rMin -${RRANGE} --rMax ${RRANGE}"
combine -d ${CARD} ${ARGS} ${FITARG} -n _initialFit_${NAME} --rMin -${RRANGE} --rMax ${RRANGE}
