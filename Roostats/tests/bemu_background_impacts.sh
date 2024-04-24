#! /bin/bash
# Evaluate the group uncertainties for the Z->emu search


CARD=$1

if [[ "${CARD}" == *".txt" ]]; then
    WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
    echo "Creating workspace ${WORKSPACE}"
    text2workspace.py ${CARD} -o ${WORKSPACE}
    CARD=${WORKSPACE}
fi

echo "Using input card ${CARD}"

ARGS="-M FitDiagnostics  --cminDefaultMinimizerStrategy 0"
ARGS="${ARGS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_multiMin_hideConstants"
# ARGS="${ARGS} --cminRunAllDiscreteCombinations"
ARGS="${ARGS} --stepSize 0.05 --setRobustFitTolerance 0.001 --setCrossingTolerance 5e-6"
ARGS="${ARGS} --rMin -100 --rMax 100 -t -1"

#Nominal fit
combine -d ${CARD} ${ARGS} -n _Nominal
FILENOM=fitDiagnostics_Nominal.root
if [ ! -f ${FILENOM} ]; then
    echo "No nominal fit file ${FILENOM} produced"
    exit
fi

#Freeze the envelope
combine -d ${CARD} ${ARGS} -n _Envelope --freezeParameters cat_10,cat_11,cat_12,cat_13
FILEENV=fitDiagnostics_Envelope.root

#Freeze all free params
combine -d ${CARD} ${ARGS} -n _Model --freezeParameters cat_10,cat_11,cat_12,cat_13,var{.*}
FILEMDL=fitDiagnostics_Model.root

echo "Envelope impact"
root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/extract_impact.C(\"${FILENOM}\", \"${FILEENV}\")"
echo "Model impact"
root.exe -q -b -l "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/extract_impact.C(\"${FILENOM}\", \"${FILEMDL}\")"
