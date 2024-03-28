#! /bin/bash

Help() {
    echo "Process all QCD transfer factors"
    echo "Options:"
    echo " 1: histogram path from /store/user/${USER}/histograms/, default = nanoaods_qcd"
    echo " 2: list of years to process, default = \"2016 2017 2018 2016,2017,2018\""
    echo " 3: data selection, default = emu"
}

HISTPATH=$1
if [[ "${HISTPATH}" == "" ]]
then
    HISTPATH="nanoaods_qcd"
elif [[ "${HISTPATH}" == "-h" ]]
then
    Help
    exit
fi

YEARS=$2
if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2017 2018 2016,2017,2018"
fi

SELECTION=$3
if [[ "${SELECTION}" == "" ]]
then
    SELECTION="emu"
fi

#Fixed base histogram set
SET=8

#Create scale factors for each input year
for YEAR in ${YEARS}
do
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", ${SET}, {${YEAR}}, \"${HISTPATH}\")"
done

echo "Finished processing all years"
