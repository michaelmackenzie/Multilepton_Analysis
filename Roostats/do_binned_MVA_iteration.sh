#! /bin/bash
#selections include z/h + emu/etau/mutau, e.g. "zmutau"
SELECTION=$1
HISTSET=$2
YEAR=$3
HISTPATH=$4
if [[ "${SELECTION}" == "" ]]
then
    SELECTION="zmutau"
fi
if [[ "${HISTSET}" == "" ]]
then
    HISTSET="{8}"
fi
if [[ "${YEAR}" == "" ]]
then
    YEAR="{2016, 2017, 2018}"
fi

if [[ "${HISTPATH}" == "" ]]
then
    HISTPATH="nanoaods"
fi

echo "******************************************************************************************************************"
echo "Performing iteration for selection ${selection} set ${histSet} and year(s) ${year} with histogram path ${histPath}"
echo "Enter \".q\" between each step to close Canvases and move to next step"
echo "******************************************************************************************************************"

run_command="root.exe -b"
if [[ "$5" != "" ]]
then
    echo "*** Overriding .q continuing, running without prompts"
    run_command="root.exe -b -q"
fi

echo "*** Retrieving MVA histograms using the DataPlotter..."
$run_command "get_MVA_histogram.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, \"${HISTPATH}\")"

echo "Creating histogram PDFs using the MVA distributions..."
$run_command "fit_background_MVA_binned.C(${HISTSET}, \"${SELECTION}\", ${YEAR})"

echo "Calculating the upper limit..."
$run_command "calculate_UL_MVA_categories.C(${HISTSET}, \"${SELECTION}\", ${YEAR})"

echo "Finished iteration"
