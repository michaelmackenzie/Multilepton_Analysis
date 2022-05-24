#! /bin/bash

YEARS=$1
SELECTIONS=$2
SET=$3

if [[ "${YEARS}" == "" ]]
then
    YEARS="2016_2017_2018"
fi

if [[ "${SELECTIONS}" == "" ]]
then
    SELECTIONS="mutau etau mutau_e etau_mu emu"
fi

if [[ "${SET}" == "" ]]
then
    SET=8
fi

echo "Doing training for selections ${SELECTIONS} on set ${SET} in year string ${YEARS}"
for SELECTION in $SELECTIONS;
do
    #Z signal
    root.exe -q -b "train_tmva.C(\"trees/background_clfv_Z0_nano_${SELECTION}_${YEARS}_${SET}.tree\")"
    #H signal
    root.exe -q -b "train_tmva.C(\"trees/background_clfv_higgs_nano_${SELECTION}_${YEARS}_${SET}.tree\")"
done

echo "Finished all selections"
