//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Sep 10 09:06:07 2019 by ROOT version 6.06/01
// from TTree bltTree_ttbar_inclusive/bltTree_ttbar_inclusive
// found on file: /eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/output_ttbar_inclusive.root
//////////////////////////////////////////////////////////

#ifndef ZTauTauHistMaker_hh
#define ZTauTauHistMaker_hh


#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TSystem.h>

// Headers needed by this particular selector
#include "TLorentzVector.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TRandom.h" //for splitting testing/training samples

// TMVA includes
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

#include <iostream>

//local includes
#include "../dataFormats/SlimElectron_t.hh"
#include "../dataFormats/SlimMuon_t.hh"
#include "../dataFormats/SlimTau_t.hh"
#include "../dataFormats/SlimJet_t.hh"
#include "../dataFormats/SlimPhoton_t.hh"
#include "../dataFormats/Tree_t.hh"
//initialize local MVA weight files
#include "../utils/TrkQualInit.cc"
//define PU weights locally
#include "../utils/PUWeight.hh"

class ZTauTauHistMaker : public TSelector {
public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
  enum {kMaxParticles = 50, kMaxCounts = 40};
  enum {kMuTau = 0, kETau = 30, kEMu = 60, kMuTauE = 90, kETauMu = 105, kMuMu = 120, kEE = 145};
  enum {kMaxMVAs = 80};

  // Readers to access the data (delete the ones you do not need).
  UInt_t runNumber                   ;
  ULong64_t eventNumber              ;
  UInt_t lumiSection                 ;
  UInt_t nPV                         ;
  Float_t nPU                        ;
  UInt_t nPartons                    ;
  UInt_t mcEra                       ;
  UInt_t triggerLeptonStatus         ;
  Float_t eventWeight                ;
  Float_t genWeight                  ;
  Float_t puWeight                   ;
  Float_t topPtWeight                ;
  Float_t btagWeight = 1.            ;
  Float_t zPtWeight                  ;
  Float_t zPt = -1.                  ;
  Float_t zMass = -1.                ;
  Bool_t  looseQCDSelection = false  ;
  Float_t genTauFlavorWeight         ;
  Int_t tauDecayMode                 ;
  Float_t tauMVA                     ;
  Int_t tauGenFlavor                 ;
  Int_t tauGenFlavorHad              ;
  Float_t tauVetoedJetPt             ;
  Float_t tauVetoedJetPtUnc          ;
  TLorentzVector* leptonOneP4 = 0    ;
  TLorentzVector* leptonTwoP4 = 0    ;
  Int_t leptonOneFlavor              ;
  Int_t leptonTwoFlavor              ;
  Float_t leptonOneD0                ;
  Float_t leptonTwoD0		     ;
  Float_t leptonOneIso		     ;
  Float_t leptonTwoIso               ; 
  UChar_t leptonOneID1               ;
  UChar_t leptonOneID2               ;
  UChar_t leptonTwoID1               ;
  UChar_t leptonTwoID2               ;
  Int_t   leptonOneIndex             ;
  Int_t   leptonTwoIndex             ;
  Float_t leptonOneWeight            ;
  Float_t leptonTwoWeight            ;
  Float_t leptonOneTrigWeight        ;
  Float_t leptonTwoTrigWeight        ;
  Bool_t  leptonOneFired             ;
  Bool_t  leptonTwoFired             ;
  TLorentzVector* genLeptonOneP4 = 0 ;
  TLorentzVector* genLeptonTwoP4 = 0 ;
  TLorentzVector* photonP4 = 0       ;
  Float_t         photonMVA          ;
  Float_t         photonIDWeight     ;
  TLorentzVector* jetOneP4 = 0       ;
  TLorentzVector* jetTwoP4 = 0       ;
  Bool_t jetOneBTag                  ;
  Float_t jetOneBMVA                 ;
  Bool_t jetTwoBTag                  ;
  Float_t jetTwoBMVA                 ;
  TLorentzVector* tauP4 = 0          ;
  Int_t  tauFlavor                   ;
  UChar_t tauDeepAntiEle             ;
  UChar_t tauDeepAntiMu              ;
  UChar_t tauDeepAntiJet             ;
  UInt_t nMuons                      ;
  UInt_t nSlimMuons                  ;
  SlimMuons_t* slimMuons             ;
  UInt_t nMuonCounts[kMaxCounts]     ;
  UInt_t nElectrons                  ;
  UInt_t nSlimElectrons              ;
  SlimElectrons_t* slimElectrons     ;
  UInt_t nElectronCounts[kMaxCounts] ;
  UInt_t nLowPtElectrons             ;
  UInt_t nTaus                       ;
  UInt_t nSlimTaus                   ;
  SlimTaus_t* slimTaus               ;
  UInt_t nTauCounts[kMaxCounts]      ;
  UInt_t nPhotons                    ;
  UInt_t nSlimPhotons                ;
  SlimPhotons_t* slimPhotons         ;
  UInt_t nJets                       ;
  UInt_t nSlimJets                   ;
  SlimJets_t* slimJets               ;
  UInt_t nJets20                     ;
  UInt_t nFwdJets                    ;
  UInt_t nBJetsUse                   ; //which to count
  UInt_t nBJets                      ;
  UInt_t nBJetsM                     ;
  UInt_t nBJetsL                     ;
  UInt_t nBJets20                    ;
  UInt_t nBJets20M                   ;
  UInt_t nBJets20L                   ;
  Float_t jetsPt[kMaxParticles]      ;
  Float_t jetsEta[kMaxParticles]     ;
  Int_t   jetsFlavor[kMaxParticles]  ;
  Int_t   jetsBTag[kMaxParticles]    ;
  UInt_t nGenTausHad                 ;
  UInt_t nGenTausLep                 ;
  UInt_t nGenElectrons               ;
  UInt_t nGenMuons                   ;
  UInt_t nGenHardTaus                ; //for DY splitting by hard process
  UInt_t nGenHardElectrons           ; //for DY splitting by hard process
  UInt_t nGenHardMuons               ; //for DY splitting by hard process
  Float_t htSum                      ;
  Float_t ht                         ;
  Float_t htPhi                      ;
  //Different met definitions
  Float_t pfMET                      ;
  Float_t pfMETphi                   ;
  Float_t pfMETCov00                 ;
  Float_t pfMETCov01                 ;
  Float_t pfMETCov11                 ;
  Float_t pfMETC                     ;
  Float_t pfMETCphi                  ;
  Float_t pfMETCCov00                ;
  Float_t pfMETCCov01                ;
  Float_t pfMETCCov11                ;
  Float_t puppMET                    ;
  Float_t puppMETphi                 ;
  Float_t puppMETCov00               ;
  Float_t puppMETCov01               ;
  Float_t puppMETCov11               ;
  Float_t puppMETC                   ;
  Float_t puppMETCphi                ;
  Float_t puppMETCCov00              ;
  Float_t puppMETCCov01              ;
  Float_t puppMETCCov11              ;
  Float_t alpacaMET                  ;
  Float_t alpacaMETphi               ;
  Float_t pcpMET                     ;
  Float_t pcpMETphi                  ;
  Float_t trkMET                     ;
  Float_t trkMETphi                  ;
  //used in original SVFit algorithm
  Float_t met                        ;
  Float_t metPhi                     ;
  Float_t metCorr                    ;
  Float_t metCorrPhi                 ;
  Float_t covMet00                   ;
  Float_t covMet01                   ;
  Float_t covMet11                   ;
  Float_t massSVFit                  ;
  Float_t massErrSVFit               ;
  Int_t   svFitStatus                ;
  TLorentzVector* leptonOneSVP4 = 0  ;
  TLorentzVector* leptonTwoSVP4 = 0  ;

  struct EventHist_t {
    TH1D* hLumiSection;
    TH1D* hTriggerStatus;
    TH1D* hNTriggered;
    TH1D* hEventWeight;
    TH1D* hGenWeight;
    TH1D* hGenTauFlavorWeight;
    TH1D* hPhotonIDWeight;
    TH1D* hIsSignal;
    TH1D* hNPV[2]; //0: with PU weights 1: without PU weights
    TH1D* hNPU[2]; //0: with PU weights 1: without PU weights
    TH1D* hNPartons;
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
    TH1D* hNJets20;
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
    TH1D* hMcEra;
    TH1D* hTriggerLeptonStatus;
    TH1D* hPuWeight;
    TH1D* hTopPtWeight;
    TH1D* hBTagWeight;
    TH1D* hZPtWeight;
    TH1D* hTauDecayMode;
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
    TH1D* hJetPt;
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
    TH1D* hLepEta;
    TH1D* hLepPhi;
    TH2D* hLepPtVsM[3]; //0: normal 1: remove Z pT weight if DY file 2: apply weights using reco scales if DY

    //Gen-level Z info
    TH2D* hZPtVsM[3]; //0: normal 1: remove Z pT weight 2: apply weights using reco scales if DY
    TH1D* hZPt[3];
    TH1D* hZMass[3];
    
    TH1D* hLepDeltaPhi;
    TH1D* hLepDeltaEta;
    TH1D* hLepDeltaR;
    TH2D* hLepDelRVsPhi;
    
    TH1D* hLepPtOverM;
    TH1D* hAlpha[4]; //alpha from arXiv:1207.4894
    TH1D* hDeltaAlpha[4]; //delta alpha from arXiv:1207.4894
    TH1D* hDeltaAlphaM[2]; //mass found by solving delta alpha equations (flipping with lepton is tau for each)
    
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
    
    TH1D* hPtSum[2]; //scalar sum of lepton Pt and Met, and photon for one
    TH1D* hPt1Sum[4]; //scalar sum of 1 lepton Pt and Met, both leptons, then both minus met
    //MVA values
    TH1D* hMVA[kMaxMVAs];
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

  struct LepHist_t {
    TH1D* hOnePz;
    TH1D* hOnePt;
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
    TH1D* hTwoP;
    TH1D* hTwoM;
    TH1D* hTwoEta;
    TH1D* hTwoPhi;
    TH1D* hTwoD0;
    TH1D* hTwoIso;
    TH1D* hTwoID1;
    TH1D* hTwoID2;
    TH1D* hTwoRelIso;
    TH1D* hTwoFlavor;
    TH1D* hTwoQ;
    TH1D* hTwoTrigger;
    TH1D* hTwoWeight;
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

  struct PhotonHist_t {
    TH1D* hPz;
    TH1D* hPt;
    TH1D* hP;
    TH1D* hEta;
    TH1D* hPhi;
    TH1D* hMVA;
    // TH1D* hIso;
    // TH1D* hRelIso;
    // TH1D* hTrigger;
  };


  //define object to apply box cuts 
  class TEventID {
  public:
    enum {
      kLepm           = 0x1 <<0,
      kLepmestimate   = 0x1 <<1,
      kLeppt          = 0x1 <<2,
      kMtone          = 0x1 <<3,
      kMttwo          = 0x1 <<4,
      kLeponept       = 0x1 <<5,
      kLeptwopt       = 0x1 <<6,
      kOnemetdeltaphi = 0x1 <<7,
      kTwometdeltaphi = 0x1 <<8,
      kLeponedeltaphi = 0x1 <<9,
      kLeptwodeltaphi = 0x1 <<10,
      kHt             = 0x1 <<11,
      kNJets          = 0x1 <<12,
      kJetpt          = 0x1 <<13,
      kLepdeltaphi    = 0x1 <<14,
      kMet            = 0x1 <<15
    };
    Float_t fMinLepm          ;
    Float_t fMinLepmestimate  ;
    Float_t fMinLeppt         ;
    Float_t fMinMtone         ;
    Float_t fMinMttwo         ;
    Float_t fMinLeponept      ;
    Float_t fMinLeptwopt      ;
    Float_t fMinOnemetdeltaphi;
    Float_t fMinTwometdeltaphi;
    Float_t fMinLeponedeltaphi;
    Float_t fMinLeptwodeltaphi;
    Float_t fMinHt            ;
    Float_t fMinNJets         ;
    Float_t fMinJetpt         ;
    Float_t fMinLepdeltaphi   ;
    Float_t fMinMet           ;

    Float_t fMaxLepm          ;
    Float_t fMaxLepmestimate  ;
    Float_t fMaxLeppt         ;
    Float_t fMaxMtone         ;
    Float_t fMaxMttwo         ;
    Float_t fMaxLeponept      ;
    Float_t fMaxLeptwopt      ;
    Float_t fMaxOnemetdeltaphi;
    Float_t fMaxTwometdeltaphi;
    Float_t fMaxLeponedeltaphi;
    Float_t fMaxLeptwodeltaphi;
    Float_t fMaxHt            ;
    Float_t fMaxNJets         ;
    Float_t fMaxJetpt         ;
    Float_t fMaxLepdeltaphi   ;
    Float_t fMaxMet           ;

    Int_t fUseMask;
    
    TEventID() {
      fUseMask  = 0xffffffff; //id mask to select which cuts to use

      fMinLepm           = -1.e9;
      fMinLepmestimate   = -1.e9;
      fMinLeppt          = -1.e9;
      fMinMtone          = -1.e9;
      fMinMttwo          = -1.e9;
      fMinLeponept       = -1.e9;
      fMinLeptwopt       = -1.e9;
      fMinOnemetdeltaphi = -1.e9;
      fMinTwometdeltaphi = -1.e9;
      fMinLeponedeltaphi = -1.e9;
      fMinLeptwodeltaphi = -1.e9;
      fMinHt             = -1.e9;
      fMinNJets          = -1.e9;
      fMinJetpt          = -1.e9;
      fMinMet            = -1.e9;
      fMinLepdeltaphi    = -1.e9;
      
      fMaxLepm           =  1.e9;
      fMaxLepmestimate   =  1.e9;
      fMaxLeppt          =  1.e9;
      fMaxMtone          =  1.e9;
      fMaxMttwo          =  1.e9;
      fMaxLeponept       =  1.e9;
      fMaxLeptwopt       =  1.e9;
      fMaxOnemetdeltaphi =  1.e9;
      fMaxTwometdeltaphi =  1.e9;
      fMaxLeponedeltaphi =  1.e9;
      fMaxLeptwodeltaphi =  1.e9;
      fMaxHt             =  1.e9;
      fMaxNJets          =  1.e9;
      fMaxJetpt          =  1.e9;
      fMaxMet            =  1.e9;
      fMaxLepdeltaphi    =  1.e9;
    }

    Int_t IDWord (Tree_t variables) {
      Int_t id_word(0);
      if(variables.lepm           < fMinLepm           || variables.lepm            > fMaxLepm           ) id_word |= kLepm           ;
      if(variables.mestimate      < fMinLepmestimate   || variables.mestimate       > fMaxLepmestimate   ) id_word |= kLepmestimate   ;
      if(variables.leppt          < fMinLeppt          || variables.leppt           > fMaxLeppt          ) id_word |= kLeppt          ;
      if(variables.mtone          < fMinMtone          || variables.mtone           > fMaxMtone          ) id_word |= kMtone          ;
      if(variables.mttwo          < fMinMttwo          || variables.mttwo           > fMaxMttwo          ) id_word |= kMttwo          ;
      if(variables.leponept       < fMinLeponept       || variables.leponept        > fMaxLeponept       ) id_word |= kLeponept       ;
      if(variables.leptwopt       < fMinLeptwopt       || variables.leptwopt        > fMaxLeptwopt       ) id_word |= kLeptwopt       ;
      if(variables.onemetdeltaphi < fMinOnemetdeltaphi || variables.onemetdeltaphi  > fMaxOnemetdeltaphi ) id_word |= kOnemetdeltaphi ;
      if(variables.twometdeltaphi < fMinTwometdeltaphi || variables.twometdeltaphi  > fMaxTwometdeltaphi ) id_word |= kTwometdeltaphi ;
      if(variables.leponedeltaphi < fMinLeponedeltaphi || variables.leponedeltaphi  > fMaxLeponedeltaphi ) id_word |= kLeponedeltaphi ;
      if(variables.leptwodeltaphi < fMinLeptwodeltaphi || variables.leptwodeltaphi  > fMaxLeptwodeltaphi ) id_word |= kLeptwodeltaphi ;
      if(variables.ht             < fMinHt             || variables.ht              > fMaxHt             ) id_word |= kHt             ;
      if(variables.njets          < fMinNJets          || variables.njets           > fMaxNJets          ) id_word |= kNJets          ;
      if(variables.jetpt          < fMinJetpt          || variables.jetpt           > fMaxJetpt          ) id_word |= kJetpt          ;
      if(variables.lepdeltaphi    < fMinLepdeltaphi    || variables.lepdeltaphi     > fMaxLepdeltaphi    ) id_word |= kLepdeltaphi    ;
      if(variables.met            < fMinMet            || variables.met             > fMaxMet            ) id_word |= kMet            ;
      return (id_word & fUseMask);
    }
  };
  
  ZTauTauHistMaker(TTree * /*tree*/ =0) { }
  virtual ~ZTauTauHistMaker() { }
  virtual Int_t   Version() const { return 2; }
  virtual void    Begin(TTree *tree);
  virtual void    SlaveBegin(TTree *tree);
  virtual void    Init(TTree *tree);
  virtual Bool_t  Notify();
  virtual Bool_t  Process(Long64_t entry);
  virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void    SetOption(const char *option) { fOption = option; }
  virtual void    SetObject(TObject *obj) { fObject = obj; }
  virtual void    SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate();
  virtual void    Terminate();

  virtual void    FillAllHistograms(Int_t index);
  virtual void    BookHistograms();
  virtual void    BookEventHistograms();
  virtual void    BookPhotonHistograms();
  virtual void    BookLepHistograms();
  virtual void    BookTrees();
  virtual void    FillEventHistogram(EventHist_t* Hist);
  virtual void    FillPhotonHistogram(PhotonHist_t* Hist);
  virtual void    FillLepHistogram(LepHist_t* Hist);
  virtual void    InitializeTreeVariables(Int_t selection);
  virtual float   GetTauFakeSF(int genFlavor);
  virtual float   CorrectMET(int selection, float met);
  virtual float   GetZPtWeight(double pt, double mass = 0., int doReco = 0);
  virtual void    CountSlimObjects();

  virtual void    ProcessLLGStudy();


  Long64_t fentry; //for tracking entry in functions
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();
  TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
  vector<TString> fMvaNames = { //mva names for getting weights
    "mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
    "etau_BDT_38.higgs","etau_BDT_38.Z0",
    "emu_BDT_68.higgs","emu_BDT_68.Z0",
    "mutau_e_BDT_68.higgs","mutau_e_BDT_68.Z0",
    "etau_mu_BDT_68.higgs","etau_mu_BDT_68.Z0"};
  vector<double> fMvaCuts = {                          //mva score cut values
			     0.12, 0.04, //mutau       //scores optimize the gained 95% CL
			     0.06, -0.02,//etau        //all are done by eye on limit gain vs MVA score plot
			     0.02, -0.11, //emu
			     0.10, 0.02, //mutau_e
			     0.08, 0.02  //etau_mu
  };
  Int_t   fIsJetBinnedMVAs[kMaxMVAs]; //storing number of jets for MVA, < 0 if not binned
  Float_t fMvaOutputs[kMaxMVAs];
  Int_t   fTrkQualVersion = TrkQualInit::Default; //for updating which variables are used
  
  //Histograms:
  const static Int_t fn = 400; //max histogram sets
  const static Int_t kIds = 60; //max box cut ID sets
  const static Int_t fQcdOffset = 200; //histogram set + offset = set with same sign selection
  Int_t fEventSets[fn];  //indicates which sets to create
  Int_t fTreeSets[fn];   //indicates which trees to create

  TFile*        fOut;
  TDirectory*   fTopDir;
  TDirectory*   fDirectories[4*fn]; // 0 - fn events, fn - 2fn photon, 2fn - 3fn lep, 3fn - 4fn trees
  EventHist_t*  fEventHist[fn];
  PhotonHist_t* fPhotonHist[fn];
  LepHist_t*    fLepHist[fn];
  TTree*        fTrees[fn];

  TEventID*     fEventId[kIds]; //for applying box cuts, 0-9 zmutau, 10-19 zetau, 20-29 zemu, higgs + 30 to z sets
  
  TString       fFolderName = ""; //name of the folder the tree is from
  Int_t         fYear = 2016;

  Bool_t        fIsSignal = false;
  
  Int_t         fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: e/mu e/mu
  Bool_t        fIsDY = false; //for checking if DY --> Z pT weights
  Bool_t        fDYTesting = false; //for speeding up histogramming to only do DY weight related aspects
  
  Int_t         fWriteTrees = 0; //write out ttrees for the events
  Double_t      fXsec = 0.; //cross-section for full event weight with trees
  Double_t      fLum = 0.; //luminosity full event weight with trees
  Tree_t        fTreeVars; //for filling the ttrees/mva evaluation
  Int_t         fEventCategory; //for identifying the process in mva trainings

  Int_t         fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
  Int_t         fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
  bool          fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
  Int_t         fMETWeights = 0; //re-weight events based on the MET

  Int_t         fRemoveTriggerWeights = 0;
  Int_t         fRemoveBTagWeights = 0;

  Int_t         fRemovePUWeights = 0; //0: do nothing 1: remove weights 2: replace weights
  PUWeight      fPUWeight; //object to define pu weights
  
  Int_t         fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally
  TH2D*         fZPtScales = 0; //histogram based z pTvsM weights
  TH2D*         fZPtRecoScales = 0; //histogram based reconstructed z pTvsM weights
  TString       fZPtHistPath = "scale_factors/z_pt_vs_m_scales_"; //path for file, not including _[year].root
  
  float         fFractionMVA = 0.; //fraction of events used to train. Ignore these events in histogram filling, reweight the rest to compensate
  TRandom*      fRnd = 0; //for splitting MVA testing/training
  Int_t         fRndSeed = 90; //random number generator seed
  bool          fReprocessMVAs = false; //whether or not to use the tree given MVA values
  Int_t         fBJetCounting = 1; // 0: pT > 30 1: pT > 25 2: pT > 20
  Int_t         fBJetTightness = 1; // 0: tight 1: medium 2: loose
  Int_t         fMETType = 1; // 0: PF corrected 1: PUPPI Corrected
  bool          fForceBJetSense = true; //force can't be more strict id bjets than looser id bjets
  bool          fIsNano = false; //whether the tree is nano AOD based or not

  Int_t         fVerbose = 0; //verbosity level
  
  ClassDef(ZTauTauHistMaker,0);

};

#endif

#ifdef ZTauTauHistMaker_cxx
void ZTauTauHistMaker::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  if(fChain == 0 && tree != 0) {

    if(!fDYTesting)
      tree->SetBranchStatus("*", 1); //ensure all branches are active
    else {
      tree->SetBranchStatus("*", 0); //turn off everything except what is needed (or hard to remove)
      tree->SetBranchStatus("triggerLeptonStatus" , 1);
      tree->SetBranchStatus("lepOneTrigWeight"    , 1);
      tree->SetBranchStatus("lepTwoTrigWeight"    , 1);
      tree->SetBranchStatus("lepOneFired"         , 1);
      tree->SetBranchStatus("lepTwoFired"         , 1);
      tree->SetBranchStatus("eventWeight"         , 1);
      tree->SetBranchStatus("genWeight"           , 1);
      tree->SetBranchStatus("puWeight"            , 1);
      tree->SetBranchStatus("topPtWeight"         , 1);
      tree->SetBranchStatus("btagWeight"          , 1);
      tree->SetBranchStatus("zPtWeight"           , 1);
      tree->SetBranchStatus("zPt"                 , 1);
      tree->SetBranchStatus("zMass"               , 1);
      tree->SetBranchStatus("leptonOneP4" 	  , 1);
      tree->SetBranchStatus("leptonTwoP4" 	  , 1);
      tree->SetBranchStatus("leptonOneFlavor"     , 1);
      tree->SetBranchStatus("leptonTwoFlavor"     , 1);
      tree->SetBranchStatus("leptonOneIndex"      , 1);
      tree->SetBranchStatus("leptonTwoIndex"      , 1);
      tree->SetBranchStatus("lepOneWeight"        , 1);
      tree->SetBranchStatus("lepTwoWeight"        , 1);
      tree->SetBranchStatus("photonP4"  	  , 1);
      tree->SetBranchStatus("jetP4"  	          , 1);
      tree->SetBranchStatus("nMuons"              , 1);
      tree->SetBranchStatus("nElectrons"          , 1);
      tree->SetBranchStatus("nTaus"               , 1);
      tree->SetBranchStatus("nPhotons"            , 1);
      tree->SetBranchStatus("nJets"               , 1);
      tree->SetBranchStatus("nJets20"             , 1);
      tree->SetBranchStatus("nFwdJets"            , 1);
      tree->SetBranchStatus("nBJets"              , 1);
      tree->SetBranchStatus("nBJetsM"             , 1);
      tree->SetBranchStatus("nBJetsL"             , 1);
      tree->SetBranchStatus("nBJets20"            , 1);
      tree->SetBranchStatus("nBJets20M"           , 1);
      tree->SetBranchStatus("nBJets20L"           , 1);
      tree->SetBranchStatus("jetsPt"              , 1);
      tree->SetBranchStatus("jetsEta"             , 1);
      tree->SetBranchStatus("jetsFlavor"          , 1);
      tree->SetBranchStatus("jetsBTag"            , 1);
      tree->SetBranchStatus("nGenTausHad"         , 1);
      tree->SetBranchStatus("nGenTausLep"         , 1);
      tree->SetBranchStatus("nGenElectrons"       , 1);
      tree->SetBranchStatus("nGenMuons"           , 1);
      tree->SetBranchStatus("nGenPromptTaus"      , 1);
      tree->SetBranchStatus("nGenPromptElectrons" , 1);
      tree->SetBranchStatus("nGenPromptMuons"     , 1);
      tree->SetBranchStatus("puppMET"             , 1);
      tree->SetBranchStatus("puppMETphi"          , 1);
      tree->SetBranchStatus("puppMETC"            , 1);
      tree->SetBranchStatus("puppMETCphi"         , 1);
      tree->SetBranchStatus("met"                 , 1);
      tree->SetBranchStatus("metPhi"              , 1);
      tree->SetBranchStatus("metCorr"             , 1);
      tree->SetBranchStatus("metCorrPhi"          , 1);
    }
    TMVA::Tools::Instance(); //load the TMVA library
    for(int i = 0; i < kMaxMVAs; ++i) mva[i] = 0; //initially 0s

    if(fReprocessMVAs) {
      for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i) {
      
	mva[mva_i] = new TMVA::Reader("!Color:!Silent");
	TString selection = "";
	if(fMvaNames[mva_i].Contains("higgs")) selection += "h";
	else selection += "z";
	if(fMvaNames[mva_i].Contains("mutau")) selection += "mutau";
	else if(fMvaNames[mva_i].Contains("etau")) selection += "etau";
	else if(fMvaNames[mva_i].Contains("emu")) selection += "emu";
	else {
	  printf ("Warning! Didn't determine mva weight file %s corresponding selection!\n",
		  fMvaNames[mva_i].Data());
	  selection += "mutau"; //just to default to something
	}
	//add for leptonic tau channels FIXME: Put as part of original check
	if(fMvaNames[mva_i].Contains("mutau_e")) selection += "_e";
	else if(fMvaNames[mva_i].Contains("etau_mu")) selection += "_mu";

	Int_t isJetBinned = -1; // -1 is not binned, 0 = 0 jets, 1 = 1 jet, 2 = >1 jets
	if(fMvaNames[mva_i].Contains("_18") || //0 jet
	   fMvaNames[mva_i].Contains("_48") ||
	   fMvaNames[mva_i].Contains("_78"))
	  isJetBinned = 0;
	else if(fMvaNames[mva_i].Contains("_19") || //1 jet
		fMvaNames[mva_i].Contains("_49") ||
		fMvaNames[mva_i].Contains("_79"))
	  isJetBinned = 1;
	else if(fMvaNames[mva_i].Contains("_20") || //>1 jet
		fMvaNames[mva_i].Contains("_50") ||
		fMvaNames[mva_i].Contains("_80"))
	  isJetBinned = 2;

	fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling
      
	printf("Using selection %s\n", selection.Data());
	//use a tool to initialize the MVA variables and spectators
	TrkQualInit trkQualInit(fTrkQualVersion, isJetBinned);
	trkQualInit.InitializeVariables(*(mva[mva_i]), selection, fTreeVars);
	//Initialize MVA weight file
	const char* f = Form("weights/%s.weights.xml",fMvaNames[mva_i].Data());
	mva[mva_i]->BookMVA(fMvaNames[mva_i].Data(),f);
	printf("Booked MVA %s with selection %s\n", fMvaNames[mva_i].Data(), selection.Data());
      }
    }
    // slimMuons = new SlimMuons_t();
    // slimElectrons = new SlimElectrons_t();
    // slimTaus = new SlimTaus_t();
    // slimPhotons = new SlimPhotons_t();
    // slimJets = new SlimJets_t();
    
    fOut = new TFile(Form("ztautau%s%s_%s.hist",(fFolderName == "") ? "" : ("_"+fFolderName).Data(),
			  (fDYType > 0) ? Form("_%i",fDYType) : "",tree->GetName()),
		     "RECREATE","ZTauTauHistMaker output histogram file");
    fTopDir = fOut->mkdir("Data");
    fTopDir->cd();

    for(int i = 0; i < fn; ++i) {
      fEventSets[i]  = 0;
    }
    for(int i = 0; i < fn; ++i) {
      fTreeSets[i]  = 0;
    }

    //Event Sets
    //currently sets 0-29 are mutau, 30-59 are etau, and 60-89 are emu
    // fEventSets [0] = 0; // all events
    if(fFolderName == "mutau") {
      fEventSets [kMuTau + 1] = 1; // all opposite signed events
      fEventSets [kMuTau + 1+fQcdOffset] = 1; // all same signed events
      fEventSets [kMuTau + 2] = 1; // events with opposite signs and >= 1 photon
      fEventSets [kMuTau + 2+fQcdOffset] = 1; // events with same signs and >= 1 photon
    
      fEventSets [kMuTau + 3] = 1; 
      fEventSets [kMuTau + 3+fQcdOffset] = 1; 
      fEventSets [kMuTau + 4] = 1;
      fEventSets [kMuTau + 4+fQcdOffset] = 1;
      fEventSets [kMuTau + 5] = 1;
      fEventSets [kMuTau + 5+fQcdOffset] = 1;
      fTreeSets  [kMuTau + 5] = 1;
      fEventSets [kMuTau + 6] = 1;
      fEventSets [kMuTau + 6+fQcdOffset] = 1;
      fTreeSets  [kMuTau + 6] = 1;

      fEventSets [kMuTau + 7] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts with no photon check
      fEventSets [kMuTau + 7+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts with no photon check
      fTreeSets  [kMuTau + 7] = 1;

      // Sets 8-10 MVA cuts applied
      fEventSets [kMuTau + 8] = 1; // events with opposite signs
      fEventSets [kMuTau + 8+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kMuTau + 8] = 1;
      fEventSets [kMuTau + 9] = 1; // events with opposite signs 
      fEventSets [kMuTau + 9+fQcdOffset] = 1; // events with same signs 
      fEventSets [kMuTau + 10] = 1; // events with opposite signs 
      fEventSets [kMuTau + 10+fQcdOffset] = 1; // events with same signs 
    
      fEventSets [kMuTau + 11] = 1; // events with opposite signs and mass window
      fEventSets [kMuTau + 11+fQcdOffset] = 1; // events with same signs and mass window
      fEventSets [kMuTau + 12] = 1; // events with opposite signs and mass window
      fEventSets [kMuTau + 12+fQcdOffset] = 1; // events with same signs and mass window

      //background regions
      fEventSets [kMuTau + 13] = 1; // events with opposite signs and top set
      fEventSets [kMuTau + 13+fQcdOffset] = 1; // events with same signs and top set
      fEventSets [kMuTau + 14] = 1; // events with opposite signs and w+jets set
      fEventSets [kMuTau + 14+fQcdOffset] = 1; // events with same signs and w+jets set
      fEventSets [kMuTau + 15] = 1; // events with opposite signs and z+jets set
      fEventSets [kMuTau + 15+fQcdOffset] = 1; // events with same signs and z+jets set

      // fEventSets [kMuTau + 16] = 1; // events with opposite signs and z box cuts
      // fEventSets [kMuTau + 16+fQcdOffset] = 1; // events with same signs and z box cuts
      // fEventSets [kMuTau + 17] = 1; // events with opposite signs and higgs box cuts
      // fEventSets [kMuTau + 17+fQcdOffset] = 1; // events with same signs and higgs box cuts

      fEventSets [kMuTau + 18] = 1; // events with opposite signs and nJets = 0
      fEventSets [kMuTau + 18+fQcdOffset] = 1; // events with same signs and nJets = 0
      fTreeSets  [kMuTau + 18] = 1;
      fEventSets [kMuTau + 19] = 1; // events with opposite signs and nJets = 1
      fEventSets [kMuTau + 19+fQcdOffset] = 1; // events with same signs and nJets = 1
      fTreeSets  [kMuTau + 19] = 1;
      fEventSets [kMuTau + 20] = 1; // events with opposite signs and nJets > 1
      fEventSets [kMuTau + 20+fQcdOffset] = 1; // events with same signs and nJets > 1
      fTreeSets  [kMuTau + 20] = 1;

      fEventSets [kMuTau + 21] = 1; // events with opposite signs and nPhotons = 0
      fEventSets [kMuTau + 21+fQcdOffset] = 1; // events with same signs and nPhotons = 0
      fEventSets [kMuTau + 22] = 1; // events with opposite signs and nPhotons > 0
      fEventSets [kMuTau + 22+fQcdOffset] = 1; // events with same signs and nPhotons > 0
      fEventSets [kMuTau + 23] = 1; 
      fEventSets [kMuTau + 23+fQcdOffset] = 1; 

      //Test sets
      fEventSets [kMuTau + 24] = 1;
      fEventSets [kMuTau + 24+fQcdOffset] = 1;
      fEventSets [kMuTau + 25] = 1;
      fEventSets [kMuTau + 25+fQcdOffset] = 1;
      
    }
    else if(fFolderName == "etau") {
      //E+Tau
      fEventSets [kETau  + 1] = 1; // all opposite signed events
      fEventSets [kETau  + 1+fQcdOffset] = 1; // all same signed events
      fEventSets [kETau  + 2] = 1; // events with opposite signs and >= 1 photon
      fEventSets [kETau  + 2+fQcdOffset] = 1; // events with same signs and >= 1 photon
    
      fEventSets [kETau  + 3] = 1; 
      fEventSets [kETau  + 3+fQcdOffset] = 1; 
      fEventSets [kETau  + 4] = 1;
      fEventSets [kETau  + 4+fQcdOffset] = 1;
      fEventSets [kETau  + 5] = 1;
      fEventSets [kETau  + 5+fQcdOffset] = 1;
      fTreeSets  [kETau + 5] = 1;
      fEventSets [kETau  + 6] = 1;
      fEventSets [kETau  + 6+fQcdOffset] = 1;
      fTreeSets  [kETau + 6] = 1;

      // fEventSets [kETau  + 6] = 1; // events with opposite signs and passing Mu+Tau Pt cuts with no photon check
      // fEventSets [kETau  + 6+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt cuts with no photon check

      fEventSets [kETau  + 7] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts with no photon check
      fEventSets [kETau  + 7+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts with no photon check
      fTreeSets  [kETau  + 7] = 1;

      // Sets 8-10 MVA cuts applied
      fEventSets [kETau  + 8] = 1; // events with opposite signs
      fEventSets [kETau  + 8+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kETau  + 8] = 1;
      fEventSets [kETau  + 9] = 1; // events with opposite signs 
      fEventSets [kETau  + 9+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kETau  + 9] = 1;
      fEventSets [kETau  + 10] = 1; // events with opposite signs 
      fEventSets [kETau  + 10+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kETau  + 10] = 1;
    
      fEventSets [kETau  + 11] = 1; // events with opposite signs and mass window
      fEventSets [kETau  + 11+fQcdOffset] = 1; // events with same signs and mass window
      fEventSets [kETau  + 12] = 1; // events with opposite signs and mass window
      fEventSets [kETau  + 12+fQcdOffset] = 1; // events with same signs and mass window

      //background regions
      fEventSets [kETau  + 13] = 1; // events with opposite signs and top set
      fEventSets [kETau  + 13+fQcdOffset] = 1; // events with same signs and top set
      fEventSets [kETau  + 14] = 1; // events with opposite signs and w+jets set
      fEventSets [kETau  + 14+fQcdOffset] = 1; // events with same signs and w+jets set
      fEventSets [kETau  + 15] = 1; // events with opposite signs and z+jets set
      fEventSets [kETau  + 15+fQcdOffset] = 1; // events with same signs and z+jets set

      // fEventSets [kETau  + 16] = 1; // events with opposite signs and z box cuts
      // fEventSets [kETau  + 16+fQcdOffset] = 1; // events with same signs and z box cuts
      // fEventSets [kETau  + 17] = 1; // events with opposite signs and higgs box cuts
      // fEventSets [kETau  + 17+fQcdOffset] = 1; // events with same signs and higgs box cuts

      fEventSets [kETau  + 18] = 1; // events with opposite signs and nJets = 0
      fEventSets [kETau  + 18+fQcdOffset] = 1; // events with same signs and nJets = 0
      fTreeSets  [kETau  + 18] = 1;
      fEventSets [kETau  + 19] = 1; // events with opposite signs and nJets = 1
      fEventSets [kETau  + 19+fQcdOffset] = 1; // events with same signs and nJets = 1
      fTreeSets  [kETau  + 19] = 1;
      fEventSets [kETau  + 20] = 1; // events with opposite signs and nJets > 1
      fEventSets [kETau  + 20+fQcdOffset] = 1; // events with same signs and nJets > 1
      fTreeSets  [kETau  + 20] = 1;

      fEventSets [kETau  + 21] = 1; // events with opposite signs and nPhotons = 0
      fEventSets [kETau  + 21+fQcdOffset] = 1; // events with same signs and nPhotons = 0
      fEventSets [kETau  + 22] = 1; // events with opposite signs and nPhotons > 0
      fEventSets [kETau  + 22+fQcdOffset] = 1; // events with same signs and nPhotons > 0
      fEventSets [kETau  + 23] = 1; 
      fEventSets [kETau  + 23+fQcdOffset] = 1; 

      //Test sets
      fEventSets [kETau  + 24] = 1;
      fEventSets [kETau  + 24+fQcdOffset] = 1; 
      fEventSets [kETau  + 25] = 1;
      fEventSets [kETau  + 25+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 26] = 1;
      // fEventSets [kEMu   + 26+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 27] = 1;
      // fEventSets [kEMu   + 27+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 28] = 1;
      // fEventSets [kEMu   + 28+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 29] = 1;
      // fEventSets [kEMu   + 29+fQcdOffset] = 1; 

    }
    else if(fFolderName == "emu") {
      //E+Mu
      fEventSets [kEMu   + 1] = 1; // all opposite signed events
      fEventSets [kEMu   + 1+fQcdOffset] = 1; // all same signed events
      fEventSets [kEMu   + 2] = 1; // events with opposite signs and >= 1 photon
      fEventSets [kEMu   + 2+fQcdOffset] = 1; // events with same signs and >= 1 photon
    
      fEventSets [kEMu   + 3] = 1; 
      fEventSets [kEMu   + 3+fQcdOffset] = 1; 
      fEventSets [kEMu   + 4] = 1;
      fEventSets [kEMu   + 4+fQcdOffset] = 1;
      fEventSets [kEMu   + 5] = 1;
      fEventSets [kEMu   + 5+fQcdOffset] = 1;
      fTreeSets  [kEMu + 5] = 1;
      fEventSets [kEMu   + 6] = 1;
      fEventSets [kEMu   + 6+fQcdOffset] = 1;
      fTreeSets  [kEMu + 6] = 1;

      // fEventSets [kEMu   + 6] = 1; // events with opposite signs and passing Mu+Tau Pt cuts with no photon check
      // fEventSets [kEMu   + 6+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt cuts with no photon check

      fEventSets [kEMu   + 7] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts with no photon check
      fEventSets [kEMu   + 7+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts with no photon check
      fTreeSets  [kEMu   + 7] = 1;

      // Sets 8-10 MVA cuts applied
      fEventSets [kEMu   + 8] = 1; // events with opposite signs
      fEventSets [kEMu   + 8+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kEMu   + 8] = 1;
      fEventSets [kEMu   + 9] = 1; // events with opposite signs 
      fEventSets [kEMu   + 9+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kEMu   + 9] = 1;
      fEventSets [kEMu   + 10] = 1; // events with opposite signs 
      fEventSets [kEMu   + 10+fQcdOffset] = 1; // events with same signs 
      fTreeSets  [kEMu   + 10] = 1;
    
      fEventSets [kEMu   + 11] = 1; // events with opposite signs and mass window
      fEventSets [kEMu   + 11+fQcdOffset] = 1; // events with same signs and mass window
      fEventSets [kEMu   + 12] = 1; // events with opposite signs and mass window
      fEventSets [kEMu   + 12+fQcdOffset] = 1; // events with same signs and mass window

      //background regions
      fEventSets [kEMu   + 13] = 1; // events with opposite signs and top set
      fEventSets [kEMu   + 13+fQcdOffset] = 1; // events with same signs and top set
      fEventSets [kEMu   + 14] = 1; // events with opposite signs and w+jets set
      fEventSets [kEMu   + 14+fQcdOffset] = 1; // events with same signs and w+jets set
      fEventSets [kEMu   + 15] = 1; // events with opposite signs and z+jets set
      fEventSets [kEMu   + 15+fQcdOffset] = 1; // events with same signs and z+jets set

      fEventSets [kEMu   + 16] = 1; // events with opposite signs and z box cuts
      fEventSets [kEMu   + 16+fQcdOffset] = 1; // events with same signs and z box cuts
      fTreeSets  [kEMu   + 16] = 1;
      fEventSets [kEMu   + 17] = 1; // events with opposite signs and higgs box cuts
      fEventSets [kEMu   + 17+fQcdOffset] = 1; // events with same signs and higgs box cuts

      fEventSets [kEMu   + 18] = 1; // events with opposite signs and nJets = 0
      fEventSets [kEMu   + 18+fQcdOffset] = 1; // events with same signs and nJets = 0
      fTreeSets  [kEMu   + 18] = 1;
      fEventSets [kEMu   + 19] = 1; // events with opposite signs and nJets = 1
      fEventSets [kEMu   + 19+fQcdOffset] = 1; // events with same signs and nJets = 1
      fTreeSets  [kEMu   + 19] = 1;
      fEventSets [kEMu   + 20] = 1; // events with opposite signs and nJets > 1
      fEventSets [kEMu   + 20+fQcdOffset] = 1; // events with same signs and nJets > 1
      fTreeSets  [kEMu   + 20] = 1;

      fEventSets [kEMu   + 21] = 1; // events with opposite signs and nPhotons = 0
      fEventSets [kEMu   + 21+fQcdOffset] = 1; // events with same signs and nPhotons = 0
      fEventSets [kEMu   + 22] = 1; // events with opposite signs and nPhotons > 0
      fEventSets [kEMu   + 22+fQcdOffset] = 1; // events with same signs and nPhotons > 0
      fEventSets [kEMu   + 23] = 1;  //mario's box cuts
      fEventSets [kEMu   + 23+fQcdOffset] = 1; 

      //Test sets
      fEventSets [kEMu   + 24] = 1;
      fEventSets [kEMu   + 24+fQcdOffset] = 1; 
      fEventSets [kEMu   + 25] = 1;
      fEventSets [kEMu   + 25+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 26] = 1;
      // fEventSets [kEMu   + 26+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 27] = 1;
      // fEventSets [kEMu   + 27+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 28] = 1;
      // fEventSets [kEMu   + 28+fQcdOffset] = 1; 
      // fEventSets [kEMu   + 29] = 1;
      // fEventSets [kEMu   + 29+fQcdOffset] = 1; 
    }
    else if(fFolderName == "mumu") {
      //Mu+Mu sets
      // if(!fDYTesting) {
	fEventSets [kMuMu + 1] = 1; // events with opposite signs
	fEventSets [kMuMu + 1+fQcdOffset] = 1; // events with same signs
	fEventSets [kMuMu + 2] = 1; // events with opposite signs
	fEventSets [kMuMu + 2+fQcdOffset] = 1; // events with same signs
      // }
      fEventSets [kMuMu + 7] = 1; // events with opposite signs
      fEventSets [kMuMu + 7+fQcdOffset] = 1; // events with same signs
      fEventSets [kMuMu + 8] = 1; // events with opposite signs and no bjets
      fEventSets [kMuMu + 8+fQcdOffset] = 1; // events with same signs
      if(!fDYTesting) {
	fEventSets [kMuMu + 9] = 1; // events with opposite signs and no jets, no taus, no photons (mostly pure Z->mu + mu)
	fEventSets [kMuMu + 9+fQcdOffset] = 1; // events with same signs
	fEventSets [kMuMu +10] = 1; // events with opposite signs and no jets, no taus, no photons (mostly pure Z->mu + mu)
	fEventSets [kMuMu +10+fQcdOffset] = 1; // events with same signs
	fEventSets [kMuMu  + 16] = 1; // events with opposite signs and z box cuts
	fEventSets [kMuMu  + 16+fQcdOffset] = 1; // events with same signs and z box cuts
	fTreeSets  [kMuMu  + 16] = 1;

	fEventSets [kMuMu + 18] = 1; // events with opposite signs + 0-jet
	fEventSets [kMuMu + 18+fQcdOffset] = 1; // events with same
	fEventSets [kMuMu + 19] = 1; // events with opposite signs + 1-jet
	fEventSets [kMuMu + 19+fQcdOffset] = 1; // events with same
	fEventSets [kMuMu + 20] = 1; // events with opposite signs + >1-jet
	fEventSets [kMuMu + 20+fQcdOffset] = 1; // events with same
      }
    }
    else if(fFolderName == "ee") {
      //Mu+Mu sets
      fEventSets [kEE + 1] = 1; // events with opposite signs
      fEventSets [kEE + 1+fQcdOffset] = 1; // events with same signs
      fEventSets [kEE + 2] = 1; // events with opposite signs
      fEventSets [kEE + 2+fQcdOffset] = 1; // events with same signs
      fEventSets [kEE + 7] = 1; // events with opposite signs
      fEventSets [kEE + 7+fQcdOffset] = 1; // events with same signs
      fEventSets [kEE + 8] = 1; // events with opposite signs and no bjets
      fEventSets [kEE + 8+fQcdOffset] = 1; // events with same signs
      fEventSets [kEE + 9] = 1; // events with opposite signs and no bjets
      fEventSets [kEE + 9+fQcdOffset] = 1; // events with same signs
      fEventSets [kEE + 10] = 1; // events with opposite signs and no bjets
      fEventSets [kEE + 10+fQcdOffset] = 1; // events with same signs
      fEventSets [kEE + 16] = 1; // events with opposite signs and z box cuts
      fEventSets [kEE + 16+fQcdOffset] = 1; // events with same signs and z box cuts
      fTreeSets  [kEE + 16] = 1;
      fEventSets [kEE + 17] = 1; // events with opposite signs and z box cuts
      fEventSets [kEE + 17+fQcdOffset] = 1; // events with same signs and z box cuts
      fTreeSets  [kEE + 17] = 1;
      fEventSets [kEE + 18] = 1; // events with opposite signs and z box cuts
      fEventSets [kEE + 18+fQcdOffset] = 1; // events with same signs and z box cuts
      fTreeSets  [kEE + 18] = 1;

    }
    if(fFolderName == "emu") {
      //Leptonic tau channels
      //mu+tau_e
      fEventSets [kMuTauE+ 9] = 1; // events with opposite signs + BDT cut
      fEventSets [kMuTauE+ 9+fQcdOffset] = 1; // events with same
      fEventSets [kMuTauE+ 10] = 1; // events with opposite signs + BDT cut
      fEventSets [kMuTauE+ 10+fQcdOffset] = 1; // events with same
      //e+tau_mu
      fEventSets [kETauMu+ 9] = 1; // events with opposite signs + BDT cut
      fEventSets [kETauMu+ 9+fQcdOffset] = 1; // events with same
      fEventSets [kETauMu+ 10] = 1; // events with opposite signs + BDT cut
      fEventSets [kETauMu+ 10+fQcdOffset] = 1; // events with same
    }

    //initialize all the histograms
    BookHistograms();

  }

  if(tree != 0) fChain = tree;
  else return;
  printf("ZTauTauHistMaker::Init fChain = tree ");
  std::cout << tree << std::endl;
  //turn off slim vectors, as they're not that slim
  fChain->SetBranchStatus("slimElectrons" , 0);
  fChain->SetBranchStatus("slimMuons"     , 0);
  fChain->SetBranchStatus("slimTaus"      , 0);
  fChain->SetBranchStatus("slimJets"      , 0);
  fChain->SetBranchStatus("slimPhotons"   , 0);

  if(!fDYTesting) {
    fChain->SetBranchAddress("runNumber"           , &runNumber            );
    fChain->SetBranchAddress("evtNumber"           , &eventNumber          );
    fChain->SetBranchAddress("lumiSection"         , &lumiSection          );
    fChain->SetBranchAddress("nPV"                 , &nPV                  );
    fChain->SetBranchAddress("nPU"                 , &nPU                  );
    fChain->SetBranchAddress("nPartons"            , &nPartons             );
    fChain->SetBranchAddress("mcEra"               , &mcEra                );
  }
  fChain->SetBranchAddress("triggerLeptonStatus" , &triggerLeptonStatus  );
  fChain->SetBranchAddress("eventWeight"         , &eventWeight          );
  fChain->SetBranchAddress("genWeight"           , &genWeight            );
  fChain->SetBranchAddress("puWeight"            , &puWeight             );
  fChain->SetBranchAddress("topPtWeight"         , &topPtWeight          );
  fChain->SetBranchAddress("btagWeight"          , &btagWeight           );
  fChain->SetBranchAddress("zPtWeight"           , &zPtWeight            );
  fChain->SetBranchAddress("zPt"                 , &zPt                  );
  fChain->SetBranchAddress("zMass"               , &zMass                );
  fChain->SetBranchAddress("looseQCDSelection"   , &looseQCDSelection    );
  if(!fDYTesting) {
    fChain->SetBranchAddress("genTauFlavorWeight"  , &genTauFlavorWeight   );
    fChain->SetBranchAddress("tauDecayMode"        , &tauDecayMode         );
    fChain->SetBranchAddress("tauMVA"              , &tauMVA               );
    fChain->SetBranchAddress("tauGenFlavor"        , &tauGenFlavor         );
    fChain->SetBranchAddress("tauGenFlavorHad"     , &tauGenFlavorHad      );
    fChain->SetBranchAddress("tauVetoedJetPt"      , &tauVetoedJetPt       );
    fChain->SetBranchAddress("tauVetoedJetPtUnc"   , &tauVetoedJetPtUnc    );
  }
  fChain->SetBranchAddress("leptonOneP4" 	 , &leptonOneP4          );
  fChain->SetBranchAddress("leptonTwoP4" 	 , &leptonTwoP4          );
  fChain->SetBranchAddress("leptonOneFlavor"     , &leptonOneFlavor      );
  fChain->SetBranchAddress("leptonTwoFlavor"     , &leptonTwoFlavor      );
  if(!fDYTesting) {
    fChain->SetBranchAddress("leptonOneD0"         , &leptonOneD0          );
    fChain->SetBranchAddress("leptonTwoD0"         , &leptonTwoD0          );
    fChain->SetBranchAddress("leptonOneIso"        , &leptonOneIso         );
    fChain->SetBranchAddress("leptonTwoIso"        , &leptonTwoIso         );
    fChain->SetBranchAddress("leptonOneID1"        , &leptonOneID1         );
    fChain->SetBranchAddress("leptonOneID2"        , &leptonOneID2         );
    fChain->SetBranchAddress("leptonTwoID1"        , &leptonTwoID1         );
    fChain->SetBranchAddress("leptonTwoID2"        , &leptonTwoID2         );
  }
  fChain->SetBranchAddress("leptonOneIndex"      , &leptonOneIndex       );
  fChain->SetBranchAddress("leptonTwoIndex"      , &leptonTwoIndex       );
  fChain->SetBranchAddress("lepOneWeight"        , &leptonOneWeight      );
  fChain->SetBranchAddress("lepTwoWeight"        , &leptonTwoWeight      );
  fChain->SetBranchAddress("lepOneTrigWeight"    , &leptonOneTrigWeight  );
  fChain->SetBranchAddress("lepTwoTrigWeight"    , &leptonTwoTrigWeight  );
  fChain->SetBranchAddress("lepOneFired"         , &leptonOneFired       );
  fChain->SetBranchAddress("lepTwoFired"         , &leptonTwoFired       );
  if(!fDYTesting) {
    fChain->SetBranchAddress("genLeptonOneP4" 	 , &genLeptonOneP4       );
    fChain->SetBranchAddress("genLeptonTwoP4" 	 , &genLeptonTwoP4       );
  }
  fChain->SetBranchAddress("photonP4"  	         , &photonP4             );
  if(!fDYTesting) {
    fChain->SetBranchAddress("photonMVA"  	 , &photonMVA            );
    fChain->SetBranchAddress("photonIDWeight"      , &photonIDWeight       );
  }
  fChain->SetBranchAddress("jetP4"  	         , &jetOneP4             );
  if(!fDYTesting) {
    fChain->SetBranchAddress("jetBTag"             , &jetOneBTag           ); 
    fChain->SetBranchAddress("jetBMVA"             , &jetOneBMVA           ); 
    if(fIsNano) {
      fChain->SetBranchAddress("nJetsNano"  	 , &nSlimJets            );
      // fChain->SetBranchAddress("slimJets"   	 , &slimJets             );
    }
    if(fFolderName == "llg_study") {
      fChain->SetBranchAddress("jetTwoP4"  	 , &jetTwoP4             );
      fChain->SetBranchAddress("jetTwoBTag"  	 , &jetTwoBTag           );
      fChain->SetBranchAddress("jetTwoBMVA"  	 , &jetTwoBMVA           );
    }
    fChain->SetBranchAddress("tauP4"  	         , &tauP4                );
    fChain->SetBranchAddress("tauFlavor"  	 , &tauFlavor            );
  }
  fChain->SetBranchAddress("nMuons"              , &nMuons               );
  if(!fDYTesting) {
    if(fIsNano) {
      fChain->SetBranchAddress("tauDeepAntiEle"    , &tauDeepAntiEle       );
      fChain->SetBranchAddress("tauDeepAntiMu"     , &tauDeepAntiMu        );
      fChain->SetBranchAddress("tauDeepAntiJet"    , &tauDeepAntiJet       );
      fChain->SetBranchAddress("nMuonsNano"        , &nSlimMuons           );
      // fChain->SetBranchAddress("slimMuons"         , &slimMuons            );
    }
  }
  fChain->SetBranchAddress("nElectrons"          , &nElectrons           );
  if(!fDYTesting) {
    if(fIsNano) {
      fChain->SetBranchAddress("nElectronsNano"    , &nSlimElectrons       );
      // fChain->SetBranchAddress("slimElectrons"     , &slimElectrons        );
    }
    // fChain->SetBranchAddress("nLowPtElectrons"     , &nLowPtElectrons      );
  }
  fChain->SetBranchAddress("nTaus"               , &nTaus                );
  if(!fDYTesting) {
    if(fIsNano) {
      fChain->SetBranchAddress("nTausNano"         , &nSlimTaus            );
      // fChain->SetBranchAddress("slimTaus"          , &slimTaus             );
    }
  }
  fChain->SetBranchAddress("nPhotons"            , &nPhotons             );
  if(!fDYTesting) {
    if(fIsNano) {
      fChain->SetBranchAddress("nPhotonsNano"  	 , &nSlimPhotons         );
      // fChain->SetBranchAddress("slimPhotons"  	 , &slimPhotons          );
    }
  }
  fChain->SetBranchAddress("nJets"               , &nJets                );
  fChain->SetBranchAddress("nJets20"             , &nJets20              );
  fChain->SetBranchAddress("nFwdJets"            , &nFwdJets             );
  fChain->SetBranchAddress("nBJets"              , &nBJets               );
  fChain->SetBranchAddress("nBJetsM"             , &nBJetsM              );
  fChain->SetBranchAddress("nBJetsL"             , &nBJetsL              );
  fChain->SetBranchAddress("nBJets20"            , &nBJets20             );
  fChain->SetBranchAddress("nBJets20M"           , &nBJets20M            );
  fChain->SetBranchAddress("nBJets20L"           , &nBJets20L            );
  fChain->SetBranchAddress("nGenTausHad"         , &nGenTausHad          );
  fChain->SetBranchAddress("nGenTausLep"         , &nGenTausLep          );
  fChain->SetBranchAddress("nGenElectrons"       , &nGenElectrons        );
  fChain->SetBranchAddress("nGenMuons"           , &nGenMuons            );
  fChain->SetBranchAddress("nGenPromptTaus"      , &nGenHardTaus         );
  fChain->SetBranchAddress("nGenPromptElectrons" , &nGenHardElectrons    );
  fChain->SetBranchAddress("nGenPromptMuons"     , &nGenHardMuons        );
  fChain->SetBranchAddress("jetsPt"              , &jetsPt               );
  fChain->SetBranchAddress("jetsEta"             , &jetsEta              );
  fChain->SetBranchAddress("jetsFlavor"          , &jetsFlavor           );
  fChain->SetBranchAddress("jetsBTag"            , &jetsBTag             );
  if(!fDYTesting) {
    fChain->SetBranchAddress("htSum"               , &htSum                );
    fChain->SetBranchAddress("ht"                  , &ht                   );
    fChain->SetBranchAddress("htPhi"               , &htPhi                );
    // fChain->SetBranchAddress("pfMET"               , &pfMET                );
    // fChain->SetBranchAddress("pfMETphi"            , &pfMETphi             );
    // fChain->SetBranchAddress("pfMETCov00"          , &pfMETCov00           );
    // fChain->SetBranchAddress("pfMETCov01"          , &pfMETCov01           );
    // fChain->SetBranchAddress("pfMETCov11"          , &pfMETCov11           );
    fChain->SetBranchAddress("pfMETC"              , &pfMETC               );
    fChain->SetBranchAddress("pfMETCphi"           , &pfMETCphi            );
    fChain->SetBranchAddress("pfMETCCov00"         , &pfMETCCov00          );
    fChain->SetBranchAddress("pfMETCCov01"         , &pfMETCCov01          );
    fChain->SetBranchAddress("pfMETCCov11"         , &pfMETCCov11          );
  }
  fChain->SetBranchAddress("puppMET"             , &puppMET              );
  fChain->SetBranchAddress("puppMETphi"          , &puppMETphi           );
  // fChain->SetBranchAddress("puppMETCov00"        , &puppMETCov00         );
  // fChain->SetBranchAddress("puppMETCov01"        , &puppMETCov01         );
  // fChain->SetBranchAddress("puppMETCov11"        , &puppMETCov11         );
  fChain->SetBranchAddress("puppMETC"            , &puppMETC             );
  fChain->SetBranchAddress("puppMETCphi"         , &puppMETCphi          );
  // fChain->SetBranchAddress("puppMETCCov00"       , &puppMETCCov00        );
  // fChain->SetBranchAddress("puppMETCCov01"       , &puppMETCCov01        );
  // fChain->SetBranchAddress("puppMETCCov11"       , &puppMETCCov11        );
  // fChain->SetBranchAddress("alpacaMET"           , &alpacaMET            );
  // fChain->SetBranchAddress("alpacaMETphi"        , &alpacaMETphi         );
  // fChain->SetBranchAddress("pcpMET"              , &pcpMET               );
  // fChain->SetBranchAddress("pcpMETphi"           , &pcpMETphi            );
  // fChain->SetBranchAddress("trkMET"              , &trkMET               );
  // fChain->SetBranchAddress("trkMETphi"           , &trkMETphi            );
  fChain->SetBranchAddress("met"                 , &met                  );
  fChain->SetBranchAddress("metPhi"              , &metPhi               );
  fChain->SetBranchAddress("metCorr"             , &metCorr              );
  fChain->SetBranchAddress("metCorrPhi"          , &metCorrPhi           );
  if(!fDYTesting) {
    fChain->SetBranchAddress("covMet00"            , &covMet00             );
    fChain->SetBranchAddress("covMet01"            , &covMet01             );
    fChain->SetBranchAddress("covMet11"            , &covMet11             );
    fChain->SetBranchAddress("massSVFit"           , &massSVFit            );
    fChain->SetBranchAddress("massErrSVFit"        , &massErrSVFit         );
    fChain->SetBranchAddress("svFitStatus"         , &svFitStatus          );
    fChain->SetBranchAddress("leptonOneSVP4" 	 , &leptonOneSVP4        );
    fChain->SetBranchAddress("leptonTwoSVP4" 	 , &leptonTwoSVP4        );
    if(!fReprocessMVAs) {
      for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i) {
	fChain->SetBranchAddress(Form("mva%i",mva_i), &fMvaOutputs[mva_i]);
      }
    }
  }
  printf("Total number of entries is %lld\n",fChain->GetEntriesFast());
}


Bool_t ZTauTauHistMaker::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef ZTauTauHistMaker_cxx
