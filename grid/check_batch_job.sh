Help() {
    echo "Check jobs in a single submission for failues"
    echo "Options:"
    echo "--help       (-h)      : print this message"
    echo "--resubmit   (-r)      : resubmit failed jobs"
    echo "--verbose (-v) [arg=1] : add additional printout information"
    echo "--dryrun               : prepare but don't submit resubmission job, with --resubmit also used"
    echo "--eosdir               : EOS directory to search for the job output in"
    echo "--tag                  : Dataset tag to use"
    echo "--vet                  : Dataset tag to ignore"
    echo "--checkfiles           : Do basic checks on the output files"
    echo "--ignorerunning        : Ignore still running jobs"
    echo "--override-outdir      : Override expected output job directory"
    echo "--memory               : Override default job memory in resubmission"
    echo "--runningtime          : Override default job running time in resubmission"
    echo "--split [arg=2]        : Split resubmitted jobs into more jobs (NOT IMPLEMENTED)"
    echo "--queue                : Override default job queue (flavour) in resubmission"
    echo "--force                : Force resubmit successful and failed jobs"
}

if [ $# -eq 0 ]
then
    echo "No parameters passed! Printing help (--help) information:"
    Help
    exit
fi

JOBNAME=""
CHECKFILES=""
RESUBMIT="" #"" for nothing, "dryrun" to prepare resubmission, anything else (e.g. "d") to perform resubmission
TAG="" #dataset tag to consider
VETO="" #dataset tag to ignore
if [[ "${HOSTNAME}" == *"lxplus"* ]]
then
    HOST="lxplus"
    LOC="cms"
    EOSDIR="histograms"
else
    HOST="lpc"
    LOC="uscms"
    EOSDIR="histograms"
fi
VERBOSE=0
DRYRUN=""
IGNORERUNNING=""
OUTDIR=""
MEMORY=""
RUNNINGTIME=""
SPLIT=0
QUEUE=""
FORCE=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--resubmit" ]] || [[ "${var}" == "-r" ]]
    then
        RESUBMIT="d"
    elif [[ "${var}" == "--verbose" ]] || [[ "${var}" == "-v" ]]
    then
        #Test if a verbosity level is given
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        if [[ $var =~ ^-?[0-9]+$ ]]
        then
            VERBOSE=${var}
        else
            iarg=$((iarg - 1))
            eval "var=\${${iarg}}"
            VERBOSE=1
        fi
    elif [[ "${var}" == "--dryrun" ]]
    then
        DRYRUN="d"
    elif [[ "${var}" == "--checkfiles" ]]
    then
        CHECKFILES="d"
    elif [[ "${var}" == "--force" ]]
    then
        FORCE="d"
    elif [[ "${var}" == "--ignorerunning" ]]
    then
        IGNORERUNNING="d"
    elif [[ "${var}" == "--eosdir" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        EOSDIR=${var}
    elif [[ "${var}" == "--override-outdir" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        OUTDIR=${var}
    elif [[ "${var}" == "--memory" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        MEMORY=${var}
    elif [[ "${var}" == "--runningtime" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        RUNNINGTIME=${var}
    elif [[ "${var}" == "--split" ]]
    then
        #Test if a splitting level is given
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        if [[ $var =~ ^-?[0-9]+$ ]]
        then
            SPLIT=${var}
        else
            iarg=$((iarg - 1))
            eval "var=\${${iarg}}"
            SPLIT=2
        fi
    elif [[ "${var}" == "--queue" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        QUEUE=${var}
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--veto" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        VETO=${var}
    elif [[ "${JOBNAME}" != "" ]]
    then
        echo "Arguments aren't configured correctly!"
        Help
        exit
    else
        JOBNAME=${var}
    fi
    iarg=$((iarg + 1))
done

if [[ "${JOBNAME}" == "" ]]
then
    echo "No jobname given, should be a job listed in batch directory"
    exit
fi

if [[ "${DRYRUN}" != "" ]] && [[ "${RESUBMIT}" != "" ]]
then
    RESUBMIT="dryrun"
fi

if [[ "${HOST}" == "lpc" ]]
then
    EOSPATH="/eos/${LOC}/store/user/${USER}/${EOSDIR}"
else
    EOSPATH="/eos/${LOC}/store/group/phys_smp/ZLFV/${EOSDIR}"
fi

if [ ${VERBOSE} -gt -2 ]
then
    echo "Using EOS path ${EOSPATH}"
    echo "Using jobname ${JOBNAME}"
fi


NFAILED=0
NPASSED=0
NRUNNING=0
FAILEDJOBS=""

FILEPATH="${EOSPATH}"
if [[ "${OUTDIR}" != "" ]]; then
    if [ ${VERBOSE} -gt -2 ]; then
        echo "Overriding output directory to use ${OUTDIR}"
    fi
    FILEPATH="${EOSPATH}/${OUTDIR}"
fi


for f in `ls -d ${JOBNAME}reports/*.log`
do
    FILE=`basename ${f} | awk -F "_" '{name=""; for( i = 1; i < NF - 1; i++){ name=name $i; if(i < NF - 2) {name=name "_";}}}END{print name}'`
    JOB=`echo ${f} | awk -F "/" '{print $2}'`
    if  grep -q "${TAG}" <<< "${FILE}"
    then
        if [ ${VERBOSE} -gt 0 ]
        then
            echo "Checking file ${FILE} in job ${JOB}"
        fi
    else
        if [ ${VERBOSE} -gt 1 ]
        then
            echo "Skipping file ${FILE} as tag ${TAG} not found"
        fi
        continue
    fi
    if  [[ "${VETO}" != "" ]] && grep -q "${VETO}" <<< "${FILE}"
    then
        if [ ${VERBOSE} -gt 1 ]
        then
            echo "Skipping file ${FILE} in job ${JOB} due to veto tag"
        fi
        continue
    fi
    STDLOG="${f/log/stdout}"
    if [ ! -f ${STDLOG} ]
    then
        NRUNNING=$((1 + $NRUNNING))
        if [[ "${IGNORERUNNING}" != "" ]]
        then
            continue
        fi
        if [ ${VERBOSE} -gt -1 ]
        then
            echo "Stdout file ${STDLOG} does not exist"
        fi
    else
        XRDEXIT=`grep -i "failure" ${STDLOG}`
        if [[ "${XRDEXIT}" != "" ]]
        then
            if [ ${VERBOSE} -gt -1 ]
            then
                echo "Failure in file ${STDLOG}, error message: ${XRDEXIT}"
            fi
            FAILEDJOBS="${FAILEDJOBS} ${FILE}"
            NFAILED=$((1 + $NFAILED))
        fi
    fi
    YEAR=${FILE: -4}
    NAME=${FILE: 0 : -5}
    #adjust names where needed
    if [[ "${NAME}" == *"-v*" ]]; then #remove wildcards/versions from signal names
        NAME=${NAME: 0 : -3}
    fi
    if [[ "${NAME}" == *"-v3*" ]]; then #remove wildcards/versions from signal names
        NAME=${NAME: 0 : -4}
    fi
    if [[ "${NAME}" == *"Run"* ]]; then #shorten data names
        NAME=`echo ${NAME} | sed 's/Run2016/-/'`
        NAME=`echo ${NAME} | sed 's/Run2017/-/'`
        NAME=`echo ${NAME} | sed 's/Run2018/-/'`
    fi
    if [[ "${NAME}" == "DY"* ]]; then #assume Drell-Yan is being split
        NAME=${NAME}-2
    fi
    ROOTFILE="${FILEPATH}/${JOB}/output_${YEAR}_${NAME}.hist"
    if compgen -G "${FILEPATH}/${JOB}/*${YEAR}_${NAME}.hist" > /dev/null; then
        ROOTFILE=`ls -d ${FILEPATH}/${JOB}/*${YEAR}_${NAME}.hist | head -n 1`
    fi
    if [ ! -f ${ROOTFILE} ]
    then
        if [ ${VERBOSE} -gt -1 ]
        then
            echo "File ${FILE} in job ${JOB} does not exist (${ROOTFILE})"
        fi
        if [[ "${XRDEXIT}" == "" ]]
        then
            FAILEDJOBS="${FAILEDJOBS} ${FILE}"
            NFAILED=$((1 + $NFAILED))
        fi
    elif [[ "${CHECKFILES}" != "" ]]
    then
        root.exe -q -l -b "check_batch_file.C(\"${ROOTFILE}\")"
        ROOTSTATUS=$?
        if [[ $ROOTSTATUS -ne 0 ]]
        then
            if [ ${VERBOSE} -gt -1 ]
            then
                echo "File ${FILE} failed ROOT file checking"
            fi
            #only add it if it hasn't already failed a check
            if [[ "${XRDEXIT}" == "" ]]
            then
                FAILEDJOBS="${FAILEDJOBS} ${FILE}"
                NFAILED=$((1 + $NFAILED))
            else
                NPASSED=$((1 + $NPASSED))
                if [[ "${FORCE}" != "" ]]
                then
                    FAILEDJOBS="${FAILEDJOBS} ${FILE}"
                fi
            fi
        fi
    else
        NPASSED=$((1 + $NPASSED))
        if [[ "${FORCE}" != "" ]]
        then
            FAILEDJOBS="${FAILEDJOBS} ${FILE}"
        fi

    fi
done
if [ ${VERBOSE} -gt -3 ]
then
    echo "${NFAILED} jobs failed checks, ${NPASSED} passed, and ${NRUNNING} are (possibly) running"
fi

if [[ "${RESUBMIT}" != "" ]]
then
    if [[ "${FOCRE}" != "" ]]
    then
        echo "Force re-submitting all jobs checked!"
    fi

    if [[ "${FAILEDJOBS}" != "" ]]
    then
        rm ${JOBNAME}recover_*.jdl
    fi
    for FILE in ${FAILEDJOBS}
    do
        DATASET=`echo ${FILE} | awk -F "_" '{name=""; for( i = 1; i < NF; i++){ name=name $i; if(i < NF - 1) {name=name "_";}}}END{print name}'`
        RECOVERY=${JOBNAME}recover_${DATASET}.jdl
        JOBINFO=${JOBNAME}batchJob_${DATASET}.jdl
        COUNT=`echo ${FILE} | awk -F "_" '{print $NF}'`
        if [ ! -f "${RECOVERY}" ]
        then
            if [ ${VERBOSE} -gt -2 ]
            then
                echo "Creating recovery jdl for dataset ${DATASET}"
            fi
            cat ${JOBINFO} | awk -v d=0 -v m=${MEMORY} -v r=${RUNNINGTIME} -v q=${QUEUE} '{
            if($1 == "Arguments") d=1;
            if(d == 0) {
              if(!(m != "" && ($0 ~ /request_memory/)) && !(r != "" && ($0 ~ /MaxRuntime/)) && !(q != "" && ($0 ~ /JobFlavour/))) {
                print $0
              }
            }
          } END {
            if(m != "") print "request_memory        =", m
            if(r != "") print "+MaxRuntime           =", r
            if(q != "") print "+JobFlavour           = \"" q "\""
          }' > ${RECOVERY}
        fi
        if [ ${VERBOSE} -gt -2 ]
        then
            echo "Adding job ${FILE} to recovery file for dataset ${DATASET}"
        fi
        #print the job submission info for the given job
        if [ ${SPLIT} -eq 0 ]
        then
            cat ${JOBINFO} | awk -v d=0 -v count=${COUNT} '{
              if($1 == "Arguments") {
                if($3 == count) {d=1;} else {d=0;}
              }
              if(d==1) {print $0;}
            }' >> ${RECOVERY}
        else
            INPUTFILE=${JOBNAME}/input_${DATASET}_${COUNT}.txt
            cat ${JOBINFO} | awk -v d=0 -v count=${COUNT} -v loop=${LOOP} '{
              if($1 == "Arguments") {
                if($3 == count) {d=1;} else {d=0;}
              }
              if(d==1) {print $0;}
            }' >> ${RECOVERY}
        fi
        if [[ "${RESUBMIT}" != "" ]]
        then
            if [ ${VERBOSE} -gt -1 ]
            then
                echo "Storing previous records for this failed job in recovery directory"
            fi
            mkdir -p batch/recovery/${JOBNAME}
            mv ${JOBNAME}reports/${FILE}* batch/recovery/${JOBNAME}/
        fi
    done

    if [[ "${RESUBMIT}" != "dryrun" ]]
    then
        cd ${JOBNAME}
        for FILE in `ls recover_*.jdl`
        do
            if [ ${VERBOSE} -gt -3 ]
            then
                echo "Submitting recovery job for dataset ${FILE}"
            fi
            condor_submit ${FILE}
        done
    else
        if [ ${VERBOSE} -gt -1 ]
        then
            echo "Finished resubmission dry-run"
        fi
    fi
fi

if [ ${VERBOSE} -gt -2 ]
then
    echo "Finished processing all jobs"
fi
