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

// Headers needed by this particular selector
#include "TLorentzVector.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"

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
  UInt_t runNumber                 ;
  ULong64_t eventNumber            ;
  UInt_t lumiSection               ;
  UInt_t nPV                       ;
  Float_t nPU                      ;
  UInt_t mcEra                     ;
  UInt_t triggerLeptonStatus       ;
  Float_t eventWeight              ;
  Float_t genWeight                ;
  Float_t puWeight                 ;
  Float_t topPtWeight              ;
  Int_t tauDecayMode               ;
  Float_t tauMVA                   ;
  Int_t tauGenFlavor               ;
  Int_t tauGenFlavorHad            ;
  Float_t tauVetoedJetPt           ;
  Float_t tauVetoedJetPtUnc        ;
  TLorentzVector* leptonOneP4 = 0  ;
  TLorentzVector* leptonTwoP4 = 0  ;
  Int_t leptonOneFlavor            ;
  Int_t leptonTwoFlavor            ;
  TLorentzVector* photonP4 = 0     ;
  UInt_t nMuons                    ;
  UInt_t nElectrons                ;
  UInt_t nTaus                     ;
  UInt_t nPhotons                  ;
  UInt_t nJets                     ;
  UInt_t nBJets                    ;
  UInt_t nGenTausHad               ;
  UInt_t nGenTausLep               ;
  Float_t htSum                    ;
  Float_t ht                       ;
  Float_t htPhi                    ;
  Float_t met                      ;
  Float_t metPhi                   ;
  Float_t covMet00                 ;
  Float_t covMet01                 ;
  Float_t covMet11                 ;
  Float_t massSVFit                ;
  Float_t massErrSVFit             ;
  Int_t   svFitStatus              ;
  TLorentzVector* leptonOneSVP4 = 0;
  TLorentzVector* leptonTwoSVP4 = 0;


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
    TH1F* hNTaus;
    TH1F* hNPhotons;
    TH1F* hNGenTausHad;
    TH1F* hNGenTausLep;
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
    TH1F* hCovMet00;
    TH1F* hCovMet01;
    TH1F* hCovMet11;
    TH1F* hMassSVFit;
    TH1F* hMassErrSVFit;
    TH1F* hSVFitStatus;
    
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
    TH1F* hTwoIso;
    TH1F* hTwoRelIso;
    TH1F* hTwoFlavor;
    TH1F* hTwoQ;
    TH1F* hTwoTrigger;
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
  virtual void    FillEventHistogram(EventHist_t* Hist);
  virtual void    FillPhotonHistogram(PhotonHist_t* Hist);
  virtual void    FillLepHistogram(LepHist_t* Hist);


  //Define relevant fields
  TStopwatch* timer = new TStopwatch();
  //Histograms:
  const static Int_t fn = 200; //max histogram sets
  const static Int_t fQcdOffset = 100; //histogram set + offset = set with same sign selection
  Int_t fEventSets[fn];  //indicates which sets to create

  TFile*        fOut;
  TDirectory*   fTopDir;
  TDirectory*   fDirectories[3*fn];
  EventHist_t*  fEventHist[fn];
  PhotonHist_t* fPhotonHist[fn];
  LepHist_t*    fLepHist[fn];

  TString fFolderName = ""; //name of the folder the tree is from
  
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
    fOut = new TFile(Form("ztautau%s_%s.hist",(fFolderName == "") ? "" : ("_"+fFolderName).Data(),tree->GetName()),
		     "RECREATE","ZTauTauHistMaker output histogram file");
    fTopDir = fOut->mkdir("Data");
    fTopDir->cd();

    for(int i = 0; i < fn; ++i) {
      fEventSets[i]  = 0;
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
    fEventSets [45] = 1; // events with opposite signs and 1 muon and 1 electron
    fEventSets [45+fQcdOffset] = 1; // events with same signs and 1 muon and 1 electron
    

    fEventSets [6] = 1; // events with opposite signs and passing Mu+Tau Pt cuts with no photon check
    fEventSets [6+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt cuts with no photon check
    fEventSets [7] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts with no photon check
    fEventSets [7+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts with no photon check

    fEventSets [11] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [11+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [12] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    fEventSets [12+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 with no photon check

    //same as 11/12 but with 40 < mll < 100
    fEventSets [13] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [13+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [14] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    fEventSets [14+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    //different coefficient for Z+g
    fEventSets [15] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    fEventSets [15+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    //Adding mt cut
    fEventSets [16] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 mtMu < 80 with no photon check
    fEventSets [16+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 mtMu < 80 with no photon check
    fEventSets [17] = 1; // events with opposite signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 mtMu < 80 with no photon check
    fEventSets [17+fQcdOffset] = 1; // events with same signs and passing Mu+Tau Pt + angle cuts and vis/inv > 0 mtMu < 80 with no photon check
    
    fEventSets [8] = 1; // events with opposite signs and passing Mu+Tau cuts with no photon check
    fEventSets [8+fQcdOffset] = 1; // events with same signs and passing Mu+Tau cuts with no photon check
    fEventSets [9] = 1; // events with opposite signs and passing Mu+Tau cuts
    fEventSets [9+fQcdOffset] = 1; // events with same signs and passing Mu+Tau cuts
    fEventSets [10] = 1; // events with opposite signs and passing Mu+Tau cuts including updated ones
    fEventSets [10+fQcdOffset] = 1; // events with same signs and passing Mu+Tau cuts including updated ones

    fEventSets [26] = 1; // events with opposite signs and passing E+Tau Pt cuts with no photon check
    fEventSets [26+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt cuts with no photon check
    fEventSets [27] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts with no photon check
    fEventSets [27+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts with no photon check

    fEventSets [31] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [31+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [32] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    fEventSets [32+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > 0 with no photon check

    //same as 21/22 but with 40 < mll < 100
    fEventSets [33] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [33+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > -0.5 with no photon check
    fEventSets [34] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    fEventSets [34+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    //different coefficient for Z+g
    fEventSets [35] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    fEventSets [35+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > 0 with no photon check
    //Adding mt cut
    fEventSets [36] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > 0 mtE < 80 with no photon check
    fEventSets [36+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > 0 mtE < 80 with no photon check
    fEventSets [37] = 1; // events with opposite signs and passing E+Tau Pt + angle cuts and vis/inv > 0 mtE < 80 with no photon check
    fEventSets [37+fQcdOffset] = 1; // events with same signs and passing E+Tau Pt + angle cuts and vis/inv > 0 mtE < 80 with no photon check

    fEventSets [28] = 1; // events with opposite signs and passing E+Tau cuts with no photon check
    fEventSets [28+fQcdOffset] = 1; // events with same signs and passing E+Tau cuts with no photon check
    fEventSets [29] = 1; // events with opposite signs and passing E+Tau cuts
    fEventSets [29+fQcdOffset] = 1; // events with same signs and passing E+Tau cuts
    fEventSets [30] = 1; // events with opposite signs and passing E+Tau cuts including updated ones
    fEventSets [30+fQcdOffset] = 1; // events with same signs and passing E+Tau cuts including updated ones
    
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
  fChain->SetBranchAddress("photonP4"  	         , &photonP4             );
  fChain->SetBranchAddress("nMuons"              , &nMuons               );
  fChain->SetBranchAddress("nElectrons"          , &nElectrons           );
  fChain->SetBranchAddress("nTaus"               , &nTaus                );
  fChain->SetBranchAddress("nPhotons"            , &nPhotons             );
  fChain->SetBranchAddress("nJets"               , &nJets                );
  fChain->SetBranchAddress("nBJets"              , &nBJets               );
  fChain->SetBranchAddress("nGenTausHad"         , &nGenTausHad          );
  fChain->SetBranchAddress("nGenTausLep"         , &nGenTausLep          );
  fChain->SetBranchAddress("htSum"               , &htSum                );
  fChain->SetBranchAddress("ht"                  , &ht                   );
  fChain->SetBranchAddress("htPhi"               , &htPhi                );
  fChain->SetBranchAddress("met"                 , &met                  );
  fChain->SetBranchAddress("metPhi"              , &metPhi               );
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
