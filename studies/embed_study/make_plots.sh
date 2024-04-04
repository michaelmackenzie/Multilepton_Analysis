#! /bin/bash

YEARS=$1
SELECS=$2
TIGHT=$3
HISTDIR=$4
#process only histogram, combine, or reco step
SINGLESTEP=$5

if [[ "${YEARS}" == "" ]]
then
    YEAR="2018"
fi

if [[ "${SELECS}" == "" ]]
then
    SELEC="emu"
fi

if [[ "${TIGHT}" != "" ]]
then
    TIGHT=true
else
    TIGHT=false
fi

if [[ "${HISTDIR}" == "" ]]
then
    HISTDIR="nanoaods_02"
fi

echo "Processing for selection(s) \"${SELECS}\" and year(s) \"${YEARS}\""
for YEAR in $YEARS
do
    for SELEC in $SELECS
    do
        echo "Processing ${SELEC} ${YEAR}"
        if [[ "${SINGLESTEP}" == "histogram" ]] || [[ "${SINGLESTEP}" == "" ]]
        then
            root.exe -q -b "make_hists.C(\"${SELEC}\", false, ${YEAR}, ${TIGHT})"
            root.exe -q -b "make_hists.C(\"${SELEC}\", true , ${YEAR}, ${TIGHT})"
        fi
        if [[ "${SINGLESTEP}" == "combine" ]] || [[ "${SINGLESTEP}" == "" ]]
        then
            root.exe -q -b "combine_plots.C(\"${SELEC}\", ${YEAR}, ${TIGHT})"
        fi
        if [[ "${SINGLESTEP}" == "reco" ]] || [[ "${SINGLESTEP}" == "" ]]
        then
            if [[ "${HISTDIR}" != "NONE" ]]; then
                root.exe -q -b "reco_plots.C(\"${SELEC}\", ${YEAR}, \"${HISTDIR}\")"
            fi
        fi
    done
done
