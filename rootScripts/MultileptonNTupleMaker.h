//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 22 11:49:28 2019 by ROOT version 6.06/01
// from TTree tree_ttbar_lep/bltTree
// found on file: /eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190215_135448/output_ttbar_leptonic.root
//////////////////////////////////////////////////////////

#ifndef MultileptonNTupleMaker_h
#define MultileptonNTupleMaker_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
// Headers needed by this particular selector
#include "TStopwatch.h"
#include "TLorentzVector.h"
#include "TRandom.h" //for btag modifier

#include <iostream>

class MultileptonNTupleMaker : public TSelector {
 public :

  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
  TString         fEnd = ""; //allowing one to add something to the output file name
  
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
  TLorentzVector* jet1P4 = 0; //forward jet or highest pt jet
  Float_t jet1D0;
  Float_t jet1Tag;
  Float_t jet1PUID;
  Int_t   jet1Flavor;
  TLorentzVector* jet2P4 = 0; //central jet or second highest pt jet
  Float_t jet2D0;
  Float_t jet2Tag;
  Float_t jet2PUID;
  Int_t   jet2Flavor;
  TLorentzVector* genBJetP4 = 0;
  Float_t genBJetTag;
  TLorentzVector* genJetP4 = 0;
  Float_t genJetTag;
  UInt_t nMuons;
  UInt_t nElectrons;
  UInt_t nJets;
  UInt_t nFwdJets;
  UInt_t nBJets;

  Bool_t jet1tagged;
  Bool_t jet2tagged;

  struct EventNT_t {
    /* UInt_t      runNumber; */
    /* ULong64_t   eventNumber; */
    Int_t       index; //for filling correct tree;
    UInt_t      lumiSection;
    Bool_t      triggerStatus;
    Float_t     eventWeight;
    Float_t     genWeight;
    UInt_t      nPV;
    Float_t     nPU;
    UInt_t      nPartons;
    Float_t     met;
    Float_t     metPhi;
    /* Float_t     leptonOneIso; */
    /* Int_t       leptonOneQ; */
    /* Int_t       leptonOneFlavor; */
    /* Bool_t      leptonOneTrigger; */
    /* Float_t     leptonTwoIso; */
    /* Int_t       leptonTwoQ; */
    /* Int_t       leptonTwoFlavor; */
    /* Bool_t      leptonTwoTrigger; */
    /* Float_t     jetD0; */
    /* Float_t     jetTag; */
    /* Float_t     jetPUID; */
    /* Int_t       jetFlavor; */
    /* Float_t     bjetD0; */
    /* Float_t     bjetTag; */
    /* Float_t     bjetPUID; */
    /* Int_t       bjetFlavor; */
    /* Float_t     genBJetTag; */
    /* Float_t     genJetTag; */
    UInt_t      nMuons;
    UInt_t      nElectrons;
    UInt_t      nJets;
    UInt_t      nFwdJets;
    UInt_t      nBJets;

    Double_t    jetsM;
    Double_t    jetsPt;
    Double_t    jetsEta;
    Double_t    jetsDelPhi;
    Double_t    jetsDelEta;
    Double_t    jetsDelR;

    Double_t    lepM;
    Double_t    lepPt;
    Double_t    lepEta;
    Double_t    lepPhi;
    Double_t    lepDelPhi;
    Double_t    lepDelEta;
    Double_t    lepDelR;
    Double_t    lepPtOverM;

    Double_t    sysM;
    Double_t    sysPt;
    Double_t    sysEta;

    Double_t    jetsLepDelPhi;
    Double_t    jetsLepDelEta;
    Double_t    jetsLepDelR;

    Double_t    jetLepM;
    Double_t    jetLepPt;
    Double_t    jetLepDelPhi;
    Double_t    jetLepDelEta;
    Double_t    jetLepDelR;

    Double_t    bjetLepM;
    Double_t    bjetLepPt;
    Double_t    bjetLepDelPhi;
    Double_t    bjetLepDelEta;
    Double_t    bjetLepDelR;

    Double_t    jetbLepDelPhi; //Angles between jet and bjet+lep system
    Double_t    jetbLepDelEta;
    Double_t    jetbLepDelR;

    Double_t    jet1px       ; 
    Double_t	jet1py       ; 
    Double_t	jet1pz       ; 
    Double_t	jet1pt       ; 
    Double_t	jet1p        ; 
    Double_t	jet1e        ; 
    Double_t	jet1m        ; 
    Double_t	jet1eta      ; 
    Double_t	jet1phi      ; 
    Int_t 	jet1flavor   ; 
    Float_t	jet1d0       ; 
    Float_t	jet1tag      ; 
    Float_t	jet1puid     ;
    Bool_t      jet1tagged   ; //whether or not is tagged as a b-Jet
    	                  
    Double_t	jet2px       ; 
    Double_t	jet2py       ; 
    Double_t	jet2pz       ; 
    Double_t	jet2pt       ; 
    Double_t	jet2p        ; 
    Double_t	jet2e        ; 
    Double_t	jet2m        ; 
    Double_t	jet2eta      ; 
    Double_t	jet2phi      ; 
    Int_t	jet2flavor   ; 
    Float_t	jet2d0       ; 
    Float_t	jet2tag      ; 
    Float_t	jet2puid     ; 
    Bool_t      jet2tagged   ; //whether or not is tagged as a b-Jet
    	                  
    Double_t	lep1px       ; 
    Double_t	lep1py       ; 
    Double_t	lep1pz       ; 
    Double_t	lep1pt       ; 
    Double_t	lep1p        ; 
    Double_t	lep1e        ; 
    Double_t	lep1m        ; 
    Double_t	lep1eta      ; 
    Double_t	lep1phi      ; 
    Int_t	lep1flavor   ; 
    Float_t	lep1iso      ; 
    Double_t	lep1reliso   ; 
    Int_t	lep1q        ; 
    Bool_t	lep1trigger  ; 
    	                  
    Double_t	lep2px       ; 
    Double_t	lep2py       ; 
    Double_t	lep2pz       ; 
    Double_t	lep2pt       ; 
    Double_t	lep2p        ; 
    Double_t	lep2e        ; 
    Double_t	lep2m        ; 
    Double_t	lep2eta      ; 
    Double_t	lep2phi      ; 
    Int_t	lep2flavor   ; 
    Float_t	lep2iso      ; 
    Double_t	lep2reliso   ; 
    Int_t	lep2q        ; 
    Bool_t	lep2trigger  ;   
    
  };



  
  MultileptonNTupleMaker(TTree * /*tree*/ =0) { }
  virtual ~MultileptonNTupleMaker() { }
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

  virtual bool    BTagModifier(int i);
  virtual void    BookTrees();
  virtual void    BookEventTrees();
  virtual void    FillEventTree(EventNT_t* tree);


  TRandom* _rng = new TRandom(90); // for b-tagging
  TStopwatch* timer = new TStopwatch();
  //Tree:
  const static Int_t fn = 100; //max histogram sets

  Int_t fEventSets[fn]; //indicates which sets to create

  TFile*       fOut;
  TDirectory*  fTopDir;
  TDirectory*  fDirectories[4*fn];
  EventNT_t* fEventNT[fn];

  TTree*   fEventTree[fn];

  ClassDef(MultileptonNTupleMaker,0);

};

#endif

#ifdef MultileptonNTupleMaker_cxx
void MultileptonNTupleMaker::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  if(fChain == 0 && tree != 0) {
    fOut = new TFile(Form("%s%s.tree",tree->GetName(),fEnd.Data()),"RECREATE","MultileptonNTupleMaker output tree file");
    fTopDir = fOut->mkdir("Data");
    fTopDir->cd();

    for(int i = 0; i < fn; ++i) {
      fEventSets[i] = 0;
    }

    //Event Sets
    fEventSets[0] = 1; // all events
    fEventSets[1] = 1; // Lepton Sys Mass < 70 GeV/c^2
    //1b1f = mass requirement + NFwdJets > 0 + nJets == 1
    //1b1c = mass requirement + NFwdJets == 0 + nJets >= 2
    fEventSets[2] = 1; // loose 1b1c
    fEventSets[3] = 1; // loose 1b1f
    fEventSets[4] = 1; // loose 1b1f || 1b1c
    fEventSets[5] = 1; // loose 1b1c + btag
    fEventSets[6] = 1; // loose 1b1f + btag
    BookTrees();

  }
  if(tree != 0) fChain = tree;
  printf("MultileptonNTupleMaker::Init fChain = tree ");
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
  fChain->SetBranchAddress("jetP4",&jet1P4);
  fChain->SetBranchAddress("jetD0",&jet1D0);
  fChain->SetBranchAddress("jetTag",&jet1Tag);
  fChain->SetBranchAddress("jetPUID",&jet1PUID);
  fChain->SetBranchAddress("jetFlavor",&jet1Flavor);
  fChain->SetBranchAddress("bjetP4",&jet2P4);
  fChain->SetBranchAddress("bjetD0",&jet2D0);
  fChain->SetBranchAddress("bjetTag",&jet2Tag);
  fChain->SetBranchAddress("bjetPUID",&jet2PUID);
  fChain->SetBranchAddress("bjetFlavor",&jet2Flavor);
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

Bool_t MultileptonNTupleMaker::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef MultileptonNTupleMaker_cxx
