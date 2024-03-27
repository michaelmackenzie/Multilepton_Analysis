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

RMIN=-10
RMAX=10
ARGUMENTS="--algo grid --setParameterRanges r=${RMIN},${RMAX} --cminDefaultMinimizerStrategy 0 --saveNLL -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --rMin ${RMIN} --rMax ${RMAX}"

rm higgsCombine_env_${CAT}*.*.mH125.*.root

#Generate a toy dataset
combine -M GenerateOnly -d ${CARD} -t 1 -m 125 -s 90 -n _env_${CAT} --saveToys
TOYDATA=higgsCombine_env_${CAT}.GenerateOnly.mH125.90.root
ls -l ${TOYDATA}

for (( PDF=0; PDF<${MAXPDFS}; PDF++ )); do
    combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} --toysFile=${TOYDATA} -t 1 --setParameters "cat_${CAT}=${PDF}" --freezeParameters "cat_${CAT}" -n "_env_${CAT}_cat_${PDF}"
done
combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} -n "_env_${CAT}_tot" --toysFile=${TOYDATA} -t 1

#To compare the scanned 1- and 2-sigma bands, fit with FitDiagnostics
combine -M MultiDimFit --algo singles -d ${CARD} -n "_env_${CAT}_tot_fit" --toysFile=${TOYDATA} -t 1 --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --rMin -100 --rMax 100 --saveToys
combine -M FitDiagnostics -d ${CARD} -n "_env_${CAT}_tot_fit" --toysFile=${TOYDATA} -t 1 --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --rMin -100 --rMax 100 --saveShapes --saveWithUncertainties

root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(${CAT})"
