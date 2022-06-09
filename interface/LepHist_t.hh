#ifndef __LEPHIST__HH
#define __LEPHIST__HH
#include "TH1.h"
#include "TH2.h"

namespace CLFV {
  struct LepHist_t {
    TH1* hJetTauOnePt[5/*tau DM*/]; //0: inclusive 1: DM 0 2: DM 1 3: DM 10 4: DM 11
    TH1* hJetTauOnePtQCD[2]; //0: nominal 1: no non-closure
    TH1* hJetTauOneR;
    TH1* hJetTauOneEta;
    TH2* hJetTauOnePtVsR[5/*tau DM + inclusive*/];
    TH1* hJetTauOneMetDeltaPhi[5/*tau DM + inclusive*/];
    TH1* hJetTauOneMetDPhiQCD[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauOneRelIso[2]; //0: nominal 1: no bias correction

    TH1* hJetTauOneMetDPhiComp; //no j-->tau weights for composition
    TH1* hJetTauTwoMetDPhiComp;
    TH1* hJetTauOnePtComp;
    TH1* hJetTauTwoPtComp;
    TH1* hJetTauMTOneComp;
    TH1* hJetTauMTTwoComp;

    TH1* hOnePz;
    TH1* hOnePt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1* hOneP;
    TH1* hOneM;
    TH1* hOnePtOverM;
    TH1* hOneEta;
    TH1* hOnePhi;
    TH1* hOneD0;
    TH1* hOneDXY;
    TH1* hOneDZ;
    TH1* hOneIso;
    TH1* hOneID1;
    TH1* hOneID2;
    TH1* hOneRelIso;
    TH1* hOneFlavor;
    TH1* hOneGenFlavor;
    TH1* hOneQ;
    TH1* hOneTrigger;
    TH1* hOneWeight;
    TH1* hOneWeight1Group;
    TH1* hOneWeight2Group;
    TH1* hOneTrigWeight;
    TH1* hOneJetOverlap;
    //Gen Info
    TH1* hOneGenPt;
    TH1* hOneGenE;
    TH1* hOneGenEta;
    TH1* hOneDeltaPt;
    TH1* hOneDeltaE;
    TH1* hOneDeltaEta;
    TH1* hOneMetDeltaPhi;
    //SVFit Info
    TH1* hOneSVPt;
    TH1* hOneSVM;
    TH1* hOneSVEta;
    TH1* hOneSVDeltaPt;
    TH1* hOneSVDeltaP;
    TH1* hOneSVDeltaE;
    TH1* hOneSVDeltaEta;

    TH1* hJetTauTwoPt[5/*tau DM*/];
    TH1* hJetTauTwoR[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauTwoEta[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauTwoEtaQCD[2]; //0: nominal 1: no j-->tau correction
    TH2* hJetTauTwoPtVsR[5/*tau DM*/];
    TH1* hJetTauTwoMetDeltaPhi[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauTwoRelIso[2]; //0: nominal 1: no bias correction

    TH1* hTwoPz;
    TH1* hTwoPt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1* hTwoP;
    TH1* hTwoM;
    TH1* hTwoPtOverM;
    TH1* hTwoEta;
    TH1* hTwoPhi;
    TH1* hTwoD0;
    TH1* hTwoDXY;
    TH1* hTwoDZ;
    TH1* hTwoIso;
    TH1* hTwoID1;
    TH1* hTwoID2;
    TH1* hTwoID3;
    TH1* hTwoRelIso;
    TH1* hTwoFlavor;
    TH1* hTwoGenFlavor;
    TH1* hTwoQ;
    TH1* hTwoTrigger;
    TH1* hTwoWeight;
    TH1* hTwoWeight1Group;
    TH1* hTwoWeight2Group;
    TH1* hTwoTrigWeight;
    TH1* hTwoJetOverlap;
    //Gen Info
    TH1* hTwoGenPt;
    TH1* hTwoGenE;
    TH1* hTwoGenEta;
    TH1* hTwoDeltaPt;
    TH1* hTwoDeltaE;
    TH1* hTwoDeltaEta;
    TH1* hTwoMetDeltaPhi;

    //SVFit Info
    TH1* hTwoSVPt;
    TH1* hTwoSVM;
    TH1* hTwoSVEta;
    TH1* hTwoSVDeltaPt;
    TH1* hTwoSVDeltaP;
    TH1* hTwoSVDeltaE;
    TH1* hTwoSVDeltaEta;


    TH1* hPtDiff;
    TH1* hD0Diff;
    TH1* hDXYDiff;
    TH1* hDZDiff;

    //2D distribution
    TH2* hTwoPtVsOnePt;
    TH2* hJetTauTwoPtVsOnePt;

  };
}
#endif
