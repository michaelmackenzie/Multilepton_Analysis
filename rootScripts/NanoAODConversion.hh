#ifndef NanoAODConversion_hh
#define NanoAODConversion_hh

//locally defined objects
#include "ParticleCorrections.hh"
#include "../dataFormats/SlimElectron_t.hh"
#include "../dataFormats/SlimMuon_t.hh"
#include "../dataFormats/SlimTau_t.hh"
#include "../dataFormats/SlimJet_t.hh"
#include "../dataFormats/SlimPhoton_t.hh"

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



class NanoAODConversion : public TSelector {
public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

  enum {kMaxParticles = 30};
  enum {kMuTau, kETau, kEMu, kMuMu, kEE, kSelections}; //selections
  
  // Output data format
  UInt_t runNumber = 0               ;
  ULong64_t eventNumber = 0          ;
  UInt_t lumiSection = 0             ;
  UInt_t nPV  = 0                    ;
  Float_t nPU = 0                    ;
  UInt_t nPartons = 0                ;
  UInt_t mcEra = 0                   ;
  UInt_t triggerLeptonStatus = 0     ;
  Float_t eventWeight = 1.           ;
  Float_t genWeight = 1.             ;
  Float_t puWeight = 1.              ;
  Float_t lepOneWeight = 1.          ;
  Float_t lepTwoWeight = 1.          ;
  Float_t topPtWeight = 1.           ;
  Float_t zPtWeight = 1.             ;
  Float_t genTauFlavorWeight = 1.    ;
  Int_t   tauDecayModeOut = 0        ;
  Float_t tauMVA  = 0                ;
  Int_t tauGenFlavor     = 0         ;
  Int_t tauGenFlavorHad  = 0         ;
  Float_t tauVetoedJetPt = 0         ;
  Float_t tauVetoedJetPtUnc = 0      ;
  TLorentzVector* leptonOneP4 = 0    ;
  TLorentzVector* leptonTwoP4 = 0    ;
  Int_t leptonOneFlavor = 0          ;
  Int_t leptonTwoFlavor = 0          ;
  Float_t leptonOneD0   = 0          ;
  Float_t leptonTwoD0	= 0	     ;
  Float_t leptonOneIso	= 0	     ;
  Float_t leptonTwoIso = 0           ; 
  UChar_t leptonOneID1 = 0           ;
  UChar_t leptonTwoID1 = 0           ;
  UChar_t leptonOneID2 = 0           ;
  UChar_t leptonTwoID2 = 0           ;
  Int_t   leptonOneIndex = 0         ;
  Int_t   leptonTwoIndex = 0         ;
  TLorentzVector* genLeptonOneP4 = 0 ;
  TLorentzVector* genLeptonTwoP4 = 0 ;
  TLorentzVector* photonP4 = 0       ;
  TLorentzVector* jetP4 = 0          ;
  TLorentzVector* tauP4 = 0          ;
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
  SlimTaus_t slimTaus                ;
  SlimJets_t slimJets                ;
  UInt_t nPhotons = 0                ;
  SlimPhotons_t slimPhotons          ;
  UInt_t nJets     = 0               ;
  UInt_t nJets25   = 0               ;
  UInt_t nJets20   = 0               ;
  UInt_t nFwdJets  = 0               ;
  UInt_t nBJetsDeepM  = 0            ;
  UInt_t nBJets       = 0            ;
  UInt_t nBJetsM      = 0            ;
  UInt_t nBJetsL      = 0            ;
  UInt_t nBJets25     = 0            ;
  UInt_t nBJets25M    = 0            ;
  UInt_t nBJets25L    = 0            ;
  UInt_t nBJets20     = 0            ;
  UInt_t nBJets20M    = 0            ;
  UInt_t nBJets20L    = 0            ;
  UInt_t nBJets25Tot  = 0            ;
  UInt_t nBJets25TotM = 0            ;
  UInt_t nBJets25TotL = 0            ;
  UInt_t nBJetsTot    = 0            ;
  UInt_t nBJetsTotM   = 0            ;
  UInt_t nBJetsTotL   = 0            ;
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

  //Input data format (only ones that differ from output)
  Int_t nGoodPV                             ;
  UInt_t nJet                               ;
  UInt_t nMuon                              ;
  UInt_t nElectron                          ;
  UInt_t nTau                               ;
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
  Float_t electronPt[kMaxParticles]         ;
  Float_t electronEta[kMaxParticles]        ;
  Float_t electronPhi[kMaxParticles]        ;
  Float_t electronMass[kMaxParticles]       ;
  Int_t   electronCharge[kMaxParticles]     ;
  Float_t electronDeltaEtaSC[kMaxParticles] ;
  Float_t electronMVAFall17[kMaxParticles]  ;
  Bool_t electronWPL [kMaxParticles]        ;
  Bool_t electronWP80[kMaxParticles]        ;
  Bool_t electronWP90[kMaxParticles]        ;
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
  Int_t   tauDecayMode[kMaxParticles]       ;
  Bool_t  tauIDDecayMode[kMaxParticles]     ;
  UChar_t tauDeep2017VsE[kMaxParticles]     ;
  UChar_t tauDeep2017VsMu[kMaxParticles]    ;
  UChar_t tauDeep2017VsJet[kMaxParticles]   ;
  Float_t jetPt[kMaxParticles]              ;
  Float_t jetEta[kMaxParticles]             ;
  Float_t jetPhi[kMaxParticles]             ;
  Float_t jetMass[kMaxParticles]            ;
  Int_t   jetID[kMaxParticles]              ;
  Int_t   jetPUID[kMaxParticles]            ;
  Float_t jetBTagDeepB[kMaxParticles]       ;
  Float_t jetBTagCMVA[kMaxParticles]        ;
  Float_t photonPt[kMaxParticles]           ;
  Float_t photonEta[kMaxParticles]          ;
  Float_t photonPhi[kMaxParticles]          ;
  Float_t photonMass[kMaxParticles]         ;
  Float_t photonDeltaEta[kMaxParticles]     ;
  Float_t photonMVAID[kMaxParticles]        ;
  Float_t photonMVAID17[kMaxParticles]      ;
  bool    HLT_IsoMu24                       ;
  bool    HLT_IsoMu27                       ;
  bool    HLT_Mu50                          ;
  bool    HLT_Ele27_WPTight_GsF             ;
  bool    HLT_Ele32_WPTight_GsF             ;
  bool    HLT_Ele32_WPTight_GsF_L1DoubleEG  ;
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
  virtual ~NanoAODConversion() { }
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
  virtual void    InitializeTreeVariables(Int_t selection);
  virtual void    CountJets();
  virtual void    CountObjects();
  virtual bool    SelectionID(Int_t selection);
  virtual float   GetTauFakeSF(int genFlavor);
  virtual float   CorrectMET(int selection, float met);
  virtual float   GetZPtWeight(float pt);


  Long64_t fentry; //for tracking entry in functions
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();

  ParticleCorrections* particleCorrections = 0;
  TFile*        fOut;
  TDirectory*   fDirs[kSelections];
  TTree*        fOutTrees[kSelections];
  Float_t       fEventCount = 0.;
  Int_t         fYear = ParticleCorrections::k2016; //data taking year
  Int_t         fMuonIso = ParticleCorrections::kTightMuIso; //which isolation selection to use
  
  TString       fFolderName = ""; //name of the folder the tree is from

  Int_t         fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: e/mu e/mu
  Tree_t        fTreeVars; //for filling the ttrees/mva evaluation

  Int_t         fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
  Int_t         fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
  bool          fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
  Int_t         fMETWeights = 0; //re-weight events based on the MET
  Int_t         fRemoveZPtWeights = 0; // 0 use given weights, 1 remove z pT weight, 2 remove and re-evaluate weights locally

  Int_t         fVerbose = 0;

  Int_t         fSkipMuMuEE = 1; // whether to skip ee/mumu selections for now
  
  //selection requirements
  std::map<Int_t, UChar_t> fMuonIsoSelect;
  std::map<Int_t, Int_t>   fMuonIDSelect; //0 = any, 1 = loose, 2 = medium, 3 = tight
  std::map<Int_t, Int_t>   fElectronIDSelect; //0 = any, 1 = WPL, 2 = WP80, 3 = WP90
  std::map<Int_t, UChar_t> fTauAntiEleSelect;
  std::map<Int_t, UChar_t> fTauAntiMuSelect;
  std::map<Int_t, Bool_t>  fTauIDDecaySelect;

  //for counting objects
  Float_t       fMuonPtCount = 0.;
  Float_t       fElectronPtCount = 0.;
  Float_t       fTauPtCount = 0.;
  //counting by selection
  std::map<Int_t, UChar_t> fMuonIsoCount;
  std::map<Int_t, Int_t>   fMuonIDCount; //0 = any, 1 = loose, 2 = medium, 3 = tight
  std::map<Int_t, Bool_t>  fCountMuons;
  std::map<Int_t, std::map<UInt_t, UInt_t>> fMuonIndices;
  std::map<Int_t, UInt_t>  fNMuons;
  std::map<Int_t, Int_t>   fElectronIDCount; //0 = any, 1 = WPL, 2 = WP80, 3 = WP90
  std::map<Int_t, Bool_t>  fCountElectrons;
  std::map<Int_t, std::map<UInt_t, UInt_t>> fElectronIndices;
  std::map<Int_t, UInt_t>  fNElectrons;
  std::map<Int_t, UChar_t> fTauAntiEleCount;
  std::map<Int_t, UChar_t> fTauAntiMuCount;
  std::map<Int_t, Bool_t>  fTauIDDecayCount;
  std::map<Int_t, Float_t> fTauDeltaRCount; //distance from light lepton
  std::map<Int_t, Bool_t>  fCountTaus;
  std::map<Int_t, std::map<UInt_t, UInt_t>> fTauIndices;
  std::map<Int_t, UInt_t>  fNTaus;
  
  //summary variables
  Int_t         fNEE = 0;
  Int_t         fNMuMu = 0;
  Int_t         fNEMu = 0;
  Int_t         fNETau = 0;
  Int_t         fNMuTau = 0;
  
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
