#Full scale factor measurement loop

YEARS="2016 2017 2018"
SELECTION=$1

if [[ "${SELECTION}" == "" ]]; then
    SELECTION="electron"
fi

[ ! -d log ] && mkdir log

for YEAR in ${YEARS}
do
    echo ${YEAR}
    time ./process_all.sh ${YEAR} "${SELECTION}" "Embed MC" "0" "0" >| log/${SELECTION}_${YEAR}_0_0.log 2>&1 &
done
wait

#Only process electrons with corrections, muons only use uncertainties, no corrections
if [[ "${SELECTION}" == "electron" ]]; then
    echo "Processing Embedding with corrections applied"
    for YEAR in ${YEARS}
    do
        echo ${YEAR}
        time ./process_all.sh ${YEAR} "${SELECTION}" "Embed" "1" "2"  >| log/${SELECTION}_${YEAR}_1_2.log 2>&1 &
    done
    wait
fi
echo "Finished processing the full loop with selection ${SELECTION}"
