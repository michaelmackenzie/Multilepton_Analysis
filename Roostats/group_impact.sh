#! /bin/bash

WORKSPACE=$1
GROUP=$2
DOOBS=$3

if [[ "${DOOBS}" == "" ]]
then
    DOOBS="-t -1"
else
    DOOBS=""
fi

#evaluate precision on r with/without the group of nuisances
combine -M FitDiagnostics -n _groupFit_Test_Nominal  -d ${WORKSPACE} ${DOOBS} --rMin -20 --rMax 20
combine -M FitDiagnostics -n _groupFit_Test_${GROUP} -d ${WORKSPACE} ${DOOBS} --rMin -20 --rMax 20 --freezeNuisanceGroups ${GROUP}

FNOMINAL="fitDiagnostics_groupFit_Test_Nominal.root"
FGROUP="fitDiagnostics_groupFit_Test_${GROUP}.root"
if [ ! -f ${FNOMINAL} ]
then
    echo "Failed to get the nominal error in workspace ${WORKSPACE}"
    exit 1
fi

if [ ! -f ${FGROUP} ]
then
    echo "Failed to get impact for group ${GROUP} in workspace ${WORKSPACE}"
    exit 2
fi

root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/extract_impact.C(\"${FNOMINAL}\", \"${FGROUP}\")"
rm ${FNOMINAL} ${FGROUP}
