#! /bin/bash

WORKSPACE=$1
GROUP=$2
SKIPNOMINAL=$3
DOOBS=$4

if [[ "${DOOBS}" == "" ]]
then
    DOOBS="-t -1"
else
    DOOBS=""
fi

RMIN="-10"
RMAX="10"

#evaluate precision on r with/without the group of nuisances
if [[ "${SKIPNOMINAL}" == "" ]]
then
    combine -M FitDiagnostics --stepSize 0.05 --setRobustFitTolerance 0.001 --setCrossingTolerance 5e-6 -n _groupFit_Test_Nominal  -d ${WORKSPACE} ${DOOBS} --rMin ${RMIN} --rMax ${RMAX}
fi
combine -M FitDiagnostics --stepSize 0.05 --setRobustFitTolerance 0.001 --setCrossingTolerance 5e-6 -n _groupFit_Test_${GROUP} -d ${WORKSPACE} ${DOOBS} --rMin ${RMIN} --rMax ${RMAX} --freezeNuisanceGroups ${GROUP}

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
rm ${FGROUP}
