#ifndef __LEPHIST__HH
#define __LEPHIST__HH
#include "TH1.h"
#include "TH2.h"

struct LepHist_t {
  TH1D* hOnePz;
  TH1D* hOnePt;
  TH1D* hJetTauOnePt[4]; //bins of extra tau pT range
  TH1D* hOneP;
  TH1D* hOneM;
  TH1D* hOneEta;
  TH1D* hOnePhi;
  TH1D* hOneD0;
  TH1D* hOneIso;
  TH1D* hOneID1;
  TH1D* hOneID2;
  TH1D* hOneRelIso;
  TH1D* hOneFlavor;
  TH1D* hOneQ;
  TH1D* hOneTrigger;
  TH1D* hOneWeight;
  TH1D* hOneWeight1Group;
  TH1D* hOneWeight2Group;
  TH1D* hOneTrigWeight;
  //Gen Info
  TH1D* hOneGenPt;
  TH1D* hOneGenE;
  TH1D* hOneGenEta;
  TH1D* hOneDeltaPt;
  TH1D* hOneDeltaE;
  TH1D* hOneDeltaEta;
  TH1D* hOneMetDeltaPhi;
  //SVFit Info
  TH1D* hOneSVPt;
  TH1D* hOneSVM;
  TH1D* hOneSVEta;
  TH1D* hOneSVDeltaPt;
  TH1D* hOneSVDeltaP;
  TH1D* hOneSVDeltaE;
  TH1D* hOneSVDeltaEta;
  TH1D* hOneSlimEQ;
  TH1D* hOneSlimMuQ;
  TH1D* hOneSlimTauQ;
  TH1D* hOneSlimEM;
  TH1D* hOneSlimEMSS;
  TH1D* hOneSlimEMOS;
  TH1D* hOneSlimMuM;
  TH1D* hOneSlimMuMSS;
  TH1D* hOneSlimMuMOS;
  TH1D* hOneSlimTauM;
  TH1D* hOneSlimTauMSS;
  TH1D* hOneSlimTauMOS;

  TH1D* hTwoPz;
  TH1D* hTwoPt;
  TH1D* hJetTauTwoPt[4]; //bins of extra tau pT range
  TH1D* hTwoP;
  TH1D* hTwoM;
  TH1D* hTwoEta;
  TH1D* hTwoPhi;
  TH1D* hTwoD0;
  TH1D* hTwoIso;
  TH1D* hTwoID1;
  TH1D* hTwoID2;
  TH1D* hTwoID3;
  TH1D* hTwoRelIso;
  TH1D* hTwoFlavor;
  TH1D* hTwoQ;
  TH1D* hTwoTrigger;
  TH1D* hTwoWeight;
  TH1D* hTwoWeight1Group;
  TH1D* hTwoWeight2Group;
  TH1D* hTwoTrigWeight;
  //Gen Info
  TH1D* hTwoGenPt;
  TH1D* hTwoGenE;
  TH1D* hTwoGenEta;
  TH1D* hTwoDeltaPt;
  TH1D* hTwoDeltaE;
  TH1D* hTwoDeltaEta;
  TH1D* hTwoMetDeltaPhi;

  //SVFit Info
  TH1D* hTwoSVPt;
  TH1D* hTwoSVM;
  TH1D* hTwoSVEta;
  TH1D* hTwoSVDeltaPt;
  TH1D* hTwoSVDeltaP;
  TH1D* hTwoSVDeltaE;
  TH1D* hTwoSVDeltaEta;

  TH1D* hTwoSlimEQ;
  TH1D* hTwoSlimMuQ;
  TH1D* hTwoSlimTauQ;
  TH1D* hTwoSlimEM;
  TH1D* hTwoSlimEMSS;
  TH1D* hTwoSlimEMOS;
  TH1D* hTwoSlimMuM;
  TH1D* hTwoSlimMuMSS;
  TH1D* hTwoSlimMuMOS;
  TH1D* hTwoSlimTauM;
  TH1D* hTwoSlimTauMSS;
  TH1D* hTwoSlimTauMOS;


  TH1D* hPtDiff;
  TH1D* hD0Diff;

  //2D distribution
  TH2D* hTwoPtVsOnePt;
};


#endif
