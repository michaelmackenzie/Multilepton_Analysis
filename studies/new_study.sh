#! /bin/bash

Help() {
    echo "Create a new study directory with the standard layout"
    echo "1: study directory name"
}

NAME=$1

if [[ "${NAME}" == "-h" ]] || [[ "${NAME}" == "--help" ]]; then
    Help
    exit
fi

if [[ "${NAME}" == "" ]]; then
    echo "No study name given!"
    Help
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
