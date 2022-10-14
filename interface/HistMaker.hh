//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format

#ifndef HISTMAKER_HH
#define HISTMAKER_HH


#include <TROOT.h>
#include <TTree.h>
#include <TChain.h>
#include <TEventList.h>
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
#include "interface/Lepton_t.hh"

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
#include "interface/SignalZWeight.hh"
#include "interface/EmbeddingWeight.hh"
#include "interface/EmbeddingTnPWeight.hh"
#include "interface/TauIDWeight.hh"
#include "interface/JetToTauWeight.hh"
#include "interface/JetToTauComposition.hh"
#include "interface/QCDWeight.hh"
#include "interface/MuonIDWeight.hh"
#include "interface/ElectronIDWeight.hh"

#include "interface/Systematics.hh"
#include "interface/SystematicShifts.hh"
#include "interface/SystematicGrouping.hh"

namespace CLFV {

  class HistMaker : public TSelector {
  public :
    TTreeReader     fReader;  //!the tree reader
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    enum {kMaxLeptons = 10, kMaxParticles = 50, kMaxTriggers = 100, kMaxGenPart = 200};
    enum {kMuTau = 0, kETau = 100, kEMu = 200, kMuTauE = 300, kETauMu = 400, kMuMu = 500, kEE = 600};
    enum {kMaxMVAs = 80};

    // Readers to access the data
    UInt_t    runNumber                  ;
    ULong64_t eventNumber                ;
    UInt_t    lumiSection                ;
    UChar_t   LHE_Njets                  ;
    Int_t     nPV                        ;
    Float_t   nPU                        ;
    UInt_t    nPartons                   ;
    Float_t   genWeight = 1.             ;
    Float_t   puWeight = 1.              ;
    Float_t   zPt                        ;
    Float_t   zMass                      ;
    Float_t   zEta                       ;
    Float_t   zLepOnePt                  ;
    Float_t   zLepTwoPt                  ;
    Float_t   zLepOneEta                 ;
    Float_t   zLepTwoEta                 ;
    Float_t   zLepOnePhi                 ;
    Float_t   zLepTwoPhi                 ;
    Float_t   zLepOneMass                ;
    Float_t   zLepTwoMass                ;
    Float_t   zLepOneID                  ;
    Float_t   zLepTwoID                  ;
    Int_t     zLepOneDecayIdx  = -99     ;
    Int_t     zLepTwoDecayIdx  = -99     ;

    //gen-level info
    UInt_t nGenPart                                    ;
    Int_t  GenPart_pdgId                [kMaxGenPart];
    Int_t  GenPart_genPartIdxMother     [kMaxGenPart];

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
    Float_t Electron_energyScaleUp        [kMaxLeptons];
    Float_t Electron_energyScaleDown      [kMaxLeptons];

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
    Float_t Tau_leadTrkPtOverTauPt        [kMaxLeptons];
    Float_t Tau_leadTrkDeltaEta           [kMaxLeptons];
    Float_t Tau_leadTrkDeltaPhi           [kMaxLeptons];
    Bool_t  Tau_TaggedAsRemovedByJet      [kMaxLeptons];
    UChar_t Tau_genPartFlav               [kMaxLeptons];
    Int_t   Tau_genPartIdx                [kMaxLeptons];
    Float_t Tau_energyScale               [kMaxLeptons]; //calculated enery scale
    Float_t Tau_energyScaleUp             [kMaxLeptons]; //calculated enery scale
    Float_t Tau_energyScaleDown           [kMaxLeptons]; //calculated enery scale

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
    Bool_t  Jet_TaggedAsRemovedByTau      [kMaxParticles];

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
    Float_t jetsRejPt   [kMaxParticles]; //fail PU ID
    Float_t jetsRejEta  [kMaxParticles];
    Float_t jetsPt      [kMaxParticles]; //accepted jets
    Float_t jetsEta     [kMaxParticles];
    Int_t   jetsFlavor  [kMaxParticles];
    Int_t   jetsBTag    [kMaxParticles];

    Float_t htSum                      ;
    Float_t ht                         ;
    Float_t htPhi                      ;

    //Different met definitions
    Float_t puppMET                    ;
    Float_t puppMETphi                 ;
    Float_t puppMETJERUp               ;
    Float_t puppMETJESUp               ;
    Float_t puppMETphiJERUp            ;
    Float_t puppMETphiJESUp            ;

    //MET field to use
    Float_t met                        ;
    Float_t metPhi                     ;
    Float_t metCorr                    ;
    Float_t metCorrPhi                 ;

    //event information
    Int_t   mcEra                      ;
    UInt_t  triggerLeptonStatus        ; //which triggers fired (muon, electron, or both)
    UInt_t  muonTriggerStatus          ; //which muon trigger was fired
    Bool_t  isFakeElectron = false     ;
    Bool_t  isFakeMuon     = false     ;
    Int_t   looseQCDSelection = 0      ;
    Bool_t  chargeTest = true          ;

    //Event weights
    Float_t eventWeight = 1.           ;
    Float_t jetPUIDWeight = 1.         ;
    Float_t prefireWeight = 1.         ;
    Float_t prefireWeight_up = 1.      ;
    Float_t prefireWeight_down = 1.    ;
    Float_t topPtWeight = 1.           ;
    Float_t btagWeight = 1.            ;
    Float_t btagWeightUp = 1.          ;
    Float_t btagWeightDown = 1.        ;
    Float_t signalZWeight = 1.         ;
    Float_t zPtWeight = 1.             ;
    Float_t zPtWeightUp = 1.           ;
    Float_t zPtWeightDown = 1.         ;
    Float_t zPtWeightSys = 1.          ;
    Float_t embeddingWeight = 1.       ;
    Float_t embeddingUnfoldingWeight = 1.;

    Float_t jetToTauWeight             ;
    Float_t jetToTauWeightUp           ;
    Float_t jetToTauWeightDown         ;
    Float_t jetToTauWeightCorr         ;
    Float_t jetToTauWeightCorrUp       ;
    Float_t jetToTauWeightCorrDown     ;
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

    //Tau information
    TLorentzVector* tauP4 = 0          ;
    Int_t   tauDecayMode               ;
    Int_t   tauGenFlavor               ;
    Int_t   tauGenID                   ;
    UChar_t tauDeepAntiEle             ;
    UChar_t tauDeepAntiMu              ;
    UChar_t tauDeepAntiJet             ;
    Float_t tauES                      ;
    Float_t tauES_up                   ;
    Float_t tauES_down                 ;
    // Int_t   tauESBin                   ;

    //Selected lepton info
    Lepton_t leptonOne                 ;
    Lepton_t leptonTwo                 ;
    TLorentzVector* genLeptonOneP4 = 0 ;
    TLorentzVector* genLeptonTwoP4 = 0 ;

    Float_t triggerWeights[3]          ;
    Float_t triggerWeightsSys[6]       ;

    //Additional event object information
    TLorentzVector* photonP4 = 0       ;
    Float_t         photonMVA          ;
    Float_t         photonIDWeight     ;
    TLorentzVector* jetOneP4 = 0       ;
    Int_t jetOneID                     ;
    Int_t jetOnePUID                   ;
    Bool_t jetOneBTag                  ;
    Float_t jetOneBMVA                 ;

    UInt_t nElectrons                  ;
    UInt_t nMuons                      ;
    UInt_t nTaus                       ;
    UInt_t nPhotons                    ;
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
    UInt_t nGenHardTaus                ; //for DY splitting by hard process
    UInt_t nGenHardElectrons           ; //for DY splitting by hard process
    UInt_t nGenHardMuons               ; //for DY splitting by hard process

    Float_t muon_trig_pt_    ; //lepton trigger thresholds
    Float_t electron_trig_pt_;

    Float_t tmpFloats[10]; //fields for debugging uses

    HistMaker(int seed = 90, TTree * /*tree*/ = 0);
    virtual ~HistMaker();

    Int_t   Version() const { return 1; }
    virtual void    Begin(TTree *tree);
    void    SlaveBegin(TTree *tree);
    void    Init(TTree *tree);
    Bool_t  Notify();
    virtual Bool_t  Process(Long64_t entry);
    Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
    void    SetOption(const char *option) { fOption = option; }
    void    SetObject(TObject *obj) { fObject = obj; }
    void    SetInputList(TList *input) { fInput = input; }
    TList  *GetOutputList() const { return fOutput; }
    void    SlaveTerminate();
    void    Terminate();

    void    InitializeInputTree(TTree* tree);
    void    SetEventList(TTree* tree);
    void    InitializeEventWeights();
    void    ApplyElectronCorrections();
    void    ApplyMuonCorrections();
    void    ApplyTauCorrections();
    void    CountObjects();
    void    CountJets();
    void    SetKinematics();
    int     GetTriggerMatch(TLorentzVector* lv, bool isMuon, Int_t& trigIndex);
    void    MatchTriggers();
    void    ApplyTriggerWeights();

    //get the index for a timer, adding it if not already in the list
    int     GetTimerNumber(TString name) {
      if(name == "") {
        std::cout << "HistMaker::" << __func__ << ": Timer name empty!\n";
        throw 20;
      }
      for(int i = 0; i < fNTimes; ++i) {
        if(name == fTimeNames[i]) return i;
        //Add the timer in the first empty slot
        if(fTimeNames[i] == "") {
          fTimeNames[i] = name;
          return i;
        }
      }
      return -1;
    }
    void    IncrementTimer(TString name, bool isinitialized) {
      IncrementTimer(GetTimerNumber(name), isinitialized);
    }
    void    IncrementTimer(int itime, bool isinitialized) {
      if(itime >= fNTimes) {
        printf("HistMaker::%s: Attempting to increment timer %i (%s), beyond maximum of %i\n", __func__, itime, fTimeNames[itime].Data(), fNTimes);
        throw 10;
      }
      if(fTimeCounts[itime] > 0 || isinitialized) {
        auto t = std::chrono::steady_clock::now();
        fDurations[itime] += std::chrono::duration_cast<std::chrono::microseconds>(t-fTimes[itime]).count();
        fTimes[itime] = t;
      } else {
        fTimes[itime] = std::chrono::steady_clock::now();
      }
      ++fTimeCounts[itime];
    }
    void    PrintTimerInfo() {
      printf("HistMaker processing time information:\n");
      for(int itime = 0; itime < fNTimes; ++itime) {
        if(fTimeCounts[itime] <= 0 || fDurations[itime] <= 0) continue;
        printf(" Timer %15s (%2i): Total processing time %10.3f s, %10.1f Hz\n",
               fTimeNames[itime].Data(), itime, fDurations[itime]/1.e6, fTimeCounts[itime]*1.e6/fDurations[itime]);
      }
    }

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

    virtual void    InitHistogramFlags();
    virtual void    BookHistograms();
    virtual void    BookEventHistograms();
    virtual void    BookPhotonHistograms();
    virtual void    BookLepHistograms();
    virtual void    BookSystematicHistograms();
    virtual void    BookTrees();
    virtual void    DeleteHistograms();

    void            BookBaseEventHistograms(Int_t index, const char* dirname);
    void            BookBaseLepHistograms(Int_t index, const char* dirname);
    void            BookBaseTree(Int_t index);

    virtual void    FillAllHistograms(Int_t index);
    virtual void    FillEventHistogram(EventHist_t* Hist);
    virtual void    FillPhotonHistogram(PhotonHist_t* Hist);
    virtual void    FillLepHistogram(LepHist_t* Hist);
    virtual void    FillSystematicHistogram(SystematicHist_t* Hist);

    void            FillBaseEventHistogram(EventHist_t* Hist);
    void            FillBaseLepHistogram(LepHist_t* Hist);

    Bool_t  InitializeEvent(Long64_t entry);
    void    SwapSameFlavor();
    void    InitializeTreeVariables();
    int     Category(TString selection);
    void    InitializeSystematics();
    virtual TString GetOutputName() {
      return Form("hist%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }

    //Check if this event is being split to a different output/run
    Bool_t SplitSampleEvent() {
      //DY Splitting
      if(fDYType > 0) {
        // 1 = tau, 2 = muon or electron channel
        if(nGenHardTaus == 0) { //Z->ee/mumu
          if(fDYType == 1) return kTRUE;
        } else if(nGenHardTaus == 2) { //Z->tautau
          if(fDYType == 2) return kTRUE;
        } else {
          std::cout << "Warning! Unable to identify type of DY event!" << std::endl
                    << "nGenHardTaus = " << nGenHardTaus << std::endl
                    << "nGenHardMuons = " << nGenHardMuons << std::endl
                    << "nGenHardElectrons = " << nGenHardElectrons << std::endl
                    << "fDYType = " << fDYType << std::endl
                    << "Entry " << fentry << std::endl;
          return kTRUE;
        }
      }

      //split W+Jets into N(LHE jets) generated for binned sample combination
      if(fWNJets > -1 && LHE_Njets != fWNJets) {
        return kTRUE;
      }

      //If running embedding, reject di-tau production from non-embedding MC (except tau-tau DY MC, which is already separated by histogram files)
      if(fDYType != 1 && fUseEmbedCuts && !fIsEmbed && !fIsData && nGenTaus == 2) {
        return kTRUE;
      }
      return kFALSE;
    }

    Long64_t fentry; //for tracking entry in functions
    //Define relevant fields
    TStopwatch* timer = new TStopwatch();
    // TStopwatch timer_funcs; //for measuring time taken in each function
    TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
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

    Int_t           fDoSystematics = 0; //0: ignore systematic histograms 1: fill them -1: only fill them

    Int_t           fWriteTrees = 0; //write out ttrees for the events
    Double_t        fXsec = 0.; //cross-section for full event weight with trees
    Double_t        fLum = 0.; //luminosity full event weight with trees
    Tree_t          fTreeVars; //for filling the ttrees/mva evaluation
    Int_t           fEventCategory; //for identifying the process in mva trainings

    Int_t           fDoHiggs = 1; //include Higgs-centered analysis
    Int_t           fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
    Int_t           fFakeElectronIsoCut = 3; //fake electron tight ID category definition
    Int_t           fFakeMuonIsoCut = 3; //fake muon tight Iso ID category definition
    Int_t           fFakeTauIsoCut = 50; //fake tau tight Iso category definition
    Int_t           fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
    Int_t           fIsEmbed = 0; //whether or not this is an embeded sample
    Int_t           fIsLLEmbed = 0; //whether or not this is an ll -> ll embedding (mumu/ee)
    Int_t           fUseEmbedCuts = 0; //whether or not to use the kinematic restrictions for embedded sample generation
    Int_t           fUseEmbedTnPWeights = 1; //whether or not to use the locally calculated lepton ID/trigger weights
    bool            fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
    Int_t           fMETWeights = 0; //re-weight events based on the MET
    Int_t           fUseMCEstimatedFakeLep = 0;
    Int_t           fDoTriggerMatching = 1; //match trigger objects to selected leptons

    Int_t           fProcessSSSF = 1; //process same-sign, same-flavor data
    Int_t           fDoEventList = 1;

    Int_t           fSystematicSeed; //for systematic variations

    Systematics     fSystematics; //systematics information

    Int_t           fRemoveTriggerWeights = 0; // 0: do nothing 1: remove weights 2: replace weights
    Int_t           fUpdateMCEra = 0; //update the MC era flag
    Int_t           fRemovePhotonIDWeights = 1;
    Int_t           fUseBTagWeights = 0; //0: do nothing 1: apply weights
    BTagWeight*     fBTagWeight;
    Int_t           fRemovePUWeights = 0; //0: do nothing 1: remove weights 2: replace weights
    PUWeight*       fPUWeight; //object to define pu weights
    RoccoR*         fRoccoR; //Rochester muon momentum corrections
    Int_t           fUseJetPUIDWeights = 1; //use jet PU ID weights
    JetPUWeight*    fJetPUWeight; //object to define jet PU ID weights
    Int_t           fUsePrefireWeights = 1; //use pre-fire weights, 0 = none, 1 = use predefined weights, 2 = replace with local weights
    PrefireWeight*  fPrefireWeight; //object to define pre-fire weights
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
    Float_t*        fJetToTauWts       = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauWtsUp     = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauWtsDown   = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauCorrs     = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauBiases    = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauComps     = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauCompsUp   = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauCompsDown = new Float_t[JetToTauComposition::kLast];
    Bool_t          fUseJetToTauComposition = false;
    Float_t*        fJetToTauMCWts     = new Float_t[JetToTauComposition::kLast]; //weights using MC-based scale factors
    Float_t*        fJetToTauMCCorrs   = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauMCBiases  = new Float_t[JetToTauComposition::kLast];

    QCDWeight       fQCDWeight; //for emu
    MuonIDWeight    fMuonIDWeight;
    ElectronIDWeight fElectronIDWeight;

    Int_t           fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally
    ZPtWeight*      fZPtWeight; //re-weight Drell-Yan pT vs Mass
    SignalZWeight   fSignalZWeight; //re-weight signal to match Drell-Yan MC
    Int_t           fUseSignalZWeights = 1; //whether or not to match the signal to the Drell-Yan MC
    EmbeddingWeight* fEmbeddingWeight; //correct di-muon embedding selection unfolding
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

    Long64_t        fCacheSize = 20000000U; //20MB cache by default
    Bool_t          fLoadBaskets = false;

    int                                   fNotifyCount = 50000;
    const static int                      fNTimes = 20;
    std::chrono::steady_clock::time_point fTimes[fNTimes]; //for tracking processing time
    long                                  fDurations[fNTimes];
    int                                   fTimeCounts[fNTimes];
    TString                               fTimeNames[fNTimes]; //name of the specific timer
    int                                   fPrintTime = 1;

    ///////////////////////////////////////////
    // Histogramming helper fields
    int set_offset;
    bool mutau;
    bool etau;
    bool emu;
    bool mumu;
    bool ee;
    int icutflow;

    ClassDef(HistMaker,0);

  };
}
#endif

#ifdef HISTMAKER_CXX
using namespace CLFV;
void HistMaker::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  printf("HistMaker::%s\n", __func__);
  fTimes[GetTimerNumber("Initialization")] = std::chrono::steady_clock::now();
  fChain = tree;
  if(!fChain) {
    printf("HistMaker::%s: Error! Tree not found to initialize\n", __func__);
    throw 1;
  }

  //setup the branch addresses
  fChain->SetCacheSize(fCacheSize);
  InitializeInputTree(fChain);

  //Load more data into memory to compensate for slow XROOTD processing
  printf("HistMaker::%s Loading baskets\n", __func__);
  if(fLoadBaskets) fChain->LoadBaskets(2.*fCacheSize);
  printf("Total number of entries is %lld\n",fChain->GetEntriesFast());
  IncrementTimer("Initialization", true);
}


Bool_t HistMaker::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef HISTMAKER_CXX
