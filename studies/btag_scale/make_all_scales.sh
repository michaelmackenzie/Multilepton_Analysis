#! /bin/bash

years="2016 2017 2018"
wps="0 1 2"
if [[ "$1" == "" ]]
then
    selection="mumu"
else
    selection="$1"
fi

for year in $years
do
    echo "Making scale factors for year = ${year}"
    for wp in $wps
    do
	root.exe -q -b "scale_factors.C(\"${selection}\", 7, ${year}, ${wp})"
    done
    echo "Finished year ${year}"
done

echo "Finished!"
    
