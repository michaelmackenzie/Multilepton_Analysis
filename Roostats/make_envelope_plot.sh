#! /bin/bash

# Run combine scans with each individual background function, generating a plot of the envelope method

CARD=$1
CAT=$2
MAXPDFS=$3

if [[ "${CARD}" == "" ]]; then
    echo "No input Combine card/workspace provided!"
    exit 1
fi

if [[ "${CAT}" == "" ]]; then
    CAT=13
fi

if [[ "${MAXPDFS}" == "" ]]; then
    MAXPDFS=10
fi

RMIN=-5
RMAX=5
ARGUMENTS="--algo grid --setParameterRanges r=-${RMIN},${RMAX} --cminDefaultMinimizerStrategy 0 --saveNLL -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints"
for (( PDF=0; PDF<${MAXPDFS}; PDF++ )); do
    combine -M MultiDimFit -d ${CARD} ${ARGUMENTS}  --setParameters "cat_${CAT}=${PDF}" --freezeParameters "cat_${CAT}=${PDF}" -n "_env_${CAT}_cat_${PDF}"
done

root.exe -q -b "../../plot_envelope.C(${CAT})"
