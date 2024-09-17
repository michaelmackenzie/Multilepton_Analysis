#! /bin/bash
# Do goodness of fit studies

Help() {
    echo "Do goodness of fit studies"
    echo "Usage: do_goodness_of_fit.sh <card> [options]"
    echo "Options:"
    echo " --rrange       (-r ): POI range (default = 30)"
    echo " --toys         (-t ): Number of toys (default = 100)"
    echo " --toysperloop   (-g): Number of toys per loop (default = 50)"
    echo " --fitarg            : Additional fit arguments"
    echo " --skipworkspace     : Don't recreate workspace for input data card"
    echo " --plotonly          : Only make plots"
    echo " --algo              : Only process the given algorithm"
    echo " --tag               : Tag for output results"
    echo " --help         (-h ): Print this information"
}

CARD=""
RRANGE="30"
NTOYS=100
TOYSPERLOOP=50
FITARG=""
SKIPWORKSPACE=""
PLOTONLY=""
TAG=""
ONLYALGO=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]; then
        Help
        exit
    elif [[ "${var}" == "--rrange" ]] || [[ "${var}" == "-r" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        RRANGE=${var}
    elif [[ "${var}" == "--toys" ]] || [[ "${var}" == "-t" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NTOYS=${var}
    elif [[ "${var}" == "--toysperloop" ]] || [[ "${var}" == "-g" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TOYSPERLOOP=${var}
    elif [[ "${var}" == "--fitarg" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        FITARG=${var}
    elif [[ "${var}" == "--tag" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--algo" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        ONLYALGO=${var}
    elif [[ "${var}" == "--skipworkspace" ]]; then
        SKIPWORKSPACE="d"
    elif [[ "${var}" == "--plotonly" ]]; then
        PLOTONLY="d"
    elif [[ "${CARD}" != "" ]]; then
        echo "Arguments aren't configured correctly! (at ${var}, CARD=${CARD})"
        Help
        exit
    else
        CARD=${var}
    fi
    iarg=$((iarg + 1))
done

if [[ "${CARD}" == "" ]]; then
    echo "No card given!"
    exit
fi

echo "Using card ${CARD} for the studies with TAG=${TAG}"
if [[ "${TAG}" != "" ]]; then
    TAG="_${TAG}"
fi

# Create a workspace if needed

WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
if [[ "${SKIPWORKSPACE}" == "" ]] && [[ "${CARD}" == *".txt" ]]; then
    text2workspace.py ${CARD} --channel-masks -o ${WORKSPACE}
    echo "Created workspace ${WORKSPACE}"
fi

FITARG="${FITARG} --cminDefaultMinimizerStrategy 0 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --rMin -${RRANGE} --rMax ${RRANGE}"
if [ ${NTOYS} -lt ${TOYSPERLOOP} ]; then
    echo "Setting N(gen) per loop to ${NTOYS}"
    TOYSPERLOOP=${NTOYS}
fi

for ALGO in "saturated" "KS" "AD"; do
    if [[ "${ONLYALGO}" != "" ]] && [[ "${ALGO}" != "${ONLYALGO}" ]]; then
        echo "Skipping ${ALGO} algorithm (only processing ${ONLYALGO} algorithm)"
        continue
    fi
    if [[ "${ALGO}" == "saturated" ]]; then
        ADDITIONAL="--toysFreq"
    else
        ADDITIONAL=""
    fi
    if [[ "${PLOTONLY}" == "" ]]; then
        echo "Performing ${ALGO} goodness of fit calculation"
        echo ">>> combine -M GoodnessOfFit --algo=${ALGO} -d ${WORKSPACE} ${FITARG} -n .${ALGO}_observed${TAG}"
        combine -M GoodnessOfFit --algo=${ALGO} -d ${WORKSPACE} ${FITARG} -n .${ALGO}_observed${TAG}

        #Generate toys in sections
        SEED=90
        OUTPUTLIST=""
        for ((IGEN=0; IGEN<${NTOYS}; IGEN+=${TOYSPERLOOP}))
        do
            SEED=$((SEED+1))
            echo ">>> combine -M GoodnessOfFit --algo=${ALGO} -d ${WORKSPACE} ${FITARG} -n .${ALGO}${TAG} -t ${TOYSPERLOOP} ${ADDITIONAL}"
            combine -M GoodnessOfFit --algo=${ALGO} -d ${WORKSPACE} ${FITARG} -n .${ALGO}${TAG} -t ${TOYSPERLOOP} ${ADDITIONAL} -s ${SEED}
            TOYFILE="higgsCombine.${ALGO}${TAG}.GoodnessOfFit.mH120.${SEED}.root"
            if [ ! -f ${TOYFILE} ]; then
                echo "${TOYFILE} not found!"
                exit
            fi
            OUTPUTLIST="${OUTPUTLIST} ${TOYFILE}"
        done
        echo "Merging output files..."
        TOYFILE="higgsCombine.${ALGO}${TAG}.GoodnessOfFit.mH120.root"
        ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/haddfitdiag.py ${TOYFILE} ${OUTPUTLIST}
        rm ${OUTPUTLIST}
    fi

    OBSFILE="higgsCombine.${ALGO}_observed${TAG}.GoodnessOfFit.mH120.root"
    if [ ! -f ${OBSFILE} ]; then
        echo "${OBSFILE} not found!"
    elif [ ! -f ${TOYFILE} ]; then
        echo "${TOYFILE} not found!"
    else
        root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_goodness_of_fit.C(\"${OBSFILE}\", \"${TOYFILE}\", \"_${ALGO}${TAG}\")"
    fi
done

echo "Finished running goodness-of-fit tests"
