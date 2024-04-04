//Histograms to study the effect on variables when varying the energy scale up/down

#ifndef __LEPESHIST__HH
#define __LEPESHIST__HH

#include "TH1.h"
#include "TH2.h"

namespace CLFV {
  struct LepESHist_t {
    TH1* hLepM[3]; //histograms setup: 0 = nominal; 1 = up; 2 = down
    TH1* hLepMEstimate[3];
    TH1* hLepMEstimateTwo[3];
    TH1* hLepPt[3];
    TH1* hMET[3];
    TH1* hMTOne[3];
    TH1* hMTTwo[3];
    TH1* hMTLep[3];
    TH1* hOneMETDeltaPhi[3];
    TH1* hTwoMETDeltaPhi[3];
    TH1* hOnePt[3];
    TH1* hTwoPt[3];
    TH1* hLepPtRatio[3];
    TH1* hBeta[2][3];
    TH1* hMVA[3]; //most relevant MVA selection
  };
}

#endif
