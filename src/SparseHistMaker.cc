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
    fEventSets [kMuTau + 8] = 1; // preselection
  }
  if(etau) {
    fEventSets [kETau + 1] = 1; // all events
    fEventSets [kETau + 8] = 1; // preselection
  }
  if(emu) {
    fEventSets [kEMu  + 1] = 1; // all events
    fEventSets [kEMu  + 8] = 1; // preselection
    //Leptonic tau channels
    fEventSets[kMuTauE + 1] = 1;
    fEventSets[kETauMu + 1] = 1;
  }
  if(mumu) {
    fEventSets [kMuMu + 1] = 1; // all events
    fEventSets [kMuMu + 8] = 1; // preselection
  }
  if(ee) {
    fEventSets [kEE   + 1] = 1; // all events
    fEventSets [kEE   + 8] = 1; // preselection
  }
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

      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                 , "ht"                  , Form("%s: Ht"                  ,dirname), 100,    0, 200, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);


      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);

      for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig.names_[j].Data()) ,
                           fMVAConfig.NBins(j), fMVAConfig.Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig.names_[j].Data()), 3000, -1.,  2., folder);
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
      Utilities::BookH1F(fLepHist[i]->hOneRelIso      , "onereliso"        , Form("%s: Iso / Pt"     ,dirname),  50,    0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hOneTrigger     , "onetrigger"       , Form("%s: Trigger"      ,dirname),  10,    0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hOneMetDeltaPhi , "onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,    0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hTwoPt[0]       , "twopt"            , Form("%s: Pt"          ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoEta         , "twoeta"           , Form("%s: Eta"         ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXY         , "twodxy"           , Form("%s: DXY"         ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZ          , "twodz"            , Form("%s: DZ"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoRelIso      , "tworeliso"        , Form("%s: Iso / Pt"    ,dirname),  50,   0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoTrigger     , "twotrigger"       , Form("%s: Trigger"     ,dirname),  10,   0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoMetDeltaPhi , "twometdeltaphi"   , Form("%s: Met Delta Phi",dirname), 50,   0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hPtDiff         , "ptdiff"           , Form("%s: 1 pT - 2 pT"  ,dirname), 100,  -80,   80, folder);

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
  Hist->hEventWeight              ->Fill(eventWeight);
  Hist->hLogEventWeight           ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hGenWeight                ->Fill(genWeight                   , eventWeight          );
  Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
  Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
  Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  Hist->hNPV[0]              ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight)   ;
  Hist->hNGenTaus            ->Fill(nGenTaus             , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons        , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons            , genWeight*eventWeight)      ;
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;


  if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOneP4->Pt()             , genWeight*eventWeight)   ;
  }
  Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);

  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
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
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()           ,eventWeight*genWeight);
  Hist->hOneDXY       ->Fill(leptonOneDXY                 ,eventWeight*genWeight);
  Hist->hOneDZ        ->Fill(leptonOneDZ                  ,eventWeight*genWeight);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOneTrigger               ,eventWeight*genWeight);
  double oneMetDelPhi  = std::fabs(leptonOneP4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI) oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()           ,eventWeight*genWeight);
  Hist->hTwoDXY       ->Fill(leptonTwoDXY                 ,eventWeight*genWeight);
  Hist->hTwoDZ        ->Fill(leptonTwoDZ                  ,eventWeight*genWeight);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwoTrigger               ,eventWeight*genWeight);
  double twoMetDelPhi  = std::fabs(leptonTwoP4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI) twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);
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
  fTimes[3] = std::chrono::steady_clock::now(); //timer for filling all histograms
  FillAllHistograms(set_offset + 1);
  IncrementTimer(3, true);

  fCutFlow->Fill(icutflow); ++icutflow; //5

  //object pT thresholds
  float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  if(std::abs(leptonOneFlavor) == 11 && leptonOneP4->Pt() <= electron_pt) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 11 && leptonTwoP4->Pt() <= electron_pt) return kTRUE;
  if(std::abs(leptonOneFlavor) == 13 && leptonOneP4->Pt() <= muon_pt    ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 13 && leptonTwoP4->Pt() <= muon_pt    ) return kTRUE;
  if(std::abs(leptonOneFlavor) == 15 && leptonOneP4->Pt() <= tau_pt     ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 15 && leptonTwoP4->Pt() <= tau_pt     ) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //6

  //eta region cuts
  const double electron_eta_max = (fUseEmbedCuts) ? 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.2 : 2.3;
  if(std::abs(leptonOneFlavor) == 11 && std::fabs(leptonOneP4->Eta()) >= electron_eta_max) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 11 && std::fabs(leptonTwoP4->Eta()) >= electron_eta_max) return kTRUE;
  if(std::abs(leptonOneFlavor) == 13 && std::fabs(leptonOneP4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 13 && std::fabs(leptonTwoP4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(std::abs(leptonOneFlavor) == 15 && std::fabs(leptonOneP4->Eta()) >= tau_eta_max     ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 15 && std::fabs(leptonTwoP4->Eta()) >= tau_eta_max     ) return kTRUE;

  if(std::fabs(leptonOneP4->DeltaR(*leptonTwoP4)) < 0.3) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //7

  const double mll = (*leptonOneP4+*leptonTwoP4).M();
  if(mll <= 51. || mll >= 170.) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //8

  if(!(leptonOneFired || leptonTwoFired)) return kTRUE;

  ee   &= !isLooseElectron && nElectrons == 2;
  mumu &= !isLooseMuon && nMuons == 2;
  mutau &= !isLooseMuon && nElectrons == 0;
  etau  &= !isLooseElectron && nMuons == 0;
  emu &= !isLooseElectron; //QCD loose ID is only loose muon region

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


  //reject electrons in the barrel/endcap gap region
  const float elec_gap_low(1.4442), elec_gap_high(1.566);
  etau &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  emu  &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  ee   &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  ee   &= elec_gap_low > std::fabs(leptonTwoSCEta) || std::fabs(leptonTwoSCEta) > elec_gap_high;

  fCutFlow->Fill(icutflow); ++icutflow; //9

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  //Tau ID selections
  mutau &= leptonTwoID2 >= 2;
  etau  &= tauDeepAntiEle >= 50;
  etau  &= leptonTwoID2 >= 2;

  mutau &= tauDecayMode != 5 && tauDecayMode != 6;
  etau  &= tauDecayMode != 5 && tauDecayMode != 6;

  fCutFlow->Fill(icutflow); ++icutflow; //10

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //11

  //b-jet veto
  if(emu && nBJets20L > 0) return kTRUE;
  if(nBJets20 > 0) return kTRUE;


  //MET cuts
  const double met_cut       = 60.;
  const double mtlep_cut     = 70.;

  fCutFlow->Fill(icutflow); ++icutflow; //12

  if(met >= met_cut) return kTRUE;
  if(fTreeVars.mtlep >= mtlep_cut) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 8 + selection offset: preselection
  ////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 8);

  fCutFlow->Fill(icutflow); ++icutflow; //13

  return kTRUE;
}
