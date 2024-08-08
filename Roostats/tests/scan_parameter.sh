#! /bin/bash
# Create a likelihood scan of a single parameter

Help() {
    echo "Create a likelihood scan of a single parameter"
    echo " --card         (-d) : Input card/workspace"
    echo " --param        (-p) : Parameter to scan"
    echo " --fitarg            : Fit arguments"
    echo " --points            : N(points) (default = 50)"
    echo " --tag               : Tag"
    echo " --plotonly          : Plot only"
    echo " --seed              : Random seed"
    echo " --releaseconstraint : Remove nuisance parameter constraint"
}

CARD=""
PARA="r"
ARGS=""
POINTS="50"
TAG=""
PLOTONLY=""
SEED=""
FREECONSTRAINT=""

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
    elif [[ "${var}" == "--param" ]] || [[ "${var}" == "-p" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        PARA=${var}
    elif [[ "${var}" == "--points" ]] || [[ "${var}" == "-p" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        POINTS=${var}
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--fitarg" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        ARGS=${var}
    elif [[ "${var}" == "--seed" ]] || [[ "${var}" == "-s" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SEED=${var}
    elif [[ "${var}" == "--plotonly" ]]
    then
        PLOTONLY="d"
    elif [[ "${var}" == "--releaseconstraint" ]]
    then
        FREECONSTRAINT="d"
    elif [[ "${var}" == "--card" ]] || [[ "${var}" == "-d" ]]
    then
        if [[ "${CARD}" != "" ]]; then
            echo "Card set more than once!"
            Help
            exit
        fi
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        CARD=${var}
    elif [[ "${CARD}" == "" ]]
    then
        CARD=${var}
    else
        echo "Arguments aren't configured correctly: flag = ${var}"
        Help
        exit
    fi
    iarg=$((iarg + 1))
done

if [[ "${CARD}" == "-h" ]] || [[ "${CARD}" == "--help" ]] || [[ "${CARD}" == "" ]]; then
    Help
    exit
fi

if [ ! -f ${CARD} ]; then
    echo "Card ${CARD} not found!"
    exit
fi

if [[ "${TAG}" != "" ]]; then
    TAG="_${TAG}"
fi

NAMETAG="Test"
if [[ "${TAG}" != "" ]]; then
    NAMETAG=$TAG
fi

if [[ "${PLOTONLY}" == "" ]]; then
    echo "Scanning parameter ${PARA} using card ${CARD}"
    if [[ "${SEED}" != "" ]]; then
        ARGS="${ARGS} -s ${SEED}"
    fi
    if [[ "${FREECONSTRAINT}" == "" ]]; then
        ARGS="${ARGS} --redefineSignalPOIs r"
    else
        ARGS="${ARGS} --redefineSignalPOIs r,${PARA}"
    fi
    ARGS="--saveNLL --algo grid -P ${PARA}  --floatOtherPOIs 1 --points ${POINTS} --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 ${ARGS}"
    echo ">>> combine -d ${CARD} -M MultiDimFit -n ${NAMETAG} ${ARGS}"
    combine -d ${CARD} -M MultiDimFit -n ${NAMETAG} ${ARGS}
fi

FILE="higgsCombine${NAMETAG}.MultiDimFit.mH120.root"
if [[ "${SEED}" != "" ]]; then
    FILE="higgsCombine${NAMETAG}.MultiDimFit.mH120.${SEED}.root"
fi


if [ ! -f ${FILE} ]; then
    echo "File ${FILE} not found!"
    exit
fi

root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_scan.C(\"${FILE}\", \"${PARA}\", \"${TAG}\")"
