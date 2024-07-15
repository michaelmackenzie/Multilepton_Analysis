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
  //perform the lepton energy scale study outside of PassesCuts() call to allow event migration
  if(fEventSets[index] && fDoLepESHists) FillLepESHistogram(fEventHist[index]);
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

    fEventSets [kMuTau + 7] = fTopTesting; //inverted b-tag set

    fEventSets [kMuTau + 8] = 1;
    fTreeSets  [kMuTau + 8] = 1;
    fTreeSets  [kMuTau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
    // fSysSets   [kMuTau + 8] = 1;

    // MVA categories
    if(fDoMVASets) {
      for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kMuTau + i] = 1;
    }

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
    fEventSets [kMuTau + 29] = 1; //debug category
    fSysSets   [kMuTau + 29] = 0;

    const bool embed_testing = false;
    if(embed_testing) { //use normal debug sets for embedding testing
      fQCDFakeTauTesting = 0;
      fWJFakeTauTesting  = 0;
      fTTFakeTauTesting  = 0;
      fJetTauTesting     = 0;
      fFakeLeptonTesting = 0;
    }

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

    //Testing embedding
    if(embed_testing) {
      fEventSets [kMuTau + 30] = 1; //tight Z->ll pT cuts
      fEventSets [kMuTau + 31] = 1; //barrel |eta| events
      fEventSets [kMuTau + 32] = 1; //endcap |eta| events
      fEventSets [kMuTau + 33] = 1; //|phi(met)| < pi/2
      fEventSets [kMuTau + 34] = 1; //|phi(met)| > pi/2
      fEventSets [kMuTau + 35] = 1; //MET < 40
      fEventSets [kMuTau + 36] = 1; //MET > 40
      // fEventSets [kMuTau + 37] = 1; //no FSR in DY MC Z->ll
      // fEventSets [kMuTau + 38] = 1; //adjusted Embedding energy scale
      fEventSets [kMuTau + 39] = 1; //lower lep delta phi
      fEventSets [kMuTau + 40] = 1; //high lep delta phi
      fEventSets [kMuTau + 41] = 1; //lower lep pt
      fEventSets [kMuTau + 42] = 1; //higher lep pt
      fEventSets [kMuTau + 43] = 1; //Tighter cuts
      fEventSets [kMuTau + 44] = 1; //High BDT score
      fEventSets [kMuTau + 45] = 1; //Low BDT score
      fEventSets [kMuTau + 46] = 1; //Not low BDT score
      fEventSets [kMuTau + 47] = 1; //High nPV
      fEventSets [kMuTau + 48] = 1; //Low nPV
    }

  }
  if(etau) {
    if(fCutFlowTesting) {
      fEventSets [kETau + 1] = 1; // all events
      fEventSets [kETau + 2] = 1;
      fEventSets [kETau + 3] = 1;
      fEventSets [kETau + 4] = 1;
    }

    fEventSets [kETau + 7] = fTopTesting; //inverted b-tag set

    fEventSets [kETau + 8] = 1;
    fTreeSets  [kETau + 8] = 1;
    fTreeSets  [kETau + 8+fMisIDOffset] = fIsData != 0; //save Loose ID data for MVA training
    // fSysSets   [kETau + 8] = 1;

    // MVA categories
    if(fDoMVASets) {
      for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kETau + i] = 1;
    }

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
    fEventSets [kETau + 29] = 1; //debug category
    fSysSets   [kETau + 29] = 0;

    const bool embed_testing = false;
    if(embed_testing) { //use normal debug sets for embedding testing
      fQCDFakeTauTesting = 0;
      fWJFakeTauTesting  = 0;
      fTTFakeTauTesting  = 0;
      fJetTauTesting     = 0;
      fFakeLeptonTesting = 0;
    }

    // jet --> tau DRs
    fEventSets [kETau + 30] = fQCDFakeTauTesting; //QCD
    fEventSets [kETau + 31] = fWJFakeTauTesting; //W+Jets
    fEventSets [kETau + 32] = fTTFakeTauTesting; //Top

    // jet --> tau DRs with MC taus
    fEventSets [kETau + 33] = fJetTauTesting; //Nominal selection without j-->tau weights, loose ID only
    fEventSets [kETau + 35] = fFakeLeptonTesting; //Nominal selection, MC fake leptons
    fEventSets [kETau + 36] = fQCDFakeTauTesting; //QCD
    fEventSets [kETau + 37] = fWJFakeTauTesting; //W+Jets
    fEventSets [kETau + 38] = fTTFakeTauTesting; //Top

    //Testing embedding
    if(embed_testing) {
      fEventSets [kETau + 30] = 1; //tight Z->ll pT cuts
      fEventSets [kETau + 31] = 1; //barrel |eta| events
      fEventSets [kETau + 32] = 1; //endcap |eta| events
      fEventSets [kETau + 33] = 1; //|phi(met)| < pi/2
      fEventSets [kETau + 34] = 1; //|phi(met)| > pi/2
      fEventSets [kETau + 35] = 1; //MET < 40
      fEventSets [kETau + 36] = 1; //MET > 40
      // fEventSets [kETau + 37] = 1; //no FSR in DY MC Z->ll
      // fEventSets [kETau + 38] = 1; //adjusted Embedding energy scale
      fEventSets [kETau + 39] = 1; //lower lep delta phi
      fEventSets [kETau + 40] = 1; //high lep delta phi
      fEventSets [kETau + 41] = 1; //lower lep pt
      fEventSets [kETau + 42] = 1; //higher lep pt
      fEventSets [kETau + 43] = 1; //Tighter cuts
      fEventSets [kETau + 44] = 1; //High BDT score
      fEventSets [kETau + 45] = 1; //Low BDT score
      fEventSets [kETau + 46] = 1; //Not low BDT score
      fEventSets [kETau + 47] = 1; //High nPV
      fEventSets [kETau + 48] = 1; //Low nPV
    }

  }
  if(emu) {
    if(fCutFlowTesting) {
      fEventSets [kEMu  + 1] = 1; // all events
      fEventSets [kEMu  + 2] = 1; // after pT cuts
      fEventSets [kEMu  + 3] = 1; // after eta, mass, trigger cuts
      fEventSets [kEMu  + 4] = 1; // after additional IDs
    }

    if(!lep_tau) {
      fEventSets [kEMu  + 7] = fTopTesting; //inverted b-tag set
      fEventSets [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8] = 1;
      fTreeSets  [kEMu  + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training
      fSysSets   [kEMu  + 8] = 1;

      fEventSets [kEMu  + 20] = fDoEleIDStudy; //test set
      // fSysSets   [kEMu  + 20] = 1;

      // MVA categories
      if(fDoMVASets) {
        for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kEMu  + i] = 1;
      }
      fSysSets[kEMu  + 9  + fMVAConfig->categories_["zemu"].size()] = 1; //most significant category
      fSysSets[kEMu  + 8  + fMVAConfig->categories_["zemu"].size()] = 1; //second most significant category
      fSysSets[kEMu  + 7  + fMVAConfig->categories_["zemu"].size()] = 1; //third most significant category
      if(fDoHiggs) {
        fSysSets[kEMu  + 14 + fMVAConfig->categories_["hemu"].size()] = 1; //start with most significant category
        fSysSets[kEMu  + 13 + fMVAConfig->categories_["hemu"].size()] = 1; //second most significant category
      }

      // MVA categories, split into barrel/endcap
      if(fDoEleIDStudy) {
        for(int imva = 0; imva < 5; ++imva) {
          fEventSets[kEMu  + imva + 70] = 1; //barrel sets
          fEventSets[kEMu  + imva + 80] = 1; //endcap sets
          if(imva > 1) { //only do systematics for the top 3 categories
            fSysSets  [kEMu  + imva + 70] = 1;
            fSysSets  [kEMu  + imva + 80] = 1;
          }
        }
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

    //Testing MET uncertainties
    fEventSets [kMuMu + 30] = 1; //tight Z->ll pT cuts
    fEventSets [kMuMu + 31] = 1; //barrel |eta| events
    fEventSets [kMuMu + 32] = 1; //endcap |eta| events
    fEventSets [kMuMu + 33] = 1; //|phi(met)| < pi/2
    fEventSets [kMuMu + 34] = 1; //|phi(met)| > pi/2
    fEventSets [kMuMu + 35] = 1; //MET < 40
    fEventSets [kMuMu + 36] = 1; //MET > 40
    fEventSets [kMuMu + 37] = 1; //no FSR in DY MC Z->ll
    // fEventSets [kMuMu + 38] = 1; //adjusted Embedding energy scale
    fEventSets [kMuMu + 39] = 1; //lower lep delta phi
    fEventSets [kMuMu + 40] = 1; //high lep delta phi
    fEventSets [kMuMu + 41] = 1; //lower lep pt
    fEventSets [kMuMu + 42] = 1; //higher lep pt
    fEventSets [kMuMu + 43] = 1; //remove an eta region

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
      fEventSets[kMuTauE + 7] = fTopTesting; //inverted b-tag set
      fEventSets[kMuTauE + 8] = 1;
      fSysSets  [kMuTauE + 8] = 1;
      fTreeSets [kMuTauE + 8] = 1;
      fTreeSets [kMuTauE + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      if(fDoMVASets) {
        for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kMuTauE + i] = 1;
      }

      fEventSets[kMuTauE + 25] = 1; //nominal mass
      fSysSets  [kMuTauE + 25] = 1;
      fEventSets[kMuTauE + 26] = 1; //high mass
      fSysSets  [kMuTauE + 26] = 1;
      fEventSets[kMuTauE + 27] = 1; //low mass
      fSysSets  [kMuTauE + 27] = 1;
      fEventSets[kMuTauE + 29] = 1; //debug category
      fSysSets  [kMuTauE + 29] = 0;

      // // MVA categories
      // for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kMuTauE + i] = 1;
    }
    //e+tau_mu
    if(lep_tau == 2) {
      fEventSets[kETauMu + 7] = fTopTesting; //inverted b-tag set
      fEventSets[kETauMu + 8] = 1;
      fSysSets  [kETauMu + 8] = 1;
      fTreeSets [kETauMu + 8] = 1;
      fTreeSets [kETauMu + 8+fQcdOffset] = fIsData != 0; //save SS data for QCD training

      // MVA categories
      if(fDoMVASets) {
        for(int i = 9; i < ((fDoHiggs) ? 19 : 15); ++i) fEventSets[kETauMu + i] = 1;
      }

      fEventSets[kETauMu + 25] = 1; //nominal mass
      fSysSets  [kETauMu + 25] = 1;
      fEventSets[kETauMu + 26] = 1; //high mass
      fSysSets  [kETauMu + 26] = 1;
      fEventSets[kETauMu + 27] = 1; //low mass
      fSysSets  [kETauMu + 27] = 1;
      fEventSets[kETauMu + 29] = 1; //debug category
      fSysSets  [kETauMu + 29] = 0;
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
void CLFVHistMaker::BookLepESHistograms(Int_t i, const char* dirname) {
  auto folder = fDirectories[0*fn + i];
  for(int lep = 0; lep < 2; ++lep) { //one, two
    for(int version = 0; version < 3; ++version) { //nominal, up, down
      const char* lep_tag  = (lep == 0) ? "one" : "two";
      const char* name_tag = (version == 0) ? "" : (version == 1) ? "_up" : "_down";
      LepESHist_t& hists = fEventHist[i]->LepESHists[lep];
      Utilities::BookH1F(hists.hLepM           [version], Form("lepes_%slepm%s"           , lep_tag, name_tag), Form("%s: %s Mass%s"          ,dirname,lep_tag,name_tag), 65, 40, 170, folder);
      Utilities::BookH1F(hists.hLepMEstimate   [version], Form("lepes_%slepmestimate%s"   , lep_tag, name_tag), Form("%s: %s Mass Estimate%s" ,dirname,lep_tag,name_tag), 65, 40, 170, folder);
      Utilities::BookH1F(hists.hLepMEstimateTwo[version], Form("lepes_%slepmestimatetwo%s", lep_tag, name_tag), Form("%s: %s Mass Estimate%s" ,dirname,lep_tag,name_tag), 65, 40, 170, folder);
      Utilities::BookH1F(hists.hLepPt          [version], Form("lepes_%sleppt%s"          , lep_tag, name_tag), Form("%s: %s Lep pT%s"        ,dirname,lep_tag,name_tag), 25,  0, 100, folder);
      Utilities::BookH1F(hists.hOnePt          [version], Form("lepes_%sonept%s"          , lep_tag, name_tag), Form("%s: %s One pT%s"        ,dirname,lep_tag,name_tag), 50,  0, 100, folder);
      Utilities::BookH1F(hists.hTwoPt          [version], Form("lepes_%stwopt%s"          , lep_tag, name_tag), Form("%s: %s Two pT%s"        ,dirname,lep_tag,name_tag), 50,  0, 100, folder);
      Utilities::BookH1F(hists.hLepPtRatio     [version], Form("lepes_%slepptratio%s"     , lep_tag, name_tag), Form("%s: %s Lep pT ratio%s"  ,dirname,lep_tag,name_tag), 25,  0,   3, folder);
      Utilities::BookH1F(hists.hMET            [version], Form("lepes_%smet%s"            , lep_tag, name_tag), Form("%s: %s MET%s"           ,dirname,lep_tag,name_tag), 50,  0, 100, folder);
      Utilities::BookH1F(hists.hMTOne          [version], Form("lepes_%smtone%s"          , lep_tag, name_tag), Form("%s: %s MTOne%s"         ,dirname,lep_tag,name_tag), 50,  0, 100, folder);
      Utilities::BookH1F(hists.hMTTwo          [version], Form("lepes_%smttwo%s"          , lep_tag, name_tag), Form("%s: %s MTTwo%s"         ,dirname,lep_tag,name_tag), 50,  0, 100, folder);
      Utilities::BookH1F(hists.hMTLep          [version], Form("lepes_%smtlep%s"          , lep_tag, name_tag), Form("%s: %s MTLep%s"         ,dirname,lep_tag,name_tag), 50,  0, 100, folder);
      Utilities::BookH1F(hists.hOneMETDeltaPhi [version], Form("lepes_%sonemetdeltaphi%s" , lep_tag, name_tag), Form("%s: %s OneMETDeltaPhi%s",dirname,lep_tag,name_tag), 30,  0, 3.5, folder);
      Utilities::BookH1F(hists.hTwoMETDeltaPhi [version], Form("lepes_%stwometdeltaphi%s" , lep_tag, name_tag), Form("%s: %s TwoMETDeltaPhi%s",dirname,lep_tag,name_tag), 30,  0, 3.5, folder);
      Utilities::BookH1F(hists.hBeta[0]        [version], Form("lepes_%sbeta0%s"          , lep_tag, name_tag), Form("%s: %s Beta 0%s"        ,dirname,lep_tag,name_tag), 30,  0,   3, folder);
      Utilities::BookH1F(hists.hBeta[1]        [version], Form("lepes_%sbeta1%s"          , lep_tag, name_tag), Form("%s: %s Beta 1%s"        ,dirname,lep_tag,name_tag), 30,  0,   3, folder);
      Utilities::BookH1F(hists.hMVA            [version], Form("lepes_%smva%s"            , lep_tag, name_tag), Form("%s: %s MVA%s"           ,dirname,lep_tag,name_tag), 20,  0,   1, folder);
    }
  }
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

      //Check for study histograms
      if(fDoLepESHists) BookLepESHistograms(i, dirname);

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

      }
      Utilities::BookH1F(fEventHist[i]->hZPt[5]    , "zpt5"          , Form("%s: Z Pt"           ,dirname)  ,  50,   0, 150, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[5]  , "zmass5"        , Form("%s: Z Mass"         ,dirname)  ,  70,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hZEta[1]   , "zeta1"         , Form("%s: ZEta"           ,dirname)  ,  50, -10,  10, folder);

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
        if(fDoSystematics == -2 && !fIsSignal) { //only do nominal systematics for non-signal
          if(sys > 0) break;
        }
        const TString name = fSystematics.GetName(sys);
        if(name == "") continue; //only initialize defined systematics
        if(name.BeginsWith("QCD") && fSelection.EndsWith("tau")) continue; //skip QCD OS --> SS in tau channels
        if(name.BeginsWith("JetToTau") && !fSelection.EndsWith("tau")) continue; //only consider j-->tau in tau channels

        if(!fSparseHists || (fSelection == "emu" || fSelection == "mumu" || fSelection == "ee")) {
          Utilities::BookH1F(fSystematicHist[i]->hLepM        [sys], Form("lepm_%i"        , sys), Form("%s: LepM %i"                   , dirname, sys) , 280,  40, 180, folder);
        }
        if(!fSparseHists) {
          Utilities::BookH1F(fSystematicHist[i]->hOnePt       [sys], Form("onept_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
          Utilities::BookH1F(fSystematicHist[i]->hTwoPt       [sys], Form("twopt_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
          Utilities::BookH1F(fSystematicHist[i]->hWeightChange[sys], Form("weightchange_%i", sys), Form("%s: Relative weight change %i" , dirname, sys) ,  30, -2.,   2, folder);
        }
        if(!fSparseHists || !(fSelection == "emu" || fSelection == "mumu" || fSelection == "ee")) {
          for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
            Utilities::BookH1D(fSystematicHist[i]->hMVA[j][sys], Form("mva%i_%i",j, sys), Form("%s: %s MVA %i" ,dirname, fMVAConfig->names_[j].Data(), sys),
                               fMVAConfig->NBins(j, i), fMVAConfig->Bins(j, i).data(), folder);
            Utilities::BookH1D(fSystematicHist[i]->hMVADiff[j][sys], Form("mvadiff%i_%i",j, sys)     , Form("%s: %s MVADiff %i" ,dirname, fMVAConfig->names_[j].Data(), sys),
                               25, -0.25, 0.25, folder);
          }
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
void CLFVHistMaker::FillLepESHistogram(EventHist_t* Hist) {
  //Event information that may be altered, to restore the event information after a systematic shift
  const TLorentzVector o_lv1(*leptonOne.p4), o_lv2(*leptonTwo.p4);
  const float o_met(met), o_metPhi(metPhi);
  float o_mvas[fMVAConfig->names_.size()], o_cdfs[fMVAConfig->names_.size()], o_fofp[fMVAConfig->names_.size()];
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    o_mvas[i] = fMvaOutputs[i];
    o_cdfs[i] = fMvaCDFs[i];
    o_fofp[i] = fMvaFofP[i];
  }

  for(int lep = 0; lep < 2; ++lep) { //one, two
    for(int version = 0; version < 3; ++version) { //nominal, up, down
      LepESHist_t& hists = Hist->LepESHists[lep];
      bool reeval = false;
      if(!fIsData && version != 0) {
        reeval = true;
        Lepton_t& lepton = (lep == 0) ? leptonOne : leptonTwo;
        //shift the lepton energy scale
        const float scale = (lepton.ES[version]/lepton.ES[0]);
        EnergyScale(scale, lepton, &met, &metPhi); //propagate the shift to the MET
        SetKinematics();
        EvalMVAs();
      }
      const bool accepted = PassesCuts(); //ensure the event isn't cut after the variation
      if(accepted) {
        const float wt(eventWeight*genWeight);
        hists.hLepM           [version]->Fill(fTreeVars.lepm           , wt);
        hists.hLepMEstimate   [version]->Fill(fTreeVars.mestimate      , wt);
        hists.hLepMEstimateTwo[version]->Fill(fTreeVars.mestimatetwo   , wt);
        hists.hLepPt          [version]->Fill(fTreeVars.leppt          , wt);
        hists.hOnePt          [version]->Fill(fTreeVars.leponept       , wt);
        hists.hTwoPt          [version]->Fill(fTreeVars.leptwopt       , wt);
        hists.hLepPtRatio     [version]->Fill(fTreeVars.ptratio        , wt);
        hists.hMET            [version]->Fill(fTreeVars.met            , wt);
        hists.hMTOne          [version]->Fill(fTreeVars.mtone          , wt);
        hists.hMTTwo          [version]->Fill(fTreeVars.mttwo          , wt);
        hists.hMTLep          [version]->Fill(fTreeVars.mtlep          , wt);
        hists.hOneMETDeltaPhi [version]->Fill(fTreeVars.onemetdeltaphi , wt);
        hists.hTwoMETDeltaPhi [version]->Fill(fTreeVars.twometdeltaphi , wt);
        hists.hBeta[0]        [version]->Fill(fTreeVars.beta1          , wt);
        hists.hBeta[1]        [version]->Fill(fTreeVars.beta2          , wt);
        const int imva = (mutau) ? 1 : (etau) ? 3 : (emu && lep_tau == 0) ? 5 : (lep_tau == 1) ? 7 : 9;
        hists.hMVA            [version]->Fill(fMvaUse[imva]            , wt);
      }

      //Restore the kinematics
      if(reeval) {
        leptonOne.setP(o_lv1);
        leptonTwo.setP(o_lv2);
        met    = o_met;
        metPhi = o_metPhi;
        SetKinematics();
        for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
          fMvaOutputs[i] = o_mvas[i];
          fMvaCDFs[i]    = o_cdfs[i];
          fMvaFofP[i]    = o_fofp[i];
        }
      }
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

  TLorentzVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;
  //for removing or alternate DY reweighting weights
  double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight; //no DY Z pT vs M weights
  float tmp_1, tmp_2, sys_reco_weight(1.);
  double recoweight = (fIsDY) ? bareweight*fZPtWeight->GetWeight(fYear, lepSys.Pt(), lepSys.M(), /*use reco weights*/ true, tmp_1, tmp_2, sys_reco_weight) : bareweight;
  sys_reco_weight *= bareweight;

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

    // const double lepDelR   = std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4));
    // const double lepDelPhi = std::fabs(leptonOne.p4->DeltaPhi(*leptonTwo.p4));

    //test Drell-Yan reweighting
    Hist->hLepPt[1]     ->Fill(lepSys.Pt()            ,bareweight);
    Hist->hLepPt[2]     ->Fill(lepSys.Pt()            ,recoweight);
    Hist->hLepM[1]      ->Fill(lepSys.M()             ,bareweight);
    Hist->hLepM[2]      ->Fill(lepSys.M()             ,recoweight);
    Hist->hLepM[3]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
    Hist->hLepM[4]      ->Fill(lepSys.M()             ,eventWeight*genWeight);

  }
  const double zpt   = (zPt   <  0.f) ? lepSys.Pt()  : zPt;
  const double zmass = (zMass <  0.f) ? lepSys.M()   : zMass;
  Hist->hZPt[5]       ->Fill(zpt  , eventWeight*genWeight);
  Hist->hZMass[5]     ->Fill(zmass, eventWeight*genWeight);
  Hist->hZEta [1]     ->Fill(zEta , bareweight           );

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

  if(!fSparseHists) {
    for(UInt_t jet = 0; jet < nJets20; ++jet) {
      float jpt = jetsPt[jet], jeta = jetsEta[jet], wt = genWeight*eventWeight;
      if(btagWeight > 0.) wt /= btagWeight; //remove previous correction
      if(std::fabs(jeta) >= 2.5) continue; //outside the b-tagging region

      // Hist->hJetsFlavor->Fill(jetsFlavor[jet], genWeight*eventWeight);
      int flavor = std::abs(jetsFlavor[jet]);
      if(flavor > 20) flavor = 1; //unknown parent defaults to light jet

      int index = 0; //light jets
      if(flavor == 4)      index = 1; // c-jet
      else if(flavor == 5) index = 2; // b-jet
      Hist->hJetsPtVsEta[index]->Fill(jeta, jpt, wt);
      if(jetsBTag[jet] > 0) { //Loose b-tag
        Hist->hBJetsLPtVsEta[index]->Fill(jeta, jpt, wt);
        if(jetsBTag[jet] > 1) { //Medium b-tag
          Hist->hBJetsMPtVsEta[index]->Fill(jeta, jpt, wt);
          if(jetsBTag[jet] > 2) { //Tight b-tag
            Hist->hBJetsPtVsEta[index]->Fill(jeta, jpt, wt);
          }
        }
      }
    }
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

  const bool isSameFlavor = std::abs(leptonOne.flavor) == std::abs(leptonTwo.flavor);
  const bool isMuTau = leptonOne.isMuon    () && leptonTwo.isTau     ();
  const bool isETau  = leptonOne.isElectron() && leptonTwo.isTau     ();
  const bool isEData = leptonOne.isElectron() || leptonTwo.isElectron();
  const bool isMData = leptonOne.isMuon()     || leptonTwo.isMuon    ();
  const bool isEMu   = fSelection == "emu"; //for Z->e+mu, ignore many background-only effects

  //Event information that may be altered, to restore the event information after a systematic shift
  const TLorentzVector o_lv1(*leptonOne.p4), o_lv2(*leptonTwo.p4), o_jet(*jetOne.p4);
  const float o_met(met), o_metPhi(metPhi);
  float o_mvas[fMVAConfig->names_.size()], o_cdfs[fMVAConfig->names_.size()], o_fofp[fMVAConfig->names_.size()], o_use[fMVAConfig->names_.size()];
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    o_mvas[i] = fMvaOutputs[i];
    o_cdfs[i] = fMvaCDFs[i];
    o_fofp[i] = fMvaFofP[i];
    o_use [i] = fMvaUse [i];
  }

  const float rho = (fIsEmbed) ? 0.5f : 1.f; //for embedding correlation
  const float rho_t = (rho < 1.f) ? std::sqrt(1.f*1.f - rho*rho) : 0.f;
  //wt = 1 + delta --> 1 + rho*delta = 1 + rho*(wt - 1) = rho*wt + (1-rho)

  const bool o_pass = PassesCuts();
  const float nominal_weight = eventWeight*genWeight;

  //count number of systematics filled
  int nfilled = 0;
  for(int sys = 0; sys < kMaxSystematics; ++sys) {
    if(fDoSystematics == -2 && !fIsSignal) { //only do nominal systematics for non-signal
      if(sys > 0) break;
    }
    float weight = nominal_weight;
    bool reeval = false;
    const TString name = fSystematics.GetName(sys);
    if(name == "") continue; //only process defined systematics

    if( (isMuTau || isETau || isSameFlavor || isEMu) && name.Contains("QCD")) continue; //only relevant to leptonic tau category MVA fits
    if(!(isMuTau || isETau) && name.Contains("Tau")) continue; //only relevant to tau categories
    if(name.BeginsWith("Emb") && !fIsEmbed) continue; //only relevant for embedding samples

    if(name == "Nominal")  weight = weight; //do nothing
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
      if(fIsData || !isEData || !fIsEmbed) continue;
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
      if(fIsData || !isEData || !fIsEmbed) continue;
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
      if(!fIsEmbed || !isEData || !fIsEmbed) continue;
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
      if(!fIsEmbed) continue;
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
      if(fIsData || !isMData || !fIsEmbed) continue;
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
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
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
      weight = EvalJetToTauStatSys(proc, dm_bin, alt_bin, fSystematics.IsUp(sys));
    } else if(name == "ZPt") {
      if(!(fIsSignal || fIsDY)) continue;
      if(fSystematics.IsUp(sys)) weight *= zPtWeightUp          / zPtWeight      ;
      else                       weight *= zPtWeightDown        / zPtWeight      ;
    } else if(name == "SignalMixing") {
      if(!fIsSignal) continue;
      if(fSystematics.IsUp(sys)) weight *= signalZMixingWeightUp  /signalZMixingWeight;
      else                       weight *= signalZMixingWeightDown/signalZMixingWeight;
    } else if(name == "SignalBDT") {
      if(!fIsSignal || !isEMu) continue;
      if(fSystematics.IsUp(sys)) weight *= 1./bdtWeight; //remove weight for up
      else                       weight *=    bdtWeight; //apply twice for down
    } else if(name == "TheoryScaleR") {
      if(fIsData || fIsEmbed) continue;
      if(fSystematics.IsUp(sys)) weight *= LHEScaleRWeightUp;
      else                       weight *= LHEScaleRWeightDown;
    } else if(name == "TheoryScaleF") {
      if(fIsData || fIsEmbed) continue;
      if(fSystematics.IsUp(sys)) weight *= LHEScaleFWeightUp;
      else                       weight *= LHEScaleFWeightDown;
    } else if(name == "TheoryPDF") {
      if(fIsData || fIsEmbed) continue;
      if(fSystematics.IsUp(sys)) weight *= LHEPdfWeightMax;
      else                       weight *= std::max(0.f, 2.f - LHEPdfWeightMax);
    } else if(name.BeginsWith("TheoryPDF")) {
      if(fIsData || fIsEmbed) continue;
      TString bin_s = name;
      bin_s.ReplaceAll("TheoryPDF", "");
      const int bin = std::stoi(bin_s.Data()) + 1; //array index 1 = TheoryPDF0 as index 0 is nominal
      if(fSystematics.IsUp(sys))          weight *=     LHEPdfWeight[bin];
      else if(bin == 31 && fYear != 2016) weight *=     LHEPdfWeight[bin+1]; //alpha variation has separate up/down weights
      else                                weight *= 1.f/LHEPdfWeight[bin];
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
      if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
      if(fSystematics.IsUp(sys)) {
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
      if(fVerbose > 5) printf("CLFVHistMaker::%s: (Possibly) applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
      //|eta| Regions: EmbEleES = |eta| < 1.5; EmbEleES1 = |eta| > 1.5;
      const float eta_min = (name.EndsWith("1")) ? 1.5f : 0.f;
      const float eta_max = (name.EndsWith("1")) ? 2.5f : 1.5f;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron() && std::fabs(leptonOne.eta) >= eta_min && std::fabs(leptonOne.eta) < eta_max && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0.) {
          reeval = true;
          EnergyScale(leptonOne.ES[1] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        }
        if(leptonTwo.isElectron() && std::fabs(leptonTwo.eta) >= eta_min && std::fabs(leptonTwo.eta) < eta_max && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.) {
          reeval = true;
          EnergyScale(leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
        }
      } else {
        if(leptonOne.isElectron() && std::fabs(leptonOne.eta) >= eta_min && std::fabs(leptonOne.eta) < eta_max && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0.) {
          reeval = true;
          EnergyScale(leptonOne.ES[2] / leptonOne.ES[0], leptonOne, &met, &metPhi);
        }
        if(leptonTwo.isElectron() && std::fabs(leptonTwo.eta) >= eta_min && std::fabs(leptonTwo.eta) < eta_max && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.) {
          reeval = true;
          EnergyScale(leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
        }
      }
    } else if(name == "MuonES") {
      if(fIsEmbed || fIsData || !isMData) continue;
      reeval = true;
      if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
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
      if(!fIsEmbed || !isMData || fUseEmbedRocco != 1) continue; //only process for embedding, and only if using Rocco systematics
      reeval = true;
      if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
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
      if(!fIsEmbed || !isMData || fUseEmbedRocco == 1) continue; //don't do binned ES if using Rocco systematics
      if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
      const float nsigma = 1.f; //N(sigma) variation for the template
      TString bin_s = name; bin_s.ReplaceAll("EmbMuonES", "");
      const int bin = std::abs(std::stoi(bin_s.Data()));
      if(bin < 0 || bin > 2) {printf("CLFVHistMaker::%s: Unknown sys bin %s\n", __func__, name.Data()); continue;}
      const float eta_min = (bin == 0) ? -1.f : (bin == 1) ? 1.2f : 2.1f;
      const float eta_max = (bin == 0) ? 1.2f : (bin == 1) ? 2.1f : 2.4f;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[1] > 0. && leptonOne.eta >= eta_min && leptonOne.eta < eta_max) {
          EnergyScale(1.f + nsigma*(leptonOne.ES[1] / leptonOne.ES[0] - 1.f), leptonOne, &met, &metPhi); reeval = true;
        }
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0. && leptonTwo.eta >= eta_min && leptonTwo.eta < eta_max) {
          EnergyScale(1.f + nsigma*(leptonTwo.ES[1] / leptonTwo.ES[0] - 1.f), leptonTwo, &met, &metPhi); reeval = true;
        }
      } else {
        if(leptonOne.isMuon() && leptonOne.ES[0] > 0. && leptonOne.ES[2] > 0. && leptonOne.eta >= eta_min && leptonOne.eta < eta_max) {
          EnergyScale(1.f + nsigma*(leptonOne.ES[2] / leptonOne.ES[0] - 1.f), leptonOne, &met, &metPhi); reeval = true;
        }
        if(leptonTwo.isMuon() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0. && leptonTwo.eta >= eta_min && leptonTwo.eta < eta_max) {
          EnergyScale(1.f + nsigma*(leptonTwo.ES[2] / leptonTwo.ES[0] - 1.f), leptonTwo, &met, &metPhi); reeval = true;
        }
      }
    } else if(name.BeginsWith("TauEleES")) { //Decay mode binned energy scale (electrons faking taus)
      if(fIsData || !leptonTwo.isTau()) continue;
      if(std::abs(leptonTwo.genFlavor) == 11) { //check that it's an electron faking a hadronic tau
        int dm_bin(0);
        if     (name == "TauEleES0") dm_bin = 0;
        else if(name == "TauEleES1") dm_bin = 1;
        else if(name == "TauEleES2") dm_bin = 2;
        else if(name == "TauEleES3") dm_bin = 3;
        if((tauDecayMode % 10 + 2*(tauDecayMode / 10)) == dm_bin) {
          if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                                  __func__, name.Data(), fSystematics.IsUp(sys));
          reeval = true;
          if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
            if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
              EnergyScale((1.f-rho) + rho*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
          } else {
            if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
              EnergyScale((1.f-rho) + rho*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
          }
        }
      }
    } else if(name.BeginsWith("TauMuES")) { //Decay mode binned energy scale (muons faking taus)
      if(fIsData || !leptonTwo.isTau()) continue;
      if(std::abs(leptonTwo.genFlavor) == 13) { //check that it's a muon faking a hadronic tau
        int dm_bin(0);
        if     (name == "TauMuES0") dm_bin = 0;
        else if(name == "TauMuES1") dm_bin = 1;
        else if(name == "TauMuES2") dm_bin = 2;
        else if(name == "TauMuES3") dm_bin = 3;
        if((tauDecayMode % 10 + 2*(tauDecayMode / 10)) == dm_bin) {
          if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                                  __func__, name.Data(), fSystematics.IsUp(sys));
          reeval = true;
          if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
            if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
              EnergyScale((1.f-rho) + rho*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
          } else {
            if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
              EnergyScale((1.f-rho) + rho*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
          }
        }
      }
    } else if(name == "TauES") { //Decay mode inclusive energy scale (genuine hadronic taus)
      if(fIsData || !leptonTwo.isTau()) continue;
      if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
      if(std::abs(leptonTwo.genFlavor) == 15) { //check that it's a genuine hadronic tau
        reeval = true;
        if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
          if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
            EnergyScale((1.f-rho) + rho*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
        } else {
          if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
            EnergyScale((1.f-rho) + rho*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
        }
      }
    } else if(name.BeginsWith("TauES")) { //Decay mode binned energy scale (genuine hadronic taus)
      if(fIsData || !leptonTwo.isTau()) continue;
      if(std::abs(leptonTwo.genFlavor) == 15) { //check that it's a genuine hadronic tau
        int dm_bin(0);
        if     (name == "TauES0") dm_bin = 0;
        else if(name == "TauES1") dm_bin = 1;
        else if(name == "TauES2") dm_bin = 2;
        else if(name == "TauES3") dm_bin = 3;
        if((tauDecayMode % 10 + 2*(tauDecayMode / 10)) == dm_bin) {
          if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                                  __func__, name.Data(), fSystematics.IsUp(sys));
          reeval = true;
          if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
            if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
              EnergyScale((1.f-rho) + rho*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
          } else {
            if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
              EnergyScale((1.f-rho) + rho*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
          }
        }
      }
    } else if(name == "EmbTauES") { //Decay mode inclusive energy scale
      if(!fIsEmbed || !leptonTwo.isTau()) continue;
      if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                              __func__, name.Data(), fSystematics.IsUp(sys));
      reeval = true;
      if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
        if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
          EnergyScale((1.f-rho_t) + rho_t*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
          EnergyScale((1.f-rho_t) + rho_t*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
      }
    } else if(name.BeginsWith("EmbTauES")) { //Decay mode binned energy scale
      if(!fIsEmbed || !leptonTwo.isTau()) continue;
      int dm_bin(0);
      if     (name == "EmbTauES0") dm_bin = 0;
      else if(name == "EmbTauES1") dm_bin = 1;
      else if(name == "EmbTauES2") dm_bin = 2;
      else if(name == "EmbTauES3") dm_bin = 3;
      if((tauDecayMode % 10 + 2*(tauDecayMode / 10)) == dm_bin) {
        if(fVerbose > 5) printf("CLFVHistMaker::%s: Applying %s energy scale (up = %i)\n",
                                __func__, name.Data(), fSystematics.IsUp(sys));
        reeval = true;
        if(fSystematics.IsUp(sys)) {  //FIXME: check if this should be propagated to the MET
          if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[1] > 0.)
            EnergyScale((1.f-rho_t) + rho_t*leptonTwo.ES[1] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
        } else {
          if(leptonTwo.isTau() && leptonTwo.ES[0] > 0. && leptonTwo.ES[2] > 0.)
            EnergyScale((1.f-rho_t) + rho_t*leptonTwo.ES[2] / leptonTwo.ES[0], leptonTwo, &met, &metPhi);
        }
      }
    } else if(name == "EmbEleRes") {
      if(!fIsEmbed || !isEData) continue; //only process for embedding with reco electrons
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isElectron() && leptonOne.genPt > 0.f)
          EnergyScale(leptonOne.Res[1] / leptonOne.Res[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isElectron() && leptonTwo.genPt > 0.f)
          EnergyScale(leptonTwo.Res[1] / leptonTwo.Res[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonOne.isElectron() && leptonOne.genPt > 0.f)
          EnergyScale(leptonOne.Res[2] / leptonOne.Res[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isElectron() && leptonTwo.genPt > 0.f)
          EnergyScale(leptonTwo.Res[2] / leptonTwo.Res[0], leptonTwo, &met, &metPhi);
      }
    } else if(name == "EmbMuonRes") {
      if(!fIsEmbed || !isMData) continue; //only process for embedding with reco muons
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        if(leptonOne.isMuon() && leptonOne.genPt > 0.f)
          EnergyScale(leptonOne.Res[1] / leptonOne.Res[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isMuon() && leptonTwo.genPt > 0.f)
          EnergyScale(leptonTwo.Res[1] / leptonTwo.Res[0], leptonTwo, &met, &metPhi);
      } else {
        if(leptonOne.isMuon() && leptonOne.genPt > 0.f)
          EnergyScale(leptonOne.Res[2] / leptonOne.Res[0], leptonOne, &met, &metPhi);
        if(leptonTwo.isMuon() && leptonTwo.genPt > 0.f)
          EnergyScale(leptonTwo.Res[2] / leptonTwo.Res[0], leptonTwo, &met, &metPhi);
      }
    } else if(name == "QCDStat") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdWeight > 0.) ? qcdWeightUp   / qcdWeight : 1.;
        else                       weight *= (qcdWeight > 0.) ? qcdWeightDown / qcdWeight : 1.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name.Contains("QCDStat")) { //jet binned QCD fit uncertainty
      if(!chargeTest) { //only shift for same-sign events
        int jet_bin(0);
        if     (name == "QCDStat0") jet_bin = 0;
        else if(name == "QCDStat1") jet_bin = 1;
        else if(name == "QCDStat2") jet_bin = 2;
        if(std::min(2, (int) nJets20) == jet_bin) {
          if(fSystematics.IsUp(sys)) weight *= (qcdWeight > 0.) ? qcdWeightUp   / qcdWeight : 1.;
          else                       weight *= (qcdWeight > 0.) ? qcdWeightDown / qcdWeight : 1.;
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
        else if(name.EndsWith("A2")) alt_bin = 2;
        else if(name.EndsWith("A3")) alt_bin = 3;
        else if(name.EndsWith("A4")) alt_bin = 4;
        else if(name.EndsWith("A5")) alt_bin = 5; //up to a 4th order polynomial is considered
        if(alt_bin < qcdWeightAltNum) { //check that there are at least this many parameters in the fit function
          if(std::min(2, (int) nJets20) == jet_bin) {
            if(fSystematics.IsUp(sys)) weight *= (qcdWeight > 0.) ? qcdWeightAltUp  [alt_bin] / qcdWeight : 1.f;
            else                       weight *= (qcdWeight > 0.) ? qcdWeightAltDown[alt_bin] / qcdWeight : 1.f;
          }
        }
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "QCDNC") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdClosure > 0.) ? 1. / qcdClosure : 1.; //remove / apply twice as uncertainty
        else                       weight *= (qcdClosure > 0.) ? 1. * qcdClosure : 1.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "QCDBias") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdIsoScale > 0.) ? 1. / qcdIsoScale : 1.; //remove / apply twice as uncertainty
        else                       weight *= (qcdIsoScale > 0.) ? 1. * qcdIsoScale : 1.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "QCDMassBDTBias") {
      if(!chargeTest) { //only shift for same-sign events
        if(fSystematics.IsUp(sys)) weight *= (qcdMassBDTScale > 0.) ? 1. / qcdMassBDTScale : 1.; //remove / apply twice as uncertainty
        else                       weight *= (qcdMassBDTScale > 0.) ? 1. * qcdMassBDTScale : 1.;
      } else continue; //no need to fill opposite signed histograms
    } else if(name == "JetToTauNC") {
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
      if(fSystematics.IsUp(sys)) weight *= jetToTauWeightCorrUp     / jetToTauWeightCorr   ;
      else                       weight *= jetToTauWeightCorrDown   / jetToTauWeightCorr   ;
    } else if(name.Contains("JetToTauNC")) { //process binned bias uncertainty
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
      //recreate the weight with shifted process bias
      int proc;
      if     (name == "JetToTauNC0") proc = JetToTauComposition::kWJets;
      else if(name == "JetToTauNC1") proc = JetToTauComposition::kTop;
      else if(name == "JetToTauNC2") proc = JetToTauComposition::kQCD;
      else {
        printf("CLFVHistMaker::%s: Unknown jet-->tau non-closure systematic bin %s\n", __func__, name.Data());
        continue;
      }
      weight = EvalJetToTauNCSys(proc, fSystematics.IsUp(sys));
    } else if(name == "JetToTauComp") {
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
      if(fSystematics.IsUp(sys)) weight *= jetToTauWeight_compUp    / jetToTauWeightBias   ;
      else                       weight *= jetToTauWeight_compDown  / jetToTauWeightBias   ;
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
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
      if(fSystematics.IsUp(sys)) weight *= jetToTauWeightBiasUp     / jetToTauWeightBias   ;
      else                       weight *= jetToTauWeightBiasDown   / jetToTauWeightBias   ;
    } else if(name.Contains("JetToTauBiasRate")) { //process binned bias uncertainty
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
      //recreate the weight with shifted process bias
      float wt_jtt = 0.f;
      int proc;
      if     (name == "JetToTauBiasRate0") proc = JetToTauComposition::kWJets; //only defined for W+jets
      else {
        printf("CLFVHistMaker::%s: Unknown jet-->tau bias systematic bin %s\n", __func__, name.Data());
        continue;
      }
      for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
        //Z+jets uses the W+jets scales
        bool test = iproc == proc || (proc == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets);
        bool apply_bias = fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets);
        const float base = fJetToTauComps[iproc] * fJetToTauWts[iproc] * fJetToTauCorrs[iproc] * ((apply_bias) ? fJetToTauBiases[iproc] : 1.f);
        float rate_unc = 1.f;
        if(test) {
          if(mutau) rate_unc = (fYear == 2016) ? 0.03f : (fYear == 2017) ? 0.029f : 0.059f;
          if(etau ) rate_unc = (fYear == 2016) ? 0.04f : (fYear == 2017) ? 0.081f : 0.150f;
        }
        //apply the rate uncertainty to the process of interest
        wt_jtt += base * ((test) ? ((fSystematics.IsUp(sys)) ? 1.f + rate_unc : 1.f - rate_unc) : 1.f);
      }
      weight *= wt_jtt / jetToTauWeightBias;
    } else if(name.Contains("JetToTauBias")) { //process binned bias uncertainty
      if(!isLooseTau) continue; //only shift the weight for loose tau ID region events
      //get the bias shift only for the relevant process
      int proc;
      if     (name == "JetToTauBias0") proc = JetToTauComposition::kWJets;
      else if(name == "JetToTauBias1") proc = JetToTauComposition::kTop;
      else if(name == "JetToTauBias2") proc = JetToTauComposition::kQCD;
      else {
        printf("CLFVHistMaker::%s: Unknown jet-->tau bias systematic bin %s\n", __func__, name.Data());
        continue;
      }
      weight = EvalJetToTauBiasSys(proc, fSystematics.IsUp(sys));
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
      if(fIsData || (fIsEmbed && fEmbedUseMETUnc != 1)) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        met    = puppMETJERUp;
        metPhi = puppMETphiJERUp;
        if(!fIsEmbed && jetOne.pt > 5.f && jetOne.jer_pt_up > 5.f) { //if there's a jet and defined uncertainties (ignore for Embedding)
          jetOne.setPtEtaPhiM(jetOne.jer_pt_up, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      } else {
        met    = puppMETJERDown;
        metPhi = puppMETphiJERDown;
        if(!fIsEmbed && jetOne.pt > 5.f && jetOne.jer_pt_down > 5.f) { //if there's a jet and defined uncertainties (ignore for Embedding)
          jetOne.setPtEtaPhiM(jetOne.jer_pt_down, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      }
    } else if(name == "JES") {
      if(fIsData || (fIsEmbed && fEmbedUseMETUnc != 1)) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        met    = puppMETJESUp;
        metPhi = puppMETphiJESUp;
        if(!fIsEmbed && jetOne.pt > 5.f && jetOne.jes_pt_up > 5.f) { //if there's a jet and defined uncertainties (ignore for Embedding)
          jetOne.setPtEtaPhiM(jetOne.jes_pt_up, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      } else {
        met    = puppMETJESDown;
        metPhi = puppMETphiJESDown;
        if(!fIsEmbed && jetOne.pt > 5.f && jetOne.jes_pt_down > 5.f) { //if there's a jet and defined uncertainties (ignore for Embedding)
          jetOne.setPtEtaPhiM(jetOne.jes_pt_down, jetOne.eta, jetOne.phi, jetOne.mass);
        }
      }
    } else if(name == "2018HEM") { //see https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/2000.html
      if(fIsData || fIsEmbed || fYear != 2018 || !fSystematics.IsUp(sys)) continue; //only relevant to 2018 MC, and only an up is defined
      //loop through the jet collection, evaluate the total shift to the MET
      float dx(0.f), dy(0.f);
      for(unsigned ijet = 0; ijet < nJets20; ++ijet) {
        if(jetsPhi[ijet] < -1.57f || jetsPhi[ijet] > -0.87f) continue; //relevant phi region
        if(jetsEta[ijet] < -3.0f || jetsEta[ijet] > -1.3f) continue; //relevant eta region
        const float factor = (jetsEta[ijet] < -2.5f) ? -0.35f : -0.20f; //factor the jet pT should be changed by
        dx += factor*jetsPt[ijet]*std::cos(jetsPhi[ijet]);
        dy += factor*jetsPt[ijet]*std::sin(jetsPhi[ijet]);
      }
      if(dx != 0.f && dy != 0.f) { //a jet is relevant
        reeval = true;
        //check if the identified jet should be updated, and if so what the pT scale factor is
        const bool hem_region = jetOne.eta >= -3.0f && jetOne.eta <= -1.3f && jetOne.phi >= -1.57f && jetOne.phi <= -0.87f;
        const float factor = (!hem_region) ? 1.f : (jetOne.eta < -2.5f) ? 0.65f : 0.80f;
        jetOne.setPtEtaPhiM(jetOne.pt*factor, jetOne.eta, jetOne.phi, jetOne.mass);
        //update the met
        const float met_x = std::cos(metPhi) * met - dx;
        const float met_y = std::sin(metPhi) * met - dy;
        met = std::sqrt(met_x*met_x + met_y*met_y);
        metPhi = Utilities::PhiFromXY(met_x, met_y);
      } else { //no jet was changed
        reeval = false;
      }
    } else if(name == "EmbMET") {
      if(!fIsEmbed || fEmbedUseMETUnc != 2) continue;
      reeval = true;
      if(fSystematics.IsUp(sys)) {
        met    = puppMETJESUp; //variable re-used for this purpose here
        metPhi = puppMETphiJESUp;
      } else { //no down variation, make one-sided
        continue; //skip since no variation is needed for down
        // reeval = false; //no need to re-evaluate
      }
    } else if(name == "EmbDetectorMET") { //effect on non-neutrino MET in embedding
      if(!fIsEmbed) continue;
      reeval = true;
      //remove nu pT from the MET
      float det_met_px = met*std::cos(metPhi) - eventNuPt*std::cos(eventNuPhi);
      float det_met_py = met*std::sin(metPhi) - eventNuPt*std::sin(eventNuPhi);
      //get the uncertainty from the reco leptons pT
      float unc_px = (leptonOne.isTau()) ? 0.05*leptonOne.p4->Px() : 0.025*leptonOne.p4->Px();
      float unc_py = (leptonOne.isTau()) ? 0.05*leptonOne.p4->Py() : 0.025*leptonOne.p4->Py();
      unc_px      += (leptonTwo.isTau()) ? 0.05*leptonTwo.p4->Px() : 0.025*leptonTwo.p4->Px();
      unc_py      += (leptonTwo.isTau()) ? 0.05*leptonTwo.p4->Py() : 0.025*leptonTwo.p4->Py();
      //add or substract the uncertainty based on up/down flag
      if(fSystematics.IsUp(sys)) {
        det_met_px += unc_px;
        det_met_py += unc_py;
      } else {
        det_met_px -= unc_px;
        det_met_py -= unc_py;
      }
      //Set the MET to be the new detector met + nu pT
      const float met_px = det_met_px + eventNuPt*std::cos(eventNuPhi);
      const float met_py = det_met_py + eventNuPt*std::sin(eventNuPhi);
      met    = std::sqrt(met_px*met_px+met_py*met_py);
      metPhi = Utilities::PhiFromXY(met_px,met_py);
    } else if(name.BeginsWith("EmbDetectorMET")) { //effect on non-neutrino MET in embedding
      if(!fIsEmbed) continue;
      const bool is_ele  = name.EndsWith("0");
      const bool is_muon = name.EndsWith("1");
      const bool is_tau  = name.EndsWith("2");
      if(is_ele && !isEData) continue;
      if(is_muon && !isMData) continue;
      if(!(isMuTau || isETau) && is_tau) continue;
      reeval = true;
      //remove nu pT from the MET
      float det_met_px = met*std::cos(metPhi) - eventNuPt*std::cos(eventNuPhi);
      float det_met_py = met*std::sin(metPhi) - eventNuPt*std::sin(eventNuPhi);
      //get the uncertainty from the reco leptons pT
      float unc_px(0.f), unc_py(0.f);
      if((leptonOne.isTau() && is_tau) || (leptonOne.isMuon() && is_muon) || (leptonOne.isElectron() && is_ele)) {
        unc_px += 0.025*leptonOne.p4->Px();
        unc_py += 0.025*leptonOne.p4->Py();
      }
      if((leptonTwo.isTau() && is_tau) || (leptonTwo.isMuon() && is_muon) || (leptonTwo.isElectron() && is_ele)) {
        unc_px += 0.025*leptonTwo.p4->Px();
        unc_py += 0.025*leptonTwo.p4->Py();
      }
      //add or substract the uncertainty based on up/down flag
      if(fSystematics.IsUp(sys)) {
        det_met_px += unc_px;
        det_met_py += unc_py;
      } else {
        det_met_px -= unc_px;
        det_met_py -= unc_py;
      }
      //Set the MET to be the new detector met + nu pT
      const float met_px = det_met_px + eventNuPt*std::cos(eventNuPhi);
      const float met_py = det_met_py + eventNuPt*std::sin(eventNuPhi);
      met    = std::sqrt(met_px*met_px+met_py*met_py);
      metPhi = Utilities::PhiFromXY(met_px,met_py);
    } else if(name == "EmbTauTau") { //tau_mu tau_mu contamination in embedding
      if(!fIsEmbed) continue;
      float scale = 0.f;
      if     (zMass < 60.f) scale = 0.10f;
      else if(zMass < 80.f) scale = 0.10f*(1.f - (zMass-60.f)/20.f); //linearly drop to 0 at 80 GeV
      if(fSystematics.IsUp(sys)) {
        weight *= 1.f + scale;
      } else { //no down variation, make one-sided
        weight *= 1.f - scale;
      }
    } else if(name == "EmbBDT") { //BDT uncertainty from di-muon selection in embedding
      if(!fIsEmbed) continue;
      if(!fUseEmbBDTUncertainty) continue;
      if(embBDTWeight <= 0.f) weight = 0.f;
      else {
        if(fSystematics.IsUp(sys)) {
          weight *= embBDTWeight;
        } else { //no down variation, make one-sided
          weight /= embBDTWeight;
        }
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
      if(fVerbose > 0) {
        printf("CLFVHistMaker::%s: Systematic %s (%i): Re-evaluating kinematics\n", __func__, name.Data(), sys);
      }
      fTimes[GetTimerNumber("SystKin")] = std::chrono::steady_clock::now(); //timer for evaluating the MVAs
      SetKinematics();
      IncrementTimer("SystKin", true);
      EvalMVAs("SystMVAs");
    } //re-evaluate variables with shifted values
    const float lepm  = fTreeVars.lepm;
    const float onept = leptonOne.pt;
    const float twopt = leptonTwo.pt;

    //apply event window cuts with (possibly) shifted kinematics
    const bool pass = (fAllowMigration && reeval) ? PassesCuts() : o_pass;
    if(!reeval && !pass && o_pass) {
      printf("CLFVHistMaker::%s: Entry %lld: Event no longer passes cuts but no kinematic updates (systematic %s/%i)\n", __func__, fentry, name.Data(), sys);
    }
    if(pass) {
      ++nfilled;
      if(!fSparseHists || (isEMu || isSameFlavor)) {
        Hist->hLepM  [sys]->Fill(lepm  , weight);
      }
      //skip histograms that aren't relevant unless requested
      if(!fSparseHists) {
        Hist->hOnePt [sys]->Fill(onept  , weight);
        Hist->hTwoPt [sys]->Fill(twopt  , weight);
        if(!fIsData) {
          Hist->hWeightChange[sys]->Fill((eventWeight*genWeight != 0.) ? (eventWeight*genWeight - weight) / (eventWeight*genWeight) : 0.);
        }
      }
      //skip MVA histograms if not relevant
      if(!isSameFlavor && !(fSparseHists && isEMu)) {
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
          // float mvascore = (fUseCDFBDTs == 1) ? fMvaCDFs[i] : (fUseCDFBDTs == 2) ? fMvaFofP[i] : fMvaOutputs[i];
          float mvascore = fMvaUse[i];
          if(!std::isfinite(mvascore) && fVerbose > 0) {
            std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << ", sys " << sys <<", MVA " << i << ": score is not finite = " << mvascore << "! Setting to -2...\n";
            mvascore = -2.;
          }
          if(mvascore < -1.) {
            std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << ", sys " << sys <<", MVA " << i << ": score is not defined = " << mvascore << "!\n";
          }
          Hist->hMVA[i][sys]->Fill(mvascore, mvaweight);
          if(reeval) Hist->hMVADiff[i][sys]->Fill(mvascore-o_use[i], mvaweight);
        }
      } //end !same flavor and !(sparse+emu)
    } //end kinematic event selection check

    //restore the nominal kinematics/scores if shifted
    if(reeval) {
      leptonOne.setP(o_lv1);
      leptonTwo.setP(o_lv2);
      jetOne   .setP(o_jet);
      met    = o_met;
      metPhi = o_metPhi;
      SetKinematics();
      for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
        fMvaOutputs[i] = o_mvas[i];
        fMvaCDFs[i]    = o_cdfs[i];
        fMvaFofP[i]    = o_fofp[i];
        fMvaUse [i]    = o_use [i];
      }
    }
  }
  //Per event histograms
  Hist->hNFills->Fill(nfilled, eventWeight*genWeight);

  if(std::fabs(nominal_weight - eventWeight*genWeight) > 1.e-7) {
    printf("CLFVHistMaker::%s: Event weight changed! Previous = %f, Current = %f\n",
           __func__, nominal_weight, eventWeight*genWeight);
  }
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t CLFVHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  if(!fUpdateMET && metCorr != 0.f) {
    throw std::runtime_error(Form("CLFVHistMaker::%s: Entry %lld: No MET updating but a MET update found, metCorr = %f, metCorrPhi = %f\n",
                                  __func__, fentry, metCorr, metCorrPhi));
  }

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


  //If cut-flow testing, remove fake leptons early to allow data-driven background estimates
  if(fCutFlowTesting) {
    if(!fUseMCEstimatedFakeLep && !fIsData && !fIsSignal) { //keep signal as even events with fakes are signal from the MC
      emu   &= !isFakeMuon;
      emu   &= !isFakeElectron;
      mumu  &= !isFakeMuon;
      ee    &= !isFakeElectron;
      mutau &= !isFakeMuon;
      etau  &= !isFakeElectron;
    }
    if(!fUseMCFakeTau) {
      mutau &= fIsData > 0 || std::abs(tauGenFlavor) != 26 || fIsSignal; //keep signal as even events with fakes are signal from the MC
      etau  &= fIsData > 0 || std::abs(tauGenFlavor) != 26 || fIsSignal;
    }
  }

  ///////////////////////////////////////////
  // Re-define N(b-jets) if needed

  // //FIXME: decide on b-jet ID to use in etau_mu/mutau_e selections
  // if(emu && lep_tau) { //use medium ID b-jets in etau_mu/mutau_e since DY is the dominant background
  //   fBJetTightness = 1;
  //   nBJetsUse = (fBJetCounting == 0) ? nBJetsL : nBJets20L;
  // }

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

  //check the pT cuts
  bool fail_pt_cuts = false;
  fail_pt_cuts |= leptonOne.isElectron() && leptonOne.pt <= electron_pt - sys_buffer;
  fail_pt_cuts |= leptonTwo.isElectron() && leptonTwo.pt <= electron_pt - sys_buffer;
  fail_pt_cuts |= leptonOne.isMuon    () && leptonOne.pt <= muon_pt     - sys_buffer;
  fail_pt_cuts |= leptonTwo.isMuon    () && leptonTwo.pt <= muon_pt     - sys_buffer;
  fail_pt_cuts |= leptonOne.isTau     () && leptonOne.pt <= tau_pt      - sys_buffer;
  fail_pt_cuts |= leptonTwo.isTau     () && leptonTwo.pt <= tau_pt      - sys_buffer;
  if(fail_pt_cuts) return kTRUE;

  //check against the triggers
  const bool triggered = PassesTrigger(sys_buffer);

  if(!triggered) return kTRUE;
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
  // const double electron_eta_max = (fUseEmbedCuts) ? (emu || ee || mumu) ? 2.4 : 2.2 : 2.5;
  // const double muon_eta_max     = (fUseEmbedCuts) ? (emu || ee || mumu) ? 2.4 : 2.2 : 2.4;
  //Use the restricted Embedding |eta| cuts by default, to facilitate MC vs. Embedding comparisons
  const double electron_eta_max = (emu || ee || mumu) ? 2.4 : 2.2;
  const double muon_eta_max     = (emu || ee || mumu) ? 2.4 : 2.2;
  const double tau_eta_max      = 2.3; //tau eta doesn't change for selections or simulation datasets
  const double min_delta_r      = 0.3; //separation between leptons

  mutau &= std::fabs(leptonOne.eta) < muon_eta_max;
  mutau &= std::fabs(leptonTwo.eta) < tau_eta_max ;
  mutau &= std::fabs(fTreeVars.lepdeltar) > min_delta_r;

  etau  &= std::fabs(leptonOne.eta) < electron_eta_max;
  etau  &= std::fabs(leptonTwo.eta) < tau_eta_max      ;
  etau  &= std::fabs(fTreeVars.lepdeltar) > min_delta_r;

  emu   &= std::fabs(leptonOne.eta) < electron_eta_max;
  emu   &= std::fabs(leptonTwo.eta) < muon_eta_max    ;
  emu   &= std::fabs(fTreeVars.lepdeltar) > min_delta_r;

  mumu  &= std::fabs(leptonOne.eta) < muon_eta_max;
  mumu  &= std::fabs(leptonTwo.eta) < muon_eta_max;
  mumu  &= std::fabs(fTreeVars.lepdeltar) > min_delta_r;

  ee    &= std::fabs(leptonOne.eta) < electron_eta_max;
  ee    &= std::fabs(leptonTwo.eta) < electron_eta_max;
  ee    &= std::fabs(fTreeVars.lepdeltar) > min_delta_r;

  //apply reasonable lepton isolation cuts
  const float max_rel_iso = 0.5;
  if(leptonOne.isElectron() && leptonOne.relIso >= max_rel_iso) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.relIso >= max_rel_iso) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.relIso >= max_rel_iso) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.relIso >= max_rel_iso) return kTRUE;

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
  const float tau_dz(0.2);
  mutau &= std::fabs(leptonOne.dxy) < max_dxy;
  mutau &= std::fabs(leptonOne.dz ) < max_dz ;
  mutau &= std::fabs(leptonTwo.dz ) < tau_dz ;
  etau  &= std::fabs(leptonOne.dxy) < max_dxy;
  etau  &= std::fabs(leptonOne.dz ) < max_dz ;
  etau  &= std::fabs(leptonTwo.dz ) < tau_dz ;
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

  ///////////////////////////////////////////////////////////////////
  //additional lepton IDs

  // if(lep_tau == 0) {
  //   emu &= Electron_convVeto[0]; //veto gamma --> e to reduce Z->mu mu* --> mu mu gamma
  // }

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
  // mutau &= leptonTwo.id2  >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  etau  &= isLooseTau || tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //15 = tight
  etau  &= tauDeepAntiEle >= fETauAntiEleCut; //tauDeepAntiEle >= 50; //31 = Medium, 63 = tight, 127 = VTight, 255 = VVTight
  // etau  &= leptonTwo.id2  >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  //Add a buffer between loose ID and tight ID taus
  if(fUseLooseTauBuffer && isLooseTau) {
    mutau &= tauDeepAntiJet < 30; //31 = medium
    etau  &= tauDeepAntiJet < 30;
  }

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

  const bool no_met_cut_test   = false; //FIXME: Remove this

  const float met_cut          = -1.f; //60.f;
  const float mtlep_cut        = (lep_tau || emu || ee || mumu) ? -1.f : 70.f;
  const float mtone_cut        = -1.f;
  const float mttwo_cut        = -1.f;
  const float mtlep_over_m_cut = -1.f;
  const float mtone_over_m_cut = -1.f; //(mutau_e)                  ? 0.8f : -1.f;
  const float mttwo_over_m_cut = (mutau || etau /* || etau_mu*/) ? -1.f : -1.f; // FIXME: Decide on this cut
  const bool nominalSelection  = (nBJetsUse == 0 &&
                                  (met_cut < 0.f || met < met_cut + sys_buffer) &&
                                  (mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer) &&
                                  (mtone_cut < 0.f || fTreeVars.mtone < mtone_cut + sys_buffer) &&
                                  (mttwo_cut < 0.f || fTreeVars.mttwo < mttwo_cut + sys_buffer) &&
                                  (mtlep_over_m_cut < 0.f || fTreeVars.mtlepoverm < mtlep_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm) &&
                                  (mtone_over_m_cut < 0.f || fTreeVars.mtoneoverm < mtone_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm) &&
                                  (mttwo_over_m_cut < 0.f || fTreeVars.mttwooverm < mttwo_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm) &&
                                  !(isLooseMuon || isLooseElectron));
  met_max_          = (no_met_cut_test) ? -1.f : met_cut;
  mtlep_max_        = (no_met_cut_test) ? -1.f : mtlep_cut;
  mtone_max_        = (no_met_cut_test) ? -1.f : mtone_cut;
  mttwo_max_        = (no_met_cut_test) ? -1.f : mttwo_cut;
  mtlep_over_m_max_ = (no_met_cut_test) ? -1.f : mtlep_over_m_cut;
  mtone_over_m_max_ = (no_met_cut_test) ? -1.f : mtone_over_m_cut;
  mttwo_over_m_max_ = (no_met_cut_test) ? -1.f : mttwo_over_m_cut;

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

  ///////////////////////////////////////////////////////////////////
  //remove MC jet -> light lepton contribution

  if(!fUseMCEstimatedFakeLep && !fIsData && !fIsSignal) { //keep signal as even events with fakes are signal from the MC
    emu   &= !isFakeElectron;
    emu   &= !isFakeMuon;
    mumu  &= !isFakeMuon;
    ee    &= !isFakeElectron;
    mutau &= !isFakeMuon;
    etau  &= !isFakeElectron;

    //remove pileup/deeper jet origin electron/muon events if requested
    const bool remove_pileup     = false && !fIsEmbed; //keep pileup events since taken from data --> fairly accurate
    const bool remove_jetorigin  = false && !fIsEmbed;
    const bool remove_photon_ele = false && !fIsEmbed; //keep photons faking electrons FIXME: Should these be from data?
    const bool remove_lep_one = !leptonOne.isTau() && ((leptonOne.isPileup && remove_pileup) || (leptonOne.jetOrigin && remove_jetorigin));
    const bool remove_lep_two = !leptonTwo.isTau() && ((leptonTwo.isPileup && remove_pileup) || (leptonTwo.jetOrigin && remove_jetorigin));


    mutau &= !remove_lep_one && !remove_lep_two;
    etau  &= !remove_lep_one && !remove_lep_two && (!remove_photon_ele || leptonOne.genFlavor != 22);
    emu   &= !remove_lep_one && !remove_lep_two && (!remove_photon_ele || leptonOne.genFlavor != 22);
    mumu  &= !remove_lep_one && !remove_lep_two;
    ee    &= !remove_lep_one && !remove_lep_two && (!remove_photon_ele || leptonOne.genFlavor != 22) && (!remove_photon_ele || leptonTwo.genFlavor != 22);
   }

  ///////////////////////////////////////////////////////////////////
  //remove MC estimated jet --> tau component

  if(!fUseMCFakeTau) {
    mutau &= fIsData > 0 || std::abs(tauGenFlavor) != 26 || fIsSignal; //keep signal as even events with fakes are signal from the MC
    etau  &= fIsData > 0 || std::abs(tauGenFlavor) != 26 || fIsSignal;
  }

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //15
  ++icutflow;

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  mutau &= no_met_cut_test || met_cut < 0.f || met < met_cut + sys_buffer;
  etau  &= no_met_cut_test || met_cut < 0.f || met < met_cut + sys_buffer;
  emu   &= no_met_cut_test || met_cut < 0.f || met < met_cut + sys_buffer;
  mumu  &= no_met_cut_test || met_cut < 0.f || met < met_cut + sys_buffer;
  ee    &= no_met_cut_test || met_cut < 0.f || met < met_cut + sys_buffer;

  mutau &= no_met_cut_test || mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;
  etau  &= no_met_cut_test || mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;
  emu   &= no_met_cut_test || mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;
  mumu  &= no_met_cut_test || mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;
  ee    &= no_met_cut_test || mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut + sys_buffer;

  mutau &= no_met_cut_test || mtone_cut < 0.f || fTreeVars.mtone < mtone_cut + sys_buffer;
  etau  &= no_met_cut_test || mtone_cut < 0.f || fTreeVars.mtone < mtone_cut + sys_buffer;
  emu   &= no_met_cut_test || mtone_cut < 0.f || fTreeVars.mtone < mtone_cut + sys_buffer;
  mumu  &= no_met_cut_test || mtone_cut < 0.f || fTreeVars.mtone < mtone_cut + sys_buffer;
  ee    &= no_met_cut_test || mtone_cut < 0.f || fTreeVars.mtone < mtone_cut + sys_buffer;

  mutau &= no_met_cut_test || mttwo_cut < 0.f || fTreeVars.mttwo < mttwo_cut + sys_buffer;
  etau  &= no_met_cut_test || mttwo_cut < 0.f || fTreeVars.mttwo < mttwo_cut + sys_buffer;
  emu   &= no_met_cut_test || mttwo_cut < 0.f || fTreeVars.mttwo < mttwo_cut + sys_buffer;
  mumu  &= no_met_cut_test || mttwo_cut < 0.f || fTreeVars.mttwo < mttwo_cut + sys_buffer;
  ee    &= no_met_cut_test || mttwo_cut < 0.f || fTreeVars.mttwo < mttwo_cut + sys_buffer;

  mutau &= no_met_cut_test || mtlep_over_m_cut < 0.f || fTreeVars.mtlepoverm < mtlep_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  etau  &= no_met_cut_test || mtlep_over_m_cut < 0.f || fTreeVars.mtlepoverm < mtlep_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  emu   &= no_met_cut_test || mtlep_over_m_cut < 0.f || fTreeVars.mtlepoverm < mtlep_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  mumu  &= no_met_cut_test || mtlep_over_m_cut < 0.f || fTreeVars.mtlepoverm < mtlep_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  ee    &= no_met_cut_test || mtlep_over_m_cut < 0.f || fTreeVars.mtlepoverm < mtlep_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;

  mutau &= no_met_cut_test || mtone_over_m_cut < 0.f || fTreeVars.mtoneoverm < mtone_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  etau  &= no_met_cut_test || mtone_over_m_cut < 0.f || fTreeVars.mtoneoverm < mtone_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  emu   &= no_met_cut_test || mtone_over_m_cut < 0.f || fTreeVars.mtoneoverm < mtone_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  mumu  &= no_met_cut_test || mtone_over_m_cut < 0.f || fTreeVars.mtoneoverm < mtone_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  ee    &= no_met_cut_test || mtone_over_m_cut < 0.f || fTreeVars.mtoneoverm < mtone_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;

  mutau &= no_met_cut_test || mttwo_over_m_cut < 0.f || fTreeVars.mttwooverm < mttwo_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  etau  &= no_met_cut_test || mttwo_over_m_cut < 0.f || fTreeVars.mttwooverm < mttwo_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  emu   &= no_met_cut_test || mttwo_over_m_cut < 0.f || fTreeVars.mttwooverm < mttwo_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  mumu  &= no_met_cut_test || mttwo_over_m_cut < 0.f || fTreeVars.mttwooverm < mttwo_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;
  ee    &= no_met_cut_test || mttwo_over_m_cut < 0.f || fTreeVars.mttwooverm < mttwo_over_m_cut + 2.f*sys_buffer/fTreeVars.lepm;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(isLooseElectron)               {fCutFlow->Fill(icutflow);} //16
  ++icutflow;
  if(isLooseElectron && chargeTest) {fCutFlow->Fill(icutflow);} //17
  ++icutflow;

  //Enforce QCD selection only using loose muon ID
  emu &= !isLooseElectron;
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  if(!looseQCDSelection && chargeTest)                                          {fCutFlow->Fill(icutflow);} //18
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight) > 0.)             {fCutFlow->Fill(icutflow);} //19
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight*eventWeight) > 0.) {fCutFlow->Fill(icutflow);} //20
  ++icutflow;

  ///////////////////////////////////////////////////////////////////////////
  // Add additional selection for leptonic tau channels

  mutau_e = lep_tau == 1 && emu; //must be doing a leptonic tau selection
  etau_mu = lep_tau == 2 && emu;
  emu    &= lep_tau == 0;

  //Stricter Drell-Yan veto set FIXME: Formalize this or remove it
  const bool do_dy_cuts = true;
  if(do_dy_cuts && (mutau || etau || lep_tau)) {
    if(mutau || etau) one_pt_min_ = std::max(one_pt_min_, 28.f);
    if(lep_tau == 1 && mutau_e) {mtone_over_m_max_ = 0.8f; mttwo_over_m_max_ = -1.f;}
    else                        {mttwo_over_m_max_ = 0.8f; mtone_over_m_max_ = -1.f;}
    etau  &= no_met_cut_test || mttwo_over_m_max_ < 0.f || fTreeVars.mttwooverm < mttwo_over_m_max_ + 2.f*sys_buffer/fTreeVars.lepm;
    mutau &= no_met_cut_test || mttwo_over_m_max_ < 0.f || fTreeVars.mttwooverm < mttwo_over_m_max_ + 2.f*sys_buffer/fTreeVars.lepm;
    etau  &= leptonOne.pt > one_pt_min_ - sys_buffer;
    mutau &= leptonOne.pt > one_pt_min_ - sys_buffer;
  }

  //define leptonic tau kinematic cuts
  if(mutau_e) { //lep 2 = prompt, lep 1 = tau_l
    two_pt_min_ = 20.f;
    ptdiff_min_ = -1.e10; ptdiff_max_ = +1.e10;
    mtlep_max_ =  90.f; mtone_max_ = (do_dy_cuts) ? -1.f : 60.f;
    if(!do_dy_cuts) {mtone_over_m_max_ = -1.f; mttwo_over_m_max_ = -1.f;}
  }
  if(etau_mu) { //lep 1 = prompt, lep 2 = tau_l
    one_pt_min_ = 20.f;
    ptdiff_min_ =    0.f; ptdiff_max_ = +1.e10;
    mtlep_max_ =  90.f; mttwo_max_ = (do_dy_cuts) ? -1.f : 60.f;
    if(!do_dy_cuts) {mtone_over_m_max_ = -1.f; mttwo_over_m_max_ = -1.f;}
  }

  if(no_met_cut_test) {
    met_max_          = -1.f;
    mtlep_max_        = -1.f;
    mtone_max_        = -1.f;
    mttwo_max_        = -1.f;
    mtlep_over_m_max_ = -1.f;
    mtone_over_m_max_ = -1.f;
    mttwo_over_m_max_ = -1.f;
  }

  mutau_e &= leptonTwo.pt > two_pt_min_ - sys_buffer; //lepton pT
  etau_mu &= leptonOne.pt > one_pt_min_ - sys_buffer;
  mutau_e &= fTreeVars.ptdiff > ptdiff_min_ - sys_buffer && fTreeVars.ptdiff < ptdiff_max_ + sys_buffer; //lepton pT - leptonic tau pT
  etau_mu &= fTreeVars.ptdiff > ptdiff_min_ - sys_buffer && fTreeVars.ptdiff < ptdiff_max_ + sys_buffer;
  mutau_e &= mtlep_max_ < 0.f || fTreeVars.mtlep < mtlep_max_ + sys_buffer; //di-lepton MT
  etau_mu &= mtlep_max_ < 0.f || fTreeVars.mtlep < mtlep_max_ + sys_buffer;
  mutau_e &= mtone_max_ < 0.f || fTreeVars.mtone < mtone_max_ + sys_buffer; //leptonic tau MT
  etau_mu &= mttwo_max_ < 0.f || fTreeVars.mttwo < mttwo_max_ + sys_buffer;
  mutau_e &= mtone_over_m_max_ < 0.f || fTreeVars.mtoneoverm < mtone_over_m_max_ + 2.f*sys_buffer/fTreeVars.lepm; //leptonic tau MT/M
  etau_mu &= mttwo_over_m_max_ < 0.f || fTreeVars.mttwooverm < mttwo_over_m_max_ + 2.f*sys_buffer/fTreeVars.lepm;

  //Omly apply significance cuts in the Z->emu search, since it's not well described in the Embedding
  const float emu_dxy_sig = 3.0;
  const float emu_dz_sig  = 4.7;
  emu     &= std::fabs(leptonTwo.dxySig) < emu_dxy_sig;
  emu     &= std::fabs(leptonOne.dxySig) < emu_dxy_sig;
  emu     &= std::fabs(leptonTwo.dzSig ) < emu_dz_sig ;
  emu     &= std::fabs(leptonOne.dzSig ) < emu_dz_sig ;

  //Z/H->e+mu only
  if(emu || (fSameFlavorEMuSelec && (ee || mumu))) {one_pt_min_ = 20.f; two_pt_min_ = 20.f; min_mass_ = 70.f; max_mass_ = 110.f;}
  emu     &= leptonOne.pt > one_pt_min_ - sys_buffer;
  emu     &= leptonTwo.pt > two_pt_min_ - sys_buffer;
  emu     &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;

  if(fSameFlavorEMuSelec && (ee || mumu)) {
    mumu    &= std::fabs(leptonTwo.dxySig) < emu_dxy_sig;
    mumu    &= std::fabs(leptonOne.dxySig) < emu_dxy_sig;
    mumu    &= std::fabs(leptonTwo.dzSig ) < emu_dz_sig ;
    mumu    &= std::fabs(leptonOne.dzSig ) < emu_dz_sig ;
    ee      &= std::fabs(leptonTwo.dxySig) < emu_dxy_sig;
    ee      &= std::fabs(leptonOne.dxySig) < emu_dxy_sig;
    ee      &= std::fabs(leptonTwo.dzSig ) < emu_dz_sig ;
    ee      &= std::fabs(leptonOne.dzSig ) < emu_dz_sig ;

    ee     &= leptonOne.pt > one_pt_min_ - sys_buffer;
    ee     &= leptonTwo.pt > two_pt_min_ - sys_buffer;
    ee     &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;
    mumu   &= leptonOne.pt > one_pt_min_ - sys_buffer;
    mumu   &= leptonTwo.pt > two_pt_min_ - sys_buffer;
    mumu   &= mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer;
  }

  if(!(mutau || etau || emu || mumu || ee || mutau_e || etau_mu)) return kTRUE;

  fCutFlow->Fill(icutflow); //21
  ++icutflow;
  if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //22
  ++icutflow;

  //Remove loose ID + same-sign events
  if(fRemoveLooseSS) {
    mutau   &= !looseQCDSelection || chargeTest;
    etau    &= !looseQCDSelection || chargeTest;
    emu     &= !looseQCDSelection || chargeTest;
    mutau_e &= !looseQCDSelection || chargeTest;
    etau_mu &= !looseQCDSelection || chargeTest;
    mumu    &= !looseQCDSelection || chargeTest;
    ee      &= !looseQCDSelection || chargeTest;
  }

  if(!(mutau || etau || emu || mumu || ee || mutau_e || etau_mu)) return kTRUE;

  fCutFlow->Fill(icutflow); //23
  ++icutflow;
  if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //24
  ++icutflow;

  if(PassesCuts()) { //apply the cuts without the systematic buffer
    if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //25
    ++icutflow;
    if(looseQCDSelection || !chargeTest) fCutFlow->Fill(icutflow); //26
    ++icutflow;
  }

  ///////////////////////////
  // Debugging selections

  //emu electron debugging, barrel vs. endcap
  const int do_barrel = 0; //0: igore; 1: only barrel electrons; -1: only endcap electrons
  const bool barrel = std::fabs(leptonOne.eta) < 1.5; //barrel electron, fewer fakes
  const bool apply_ele_id_cuts = false;
  //additional electron IDs to reduce Z->mumu backgrounds
  if(fDoEleIDStudy && apply_ele_id_cuts) {
    emu &= Electron_convVeto[0]; //veto gamma --> e ID'd events
    //PF candidate flag not defined in the embedding simulation
    emu &= fIsEmbed || Electron_isPFcand[0]; //gamma->e less likely to be a PF candidate
    emu &= Electron_lostHits[0] == 0; //gamma --> e leaves more missing hits
    emu &= leptonOne.dxy  < 0.01;
    emu &= leptonOne.dz   < 0.03;
    emu &= Electron_r9[0] > 0.45;
    emu &= Electron_mvaFall17V2noIso_WP80[0];
  }

  // //FIXME: Keep or remove these electron ID cuts
  // mutau_e &= Electron_convVeto[0]; //reject gamma -> e (e.g. mu* -> mu+gamma) events
  // etau_mu &= Electron_convVeto[0];
  // mutau_e &= Electron_lostHits[0] < 2;
  // etau_mu &= Electron_lostHits[0] < 2;

  ////////////////////////////////////////////////////////////////////////////
  // Set 7 + selection offset: No MC estimated fake taus, inverted b-jet cut
  ////////////////////////////////////////////////////////////////////////////
  if(nBJetsUse > 0 && (ee || mumu || fTopTesting) && fDoSystematics >= 0)
    FillAllHistograms(set_offset + 7);

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //27
  ++icutflow;

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  const bool invert_bjet_veto = fTopTesting > 1; //perform studies (e.g. Z->e+mu fits) in inverted b-tag regions

  if(invert_bjet_veto) {
    mutau   &= nBJetsUse >  0;
    etau    &= nBJetsUse >  0;
    emu     &= nBJetsUse >  0;
    mumu    &= nBJetsUse >  0;
    ee      &= nBJetsUse >  0;
    mutau_e &= nBJetsUse >  0;
    etau_mu &= nBJetsUse >  0;
  } else {
    mutau   &= nBJetsUse == 0;
    etau    &= nBJetsUse == 0;
    emu     &= nBJetsUse == 0;
    mumu    &= nBJetsUse == 0;
    ee      &= nBJetsUse == 0;
    mutau_e &= nBJetsUse == 0;
    etau_mu &= nBJetsUse == 0;
  }

  //Add BDT score cut FIXME: Formalize this
  if(!ee && !mumu && (!emu || lep_tau != 0)) { //only for tau channel regions
    // min_bdt_ = 0.2f;
    min_bdt_ = 0.25f;
    // min_bdt_ = 0.30f;
    if(sys_buffer == 0.f) { //if not systematic buffers, apply the cut, otherwise allow for complete migration
      const int imva = (mutau) ? 1 : (etau) ? 3 : (emu && lep_tau == 0) ? 5 : (lep_tau == 1) ? 7 : 9;
      if(fMvaUse[imva] <= min_bdt_) return kTRUE;
    }
  }

  if(!(mutau || etau || emu || mumu || ee || etau_mu || mutau_e)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //28
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////

  // if(!fIsData && !fIsSignal && !fUseMCEstimatedFakeLep && (leptonOne.isPileup || leptonTwo.isPileup)) {
  //   printf("CLFVHistMaker::%s: Entry %lld has pileup leptons\n", __func__, fentry);
  // }


  if(!lep_tau) {
    if(mll > min_mass_ - sys_buffer) {
      if(!emu || do_barrel == 0 || (barrel && do_barrel == 1) || (!barrel && do_barrel == -1)) {
        fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
        FillAllHistograms(set_offset + 8);
        IncrementTimer("SingleFill", true);
      }
    }
  } else if(mutau_e || etau_mu) { //single mass category search for leptonic taus
    if(lep_tau == 1 && mutau_e && mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer ) {
      fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
      FillAllHistograms(set_offset + (kMuTauE - kEMu) + 8);
      IncrementTimer("SingleFill", true);
      // if(fWNJets == 0) printf("CLFVHistMaker::%s: Entry %lld has 0 LHE_Njets\n", __func__, fentry);
    }
    if(lep_tau == 2 && etau_mu && mll > min_mass_ - sys_buffer && mll < max_mass_ + sys_buffer) {
      fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
      FillAllHistograms(set_offset + (kETauMu - kEMu) + 8);
      IncrementTimer("SingleFill", true);
    }
  }

  //Testing Embedding vs. Drell-Yan
  if(mumu) {
    //tight Z->ll pT cuts
    if(std::fabs(leptonOne.pt - 45.f) < 5.f && leptonTwo.pt > 25.f) FillAllHistograms(set_offset + 30);
    //barrel events
    if(std::fabs(leptonOne.eta) < 0.9f && std::fabs(leptonTwo.eta) < 0.9f) FillAllHistograms(set_offset + 31);
    //endcap events
    if(std::fabs(leptonOne.eta) > 1.5f && std::fabs(leptonTwo.eta) > 1.5f) FillAllHistograms(set_offset + 32);
    //low |phi(met)| events
    if(std::fabs(metPhi) < M_PI/2.) FillAllHistograms(set_offset + 33);
    //high |phi(met)| events
    if(std::fabs(metPhi) > M_PI/2.) FillAllHistograms(set_offset + 34);
    //lower MET events
    if(met < 40.f) FillAllHistograms(set_offset + 35);
    //high MET events
    if(met > 40.f) FillAllHistograms(set_offset + 36);
    //no gen-level FSR events, re-weighted to account for event loss
    {
      bool test = true;
      const float o_ewt = eventWeight;
      if(fIsDY && fDYType == 2) {
        float eloss_1, eloss_2;
        //associate the leptons by charge
        if(zLepOneID*leptonOne.flavor > 0) {
          eloss_1 = (zLepOnePt - leptonOne.genPt)/zLepOnePt;
          eloss_2 = (zLepTwoPt - leptonTwo.genPt)/zLepTwoPt;
        } else {
          eloss_1 = (zLepTwoPt - leptonOne.genPt)/zLepTwoPt;
          eloss_2 = (zLepOnePt - leptonTwo.genPt)/zLepOnePt;
        }
        test = eloss_1 < 0.005 && eloss_2 < 0.005;
        eventWeight *= 1.308;
        fTreeVars.eventweightMVA *= 1.308;
      }
      if(test) FillAllHistograms(set_offset + 37);
      eventWeight = o_ewt;
    }
    // //Test adjusting the Embedding lepton scale a bit
    // {
    //   const float scale = 0.99;
    //   if(fIsEmbed) {
    //     EnergyScale(scale, leptonOne, &met, &metPhi); //propagate the shift to the MET
    //     EnergyScale(scale, leptonTwo, &met, &metPhi);
    //     SetKinematics();
    //     EvalMVAs();
    //   }
    //   if(PassesCuts()) FillAllHistograms(set_offset + 38);
    //   if(fIsEmbed) { //restore the event variables
    //     EnergyScale(1.f/scale, leptonOne, &met, &metPhi); //propagate the shift to the MET
    //     EnergyScale(1.f/scale, leptonTwo, &met, &metPhi);
    //     SetKinematics();
    //     EvalMVAs();
    //   }
    // }
    //Lower vs. high di-lepton delta phi
    if(std::fabs(fTreeVars.lepdeltaphi - M_PI/2.) < 0.5) FillAllHistograms(set_offset + 39);
    else if(fTreeVars.lepdeltaphi > M_PI*0.95)           FillAllHistograms(set_offset + 40);
    //Lower vs. higher di-lepton pT
    if(fTreeVars.leppt < 10.f) FillAllHistograms(set_offset + 41);
    else                       FillAllHistograms(set_offset + 42);
    //remove odd eta region
    if((fabs(leptonOne.eta) < 0.55f || fabs(leptonOne.eta) > 0.8f) &&
       (fabs(leptonTwo.eta) < 0.55f || fabs(leptonTwo.eta) > 0.8f) &&
       (fabs(leptonOne.eta) < 1.75f || fabs(leptonOne.eta) > 2.0f) &&
       (fabs(leptonTwo.eta) < 1.75f || fabs(leptonTwo.eta) > 2.0f)) {
      FillAllHistograms(set_offset + 43);
    }
  }

  const bool embed_testing = false;
  if(embed_testing && (mutau || etau)) {
    //tighter pT cuts
    if(leptonOne.pt > 30.f && leptonTwo.pt > 30.f) FillAllHistograms(set_offset + 30);
    //barrel events
    if(std::fabs(leptonOne.eta) < 0.9f && std::fabs(leptonTwo.eta) < 0.9f) FillAllHistograms(set_offset + 31);
    //endcap events
    if(std::fabs(leptonOne.eta) > 1.5f && std::fabs(leptonTwo.eta) > 1.5f) FillAllHistograms(set_offset + 32);
    //low |phi(met)| events
    if(std::fabs(metPhi) < M_PI/2.) FillAllHistograms(set_offset + 33);
    //high |phi(met)| events
    if(std::fabs(metPhi) > M_PI/2.) FillAllHistograms(set_offset + 34);
    //lower MET events
    if(met < 40.f) FillAllHistograms(set_offset + 35);
    //high MET events
    if(met > 40.f) FillAllHistograms(set_offset + 36);
    //Lower vs. high di-lepton delta phi
    if(std::fabs(fTreeVars.lepdeltaphi - M_PI/2.) < 0.5) FillAllHistograms(set_offset + 39);
    else if(fTreeVars.lepdeltaphi > M_PI*0.95)           FillAllHistograms(set_offset + 40);
    //Lower vs. higher di-lepton pT
    if(fTreeVars.leppt < 10.f) FillAllHistograms(set_offset + 41);
    else                       FillAllHistograms(set_offset + 42);
    //Stricted Drell-Yan veto set
    bool test = fTreeVars.mttwo < 60.f && met < 40.f && leptonOne.pt > 30.f;
    if(test) FillAllHistograms(set_offset + 43);
    //BDT score regions
    const int imva = (mutau) ? 1 : (etau) ? 3 : (emu && lep_tau == 0) ? 5 : (lep_tau == 1) ? 7 : 9;
    if(fMvaUse[imva] > 0.4) FillAllHistograms(set_offset + 44);
    if(fMvaUse[imva] < 0.2) FillAllHistograms(set_offset + 45);
    if(fMvaUse[imva] > 0.2) FillAllHistograms(set_offset + 46);
    if(nPV > 25) FillAllHistograms(set_offset + 47);
    else         FillAllHistograms(set_offset + 48);
  }

  //End the cutflow testing
  if(fCutFlowTesting) return kTRUE;

  //Test Z->mumu rejection
  if(fDoEleIDStudy) {  //reduce gamma --> e to reduce Z->mu mu* --> mu mu gamma
    bool emu_study = emu; //use a local version of the emu selection flag
    emu_study &= Electron_convVeto[0]; //veto gamma --> e ID'd events
    emu_study &= Electron_isPFcand[0]; //gamma->e less likely to be a PF candidate
    emu_study &= Electron_lostHits[0] == 0; //gamma --> e leaves more missing hits
    emu_study &= leptonOne.dxy < 0.01;
    emu_study &= leptonOne.dz  < 0.03;
    // emu_study &= std::fabs(leptonTwo.dxy-leptonOne.dxy) < 0.01;
    emu_study &= Electron_r9[0] > 0.45;
    emu_study &= Electron_mvaFall17V2noIso_WP80[0];
    if(emu_study && (do_barrel == 0 || (barrel && do_barrel == 1) || (!barrel && do_barrel == -1)))
      FillAllHistograms(set_offset + 20);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 25/26/27/28 + selection offset: Mass region splitting of searches
  ////////////////////////////////////////////////////////////////////////////

  if(mutau || etau || mutau_e || etau_mu) {
    const float prev_min(min_mass_), prev_max(max_mass_); //full mass range
    const float z_region = (mutau_e || etau_mu) ? 50.f : 60.f; //low mass region for Z->tau tau
    const float zll_low(85.f), zll_high(100.f); //Z->ll region (for l->tau_h)
    const float fake_region = (mutau_e || etau_mu) ? 100.f : zll_high; //high mass region with fakes/WW/ttbar
    const float central_high = (mutau_e || etau_mu) ? fake_region : zll_low; //upper bound on mid mass range
    const int loc_set_offset = (mutau_e) ? set_offset + (kMuTauE-kEMu) : (etau_mu) ? set_offset + (kETauMu - kEMu) : set_offset;
    //central mass region
    if(mll > z_region   - sys_buffer && mll < central_high + sys_buffer) {min_mass_ = z_region   ; max_mass_ = central_high; FillAllHistograms(loc_set_offset + 25);}
    //j-->tau/QCD/WW/ttbar control
    if(mll > fake_region - sys_buffer)                                   {min_mass_ = fake_region; max_mass_ = prev_max    ; FillAllHistograms(loc_set_offset + 26);}
    //Z->tau tau control
    if(mll < z_region   + sys_buffer)                                    {min_mass_ = prev_min   ; max_mass_ = z_region    ; FillAllHistograms(loc_set_offset + 27);}
      //Z->ee/mumu control
    if(mutau || etau) {
      if(mll > zll_low - sys_buffer && mll < zll_high + sys_buffer)      {min_mass_ = zll_low    ; max_mass_ = zll_high    ; FillAllHistograms(loc_set_offset + 28);}
    }

    //Debug set (29)
    if(mutau || etau || mutau_e || etau_mu) {
      //low score, central mass region
      const bool mass_check = mll > z_region - sys_buffer && mll < central_high + sys_buffer;
      const int imva = (mutau) ? 1 : (etau) ? 3 : (mutau_e) ? 7 : /*etau_mu*/ 9;
      const float mvascore = fMvaUse[imva];
      const bool score_check = mvascore > -0.5 && (mvascore < ((mutau) ? 0.20 : (etau) ? 0.25 : (mutau_e) ? 0.15 : /*etau_mu*/ 0.15)); //region of interest
      if(mass_check && score_check) {min_mass_ = z_region  ; max_mass_ = central_high; FillAllHistograms(loc_set_offset + 29);}
    }

    //restore the mass cuts to the full mass region
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

  if(!lep_tau && emu) {
    ////////////////////////////////////////////////////////////////////////////
    // Set 24-25 : Mass window sets
    ////////////////////////////////////////////////////////////////////////////
    //Z window
    if(emu && mll < 96.f && mll > 85.f)    FillAllHistograms(set_offset + 24);
    //higgs window
    if(fDoHiggs && emu && mll < 130.f && mll > 120.f)  FillAllHistograms(set_offset + 25);
  }

  if(!fDoMVASets) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 9-18 : BDT Cut categories
  ////////////////////////////////////////////////////////////////////////////

  if(fDoMVASets) {
    //Set event weight to ignore training sample
    Float_t prev_wt = eventWeight;
    //only use weight if MC or is same sign data
    eventWeight = (fIsData == 0 || !chargeTest) ? std::fabs(fTreeVars.eventweightMVA) : std::fabs(prev_wt); //use abs to remove gen weight sign

    //Loop through each boson MVA
    for(TString boson : {"z", "h"}) {
      if(boson == "h" && !fDoHiggs) continue;
      int category = -1; //histogram set to use, based on MVA score
      //Z MVAs: 9 - 13; H MVAs 14 - 18
      if     (mutau  ) category = Category(boson + "mutau");
      else if(etau   ) category = Category(boson + "etau");
      else if(mutau_e) category = Category(boson + "etau_mu");
      else if(etau_mu) category = Category(boson + "etau_mu");
      else if(emu)     category = Category(boson + "emu");
      else if(mumu)    category = Category(boson + "emu");
      else if(ee)      category = Category(boson + "emu");
      if(category > -1) {
        const int mva_set_offset = set_offset + ((mutau_e) ? (kMuTauE-kEMu) : (etau_mu) ? (kETauMu - kEMu) : 0) + ((boson == "z") ? 9 : 14);
        FillAllHistograms(mva_set_offset + category);
        if(emu && fDoEleIDStudy && boson == "z") {
          if(barrel)
            FillAllHistograms(set_offset + 70 + category);
          else
            FillAllHistograms(set_offset + 80 + category);
        }
      }
    }
    //restore event weight
    eventWeight = prev_wt;
  }


  return kTRUE;
}
