#! /bin/bash

HISTSET=$1
HISTPATH=$2

if [[ "${HISTSET}" == "" ]]
then
    HISTSET=8
fi
if [[ "${HISTPATH}" == "" ]]
then
    HISTPATH="nanoaods_dev"
fi

root.exe -q -b "scale_factors.C(true , ${HISTSET}, 2016, \"${HISTPATH}\")"
root.exe -q -b "scale_factors.C(true , ${HISTSET}, 2017, \"${HISTPATH}\")"
root.exe -q -b "scale_factors.C(true , ${HISTSET}, 2018, \"${HISTPATH}\")"
root.exe -q -b "scale_factors.C(false, ${HISTSET}, 2016, \"${HISTPATH}\")"
root.exe -q -b "scale_factors.C(false, ${HISTSET}, 2017, \"${HISTPATH}\")"
root.exe -q -b "scale_factors.C(false, ${HISTSET}, 2018, \"${HISTPATH}\")"
