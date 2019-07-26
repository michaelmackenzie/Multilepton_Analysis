//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 22 11:49:28 2019 by ROOT version 6.06/01
// from TTree tree_ttbar_lep/bltTree
// found on file: /eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190215_135448/output_ttbar_leptonic.root
//////////////////////////////////////////////////////////

#ifndef MultileptonHistMaker_hh
#define MultileptonHistMaker_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
// Headers needed by this particular selector
#include "TStopwatch.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"


#include <iostream>

class MultileptonHistMaker : public TSelector {
 public :

  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

  // Readers to access the data (delete the ones you do not need).
  UInt_t runNumber;
  ULong64_t eventNumber;
  UInt_t lumiSection;
  Bool_t triggerStatus;
  Float_t eventWeight;
  Float_t genWeight;
  UInt_t nPV;
  Float_t nPU;
  UInt_t nPartons;
  Float_t met;
  Float_t metPhi;
  TLorentzVector* leptonOneP4 = 0;
  Float_t leptonOneIso;
  Int_t leptonOneQ;
  Int_t leptonOneFlavor;
  Bool_t leptonOneTrigger;
  TLorentzVector* leptonTwoP4 = 0;
  Float_t leptonTwoIso;
  Int_t leptonTwoQ;
  Int_t leptonTwoFlavor;
  Bool_t leptonTwoTrigger;
  TLorentzVector* jetP4 = 0;
  Float_t jetD0;
  Float_t jetTag;
  Float_t jetPUID;
  Int_t jetFlavor;
  TLorentzVector* bjetP4 = 0;
  Float_t bjetD0;
  Float_t bjetTag;
  Float_t bjetPUID;
  Int_t bjetFlavor;
  TLorentzVector* genBJetP4 = 0;
  Float_t genBJetTag;
  TLorentzVector* genJetP4 = 0;
  Float_t genJetTag;
  UInt_t nMuons;
  UInt_t nElectrons;
  UInt_t nJets;
  UInt_t nFwdJets;
  UInt_t nBJets;

  struct EventHist_t {
    TH1F* hLumiSection;
    TH1F* hTriggerStatus;
    TH1F* hEventWeight;
    TH1F* hGenWeight;
    TH1F* hNPV;
    TH1F* hNPU;
    TH1F* hNPartons;
    TH1F* hMet;
    TH1F* hMetPhi;
    TH1F* hNMuons;
    TH1F* hNElectrons;
    TH1F* hNJets;
    TH1F* hNFwdJets;
    TH1F* hNBJets;

    TH1F* hJetsM;
    TH1F* hJetsPt;
    TH1F* hJetsEta;
    TH1F* hJetsDelPhi;
    TH1F* hJetsDelEta;
    TH1F* hJetsDelR;
    TH2F* hJetsDelRVsPhi;
    
    TH1F* hLepPt;
    TH1F* hLepP;
    TH1F* hLepE;
    TH1F* hLepM;
    TH1F* hLepEta;
    TH1F* hLepPhi;
    TH1F* hLepDeltaPhi;
    TH1F* hLepDeltaEta;
    TH1F* hLepDeltaR;
    TH2F* hLepDelRVsPhi;
    TH1F* hLepPtOverM;

    TH1F* hSysM;
    TH1F* hSysPt;
    TH1F* hSysEta;
    TH1F* hJetsLepDelPhi;
    TH1F* hJetsLepDelEta;
    TH1F* hJetsLepDelR;
    TH2F* hJetsLepDelRVsPhi;
    TH1F* hJetLepDelPhi;
    TH1F* hJetLepDelEta;
    TH1F* hJetLepDelR;
    TH2F* hJetLepDelRVsPhi;

    TH1F* hBJetLepM;
    TH1F* hBJetLepPt;
    TH1F* hBJetLepDelPhi;
    TH1F* hBJetLepDelEta;
    TH1F* hBJetLepDelR;     
    TH2F* hBJetLepDelRVsPhi;     

    TH2F* hBJetLepMVsLepM;
  };

  struct JetHist_t {
    TH1F* hPx;
    TH1F* hPy;
    TH1F* hPz;
    TH1F* hPt;
    TH1F* hP;
    TH1F* hE;
    TH1F* hM;
    TH1F* hEta;
    TH1F* hPhi;
    TH1F* hFlavor;
    TH1F* hJetD0;
    TH1F* hJetTag;
    TH1F* hJetPUID;    

  };

  struct LepHist_t {
    TH1F* hOnePx;
    TH1F* hOnePy;
    TH1F* hOnePz;
    TH1F* hOnePt;
    TH1F* hOneP;
    TH1F* hOneE;
    TH1F* hOneM;
    TH1F* hOneEta;
    TH1F* hOnePhi;
    TH1F* hOneIso;
    TH1F* hOneRelIso;
    TH1F* hOneFlavor;
    TH1F* hOneQ;
    TH1F* hOneTrigger;
    TH1F* hTwoPx;
    TH1F* hTwoPy;
    TH1F* hTwoPz;
    TH1F* hTwoPt;
    TH1F* hTwoP;
    TH1F* hTwoE;
    TH1F* hTwoM;
    TH1F* hTwoEta;
    TH1F* hTwoPhi;
    TH1F* hTwoIso;
    TH1F* hTwoRelIso;
    TH1F* hTwoFlavor;
    TH1F* hTwoQ;
    TH1F* hTwoTrigger;
  };


  
  MultileptonHistMaker(TTree * /*tree*/ =0) { }
  virtual ~MultileptonHistMaker() { }
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
  virtual void    BookJetHistograms();
  virtual void    BookBJetHistograms();
  virtual void    BookLepHistograms();
  virtual void    FillEventHistogram(EventHist_t* Hist);
  virtual void    FillJetHistogram(JetHist_t* Hist);
  virtual void    FillBJetHistogram(JetHist_t* Hist);
  virtual void    FillLepHistogram(LepHist_t* Hist);

  //IDWord Definition
  class TEventID {
  private:
    MultileptonHistMaker* const fOwner;
  public:
    enum {
      kPtOverM        = 0x1 <<  0,
      kMet            = 0x1 <<  1,
      kJetsLepDPhi    = 0x1 <<  2,
      kJetsLepDR      = 0x1 <<  3,
      kBJetLepDR      = 0x1 <<  4,
      kBJetLepDPhi    = 0x1 <<  5,
      kLeadingPt      = 0x1 <<  6,
      kTrailingPt     = 0x1 <<  7,
      kLepEta         = 0x1 <<  8,
      kLeadingIso     = 0x1 <<  9,
      kTrailingIso    = 0x1 << 10
    };
  
    Float_t fMinPtOverM        = -1.e6;
    Float_t fMaxPtOverM        =  1.e6;
    Float_t fMinMet            = -1.e6;
    Float_t fMaxMet            =  1.e6;
    Float_t fMinJetsLepDPhi    = -1.e6;
    Float_t fMaxJetsLepDPhi    =  1.e6;
    Float_t fMinJetsLepDR      = -1.e6;
    Float_t fMaxJetsLepDR      =  1.e6;
    Float_t fMinBJetLepDPhi    = -1.e6;
    Float_t fMaxBJetLepDPhi    =  1.e6;
    Float_t fMinBJetLepDR      = -1.e6;
    Float_t fMaxBJetLepDR      =  1.e6;
    Float_t fMinLeadingPt      = -1.e6;
    Float_t fMaxLeadingPt      =  1.e6;
    Float_t fMinTrailingPt     = -1.e6;
    Float_t fMaxTrailingPt     =  1.e6;
    Float_t fMinLepEta         = -1.e6;
    Float_t fMaxLepEta         =  1.e6;
    Float_t fMinLeadingIso     = -1.e6;
    Float_t fMaxLeadingIso     =  1.e6;
    Float_t fMinTrailingIso    = -1.e6;
    Float_t fMaxTrailingIso    =  1.e6;

    Int_t   fUseMask;

    TEventID(MultileptonHistMaker & Owner);
    virtual ~TEventID();

    //Accessors
    Float_t MinPtOverM      () const { return fMinPtOverM     ; };
    Float_t MaxPtOverM      () const { return fMaxPtOverM     ; };
    Float_t MinMet          () const { return fMinMet         ; };
    Float_t MaxMet          () const { return fMaxMet         ; };
    Float_t MinJetsLepDPhi  () const { return fMinJetsLepDPhi ; };
    Float_t MaxJetsLepDPhi  () const { return fMaxJetsLepDPhi ; };
    Float_t MinJetsLepDR    () const { return fMinJetsLepDR   ; };
    Float_t MaxJetsLepDR    () const { return fMaxJetsLepDR   ; };
    Float_t MinBJetLepDPhi  () const { return fMinBJetLepDPhi ; };
    Float_t MaxBJetLepDPhi  () const { return fMaxBJetLepDPhi ; };
    Float_t MinBJetLepDR    () const { return fMinBJetLepDR   ; };
    Float_t MaxBJetLepDR    () const { return fMaxBJetLepDR   ; };
    Float_t MinLeadingPt    () const { return fMinLeadingPt   ; };
    Float_t MaxLeadingPt    () const { return fMaxLeadingPt   ; };
    Float_t MinTrailingPt   () const { return fMinTrailingPt  ; };
    Float_t MaxTrailingPt   () const { return fMaxTrailingPt  ; };
    Float_t MinLepEta       () const { return fMinLepEta      ; };
    Float_t MaxLepEta       () const { return fMaxLepEta      ; };
    Float_t MinLeadingIso   () const { return fMinLeadingIso  ; };
    Float_t MaxLeadingIso   () const { return fMaxLeadingIso  ; };
    Float_t MinTrailingIso  () const { return fMinTrailingIso ; };
    Float_t MaxTrailingIso  () const { return fMaxTrailingIso ; };

    //Modifiers
    void SetMinPtOverM      (Float_t MinPtOverM     ) { fMinPtOverM     = MinPtOverM     ; };
    void SetMaxPtOverM      (Float_t MaxPtOverM     ) { fMaxPtOverM     = MaxPtOverM     ; };
    void SetMinMet          (Float_t MinMet         ) { fMinMet         = MinMet         ; };
    void SetMaxMet          (Float_t MaxMet         ) { fMaxMet         = MaxMet         ; };
    void SetMinJetsLepDPhi  (Float_t MinJetsLepDPhi ) { fMinJetsLepDPhi = MinJetsLepDPhi ; };
    void SetMaxJetsLepDPhi  (Float_t MaxJetsLepDPhi ) { fMaxJetsLepDPhi = MaxJetsLepDPhi ; };
    void SetMinJetsLepDR    (Float_t MinJetsLepDR   ) { fMinJetsLepDR   = MinJetsLepDR   ; };
    void SetMaxJetsLepDR    (Float_t MaxJetsLepDR   ) { fMaxJetsLepDR   = MaxJetsLepDR   ; };
    void SetMinBJetLepDPhi  (Float_t MinBJetLepDPhi ) { fMinBJetLepDPhi = MinBJetLepDPhi ; };
    void SetMaxBJetLepDPhi  (Float_t MaxBJetLepDPhi ) { fMaxBJetLepDPhi = MaxBJetLepDPhi ; };
    void SetMinBJetLepDR    (Float_t MinBJetLepDR   ) { fMinBJetLepDR   = MinBJetLepDR   ; };
    void SetMaxBJetLepDR    (Float_t MaxBJetLepDR   ) { fMaxBJetLepDR   = MaxBJetLepDR   ; };
    void SetMinLeadingPt    (Float_t MinLeadingPt   ) { fMinLeadingPt   = MinLeadingPt   ; };
    void SetMaxLeadingPt    (Float_t MaxLeadingPt   ) { fMaxLeadingPt   = MaxLeadingPt   ; };
    void SetMinTrailingPt   (Float_t MinTrailingPt  ) { fMinTrailingPt  = MinTrailingPt  ; };
    void SetMaxTrailingPt   (Float_t MaxTrailingPt  ) { fMaxTrailingPt  = MaxTrailingPt  ; };
    void SetMinLepEta       (Float_t MinLepEta      ) { fMinLepEta      = MinLepEta      ; };
    void SetMaxLepEta       (Float_t MaxLepEta      ) { fMaxLepEta      = MaxLepEta      ; };
    void SetMinLeadingIso   (Float_t MinLeadingIso  ) { fMinLeadingIso  = MinLeadingIso  ; };
    void SetMaxLeadingIso   (Float_t MaxLeadingIso  ) { fMaxLeadingIso  = MaxLeadingIso  ; };
    void SetMinTrailingIso  (Float_t MinTrailingIso ) { fMinTrailingIso = MinTrailingIso ; };
    void SetMaxTrailingIso  (Float_t MaxTrailingIso ) { fMaxTrailingIso = MaxTrailingIso ; };

    virtual Int_t IDWord () ;

  };


  TStopwatch* timer = new TStopwatch();
  //Histograms:
  const static Int_t fn = 100; //max histogram sets

  Int_t fEventSets[fn]; //indicates which sets to create
  Int_t fJetSets[fn]; //indicates which sets to create
  Int_t fBJetSets[fn]; //indicates which sets to create
  Int_t fLepSets[fn]; //indicates which sets to create

  TFile*       fOut;
  TDirectory*  fTopDir;
  TDirectory*  fDirectories[4*fn];
  EventHist_t* fEventHist[fn];
  JetHist_t*   fJetHist[fn];
  JetHist_t*   fBJetHist[fn];
  LepHist_t*   fLepHist[fn];

  ClassDef(MultileptonHistMaker,0);

};

#endif

#ifdef MultileptonHistMaker_cxx
void MultileptonHistMaker::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  if(fChain == 0 && tree != 0) {
    fOut = new TFile(Form("%s.hist",tree->GetName()),"RECREATE","MultileptonHistMaker output histogram file");
    fTopDir = fOut->mkdir("Data");
    fTopDir->cd();

    for(int i = 0; i < fn; ++i) {
      fEventSets[i] = 0;
      fJetSets[i]   = 0;
      fBJetSets[i]  = 0;
      fLepSets[i]   = 0;
    }

    //Event Sets
    fEventSets[0] = 1; // all events
    fEventSets[1] = 1; // 12 < Lepton Sys Mass < 70 GeV/c^2
    //1b1f = mass requirement + NFwdJets > 0 + nJets == 0 + nBJets == 1
    //1b1c = mass requirement + NFwdJets == 0 + (nJets + nBJets) == 2 + met < 40 + deltaphi(leps,jets) > 2.5
    //       + nBJets > 0
    fEventSets[2] = 1; // 1b1c
    fEventSets[3] = 1; // 1b1f
    fEventSets[4] = 1; // 1b1f || 1b1c
    fEventSets[5] = 1; // (1b1f || 1b1c) && Lepton Pt / M > 2.
    fEventSets[6] = 1; // (1b1c) && Lepton Pt / M > 2.
    fEventSets[7] = 1; // (1b1f) && Lepton Pt / M > 2.

    fEventSets[8]  = 1; // exactly 1 central jet + 1 bjet and no forward jets and 12 < M_mumu < 70
    fEventSets[9]  = 1; // at least 1 forward jet + 1 bjet and no other central jets  and 12 < M_mumu < 70
    fEventSets[10] = 1; // Central Jet no Pt/M Box Cut Defining Folder
    fEventSets[11] = 1; // Forward Jet no Pt/M Box Cut Defining Folder
    fEventSets[12] = 1; // Combined no Pt/M Box Cut Defining Folder

    fEventSets[14] = 1; // Central Jet Pt/M Box Cut Defining Folder
    fEventSets[15] = 1; // Forward Jet Pt/M Box Cut Defining Folder
    fEventSets[16] = 1; // Combined Pt/M Box Cut Defining Folder

    fEventSets[20] = 1; // Central Jet Box Cut Defining Folder
    fEventSets[21] = 1; // Forward Jet Box Cut Defining Folder
    fEventSets[22] = 1; // Combined Box Cut Defining Folder

    //Studying Central Events Failing
    fEventSets[30] = 1;
    fEventSets[31] = 1;
    fEventSets[32] = 1;
    fEventSets[33] = 1;
    fEventSets[34] = 1;
    fEventSets[35] = 1;
    fEventSets[36] = 1;
    fEventSets[37] = 1;
    fEventSets[38] = 1;
    fEventSets[39] = 1;

    //Studying Forward Events Failing
    fEventSets[40] = 1;
    fEventSets[41] = 1;
    fEventSets[42] = 1;
    fEventSets[43] = 1;
    fEventSets[44] = 1;
    fEventSets[45] = 1;
    fEventSets[46] = 1;
    fEventSets[47] = 1;
    fEventSets[48] = 1;
    fEventSets[49] = 1;


    //Jet Sets
    fJetSets[0] = 1; // all events
    fJetSets[1] = 1; // 12 < Lepton Sys Mass < 70 GeV/c^2
    fJetSets[8]  = 1; // exactly 1 central jet + 1 bjet and no forward jets and 12 < M_mumu < 70
    fJetSets[9]  = 1; // Forward selection

    fJetSets[10] = 1; // Central Jet no Pt/M Box Cut Defining Folder
    fJetSets[11] = 1; // Forward Jet no Pt/M Box Cut Defining Folder
    fJetSets[12] = 1; // Combined no Pt/M Box Cut Defining Folder

    fJetSets[14] = 1; // Central Jet Pt/M Box Cut Defining Folder
    fJetSets[15] = 1; // Forward Jet Pt/M Box Cut Defining Folder
    fJetSets[16] = 1; // Combined Pt/M Box Cut Defining Folder

    fJetSets[20] = 1; // Central Jet Box Cut Defining Folder
    fJetSets[21] = 1; // Forward Jet Box Cut Defining Folder
    fJetSets[22] = 1; // Combined Box Cut Defining Folder

    //Jet Sets
    fBJetSets[0] = 1; // all events
    fBJetSets[1] = 1; // 12 < Lepton Sys Mass < 70 GeV/c^2
    fBJetSets[8]  = 1; // exactly 1 central jet + 1 bjet and no forward jets and 12 < M_mumu < 70
    fBJetSets[9]  = 1; // Forward selection

    fBJetSets[10] = 1; // Central Jet no Pt/M Box Cut Defining Folder
    fBJetSets[11] = 1; // Forward Jet no Pt/M Box Cut Defining Folder
    fBJetSets[12] = 1; // Combined no Pt/M Box Cut Defining Folder

    fBJetSets[14] = 1; // Central Jet Pt/M Box Cut Defining Folder
    fBJetSets[15] = 1; // Forward Jet Pt/M Box Cut Defining Folder
    fBJetSets[16] = 1; // Combined Pt/M Box Cut Defining Folder

    fBJetSets[20] = 1; // Central Jet Box Cut Defining Folder
    fBJetSets[21] = 1; // Forward Jet Box Cut Defining Folder
    fBJetSets[22] = 1; // Combined Box Cut Defining Folder
 
 
    //Lepton Sets
    fLepSets[0] = 1; // all events
    fLepSets[1] = 1; // E > 0
    fLepSets[8]  = 1; // exactly 1 central jet + 1 bjet and no forward jets and 12 < M_mumu < 70
    fLepSets[9]  = 1; // Forward selection

    fLepSets[10] = 1; // Central Jet no Pt/M Box Cut Defining Folder
    fLepSets[11] = 1; // Forward Jet no Pt/M Box Cut Defining Folder
    fLepSets[12] = 1; // Combined no Pt/M Box Cut Defining Folder

    fLepSets[14] = 1; // Central Jet Pt/M Box Cut Defining Folder
    fLepSets[15] = 1; // Forward Jet Pt/M Box Cut Defining Folder
    fLepSets[16] = 1; // Combined Pt/M Box Cut Defining Folder

    fLepSets[20] = 1; // Central Jet Box Cut Defining Folder
    fLepSets[21] = 1; // Forward Jet Box Cut Defining Folder
    fLepSets[22] = 1; // Combined Box Cut Defining Folder

    BookHistograms();

  }
  if(tree != 0) fChain = tree;
  printf("MultileptonHistMaker::Init fChain = tree ");
  std::cout << tree << std::endl;
  
  fChain->SetBranchAddress("runNumber",&runNumber);
  fChain->SetBranchAddress("evtNumber",&eventNumber);
  fChain->SetBranchAddress("lumiSection",&lumiSection);
  fChain->SetBranchAddress("triggerStatus",&triggerStatus);
  fChain->SetBranchAddress("eventWeight",&eventWeight);
  fChain->SetBranchAddress("genWeight",&genWeight);
  fChain->SetBranchAddress("nPV",&nPV);
  fChain->SetBranchAddress("nPU",&nPU);
  fChain->SetBranchAddress("nPartons",&nPartons);
  fChain->SetBranchAddress("met",&met);
  fChain->SetBranchAddress("metPhi",&metPhi);
  fChain->SetBranchAddress("leptonOneP4",&leptonOneP4);
  fChain->SetBranchAddress("leptonOneIso",&leptonOneIso);
  fChain->SetBranchAddress("leptonOneQ",&leptonOneQ);
  fChain->SetBranchAddress("leptonOneFlavor",&leptonOneFlavor);
  fChain->SetBranchAddress("leptonOneTrigger",&leptonOneTrigger);
  fChain->SetBranchAddress("leptonTwoP4",&leptonTwoP4);
  fChain->SetBranchAddress("leptonTwoIso",&leptonTwoIso);
  fChain->SetBranchAddress("leptonTwoQ",&leptonTwoQ);
  fChain->SetBranchAddress("leptonTwoFlavor",&leptonTwoFlavor);
  fChain->SetBranchAddress("leptonTwoTrigger",&leptonTwoTrigger);
  fChain->SetBranchAddress("jetP4",&jetP4);
  fChain->SetBranchAddress("jetD0",&jetD0);
  fChain->SetBranchAddress("jetTag",&jetTag);
  fChain->SetBranchAddress("jetPUID",&jetPUID);
  fChain->SetBranchAddress("jetFlavor",&jetFlavor);
  fChain->SetBranchAddress("bjetP4",&bjetP4);
  fChain->SetBranchAddress("bjetD0",&bjetD0);
  fChain->SetBranchAddress("bjetTag",&bjetTag);
  fChain->SetBranchAddress("bjetPUID",&bjetPUID);
  fChain->SetBranchAddress("bjetFlavor",&bjetFlavor);
  fChain->SetBranchAddress("genBJetP4",&genBJetP4);
  fChain->SetBranchAddress("genBJetTag",&genBJetTag);
  fChain->SetBranchAddress("genJetP4",&genJetP4);
  fChain->SetBranchAddress("genJetTag",&genJetTag);
  fChain->SetBranchAddress("nMuons",&nMuons);
  fChain->SetBranchAddress("nElectrons",&nElectrons);
  fChain->SetBranchAddress("nJets",&nJets);
  fChain->SetBranchAddress("nFwdJets",&nFwdJets);
  fChain->SetBranchAddress("nBJets",&nBJets);


  fReader.SetTree(tree);

  printf("Total number of entries is %lld\n",fChain->GetEntriesFast());
}

Bool_t MultileptonHistMaker::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef MultileptonHistMaker_cxx
