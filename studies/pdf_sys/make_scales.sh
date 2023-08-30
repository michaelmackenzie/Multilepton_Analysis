#! /bin/bash

Help() {
    echo "Process theory PDF/scale uncertainties for Z->ll production"
    echo " 1: Indices to process (default = \"-1 -1002 -1003\")"
    echo " 2: Years to process (default = \"2016 2018\")"
    echo " 3: Skip second processing of scales (\"d\" to skip)"
}

INDICES=$1
YEARS=$2
SKIP=$3

if [[ "${INDICES}" == "--help" ]] || [[ "${INDICES}" == "-h" ]]; then
    Help
    exit
fi

if [[ "${INDICES}" == "" ]]; then
    INDICES="-1 -1002 -1003"
fi

if [[ "${YEARS}" == "" ]]; then
    YEARS="2016 2018"
fi

for YEAR in ${YEARS}; do
    for INDEX in ${INDICES}; do
        echo "Processing index ${INDEX} for year ${YEAR}"
        root.exe -q -b -l "scale_factors.C(${YEAR}, ${INDEX})"
        if [[ "${SKIP}" == "" ]]; then
            root.exe -q -b -l "scale_factors.C(${YEAR}, ${INDEX})"
        fi
    done
done

echo "Finished processing"
