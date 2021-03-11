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
#include "interface/SlimElectron_t.hh"
#include "interface/SlimMuon_t.hh"
#include "interface/SlimTau_t.hh"
#include "interface/SlimJet_t.hh"
#include "interface/SlimPhoton_t.hh"
#include "interface/Tree_t.hh"
#include "interface/EventHist_t.hh"
#include "interface/LepHist_t.hh"
#include "interface/PhotonHist_t.hh"
#include "interface/SystematicHist_t.hh"
//initialize local MVA weight files
#include "interface/TrkQualInit.hh"
#include "interface/MVAConfig.hh"
//define PU weights locally
#include "interface/PUWeight.hh"
//define BTag weights locally
#include "interface/BTagWeight.hh"
//define Z pT weights locally
#include "interface/ZPtWeight.hh"
//define Jet->tau weights locally
#include "interface/JetToTauWeight.hh"
//define Jet->lep weights locally
#include "interface/JetToLepWeight.hh"
//define SS->OS weights locally
#include "interface/QCDWeight.hh"
//define a systematic variation
#include "interface/SystematicShifts.hh"

class ZTauTauHistMaker : public TSelector {
public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
  enum {kMaxParticles = 50, kMaxCounts = 40};
  enum {kMuTau = 0, kETau = 100, kEMu = 200, kMuTauE = 300, kETauMu = 400, kMuMu = 500, kEE = 600};
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
  Float_t zPtWeightUp                ;
  Float_t zPtWeightDown              ;
  Float_t zPtWeightSys               ;
  Float_t zPt = -1.                  ;
  Float_t zMass = -1.                ;
  Bool_t  looseQCDSelection = false  ;
  Float_t genTauFlavorWeight         ;
  Float_t jetToTauWeight             ;
  Float_t jetToTauWeightUp           ;
  Float_t jetToTauWeightDown         ;
  Float_t jetToTauWeightSys          ;
  Float_t qcdWeight                  ;
  Float_t qcdWeightUp                ;
  Float_t qcdWeightDown              ;
  Float_t qcdWeightSys               ;
  Int_t tauDecayMode                 ;
  Float_t tauMVA                     ;
  Float_t tauES                      ;
  Float_t tauESUp                    ;
  Float_t tauESDown                  ;
  Int_t   tauESBin                   ;
  Int_t tauGenFlavor                 ;
  Int_t tauGenFlavorHad              ;
  Float_t tauVetoedJetPt             ;
  Float_t tauVetoedJetPtUnc          ;
  TLorentzVector* leptonOneP4 = 0    ;
  TLorentzVector* leptonTwoP4 = 0    ;
  Int_t leptonOneFlavor              ;
  Int_t leptonTwoFlavor              ;
  Float_t leptonOneD0                ;
  Float_t leptonTwoD0                ;
  Float_t leptonOneIso               ;
  Float_t leptonTwoIso               ;
  UChar_t leptonOneID1               ;
  UChar_t leptonOneID2               ;
  UChar_t leptonTwoID1               ;
  UChar_t leptonTwoID2               ;
  UChar_t leptonTwoID3               ;
  Int_t   leptonOneIndex             ;
  Int_t   leptonTwoIndex             ;
  Float_t leptonOneWeight1 = 1.      ;
  Float_t leptonOneWeight1_up = 1.   ;
  Float_t leptonOneWeight1_down = 1. ;
  Int_t   leptonOneWeight1_bin = 0   ;
  Float_t leptonOneWeight1_sys       ;
  Float_t leptonOneWeight2 = 1.      ;
  Float_t leptonOneWeight2_up = 1.   ;
  Float_t leptonOneWeight2_down = 1. ;
  Int_t   leptonOneWeight2_bin = 0   ;
  Float_t leptonOneWeight2_sys       ;
  Float_t leptonTwoWeight1 = 1.      ;
  Float_t leptonTwoWeight1_up = 1.   ;
  Float_t leptonTwoWeight1_down = 1. ;
  Int_t   leptonTwoWeight1_bin = 0   ;
  Float_t leptonTwoWeight1_sys       ;
  Float_t leptonTwoWeight2 = 1.      ;
  Float_t leptonTwoWeight2_up = 1.   ;
  Float_t leptonTwoWeight2_down = 1. ;
  Int_t   leptonTwoWeight2_bin = 0   ;
  Float_t leptonTwoWeight2_sys       ;
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
  Bool_t isLooseMuon                 ;
  Bool_t isLooseElectron             ;
  Bool_t isLooseTau                  ;
  Float_t jetsPt[kMaxParticles]      ;
  Float_t jetsEta[kMaxParticles]     ;
  Int_t   jetsFlavor[kMaxParticles]  ;
  Int_t   jetsBTag[kMaxParticles]    ;
  Float_t tausPt[kMaxParticles]      ;
  Float_t tausEta[kMaxParticles]     ;
  Float_t tausPhi[kMaxParticles]     ;
  Bool_t  tausIsPositive[kMaxParticles];
  Int_t   tausDM[kMaxParticles]      ;
  Int_t   tausGenFlavor[kMaxParticles];
  UChar_t tausAntiJet[kMaxParticles] ;
  UChar_t tausMVAAntiMu[kMaxParticles];
  UChar_t tausAntiMu[kMaxParticles]  ;
  UChar_t tausAntiEle[kMaxParticles] ;
  Float_t tausWeight[kMaxParticles]  ;
  UInt_t nExtraLep = 0               ;
  Float_t leptonsPt[kMaxParticles]   ;
  Float_t leptonsEta[kMaxParticles]  ;
  Bool_t  leptonsIsPositive[kMaxParticles];
  Bool_t  leptonsIsMuon[kMaxParticles];
  UChar_t leptonsID[kMaxParticles]   ;
  UChar_t leptonsIsoID[kMaxParticles];
  UChar_t leptonsTriggered[kMaxParticles];
  UChar_t leptonsGenFlavor[kMaxParticles];
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

  ZTauTauHistMaker(TTree * /*tree*/ = 0) : fMuonJetToTauWeight("mumu", 0), fMuonJetToTauMCWeight("mumu", 2),
                                           fElectronJetToTauWeight("ee", 0),
                                           fJetToMuonWeight("mumu"), fJetToElectronWeight("ee"),
                                           fQCDWeight("emu") { }
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
  virtual void    BookSystematicHistograms();
  virtual void    BookTrees();
  virtual void    FillEventHistogram(EventHist_t* Hist);
  virtual void    FillPhotonHistogram(PhotonHist_t* Hist);
  virtual void    FillLepHistogram(LepHist_t* Hist);
  virtual void    FillSystematicHistogram(SystematicHist_t* Hist);
  virtual void    InitializeTreeVariables(Int_t selection);
  virtual float   GetTauFakeSF(int genFlavor);
  virtual float   CorrectMET(int selection, float met);
  virtual void    CountSlimObjects();
  virtual int     Category(TString selection);
  virtual void    InitializeSystematics();

  virtual void    ProcessLLGStudy();


  Long64_t fentry; //for tracking entry in functions
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();
  TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
  std::vector<TString> fMvaNames = { //mva names for getting weights
    "mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
    "etau_BDT_38.higgs","etau_BDT_38.Z0",
    "emu_BDT_68.higgs","emu_BDT_68.Z0",
    "mutau_e_BDT_68.higgs","mutau_e_BDT_68.Z0",
    "etau_mu_BDT_68.higgs","etau_mu_BDT_68.Z0"};
  std::map<TString, std::vector<double>> fMVACutsByCategory;
  MVAConfig fMVAConfig; //contains MVA names and categories

  Int_t   fIsJetBinnedMVAs[kMaxMVAs]; //storing number of jets for MVA, < 0 if not binned
  Float_t fMvaOutputs[kMaxMVAs];
  Int_t   fTrkQualVersion = TrkQualInit::Default; //for updating which variables are used

  //Histograms:
  const static Int_t fn = 4000; //max histogram sets: 0 - 999 typical, 1000 - 1999 QCD, 2000 - 2999 MisID, 3000 - 3999 QCD+MisID
  const static Int_t kIds = 60; //max box cut ID sets
  const static Int_t fQcdOffset = 1000; //histogram set + offset = set with same sign selection
  const static Int_t fMisIDOffset = 2000; //histogram set + offset = set with loose ID selection
  Int_t fEventSets[fn];  //indicates which sets to create
  Int_t fTreeSets[fn];   //indicates which trees to create

  TFile*          fOut;
  TDirectory*     fTopDir;
  TDirectory*     fDirectories[5*fn]; // 0 - fn events, fn - 2fn photon, 2fn - 3fn lep, 3fn - 4fn trees, 4fn - 5fn systematic histograms
  EventHist_t*    fEventHist[fn];
  PhotonHist_t*   fPhotonHist[fn];
  LepHist_t*      fLepHist[fn];
  SystematicHist_t* fSystematicHist[fn];
  TTree*          fTrees[fn];

  TEventID*       fEventId[kIds]; //for applying box cuts, 0-9 zmutau, 10-19 zetau, 20-29 zemu, higgs + 30 to z sets

  TString         fFolderName = ""; //name of the folder the tree is from
  Int_t           fYear = 2016;

  Bool_t          fIsSignal = false;

  Int_t           fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: e/mu e/mu
  Bool_t          fIsDY = false; //for checking if DY --> Z pT weights
  Bool_t          fDYTesting = false; //for speeding up histogramming to only do DY weight related aspects
  Bool_t          fDYFakeTauTesting = false; //for speeding up histogramming to only do DY jet --> fake tau scale factor aspects
  Bool_t          fWJFakeTauTesting = false; //for speeding up histogramming to only do W+jet --> fake tau scale factor aspects
  Bool_t          fTTFakeTauTesting = false; //for speeding up histogramming to only do ttbar jet --> fake tau scale factor aspects
  Bool_t          fQCDFakeTauTesting = false; //for speeding up histogramming to only do qcd jet --> fake tau scale factor aspects
  Bool_t          fCutFlowTesting = false; //for only testing basic cutflow sets

  Bool_t          fDoSystematics = false;

  Int_t           fWriteTrees = 0; //write out ttrees for the events
  Double_t        fXsec = 0.; //cross-section for full event weight with trees
  Double_t        fLum = 0.; //luminosity full event weight with trees
  Tree_t          fTreeVars; //for filling the ttrees/mva evaluation
  Int_t           fEventCategory; //for identifying the process in mva trainings

  Int_t           fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
  Int_t           fFakeElectronIsoCut = 3; //fake electron tight ID category definition
  Int_t           fFakeMuonIsoCut = 3; //fake muon tight Iso ID category definition
  Int_t           fFakeTauIsoCut = 50; //fake tau tight Iso category definition
  Int_t           fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
  bool            fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
  Int_t           fMETWeights = 0; //re-weight events based on the MET

  Int_t           fRemoveTriggerWeights = 0;
  Int_t           fRemovePhotonIDWeights = 1;
  Int_t           fRemoveBTagWeights = 0; //0: do nothing 1: remove weights 2: replace weights
  BTagWeight      fBTagWeight;
  Int_t           fRemovePUWeights = 0; //0: do nothing 1: remove weights 2: replace weights
  PUWeight        fPUWeight; //object to define pu weights
  Int_t           fAddJetTauWeights = 1; //0: do nothing 1: weight anti-iso tau CR data
  JetToTauWeight  fMuonJetToTauWeight; //for mutau
  JetToTauWeight  fMuonJetToTauMCWeight; //for mutau using MC estimated factors
  JetToTauWeight  fElectronJetToTauWeight; //for etau
  JetToLepWeight  fJetToMuonWeight; //for mutau
  JetToLepWeight  fJetToElectronWeight; //for etau
  QCDWeight       fQCDWeight; //for emu

  Int_t           fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally
  ZPtWeight       fZPtWeight; //re-weight Drell-Yan pT vs Mass

  float           fFractionMVA = 0.; //fraction of events used to train. Ignore these events in histogram filling, reweight the rest to compensate
  TRandom*        fRnd = 0; //for splitting MVA testing/training
  Int_t           fRndSeed = 90; //random number generator seed
  Int_t           fSystematicSeed = 90; //for systematic variations
  SystematicShifts* fSystematicShifts; //decides if a systematic is shifted up or down
  bool            fReprocessMVAs = false; //whether or not to use the tree given MVA values
  Int_t           fBJetCounting = 1; // 0: pT > 30 1: pT > 25 2: pT > 20
  Int_t           fBJetTightness = 1; // 0: tight 1: medium 2: loose
  Int_t           fMETType = 0; // 0: PF corrected 1: PUPPI Corrected
  bool            fForceBJetSense = true; //force can't be more strict id bjets than looser id bjets
  bool            fIsNano = false; //whether the tree is nano AOD based or not

  Int_t           fVerbose = 0; //verbosity level

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
      tree->SetBranchStatus("leptonOneP4"         , 1);
      tree->SetBranchStatus("leptonTwoP4"         , 1);
      tree->SetBranchStatus("leptonOneFlavor"     , 1);
      tree->SetBranchStatus("leptonTwoFlavor"     , 1);
      tree->SetBranchStatus("leptonOneIndex"      , 1);
      tree->SetBranchStatus("leptonTwoIndex"      , 1);
      tree->SetBranchStatus("lepOneWeight1"       , 1);
      tree->SetBranchStatus("lepOneWeight1_up"    , 1);
      tree->SetBranchStatus("lepOneWeight1_down"  , 1);
      tree->SetBranchStatus("lepOneWeight1_bin"   , 1);
      tree->SetBranchStatus("lepOneWeight2"       , 1);
      tree->SetBranchStatus("lepOneWeight2_up"    , 1);
      tree->SetBranchStatus("lepOneWeight2_down"  , 1);
      tree->SetBranchStatus("lepOneWeight2_bin"   , 1);
      tree->SetBranchStatus("lepTwoWeight1"       , 1);
      tree->SetBranchStatus("lepTwoWeight1_up"    , 1);
      tree->SetBranchStatus("lepTwoWeight1_down"  , 1);
      tree->SetBranchStatus("lepTwoWeight1_bin"   , 1);
      tree->SetBranchStatus("lepTwoWeight2"       , 1);
      tree->SetBranchStatus("lepTwoWeight2_up"    , 1);
      tree->SetBranchStatus("lepTwoWeight2_down"  , 1);
      tree->SetBranchStatus("lepTwoWeight2_bin"   , 1);
      tree->SetBranchStatus("leptonOneID1"        , 1);
      tree->SetBranchStatus("leptonTwoID1"        , 1);
      tree->SetBranchStatus("leptonTwoID2"        , 1);
      tree->SetBranchStatus("photonP4"            , 1);
      tree->SetBranchStatus("photonIDWeight"      , 1);
      tree->SetBranchStatus("jetP4"               , 1);
      tree->SetBranchStatus("tauP4"               , 1);
      tree->SetBranchStatus("tauDeepAntiJet"      , 1);
      tree->SetBranchStatus("tauDeepAntiEle"      , 1);
      tree->SetBranchStatus("tauDeepAntiMu"       , 1);
      tree->SetBranchStatus("tauGenFlavor"        , 1);
      tree->SetBranchStatus("tauDecayMode"        , 1);
      tree->SetBranchStatus("tauFlavor"           , 1);
      tree->SetBranchStatus("tauEnergyScale"      , 1);
      tree->SetBranchStatus("tauES_up"            , 1);
      tree->SetBranchStatus("tauES_down"          , 1);
      tree->SetBranchStatus("looseQCDSelection"   , 1);
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
      tree->SetBranchStatus("tausPt"              , 1);
      tree->SetBranchStatus("tausWeight"          , 1);
      tree->SetBranchStatus("tausEta"             , 1);
      tree->SetBranchStatus("tausIsPositive"      , 1);
      tree->SetBranchStatus("tausDM"              , 1);
      tree->SetBranchStatus("tausGenFlavor"       , 1);
      tree->SetBranchStatus("tausAntiJet"         , 1);
      tree->SetBranchStatus("tausAntiEle"         , 1);
      tree->SetBranchStatus("tausAntiMu"          , 1);
      tree->SetBranchStatus("tausMVAAntiMu"       , 1);
      tree->SetBranchStatus("nExtraLep"           , 1);
      tree->SetBranchStatus("leptonsPt"           , 1);
      tree->SetBranchStatus("leptonsEta"          , 1);
      tree->SetBranchStatus("leptonsIsPositive"   , 1);
      tree->SetBranchStatus("leptonsIsMuon"       , 1);
      tree->SetBranchStatus("leptonsID"           , 1);
      tree->SetBranchStatus("leptonsIsoID"        , 1);
      tree->SetBranchStatus("leptonsTriggered"    , 1);
      tree->SetBranchStatus("leptonsGenFlavor"    , 1);
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
      if(!fReprocessMVAs) {
        for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {
          tree->SetBranchStatus(Form("mva%i",mva_i), 1);
        }
      }
    }
    TMVA::Tools::Instance(); //load the TMVA library
    for(int i = 0; i < kMaxMVAs; ++i) mva[i] = 0; //initially 0s

    if(fReprocessMVAs) {
      for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {

        mva[mva_i] = new TMVA::Reader("!Color:!Silent");
        TString selection = "";
        if(fMVAConfig.names_[mva_i].Contains("higgs")) selection += "h";
        else selection += "z";
        if(fMVAConfig.names_[mva_i].Contains("mutau")) selection += "mutau";
        else if(fMVAConfig.names_[mva_i].Contains("etau")) selection += "etau";
        else if(fMVAConfig.names_[mva_i].Contains("emu")) selection += "emu";
        else {
          printf ("Warning! Didn't determine mva weight file %s corresponding selection!\n",
                  fMVAConfig.names_[mva_i].Data());
          selection += "mutau"; //just to default to something
        }
        //add for leptonic tau channels FIXME: Put as part of original check
        if(fMVAConfig.names_[mva_i].Contains("mutau_e")) selection += "_e";
        else if(fMVAConfig.names_[mva_i].Contains("etau_mu")) selection += "_mu";

        Int_t isJetBinned = -1; // -1 is not binned, 0 = 0 jets, 1 = 1 jet, 2 = >1 jets
        if(fMVAConfig.names_[mva_i].Contains("_18") || //0 jet
           fMVAConfig.names_[mva_i].Contains("_48") ||
           fMVAConfig.names_[mva_i].Contains("_78"))
          isJetBinned = 0;
        else if(fMVAConfig.names_[mva_i].Contains("_19") || //1 jet
                fMVAConfig.names_[mva_i].Contains("_49") ||
                fMVAConfig.names_[mva_i].Contains("_79"))
          isJetBinned = 1;
        else if(fMVAConfig.names_[mva_i].Contains("_20") || //>1 jet
                fMVAConfig.names_[mva_i].Contains("_50") ||
                fMVAConfig.names_[mva_i].Contains("_80"))
          isJetBinned = 2;

        fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling

        printf("Using selection %s\n", selection.Data());
        //use a tool to initialize the MVA variables and spectators
        TrkQualInit trkQualInit(fTrkQualVersion, isJetBinned);
        trkQualInit.InitializeVariables(*(mva[mva_i]), selection, fTreeVars);
        //Initialize MVA weight file
        const char* f = Form("weights/%s.weights.xml",fMVAConfig.names_[mva_i].Data());
        mva[mva_i]->BookMVA(fMVAConfig.names_[mva_i].Data(),f);
        printf("Booked MVA %s with selection %s\n", fMVAConfig.names_[mva_i].Data(), selection.Data());
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
      fEventSets [kMuTau + 1] = 1; // all events
      fEventSets [kMuTau + 2] = 1; // events with >= 1 photon

      fEventSets [kMuTau + 3] = 1;
      fEventSets [kMuTau + 4] = 1;
      fEventSets [kMuTau + 5] = 1;
      fEventSets [kMuTau + 6] = 1;
      fEventSets [kMuTau + 7] = 1;
      fEventSets [kMuTau + 8] = 1;
      fTreeSets  [kMuTau + 8] = 1;
      fTreeSets  [kMuTau + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kMuTau + i] = 1;


      fEventSets [kMuTau + 20] = 1; //
      fEventSets [kMuTau + 21] = 1; //
      fEventSets [kMuTau + 22] = 1; // events with nJets = 0
      fEventSets [kMuTau + 23] = 1; // events with nJets > 0

      fEventSets [kMuTau + 24] = 1; // events within mass window
      fEventSets [kMuTau + 25] = 1; // events within mass window

      fEventSets [kMuTau + 26] = 1; // events top set
      fEventSets [kMuTau + 27] = 1;
      fEventSets [kMuTau + 28] = 1;
      fEventSets [kMuTau + 29] = 1;

      fEventSets [kMuTau + 30] = 1;
      fEventSets [kMuTau + 31] = 1;
      fEventSets [kMuTau + 32] = 1;

      fEventSets [kMuTau + 34] = 1;
      fEventSets [kMuTau + 35] = 1;
      fEventSets [kMuTau + 36] = 1;

      fEventSets [kMuTau + 40] = 1;
      fEventSets [kMuTau + 41] = 1;

    }
    else if(fFolderName == "etau") {
      fEventSets [kETau + 1] = 1; // all events
      fEventSets [kETau + 2] = 1; // events with >= 1 photon

      fEventSets [kETau + 3] = 1;
      fEventSets [kETau + 4] = 1;
      fEventSets [kETau + 5] = 1;
      fEventSets [kETau + 6] = 1;
      fEventSets [kETau + 7] = 1;
      fEventSets [kETau + 8] = 1;
      fTreeSets  [kETau + 8] = 1;
      fTreeSets  [kETau + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kETau + i] = 1;

      fEventSets [kETau + 20] = 1; //
      fEventSets [kETau + 21] = 1; //
      fEventSets [kETau + 22] = 1; // events with nJets = 0
      fEventSets [kETau + 23] = 1; // events with nJets > 0

      fEventSets [kETau + 24] = 1; // events within mass window
      fEventSets [kETau + 25] = 1; // events within mass window

      fEventSets [kETau + 26] = 1; // events top set
      fEventSets [kETau + 27] = 1;
      fEventSets [kETau + 28] = 1;
      fEventSets [kETau + 29] = 1;
      fEventSets [kETau + 30] = 1;
      fEventSets [kETau + 31] = 1;
      fEventSets [kETau + 32] = 1;

      fEventSets [kETau + 34] = 1;
      fEventSets [kETau + 35] = 1;
      fEventSets [kETau + 36] = 1;
    }
    else if(fFolderName == "emu") {
      fEventSets [kEMu  + 1] = 1; // all events
      fEventSets [kEMu  + 2] = 1; // events with >= 1 photon

      fEventSets [kEMu  + 3] = 1;
      fEventSets [kEMu  + 4] = 1;
      fEventSets [kEMu  + 5] = 1;
      fEventSets [kEMu  + 6] = 1;
      fEventSets [kEMu  + 7] = 1;
      fEventSets [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kEMu  + i] = 1;

      fEventSets [kEMu  + 20] = 1; //
      fEventSets [kEMu  + 21] = 1; //
      fEventSets [kEMu  + 22] = 1; // events with nJets = 0
      fEventSets [kEMu  + 23] = 1; // events with nJets > 0

      fEventSets [kEMu  + 24] = 1; // events within mass window
      fEventSets [kEMu  + 25] = 1; // events within mass window

      fEventSets [kEMu  + 26] = 1; // events top set
      fEventSets [kEMu  + 27] = 1;
      fEventSets [kEMu  + 28] = 1;
      fEventSets [kEMu  + 29] = 1;

      fEventSets [kEMu  + 34] = 1;
      fEventSets [kEMu  + 35] = 1;
      fEventSets [kEMu  + 36] = 1;
    }
    else if(fFolderName == "mumu") {
      fEventSets [kMuMu + 1] = 1; // all events
      fEventSets [kMuMu + 2] = 1; // events with >= 1 photon

      fEventSets [kMuMu + 3] = 1;
      fEventSets [kMuMu + 4] = 1;
      fEventSets [kMuMu + 5] = 1;
      fEventSets [kMuMu + 6] = 1;
      fEventSets [kMuMu + 7] = 1;
      fEventSets [kMuMu + 8] = 1;
      fTreeSets  [kMuMu + 8] = 1;
      fTreeSets  [kMuMu + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kMuMu + i] = 1;

      fEventSets [kMuMu + 20] = 1; //
      fEventSets [kMuMu + 21] = 1; //
      fEventSets [kMuMu + 22] = 1; // events with nJets = 0
      fEventSets [kMuMu + 23] = 1; // events with nJets > 0

      fEventSets [kMuMu + 24] = 1; // events within mass window
      fEventSets [kMuMu + 25] = 1; // events within mass window

      fEventSets [kMuMu + 26] = 1; // events top set
      fEventSets [kMuMu + 27] = 1;
      fEventSets [kMuMu + 28] = 1;
      fEventSets [kMuMu + 29] = 1;
      fEventSets [kMuMu + 30] = 1;

      fEventSets [kMuMu + 34] = 1;
      fEventSets [kMuMu + 35] = 1;
      fEventSets [kMuMu + 36] = 1;
      fEventSets [kMuMu + 50] = 1;
      fEventSets [kMuMu + 51] = 1;
    }
    else if(fFolderName == "ee") {
      fEventSets [kEE   + 1] = 1; // all events
      fEventSets [kEE   + 2] = 1; // events with >= 1 photon

      fEventSets [kEE   + 3] = 1;
      fEventSets [kEE   + 4] = 1;
      fEventSets [kEE   + 5] = 1;
      fEventSets [kEE   + 6] = 1;
      fEventSets [kEE   + 7] = 1;
      fEventSets [kEE   + 8] = 1;
      fTreeSets  [kEE   + 8] = 1;
      fTreeSets  [kEE   + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kEE   + i] = 1;

      fEventSets [kEE   + 20] = 1; //
      fEventSets [kEE   + 21] = 1; //
      fEventSets [kEE   + 22] = 1; // events with nJets = 0
      fEventSets [kEE   + 23] = 1; // events with nJets > 0

      fEventSets [kEE   + 24] = 1; // events within mass window
      fEventSets [kEE   + 25] = 1; // events within mass window

      fEventSets [kEE   + 26] = 1; // events top set
      fEventSets [kEE   + 27] = 1;
      fEventSets [kEE   + 28] = 1;
      fEventSets [kEE   + 29] = 1;
      fEventSets [kEE   + 30] = 1;

      fEventSets [kEE   + 34] = 1;
      fEventSets [kEE   + 35] = 1;
      fEventSets [kEE   + 36] = 1;
      fEventSets [kEE   + 50] = 1;
      fEventSets [kEE   + 51] = 1;
    }
    if(fFolderName == "emu") {
      //Leptonic tau channels
      //mu+tau_e
      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kMuTauE + i] = 1;
      //e+tau_mu
      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kETauMu + i] = 1;
    }

    //initialize all the histograms
    BookHistograms();

  }

  if(tree != 0) fChain = tree;
  else return;
  printf("ZTauTauHistMaker::Init fChain = tree ");
  std::cout << tree << std::endl;
  // //turn off slim vectors, as they're not that slim
  // fChain->SetBranchStatus("slimElectrons" , 0);
  // fChain->SetBranchStatus("slimMuons"     , 0);
  // fChain->SetBranchStatus("slimTaus"      , 0);
  // fChain->SetBranchStatus("slimJets"      , 0);
  // fChain->SetBranchStatus("slimPhotons"   , 0);

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
  fChain->SetBranchAddress("tauGenFlavor"        , &tauGenFlavor         );
  fChain->SetBranchAddress("tauDecayMode"        , &tauDecayMode         );
  fChain->SetBranchAddress("tauEnergyScale"      , &tauES                );
  fChain->SetBranchAddress("tauES_up"            , &tauESUp              );
  fChain->SetBranchAddress("tauES_down"          , &tauESDown            );
  if(!fDYTesting) {
    fChain->SetBranchAddress("genTauFlavorWeight"  , &genTauFlavorWeight   );
    fChain->SetBranchAddress("tauMVA"              , &tauMVA               );
    fChain->SetBranchAddress("tauGenFlavorHad"     , &tauGenFlavorHad      );
    fChain->SetBranchAddress("tauVetoedJetPt"      , &tauVetoedJetPt       );
    fChain->SetBranchAddress("tauVetoedJetPtUnc"   , &tauVetoedJetPtUnc    );
  }
  fChain->SetBranchAddress("leptonOneP4"         , &leptonOneP4          );
  fChain->SetBranchAddress("leptonTwoP4"         , &leptonTwoP4          );
  fChain->SetBranchAddress("leptonOneFlavor"     , &leptonOneFlavor      );
  fChain->SetBranchAddress("leptonTwoFlavor"     , &leptonTwoFlavor      );
  if(!fDYTesting) {
    fChain->SetBranchAddress("leptonOneD0"         , &leptonOneD0          );
    fChain->SetBranchAddress("leptonTwoD0"         , &leptonTwoD0          );
    fChain->SetBranchAddress("leptonOneIso"        , &leptonOneIso         );
    fChain->SetBranchAddress("leptonTwoIso"        , &leptonTwoIso         );
    fChain->SetBranchAddress("leptonOneID2"        , &leptonOneID2         );
    fChain->SetBranchAddress("leptonTwoID3"        , &leptonTwoID3         );
  }
  fChain->SetBranchAddress("leptonOneID1"        , &leptonOneID1         );
  fChain->SetBranchAddress("leptonTwoID1"        , &leptonTwoID1         );
  fChain->SetBranchAddress("leptonTwoID2"        , &leptonTwoID2         );
  fChain->SetBranchAddress("leptonOneIndex"      , &leptonOneIndex       );
  fChain->SetBranchAddress("leptonTwoIndex"      , &leptonTwoIndex       );

  fChain->SetBranchAddress("lepOneWeight1"       , &leptonOneWeight1     );
  fChain->SetBranchAddress("lepOneWeight1_up"    , &leptonOneWeight1_up  );
  fChain->SetBranchAddress("lepOneWeight1_down"  , &leptonOneWeight1_down);
  fChain->SetBranchAddress("lepOneWeight1_bin"   , &leptonOneWeight1_bin );
  fChain->SetBranchAddress("lepOneWeight2"       , &leptonOneWeight2     );
  fChain->SetBranchAddress("lepOneWeight2_up"    , &leptonOneWeight2_up  );
  fChain->SetBranchAddress("lepOneWeight2_down"  , &leptonOneWeight2_down);
  fChain->SetBranchAddress("lepOneWeight2_bin"   , &leptonOneWeight2_bin );
  fChain->SetBranchAddress("lepTwoWeight1"       , &leptonTwoWeight1     );
  fChain->SetBranchAddress("lepTwoWeight1_up"    , &leptonTwoWeight1_up  );
  fChain->SetBranchAddress("lepTwoWeight1_down"  , &leptonTwoWeight1_down);
  fChain->SetBranchAddress("lepTwoWeight1_bin"   , &leptonTwoWeight1_bin );
  fChain->SetBranchAddress("lepTwoWeight2"       , &leptonTwoWeight2     );
  fChain->SetBranchAddress("lepTwoWeight2_up"    , &leptonTwoWeight2_up  );
  fChain->SetBranchAddress("lepTwoWeight2_down"  , &leptonTwoWeight2_down);
  fChain->SetBranchAddress("lepTwoWeight2_bin"   , &leptonTwoWeight2_bin );

  fChain->SetBranchAddress("lepOneTrigWeight"    , &leptonOneTrigWeight  );
  fChain->SetBranchAddress("lepTwoTrigWeight"    , &leptonTwoTrigWeight  );
  fChain->SetBranchAddress("lepOneFired"         , &leptonOneFired       );
  fChain->SetBranchAddress("lepTwoFired"         , &leptonTwoFired       );
  if(!fDYTesting) {
    fChain->SetBranchAddress("genLeptonOneP4"    , &genLeptonOneP4       );
    fChain->SetBranchAddress("genLeptonTwoP4"    , &genLeptonTwoP4       );
  }
  fChain->SetBranchAddress("photonP4"            , &photonP4             );
  if(!fDYTesting) {
    fChain->SetBranchAddress("photonMVA"         , &photonMVA            );
    fChain->SetBranchAddress("photonIDWeight"      , &photonIDWeight       );
  }
  fChain->SetBranchAddress("jetP4"               , &jetOneP4             );
  if(!fDYTesting) {
    fChain->SetBranchAddress("jetBTag"             , &jetOneBTag           );
    fChain->SetBranchAddress("jetBMVA"             , &jetOneBMVA           );
    if(fIsNano) {
      fChain->SetBranchAddress("nJetsNano"       , &nSlimJets            );
      // fChain->SetBranchAddress("slimJets"     , &slimJets             );
    }
    if(fFolderName == "llg_study") {
      fChain->SetBranchAddress("jetTwoP4"        , &jetTwoP4             );
      fChain->SetBranchAddress("jetTwoBTag"      , &jetTwoBTag           );
      fChain->SetBranchAddress("jetTwoBMVA"      , &jetTwoBMVA           );
    }
  }
  fChain->SetBranchAddress("tauP4"               , &tauP4                );
  fChain->SetBranchAddress("tauFlavor"           , &tauFlavor            );
  fChain->SetBranchAddress("nMuons"              , &nMuons               );
  fChain->SetBranchAddress("tauDeepAntiJet"    , &tauDeepAntiJet       );
  fChain->SetBranchAddress("tauDeepAntiEle"    , &tauDeepAntiEle       );
  fChain->SetBranchAddress("tauDeepAntiMu"     , &tauDeepAntiMu        );
  if(!fDYTesting) {
    if(fIsNano) {
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
      fChain->SetBranchAddress("nPhotonsNano"    , &nSlimPhotons         );
      // fChain->SetBranchAddress("slimPhotons"          , &slimPhotons          );
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
  fChain->SetBranchAddress("tausPt"               , &tausPt               );
  fChain->SetBranchAddress("tausWeight"           , &tausWeight           );
  fChain->SetBranchAddress("tausEta"              , &tausEta              );
  fChain->SetBranchAddress("tausIsPositive"       , &tausIsPositive       );
  fChain->SetBranchAddress("tausDM"               , &tausDM               );
  fChain->SetBranchAddress("tausGenFlavor"        , &tausGenFlavor        );
  fChain->SetBranchAddress("tausAntiJet"          , &tausAntiJet          );
  fChain->SetBranchAddress("tausAntiEle"          , &tausAntiEle          );
  fChain->SetBranchAddress("tausAntiMu"           , &tausAntiMu           );
  fChain->SetBranchAddress("tausMVAAntiMu"        , &tausMVAAntiMu        );
  fChain->SetBranchAddress("nExtraLep"            , &nExtraLep            );
  fChain->SetBranchAddress("leptonsPt"            , &leptonsPt            );
  fChain->SetBranchAddress("leptonsEta"           , &leptonsEta           );
  fChain->SetBranchAddress("leptonsIsPositive"    , &leptonsIsPositive    );
  fChain->SetBranchAddress("leptonsIsMuon"        , &leptonsIsMuon        );
  fChain->SetBranchAddress("leptonsID"            , &leptonsID            );
  fChain->SetBranchAddress("leptonsIsoID"         , &leptonsIsoID         );
  fChain->SetBranchAddress("leptonsTriggered"     , &leptonsTriggered     );
  fChain->SetBranchAddress("leptonsGenFlavor"     , &leptonsGenFlavor     );
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
    fChain->SetBranchAddress("leptonOneSVP4"     , &leptonOneSVP4        );
    fChain->SetBranchAddress("leptonTwoSVP4"     , &leptonTwoSVP4        );
  }
  if(!fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {
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
