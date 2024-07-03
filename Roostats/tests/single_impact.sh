#! /bin/bash
# Process the impact fit step for a single parameter

Help() {
    echo "Process the impact fit step for a single parameter"
    echo " 1: Input card/workspace"
    echo " 2: Parameter"
    echo " 3: Name"
    echo " 4: Additional arguments"
    echo " 5: r range (default = 30)"
}

CARD=$1
PARAM=$2
NAME=$3
FITARG=$4
RRANGE=$5

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

ARGS="-M MultiDimFit --algo impact --redefineSignalPOIs r -m 0 --floatOtherPOIs 1 --saveInactivePOI 1"
if [[ "${FITARG}" != *"robustFit"* ]]; then
    ARGS="${ARGS} --robustFit 1"
fi

echo ">>> combine -d ${CARD} ${ARGS} ${FITARG} -n _paramFit_${NAME}_${PARAM} -P ${PARAM} --rMin -${RRANGE} --rMax ${RRANGE}"
combine -d ${CARD} ${ARGS} ${FITARG} -n _paramFit_${NAME}_${PARAM} -P ${PARAM} --rMin -${RRANGE} --rMax ${RRANGE}
