#! /bin/bash
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
    histPath="../histograms/nanoaods_dev/"
fi

echo "******************************************************************************************************************"
echo "Performing iteration for selection ${selection} set ${histSet} and year(s) ${year} with histogram path ${histPath}"
echo "Enter \".q\" between each step to close Canvases and move to next step"
echo "******************************************************************************************************************"

run_command="root.exe"
if [[ "$5" != "" ]]
then
    echo "Overriding .q continuing, running in batch mode!"
    run_command="root.exe -b -q"
fi

echo "Creating the signal PDF..."
$run_command "fit_signal_MVA.C(${histSet}, \"${selection}\", ${year}, \"${histPath}\")"

echo "Creating background trees..."
$run_command "create_background_trees.C(${histSet}, ${year}, \"${histPath}\", \"${selection}\")"

echo "Fitting background MC distribution..."
$run_command "fit_background_MVA.C(${histSet}, \"${selection}\", ${year})"

echo "Calculating the upper limit..."
$run_command "calculate_UL_MVA.C(${histSet}, \"${selection}\", ${year})"

echo "Finished!"
