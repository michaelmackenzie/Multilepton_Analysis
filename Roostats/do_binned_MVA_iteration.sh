#! /bin/bash
#selections include z/h + emu/etau/mutau, e.g. "zmutau"
selection=$1
histSet=$2
year=$3
histPath=$4
if [[ "${selection}" == "" ]]
then
    selection="zmutau"
fi
if [[ "${histSet}" == "" ]]
then
    histSet=8
fi
if [[ "${year}" == "" ]]
then
    year="{2016}"
elif [[ "${year}" == "all" ]]
then
    year="{2016, 2017, 2018}"
else
    year="{${year}}"
fi

if [[ "${histPath}" == "" ]]
then
    histPath="nanoaods_mva"
fi

echo "******************************************************************************************************************"
echo "Performing iteration for selection ${selection} set ${histSet} and year(s) ${year} with histogram path ${histPath}"
echo "Enter \".q\" between each step to close Canvases and move to next step"
echo "******************************************************************************************************************"

run_command="root.exe"
if [[ "$5" != "" ]]
then
    echo "*** Overriding .q continuing, running in batch mode!"
    run_command="root.exe -b -q"
fi

echo "*** Retrieving MVA histograms using the DataPlotter..."
$run_command "get_MVA_histogram.C(${histSet}, \"${selection}\", ${year}, \"${histPath}\")"
if [[ "${selection}" == *"tau" ]]
then
    echo "*** Retrieving leptonic decay MVA histograms using the DataPlotter..."
    if [[ "${selection}" == *"etau" ]]
    then
        $run_command "get_MVA_histogram.C(${histSet}, \"${selection}_mu\", ${year}, \"${histPath}\")"
    else
        $run_command "get_MVA_histogram.C(${histSet}, \"${selection}_e\", ${year}, \"${histPath}\")"
    fi
fi

echo "Creating histogram PDFs using the MVA distributions..."
$run_command "fit_background_MVA_binned.C(${histSet}, \"${selection}\", ${year})"

echo "Calculating the upper limit..."
$run_command "calculate_UL_MVA_categories.C(${histSet}, \"${selection}\", ${year})"

echo "Finished!"
