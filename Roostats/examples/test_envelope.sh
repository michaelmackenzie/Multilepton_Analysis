#! /bin/bash

# Process a test of fitting with a function envelope

SEED=$1
SKIPCREATE=$2
SKIPPLOT=$3

if [[ "${SEED}" == "" ]]; then
    SEED=107
fi

########################################################################
# Create the setup

if [[ "${SKIPCREATE}" == "" ]]; then
    root.exe -q -b test_envelope.C
fi


# Convert the card to a workspace
WORKSPACE=test_envelope_workspace.root
text2workspace.py  test_envelope.txt -o  ${WORKSPACE}

########################################################################
# Generate and fit a toy event with the issue

ARGUMENTS="-s ${SEED} -t 1 --saveNLL --saveSpecifiedIndex cat --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --cminDefaultMinimizerStrategy=0 --setParameterRanges r=-5,5 --rMin -10 --rMax 10 --X-rtd MINIMIZER_freezeDisassociatedParams"
if [[ "${SKIPPLOT}" == "" ]]; then
    ARGUMENTS="${ARGUMENTS} --algo grid --points 40"
else
    ARGUMENTS="${ARGUMENTS} --algo none"
fi

echo ${ARGUMENTS}
combine -d ${WORKSPACE} -M MultiDimFit ${ARGUMENTS}
FITFILE=higgsCombineTest.MultiDimFit.mH120.${SEED}.root

# Extract the fit results
if [[ "${SKIPPLOT}" == "" ]]; then
    root.exe -q -b "print_single_scan.C(\"${FITFILE}\", \"cat\", \"envelope_1\")"
fi

########################################################################
# Generate and fit a toy event without the issue

combine -d ${WORKSPACE} -M MultiDimFit ${ARGUMENTS} --cminRunAllDiscreteCombinations

# Extract the fit results
if [[ "${SKIPPLOT}" == "" ]]; then
    root.exe -q -b "print_single_scan.C(\"${FITFILE}\", \"cat\", \"envelope_2\")"
fi
