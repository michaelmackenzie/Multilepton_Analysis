#ifndef HistSelector_h
#define HistSelector_h

#include "interface/BaseSelector.hh"

#include <TEventList.h>
#include <TSystem.h>

#include <TLorentzVector.h>
#include <TStopwatch.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TRandom3.h>
#include <TDirectory.h>
#include <TFolder.h>

// TMVA includes
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

// c++ includes
#include <iostream>

// local includes
#include "interface/GlobalConstants.h"

#include "interface/Tree_t.hh"
#include "interface/EventHist_t.hh"
#include "interface/LepHist_t.hh"
#include "interface/PhotonHist_t.hh"
#include "interface/SystematicHist_t.hh"
#include "interface/Lepton_t.hh"
#include "interface/Jet_t.hh"

// initialize local MVA weight files
#include "interface/TrkQualInit.hh"
#include "interface/MVAConfig.hh"

// Correction objects
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


namespace CLFV {
  using namespace CLFV;

  class HistSelector : public BaseSelector {
  public :

    enum {kMaxLeptons = 10, kMaxParticles = 50, kMaxTriggers = 100, kMaxGenPart = 200};
    enum {kMuTau = 0, kETau = 100, kEMu = 200, kMuTauE = 300, kETauMu = 400, kMuMu = 500, kEE = 600};
    enum {kMaxMVAs = 80};

    //Additional info calculated for input objects

    Float_t Muon_RoccoSF                  [kMaxLeptons]; //calculated momentum scale
    Float_t Muon_ESErr                    [kMaxLeptons]; //momentum scale uncertainty

    Float_t Electron_energyScale          [kMaxLeptons]; //energy scale replacing given one
    Float_t Electron_energyScaleUp        [kMaxLeptons];
    Float_t Electron_energyScaleDown      [kMaxLeptons];

    Float_t Tau_energyScale               [kMaxLeptons]; //calculated enery scale
    Float_t Tau_energyScaleUp             [kMaxLeptons]; //calculated enery scale
    Float_t Tau_energyScaleDown           [kMaxLeptons]; //calculated enery scale

    Float_t jetsRejPt   [kMaxParticles]; //fail PU ID
    Float_t jetsRejEta  [kMaxParticles];
    Float_t jetsPt      [kMaxParticles]; //accepted jets
    Float_t jetsEta     [kMaxParticles];
    Int_t   jetsFlavor  [kMaxParticles];
    Int_t   jetsBTag    [kMaxParticles];

    Float_t htSum                      ;
    Float_t ht                         ;
    Float_t htPhi                      ;

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

    Int_t     nPV                      ;
    Float_t   nPU                      ;
    UInt_t    nPartons                 ;
    UInt_t    runNumber                ;
    ULong64_t eventNumber              ;
    UInt_t    lumiSection              ;
    UChar_t   LHE_Njets                ;

    //Event weights
    Float_t eventWeight = 1.           ;
    Float_t jetPUIDWeight = 1.         ;
    Float_t prefireWeight = 1.         ;
    Float_t prefireWeight_up = 1.      ;
    Float_t prefireWeight_down = 1.    ;
    Float_t topPtWeight = 1.           ;
    Float_t btagWeight = 1.            ;
    Float_t btagWeightUpBC = 1.        ;
    Float_t btagWeightDownBC = 1.      ;
    Float_t btagWeightUpL = 1.         ;
    Float_t btagWeightDownL = 1.       ;
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
    Jet_t           jetOne             ;

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
    Int_t  nGenMuons = 0               ; // counting from gen part list matched to electroweak process in skimming stage
    Int_t  nGenElectrons = 0           ; // counting from gen part list matched to electroweak process in skimming stage
    Int_t  nGenTaus = 0                ; // counting from gen part list matched to electroweak process in skimming stage
    UInt_t nGenHardTaus                ; //for DY splitting by hard process
    UInt_t nGenHardElectrons           ; //for DY splitting by hard process
    UInt_t nGenHardMuons               ; //for DY splitting by hard process

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

    Float_t muon_trig_pt_    ; //lepton trigger thresholds
    Float_t electron_trig_pt_;
    Float_t one_pt_min_ =  0.f; //threshold cuts
    Float_t two_pt_min_ =  0.f;
    Float_t ptdiff_min_ = -1.e10f; //one pt - two pt
    Float_t ptdiff_max_ = +1.e10f; //one pt - two pt
    Float_t met_max_    = -1.f;
    Float_t mtone_max_  = -1.f;
    Float_t mttwo_max_  = -1.f;
    Float_t mtlep_max_  = -1.f;
    Float_t min_mass_   =  0.f;
    Float_t max_mass_   = -1.f;

    HistSelector(int seed = 90, TTree * /*tree*/ = 0);
    virtual ~HistSelector();
    virtual void    Begin(TTree *tree);
    virtual void    Terminate();

    Int_t   Version() const { return 1; }

    virtual Bool_t  Process(Long64_t entry);

    //Apply event selection cuts
    bool    PassesCuts() {
      //apply kinematic cuts
      bool pass(true);
      pass &= leptonOne.pt > one_pt_min_;
      pass &= leptonTwo.pt > two_pt_min_;
      pass &= fTreeVars.ptdiff > ptdiff_min_;
      pass &= fTreeVars.ptdiff < ptdiff_max_;
      pass &= min_mass_  < 0.f || fTreeVars.lepm > min_mass_;
      pass &= max_mass_  < 0.f || fTreeVars.lepm < max_mass_;
      pass &= met_max_   < 0.f || met < met_max_;
      pass &= mtone_max_ < 0.f || fTreeVars.mtone < mtone_max_;
      pass &= mttwo_max_ < 0.f || fTreeVars.mttwo < mttwo_max_;
      pass &= mtlep_max_ < 0.f || fTreeVars.mtlep < mtlep_max_;
      if(!pass) return false;

      //apply trigger cuts
      bool triggered(false);
      triggered |= leptonOne.isMuon    () && leptonOne.matched && leptonOne.pt > muon_trig_pt_    ;
      triggered |= leptonTwo.isMuon    () && leptonTwo.matched && leptonTwo.pt > muon_trig_pt_    ;
      triggered |= leptonOne.isElectron() && leptonOne.matched && leptonOne.pt > electron_trig_pt_;
      triggered |= leptonTwo.isElectron() && leptonTwo.matched && leptonTwo.pt > electron_trig_pt_;
      return triggered;
    }

    virtual TString GetOutputName() {
      return Form("hist%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }
    //Check if this event is being split to a different output/run
    Bool_t SplitSampleEvent() {
      if(fIsData) return kFALSE;
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
      //If testing ee/mumu with embedding, reject ee/mumu events instead
      if(fUseEmbedCuts && !fIsEmbed && !fIsData) {
        if(fDYType != 1 && nGenTaus == 2 && !(fSelection == "ee" || fSelection == "mumu")) {
          return kTRUE;
        } else if(fDYType != 2 && fUseEmbedCuts == 2 && nGenMuons == 2 && fSelection == "mumu") {
          return kTRUE;
        } else if(fDYType != 2 && fUseEmbedCuts == 2 && nGenElectrons == 2 && fSelection == "ee") {
          return kTRUE;
        }
      }
      return kFALSE;
    }

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
        printf(" Timer %15s (%2i): Total processing time %10.3f s, %10i evt, %10.1f Hz\n",
               fTimeNames[itime].Data(), itime, fDurations[itime]/1.e6, fTimeCounts[itime], fTimeCounts[itime]*1.e6/fDurations[itime]);
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
    virtual void    FillLepHistogram(LepHist_t* Hist);

    void            FillBaseEventHistogram(EventHist_t* Hist);
    void            FillBaseLepHistogram(LepHist_t* Hist);

    Bool_t  InitializeEvent(Long64_t entry);
    void    SwapSameFlavor();
    void    InitializeTreeVariables();
    int     Category(TString selection);
    void    InitializeSystematics();

    void    InitializeEventWeights();
    void    EnergyScale(const float scale, Lepton_t& lep, float* MET = nullptr, float* METPhi = nullptr);
    void    EnergyScale(const float scale, TLorentzVector& lv, float* MET = nullptr, float* METPhi = nullptr);
    void    ApplyElectronCorrections();
    void    ApplyMuonCorrections();
    void    ApplyTauCorrections();
    void    CountObjects();
    void    CountJets();
    void    SetKinematics();
    void    EstimateNeutrinos();
    void    EvalMVAs(TString TimerName = "");
    int     GetTriggerMatch(TLorentzVector* lv, bool isMuon, Int_t& trigIndex);
    void    MatchTriggers();
    void    ApplyTriggerWeights();
    void    EvalJetToTauWeights(float& wt, float& wtcorr, float& wtbias);

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
    Int_t fSetFills[fn]; //number of times a histogram set is filled

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
    Int_t           fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: ee/mumu
    Bool_t          fIsDY = false; //for checking if DY --> Z pT weights
    Int_t           fWNJets = -1;  //for splitting w+jets samples

    Int_t           fDoSystematics = 0; //0: ignore systematic histograms 1: fill them -1: only fill them

    Int_t           fWriteTrees = 0; //write out ttrees for the events
    Double_t        fXsec = 0.; //cross-section for full event weight with trees
    Double_t        fLum = 0.; //luminosity full event weight with trees
    Tree_t          fTreeVars; //for filling the ttrees/mva evaluation
    Int_t           fEventCategory; //for identifying the process in mva trainings

    Int_t           fDoHiggs = 1; //include Higgs-centered analysis
    Int_t           fFakeTauIsoCut = 50; //fake tau tight Iso category definition
    Int_t           fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
    Int_t           fIsEmbed = 0; //whether or not this is an embeded sample
    Int_t           fIsLLEmbed = 0; //whether or not this is an ll -> ll embedding (mumu/ee)
    Int_t           fUseEmbedCuts = 0; //whether or not to use the kinematic restrictions for embedded sample generation
    Int_t           fUseEmbedTnPWeights = 1; //whether or not to use the locally calculated lepton ID/trigger weights
    Int_t           fUseEmbedRocco = 1; //whether or not to use Rochester corrections + uncertainties with Embedded muons
    Int_t           fUseRoccoCorr = 1; //use Rochester corrections, 0 = none, 1 = local, 2 = ntuple-level
    bool            fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
    Int_t           fMETWeights = 0; //re-weight events based on the MET
    Int_t           fUseMCEstimatedFakeLep = 0;
    Int_t           fDoTriggerMatching = 1; //match trigger objects to selected leptons

    Bool_t          fSparseHists = false; //only fill/init more basic histograms

    Int_t           fProcessSSSF = 1; //process same-sign, same-flavor data
    Int_t           fDoEventList = 1;

    Float_t         fMaxLepM = -1.f; //for filtering on the ntuple-level di-lepton mass
    Float_t         fMinLepM = -1.f; //for filtering on the ntuple-level di-lepton mass

    Int_t           fSystematicSeed; //for systematic variations

    Systematics     fSystematics; //systematics information

    Int_t           fRemoveTriggerWeights = 0; // 0: do nothing 1: remove weights 2: replace weights
    Int_t           fUpdateMCEra = 0; //update the MC era flag
    Int_t           fRemovePhotonIDWeights = 1;
    Int_t           fUseBTagWeights = 1; //0: do nothing; 1: apply local weights; 2: use ntuple-level weights
    BTagWeight*     fBTagWeight;
    Int_t           fRemovePUWeights = 0; //0: do nothing 1: remove weights 2: replace weights
    PUWeight*       fPUWeight; //object to define pu weights
    RoccoR*         fRoccoR; //Rochester muon momentum corrections
    Int_t           fUseJetPUIDWeights = 1; //0: do nothing; 1: use local jet PU ID weight; 2: use ntuple-level jet PU ID weight
    JetPUWeight*    fJetPUWeight; //object to define jet PU ID weights
    Int_t           fUsePrefireWeights = 1; //use pre-fire weights, 0 = none, 1 = use predefined weights, 2 = replace with local weights
    CLFV::PrefireWeight*  fPrefireWeight; //object to define pre-fire weights
    Int_t           fUseQCDWeights = 1; //use QCD SS --> OS transfer weights
    Int_t           fAddJetTauWeights = 1; //0: do nothing 1: weight anti-iso tau CR data
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
    Bool_t          fUseJetToTauComposition = true;
    Float_t*        fJetToTauMCWts     = new Float_t[JetToTauComposition::kLast]; //weights using MC-based scale factors
    Float_t*        fJetToTauMCCorrs   = new Float_t[JetToTauComposition::kLast];
    Float_t*        fJetToTauMCBiases  = new Float_t[JetToTauComposition::kLast];

    QCDWeight       fQCDWeight; //for emu
    MuonIDWeight    fMuonIDWeight;
    ElectronIDWeight fElectronIDWeight;

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
    int                                   fPrintTime = 2;
    int                                   fPrintFilling = 0; //print histogram filling information

    ///////////////////////////////////////////
    // Histogramming helper fields
    int set_offset;
    bool mutau;
    bool etau;
    bool emu;
    bool mumu;
    bool ee;
    bool etau_mu;
    bool mutau_e;
    int  lep_tau = 0; //flag for processing e+mu as leptonic tau decays

    int icutflow;

    ClassDef(HistSelector,0);
  };
}
#endif