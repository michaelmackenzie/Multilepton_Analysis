#! /bin/bash

# Create and inspect a single toy dataset

CARD=$1
SEED=$2
TAG=$3

if [[ "${SEED}" == "" ]]; then
    SEED=90
fi

CAT=`echo ${CARD} | sed 's/combine_bemu_zemu_//' | sed 's/.txt//' | sed 's/_workspace.root//'`
echo ${CAT}

######################################
# Generate a toy dataset
######################################

combine -M GenerateOnly -d ${CARD} -t 1 -m 125 -s ${SEED} -n _toy_test${TAG} --saveToys
TOYDATA=higgsCombine_toy_test${TAG}.GenerateOnly.mH125.${SEED}.root

if [ ! -f ${TOYDATA} ]; then
    echo "Failed to generate toy data ${TOYDATA}"
    exit
fi
ls -l ${TOYDATA}

RMIN=-100
RMAX=100

######################################
# Fit the data using FitDiagnostics
######################################

ARGUMENTS="--setParameterRanges r=${RMIN},${RMAX} --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints"
ARGUMENTS="${ARGUMENTS} --saveShapes --saveNLL --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1"

combine -M FitDiagnostics -d ${CARD} -n "_toy_test${TAG}" --toysFile=${TOYDATA} -t 1 ${ARGUMENTS}

FITFILE=fitDiagnostics_toy_test${TAG}.root
if [ ! -f ${FITFILE} ]; then
    echo "Failed to generate fit file ${FITFILE}"
    exit
fi
ls -l ${FITFILE}

#Print the fit results
root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_fit.C(\"${FITFILE}\", \"figures/test_toy${TAG}\")"


######################################
# Fit the data using MultiDimFit
######################################

ARGUMENTS="--algo singles --setParameterRanges r=${RMIN},${RMAX} -m 125"
# ARGUMENTS="--algo cross --cl=0.68 --setParameterRanges r=${RMIN},${RMAX} -m 125"
ARGUMENTS="${ARGUMENTS}  --cminDefaultMinimizerStrategy=0"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints"
ARGUMENTS="${ARGUMENTS} --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --saveNLL --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --saveWorkspace --saveFitResult"
combine -M MultiDimFit -d ${CARD} -n "_toy_test${TAG}" --toysFile=${TOYDATA} -t 1 ${ARGUMENTS}

FITFILE=higgsCombine_toy_test${TAG}.MultiDimFit.mH125.123456.root
if [ ! -f ${FITFILE} ]; then
    echo "Failed to generate fit file ${FITFILE}"
    exit
fi
ls -l ${FITFILE}

ARGUMENTS="--algo grid --setParameterRanges r=-10,10 --points 100 --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints"
ARGUMENTS="${ARGUMENTS} --saveNLL --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --saveWorkspace --saveFitResult"
combine -M MultiDimFit -d ${CARD} -n "_toy_test_grid${TAG}" --toysFile=${TOYDATA} -t 1 ${ARGUMENTS}

FITFILE=higgsCombine_toy_test_grid${TAG}.MultiDimFit.mH125.123456.root
if [ ! -f ${FITFILE} ]; then
    echo "Failed to generate fit file ${FITFILE}"
    exit
fi
ls -l ${FITFILE}

#plot the envelope
root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(0, \"${FITFILE}\", \"_toy_test${TAG}\", 0)"
