#! /bin/bash
outdir=$1
indir=$2
force=""
if [ "$1" == "" ]
then
    outdir="nanoaods_dev/"
fi

year=$4
selection=$5

if [ "${indir}" == "" ]
then
    indir="./ztautau_${selection}*${year}*.hist"
else
    indir="root://cmseos.fnal.gov//store/user/mmackenz/"${indir}"/ztautau_${selection}*${year}*.tree"
fi

if [ "$3" == "" ]
then
    force=""
else
    force="-f "
fi

outdir="root://cmseos.fnal.gov//store/user/mmackenz/histograms/"${outdir}

echo "Using input path ${indir} and output path ${outdir}"
for f in `ls ${indir}`; do echo "Copying tree file "${f}; xrdcp ${force}$f $outdir; done

echo "Finished moving histograms! Remove local files when confident there were no errors"
