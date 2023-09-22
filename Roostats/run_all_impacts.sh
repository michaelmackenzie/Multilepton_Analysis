#! /bin/bash

DOAPPROX=$1
../../impacts_mva.sh "combine_mva_zetau_25_26_27_28_2016_2017_2018.txt" "" "" "" "${DOAPPROX}"
../../impacts_mva.sh "combine_mva_zetau_mu_25_26_27_2016_2017_2018.txt" "" "" "" "${DOAPPROX}"
../../impacts_mva.sh "combine_mva_total_zetau_had_25_26_27_28_lep_25_26_27_2016_2017_2018.txt" "" "" "" "${DOAPPROX}"
../../impacts_mva.sh "combine_mva_zmutau_25_26_27_28_2016_2017_2018.txt" "" "" "" "${DOAPPROX}"
../../impacts_mva.sh "combine_mva_zmutau_e_25_26_27_2016_2017_2018.txt" "" "" "" "${DOAPPROX}"
../../impacts_mva.sh "combine_mva_total_zmutau_had_25_26_27_28_lep_25_26_27_2016_2017_2018.txt" "" "" "" "${DOAPPROX}"

VERSION="v03"

cp impacts_mva_zetau_25_26_27_28_2016_2017_2018.pdf ../stable/2016_2017_2018/impacts_mva_zetau_2016_2017_2018_approx_${VERSION}.pdf
cp impacts_mva_zetau_mu_25_26_27_2016_2017_2018.pdf ../stable/2016_2017_2018/impacts_mva_zetau_mu_2016_2017_2018_approx_${VERSION}.pdf
cp impacts_mva_total_zetau_had_25_26_27_28_lep_25_26_27_2016_2017_2018.pdf ../stable/2016_2017_2018/impacts_mva_total_zetau_2016_2017_2018_approx_${VERSION}.pdf
cp impacts_mva_zmutau_25_26_27_28_2016_2017_2018.pdf ../stable/2016_2017_2018/impacts_mva_zmutau_2016_2017_2018_approx_${VERSION}.pdf
cp impacts_mva_zmutau_e_25_26_27_2016_2017_2018.pdf ../stable/2016_2017_2018/impacts_mva_zmutau_e_2016_2017_2018_approx_${VERSION}.pdf
cp impacts_mva_total_zmutau_had_25_26_27_28_lep_25_26_27_2016_2017_2018.pdf ../stable/2016_2017_2018/impacts_mva_total_zmutau_2016_2017_2018_approx_${VERSION}.pdf

cp impacts_mva_zetau_25_26_27_28_2016_2017_2018.json ../stable/2016_2017_2018/impacts_mva_zetau_2016_2017_2018_approx_${VERSION}.json
cp impacts_mva_zetau_mu_25_26_27_2016_2017_2018.json ../stable/2016_2017_2018/impacts_mva_zetau_mu_2016_2017_2018_approx_${VERSION}.json
cp impacts_mva_total_zetau_had_25_26_27_28_lep_25_26_27_2016_2017_2018.json ../stable/2016_2017_2018/impacts_mva_total_zetau_2016_2017_2018_approx_${VERSION}.json
cp impacts_mva_zmutau_25_26_27_28_2016_2017_2018.json ../stable/2016_2017_2018/impacts_mva_zmutau_2016_2017_2018_approx_${VERSION}.json
cp impacts_mva_zmutau_e_25_26_27_2016_2017_2018.json ../stable/2016_2017_2018/impacts_mva_zmutau_e_2016_2017_2018_approx_${VERSION}.json
cp impacts_mva_total_zmutau_had_25_26_27_28_lep_25_26_27_2016_2017_2018.json ../stable/2016_2017_2018/impacts_mva_total_zmutau_2016_2017_2018_approx_${VERSION}.json

echo "Finished processing/copying"
