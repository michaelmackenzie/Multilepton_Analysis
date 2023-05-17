#define CLFVHISTMAKER_CXX

#include "interface/CLFVHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
CLFVHistMaker::CLFVHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {

}

//--------------------------------------------------------------------------------------------------------------
CLFVHistMaker::~CLFVHistMaker() {
  // HistMaker::~HistMaker();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::DeleteHistograms() {
  HistMaker::DeleteHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  HistMaker::Begin(nullptr);

}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillAllHistograms(Int_t index) {
  HistMaker::FillAllHistograms(index);
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i] = 0;
    fSysSets  [i] = 0;
    fTreeSets [i] = 0;
    fSetFills [i] = 0;
  }

  //Event Sets
  const bool mutau = fSelection == "" || fSelection == "mutau";
  const bool etau  = fSelection == "" || fSelection == "etau" ;
  const bool emu   = fSelection == "" || fSelection == "emu"  || lep_tau != 0;
  const bool mumu  = fSelection == "" || fSelection == "mumu" ;
  const bool ee    = fSelection == "" || fSelection == "ee"   ;

  if(mutau) {
    if(fCutFlowTesting) {
      fEventSets [kMuTau + 1] = 1; // all events
      fEventSets [kMuTau + 2] = 1; //
      fEventSets [kMuTau + 3] = 1;
      fEventSets [kMuTau + 4] = 1;
    }

    // fEventSets [kMuTau + 7] = 1;

    fEventSets [kMuTau + 8] = 1;
    fTreeSets  [kMuTau + 8] = 1;
    fTreeSets  [kMuTau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
    // fSysSets   [kMuTau + 8] = 1;

    // fEventSets [kMuTau + 20] = 1; //test set
    // fSysSets   [kMuTau + 20] = 1;
    // fEventSets [kMuTau + 21] = 1; //1-prong taus
    // fSysSets   [kMuTau + 21] = 1;
    // fEventSets [kMuTau + 22] = 1; //3-prong taus
    // fSysSets   [kMuTau + 22] = 1;

    fEventSets [kMuTau + 25] = 1; //nominal mass
    fSysSets   [kMuTau + 25] = 1;
    fEventSets [kMuTau + 26] = 1; //high mass
    fSysSets   [kMuTau + 26] = 1;
    fEventSets [kMuTau + 27] = 1; //low mass
    fSysSets   [kMuTau + 27] = 1;
    fEventSets [kMuTau + 28] = 1; //Z->ee/mumu
    fSysSets   [kMuTau + 28] = 1;

    // jet --> tau DRs
    fEventSets [kMuTau + 30] = fQCDFakeTauTesting; //QCD
    fEventSets [kMuTau + 31] = fWJFakeTauTesting; //W+Jets
    fEventSets [kMuTau + 32] = fTTFakeTauTesting; //Top

    // jet --> tau DRs with MC taus
    fEventSets [kMuTau + 33] = fJetTauTesting; //Nominal selection without j-->tau weights, loose ID only
    fEventSets [kMuTau + 35] = fFakeLeptonTesting; //Nominal selection, MC fake leptons
    fEventSets [kMuTau + 36] = fQCDFakeTauTesting; //QCD
    fEventSets [kMuTau + 37] = fWJFakeTauTesting; //W+Jets
    fEventSets [kMuTau + 38] = fTTFakeTauTesting; //Top

  }
  if(etau) {
    if(fCutFlowTesting) {
      fEventSets [kETau + 1] = 1; // all events
      fEventSets [kETau + 2] = 1;
      fEventSets [kETau + 3] = 1;
      fEventSets [kETau + 4] = 1;
    }

    // fEventSets [kETau + 7] = 1;

    fEventSets [kETau + 8] = 1;
    fTreeSets  [kETau + 8] = 1;
    fTreeSets  [kETau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
    // fSysSets   [kETau + 8] = 1;

    // fEventSets [kETau + 20] = 1; //test set
    // fSysSets   [kETau + 20] = 1;
    // fEventSets [kETau + 21] = 1; //1-prong taus
    // fSysSets   [kETau + 21] = 1;
    // fEventSets [kETau + 22] = 1; //3-prong taus
    // fSysSets   [kETau + 22] = 1;

    fEventSets [kETau + 25] = 1; //nominal mass
    fSysSets   [kETau + 25] = 1;
    fEventSets [kETau + 26] = 1; //high mass
    fSysSets   [kETau + 26] = 1;
    fEventSets [kETau + 27] = 1; //low mass
    fSysSets   [kETau + 27] = 1;
    fEventSets [kETau + 28] = 1; //Z->ee/mumu
    fSysSets   [kETau + 28] = 1;

    // jet --> tau DRs
    fEventSets [kETau + 30] = fQCDFakeTauTesting; //QCD
    fEventSets [kETau + 31] = fWJFakeTauTesting; //W+Jets
    fEventSets [kETau + 32] = fTTFakeTauTesting; //Top

    // jet --> tau DRs with MC taus
    fEventSets [kETau + 33] = fJetTauTesting; //Nominal selection without j-->tau weights, loose ID only
    fEventSets [kETau + 35] = fFakeLeptonTesting; //Nominal selection
    fEventSets [kETau + 36] = fQCDFakeTauTesting; //QCD
    fEventSets [kETau + 37] = fWJFakeTauTesting; //W+Jets
    fEventSets [kETau + 38] = fTTFakeTauTesting; //Top

  }
  if(emu) {
    if(fCutFlowTesting) {
      fEventSets [kEMu  + 1] = 1; // all events
      fEventSets [kEMu  + 2] = 1; // after pT cuts
      fEventSets [kEMu  + 3] = 1; // after eta, mass, trigger cuts
      fEventSets [kEMu  + 4] = 1; // after additional IDs
    }

    if(!lep_tau) {
      // fEventSets [kEMu  + 7] = 1;
      fEventSets [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training
      fSysSets   [kEMu  + 8] = 1;

      // fEventSets [kEMu  + 20] = 1; //test set
      // fSysSets   [kEMu  + 20] = 1;

      // MVA categories
      for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kEMu  + i] = 1;
      fSysSets[kEMu  + 9  + fMVAConfig->categories_["zemu"].size()] = 1; //most significant category
      fSysSets[kEMu  + 8  + fMVAConfig->categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kEMu  + 7  + fMVAConfig->categories_["zemu"].size()] = 1; //third most significant category
      if(fDoHiggs) {
        fSysSets[kEMu  + 14 + fMVAConfig->categories_["hemu"].size()] = 1; //start with most significant category
        fSysSets[kEMu  + 13 + fMVAConfig->categories_["hemu"].size()] = 1; //second most significant category
      }

      fEventSets [kEMu  + 24] = 1; // events within mass window
      if(fDoHiggs) {
        fEventSets [kEMu  + 25] = 1; // events within mass window
      }

      fEventSets [kEMu  + 32] = fTTFakeTauTesting; // > 0 b-jets (top selection)

      fEventSets [kEMu  + 35] = fFakeLeptonTesting;

      if(fTriggerTesting) { //testing e+mu triggering
        // fEventSets [kEMu  + 60] = 1; //electron fired
        // fEventSets [kEMu  + 61] = 1; //muon fired
        // fEventSets [kEMu  + 62] = 1; //both fired
        fEventSets [kEMu  + 63] = 1; //electron could trigger
        fEventSets [kEMu  + 64] = 1; //muon could trigger
        fEventSets [kEMu  + 65] = 1; //both could trigger
        fEventSets [kEMu  + 66] = 1; //electron can't trigger
        fEventSets [kEMu  + 67] = 1; //muon can't trigger
      }

    }
  }
  if(mumu) {
    if(fCutFlowTesting) {
      fEventSets [kMuMu + 1] = 1; // all events
      fEventSets [kMuMu + 2] = 1; // events with >= 1 photon
      fEventSets [kMuMu + 3] = 1;
      fEventSets [kMuMu + 4] = 1;
    }

    fEventSets [kMuMu + 7] = 1;
    fEventSets [kMuMu + 8] = 1;
    fSysSets   [kMuMu + 8] = 1;

    // MVA categories
    if(fDoMVASets) {
      for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kMuMu + i] = 1;
    }

    if(fTriggerTesting) { //testing triggering
      // fEventSets [kMuMu + 60] = 1; //one fired
      // fEventSets [kMuMu + 61] = 1; //two fired
      // fEventSets [kMuMu + 62] = 1; //both fired
      fEventSets [kMuMu + 63] = 1; //two could trigger
      fEventSets [kMuMu + 66] = 1; //two can't trigger
    }
  }
  if(ee) {
    if(fCutFlowTesting) {
      fEventSets [kEE   + 1] = 1; // all events
      fEventSets [kEE   + 2] = 1;
      fEventSets [kEE   + 3] = 1;
      fEventSets [kEE   + 4] = 1;
    }

    fEventSets [kEE   + 7] = 1;
    fEventSets [kEE   + 8] = 1;
    fSysSets   [kEE   + 8] = 1;

    // MVA categories
    if(fDoMVASets) {
      for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kEE + i] = 1;
    }

    if(fTriggerTesting) { //testing triggering
      // fEventSets [kEE  + 60] = 1; //one fired
      // fEventSets [kEE  + 61] = 1; //two fired
      // fEventSets [kEE  + 62] = 1; //both fired
      fEventSets [kEE  + 63] = 1; //two could trigger
      fEventSets [kEE  + 66] = 1; //two can't trigger
    }
  }

  //Leptonic tau channels
  if(lep_tau != 0) {
    //mu+tau_e
    if(lep_tau == 1) {
      fEventSets[kMuTauE + 8] = 1;
      fSysSets  [kMuTauE + 8] = 1;
      fTreeSets [kMuTauE + 8] = 1;
      fTreeSets [kMuTauE + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      fEventSets[kMuTauE + 25] = 1; //nominal mass
      fSysSets  [kMuTauE + 25] = 1;
      fEventSets[kMuTauE + 26] = 1; //high mass
      fSysSets  [kMuTauE + 26] = 1;
      fEventSets[kMuTauE + 27] = 1; //low mass
      fSysSets  [kMuTauE + 27] = 1;
      // // MVA categories
      // for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kMuTauE + i] = 1;
    }
    //e+tau_mu
    if(lep_tau == 2) {
      fEventSets[kETauMu + 8] = 1;
      fSysSets  [kETauMu + 8] = 1;
      fTreeSets [kETauMu + 8] = 1;
      fTreeSets [kETauMu + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      fEventSets[kETauMu + 25] = 1; //nominal mass
      fSysSets  [kETauMu + 25] = 1;
      fEventSets[kETauMu + 26] = 1; //high mass
      fSysSets  [kETauMu + 26] = 1;
      fEventSets[kETauMu + 27] = 1; //low mass
      fSysSets  [kETauMu + 27] = 1;
      // // MVA categories
      // for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kETauMu + i] = 1;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  // BookPhotonHistograms();
  if(fWriteTrees && fDoSystematics >= 0) BookTrees();
  if(fDoSystematics) BookSystematicHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookEventHistograms() {
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

      if(!fSparseHists) {
        //Additional j-->tau info
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

        //b-jet ID measurement histograms
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

        Utilities::BookH1F(fEventHist[i]->hZPt[5]     , "zpt5"          , Form("%s: Z Pt"           ,dirname)  ,     50,   0.,  150., folder);
        Utilities::BookH1F(fEventHist[i]->hZMass[5]   , "zmass5"        , Form("%s: Z Mass"         ,dirname)  ,     70,  40.,  180., folder);
        Utilities::BookH1F(fEventHist[i]->hZEta[1]    , "zeta1"         , Form("%s: ZEta"           ,dirname)  ,     50,  -10,    10, folder);
      }

      //boosted frame variables
      for(int mode = 0; mode < 3; ++mode) {
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePx[mode], Form("leponeprimepx%i", mode), Form("%s: l1 px" , dirname),  50, -100., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePx[mode], Form("leptwoprimepx%i", mode), Form("%s: l2 px" , dirname),  50, -100., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePx   [mode], Form("metprimepx%i"   , mode), Form("%s: met px", dirname),  50, -100., 100., folder);
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
void CLFVHistMaker::BookPhotonHistograms() {
  HistMaker::BookPhotonHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i] = folder;
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;

      BookBaseLepHistograms(i, dirname);

      for(int j = 1; j < 14; ++j) { //lepton pT with/without various weights
        TString name = Form("onept%i", j);
        Utilities::BookH1F(fLepHist[i]->hOnePt[j], name.Data(), Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      }

      // Lepton two info //
      for(int j = 1; j < 14; ++j) {
        TString name = Form("twopt%i", j);
        Utilities::BookH1F(fLepHist[i]->hTwoPt[j]     , name.Data(), Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      }

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookSystematicHistograms() {
  for(int i = 0; i < fQcdOffset; ++i) {
    if(fSysSets[i]) { //turn on all offset histogram sets
      fSysSets[i+fQcdOffset] = 1; //FIXME: Is this needed in etau/mutau?
      fSysSets[i+fMisIDOffset] = 1; //FIXME: Is this needed in emu/ee/mumu?
      fSysSets[i+fQcdOffset+fMisIDOffset] = 1; //FIXME: Is this histogram set needed?
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
        const TString name = fSystematics.GetName(sys);
        if(name == "") continue; //only initialize defined systematics

        Utilities::BookH1F(fSystematicHist[i]->hLepM        [sys], Form("lepm_%i"        , sys), Form("%s: LepM %i"                   , dirname, sys) , 280,  40, 180, folder);
        if(!fSparseHists) {
          Utilities::BookH1F(fSystematicHist[i]->hOnePt       [sys], Form("onept_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
          Utilities::BookH1F(fSystematicHist[i]->hTwoPt       [sys], Form("twopt_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
          Utilities::BookH1F(fSystematicHist[i]->hWeightChange[sys], Form("weightchange_%i", sys), Form("%s: Relative weight change %i" , dirname, sys) ,  30, -2.,   2, folder);
        }
        for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
          Utilities::BookH1D(fSystematicHist[i]->hMVA[j][sys], Form("mva%i_%i",j, sys), Form("%s: %s MVA %i" ,dirname, fMVAConfig->names_[j].Data(), sys),
                             fMVAConfig->NBins(j, i), fMVAConfig->Bins(j, i).data(), folder);
          Utilities::BookH1D(fSystematicHist[i]->hMVADiff[j][sys], Form("mvadiff%i_%i",j, sys)     , Form("%s: %s MVADiff %i" ,dirname, fMVAConfig->names_[j].Data(), sys),
                             25, -0.25, 0.25, folder);
        }
      }
      //Per book histograms
      Utilities::BookH1F(fSystematicHist[i]->hNFills, "nfills", Form("%s: NFills", dirname) , kMaxSystematics+1, 0, kMaxSystematics+1, folder);

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      fTrees[i] = new TTree(Form("tree_%i",i),Form("CLFVHistMaker TTree %i",i));
      BookBaseTree(i);
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("CLFVHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  FillBaseEventHistogram(Hist);

  if(!fSparseHists) {
    //Additional j-->tau info
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
    if(btagWeight > 0.f) nobwt /= btagWeight;
    Hist->hNBJets20[1]         ->Fill(nBJets20           , nobwt);
    Hist->hNBJets20M[1]        ->Fill(nBJets20M          , nobwt);
    Hist->hNBJets20L[1]        ->Fill(nBJets20L          , nobwt);

    //muon trigger studies
    Hist->hMuonTriggerStatus   ->Fill(muonTriggerStatus, genWeight*eventWeight)   ;

    //tau decay mode study
    Hist->hTauDecayMode[1]   ->Fill(tauDecayMode);


    TLorentzVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
    TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

    // const double lepDelR   = std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4));
    // const double lepDelPhi = std::fabs(leptonOne.p4->DeltaPhi(*leptonTwo.p4));

    //for removing or alternate DY reweighting weights
    double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight; //no DY Z pT vs M weights
    float tmp_1, tmp_2, sys_reco_weight(1.);
    double recoweight = (fIsDY) ? bareweight*fZPtWeight->GetWeight(fYear, lepSys.Pt(), lepSys.M(), /*use reco weights*/ true, tmp_1, tmp_2, sys_reco_weight) : bareweight;
    sys_reco_weight *= bareweight;

    //test Drell-Yan reweighting
    Hist->hLepPt[1]     ->Fill(lepSys.Pt()            ,bareweight);
    Hist->hLepPt[2]     ->Fill(lepSys.Pt()            ,recoweight);
    Hist->hLepM[1]      ->Fill(lepSys.M()             ,bareweight);
    Hist->hLepM[2]      ->Fill(lepSys.M()             ,recoweight);
    Hist->hLepM[3]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
    Hist->hLepM[4]      ->Fill(lepSys.M()             ,eventWeight*genWeight);

    const double zpt   = (zPt   <  0.f) ? lepSys.Pt()  : zPt;
    const double zmass = (zMass <  0.f) ? lepSys.M()   : zMass;
    Hist->hZPt[5]       ->Fill(zpt  , eventWeight*genWeight);
    Hist->hZMass[5]     ->Fill(zmass, eventWeight*genWeight);
    Hist->hZEta [1]     ->Fill(zEta , bareweight           );
  }

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
void CLFVHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  HistMaker::FillPhotonHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillLepHistogram(LepHist_t* Hist) {
  FillBaseLepHistogram(Hist);

  /////////////
  //  Lep 1  //
  /////////////
  //trigger weight variations
  float wt = (fIsData) ? 1.f : std::max(0.001f, leptonOne.trig_wt * leptonTwo.trig_wt);
  Hist->hOnePt[1]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  Hist->hOnePt[2]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[0]);
  Hist->hOnePt[3]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[1]);
  Hist->hOnePt[4]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[2]);
  Hist->hOnePt[5]->Fill(fTreeVars.leponept, eventWeight*genWeight/leptonOne.trig_wt);
  //remove ID weights
  wt = (leptonOne.wt1[0] > 0.) ? leptonOne.wt1[0] : 1.;
  Hist->hOnePt[6]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  wt = (leptonOne.wt2[0] > 0.) ? leptonOne.wt2[0] : 1.;
  Hist->hOnePt[7]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  //remove Z pT weight
  wt = (zPtWeight > 0.) ? zPtWeight : 1.;
  Hist->hOnePt[8]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  //remove b-tag weight
  wt = (btagWeight > 0.f) ? btagWeight : 1.f;
  Hist->hOnePt[9]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  // //remove j->tau weight
  wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
  Hist->hOnePt[10]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  //remove j->tau weight correction + bias correction
  wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
  Hist->hOnePt[11]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);

  //energy scale uncertainty shifted pT
  Hist->hOnePt[12]->Fill(leptonOne.pt*((fIsData) ? 1.f : leptonOne.ES[1]/leptonOne.ES[0]), eventWeight*genWeight);
  Hist->hOnePt[13]->Fill(leptonOne.pt*((fIsData) ? 1.f : leptonOne.ES[2]/leptonOne.ES[0]), eventWeight*genWeight);


  /////////////
  //  Lep 2  //
  /////////////

  //trigger weight variations
  wt = (fIsData) ? 1.f : std::max(0.001f, leptonOne.trig_wt * leptonTwo.trig_wt);
  Hist->hTwoPt[1]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  Hist->hTwoPt[2]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[0]);
  Hist->hTwoPt[3]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[1]);
  Hist->hTwoPt[4]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[2]);
  Hist->hTwoPt[5]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/leptonTwo.trig_wt);
  //remove ID weights
  wt = leptonTwo.wt1[0];
  Hist->hTwoPt[6]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  wt = leptonTwo.wt2[0];
  Hist->hTwoPt[7]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove Z pT weight
  wt = zPtWeight;
  Hist->hTwoPt[8]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove b-tag weight
  wt = (btagWeight > 0.f) ? btagWeight : 1.f;
  Hist->hTwoPt[9]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove j->tau weight
  wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
  Hist->hTwoPt[10]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove j->tau weight correction
  wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
  Hist->hTwoPt[11]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);

  //energy scale uncertainty shifted pT
  Hist->hTwoPt[12]->Fill(leptonTwo.pt*((fIsData) ? 1.f : leptonTwo.ES[1]/leptonTwo.ES[0]), eventWeight*genWeight);
  Hist->hTwoPt[13]->Fill(leptonTwo.pt*((fIsData) ? 1.f : leptonTwo.ES[2]/leptonTwo.ES[0]), eventWeight*genWeight);

}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillSystematicHistogram(SystematicHist_t* Hist) {
  if(!fDoSSSystematics && !chargeTest) return;
  if(!fDoLooseSystematics && looseQCDSelection) return;
  if(eventWeight*genWeight == 0.) return; //no way to re-scale 0, contributes nothing to histograms so can just skip filling

  const bool isSameFlavor = std::abs(leptonOne.flavor) == std::abs(leptonTwo.flavor);
  const bool isMuTau = leptonOne.isMuon    () && leptonTwo.isTau     ();
  const bool isETau  = leptonOne.isElectron() && leptonTwo.isTau     ();
  const bool isEData = leptonOne.isElectron() || leptonTwo.isElectron();
  const bool isMData = leptonOne.isMuon()     || leptonTwo.isMuon    ();
  const bool isEMu   = fSelection == "emu"; //for Z->e+mu, ignore many background-only effects

  //Event information that may be altered, to restore the event information after a systematic shift
  const TLorentzVector o_lv1(*leptonOne.p4), o_lv2(*leptonTwo.p4), o_jet(*jetOne.p4);
  const float o_met(met), o_metPhi(metPhi);
  float o_mvas[fMVAConfig->names_.size()], o_cdfs[fMVAConfig->names_.size()], o_fofp[fMVAConfig->names_.size()];
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    o_mvas[i] = fMvaOutputs[i];
    o_cdfs[i] = fMvaCDFs[i];
    o_fofp[i] = fMvaFofP[i];
  }

  const float rho = (fIsEmbed) ? 0.5f : 1.f; //for embedding correlation
  const float rho_t = (rho < 1.f) ? std::sqrt(1.f*1.f - rho*rho) : 0.f;
  //wt = 1 + delta --> = 1 + rho*delta = 1 + rho*(wt - 1) = rho*wt + (1-rho)

  const bool o_pass = PassesCuts();

  //count number of systematics filled
  int nfilled = 0;
  for(int sys = 0; sys < kMaxSystematics; ++sys) {
    float weight = eventWeight*genWeight;
    bool reeval = false;
    const TString name = fSystematics.GetName(sys);
    if(name == "") continue; //only process defined systematics

    if( (isMuTau || isETau || isSameFlavor || isEMu) && name.Contains("QCD")) continue; //only relevant to e+mu category MVA fits
    if(!(isMuTau || isETau) && name.Contains("Tau")) continue; //only relevant to tau categories
    if(name.BeginsWith("Emb") && !fIsEmbed) continue; //only relevant for embedding samples

    if(name == "Nominal")  weight = weight;                                //do nothing
    else if  (name == "EleID") {
      if(fIsData || !isEData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron()) weight *= (1.f-rho) + rho*leptonOne.wt1[1] / leptonOne.wt1[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
      } else {
        if(leptonOne.isElectron()) weight *= (1.f-rho) + rho*leptonOne.wt1[2] / leptonOne.wt1[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if  (name == "EmbEleID") {
      if(fIsData || !isEData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt1[1] / leptonOne.wt1[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
      } else {
        if(leptonOne.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt1[2] / leptonOne.wt1[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "EleRecoID") {
      if(fIsData || !isEData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron()) weight *= (1.f-rho) + rho*leptonOne.wt2[1] / leptonOne.wt2[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho) + rho*leptonTwo.wt2[1] / leptonTwo.wt2[0];
      } else {
        if(leptonOne.isElectron()) weight *= (1.f-rho) + rho*leptonOne.wt2[2] / leptonOne.wt2[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho) + rho*leptonTwo.wt2[2] / leptonTwo.wt2[0];
      }
    } else if(name == "EmbEleRecoID") {
      if(fIsData || !isEData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt2[1] / leptonOne.wt2[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt2[1] / leptonTwo.wt2[0];
      } else {
        if(leptonOne.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt2[2] / leptonOne.wt2[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt2[2] / leptonTwo.wt2[0];
      }
    } else if(name == "EleIsoID") {
      if(fIsData || !isEData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron()) weight *= (1.f-rho) + rho*leptonOne.wt3[1] / leptonOne.wt3[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho) + rho*leptonTwo.wt3[1] / leptonTwo.wt3[0];
      } else {
        if(leptonOne.isElectron()) weight *= (1.f-rho) + rho*leptonOne.wt3[2] / leptonOne.wt3[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho) + rho*leptonTwo.wt3[2] / leptonTwo.wt3[0];
      }
    } else if(name == "EmbEleIsoID") {
      if(!fIsEmbed || !isEData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt3[1] / leptonOne.wt3[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt3[1] / leptonTwo.wt3[0];
      } else {
        if(leptonOne.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt3[2] / leptonOne.wt3[0];
        if(leptonTwo.isElectron()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt3[2] / leptonTwo.wt3[0];
      }
    } else if(name == "MuonID") {
      if(fIsData || !isMData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho) + rho*leptonOne.wt1[1] / leptonOne.wt1[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
      } else {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho) + rho*leptonOne.wt1[2] / leptonOne.wt1[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "EmbMuonID") {
      if(fIsData || !isMData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt1[1] / leptonOne.wt1[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
      } else {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt1[2] / leptonOne.wt1[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "MuonIsoID") {
      if(fIsData || !isMData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho) + rho*leptonOne.wt2[1] / leptonOne.wt2[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho) + rho*leptonTwo.wt2[1] / leptonTwo.wt2[0];
      } else {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho) + rho*leptonOne.wt2[2] / leptonOne.wt2[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho) + rho*leptonTwo.wt2[2] / leptonTwo.wt2[0];
      }
    } else if(name == "EmbMuonIsoID") {
      if(fIsData || !isMData) continue;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt2[1] / leptonOne.wt2[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt2[1] / leptonTwo.wt2[0];
      } else {
        if(leptonOne.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonOne.wt2[2] / leptonOne.wt2[0];
        if(leptonTwo.isMuon    ()) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt2[2] / leptonTwo.wt2[0];
      }
    } else if(name == "JetToTauStat") {
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        if(fSystematics.IsUp(sys)) weight *= jetToTauWeightUp     / jetToTauWeight ;
        else                       weight *= jetToTauWeightDown   / jetToTauWeight ;
      }
    } else if(name.Contains("JetToTauStat")) { //process (3) and DM (4) binned stat. uncertainty
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        //recreate the weight with shifted process bias
        TString id_s = name; id_s.ReplaceAll("JetToTauStat", "");
        //bins 0,1,2,3 = W+Jets, 4,5,6,7 = Top, 8,9,10,11 = QCD
        const int id = std::abs(std::stoi(id_s.Data()));
        const int dm_bin = id % 4; //0->0, 1->1, 10->2, 11->3
        int proc = id / 4;
        if     (proc == 0) proc = JetToTauComposition::kWJets;
        else if(proc == 1) proc = JetToTauComposition::kTop;
        else if(proc == 2) proc = JetToTauComposition::kQCD;
        else {
          printf("CLFVHistMaker::%s: Unknown jet-->tau stat systematic bin %s\n", __func__, name.Data());
          continue;
        }
        float wt_jtt = 0.f;
        for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
          const bool test = (((iproc == proc || (proc == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets)))
                             && (tauDecayMode % 10 == 0 || tauDecayMode % 10 == 1) && ((tauDecayMode%10 + 2*(tauDecayMode/10)) == dm_bin));
          float base = fJetToTauComps[iproc] * fJetToTauCorrs[iproc];
          if(fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets)) {
            base *= fJetToTauBiases[proc];
          }
          wt_jtt += base * ((test) ?
                            (fSystematics.IsUp(sys)) ? fJetToTauWtsUp[iproc] : fJetToTauWtsDown[iproc]
                            : fJetToTauWts[iproc]);
        }
        if(wt_jtt <= 0.f || (fVerbose && std::abs(wt_jtt-jetToTauWeightBias)/wt_jtt > 2.f)) {
          printf("CLFVHistMaker::%s: Unexpected j-->tau weight variation! wt = %.3f, nominal = %.3f, proc = %i, dm_bin = %i, dm = %i\n",
                 __func__, wt_jtt, jetToTauWeightBias, proc, dm_bin, tauDecayMode);
        }
        weight *= wt_jtt / jetToTauWeightBias;
      }
    } else if(name.Contains("JetToTauAlt")) { //process (3) x DM (4) x fit param (3) binned stat. uncertainty
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        //recreate the weight with shifted process bias
        TString id_s = name; id_s.ReplaceAll("JetToTauAlt", "");
        int proc = 0;
        if     (id_s.BeginsWith("P0")) proc = JetToTauComposition::kWJets;
        else if(id_s.BeginsWith("P1")) proc = JetToTauComposition::kTop;
        else if(id_s.BeginsWith("P2")) proc = JetToTauComposition::kQCD;
        else continue;
        int dm_bin = 0;
        if     (id_s.Contains("D0")) dm_bin = 0;
        else if(id_s.Contains("D1")) dm_bin = 1;
        else if(id_s.Contains("D2")) dm_bin = 2;
        else if(id_s.Contains("D3")) dm_bin = 3;
        else continue;
        int alt_bin = 0;
        if     (id_s.EndsWith("A0")) alt_bin = 0;
        else if(id_s.EndsWith("A1")) alt_bin = 1;
        else if(id_s.EndsWith("A2")) alt_bin = 2;
        else continue;
        float wt_jtt = 0.f;
        for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
          //Z+jets uses the W+jets scales
          bool test = ((iproc == proc || (proc == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets)) &&
                       (tauDecayMode % 10 == 0 || tauDecayMode % 10 == 1)
                       && ((tauDecayMode%10 + 2*(tauDecayMode/10)) == dm_bin)
                       && (alt_bin < fJetToTauAltNum[proc]));
          float corr;
          if(fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets)) {
            corr = fJetToTauComps[iproc] * fJetToTauCorrs[iproc] * fJetToTauBiases[iproc];
          } else {
            corr = fJetToTauComps[iproc] * fJetToTauCorrs[iproc];
          }
          if(test) {
            wt_jtt += corr * ((fSystematics.IsUp(sys)) ? fJetToTauAltUp[iproc*kMaxAltFunc + alt_bin] : fJetToTauAltDown[iproc*kMaxAltFunc + alt_bin]);
          } else {
            wt_jtt += corr * fJetToTauWts[iproc];
          }
        }
        if(wt_jtt <= 0.f || (fVerbose && std::abs(wt_jtt-jetToTauWeightBias)/wt_jtt > 2.f)) {
          printf("CLFVHistMaker::%s: Entry %lld: Unexpected j-->tau weight variation! wt = %.3f, nominal = %.3f, proc = %i, dm_bin = %i, dm = %i\n",
                 __func__, fentry, wt_jtt, jetToTauWeightBias, proc, dm_bin, tauDecayMode);
        }
        weight *= wt_jtt / jetToTauWeightBias;
      } else continue; //skip tight ID events, take default histogram
    } else if(name == "ZPt") {
      if(!(fIsSignal || fIsDY)) continue;
      if(fSystematics.IsUp(sys)) weight *= zPtWeightUp          / zPtWeight      ;
      else                       weight *= zPtWeightDown        / zPtWeight      ;
    } else if(name == "Pileup") {
      if(fIsData || fIsEmbed) continue;
      if(fSystematics.IsUp(sys)) weight *= puWeight_up          / puWeight       ;
      else                       weight *= puWeight_down        / puWeight       ;
    } else if(name == "Prefire") {
      if(fIsData) continue;
      if(fSystematics.IsUp(sys)) weight *= prefireWeight_up     / prefireWeight  ;
      else                       weight *= prefireWeight_down   / prefireWeight  ;
    } else if(name == "EleES") {
      if(fIsEmbed || fIsData || !isEData) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) { //FIXME: check if this should be propagated to the MET
        if(leptonOne.isElectron() && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0.)
          EnergyScale(leptonOne.ES[1] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isElectron() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale(leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonOne.isElectron() && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0.)
          EnergyScale(leptonOne.ES[2] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isElectron() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale(leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name.BeginsWith("EmbEleES")) {
      if(!fIsEmbed || !isEData) continue;
      reeval = true;
      //|eta| Regions: EmbEleES = |eta| < 1.5; EmbEleES1 = |eta| > 1.5;
      const float eta_min = (name.EndsWith("1")) ? 1.5f : 0.f;
      const float eta_max = (name.EndsWith("1")) ? 2.5f : 1.5f;
      if(fSystematics.IsUp(sys)) { //FIXME: check if this should be propagated to the MET
        if(leptonOne.isElectron() && std::fabs(leptonOne.eta) >= eta_min && std::fabs(leptonOne.eta) < eta_max && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0.)
          EnergyScale(leptonOne.ES[1] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isElectron() && std::fabs(leptonTwo.eta) >= eta_min && std::fabs(leptonTwo.eta) < eta_max && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale(leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonOne.isElectron() && std::fabs(leptonOne.eta) >= eta_min && std::fabs(leptonOne.eta) < eta_max && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0.)
          EnergyScale(leptonOne.ES[2] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isElectron() && std::fabs(leptonTwo.eta) >= eta_min && std::fabs(leptonTwo.eta) < eta_max && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale(leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name == "MuonES") {
      if(fIsEmbed || fIsData || !isMData) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0.)
          EnergyScale(leptonOne.ES[1] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale(leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0.)
          EnergyScale(leptonOne.ES[2] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale(leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name == "EmbMuonES") {
      if(!fIsEmbed || !isMData || !fUseEmbedRocco) continue; //only process for embedding, and only if using Rocco systematics
      reeval = true;
      if(std::fabs(std::fabs(1.f - leptonOne.ES[1]/leptonOne.ES[0]) - std::fabs(1.f - leptonOne.ES[2]/leptonOne.ES[0])) > 1.e3f) {
        printf("%s: Energy scales disagree! ES = %.4f, up = %.4f, down = %.4f, pt = %.1f, eta = %.2f\n",
               __func__, leptonOne.ES[0], leptonOne.ES[1], leptonOne.ES[2], leptonOne.pt, leptonOne.eta);
        throw 20;
      }
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0.)
          EnergyScale(leptonOne.ES[1] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale(leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0.)
          EnergyScale(leptonOne.ES[2] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale(leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name.Contains("EmbMuonES")) { //bins of energy scale uncertainty
      if(!fIsEmbed || !isMData || fUseEmbedRocco) continue; //don't do binned ES if using Rocco systematics
      TString bin_s = name; bin_s.ReplaceAll("EmbMuonES", "");
      const int bin = std::abs(std::stoi(bin_s.Data()));
      if(bin < 0 || bin > 2) {printf("CLFVHistMaker::%s: Unknown sys bin %s\n", __func__, name.Data()); continue;}
      const float eta_min = (bin == 0) ? -1.f : (bin == 1) ? 1.2f : 2.1f;
      const float eta_max = (bin == 0) ? 1.2f : (bin == 1) ? 2.1f : 2.4f;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0. && leptonOne.eta >= eta_min && leptonOne.eta < eta_max) {
          EnergyScale(leptonOne.ES[1] / leptonOne.ES[0], leptonOne, &met, &metPhi); reeval = true;
        }
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0. && leptonTwo.eta >= eta_min && leptonTwo.eta < eta_max) {
          EnergyScale(leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi); reeval = true;
        }
      } else {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0. && leptonOne.eta >= eta_min && leptonOne.eta < eta_max) {
          EnergyScale(leptonOne.ES[2] / leptonOne.ES[0], leptonOne, &met, &metPhi); reeval = true;
        }
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0. && leptonTwo.eta >= eta_min && leptonTwo.eta < eta_max) {
          EnergyScale(leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi); reeval = true;
        }
      }
    } else if(name == "TauES") {
      if(fIsData || !leptonTwo.isTau()) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
        if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale((1.f-rho) + rho*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale((1.f-rho) + rho*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name == "EmbTauES") {
      if(!fIsEmbed || !leptonTwo.isTau()) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
        if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale((1.f-rho_t) + rho_t*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale((1.f-rho_t) + rho_t*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name == "QCDStat") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdWeight > 0.) ? qcdWeightUp   / qcdWeight : 0.;
        else                       weight *= (qcdWeight > 0.) ? qcdWeightDown / qcdWeight : 0.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name.Contains("QCDStat")) { //jet binned QCD fit uncertainty
      if(!chargeTest) { //only shift for same-sign events
        int jet_bin(0);
        if     (name == "QCDStat0") jet_bin = 0;
        else if(name == "QCDStat1") jet_bin = 1;
        else if(name == "QCDStat2") jet_bin = 2;
        if(std::min(2, (int) nJets20) == jet_bin) {
          if(fSystematics.IsUp(sys)) weight *= (qcdWeight > 0.) ? qcdWeightUp   / qcdWeight : 0.;
          else                       weight *= (qcdWeight > 0.) ? qcdWeightDown / qcdWeight : 0.;
        }
      } else continue; //no need to fill opposite signed histograms
    } else if(name.Contains("QCDAlt")) { //jet binned QCD fit uncertainty using alternate shapes
      if(!chargeTest) { //only shift for same-sign events
        int jet_bin(0), alt_bin(0);
        if     (name.BeginsWith("QCDAltJ0")) jet_bin = 0;
        else if(name.BeginsWith("QCDAltJ1")) jet_bin = 1;
        else if(name.BeginsWith("QCDAltJ2")) jet_bin = 2;
        if     (name.EndsWith("A0")) alt_bin = 0;
        else if(name.EndsWith("A1")) alt_bin = 1;
        if(std::min(2, (int) nJets20) == jet_bin) {
          if(fSystematics.IsUp(sys)) weight *= (qcdWeight > 0.) ? qcdWeightAltUp  [alt_bin] / qcdWeight : 0.;
          else                       weight *= (qcdWeight > 0.) ? qcdWeightAltDown[alt_bin] / qcdWeight : 0.;
        }
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "QCDNC") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdClosure > 0.) ? 1. / qcdClosure : 0.; //remove / apply twice as uncertainty
        else                       weight *= (qcdClosure > 0.) ? 1. * qcdClosure : 0.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "QCDBias") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdIsoScale > 0.) ? 1. / qcdIsoScale : 0.; //remove / apply twice as uncertainty
        else                       weight *= (qcdIsoScale > 0.) ? 1. * qcdIsoScale : 0.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "JetToTauNC") {
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        if(fSystematics.IsUp(sys)) weight *= jetToTauWeightCorrUp     / jetToTauWeightCorr   ;
        else                       weight *= jetToTauWeightCorrDown   / jetToTauWeightCorr   ;
      } else continue; //no need to fill tight ID histograms
    } else if(name.Contains("JetToTauNC")) { //process binned bias uncertainty
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        //recreate the weight with shifted process bias
        float wt_jtt = 0.f;
        int proc;
        if     (name == "JetToTauNC0") proc = JetToTauComposition::kWJets;
        else if(name == "JetToTauNC1") proc = JetToTauComposition::kTop;
        else if(name == "JetToTauNC2") proc = JetToTauComposition::kQCD;
        else {
          printf("CLFVHistMaker::%s: Unknown jet-->tau non-closure systematic bin %s\n", __func__, name.Data());
          continue;
        }
        for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
          //Z+jets uses the W+jets scales
          bool test = iproc == proc || (iproc == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets);
          float corr;
          if(fApplyJetToTauMCBias || (proc != JetToTauComposition::kWJets && proc != JetToTauComposition::kZJets)) {
            corr = fJetToTauComps[iproc] * fJetToTauWts[iproc] * fJetToTauBiases[iproc];
          } else {
            corr = fJetToTauComps[iproc] * fJetToTauWts[iproc];
          }
          //take up as apply correction twice, down as no correction
          if(test) {
            wt_jtt += corr * ((fSystematics.IsUp(sys)) ? fJetToTauCorrs[iproc]*fJetToTauCorrs[iproc] : 1.f);
          } else {
            wt_jtt += corr * fJetToTauCorrs[iproc];
          }
        }
        weight *= wt_jtt / jetToTauWeightBias;
      } else continue; //no need to fill tight ID histograms
    } else if(name == "JetToTauComp") {
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        if(fSystematics.IsUp(sys)) weight *= jetToTauWeight_compUp    / jetToTauWeightBias   ;
        else                       weight *= jetToTauWeight_compDown  / jetToTauWeightBias   ;
      } else continue; //no need to fill tight ID histograms
    } else if(name == "TauJetID") {
      if(fIsData) continue;
      if(leptonTwo.isTau() && std::abs(tauGenFlavor) == 15) { //only evaluate for true hadronic taus
        if(fSystematics.IsUp(sys)) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
        else                       weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "EmbTauJetID") {
      if(fIsData) continue;
      if(leptonTwo.isTau() && std::abs(tauGenFlavor) == 15) { //only evaluate for true hadronic taus
        if(fSystematics.IsUp(sys)) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
        else                       weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "TauMuID") {
      if(fIsData) continue;
      if(leptonTwo.isTau() && std::abs(tauGenFlavor) == 13) { //only evaluate for mu --> hadronic tau
        if(fSystematics.IsUp(sys)) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
        else                       weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "EmbTauMuID") {
      if(fIsData) continue;
      if(leptonTwo.isTau() && std::abs(tauGenFlavor) == 13) { //only evaluate for mu --> hadronic tau
        if(fSystematics.IsUp(sys)) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
        else                       weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "TauEleID") {
      if(fIsData) continue;
      if(leptonTwo.isTau() && std::abs(tauGenFlavor) == 11) {  //only evaluate for e --> hadronic tau
        if(fSystematics.IsUp(sys)) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
        else                       weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "EmbTauEleID") {
      if(fIsData) continue;
      if(leptonTwo.isTau() && std::abs(tauGenFlavor) == 11) {  //only evaluate for e --> hadronic tau
        if(fSystematics.IsUp(sys)) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
        else                       weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
      }
    } else if(name == "Lumi") {
      if(fIsData || fIsEmbed) continue;
      const float unc = (fYear == 2016) ? 0.012 : (fYear == 2017) ? 0.023 : 0.025;
      if(fSystematics.IsUp(sys))  weight *= (fIsData || fIsEmbed) ? 1.f : 1.f + unc;
      else                        weight *= (fIsData || fIsEmbed) ? 1.f : 1.f - unc;
    } else if(name == "BTag" || name == "BTagHeavy") {
      if(fIsData || fIsEmbed) continue;
      if((btagWeightUpBC - btagWeight)*(btagWeightDownBC - btagWeight) > 1.e-6) { //weights are in the same direction
        printf("HistMaker::%s: Warning! Entry %lld: B-tag gen b/c weight has up/down in same direction: wt = %.4f, up = %.4f, down = %.4f, nJets = %i\n",
               __func__, fentry, btagWeight, btagWeightUpBC, btagWeightDownBC, nJets20);
      }
      if(fSystematics.IsUp(sys)) weight *= (btagWeight > 0.f) ? btagWeightUpBC   / btagWeight : 1.f;
      else                       weight *= (btagWeight > 0.f) ? btagWeightDownBC / btagWeight : 1.f;
    } else if(name == "BTagLight") { //non-B/C jet uncertainty
      if(fIsData || fIsEmbed) continue;
      if(fSystematics.IsUp(sys)) weight *= (btagWeight > 0.f) ? btagWeightUpL   / btagWeight : 1.f;
      else                       weight *= (btagWeight > 0.f) ? btagWeightDownL / btagWeight : 1.f;
    } else if(name == "JetToTauBias") {
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        if(fSystematics.IsUp(sys)) weight *= jetToTauWeightBiasUp     / jetToTauWeightBias   ;
        else                       weight *= jetToTauWeightBiasDown   / jetToTauWeightBias   ;
      } else continue; //no need to fill tight ID histograms
    } else if(name.Contains("JetToTauBias")) { //process binned bias uncertainty
      if(isLooseTau) { //only shift the weight for loose tau ID region events
        //recreate the weight with shifted process bias
        float wt_jtt = 0.f;
        int proc;
        if     (name == "JetToTauBias0") proc = JetToTauComposition::kWJets;
        else if(name == "JetToTauBias1") proc = JetToTauComposition::kTop;
        else if(name == "JetToTauBias2") proc = JetToTauComposition::kQCD;
        else {
          printf("CLFVHistMaker::%s: Unknown jet-->tau bias systematic bin %s\n", __func__, name.Data());
          continue;
        }
        for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
          //Z+jets uses the W+jets scales
          bool test = iproc == proc || (proc == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets);
          const float base = fJetToTauComps[iproc] * fJetToTauWts[iproc] * fJetToTauCorrs[iproc];
          if(fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets)) {
            //take up as applying the bias correction twice, down as no correction
            wt_jtt += base * ((test) ?
                              (fSystematics.IsUp(sys)) ? fJetToTauBiases[iproc]*fJetToTauBiases[iproc] : 1.f
                              : fJetToTauBiases[iproc]);
          } else {
            //take up as applying the bias correction, down dividing by the correction
            wt_jtt += base * ((test) ?
                              (fSystematics.IsUp(sys)) ? fJetToTauBiases[iproc] : 1.f/fJetToTauBiases[iproc]
                              : 1.f);

          }
        }
        if(proc == JetToTauComposition::kTop && wt_jtt != jetToTauWeightBias) {
          printf("%s: Bias found in j-->tau top estimation! wt(nom) = %.4f, wt(sys) = %.4f, Bias = %.4f\n",
                 __func__, jetToTauWeightBias, wt_jtt, fJetToTauBiases[proc]);
          printf(" event info: lepm = %.1f, mtone = %.1f, one pt = %.1f, two pt = %.1f\n",
                 fTreeVars.lepm, fTreeVars.mtone, fTreeVars.leponept, fTreeVars.leptwopt);
          for(int jproc = 0; jproc < JetToTauComposition::kLast; ++jproc) {
            printf(" proc %i: comp = %.4f, wt = %.4f, correction = %.4f, bias = %.4f\n",
                   jproc, fJetToTauComps[jproc], fJetToTauWts[jproc], fJetToTauCorrs[jproc], fJetToTauBiases[jproc]);
          }
          throw 20;
        }
        weight *= wt_jtt / jetToTauWeightBias;
      } else continue; //no need to fill tight ID histograms
    } else if(name == "EmbedUnfold") {
      if(!fIsEmbed) continue;
      if(fSystematics.IsUp(sys)) weight *= 1.04f;
      else                       weight *= 0.96f;
    } else if(name == "EleTrig") {
      if(fIsData || !isEData) continue;
      const float trig_wt = leptonOne.trig_wt * leptonTwo.trig_wt;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron() && leptonTwo.isElectron()) { weight *= (1.f-rho) + rho*triggerWeightsSys[4] / trig_wt; }
        else if(leptonOne.isElectron())                      { weight *= (1.f-rho) + rho*triggerWeightsSys[0] / trig_wt; }
        else if(leptonTwo.isElectron())                      { weight *= (1.f-rho) + rho*triggerWeightsSys[2] / trig_wt; }
      } else {
        if(leptonOne.isElectron() && leptonTwo.isElectron()) { weight *= (1.f-rho) + rho*triggerWeightsSys[5] / trig_wt; }
        else if(leptonOne.isElectron())                      { weight *= (1.f-rho) + rho*triggerWeightsSys[1] / trig_wt; }
        else if(leptonTwo.isElectron())                      { weight *= (1.f-rho) + rho*triggerWeightsSys[3] / trig_wt; }
      }
    } else if(name == "EmbEleTrig") {
      if(!fIsEmbed || !isEData) continue;
      const float trig_wt = leptonOne.trig_wt * leptonTwo.trig_wt;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron() && leptonTwo.isElectron()) { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[4] / trig_wt; }
        else if(leptonOne.isElectron())                      { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[0] / trig_wt; }
        else if(leptonTwo.isElectron())                      { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[2] / trig_wt; }
      } else {
        if(leptonOne.isElectron() && leptonTwo.isElectron()) { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[5] / trig_wt; }
        else if(leptonOne.isElectron())                      { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[1] / trig_wt; }
        else if(leptonTwo.isElectron())                      { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[3] / trig_wt; }
      }
    } else if(name == "MuonTrig") {
      if(fIsData || !isMData) continue;
      const float trig_wt = leptonOne.trig_wt * leptonTwo.trig_wt;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonTwo.isMuon())         { weight *= (1.f-rho) + rho*triggerWeightsSys[4] / trig_wt; }
        else if(leptonOne.isMuon())                          { weight *= (1.f-rho) + rho*triggerWeightsSys[0] / trig_wt; }
        else if(leptonTwo.isMuon())                          { weight *= (1.f-rho) + rho*triggerWeightsSys[2] / trig_wt; }
      } else {
        if(leptonOne.isMuon() && leptonTwo.isMuon())         { weight *= (1.f-rho) + rho*triggerWeightsSys[5] / trig_wt; }
        else if(leptonOne.isMuon())                          { weight *= (1.f-rho) + rho*triggerWeightsSys[1] / trig_wt; }
        else if(leptonTwo.isMuon())                          { weight *= (1.f-rho) + rho*triggerWeightsSys[3] / trig_wt; }
      }
    } else if(name == "EmbMuonTrig") {
      if(!fIsEmbed || !isMData) continue;
      const float trig_wt = leptonOne.trig_wt * leptonTwo.trig_wt;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonTwo.isMuon())         { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[4] / trig_wt; }
        else if(leptonOne.isMuon())                          { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[0] / trig_wt; }
        else if(leptonTwo.isMuon())                          { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[2] / trig_wt; }
      } else {
        if(leptonOne.isMuon() && leptonTwo.isMuon())         { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[5] / trig_wt; }
        else if(leptonOne.isMuon())                          { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[1] / trig_wt; }
        else if(leptonTwo.isMuon())                          { weight *= (1.f-rho_t) + rho_t*triggerWeightsSys[3] / trig_wt; }
      }
    } else if(name == "JER") {
      if(fIsData || fIsEmbed) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        met    = puppMETJERUp;
        metPhi = puppMETphiJERUp;
        if(jetOne.pt > 5.f && jetOne.jer_pt_up > 5.f) { //if there's a jet and defined uncertainties
          jetOne.setPtEtaPhiM(jetOne.jer_pt_up, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      } else {
        const float dx(met*std::cos(metPhi) - puppMETJERUp*std::cos(puppMETphiJERUp)), dy(met*std::sin(metPhi) - puppMETJERUp*std::sin(puppMETphiJERUp));
        const float new_x(met*std::cos(metPhi)+dx), new_y(met*std::sin(metPhi)+dy);
        met    = std::sqrt(std::pow(new_x, 2) + std::pow(new_y, 2));
        metPhi = (met > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, new_x/met)))*(new_y < 0.f ? -1.f : 1.f) : 0.f;
        if(jetOne.pt > 5.f && jetOne.jer_pt_down > 5.f) { //if there's a jet and defined uncertainties
          jetOne.setPtEtaPhiM(jetOne.jer_pt_down, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      }
    } else if(name == "JES") {
      if(fIsData || fIsEmbed) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        met    = puppMETJESUp;
        metPhi = puppMETphiJESUp;
        if(jetOne.pt > 5.f && jetOne.jes_pt_up > 5.f) { //if there's a jet and defined uncertainties
          jetOne.setPtEtaPhiM(jetOne.jes_pt_up, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      } else {
        float dx(met*std::cos(metPhi) - puppMETJESUp*std::cos(puppMETphiJESUp)), dy(met*std::sin(metPhi) - puppMETJESUp*std::sin(puppMETphiJESUp));
        float new_x(met*std::cos(metPhi)+dx), new_y(met*std::sin(metPhi)+dy);
        met    = std::sqrt(std::pow(new_x, 2) + std::pow(new_y, 2));
        metPhi = (met > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, new_x/met)))*(new_y < 0.f ? -1.f : 1.f) : 0.f;
        if(jetOne.pt > 5.f && jetOne.jes_pt_down > 5.f) { //if there's a jet and defined uncertainties
          jetOne.setPtEtaPhiM(jetOne.jes_pt_down, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      }
    } else if(name == "METCorr") { //FIXME: Remove
      if(fIsData) continue;
      reeval = true;
      const float met_c_x(metCorr*std::cos(metCorrPhi)), met_c_y(metCorr*std::sin(metCorrPhi));
      if(fSystematics.IsUp(sys)) {
        const float met_x(met*std::cos(metPhi) - met_c_x), met_y(met*std::sin(metPhi) - met_c_y);
        met    = std::sqrt(met_x*met_x + met_y*met_y);
        metPhi = (met > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, met_x/met)))*(met_y < 0.f ? -1 : 1) : 0.f;
      } else {
        const float met_x(met*std::cos(metPhi) + met_c_x), met_y(met*std::sin(metPhi) + met_c_y);
        met    = std::sqrt(met_x*met_x + met_y*met_y);
        metPhi = (met > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, met_x/met)))*(met_y < 0.f ? -1 : 1) : 0.f;
      }
    } else if(name.BeginsWith("TauJetID")) { //a tau anti-jet ID bin
      if(fIsData || !leptonTwo.isTau()) continue;
      if(std::abs(tauGenFlavor) == 15) {
        TString bin_s = name;
        bin_s.ReplaceAll("TauJetID", "");
        const int bin = std::stoi(bin_s.Data());
        if(bin == leptonTwo.wt1_bin) {
          if(fSystematics.IsUp(sys)) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
          else                       weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
        }
      }
    } else if(name.BeginsWith("EmbTauJetID")) { //a tau anti-jet ID bin
      if(!fIsEmbed || !leptonTwo.isTau()) continue;
      if(std::abs(tauGenFlavor) == 15) {
        TString bin_s = name;
        bin_s.ReplaceAll("EmbTauJetID", "");
        const int bin = std::stoi(bin_s.Data());
        if(bin == leptonTwo.wt1_bin) {
          if(fSystematics.IsUp(sys)) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
          else                       weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
        }
      }
    } else if(name.BeginsWith("TauMuID")) {
      if(fIsData || !leptonTwo.isTau()) continue;
      if(std::abs(tauGenFlavor) == 13) {
        TString bin_s = name;
        bin_s.ReplaceAll("TauMuID", "");
        const int bin = std::stoi(bin_s.Data());
        if(bin == leptonTwo.wt1_bin) { //uncorrelated between bins
          if(fSystematics.IsUp(sys)) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
          else                       weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
        }
      }
    } else if(name.BeginsWith("EmbTauMuID")) {
      if(!fIsEmbed || !leptonTwo.isTau()) continue;
      if(std::abs(tauGenFlavor) == 13) {
        TString bin_s = name;
        bin_s.ReplaceAll("EmbTauMuID", "");
        const int bin = std::stoi(bin_s.Data());
        if(bin == leptonTwo.wt1_bin) { //uncorrelated between bins
          if(fSystematics.IsUp(sys)) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
          else                       weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
        }
      }
    } else if(name.BeginsWith("TauEleID")) {
      if(fIsData || !leptonTwo.isTau()) continue;
      if(std::abs(tauGenFlavor) == 11) {
        TString bin_s = name;
        bin_s.ReplaceAll("TauEleID", "");
        const int bin = std::stoi(bin_s.Data());
        if(bin == leptonTwo.wt1_bin) { //uncorrelated between bins
          if(fSystematics.IsUp(sys)) weight *= (1.f-rho) + rho*leptonTwo.wt1[1] / leptonTwo.wt1[0];
          else                       weight *= (1.f-rho) + rho*leptonTwo.wt1[2] / leptonTwo.wt1[0];
        }
      }
    } else if(name.BeginsWith("EmbTauEleID")) {
      if(!fIsEmbed || !leptonTwo.isTau()) continue;
      if(std::abs(tauGenFlavor) == 11) {
        TString bin_s = name;
        bin_s.ReplaceAll("EmbTauEleID", "");
        const int bin = std::stoi(bin_s.Data());
        if(bin == leptonTwo.wt1_bin) { //uncorrelated between bins
          if(fSystematics.IsUp(sys)) weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[1] / leptonTwo.wt1[0];
          else                       weight *= (1.f-rho_t) + rho_t*leptonTwo.wt1[2] / leptonTwo.wt1[0];
        }
      }
    } else if(name != "") {
      std::cout << "Sytematic " << name.Data() << " (" << sys << ") defined but no uncertainty implemented!\n";
      //fill with unshifted values for now
    } else continue; //no need to fill undefined systematics

    if(!std::isfinite(weight)) {
      std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << " Systematic " << name.Data() << "(" << sys
                << "): Weight is not finite! weight = " << weight << " event weight = " << eventWeight
                << ", setting to 0...\n";
      weight = 0.;
    }

    if(reeval) {
      fTimes[GetTimerNumber("SystKin")] = std::chrono::steady_clock::now(); //timer for evaluating the MVAs
      SetKinematics();
      IncrementTimer("SystKin", true);
      EvalMVAs("SystMVAs");
    } //re-evaluate variables with shifted values
    const float lepm  = fTreeVars.lepm;
    const float onept = leptonOne.pt;
    const float twopt = leptonTwo.pt;

    //apply event window cuts with (possibly) shifted kinematics
    const bool pass = (fAllowMigration) ? PassesCuts() : o_pass;

    if(pass) {
      ++nfilled;
      Hist->hLepM  [sys]->Fill(lepm  , weight);
      //skip all other histograms in same-flavor selection, only using the M_{ll} histogram
      if(!fSparseHists) {
        Hist->hOnePt [sys]->Fill(onept  , weight);
        Hist->hTwoPt [sys]->Fill(twopt  , weight);
        if(!fIsData) {
          Hist->hWeightChange[sys]->Fill((eventWeight*genWeight != 0.) ? (eventWeight*genWeight - weight) / (eventWeight*genWeight) : 0.);
        }
      }
      if(isSameFlavor) continue;
      //MVA outputs
      float mvaweight = fTreeVars.eventweightMVA*(weight/(eventWeight*genWeight)); //apply the fractional weight change to the MVA weight accounting for test/train splitting
      if(!std::isfinite(mvaweight)) {
        std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << " MVA weight is not finite! mvaweight = " << mvaweight << " eventweightMVA = "
                  << fTreeVars.eventweightMVA << ", setting to 0...\n";
        mvaweight = 0.;
      }
      for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
        //assume only relevant MVAs are initialized
        if(!mva[i]) continue;
        float mvascore = (fUseCDFBDTs == 1) ? fMvaCDFs[i] : (fUseCDFBDTs == 2) ? fMvaFofP[i] : fMvaOutputs[i];
        if(!std::isfinite(mvascore) && fVerbose > 0) {
          std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << ", sys " << sys <<", MVA " << i << ": score is not finite = " << mvascore << "! Setting to -2...\n";
          mvascore = -2.;
        }
        if(mvascore < -1.) {
          std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << ", sys " << sys <<", MVA " << i << ": score is not defined = " << mvascore << "!\n";
        }
        Hist->hMVA[i][sys]->Fill(mvascore, mvaweight);
        if(reeval) Hist->hMVADiff[i][sys]->Fill(mvascore-((fUseCDFBDTs == 1) ? o_cdfs[i] : (fUseCDFBDTs == 2) ? o_fofp[i] : o_mvas[i]), mvaweight);
      }
    } //end kinematic event selection check

    //restore the nominal results
    if(reeval) {
      leptonOne.setP(o_lv1);
      leptonTwo.setP(o_lv2);
      jetOne.setP(o_jet);
      met = o_met;
      metPhi = o_metPhi;
      SetKinematics();
      for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
        fMvaOutputs[i] = o_mvas[i];
        fMvaCDFs[i] = o_cdfs[i];
        fMvaFofP[i] = o_fofp[i];
      }
    }
  }
  //Per event histograms
  Hist->hNFills->Fill(nfilled, eventWeight*genWeight);
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t CLFVHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  //object pT thresholds
  const float muon_pt(10.f), electron_pt(15.f), tau_pt(20.f);
  const float sys_buffer(fMigrationBuffer); //window widening for event migration checks, in GeV

  if(leptonOne.isElectron()) one_pt_min_ = electron_pt;
  if(leptonTwo.isElectron()) two_pt_min_ = electron_pt;
  if(leptonOne.isMuon    ()) one_pt_min_ = muon_pt;
  if(leptonTwo.isMuon    ()) two_pt_min_ = muon_pt;
  if(leptonOne.isTau     ()) one_pt_min_ = tau_pt;
  if(leptonTwo.isTau     ()) two_pt_min_ = tau_pt;

  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////

  // //Skip Drell-Yan events with gen-level decay mode inconsistent with the one of interest
  // //Warning: Only use for gen-level studies!
  // if(false && fIsDY && fTreeVars.zdecaymode != 0) {
  //   if(fSelection == "emu"   && fTreeVars.zdecaymode != 5) return kTRUE;
  //   if(fSelection == "etau"  && fTreeVars.zdecaymode != 6) return kTRUE;
  //   if(fSelection == "mutau" && fTreeVars.zdecaymode != 7) return kTRUE;
  // }

  //Ignore loose lepton selection in same flavor category for now
  ee   &= !isLooseElectron;
  mumu &= !isLooseMuon;

  ///////////////////////////////////////////
  // Re-define N(b-jets) if needed

  //FIXME: decide on b-jet ID to use in etau_mu/mutau_e selections
  if(emu && lep_tau) { //use medium ID b-jets in etau_mu/mutau_e since DY is the dominant background
    fBJetTightness = 1;
    nBJetsUse = (fBJetCounting == 0) ? nBJetsL : nBJets20L;
  }

  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: base selection
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 1);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //4

  ///////////////////////////////////////////////////////////////////
  //ensure the objects PDG IDs make sense

  mutau &= leptonOne.isMuon    () && leptonTwo.isTau     ();
  etau  &= leptonOne.isElectron() && leptonTwo.isTau     ();
  emu   &= leptonOne.isElectron() && leptonTwo.isMuon    ();
  mumu  &= leptonOne.isMuon    () && leptonTwo.isMuon    ();
  ee    &= leptonOne.isElectron() && leptonTwo.isElectron();

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //5


  ///////////////////////////////////////////////////////////////////
  //leptons must satisfy the pt requirements and fire a trigger

  mutau &= leptonOne.pt > muon_trig_pt_     - sys_buffer && leptonTwo.pt > tau_pt && leptonOne.matched;
  etau  &= leptonOne.pt > electron_trig_pt_ - sys_buffer && leptonTwo.pt > tau_pt && leptonOne.matched;
  emu   &= (leptonOne.pt > electron_pt && leptonTwo.pt > muon_pt &&
            ((leptonOne.pt > electron_trig_pt_ - sys_buffer && leptonOne.matched) ||
             (leptonTwo.pt > muon_trig_pt_     - sys_buffer && leptonTwo.matched)));
  mumu  &= (leptonOne.pt > muon_pt && leptonTwo.pt > muon_pt &&
            ((leptonOne.pt > muon_trig_pt_ - sys_buffer && leptonOne.matched) ||
             (leptonTwo.pt > muon_trig_pt_ - sys_buffer && leptonTwo.matched)));
  ee    &= (leptonOne.pt > electron_pt && leptonTwo.pt > electron_pt &&
            ((leptonOne.pt > electron_trig_pt_ - sys_buffer && leptonOne.matched) ||
             (leptonTwo.pt > electron_trig_pt_ - sys_buffer && leptonTwo.matched)));

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //6

  ///////////////////////////////////////////////////////////////////
  //don't allow multiple muons/electrons in mumu/ee selections

  mumu &= nMuons     == 2;
  ee   &= nElectrons == 2;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //7

  ///////////////////////////////////////////////////////////////////
  //reject electrons in the barrel/endcap gap region

  const float elec_gap_low(1.444), elec_gap_high(1.566);
  etau &= std::fabs(leptonOne.scEta) < elec_gap_low || std::fabs(leptonOne.scEta) > elec_gap_high;
  emu  &= std::fabs(leptonOne.scEta) < elec_gap_low || std::fabs(leptonOne.scEta) > elec_gap_high;
  ee   &= std::fabs(leptonOne.scEta) < elec_gap_low || std::fabs(leptonOne.scEta) > elec_gap_high;
  ee   &= std::fabs(leptonTwo.scEta) < elec_gap_low || std::fabs(leptonTwo.scEta) > elec_gap_high;

  if(!(mutau || etau || emu || mumu  || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //8

  ////////////////////////////////////////////////////////////
  // Set 2 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 2);
  }

  ///////////////////////////////////////////////////////////////////
  //Apply eta region cuts

  //in tau_l tau_h embedding, gen-level cut with |eta_l| < 2.2 and |eta_had| < 2.4
  //in tau_e tau_mu/ee/mumu embedding, no gen-level |eta| cut, but muons go up to 2.4 so cut there
  const double electron_eta_max = (fUseEmbedCuts) ? (emu || ee || mumu) ? 2.4 : 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? (emu || ee || mumu) ? 2.4 : 2.2 : 2.4;
  const double tau_eta_max      = 2.3; //tau eta doesn't change for selections or simulation datasets
  const double min_delta_r      = 0.3; //separation between leptons

  mutau &= std::fabs(leptonOne.eta) < muon_eta_max;
  mutau &= std::fabs(leptonTwo.eta) < tau_eta_max ;
  mutau &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  etau  &= std::fabs(leptonOne.eta) < electron_eta_max;
  etau  &= std::fabs(leptonTwo.eta) < tau_eta_max      ;
  etau  &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  emu   &= std::fabs(leptonOne.eta) < electron_eta_max;
  emu   &= std::fabs(leptonTwo.eta) < muon_eta_max    ;
  emu   &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  mumu  &= std::fabs(leptonOne.eta) < muon_eta_max;
  mumu  &= std::fabs(leptonTwo.eta) < muon_eta_max;
  mumu  &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  ee    &= std::fabs(leptonOne.eta) < electron_eta_max;
  ee    &= std::fabs(leptonTwo.eta) < electron_eta_max;
  ee    &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  ///////////////////////////////////////////////////////////////////
  //Apply di-lepton mass cuts

  const float mll = fTreeVars.lepm;
  min_mass_ = (etau || mutau || ee || mumu) ?  40.f :  40.f;
  max_mass_ = (etau || mutau)               ? 170.f : 170.f;

  mutau &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;
  etau  &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;
  emu   &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;
  mumu  &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;
  ee    &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;

  ///////////////////////////////////////////////////////////////////
  //ensure reasonable dxy/dz cuts for electrons/muons

  const float max_dz(0.5), max_dxy(0.2);
  mutau &= std::fabs(leptonOne.dxy) < max_dxy;
  mutau &= std::fabs(leptonOne.dz ) < max_dz ;
  etau  &= std::fabs(leptonOne.dxy) < max_dxy;
  etau  &= std::fabs(leptonOne.dz ) < max_dz ;
  emu   &= std::fabs(leptonOne.dxy) < max_dxy;
  emu   &= std::fabs(leptonOne.dz ) < max_dz ;
  emu   &= std::fabs(leptonTwo.dxy) < max_dxy;
  emu   &= std::fabs(leptonTwo.dz ) < max_dz ;
  mumu  &= std::fabs(leptonOne.dxy) < max_dxy;
  mumu  &= std::fabs(leptonOne.dz ) < max_dz ;
  mumu  &= std::fabs(leptonTwo.dxy) < max_dxy;
  mumu  &= std::fabs(leptonTwo.dz ) < max_dz ;
  ee    &= std::fabs(leptonOne.dxy) < max_dxy;
  ee    &= std::fabs(leptonOne.dz ) < max_dz ;
  ee    &= std::fabs(leptonTwo.dxy) < max_dxy;
  ee    &= std::fabs(leptonTwo.dz ) < max_dz ;

  //FIXME: Decide dxy/dz significance cuts
  const int use_dxyz_sig = 1; //0: None; 1: Z->e+mu only; 2: All channels
  if(use_dxyz_sig > 1) {
    mutau &= std::fabs(leptonOne.dxySig) < 3.0;
    mutau &= std::fabs(leptonOne.dzSig ) < 4.7;
    etau  &= std::fabs(leptonOne.dxySig) < 3.0;
    etau  &= std::fabs(leptonOne.dzSig ) < 4.7;
  }

  ///////////////////////////////////////////////////////////////////
  //remove additional leptons in tau categories

  mutau &= nElectrons == 0;
  etau  &= nMuons     == 0;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  fCutFlow->Fill(icutflow); ++icutflow; //9

  ////////////////////////////////////////////////////////////
  // Set 3 + selection offset: eta, mass, and trigger cuts
  ////////////////////////////////////////////////////////////

  if(fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 3);
  }

  ///////////////////////////////////////////////////////////////////
  // Apply additional hadronic tau IDs

  mutau &= isLooseTau || tauDeepAntiJet >= 50; //63 = tight
  mutau &= tauDeepAntiMu  >= 10; //15 = tight
  mutau &= tauDeepAntiEle >= 10; //7 = VLoose, 15 = Loose, 31 = Medium
  mutau &= leptonTwo.id2  >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  etau  &= isLooseTau || tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //15 = tight
  etau  &= tauDeepAntiEle >= fETauAntiEleCut; //tauDeepAntiEle >= 50; //31 = Medium, 63 = tight, 127 = VTight, 255 = VVTight
  etau  &= leptonTwo.id2  >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  //remove tau decay modes not interested in (keep only 0, 1, 10, 11)
  mutau &= tauDecayMode % 10 < 2 && tauDecayMode <= 11;
  etau  &= tauDecayMode % 10 < 2 && tauDecayMode <= 11;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 4 + selection offset: Tau ID cuts
  ////////////////////////////////////////////////////////////

  if(fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 4);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //10

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //11
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //12
  ++icutflow;

  //cut-flow for fake leptons
  if(fIsData) fCutFlow->Fill(icutflow); //13
  else if(emu) {
    if((std::abs(leptonOne.flavor) == std::abs(leptonOne.genFlavor))
       && (std::abs(leptonTwo.flavor) == std::abs(leptonTwo.genFlavor))) {
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

  const double met_cut         = -1.; //60.;
  const double mtlep_cut       = (lep_tau) ? -1. : 70.;
  const double qcd_mtlep_cut   = mtlep_cut; //(etau) ? 45. : mtlep_cut;
  const bool looseQCDRegion    = (mutau || etau) && nBJetsUse == 0 && fTreeVars.mtlep < mtlep_cut && (met_cut < 0 || met < met_cut); // no isolation cut (0 - 0.5 allowed)
  const bool qcdSelection      = looseQCDRegion && fTreeVars.mtlep < qcd_mtlep_cut && (fTreeVars.leponereliso > 0.05) && !(isLooseMuon || isLooseElectron);
  const bool wjetsSelection    = (mutau || etau) && fTreeVars.mtlep > mtlep_cut && nBJetsUse == 0 && !(isLooseMuon || isLooseElectron);
  const bool topSelection      = (emu || etau || mutau) && nBJetsUse >= 1 && !(isLooseMuon || isLooseElectron);
  const bool nominalSelection  = nBJetsUse == 0 && (met_cut < 0 ||met < met_cut) && fTreeVars.mtlep < mtlep_cut + sys_buffer && !(isLooseMuon || isLooseElectron);
  mtlep_max_ = mtlep_cut;
  met_max_   = met_cut;

  ///////////////////////////////////////////////////////////////////
  // Handle anti-isolated light leptons

  mutau &= !isLooseMuon;
  etau  &= !isLooseElectron;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //14
  ++icutflow;


  ////////////////////////////////////////////////////////////
  // Set 35 + selection offset: last set with MC estimated taus and leptons
  ////////////////////////////////////////////////////////////

  //Nominal mixture, with fake e/mu included
  if(fFakeLeptonTesting && emu && !lep_tau) FillAllHistograms(set_offset + 35);

  if((fFakeLeptonTesting || fJetTauTesting) && !(mumu || ee || emu) && nominalSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = isLooseTau;

    //Nominal mixture
    if(fFakeLeptonTesting) FillAllHistograms(set_offset + 35);

    //Nominal mixture, without j-->tau weights
    if(fJetTauTesting) {
      if(add_wt) {
        jetToTauWeight     = 1.f;
        jetToTauWeightCorr = 1.f;
        jetToTauWeightBias = 1.f;
        eventWeight        = evt_wt_bare;
      }
      if(isLooseTau) {
        FillAllHistograms(set_offset + 33); //only fill for loose ID taus, since non-loose already have no j-->tau weights
      }

      //Restore the previous weights
      eventWeight        = prev_evt_wt;
      jetToTauWeight     = prev_jtt_wt;
      jetToTauWeightCorr = prev_jtt_cr;
      jetToTauWeightBias = prev_jtt_bs;
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 36 + selection offset: QCD selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if(fQCDFakeTauTesting && qcdSelection) {
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
  if(fWJFakeTauTesting && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool    add_wt      = fUseJetToTauComposition && isLooseTau;

    //Data measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 37);

    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 38 + selection offset: ttbar selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if(fTTFakeTauTesting && (mutau || etau) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool    add_wt      = fUseJetToTauComposition && isLooseTau;

    if(add_wt) {
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


  ///////////////////////////////////////////////////////////////////
  //remove MC jet -> light lepton contribution

  if(!fUseMCEstimatedFakeLep && !fIsData) {
    emu   &= !isFakeMuon;
    emu   &= !isFakeElectron;
    mumu  &= !isFakeMuon;
    ee    &= !isFakeElectron;
    mutau &= !isFakeMuon;
    etau  &= !isFakeElectron;
  }

  ///////////////////////////////////////////////////////////////////
  //remove MC estimated jet --> tau component

  mutau &= fIsData > 0 || std::abs(tauGenFlavor) != 26;
  etau  &= fIsData > 0 || std::abs(tauGenFlavor) != 26;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //15
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 30 + selection offset: QCD selection
  ////////////////////////////////////////////////////////////////////////////
  if(fQCDFakeTauTesting && qcdSelection) {
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
  if(fWJFakeTauTesting && wjetsSelection) {
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
  if(fTTFakeTauTesting && topSelection && !lep_tau) {
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

  mutau &= met_cut < 0.f || met < met_cut + sys_buffer;
  etau  &= met_cut < 0.f || met < met_cut + sys_buffer;
  emu   &= met_cut < 0.f || met < met_cut + sys_buffer;

  mutau &= mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;
  etau  &= mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;
  emu   &= mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 7 + selection offset: No MC estimated fake taus, no b-jet cut
  ////////////////////////////////////////////////////////////////////////////
  if(!lep_tau && (mumu || ee) && fDoSystematics >= 0)
    FillAllHistograms(set_offset + 7);

  if(fCutFlowTesting) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //16
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


  if(isLooseElectron)               {fCutFlow->Fill(icutflow);} //17
  ++icutflow;
  if(isLooseElectron && chargeTest) {fCutFlow->Fill(icutflow);} //18
  ++icutflow;

  //Enforce QCD selection only using loose muon ID
  emu &= !isLooseElectron;
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  if(!looseQCDSelection && chargeTest)                                          {fCutFlow->Fill(icutflow);} //19
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight) > 0.)             {fCutFlow->Fill(icutflow);} //20
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight*eventWeight) > 0.) {fCutFlow->Fill(icutflow);} //21
  ++icutflow;

  ///////////////////////////////////////////////////////////////////////////
  // Add additional selection for leptonic tau channels

  mutau_e = lep_tau == 1 && emu; //must be doing a leptonic tau selection
  etau_mu = lep_tau == 2 && emu;
  emu    &= lep_tau == 0;

  //define leptonic tau kinematic cuts
  if(mutau_e) {two_pt_min_ = 20.f; ptdiff_min_ = -1.e10; ptdiff_max_ = +1.e10; mtlep_max_ =  90.f; mtone_max_ = 60.f;} //lep 2 = prompt, lep 1 = tau_l
  if(etau_mu) {one_pt_min_ = 20.f; ptdiff_min_ =    0.f; ptdiff_max_ = +1.e10; mtlep_max_ =  90.f; mttwo_max_ = 60.f;} //lep 1 = prompt, lep 2 = tau_l

  mutau_e &= leptonTwo.pt > two_pt_min_ - sys_buffer; //lepton pT,
  etau_mu &= leptonOne.pt > one_pt_min_ - sys_buffer;
  mutau_e &= fTreeVars.ptdiff > ptdiff_min_ - sys_buffer && fTreeVars.ptdiff < ptdiff_max_ + sys_buffer; //lepton pT - leptonic tau pT
  etau_mu &= fTreeVars.ptdiff > ptdiff_min_ - sys_buffer && fTreeVars.ptdiff < ptdiff_max_ + sys_buffer;
  mutau_e &= fTreeVars.mtlep < mtlep_max_ + sys_buffer; //di-lepton MT
  etau_mu &= fTreeVars.mtlep < mtlep_max_ + sys_buffer;
  mutau_e &= fTreeVars.mtone < mtone_max_ + sys_buffer; //leptonic tau MT (should be very collimated, so small MT)
  etau_mu &= fTreeVars.mttwo < mttwo_max_ + sys_buffer;

  //FIXME: Decide dxy/dz significance cuts
  if(use_dxyz_sig > 1) {
    mutau_e &= std::fabs(leptonTwo.dxySig) < 3.0;
    mutau_e &= std::fabs(leptonTwo.dzSig ) < 4.7;
    etau_mu &= std::fabs(leptonOne.dxySig) < 3.0;
    etau_mu &= std::fabs(leptonOne.dzSig ) < 4.7;
  }
  if(use_dxyz_sig > 0) {
    emu     &= std::fabs(leptonTwo.dxySig) < 3.0;
    emu     &= std::fabs(leptonOne.dxySig) < 3.0;
    emu     &= std::fabs(leptonTwo.dzSig ) < 4.7;
    emu     &= std::fabs(leptonOne.dzSig ) < 4.7;
  }

  //Z/H->e+mu only
  if(emu) {one_pt_min_ = 20.f; two_pt_min_ = 20.f; min_mass_ = 70.f; max_mass_ = 110.f;}
  emu     &= leptonOne.pt > one_pt_min_ - sys_buffer;
  emu     &= leptonTwo.pt > two_pt_min_ - sys_buffer;
  emu     &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;

  if(!(mutau || etau || emu || mumu || ee || mutau_e || etau_mu)) return kTRUE;

  fCutFlow->Fill(icutflow); //22
  ++icutflow;
  if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //23
  ++icutflow;

  //Remove loose ID + same-sign events
  mutau   &= !looseQCDSelection || chargeTest;
  etau    &= !looseQCDSelection || chargeTest;
  emu     &= !looseQCDSelection || chargeTest;
  mutau_e &= !looseQCDSelection || chargeTest;
  etau_mu &= !looseQCDSelection || chargeTest;
  mumu    &= !looseQCDSelection || chargeTest;
  ee      &= !looseQCDSelection || chargeTest;

  if(!(mutau || etau || emu || mumu || ee || mutau_e || etau_mu)) return kTRUE;

  fCutFlow->Fill(icutflow); //24
  ++icutflow;
  if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //25
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  if(!lep_tau) {
    if(mll > min_mass_ - sys_buffer) {
      fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
      FillAllHistograms(set_offset + 8);
      IncrementTimer("SingleFill", true);
    }
  } else if(mutau_e || etau_mu) { //single mass category search for leptonic taus
    if(lep_tau == 1 && mutau_e && mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer ) {
      fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
      FillAllHistograms(set_offset + (kMuTauE - kEMu) + 8);
      IncrementTimer("SingleFill", true);
    }
    if(lep_tau == 2 && etau_mu && mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer) {
      fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
      FillAllHistograms(set_offset + (kETauMu - kEMu) + 8);
      IncrementTimer("SingleFill", true);
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 25/26/27/28 + selection offset: Mass region splitting of searches
  ////////////////////////////////////////////////////////////////////////////

  if(mutau || etau || mutau_e || etau_mu) {
    const bool do_zll_cut = true; //create a region for Z->ee/mumu backgrounds in hadronic tau
    const float prev_min(min_mass_), prev_max(max_mass_);
    const float jtt_region((mutau_e || etau_mu) ? 100.f : 110.f), z_region((mutau_e || etau_mu) ? 50.f : 60.f);
    const float zll_low(85.f), zll_high(100.f);
    const float jtt_low((!do_zll_cut || mutau_e || etau_mu) ? jtt_region : zll_high);
    const float central_high ((!do_zll_cut || mutau_e || etau_mu) ? jtt_region : zll_low );
    const int loc_set_offset = (mutau_e) ? set_offset + (kMuTauE-kEMu) : (etau_mu) ? set_offset + (kETauMu - kEMu) : set_offset;
    //central mass region
    if(mll > z_region   - sys_buffer && mll < central_high + sys_buffer) {min_mass_ = z_region  ; max_mass_ = central_high; FillAllHistograms(loc_set_offset + 25);}
    //j-->tau control
    if(mll > jtt_low - sys_buffer)                                       {min_mass_ = jtt_low   ; max_mass_ = prev_max    ; FillAllHistograms(loc_set_offset + 26);}
    //Z->tau tau control
    if(mll < z_region   + sys_buffer)                                    {min_mass_ = prev_min  ; max_mass_ = z_region    ; FillAllHistograms(loc_set_offset + 27);}
    if(do_zll_cut && (mutau || etau)) {
      //Z->ee/mumu control
      if(mll > zll_low - sys_buffer && mll < zll_high + sys_buffer)      {min_mass_ = zll_low   ; max_mass_ = zll_high    ; FillAllHistograms(loc_set_offset + 28);}
    }
    min_mass_ = prev_min; max_mass_ = prev_max;
  }

  // ////////////////////////////////////////////////////////////////////////////
  // // Set 21 + selection offset: 1-prong taus
  // ////////////////////////////////////////////////////////////////////////////

  // if((mutau || etau) && tauDecayMode <= 1) {
  //   FillAllHistograms(set_offset + 21);
  // }

  // ////////////////////////////////////////////////////////////////////////////
  // // Set 22 + selection offset: 3-prong taus
  // ////////////////////////////////////////////////////////////////////////////

  // if((mutau || etau) && tauDecayMode >= 10 && tauDecayMode <= 11) {
  //   FillAllHistograms(set_offset + 22);
  // }

  if(fTriggerTesting && !lep_tau && (emu || mumu || ee)) {
    if(emu) { //e+mu trigger testing

      ////////////////////////////////////////////////////////////////////////////
      // Set 63 + selection offset: Electron triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOne.pt > electron_trig_pt_) {
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
      if(leptonTwo.pt > muon_trig_pt_) {
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
      if(leptonOne.pt > electron_trig_pt_ && leptonTwo.pt > muon_trig_pt_) {
        FillAllHistograms(set_offset + 65);
      }
    } else { //ee and mumu trigger testing
      const double trig_threshold = (ee) ? electron_trig_pt_ : muon_trig_pt_;

      ////////////////////////////////////////////////////////////////////////////
      // Set 63 + selection offset: lepton two triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwo.pt > trig_threshold) {
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

  if(!lep_tau) {
    ////////////////////////////////////////////////////////////////////////////
    // Set 24-25 : Mass window sets
    ////////////////////////////////////////////////////////////////////////////
    //Z window
    if(emu && mll < 96.f && mll > 85.f)    FillAllHistograms(set_offset + 24);
    //higgs window
    if(fDoHiggs && emu && mll < 130.f && mll > 120.f)  FillAllHistograms(set_offset + 25);
  }

  if(!fDoMVASets) return kTRUE;

  if(!lep_tau) {
    ////////////////////////////////////////////////////////////////////////////
    // Set 9-18 : BDT Cut categories
    ////////////////////////////////////////////////////////////////////////////
    //Set event weight to ignore training sample
    Float_t prev_wt = eventWeight;
    //only use weight if MC or is same sign data
    eventWeight = (fIsData == 0 || !chargeTest) ? std::fabs(fTreeVars.eventweightMVA) : std::fabs(prev_wt); //use abs to remove gen weight sign
    bool doMVASets = fDoMVASets;
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
        // category = Category("zmutau_e");
        // FillAllHistograms(set_offset + kMuTauE - kEMu + 9 + category);
        // category = Category("zetau_mu");
        // FillAllHistograms(set_offset + kETauMu - kEMu + 9 + category);
      } else if(mumu) {
        category = Category("zemu");
      } else if(ee) {
        category = Category("zemu");
      }

      if(category >= 0 && !emu) //do emu separately, rest do here
        FillAllHistograms(set_offset + 9 + category);

      //Total background higgs MVAs 14 - 18
      category = -1;
      if(fDoHiggs && doMVASets) {
        if(mutau) {
          category = Category("hmutau");
        } else if(etau) {
          category = Category("hetau");
        } else if(emu) {
          category = Category("hemu");
          FillAllHistograms(set_offset + 14 + category);
          // category = Category("hmutau_e");
          // FillAllHistograms(set_offset + kMuTauE - kEMu + 14 + category);
          // category = Category("hetau_mu");
          // FillAllHistograms(set_offset + kETauMu - kEMu + 14 + category);
        } else if(mumu) {
          category = Category("hemu");
        } else if(ee) {
          category = Category("hemu");
        }
      }
    }

    if(fDoHiggs && category >= 0 && !emu) //do emu separately, rest do here
      FillAllHistograms(set_offset + 14 + category);

    //restore event weight
    eventWeight = prev_wt;
  }


  return kTRUE;
}
