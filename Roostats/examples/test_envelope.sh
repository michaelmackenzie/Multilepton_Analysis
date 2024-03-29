#! /bin/bash

# Process a test of fitting with a function envelope

########################################################################
# Create the setup

root.exe -q -b test_envelope.C

# Convert the card to a workspace
WORKSPACE=test_envelope_workspace.root
text2workspace.py  test_envelope.txt -o  ${WORKSPACE}

########################################################################
# Generate and fit a toy event

SEED=90
ARGUMENTS="-s ${SEED} -t 1 --saveNLL --saveSpecifiedIndex cat --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --cminDefaultMinimizerStrategy=0 --algo grid --points 40 --setParameterRanges r=-2,6"
combine -d ${WORKSPACE} -M MultiDimFit ${ARGUMENTS}
FITFILE=higgsCombineTest.MultiDimFit.mH120.${SEED}.root

# Extract the fit results
root.exe -q -b "print_single_scan.C(\"${FITFILE}\", \"cat\", \"envelope_1\")"

########################################################################
# Generate and fit a toy event, freeze disassociated parameters

ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams"
combine -d ${WORKSPACE} -M MultiDimFit ${ARGUMENTS}
FITFILE=higgsCombineTest.MultiDimFit.mH120.${SEED}.root

# Extract the fit results
root.exe -q -b "print_single_scan.C(\"${FITFILE}\", \"cat\", \"envelope_2\")"
