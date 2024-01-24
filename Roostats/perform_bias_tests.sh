#! /bin/bash
#Perform bias test in Higgs Combine

Help () {
    echo "Evaluate bias tests using Higgs Combine FitDiagnostics"
    echo " 1: input workspace"
    echo " 2: output name"
    echo " 3: signal r-value (default = 0)"
    echo " 4: N(toys) (default = 100)"
    echo " 5: signal r range (default = 50)"
    echo " 6: additional combine arguments"
    echo " 7: flag to skip combine processing"
}

WORKSPACE=$1
OUTNAME=$2
RVALUE=$3
NTOYS=$4
RRANGE=$5
ARGUMENTS=$6
SKIPCOMBINE=$7

if [[ "${WORKSPACE}" == "" ]]; then
    echo "No input workspace provided!"
    Help
    exit
fi

if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
    Help
    exit
fi

if [[ "${OUTNAME}" == "" ]]; then
    echo "No output name provided!"
    exit
fi

if [[ "${RVALUE}" == "" ]]; then
    RVALUE="0.0"
fi

if [[ "${NTOYS}" == "" ]]; then
    NTOYS="100"
fi

if [[ "${RRANGE}" == "" ]]; then
    RRANGE="50"
fi

if [[ ! -f ${WORKSPACE} ]]; then
    echo "Workspace ${WORKSPACE} not found"
    exit
fi

if [[ "${ARGUMENTS}" == "" ]]; then
    ARGUMENTS="--cminDefaultMinimizerStrategy 0"
else
    ARGUMENTS="${ARGUMENTS} --cminDefaultMinimizerStrategy 0"
fi

if [[ "${SKIPCOMBINE}" == "" ]]; then
    echo "Running combine tests..."
    echo "combine -d ${WORKSPACE} -M FitDiagnostics ${ARGUMENTS} -t ${NTOYS} --expectSignal ${RVALUE} --rMin -${RRANGE} --rMax ${RRANGE} -n .${OUTNAME}"
    combine -d ${WORKSPACE} -M FitDiagnostics ${ARGUMENTS} -t ${NTOYS} --expectSignal ${RVALUE} --rMin -${RRANGE} --rMax ${RRANGE} -n .${OUTNAME}
fi

if [[ ! -f "fitDiagnostics.${OUTNAME}.root" ]]; then
    echo "No output fit diagnostics file found!"
    exit
fi

echo "Creating bias plots..."
root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/plot_combine_fits.C(\"fitDiagnostics.${OUTNAME}.root\", ${RVALUE}, \"bias_${OUTNAME}\")"
