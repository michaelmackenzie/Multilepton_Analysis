//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format

#ifndef CLFVHistMaker_hh
#define CLFVHistMaker_hh


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
#include "TRandom3.h" //for splitting testing/training samples
#include "TDirectory.h"
#include "TFolder.h"

// TMVA includes
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

#include <iostream>

//local includes
#include "interface/GlobalConstants.h"

#include "interface/Tree_t.hh"
#include "interface/EventHist_t.hh"
#include "interface/LepHist_t.hh"
#include "interface/PhotonHist_t.hh"
#include "interface/SystematicHist_t.hh"

//initialize local MVA weight files
#include "interface/TrkQualInit.hh"
#include "interface/MVAConfig.hh"

//Correction objects
#include "interface/PUWeight.hh"
#include "interface/RoccoR.h"
#include "interface/JetPUWeight.hh"
#include "interface/PrefireWeight.hh"
#include "interface/BTagWeight.hh"
#include "interface/ZPtWeight.hh"
#include "interface/EmbeddingWeight.hh"
#include "interface/EmbeddingTnPWeight.hh"
#include "interface/TauIDWeight.hh"
#include "interface/JetToTauWeight.hh"
#include "interface/JetToTauComposition.hh"
// #include "interface/JetToLepWeight.hh"
#include "interface/QCDWeight.hh"
#include "interface/MuonIDWeight.hh"
#include "interface/ElectronIDWeight.hh"

#include "interface/SystematicShifts.hh"
#include "interface/SystematicGrouping.hh"

namespace CLFV {

  class CLFVHistMaker : public TSelector {
  public :
    TTreeReader     fReader;  //!the tree reader
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    enum {kMaxLeptons = 10, kMaxParticles = 50, kMaxTriggers = 100};
    enum {kMuTau = 0, kETau = 100, kEMu = 200, kMuTauE = 300, kETauMu = 400, kMuMu = 500, kEE = 600};
    enum {kMaxMVAs = 80};

    // Readers to access the data
    UInt_t  runNumber                  ;
    ULong64_t eventNumber              ;
    UInt_t  lumiSection                ;
    Int_t   nPV                        ;
    Float_t nPU                        ;
    UInt_t  nPartons                   ;
    Float_t genWeight = 1.             ;
    Float_t puWeight = 1.              ;
    Float_t zPt                        ;
    Float_t zMass                      ;
    Float_t zLepOnePt                  ;
    Float_t zLepTwoPt                  ;
    Float_t zLepOneEta                 ;
    Float_t zLepTwoEta                 ;
    Float_t zLepOnePhi                 ;
    Float_t zLepTwoPhi                 ;
    Float_t zLepOneMass                ;
    Float_t zLepTwoMass                ;
    Float_t zLepOneID                  ;
    Float_t zLepTwoID                  ;

    //lepton information
    //muons
    UInt_t  nMuon                                      ;
    Int_t   nGenMuons = 0                              ; // counting from gen part list matched to electroweak process in skimming stage
    Float_t Muon_pt                       [kMaxLeptons];
    Float_t Muon_eta                      [kMaxLeptons];
    Float_t Muon_phi                      [kMaxLeptons];
    Int_t   Muon_charge                   [kMaxLeptons];
    Bool_t  Muon_looseId                  [kMaxLeptons];
    Bool_t  Muon_mediumId                 [kMaxLeptons];
    Bool_t  Muon_tightId                  [kMaxLeptons];
    Float_t Muon_pfRelIso04_all           [kMaxLeptons];
    Float_t Muon_dxy                      [kMaxLeptons];
    Float_t Muon_dxyErr                   [kMaxLeptons];
    Float_t Muon_dz                       [kMaxLeptons];
    Float_t Muon_dzErr                    [kMaxLeptons];
    Int_t   Muon_nTrackerLayers           [kMaxLeptons];
    Bool_t  Muon_TaggedAsRemovedByJet     [kMaxLeptons];
    UChar_t Muon_genPartFlav              [kMaxLeptons];
    Int_t   Muon_genPartIdx               [kMaxLeptons];
    Float_t Muon_RoccoSF                  [kMaxLeptons]; //calculated momentum scale

    //electrons
    UInt_t  nElectron                                  ;
    Int_t   nGenElectrons = 0                          ; // counting from gen part list matched to electroweak process in skimming stage
    Float_t Electron_pt                   [kMaxLeptons];
    Float_t Electron_eta                  [kMaxLeptons];
    Float_t Electron_phi                  [kMaxLeptons];
    Int_t   Electron_charge               [kMaxLeptons];
    Float_t Electron_deltaEtaSC           [kMaxLeptons];
    Bool_t  Electron_mvaFall17V2Iso_WPL   [kMaxLeptons];
    Bool_t  Electron_mvaFall17V2Iso_WP90  [kMaxLeptons];
    Bool_t  Electron_mvaFall17V2Iso_WP80  [kMaxLeptons];
    Bool_t  Electron_mvaFall17V2noIso_WPL [kMaxLeptons];
    Bool_t  Electron_mvaFall17V2noIso_WP90[kMaxLeptons];
    Bool_t  Electron_mvaFall17V2noIso_WP80[kMaxLeptons];
    Float_t Electron_pfRelIso03_all       [kMaxLeptons];
    Float_t Electron_eCorr                [kMaxLeptons];
    Float_t Electron_dxy                  [kMaxLeptons];
    Float_t Electron_dxyErr               [kMaxLeptons];
    Float_t Electron_dz                   [kMaxLeptons];
    Float_t Electron_dzErr                [kMaxLeptons];
    Bool_t  Electron_TaggedAsRemovedByJet [kMaxLeptons];
    UChar_t Electron_genPartFlav          [kMaxLeptons];
    Int_t   Electron_genPartIdx           [kMaxLeptons];
    Float_t Electron_energyScale          [kMaxLeptons]; //energy scale replacing given one

    //taus
    UInt_t  nTau                                       ;
    Int_t   nGenTaus = 0                               ; // counting from gen part list matched to electroweak process in skimming stage
    Float_t Tau_pt                        [kMaxLeptons];
    Float_t Tau_eta                       [kMaxLeptons];
    Float_t Tau_phi                       [kMaxLeptons];
    Float_t Tau_mass                      [kMaxLeptons];
    Int_t   Tau_charge                    [kMaxLeptons];
    UChar_t Tau_idDeepTau2017v2p1VSe      [kMaxLeptons];
    UChar_t Tau_idDeepTau2017v2p1VSmu     [kMaxLeptons];
    UChar_t Tau_idDeepTau2017v2p1VSjet    [kMaxLeptons];
    UChar_t Tau_idAntiEle                 [kMaxLeptons];
    UChar_t Tau_idAntiEle2018             [kMaxLeptons];
    UChar_t Tau_idAntiMu                  [kMaxLeptons];
    UChar_t Tau_idAntiJet                 [kMaxLeptons];
    Int_t   Tau_decayMode                 [kMaxLeptons];
    Bool_t  Tau_idDecayMode               [kMaxLeptons];
    Bool_t  Tau_idDecayModeNewDMs         [kMaxLeptons];
    Float_t Tau_pfRelIso03_all            [kMaxLeptons];
    Float_t Tau_eCorr                     [kMaxLeptons];
    Float_t Tau_dxy                       [kMaxLeptons];
    Float_t Tau_dz                        [kMaxLeptons];
    Bool_t  Tau_TaggedAsRemovedByJet      [kMaxLeptons];
    UChar_t Tau_genPartFlav               [kMaxLeptons];
    Int_t   Tau_genPartIdx                [kMaxLeptons];
    Float_t Tau_energyScale               [kMaxLeptons]; //calculated enery scale

    //jets
    UInt_t  nJet                                         ;
    Float_t Jet_pt                        [kMaxParticles];
    Float_t Jet_eta                       [kMaxParticles];
    Float_t Jet_phi                       [kMaxParticles];
    Float_t Jet_mass                      [kMaxParticles];
    Int_t   Jet_charge                    [kMaxParticles];
    Int_t   Jet_jetId                     [kMaxParticles];
    Int_t   Jet_puId                      [kMaxParticles];
    Float_t Jet_pfRelIso03_all            [kMaxParticles];
    Float_t Jet_btagDeepB                 [kMaxParticles];
    Float_t Jet_btagCMVA                  [kMaxParticles];
    Float_t Jet_eCorr                     [kMaxParticles];
    Float_t Jet_dxy                       [kMaxParticles];
    Float_t Jet_dxyErr                    [kMaxParticles];
    Float_t Jet_dz                        [kMaxParticles];
    Float_t Jet_dzErr                     [kMaxParticles];
    Int_t   Jet_partonFlavour             [kMaxParticles];
    Bool_t  Jet_TaggedAsRemovedByMuon     [kMaxParticles];
    Bool_t  Jet_TaggedAsRemovedByElectron [kMaxParticles];

    //Trigger info
    Bool_t  HLT_IsoMu24                                  ;
    Bool_t  HLT_IsoMu27                                  ;
    Bool_t  HLT_Mu50                                     ;
    Bool_t  HLT_Ele27_WPTight_GsF                        ;
    Bool_t  HLT_Ele32_WPTight_GsF                        ;
    Bool_t  HLT_Ele32_WPTight_GsF_L1DoubleEG             ;
    UInt_t  nTrigObj                                     ;
    Int_t   TrigObj_filterBits             [kMaxTriggers];
    Float_t TrigObj_pt                     [kMaxTriggers];
    Float_t TrigObj_eta                    [kMaxTriggers];
    Float_t TrigObj_phi                    [kMaxTriggers];
    Int_t   TrigObj_id                     [kMaxTriggers];


    //Calculated values
    Float_t jetsRejPt[kMaxParticles]   ;
    Float_t jetsRejEta[kMaxParticles]  ;
    Float_t jetsPt[kMaxParticles]      ;
    Float_t jetsEta[kMaxParticles]     ;
    Int_t   jetsFlavor[kMaxParticles]  ;
    Int_t   jetsBTag[kMaxParticles]    ;

    Float_t tausPt[kMaxParticles]      ; //FIXME: Remove these tau arrays
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

    UInt_t nExtraLep = 0               ; //FIXME: Remove these light lepton arrays
    Float_t leptonsPt[kMaxParticles]   ;
    Float_t leptonsEta[kMaxParticles]  ;
    Bool_t  leptonsIsPositive[kMaxParticles];
    Bool_t  leptonsIsMuon[kMaxParticles];
    UChar_t leptonsID[kMaxParticles]   ;
    UChar_t leptonsIsoID[kMaxParticles];
    UChar_t leptonsTriggered[kMaxParticles];
    UChar_t leptonsGenFlavor[kMaxParticles];

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
    //gen-level info
    UChar_t LHE_Njets                  ;



    Int_t  mcEra                       ;
    UInt_t triggerLeptonStatus         ;
    UInt_t muonTriggerStatus           ;
    Float_t eventWeight                ;
    Float_t jetPUIDWeight = 1.         ;
    Float_t prefireWeight = 1.         ;
    Float_t topPtWeight = 1.           ;
    Float_t btagWeight = 1.            ;
    Float_t btagWeightUp = 1.          ;
    Float_t btagWeightDown = 1.        ;
    Float_t zPtWeight                  ;
    Float_t zPtWeightUp                ;
    Float_t zPtWeightDown              ;
    Float_t zPtWeightSys               ;
    Float_t embeddingWeight = 1.       ;
    Float_t embeddingUnfoldingWeight   ;
    Int_t   looseQCDSelection = 0      ;
    Float_t genTauFlavorWeight         ;

    Float_t jetToTauWeight             ;
    Float_t jetToTauWeightUp           ;
    Float_t jetToTauWeightDown         ;
    Float_t jetToTauWeightSys          ;
    Int_t   jetToTauWeightGroup        ;
    Float_t jetToTauWeightCorr         ;
    Float_t jetToTauWeightCorrUp       ;
    Float_t jetToTauWeightCorrDown     ;
    Float_t jetToTauWeightCorrSys      ;
    Float_t jetToTauWeightBias         ;
    Float_t jetToTauWeightBiasUp       ;
    Float_t jetToTauWeightBiasDown     ;
    Float_t jetToTauWeight_compUp      ;
    Float_t jetToTauWeight_compDown    ;

    Float_t qcdWeight                  ;
    Float_t qcdWeightUp                ;
    Float_t qcdWeightDown              ;
    Float_t qcdWeightSys               ;
    Float_t qcdClosure                 ; //Non-closure correction
    Float_t qcdIsoScale = 1.           ; //QCD bias correction

    Int_t tauDecayMode                 ;
    Float_t tauMVA                     ;
    Int_t tauGenFlavor                 ;
    Int_t tauGenID                     ;
    Int_t tauGenFlavorHad              ;

    TLorentzVector* leptonOneP4        ;
    TLorentzVector* leptonTwoP4        ;
    Float_t leptonOneSCEta             ;
    Float_t leptonTwoSCEta             ;
    Int_t leptonOneFlavor              ;
    Int_t leptonTwoFlavor              ;
    Int_t leptonOneGenFlavor           ;
    Int_t leptonTwoGenFlavor           ;
    Float_t leptonOnePtSF              ;
    Float_t leptonTwoPtSF              ;
    Bool_t leptonOneJetOverlap         ;
    Bool_t leptonTwoJetOverlap         ;
    Bool_t isFakeElectron = false      ;
    Bool_t isFakeMuon     = false      ;
    Float_t leptonOneDXY               ;
    Float_t leptonTwoDXY               ;
    Float_t leptonOneDZ                ;
    Float_t leptonTwoDZ                ;
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
    Int_t   leptonOneWeight1_group = 0 ;
    Float_t leptonOneWeight1_sys       ;
    Float_t leptonOneWeight2 = 1.      ;
    Float_t leptonOneWeight2_up = 1.   ;
    Float_t leptonOneWeight2_down = 1. ;
    Int_t   leptonOneWeight2_bin = 0   ;
    Float_t leptonOneWeight2_sys       ;
    Int_t   leptonOneWeight2_group = 0 ;
    Float_t leptonTwoWeight1 = 1.      ;
    Float_t leptonTwoWeight1_up = 1.   ;
    Float_t leptonTwoWeight1_down = 1. ;
    Int_t   leptonTwoWeight1_bin = 0   ;
    Int_t   leptonTwoWeight1_group = 0 ;
    Float_t leptonTwoWeight1_sys       ;
    Float_t leptonTwoWeight2 = 1.      ;
    Float_t leptonTwoWeight2_up = 1.   ;
    Float_t leptonTwoWeight2_down = 1. ;
    Int_t   leptonTwoWeight2_bin = 0   ;
    Int_t   leptonTwoWeight2_group = 0 ;
    Float_t leptonTwoWeight2_sys       ;
    Float_t leptonOneTrigWeight        ;
    Float_t leptonTwoTrigWeight        ;
    Int_t   leptonOneTrigger           ;
    Int_t   leptonTwoTrigger           ;
    Bool_t  leptonOneFired             ;
    Bool_t  leptonTwoFired             ;

    Int_t   nTrigModes                 ;
    Float_t triggerWeights[3]          ;
    TLorentzVector* genLeptonOneP4 = 0 ;
    TLorentzVector* genLeptonTwoP4 = 0 ;
    TLorentzVector* photonP4 = 0       ;
    Float_t         photonMVA          ;
    Float_t         photonIDWeight     ;
    TLorentzVector* jetOneP4 = 0       ;
    TLorentzVector* jetTwoP4 = 0       ;
    Int_t jetOneID                     ;
    Int_t jetOnePUID                   ;
    Bool_t jetOneBTag                  ;
    Float_t jetOneBMVA                 ;
    Bool_t jetTwoBTag                  ;
    Float_t jetTwoBMVA                 ;
    TLorentzVector* tauP4 = 0          ;
    Int_t  tauFlavor                   ;
    UChar_t tauDeepAntiEle             ;
    UChar_t tauDeepAntiMu              ;
    UChar_t tauDeepAntiJet             ;
    Float_t tauES                      ;
    Float_t tauES_up                   ;
    Float_t tauES_down                 ;
    Int_t   tauESBin                   ;
    Float_t eleES                      ;
    Float_t eleES_up                   ;
    Float_t eleES_down                 ;
    UInt_t nElectrons;
    UInt_t nLowPtElectrons             ;
    UInt_t nMuons;
    UInt_t nTaus;
    UInt_t nPhotons;
    UInt_t nJets                       ;
    UInt_t nJets20                     ;
    UInt_t nJets20Rej                  ;
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
    UInt_t nGenTausHad                 ;
    UInt_t nGenTausLep                 ;
    UInt_t nGenHardTaus                ; //for DY splitting by hard process
    UInt_t nGenHardElectrons           ; //for DY splitting by hard process
    UInt_t nGenHardMuons               ; //for DY splitting by hard process
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


    Float_t muon_trig_pt_    ; //lepton trigger thresholds
    Float_t electron_trig_pt_;

    CLFVHistMaker(int seed = 90, TTree * /*tree*/ = 0);
    ~CLFVHistMaker() {}

    Int_t   Version() const { return 2; }
    void    Begin(TTree *tree);
    void    SlaveBegin(TTree *tree);
    void    Init(TTree *tree);
    Bool_t  Notify();
    Bool_t  Process(Long64_t entry);
    Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
    void    SetOption(const char *option) { fOption = option; }
    void    SetObject(TObject *obj) { fObject = obj; }
    void    SetInputList(TList *input) { fInput = input; }
    TList  *GetOutputList() const { return fOutput; }
    void    SlaveTerminate();
    void    Terminate();

    void    InitializeInputTree(TTree* tree);
    void    InitializeEventWeights();
    void    ApplyElectronCorrections();
    void    ApplyMuonCorrections();
    void    ApplyTauCorrections();
    void    CountObjects();
    void    CountJets();
    void    MatchTriggers();
    void    ApplyTriggerWeights();

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
    int             MuonRelIsoID(float reliso) {
      //muon isolation levels
      //kVVLooseMuIso, kVLooseMuIso, kLooseMuIso, kMediumMuIso, kTightMuIso, kVTightMuIso, kVVTightMuIso, kMuonIsos
      constexpr double muonIsoValues[] = {0.5, 0.4, 0.25, 0.20, 0.15, 0.10, 0.05}; //corresponding values
      int id = 0;
      for(int level = 0; level < MuonIDWeight::kMuonIsos; ++level) {
        if(reliso < muonIsoValues[level]) ++id;
        else break;
      }
      return id;
    }

    void    BookHistograms();
    void    BookEventHistograms();
    void    BookPhotonHistograms();
    void    BookLepHistograms();
    void    BookSystematicHistograms();
    void    BookTrees();
    void    DeleteHistograms();

    void    FillAllHistograms(Int_t index);
    void    FillEventHistogram(EventHist_t* Hist);
    void    FillPhotonHistogram(PhotonHist_t* Hist);
    void    FillLepHistogram(LepHist_t* Hist);
    void    FillSystematicHistogram(SystematicHist_t* Hist);
    void    InitializeTreeVariables(Int_t selection);
    float   GetTauFakeSF(int genFlavor);
    float   CorrectMET(int selection, float met);
    int     Category(TString selection);
    void    InitializeSystematics();
    TString GetOutputName() {
      return Form("clfv%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }


    Long64_t fentry; //for tracking entry in functions
    //Define relevant fields
    TStopwatch* timer = new TStopwatch();
    // TStopwatch timer_funcs; //for measuring time taken in each function
    TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
    std::vector<TString> fMvaNames = { //mva names for getting weights
                                      "mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
                                      "etau_BDT_38.higgs","etau_BDT_38.Z0",
                                      "emu_BDT_68.higgs","emu_BDT_68.Z0",
                                      "mutau_e_BDT_68.higgs","mutau_e_BDT_68.Z0",
                                      "etau_mu_BDT_68.higgs","etau_mu_BDT_68.Z0",
                                      "mutau_TMlpANN_8.higgs","mutau_TMlpANN_8.Z0", //10 - 19: alternate mvas
                                      "etau_TMlpANN_8.higgs","etau_TMlpANN_8.Z0",
                                      "emu_BDT_68.higgs","emu_BDT_68.Z0",
                                      "mutau_e_TMlpANN_8.higgs","mutau_e_TMlpANN_8.Z0",
                                      "etau_mu_TMlpANN_8.higgs","etau_mu_TMlpANN_8.Z0"
    };
    std::map<TString, std::vector<double>> fMVACutsByCategory;
    MVAConfig fMVAConfig; //contains MVA names and categories

    Int_t   fIsJetBinnedMVAs[kMaxMVAs]; //storing number of jets for MVA, < 0 if not binned
    Float_t fMvaOutputs[kMaxMVAs];
    Int_t   fTrkQualVersion = TrkQualInit::Default; //for updating which variables are used

    //Histograms:
    const static Int_t fn = 4000; //max histogram sets: 0 - 999 typical, 1000 - 1999 QCD, 2000 - 2999 MisID, 3000 - 3999 QCD+MisID
    const static Int_t fQcdOffset = 1000; //histogram set + offset = set with same sign selection
    const static Int_t fMisIDOffset = 2000; //histogram set + offset = set with loose ID selection
    Int_t fEventSets[fn];  //indicates which sets to create
    Int_t fSysSets[fn];  //indicates which systematic sets to create
    Int_t fTreeSets[fn];   //indicates which trees to create

    TFile*          fOut;
    TDirectory*     fTopDir;
    TDirectory*     fDirectories[5*fn]; // 0 - fn events, fn - 2fn photon, 2fn - 3fn lep, 3fn - 4fn trees, 4fn - 5fn systematic histograms
    EventHist_t*    fEventHist[fn];
    PhotonHist_t*   fPhotonHist[fn];
    LepHist_t*      fLepHist[fn];
    SystematicHist_t* fSystematicHist[fn];
    TTree*          fTrees[fn];
    TH1D*           fCutFlow; //cut-flow

    TString         fSelection = ""; //data selection to consider
    Int_t           fYear = 2016;
    TString         fDataset = ""; //data set being analyzed

    Bool_t          fIsSignal = false;
    Int_t           fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: e/mu e/mu
    Bool_t          fIsDY = false; //for checking if DY --> Z pT weights
    Int_t           fWNJets = -1;  //for splitting w+jets samples
    Bool_t          fDYTesting = false; //for speeding up histogramming to only do DY weight related aspects
    Bool_t          fDYFakeTauTesting = false; //for speeding up histogramming to only do DY jet --> fake tau scale factor aspects
    Bool_t          fWJFakeTauTesting = false; //for speeding up histogramming to only do W+jet --> fake tau scale factor aspects
    Bool_t          fTTFakeTauTesting = false; //for speeding up histogramming to only do ttbar jet --> fake tau scale factor aspects
    Bool_t          fQCDFakeTauTesting = false; //for speeding up histogramming to only do qcd jet --> fake tau scale factor aspects
    Bool_t          fCutFlowTesting = false; //for only testing basic cutflow sets
    Bool_t          fJetTauTesting = false; //for including MC j->tau sets in testing mode
    Bool_t          fTriggerTesting = false; //add emu histograms for electron/muon triggered events
    Bool_t          fDoMVASets = false; //for filling MVA cut sets even in DYTesting mode

    Int_t           fDoSystematics = 0; //0: ignore systematic histograms 1: fill them -1: only fill them

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
    Int_t           fIsEmbed = 0; //whether or not this is an embeded sample
    Int_t           fUseEmbedCuts = 0; //whether or not to use the kinematic restrictions for embedded sample generation
    Int_t           fUseEmbedTnPWeights = 1; //whether or not to use the locally calculated lepton ID/trigger weights
    bool            fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
    Int_t           fMETWeights = 0; //re-weight events based on the MET
    Int_t           fUseMCEstimatedFakeLep = 0;

    Int_t           fSystematicSeed; //for systematic variations

    Int_t           fRemoveTriggerWeights = 0; // 0: do nothing 1: remove weights 2: replace weights
    Int_t           fUpdateMCEra = 0; //update the MC era flag
    Int_t           fRemovePhotonIDWeights = 1;
    Int_t           fUseBTagWeights = 0; //0: do nothing 1: apply weights
    BTagWeight      fBTagWeight;
    Int_t           fRemovePUWeights = 0; //0: do nothing 1: remove weights 2: replace weights
    PUWeight        fPUWeight; //object to define pu weights
    RoccoR*         fRoccoR; //Rochester muon momentum corrections
    Int_t           fUseJetPUIDWeights = 1; //use jet PU ID weights
    JetPUWeight     fJetPUWeight; //object to define jet PU ID weights
    Int_t           fUsePrefireWeights = 1; //use pre-fire weights
    PrefireWeight   fPrefireWeight; //object to define pre-fire weights
    Int_t           fUseQCDWeights = 1; //use QCD SS --> OS transfer weights
    Int_t           fAddJetTauWeights = 1; //0: do nothing 1: weight anti-iso tau CR data
    // JetToTauWeight  fMuonJetToTauWeight; //for mutau
    // JetToTauWeight  fMuonJetToTauMCWeight; //for mutau using MC estimated factors
    JetToTauComposition  fMuonJetToTauComp; //for mutau
    JetToTauComposition  fMuonJetToTauSSComp; //for mutau SS systematic test
    JetToTauWeight* fMuonJetToTauWeights  [JetToTauComposition::kLast];
    JetToTauWeight* fMuonJetToTauMCWeights[JetToTauComposition::kLast]; //for measuring DR to AR/SR biases
    // JetToTauWeight  fElectronJetToTauWeight; //for etau
    JetToTauComposition  fElectronJetToTauComp; //for etau
    JetToTauComposition  fElectronJetToTauSSComp; //for etau SS systematic test
    JetToTauWeight* fElectronJetToTauWeights  [JetToTauComposition::kLast];
    JetToTauWeight* fElectronJetToTauMCWeights[JetToTauComposition::kLast]; //for measuring DR to AR/SR biases
    Bool_t          fUseJetToTauComposition = false;
    Float_t*        fJetToTauWts       = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauCorrs     = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauBiases    = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauComps     = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauCompsUp   = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauCompsDown = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauMCWts     = new Float_t[JetToTauComposition::kLast]; //weights using MC-based scale factors
    Float_t*        fJetToTauMCCorrs   = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauMCBiases  = new Float_t[JetToTauComposition::kLast];

    // JetToLepWeight  fJetToMuonWeight; //for mutau
    // JetToLepWeight  fJetToElectronWeight; //for etau
    QCDWeight       fQCDWeight; //for emu
    MuonIDWeight    fMuonIDWeight;
    ElectronIDWeight fElectronIDWeight;

    Int_t           fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally
    ZPtWeight       fZPtWeight; //re-weight Drell-Yan pT vs Mass
    EmbeddingWeight fEmbeddingWeight; //correct di-muon embedding selection unfolding
    EmbeddingTnPWeight fEmbeddingTnPWeight; //correct lepton ID/trigger efficiencies in embedding simulation
    Int_t           fEmbeddedTesting = 0; //play with embedding configurations/weights
    TauIDWeight*    fTauIDWeight; //tau ID/ES corrections

    float           fFractionMVA = 0.; //fraction of events used to train. Ignore these events in histogram filling, reweight the rest to compensate
    TRandom3*       fRnd = 0; //for splitting MVA testing/training
    Int_t           fRndSeed = 90; //random number generator seed (not the same as systematics, as want fixed even for systematic studies)
    SystematicShifts* fSystematicShifts; //decides if a systematic is shifted up or down
    SystematicGrouping fSystematicGrouping; //Groups systematics together
    bool            fReprocessMVAs = false; //whether or not to use the tree given MVA values
    Int_t           fBJetCounting = 1; // 0: pT > 30 1: pT > 25 2: pT > 20
    Int_t           fBJetTightness = 1; // 0: tight 1: medium 2: loose
    Int_t           fMETType = 0; // 0: PF corrected 1: PUPPI Corrected
    bool            fForceBJetSense = true; //force can't be more strict id bjets than looser id bjets
    bool            fIsNano = false; //whether the tree is nano AOD based or not

    Int_t           fVerbose = 0; //verbosity level

    ClassDef(CLFVHistMaker,0);

  };
}
#endif

#ifdef CLFVHISTMAKER_CXX
using namespace CLFV;
void CLFVHistMaker::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  if(fChain == 0 && tree != 0) {

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
    fChain = tree;
    fOut = new TFile(GetOutputName(), "RECREATE","CLFVHistMaker output histogram file");
    fTopDir = fOut->mkdir("Data");
    fTopDir->cd();
    std::cout << "Using output filename " << GetOutputName() << std::endl;

    for(int i = 0; i < fn; ++i) {
      fEventSets[i]  = 0;
      fSysSets[i] = 0;
    }
    for(int i = 0; i < fn; ++i) {
      fTreeSets[i]  = 0;
    }

    //Event Sets
    // fEventSets [0] = 0; // all events
    const bool mutau = fSelection == "" || fSelection == "mutau";
    const bool etau  = fSelection == "" || fSelection == "etau" ;
    const bool emu   = fSelection == "" || fSelection == "emu"  ;
    const bool mumu  = fSelection == "" || fSelection == "mumu" ;
    const bool ee    = fSelection == "" || fSelection == "ee"   ;
    if(mutau) {
      fEventSets [kMuTau + 1] = 1; // all events
      fEventSets [kMuTau + 2] = 1; //
      fEventSets [kMuTau + 3] = 1;
      fEventSets [kMuTau + 4] = 1;

      // fEventSets [kMuTau + 5] = 1;
      // fEventSets [kMuTau + 6] = 1;
      fEventSets [kMuTau + 7] = 1;
      fEventSets [kMuTau + 8] = 1;
      fTreeSets  [kMuTau + 8] = 1;
      fTreeSets  [kMuTau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
      fSysSets   [kMuTau + 8] = 1;
      // MVA categories
      // for(int i = 9; i < 19; ++i) fEventSets[kMuTau + i] = 1;


      // fEventSets [kMuTau + 20] = 1; //
      // fEventSets [kMuTau + 21] = 1; //
      // fEventSets [kMuTau + 22] = 1; // events with nJets = 0
      // fSysSets   [kMuTau + 22] = 1;
      // fEventSets [kMuTau + 23] = 1; // events with nJets > 0
      // fSysSets   [kMuTau + 23] = 1;

      // fEventSets [kMuTau + 24] = 1; // events within mass window
      // fEventSets [kMuTau + 25] = 1; // events within mass window

      // fEventSets [kMuTau + 26] = 1; // events top set
      // fEventSets [kMuTau + 27] = 1; //genuine tau MC
      // fEventSets [kMuTau + 28] = 1;
      // fEventSets [kMuTau + 29] = 1;


      // jet --> tau DRs
      fEventSets [kMuTau + 30] = 1; //QCD
      fEventSets [kMuTau + 31] = 1; //W+Jets
      fEventSets [kMuTau + 32] = 1; //Top
      fEventSets [kMuTau + 34] = 1; //signal region but W+Jets j-->tau weights (no composition)
      fEventSets [kMuTau + 89] = 1; //W+Jets MC weights

      // jet --> tau DRs with MC taus
      fEventSets [kMuTau + 33] = 1; //Nominal selection without j-->tau weights, loose ID only
      fEventSets [kMuTau + 35] = 1; //Nominal selection
      fEventSets [kMuTau + 36] = 1; //QCD
      fEventSets [kMuTau + 37] = 1; //W+Jets
      fEventSets [kMuTau + 38] = 1; //Top
      fEventSets [kMuTau + 88] = 1; //W+Jets MC weights
      // fEventSets [kMuTau + 90] = 1; //Loose nominal selection
      fEventSets [kMuTau + 92] = 1; //Loose nominal selection
      fEventSets [kMuTau + 94] = 1; //Loose nominal selection

      // jet --> tau MC taus in nominal selection, different process weights
      // fEventSets [kMuTau + 80] = 1; //QCD
      fEventSets [kMuTau + 81] = 1; //W+Jets
      fEventSets [kMuTau + 82] = 1; //Top
      // fEventSets [kMuTau + 85] = 1; //QCD, no fake MC
      fEventSets [kMuTau + 86] = 1; //W+Jets, no fake MC
      fEventSets [kMuTau + 87] = 1; //Top, no fake MC
      // fEventSets [kMuTau + 91] = 1; //Loose nominal selection, no fake MC
      fEventSets [kMuTau + 93] = 1; //Loose nominal selection, no fake MC
      fEventSets [kMuTau + 95] = 1; //Loose nominal selection, no fake MC

      // fEventSets [kMuTau + 40] = 1; //Closure j-->tau test, true MC taus
      // fEventSets [kMuTau + 41] = 1; //Closure j-->tau test, fake MC taus + weights
      fEventSets [kMuTau + 42] = 1; //fake MC taus in the nominal selection

      // fEventSets [kMuTau + 45] = 1; //taus in the barrel
      // fEventSets [kMuTau + 46] = 1; //taus in the endcap
    }
    if(etau) {
      fEventSets [kETau + 1] = 1; // all events
      fEventSets [kETau + 2] = 1;
      fEventSets [kETau + 3] = 1;
      fEventSets [kETau + 4] = 1;
      // fEventSets [kETau + 5] = 1;
      // fEventSets [kETau + 6] = 1;
      fEventSets [kETau + 7] = 1;
      fEventSets [kETau + 8] = 1;
      fTreeSets  [kETau + 8] = 1;
      fTreeSets  [kETau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
      fSysSets   [kETau + 8] = 1;

      // // MVA categories
      // for(int i = 9; i < 19; ++i) fEventSets[kETau + i] = 1;

      // fEventSets [kETau + 20] = 1; //
      // fEventSets [kETau + 21] = 1; //
      // fEventSets [kETau + 22] = 1; // events with nJets = 0
      // fSysSets   [kETau + 22] = 1;
      // fEventSets [kETau + 23] = 1; // events with nJets > 0
      // fSysSets   [kETau + 23] = 1;

      // fEventSets [kETau + 24] = 1; // events within mass window
      // fEventSets [kETau + 25] = 1; // events within mass window

      // fEventSets [kETau + 26] = 1; // events top set
      // fEventSets [kETau + 27] = 1;

      // jet --> tau DRs
      fEventSets [kETau + 30] = 1; //QCD
      fEventSets [kETau + 31] = 1; //W+Jets
      fEventSets [kETau + 32] = 1; //Top
      fEventSets [kETau + 34] = 1; //signal region but W+Jets j-->tau weights (no composition)
      fEventSets [kETau + 89] = 1; //W+Jets MC weights

      // jet --> tau DRs with MC taus
      fEventSets [kETau + 33] = 1; //Nominal selection without j-->tau weights, loose ID only
      fEventSets [kETau + 35] = 1; //Nominal selection
      fEventSets [kETau + 36] = 1; //QCD
      fEventSets [kETau + 37] = 1; //W+Jets
      fEventSets [kETau + 38] = 1; //Top
      fEventSets [kETau + 88] = 1; //W+Jets MC weights
      // fEventSets [kETau + 90] = 1; //Loose nominal selection
      fEventSets [kETau + 92] = 1; //Loose nominal selection
      fEventSets [kETau + 94] = 1; //Loose nominal selection

      // jet --> tau MC taus in nominal selection, different process weights
      // fEventSets [kETau + 80] = 1; //QCD
      fEventSets [kETau + 81] = 1; //W+Jets
      fEventSets [kETau + 82] = 1; //Top
      // fEventSets [kETau + 85] = 1; //QCD, no fake MC
      fEventSets [kETau + 86] = 1; //W+Jets, no fake MC
      fEventSets [kETau + 87] = 1; //Top, no fake MC
      // fEventSets [kETau + 91] = 1; //Loose nominal selection, no fake MC
      fEventSets [kETau + 93] = 1; //Loose nominal selection, no fake MC
      fEventSets [kETau + 95] = 1; //Loose nominal selection, no fake MC

      // fEventSets [kETau + 40] = 1; //Closure j-->tau test, true MC taus
      // fEventSets [kETau + 41] = 1; //Closure j-->tau test, fake MC taus + weights
      fEventSets [kETau + 42] = 1; //fake MC taus in the nominal selection

      // fEventSets [kETau + 45] = 1; //taus in the barrel
      // fEventSets [kETau + 46] = 1; //taus in the endcap
    }
    if(emu) {
      fEventSets [kEMu  + 1] = 1; // all events
      fEventSets [kEMu  + 2] = 1; // after pT cuts
      fEventSets [kEMu  + 3] = 1; // after eta, mass, trigger cuts
      fEventSets [kEMu  + 4] = 1; // after additional IDs

      // fEventSets [kEMu  + 5] = 1;
      // fEventSets [kEMu  + 6] = 1;
      fEventSets [kEMu  + 7] = 1;
      fEventSets [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training
      fSysSets   [kEMu  + 8] = 1;

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kEMu  + i] = 1;
      fSysSets[kEMu  + 9  + fMVAConfig.categories_["zemu"].size()] = 1; //start with most significant category
      fSysSets[kEMu  + 8  + fMVAConfig.categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kEMu  + 14 + fMVAConfig.categories_["hemu"].size()] = 1; //start with most significant category
      fSysSets[kEMu  + 13 + fMVAConfig.categories_["hemu"].size()] = 1; //second most significant category

      // fEventSets [kEMu  + 20] = 1; //
      // fEventSets [kEMu  + 21] = 1; //
      // fEventSets [kEMu  + 22] = 1; // events with nJets = 0
      // fSysSets   [kEMu  + 22] = 1;
      // fEventSets [kEMu  + 23] = 1; // events with nJets > 0
      // fSysSets   [kEMu  + 23] = 1;

      fEventSets [kEMu  + 24] = 1; // events within mass window
      fEventSets [kEMu  + 25] = 1; // events within mass window

      // fEventSets [kEMu  + 26] = 1; // events top set
      // fEventSets [kEMu  + 27] = 1;
      // fEventSets [kEMu  + 28] = 1;
      // fEventSets [kEMu  + 29] = 1;

      fEventSets [kEMu  + 32] = 1; // > 0 b-jets (top selection)

      // fEventSets [kEMu  + 34] = 1;
      fEventSets [kEMu  + 35] = 1;
      fEventSets [kEMu  + 90] = 1; //Loose nominal selection
      fEventSets [kEMu  + 91] = 1; //Loose nominal selection, no fake MC
      // fEventSets [kEMu  + 36] = 1;
      // fEventSets [kEMu  + 45] = 1; //taus in the barrel, but put all e+mu here
      // fEventSets [kEMu  + 46] = 1; //taus in the endcap, but put no e+mu here

      if(!fDYTesting || fTriggerTesting) { //testing e+mu triggering
        fEventSets [kEMu  + 60] = 1; //electron fired
        fEventSets [kEMu  + 61] = 1; //muon fired
        fEventSets [kEMu  + 62] = 1; //both fired
        fEventSets [kEMu  + 63] = 1; //electron could trigger
        fEventSets [kEMu  + 64] = 1; //muon could trigger
        fEventSets [kEMu  + 65] = 1; //both could trigger
        fEventSets [kEMu  + 66] = 1; //electron can't trigger
        fEventSets [kEMu  + 67] = 1; //muon can't trigger
      }

      fEventSets [kEMu  + 70] = 1; //loose ID electron, not muon region
      fEventSets [kEMu  + 71] = 1; //loose ID electron and muon region
    }
    if(mumu) {
      fEventSets [kMuMu + 1] = 1; // all events
      fEventSets [kMuMu + 2] = 1; // events with >= 1 photon

      fEventSets [kMuMu + 3] = 1;
      fEventSets [kMuMu + 4] = 1;
      // fEventSets [kMuMu + 5] = 1;
      // fEventSets [kMuMu + 6] = 1;
      fEventSets [kMuMu + 7] = 1;
      fEventSets [kMuMu + 8] = 1;
      fSysSets   [kMuMu + 8] = 1;
      // fTreeSets  [kMuMu + 8] = 1;
      // fTreeSets  [kMuMu + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kMuMu + i] = 1;
      fSysSets[kMuMu  + 9  + fMVAConfig.categories_["zemu"].size()] = 1; //start with most significant category
      fSysSets[kMuMu  + 8  + fMVAConfig.categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kMuMu  + 14 + fMVAConfig.categories_["hemu"].size()] = 1; //start with most significant category
      fSysSets[kMuMu  + 13 + fMVAConfig.categories_["hemu"].size()] = 1; //second most significant category

      // fEventSets [kMuMu + 20] = 1; //
      // fEventSets [kMuMu + 21] = 1; //
      fEventSets [kMuMu + 22] = 1; // events with nJets = 0
      // fSysSets   [kMuMu + 22] = 1;
      fEventSets [kMuMu + 23] = 1; // events with nJets > 0
      // fSysSets   [kMuMu + 23] = 1;

      fEventSets [kMuMu + 24] = 1; // events within mass window
      fEventSets [kMuMu + 25] = 1; // events within mass window

      fEventSets [kMuMu + 26] = 1; // events top set
      fEventSets [kMuMu + 27] = 1;
      // fEventSets [kMuMu + 28] = 1;
      // fEventSets [kMuMu + 29] = 1;
      // fEventSets [kMuMu + 30] = 1;

      // fEventSets [kMuMu + 34] = 1;
      fEventSets [kMuMu + 35] = 1;
      // fEventSets [kMuMu + 36] = 1;
      fEventSets [kMuMu + 50] = 1;
      fEventSets [kMuMu + 51] = 1;

      if(!fDYTesting || fTriggerTesting) { //testing triggering
        fEventSets [kMuMu + 60] = 1; //one fired
        fEventSets [kMuMu + 61] = 1; //two fired
        fEventSets [kMuMu + 62] = 1; //both fired
        fEventSets [kMuMu + 63] = 1; //two could trigger
        fEventSets [kMuMu + 66] = 1; //two can't trigger
      }
    }
    if(ee) {
      fEventSets [kEE   + 1] = 1; // all events
      fEventSets [kEE   + 2] = 1;
      fEventSets [kEE   + 3] = 1;
      fEventSets [kEE   + 4] = 1;

      // fEventSets [kEE   + 5] = 1;
      // fEventSets [kEE   + 6] = 1;
      fEventSets [kEE   + 7] = 1;
      fEventSets [kEE   + 8] = 1;
      fSysSets   [kEE   + 8] = 1;
      // fTreeSets  [kEE   + 8] = 1;
      // fTreeSets  [kEE   + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kEE + i] = 1;
      fSysSets[kEE    + 9  + fMVAConfig.categories_["zemu"].size()] = 1; //start with most significant category
      fSysSets[kEE    + 8  + fMVAConfig.categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kEE    + 14 + fMVAConfig.categories_["hemu"].size()] = 1; //start with most significant category
      fSysSets[kEE    + 13 + fMVAConfig.categories_["hemu"].size()] = 1; //second most significant category

      // fEventSets [kEE   + 20] = 1; //
      // fEventSets [kEE   + 21] = 1; //
      fEventSets [kEE   + 22] = 1; // events with nJets = 0
      // fSysSets   [kEE   + 22] = 1;
      fEventSets [kEE   + 23] = 1; // events with nJets > 0
      // fSysSets   [kEE   + 23] = 1;

      fEventSets [kEE   + 24] = 1; // events within mass window
      fEventSets [kEE   + 25] = 1; // events within mass window

      fEventSets [kEE   + 26] = 1; // events top set
      fEventSets [kEE   + 27] = 1;
      // fEventSets [kEE   + 28] = 1;
      // fEventSets [kEE   + 29] = 1;
      // fEventSets [kEE   + 30] = 1;

      // fEventSets [kEE   + 34] = 1;
      fEventSets [kEE   + 35] = 1;
      // fEventSets [kEE   + 36] = 1;
      fEventSets [kEE   + 50] = 1;
      fEventSets [kEE   + 51] = 1;

      if(!fDYTesting || fTriggerTesting) { //testing triggering
        fEventSets [kEE  + 60] = 1; //one fired
        fEventSets [kEE  + 61] = 1; //two fired
        fEventSets [kEE  + 62] = 1; //both fired
        fEventSets [kEE  + 63] = 1; //two could trigger
        fEventSets [kEE  + 66] = 1; //two can't trigger
      }
    }
    if(emu) {
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

  if(tree) fChain = tree;
  else return;
  printf("CLFVHistMaker::Init fChain = tree ");
  std::cout << tree << std::endl;

  //setup the branch addresses
  InitializeInputTree(tree);
  if(!fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {
      fChain->SetBranchAddress(Form("mva%i",mva_i), &fMvaOutputs[mva_i]);
    }
  }
  //Load more data into memory to compensate for slow XROOTD processing
  auto cachesize = 500000000U;
  fChain->SetCacheSize(cachesize);
  fChain->AddBranchToCache("*",true);
  fChain->LoadBaskets();
  printf("Total number of entries is %lld\n",fChain->GetEntriesFast());
}


Bool_t CLFVHistMaker::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef CLFVHISTMAKER_CXX
