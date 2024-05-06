#! /bin/bash
Help() {
    echo "Retrieve histograms, create PDFs, and create Combine cards/workspaces"
    echo "Usage: process_cards.sh -S <selection> -H <hist path> [options]"
    echo " --selection     (-S  ): Selection (e.g. \"zmutau\")"
    echo " --hists         (-H  ): Histogram path (e.g. \"nanoaods_mva_dev\")"
    echo " --years         (-y  ): Year list (default \"{2016,2017,2018}\")"
    echo " --hadsets       (-had): Histogram sets for hadronic taus (default \"{8}\")"
    echo " --lepsets       (-lep): Histogram sets for leptonic taus (default \"{8}\")"
    echo " --skipretrieval (-R  ): Skip histogram retrieval step flag"
    echo " --skipcards     (-C  ): Skip card creation step flag, only merge cards"
    echo " --parallel      (-p  ): Retrieve histograms in parallel"
    echo " --tag                 : Tag for produced combine cards (Not implemented)"
    echo " --dryrun              : Dry-run flag"
}

SELECTION=""
YEARS="{2016,2017,2018}"
HISTPATH=""
HADSETS="{25,26,27,28}"
LEPSETS="{25,26,27}"
SKIPRETRIEVAL=""
SKIPCREATION=""
PARALLEL=""
TAG=""
DRYRUN=""

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
    elif [[ "${var}" == "--hadsets" ]] || [[ "${var}" == "-had" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        HADSETS=${var}
    elif [[ "${var}" == "--lepsets" ]] || [[ "${var}" == "-lep" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        LEPSETS=${var}
    elif [[ "${var}" == "--parallel" ]] || [[ "${var}" == "-p" ]]
    then
        PARALLEL="d"
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


if [[ "${SELECTION}" == "" ]]; then
    echo "No selection provided!"
    Help
    exit
fi

if [[ "${HISTPATH}" == "" ]]; then
    echo "No histogram path provided!"
    Help
    exit
fi

if [[ "${DRYRUN}" != "" ]]; then
    HEAD="echo "
else
    HEAD=""
fi


YEARSTRING=`echo ${YEARS} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
YEARLIST=`echo ${YEARS} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
HADSTRING=`echo ${HADSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
LEPSTRING=`echo ${LEPSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
HADLIST=`echo ${HADSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
LEPLIST=`echo ${LEPSETS} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
LEPSIGNAL="${SELECTION}"
if [[ "${SELECTION}" == *"mutau"* ]]; then
    LEPSIGNAL="${LEPSIGNAL}_e";
else
    LEPSIGNAL="${LEPSIGNAL}_mu";
fi

[ ! -d datacards/${YEARSTRING} ] && mkdir -p datacards/${YEARSTRING}

echo "Running with selection = ${SELECTION} (with ${LEPSIGNAL}), years = ${YEARS}, hadronic sets = ${HADSETS}, leptonic sets = ${LEPSETS}"
if [[ "${SKIPRETRIEVAL}" == "" ]]
then
    [ ! -d log ] && mkdir log
    #get the BDT distributions
    if [[ "${HADSTRING}" != "" ]]; then
        echo "Retrieving hadronic tau histograms"
        for IYEAR in ${YEARLIST}; do
            for HADSET in ${HADLIST}; do
                if [[ "${PARALLEL}" != "" ]]; then
                    echo "Starting processing to log/${SELECTION}_mva_had_${HADSET}_${IYEAR}.log"
                    ${HEAD} root.exe -q -b "get_MVA_histogram.C({${HADSET}}, \"${SELECTION}\", {${IYEAR}}, \"${HISTPATH}\", 1)" >| log/${SELECTION}_mva_had_${HADSET}_${IYEAR}.log 2>&1 &
                else
                    ${HEAD} root.exe -q -b "get_MVA_histogram.C({${HADSET}}, \"${SELECTION}\", {${IYEAR}}, \"${HISTPATH}\", 1)"
                fi
            done
            if [[ "${PARALLEL}" != "" ]]; then
                wait
                for HADSET in ${HADLIST}; do
                    LOGFILE=log/${SELECTION}_mva_had_${HADSET}_${IYEAR}.log
                    ENDLINE=`cat ${LOGFILE} | tail -n 1`
                    if [[ "${ENDLINE}" != *"(int) 0"* ]]; then
                        echo "Hadronic set ${HADSET} had failure, see log file ${LOGFILE}"
                        exit
                    fi
                done
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
                for LEPSET in ${LEPLIST}; do
                    LOGFILE=log/${SELECTION}_mva_lep_${LEPSET}_${IYEAR}.log
                    ENDLINE=`cat ${LOGFILE} | tail -n 1`
                    if [[ "${ENDLINE}" != *"(int) 0"* ]]; then
                        echo "Leptonic set ${LEPSET} had failure, see log file ${LOGFILE}"
                        exit
                    fi
                done
            fi
        done
    fi
fi

echo "Creating data cards"
#make the data cards
if [[ "${SKIPCREATION}" == "" ]]; then
    if [[ "${HADSTRING}" != "" ]]; then
        ${HEAD} root.exe -q -b "create_combine_cards.C(${HADSETS}, \"${SELECTION}\", ${YEARS}, 1, \"${TAG}\")"
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        ${HEAD} root.exe -q -b "create_combine_cards.C(${LEPSETS}, \"${SELECTION}\", ${YEARS}, -1, \"${TAG}\")"
    fi
fi

echo "Processing year list ${YEARLIST}"
#Iterate through list of years, combining cards
YEARHADMERGE="combineCards.py"
YEARLEPMERGE="combineCards.py"
for YEAR_I in ${YEARLIST}
do
    echo "Processing year ${YEAR_I}"
    cd datacards/${YEAR_I}/
    #iterate through each histogram set to build the hadronic tau combined card
    COMMAND="combineCards.py"
    FINALHADCARD="combine_mva_${SELECTION}_${HADSTRING}${TAG}_${YEAR_I}.txt"
    FINALLEPCARD="combine_mva_${LEPSIGNAL}_${LEPSTRING}${TAG}_${YEAR_I}.txt"
    FINALCARD="combine_mva_total_${SELECTION}_had_${HADSTRING}_lep_${LEPSTRING}${TAG}_${YEAR_I}.txt"
    if [[ "${HADSTRING}" != "" ]]; then
        for SET_I in $HADLIST
        do
            COMMAND="${COMMAND} mva_${SET_I}=combine_mva_${SELECTION}_${SET_I}${TAG}_${YEAR_I}.txt"
            if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp "combine_mva_${SELECTION}_${SET_I}${TAG}_${YEAR_I}.root" ../${YEARSTRING}/; fi
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} ${FINALHADCARD}
        elif [[ "${HADSTRING}" != "${HADLIST}" ]]; then
            echo ${COMMAND} ${FINALHADCARD}
            ${COMMAND} >| ${FINALHADCARD}
        fi
        if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp ${FINALHADCARD} ../${YEARSTRING}/; fi
    fi

    #iterate through each histogram set to build the leptonic tau combined card
    COMMAND="combineCards.py"
    if [[ "${LEPSTRING}" != "" ]]; then
        for SET_I in $LEPLIST
        do
            COMMAND="${COMMAND} mva_${SET_I}=combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEAR_I}.txt"
            if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp "combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEAR_I}.root" ../${YEARSTRING}/; fi
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} ${FINALLEPCARD}
        elif [[ "${LEPSTRING}" != "${LEPLIST}" ]]; then
            echo ${COMMAND} ${FINALLEPCARD}
            ${COMMAND} >| ${FINALLEPCARD}
        fi
        if [[ "${YEARSTRING}" != "${YEARLIST}" ]]; then ${HEAD} cp ${FINALLEPCARD} ../${YEARSTRING}/; fi
    fi

    if [[ "${HADSTRING}" != "" ]] && [[ "${LEPSTRING}" != "" ]]; then
        #combine the hadronic and leptonic channels
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} combineCards.py had=${FINALHADCARD} lep=${FINALLEPCARD} ${FINALCARD}
        else
            echo combineCards.py had=${FINALHADCARD} lep=${FINALLEPCARD} ${FINALCARD}
            combineCards.py had=${FINALHADCARD} lep=${FINALLEPCARD} >| ${FINALCARD}
        fi
        ${HEAD} cp ${FINALCARD} ../${YEARSTRING}/
    fi

    YEARHADMERGE="${YEARHADMERGE} y${YEAR_I}=${FINALHADCARD}"
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
if [[ "${HADSTRING}" != "" ]]; then
    for SET_I in $HADLIST
    do
        COMMAND="combineCards.py "
        for YEAR_I in $YEARLIST
        do
            COMMAND="${COMMAND} year_${YEAR_I}=combine_mva_${SELECTION}_${SET_I}${TAG}_${YEAR_I}.txt"
            ${HEAD} cp ../${YEAR_I}/combine_mva_${SELECTION}_${SET_I}${TAG}_${YEAR_I}.txt ./
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} "combine_mva_${SELECTION}_${SET_I}${TAG}_${YEARSTRING}.txt"
        else
            echo ${COMMAND}  "combine_mva_${SELECTION}_${SET_I}${TAG}_${YEARSTRING}.txt"
            ${COMMAND} >| "combine_mva_${SELECTION}_${SET_I}${TAG}_${YEARSTRING}.txt"
        fi
    done
fi
if [[ "${LEPSTRING}" != "" ]]; then
    for SET_I in $LEPLIST
    do
        COMMAND="combineCards.py "
        for YEAR_I in $YEARLIST
        do
            COMMAND="${COMMAND} year_${YEAR_I}=combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEAR_I}.txt"
            ${HEAD} cp ../${YEAR_I}/combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEAR_I}.txt ./
        done
        if [[ "${DRYRUN}" != "" ]]; then
            ${HEAD} ${COMMAND} "combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEARSTRING}.txt"
        else
            echo ${COMMAND} "combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEARSTRING}.txt"
            ${COMMAND} >| "combine_mva_${LEPSIGNAL}_${SET_I}${TAG}_${YEARSTRING}.txt"
        fi
    done
fi

#Combine the runs from each year
echo "Merging total year cards for all sets"
#Use the last processed cards to create the total card names
FINALHADCARD="combine_mva_${SELECTION}_${HADSTRING}${TAG}_${YEARSTRING}.txt"
FINALLEPCARD="combine_mva_${LEPSIGNAL}_${LEPSTRING}${TAG}_${YEARSTRING}.txt"
FINALCARD="combine_mva_total_${SELECTION}_had_${HADSTRING}_lep_${LEPSTRING}${TAG}_${YEARSTRING}.txt"

if [[ "${DRYRUN}" != "" ]]; then
    if [[ "${HADSTRING}" != "" ]]; then
        ${HEAD} ${YEARHADMERGE} ${FINALHADCARD}
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        ${HEAD} ${YEARLEPMERGE} ${FINALLEPCARD}
    fi
    if [[ "${HADSTRING}" != "" ]] && [[ "${LEPSTRING}" != "" ]]; then
        ${HEAD} combineCards.py had=${FINALHADCARD} lep=${FINALLEPCARD} ${FINALCARD}
    fi
else
    if [[ "${HADSTRING}" != "" ]]; then
        ${YEARHADMERGE} >| ${FINALHADCARD}
    fi
    if [[ "${LEPSTRING}" != "" ]]; then
        ${YEARLEPMERGE} >| ${FINALLEPCARD}
    fi
    if [[ "${HADSTRING}" != "" ]] && [[ "${LEPSTRING}" != "" ]]; then
        combineCards.py had=${FINALHADCARD} lep=${FINALLEPCARD} >| ${FINALCARD}
    fi
fi
