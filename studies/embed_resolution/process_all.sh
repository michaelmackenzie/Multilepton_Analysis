#! /bin/bash

Help() {
    echo "Process MC resolution histogramming and plotting"
    echo " 1: Year list (default = \"2016 2017 2018\")"
    echo " 2: Optional lepton flavor (\"muon\" or \"electron\")"
    echo " 3: Optional samples (default = \"Embed MC Data\")"
    echo " 4: Use resolution corrections (default = 0)"
    echo " 5: Use scale corrections (default = 0)"
}

YEARS=$1
FLAVOR=$2
SAMPLES=$3
RESOLUTION=$4
SCALE=$5
PERIOD="-1"

if [[ "${YEARS}" == "-h" ]] || [[ "${YEARS}" == "--help" ]]; then
    Help
    exit
fi

if [[ "${YEARS}" == "" ]]; then
    YEARS="2016 2017 2018";
fi

if [[ "${SAMPLES}" == "" ]]; then
    SAMPLES="Embed MC Data";
fi

if [[ "${RESOLUTION}" == "" ]]; then
    RESOLUTION="0"
fi

if [[ "${SCALE}" == "" ]]; then
    SCALE="0"
fi

if [[ "${FLAVOR}" != "" ]]; then
    if [[ "${FLAVOR}" != "muon" ]] && [[ "${FLAVOR}" != "electron" ]]; then
        echo "Unknown flavor option ${FLAVOR}"
        Help
        exit
    fi
    echo "Only processing ${FLAVOR} measurements"
fi

for YEAR in $YEARS
do
    if [[ "${FLAVOR}" != "electron" ]]; then
        echo "Processing year ${YEAR} muon resolution"
        if [[ "${SAMPLES}" == *"Embed"* ]]; then
            root.exe -q -b "resolution.C(1, true, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        if [[ "${SAMPLES}" == *"MC"* ]]; then
            root.exe -q -b "resolution.C(2, true, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        if [[ "${SAMPLES}" == *"UL"* ]]; then
            root.exe -q -b "resolution.C(3, true, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        if [[ "${SAMPLES}" == *"Data"* ]]; then
            root.exe -q -b "resolution.C(0, true, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        root.exe -q -b "compare.C(true, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
    fi
    if [[ "${FLAVOR}" != "muon" ]]; then
        echo "Processing year ${YEAR} electron resolution"
        if [[ "${SAMPLES}" == *"Embed"* ]]; then
            root.exe -q -b "resolution.C(1, false, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        if [[ "${SAMPLES}" == *"MC"* ]]; then
            root.exe -q -b "resolution.C(2, false, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        if [[ "${SAMPLES}" == *"UL"* ]]; then
            root.exe -q -b "resolution.C(3, false, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        if [[ "${SAMPLES}" == *"Data"* ]]; then
            root.exe -q -b "resolution.C(0, false, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
        fi
        root.exe -q -b "compare.C(false, ${YEAR}, ${PERIOD}, ${RESOLUTION}, ${SCALE})"
    fi
done
