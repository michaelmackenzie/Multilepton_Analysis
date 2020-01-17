//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Sep 10 09:06:07 2019 by ROOT version 6.06/01
// from TTree bltTree_ttbar_inclusive/bltTree_ttbar_inclusive
// found on file: /eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/output_ttbar_inclusive.root
//////////////////////////////////////////////////////////

#ifndef ZTauTauHistMaker_hh
#define ZTauTauHistMaker_hh

// // Analysis tools
// #include "BLT/BLTAnalysis/interface/BLTSelector.hh"
// #include "BLT/BLTAnalysis/interface/BLTHelper.hh"
// #include "BLT/BLTAnalysis/interface/Parameters.hh"
// #include "BLT/BLTAnalysis/interface/Cuts.hh"
// #include "BLT/BLTAnalysis/interface/TriggerSelector.hh"
// #include "BLT/BLTAnalysis/interface/ParticleSelector.hh"
// #include "BLT/BLTAnalysis/interface/WeightUtils.h"
// #include "BLT/BLTAnalysis/interface/ElectronCorrector.h"

// #include "BLT/BLTAnalysis/interface/RoccoR.h"

// // BaconAna class definitions (might need to add more)
// #include "BaconAna/Utils/interface/TTrigger.hh"
// #include "BaconAna/DataFormats/interface/TLHEWeight.hh"

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

// TMVA includes
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

// Headers for ditau vertex mass
// #include "TauAnalysis/ClassicSVfit/interface/ClassicSVfit.h"
// #include "TauAnalysis/ClassicSVfit/interface/MeasuredTauLepton.h"
// #include "TauAnalysis/ClassicSVfit/interface/svFitHistogramAdapter.h"

#include <iostream>


class ZTauTauHistMaker : public TSelector {
public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

  // Readers to access the data (delete the ones you do not need).
  UInt_t runNumber                   ;
  ULong64_t eventNumber              ;
  UInt_t lumiSection                 ;
  UInt_t nPV                         ;
  Float_t nPU                        ;
  UInt_t mcEra                       ;
  UInt_t triggerLeptonStatus         ;
  Float_t eventWeight                ;
  Float_t genWeight                  ;
  Float_t puWeight                   ;
  Float_t topPtWeight                ;
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
  TLorentzVector* genLeptonOneP4 = 0 ;
  TLorentzVector* genLeptonTwoP4 = 0 ;
  TLorentzVector* photonP4 = 0       ;
  UInt_t nMuons                      ;
  UInt_t nElectrons                  ;
  UInt_t nTaus                       ;
  UInt_t nPhotons                    ;
  UInt_t nJets                       ;
  UInt_t nBJets                      ;
  UInt_t nGenTausHad                 ;
  UInt_t nGenTausLep                 ;
  UInt_t nGenElectrons               ;
  UInt_t nGenMuons                   ;
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
    TH1F* hNPV;
    TH1F* hNPU;
    TH1F* hNPartons;
    TH1F* hNMuons;
    TH1F* hNElectrons;
    TH1F* hNTaus;
    TH1F* hNPhotons;
    TH1F* hNGenTausHad;
    TH1F* hNGenTausLep;
    TH1F* hNGenTaus;
    TH1F* hNGenElectrons;
    TH1F* hNGenMuons;
    TH1F* hNJets;
    TH1F* hNFwdJets;
    TH1F* hNBJets;
    TH1F* hMcEra;
    TH1F* hTriggerLeptonStatus;
    TH1F* hPuWeight;
    TH1F* hTopPtWeight;
    TH1F* hTauDecayMode;
    TH1F* hTauMVA;
    TH1F* hTauGenFlavor;
    TH1F* hTauGenFlavorHad;
    TH1F* hTauVetoedJetPt;
    TH1F* hTauVetoedJetPtUnc;
    TH1F* hHtSum;
    TH1F* hHt;
    TH1F* hHtPhi;
    TH1F* hPFMet;
    TH1F* hPFMetPhi;
    TH1F* hPFCovMet00;
    TH1F* hPFCovMet01;
    TH1F* hPFCovMet11;
    TH1F* hPuppMet;
    TH1F* hPuppMetPhi;
    TH1F* hPuppCovMet00;
    TH1F* hPuppCovMet01;
    TH1F* hPuppCovMet11;
    TH1F* hTrkMet;
    TH1F* hTrkMetPhi;

    TH1F* hMet;
    TH1F* hMetPhi;
    TH1F* hMetCorr;
    TH1F* hMetCorrPhi;
    TH1F* hCovMet00;
    TH1F* hCovMet01;
    TH1F* hCovMet11;
    TH1F* hMassSVFit;
    TH1F* hMassErrSVFit;
    TH1F* hSVFitStatus;
    TH2F* hMetVsPt;
    
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
    TH1F* hHtDeltaPhi;
    TH1F* hMetDeltaPhi;
    TH1F* hLepOneDeltaPhi;
    TH1F* hLepTwoDeltaPhi;

    TH1F* hLepSVPt;
    TH1F* hLepSVP;
    TH1F* hLepSVE;
    TH1F* hLepSVM;
    TH1F* hLepSVEta;
    TH1F* hLepSVPhi;
    TH1F* hLepSVDeltaPhi;
    TH1F* hLepSVDeltaEta;
    TH1F* hLepSVDeltaR;
    TH2F* hLepSVDelRVsPhi;
    TH1F* hLepSVPtOverM;
    
    TH1F* hSysM;
    TH1F* hSysPt;
    TH1F* hSysEta;

    TH1F* hSysSVM;
    TH1F* hSysSVPt;
    TH1F* hSysSVEta;
    
    //Transverse Masses
    TH1F* hMTMu;
    TH1F* hMTE;
    TH1F* hMTTau;

    //mass difference between m_gll and m_ll (with a weight)
    TH1F* hMDiff;

    //three sets for combining photon with a lepton or leptons vs photon
    TH1F* hPXiVis[3];
    TH1F* hPXiInv[3];
    TH1F* hPXiVisOverInv[3];
    TH2F* hPXiInvVsVis[3];
    TH1F* hPXiDiff[3];
    TH1F* hPXiDiff2[3];//difference with coeffecients and offset

    TH1F* hPtSum[2]; //scalar sum of lepton Pt and Met, and photon for one
    TH1F* hPt1Sum[4]; //scalar sum of 1 lepton Pt and Met, both leptons, then both minus met
    //MVA values
    TH1F* hMVA[10];
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
    TH1F* hOneD0;
    TH1F* hOneIso;
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
    TH1F* hOneSVP;
    TH1F* hOneSVE;
    TH1F* hOneSVM;    
    TH1F* hOneSVEta;
    TH1F* hOneSVDeltaPt;
    TH1F* hOneSVDeltaP;
    TH1F* hOneSVDeltaE;
    TH1F* hOneSVDeltaEta;

    TH1F* hTwoPx;
    TH1F* hTwoPy;
    TH1F* hTwoPz;
    TH1F* hTwoPt;
    TH1F* hTwoP;
    TH1F* hTwoE;
    TH1F* hTwoM;
    TH1F* hTwoEta;
    TH1F* hTwoPhi;
    TH1F* hTwoD0;
    TH1F* hTwoIso;
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
    TH1F* hTwoSVP;
    TH1F* hTwoSVE;
    TH1F* hTwoSVM;    
    TH1F* hTwoSVEta;
    TH1F* hTwoSVDeltaPt;
    TH1F* hTwoSVDeltaP;
    TH1F* hTwoSVDeltaE;
    TH1F* hTwoSVDeltaEta;
  };

  struct PhotonHist_t {
    TH1F* hPx;
    TH1F* hPy;
    TH1F* hPz;
    TH1F* hPt;
    TH1F* hP;
    TH1F* hE;
    TH1F* hEta;
    TH1F* hPhi;
    TH1F* hIso;
    TH1F* hRelIso;
    TH1F* hTrigger;
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
    float  njets;
    float  nbjets;
    float  nphotons;
    float eventweight;
    float fulleventweight; //includes cross-section and number gen
    float eventcategory; //for identifying the process in mva trainings
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
  virtual void    InitializeTreeVariables();
  virtual float   GetTauFakeSF(int genFlavor);


  Long64_t fentry; //for tracking entry in functions
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();
  TMVA::Reader* mva; //read and apply mva weight files
  vector<TString> fMvaNames = { //mva names for getting weights
    "mutau_MLP_MM_8","mutau_BDT_8",
    "mutau_DY_MLP_MM_8","mutau_WJets_MLP_MM_8",
    "etau_BDT_28", "emu_BDT_48"
  };
  vector<double> fMvaCuts = { //mva score cut values
    0.8603, 0.1175,  //scores optimize significance with given branching ratios
    0.7, 0.6,//scores are to cut ~30% of the signal currently
    0.1069, 0.2536 //scores optimize significance with given branching ratios
  };
  double fMvaOutputs[20];
  
  //Histograms:
  const static Int_t fn = 200; //max histogram sets
  const static Int_t fQcdOffset = 100; //histogram set + offset = set with same sign selection
  Int_t fEventSets[fn];  //indicates which sets to create
  Int_t fTreeSets[fn];   //indicates which trees to create

  TFile*        fOut;
  TDirectory*   fTopDir;
  TDirectory*   fDirectories[4*fn]; // 0 - fn events, fn - 2fn photon, 2fn - 3fn lep, 3fn - 4fn trees
  EventHist_t*  fEventHist[fn];
  PhotonHist_t* fPhotonHist[fn];
  LepHist_t*    fLepHist[fn];
  TTree*        fTrees[fn];

  TString       fFolderName = ""; //name of the folder the tree is from

  Int_t         fDYType = -1; //for splitting Z->ll into 1: tau tau and 2: e/mu e/mu
  Int_t         fWriteTrees = 0; //write out ttrees for the events
  Double_t      fXsec = 0.; //cross-section for full event weight with trees
  Tree_t        fTreeVars; //for filling the ttrees/mva evaluation
  Int_t         fEventCategory; //for identifying the process in mva trainings

  Int_t         fUseTauFakeSF = 0; //add in fake tau scale factor weight to event weights (2 to use ones defined here)
  Int_t         fIsData = 0; //0 if MC, 1 if electron data, 2 if muon data
  bool          fSkipDoubleTrigger = false; //skip events with both triggers (to avoid double counting), only count this lepton status events
  
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
    TMVA::Tools::Instance(); //load the library
    mva = new TMVA::Reader("!Color:!Silent");

    mva->AddVariable("lepm"            ,&fTreeVars.lepm           ); 
    mva->AddVariable("mtone"           ,&fTreeVars.mtone          );
    mva->AddVariable("mttwo"           ,&fTreeVars.mttwo          );
    mva->AddVariable("leponept"        ,&fTreeVars.leponept       );
    mva->AddVariable("leptwopt"        ,&fTreeVars.leptwopt       );
    mva->AddVariable("pxivis"          ,&fTreeVars.pxivis         );
    mva->AddVariable("pxiinv"          ,&fTreeVars.pxiinv         );
    mva->AddVariable("njets"           ,&fTreeVars.njets          );
    mva->AddVariable("lepdeltaeta"     ,&fTreeVars.lepdeltaeta    );
    mva->AddVariable("lepdeltaphi"     ,&fTreeVars.lepdeltaphi    );
    mva->AddVariable("leponedeltaphi"  ,&fTreeVars.leponedeltaphi );
    mva->AddVariable("leptwodeltaphi"  ,&fTreeVars.leptwodeltaphi );
    
    mva->AddSpectator("leponed0"       ,&fTreeVars.leponed0       );
    mva->AddSpectator("leptwod0"       ,&fTreeVars.leptwod0       );
    mva->AddSpectator("htdeltaphi"     ,&fTreeVars.htdeltaphi     );
    mva->AddSpectator("leponeiso"      ,&fTreeVars.leponeiso      );
    mva->AddSpectator("onemetdeltaphi" ,&fTreeVars.onemetdeltaphi );
    mva->AddSpectator("twometdeltaphi" ,&fTreeVars.twometdeltaphi );
    mva->AddSpectator("metdeltaphi"    ,&fTreeVars.metdeltaphi    );
    mva->AddSpectator("leppt"          ,&fTreeVars.leppt          );
    mva->AddSpectator("met"            ,&fTreeVars.met            );
    mva->AddSpectator("lepdeltar"      ,&fTreeVars.lepdeltar      );
    mva->AddSpectator("fulleventweight",&fTreeVars.fulleventweight);
    mva->AddSpectator("eventweight"    ,&fTreeVars.eventweight    );
    mva->AddSpectator("eventcategory"  ,&fTreeVars.eventcategory  );

    //Initialize MVAs
    for(auto name : fMvaNames) {
      const char* f = Form("weights/%s.weights.xml",name.Data());
      if(!gSystem->AccessPathName(f))
	mva->BookMVA(name.Data(),f);
      else
	printf("Init: Warning! Weights file %s not found\n",f);
    }
    
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
    fEventSets [0] = 1; // all events

    fEventSets [1] = 1; // all opposite signed events
    fEventSets [1+fQcdOffset] = 1; // all same signed events

    fEventSets [2] = 1; // events with opposite signs and >= 1 photon
    fEventSets [2+fQcdOffset] = 1; // events with same signs and >= 1 photon
    fEventSets [3] = 1; // events with opposite signs and 1 photon
    fEventSets [3+fQcdOffset] = 1; // events with same signs and 1 photon

    fEventSets [5] = 1; // events with opposite signs and 1 tau and 1 muon
    fEventSets [5+fQcdOffset] = 1; // events with same signs and 1 tau and 1 muon
    fEventSets [25] = 1; // events with opposite signs and 1 tau and 1 electron
    fEventSets [25+fQcdOffset] = 1; // events with same signs and 1 tau and 1 electron    

    fEventSets [6] = 1; // events with opposite signs and passing Mu+Tau Pt cuts with no photon check
    fEventSets [6+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt cuts with no photon check
    fEventSets [7] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts with no photon check
    fEventSets [7+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts with no photon check
    fTreeSets  [7] = 0;

    // Sets 8-10 MVA cuts applied
    fEventSets [8] = 1; // events with opposite signs
    fEventSets [8+fQcdOffset] = 1; // events with same signs 
    fTreeSets  [8] = 0;
    fEventSets [9] = 1; // events with opposite signs 
    fEventSets [9+fQcdOffset] = 1; // events with same signs 
    fEventSets [10] = 1; // events with opposite signs 
    fEventSets [10+fQcdOffset] = 1; // events with same signs 
    
    fEventSets [11] = 1; // events with opposite signs and inv > 0.5*vis - 35
    fEventSets [11+fQcdOffset] = 1; // events with same signs and inv > 0.5*vis - 35
    fEventSets [12] = 1; // events with opposite signs and inv > 0.5*vis - 20
    fEventSets [12+fQcdOffset] = 1; // events with same signs and inv > 0.5*vis - 20

    fEventSets [13] = 1; // events with opposite signs and 50 < mll < 100
    fEventSets [13+fQcdOffset] = 1; // events with same signs and 50 < mll < 100

    fEventSets [14] = 1; // events with opposite signs and inv > 0.5*vis - 20
    fEventSets [14+fQcdOffset] = 1; // events with same signs and inv > 0.5*vis - 20

    fEventSets [15] = 1; // events with opposite signs and mT_lep < 80
    fEventSets [15+fQcdOffset] = 1; // events with same signs and mT_lep < 80
    fEventSets [16] = 1; // events with opposite signs and mT_tau < 80
    fEventSets [16+fQcdOffset] = 1; // events with same signs and mT_tau < 80

    fEventSets [17] = 1; // events with opposite signs and nBJets = 0
    fEventSets [17+fQcdOffset] = 1; // events with same signs and nBJets = 0
    fTreeSets  [17] = 0;

    fEventSets [18] = 1; // events with opposite signs and nJets = 0
    fEventSets [18+fQcdOffset] = 1; // events with same signs and nJets = 0
    fEventSets [19] = 1; // events with opposite signs and nJets = 1
    fEventSets [19+fQcdOffset] = 1; // events with same signs and nJets = 1
    fEventSets [20] = 1; // events with opposite signs and nJets > 1
    fEventSets [20+fQcdOffset] = 1; // events with same signs and nJets > 1

    fEventSets [21] = 1; // events with opposite signs and nPhotons = 0
    fEventSets [21+fQcdOffset] = 1; // events with same signs and nPhotons = 0
    fEventSets [22] = 1; // events with opposite signs and nPhotons = 1
    fEventSets [22+fQcdOffset] = 1; // events with same signs and nPhotons = 1
    fEventSets [23] = 1; // events with opposite signs and nPhotons > 1
    fEventSets [23+fQcdOffset] = 1; // events with same signs and nPhotons > 1

    fEventSets [24] = 1; // events with opposite signs and BDT > -0.2
    fEventSets [24+fQcdOffset] = 1; // events with same signs and BDT > -0.2
    fTreeSets  [24] = 0;
    
    fEventSets [26] = 1; // events with opposite signs and passing E+Tau Pt cuts with no photon check
    fEventSets [26+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt cuts with no photon check
    fEventSets [27] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts with no photon check
    fEventSets [27+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts with no photon check
    fTreeSets  [27] = 0;

    // Sets 8-10 MVA cuts applied
    fEventSets [28] = 1; // events with opposite signs
    fEventSets [28+fQcdOffset] = 1; // events with same signs 
    fTreeSets  [28] = 0;
    fEventSets [29] = 1; // events with opposite signs 
    fEventSets [29+fQcdOffset] = 1; // events with same signs 
    fEventSets [30] = 1; // events with opposite signs 
    fEventSets [30+fQcdOffset] = 1; // events with same signs 

    fEventSets [31] = 1; // events with opposite signs and inv > 0.5*vis - 35
    fEventSets [31+fQcdOffset] = 1; // events with same signs and inv > 0.5*vis - 35
    fEventSets [32] = 1; // events with opposite signs and inv > 0.5*vis - 20
    fEventSets [32+fQcdOffset] = 1; // events with same signs and inv > 0.5*vis - 20

    fEventSets [33] = 1; // events with opposite signs and 50 < mll < 100
    fEventSets [33+fQcdOffset] = 1; // events with same signs and 50 < mll < 100

    fEventSets [34] = 1; // events with opposite signs and inv > 0.5*vis - 20
    fEventSets [34+fQcdOffset] = 1; // events with same signs and inv > 0.5*vis - 20

    fEventSets [35] = 1; // events with opposite signs and mT_lep < 80
    fEventSets [35+fQcdOffset] = 1; // events with same signs and mT_lep < 80
    fEventSets [36] = 1; // events with opposite signs and mT_tau < 80
    fEventSets [36+fQcdOffset] = 1; // events with same signs and mT_tau < 80

    fEventSets [37] = 1; // events with opposite signs and nBJets = 0
    fEventSets [37+fQcdOffset] = 1; // events with same signs and nBJets = 0
    fTreeSets  [37] = 0;

    fEventSets [38] = 1; // events with opposite signs and nJets = 0
    fEventSets [38+fQcdOffset] = 1; // events with same signs and nJets = 0
    fEventSets [39] = 1; // events with opposite signs and nJets = 1
    fEventSets [39+fQcdOffset] = 1; // events with same signs and nJets = 1
    fEventSets [40] = 1; // events with opposite signs and nJets > 1
    fEventSets [40+fQcdOffset] = 1; // events with same signs and nJets > 1

    fEventSets [41] = 1; // events with opposite signs and nPhotons = 0
    fEventSets [41+fQcdOffset] = 1; // events with same signs and nPhotons = 0
    fEventSets [42] = 1; // events with opposite signs and nPhotons = 1
    fEventSets [42+fQcdOffset] = 1; // events with same signs and nPhotons = 1
    fEventSets [43] = 1; // events with opposite signs and nPhotons > 1
    fEventSets [43+fQcdOffset] = 1; // events with same signs and nPhotons > 1
    
    fEventSets [44] = 1; // events with opposite signs and BDT > -0.2
    fEventSets [44+fQcdOffset] = 1; // events with same signs and BDT > -0.2
    fTreeSets  [44] = 0;


    // E+Mu Books
    fEventSets [45] = 1; // events with opposite signs and 1 muon and 1 electron
    fEventSets [45+fQcdOffset] = 1; // events with same signs and 1 muon and 1 electron
    fEventSets [46] = 1; // events with opposite signs
    fEventSets [46+fQcdOffset] = 1; // events with same signs
    fEventSets [47] = 1; // events with opposite signs
    fEventSets [47+fQcdOffset] = 1; // events with same
    fEventSets [48] = 1; // events with opposite signs + no bjets
    fEventSets [48+fQcdOffset] = 1; // events with same
    fEventSets [49] = 1; // events with opposite signs + BDT cut
    fEventSets [49+fQcdOffset] = 1; // events with same
    fTreeSets  [48] = 1;
    fEventSets [53] = 1; // events with opposite signs + met cut
    fEventSets [53+fQcdOffset] = 1; // events with same
    fEventSets [55] = 1; // events with opposite signs + mt_e/mt_mu cuts
    fEventSets [55+fQcdOffset] = 1; // events with same
    fEventSets [56] = 1; // events with opposite signs + mass window
    fEventSets [56+fQcdOffset] = 1; // events with same
    fEventSets [58] = 1; // events with opposite signs + 0-jet
    fEventSets [58+fQcdOffset] = 1; // events with same
    fEventSets [59] = 1; // events with opposite signs + 1-jet
    fEventSets [59+fQcdOffset] = 1; // events with same
    fEventSets [60] = 1; // events with opposite signs + >1-jet
    fEventSets [60+fQcdOffset] = 1; // events with same
    
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
  fChain->SetBranchAddress("mcEra"               , &mcEra                );
  fChain->SetBranchAddress("triggerLeptonStatus" , &triggerLeptonStatus  );
  fChain->SetBranchAddress("eventWeight"         , &eventWeight          );
  fChain->SetBranchAddress("genWeight"           , &genWeight            );
  fChain->SetBranchAddress("puWeight"            , &puWeight             );
  fChain->SetBranchAddress("topPtWeight"         , &topPtWeight          );
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
  fChain->SetBranchAddress("genLeptonOneP4" 	 , &genLeptonOneP4       );
  fChain->SetBranchAddress("genLeptonTwoP4" 	 , &genLeptonTwoP4       );
  fChain->SetBranchAddress("photonP4"  	         , &photonP4             );
  fChain->SetBranchAddress("nMuons"              , &nMuons               );
  fChain->SetBranchAddress("nElectrons"          , &nElectrons           );
  fChain->SetBranchAddress("nTaus"               , &nTaus                );
  fChain->SetBranchAddress("nPhotons"            , &nPhotons             );
  fChain->SetBranchAddress("nJets"               , &nJets                );
  fChain->SetBranchAddress("nBJets"              , &nBJets               );
  fChain->SetBranchAddress("nGenTausHad"         , &nGenTausHad          );
  fChain->SetBranchAddress("nGenTausLep"         , &nGenTausLep          );
  fChain->SetBranchAddress("nGenElectrons"       , &nGenElectrons        );
  fChain->SetBranchAddress("nGenMuons"           , &nGenMuons            );
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
  // fChain->SetBranchAddress("puppMET"             , &puppMET              );
  // fChain->SetBranchAddress("puppMETphi"          , &puppMETphi           );
  // fChain->SetBranchAddress("puppMETCov00"        , &puppMETCov00         );
  // fChain->SetBranchAddress("puppMETCov01"        , &puppMETCov01         );
  // fChain->SetBranchAddress("puppMETCov11"        , &puppMETCov11         );
  fChain->SetBranchAddress("puppMETC"            , &puppMETC             );
  fChain->SetBranchAddress("puppMETCphi"         , &puppMETCphi          );
  fChain->SetBranchAddress("puppMETCCov00"       , &puppMETCCov00        );
  fChain->SetBranchAddress("puppMETCCov01"       , &puppMETCCov01        );
  fChain->SetBranchAddress("puppMETCCov11"       , &puppMETCCov11        );
  // fChain->SetBranchAddress("alpacaMET"           , &alpacaMET            );
  // fChain->SetBranchAddress("alpacaMETphi"        , &alpacaMETphi         );
  // fChain->SetBranchAddress("pcpMET"              , &pcpMET               );
  // fChain->SetBranchAddress("pcpMETphi"           , &pcpMETphi            );
  fChain->SetBranchAddress("trkMET"              , &trkMET               );
  fChain->SetBranchAddress("trkMETphi"           , &trkMETphi            );
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
