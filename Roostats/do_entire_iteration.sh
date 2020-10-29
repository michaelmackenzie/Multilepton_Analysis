#! /bin/bash
histSet=$1
year=$2
histPath="$3"
if [[ "$1" == "" ]]
then
    histSet=8
fi
if [[ "$2" == "" ]]
then
    year=2016
fi
if [[ "$3" == "" ]]
then
    histPath="../histograms/nanoaods_dev/"
fi

echo "**************************************************************************************************"
echo "Performing iteration for set ${histSet} and year ${year} with histogram path ${histPath}"
echo "Enter \".q\" between each step to close Canvases and move to next step"
echo "**************************************************************************************************"

echo "Getting same flavor histograms..."
root.exe "create_same_flavor_histograms.C(${histSet}, \"${histPath}\", ${year})"

echo "Fitting same flavor muon histograms..."
root.exe "fit_same_flavor.C(${histSet}, ${year}, true)"

echo "Fitting same flavor electron histograms..."
root.exe "fit_same_flavor.C(${histSet}, ${year}, false)"

echo "Morphing same flavor histograms to make signal PDF..."
root.exe "morph_signal.C(${histSet}, ${year})"

echo "Creating background trees..."
root.exe "create_background_trees.C(${histSet}, ${year}, \"${histPath}\")"

echo "Fitting background MC distribution..."
root.exe "fit_background.C(${histSet}, ${year})"

echo "Calculating the upper limit..."
root.exe "calculate_UL.C(${histSet}, ${year})"

echo "Finished!"
