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


class ZTauTauHistMaker : public TSelector {
public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
  enum {kMaxParticles = 20, kMaxCounts = 30};
  enum {kMuTau = 0, kETau = 30, kEMu = 60, kMuTauE = 90, kETauMu = 105, kMuMu = 120};
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
  Float_t zPtWeight                  ;
  Float_t zPt                        ;
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
  UInt_t nJets                       ;
  UInt_t nJets25                     ;
  UInt_t nJets20                     ;
  UInt_t nJets25Tot                  ;
  UInt_t nJetsTot                    ;
  UInt_t nFwdJets                    ;
  UInt_t nBJetsUse                   ; //which to count
  UInt_t nBJets                      ;
  UInt_t nBJetsM                     ;
  UInt_t nBJetsL                     ;
  UInt_t nBJets25                    ;
  UInt_t nBJets25M                   ;
  UInt_t nBJets25L                   ;
  UInt_t nBJets20                    ;
  UInt_t nBJets20M                   ;
  UInt_t nBJets20L                   ;
  UInt_t nBJets25Tot                 ;
  UInt_t nBJets25TotM                ;
  UInt_t nBJets25TotL                ;
  UInt_t nBJetsTot                   ;
  UInt_t nBJetsTotM                  ;
  UInt_t nBJetsTotL                  ;
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
    TH1F* hLumiSection;
    TH1F* hTriggerStatus;
    TH1F* hEventWeight;
    TH1F* hGenWeight;
    TH1F* hGenTauFlavorWeight;
    TH1F* hPhotonIDWeight;
    TH1F* hNPV;
    TH1F* hNPU;
    TH1F* hNPartons;
    TH1F* hNMuons;
    TH1F* hNSlimMuons;
    TH1F* hNMuonCounts[kMaxCounts];
    TH1F* hNElectrons;
    TH1F* hNSlimElectrons;
    TH1F* hNElectronCounts[kMaxCounts];
    TH1F* hNLowPtElectrons;
    TH1F* hNTaus;
    TH1F* hNSlimTaus;
    TH1F* hNTauCounts[kMaxCounts];
    TH1F* hNPhotons;
    TH1F* hNGenTausHad;
    TH1F* hNGenTausLep;
    TH1F* hNGenTaus;
    TH1F* hNGenElectrons;
    TH1F* hNGenMuons;
    TH1F* hNGenHardTaus;
    TH1F* hNGenHardElectrons;
    TH1F* hNGenHardMuons;
    TH1F* hNJets;
    TH1F* hNJets25;
    TH1F* hNJets20;
    TH1F* hNJets25Tot;
    TH1F* hNJetsTot;
    TH1F* hNFwdJets;
    TH1F* hNBJets;
    TH1F* hNBJetsM;
    TH1F* hNBJetsL;
    TH1F* hNBJets25;
    TH1F* hNBJets25M;
    TH1F* hNBJets25L;
    TH1F* hNBJets20;
    TH1F* hNBJets20M;
    TH1F* hNBJets20L;
    TH1F* hNBJets25Tot;
    TH1F* hNBJets25TotM;
    TH1F* hNBJets25TotL;
    TH1F* hNBJetsTot;
    TH1F* hNBJetsTotM;
    TH1F* hNBJetsTotL;
    TH1F* hMcEra;
    TH1F* hTriggerLeptonStatus;
    TH1F* hPuWeight;
    TH1F* hTopPtWeight;
    TH1F* hZPtWeight;
    TH1F* hTauDecayMode;
    TH1F* hTauMVA;
    TH1F* hTauGenFlavor;
    TH1F* hTauGenFlavorHad;
    TH1F* hTauVetoedJetPt;
    TH1F* hTauVetoedJetPtUnc;
    TH1F* hHtSum;
    TH1F* hHt;
    TH1F* hHtPhi;
    TH1F* hJetPt;
    TH1F* hJetM;
    TH1F* hJetEta;
    TH1F* hJetPhi;    
    TH1F* hJetBTag;    
    TH1F* hJetBMVA;    
    TH1F* hTauPt;
    TH1F* hTauM;
    TH1F* hTauEta;
    TH1F* hTauPhi;    
    TH1F* hPuppMet;
    TH1F* hPFMet;
    TH1F* hPFMetPhi;
    TH1F* hPFCovMet00;
    TH1F* hPFCovMet01;
    TH1F* hPFCovMet11;
    TH1F* hTrkMet;
    TH1F* hTrkMetPhi;

    TH1F* hMet;
    TH1F* hMetPhi;
    TH1F* hMetCorr;
    TH1F* hMetCorrPhi;
    TH1F* hCovMet00;
    TH1F* hCovMet01;
    TH1F* hCovMet11;
    TH2F* hMetVsPt;
    TH2F* hMetVsM;
    TH2F* hMetVsHtSum; //MET ~ sqrt(htsum)
    TH1F* hMetOverSqrtHtSum; //MET ~ sqrt(htsum)
    
    TH1F* hMassSVFit;
    TH1F* hMassErrSVFit;
    TH1F* hSVFitStatus;

    //di-lepton histograms
    TH1F* hLepPt;
    TH1F* hLepP;
    TH1F* hLepM;
    TH1F* hLepEta;
    TH1F* hLepPhi;
    
    TH1F* hLepDeltaPhi;
    TH1F* hLepDeltaEta;
    TH1F* hLepDeltaR;
    TH2F* hLepDelRVsPhi;
    
    TH1F* hLepPtOverM;
    TH1F* hHtDeltaPhi;
    TH1F* hMetDeltaPhi;
    TH1F* hJetDeltaPhi;
    TH1F* hLepOneDeltaPhi;
    TH1F* hLepTwoDeltaPhi;

    //angles between leptons and jets
    TH1F* hLepOneJetDeltaR;
    TH1F* hLepOneJetDeltaPhi;
    TH1F* hLepOneJetDeltaEta;
    TH1F* hLepTwoJetDeltaR;
    TH1F* hLepTwoJetDeltaPhi;
    TH1F* hLepTwoJetDeltaEta;
    
    TH1F* hLepSVPt;
    TH1F* hLepSVM;
    TH1F* hLepSVEta;
    TH1F* hLepSVPhi;
    
    TH1F* hLepSVDeltaPhi;
    TH1F* hLepSVDeltaEta;
    TH1F* hLepSVDeltaM;
    TH1F* hLepSVDeltaPt;
    TH1F* hLepSVPtOverM;
    
    TH1F* hSysM;
    TH1F* hSysPt;
    TH1F* hSysEta;
    TH2F* hSysMvsLepM;
    
    //Transverse Masses
    TH1F* hMTOne;
    TH1F* hMTTwo;

    //three sets for combining photon with a lepton or leptons vs photon
    TH1F* hPXiVis[3];
    TH1F* hPXiInv[3];
    TH1F* hPXiVisOverInv[3];
    TH2F* hPXiInvVsVis[3];
    TH1F* hPXiDiff[3];
    TH1F* hPXiDiff2[3];//difference with coeffecients and offset
    TH1F* hPXiDiff3[3];
    
    //For assuming MET along tau is tau neutrino, only makes sense for e/mu + tau
    TH1F* hPTauVisFrac;
    TH1F* hLepMEstimate;
    TH1F* hLepMEstimateTwo;
    
    TH1F* hPtSum[2]; //scalar sum of lepton Pt and Met, and photon for one
    TH1F* hPt1Sum[4]; //scalar sum of 1 lepton Pt and Met, both leptons, then both minus met
    //MVA values
    TH1F* hMVA[kMaxMVAs];
    TH1F* hMVATrain[kMaxMVAs];
    TH1F* hMVATest[kMaxMVAs];

    //llg study histograms
    TH1F* hObjMasses[14]; //jets, jets+gamma, jet1/2 + gamma, jets + l1/2, jet1/2 + l1/2, jets+l1+l2, jets + gamma + l1/2, jets + gamma + l1 + l2
    TH1F* hJetTwoM;
    TH1F* hJetTwoPt;
    TH1F* hJetTwoEta;
    TH1F* hJetTwoBTag;    
    TH1F* hJetTwoBMVA;    
    TH1F* hJetsDeltaR;
    TH1F* hJetsDeltaEta;
    TH1F* hJetsDeltaPhi;
    TH1F* hJetsPt;
    TH1F* hJetsEta;
    TH1F* hJetsGammaDeltaR;
    TH1F* hJetsGammaDeltaEta;
    TH1F* hJetsGammaDeltaPhi;
    TH1F* hJetsGammaPt;
    TH1F* hJetsGammaEta;
  };

  struct LepHist_t {
    TH1F* hOnePz;
    TH1F* hOnePt;
    TH1F* hOneP;
    TH1F* hOneM;
    TH1F* hOneEta;
    TH1F* hOnePhi;
    TH1F* hOneD0;
    TH1F* hOneIso;
    TH1F* hOneID1;
    TH1F* hOneID2;
    TH1F* hOneRelIso;
    TH1F* hOneFlavor;
    TH1F* hOneQ;
    TH1F* hOneTrigger;
    //Gen Info
    TH1F* hOneGenPt;
    TH1F* hOneGenE;
    TH1F* hOneGenEta;
    TH1F* hOneDeltaPt;
    TH1F* hOneDeltaE;
    TH1F* hOneDeltaEta;
    TH1F* hOneMetDeltaPhi;
    //SVFit Info
    TH1F* hOneSVPt;
    TH1F* hOneSVM;    
    TH1F* hOneSVEta;
    TH1F* hOneSVDeltaPt;
    TH1F* hOneSVDeltaP;
    TH1F* hOneSVDeltaE;
    TH1F* hOneSVDeltaEta;
    TH1F* hOneSlimEQ;
    TH1F* hOneSlimMuQ;
    TH1F* hOneSlimTauQ;
    TH1F* hOneSlimEM;
    TH1F* hOneSlimEMSS;
    TH1F* hOneSlimEMOS;
    TH1F* hOneSlimMuM;
    TH1F* hOneSlimMuMSS;
    TH1F* hOneSlimMuMOS;
    TH1F* hOneSlimTauM;    
    TH1F* hOneSlimTauMSS;    
    TH1F* hOneSlimTauMOS;    
    
    TH1F* hTwoPz;
    TH1F* hTwoPt;
    TH1F* hTwoP;
    TH1F* hTwoM;
    TH1F* hTwoEta;
    TH1F* hTwoPhi;
    TH1F* hTwoD0;
    TH1F* hTwoIso;
    TH1F* hTwoID1;
    TH1F* hTwoID2;
    TH1F* hTwoRelIso;
    TH1F* hTwoFlavor;
    TH1F* hTwoQ;
    TH1F* hTwoTrigger;
    //Gen Info
    TH1F* hTwoGenPt;
    TH1F* hTwoGenE;
    TH1F* hTwoGenEta;
    TH1F* hTwoDeltaPt;
    TH1F* hTwoDeltaE;
    TH1F* hTwoDeltaEta;
    TH1F* hTwoMetDeltaPhi;

    //SVFit Info
    TH1F* hTwoSVPt;
    TH1F* hTwoSVM;    
    TH1F* hTwoSVEta;
    TH1F* hTwoSVDeltaPt;
    TH1F* hTwoSVDeltaP;
    TH1F* hTwoSVDeltaE;
    TH1F* hTwoSVDeltaEta;

    TH1F* hTwoSlimEQ;
    TH1F* hTwoSlimMuQ;
    TH1F* hTwoSlimTauQ;
    TH1F* hTwoSlimEM;
    TH1F* hTwoSlimEMSS;
    TH1F* hTwoSlimEMOS;
    TH1F* hTwoSlimMuM;
    TH1F* hTwoSlimMuMSS;
    TH1F* hTwoSlimMuMOS;
    TH1F* hTwoSlimTauM;    
    TH1F* hTwoSlimTauMSS;    
    TH1F* hTwoSlimTauMOS;    
    
    
    TH1F* hD0Diff;
    
    //2D distribution
    TH2F* hTwoPtVsOnePt;
  };

  struct PhotonHist_t {
    TH1F* hPz;
    TH1F* hPt;
    TH1F* hP;
    TH1F* hEta;
    TH1F* hPhi;
    TH1F* hMVA;
    // TH1F* hIso;
    // TH1F* hRelIso;
    // TH1F* hTrigger;
  };

  //Tree Variables
  struct Tree_t {
    //lepton variables
    float leponept;
    float leponem;
    float leponeeta;
    float leponed0;
    float leponeiso;
    float leptwopt;
    float leptwom;
    float leptwoeta;
    float leptwod0;
    float leptwoiso;
    //di-lepton variables
    float lepp;
    float leppt;
    float lepm;
    float lepeta;
    float lepdeltaeta;
    float lepdeltar;
    float lepdeltaphi;

    //extra angles
    float htdeltaphi;    
    float metdeltaphi;
    float leponedeltaphi;
    float leptwodeltaphi;
    float onemetdeltaphi;
    float twometdeltaphi;

    //MET variables
    float met;
    float mtone;
    float mttwo;
    float pxivis;
    float pxiinv;
    float ptauvisfrac;
    float mestimate;
    float mestimatetwo;
    
    //Event variables
    float ht;
    float htsum;
    float njets;
    float nbjets; //pt > 30
    float nbjetsm;
    float nbjetsl;
    float nbjetstot25; //pt > 25
    float nbjetstot25m;
    float nbjetstot25l;
    float nbjetstot20; //pt > 20
    float nbjetstot20m;
    float nbjetstot20l;
    float nphotons;
    float eventweight;
    float fulleventweight; //includes cross-section and number gen
    float eventcategory; //for identifying the process in mva trainings

    //identify to use in training
    float train; //  < 0 --> testing, > 0 --> training sample
  };

  //define object to apply box cuts 
  class TEventID {
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
      kHt             = 0x1 <<11
    };
  public:
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
  virtual float   GetZPtWeight(float pt);
  virtual void    CountSlimObjects();

  virtual void    ProcessLLGStudy();


  Long64_t fentry; //for tracking entry in functions
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();
  TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
  vector<TString> fMvaNames = { //mva names for getting weights
    "mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
    "etau_BDT_28.higgs","etau_BDT_28.Z0",
    "emu_BDT_47.higgs","emu_BDT_47.Z0",
    "mutau_e_BDT_48.higgs","mutau_e_BDT_48.Z0",
    "etau_mu_BDT_48.higgs","etau_mu_BDT_48.Z0", 
    "mutau_BDT_18.higgs","mutau_BDT_18.Z0", //10 - 19: 0 jets
    "etau_BDT_38.higgs","etau_BDT_38.Z0",
    "emu_BDT_58.higgs","emu_BDT_58.Z0",
    "mutau_e_BDT_58.higgs","mutau_e_BDT_58.Z0",
    "etau_mu_BDT_58.higgs","etau_mu_BDT_58.Z0", 
    "mutau_BDT_19.higgs","mutau_BDT_19.Z0", // 20 - 29: 1 jet
    "etau_BDT_39.higgs","etau_BDT_39.Z0",
    "emu_BDT_59.higgs","emu_BDT_59.Z0",
    "mutau_e_BDT_59.higgs","mutau_e_BDT_59.Z0",
    "etau_mu_BDT_59.higgs","etau_mu_BDT_59.Z0", 
    "mutau_BDT_20.higgs","mutau_BDT_20.Z0",  // 30 - 39: >1 jet
    "etau_BDT_40.higgs","etau_BDT_40.Z0",
    "emu_BDT_60.higgs","emu_BDT_60.Z0",
    "mutau_e_BDT_60.higgs","mutau_e_BDT_60.Z0",
    "etau_mu_BDT_60.higgs","etau_mu_BDT_60.Z0"
  };
  vector<double> fMvaCuts = { //mva score cut values
    0.18, 0.08,  //scores optimize the gained 95% CL
    0.159, 0.053,//all are done by eye on limit gain vs MVA score plot
    0.2, 0.31,
    0.15, 0.078, 
    0.13, 0.066,
    0.13, 0.08, //0jet
    0.15, 0.05,
    0.1, 0.1,
    0.1, 0.035, //mutau_e
    0.12, 0.05, //etau_mu
    0.18, 0.05, //1jet
    0.13, 0.03,
    0.1, 0.1,
    0.09, 0.03, //mutau_e
    0.08, 0.04, //etau_mu
    0.13, 0.04, //>1jet
    0.1325, 0.03,
    0.17, 0.11,
    0.08, 0.035, //mutau_e
    0.08, 0.03  //etau_mu
  };
  Int_t  fIsJetBinnedMVAs[kMaxMVAs]; //storing number of jets for MVA, < 0 if not binned
  float fMvaOutputs[kMaxMVAs];
  
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

  Int_t         fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: e/mu e/mu
  Int_t         fWriteTrees = 0; //write out ttrees for the events
  Double_t      fXsec = 0.; //cross-section for full event weight with trees
  Tree_t        fTreeVars; //for filling the ttrees/mva evaluation
  Int_t         fEventCategory; //for identifying the process in mva trainings

  Int_t         fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
  Int_t         fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
  bool          fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
  Int_t         fMETWeights = 0; //re-weight events based on the MET
  Int_t         fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally
  float         fFractionMVA = 0.; //fraction of events used to train. Ignore these events in histogram filling, reweight the rest to compensate
  TRandom*      fRnd = 0; //for splitting MVA testing/training
  Int_t         fRndSeed = 90; //random number generator seed
  bool          fReprocessMVAs = false; //whether or not to use the tree given MVA values
  Int_t         fBJetCounting = 1; // 0: pT > 30 1: pT > 25 2: pT > 20
  Int_t         fBJetTightness = 1; // 0: tight 1: medium 2: loose
  Int_t         fMETType = 1; // 0: PF corrected 1: PUPPI Corrected
  bool          fForceBJetSense = true; //force can't be more strict id bjets than looser id bjets
  bool          fIsNano = false; //whether the tree is nano AOD based or not
  
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

    tree->SetBranchStatus("*", 1); //ensure all branches are active

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
	   fMvaNames[mva_i].Contains("_38") ||
	   fMvaNames[mva_i].Contains("_58"))
	  isJetBinned = 0;
	else if(fMvaNames[mva_i].Contains("_19") || //1 jet
		fMvaNames[mva_i].Contains("_39") ||
		fMvaNames[mva_i].Contains("_59"))
	  isJetBinned = 1;
	else if(fMvaNames[mva_i].Contains("_20") || //>1 jet
		fMvaNames[mva_i].Contains("_40") ||
		fMvaNames[mva_i].Contains("_60"))
	  isJetBinned = 2;

	fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling
      
	printf("Using selection %s\n", selection.Data());
	//Order must match the mva training!
	mva[mva_i]->AddVariable("lepm"            ,&fTreeVars.lepm           ); 
	mva[mva_i]->AddVariable("mtone"           ,&fTreeVars.mtone          );
	mva[mva_i]->AddVariable("mttwo"           ,&fTreeVars.mttwo          );
	mva[mva_i]->AddVariable("leponept"        ,&fTreeVars.leponept       );
	mva[mva_i]->AddVariable("leptwopt"        ,&fTreeVars.leptwopt       );
	mva[mva_i]->AddVariable("leppt"           ,&fTreeVars.leppt          );
	mva[mva_i]->AddSpectator("pxivis"         ,&fTreeVars.pxivis         );
	mva[mva_i]->AddSpectator("pxiinv"         ,&fTreeVars.pxiinv         );
	if(isJetBinned < 0)
	  mva[mva_i]->AddVariable("njets"         ,&fTreeVars.njets          );
	else
	  mva[mva_i]->AddSpectator("njets"        ,&fTreeVars.njets          );

	if(fMvaNames[mva_i].Contains("_47"))
	  mva[mva_i]->AddVariable("nbjetstot20m", &fTreeVars.nbjetstot20m);

	mva[mva_i]->AddSpectator("lepdeltaeta"     ,&fTreeVars.lepdeltaeta    );
	mva[mva_i]->AddSpectator("metdeltaphi"     ,&fTreeVars.metdeltaphi    );

	//tau specific
	if(selection.Contains("tau")) {
	  mva[mva_i]->AddVariable("lepmestimate"   ,&fTreeVars.mestimate     ); 
	  mva[mva_i]->AddVariable("onemetdeltaphi" ,&fTreeVars.onemetdeltaphi);
	  mva[mva_i]->AddVariable("twometdeltaphi" ,&fTreeVars.twometdeltaphi);
	} else {
	  mva[mva_i]->AddSpectator("lepmestimate"  ,&fTreeVars.mestimate     ); 
	  mva[mva_i]->AddSpectator("onemetdeltaphi",&fTreeVars.onemetdeltaphi);
	  mva[mva_i]->AddSpectator("twometdeltaphi" ,&fTreeVars.twometdeltaphi); 
	}
      
	//Spectators from mva training also required!
	mva[mva_i]->AddSpectator("leponedeltaphi"  ,&fTreeVars.leponedeltaphi );
	mva[mva_i]->AddSpectator("leptwodeltaphi"  ,&fTreeVars.leptwodeltaphi );
	mva[mva_i]->AddSpectator("leponed0"       ,&fTreeVars.leponed0       );
	mva[mva_i]->AddSpectator("leptwod0"       ,&fTreeVars.leptwod0       );

	if(isJetBinned != 0) {
	  mva[mva_i]->AddVariable("htdeltaphi"    ,&fTreeVars.htdeltaphi     );
	  mva[mva_i]->AddVariable("ht"            ,&fTreeVars.ht             ); 
	} else {
	  mva[mva_i]->AddSpectator("htdeltaphi"   ,&fTreeVars.htdeltaphi     );
	  mva[mva_i]->AddSpectator("ht"           ,&fTreeVars.ht             ); 
	}
	mva[mva_i]->AddVariable("lepdeltaphi"    ,&fTreeVars.lepdeltaphi    );
	mva[mva_i]->AddSpectator("htsum"          ,&fTreeVars.htsum          ); 
	mva[mva_i]->AddSpectator("leponeiso"      ,&fTreeVars.leponeiso      );
	mva[mva_i]->AddSpectator("met"            ,&fTreeVars.met            );
	mva[mva_i]->AddSpectator("lepdeltar"      ,&fTreeVars.lepdeltar      );
	mva[mva_i]->AddSpectator("fulleventweight",&fTreeVars.fulleventweight);
	mva[mva_i]->AddSpectator("eventweight"    ,&fTreeVars.eventweight    );
	mva[mva_i]->AddSpectator("eventcategory"  ,&fTreeVars.eventcategory  );

	//Initialize MVA weight file
	const char* f = Form("weights/%s.weights.xml",fMvaNames[mva_i].Data());
	mva[mva_i]->BookMVA(fMvaNames[mva_i].Data(),f);
	printf("Booked MVA %s with selection %s\n", fMvaNames[mva_i].Data(), selection.Data());
      }
    }
    slimMuons = new SlimMuons_t();
    slimElectrons = new SlimElectrons_t();
    slimTaus = new SlimTaus_t();
    
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
    //currently sets 5-24 are mutau, 25-44 are etau, and 45-64 are emu
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
      fEventSets [kMuTau + 6] = 1;
      fEventSets [kMuTau + 6+fQcdOffset] = 1;

      // fEventSets [kMuTau + 6] = 1; // events with opposite signs and passing Mu+Tau Pt cuts with no photon check
      // fEventSets [kMuTau + 6+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt cuts with no photon check

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
      fEventSets [kMuTau + 24] = 1;
      fEventSets [kMuTau + 24+fQcdOffset] = 1;
      fEventSets [kMuTau + 25] = 1;
      fEventSets [kMuTau + 25+fQcdOffset] = 1;
    }
    if(fFolderName == "etau") {
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
      fEventSets [kETau  + 6] = 1;
      fEventSets [kETau  + 6+fQcdOffset] = 1;

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
      fEventSets [kETau  + 10] = 1; // events with opposite signs 
      fEventSets [kETau  + 10+fQcdOffset] = 1; // events with same signs 
    
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
      fEventSets [kETau  + 24] = 1;
      fEventSets [kETau  + 24+fQcdOffset] = 1;
      fEventSets [kETau  + 25] = 1;
      fEventSets [kETau  + 25+fQcdOffset] = 1;
    }
    if(fFolderName == "emu") {
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
      fEventSets [kEMu   + 6] = 1;
      fEventSets [kEMu   + 6+fQcdOffset] = 1;

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
      fEventSets [kEMu   + 10] = 1; // events with opposite signs 
      fEventSets [kEMu   + 10+fQcdOffset] = 1; // events with same signs 
    
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
    }
    if(fFolderName == "mumu") {
      //Mu+Mu sets
      fEventSets [kMuMu + 7] = 1; // events with opposite signs
      fEventSets [kMuMu + 7+fQcdOffset] = 1; // events with same signs
      fEventSets [kMuMu + 8] = 1; // events with opposite signs and no bjets
      fEventSets [kMuMu + 8+fQcdOffset] = 1; // events with same signs
      fEventSets [kMuMu + 9] = 1; // events with opposite signs and no jets, no taus, no photons (mostly pure Z->mu + mu)
      fEventSets [kMuMu + 9+fQcdOffset] = 1; // events with same signs

      fEventSets [kMuMu + 18] = 1; // events with opposite signs + 0-jet
      fEventSets [kMuMu + 18+fQcdOffset] = 1; // events with same
      fEventSets [kMuMu + 19] = 1; // events with opposite signs + 1-jet
      fEventSets [kMuMu + 19+fQcdOffset] = 1; // events with same
      fEventSets [kMuMu + 20] = 1; // events with opposite signs + >1-jet
      fEventSets [kMuMu + 20+fQcdOffset] = 1; // events with same

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

  fChain->SetBranchAddress("runNumber"           , &runNumber            );
  fChain->SetBranchAddress("evtNumber"           , &eventNumber          );
  fChain->SetBranchAddress("lumiSection"         , &lumiSection          );
  fChain->SetBranchAddress("nPV"                 , &nPV                  );
  fChain->SetBranchAddress("nPU"                 , &nPU                  );
  fChain->SetBranchAddress("nPartons"            , &nPartons             );
  fChain->SetBranchAddress("mcEra"               , &mcEra                );
  fChain->SetBranchAddress("triggerLeptonStatus" , &triggerLeptonStatus  );
  fChain->SetBranchAddress("eventWeight"         , &eventWeight          );
  fChain->SetBranchAddress("genWeight"           , &genWeight            );
  fChain->SetBranchAddress("puWeight"            , &puWeight             );
  fChain->SetBranchAddress("topPtWeight"         , &topPtWeight          );
  fChain->SetBranchAddress("zPtWeight"           , &zPtWeight            );
  fChain->SetBranchAddress("zPt"                 , &zPt                  );
  fChain->SetBranchAddress("genTauFlavorWeight"  , &genTauFlavorWeight   );
  fChain->SetBranchAddress("tauDecayMode"        , &tauDecayMode         );
  fChain->SetBranchAddress("tauMVA"              , &tauMVA               );
  fChain->SetBranchAddress("tauGenFlavor"        , &tauGenFlavor         );
  fChain->SetBranchAddress("tauGenFlavorHad"     , &tauGenFlavorHad      );
  fChain->SetBranchAddress("tauVetoedJetPt"      , &tauVetoedJetPt       );
  fChain->SetBranchAddress("tauVetoedJetPtUnc"   , &tauVetoedJetPtUnc    );
  fChain->SetBranchAddress("leptonOneP4" 	 , &leptonOneP4          );
  fChain->SetBranchAddress("leptonTwoP4" 	 , &leptonTwoP4          );
  fChain->SetBranchAddress("leptonOneFlavor"     , &leptonOneFlavor      );
  fChain->SetBranchAddress("leptonTwoFlavor"     , &leptonTwoFlavor      );
  fChain->SetBranchAddress("leptonOneD0"         , &leptonOneD0          );
  fChain->SetBranchAddress("leptonTwoD0"         , &leptonTwoD0          );
  fChain->SetBranchAddress("leptonOneIso"        , &leptonOneIso         );
  fChain->SetBranchAddress("leptonTwoIso"        , &leptonTwoIso         );
  fChain->SetBranchAddress("leptonOneID1"        , &leptonOneID1         );
  fChain->SetBranchAddress("leptonOneID2"        , &leptonOneID2         );
  fChain->SetBranchAddress("leptonTwoID1"        , &leptonTwoID1         );
  fChain->SetBranchAddress("leptonTwoID2"        , &leptonTwoID2         );
  fChain->SetBranchAddress("genLeptonOneP4" 	 , &genLeptonOneP4       );
  fChain->SetBranchAddress("genLeptonTwoP4" 	 , &genLeptonTwoP4       );
  fChain->SetBranchAddress("photonP4"  	         , &photonP4             );
  fChain->SetBranchAddress("photonMVA"  	 , &photonMVA            );
  fChain->SetBranchAddress("photonIDWeight"      , &photonIDWeight       );
  fChain->SetBranchAddress("jetP4"  	         , &jetOneP4             );
  fChain->SetBranchAddress("jetBTag"             , &jetOneBTag           ); 
  fChain->SetBranchAddress("jetBMVA"             , &jetOneBMVA           ); 
  if(fFolderName == "llg_study") {
    fChain->SetBranchAddress("jetTwoP4"  	 , &jetTwoP4             );
    fChain->SetBranchAddress("jetTwoBTag"  	 , &jetTwoBTag           );
    fChain->SetBranchAddress("jetTwoBMVA"  	 , &jetTwoBMVA           );
  }
  fChain->SetBranchAddress("tauP4"  	         , &tauP4                );
  fChain->SetBranchAddress("tauFlavor"  	 , &tauFlavor            );
  fChain->SetBranchAddress("nMuons"              , &nMuons               );
  if(fIsNano) {
    fChain->SetBranchAddress("nMuonsNano"        , &nSlimMuons           );
    fChain->SetBranchAddress("slimMuons"         , &slimMuons            );
  }
  fChain->SetBranchAddress("nElectrons"          , &nElectrons           );
  if(fIsNano) {
    fChain->SetBranchAddress("nElectronsNano"    , &nSlimElectrons       );
    fChain->SetBranchAddress("slimElectrons"     , &slimElectrons        );
  }
  fChain->SetBranchAddress("nLowPtElectrons"     , &nLowPtElectrons      );
  fChain->SetBranchAddress("nTaus"               , &nTaus                );
  if(fIsNano) {
    fChain->SetBranchAddress("nTausNano"         , &nSlimTaus            );
    fChain->SetBranchAddress("slimTaus"          , &slimTaus             );
  }
  fChain->SetBranchAddress("nPhotons"            , &nPhotons             );
  fChain->SetBranchAddress("nJets"               , &nJets                );
  fChain->SetBranchAddress("nJets25"             , &nJets25              );
  fChain->SetBranchAddress("nJets20"             , &nJets20              );
  fChain->SetBranchAddress("nFwdJets"            , &nFwdJets             );
  fChain->SetBranchAddress("nBJets"              , &nBJets               );
  fChain->SetBranchAddress("nBJetsM"             , &nBJetsM              );
  fChain->SetBranchAddress("nBJetsL"             , &nBJetsL              );
  fChain->SetBranchAddress("nBJets25"            , &nBJets25             );
  fChain->SetBranchAddress("nBJets25M"           , &nBJets25M            );
  fChain->SetBranchAddress("nBJets25L"           , &nBJets25L            );
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
