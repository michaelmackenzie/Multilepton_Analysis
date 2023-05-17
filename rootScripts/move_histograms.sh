#! /bin/bash

Help() {
    echo "Move histograms to eos space"
    echo " 1: Output directory (e.g. nanoaods_mva/)"
    echo " 2: Input directory (\"\" for local, e.g. \"nanoaods_mva\" for from eos)"
    echo " 3: Don't force (\"d\" to not overwrite previous files)"
    echo " 4: Year tag"
    echo " 5: Selection tag"
    echo " 6: Remove after copying (e.g. \"d\")"
    echo " 7: Apply timeout to retry (e.g. \"d\")"
}

if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
    Help
    exit
fi

OUTDIR=$1
INDIR=$2
DONTFORCE=$3
YEAR=$4
SELECTION=$5
RMAFTER=$6
APPLYTIMEOUT=$7

if [[ "${OUTDIR}" == "" ]]
then
    OUTDIR="nanoaods_dev/"
fi

lscommand="ls"
filehead=""
rmhead="rm"
if [[ "${INDIR}" == "" ]]
then
    INDIR="./*${SELECTION}*${YEAR}*.hist"
else
    filehead="root://cmseos.fnal.gov//store/user/${USER}/histograms/"${INDIR}"/"
    INDIR="/store/user/${USER}/histograms/${INDIR}/*${SELECTION}*${YEAR}*.hist"
    lscommand="eos root://cmseos.fnal.gov ls"
    rmhead="eos root://cmseos.fnal.gov rm"
fi

# if [[ "${DONTFORCE}" != "" ]]
# then
#     FORCE=""
# else
FORCE="-f "
# fi

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
