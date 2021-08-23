#ifndef __EVENTHIST__HH
#define __EVENTHIST__HH
#include "TH1.h"
#include "TH2.h"
#include "interface/JetToTauComposition.hh"

enum{kMaxCounts = 40, kMaxMVAs = 80};
struct EventHist_t {
  TH1D* hLumiSection;
  TH1D* hTriggerStatus;
  TH1D* hNTriggered;
  TH1D* hEventWeight;
  TH1D* hEventWeightMVA;
  TH1D* hGenWeight;
  TH1D* hGenTauFlavorWeight;
  TH1D* hPhotonIDWeight;
  TH1D* hJetToTauWeight;
  TH1D* hJetToTauWeightGroup;
  TH1D* hJetToTauWeightCorr;
  TH1D* hJetToTauComps[JetToTauComposition::kLast];
  TH1D* hJetToTauWts  [JetToTauComposition::kLast];
  TH1D* hIsSignal;
  TH1D* hNPV[2]; //0: with PU weights 1: without PU weights
  TH1D* hNPU[2]; //0: with PU weights 1: without PU weights
  TH1D* hNPartons;
  TH1D* hLHENJets;
  TH1D* hNMuons;
  TH1D* hNSlimMuons;
  TH1D* hNMuonCounts[kMaxCounts];
  TH1D* hNElectrons;
  TH1D* hNSlimElectrons;
  TH1D* hNElectronCounts[kMaxCounts];
  TH1D* hNLowPtElectrons;
  TH1D* hNTaus;
  TH1D* hNSlimTaus;
  TH1D* hNTauCounts[kMaxCounts];
  TH1D* hNPhotons;
  TH1D* hNSlimPhotons;
  TH1D* hNSlimJets;
  TH1D* hNGenTausHad;
  TH1D* hNGenTausLep;
  TH1D* hNGenTaus;
  TH1D* hNGenElectrons;
  TH1D* hNGenMuons;
  TH1D* hNGenHardTaus;
  TH1D* hNGenHardElectrons;
  TH1D* hNGenHardMuons;
  TH1D* hNJets;
  TH1D* hNJets20[2];
  TH1D* hNJets20Rej[2];
  TH1D* hNFwdJets;
  TH1D* hNBJets;
  TH1D* hNBJetsM;
  TH1D* hNBJetsL;
  TH1D* hNBJets20[2]; //0: nominal 1: without btag weight
  TH1D* hNBJets20M[2]; //0: nominal 1: without btag weight
  TH1D* hNBJets20L[2]; //0: nominal 1: without btag weight
  TH1D* hJetsFlavor;
  TH2D* hJetsPtVsEta  [3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
  TH2D* hBJetsPtVsEta [3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
  TH2D* hBJetsMPtVsEta[3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
  TH2D* hBJetsLPtVsEta[3]; //0: gen-level light jet 1: gen-level c-jet 2: gen-level b-jet
  TH2D* hJetsPtVsEtaPU[2]; //jet pT vs eta for PU ID MC efficiency calculation
  TH1D* hMcEra;
  TH1D* hTriggerLeptonStatus;
  TH1D* hMuonTriggerStatus;
  TH1D* hPuWeight;
  TH1D* hJetPUIDWeight;
  TH1D* hPrefireWeight;
  TH1D* hTopPtWeight;
  TH1D* hBTagWeight;
  TH1D* hZPtWeight;
  TH1D* hTauDecayMode[2]; //with and without event weights
  TH1D* hTauMVA;
  TH1D* hTauGenFlavor;
  TH1D* hTauGenFlavorHad;
  TH1D* hTauVetoedJetPt;
  TH1D* hTauVetoedJetPtUnc;
  TH1D* hTauDeepAntiEle;
  TH1D* hTauDeepAntiMu ;
  TH1D* hTauDeepAntiJet;
  TH1D* hHtSum;
  TH1D* hHt;
  TH1D* hHtPhi;
  TH1D* hJetPt[2];
  TH1D* hJetM;
  TH1D* hJetEta;
  TH1D* hJetPhi;
  TH1D* hJetBTag;
  TH1D* hJetBMVA;
  TH1D* hTauPt;
  TH1D* hTauM;
  TH1D* hTauEta;
  TH1D* hTauPhi;
  TH1D* hPuppMet;
  TH1D* hPFMet;
  TH1D* hPFMetPhi;
  TH1D* hPFCovMet00;
  TH1D* hPFCovMet01;
  TH1D* hPFCovMet11;
  TH1D* hTrkMet;
  TH1D* hTrkMetPhi;

  TH1D* hMet;
  TH1D* hMetPhi;
  TH1D* hMetCorr;
  TH1D* hMetCorrPhi;
  TH1D* hCovMet00;
  TH1D* hCovMet01;
  TH1D* hCovMet11;
  TH2D* hMetVsPt;
  TH2D* hMetVsM;
  TH2D* hMetVsHtSum; //MET ~ sqrt(htsum)
  TH1D* hMetOverSqrtHtSum; //MET ~ sqrt(htsum)

  TH1D* hMassSVFit;
  TH1D* hMassErrSVFit;
  TH1D* hSVFitStatus;

  //di-lepton histograms
  TH1D* hLepPt[3]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY
  TH1D* hLepP;
  TH1D* hLepM[5]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY 3: Zoomed on Z Mass 4: Zoomed on H Mass
  TH1D* hLepMt;
  TH1D* hLepEta;
  TH1D* hLepPhi;
  TH2D* hLepPtVsM[5]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY 3: using systematic gen-level weights 4: using systematic reco-level weights
  TH2D* hLepMVsMVA[2]; //0: zemu MVA 1: hemu MVA

  //Gen-level Z info
  TH2D* hZPtVsM[5]; //0: normal 1: remove Z pT weight 2: apply weights using reco scales if DY 3: using systematic gen-level weights 4: using systematic reco-level weights
  TH1D* hZPt[5];
  TH1D* hZMass[5];

  TH1D* hLooseLep;

  //Jet --> tau_h histograms
  TH2D* hFakeTauNJetDMPtEta  [5/*leading muon pT regions*/][3/*iso cat*/][5/*njets*/][4/*DM*/];
  TH2D* hFakeTauMCNJetDMPtEta[5/*leading muon pT regions*/][3/*iso cat*/][5/*njets*/][4/*DM*/];
  TH1D* hTausPt;
  TH1D* hTausEta;
  TH1D* hTausDM;
  TH1D* hTausAntiJet;
  TH1D* hTausAntiEle;
  TH1D* hTausAntiMu;
  TH1D* hTausMVAAntiMu;
  TH1D* hTausGenFlavor;
  TH1D* hTausDeltaR;
  TH1D* hFakeTausPt;
  TH1D* hFakeTausEta;
  TH1D* hFakeTausDM;

  //Jet --> electron/muon histograms
  TH2D* hFakeLepPtEta[3/*iso cat*/];
  TH1D* hLeptonsPt;
  TH1D* hLeptonsEta;
  TH1D* hLeptonsID;
  TH1D* hLeptonsIsoID;
  TH1D* hLeptonsGenFlavor;

  TH1D* hLepDeltaPhi[2]; //0: nominal 1: without qcd weights
  TH1D* hLepDeltaEta;
  TH1D* hLepDeltaR[3]; //0: nominal 1: without qcd weights 2: scale bin with weights
  TH2D* hLepDelRVsPhi;
  TH2D* hLepDelRVsOneEta[2]; //0: nominal 1: without QCD weights
  TH2D* hLepDelPhiVsOneEta[2]; //0: nominal 1: without QCD weights

  TH1D* hLepPtOverM;
  TH1D* hAlpha[4]; //alpha from arXiv:1207.4894
  TH1D* hDeltaAlpha[4]; //delta alpha from arXiv:1207.4894
  TH1D* hDeltaAlphaM[2]; //mass found by solving delta alpha equations (flipping with lepton is tau for each)
  TH1D* hDeltaAlphaMColM[2]; //Collimated mass - delta alpha mass

  TH1D* hHtDeltaPhi;
  TH1D* hMetDeltaPhi;
  TH1D* hJetDeltaPhi;
  TH1D* hLepOneDeltaPhi;
  TH1D* hLepTwoDeltaPhi;

  //angles between leptons and jets
  TH1D* hLepOneJetDeltaR;
  TH1D* hLepOneJetDeltaPhi;
  TH1D* hLepOneJetDeltaEta;
  TH1D* hLepTwoJetDeltaR;
  TH1D* hLepTwoJetDeltaPhi;
  TH1D* hLepTwoJetDeltaEta;

  TH1D* hLepSVPt;
  TH1D* hLepSVM;
  TH1D* hLepSVEta;
  TH1D* hLepSVPhi;

  TH1D* hLepSVDeltaPhi;
  TH1D* hLepSVDeltaEta;
  TH1D* hLepSVDeltaM;
  TH1D* hLepSVDeltaPt;
  TH1D* hLepSVPtOverM;

  TH1D* hSysM;
  TH1D* hSysPt;
  TH1D* hSysEta;
  TH2D* hSysMvsLepM;

  //Transverse Masses
  TH1D* hMTOne;
  TH1D* hMTTwo;
  TH1D* hMTLep;
  TH1D* hMTOneOverM;
  TH1D* hMTTwoOverM;

  //three sets for combining photon with a lepton or leptons vs photon
  TH1D* hPXiVis[3];
  TH1D* hPXiInv[3];
  TH1D* hPXiVisOverInv[3];
  TH2D* hPXiInvVsVis[3];
  TH1D* hPXiDiff[3];
  TH1D* hPXiDiff2[3];//difference with coeffecients and offset
  TH1D* hPXiDiff3[3];

  //For assuming MET along tau is tau neutrino, only makes sense for e/mu + tau
  TH1D* hPTauVisFrac;
  TH1D* hLepMEstimate;
  TH1D* hLepMEstimateTwo;
  TH1D* hLepDot;

  TH1D* hPtSum[2]; //scalar sum of lepton Pt and Met, and photon for one
  TH1D* hPt1Sum[4]; //scalar sum of 1 lepton Pt and Met, both leptons, then both minus met
  //MVA values
  TH1D* hMVA[kMaxMVAs][2]; //0: high binning score for CDF 1: distribution for fitting
  TH1D* hMVATrain[kMaxMVAs];
  TH1D* hMVATest[kMaxMVAs];

  //llg study histograms
  TH1D* hObjMasses[14]; //jets, jets+gamma, jet1/2 + gamma, jets + l1/2, jet1/2 + l1/2, jets+l1+l2, jets + gamma + l1/2, jets + gamma + l1 + l2
  TH1D* hJetTwoM;
  TH1D* hJetTwoPt;
  TH1D* hJetTwoEta;
  TH1D* hJetTwoBTag;
  TH1D* hJetTwoBMVA;
  TH1D* hJetsDeltaR;
  TH1D* hJetsDeltaEta;
  TH1D* hJetsDeltaPhi;
  TH1D* hJetsPt;
  TH1D* hJetsEta;
  TH1D* hJetsGammaDeltaR;
  TH1D* hJetsGammaDeltaEta;
  TH1D* hJetsGammaDeltaPhi;
  TH1D* hJetsGammaPt;
  TH1D* hJetsGammaEta;
};


#endif
