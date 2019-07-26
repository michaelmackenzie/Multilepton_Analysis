#! /bin/bash

root process_multilepton.C
#rename .hist .ptstudy_hist *
mv *hist ../histograms/
cd ../histograms/
root.exe find_cuts.C
