#! /bin/bash

for f in `ls -d ../../tmva_training/training_*/*/*Weights/*.xml`;
do
    echo $f;

    # Find the selection
    a="";
    if [[ $f == *"mutau_e"* ]]; then
        a="mutau_e_";
    elif [[ $f == *"mutau"* ]]; then
        a="mutau_";
    elif [[ $f == *"etau_mu"* ]]; then
        a="etau_mu_";
    elif [[ $f == *"etau"* ]]; then
        a="etau_";
    elif [[ $f == *"emu"* ]]; then
        a="emu_";
    else
        echo "Unidentified selection!";
    fi

    # Find the set
    set=-1;
    if [[ $f == *"_7_"* ]]; then
        set=7;
    elif [[ $f == *"_8_"* ]]; then
        set=8;
    elif [[ $f == *"_18_"* ]]; then
        set=18;
    elif [[ $f == *"_19_"* ]]; then
        set=19;
    elif [[ $f == *"_20_"* ]]; then
        set=20;
    elif [[ $f == *"_28_"* ]]; then
        set=28;
    elif [[ $f == *"_37_"* ]]; then
        set=37;
    elif [[ $f == *"_38_"* ]]; then
        set=38;
    elif [[ $f == *"_39_"* ]]; then
        set=39;
    elif [[ $f == *"_40_"* ]]; then
        set=40;
    elif [[ $f == *"_48_"* ]]; then
        set=48;
    elif [[ $f == *"_58_"* ]]; then
        set=58;
    elif [[ $f == *"_59_"* ]]; then
        set=59;
    elif [[ $f == *"_60_"* ]]; then
        set=60;
    elif [[ $f == *"_67_"* ]]; then
        set=67;
    elif [[ $f == *"_68_"* ]]; then
        set=68;
    fi

    # Find the MVA name
    if [[ $f == *"BDTRT"* ]]; then
        a=$a"BDTRT_";
    elif [[ $f == *"BDT_MM"* ]]; then
        a=$a"BDT_MM_";
    elif [[ $f == *"BDT"* ]]; then
        a=$a"BDT_";
    elif [[ $f == *"MLP_MM"* ]]; then
        a=$a"MLP_MM_";
    elif [[ $f == *"MLP"* ]]; then
        a=$a"MLP_";
    elif [[ $f == *"TMlpANN"* ]]; then
        a=$a"TMlpANN_";
    else
        echo "Unidentified MVA!";
        a=$a"UNKNOWN_";
    fi

    # Find the decaying boson
    a=$a$set;
    if [[ $f == *"higgs"* ]]; then
        a=$a".higgs"
    elif [[ $f == *"Z0"* ]]; then
        a=$a".Z0";
    fi

    # Find the year it's trained for
    if [[ $f == *"_2016_2017_2018_"* ]]; then
        a=$a".2016_2017_2018"
    elif [[ $f == *"_2016_"* ]]; then
        a=$a".2016"
    elif [[ $f == *"_2017_"* ]]; then
        a=$a".2017"
    elif [[ $f == *"_2018_"* ]]; then
        a=$a".2018"
    fi

    # Copy the file
    a=$a".weights.xml";
    echo "cp "$f" "$a;
    cp $f $a;
done
