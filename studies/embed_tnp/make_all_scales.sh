#! /bin/bash

Help() {
    echo "Process all Embedding muon and electron ID/trigger TnP scale factors"
    echo "Options:"
    echo " 1: List of years to process, default = \"2016 2016_BF 2016_GH 2017 2018\""
    echo " 2: Specific flavor to process, e.g. \"muon\" or \"electron\", default = \"\""
}

YEARS=$1 #space separated list of years to process
FLAVOR=$2 #electron, muon, or nothing for both

if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2016_BF 2016_GH 2017 2018"
elif [[ "${YEARS}" == "-h" ]]
then
    Help
    exit
fi

#For each year, make ID1, ID2 (if it exists), then trigger scales due to the conditional probabilities
for YEAR in ${YEARS}
do
    if [[ "${FLAVOR}" != "electron" ]]
    then
        PERIOD="-1"
        if [[ "${YEAR}" == 2016_BF ]]
        then
            PERIOD="0"
            YEAR="2016"
        elif [[ "${YEAR}" == 2016_GH ]]
        then
            PERIOD="1"
            YEAR="2016"
        fi
        echo "Creating muon scale factor for ${YEAR}"
        root.exe -q -b "scale_factors.C(1, 1, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(1, 0, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(1, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(2, 1, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(2, 0, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(2, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(3, 1, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(3, 0, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(3, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(0, 1, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(0, 0, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(0, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(4, 1, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(4, 0, true, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(4, true, ${YEAR}, ${PERIOD})";
    fi
    if [[ "${FLAVOR}" != "muon" ]]
    then
        echo "Creating electron scale factors for ${YEAR}, ${PERIOD}"
        root.exe -q -b "scale_factors.C(1, 1, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(1, 0, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(1, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(3, 1, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(3, 0, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(3, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(0, 1, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(0, 0, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(0, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(4, 1, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(4, 0, false, ${YEAR}, ${PERIOD})";
        root.exe -q -b "combine_efficiencies.C(4, false, ${YEAR}, ${PERIOD})";
    fi
done

echo "Finished creating scale factor files"
