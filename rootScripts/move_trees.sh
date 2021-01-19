#! /bin/bash
outdir=$1
indir=$2
force=""
if [ "$1" == "" ]
then
    outdir="ztautau_nanoaod_trees_nomva"
fi

year=$4

if [ "$2" == "" ]
then
    indir="./*${year}*.tree"
else
    indir="root://cmseos.fnal.gov//store/user/mmackenz/"${indir}"/*${year}*.tree"
fi
if [ "$3" == "" ]
then
    force=""
else
    force="-f "
fi

outdir="root://cmseos.fnal.gov//store/user/mmackenz/"${outdir}

echo "Using input path ${indir} and output path ${outdir}"
for f in `ls ${indir}`; do echo "Copying tree file "${f}; xrdcp ${force}$f $outdir; done

echo "Finished moving trees! Remove local trees when confident there were no errors"
