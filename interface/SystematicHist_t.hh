#ifndef __SYSTEMATICHIST__HH
#define __SYSTEMATICHIST__HH
#include "TH1.h"
#include "TH2.h"
#include "interface/EventHist_t.hh"
#include "interface/GlobalConstants.h"

namespace CLFV {

  struct SystematicHist_t {
    TH1* hEventWeight[kMaxSystematics];
    TH1* hPuWeight[kMaxSystematics];
    TH1* hTopPtWeight[kMaxSystematics];
    TH1* hBTagWeight[kMaxSystematics];
    TH1* hZPtWeight[kMaxSystematics];
    TH1* hWeightChange[kMaxSystematics];

    TH1* hNPV[kMaxSystematics];
    TH1* hNPU[kMaxSystematics];
    TH1* hNJets[kMaxSystematics];
    TH1* hNJets20[kMaxSystematics];
    TH1* hNBJets[kMaxSystematics];
    TH1* hNBJetsM[kMaxSystematics];
    TH1* hNBJetsL[kMaxSystematics];
    TH1* hNBJets20[kMaxSystematics];
    TH1* hNBJets20M[kMaxSystematics];
    TH1* hNBJets20L[kMaxSystematics];

    TH1* hTauDecayMode[kMaxSystematics];
    TH1* hTauDeepAntiEle[kMaxSystematics];
    TH1* hTauDeepAntiMu [kMaxSystematics];
    TH1* hTauDeepAntiJet[kMaxSystematics];

    TH1* hJetPt[kMaxSystematics];

    TH1* hMet[kMaxSystematics];
    TH1* hMetPhi[kMaxSystematics];
    TH1* hMetCorr[kMaxSystematics];
    TH1* hMetCorrPhi[kMaxSystematics];

    //lepton histograms
    TH1* hOnePt[kMaxSystematics];
    TH1* hOneEta[kMaxSystematics];
    TH1* hTwoPt[kMaxSystematics];
    TH1* hTwoEta[kMaxSystematics];

    //di-lepton histograms
    TH1* hLepPt[kMaxSystematics];
    TH1* hLepP[kMaxSystematics];
    TH1* hLepM[kMaxSystematics];
    TH1* hLepMt[kMaxSystematics];
    TH1* hLepEta[kMaxSystematics];
    TH1* hLepPhi[kMaxSystematics];
    TH1* hLepDeltaPhi[kMaxSystematics];
    TH1* hLepDeltaEta[kMaxSystematics];
    TH1* hLepDeltaR[kMaxSystematics];
    TH1* hLepPtOverM[kMaxSystematics];

    //Gen-level Z info
    TH1* hZPt[kMaxSystematics];
    TH1* hZMass[kMaxSystematics];

    //Delta-Alpha histograms
    TH1* hAlpha[4][kMaxSystematics]; //alpha from arXiv:1207.4894
    TH1* hDeltaAlpha[4][kMaxSystematics]; //delta alpha from arXiv:1207.4894
    TH1* hDeltaAlphaM[2][kMaxSystematics]; //mass found by solving delta alpha equations (flipping with lepton is tau for each)
    TH1* hDeltaAlphaMColM[2][kMaxSystematics]; //Collimated mass - delta alpha mass

    TH1* hMetDeltaPhi[kMaxSystematics];
    TH1* hJetDeltaPhi[kMaxSystematics];
    TH1* hLepOneDeltaPhi[kMaxSystematics];
    TH1* hLepTwoDeltaPhi[kMaxSystematics];

    //Transverse Masses
    TH1* hMTOne[kMaxSystematics];
    TH1* hMTTwo[kMaxSystematics];
    TH1* hMTLep[kMaxSystematics];
    TH1* hMTOneOverM[kMaxSystematics];
    TH1* hMTTwoOverM[kMaxSystematics];

    //For assuming MET along tau is tau neutrino, only makes sense for e/mu + tau
    TH1* hPTauVisFrac[kMaxSystematics];
    TH1* hLepMEstimate[kMaxSystematics];
    TH1* hLepMEstimateTwo[kMaxSystematics];

    //MVA values
    TH1* hMVA[kMaxMVAs][kMaxSystematics];
    TH1* hMVADiff[kMaxMVAs][kMaxSystematics]; //for re-evaluated scores

    //Total histograms
    TH1* hNFills; //N(fills) for a given event

  };
}
#endif
