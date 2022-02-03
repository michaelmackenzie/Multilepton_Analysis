#! /bin/bash

YEARS=$1 #space separated list of years to process
FLAVOR=$2 #electron, muon, or nothing for both

if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2017 2018"
fi

#For each year, make ID1, ID2 (if it exists), then trigger scales due to the conditional probabilities
for YEAR in ${YEARS}
do
    if [[ "${FLAVOR}" != "electron" ]]
    then
        echo "Creating muon scale factor for ${YEAR}"
        root.exe -q -b "scale_factors.C(1, 1, true, ${YEAR})";
        root.exe -q -b "scale_factors.C(1, 0, true, ${YEAR})";
        root.exe -q -b "combine_efficiencies.C(1, true, ${YEAR})";
        root.exe -q -b "scale_factors.C(2, 1, true, ${YEAR})";
        root.exe -q -b "scale_factors.C(2, 0, true, ${YEAR})";
        root.exe -q -b "combine_efficiencies.C(2, true, ${YEAR})";
        root.exe -q -b "scale_factors.C(0, 1, true, ${YEAR})";
        root.exe -q -b "scale_factors.C(0, 0, true, ${YEAR})";
        root.exe -q -b "combine_efficiencies.C(0, true, ${YEAR})";
    fi
    if [[ "${FLAVOR}" != "muon" ]]
    then
        echo "Creating electron scale factors for ${YEAR}"
        root.exe -q -b "scale_factors.C(1, 1, false, ${YEAR})";
        root.exe -q -b "scale_factors.C(1, 0, false, ${YEAR})";
        root.exe -q -b "combine_efficiencies.C(1, false, ${YEAR})";
        root.exe -q -b "scale_factors.C(0, 1, false, ${YEAR})";
        root.exe -q -b "scale_factors.C(0, 0, false, ${YEAR})";
        root.exe -q -b "combine_efficiencies.C(0, false, ${YEAR})";
    fi
done

echo "Finished creating scale factor files"
