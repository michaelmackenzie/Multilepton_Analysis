#! /bin/bash
# Combine arguments for standard B->e+mu fits

Help() {
    echo "Process B->e+mu datacard"
    echo "1: Card name"
    echo "2: Additional arguments"
    echo "3: Do Asimov (add \"-t -1 --run blind --noFitAsimov\" to the arguments)"
    echo "4: r range (default = 100)"
}

CARD=$1
ARGS=$2
DOASIMOV=$3
RRANGE=$4

if [[ "${CARD}" == "" ]] || [[ "${CARD}" == "--help" ]] || [[ "${CARD}" == "-h" ]]; then
    Help
    exit
fi

if [[ "${RRANGE}" == "" ]]; then
    RRANGE="20"
fi

ARGS="${ARGS} --cminDefaultMinimizerStrategy 0"
ARGS="${ARGS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_multiMin_hideConstants"
ARGS="${ARGS} --cminRunAllDiscreteCombinations"
# ARGS="${ARGS} --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_multiMin_hideConstants --X-rtd MINIMIZER_multiMin_maskConstraints"
if [[ "${DOASIMOV}" != "" ]]; then
    if [[ "${ARGS}" != *"-M "* ]]; then
        ARGS="${ARGS} -t -1 --run blind --noFitAsimov"
    else
        ARGS="${ARGS} -t -1"
    fi
fi

echo combine -d ${CARD} ${ARGS} --rMin -${RRANGE} --rMax ${RRANGE}
combine -d ${CARD} ${ARGS} --rMin -${RRANGE} --rMax ${RRANGE}
