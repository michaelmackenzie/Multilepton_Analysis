#! /bin/bash


Help() {
    echo "Process all j-->tau transfer/closure/bias factors"
    echo "Options:"
    echo " 1: Selection to process, default = \"mutau\""
    echo " 2: Histogram directory, default = \"nanoaods_jtt\""
    echo " 3: List of years to process, default = \"2016 2017 2018\""
    echo " 4: Processes to process, default = \"QCD QCD2 QCD3 QCD4 WJets WJets3 WJets4 Top3 ZJets\""
    echo " 5: Skip composition, default = \"\", pass anything to skip"
}

if [[ "$1" == "-h" ]]
then
    Help
    exit
fi

SELECTION=$1
HISTS=$2
YEARS=$3
PROCESSES=$4
SKIPCOMP=$5

if [[ "${SELECTION}" == "" ]]
then
    SELECTION="mutau"
fi

if [[ "${HISTS}" == "" ]]
then
    HISTS="nanoaods_jtt"
fi

if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2017 2018"
fi

if [[ "${PROCESSES}" == "" ]]
then
    PROCESSES="QCD QCD2 QCD3 QCD4 WJets WJets3 WJets4 Top3 ZJets"
fi


echo "Creating transfer factors for selection ${SELECTION}, histogram path ${HISTS}, and years ${YEARS}"

for PROCESS in $PROCESSES
do
    if [[ "${PROCESS}" == "QCD" ]] #QCD DR
    then
        SET1=1030
        SET2=3030
    elif [[ "${PROCESS}" == "QCD2" ]] #QCD SS region but wider lepton iso allowed (0 - 0.5)
    then
        SET1=1093
        SET2=3093
        PROCESS="QCD"
    elif [[ "${PROCESS}" == "QCD3" ]] #QCD SS region but high lepton iso (0.15 - 0.5)
    then
        SET1=1095
        SET2=3095
        PROCESS="QCD"
    elif [[ "${PROCESS}" == "QCD4" ]] #QCD OS region but high lepton iso (0.15 - 0.5)
    then
        SET1=95
        SET2=2095
        PROCESS="QCD"
    elif [[ "${PROCESS}" == "WJets" ]] #W+Jets DR
    then
        SET1=31
        SET2=2031
    elif [[ "${PROCESS}" == "WJets2" ]] #MC W+Jets set, for MC based scale factors: NOT USED, uses data-measured factors in 37/2037
    then
        SET1=37
        SET2=2037
        PROCESS="WJets"
    elif [[ "${PROCESS}" == "WJets3" ]] #MC W+Jets in nominal selection, for MC based scale factors (no non-closure) and bias tests
    then
        SET1=81
        SET2=2081
        PROCESS="WJets"
    elif [[ "${PROCESS}" == "WJets4" ]] #MC W+Jets in DR selection with MC measured weights, for MC based scale factors
    then
        SET1=88
        SET2=2088
        PROCESS="WJets"
    elif [[ "${PROCESS}" == "ZJets" ]] #MC Z+Jets in nominal selection, for MC based scale factors (no non-closure) and bias tests
    then
        SET1=81
        SET2=2081
        PROCESS="ZJets"
    elif [[ "${PROCESS}" == "WJetsDR" ]] #W+Jets DR region but without subtracting Top/QCD contributions
    then
        SET1=31
        SET2=2031
        PROCESS=""
    elif [[ "${PROCESS}" == "Top" ]] #Top DR: NOT USED
    then
        SET1=32
        SET2=2032
    elif [[ "${PROCESS}" == "Top2" ]] #MC top set, for MC based scale factors: NOT USED
    then
        SET1=38
        SET2=2038
        PROCESS="Top"
    elif [[ "${PROCESS}" == "Top3" ]] #MC tops in nominal selection, for MC based scale factors
    then
        SET1=82
        SET2=2082
        PROCESS="Top"
    else
        echo "Unknown process ${PROCESS}"
        continue
    fi
    for YEAR in $YEARS
    do
        echo "Making scales for process \"${PROCESS}\" year ${YEAR}, sets are ${SET1} and ${SET2}"
        root.exe -q -b "scale_factors.C(\"${SELECTION}\", \"${PROCESS}\", ${SET1}, ${SET2}, ${YEAR}, \"${HISTS}\")"
    done
done

echo "Finished all scale factors and (initial) non-closure corrections"

if [[ "${SKIPCOMP}" == "" ]]
then
    echo "Beginning composition measurements"

    for YEAR in $YEARS
    do
        root.exe -q -b "composition.C(\"${SELECTION}\", 2042, 2035, ${YEAR}, \"${HISTS}\")"
        root.exe -q -b "composition.C(\"${SELECTION}\", 3042, 3035, ${YEAR}, \"${HISTS}\")"
        root.exe -q -b "composition.C(\"${SELECTION}\",   42,   35, ${YEAR}, \"${HISTS}\")"
    done

    echo "Finished all composition measurements"
fi
