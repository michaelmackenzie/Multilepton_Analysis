#!/bin/sh

echo "Job submitted on host `hostname` on `date`"
echo ">>> arguments: $@"

### Required parameters #####
COMMAND=$1
OUTDIR=$2

### Transfer files, prepare directory ###
TOPDIR=$PWD

# lxplus/lpc
if [[ "${HOSTNAME}" == *"cern.ch"* ]]
then
    echo "Exporting the x509 token location (${USER})"
    export X509_USER_PROXY=/afs/cern.ch/user/m/mimacken/voms_proxy/x509up_u117705
fi

export SCRAM_ARCH=slc7_amd64_gcc820
export CMSSW_VERSION=CMSSW_10_6_29
source /cvmfs/cms.cern.ch/cmsset_default.sh

# temporary fix
tar -xzf source.tar.gz
cd $CMSSW_VERSION/src
cmsenv
cd ..
export CMSSW_BASE=$PWD
cd src/CLFVAnalysis
make clean; make -j8;

echo "Inputs: "
echo $DATASET $OUTDIR
echo "PATH: "
echo $PATH
echo "Starting working dir: "
pwd
ls
ls lib
cd Roostats/
echo "Working dir: "
pwd
echo ""

ls ${CMSSW_BASE}/src/CLFVAnalysis/lib/libCLFVAnalysis.so

### Run combine
echo "Running command ${COMMAND}"
${COMMAND}
COMBINEEXIT=$?
if [[ ${COMBINEEXIT} -ne 0 ]]; then
    echo "Combine processing code ${ROOTEXIT}, exit code 1, failure in processing"
    exit 1
fi

# Retrieve the files
FILES=`ls *.root`
if [[ "${FILES}" == "" ]]; then
    echo "No root files found, exit code 1, failure in processing"
    exit 1
fi

#copy back the files
for FILE in ${FILES}; do
    OUTNAME=`echo ${FILE} | sed 's/.root/${NAME}.root/'`
    #Give 5 attempts, to account for xrootd errors
    for ATTEMPT in {1..5}; do
        echo "Attempt ${ATTEMPT}: Copying back merged file ${FILE} to ${OUTDIR}"
        date
        xrdcp -f ${FILE} ${OUTDIR}/${OUTNAME} 2>&1
        XRDEXIT=$?
        date
        if [[ $XRDEXIT -ne 0 ]]; then
            sleep 60
            continue
        else
            break
        fi
    done
done

if [[ $XRDEXIT -ne 0 ]]; then
    rm *.root
    echo "exit code $XRDEXIT, failure in xrdcp"
    exit $XRDEXIT
fi


rm ${FILES}

echo "Finished processing"
