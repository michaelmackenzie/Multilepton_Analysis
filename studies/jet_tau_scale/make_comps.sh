#! /bin/bash

SELECTION=$1
if [[ "${SELECTION}" == "" ]]
then
    SELECTION="mutau"
fi

HISTS=$2
if [[ "${HISTS}" == "" ]]
then
    HISTS="nanoaods"
fi

root.exe -q -b "composition.C(\"${SELECTION}\", 2042, 2035, 2016, \"${HISTS}\")"
root.exe -q -b "composition.C(\"${SELECTION}\", 2042, 2035, 2017, \"${HISTS}\")"
root.exe -q -b "composition.C(\"${SELECTION}\", 2042, 2035, 2018, \"${HISTS}\")"
root.exe -q -b "composition.C(\"${SELECTION}\", 3042, 3035, 2016, \"${HISTS}\")"
root.exe -q -b "composition.C(\"${SELECTION}\", 3042, 3035, 2017, \"${HISTS}\")"
root.exe -q -b "composition.C(\"${SELECTION}\", 3042, 3035, 2018, \"${HISTS}\")"
