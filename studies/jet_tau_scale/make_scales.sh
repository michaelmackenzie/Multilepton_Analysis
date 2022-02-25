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

PROCESSES="WJets QCD Top Top2"
YEARS="2016 2017 2018"

for PROCESS in $PROCESSES
do
    SET1=31
    SET2=2031
    if [[ "${PROCESS}" == "QCD" ]]
    then
        SET1=1030
        SET2=3030
    elif [[ "${PROCESS}" == "Top" ]]
    then
        SET1=32
        SET2=2032
    elif [[ "${PROCESS}" == "Top2" ]] #MC top set, for MC based scale factors
    then
        SET1=38
        SET2=2038
        PROCESS="Top"
    elif [[ "${PROCESS}" == "Top3" ]] #MC tops in nominal selection, for MC based scale factors
    then
        SET1=35
        SET2=2035
        PROCESS="Top"
    fi
    for YEAR in $YEARS
    do
        echo "Making scales for process ${PROCESS} year ${YEAR}, sets are ${SET1} and ${SET2}"
        root.exe -q -b "scale_factors.C(\"${SELECTION}\", \"${PROCESS}\", ${SET1}, ${SET2}, ${YEAR}, \"${HISTS}\")"
        echo "Making non-closure corrections for process ${PROCESS} year ${YEAR}, sets are ${SET1} and ${SET2}"
        root.exe -q -b "test_data_scale.C(\"${SELECTION}\", \"${PROCESS}\", ${SET1}, ${SET2}, ${YEAR}, \"${HISTS}\")"
    done
done

echo "Finished all scale factors and (initial) non-closure corrections"
