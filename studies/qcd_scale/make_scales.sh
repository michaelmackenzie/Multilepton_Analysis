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

#Create scale factors for each input year
for YEAR in ${YEARS}
do
    #Nominal measurement region (tight electron, loose muon)
    root.exe -q -b "scale_factors.C(\"${SELECTION}\",  8, {${YEAR}}, \"${HISTPATH}\")"
    #loose electron, loose muon region
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", 71, {${YEAR}}, \"${HISTPATH}\")"
    #loose electron, tight muon region
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", 70, {${YEAR}}, \"${HISTPATH}\")"
    #mutau_e region (tight electron)
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", 72, {${YEAR}}, \"${HISTPATH}\")"
    #etau_mu region (tight electron)
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", 73, {${YEAR}}, \"${HISTPATH}\")"
    #mutau_e region (tight muon)
    root.exe -q -b "scale_factors.C(\"${SELECTION}\", 74, {${YEAR}}, \"${HISTPATH}\")"
done

echo "Finished processing all years"
