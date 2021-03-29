#! /bin/bash

HISTSET=$1
SELECTION=$2
YEAR=$3
NFITS=$4
MAXSYS=$5
SEED=$6

if [[ "$HISTSET" == "" ]]
then
    HISTSET=8
fi
if [[ "$SELECTION" == "" ]]
then
    SELECTION="zemu"
fi
if [[ "$YEAR" == "" ]]
then
    YEAR="{2016, 2017, 2018}"
fi
if [[ "$NFITS" == "" ]]
then
    NFITS=1000
fi
if [[ "$MAXSYS" == "" ]]
then
    MAXSYS=30
fi
if [[ "$SEED" == "" ]]
then
    SEED=90
fi


echo "Performing ${NFITS} fits on systematics for ${SELECTION} up to ${MAXSYS} using selection set ${HISTSET} from year ${YEAR} with base seed ${SEED}"
echo "- Performing self test..."
root.exe -q -b "toyMC_bemu_systematics.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, 0, ${NFITS}, true, true, ${SEED})"

for (( isys=0; isys<=$MAXSYS; isys++ ))
do
    echo "- Systematic number ${isys}..."
    root.exe -q -b "toyMC_bemu_systematics.C(${HISTSET}, \"${SELECTION}\", ${YEAR}, ${isys}, ${NFITS}, true, false, ${SEED})"
done

echo "Finished!"
