#define SPARSEHISTMAKER_CXX

#include "interface/SparseHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
SparseHistMaker::SparseHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {
  fDoSystematics = 0;
  fWriteTrees = 0;
}

//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::Begin(TTree * /*tree*/)
{
  HistMaker::Begin(nullptr);
}


//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i]  = 0;
    fSysSets[i] = 0;
    fTreeSets[i]  = 0;
    fSetFills [i] = 0;
  }

  //Event Sets
  const bool mutau   = fSelection == "" || fSelection == "mutau"  ;
  const bool etau    = fSelection == "" || fSelection == "etau"   ;
  const bool emu     = fSelection == "" || fSelection == "emu"    ;
  const bool mumu    = fSelection == "" || fSelection == "mumu"   ;
  const bool ee      = fSelection == "" || fSelection == "ee"     ;
  const bool mutau_e = fSelection == "" || fSelection == "mutau_e";
  const bool etau_mu = fSelection == "" || fSelection == "etau_mu";

  if(mutau) {
    fEventSets [kMuTau + 1] = 1; // all events
    fEventSets [kMuTau + 8] = 1; // preselection

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kMuTau + 20] = 1; //object cleaning
      fEventSets [kMuTau + 21] = 1; //
      fEventSets [kMuTau + 22] = 1; //
      fEventSets [kMuTau + 23] = 1; //
      fEventSets [kMuTau + 24] = 1; //
      fEventSets [kMuTau + 25] = 1; //
      fEventSets [kMuTau + 26] = 1; //
      fEventSets [kMuTau + 27] = 1; //
      fEventSets [kMuTau + 28] = 1; //
      fEventSets [kMuTau + 30] = 1; // BDT score cut
    }
  }
  if(etau) {
    fEventSets [kETau + 1] = 1; // all events
    fEventSets [kETau + 8] = 1; // preselection

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kETau + 20] = 1; //object cleaning
      fEventSets [kETau + 21] = 1; //
      fEventSets [kETau + 22] = 1; //
      fEventSets [kETau + 23] = 1; //
      fEventSets [kETau + 24] = 1; //
      fEventSets [kETau + 25] = 1; //
      fEventSets [kETau + 26] = 1; //
      fEventSets [kETau + 27] = 1; //
      fEventSets [kETau + 28] = 1; //
      fEventSets [kETau + 30] = 1; // BDT score cut
    }
  }
  if(emu || mutau_e || etau_mu) {
    fEventSets [kEMu  + 1] = 1; // all events
    fEventSets [kEMu  + 8] = 1; // preselection

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kEMu  + 20] = 1; //object cleaning
      fEventSets [kEMu  + 21] = 1; //
      fEventSets [kEMu  + 22] = 1; //
      fEventSets [kEMu  + 23] = 1; //
      fEventSets [kEMu  + 24] = 1; //
      fEventSets [kEMu  + 25] = 1; //
      fEventSets [kEMu  + 26] = 1; //
      fEventSets [kEMu  + 27] = 1; //
      fEventSets [kEMu  + 28] = 1; //
      fEventSets [kEMu  + 30] = 1; // BDT score cut
    }
  }

  //Leptonic tau channels
  if(mutau_e) {
    fEventSets[kMuTauE + 1] = 1;
    fEventSets[kMuTauE + 8] = 1;

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kMuTauE + 20] = 1; //object cleaning
      fEventSets [kMuTauE + 21] = 1; //
      fEventSets [kMuTauE + 22] = 1; //
      fEventSets [kMuTauE + 23] = 1; //
      fEventSets [kMuTauE + 24] = 1; //
      fEventSets [kMuTauE + 25] = 1; //
      fEventSets [kMuTauE + 26] = 1; //
      fEventSets [kMuTauE + 27] = 1; //
      fEventSets [kMuTauE + 28] = 1; //
      fEventSets [kMuTauE + 30] = 1; // BDT score cut
    }
  }

  if(etau_mu) {
    fEventSets[kETauMu + 1] = 1;
    fEventSets[kETauMu + 8] = 1;

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kETauMu + 20] = 1; //object cleaning
      fEventSets [kETauMu + 21] = 1; //
      fEventSets [kETauMu + 22] = 1; //
      fEventSets [kETauMu + 23] = 1; //
      fEventSets [kETauMu + 24] = 1; //
      fEventSets [kETauMu + 25] = 1; //
      fEventSets [kETauMu + 26] = 1; //
      fEventSets [kETauMu + 27] = 1; //
      fEventSets [kETauMu + 28] = 1; //
      fEventSets [kETauMu + 30] = 1; // BDT score cut
    }
  }

  if(mumu) {
    fEventSets [kMuMu + 1] = 1; // all events
    fEventSets [kMuMu + 8] = 1; // preselection

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kMuMu + 20] = 1; //object cleaning
      fEventSets [kMuMu + 21] = 1; //
      fEventSets [kMuMu + 22] = 1; //
      fEventSets [kMuMu + 23] = 1; //
      fEventSets [kMuMu + 24] = 1; //
      fEventSets [kMuMu + 25] = 1; //
      fEventSets [kMuMu + 26] = 1; //
      fEventSets [kMuMu + 27] = 1; //
      fEventSets [kMuMu + 28] = 1; //
      fEventSets [kMuMu + 30] = 1; // BDT score cut
    }
  }
  if(ee) {
    fEventSets [kEE   + 1] = 1; // all events
    fEventSets [kEE   + 8] = 1; // preselection

    //cutflow histogram sets
    if(fDoCutFlowSets) {
      fEventSets [kEE   + 20] = 1; //object cleaning
      fEventSets [kEE   + 21] = 1; //
      fEventSets [kEE   + 22] = 1; //
      fEventSets [kEE   + 23] = 1; //
      fEventSets [kEE   + 24] = 1; //
      fEventSets [kEE   + 25] = 1; //
      fEventSets [kEE   + 26] = 1; //
      fEventSets [kEE   + 27] = 1; //
      fEventSets [kEE   + 28] = 1; //
      fEventSets [kEE   + 30] = 1; // BDT score cut
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::InitializeInputTree(TTree* tree) {
  HistMaker::InitializeInputTree(tree);
}

//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::BookEventHistograms() {
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
      Utilities::BookH1F(fEventHist[i]->hEventWeight             , "eventweight"             , Form("%s: EventWeight"                 ,dirname), 100,   -1,   3, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEventWeight          , "logeventweight"          , Form("%s: LogEventWeight"              ,dirname),  50,  -10,   1, folder);
      Utilities::BookH1D(fEventHist[i]->hGenWeight               , "genweight"               , Form("%s: GenWeight"                   ,dirname),   5, -2.5, 2.5, folder);
      Utilities::BookH1D(fEventHist[i]->hNMuons                  , "nmuons"                  , Form("%s: NMuons"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNElectrons              , "nelectrons"              , Form("%s: NElectrons"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTaus                   , "ntaus"                   , Form("%s: NTaus"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hNPV[0]                  , "npv"                     , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNFwdJets                , "nfwdjets"                , Form("%s: NFwdJets"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPt[0]                , "jetpt"                   , Form("%s: JetPt"                       ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                      , "ht"                      , Form("%s: Ht"                          ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMet                     , "met"                     , Form("%s: Met"                         ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMTOne                   , "mtone"                   , Form("%s: MTOne"                       ,dirname), 100,    0, 150, folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo                   , "mttwo"                   , Form("%s: MTTwo"                       ,dirname), 100,    0, 150, folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep                   , "mtlep"                   , Form("%s: MTLep"                       ,dirname), 100,    0, 150, folder);
      Utilities::BookH1F(fEventHist[i]->hMTRatio                 , "mtratio"                 , Form("%s: MTRatio"                     ,dirname),  40,   0.,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hMTLead                  , "mtlead"                  , Form("%s: MTLead"                      ,dirname),  20,   0., 150, folder);
      Utilities::BookH1F(fEventHist[i]->hMTTrail                 , "mttrail"                 , Form("%s: MTTrail"                     ,dirname),  20,   0., 150, folder);
      Utilities::BookH1F(fEventHist[i]->hMTOneOverM              , "mtoneoverm"              , Form("%s: MTOneOverM"                  ,dirname), 100,   0.,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwoOverM              , "mttwooverm"              , Form("%s: MTTwoOverM"                  ,dirname), 100,   0.,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hMTLepOverM              , "mtlepoverm"              , Form("%s: MTLepOverM"                  ,dirname), 100,   0.,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPt[0]                , "leppt"                   , Form("%s: Lepton Pt"                   ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0]                 , "lepm"                    , Form("%s: Lepton M"                    ,dirname), 280,   40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt                   , "lepmt"                   , Form("%s: Lepton Mt"                   ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate            , "lepmestimate"            , Form("%s: Estimate di-lepton mass"     ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo         , "lepmestimatetwo"         , Form("%s: Estimate di-lepton mass"     ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]           , "deltaalpha0"             , Form("%s: Delta Alpha (Z) 0"           ,dirname),  80,   -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]           , "deltaalpha1"             , Form("%s: Delta Alpha (Z) 1"           ,dirname),  80,   -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]           , "deltaalpha2"             , Form("%s: Delta Alpha (H) 0"           ,dirname),  80,   -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]           , "deltaalpha3"             , Form("%s: Delta Alpha (H) 1"           ,dirname),  80,   -5,  10, folder);

      for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig->names_[j].Data()) ,
                           fMVAConfig->NBins(j), fMVAConfig->Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig->names_[j].Data()), 3000, -1.,  2., folder);
      }

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i] = folder;
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;

      Utilities::BookH1F(fLepHist[i]->hOnePt[0]       , "onept"            , Form("%s: Pt"           ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOneEta         , "oneeta"           , Form("%s: Eta"          ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDXY         , "onedxy"           , Form("%s: DXY"          ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDZ          , "onedz"            , Form("%s: DZ"           ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDXYSig      , "onedxysig"        , Form("%s: DXY/err"      ,dirname),  30,    0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDZSig       , "onedzsig"         , Form("%s: DZ/err"       ,dirname),  30,    0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneRelIso      , "onereliso"        , Form("%s: Iso / Pt"     ,dirname),  50,    0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hOneTrigger     , "onetrigger"       , Form("%s: Trigger"      ,dirname),  10,    0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hOneMetDeltaPhi , "onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,    0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hTwoPt[0]       , "twopt"            , Form("%s: Pt"          ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoEta         , "twoeta"           , Form("%s: Eta"         ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXY         , "twodxy"           , Form("%s: DXY"         ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZ          , "twodz"            , Form("%s: DZ"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXYSig      , "twodxysig"        , Form("%s: DXY/err"     ,dirname),  30,   0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZSig       , "twodzsig"         , Form("%s: DZ/err"      ,dirname),  30,   0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoRelIso      , "tworeliso"        , Form("%s: Iso / Pt"    ,dirname),  50,   0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoTrigger     , "twotrigger"       , Form("%s: Trigger"     ,dirname),  10,   0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoMetDeltaPhi , "twometdeltaphi"   , Form("%s: Met Delta Phi",dirname), 50,   0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hPtDiff         , "ptdiff"           , Form("%s: 1 pT - 2 pT"  ,dirname), 100,  -80,   80, folder);
      Utilities::BookH1F(fLepHist[i]->hPtRatio        , "ptratio"          , Form("%s: 1 pT / 2 pT"  ,dirname),  50,    0,    3, folder);
      Utilities::BookH1F(fLepHist[i]->hPtTrailOverLead, "pttrailoverlead"  , Form("%s: trail pT / lead pT",dirname),  20,    0,    1, folder);

      delete dirname;
    }
  }
}


//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  Hist->hEventWeight         ->Fill(eventWeight);
  Hist->hLogEventWeight      ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hGenWeight           ->Fill(genWeight               , eventWeight          );
  Hist->hNMuons              ->Fill(nMuons                  , genWeight*eventWeight);
  Hist->hNElectrons          ->Fill(nElectrons              , genWeight*eventWeight);
  Hist->hNTaus               ->Fill(nTaus                   , genWeight*eventWeight);
  Hist->hNPV[0]              ->Fill(nPV                     , genWeight*eventWeight);
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight);
  Hist->hNGenTaus            ->Fill(nGenTaus                , genWeight*eventWeight);
  Hist->hNGenElectrons       ->Fill(nGenElectrons           , genWeight*eventWeight);
  Hist->hNGenMuons           ->Fill(nGenMuons               , genWeight*eventWeight);
  Hist->hNJets20[0]          ->Fill(nJets20                 , genWeight*eventWeight);
  Hist->hNFwdJets            ->Fill(nFwdJets                , genWeight*eventWeight);
  Hist->hNBJets20[0]         ->Fill(nBJets20                , genWeight*eventWeight);
  Hist->hNBJets20L[0]        ->Fill(nBJets20L               , genWeight*eventWeight);


  if(jetOne.p4 && jetOne.p4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOne.p4->Pt()            , genWeight*eventWeight);
  }
  Hist->hHt                ->Fill(ht                        , genWeight*eventWeight);

  Hist->hMet               ->Fill(met                       , genWeight*eventWeight);
  Hist->hMTOne             ->Fill(fTreeVars.mtone           , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo           , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep           , eventWeight*genWeight);
  Hist->hMTLead            ->Fill(fTreeVars.mtlead          , eventWeight*genWeight);
  Hist->hMTTrail           ->Fill(fTreeVars.mttrail         , eventWeight*genWeight);
  Hist->hMTRatio           ->Fill(fTreeVars.mtone / fTreeVars.mttwo, eventWeight*genWeight);
  Hist->hMTOneOverM        ->Fill(fTreeVars.mtoneoverm      , eventWeight*genWeight);
  Hist->hMTTwoOverM        ->Fill(fTreeVars.mttwooverm      , eventWeight*genWeight);
  Hist->hMTLepOverM        ->Fill(fTreeVars.mtlepoverm      , eventWeight*genWeight);

  TLorentzVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);

  Hist->hLepPt[0]          ->Fill(lepSys.Pt()               , eventWeight*genWeight);
  Hist->hLepM[0]           ->Fill(lepSys.M()                , eventWeight*genWeight);
  Hist->hLepMt             ->Fill(lepSys.Mt()               , eventWeight*genWeight);
  Hist->hLepMEstimate      ->Fill(fTreeVars.mestimate       , eventWeight*genWeight);
  Hist->hLepMEstimateTwo   ->Fill(fTreeVars.mestimatetwo    , eventWeight*genWeight);

  Hist->hDeltaAlpha[0]     ->Fill(fTreeVars.deltaalphaz1    , eventWeight*genWeight);
  Hist->hDeltaAlpha[1]     ->Fill(fTreeVars.deltaalphaz2    , eventWeight*genWeight);
  Hist->hDeltaAlpha[2]     ->Fill(fTreeVars.deltaalphah1    , eventWeight*genWeight);
  Hist->hDeltaAlpha[3]     ->Fill(fTreeVars.deltaalphah2    , eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
  }
}

//--------------------------------------------------------------------------------------------------------------
void SparseHistMaker::FillLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }

  /////////////
  //  Lep 1  //
  /////////////
  Hist->hOnePt[0]          ->Fill(fTreeVars.leponept        , eventWeight*genWeight);
  Hist->hOneEta            ->Fill(leptonOne.p4->Eta()        , eventWeight*genWeight);
  Hist->hOneDXY            ->Fill(leptonOne.dxy              , eventWeight*genWeight);
  Hist->hOneDZ             ->Fill(leptonOne.dz               , eventWeight*genWeight);
  Hist->hOneDXYSig         ->Fill(leptonOne.dxySig              ,eventWeight*genWeight);
  Hist->hOneDZSig          ->Fill(leptonOne.dzSig               ,eventWeight*genWeight);
  Hist->hOneRelIso         ->Fill(fTreeVars.leponereliso    , eventWeight*genWeight);
  Hist->hOneTrigger        ->Fill(leptonOne.trigger          , eventWeight*genWeight);
  double oneMetDelPhi  = std::fabs(leptonOne.p4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI) oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi    ->Fill(oneMetDelPhi              , eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPt[0]          ->Fill(fTreeVars.leptwopt        , eventWeight*genWeight);
  Hist->hTwoEta            ->Fill(leptonTwo.p4->Eta()        , eventWeight*genWeight);
  Hist->hTwoDXY            ->Fill(leptonTwo.dxy              , eventWeight*genWeight);
  Hist->hTwoDZ             ->Fill(leptonTwo.dz               , eventWeight*genWeight);
  Hist->hTwoDXYSig         ->Fill(leptonTwo.dxySig              ,eventWeight*genWeight);
  Hist->hTwoDZSig          ->Fill(leptonTwo.dzSig               ,eventWeight*genWeight);
  Hist->hTwoRelIso         ->Fill(fTreeVars.leptworeliso    , eventWeight*genWeight);
  Hist->hTwoTrigger        ->Fill(leptonTwo.trigger          , eventWeight*genWeight);
  double twoMetDelPhi  = std::fabs(leptonTwo.p4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI) twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi    ->Fill(twoMetDelPhi              , eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff            ->Fill(fTreeVars.leponept-fTreeVars.leptwopt, eventWeight*genWeight);
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t SparseHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: base selection
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  FillAllHistograms(set_offset + 1);

  fCutFlow->Fill(icutflow); ++icutflow; //5

  //object pT thresholds
  float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  if(leptonOne.isElectron()) one_pt_min_ = electron_pt;
  if(leptonTwo.isElectron()) two_pt_min_ = electron_pt;
  if(leptonOne.isMuon    ()) one_pt_min_ = muon_pt;
  if(leptonTwo.isMuon    ()) two_pt_min_ = muon_pt;
  if(leptonOne.isTau     ()) one_pt_min_ = tau_pt;
  if(leptonTwo.isTau     ()) two_pt_min_ = tau_pt;

  //remove events with extra leptons
  ee    &= nElectrons == 2;
  mumu  &= nMuons     == 2;
  etau  &= nMuons     == 0;
  mutau &= nElectrons == 0;
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ///////////////////////////////////////////////////////////////////
  //ensure the objects PDG IDs make sense

  mutau &= leptonOne.isMuon    () && leptonTwo.isTau     ();
  etau  &= leptonOne.isElectron() && leptonTwo.isTau     ();
  emu   &= leptonOne.isElectron() && leptonTwo.isMuon    ();
  mumu  &= leptonOne.isMuon    () && leptonTwo.isMuon    ();
  ee    &= leptonOne.isElectron() && leptonTwo.isElectron();


  ///////////////////////////////////////////////////////////////////
  //leptons must satisfy the pt requirements and fire a trigger

  if(leptonOne.isElectron() && leptonOne.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonOne.isTau     () && leptonOne.p4->Pt() <= tau_pt     ) return kTRUE;
  if(leptonTwo.isTau     () && leptonTwo.p4->Pt() <= tau_pt     ) return kTRUE;

  bool trigger_matched(false); //check if a lepton is matched to a trigger
  trigger_matched |= leptonOne.isMuon    () && leptonOne.pt > muon_trig_pt_     && leptonOne.matched;
  trigger_matched |= leptonTwo.isMuon    () && leptonTwo.pt > muon_trig_pt_     && leptonTwo.matched;
  trigger_matched |= leptonOne.isElectron() && leptonOne.pt > electron_trig_pt_ && leptonOne.matched;
  trigger_matched |= leptonTwo.isElectron() && leptonTwo.pt > electron_trig_pt_ && leptonTwo.matched;

  if(!trigger_matched) return kTRUE;

  //equivalent check
  if(!(leptonOne.fired || leptonTwo.fired)) {
    printf("!!! Entry %lld: Leptons are matched and pass trigger pT threshold, but aren't marked as firing!\n", fentry);
    return kTRUE;
  }

  fCutFlow->Fill(icutflow); ++icutflow; //6

  ///////////////////////////////////////////////////////////////////
  //eta region and delta R cuts

  const double electron_eta_max = (fUseEmbedCuts) ? (emu || ee || mumu) ? 2.4 : 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? (emu || ee || mumu) ? 2.4 : 2.2 : 2.4;
  const double tau_eta_max      = 2.3;
  const double min_delta_r      = 0.3; //separation between leptons
  const float elec_gap_low(1.444), elec_gap_high(1.566);

  if(leptonOne.isElectron() && std::fabs(leptonOne.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonOne.isMuon    () && std::fabs(leptonOne.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonTwo.isMuon    () && std::fabs(leptonTwo.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonOne.isTau     () && std::fabs(leptonOne.p4->Eta()) >= tau_eta_max     ) return kTRUE;
  if(leptonTwo.isTau     () && std::fabs(leptonTwo.p4->Eta()) >= tau_eta_max     ) return kTRUE;

  if(leptonOne.isElectron() && std::fabs(leptonOne.scEta) >= elec_gap_low && std::fabs(leptonOne.scEta) <= elec_gap_high) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.scEta) >= elec_gap_low && std::fabs(leptonTwo.scEta) <= elec_gap_high) return kTRUE;

  if(fTreeVars.lepdeltar <= min_delta_r) return kTRUE;

  //apply reasonable lepton isolation cuts
  const float max_rel_iso = 0.5;
  if(leptonOne.isElectron() && leptonOne.relIso >= max_rel_iso) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.relIso >= max_rel_iso) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.relIso >= max_rel_iso) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.relIso >= max_rel_iso) return kTRUE;

  //remove loose ID light lepton events (only used for control region measurements)
  if(isLooseMuon    ) return kTRUE;
  if(isLooseElectron) return kTRUE;

  ///////////////////////////////////////////////////////////////////
  //ensure reasonable dxy/dz cuts for electrons/muons

  const float max_dz(0.5), max_dxy(0.2);
  if((leptonOne.isElectron() || leptonOne.isMuon()) && std::fabs(leptonOne.dxy) >= max_dxy) return kTRUE;
  if((leptonOne.isElectron() || leptonOne.isMuon()) && std::fabs(leptonOne.dz ) >= max_dz ) return kTRUE;
  if((leptonTwo.isElectron() || leptonOne.isMuon()) && std::fabs(leptonOne.dxy) >= max_dxy) return kTRUE;
  if((leptonTwo.isElectron() || leptonTwo.isMuon()) && std::fabs(leptonTwo.dz ) >= max_dz ) return kTRUE;

  ///////////////////////////////////////////////////////////////////
  //check additional object IDs

  //Hadronic tau IDs
  if(leptonOne.isTau() && !(isLooseTau || tauDeepAntiJet >= 50))          return kTRUE; //tight anti-jet ID
  if(leptonTwo.isTau() && !(isLooseTau || tauDeepAntiJet >= 50))          return kTRUE;
  if(leptonOne.isTau() && tauDeepAntiEle < 10)                            return kTRUE; //tight (deep) anti-ele ID
  if(leptonTwo.isTau() && tauDeepAntiEle < 10)                            return kTRUE;
  if(leptonOne.isTau() && tauDeepAntiMu  < 10)                            return kTRUE; //tight (deep) anti-muon ID
  if(leptonTwo.isTau() && tauDeepAntiMu  < 10)                            return kTRUE;
  if(leptonOne.isTau() && leptonOne.id2  <  2)                            return kTRUE; //tight (old) anti-muon ID
  if(leptonTwo.isTau() && leptonTwo.id2  <  2)                            return kTRUE;
  if(leptonOne.isTau() && !(tauDecayMode % 10 < 2 && tauDecayMode <= 11)) return kTRUE; //relevant tau decay modes
  if(leptonTwo.isTau() && !(tauDecayMode % 10 < 2 && tauDecayMode <= 11)) return kTRUE;

  if(etau) {
    if(leptonOne.isTau() && tauDeepAntiEle < fETauAntiEleCut)             return kTRUE; //tight (deep) anti-ele ID
    if(leptonTwo.isTau() && tauDeepAntiEle < fETauAntiEleCut)             return kTRUE;
  }

  ///////////////////////////////////////////////////////////////////
  // Remove MC fake contributions

  //remove MC jet -> light lepton contribution
  if(!fUseMCEstimatedFakeLep && !fIsData) {
    if(isFakeMuon || isFakeElectron) return kTRUE;
  }

  //remove MC estimated jet --> tau component
  if((mutau || etau) && !fIsData && std::abs(tauGenFlavor) == 26) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //7

  ////////////////////////////////////////////////////////////
  // Set 20 + selection offset: base object cleaning
  ////////////////////////////////////////////////////////////

  int cut_flow_set = 20; //histogram index for cut flow histogramming
  if(fDoCutFlowSets) {
    if(btagWeight > 0.3 && !fIsData && !fIsEmbed) eventWeight = eventWeight / btagWeight;
    FillAllHistograms(set_offset + cut_flow_set);
    if(btagWeight > 0.3 && !fIsData && !fIsEmbed) eventWeight = eventWeight * btagWeight;
  }
  ++cut_flow_set;

  ///////////////////////////////////////////////////////////////////
  //Apply di-lepton mass cuts

  const float mll = fTreeVars.lepm;
  min_mass_ = (emu && !lep_tau) ?  70.f :  40.f; //FIXME: Add flag for emu-like ee/mumu histogramming
  max_mass_ = (emu && !lep_tau) ? 110.f : 170.f;

  if(mll <= min_mass_ || mll >= max_mass_) return kTRUE;


  fCutFlow->Fill(icutflow); ++icutflow; //8

  ////////////////////////////////////////////////////////////
  // Set 21 + selection offset: mass selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    if(btagWeight > 0.3 && !fIsData && !fIsEmbed) eventWeight = eventWeight / btagWeight; //no b-tag cut yet, so remove the b-tag correction
    FillAllHistograms(set_offset + cut_flow_set);
    if(btagWeight > 0.3 && !fIsData && !fIsEmbed) eventWeight = eventWeight * btagWeight;
  }
  ++cut_flow_set;


  ///////////////////////////////////////////////////////////////////
  //Apply b-jet veto

  //b-jet veto
  if(nBJetsUse > 0) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 22 + selection offset: b-tag selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  ///////////////////////////////////////////////////////////////////
  //Apply tighter light lepton cuts

  mutau_e &= emu && lep_tau == 1;
  etau_mu &= emu && lep_tau == 2;
  emu &= !lep_tau;
  if(emu) {one_pt_min_ = 20.f; two_pt_min_ = 20.f;}
  else if(mutau_e) {two_pt_min_ = 20.f;}
  else if(etau_mu) {one_pt_min_ = 20.f;}
  else if(mutau || etau) {one_pt_min_ = std::max(one_pt_min_, 28.f);}

  if(leptonOne.pt <= one_pt_min_ || leptonTwo.pt <= two_pt_min_) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //9

  ////////////////////////////////////////////////////////////
  // Set 23 + selection offset: lepton pT selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  if     (mutau_e) {ptdiff_min_ = -1.e10; ptdiff_max_ = +1.e10;}
  else if(etau_mu) {ptdiff_min_ =     0.; ptdiff_max_ = +1.e10;}

  if(fTreeVars.ptdiff <= ptdiff_min_ || fTreeVars.ptdiff >= ptdiff_max_) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //10

  ////////////////////////////////////////////////////////////
  // Set 24 + selection offset: lepton delta pT selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  ///////////////////////////////////////////////////////////////////
  //Apply MET/MT cuts

  //Apply the MET cut
  met_max_   = -1.;
  if(met_max_ > 0. && met >= met_max_) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //10

  ////////////////////////////////////////////////////////////
  // Set 25 + selection offset: MET selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  //Apply the MT(ll,MET) cut
  mtlep_max_ = (emu || ee || mumu) ? -1. : (mutau_e || etau_mu) ? 90.f : 70.f;
  if(mtlep_max_ > 0. && fTreeVars.mtlep >= mtlep_max_) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //11

  ////////////////////////////////////////////////////////////
  // Set 26 + selection offset: MT(ll,MET) selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  //Apply the MT(l,MET) cuts
  mtone_max_        = -1.f;
  mttwo_max_        = -1.f;
  if(mutau || etau || etau_mu) {
    mttwo_over_m_max_ = 0.8f;
    mtone_over_m_max_ = -1.f;
  } else if(mutau_e) {
    mtone_over_m_max_ = 0.8f;
    mttwo_over_m_max_ = -1.f;
  }

  if(mtone_max_ > 0. && fTreeVars.mtone >= mtone_max_) return kTRUE;
  if(mttwo_max_ > 0. && fTreeVars.mttwo >= mttwo_max_) return kTRUE;
  if(mtone_over_m_max_ > 0. && fTreeVars.mtoneoverm >= mtone_over_m_max_) return kTRUE;
  if(mttwo_over_m_max_ > 0. && fTreeVars.mttwooverm >= mttwo_over_m_max_) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //12

  ////////////////////////////////////////////////////////////
  // Set 27 + selection offset: MT(l,MET) selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  fCutFlow->Fill(icutflow); ++icutflow; //12

  ///////////////////////////////////////////////////////////////////
  // Apply lepton dxy/dz significance cuts

  if(emu) {
    if(std::fabs(leptonTwo.dxySig) >= 3.0) return kTRUE;
    if(std::fabs(leptonOne.dxySig) >= 3.0) return kTRUE;
    if(std::fabs(leptonTwo.dzSig ) >= 4.7) return kTRUE;
    if(std::fabs(leptonOne.dzSig ) >= 4.7) return kTRUE;
  }

  ////////////////////////////////////////////////////////////
  // Set 28 + selection offset: dxy/dz significance selection
  ////////////////////////////////////////////////////////////

  if(fDoCutFlowSets) {
    FillAllHistograms(set_offset + cut_flow_set);
  }
  ++cut_flow_set;

  if(!(mutau || etau || mutau_e || etau_mu || emu || mumu || ee)) return kTRUE;


  ////////////////////////////////////////////////////////////
  // Set 8 + selection offset: preselection
  ////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 8);

  fCutFlow->Fill(icutflow); ++icutflow; //13

  //Apply the BDT score cuts
  const int imva = (mutau) ? 1 : (etau) ? 3 : (emu) ? 5 : (mutau_e) ? 7 : 9;
  min_bdt_ = (emu) ? -1.f : 0.25f;
  if(fMvaUse[imva] <= min_bdt_) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 30 + selection offset: After BDT cut
  ////////////////////////////////////////////////////////////

  FillAllHistograms(set_offset + 30);

  fCutFlow->Fill(icutflow); ++icutflow; //14

  return kTRUE;
}
