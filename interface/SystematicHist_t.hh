#ifndef __SYSTEMATICHIST__HH
#define __SYSTEMATICHIST__HH
#include "TH1.h"
#include "TH2.h"
#include "interface/EventHist_t.hh"
#include "interface/GlobalConstants.h"

namespace CLFV {

  struct SystematicHist_t {
    TH1D* hEventWeight[kMaxSystematics];
    TH1D* hPuWeight[kMaxSystematics];
    TH1D* hTopPtWeight[kMaxSystematics];
    TH1D* hBTagWeight[kMaxSystematics];
    TH1D* hZPtWeight[kMaxSystematics];
    TH1D* hWeightChange[kMaxSystematics];

    TH1D* hNPV[kMaxSystematics];
    TH1D* hNPU[kMaxSystematics];
    TH1D* hNJets[kMaxSystematics];
    TH1D* hNJets20[kMaxSystematics];
    TH1D* hNBJets[kMaxSystematics];
    TH1D* hNBJetsM[kMaxSystematics];
    TH1D* hNBJetsL[kMaxSystematics];
    TH1D* hNBJets20[kMaxSystematics];
    TH1D* hNBJets20M[kMaxSystematics];
    TH1D* hNBJets20L[kMaxSystematics];

    TH1D* hTauDecayMode[kMaxSystematics];
    TH1D* hTauDeepAntiEle[kMaxSystematics];
    TH1D* hTauDeepAntiMu [kMaxSystematics];
    TH1D* hTauDeepAntiJet[kMaxSystematics];

    TH1D* hJetPt[kMaxSystematics];

    TH1D* hMet[kMaxSystematics];
    TH1D* hMetPhi[kMaxSystematics];
    TH1D* hMetCorr[kMaxSystematics];
    TH1D* hMetCorrPhi[kMaxSystematics];

    //lepton histograms
    TH1D* hOnePt[kMaxSystematics];
    TH1D* hOneEta[kMaxSystematics];
    TH1D* hTwoPt[kMaxSystematics];
    TH1D* hTwoEta[kMaxSystematics];

    //di-lepton histograms
    TH1D* hLepPt[kMaxSystematics];
    TH1D* hLepP[kMaxSystematics];
    TH1D* hLepM[kMaxSystematics];
    TH1D* hLepMt[kMaxSystematics];
    TH1D* hLepEta[kMaxSystematics];
    TH1D* hLepPhi[kMaxSystematics];
    TH1D* hLepDeltaPhi[kMaxSystematics];
    TH1D* hLepDeltaEta[kMaxSystematics];
    TH1D* hLepDeltaR[kMaxSystematics];
    TH1D* hLepPtOverM[kMaxSystematics];

    //Gen-level Z info
    TH1D* hZPt[kMaxSystematics];
    TH1D* hZMass[kMaxSystematics];

    //Delta-Alpha histograms
    TH1D* hAlpha[4][kMaxSystematics]; //alpha from arXiv:1207.4894
    TH1D* hDeltaAlpha[4][kMaxSystematics]; //delta alpha from arXiv:1207.4894
    TH1D* hDeltaAlphaM[2][kMaxSystematics]; //mass found by solving delta alpha equations (flipping with lepton is tau for each)
    TH1D* hDeltaAlphaMColM[2][kMaxSystematics]; //Collimated mass - delta alpha mass

    TH1D* hMetDeltaPhi[kMaxSystematics];
    TH1D* hJetDeltaPhi[kMaxSystematics];
    TH1D* hLepOneDeltaPhi[kMaxSystematics];
    TH1D* hLepTwoDeltaPhi[kMaxSystematics];

    //Transverse Masses
    TH1D* hMTOne[kMaxSystematics];
    TH1D* hMTTwo[kMaxSystematics];
    TH1D* hMTLep[kMaxSystematics];
    TH1D* hMTOneOverM[kMaxSystematics];
    TH1D* hMTTwoOverM[kMaxSystematics];

    //For assuming MET along tau is tau neutrino, only makes sense for e/mu + tau
    TH1D* hPTauVisFrac[kMaxSystematics];
    TH1D* hLepMEstimate[kMaxSystematics];
    TH1D* hLepMEstimateTwo[kMaxSystematics];

    //MVA values
    TH1D* hMVA[kMaxMVAs][kMaxSystematics];

  };
}
#endif
