#! /bin/bash

#Test multipdf in combine


MODE=$1
NTOYS=$2
if [[ "${NTOYS}" == "" ]]; then
    NTOYS="1"
fi
ARGS="-M FitDiagnostics -t ${NTOYS} --cminDefaultMinimizerStrategy=0 --toysFrequentist --rMin -20. --rMax 20."
ARGS="${ARGS} --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstants --X-rtd MINIMIZER_multiMin_maskConstraints"
# ARGS="${ARGS} --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstants --X-rtd MINIMIZER_multiMin_maskConstraints"
# ARGS="-M FitDiagnostics -t ${NTOYS} --cminDefaultMinimizerStrategy=0 --toysFrequentist --rMin -20. --rMax 20. --freezeParameters cat"
if [[ "${MODE}" == "" ]]; then
    echo "Performing combine test with nominal multipdf example with Bernstein polynomials"
    root.exe -l -q -b "test_multipdf_combine.C(0)"
    echo combine -d combine_test_multipdf.txt ${ARGS}
    combine -d combine_test_multipdf.txt ${ARGS}
elif [[ "${MODE}" == "Cheb" ]]; then
    echo "Performing combine test with nominal multipdf example with Chebychev polynomials"
    CHEB_MODE=2
    root.exe -l -q -b "test_multipdf_combine.C(${CHEB_MODE})"
    echo combine -d combine_test_multipdf.txt ${ARGS}
    combine -d combine_test_multipdf.txt ${ARGS}
elif [[ "${MODE}" == "Hgg" ]]; then
    echo "Performing example combine test for H->gamma gamma"
    root.exe -l -q -b "combine_multipdf_example.C"
    echo combine -d combine_multipdf_example.txt ${ARGS}
    combine -d combine_multipdf_example.txt ${ARGS}
elif [[ "${MODE}" == "AltHgg" ]]; then
    echo "Performing altered example combine test for H->gamma gamma"
    root.exe -l -q -b "combine_multipdf_example_alt.C"
    echo combine -d combine_multipdf_example_alt.txt ${ARGS}
    combine -d combine_multipdf_example_alt.txt ${ARGS}
else
    echo "Unknown Mode ${MODE}"
    exit
fi

rm combine_logger.out
rm higgsCombineTest*.root
rm fitDiagnostics.root
