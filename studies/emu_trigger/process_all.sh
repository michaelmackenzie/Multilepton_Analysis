#! /bin/bash

SIGNALS=$1
YEARS=$2

if [[ "${SIGNALS}" == "" ]]; then
    SIGNALS="ZETau ZMuTau ZEMu"
fi

if [[ "${YEARS}" == "" ]]; then
    YEARS="2016 2017 2018"
fi

for YEAR in ${YEARS}
do
    for SIGNAL in ${SIGNALS}
    do
        root.exe -q -b "compare.C(\"${SIGNAL}\", ${YEAR}, true)"
    done
done

echo "Finished processing"
