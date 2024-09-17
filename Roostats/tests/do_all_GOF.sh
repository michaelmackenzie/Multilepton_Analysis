#! /bin/bash
# Perform all relevant goodness-of-fit tests

Help() {
    echo "Do goodness of fit studies"
    echo "Usage: do_all_GOF.sh --signal <signal> [options]"
    echo "Options:"
    echo " --tag            : Tag for output results (default = v01)"
    echo " --signal         : Signal to test (zetau or zmutau)"
    echo " --gof-arg        : Arguments passed to do_goodness_of_fit.sh"
    echo " --help      (-h ): Print this information"
}

TAG="v01"
SIGNAL=""
GOFARG=""
YEARS="2016_2017_2018"
HADSETS="25_26_27_28"
LEPSETS="25_26_27"

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]; then
        Help
        exit
    elif [[ "${var}" == "--gof-arg" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GOFARG=${var}
    elif [[ "${var}" == "--tag" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--signal" ]]; then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SIGNAL=${var}
    else
        echo "Arguments aren't configured correctly! (at ${var}, CARD=${CARD})"
        Help
        exit
    fi
    iarg=$((iarg + 1))
done

if [[ "${SIGNAL}" == "" ]]; then
    echo "No signal provided"
    Help
    exit
elif [[ "${SIGNAL}" != "zetau" ]] && [[ "${SIGNAL}" != "zmutau" ]]; then
    echo "Unknown signal ${SIGNAL}"
    Help
    exit
fi

# Get the leptonic category name
LEP="${SIGNAL}_mu"
if [[ "${SIGNAL}" == "zmutau" ]]; then
    LEP="${SIGNAL}_e"
fi

# Perform relevant GOF tests
CARD="combine_mva_${SIGNAL}_${HADSETS}_${YEARS}.txt"
echo "Performing hadronic region tests with card ${CARD}"
time ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/do_goodness_of_fit.sh ${CARD} --tag ${SIGNAL}_h_${TAG} -g 50 ${GOFARG}
CARD="combine_mva_${LEP}_${LEPSETS}_${YEARS}.txt"
echo "Performing leptonic region tests with card ${CARD}"
time ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/do_goodness_of_fit.sh ${CARD} --tag ${LEP}_${TAG} -g 50 ${GOFARG}
CARD="combine_mva_total_${SIGNAL}_had_${HADSETS}_lep_${LEPSETS}_${YEARS}.txt"
echo "Performing total tests with card ${CARD}"
time ${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tests/do_goodness_of_fit.sh ${CARD} --tag ${SIGNAL}_${TAG} -g 20 ${GOFARG}

echo "Finished GOF tests"
