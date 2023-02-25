#! /bin/bash

SELECTION=$1
YEAR=$2
HISTPATH=$3
DOTOTAL=$4
SKIPRETRIEVAL=$5

HISTSET="{21,22}"
YEARSTRING=`echo ${YEAR} | sed 's/{//' | sed 's/}//' | sed 's/,/_/'`

if [[ "${SKIPRETRIEVAL}" == "" ]]
then
    #get the BDT distributions for each hadronic tau decay mode
    echo "Retrieving histograms"
    root.exe -q -b "get_MVA_histogram.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, \"${HISTPATH}\", 1)"
fi
#make the data cards
root.exe -q -b "create_combine_cards.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, 1)"
#make the combined category cards
cd datacards/${YEARSTRING}/
combineCards.py p1_mva=combine_mva_${SELECTION}_21_${YEARSTRING}.txt p3_mva=combine_mva_${SELECTION}_22_${YEARSTRING}.txt >| combine_mva_${SELECTION}_21_22_${YEARSTRING}.txt
if [[ "${SELECTION}" == *"etau"* ]]
then
    combineCards.py lep_mva=combine_mva_${SELECTION}_mu_8_${YEARSTRING}.txt had=combine_mva_${SELECTION}_21_22_${YEARSTRING}.txt >| combine_mva_total_${SELECTION}_lep_had_prong_${YEARSTRING}.txt
else
    combineCards.py lep_mva=combine_mva_${SELECTION}_e_8_${YEARSTRING}.txt had=combine_mva_${SELECTION}_21_22_${YEARSTRING}.txt >| combine_mva_total_${SELECTION}_lep_had_prong_${YEARSTRING}.txt
fi

if [[ "${DOTOTAL}" != "" ]]
then
    echo "Merging all years into Run-II datacards"
    cd ../2016_2017_2018/
    #Copy needed files over
    cp ../201?/combine_mva_${SELECTION}_*_8_201?.root ./
    cp ../201?/combine_mva_${SELECTION}_2?_201?.root ./
    cp ../201?/combine_mva_${SELECTION}_21_22_201?.txt ./
    cp ../201?/combine_mva_total_${SELECTION}_lep_had_prong_201?.txt ./
    #Merge the datacards
    combineCards.py y2016=combine_mva_${SELECTION}_21_22_2016.txt  \
                    y2017=combine_mva_${SELECTION}_21_22_2017.txt  \
                    y2018=combine_mva_${SELECTION}_21_22_2018.txt  \
                    >| combine_mva_${SELECTION}_21_22_2016_2017_2018.txt
    combineCards.py y2016=combine_mva_total_${SELECTION}_lep_had_prong_2016.txt  \
                    y2017=combine_mva_total_${SELECTION}_lep_had_prong_2017.txt  \
                    y2018=combine_mva_total_${SELECTION}_lep_had_prong_2018.txt  \
                    >| combine_mva_total_${SELECTION}_lep_had_prong_2016_2017_2018.txt
fi
