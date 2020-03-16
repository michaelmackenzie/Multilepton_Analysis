#! /bin/bash

l=("mutau" "etau" "emu" "mumu")

for selection in ${l[@]}; do
    rm ztautau/ztautau_${selection}_bltTree_wjets_total.hist
    hadd ztautau/ztautau_${selection}_bltTree_wjets_total.hist ztautau/ztautau_${selection}_*_wjets*.hist
done
