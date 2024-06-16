#! /bin/bash

Help() {
    echo "Move histograms to eos space"
    echo "Usage: move_histograms.sh [output directory e.g. nanoaods_mva/] [options]"
    echo " --help (-h): Print this message"
    echo " --indir    : Input directory (\"\" for local, e.g. \"nanoaods_mva\" for from eos)"
    echo " --year     : Year tag"
    echo " --selection: Selection tag"
    echo " --maker    : HistMaker label"
    echo " --tag      : Sample tag"
    echo " --veto     : Veto tag"
    echo " --remove   : Remove after copying"
    echo " --timeout  : Apply timeout to retry"
    echo " --dontforce: Don't overwrite existing files"
}

if [ $# -eq 0 ]
then
    echo "No parameters passed! Printing help (--help) information:"
    Help
    exit
fi

OUTDIR=""
INDIR=""
DONTFORCE=""
YEAR=""
SELECTION=""
MAKER=""
VETO=""
TAG=""
RMAFTER=""
APPLYTIMEOUT=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--indir" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        INDIR=${var}
    elif [[ "${var}" == "--selection" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SELECTION=${var}
    elif [[ "${var}" == "--year" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        YEAR=${var}
    elif [[ "${var}" == "--maker" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        MAKER=${var}
    elif [[ "${var}" == "--veto" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        VETO=${var}
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--dontforce" ]]
    then
        DONTFORCE="d"
    elif [[ "${var}" == "--remove" ]]
    then
        RMAFTER="d"
    elif [[ "${var}" == "--timeout" ]]
    then
        APPLYTIMEOUT="d"
    elif [[ "${var}" == "--"* ]]
    then
        echo "Unknown option ${var}"
        Help
        exit
    elif [[ "${OUTDIR}" != "" ]]
    then
        echo "Too many unflagged options!"
        Help
        exit
    else
        OUTDIR=${var}
    fi
    iarg=$((iarg + 1))
done

if [[ "${OUTDIR}" == "" ]]; then
    echo "No output directory given!"
    Help
    exit
fi

#ensure the output directory format is as expected
if [[ "${OUTDIR: -1}" != "/" ]]; then
    OUTDIR="${OUTDIR}/"
fi

lscommand="ls"
filehead=""
rmhead="rm"
if [[ "${INDIR}" == "" ]]
then
    INDIR="./${MAKER}*${SELECTION}*${YEAR}*.hist"
else
    filehead="root://cmseos.fnal.gov//store/user/${USER}/histograms/"${INDIR}"/"
    INDIR="/store/user/${USER}/histograms/${INDIR}/${MAKER}*${SELECTION}*${YEAR}*.hist"
    lscommand="eos root://cmseos.fnal.gov ls"
    rmhead="eos root://cmseos.fnal.gov rm"
fi

if [[ "${DONTFORCE}" != "" ]]
then
    FORCE=""
else
    FORCE="-f "
fi

if [[ "${RMAFTER}" != "" ]]
then
    echo "Removing files after copying!"
fi

OUTDIR="/store/user/${USER}/histograms/"${OUTDIR}
if [ `eos root://cmseos.fnal.gov ls ${OUTDIR} 2>&1 | grep -v "Unable to stat" | head -n 1 | wc | awk '{print $1}'` -eq 0 ]
then
    echo "Creating output directory ${OUTDIR}"
    eos root://cmseos.fnal.gov mkdir -p ${OUTDIR}
fi
OUTDIR="root://cmseos.fnal.gov/"${OUTDIR}

echo "Using input path ${INDIR} and output path ${OUTDIR}"
for f in `${lscommand} ${INDIR}`;
do
    if [[ "${VETO}" != "" ]] && [[ "${f}" == *"${VETO}"* ]]; then
        continue
    fi
    if [[ "${TAG}" != "" ]] && [[ "${f}" != *"${TAG}"* ]]; then
	continue;
    fi
    echo "Copying hist file "${filehead}${f};
    if [[ "$APPLYTIMEOUT" != "" ]]
    then
        while [ 1 ]; do
            timeout 20 xrdcp ${FORCE}${filehead}$f $OUTDIR;
            if [ $? -eq 0 ]; then
                break
            else
                echo "Re-attempting to copy ${filehead}${f}"
            fi
        done
    else
        xrdcp ${FORCE}${filehead}$f $OUTDIR
    fi
    if [[ "${RMAFTER}" != "" ]]
    then
        echo "Removing hist file ${filehead}${f}"
        ${rmhead} ${filehead}${f}
    fi
done

echo "Finished moving histograms! Remove local files when confident there were no errors"
