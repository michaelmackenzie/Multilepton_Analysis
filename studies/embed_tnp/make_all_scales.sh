#! /bin/bash

Help() {
    echo "Process all Embedding muon and electron ID/trigger TnP scale factors"
    echo "Options:"
    echo " 1: List of years to process, default = \"2016 2016_BF 2016_GH 2017 2018\""
    echo " 2: Specific flavor to process, e.g. \"muon\" or \"electron\", default = \"\""
    echo " 3: Scale factor modes to process, e.g. \"1 0\", default = \"1 2 3 0 4\""
    echo " 4: Data/Embed/MC flag, default = \"\""
}

YEARS=$1 #space separated list of years to process
FLAVOR=$2 #electron, muon, or nothing for both
MODES=$3 #space separated list of scale factor modes to process
DATA=$4

if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2016_BF 2016_GH 2017 2018"
elif [[ "${YEARS}" == "-h" ]]
then
    Help
    exit
fi

if [[ "${FLAVOR}" != "" ]] && [[ "${FLAVOR}" != "muon" ]] && [[ "${FLAVOR}" != "electron" ]]
then
    echo "Unknown flavor \"${FLAVOR}\""
    Help
    exit
fi

if [[ "${MODES}" == "" ]]
then
    MODES="1 2 3 0 4"
fi

if [[ "${DATA}" != "" ]]
then
    echo "Using data processing flag ${DATA}"
fi

#For each year, make ID1, ID2 (if it exists), then trigger scales due to the conditional probabilities
for YEAR in ${YEARS}
do
    PERIOD="-1"
    if [[ "${YEAR}" == 2016_BF ]]
    then
        PERIOD="0"
        YEAR="2016"
    elif [[ "${YEAR}" == 2016_GH ]]
    then
        PERIOD="1"
        YEAR="2016"
    elif [[ "${YEAR}" == 2018_ABC ]]
    then
        PERIOD="0"
        YEAR="2018"
    elif [[ "${YEAR}" == 2018_D ]]
    then
        PERIOD="1"
        YEAR="2018"
    fi
    for MODE in ${MODES}
    do
        if [[ "${FLAVOR}" != "electron" ]]
        then
            echo "Creating muon scale factor for ${YEAR}, period ${PERIOD}, mode ${MODE}"
            if [[ "${DATA}" == "" ]] || [[ "${DATA}" == "Embed" ]]; then root.exe -q -b "scale_factors.C(${MODE}, 1, true, ${YEAR}, ${PERIOD})"; fi
            if [[ "${DATA}" == "" ]] || [[ "${DATA}" == "Data"  ]]; then root.exe -q -b "scale_factors.C(${MODE}, 0, true, ${YEAR}, ${PERIOD})"; fi
            # No period dependent MC
            if [[ "${DATA}" == "" ]] || [[ "${DATA}" == "MC"    ]]; then root.exe -q -b "scale_factors.C(${MODE}, 2, true, ${YEAR})"; fi
            root.exe -q -b "combine_efficiencies.C(${MODE}, true, ${YEAR}, ${PERIOD})";
        fi
        if [[ "${FLAVOR}" != "muon" ]]
        then
            echo "Creating electron scale factors for ${YEAR}, period ${PERIOD}, mode ${MODE}"
            if [[ "${DATA}" == "" ]] || [[ "${DATA}" == "Embed" ]]; then root.exe -q -b "scale_factors.C(${MODE}, 1, false, ${YEAR}, ${PERIOD})"; fi
            if [[ "${DATA}" == "" ]] || [[ "${DATA}" == "Data"  ]]; then root.exe -q -b "scale_factors.C(${MODE}, 0, false, ${YEAR}, ${PERIOD})"; fi
            # No period dependent MC
            if [[ "${DATA}" == "" ]] || [[ "${DATA}" == "MC"    ]]; then root.exe -q -b "scale_factors.C(${MODE}, 2, false, ${YEAR})"; fi
            root.exe -q -b "combine_efficiencies.C(${MODE}, false, ${YEAR}, ${PERIOD})";
        fi
    done
    #perform validation checks
    if [[ "${FLAVOR}" != "electron" ]]
    then
        root.exe -q -b "validation.C(true, ${YEAR}, ${PERIOD})"
    fi
    if [[ "${FLAVOR}" != "muon" ]]
    then
        root.exe -q -b "validation.C(false, ${YEAR}, ${PERIOD})"
    fi
done

echo "Finished creating scale factor files"
