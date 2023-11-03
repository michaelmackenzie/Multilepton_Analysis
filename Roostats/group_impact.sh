#! /bin/bash

WORKSPACE=$1
GROUP=$2
SKIPNOMINAL=$3
DOOBS=$4
TOLERANCE=$5
EXPECTSIGNAL=$6

if [[ "${DOOBS}" == "" ]]
then
    DOOBS="-t -1"
else
    DOOBS=""
fi

if [[ "${TOLERANCE}" == "" ]]
then
    TOLERANCE="0.001"
fi

if [[ "${EXPECTSIGNAL}" == "" ]]
then
    EXPECTSIGNAL="0"
fi

RMIN="-20"
RMAX="20"

#evaluate precision on r with/without the group of nuisances
if [[ "${SKIPNOMINAL}" == "" ]]
then
    combine -M FitDiagnostics --stepSize 0.01 --setRobustFitTolerance ${TOLERANCE} --setCrossingTolerance 5e-7 -n _groupFit_Test_Nominal  -d ${WORKSPACE} ${DOOBS} --rMin ${RMIN} --rMax ${RMAX} --cminDefaultMinimizerStrategy 0 --expectSignal ${EXPECTSIGNAL}
fi
combine -M FitDiagnostics --stepSize 0.01 --setRobustFitTolerance ${TOLERANCE} --setCrossingTolerance 5e-7 -n _groupFit_Test_${GROUP} -d ${WORKSPACE} ${DOOBS} --rMin ${RMIN} --rMax ${RMAX} --freezeNuisanceGroups ${GROUP} --cminDefaultMinimizerStrategy 0 --expectSignal ${EXPECTSIGNAL}

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
