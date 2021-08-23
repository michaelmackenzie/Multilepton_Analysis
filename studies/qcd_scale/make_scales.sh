#! /bin/bash

HISTPATH=$1

if [[ "${HISTPATH}" == "" ]]
then
    HISTPATH="nanoaods"
fi

SELECTION=$2

if [[ "${SELECTION}" == "" ]]
then
    SELECTION="emu"
fi

YEARS="2016 2017 2018"

for YEAR in ${YEARS}
do
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", 8, ${YEAR}, \"${HISTPATH}\")"
done

echo "Finished processing all years"
