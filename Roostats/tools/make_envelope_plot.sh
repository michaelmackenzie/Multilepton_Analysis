#! /bin/bash

# Run combine scans with each individual background function, generating a plot of the envelope method

Help() {
    echo "Make an envelope plot for a single Z->emu fit category"
    echo " Usage: make_envelope_plot.sh <card> [options]"
    echo " --cat      : Category number for PDF index variable (default = 13)"
    echo " --maxpdfs  : Maximum PDF index (default = 2)"
    echo " --rrange   : r parameter range (default = 10)"
    echo " --rgen     : r for generated toy (default = 0)"
    echo " --index_gen: PDF index for generated toy"
    echo " --npoints  : N(scan points) (default = 100)"
    echo " --tag      : Tag for output plot"
    echo " --seed     : Seed for generation"
}

CARD=""
CAT=13
MAXPDFS=2
RRANGE=10
RGEN=0
TAG=""
GENINDEX=""
NPOINTS=100
SEED=90

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--rrange" ]] || [[ "${var}" == "-r" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        RRANGE=${var}
    elif [[ "${var}" == "--rgen" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
    elif [[ "${var}" == "--npoints" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NPOINTS=${var}
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--maxpdfs" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        MAXPDFS=${var}
    elif [[ "${var}" == "--index_gen" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENINDEX=${var}
    elif [[ "${var}" == "--genarg" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENARG=${var}
    elif [[ "${var}" == "--cat" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        CAT=${var}
    elif [[ "${var}" == "--seed" ]] || [[ "${var}" == "-s" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SEED=${var}
    elif [[ "${CARD}" != "" ]]
    then
        echo "Arguments aren't configured correctly!"
        Help
        exit
    else
        CARD=${var}
    fi
    iarg=$((iarg + 1))
done

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

RMIN=-${RRANGE}
RMAX=${RRANGE}
ARGUMENTS="--algo grid --setParameterRanges r=${RMIN},${RMAX} --cminDefaultMinimizerStrategy 0 --saveNLL -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --rMin ${RMIN} --rMax ${RMAX}"

rm higgsCombine_env_${CAT}*.*.mH125.*.root

#Generate a toy dataset
combine -M GenerateOnly -d ${CARD} -t 1 -m 125 -s ${SEED} -n _env_${CAT} --saveToys
TOYDATA=higgsCombine_env_${CAT}.GenerateOnly.mH125.${SEED}.root
ls -l ${TOYDATA}

for (( PDF=0; PDF<${MAXPDFS}; PDF++ )); do
    combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} --points ${NPOINTS} --toysFile=${TOYDATA} -t 1 --setParameters "cat_${CAT}=${PDF}" --freezeParameters "cat_${CAT}" -n "_env_${CAT}_cat_${PDF}"
done
combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} --points ${NPOINTS} -n "_env_${CAT}_tot" --toysFile=${TOYDATA} -t 1

#To compare the scanned 1- and 2-sigma bands, fit with FitDiagnostics
combine -M MultiDimFit --algo singles -d ${CARD} -n "_env_${CAT}_tot_fit" --toysFile=${TOYDATA} -t 1 --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --rMin -100 --rMax 100 --saveToys
# combine -M FitDiagnostics -d ${CARD} -n "_env_${CAT}_tot_fit" --toysFile=${TOYDATA} -t 1 --cminDefaultMinimizerStrategy=0 -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --rMin -100 --rMax 100 --saveShapes --saveWithUncertainties

root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(${CAT}, \"\", \"${TAG}\")"
