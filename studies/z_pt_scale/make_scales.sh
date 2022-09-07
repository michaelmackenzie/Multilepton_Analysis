#! /bin/bash

HISTSET=$1
HISTPATH=$2
YEARS=$3

if [[ "${HISTSET}" == "" ]]
then
    HISTSET=7
fi
if [[ "${HISTPATH}" == "" ]]
then
    HISTPATH="nanoaods_zpt"
fi
if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2017 2018"
fi

for YEAR in ${YEARS}
do
    echo "Processing year ${YEAR} set ${HISTSET} with path ${HISTPATH}"
    root.exe -q -b "scale_factors.C(true , ${HISTSET}, ${YEAR}, \"${HISTPATH}\")"
    root.exe -q -b "scale_factors.C(false, ${HISTSET}, ${YEAR}, \"${HISTPATH}\")"
done

echo "Finished processing"
