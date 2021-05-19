#ifndef NanoAODConversion_hh
#define NanoAODConversion_hh

//locally defined objects
#include "interface/ParticleCorrections.hh"
#include "interface/SlimElectron_t.hh"
#include "interface/SlimMuon_t.hh"
#include "interface/SlimTau_t.hh"
#include "interface/SlimJet_t.hh"
#include "interface/SlimPhoton_t.hh"
#include "interface/PUWeight.hh"
#include "interface/RoccoR.h"

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
#include "TRandom3.h" //for applying Rochester corrections

// TMVA includes
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

#include <iostream>



class NanoAODConversion : public TSelector {
public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

  enum {kMaxParticles = 50, kMaxTriggers = 100, kTrigModes = 3};
  enum {kMuTau, kETau, kEMu, kMuMu, kEE, kSelections}; //selections
  TString fSelecNames[kSelections];
  // Output data format
  UInt_t runNumber = 0               ;
  ULong64_t eventNumber = 0          ;
  UInt_t lumiSection = 0             ;
  UInt_t nPV  = 0                    ;
  Float_t nPU = 0                    ;
  Int_t  nPUAdded = 0                ;
  UInt_t nPartons = 0                ;
  UInt_t mcEra = 0                   ;
  UInt_t triggerLeptonStatus = 0     ;
  UInt_t muonTriggerStatus = 0       ;
  Float_t eventWeight = 1.           ;
  Float_t genWeight = 1.             ;
  Float_t puWeight = 1.              ;
  Float_t lepOneWeight1              ;
  Float_t lepOneWeight1_up           ;
  Float_t lepOneWeight1_down         ;
  Int_t   lepOneWeight1_bin          ;
  Float_t lepOneWeight2              ;
  Float_t lepOneWeight2_up           ;
  Float_t lepOneWeight2_down         ;
  Int_t   lepOneWeight2_bin          ;
  Float_t lepTwoWeight1              ;
  Float_t lepTwoWeight1_up           ;
  Float_t lepTwoWeight1_down         ;
  Int_t   lepTwoWeight1_bin          ;
  Float_t lepTwoWeight2              ;
  Float_t lepTwoWeight2_up           ;
  Float_t lepTwoWeight2_down         ;
  Int_t   lepTwoWeight2_bin          ;
  Float_t lepOneTrigWeight = 1.      ;
  Float_t lepTwoTrigWeight = 1.      ;
  Bool_t  lepOneFired = false        ;
  Bool_t  lepTwoFired = false        ;
  Int_t   lepOneTrigger              ;
  Int_t   lepTwoTrigger              ;
  Int_t   nTrigModes = kTrigModes    ;
  Float_t triggerWeights[kTrigModes] ;
  Float_t topPtWeight = 1.           ;
  Float_t btagWeight = 1.            ;
  Float_t zPtWeight = 1.             ;
  Float_t zPtOut    = -1.            ;
  Float_t zMassOut    = -1.          ;
  Bool_t  looseQCDSelection = false  ;
  Float_t genTauFlavorWeight = 1.    ;
  Float_t tauEnergyScale = 1.        ;
  Float_t tauES_up                   ;
  Float_t tauES_down                 ;
  Int_t   tauES_bin                  ;
  Int_t   tauDecayModeOut = 0        ;
  Float_t tauMVA  = 0                ;
  Int_t tauGenIDOut      = 0         ;
  Int_t tauGenFlavor     = 0         ;
  Int_t tauGenFlavorHad  = 0         ;
  Float_t tauVetoedJetPt = 0         ;
  Float_t tauVetoedJetPtUnc = 0      ;
  TLorentzVector* leptonOneP4 = 0    ;
  TLorentzVector* leptonTwoP4 = 0    ;
  Int_t leptonOneFlavor = 0          ;
  Int_t leptonTwoFlavor = 0          ;
  Int_t leptonOneGenFlavor = 0       ;
  Int_t leptonTwoGenFlavor = 0       ;
  Int_t leptonOneSkimFlavor = 0      ;
  Int_t leptonTwoSkimFlavor = 0      ;
  Float_t leptonOneD0   = 0          ;
  Float_t leptonTwoD0   = 0          ;
  Float_t leptonOneIso  = 0          ;
  Float_t leptonTwoIso = 0           ;
  UChar_t leptonOneID1 = 0           ;
  UChar_t leptonTwoID1 = 0           ;
  UChar_t leptonOneID2 = 0           ;
  UChar_t leptonTwoID2 = 0           ;
  UChar_t leptonTwoID3 = 0           ;
  Int_t   leptonOneIndex = 0         ;
  Int_t   leptonTwoIndex = 0         ;
  Int_t   leptonOneSkimIndex = -1    ;
  Int_t   leptonTwoSkimIndex = -1    ;
  Float_t leptonOnePtSF = 1.         ;
  Float_t leptonTwoPtSF = 1.         ;
  TLorentzVector* genLeptonOneP4 = 0 ;
  TLorentzVector* genLeptonTwoP4 = 0 ;
  TLorentzVector* photonP4 = 0       ;
  TLorentzVector* jetP4 = 0          ;
  TLorentzVector* tauP4 = 0          ;
  Int_t    jetIDOut                  ;
  Int_t    jetPUIDOut                ;
  Float_t  photonIDWeight = 1.       ;
  Int_t tauFlavor                    ;
  Float_t taudxyOut = 0.             ;
  Float_t taudzOut = 0.              ;
  UChar_t tauDeepAntiEle             ;
  UChar_t tauDeepAntiMu              ;
  UChar_t tauDeepAntiJet             ;
  UInt_t nMuons = 0                  ;
  SlimMuons_t slimMuons              ;
  UInt_t nElectrons = 0              ;
  SlimElectrons_t slimElectrons      ;
  UInt_t nTaus = 0                   ;
  UInt_t nExtraLep = 0               ;
  SlimTaus_t slimTaus                ;
  SlimJets_t slimJets                ;
  UInt_t nPhotons = 0                ;
  SlimPhotons_t slimPhotons          ;
  UInt_t nJets     = 0               ;
  UInt_t nJets20   = 0               ;
  UInt_t nJets20Rej = 0              ;
  UInt_t nFwdJets  = 0               ;
  UInt_t nBJets       = 0            ;
  UInt_t nBJetsM      = 0            ;
  UInt_t nBJetsL      = 0            ;
  UInt_t nBJets20     = 0            ;
  UInt_t nBJets20M    = 0            ;
  UInt_t nBJets20L    = 0            ;
  UInt_t nGenTausHad = 0             ;
  UInt_t nGenTausLep = 0             ;
  UInt_t nGenElectrons = 0           ;
  UInt_t nGenMuons = 0               ;
  UInt_t nGenHardTaus = 0            ; //for DY splitting by hard process
  UInt_t nGenHardElectrons = 0       ; //for DY splitting by hard process
  UInt_t nGenHardMuons = 0           ; //for DY splitting by hard process
  Float_t htSum = 0                  ;
  Float_t ht = 0                     ;
  Float_t htPhi = 0                  ;
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
  Float_t metCorr = 0.               ;
  Float_t metCorrPhi = 0.            ;
  Float_t covMet00  = 0.             ;
  Float_t covMet01  = 0.             ;
  Float_t covMet11  = 0.             ;
  Float_t massSVFit                  ;
  Float_t massErrSVFit               ;
  Int_t   svFitStatus                ;
  TLorentzVector* leptonOneSVP4 = 0  ;
  TLorentzVector* leptonTwoSVP4 = 0  ;
  //Info for b-tag studies
  Float_t jetsPt[kMaxParticles]      ;
  Float_t jetsEta[kMaxParticles]     ;
  Int_t   jetsFlavor[kMaxParticles]  ;
  Int_t   jetsBTag[kMaxParticles]    ;
  Float_t jetsRejPt[kMaxParticles]   ; //rejected by PU ID jet information for PU ID scale factors
  Float_t jetsRejEta[kMaxParticles]  ;
  //Info for fake tau studies
  Float_t tausPt[kMaxParticles]      ;
  Float_t tausEta[kMaxParticles]     ;
  Float_t tausPhi[kMaxParticles]     ;
  Bool_t  tausIsPositive[kMaxParticles];
  Int_t   tausDM[kMaxParticles]      ;
  Int_t   tausGenFlavor[kMaxParticles];
  UChar_t tausAntiJet[kMaxParticles] ;
  UChar_t tausAntiMu[kMaxParticles]  ;
  UChar_t tausMVAAntiMu[kMaxParticles];
  UChar_t tausAntiEle[kMaxParticles] ;
  Float_t tausWeight[kMaxParticles]  ;
  //Info for fake light lepton studies
  Float_t leptonsPt[kMaxParticles]   ;
  Float_t leptonsEta[kMaxParticles]  ;
  Bool_t  leptonsIsPositive[kMaxParticles];
  Bool_t  leptonsIsMuon[kMaxParticles];
  UChar_t leptonsID[kMaxParticles]   ;
  UChar_t leptonsIsoID[kMaxParticles];
  UChar_t leptonsTriggered[kMaxParticles];
  UChar_t leptonsGenFlavor[kMaxParticles];

  //Input data format (only ones that differ from output)
  Int_t nGoodPV                             ;
  UInt_t nJet                               ;
  UInt_t nMuon                              ;
  Int_t  nMuonsSkim                         ;
  UInt_t nElectron                          ;
  Int_t  nElectronsSkim                     ;
  UInt_t nTau                               ;
  Int_t  nTausSkim                          ;
  UInt_t nPhoton                            ;
  Float_t muonPt[kMaxParticles]             ;
  Float_t muonEta[kMaxParticles]            ;
  Float_t muonPhi[kMaxParticles]            ;
  Float_t muonMass[kMaxParticles]           ;
  Int_t   muonCharge[kMaxParticles]         ;
  Float_t muonRelIso[kMaxParticles]         ;
  UChar_t muonIsoId[kMaxParticles]          ;
  Bool_t  muonLooseId[kMaxParticles]        ;
  Bool_t  muonMediumId[kMaxParticles]       ;
  Bool_t  muonTightId[kMaxParticles]        ;
  Int_t   muonNTrkLayers[kMaxParticles]     ;
  Float_t muonRoccoSF[kMaxParticles]        ;
  UChar_t muonGenFlavor[kMaxParticles]      ;
  Float_t electronPt[kMaxParticles]         ;
  Float_t electronEta[kMaxParticles]        ;
  Float_t electronPhi[kMaxParticles]        ;
  Float_t electronMass[kMaxParticles]       ;
  Int_t   electronCharge[kMaxParticles]     ;
  Float_t electronDeltaEtaSC[kMaxParticles] ;
  Float_t electronMVAFall17[kMaxParticles]  ;
  Bool_t  electronWPL [kMaxParticles]       ;
  Bool_t  electronWP80[kMaxParticles]       ;
  Bool_t  electronWP90[kMaxParticles]       ;
  UChar_t electronGenFlavor[kMaxParticles]  ;
  Float_t tauPt[kMaxParticles]              ;
  Float_t tauEta[kMaxParticles]             ;
  Float_t tauPhi[kMaxParticles]             ;
  Float_t tauMass[kMaxParticles]            ;
  Int_t   tauCharge[kMaxParticles]          ;
  Float_t taudxy[kMaxParticles]             ;
  Float_t taudz[kMaxParticles]              ;
  UChar_t tauAntiEle[kMaxParticles]         ;
  UChar_t tauAntiEle2018[kMaxParticles]     ;
  UChar_t tauAntiMu[kMaxParticles]          ;
  UChar_t tauAntiJet[kMaxParticles]         ;
  Int_t   tauDecayMode[kMaxParticles]       ;
  Bool_t  tauIDDecayMode[kMaxParticles]     ;
  Bool_t  tauIDDecayModeNew[kMaxParticles]  ;
  UChar_t tauDeep2017VsE[kMaxParticles]     ;
  UChar_t tauDeep2017VsMu[kMaxParticles]    ;
  UChar_t tauDeep2017VsJet[kMaxParticles]   ;
  UChar_t tauGenID[kMaxParticles]           ;
  Float_t jetPt[kMaxParticles]              ;
  Float_t jetEta[kMaxParticles]             ;
  Float_t jetPhi[kMaxParticles]             ;
  Float_t jetMass[kMaxParticles]            ;
  Int_t   jetID[kMaxParticles]              ;
  Int_t   jetPUID[kMaxParticles]            ;
  Float_t jetBTagDeepB[kMaxParticles]       ;
  Float_t jetBTagCMVA[kMaxParticles]        ;
  Int_t   jetFlavor[kMaxParticles]          ;
  Float_t photonPt[kMaxParticles]           ;
  Float_t photonEta[kMaxParticles]          ;
  Float_t photonPhi[kMaxParticles]          ;
  Float_t photonMass[kMaxParticles]         ;
  Float_t photonDeltaEta[kMaxParticles]     ;
  Float_t photonMVAID[kMaxParticles]        ;
  Float_t photonMVAID17[kMaxParticles]      ;
  Bool_t  photonWP80[kMaxParticles]         ;
  Bool_t  photonWP90[kMaxParticles]         ;
  UInt_t  nTrigObjs = 0                     ;
  Int_t   trigObjFilterBits[kMaxTriggers]   ;
  Float_t trigObjPt[kMaxTriggers]           ;
  Float_t trigObjEta[kMaxTriggers]          ;
  Float_t trigObjPhi[kMaxTriggers]          ;
  Int_t   trigObjID[kMaxTriggers]           ;
  bool    HLT_IsoMu24                       ;
  bool    HLT_IsoMu27                       ;
  bool    HLT_Mu50                          ;
  bool    HLT_Ele27_WPTight_GsF             ;
  bool    HLT_Ele32_WPTight_GsF             ;
  bool    HLT_Ele32_WPTight_GsF_L1DoubleEG  ;
  Float_t zPtIn              = -1.          ;
  Float_t zMassIn            = -1.          ;
  Int_t   zLepOne            = 0            ;
  Int_t   zLepTwo            = 0            ;
  Float_t leptonOneGenPt     = 0.           ;
  Float_t leptonTwoGenPt     = 0.           ;
// Float_t genWeight                         ;
  // Float_t piWeight                          ;
  // Float_t PuppiMET                          ;
  // Float_t nPV                               ;

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
    float nbjets;
    float nbjetsm;
    float nbjetsl;
    float nphotons;
    float eventweight;
    float fulleventweight; //includes cross-section and number gen
    float eventcategory; //for identifying the process in mva trainings

    //identify to use in training
    float train; //  < 0 --> testing, > 0 --> training sample
  };

  NanoAODConversion(TTree * /*tree*/ =0) { }
  ~NanoAODConversion() { if(particleCorrections) delete particleCorrections; }
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

  virtual void    InitializeOutBranchStructure(TTree* tree);
  virtual void    InitializeInBranchStructure(TTree* tree);
  virtual float   BTagWeight(int WP);
  virtual void    InitializeTreeVariables(Int_t selection);
  virtual void    CountJets(int selection);
  virtual void    CountTaus(int selection);
  virtual void    CountLightLeptons(int selection);
  virtual void    ApplyMuonCorrections();
  virtual void    CountObjects();
  virtual bool    SelectionID(Int_t selection);
  virtual float   GetTauFakeSF(int genFlavor);
  virtual float   CorrectMET(int selection, float met);
  virtual float   GetZPtWeight(float pt);
  int             TauFlavorFromID(int ID) {
    if(ID == 1 || ID == 3) return 11;
    if(ID == 2 || ID == 4) return 13;
    if(ID == 5) return 15;
    return 26; //unknown
  }
  int             MuonFlavorFromID(int ID) {
    if(ID == 1 || ID == 5 || ID == 4 || ID == 3 || ID == 15) return 13;
    return 26; //unknown
  }
  int             ElectronFlavorFromID(int ID) {
    if(ID == 1 || ID == 5 || ID == 4 || ID == 3 || ID == 15) return 11;
    if(ID == 22) return 22;
    return 26; //unknown
  }
  virtual int      GetTriggerMatch(UInt_t index, bool isMuon, Int_t& trigIndex);
  int      GetTriggerMatch(UInt_t index, bool isMuon) {
    int tmp;
    return GetTriggerMatch(index, isMuon, tmp);
  }

  Long64_t fentry; //for tracking entry in functions
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();

  ParticleCorrections* particleCorrections = 0;
  PUWeight      fPUWeight;
  Int_t         fReplacePUWeights = 0; //0: do nothing 1: use locally defined value
  RoccoR*       fRoccoR;
  TRandom3*     fRnd;
  Int_t         fRndSeed = 90;

  TFile*        fOut;
  TDirectory*   fDirs[kSelections];
  TTree*        fOutTrees[kSelections];
  Float_t       fEventCount = 0.;
  Int_t         fYear = ParticleCorrections::k2016; //data taking year
  Int_t         fMuonIso = ParticleCorrections::kTightMuIso; //which isolation selection to use

  TString       fFolderName = ""; //name of the folder the tree is from

  Bool_t        fIsDY = false; //for applying Z pT and mass weights
  Tree_t        fTreeVars; //for filling the ttrees/mva evaluation

  Int_t         fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
  Int_t         fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
  Bool_t        fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
  Int_t         fMETWeights = 0; //re-weight events based on the MET
  Int_t         fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally
  Bool_t        fDoTriggerMatchPt = true; //use pT threshold when matching triggers
  Int_t         fUsePhotonWeight = -1; //whether to include a photon ID weight or not, -1 means set to 1

  Int_t         fVerbose = 0;
  Bool_t        fIgnoreNoTriggerMatch = false; //don't print a warning if no trigger match found
  Bool_t        fIgnoreNoSelection = false; //don't print a warning if no selection found

  Int_t         fSkipMuMuEE = 0; // whether to skip ee/mumu selections for now
  Int_t         fSkipMuMuEESS = 1; //whether to skip same sign ee/mumu selections for now

  //selection requirements
  /** Muons **/
  std::map<Int_t, UChar_t> fMuonIsoSelect;
  std::map<Int_t, Int_t>   fMuonIDSelect; //0 = any, 1 = loose, 2 = medium, 3 = tight
  /** Electrons **/
  std::map<Int_t, Int_t>   fElectronIDSelect; //0 = any, 1 = WPL, 2 = WP80, 3 = WP90
  /** Taus **/
  std::map<Int_t, Bool_t>  fTauUseDeep;
  std::map<Int_t, UChar_t> fTauAntiEleSelect;
  std::map<Int_t, UChar_t> fTauAntiMuSelect;
  std::map<Int_t, Bool_t>  fTauIDDecaySelect;
  std::map<Int_t, UChar_t> fTauAntiJetSelect;

  //for counting objects (usually by selection)
  /** Muons **/
  Float_t       fMuonPtCount = 0.;
  Float_t       fMuonEtaCount = 2.4;
  std::map<UInt_t, UChar_t> fMuonIsoCount;
  std::map<UInt_t, Int_t>   fMuonIDCount; //0 = any, 1 = loose, 2 = medium, 3 = tight
  std::map<UInt_t, Bool_t>  fCountMuons;
  std::map<UInt_t, std::map<UInt_t, UInt_t>> fMuonIndices;
  std::map<UInt_t, UInt_t>  fNMuons;
  /** Electrons **/
  Float_t       fElectronPtCount = 0.;
  Float_t       fElectronEtaCount = 2.5;
  std::map<UInt_t, Int_t>   fElectronIDCount; //0 = any, 1 = WPL, 2 = WP80, 3 = WP90
  std::map<UInt_t, Bool_t>  fCountElectrons;
  std::map<UInt_t, std::map<UInt_t, UInt_t>> fElectronIndices;
  std::map<UInt_t, UInt_t>  fNElectrons;
  /** Taus **/
  Float_t       fTauPtCount = 0.;
  Float_t       fTauEtaCount = 2.3;
  std::map<UInt_t, UChar_t> fTauAntiEleCount;
  std::map<UInt_t, UChar_t> fTauAntiMuCount;
  std::map<UInt_t, UChar_t> fTauAntiOldMuCount;
  std::map<UInt_t, UChar_t> fTauAntiJetCount;
  std::map<UInt_t, Bool_t>  fTauIDDecayCount;
  std::map<UInt_t, Float_t> fTauDeltaRCount; //distance from light lepton
  std::map<UInt_t, Bool_t>  fCountTaus;
  std::map<UInt_t, std::map<UInt_t, UInt_t>> fTauIndices;
  std::map<UInt_t, UInt_t>  fNTaus;
  /** Photons **/
  Float_t       fPhotonPtCount = 0.;
  std::map<UInt_t, Float_t> fPhotonDeltaRCount; //distance from leptons
  std::map<UInt_t, UInt_t>  fPhotonIDCount;
  std::map<UInt_t, std::map<UInt_t, UInt_t>> fPhotonIndices;
  std::map<UInt_t, UInt_t>  fNPhotons;


  //summary variables
  Int_t         fNEE         = 0;
  Int_t         fNMuMu       = 0;
  Int_t         fNEMu        = 0;
  Int_t         fNETau       = 0;
  Int_t         fNMuTau      = 0;
  Int_t         fNFailedTrig = 0;
  Int_t         fNFailed     = 0;
  Int_t         fNSkipped    = 0;
  Int_t         fQCDCounts[kSelections];
  Int_t         fNHighMuonTriggered = 0;

  ClassDef(NanoAODConversion,0);

};

#endif

#ifdef NanoAODConversion_cxx
void NanoAODConversion::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  for(int i = 0; i < kMaxParticles; ++i) jetFlavor[i] = 999; //initialize as unknown
  //initialize output here so it can steal the tree name for file naming
  if(fChain == 0 && tree != 0) {
    //name the tree and the file similarly
    TString name = Form("clfv_%i_%s",fYear+(2016-ParticleCorrections::k2016),fFolderName.Data());
    fOut = new TFile((name+".tree").Data(), "RECREATE","NanoAODConversion output tree file");
    TH1F* h = new TH1F(("Events_"+name).Data(), "Event counting", 10, 1, 11);
    h->Fill(1, fEventCount);
    h->Write();

    for(int selection = 0; selection < kSelections; ++selection) {
      TString selectionName = "";
      if     (selection == kMuTau) selectionName = "mutau";
      else if(selection == kETau ) selectionName = "etau";
      else if(selection == kEMu  ) selectionName = "emu";
      else if(selection == kMuMu ) selectionName = "mumu";
      else if(selection == kEE   ) selectionName = "ee";
      fDirs[selection] = fOut->mkdir(selectionName.Data());
      fDirs[selection]->cd();
      fOutTrees[selection] = new TTree(name.Data(), "NanoAODConversion tree");
      InitializeOutBranchStructure(fOutTrees[selection]);
    }
  }

  if(tree != 0) fChain = tree;
  else return;
  printf("NanoAODConversion::Init fChain = tree ");
  std::cout << tree->GetName() << ": " << tree << std::endl;
  InitializeInBranchStructure(fChain);

  printf("Total number of entries is %lld\n",fChain->GetEntriesFast());
}


Bool_t NanoAODConversion::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef NanoAODConversion_cxx
