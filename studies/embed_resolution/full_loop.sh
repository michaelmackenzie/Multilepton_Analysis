#Full scale factor measurement loop

YEARS="2016 2017 2018"

for YEAR in ${YEARS}
do
    echo ${YEAR}
    time ./process_all.sh ${YEAR} "electron" "Embed MC" "0" "0" >| electron_${YEAR}_0_0.log 2>&1 &
done
wait

echo "Processing Embedding with corrections applied"
for YEAR in ${YEARS}
do
    echo ${YEAR}
    # time process_all.sh ${YEAR} "electron" "Embed" "0" "2"
    time ./process_all.sh ${YEAR} "electron" "Embed" "1" "2"  >| electron_${YEAR}_1_2.log 2>&1 &
done
