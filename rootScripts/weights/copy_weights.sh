#! /bin/bash

for f in `ls -d ../../tmva_training/training_*/*Weights/*.xml`;
do
    echo $f;
    a="";
    set=-1;
    if [[ $f == *"mutau_e"* ]]; then
	a="mutau_e_";
	set=48;
    elif [[ $f == *"mutau"* ]]; then
	a="mutau_";
	set=8;
    elif [[ $f == *"etau_mu"* ]]; then
	a="etau_mu_";
	set=48;
    elif [[ $f == *"etau"* ]]; then
	a="etau_";
	set=28;
    elif [[ $f == *"emu"* ]]; then
	a="emu_";
	set=48;
    else
	echo "Unidentified selection!";
    fi

    if [[ $f == *"BDTRT"* ]]; then
	a=$a"BDTRT_";
    elif [[ $f == *"BDT"* ]]; then
	a=$a"BDT_";
    elif [[ $f == *"MLP_MM"* ]]; then
	a=$a"MLP_MM_";
    elif [[ $f == *"MLP"* ]]; then
	a=$a"MLP_";
    else
	echo "Unidentified MVA!";
    fi
    a=$a$set;
    if [[ $f == *"higgs"* ]]; then
	a=$a".higgs"
    elif [[ $f == *"Z0"* ]]; then
	a=$a".Z0";
    fi
    a=$a".weights.xml";
    echo "cp "$f" "$a;
    cp $f $a;
done
