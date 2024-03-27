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

NTOYS=1
combine -M GenerateOnly -d ${CARD} -t ${NTOYS} -m 125 -s ${SEED} -n _toy_test${TAG} --saveToys
TOYDATA=higgsCombine_toy_test${TAG}.GenerateOnly.mH125.${SEED}.root

if [ ! -f ${TOYDATA} ]; then
    echo "Failed to generate toy data ${TOYDATA}"
    exit
fi
ls -l ${TOYDATA}

RMIN=-10
RMAX=10

######################################
# Fit the data using MultiDimFit
######################################

OUTTAG="_toy_scan${TAG}"
ARGUMENTS="--algo grid --setParameterRanges r=${RMIN},${RMAX} --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints"
ARGUMENTS="${ARGUMENTS} --saveNLL --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --saveWorkspace --saveFitResult"
combine -M MultiDimFit -d ${CARD} -n ${OUTTAG} --toysFile=${TOYDATA} -t 1 ${ARGUMENTS}

FITFILE=higgsCombine${OUTTAG}.MultiDimFit.mH125.123456.root
if [ ! -f ${FITFILE} ]; then
    echo "Failed to generate fit file ${FITFILE}"
    exit
fi
ls -l ${FITFILE}
