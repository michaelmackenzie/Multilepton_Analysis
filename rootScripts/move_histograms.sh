#! /bin/bash
outdir=$1
indir=$2
force=$3
year=$4
selection=$5
if [ "$1" == "" ]
then
    outdir="nanoaods_dev/"
fi

lscommand="ls"
filehead=""
if [ "${indir}" == "" ]
then
    indir="./*${selection}*${year}*.hist"
else
    filehead="root://cmseos.fnal.gov//store/user/${USER}/histograms/"${indir}"/"
    indir="/store/user/${USER}/histograms/${indir}/*${selection}*${year}*.hist"
    lscommand="eos root://cmseos.fnal.gov ls"
fi

if [ "$3" == "" ]
then
    force=""
else
    force="-f "
fi

outdir="/store/user/${USER}/histograms/"${outdir}
if [ `eos root://cmseos.fnal.gov ls ${outdir} 2>&1 | grep -v "Unable to stat" | head -n 1 | wc | awk '{print $1}'` -eq 0 ]
then
    echo "Creating output directory ${outdir}"
    eos root://cmseos.fnal.gov mkdir -p ${outdir}
fi
outdir="root://cmseos.fnal.gov/"${outdir}

echo "Using input path ${indir} and output path ${outdir}"
for f in `${lscommand} ${indir}`; do echo "Copying hist file "${filehead}${f}; xrdcp ${force}${filehead}$f $outdir; done

echo "Finished moving histograms! Remove local files when confident there were no errors"
