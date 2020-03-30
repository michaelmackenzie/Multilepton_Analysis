#! /bin/bash

l=("mutau" "etau" "emu" "mumu")
dir=$1
if [ "$dir" == "" ]
then
    echo "No directory given, defaulting to ztautau!"
    dir="ztautau"
fi
for selection in ${l[@]}; do
    rm ${dir}/ztautau_${selection}_bltTree_wjets_total.hist
    hadd ${dir}/ztautau_${selection}_bltTree_wjets_total.hist ztautau/ztautau_${selection}_*_wjets*.hist
done
