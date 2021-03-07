#! /bin/bash
outdir=$1
indir=$2
force=$3
year=$4

if [ "${outdir}" == "" ]
then
    outdir="ztautau_nanoaod_trees_nomva"
fi

if [ "${indir}" == "" ]
then
    indir="./*${year}*.tree"
else
    indir="root://cmseos.fnal.gov//store/user/mmackenz/"${indir}"/*${year}*.tree"
fi
if [ "${force}" == "" ]
then
    force=""
else
    force="-f "
fi

outdir="/store/user/mmackenz/"${outdir}
if [ `eos root://cmseos.fnal.gov ls ${outdir} 2>&1 | grep -v "Unable to stat" | head -n 1 | wc | awk '{print $1}'` -eq 0 ]
then
    echo "Creating output directory ${outdir}"
    eos root://cmseos.fnal.gov mkdir ${outdir}
fi
outdir="root://cmseos.fnal.gov/"${outdir}

echo "Using input path ${indir} and output path ${outdir}"
for f in `ls ${indir}`; do echo "Copying tree file "${f}; xrdcp ${force}$f $outdir; done

echo "Finished moving trees! Remove local trees when confident there were no errors"
