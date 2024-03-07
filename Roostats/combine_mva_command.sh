#! /bin/bash
# Combine arguments for standard MVA score fits

Help() {
    echo "Process MVA score fitting datacard"
    echo "1: Card name"
    echo "2: Additional arguments"
    echo "3: Do Asimov (add \"-t -1 --run blind --noFitAsimov\" (or just \"-t -1\" if \"-M\" is provided) to the arguments)"
    echo "4: r range (default = 100)"
    echo "5: Increase tolerance"
}

CARD=$1
ARGS=$2
DOASIMOV=$3
RRANGE=$4
TOLERANCE=$5

if [[ "${CARD}" == "" ]] || [[ "${CARD}" == "--help" ]] || [[ "${CARD}" == "-h" ]]; then
    Help
    exit
fi

if [[ "${RRANGE}" == "" ]]; then
    RRANGE="100"
fi

ARGS="${ARGS} --cminDefaultMinimizerStrategy=0 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGS="${ARGS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstants --X-rtd MINIMIZER_multiMin_maskConstraints"
if [[ "${DOASIMOV}" != "" ]]; then
    if [[ "${ARGS}" != *"-M "* ]]; then
        ARGS="${ARGS} -t -1 --run blind --noFitAsimov"
    else
        ARGS="${ARGS} -t -1"
    fi
fi

if [[ "${TOLERANCE}" != "" ]]; then
    ARGS="${ARGS} --cminDefaultMinimizerTolerance 0.001 --cminDiscreteMinTol 0.0001"
    if [[ "${ARGS}" == *"AsympototicLimits"* ]] || [[ "${ARGS}" != *"-M"* ]]; then
        ARGS="${ARGS} --rAbsAcc 0.0005 --rRelAcc 0.0005"
    fi
fi

echo combine -d ${CARD} ${ARGS} --rMin -${RRANGE} --rMax ${RRANGE}
combine -d ${CARD} ${ARGS} --rMin -${RRANGE} --rMax ${RRANGE}
