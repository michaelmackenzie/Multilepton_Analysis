#! /bin/bash
Help() {
    echo "Retrieve histograms, create PDFs, and create Combine cards/workspaces"
    echo "Usage:"
    echo "1: Selection (e.g. \"zemu\")"
    echo "2: Year list (e.g. \"{2016,2017,2018}\")"
    echo "3: Histogram path (e.g. \"nanoaods_mva_dev\")"
    echo "4: Histogram sets (e.g. \"{8}\")"
    echo "5: Optional, skip histogram retrieval step flag"
    echo "6: Optional, skip card creation step flag, only merge cards"
    echo "7: Optional, dry-run flag"
    echo "8: MC template cards"
}

SELECTION=$1
YEAR=$2
HISTPATH=$3
HISTSETS=$4
SKIPRETRIEVAL=$5
SKIPCREATION=$6
DRYRUN=$7
MCTEMPLATES=$8


if [[ "${SELECTION}" == "" ]] || [[ "${SELECTION}" == "-h" ]] || [[ "${SELECTION}" == "--help" ]]; then
    Help
    exit
fi

if [[ "${DRYRUN}" != "" ]]; then
    HEAD="echo "
else
    HEAD=""
fi

YEARSTRING=`echo ${YEAR} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
YEARLIST=`echo ${YEAR} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
HISTSTRING=`echo ${HISTSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
HISTLIST=`echo ${HISTSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`

if [[ "${YEARSTRING}" == "" ]]; then
    echo "No years identified!"
    exit
fi
if [[ "${HISTSTRING}" == "" ]]; then
    echo "No histogram sets identified!"
    exit
fi
if [[ "${MCTEMPLATES}" != "" ]]; then
    MCTEMPLATES="mc_"
fi

echo "Running with selection = ${SELECTION}, years = ${YEAR}, hist sets = ${HISTSETS}"
if [[ "${SKIPRETRIEVAL}" == "" ]]
then
    #get the BDT distributions
    echo "Retrieving histograms"
    ${HEAD} root.exe -q -b "get_bemu_histogram.C(${HISTSETS}, \"${SELECTION}\", ${YEAR}, \"${HISTPATH}\")"
fi

echo "Creating data cards"
#make the data cards
if [[ "${SKIPCREATION}" == "" ]]; then
    ${HEAD} root.exe -q -b "convert_bemu_to_combine.C(${HISTSETS}, \"${SELECTION}\", ${YEAR})"
fi

if [[ "${HISTLIST}" == "${HISTSTRING}" ]]; then
    echo "Finished processing!"
    exit
fi

echo "Processing set list ${HISTLIST}"
#Iterate through list of years, combining cards
SETMERGE="combineCards.py"
cd datacards/${YEARSTRING}/
#iterate through each histogram set to build the combined card
COMMAND="combineCards.py"
FINALCARD="combine_bemu_${SELECTION}_${MCTEMPLATES}${HISTSTRING}.txt"
for SET_I in $HISTLIST
do
    COMMAND="${COMMAND} mva_${SET_I}=combine_bemu_${SELECTION}_${MCTEMPLATES}${SET_I}.txt"
done
if [[ "${DRYRUN}" != "" ]]; then
    ${HEAD} ${COMMAND} ${FINALCARD}
else
    echo ${COMMAND} ${FINALCARD}
    ${COMMAND} >| ${FINALCARD}
fi
