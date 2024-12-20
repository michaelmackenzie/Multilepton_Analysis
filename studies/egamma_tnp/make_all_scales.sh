#! /bin/bash

YEARS=$1
WPS=$2


if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2016_BF 2016_GH 2017 2018"
elif [[ "${YEARS}" == "" ]]
then
    echo "1: Years (default = \"2016 2017 2018\")"
    echo "2: Working points (default = \"0\")"
    exit
fi

if [[ "${WPS}" == "" ]]
then
    #standard WPs = WPL (1), WP80 (3), and WPL + !WP80 (5)
    WPS="0"
fi

for YEAR in ${YEARS}
do
    PERIOD=-1
    if [[ "${YEAR}" == "2016_BF" ]]
    then
        YEAR=2016
        PERIOD=0
    elif [[ "${YEAR}" == "2016_BF" ]]
    then
        YEAR=2016
        PERIOD=1
    fi
    for WP in ${WPS}
    do
        echo "Performing TnP measurement for year ${YEAR} and WP ${WP}"
        root.exe -q -b "scale_factors.C(${YEAR}, 0, ${WP}, ${PERIOD})";
        root.exe -q -b "scale_factors.C(${YEAR}, 1, ${WP})"; #Always use default period for MC
        root.exe -q -b "combine_efficiencies.C(${YEAR}, ${WP}, ${PERIOD})";
    done
done
