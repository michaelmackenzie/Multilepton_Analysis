#! /bin/bash

YEARS=$1
if [[ "${YEARS}" == "" ]]
then
    YEARS="2016_2017_2018"
fi

SELECTIONS="mutau etau emu mutau_e etau_mu"

SET=8

echo "Doing training for selections ${SELECTIONS} on set ${SET} in year string ${YEARS}"
for SELECTION in $SELECTIONS;
do
    #Z signal
    root.exe -q -b "train_tmva.C(\"trees/background_clfv_Z0_nano_${SELECTION}_${YEARS}_${SET}.tree\")"
    #H signal
    root.exe -q -b "train_tmva.C(\"trees/background_clfv_higgs_nano_${SELECTION}_${YEARS}_${SET}.tree\")"
done

echo "Finished all selections"
