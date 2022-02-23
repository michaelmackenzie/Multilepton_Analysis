#ifndef __EVENTHIST__HH
#define __EVENTHIST__HH
#include "TH1.h"
#include "TH2.h"
#include "interface/JetToTauComposition.hh"

namespace CLFV {
  enum{kMaxCounts = 40, kMaxMVAs = 80};
  struct EventHist_t {
    TH1* hLumiSection;
    TH1* hTriggerStatus;
    TH1* hNTriggered;
    TH1* hEventWeight;
    TH1* hLogEventWeight;
    TH1* hEventWeightMVA;
    TH1* hFullEventWeightLum;
    TH1* hLogFullEventWeightLum;
    TH1* hGenWeight;
    TH1* hEmbeddingWeight;
    TH1* hLogEmbeddingWeight;
    TH1* hEmbeddingUnfoldingWeight;
    TH1* hGenTauFlavorWeight;
    TH1* hPhotonIDWeight;
    TH1* hJetToTauWeight;
    TH1* hJetToTauWeightGroup;
    TH1* hJetToTauWeightCorr;
    TH1* hJetToTauComps[JetToTauComposition::kLast];
    TH1* hJetToTauWts  [JetToTauComposition::kLast];
    TH1* hIsSignal;
    TH1* hNPV[2]; //0: with PU weights 1: without PU weights
    TH1* hNPU[2]; //0: with PU weights 1: without PU weights
    TH1* hNPartons;
    TH1* hLHENJets;
    TH1* hNMuons;
    TH1* hNSlimMuons;
    TH1* hNMuonCounts[kMaxCounts];
    TH1* hNElectrons;
    TH1* hNSlimElectrons;
    TH1* hNElectronCounts[kMaxCounts];
    TH1* hNLowPtElectrons;
    TH1* hNTaus;
    TH1* hNSlimTaus;
    TH1* hNTauCounts[kMaxCounts];
    TH1* hNPhotons;
    TH1* hNSlimPhotons;
    TH1* hNSlimJets;
    TH1* hNGenTausHad;
    TH1* hNGenTausLep;
    TH1* hNGenTaus;
    TH1* hNGenElectrons;
    TH1* hNGenMuons;
    TH1* hNGenHardTaus;
    TH1* hNGenHardElectrons;
    TH1* hNGenHardMuons;
    TH1* hNJets;
    TH1* hNJets20[2];
    TH1* hNJets20Rej[2];
    TH1* hNFwdJets;
    TH1* hNBJets;
    TH1* hNBJetsM;
    TH1* hNBJetsL;
    TH1* hNBJets20[2]; //0: nominal 1: without btag weight
    TH1* hNBJets20M[2]; //0: nominal 1: without btag weight
    TH1* hNBJets20L[2]; //0: nominal 1: without btag weight
    TH1* hJetsFlavor;
    TH2D* hJetsPtVsEta  [3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
    TH2D* hBJetsPtVsEta [3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
    TH2D* hBJetsMPtVsEta[3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
    TH2D* hBJetsLPtVsEta[3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
    TH2D* hJetsPtVsEtaPU[2]; //jet pT vs eta for PU ID MC efficiency calculation
    TH1* hMcEra;
    TH1* hTriggerLeptonStatus;
    TH1* hMuonTriggerStatus;
    TH1* hPuWeight;
    TH1* hJetPUIDWeight;
    TH1* hPrefireWeight;
    TH1* hTopPtWeight;
    TH1* hBTagWeight;
    TH1* hZPtWeight;
    TH1* hTauDecayMode[2]; //with and without event weights
    TH1* hTauMVA;
    TH1* hTauGenFlavor;
    TH1* hTauGenFlavorHad;
    TH1* hTauVetoedJetPt;
    TH1* hTauVetoedJetPtUnc;
    TH1* hTauDeepAntiEle;
    TH1* hTauDeepAntiMu ;
    TH1* hTauDeepAntiJet;
    TH1* hHtSum;
    TH1* hHt;
    TH1* hHtPhi;
    TH1* hJetPt[2];
    TH1* hJetM;
    TH1* hJetEta;
    TH1* hJetPhi;
    TH1* hJetBTag;
    TH1* hJetBMVA;
    TH1* hTauPt;
    TH1* hTauM;
    TH1* hTauEta;
    TH1* hTauPhi;
    TH1* hPuppMet;
    TH1* hPFMet;
    TH1* hPFMetPhi;
    TH1* hPFCovMet00;
    TH1* hPFCovMet01;
    TH1* hPFCovMet11;
    TH1* hTrkMet;
    TH1* hTrkMetPhi;

    TH1* hMet;
    TH1* hMetPhi;
    TH1* hMetCorr;
    TH1* hMetCorrPhi;
    TH1* hCovMet00;
    TH1* hCovMet01;
    TH1* hCovMet11;
    TH2D* hMetVsPt;
    TH2D* hMetVsM;
    TH2D* hMetVsHtSum; //MET ~ sqrt(htsum)
    TH1* hMetOverSqrtHtSum; //MET ~ sqrt(htsum)

    TH1* hMassSVFit;
    TH1* hMassErrSVFit;
    TH1* hSVFitStatus;

    //di-lepton histograms
    TH1* hLepPt[3]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY
    TH1* hLepP;
    TH1* hLepM[5]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY 3: Zoomed on Z Mass 4: Zoomed on H Mass
    TH1* hLepMt;
    TH1* hLepEta;
    TH1* hLepPhi;
    TH2D* hLepPtVsM[5]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY 3: using systematic gen-level weights 4: using systematic reco-level weights
    TH2D* hLepMVsMVA[2]; //0: zemu MVA 1: hemu MVA

    //Gen-level Z info
    TH2D* hZPtVsM[5]; //0: normal 1: remove Z pT weight 2: apply weights using reco scales if DY 3: using systematic gen-level weights 4: using systematic reco-level weights
    TH1* hZPt[5];
    TH1* hZMass[5];
    TH1* hZLepOnePt;
    TH1* hZLepTwoPt;
    TH1* hZLepOneEta;
    TH1* hZLepTwoEta;

    TH1* hLooseLep;

    //Jet --> tau_h histograms
    TH2D* hFakeTauNJetDMPtEta  [5/*leading muon pT regions*/][3/*iso cat*/][5/*njets*/][4/*DM*/];
    TH2D* hFakeTauMCNJetDMPtEta[5/*leading muon pT regions*/][3/*iso cat*/][5/*njets*/][4/*DM*/];
    TH1* hTausPt;
    TH1* hTausEta;
    TH1* hTausDM;
    TH1* hTausAntiJet;
    TH1* hTausAntiEle;
    TH1* hTausAntiMu;
    TH1* hTausMVAAntiMu;
    TH1* hTausGenFlavor;
    TH1* hTausDeltaR;
    TH1* hFakeTausPt;
    TH1* hFakeTausEta;
    TH1* hFakeTausDM;

    //Jet --> electron/muon histograms
    TH2D* hFakeLepPtEta[3/*iso cat*/];
    TH1* hLeptonsPt;
    TH1* hLeptonsEta;
    TH1* hLeptonsID;
    TH1* hLeptonsIsoID;
    TH1* hLeptonsGenFlavor;

    TH1* hLepDeltaPhi[2]; //0: nominal 1: without qcd weights
    TH1* hLepDeltaEta;
    TH1* hLepDeltaR[3]; //0: nominal 1: without qcd weights 2: scale bin with weights
    TH2D* hLepDelRVsPhi;
    TH2D* hLepDelRVsOneEta[2]; //0: nominal 1: without QCD weights
    TH2D* hLepDelPhiVsOneEta[2]; //0: nominal 1: without QCD weights

    TH1* hLepPtOverM;
    TH1* hAlpha[4]; //alpha from arXiv:1207.4894
    TH1* hDeltaAlpha[4]; //delta alpha from arXiv:1207.4894
    TH1* hDeltaAlphaM[2]; //mass found by solving delta alpha equations (flipping with lepton is tau for each)
    TH1* hDeltaAlphaMColM[2]; //Collimated mass - delta alpha mass

    TH1* hHtDeltaPhi;
    TH1* hMetDeltaPhi;
    TH1* hJetDeltaPhi;
    TH1* hLepOneDeltaPhi;
    TH1* hLepTwoDeltaPhi;

    //angles between leptons and jets
    TH1* hLepOneJetDeltaR;
    TH1* hLepOneJetDeltaPhi;
    TH1* hLepOneJetDeltaEta;
    TH1* hLepTwoJetDeltaR;
    TH1* hLepTwoJetDeltaPhi;
    TH1* hLepTwoJetDeltaEta;

    TH1* hLepSVPt;
    TH1* hLepSVM;
    TH1* hLepSVEta;
    TH1* hLepSVPhi;

    TH1* hLepSVDeltaPhi;
    TH1* hLepSVDeltaEta;
    TH1* hLepSVDeltaM;
    TH1* hLepSVDeltaPt;
    TH1* hLepSVPtOverM;

    TH1* hSysM;
    TH1* hSysPt;
    TH1* hSysEta;
    TH2D* hSysMvsLepM;

    //Transverse Masses
    TH1* hMTOne;
    TH1* hMTTwo;
    TH2D* hMTTwoVsOne;
    TH1* hMTLep;
    TH1* hMTOneOverM;
    TH1* hMTTwoOverM;

    //three sets for combining photon with a lepton or leptons vs photon
    TH1* hPXiVis[3];
    TH1* hPXiInv[3];
    TH1* hPXiVisOverInv[3];
    TH2D* hPXiInvVsVis[3];
    TH1* hPXiDiff[3];
    TH1* hPXiDiff2[3];//difference with coeffecients and offset
    TH1* hPXiDiff3[3];

    //For assuming MET along tau is tau neutrino, only makes sense for e/mu + tau
    TH1* hPTauVisFrac;
    TH1* hLepMEstimate;
    TH1* hLepMEstimateTwo;
    TH1* hLepDot;

    //Transformed frame variables
    TH1* hLepOnePrimePx[3];
    TH1* hLepTwoPrimePx[3];
    TH1* hMetPrimePx   [3];
    TH1* hLepOnePrimePy[3];
    TH1* hLepTwoPrimePy[3];
    TH1* hMetPrimePy   [3];
    TH1* hLepOnePrimePz[3];
    TH1* hLepTwoPrimePz[3];
    TH1* hMetPrimePz   [3];
    TH1* hLepOnePrimeE [3];
    TH1* hLepTwoPrimeE [3];
    TH1* hMetPrimeE    [3];

    TH1* hPtSum[2]; //scalar sum of lepton Pt and Met, and photon for one
    TH1* hPt1Sum[4]; //scalar sum of 1 lepton Pt and Met, both leptons, then both minus met
    //MVA values
    TH1* hMVA[kMaxMVAs][2]; //0: high binning score for CDF 1: distribution for fitting
    TH1* hMVATrain[kMaxMVAs];
    TH1* hMVATest[kMaxMVAs];

    //llg study histograms
    TH1* hObjMasses[14]; //jets, jets+gamma, jet1/2 + gamma, jets + l1/2, jet1/2 + l1/2, jets+l1+l2, jets + gamma + l1/2, jets + gamma + l1 + l2
    TH1* hJetTwoM;
    TH1* hJetTwoPt;
    TH1* hJetTwoEta;
    TH1* hJetTwoBTag;
    TH1* hJetTwoBMVA;
    TH1* hJetsDeltaR;
    TH1* hJetsDeltaEta;
    TH1* hJetsDeltaPhi;
    TH1* hJetsPt;
    TH1* hJetsEta;
    TH1* hJetsGammaDeltaR;
    TH1* hJetsGammaDeltaEta;
    TH1* hJetsGammaDeltaPhi;
    TH1* hJetsGammaPt;
    TH1* hJetsGammaEta;
  };
}
#endif
