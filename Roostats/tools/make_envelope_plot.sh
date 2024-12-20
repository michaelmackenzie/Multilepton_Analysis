#! /bin/bash

# Run combine scans with each individual background function, generating a plot of the envelope method

Help() {
    echo "Make an envelope plot for a single Z->emu fit category"
    echo " Usage: make_envelope_plot.sh <card> [options]"
    echo " --cat      : Category number for PDF index variable (default = 13)"
    echo " --maxpdfs  : Maximum PDF index (default = 2)"
    echo " --rrange   : r parameter range (default = 10)"
    echo " --rgen     : r for generated toy (default = 0)"
    echo " --index_gen: PDF index for generated toy"
    echo " --npoints  : N(scan points) (default = 100)"
    echo " --george   : Assume processing data cards from George"
    echo " --obs      : Use the observed dataset"
    echo " --plotonly : Only print plots"
    echo " --dryrun   : Only setup commands"
    echo " --dodebug  : Apply the debug parameter initialization and arguments"
    echo " --tag      : Tag for output plot"
    echo " --seed     : Seed for generation"
}

CARD=""
CAT=13
MAXPDFS=2
RRANGE=10
RGEN=0
TAG=""
GENINDEX=""
NPOINTS=100
SEED=90
GEORGE=""
OBS=""
PLOTONLY=""
DRYRUN=""
DODEBUG=""

iarg=1
while [ ${iarg} -le $# ]
do
    eval "var=\${${iarg}}"
    if [[ "${var}" == "--help" ]] || [[ "${var}" == "-h" ]]
    then
        Help
        exit
    elif [[ "${var}" == "--rrange" ]] || [[ "${var}" == "-r" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        RRANGE=${var}
    elif [[ "${var}" == "--rgen" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
    elif [[ "${var}" == "--npoints" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        NPOINTS=${var}
    elif [[ "${var}" == "--tag" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        TAG=${var}
    elif [[ "${var}" == "--maxpdfs" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        MAXPDFS=${var}
    elif [[ "${var}" == "--index_gen" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENINDEX=${var}
    elif [[ "${var}" == "--genarg" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        GENARG=${var}
    elif [[ "${var}" == "--cat" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        CAT=${var}
    elif [[ "${var}" == "--seed" ]] || [[ "${var}" == "-s" ]]
    then
        iarg=$((iarg + 1))
        eval "var=\${${iarg}}"
        SEED=${var}
    elif [[ "${var}" == "--george" ]]; then
        GEORGE="d"
    elif [[ "${var}" == "--obs" ]]; then
        OBS="d"
    elif [[ "${var}" == "--plotonly" ]]; then
        PLOTONLY="d"
    elif [[ "${var}" == "--dryrun" ]]; then
        DRYRUN="d"
    elif [[ "${var}" == "--dodebug" ]]; then
        DODEBUG="d"
    elif [[ "${CARD}" != "" ]]
    then
        echo "Arguments aren't configured correctly!"
        Help
        exit
    else
        CARD=${var}
    fi
    iarg=$((iarg + 1))
done

if [[ "${CARD}" == "" ]]; then
    echo "No input Combine card/workspace provided!"
    exit 1
fi

if [[ "${CAT}" == "" ]]; then
    CAT=13
fi

if [[ "${MAXPDFS}" == "" ]]; then
    MAXPDFS=10
fi

RMIN=-${RRANGE}
RMAX=${RRANGE}
ARGRANGES=" --setParameterRanges r=${RMIN},${RMAX}"
ARGUMENTS="--algo grid --cminDefaultMinimizerStrategy 0 --saveNLL -m 125 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1"
ARGUMENTS="${ARGUMENTS} --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints"
ARGUMENTS="${ARGUMENTS} --rMin ${RMIN} --rMax ${RMAX}"

# Debug: setting initial parameters:
PARAMS=""
# Bin 1
if [[ "${CAT}" == "1" ]]; then
    PARAMS="${PARAMS}bkg_gspol1_pdf_bin1_b0=-0.00639766,bkg_gspol1_pdf_bin1_mu=55.3031,bkg_gspol1_pdf_bin1_wd=12.1883,"
    PARAMS="${PARAMS}bkg_gspol2_pdf_bin1_b0=0.0696676,bkg_gspol2_pdf_bin1_b1=-0.000496288,bkg_gspol2_pdf_bin1_mu=57.9333,bkg_gspol2_pdf_bin1_wd=12.2978,"
    PARAMS="${PARAMS}bkg_gsexp1_pdf_bin1_mu=69,bkg_gsexp1_pdf_bin1_wd=150,bkg_gsexp1_pdf_bin1_x0=-0.105078,"
    PARAMS="${PARAMS}bkg_gsexp2_pdf_bin1_c0=0.77646,bkg_gsexp2_pdf_bin1_mu=69,bkg_gsexp2_pdf_bin1_wd=5.47864,bkg_gsexp2_pdf_bin1_x0=-0.047225,bkg_gsexp2_pdf_bin1_x1=0.0242386,"
    PARAMS="${PARAMS}bkg_gsplaw1_pdf_bin1_a0=-1.20994,bkg_gsplaw1_pdf_bin1_mu=60.4097,bkg_gsplaw1_pdf_bin1_wd=10.993,"
    PARAMS="${PARAMS}bkg_gsplaw2_pdf_bin1_a0=4.91931,bkg_gsplaw2_pdf_bin1_a1=-3.33746,bkg_gsplaw2_pdf_bin1_c0=0.0832076,bkg_gsplaw2_pdf_bin1_mu=69,bkg_gsplaw2_pdf_bin1_wd=5.57072,"
    PARAMS="${PARAMS}bkg_cheb3_pdf_bin1_0=-0.744645,bkg_cheb3_pdf_bin1_1=0.31931,bkg_cheb3_pdf_bin1_2=-0.0970132,"
    PARAMS="${PARAMS}bkg_cheb4_pdf_bin1_0=-0.744743,bkg_cheb4_pdf_bin1_1=0.320949,bkg_cheb4_pdf_bin1_2=-0.0979635,bkg_cheb4_pdf_bin1_3=0.00261585,"
fi

# Bin 2
if [[ "${CAT}" == "2" ]]; then
    PARAMS="${PARAMS}bkg_gspol1_pdf_bin2_b0=-0.0061644,bkg_gspol1_pdf_bin2_mu=64.5875,bkg_gspol1_pdf_bin2_wd=9.03786,"
    PARAMS="${PARAMS}bkg_gspol2_pdf_bin2_b0=0.0671617,bkg_gspol2_pdf_bin2_b1=-0.000521754,bkg_gspol2_pdf_bin2_mu=64.1955,bkg_gspol2_pdf_bin2_wd=9.26246,"
    PARAMS="${PARAMS}bkg_gsexp1_pdf_bin2_mu=64.8665,bkg_gsexp1_pdf_bin2_wd=8.8785,bkg_gsexp1_pdf_bin2_x0=-0.0168155,"
    PARAMS="${PARAMS}bkg_gsexp2_pdf_bin2_c0=0.704532,bkg_gsexp2_pdf_bin2_mu=68.9973,bkg_gsexp2_pdf_bin2_wd=7.52105,bkg_gsexp2_pdf_bin2_x0=-0.0162419,bkg_gsexp2_pdf_bin2_x1=-0.181696,"
    PARAMS="${PARAMS}bkg_gsplaw2_pdf_bin2_a0=-12.3945,bkg_gsplaw2_pdf_bin2_a1=-1.41593,bkg_gsplaw2_pdf_bin2_c0=0.329129,bkg_gsplaw2_pdf_bin2_mu=68.9977,bkg_gsplaw2_pdf_bin2_wd=7.43669,"
    PARAMS="${PARAMS}bkg_gsplaw3_pdf_bin2_a0=-14.7373,bkg_gsplaw3_pdf_bin2_a1=-6.1344,bkg_gsplaw3_pdf_bin2_a2=-1.33806,bkg_gsplaw3_pdf_bin2_c0=0.222879,bkg_gsplaw3_pdf_bin2_c1=0.16461,bkg_gsplaw3_pdf_bin2_mu=68.9999,bkg_gsplaw3_pdf_bin2_wd=7.44786,"
    PARAMS="${PARAMS}bkg_cheb5_pdf_bin2_0=-1.22725,bkg_cheb5_pdf_bin2_1=0.639567,bkg_cheb5_pdf_bin2_2=-0.195609,bkg_cheb5_pdf_bin2_3=0.00271248,bkg_cheb5_pdf_bin2_4=0.0307185,"
fi

# Bin 3
if [[ "${CAT}" == "3" ]]; then
    PARAMS="${PARAMS}bkg_gspol1_pdf_bin3_b0=-0.008133,bkg_gspol1_pdf_bin3_mu=65.5644,bkg_gspol1_pdf_bin3_wd=8.84455,"
    PARAMS="${PARAMS}bkg_gspol2_pdf_bin3_b0=0.059798,bkg_gspol2_pdf_bin3_b1=-0.000567103,bkg_gspol2_pdf_bin3_mu=65.3745,bkg_gspol2_pdf_bin3_wd=8.96678,"
    PARAMS="${PARAMS}bkg_gsexp1_pdf_bin3_mu=66.2777,bkg_gsexp1_pdf_bin3_wd=8.43507,bkg_gsexp1_pdf_bin3_x0=-0.0492584,"
    PARAMS="${PARAMS}bkg_gsexp2_pdf_bin3_c0=0.11467,bkg_gsexp2_pdf_bin3_mu=66.3163,bkg_gsexp2_pdf_bin3_wd=8.41403,bkg_gsexp2_pdf_bin3_x0=-0.0508909,bkg_gsexp2_pdf_bin3_x1=-0.0494316,"
    PARAMS="${PARAMS}bkg_gsplaw1_pdf_bin3_a0=-5.03222,bkg_gsplaw1_pdf_bin3_mu=66.7112,bkg_gsplaw1_pdf_bin3_wd=8.25766,"
    PARAMS="${PARAMS}bkg_gsplaw2_pdf_bin3_a0=-14.2918,bkg_gsplaw2_pdf_bin3_a1=-5.08336,bkg_gsplaw2_pdf_bin3_c0=0.00223443,bkg_gsplaw2_pdf_bin3_mu=66.7952,bkg_gsplaw2_pdf_bin3_wd=8.21747,"
    PARAMS="${PARAMS}bkg_cheb5_pdf_bin3_0=-1.49644,bkg_cheb5_pdf_bin3_1=0.772098,bkg_cheb5_pdf_bin3_2=-0.222063,bkg_cheb5_pdf_bin3_3=-0.0116984,bkg_cheb5_pdf_bin3_4=0.0391811,"

    ARGRANGES="${ARGRANGES},bkg_cheb5_pdf_bin3_0=-3,3,bkg_cheb5_pdf_bin3_1=-2,2,bkg_cheb5_pdf_bin3_2=-1,1,bkg_cheb5_pdf_bin3_3=-0.5,0.5,bkg_cheb5_pdf_bin3_4=-0.2,0.2"
fi

DEBUGARG="--cminRunAllDiscreteCombinations --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1 --X-rtd MINIMIZER_multiMin_maskChannels=2 "

if [[ "${DODEBUG}" == "" ]]; then
    PARAMS=""
    DEBUGARG=""
fi

if [[ "${PLOTONLY}" == "" ]]; then
    rm higgsCombine_env_${CAT}*.*.mH125.*.root

    #Generate a toy dataset
    if [[ "${OBS}" == "" ]]; then
        if [[ "${DRYRUN}" == "" ]]; then
            combine -M GenerateOnly -d ${CARD} -t 1 -m 125 -s ${SEED} -n _env_${CAT} --saveToys
        fi
        TOYDATA=higgsCombine_env_${CAT}.GenerateOnly.mH125.${SEED}.root
        ls -l ${TOYDATA}
        TOYARG="--toysFile=${TOYDATA} -t -1"
    else
        TOYARG=""
    fi

    for (( PDF=0; PDF<${MAXPDFS}; PDF++ )); do
        if [[ "${GEORGE}" != "" ]]; then
            CATVAR="pdfindex_bin${CAT}"
        else
            CATVAR="cat_${CAT}"
        fi

        echo combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} ${ARGRANGES} --points ${NPOINTS} ${TOYARG} --setParameters ${PARAMS}${CATVAR}=${PDF} --freezeParameters "${CATVAR}" -n "_env_${CAT}_cat_${PDF}" ${DEBUGARG}
        if [[ "${DRYRUN}" == "" ]]; then
            combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} ${ARGRANGES} --points ${NPOINTS} ${TOYARG} --setParameters ${PARAMS}${CATVAR}=${PDF} --freezeParameters "${CATVAR}" -n "_env_${CAT}_cat_${PDF}" ${DEBUGARG} -v 3 >| fit_cat_${PDF}.log
        fi
    done
    if [[ "${DODEBUG}" == "" ]]; then
        echo combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} ${ARGRANGES} --points ${NPOINTS} -n "_env_${CAT}_tot" ${TOYARG}
        if [[ "${DRYRUN}" == "" ]]; then
            combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} ${ARGRANGES} --points ${NPOINTS} -n "_env_${CAT}_tot" ${TOYARG}
        fi
    else
        echo combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} ${ARGRANGES} --points ${NPOINTS} -n "_env_${CAT}_tot" ${TOYARG} --setParameters ${PARAMS} ${DEBUGARG}
        if [[ "${DRYRUN}" == "" ]]; then
            combine -M MultiDimFit -d ${CARD} ${ARGUMENTS} ${ARGRANGES} --points ${NPOINTS} -n "_env_${CAT}_tot" ${TOYARG} --setParameters ${PARAMS} ${DEBUGARG}
        fi
    fi

fi

if [[ "${DRYRUN}" == "" ]]; then
    if [[ "${OBS}" == "" ]]; then
        if [[ "${GEORGE}" != "" ]]; then
            root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(${CAT}, \"\", \"${TAG}\", true)"
        else
            root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(${CAT}, \"\", \"${TAG}\", false)"
        fi
    else
        if [[ "${GEORGE}" != "" ]]; then
            root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(${CAT}, \"\", \"${TAG}\", true, true)"
        else
            root.exe -q -b "${CMSSW_BASE}/src/CLFVAnalysis/Roostats/tools/plot_envelope.C(${CAT}, \"\", \"${TAG}\", false, true)"
        fi
    fi
fi
