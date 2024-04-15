#! /bin/bash
Help() {
    echo "Retrieve histograms, create PDFs, and create Combine cards/workspaces"
    echo "Usage:"
    echo "1: Selection (e.g. \"zmutau\")"
    echo "2: Year list (e.g. \"{2016,2017,2018}\")"
    echo "3: Histogram path (e.g. \"nanoaods_mva_dev\")"
    echo "4: Hadronic channel histogram sets (e.g. \"{8}\")"
    echo "5: Leptonic channel histogram sets (e.g. \"{8}\")"
    echo "6: Optional, skip histogram retrieval step flag"
    echo "7: Optional, skip card creation step flag, only merge cards"
    echo "8: Optional parallel-processing flag for histogram sets"
    echo "9: Optional, dry-run flag"
}

SELECTION=$1
YEAR=$2
HISTPATH=$3
TAUSETS=$4
LEPSETS=$5
SKIPRETRIEVAL=$6
SKIPCREATION=$7
PARALLEL=$8
DRYRUN=$9


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
TAUSTRING=`echo ${TAUSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
LEPSTRING=`echo ${LEPSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
TAULIST=`echo ${TAUSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
LEPLIST=`echo ${LEPSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
LEPSIGNAL="${SELECTION}"
if [[ "${SELECTION}" == *"mutau"* ]]; then
    LEPSIGNAL="${LEPSIGNAL}_e";
else
    LEPSIGNAL="${LEPSIGNAL}_mu";
fi

[ ! -d datacards/${YEARSTRING} ] && mkdir -p datacards/${YEARSTRING}

echo "Running with selection = ${SELECTION} (with ${LEPSIGNAL}), years = ${YEAR}, tau sets = ${TAUSETS}, lep sets = ${LEPSETS}"
if [[ "${SKIPRETRIEVAL}" == "" ]]
then
    [ ! -d log ] && mkdir log
    #get the BDT distributions
    if [[ "${TAUSTRING}" != "" ]]; then
        echo "Retrieving hadronic tau histograms"
        for IYEAR in ${YEARLIST}; do
            for TAUSET in ${TAULIST}; do
                if [[ "${PARALLEL}" != "" ]]; then
                    echo "Starting processing to log/${SELECTION}_mva_had_${TAUSET}_${IYEAR}.log"
                    ${HEAD} root.exe -q -b "get_MVA_histogram.C({${TAUSET}}, \"${SELECTION}\", {${IYEAR}}, \"${HISTPATH}\", 1)" >| log/${SELECTION}_mva_had_${TAUSET}_${IYEAR}.log 2>&1 &
                else
                    ${HEAD} root.exe -q -b "get_MVA_histogram.C({${TAUSET}}, \"${SELECTION}\", {${IYEAR}}, \"${HISTPATH}\", 1)"
                fi
            done
            if [[ "${PARALLEL}" != "" ]]; then
                wait
            fi
        done
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        echo "Retrieving leptonic tau histograms"
        for IYEAR in ${YEARLIST}; do
            for LEPSET in ${LEPLIST}; do
                if [[ "${PARALLEL}" != "" ]]; then
                    echo "Starting processing to log/${SELECTION}_mva_lep_${LEPSET}_${IYEAR}.log"
                    ${HEAD} root.exe -q -b "get_MVA_histogram.C({${LEPSET}}, \"${SELECTION}\", {${IYEAR}}, \"${HISTPATH}\", -1)" >| log/${SELECTION}_mva_lep_${LEPSET}_${IYEAR}.log 2>&1 &
                else
                    ${HEAD} root.exe -q -b "get_MVA_histogram.C({${LEPSET}}, \"${SELECTION}\", {${IYEAR}}, \"${HISTPATH}\", -1)"
                fi
            done
            if [[ "${PARALLEL}" != "" ]]; then
                wait
            fi
        done
    fi
fi

echo "Creating data cards"
#make the data cards
if [[ "${SKIPCREATION}" == "" ]]; then
    if [[ "${TAUSTRING}" != "" ]]; then
        ${HEAD} root.exe -q -b "create_combine_cards.C(${TAUSETS}, \"${SELECTION}\", ${YEAR}, 1)"
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        ${HEAD} root.exe -q -b "create_combine_cards.C(${LEPSETS}, \"${SELECTION}\", ${YEAR}, -1)"
    fi
fi

echo "Processing year list ${YEARLIST}"
#Iterate through list of years, combining cards
YEARTAUMERGE="combineCards.py"
YEARLEPMERGE="combineCards.py"
for YEAR_I in ${YEARLIST}
do
    echo "Processing year ${YEAR_I}"
    cd datacards/${YEAR_I}/
    #iterate through each histogram set to build the hadronic tau combined card
    COMMAND="combineCards.py"
    FINALTAUCARD="combine_mva_${SELECTION}_${TAUSTRING}_${YEAR_I}.txt"
    FINALLEPCARD="combine_mva_${LEPSIGNAL}_${LEPSTRING}_${YEAR_I}.txt"
    FINALCARD="combine_mva_total_${SELECTION}_had_${TAUSTRING}_lep_${LEPSTRING}_${YEAR_I}.txt"
    if [[ "${TAUSTRING}" != "" ]]; then
        for SET_I in $TAULIST
        do
            COMMAND="${COMMAND} mva_${SET_I}=combine_mva_${SELECTION}_${SET_I}_${YEAR_I}.txt"
            if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp "combine_mva_${SELECTION}_${SET_I}_${YEAR_I}.root" ../${YEARSTRING}/; fi
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} ${FINALTAUCARD}
        elif [[ "${TAUSTRING}" != "${TAULIST}" ]]; then
            echo ${COMMAND} ${FINALTAUCARD}
            ${COMMAND} >| ${FINALTAUCARD}
        fi
        if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp ${FINALTAUCARD} ../${YEARSTRING}/; fi
    fi

    #iterate through each histogram set to build the leptonic tau combined card
    COMMAND="combineCards.py"
    if [[ "${LEPSTRING}" != "" ]]; then
        for SET_I in $LEPLIST
        do
            COMMAND="${COMMAND} mva_${SET_I}=combine_mva_${LEPSIGNAL}_${SET_I}_${YEAR_I}.txt"
            if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp "combine_mva_${LEPSIGNAL}_${SET_I}_${YEAR_I}.root" ../${YEARSTRING}/; fi
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} ${FINALLEPCARD}
        elif [[ "${LEPSTRING}" != "${LEPLIST}" ]]; then
            echo ${COMMAND} ${FINALLEPCARD}
            ${COMMAND} >| ${FINALLEPCARD}
        fi
        if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp ${FINALLEPCARD} ../${YEARSTRING}/; fi
    fi

    if [[ "${TAUSTRING}" != "" ]] && [[ "${LEPSTRING}" != "" ]]; then
        #combine the hadronic and leptonic channels
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} combineCards.py had=${FINALTAUCARD} lep=${FINALLEPCARD} ${FINALCARD}
        else
            echo combineCards.py had=${FINALTAUCARD} lep=${FINALLEPCARD} ${FINALCARD}
            combineCards.py had=${FINALTAUCARD} lep=${FINALLEPCARD} >| ${FINALCARD}
        fi
        ${HEAD} cp ${FINALCARD} ../${YEARSTRING}/
    fi

    YEARTAUMERGE="${YEARTAUMERGE} y${YEAR_I}=${FINALTAUCARD}"
    YEARLEPMERGE="${YEARLEPMERGE} y${YEAR_I}=${FINALLEPCARD}"
    cd ../..
done

#if only one year, no need for merging
if [[ "${YEARSTRING}" == "${YEARLIST}" ]]; then
    exit
fi

echo "Merging total year cards per set"
cd "datacards/${YEARSTRING}/"
#Merge each set for the full year list version
if [[ "${TAUSTRING}" != "" ]]; then
    for SET_I in $TAULIST
    do
        COMMAND="combineCards.py "
        for YEAR_I in $YEARLIST
        do
            COMMAND="${COMMAND} year_${YEAR_I}=combine_mva_${SELECTION}_${SET_I}_${YEAR_I}.txt"
            ${HEAD} cp ../${YEAR_I}/combine_mva_${SELECTION}_${SET_I}_${YEAR_I}.txt ./
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} "combine_mva_${SELECTION}_${SET_I}_${YEARSTRING}.txt"
        else
            echo ${COMMAND}  "combine_mva_${SELECTION}_${SET_I}_${YEARSTRING}.txt"
            ${COMMAND} >| "combine_mva_${SELECTION}_${SET_I}_${YEARSTRING}.txt"
        fi
    done
fi
if [[ "${LEPSTRING}" != "" ]]; then
    for SET_I in $LEPLIST
    do
        COMMAND="combineCards.py "
        for YEAR_I in $YEARLIST
        do
            COMMAND="${COMMAND} year_${YEAR_I}=combine_mva_${LEPSIGNAL}_${SET_I}_${YEAR_I}.txt"
            ${HEAD} cp ../${YEAR_I}/combine_mva_${LEPSIGNAL}_${SET_I}_${YEAR_I}.txt ./
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} "combine_mva_${LEPSIGNAL}_${SET_I}_${YEARSTRING}.txt"
        else
            echo ${COMMAND} "combine_mva_${LEPSIGNAL}_${SET_I}_${YEARSTRING}.txt"
            ${COMMAND} >| "combine_mva_${LEPSIGNAL}_${SET_I}_${YEARSTRING}.txt"
        fi
    done
fi

#Combine the runs from each year
echo "Merging total year cards for all sets"
#Use the last processed cards to create the total card names
FINALTAUCARD="combine_mva_${SELECTION}_${TAUSTRING}_${YEARSTRING}.txt"
FINALLEPCARD="combine_mva_${LEPSIGNAL}_${LEPSTRING}_${YEARSTRING}.txt"
FINALCARD="combine_mva_total_${SELECTION}_had_${TAUSTRING}_lep_${LEPSTRING}_${YEARSTRING}.txt"

if [[ "${DRYRUN}" != "" ]]; then
    if [[ "${TAUSTRING}" != "" ]]; then
        ${HEAD} ${YEARTAUMERGE} ${FINALTAUCARD}
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        ${HEAD} ${YEARLEPMERGE} ${FINALLEPCARD}
    fi
    if [[ "${TAUSTRING}" != "" ]] && [[ "${LEPSTRING}" != "" ]]; then
        ${HEAD} combineCards.py had=${FINALTAUCARD} lep=${FINALLEPCARD} ${FINALCARD}
    fi
else
    if [[ "${TAUSTRING}" != "" ]]; then
        ${YEARTAUMERGE} >| ${FINALTAUCARD}
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        ${YEARLEPMERGE} >| ${FINALLEPCARD}
    fi
    if [[ "${TAUSTRING}" != "" ]] && [[ "${LEPSTRING}" != "" ]]; then
        combineCards.py had=${FINALTAUCARD} lep=${FINALLEPCARD} >| ${FINALCARD}
    fi
fi
