#! /bin/bash
Help() {
    echo "Retrieve histograms, create PDFs, and create Combine cards/workspaces"
    echo "Usage:"
    echo " --selection     (-S): Selection (default \"zemu\")"
    echo " --years         (-y): Year list (default \"{2016,2017,2018}\")"
    echo " --hists         (-H): Histogram path (default \"nanoaods_mva_emu_dev\")"
    echo " --sets          (-s): Histogram sets (default \"{11,12,13}\")"
    echo " --skipretrieval (-R): Skip histogram retrieval step flag"
    echo " --skipcards     (-C): Skip card creation step flag, only merge cards"
    echo " --mctemplates   (-M): MC template cards"
    echo " --tag               : Tag for produced combine cards"
    echo " --dryrun            : Dry-run flag"
}

SELECTION="zemu"
YEAR="{2016,2017,2018}"
HISTPATH="nanoaods_mva_emu_dev"
HISTSETS="{11,12,13}"
SKIPRETRIEVAL=""
SKIPCREATION=""
DRYRUN=""
MCTEMPLATES=""
TAG=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--selection" ]] || [[ "${var}" == "-S" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SELECTION=${var}
    elif [[ "${var}" == "--years" ]] || [[ "${var}" == "-y" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        YEARS=${var}
    elif [[ "${var}" == "--hist" ]] || [[ "${var}" == "-H" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        HISTPATH=${var}
    elif [[ "${var}" == "--sets" ]] || [[ "${var}" == "-s" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        HISTSETS=${var}
    elif [[ "${var}" == "--skipretrieval" ]] || [[ "${var}" == "-R" ]]
    then
        SKIPRETRIEVAL="d"
    elif [[ "${var}" == "--skipcards" ]] || [[ "${var}" == "-C" ]]
    then
        SKIPCREATION="d"
    elif [[ "${var}" == "--mctemplates" ]] || [[ "${var}" == "-M" ]]
    then
        MCTEMPLATES="d"
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG="_${var}"
    elif [[ "${var}" == "--dryrun" ]]
    then
        DRYRUN="d"
    else
        echo "Unknown option ${var}"
        Help
        exit
    fi
    iarg=$((iarg + 1))
done


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

echo "Running with selection = ${SELECTION}, years = ${YEAR}, hist sets = ${HISTSETS}, tag = ${TAG}"
if [[ "${SKIPRETRIEVAL}" == "" ]]
then
    #get the BDT distributions
    echo "Retrieving histograms"
    ${HEAD} root.exe -q -b "get_bemu_histogram.C(${HISTSETS}, \"${SELECTION}\", ${YEAR}, \"${HISTPATH}\")"
fi

echo "Creating data cards"
#make the data cards
if [[ "${SKIPCREATION}" == "" ]]; then
    ${HEAD} root.exe -q -b "convert_bemu_to_combine.C(${HISTSETS}, \"${SELECTION}\", ${YEAR}, \"${TAG}\")"
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
FINALCARD="combine_bemu_${SELECTION}_${MCTEMPLATES}${HISTSTRING}${TAG}.txt"
for SET_I in $HISTLIST
do
    COMMAND="${COMMAND} mva_${SET_I}=combine_bemu_${SELECTION}_${MCTEMPLATES}${SET_I}${TAG}.txt"
done
if [[ "${DRYRUN}" != "" ]]; then
    ${HEAD} ${COMMAND} ${FINALCARD}
else
    echo ${COMMAND} ${FINALCARD}
    ${COMMAND} >| ${FINALCARD}
fi
