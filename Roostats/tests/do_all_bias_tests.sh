#! /bin/bash
# Perform the standard bias tests for the Run 2 tau channel searches

DONOSTATS="$1"
ARGUMENTS="$2"
TAG="$3"
SKIPCOMBINE="$4"
HADSETS="25_27_28"
LEPSETS="25_27"

if [[ "${DONOSTATS}" != "" ]]; then
    ARGUMENTS="${ARGUMENTS} --freezeNuisanceGroups autoMCStats"
    TAG="${TAG}.nomcstats"
fi

#path to bias test script
COMMAND="${CMSSW_BASE}/src/CLFVAnalysis/Roostats/perform_bias_tests.sh"

${COMMAND} "combine_mva_zmutau_${HADSETS}_2016_2017_2018.txt" "zmutau_h.${HADSETS}${TAG}.r_0" "0" "100" "200" "${ARGUMENTS}" "${SKIPCOMBINE}"
${COMMAND} "combine_mva_zmutau_e_${LEPSETS}_2016_2017_2018.txt" "zmutau_e.${LEPSETS}${TAG}.r_0" "0" "100" "200" "${ARGUMENTS}" "${SKIPCOMBINE}"
${COMMAND} "combine_mva_total_zmutau_had_${HADSETS}_lep_${LEPSETS}_2016_2017_2018.txt" "zmutau.had_${HADSETS}_lep_${LEPSETS}${TAG}.r_0" "0" "100" "100" "${ARGUMENTS}" "${SKIPCOMBINE}"
${COMMAND} "combine_mva_zetau_${HADSETS}_2016_2017_2018.txt" "zetau_h.${HADSETS}${TAG}.r_0" "0" "100" "200" "${ARGUMENTS}" "${SKIPCOMBINE}"
${COMMAND} "combine_mva_zetau_mu_${LEPSETS}_2016_2017_2018.txt" "zetau_mu.${LEPSETS}${TAG}.r_0" "0" "100" "200" "${ARGUMENTS}" "${SKIPCOMBINE}"
${COMMAND} "combine_mva_total_zetau_had_${HADSETS}_lep_${LEPSETS}_2016_2017_2018.txt" "zetau.had_${HADSETS}_lep_${LEPSETS}${TAG}.r_0" "0" "100" "100" "${ARGUMENTS}" "${SKIPCOMBINE}"

echo "Finished all bias measurements"
