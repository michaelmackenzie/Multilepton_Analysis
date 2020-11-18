#! /bin/bash
histSet=$1
year=$2
histPath="$3"
if [[ "$1" == "" ]]
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

if [[ "$3" == "" ]]
then
    histPath="../histograms/nanoaods_dev/"
fi
run_command="root.exe"
if [[ "$4" != "" ]]
then
    echo "Overriding .q continuing, running in batch mode!"
    run_command="root.exe -b -q"
fi

echo "*****************************************************************************************************"
echo "Performing iteration for set ${histSet} and year(s) ${year} with histogram path ${histPath}"
echo "Enter \".q\" between each step to close Canvases and move to next step"
echo "*****************************************************************************************************"

echo "Getting same flavor histograms..."
$run_command "create_same_flavor_histograms.C(${histSet}, ${year}, \"${histPath}\")"

echo "Fitting same flavor muon histograms..."
$run_command "fit_same_flavor.C(${histSet}, ${year}, true)"

echo "Fitting same flavor electron histograms..."
$run_command "fit_same_flavor.C(${histSet}, ${year}, false)"

echo "Morphing same flavor histograms to make signal PDF..."
$run_command "morph_signal.C(${histSet}, ${year})"

echo "Creating background trees..."
$run_command "create_background_trees.C(${histSet}, ${year}, \"${histPath}\")"

echo "Fitting background MC distribution..."
$run_command "fit_background.C(${histSet}, ${year})"

echo "Calculating the upper limit..."
$run_command "calculate_UL.C(${histSet}, ${year})"

echo "Finished!"
