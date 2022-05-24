#! /bin/bash

YEARS=$1
WPS=$2

if [[ "${YEARS}" == "" ]]
then
    YEARS="2016 2017 2018"
fi

if [[ "${WPS}" == "" ]]
then
    #standard WPs = WPL (1), WP80 (3), and WPL + !WP80 (5)
    WPS="3 5"
fi

for YEAR in ${YEARS}
do
    for WP in ${WPS}
    do
        echo "Performing TnP measurement for year ${YEAR} and WP ${WP}"
        root.exe -q -b "scale_factors.C(${YEAR}, 0, ${WP})";
        root.exe -q -b "scale_factors.C(${YEAR}, 1, ${WP})";
        root.exe -q -b "combine_efficiencies.C(${YEAR}, ${WP})";
    done
done
