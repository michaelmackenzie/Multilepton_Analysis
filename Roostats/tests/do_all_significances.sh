#! /bin/bash

YEARS="2016_2017_2018"
HADSETS="25_26_27_28"
LEPSETS="25_26_27"
TAG="v09j"

USEWS=$1
if [[ "${USEWS}" == "" ]]; then
    CARDEND=".txt"
else
    CARDEND="_workspace.root"
fi

for SIGNAL in "zmutau" "zetau"
do
    # Get the leptonic category name
    LEP="${SIGNAL}_mu"
    if [[ "${SIGNAL}" == "zmutau" ]]; then
        LEP="${SIGNAL}_e"
    fi

    for OPT in "" "--pval"; do
        CARDOPT=""
        if [[ "${OPT}" != "" ]]; then
            echo "Running with fit option ${OPT}"
            CARDOPT="--fitarg ${OPT}"
        fi
        CARD="combine_mva_${SIGNAL}_${HADSETS}_${YEARS}${CARDEND}"
        ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/significance.sh ${CARD} -r 50 --tag ${SIGNAL}_h_${TAG} ${CARDOPT}
        CARD="combine_mva_${LEP}_${LEPSETS}_${YEARS}${CARDEND}"
        ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/significance.sh ${CARD} -r 50 --tag ${LEP}_${TAG} ${CARDOPT}
        CARD="combine_mva_total_${SIGNAL}_had_${HADSETS}_lep_${LEPSETS}_${YEARS}${CARDEND}"
        ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/significance.sh ${CARD} -r 50 --tag ${SIGNAL}_${TAG} ${CARDOPT}
    done
done

echo "Done processing significances"
