#! /bin/bash


NAME=$1
PATHIN=$2
PATHOUT=$3

if [[ "${PATHIN}" == "" ]]
then
    PATHIN="figures"
fi
if [[ "${PATHOUT}" == "" ]]
then
    PATHOUT="figures"
fi

echo "Combining image files ${PATHIN}/${NAME}/${NAME}*.png --> ${PATHOUT}/${NAME}.gif"

if [ ! -d ${PATHIN} ]
then
    echo "Input path ${PATHIN} doesn't exist!"
    exit
fi
[ ! -d ${PATHOUT} ] && mkdir -p ${PATHOUT}

if [ `ls ${PATHIN}/${NAME}/${NAME}*.png | wc | awk '{print $NF}'` -gt 0 ]
then
    convert -delay 50 ${PATHIN}/${NAME}/${NAME}*.png -loop 0 ${PATHOUT}/${NAME}.gif
    echo "Finished!"
else
    echo "No image files found!"
fi

