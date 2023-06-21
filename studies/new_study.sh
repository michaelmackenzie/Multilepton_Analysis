#! /bin/bash

NAME=$1
if [[ "${NAME}" == "" ]]; then
    echo "No study name given!"
    exit
fi

mkdir ${NAME}
if [ ! -d ${NAME} ]; then
    echo "Failed to make directory ${NAME}"
    exit
fi

cd ${NAME}
mkdir figures
mkdir rootfiles
ln -s ../../rootScripts/rootlogon.C rootlogon.C

echo "Made study directory ${NAME}"
