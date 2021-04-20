#! /bin/bash

SELECTION=$1
HISTDIR=$2
HISTSET=$3
YEARS=$4
WPS="0 1 2"
if [[ "$SELECTION" == "" ]]
then
    SELECTION="mumu"
fi
if [[ "$HISTDIR" == "" ]]
then
    HISTDIR="nanoaods_dev"
fi
if [[ "$HISTSET" == "" ]]
then
    HISTSET=7
fi
if [[ "$YEARS" == "" ]]
then
    YEARS="2016 2017 2018"
fi

echo "Using YEARS = ${YEARS}, WPS = ${WPS}, SELECTION = ${SELECTION}, HISTSET = ${HISTSET}, and HISTDIR = ${HISTDIR}"

for year in $YEARS
do
    echo "Making scale factors for year = ${year}"
    for wp in $WPS
    do
        root.exe -q -b "scale_factors.C(\"${SELECTION}\", ${HISTSET}, ${year}, ${wp})"
    done
    echo "Finished year ${year}"
done

echo "Finished all years and working points "
