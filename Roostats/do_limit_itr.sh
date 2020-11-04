#! /bin/bash

HISTSET=$1
YEAR=$2
NFITS=$3
SEED=$4
if [[ "$SEED" == "" ]]
then
    SEED=90
fi
echo "Performing ${NFITS} fits using selection set ${HISTSET} from year {YEAR} with base seed ${SEED}"

echo "nfit/I:upperLimit/F:expectedLimit/F:plusLimit/F:minusLimit/F" >| toyMC_limits_${YEAR}_${HISTSET}.txt

for (( nfit=1; nfit<=$NFITS; nfit++ ))
do
    rndseed=$(($SEED+$nfit))
    echo "Fit number ${nfit} with seed ${rndseed}..."
    root.exe -q -b "toyMC_limits.C(${HISTSET}, ${YEAR}, ${rndseed})"

done
