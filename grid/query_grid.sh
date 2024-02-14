#! /bin/bash

Help() {
    echo "Query condor to check for running jobs"
    echo "Options:"
    echo "--help    (-h): print this message"
    echo "--summary (-s): print only the job totals in a summary line"
    echo "--verbose (-v): print detailed job information"
    echo "--running (-r): print job info for running jobs"
    echo "--tag         : print job info for jobs with the given tag"
}

SUMMARY=""
VERBOSE=""
RUNNING=""
TAG=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--summary" ]] || [[ "${var}" == "-s" ]]
    then
        SUMMARY="s"
    elif [[ "${var}" == "--verbose" ]] || [[ "${var}" == "-v" ]]
    then
        VERBOSE="v"
    elif [[ "${var}" == "--running" ]] || [[ "${var}" == "-r" ]]
    then
        RUNNING="r"
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
	eval "var=\${${iarg}}"
        TAG="${var}"
    else
        echo "Unknown argument ${var}"
        exit
    fi
    iarg=$((iarg + 1))
done

date

condor_q -nobatch | awk -v user=${USER} -v summary=${SUMMARY} -v verbose=${VERBOSE} -v running=${RUNNING} -v tag=${TAG} '
{
    if($2 == "Schedd:" && verbose != "") {
	print $0
    }
    if($2 == "Schedd:") {
	schedd=$3
    }
    if($2 == user) {
	if(verbose != "" && (tag == "" || ($0 ~ tag))) print $0
        ++total
        split($1,arr,".")
        name=arr[1]
        users[name] = $2
	schedds[name] = schedd
        if(name in names) {
            ++names[name]
        } else {
            names[name] = 1;
            status[name]["I"] = 0;
            status[name]["R"] = 0;
            status[name]["H"] = 0;
            status[name]["X"] = 0;
	    scripts[name] = $9;
	    for(i = 10; i < NF; ++i) {
		if($i ~ /_/) {
		    datasets[name] = $i;
		    i = NF + 1;
		}
	    }
        }
        ++statuses[name][$6]
	++counts[$9];
	++script_statuses[$9][$6];
    }
} END {
    print "Jobsub summary: Total of",total,"jobs found for user",user
    if(summary != "s") {
	print "User       Job-ID              Total      Idle   Running      Held         X               dataset                                   script                 Schedd"
	for(name in names) {
	    if(tag == "" || (datasets[name] ~ tag)) {
		if(running == "" || statuses[name]["R"] > 0) {
		    printf "%-10s %-10s %14i %9i %9i %9i %9i   %-50s %-20s  %s\n", users[name], name, names[name],
			statuses[name]["I"], statuses[name]["R"], statuses[name]["H"], statuses[name]["X"], datasets[name], scripts[name], schedds[name];
		}
	    }
	}
    }
    print "Job script               :     Total      Idle   Running      Held         X"
    for(script in counts) {
	printf "%-25s: %9i %9i %9i %9i %9i\n", script, counts[script], script_statuses[script]["I"], script_statuses[script]["R"],
	    script_statuses[script]["H"], script_statuses[script]["X"];
    }
 }'

# This tells emacs to view this file in awk mode.
# Local Variables:
# mode: awk
# End:
