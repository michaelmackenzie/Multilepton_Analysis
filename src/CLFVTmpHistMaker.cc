#define CLFVTMPHISTMAKER_CXX

#include "interface/CLFVTmpHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
CLFVTmpHistMaker::CLFVTmpHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {

}

//--------------------------------------------------------------------------------------------------------------
CLFVTmpHistMaker::~CLFVTmpHistMaker() {
  // HistMaker::~HistMaker();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::DeleteHistograms() {
  HistMaker::DeleteHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  HistMaker::Begin(nullptr);
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::FillAllHistograms(Int_t index) {
  HistMaker::FillAllHistograms(index);
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i]  = 0;
    fSysSets[i] = 0;
  }
  for(int i = 0; i < fn; ++i) {
    fTreeSets[i]  = 0;
  }

  //Event Sets
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

      fEventSets [kMuTau + 7] = 1;
      fEventSets [kMuTau + 8] = 1;
      fTreeSets  [kMuTau + 8] = 1;
      fTreeSets  [kMuTau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
      fSysSets   [kMuTau + 8] = 1;

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
      fEventSets [kMuTau + 92] = 1; //Loose nominal selection
      fEventSets [kMuTau + 94] = 1; //Loose nominal selection

      // jet --> tau MC taus in nominal selection, different process weights
      fEventSets [kMuTau + 81] = 1; //W+Jets
      fEventSets [kMuTau + 82] = 1; //Top
      fEventSets [kMuTau + 86] = 1; //W+Jets, no fake MC
      fEventSets [kMuTau + 87] = 1; //Top, no fake MC
      fEventSets [kMuTau + 93] = 1; //Loose nominal selection, no fake MC
      fEventSets [kMuTau + 95] = 1; //Loose nominal selection, no fake MC

      fEventSets [kMuTau + 42] = 1; //fake MC taus in the nominal selection
    }
    if(etau) {
      fEventSets [kETau + 1] = 1; // all events
      fEventSets [kETau + 2] = 1;
      fEventSets [kETau + 3] = 1;
      fEventSets [kETau + 4] = 1;

      fEventSets [kETau + 7] = 1;
      fEventSets [kETau + 8] = 1;
      fTreeSets  [kETau + 8] = 1;
      fTreeSets  [kETau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
      fSysSets   [kETau + 8] = 1;

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
      fEventSets [kETau + 92] = 1; //Loose nominal selection
      fEventSets [kETau + 94] = 1; //Loose nominal selection

      // jet --> tau MC taus in nominal selection, different process weights
      fEventSets [kETau + 81] = 1; //W+Jets
      fEventSets [kETau + 82] = 1; //Top
      fEventSets [kETau + 86] = 1; //W+Jets, no fake MC
      fEventSets [kETau + 87] = 1; //Top, no fake MC
      fEventSets [kETau + 93] = 1; //Loose nominal selection, no fake MC
      fEventSets [kETau + 95] = 1; //Loose nominal selection, no fake MC

      fEventSets [kETau + 42] = 1; //fake MC taus in the nominal selection
    }
    if(emu) {
      fEventSets [kEMu  + 1] = 1; // all events
      fEventSets [kEMu  + 2] = 1; // after pT cuts
      fEventSets [kEMu  + 3] = 1; // after eta, mass, trigger cuts
      fEventSets [kEMu  + 4] = 1; // after additional IDs

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

      fEventSets [kEMu  + 24] = 1; // events within mass window
      fEventSets [kEMu  + 25] = 1; // events within mass window

      fEventSets [kEMu  + 32] = 1; // > 0 b-jets (top selection)

      fEventSets [kEMu  + 35] = 1;
      fEventSets [kEMu  + 90] = 1; //Loose nominal selection
      fEventSets [kEMu  + 91] = 1; //Loose nominal selection, no fake MC

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
      fEventSets [kMuMu + 7] = 1;
      fEventSets [kMuMu + 8] = 1;
      fSysSets   [kMuMu + 8] = 1;

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kMuMu + i] = 1;
      fSysSets[kMuMu  + 9  + fMVAConfig.categories_["zemu"].size()] = 1; //start with most significant category
      fSysSets[kMuMu  + 8  + fMVAConfig.categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kMuMu  + 14 + fMVAConfig.categories_["hemu"].size()] = 1; //start with most significant category
      fSysSets[kMuMu  + 13 + fMVAConfig.categories_["hemu"].size()] = 1; //second most significant category

      fEventSets [kMuMu + 22] = 1; // events with nJets = 0
      fEventSets [kMuMu + 23] = 1; // events with nJets > 0

      fEventSets [kMuMu + 24] = 1; // events within mass window
      fEventSets [kMuMu + 25] = 1; // events within mass window

      fEventSets [kMuMu + 26] = 1; // events top set
      fEventSets [kMuMu + 27] = 1;

      fEventSets [kMuMu + 35] = 1;
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

      fEventSets [kEE   + 7] = 1;
      fEventSets [kEE   + 8] = 1;
      fSysSets   [kEE   + 8] = 1;

      // MVA categories
      for(int i = 9; i < 19; ++i) fEventSets[kEE + i] = 1;
      fSysSets[kEE    + 9  + fMVAConfig.categories_["zemu"].size()] = 1; //start with most significant category
      fSysSets[kEE    + 8  + fMVAConfig.categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kEE    + 14 + fMVAConfig.categories_["hemu"].size()] = 1; //start with most significant category
      fSysSets[kEE    + 13 + fMVAConfig.categories_["hemu"].size()] = 1; //second most significant category

      fEventSets [kEE   + 22] = 1; // events with nJets = 0
      fEventSets [kEE   + 23] = 1; // events with nJets > 0

      fEventSets [kEE   + 24] = 1; // events within mass window
      fEventSets [kEE   + 25] = 1; // events within mass window

      fEventSets [kEE   + 26] = 1; // events top set
      fEventSets [kEE   + 27] = 1;

      fEventSets [kEE   + 35] = 1;
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
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  // BookPhotonHistograms();
  if(fWriteTrees && fDoSystematics >= 0) BookTrees();
  if(fDoSystematics) BookSystematicHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::BookEventHistograms() {
  for(int i = 0; i < fQcdOffset; ++i) {
    if(fEventSets[i]) { //turn on all offset histogram sets
      fEventSets[i+fQcdOffset] = 1;
      fEventSets[i+fMisIDOffset] = 1;
      fEventSets[i+fQcdOffset+fMisIDOffset] = 1;
    }
  }
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"event_%i",i);
      TDirectory* folder = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i] = folder;
      folder->cd();
      fEventHist[i] = new EventHist_t;
      BookBaseEventHistograms(i, dirname);

      //Additional j-->tau info
      int jstart = SystematicGrouping::kJetToTau;
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightGroup     , "jettotauweightgroup"     , Form("%s: JetToTauWeightGroup"         ,dirname),  50, jstart, 50+jstart, folder);
      for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
        Utilities::BookH1F(fEventHist[i]->hJetToTauComps[ji]     , Form("jettotaucomps_%i", ji), Form("%s: JetToTauComps %i"      ,dirname, ji),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hJetToTauWts  [ji]     , Form("jettotauwts_%i"  , ji), Form("%s: JetToTauWts %i"        ,dirname, ji),  50,    0,   2, folder);
      }
      Utilities::BookH1F(fEventHist[i]->hJetToTauCompEffect      , "jettotaucompeffect"      , Form("%s: JetToTauCompEffect"          ,dirname),  50,    0,   2, folder);

      //Testing PU weights
      Utilities::BookH1F(fEventHist[i]->hNPV[1]                  , "npv1"                    , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1F(fEventHist[i]->hNPU[1]                  , "npu1"                    , Form("%s: NPU"                         ,dirname),  50,    0, 100, folder);

      //Testing Jet PU ID weights
      Utilities::BookH1D(fEventHist[i]->hNJets20[1]              , "njets201"                , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20Rej[1]           , "njets20rej1"             , Form("%s: NJets20Rej"                  ,dirname),  10,    0,  10, folder);

      //Testing b-jet IDs
      Utilities::BookH1D(fEventHist[i]->hNBJets                  , "nbjets"                  , Form("%s: NBJets"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJetsM                 , "nbjetsm"                 , Form("%s: NBJetsM"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJetsL                 , "nbjetsl"                 , Form("%s: NBJetsL"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20M[0]            , "nbjets20m"               , Form("%s: NBJets20M"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[1]             , "nbjets201"               , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20M[1]            , "nbjets20m1"              , Form("%s: NBJets20M"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[1]            , "nbjets20l1"              , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);

      //j-->tau measurement histograms
      int njetspt = 6;
      double jetspt[] = {0.  , 30. , 40. , 50.  , 70.,
                         100.,
                         1000.};
      int njetseta = 10;
      double jetseta[] = {-2.5 , -2. , -1.5 , -1.,
                          -0.5 , 0.   , 0.5 , 1.   , 1.5,
                          2.   ,
                          2.5};

      Utilities::BookH2F(fEventHist[i]->hJetsPtVsEta[0]  , "jetsptvseta0"        , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hJetsPtVsEta[1]  , "jetsptvseta1"        , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hJetsPtVsEta[2]  , "jetsptvseta2"        , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsPtVsEta[0] , "bjetsptvseta0"       , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsPtVsEta[1] , "bjetsptvseta1"       , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsPtVsEta[2] , "bjetsptvseta2"       , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsMPtVsEta[0], "bjetsmptvseta0"      , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsMPtVsEta[1], "bjetsmptvseta1"      , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsMPtVsEta[2], "bjetsmptvseta2"      , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsLPtVsEta[0], "bjetslptvseta0"      , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsLPtVsEta[1], "bjetslptvseta1"      , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsLPtVsEta[2], "bjetslptvseta2"      , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);

      //Testing muon triggers
      Utilities::BookH1D(fEventHist[i]->hMuonTriggerStatus  , "muontriggerstatus"   , Form("%s: MuonTriggerStatus"   ,dirname),  10,    0,  10, folder);

      //Testing tau weights
      Utilities::BookH1D(fEventHist[i]->hTauDecayMode[1]    , "taudecaymode1"       , Form("%s: TauDecayMode"        ,dirname),  15,    0,  15, folder);

      //Testing jet ID weights
      Utilities::BookH1F(fEventHist[i]->hJetPt[1]           , "jetpt1"              , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);


      //Testing Z pT weights
      Utilities::BookH1F(fEventHist[i]->hLepPt[1], "leppt1"        , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPt[2], "leppt2"        , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[1] , "lepm1"         , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[2] , "lepm2"         , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[3] , "lepm3"         , Form("%s: Lepton M"       ,dirname)  ,  80,  70, 110, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[4] , "lepm4"         , Form("%s: Lepton M"       ,dirname)  ,  40, 105, 145, folder);

      //Z pT weights measurement
      const double mbins[] = {50., 80., 100., 130., 500.}; //modified from H->tautau
      const int nmbins = sizeof(mbins) / sizeof(*mbins) - 1;
      const double pbins[] = {0., 3., 6., 10., 15., 20., 30., 40., 50., 100., 150., 200., 300., 400., 1000.}; //modified from H->tautau
      const int npbins = sizeof(pbins) / sizeof(*pbins) - 1;

      Utilities::BookH2F(fEventHist[i]->hLepPtVsM[0], "lepptvsm0"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepPtVsM[1], "lepptvsm1"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepPtVsM[2], "lepptvsm2"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepPtVsM[3], "lepptvsm3"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepPtVsM[4], "lepptvsm4"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hZPtVsM[0]  , "zptvsm0"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hZPtVsM[1]  , "zptvsm1"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hZPtVsM[2]  , "zptvsm2"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hZPtVsM[3]  , "zptvsm3"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH2F(fEventHist[i]->hZPtVsM[4]  , "zptvsm4"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZPt[0]     , "zpt"           , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZPt[1]     , "zpt1"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZPt[2]     , "zpt2"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZPt[3]     , "zpt3"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZPt[4]     , "zpt4"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[0]   , "zmass"         , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[1]   , "zmass1"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[2]   , "zmass2"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[3]   , "zmass3"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[4]   , "zmass4"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);


      //j->tau measurement histograms
      if(fSelection == "" || fSelection == "mutau" || fSelection == "etau") {
        //variable width bins for eta vs pT
        const double tetabins[] = { 0., 1.5, 2.3};
        const int ntetabins = sizeof(tetabins) / sizeof(*tetabins) - 1;
        const double tpbins[] = { 20., 22., 24., 26., 29.,
                                  33., 37., 42., 50., 60.,
                                  100.,
                                  200.};
        const int ntpbins = sizeof(tpbins) / sizeof(*tpbins) - 1;

        const int ndms = 4; //decay mode categories
        const int njetsmax = 2; //N(jets) categories: 0, 1, and >= 2 jets
        const int ncats = 3; //tau isolation categories: 0 = inclusive; 1 = anti-isolated; 2 = isolated
        for(int cat = 0; cat < ncats; ++cat) {
          for(int dm = 0; dm < ndms; ++dm) {
            for(int njets = 0; njets <= njetsmax; ++njets) {
              const char* hname = Form("faketau%ijet%idm_%i", njets, dm, cat);
              Utilities::BookH2F(fEventHist[i]->hFakeTauNJetDMPtEta[cat][njets][dm], hname, Form("%s: Fake tau Eta vs Pt" ,dirname),
                                 ntpbins, tpbins, ntetabins, tetabins, folder);
              const char* hmcname = Form("faketaumc%ijet%idm_%i", njets, dm, cat);
              Utilities::BookH2F(fEventHist[i]->hFakeTauMCNJetDMPtEta[cat][njets][dm], hmcname, Form("%s: Fake tau MC Eta vs Pt" ,dirname),
                                 ntpbins, tpbins, ntetabins, tetabins, folder);
            }
          }
        }

        Utilities::BookH1F(fEventHist[i]->hTausPt       , "tauspt"       , Form("%s: TausPt     "  ,dirname), 100,  0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hTausEta      , "tauseta"      , Form("%s: TausEta    "  ,dirname),  50, -2.5, 2.5, folder);
        Utilities::BookH1D(fEventHist[i]->hTausDM       , "tausdm"       , Form("%s: TausDM     "  ,dirname),  15,  0.,  15., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiJet  , "tausantijet"  , Form("%s: TausAntiJet"  ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiEle  , "tausantiele"  , Form("%s: TausAntiEle"  ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiMu   , "tausantimu"   , Form("%s: TausAntiMu"   ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausMVAAntiMu, "tausmvaantimu", Form("%s: TausMVAAntiMu",dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausGenFlavor, "tausgenflavor", Form("%s: TausGenFlavor",dirname),  40,  0.,  40., folder);
        Utilities::BookH1F(fEventHist[i]->hTausDeltaR   , "tausdeltar"   , Form("%s: TausDeltaR"   ,dirname),  50,  0.,   5., folder);
        Utilities::BookH1F(fEventHist[i]->hFakeTausPt   , "faketauspt"   , Form("%s: TausPt     "  ,dirname), 100,  0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hFakeTausEta  , "faketauseta"  , Form("%s: TausEta    "  ,dirname),  50, -2.5, 2.5, folder);
        Utilities::BookH1D(fEventHist[i]->hFakeTausDM   , "faketausdm"   , Form("%s: TausDM     "  ,dirname),  15,  0.,  15., folder);

        // jet --> tau non-closure histograms
        const double jt_lepm_bins[] = {50., 60., 70., 80., 100., 120., 140., 200.};
        const int    jt_nlepm_bins  = sizeof(jt_lepm_bins) / sizeof(*jt_lepm_bins) - 1;
        const double jt_dr_bins[]   = {0., 2., 2.5, 3., 3.5, 4., 5.};
        const int    jt_ndr_bins    = sizeof(jt_dr_bins) / sizeof(*jt_dr_bins) - 1;
        const double jt_mt_bins[]   = {0., 20., 35., 45., 55., 65., 75., 85., 150.};
        const int    jt_nmt_bins    = sizeof(jt_mt_bins) / sizeof(*jt_mt_bins) - 1;
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[0], "jettaudeltar0", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[1], "jettaudeltar1", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[2], "jettaudeltar2", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [0], "jettaulepm0"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [1], "jettaulepm1"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [2], "jettaulepm2"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [0], "jettaumtlep0" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [1], "jettaumtlep1" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [2], "jettaumtlep2" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [0], "jettaumtone0" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [1], "jettaumtone1" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [2], "jettaumtone2" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [0], "jettaumttwo0" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [1], "jettaumttwo1" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [2], "jettaumttwo2" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
      }


      //QCD weight testing
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[1], "lepdeltaphi1"  , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);
      const double drbins[] = {0.  , 1.5 , 2.  , 2.5 , 2.8 ,
                               3.  , 3.2 , 3.4 , 3.6 , 3.8 ,
                               4.  , 4.3 , 4.6 , 4.9 ,
                               6.};
      const int ndrbins = (sizeof(drbins)/sizeof(*drbins) - 1);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[1]        , "lepdeltar1"        , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[2]        , "lepdeltar2"        , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelRVsOneEta[0]  , "lepdelrvsoneeta"   , Form("%s: LepDelRVsOneEta"  ,dirname),  15, 0., 2.5, ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelRVsOneEta[1]  , "lepdelrvsoneeta1"  , Form("%s: LepDelRVsOneEta"  ,dirname),  15, 0., 2.5, ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelPhiVsOneEta[0], "lepdelphivsoneeta" , Form("%s: LepDelPhiVsOneEta",dirname), 15, 0., 2.5, 15, 0., 3.5, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelPhiVsOneEta[1], "lepdelphivsoneeta1", Form("%s: LepDelPhiVsOneEta",dirname), 15, 0., 2.5, 15, 0., 3.5, folder);
      Utilities::BookH1F(fEventHist[i]->hQCDDelRJ[0]         , "qcddelrj0"         , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH1F(fEventHist[i]->hQCDDelRJ[1]         , "qcddelrj1"         , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH1F(fEventHist[i]->hQCDDelRJ[2]         , "qcddelrj2"         , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      const double onebins[] = {10., 20., 30., 40., 150.};
      const double twobins[] = {10., 20., 30., 40., 150.};
      const int    nonebins  = sizeof(onebins) / sizeof(*onebins) - 1;
      const int    ntwobins  = sizeof(twobins) / sizeof(*twobins) - 1;
      Utilities::BookH2F(fEventHist[i]->hQCDOnePtVsTwoPtJ[0] , "qcdoneptvstwoptj0" , Form("%s: QCD one pt vs two pt",dirname), nonebins, onebins, ntwobins, twobins, folder);
      Utilities::BookH2F(fEventHist[i]->hQCDOnePtVsTwoPtJ[1] , "qcdoneptvstwoptj1" , Form("%s: QCD one pt vs two pt",dirname), nonebins, onebins, ntwobins, twobins, folder);
      Utilities::BookH2F(fEventHist[i]->hQCDOnePtVsTwoPtJ[2] , "qcdoneptvstwoptj2" , Form("%s: QCD one pt vs two pt",dirname), nonebins, onebins, ntwobins, twobins, folder);
      Utilities::BookH2F(fEventHist[i]->hQCDOnePtVsTwoPtJ[3] , "qcdoneptvstwopt"   , Form("%s: QCD one pt vs two pt",dirname), nonebins, onebins, ntwobins, twobins, folder);
      Utilities::BookH2F(fEventHist[i]->hQCDOnePtVsTwoPtIso[0] , "qcdoneptvstwoptiso", Form("%s: QCD one pt vs two pt",dirname), nonebins, onebins, ntwobins, twobins, folder);
      Utilities::BookH2F(fEventHist[i]->hQCDOnePtVsTwoPtIso[1] , "qcdoneptvstwoptiso1", Form("%s: QCD one pt vs two pt",dirname), nonebins, onebins, ntwobins, twobins, folder);


      //ATLAS Boosted frame variables
      for(int mode = 0; mode < 3; ++mode) {
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePx[mode], Form("leponeprimepx%i", mode), Form("%s: l1 px" , dirname),  50, -100., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePx[mode], Form("leptwoprimepx%i", mode), Form("%s: l2 px" , dirname),  50, -100., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePx   [mode], Form("metprimepx%i"   , mode), Form("%s: met px", dirname),  50,    0., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePy[mode], Form("leponeprimepy%i", mode), Form("%s: l1 py" , dirname),  50, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePy[mode], Form("leptwoprimepy%i", mode), Form("%s: l2 py" , dirname),  50, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePy   [mode], Form("metprimepy%i"   , mode), Form("%s: met py", dirname),  50, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePz[mode], Form("leponeprimepz%i", mode), Form("%s: l1 pz" , dirname), 100, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePz[mode], Form("leptwoprimepz%i", mode), Form("%s: l2 pz" , dirname), 100, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePz   [mode], Form("metprimepz%i"   , mode), Form("%s: met pz", dirname), 100, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimeE [mode], Form("leponeprimee%i" , mode), Form("%s: l1 e " , dirname), 100,    0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimeE [mode], Form("leptwoprimee%i" , mode), Form("%s: l2 e " , dirname), 100,    0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimeE    [mode], Form("metprimee%i"    , mode), Form("%s: met e ", dirname), 100,    0., 200., folder);
      }

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::BookPhotonHistograms() {
  HistMaker::BookPhotonHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::BookLepHistograms() {
  // if(fDYTesting) return;
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i] = folder;
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;

      BookBaseLepHistograms(i, dirname);

      //for correcting jet -> tau scale factors
      const double pts[] = {0.  , 20. , 25. , 30. , 35. ,
                            40. , 45. , 50. , 55. , 65. ,
                            80. , 100. , 120.,
                            200.};
      const int nbins_pt = sizeof(pts)/sizeof(*pts) - 1;

      const double rbins[] = {0.  , 1.  , 1.5 , 2.  , 2.5,
                              3.  , 3.5 ,
                              5.};
      const int nrbins = sizeof(rbins)/sizeof(*rbins) - 1;
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettauoneptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        Utilities::BookH2F(fLepHist[i]->hJetTauOnePtVsR[dmregion], name_r.Data() , Form("%s: Delta R Vs One Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins, folder);
        TString name = "jettauonept";
        if(dmregion > 0) {name += "_"; name += dmregion;}
        Utilities::BookH1F(fLepHist[i]->hJetTauOnePt[dmregion], name.Data() , Form("%s: One Pt"   ,dirname)  , nbins_pt, pts, folder);
      }
      Utilities::BookH1F(fLepHist[i]->hJetTauOneR  , "jettauoner" , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);

      const double pts_qcd[] = {0.  , 20. , 25. , 30. , 35. ,
                                40. , 50. , 60. ,
                                200.};
      const int nbins_pt_qcd = sizeof(pts_qcd)/sizeof(*pts_qcd) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtQCD[0], "jettauoneptqcd_0" , Form("%s: One Pt"   ,dirname)  , nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtQCD[1], "jettauoneptqcd_1" , Form("%s: One Pt"   ,dirname)  , nbins_pt_qcd, pts_qcd, folder);

      Utilities::BookH1F(fLepHist[i]->hJetTauOneEta, "jettauoneeta", Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      const double metbins[] = {0., 0.5, 0.9, 1.2, 1.4, 1.6, 1.8, 2., 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 4., 5.};
      const int nmetbins = sizeof(metbins)/sizeof(*metbins) - 1;
      for(int idm = -1; idm < 4; ++idm) {
        TString nm = (idm > -1) ? Form("jettauonemetdeltaphi_%i", idm) : "jettauonemetdeltaphi";
        Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDeltaPhi[idm+1], nm.Data(), Form("%s: OneMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      }
      const double metqcdbins[] = {0., 0.3, 0.6, 0.9, 1.3, 1.8, 2.5, 3.2, 4.};
      const int nmetqcdbins = sizeof(metqcdbins)/sizeof(*metqcdbins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiQCD[0], "jettauonemetdphiqcd0", Form("%s: OneMetDeltaPhi"  ,dirname), nmetqcdbins, metqcdbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiQCD[1], "jettauonemetdphiqcd1", Form("%s: OneMetDeltaPhi"  ,dirname), nmetqcdbins, metqcdbins, folder);
      const double isobins[] = {0., 0.02, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.40, 0.50};
      const int nisobins = sizeof(isobins)/sizeof(*isobins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneRelIso[0], "jettauonereliso_0", Form("%s: OneRelIso"  ,dirname), nisobins, isobins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOneRelIso[1], "jettauonereliso_1", Form("%s: OneRelIso"  ,dirname), nisobins, isobins, folder);

      //j-->tau compositions
      const double mtbins[] = {0., 10., 20., 30., 40., 50., 65., 90., 150.};
      const int    nmtbins  = sizeof(mtbins) / sizeof(*mtbins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiComp, "jettauonemetdphicomp", Form("%s: OneMetDeltaPhi"  ,dirname), nmetbins    , metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDPhiComp, "jettautwometdphicomp", Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins    , metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtComp     , "jettauoneptcomp"     , Form("%s: OnePt"           ,dirname), nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoPtComp     , "jettautwoptcomp"     , Form("%s: TwoPt"           ,dirname), nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauMTOneComp     , "jettaumtonecomp"     , Form("%s: MTOne"           ,dirname), nmtbins     , mtbins , folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauMTTwoComp     , "jettaumttwocomp"     , Form("%s: MTTwo"           ,dirname), nmtbins     , mtbins , folder);

      //end j-->tau histograms

      for(int j = 1; j < 12; ++j) { //lepton pT with/without various weights
        TString name = Form("onept%i", j);
        Utilities::BookH1F(fLepHist[i]->hOnePt[j], name.Data(), Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      }

      // Lepton two info //
      for(int j = 1; j < 12; ++j) {
        TString name = Form("twopt%i", j);
        Utilities::BookH1F(fLepHist[i]->hTwoPt[j]     , name.Data(), Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      }
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettautwoptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        Utilities::BookH2F(fLepHist[i]->hJetTauTwoPtVsR[dmregion], name_r.Data() , Form("%s: Delta R Vs Two Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins, folder);
        TString name = "jettautwopt";
        if(dmregion > 0) {name += "_"; name += dmregion;}
        Utilities::BookH1F(fLepHist[i]->hJetTauTwoPt[dmregion], name.Data() , Form("%s: Two Pt"   ,dirname)  , nbins_pt, pts, folder);
      }
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoR[0]     , "jettautwor0"     , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoR[1]     , "jettautwor"      , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEta[0]   , "jettautwoeta0"   , Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEta[1]   , "jettautwoeta"    , Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEtaQCD[0], "jettautwoetaqcd0", Form("%s: |Eta|"  ,dirname)  , 10,    0, 2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEtaQCD[1], "jettautwoetaqcd1", Form("%s: |Eta|"  ,dirname)  , 10,    0, 2.5, folder);

      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDeltaPhi[0], "jettautwometdeltaphi0", Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDeltaPhi[1], "jettautwometdeltaphi" , Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoRelIso[0], "jettautworeliso_0", Form("%s: TwoRelIso"  ,dirname), nisobins, isobins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoRelIso[1], "jettautworeliso_1", Form("%s: TwoRelIso"  ,dirname), nisobins, isobins, folder);


      //2D distributions
      const double tau_pts[] = {0., 25., 30., 35., 40.,
                                50.,
                                150.};
      const int nbins_tau_pt = sizeof(tau_pts) / sizeof(*tau_pts) - 1;
      Utilities::BookH2F(fLepHist[i]->hJetTauTwoPtVsOnePt, "jettautwoptvsonept", Form("%s: Two pT vs One pT", dirname), nbins_pt, pts, nbins_tau_pt, tau_pts, folder);
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::BookSystematicHistograms() {
  for(int i = 0; i < fQcdOffset; ++i) {
    if(fSysSets[i]) { //turn on all offset histogram sets
      fSysSets[i+fQcdOffset] = 1;
      fSysSets[i+fMisIDOffset] = 1;
      fSysSets[i+fQcdOffset+fMisIDOffset] = 1;
    }
  }
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0 && fSysSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"systematic_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[4*fn + i] = folder;
      fDirectories[4*fn + i]->cd();
      fSystematicHist[i] = new SystematicHist_t;
      for(int sys = 0; sys < kMaxSystematics; ++sys) {
        Utilities::BookH1F(fSystematicHist[i]->hLepM        [sys], Form("lepm_%i"        , sys), Form("%s: LepM %i"                   , dirname, sys) , 280,  40, 180, folder);
        Utilities::BookH1F(fSystematicHist[i]->hLepPt       [sys], Form("leppt_%i"       , sys), Form("%s: LepPt %i"                  , dirname, sys) ,  30,   0, 100, folder);
        Utilities::BookH1F(fSystematicHist[i]->hOnePt       [sys], Form("onept_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
        Utilities::BookH1F(fSystematicHist[i]->hOneEta      [sys], Form("oneeta_%i"      , sys), Form("%s: Eta %i"                    , dirname, sys) ,  30,  -3,   3, folder);
        Utilities::BookH1F(fSystematicHist[i]->hTwoPt       [sys], Form("twopt_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
        Utilities::BookH1F(fSystematicHist[i]->hTwoEta      [sys], Form("twoeta_%i"      , sys), Form("%s: Eta %i"                    , dirname, sys) ,  30,  -3,   3, folder);
        Utilities::BookH1F(fSystematicHist[i]->hWeightChange[sys], Form("weightchange_%i", sys), Form("%s: Relative weight change %i" , dirname, sys) ,  30, -2.,   2, folder);
        for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
          Utilities::BookH1D(fSystematicHist[i]->hMVA[j][sys], Form("mva%i_%i",j, sys)     , Form("%s: %s MVA %i" ,dirname, fMVAConfig.names_[j].Data(), sys),
                             fMVAConfig.NBins(j), fMVAConfig.Bins(j).data(), folder);
        }
      }
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      delete dirname;
      fTrees[i] = new TTree(Form("tree_%i",i),Form("CLFVTmpHistMaker TTree %i",i));
      fTrees[i]->Branch("leponept",           &fTreeVars.leponept          );
      fTrees[i]->Branch("leponeptoverm",      &fTreeVars.leponeptoverm     );
      // fTrees[i]->Branch("leponem",            &fTreeVars.leponem           );
      // fTrees[i]->Branch("leponeeta",          &fTreeVars.leponeeta         );
      // fTrees[i]->Branch("leponed0",           &fTreeVars.leponed0          );
      // fTrees[i]->Branch("leponeiso",          &fTreeVars.leponeiso         );
      // fTrees[i]->Branch("leponeidone",        &fTreeVars.leponeidone       );
      // fTrees[i]->Branch("leponeidtwo",        &fTreeVars.leponeidtwo       );
      // fTrees[i]->Branch("leponeidthree",      &fTreeVars.leponeidthree     );
      fTrees[i]->Branch("leptwopt",           &fTreeVars.leptwopt          );
      fTrees[i]->Branch("leptwoptoverm",      &fTreeVars.leptwoptoverm     );
      // fTrees[i]->Branch("leptwom",            &fTreeVars.leptwom           );
      // fTrees[i]->Branch("leptwoeta",          &fTreeVars.leptwoeta         );
      fTrees[i]->Branch("leptwod0",           &fTreeVars.leptwod0          );
      // fTrees[i]->Branch("leptwoiso",          &fTreeVars.leptwoiso         );
      // fTrees[i]->Branch("leptwoidone",        &fTreeVars.leptwoidone       );
      // fTrees[i]->Branch("leptwoidtwo",        &fTreeVars.leptwoidtwo       );
      // fTrees[i]->Branch("leptwoidthree",      &fTreeVars.leptwoidthree     );
      // fTrees[i]->Branch("lepp",               &fTreeVars.lepp              );
      fTrees[i]->Branch("leppt",              &fTreeVars.leppt             );
      fTrees[i]->Branch("lepm",               &fTreeVars.lepm              );
      fTrees[i]->Branch("lepptoverm",         &fTreeVars.lepptoverm        );
      fTrees[i]->Branch("lepeta",             &fTreeVars.lepeta            );
      fTrees[i]->Branch("lepdeltaeta",        &fTreeVars.lepdeltaeta       );
      fTrees[i]->Branch("lepdeltar",          &fTreeVars.lepdeltar         );
      fTrees[i]->Branch("lepdeltaphi",        &fTreeVars.lepdeltaphi       );
      fTrees[i]->Branch("deltaalphaz1",       &fTreeVars.deltaalphaz1      );
      fTrees[i]->Branch("deltaalphaz2",       &fTreeVars.deltaalphaz2      );
      fTrees[i]->Branch("deltaalphah1",       &fTreeVars.deltaalphah1      );
      fTrees[i]->Branch("deltaalphah2",       &fTreeVars.deltaalphah2      );
      fTrees[i]->Branch("deltaalpham1",       &fTreeVars.deltaalpham1      );
      fTrees[i]->Branch("deltaalpham2",       &fTreeVars.deltaalpham2      );
      // fTrees[i]->Branch("htdeltaphi",         &fTreeVars.htdeltaphi        );
      fTrees[i]->Branch("metdeltaphi",        &fTreeVars.metdeltaphi       );
      fTrees[i]->Branch("leponedeltaphi",     &fTreeVars.leponedeltaphi    );
      fTrees[i]->Branch("leptwodeltaphi",     &fTreeVars.leptwodeltaphi    );
      fTrees[i]->Branch("onemetdeltaphi",     &fTreeVars.onemetdeltaphi    );
      fTrees[i]->Branch("twometdeltaphi",     &fTreeVars.twometdeltaphi    );
      fTrees[i]->Branch("lepmestimate",       &fTreeVars.mestimate         );
      fTrees[i]->Branch("lepmestimatetwo",    &fTreeVars.mestimatetwo      );
      fTrees[i]->Branch("met",                &fTreeVars.met               );
      fTrees[i]->Branch("mtone",              &fTreeVars.mtone             );
      fTrees[i]->Branch("mttwo",              &fTreeVars.mttwo             );
      fTrees[i]->Branch("mtoneoverm",         &fTreeVars.mtoneoverm        );
      fTrees[i]->Branch("mttwooverm",         &fTreeVars.mttwooverm        );
      // fTrees[i]->Branch("pxivis",             &fTreeVars.pxivis            );
      // fTrees[i]->Branch("pxiinv",             &fTreeVars.pxiinv            );
      // fTrees[i]->Branch("ht",                 &fTreeVars.ht                );
      // fTrees[i]->Branch("htsum",              &fTreeVars.htsum             );
      fTrees[i]->Branch("jetpt",              &fTreeVars.jetpt             );
      fTrees[i]->Branch("njets",              &fTreeVars.njets             );

      //boosted frame variables
      for(int mode = 0; mode < 3; ++mode) {
        fTrees[i]->Branch(Form("leponeprimepx%i", mode), &fTreeVars.leponeprimepx[mode]);
        fTrees[i]->Branch(Form("leptwoprimepx%i", mode), &fTreeVars.leptwoprimepx[mode]);
        fTrees[i]->Branch(Form("metprimepx%i"   , mode), &fTreeVars.metprimepx   [mode]);
        fTrees[i]->Branch(Form("leponeprimepy%i", mode), &fTreeVars.leponeprimepy[mode]);
        fTrees[i]->Branch(Form("leptwoprimepy%i", mode), &fTreeVars.leptwoprimepy[mode]);
        fTrees[i]->Branch(Form("metprimepy%i"   , mode), &fTreeVars.metprimepy   [mode]);
        fTrees[i]->Branch(Form("leponeprimepz%i", mode), &fTreeVars.leponeprimepz[mode]);
        fTrees[i]->Branch(Form("leptwoprimepz%i", mode), &fTreeVars.leptwoprimepz[mode]);
        fTrees[i]->Branch(Form("metprimepz%i"   , mode), &fTreeVars.metprimepz   [mode]);
        fTrees[i]->Branch(Form("leponeprimee%i" , mode), &fTreeVars.leponeprimee [mode]);
        fTrees[i]->Branch(Form("leptwoprimee%i" , mode), &fTreeVars.leptwoprimee [mode]);
        fTrees[i]->Branch(Form("metprimee%i"    , mode), &fTreeVars.metprimee    [mode]);
      }

      fTrees[i]->Branch("eventweight",        &fTreeVars.eventweightMVA    );
      fTrees[i]->Branch("fulleventweight",    &fTreeVars.fulleventweight   );
      fTrees[i]->Branch("fulleventweightlum", &fTreeVars.fulleventweightlum);
      fTrees[i]->Branch("eventcategory",      &fTreeVars.eventcategory     );
      fTrees[i]->Branch("category",           &fTreeVars.category          );
      fTrees[i]->Branch("train",              &fTreeVars.train             );
      fTrees[i]->Branch("issignal",           &fTreeVars.issignal          );
      fTrees[i]->Branch("type",               &fTreeVars.type              );

      fTrees[i]->Branch("jettotaunonclosure", &fTreeVars.jettotaunonclosure);
      fTrees[i]->Branch("zptup"             , &fTreeVars.zptup             );
      fTrees[i]->Branch("zptdown"           , &fTreeVars.zptdown           );

      //add MVA score branches
      for(unsigned index = 0; index < fMVAConfig.names_.size(); ++index) {
        fTrees[i]->Branch(Form("mva%i", index), &fMvaOutputs[index]);
      }
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("CLFVTmpHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  FillBaseEventHistogram(Hist);

  //Additional j-->tau info
  Hist->hJetToTauWeightGroup ->Fill(jetToTauWeightGroup, genWeight*eventWeight);
  for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
    Hist->hJetToTauComps[ji]->Fill(fJetToTauComps[ji]);
    Hist->hJetToTauWts  [ji]->Fill(fJetToTauWts  [ji]);
  }
  //quantify the effect of using composition by the size of the full weight / W+Jets weight alone
  const float wj_jtau_wt = fJetToTauWts[JetToTauComposition::kWJets]*fJetToTauCorrs[JetToTauComposition::kWJets]*fJetToTauBiases[JetToTauComposition::kWJets];
  Hist->hJetToTauCompEffect->Fill((wj_jtau_wt > 0.) ? jetToTauWeightBias / wj_jtau_wt : 0., eventWeight*genWeight);

  //PU weights effect
  Hist->hNPV[1]              ->Fill(nPV                , genWeight*eventWeight*((puWeight > 0.) ? 1./puWeight : 1.));
  Hist->hNPU[1]              ->Fill(nPU                , genWeight*eventWeight*((puWeight > 0.) ? 1./puWeight : 1.));

  //Jet ID weights effect
  Hist->hNJets20[1]          ->Fill(nJets20            , genWeight*eventWeight/jetPUIDWeight);
  Hist->hNJets20Rej[1]       ->Fill(nJets20Rej         , genWeight*eventWeight/jetPUIDWeight);

  //b-jet studies
  Hist->hNBJets20M[0]        ->Fill(nBJets20M          , genWeight*eventWeight)      ;
  float nobwt = genWeight*eventWeight;
  if(btagWeight > 0.) nobwt /= btagWeight;
  Hist->hNBJets20[1]         ->Fill(nBJets20           , nobwt);
  Hist->hNBJets20M[1]        ->Fill(nBJets20M          , nobwt);
  Hist->hNBJets20L[1]        ->Fill(nBJets20L          , nobwt);

  //muon trigger studies
  Hist->hMuonTriggerStatus   ->Fill(muonTriggerStatus, genWeight*eventWeight)   ;

  //tau decay mode study
  Hist->hTauDecayMode[1]   ->Fill(tauDecayMode);


  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  const double lepDelR   = std::fabs(leptonOneP4->DeltaR(*leptonTwoP4));
  const double lepDelPhi = std::fabs(leptonOneP4->DeltaPhi(*leptonTwoP4));

  //for removing or alternate DY reweighting weights
  double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight; //no DY Z pT vs M weights
  float tmp_1, tmp_2, sys_reco_weight(1.);
  double recoweight = (fIsDY) ? bareweight*fZPtWeight.GetWeight(fYear, lepSys.Pt(), lepSys.M(), /*use reco weights*/ true, tmp_1, tmp_2, sys_reco_weight) : bareweight;
  sys_reco_weight *= bareweight;
  double zpt   = (!fIsDY || zPt <  0.  ) ? lepSys.Pt() : zPt; //for DY to use given Z pT and Mass
  double zmass = (!fIsDY || zMass <  0.) ? lepSys.M()  : zMass;
  if(zpt < 0.) zpt = 0.;
  if(zmass < 0.) zmass = 0.;

  Hist->hLepPt[1]     ->Fill(lepSys.Pt()            ,bareweight);
  Hist->hLepPt[2]     ->Fill(lepSys.Pt()            ,recoweight);
  Hist->hLepM[1]      ->Fill(lepSys.M()             ,bareweight);
  Hist->hLepM[2]      ->Fill(lepSys.M()             ,recoweight);
  Hist->hLepM[3]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepM[4]      ->Fill(lepSys.M()             ,eventWeight*genWeight);

  //2D histograms for DY reweighting
  Hist->hLepPtVsM[0]  ->Fill(lepSys.M(), lepSys.Pt(), eventWeight*genWeight);
  Hist->hLepPtVsM[1]  ->Fill(lepSys.M(), lepSys.Pt(), bareweight);
  Hist->hLepPtVsM[2]  ->Fill(lepSys.M(), lepSys.Pt(), recoweight);
  Hist->hLepPtVsM[3]  ->Fill(lepSys.M(), lepSys.Pt(), bareweight*zPtWeightSys);
  Hist->hLepPtVsM[4]  ->Fill(lepSys.M(), lepSys.Pt(), sys_reco_weight);
  Hist->hZPtVsM[0]    ->Fill(zmass, zpt, eventWeight*genWeight);
  Hist->hZPtVsM[1]    ->Fill(zmass, zpt, bareweight);
  Hist->hZPtVsM[2]    ->Fill(zmass, zpt, recoweight);
  Hist->hZPtVsM[3]    ->Fill(zmass, zpt, bareweight*zPtWeightSys);
  Hist->hZPtVsM[4]    ->Fill(zmass, zpt, sys_reco_weight);
  Hist->hZPt[0]       ->Fill(zpt  , eventWeight*genWeight);
  Hist->hZPt[1]       ->Fill(zpt  , bareweight);
  Hist->hZPt[2]       ->Fill(zpt  , recoweight);
  Hist->hZPt[3]       ->Fill(zpt  , bareweight*zPtWeightSys);
  Hist->hZPt[4]       ->Fill(zpt  , sys_reco_weight);
  Hist->hZMass[0]     ->Fill(zmass, eventWeight*genWeight);
  Hist->hZMass[1]     ->Fill(zmass, bareweight);
  Hist->hZMass[2]     ->Fill(zmass, recoweight);
  Hist->hZMass[3]     ->Fill(zmass, bareweight*zPtWeightSys);
  Hist->hZMass[4]     ->Fill(zmass, sys_reco_weight);

  //QCD studies
  const Float_t wt_noqcd     = (qcdWeight > 0.) ? eventWeight*genWeight/qcdWeight : eventWeight*genWeight;
  const Float_t wt_noqcdcl   = (qcdClosure > 0.) ? eventWeight*genWeight/qcdClosure : eventWeight*genWeight;
  const Float_t wt_noantiiso = (qcdIsoScale > 0.) ? eventWeight*genWeight/qcdIsoScale : eventWeight*genWeight;
  Hist->hLepDeltaPhi[1]->Fill(lepDelPhi             ,wt_noqcd);
  Hist->hLepDeltaR[1] ->Fill(lepDelR                ,wt_noqcd);
  Hist->hLepDeltaR[2] ->Fill(lepDelR                ,eventWeight*genWeight); //same binning as scale factor measurement
  Hist->hLepDelRVsOneEta[0]->Fill(std::fabs(fTreeVars.leponeeta), lepDelR, eventWeight*genWeight);
  Hist->hLepDelRVsOneEta[1]->Fill(std::fabs(fTreeVars.leponeeta), lepDelR, wt_noqcd);
  Hist->hLepDelPhiVsOneEta[0]->Fill(std::fabs(fTreeVars.leponeeta), lepDelPhi, eventWeight*genWeight);
  Hist->hLepDelPhiVsOneEta[1]->Fill(std::fabs(fTreeVars.leponeeta), lepDelPhi, wt_noqcd);
  //Jet-binned QCD transfer factor measurement histograms
  if(nJets == 0) {
    Hist->hQCDDelRJ[0] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hQCDOnePtVsTwoPtJ[0]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl);
  } else if(nJets == 1) {
    Hist->hQCDDelRJ[1] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hQCDOnePtVsTwoPtJ[1]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl);
  } else {
    Hist->hQCDDelRJ[2] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hQCDOnePtVsTwoPtJ[2]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl);
  }
  Hist->hQCDOnePtVsTwoPtJ[3]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl); //jet-inclusive correction
  Hist->hQCDOnePtVsTwoPtIso[0]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcd); //no QCD SS-->OS weights
  Hist->hQCDOnePtVsTwoPtIso[1]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noantiiso); //no muon anti-iso --> iso weight

  //Histograms for jet --> tau_h scale factors
  // Hist->hLooseLep     ->Fill(isLooseElectron + 2*isLooseMuon + 4*isLooseTau, eventWeight*genWeight);
  if(nTaus == 1 && (std::abs(leptonTwoFlavor) == 15) ) {
    for(UInt_t itau = 0; itau < nTaus; ++itau) {
      int dm = -1;
      const int njets = std::min(2, (int) nJets); //0, 1, or >= 2 jets
      bool fakeMC = fIsData == 0 && std::abs(TauFlavorFromID(Tau_genPartFlav[itau])) == 26;
      if(std::fabs(Tau_eta[itau]) > 2.3) continue;
      if(Tau_idAntiMu[itau] < 2) continue; //ignore ones that fail the old MVA anti-mu tight ID
      float tau_wt = 1.; //FIXME: Add an array of tau specific weights
      if(fIsData || tau_wt <= 0.) tau_wt = 1.;
      //remove j->tau weights if set
      if(jetToTauWeightBias > 0.) tau_wt *= 1./jetToTauWeightBias;
      if(!fakeMC) {
        Hist->hTausPt->Fill(Tau_pt[itau], tau_wt*eventWeight*genWeight);
        Hist->hTausEta->Fill(Tau_eta[itau], tau_wt*eventWeight*genWeight);
        Hist->hTausDM->Fill(Tau_decayMode[itau], tau_wt*eventWeight*genWeight);
        Hist->hTausAntiJet->Fill(std::log2(Tau_idDeepTau2017v2p1VSjet[itau]+1), tau_wt*eventWeight*genWeight);
        Hist->hTausAntiEle->Fill(std::log2(Tau_idDeepTau2017v2p1VSe[itau]+1), tau_wt*eventWeight*genWeight);
        Hist->hTausAntiMu ->Fill(Tau_idDeepTau2017v2p1VSmu[itau]              , tau_wt*eventWeight*genWeight);
        Hist->hTausMVAAntiMu->Fill(Tau_idAntiMu[itau]         , tau_wt*eventWeight*genWeight);
        Hist->hTausGenFlavor->Fill(TauFlavorFromID(Tau_genPartFlav[itau]), tau_wt*eventWeight*genWeight);
        TLorentzVector tausLV;
        tausLV.SetPtEtaPhiM(Tau_pt[itau], Tau_eta[itau], Tau_phi[itau], Tau_mass[itau]);
        Hist->hTausDeltaR->Fill(std::fabs(tausLV.DeltaR(*leptonOneP4)), tau_wt*eventWeight*genWeight);
      } else {
        Hist->hFakeTausPt->Fill(Tau_pt[itau], tau_wt*eventWeight*genWeight);
        Hist->hFakeTausEta->Fill(Tau_eta[itau], tau_wt*eventWeight*genWeight);
        Hist->hFakeTausDM->Fill(Tau_decayMode[itau], tau_wt*eventWeight*genWeight);
      }

      if     (Tau_decayMode[itau] == 0 ) dm = 0;
      else if(Tau_decayMode[itau] == 1 ) dm = 1;
      else if(Tau_decayMode[itau] == 10) dm = 2;
      else if(Tau_decayMode[itau] == 11) dm = 3;
      else if(Tau_decayMode[itau] < 0  ) dm = 0; //only happens in tree version with bug --> default to progress until fixed
      else continue; //non-accepted decay mode

      if(fakeMC) { //MC fake tau
        Hist->hFakeTauMCNJetDMPtEta[0][njets][dm]->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //all taus
      } else { //genuine MC tau or data tau
        Hist->hFakeTauNJetDMPtEta[0][njets][dm]  ->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //all taus
      }
      if(Tau_idDeepTau2017v2p1VSjet[itau] > fFakeTauIsoCut) { //isolated tau
        if(fakeMC) { //MC fake tau
          Hist->hFakeTauMCNJetDMPtEta[1][njets][dm]->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
        } else {
          Hist->hFakeTauNJetDMPtEta[1][njets][dm]  ->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
        }
      } else { //anti-isolated tau
        if(fakeMC) { //MC fake tau
          Hist->hFakeTauMCNJetDMPtEta[2][njets][dm]->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
        } else { //genuine MC tau or data tau
          Hist->hFakeTauNJetDMPtEta[2][njets][dm]  ->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
        }
      }
      // jet --> tau non-closure histograms
      Float_t wt_no_nonclosure(eventWeight*genWeight), wt_no_biascorr(eventWeight*genWeight);
      if(jetToTauWeightCorr > 0.) wt_no_nonclosure *= jetToTauWeight / jetToTauWeightBias; //remove non-closure and bias corrections
      if(jetToTauWeightBias > 0.) wt_no_biascorr   *= jetToTauWeightCorr / jetToTauWeightBias; //remove the bias correction

      Hist->hJetTauDeltaR[0]->Fill(fTreeVars.lepdeltar, eventWeight*genWeight);
      Hist->hJetTauDeltaR[1]->Fill(fTreeVars.lepdeltar, wt_no_nonclosure     );
      Hist->hJetTauDeltaR[2]->Fill(fTreeVars.lepdeltar, wt_no_biascorr       );
      Hist->hJetTauLepM  [0]->Fill(fTreeVars.lepm     , eventWeight*genWeight);
      Hist->hJetTauLepM  [1]->Fill(fTreeVars.lepm     , wt_no_nonclosure     );
      Hist->hJetTauLepM  [2]->Fill(fTreeVars.lepm     , wt_no_biascorr       );
      Hist->hJetTauMTLep [0]->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
      Hist->hJetTauMTLep [1]->Fill(fTreeVars.mtlep    , wt_no_nonclosure     );
      Hist->hJetTauMTLep [2]->Fill(fTreeVars.mtlep    , wt_no_biascorr       );
      Hist->hJetTauMTOne [0]->Fill(fTreeVars.mtone    , eventWeight*genWeight);
      Hist->hJetTauMTOne [1]->Fill(fTreeVars.mtone    , wt_no_nonclosure     );
      Hist->hJetTauMTOne [2]->Fill(fTreeVars.mtone    , wt_no_biascorr       );
      Hist->hJetTauMTTwo [0]->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
      Hist->hJetTauMTTwo [1]->Fill(fTreeVars.mttwo    , wt_no_nonclosure     );
      Hist->hJetTauMTTwo [2]->Fill(fTreeVars.mttwo    , wt_no_biascorr       );
    }
  } //end j-->tau scale factor section

  //ATLAS boosted frame variables
  for(int mode = 0; mode < 3; ++mode) {
    Hist->hLepOnePrimePx[mode]->Fill(fTreeVars.leponeprimepx[mode], eventWeight*genWeight);
    Hist->hLepTwoPrimePx[mode]->Fill(fTreeVars.leptwoprimepx[mode], eventWeight*genWeight);
    Hist->hMetPrimePx   [mode]->Fill(fTreeVars.metprimepx   [mode], eventWeight*genWeight);
    Hist->hLepOnePrimePy[mode]->Fill(fTreeVars.leponeprimepy[mode], eventWeight*genWeight);
    Hist->hLepTwoPrimePy[mode]->Fill(fTreeVars.leptwoprimepy[mode], eventWeight*genWeight);
    Hist->hMetPrimePy   [mode]->Fill(fTreeVars.metprimepy   [mode], eventWeight*genWeight);
    Hist->hLepOnePrimePz[mode]->Fill(fTreeVars.leponeprimepz[mode], eventWeight*genWeight);
    Hist->hLepTwoPrimePz[mode]->Fill(fTreeVars.leptwoprimepz[mode], eventWeight*genWeight);
    Hist->hMetPrimePz   [mode]->Fill(fTreeVars.metprimepz   [mode], eventWeight*genWeight);
    Hist->hLepOnePrimeE [mode]->Fill(fTreeVars.leponeprimee [mode], eventWeight*genWeight);
    Hist->hLepTwoPrimeE [mode]->Fill(fTreeVars.leptwoprimee [mode], eventWeight*genWeight);
    Hist->hMetPrimeE    [mode]->Fill(fTreeVars.metprimee    [mode], eventWeight*genWeight);
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  HistMaker::FillPhotonHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::FillLepHistogram(LepHist_t* Hist) {
  FillBaseLepHistogram(Hist);

  /////////////
  //  Lep 1  //
  /////////////
  //trigger weight variations
  float wt = (fIsData) ? 1.f : std::max(0.001f, leptonOneTrigWeight * leptonTwoTrigWeight);
  Hist->hOnePt[1]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  Hist->hOnePt[2]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[0]);
  Hist->hOnePt[3]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[1]);
  Hist->hOnePt[4]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[2]);
  Hist->hOnePt[5]->Fill(fTreeVars.leponept, eventWeight*genWeight/leptonOneTrigWeight);
  //remove ID weights
  wt = (leptonOneWeight1 > 0.) ? leptonOneWeight1 : 1.;
  Hist->hOnePt[6]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  wt = (leptonOneWeight2 > 0.) ? leptonOneWeight2 : 1.;
  Hist->hOnePt[7]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  //remove Z pT weight
  wt = (zPtWeight > 0.) ? zPtWeight : 1.;
  Hist->hOnePt[8]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  //remove b-tag weight
  wt = (btagWeight > 0.) ? btagWeight : 1.;
  Hist->hOnePt[9]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  // //remove j->tau weight
  wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
  Hist->hOnePt[10]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  //remove j->tau weight correction + bias correction
  wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
  Hist->hOnePt[11]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);

  if(nTaus == 1 && ((std::abs(leptonTwoFlavor) == 15 && tauDecayMode%10 < 2) || (Tau_decayMode[0]%10 < 2))) {
    int dmr = (std::abs(leptonTwoFlavor) == 15) ? tauDecayMode : Tau_decayMode[0];
    if(dmr > 9) dmr -= (10 - 2); //10,11 --> 2,3
    dmr += 1; //dmr = DM ID + 1, so 0 can be inclusive
    dmr = std::min(4, dmr); //ensure it's not out of bounds
    TLorentzVector taulv;
    if(std::abs(leptonTwoFlavor) == 15) taulv = *leptonTwoP4;
    else taulv.SetPtEtaPhiM(Tau_pt[0], Tau_eta[0], Tau_phi[0], Tau_mass[0]);

    //FIXME: Remove tau pT region histograms
    float wt_nojt(eventWeight*genWeight), wt_nocorr(eventWeight*genWeight), wt_nobias(eventWeight*genWeight);
    if(jetToTauWeightBias > 0.) wt_nojt   *= 1.                 / jetToTauWeightBias; // remove the entire weight
    if(jetToTauWeightBias > 0.) wt_nocorr *= jetToTauWeight     / jetToTauWeightBias; // remove the non-closure and bias corrections
    if(jetToTauWeightBias > 0.) wt_nobias *= jetToTauWeightCorr / jetToTauWeightBias; // remove the bias corrections
    double dr = std::fabs(leptonOneP4->DeltaR(taulv));
    Hist->hJetTauOnePt[0]       ->Fill(fTreeVars.leponept, wt_nocorr);
    Hist->hJetTauOnePt[dmr]     ->Fill(fTreeVars.leponept, wt_nocorr);
    Hist->hJetTauOnePtQCD[0]    ->Fill(fTreeVars.leponept, eventWeight*genWeight);
    Hist->hJetTauOnePtQCD[1]    ->Fill(fTreeVars.leponept, wt_nocorr);
    Hist->hJetTauOnePtVsR[0]    ->Fill(fTreeVars.leponept, dr, wt_nocorr);
    Hist->hJetTauOnePtVsR[dmr]  ->Fill(fTreeVars.leponept, dr, wt_nocorr);
    Hist->hJetTauOneR->Fill(dr, wt_nocorr);
    Hist->hJetTauOneEta->Fill(std::fabs(leptonOneP4->Eta()), wt_nocorr);
    Hist->hJetTauOneMetDeltaPhi[0]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr); //inclusive
    Hist->hJetTauOneMetDeltaPhi[dmr]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr);
    Hist->hJetTauOneMetDPhiQCD[0]->Fill(fTreeVars.onemetdeltaphi, eventWeight*genWeight); //inclusive
    Hist->hJetTauOneMetDPhiQCD[1]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr); //inclusive
    Hist->hJetTauOneRelIso[0]->Fill(fTreeVars.leponereliso, eventWeight*genWeight);
    Hist->hJetTauOneRelIso[1]->Fill(fTreeVars.leponereliso, wt_nobias);

    Hist->hJetTauOneMetDPhiComp->Fill(fTreeVars.onemetdeltaphi, wt_nojt); //compositions
    Hist->hJetTauTwoMetDPhiComp->Fill(fTreeVars.twometdeltaphi, wt_nojt);
    Hist->hJetTauOnePtComp     ->Fill(fTreeVars.leponept       , wt_nojt);
    Hist->hJetTauTwoPtComp     ->Fill(fTreeVars.leptwopt       , wt_nojt);
    Hist->hJetTauMTOneComp     ->Fill(fTreeVars.mtone         , wt_nojt);
    Hist->hJetTauMTTwoComp     ->Fill(fTreeVars.mttwo         , wt_nojt);

    dr = std::fabs(leptonTwoP4->DeltaR(taulv));
    Hist->hJetTauTwoPt[0]       ->Fill(fTreeVars.leptwopt, wt_nocorr);
    Hist->hJetTauTwoPt[dmr]     ->Fill(fTreeVars.leptwopt, wt_nocorr);
    Hist->hJetTauTwoPtVsR[0]    ->Fill(fTreeVars.leptwopt, dr, wt_nocorr);
    Hist->hJetTauTwoPtVsR[dmr]  ->Fill(fTreeVars.leptwopt, dr, wt_nocorr);
    Hist->hJetTauTwoR[0]->Fill(dr, eventWeight*genWeight);
    Hist->hJetTauTwoR[1]->Fill(dr, wt_nocorr);
    Hist->hJetTauTwoEta[0]->Fill(std::fabs(leptonTwoP4->Eta()), eventWeight*genWeight);
    Hist->hJetTauTwoEta[1]->Fill(std::fabs(leptonTwoP4->Eta()), wt_nocorr);
    Hist->hJetTauTwoEtaQCD[0]->Fill(std::fabs(leptonTwoP4->Eta()), eventWeight*genWeight);
    Hist->hJetTauTwoEtaQCD[1]->Fill(std::fabs(leptonTwoP4->Eta()), wt_nocorr);
    Hist->hJetTauTwoPtVsOnePt->Fill(fTreeVars.leponept, taulv.Pt(), wt_nocorr);
    Hist->hJetTauTwoMetDeltaPhi[0]->Fill(fTreeVars.twometdeltaphi, eventWeight*genWeight);
    Hist->hJetTauTwoMetDeltaPhi[1]->Fill(fTreeVars.twometdeltaphi, wt_nocorr);
    Hist->hJetTauTwoRelIso[0]->Fill(fTreeVars.leptworeliso, eventWeight*genWeight);
    Hist->hJetTauTwoRelIso[1]->Fill(fTreeVars.leptworeliso, wt_nobias);
  }

  /////////////
  //  Lep 2  //
  /////////////

  //trigger weight variations
  wt = (fIsData) ? 1.f : std::max(0.001f, leptonOneTrigWeight * leptonTwoTrigWeight);
  Hist->hTwoPt[1]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  Hist->hTwoPt[2]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[0]);
  Hist->hTwoPt[3]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[1]);
  Hist->hTwoPt[4]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[2]);
  Hist->hTwoPt[5]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/leptonTwoTrigWeight);
  //remove ID weights
  wt = leptonTwoWeight1;
  Hist->hTwoPt[6]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  wt = leptonTwoWeight2;
  Hist->hTwoPt[7]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove Z pT weight
  wt = zPtWeight;
  Hist->hTwoPt[8]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove b-tag weight
  wt = btagWeight;
  Hist->hTwoPt[9]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove j->tau weight
  wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
  Hist->hTwoPt[10]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove j->tau weight correction
  wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
  Hist->hTwoPt[11]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);

}

//--------------------------------------------------------------------------------------------------------------
void CLFVTmpHistMaker::FillSystematicHistogram(SystematicHist_t* Hist) {
  bool isSameFlavor = std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor);
  bool isMuTau = std::abs(leptonOneFlavor) == 13 && std::abs(leptonTwoFlavor) == 15;
  bool isETau  = std::abs(leptonOneFlavor) == 13 && std::abs(leptonTwoFlavor) == 15;
  for(int sys = 0; sys < kMaxSystematics; ++sys) {
    float weight = eventWeight*genWeight;
    if(weight == 0.) continue; //no way to re-scale 0, contributes nothing to histograms so can just skip
    TLorentzVector lv1 = *leptonOneP4;
    TLorentzVector lv2 = *leptonTwoP4;
    if(sys == 0) weight = weight;                                          //do nothing
    else if  (sys ==  1) {                                                 //electron ID scale factors
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  2) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  3) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_sys / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
    } else if(sys ==  4) {                                                 //muon ID scale factors
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  5) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  6) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_sys  / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_sys  / leptonTwoWeight1;
    } else if(sys ==  7) {                                                 //tau ID scale factors
      if(std::abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  8) {
      if(std::abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  9) {
      if(std::abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_sys  / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_sys  / leptonTwoWeight1;
    } else if(sys == 10) weight *= jetToTauWeightUp     / jetToTauWeight ; //Jet --> tau weights
    else if  (sys == 11) weight *= jetToTauWeightDown   / jetToTauWeight ;
    else if  (sys == 12) weight *= jetToTauWeightSys    / jetToTauWeight ;
    else if  (sys == 13) weight *= zPtWeightUp          / zPtWeight      ; //Drell-Yan pT weights
    else if  (sys == 14) weight *= zPtWeightDown        / zPtWeight      ;
    else if  (sys == 15) weight *= zPtWeightSys         / zPtWeight      ;
    else if  (sys == 16) {                                                 //electron reco ID scale factors
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_up / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_up / leptonTwoWeight2;
    } else if(sys == 17) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_down / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    } else if(sys == 18) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_sys / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_sys / leptonTwoWeight2;
    } else if(sys == 19) {                                                 //muon iso ID scale factors
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_up / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_up / leptonTwoWeight2;
    } else if(sys == 20) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_down / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    } else if(sys == 21) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_sys  / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_sys  / leptonTwoWeight2;
    }
    //FIXME: Need to re-evaluate variables using the updated tau four momentum
    else if  (sys == 22) {                                                 //Tau ES
      if(std::abs(leptonTwoFlavor) == 15 && tauES > 0. && tauES_up > 0.) {
        lv2 *= (tauES_up / tauES);
      }
    } else if(sys == 23) {
      if(std::abs(leptonTwoFlavor) == 15 && tauES > 0. && tauES_down > 0.) {
        lv2 *= (tauES_down / tauES);
      }
    }
    else if  (sys == 25) weight *= (qcdWeight > 0.) ? qcdWeightUp  / qcdWeight : 0.; //SS --> OS weights
    else if  (sys == 26) weight *= (qcdWeight > 0.) ? qcdWeightDown/ qcdWeight : 0.;
    else if  (sys == 27) weight *= (qcdWeight > 0.) ? qcdWeightSys / qcdWeight : 0.;
    else if  (sys == 28) weight *= jetToTauWeightCorrUp     / jetToTauWeightCorr   ; //Jet --> tau weight pT corrections
    else if  (sys == 29) weight *= jetToTauWeightCorrDown   / jetToTauWeightCorr   ;
    else if  (sys == 30) weight *= jetToTauWeightCorrSys    / jetToTauWeightCorr   ;
    else if  (sys == 31) weight *= jetToTauWeight_compUp    / jetToTauWeightBias   ; //Jet --> tau weight composition
    else if  (sys == 32) weight *= jetToTauWeight_compDown  / jetToTauWeightBias   ;
    else if  (sys == 33) weight *= jetToTauWeight_compUp    / jetToTauWeightBias   ;
    else if(sys > 33 && sys < 43) {                                                // tau IDs by PDG ID
      if(std::abs(leptonTwoFlavor) == 15) {
        if(sys == 34 && std::abs(tauGenFlavor) == 15)       //tau anti-jet ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 35 && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 36 && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
        else if(sys == 37 && std::abs(tauGenFlavor) == 13)  //tau anti-mu ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 38 && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 39 && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
        else if(sys == 40 && std::abs(tauGenFlavor) == 11)  //tau anti-ele ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 41 && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 42 && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
      }
    } else if  (sys == 43) { weight *= (fIsData || fIsEmbed) ? 1. : 1. + 0.016; //luminosity uncertainty
    } else if  (sys == 44) { weight *= (fIsData || fIsEmbed) ? 1. : 1. - 0.016;
    } else if  (sys == 45) { weight *= (btagWeight > 0.) ? btagWeightUp   / btagWeight : 1.; //btag uncertainty
    } else if  (sys == 46) { weight *= (btagWeight > 0.) ? btagWeightDown / btagWeight : 1.;
    } else if  (sys == 47) { weight *= jetToTauWeightBiasUp     / jetToTauWeightBias   ; //Jet --> tau weight bias corrections
    } else if  (sys == 48) { weight *= jetToTauWeightBiasDown   / jetToTauWeightBias   ;

      //Removed j->tau groupings since now using fitter errors uncorrelated by years
    } else if  (sys >= SystematicGrouping::kJetToTau && sys < 100) { //Jet --> tau systematic groupings, (4 DM)*(2 eta)*(3 years)*(2 up/down) = 48 histograms
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int offset = SystematicGrouping::kJetToTau;
      if(2*(jetToTauWeightGroup) - offset     == sys) weight *= jetToTauWeightUp   / jetToTauWeight;
      if(2*(jetToTauWeightGroup) - offset + 1 == sys) weight *= jetToTauWeightDown / jetToTauWeight;
      //Removed muon ID groupings since it appears to be small enough to treat correlated
    // } else if  (sys >= SystematicGrouping::kMuonID && sys < 150) { //Muon ID
    //   int offset = SystematicGrouping::kMuonID;
    //   if(std::abs(leptonOneFlavor) == 13) {
    //     if(2*(leptonOneWeight1_group) - offset     == sys) weight *= leptonOneWeight1_up   / leptonOneWeight1;
    //     if(2*(leptonOneWeight1_group) - offset + 1 == sys) weight *= leptonOneWeight1_down / leptonOneWeight1;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 13) {
    //     if(2*(leptonTwoWeight1_group) - offset     == sys) weight *= leptonTwoWeight1_up   / leptonTwoWeight1;
    //     if(2*(leptonTwoWeight1_group) - offset + 1 == sys) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    //   }
      //Removed muon iso ID groupings since it appears to be small enough to treat correlated
    // } else if  (sys >= SystematicGrouping::kMuonIsoID && sys < 200) { //Muon ID
    //   int offset = SystematicGrouping::kMuonIsoID;
    //   if(std::abs(leptonOneFlavor) == 13) {
    //     if(2*(leptonOneWeight2_group) - offset     == sys) weight *= leptonOneWeight2_up   / leptonOneWeight2;
    //     if(2*(leptonOneWeight2_group) - offset + 1 == sys) weight *= leptonOneWeight2_down / leptonOneWeight2;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 13) {
    //     if(2*(leptonTwoWeight2_group) - offset     == sys) weight *= leptonTwoWeight2_up   / leptonTwoWeight2;
    //     if(2*(leptonTwoWeight2_group) - offset + 1 == sys) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    //   }
    } else if  (sys >= 100 && sys < 106) { //tau anti-jet ID, separated by years
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int base = 100;
      if(std::abs(leptonTwoFlavor) == 15) {
        int bin = 2*(fYear - 2016); //uncorrelated between years
        if((sys == base + bin) && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + bin) && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    } else if  (sys >= 110 && sys < 140) { //tau anti-mu ID, separated by 5 bins per year --> 15 histograms per up/down
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int base = 110;
      if(std::abs(leptonTwoFlavor) == 15) {
        int bin = 5*(fYear - 2016) + leptonTwoWeight1_bin; //uncorrelated between years and bins
        if((sys == base + 2*bin) && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + 2*bin) && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    } else if  (sys >= 140 && sys < 158) { //tau anti-ele ID, separated by 3 bins per year --> 9 histograms per up/down
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int base = 140;
      if(std::abs(leptonTwoFlavor) == 15) {
        int bin = 3*(fYear - 2016) + leptonTwoWeight1_bin; //uncorrelated between years and bins
        if((sys == base + 2*bin) && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + 2*bin) && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    //  //Remove electron ID and Reco ID groupings, as generally systematically correlated errors
    // } else if  (sys >= SystematicGrouping::kElectronID && sys < 250) { //Electron ID
    //   int offset = SystematicGrouping::kElectronID;
    //   if(std::abs(leptonOneFlavor) == 11) {
    //     if(2*(leptonOneWeight1_group) - offset     == sys) weight *= leptonOneWeight1_up   / leptonOneWeight1;
    //     if(2*(leptonOneWeight1_group) - offset + 1 == sys) weight *= leptonOneWeight1_down / leptonOneWeight1;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 11) {
    //     if(2*(leptonTwoWeight1_group) - offset     == sys) weight *= leptonTwoWeight1_up   / leptonTwoWeight1;
    //     if(2*(leptonTwoWeight1_group) - offset + 1 == sys) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    //   }
    // } else if  (sys >= SystematicGrouping::kElectronRecoID && sys < 300) { //Electron Reco ID
    //   int offset = SystematicGrouping::kElectronRecoID;
    //   if(std::abs(leptonOneFlavor) == 11) {
    //     if(2*(leptonOneWeight2_group) - offset     == sys) weight *= leptonOneWeight2_up   / leptonOneWeight2;
    //     if(2*(leptonOneWeight2_group) - offset + 1 == sys) weight *= leptonOneWeight2_down / leptonOneWeight2;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 11) {
    //     if(2*(leptonTwoWeight2_group) - offset     == sys) weight *= leptonTwoWeight2_up   / leptonTwoWeight2;
    //     if(2*(leptonTwoWeight2_group) - offset + 1 == sys) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    //   }
    } else continue; //no need to fill undefined systematics

    if(!std::isfinite(weight)) {
      std::cout << "CLFVTmpHistMaker::" << __func__ << ": Entry " << fentry << " Systematic " << sys << ": Weight is not finite! weight = " << weight << " event weight = "
                << eventWeight << ", setting to 0...\n";
      weight = 0.;
    }

    TLorentzVector lepSys = lv1 + lv2;

    Hist->hLepM  [sys]->Fill(lepSys.M()  , weight);
    //skip all other histograms in same-flavor selection, only using the M_{ll} histogram
    if(isSameFlavor) continue;
    Hist->hOnePt [sys]->Fill(lv1.Pt()    , weight);
    Hist->hTwoPt [sys]->Fill(lv2.Pt()    , weight);
    if(!fIsData)
      Hist->hWeightChange[sys]->Fill((eventWeight*genWeight != 0.) ? (eventWeight*genWeight - weight) / (eventWeight*genWeight) : 0.);
    //MVA outputs
    float mvaweight = fTreeVars.eventweightMVA*(weight/(eventWeight*genWeight));
    if(!std::isfinite(mvaweight)) {
      std::cout << "CLFVTmpHistMaker::" << __func__ << ": Entry " << fentry << " MVA weight is not finite! mvaweight = " << mvaweight << " eventweightMVA = "
                << fTreeVars.eventweightMVA << ", setting to 0...\n";
      mvaweight = 0.;
    }
    for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
      const int category = fMVAConfig.data_cat_[i];
      //Only fill relevant MVA histograms for the datset
      if(!((category == 1 && isMuTau) ||
           (category == 2 && isETau) ||
           (category == 0 && !(isMuTau || isETau)))
         ) continue;
      float mvascore = fMvaOutputs[i];
      if(!std::isfinite(mvascore) && fVerbose > 0) {
        std::cout << "CLFVTmpHistMaker::" << __func__ << ": Entry " << fentry << " MVA " << i << " score is not finite = " << mvascore << "! Setting to -2...\n";
        mvascore = -2.;
      }
      Hist->hMVA[i][sys]->Fill(mvascore, mvaweight);
    }
  }
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t CLFVTmpHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  //object pT thresholds
  float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////

  //Ignore loose lepton selection in same flavor category for now
  ee   &= !isLooseElectron;
  mumu &= !isLooseMuon;

  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: base selection
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 1);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //5

  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  TLorentzVector* muon_2 = 0;
  TLorentzVector* electron_2 = 0;
  //same flavor categories
  if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) {
    if(std::abs(leptonOneFlavor) == 13) { //mumu
      muon = leptonOneP4;
      muon_2 = leptonTwoP4;
    } else if(std::abs(leptonOneFlavor) == 11) { //ee
      electron = leptonOneP4;
      electron_2 = leptonTwoP4;
    }
  } else {
    if(std::abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
    else if(std::abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
    if(std::abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
    else if(std::abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
    if(std::abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
    else if(std::abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;
  }

  //lepton vectors must be found
  mutau = mutau && (tau != 0) && (muon != 0);
  etau  = etau  && (tau != 0) && (electron != 0);
  emu   = emu   && (muon != 0) && (electron != 0);
  mumu  = mumu  && (muon != 0) && (muon_2 != 0);
  ee    = ee    && (electron != 0) && (electron_2 != 0);


  //leptons must satisfy the pt requirements and fire a trigger
  mutau = mutau && muon->Pt() > muon_trig_pt_ && tau->Pt() > tau_pt && leptonOneFired;
  etau  = etau  && electron->Pt() > electron_trig_pt_ && tau->Pt() > tau_pt && leptonOneFired;
  emu   = emu   && ((electron->Pt() > electron_trig_pt_ && muon->Pt() > muon_pt && leptonOneFired) ||
                    (electron->Pt() > electron_pt && muon->Pt() > muon_trig_pt_ && leptonTwoFired));
  mumu  = mumu  && ((muon->Pt() > muon_trig_pt_ && muon_2->Pt() > muon_pt && leptonOneFired) ||
                    (muon->Pt() > muon_pt  && muon_2->Pt() > muon_trig_pt_ && leptonTwoFired));
  ee    = ee    && ((electron->Pt() > electron_trig_pt_ && electron_2->Pt() > electron_pt && leptonOneFired) ||
                    (electron->Pt() > electron_pt && electron_2->Pt() > electron_trig_pt_ && leptonTwoFired));

  //don't allow multiple muons/electrons in mumu/ee selections
  mumu &= nMuons     == 2;
  ee   &= nElectrons == 2;

  //reject electrons in the barrel/endcap gap region
  const float elec_gap_low(1.4442), elec_gap_high(1.566);
  etau &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  emu  &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  ee   &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  ee   &= elec_gap_low > std::fabs(leptonTwoSCEta) || std::fabs(leptonTwoSCEta) > elec_gap_high;

  ////////////////////////////////////////////////////////////
  // Set 2 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 2);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //6

  const double electron_eta_max = (fUseEmbedCuts) ? 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.2 : 2.3;

  mutau = mutau && std::fabs(muon->Eta()) < muon_eta_max;
  mutau = mutau && std::fabs(tau->Eta()) < tau_eta_max;
  mutau = mutau && std::fabs(tau->DeltaR(*muon)) > 0.3;

  etau  = etau  && std::fabs(electron->Eta()) < electron_eta_max;
  etau  = etau  && std::fabs(tau->Eta()) < tau_eta_max;
  etau  = etau  && std::fabs(tau->DeltaR(*electron)) > 0.3;

  emu   = emu   && std::fabs(electron->Eta()) < electron_eta_max;
  emu   = emu   && std::fabs(muon->Eta()) < muon_eta_max;
  emu   = emu   && std::fabs(muon->DeltaR(*electron)) > 0.3;

  mumu  = mumu  && std::fabs(muon->Eta()) < muon_eta_max;
  mumu  = mumu  && std::fabs(muon_2->Eta()) < muon_eta_max;
  mumu  = mumu  && std::fabs(muon->DeltaR(*muon_2)) > 0.3;

  ee    = ee    && std::fabs(electron->Eta()) < electron_eta_max;
  ee    = ee    && std::fabs(electron_2->Eta()) < electron_eta_max;
  ee    = ee    && std::fabs(electron->DeltaR(*electron_2)) > 0.3;

  //mass cuts, due to generation ranges need > 50 GeV for all sets
  const double mll = (*leptonOneP4+*leptonTwoP4).M();
  mutau &= mll > 51. && mll < 170.;
  etau  &= mll > 51. && mll < 170.;
  emu   &= mll > 51. && mll < 170.;
  mumu  &= mll > 51. && mll < 170.;
  ee    &= mll > 51. && mll < 170.;

  //ensure a trigger was fired
  mutau &= leptonOneFired || leptonTwoFired;
  etau  &= leptonOneFired || leptonTwoFired;
  emu   &= leptonOneFired || leptonTwoFired;
  mumu  &= leptonOneFired || leptonTwoFired;
  ee    &= leptonOneFired || leptonTwoFired;


  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  fCutFlow->Fill(icutflow); ++icutflow; //7

  ////////////////////////////////////////////////////////////
  // Set 3 + selection offset: eta, mass, and trigger cuts
  ////////////////////////////////////////////////////////////

  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 3);
  }

  ////////////////////////////////////////////////////////////
  // Set 4 + selection offset: add tau IDs
  ////////////////////////////////////////////////////////////

  mutau &= isLooseTau || tauDeepAntiJet >= 50; //63 = tight
  mutau &= tauDeepAntiMu  >= 10; //15 = tight
  mutau &= tauDeepAntiEle >= 10; //15 = loose
  mutau &= leptonTwoID2   >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  etau  &= isLooseTau || tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //15 = tight
  etau  &= tauDeepAntiEle >= 50; //63 = tight
  etau  &= leptonTwoID2   >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  //remove tau decay modes not interested in
  mutau &= tauDecayMode != 5 && tauDecayMode != 6;
  etau  &= tauDecayMode != 5 && tauDecayMode != 6;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 4);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //8

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //9
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //10
  ++icutflow;

  //cut-flow for fake leptons
  if(fIsData) fCutFlow->Fill(icutflow); //11
  else if(emu) {
    if((std::abs(leptonOneFlavor) == std::abs(leptonOneGenFlavor))
       && (std::abs(leptonTwoFlavor) == std::abs(leptonTwoGenFlavor))) {
      fCutFlow->Fill(icutflow);
    }
  }
  ++icutflow;

  //////////////////////////////////////////////////////////////
  //
  // Analysis cut section
  //
  //////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // jet --> tau cuts and region definitions
  ////////////////////////////////////////////////////////////

  const double met_cut       = 60.;
  const double mtlep_cut     = 70.;
  const double qcd_mtlep_cut = (etau) ? 45. : mtlep_cut;
  // const bool qcdSelection     = (etau || (mutau && leptonOneIso/fTreeVars.leponept > 0.05)) && fTreeVars.mtlep < 40. && met < met_cut && nBJetsUse == 0;
  const bool looseQCDRegion    = fTreeVars.mtlep < mtlep_cut && met < met_cut && nBJetsUse == 0; // no isolation cut (mutau, 0 - 0.5 allowed) and electron WPL + tight ID
  const bool qcdSelection      = looseQCDRegion && fTreeVars.mtlep < qcd_mtlep_cut && (etau || (mutau && fTreeVars.leponereliso > 0.05)) && !(isLooseMuon || isLooseElectron);
  const bool wjetsSelection    = (etau || mutau) && fTreeVars.mtlep > 70. && nBJetsUse == 0 && !(isLooseMuon || isLooseElectron);
  const bool topSelection      = (etau || mutau || emu) && nBJetsUse >= 1 && !(isLooseMuon || isLooseElectron);
  const bool nominalSelection  = nBJetsUse == 0 && met < met_cut && fTreeVars.mtlep < mtlep_cut && !(isLooseMuon || isLooseElectron);
  // const bool looseNominal      = nBJetsUse == 0 && !(isLooseMuon || isLooseElectron); //no MET or MT(ll, MET) cuts

  ////////////////////////////////////////////////////////////
  // Set 92 (93) + selection offset: QCD selection without iso/mtlep cut with (without) MC taus
  ////////////////////////////////////////////////////////////

  if((etau || mutau) && looseQCDRegion) {
    int qcd_set_offset = set_offset;
    if(looseQCDSelection && !isLooseTau && (isLooseMuon || isLooseElectron)) qcd_set_offset -= fMisIDOffset; //remove offset if the loose lepton isn't the tau
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = fUseJetToTauComposition && isLooseTau;
    const bool isfake = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);
    //QCD weights from DR
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr;//*fJetToTauBiases[JetToTauComposition::kQCD];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(mutau || (etau && !isLooseElectron)) { //full isolation/mtlep region for isolation/mtlep bias measurement, no bias corrections
      FillAllHistograms(qcd_set_offset + 92);
      if(!isfake) FillAllHistograms(qcd_set_offset + 93);
    }
    if(fTreeVars.mtlep < qcd_mtlep_cut && ((mutau && fTreeVars.leponereliso > 0.15) || (etau && isLooseElectron))) { //high isolation/loose ID region for SS --> OS bias measurement
      //QCD weights from high iso region, no bias corrections (stored in MC weights, but high iso weights)
      if(add_wt) {
        jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kQCD];
        jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kQCD];
        jetToTauWeightBias = jetToTauWeightCorr;//*fJetToTauMCBiases[JetToTauComposition::kQCD];
        eventWeight        = jetToTauWeightBias*evt_wt_bare;
      }
      FillAllHistograms(qcd_set_offset + 94);
      if(!isfake) FillAllHistograms(qcd_set_offset + 95);
    }
    //Restore the previous weights
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }


  //////////////////////////
  // Handle loose leptons //
  //////////////////////////

  //FIXME: add back in loose light leptons
  mutau &= !isLooseMuon;
  etau  &= !isLooseElectron;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //12
  ++icutflow;


  ////////////////////////////////////////////////////////////
  // Set 35 + selection offset: last set with MC estimated taus and leptons
  ////////////////////////////////////////////////////////////

  if(!(mumu || ee || emu) && nominalSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = fUseJetToTauComposition && isLooseTau;
    const bool isfake = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);

    //Nominal mixture
    FillAllHistograms(set_offset + 35);
    //fake tau only contribution
    if(isfake) FillAllHistograms(set_offset + 42);

    //Nominal mixture, without j-->tau weights
    if(add_wt) {
      jetToTauWeight     = 1.f;
      jetToTauWeightCorr = 1.f;
      jetToTauWeightBias = 1.f;
      eventWeight        = evt_wt_bare;
    }
    if(isLooseTau) {
      FillAllHistograms(set_offset + 33); //only fill for loose ID taus, since non-loose already have no j-->tau weights
    }

    //W+Jets data based weights from DR to test W+Jets only vs composition
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauBiases[JetToTauComposition::kWJets];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 34);
    }

    //W+Jets MC based weights from DR for bias tests
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauMCBiases[JetToTauComposition::kWJets];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 81);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 86);
    }
    //Top MC based weights from DR (nominal weights are MC in this region anyway)
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauMCBiases[JetToTauComposition::kTop];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 82);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 87);
    }
    //Restore the previous weights
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 36 + selection offset: QCD selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((fQCDFakeTauTesting || !fDYTesting) && qcdSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 36);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 37 + selection offset: W+Jets selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((fWJFakeTauTesting || !fDYTesting) && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool    add_wt      = fUseJetToTauComposition && isLooseTau;
    const bool    isfake      = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);
    //Data measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 37);
    //MC measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                 [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs[JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 88);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 89);
    }
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 38 + selection offset: ttbar selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((fTTFakeTauTesting || !fDYTesting) && (mutau || etau) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 38);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }


  //remove MC jet -> light lepton contribution
  if(!fUseMCEstimatedFakeLep && !fIsData) {
    emu   &= !isFakeMuon;
    emu   &= !isFakeElectron;
    mumu  &= !isFakeMuon;
    ee    &= !isFakeElectron;
    mutau &= !isFakeMuon;
    etau  &= !isFakeElectron;
  }

  //remove MC estimated jet --> tau component
  mutau &= fIsData > 0 || std::abs(tauGenFlavor) != 26;
  etau  &= fIsData > 0 || std::abs(tauGenFlavor) != 26;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //13
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 30 + selection offset: QCD selection
  ////////////////////////////////////////////////////////////////////////////
  if((fQCDFakeTauTesting || !fDYTesting) && qcdSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 30);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 31 + selection offset: W+Jets selection
  ////////////////////////////////////////////////////////////////////////////
  if((fWJFakeTauTesting || !fDYTesting) && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 31);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 32 + selection offset: Top selection
  ////////////////////////////////////////////////////////////////////////////
  if((fTTFakeTauTesting || !fDYTesting) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 32);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  mutau &= met < met_cut;
  etau  &= met < met_cut;
  emu   &= met < met_cut;

  //Add W+Jets selection orthogonality condition
  mutau &= fTreeVars.mtlep < mtlep_cut;
  etau  &= fTreeVars.mtlep < mtlep_cut;
  emu   &= fTreeVars.mtlep < mtlep_cut;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 7 + selection offset: No MC estimated fake taus, no b-jet cut
  ////////////////////////////////////////////////////////////////////////////
  if((!fDYTesting || (mumu || ee)) && fDoSystematics >= 0)
    FillAllHistograms(set_offset + 7);

  if(fCutFlowTesting) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //14
  ++icutflow;

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  mutau &= nBJetsUse == 0;
  etau  &= nBJetsUse == 0;
  emu   &= nBJetsUse == 0;
  mumu  &= nBJetsUse == 0;
  ee    &= nBJetsUse == 0;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  //Test selection for QCD using loose electron or both loose leptons
  if(emu && isLooseElectron) {
    ////////////////////////////////////////////////////////////////////////////
    // Set 70 + selection offset: loose electron + tight muon region
    ////////////////////////////////////////////////////////////////////////////
    if(!isLooseMuon) FillAllHistograms(set_offset + 70);
    ////////////////////////////////////////////////////////////////////////////
    // Set 71 + selection offset: loose electron + loose muon region
    ////////////////////////////////////////////////////////////////////////////
    else             FillAllHistograms(set_offset + 71);
  }

  //Enforce QCD selection only using loose muon ID
  emu &= !isLooseElectron;
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  if(!looseQCDSelection && chargeTest)                                          {fCutFlow->Fill(icutflow);} //15
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight) > 0.)             {fCutFlow->Fill(icutflow);} //16
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight*eventWeight) > 0.) {fCutFlow->Fill(icutflow);} //17
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  fTimes[3] = std::chrono::steady_clock::now(); //timer for initializing base objects
  FillAllHistograms(set_offset + 8);
  IncrementTimer(3, true);

  if((emu || mumu || ee) && (!fDYTesting || fTriggerTesting)) {
    if(emu) { //e+mu trigger testing
      ////////////////////////////////////////////////////////////////////////////
      // Set 60 + selection offset: Electron triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonOneP4->Pt() > electron_trig_pt_) {
        FillAllHistograms(set_offset + 60);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 61 + selection offset: Muon triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoFired && leptonTwoP4->Pt() > muon_trig_pt_) {
        FillAllHistograms(set_offset + 61);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 62 + selection offset: Both leptons triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonTwoFired) {
        FillAllHistograms(set_offset + 62);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 63 + selection offset: Electron triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneP4->Pt() > electron_trig_pt_) {
        FillAllHistograms(set_offset + 63);
      }
      ////////////////////////////////////////////////////////////////////////////
      // Set 66 + selection offset: electron not triggerable
      ////////////////////////////////////////////////////////////////////////////
      else {
        FillAllHistograms(set_offset + 66);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 64 + selection offset: muon triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoP4->Pt() > muon_trig_pt_) {
        FillAllHistograms(set_offset + 64);
      }
      ////////////////////////////////////////////////////////////////////////////
      // Set 67 + selection offset: muon not triggerable
      ////////////////////////////////////////////////////////////////////////////
      else {
        FillAllHistograms(set_offset + 67);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 65 + selection offset: Electron+Muon triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneP4->Pt() > electron_trig_pt_ && leptonTwoP4->Pt() > muon_trig_pt_) {
        FillAllHistograms(set_offset + 65);
      }
    } else { //ee and mumu trigger testing
      const double trig_threshold = (ee) ? electron_trig_pt_ : muon_trig_pt_;

      ////////////////////////////////////////////////////////////////////////////
      // Set 60 + selection offset: lepton one triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonOneP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 60);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 61 + selection offset: lepton two triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoFired && leptonTwoP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 61);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 62 + selection offset: both leptons triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonOneP4->Pt() > trig_threshold && leptonTwoFired && leptonTwoP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 62);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 63 + selection offset: lepton two triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 63);
      }
      ////////////////////////////////////////////////////////////////////////////
      // Set 66 + selection offset: lepton two not triggerable
      ////////////////////////////////////////////////////////////////////////////
      else {
        FillAllHistograms(set_offset + 66);
      }

    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 24-25 : Mass window sets
  ////////////////////////////////////////////////////////////////////////////
  //Z0 window
  if(emu && mll < 96. && mll > 85.)    FillAllHistograms(set_offset + 24);
  //higgs window
  if(emu && mll < 130. && mll > 120.)  FillAllHistograms(set_offset + 25);

  if(fDYTesting && !fDoMVASets) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 9-18 : BDT Cut categories
  ////////////////////////////////////////////////////////////////////////////
  //Set event weight to ignore training sample
  Float_t prev_wt = eventWeight;
  //only use weight if MC or is same sign data
  eventWeight = (fIsData == 0 || !chargeTest) ? std::fabs(fTreeVars.eventweightMVA) : std::fabs(prev_wt); //use abs to remove gen weight sign
  bool doMVASets = !fDYTesting || fDoMVASets;
  int category = -1; //histogram set to use, based on MVA score
  if(doMVASets) {
    //Total background Z0 MVAs: 9 - 13
    if(mutau) {
      category = Category("zmutau");
    } else if(etau) {
      category = Category("zetau");
    } else if(emu) {
      category = Category("zemu");
      FillAllHistograms(set_offset + 9 + category);
      category = Category("zmutau_e");
      FillAllHistograms(set_offset + kMuTauE - kEMu + 9 + category);
      category = Category("zetau_mu");
      FillAllHistograms(set_offset + kETauMu - kEMu + 9 + category);
    } else if(mumu) {
      category = Category("zemu");
    } else if(ee) {
      category = Category("zemu");
    }
  }
  if(category >= 0 && !emu) //do emu separately, rest do here
    FillAllHistograms(set_offset + 9 + category);

  //Total background higgs MVAs 14 - 18
  category = -1;
  if(doMVASets) {
    if(mutau) {
      category = Category("hmutau");
    } else if(etau) {
      category = Category("hetau");
    } else if(emu) {
      category = Category("hemu");
      FillAllHistograms(set_offset + 14 + category);
      category = Category("hmutau_e");
      FillAllHistograms(set_offset + kMuTauE - kEMu + 14 + category);
      category = Category("hetau_mu");
      FillAllHistograms(set_offset + kETauMu - kEMu + 14 + category);
    } else if(mumu) {
      category = Category("hemu");
    } else if(ee) {
      category = Category("hemu");
    }
  }

  if(category >= 0 && !emu) //do emu separately, rest do here
    FillAllHistograms(set_offset + 14 + category);

  //restore event weight
  eventWeight = prev_wt;

  if(fDYTesting) return kTRUE;


  return kTRUE;
}
