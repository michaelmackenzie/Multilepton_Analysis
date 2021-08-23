#! /bin/bash

root.exe -q -b "scale_factors.C(2016, 0)";
root.exe -q -b "scale_factors.C(2016, 1)";
root.exe -q -b "scale_factors.C(2017, 0)";
root.exe -q -b "scale_factors.C(2017, 1)";
root.exe -q -b "scale_factors.C(2018, 0)";
root.exe -q -b "scale_factors.C(2018, 1)";

root.exe -q -b "combine_efficiencies.C(2016)";
root.exe -q -b "combine_efficiencies.C(2017)";
root.exe -q -b "combine_efficiencies.C(2018)";
