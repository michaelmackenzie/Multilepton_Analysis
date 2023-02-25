#! /bin/bash

SELECTION=$1
YEAR=$2
HISTPATH=$3
DOTOTAL=$4
SKIPRETRIEVAL=$5


HISTSET="{25,26,27}"
YEARSTRING=`echo ${YEAR} | sed 's/{//' | sed 's/}//' | sed 's/,/_/g'`
YEARLIST=`echo ${YEAR} | sed 's/{//' | sed 's/}//' | sed 's/,/ /g'`
if [[ "${SELECTION}" == *"_"* ]]
then
    DOLEP=1
else
    DOLEP=0
fi
SIGNAL=`echo ${SELECTION} | sed 's/_e//' | sed 's/_mu//'`

echo "Running with selection = ${SELECTION} (signal = ${SIGNAL}) sets = ${SET}, years = ${YEAR}, doLep = ${DOLEP}"

if [[ "${SKIPRETRIEVAL}" == "" ]]
then
    #get the BDT distributions for each hadronic tau decay mode
    echo "Retrieving histograms"
    if [[ $DOLEP -eq 0 ]]; then
        root.exe -q -b "get_MVA_histogram.C(${HISTSET}, \"${SIGNAL}\", ${YEAR}, \"${HISTPATH}\", 1)"
    else
        root.exe -q -b "get_MVA_histogram.C(${HISTSET}, \"${SIGNAL}\", ${YEAR}, \"${HISTPATH}\", -1)"
    fi
fi

echo "Creating data cards"
#make the data cards
if [[ $DOLEP -eq 0 ]]; then
    root.exe -q -b "create_combine_cards.C(${HISTSET}, \"${SIGNAL}\", ${YEAR}, 1)"
else
    root.exe -q -b "create_combine_cards.C(${HISTSET}, \"${SIGNAL}\", ${YEAR}, -1)"
fi

#Iterate through list of years, combining cards
for YEAR_I in $YEARLIST
do
    #make the combined category cards
    cd datacards/${YEAR_I}/
    combineCards.py nomM_mva=combine_mva_${SELECTION}_25_${YEAR_I}.txt \
                    highM_mva=combine_mva_${SELECTION}_26_${YEAR_I}.txt \
                    lowM_mva=combine_mva_${SELECTION}_27_${YEAR_I}.txt >| combine_mva_${SELECTION}_25_26_27_${YEAR_I}.txt

    if [[ $DOLEP -eq 1 ]]; then
        combineCards.py lep=combine_mva_${SELECTION}_25_26_27_${YEAR_I}.txt \
                        had=combine_mva_${SIGNAL}_25_26_27_${YEAR_I}.txt \
                        >| combine_mva_total_${SIGNAL}_lep_had_mass_${YEAR_I}.txt
    else
        if [[ "${SELECTION}" == *"etau"* ]]; then
            combineCards.py lep_mva=combine_mva_${SELECTION}_mu_8_${YEAR_I}.txt \
                            had=combine_mva_${SELECTION}_25_26_27_${YEAR_I}.txt \
                            >| combine_mva_total_${SELECTION}_lep_had_mass_${YEAR_I}.txt
        else
            combineCards.py lep_mva=combine_mva_${SELECTION}_e_8_${YEAR_I}.txt \
                            had=combine_mva_${SELECTION}_25_26_27_${YEAR_I}.txt \
                            >| combine_mva_total_${SELECTION}_lep_had_mass_${YEAR_I}.txt
        fi
    fi
    cd ../..
done

if [[ "${DOTOTAL}" != "" ]]
then
    echo "Merging all years into Run-II datacards"
    cd datacards/2016_2017_2018/
    #Copy needed files over
    cp ../201?/combine_mva_${SIGNAL}_*_8_201?.root ./
    cp ../201?/combine_mva_${SIGNAL}*_2?_201?.root ./
    cp ../201?/combine_mva_${SIGNAL}*_25_201?.txt ./
    cp ../2016/combine_mva_${SIGNAL}*_25_26_27_2016.txt ./
    cp ../2017/combine_mva_${SIGNAL}*_25_26_27_2017.txt ./
    cp ../2018/combine_mva_${SIGNAL}*_25_26_27_2018.txt ./
    cp ../2016/combine_mva_total_${SIGNAL}_lep_had_mass_2016.txt ./
    cp ../2017/combine_mva_total_${SIGNAL}_lep_had_mass_2017.txt ./
    cp ../2018/combine_mva_total_${SIGNAL}_lep_had_mass_2018.txt ./
    #Merge the datacards
    combineCards.py y2016=combine_mva_${SELECTION}_25_2016.txt  \
                    y2017=combine_mva_${SELECTION}_25_2017.txt  \
                    y2018=combine_mva_${SELECTION}_25_2018.txt  \
                    >| combine_mva_${SELECTION}_25_2016_2017_2018.txt
    combineCards.py y2016=combine_mva_${SELECTION}_25_26_27_2016.txt  \
                    y2017=combine_mva_${SELECTION}_25_26_27_2017.txt  \
                    y2018=combine_mva_${SELECTION}_25_26_27_2018.txt  \
                    >| combine_mva_${SELECTION}_25_26_27_2016_2017_2018.txt
    combineCards.py y2016=combine_mva_total_${SIGNAL}_lep_had_mass_2016.txt  \
                    y2017=combine_mva_total_${SIGNAL}_lep_had_mass_2017.txt  \
                    y2018=combine_mva_total_${SIGNAL}_lep_had_mass_2018.txt  \
                    >| combine_mva_total_${SIGNAL}_lep_had_mass_2016_2017_2018.txt
fi
