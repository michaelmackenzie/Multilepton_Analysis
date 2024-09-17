#! /bin/bash
# Compute the significance using toys

Help() {
    echo "Compute the significance using toys"
    echo "Usage: compute_significance.sh <card> [options]"
    echo "Options:"
    echo " --rrange     (-r ): POI range (default = 30)"
    echo " --fitarg          : Additional fit arguments"
    echo " --seed       (-s ): Seed for generation (default = -1)"
    echo " --iterations (-i ): Generation iterations (default = 1)"
    echo " --toys       (-t ): Generation N(toys) (default = 10)"
    echo " --grid-jobs  (-g ): Number of grid jobs to submit (default = 100)"
    echo " --grid-sub        : Prepare a grid submission"
    echo " --grid-fin        : Finish processing a grid submission"
    echo " --skipworkspace   : Don't recreate workspace for input data card"
    echo " --plotonly        : Only make plots"
    echo " --dryrun          : Only setup the processing"
    echo " --tag             : Tag for output results"
    echo " --help       (-h ): Print this information"
}

CARD=""
RRANGE="30"
FITARG=""
SEED="-1"
ITERATIONS="1"
NTOYS="10"
NJOBS=10
GRIDSUB=""
GRIDFIN=""
SKIPWORKSPACE=""
PLOTONLY=""
DRYRUN=""
TAG="Test"

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
    elif [[ "${var}" == "--fitarg" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        FITARG=${var}
    elif [[ "${var}" == "--seed" ]] || [[ "${var}" == "-s" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SEED=${var}
    elif [[ "${var}" == "--iterations" ]] || [[ "${var}" == "-i" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        ITERATIONS=${var}
    elif [[ "${var}" == "--toys" ]] || [[ "${var}" == "-t" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NTOYS=${var}
    elif [[ "${var}" == "--grid-jobs" ]] || [[ "${var}" == "-g" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NJOBS=${var}
    elif [[ "${var}" == "--grid-sub" ]]; then
        GRIDSUB="d"
    elif [[ "${var}" == "--grid-fin" ]]; then
        GRIDFIN="d"
    elif [[ "${var}" == "--tag" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--skipworkspace" ]]; then
        SKIPWORKSPACE="d"
    elif [[ "${var}" == "--plotonly" ]]; then
        PLOTONLY="d"
    elif [[ "${var}" == "--dryrun" ]]; then
        DRYRUN="d"
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
TAG="${TAG}"

# Create a workspace if needed

WORKSPACE=`echo ${CARD} | sed 's/.txt/_workspace.root/'`
FITNAME=`echo ${CARD} | sed 's/.txt//' | sed 's/_workspace.root//' | sed 's/.root//' | sed 's/combine_mva_//'`
FITNAME="sig_${FITNAME}"
# Task name for grid processing
TASK=`echo ${CARD} | sed 's/.txt//' | sed 's/_workspace.root//' | sed 's/.root//'`
TASK="sig_${TASK}"
if [[ "${TAG}" != "" ]]; then
    FITNAME="${FITNAME}_${TAG}"
    TASK="${TASK}_${TAG}"
fi
if [[ "${SKIPWORKSPACE}" == "" ]] && [[ "${CARD}" == *".txt" ]] && [[ "${GRIDFIN}" == "" ]]; then
    text2workspace.py ${CARD} --channel-masks -o ${WORKSPACE}
    echo "Created workspace ${WORKSPACE}"
fi

FITARG="--LHCmode LHC-significance ${FITARG} --cminDefaultMinimizerStrategy 0 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --rMin -${RRANGE} --rMax ${RRANGE}"
GENARG="--saveToys --fullBToys --saveHybridResult -T ${NTOYS} -i ${ITERATIONS} -s ${SEED}"
if [[ "${PLOTONLY}" == "" ]] && [[ "${GRIDFIN}" == "" ]]; then
    # Local processing:
    if [[ "${GRIDSUB}" == "" ]]; then
        echo "Performing significance calculation"
        echo ">>> combine -M HybridNew -d ${WORKSPACE} ${GENARG} ${FITARG} -n .${TAG}"
        if [[ "${DRYRUN}" == "" ]]; then
            combine -M HybridNew -d ${WORKSPACE} ${GENARG} ${FITARG} -n .${TAG}
        fi
    else # Grid submission setup
        echo "Using grid task name ${TASK}"
        RESULTDIR=grid/${TASK}
        [ ! -f ${RESULTDIR} ] && mkdir -p ${RESULTDIR}
        cd ${RESULTDIR}
        ln -s ../../${WORKSPACE} ${WORKSPACE}
        if [[ "${DRYRUN}" != "" ]]; then
            FITARG="${FITARG} --dry-run"
        fi
        echo ">>> combineTool.py -M HybridNewGrid -d ${WORKSPACE} ${GENARG} ${FITARG} -n .${TAG} --job-mode condor --task-name ${TASK} --sub-opts +DesiredOS = \"SL7\" +JobFlavour = \"longlunch\""
        combineTool.py -M HybridNew -d ${WORKSPACE} ${GENARG} ${FITARG} -n .${TAG} --job-mode condor --task-name ${TASK} --sub-opts '+DesiredOS = "SL7"\n+JobFlavour = "longlunch"'

        # Update condor scripts to process N(grid jobs)
        sed -i "s/queue 1/queue ${NJOBS}/g" condor_${TASK}.sub
        sed -i "s/-eq 0/-lt ${NJOBS}/g" condor_${TASK}.sh
        TASKDIR=~/private/grid/combine/${TASK}
        COMMAND="mv condor_${TASK}.s* ${TASKDIR}/"
        echo ">>> ${COMMAND}"
        if [[ "${DRYRUN}" == "" ]]; then
            [ -d ${TASKDIR} ] && rm ${TASKDIR}/*
            [ ! -d ${TASKDIR} ] && mkdir -p ${TASKDIR}
            ${COMMAND}
        fi
        # Hack to generate N grid submissions
        echo "Exiting with grid setup complete, use grid directory ${TASKDIR}"
        exit
    fi
fi

if [[ "${GRIDFIN}" == "" ]]; then
    FILE="higgsCombine.${TAG}.HybridNew.mH120.${SEED}.root"
    if [ ! -f ${FILE} ]; then
        echo "File ${FILE} not found!"
    else
        echo "Finished running significance tests"
    fi
    exit
fi

# Assume a grid submission has been processed, merge the toys and evaluate the significance
FILE="higgsCombine.${TAG}.HybridNew.mH120.root"
if [[ "${PLOTONLY}" == "" ]]; then
    echo "Merging output grid files"
    RESULTDIR=grid/${TASK}
    cd ${RESULTDIR}
    echo ">>> hadd -f ${FILE} higgsCombine.${TAG}.HybridNew.mH120.*.root"
    if [[ "${DRYRUN}" == "" ]]; then
        hadd -f ${FILE} higgsCombine.${TAG}.HybridNew.mH120.*.root
    fi
fi

if [ ! -f ${FILE} ] && [[ "${DRYRUN}" == "" ]]; then
    echo "File ${FILE} not found!"
    exit
fi

# Evaluate the total significance
echo ">>> combine -M HybridNew -d ${WORKSPACE} ${FITARG} -n .obs_${TAG} --toysFile=${FILE} --readHybridResult"
if [[ "${DRYRUN}" == "" ]]; then
    combine -M HybridNew -d ${WORKSPACE} ${FITARG} -n .obs_${TAG} --toysFile=${FILE} --readHybridResult
fi
# combine -M HybridNew -d ${WORKSPACE} ${FITARG} -n .obs_${TAG} --toysFile=${FILE} --pvalue
