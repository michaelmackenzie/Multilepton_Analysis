#define ZPTHISTMAKER_CXX

#include "interface/ZPtHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
ZPtHistMaker::ZPtHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {
  fDoSystematics = 0;
  fWriteTrees = 0;
}

//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::Begin(TTree * /*tree*/)
{
  HistMaker::Begin(nullptr);
}


//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i]  = 0;
    fSysSets[i] = 0;
    fTreeSets[i]  = 0;
    fSetFills [i] = 0;
  }

  //Event Sets
  const bool mutau = fSelection == "" || fSelection == "mutau";
  const bool etau  = fSelection == "" || fSelection == "etau" ;
  const bool emu   = fSelection == "" || fSelection == "emu"  ;
  const bool mumu  = fSelection == "" || fSelection == "mumu" ;
  const bool ee    = fSelection == "" || fSelection == "ee"   ;

  if(mutau) {
    fEventSets [kMuTau + 8] = 1; // preselection
  }
  if(etau) {
    fEventSets [kETau + 8] = 1; // preselection
  }
  if(emu) {
    fEventSets [kEMu  + 8] = 1; // preselection
  }
  if(mumu) {
    fEventSets [kMuMu + 7] = 1; // preselection without b-jet cut
    fEventSets [kMuMu + 8] = 1; // preselection
  }
  if(ee) {
    fEventSets [kEE   + 7] = 1; // preselection without b-jet cut
    fEventSets [kEE   + 8] = 1; // preselection
  }
}

//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::BookEventHistograms() {
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
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                 , "ht"                  , Form("%s: Ht"                  ,dirname), 100,    0, 200, folder);

      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);

      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[0], "lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaEta   , "lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[0]  , "lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname), 50,   0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);

      for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig->names_[j].Data()) ,
                           fMVAConfig->NBins(j), fMVAConfig->Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig->names_[j].Data()), 3000, -1.,  2., folder);
      }

      /////////////////////////////////////
      // Z pT spectrum specific histograms


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
      Utilities::BookH1F(fEventHist[i]->hZPt[5]     , "zpt5"          , Form("%s: Z Pt"           ,dirname)  ,     50,   0.,  150., folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[0]   , "zmass"         , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[1]   , "zmass1"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[2]   , "zmass2"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[3]   , "zmass3"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[4]   , "zmass4"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[5]   , "zmass5"        , Form("%s: Z Mass"         ,dirname)  ,     70,  40.,  180., folder);
      Utilities::BookH1F(fEventHist[i]->hZEta[1]    , "zeta1"         , Form("%s: ZEta"           ,dirname)  ,     50,  -10,    10, folder);

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::BookLepHistograms() {
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

      /////////////////////////////////////
      // Z pT spectrum specific histograms


      delete dirname;
    }
  }
}


//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("ZPtHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
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
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  if(jetOne.p4 && jetOne.p4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOne.p4->Pt()             , genWeight*eventWeight)   ;
  }
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;
  Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;

  Hist->hNGenTaus            ->Fill(nGenTaus             , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons        , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons            , genWeight*eventWeight)      ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);

  PtEtaPhiMVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
  // PtEtaPhiMVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;
  const double lepDelR   = std::fabs(Utilities::DeltaR(*leptonOne.p4, *leptonTwo.p4));
  const double lepDelPhi = std::fabs(Utilities::DeltaPhi(leptonOne.p4->Phi(), leptonTwo.p4->Phi()));
  const double lepDelEta = std::fabs(leptonOne.p4->Eta() - leptonTwo.p4->Eta());

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);
  Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR[0] ->Fill(lepDelR                ,eventWeight*genWeight);

  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
  }

  /////////////////////////////////////
  // Z pT spectrum specific histograms

  //for removing or alternate DY reweighting weights
  double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight; //no DY Z pT vs M weights
  float tmp_1, tmp_2, sys_reco_weight(1.);
  double recoweight = (fIsDY) ? bareweight*fZPtWeight->GetWeight(fYear, lepSys.Pt(), lepSys.M(), /*use reco weights*/ true, tmp_1, tmp_2, sys_reco_weight) : bareweight;
  sys_reco_weight *= bareweight;
  const double zpt   = (zPt   <  0.f) ? lepSys.Pt() : zPt;
  const double zmass = (zMass <  0.f) ? lepSys.M()  : zMass;

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
  Hist->hZPt[5]       ->Fill(zpt  , eventWeight*genWeight);
  Hist->hZMass[0]     ->Fill(zmass, eventWeight*genWeight);
  Hist->hZMass[1]     ->Fill(zmass, bareweight);
  Hist->hZMass[2]     ->Fill(zmass, recoweight);
  Hist->hZMass[3]     ->Fill(zmass, bareweight*zPtWeightSys);
  Hist->hZMass[4]     ->Fill(zmass, sys_reco_weight);
  Hist->hZMass[5]     ->Fill(zmass, eventWeight*genWeight);
  Hist->hZEta [1]     ->Fill(zEta , bareweight);

}

//--------------------------------------------------------------------------------------------------------------
void ZPtHistMaker::FillLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("ZPtHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }

  /////////////
  //  Lep 1  //
  /////////////
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOne.p4->Eta()           ,eventWeight*genWeight);
  Hist->hOneDXY       ->Fill(leptonOne.dxy                 ,eventWeight*genWeight);
  Hist->hOneDZ        ->Fill(leptonOne.dz                  ,eventWeight*genWeight);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOne.trigger               ,eventWeight*genWeight);
  double oneMetDelPhi  = std::fabs(leptonOne.p4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI) oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwo.p4->Eta()           ,eventWeight*genWeight);
  Hist->hTwoDXY       ->Fill(leptonTwo.dxy                 ,eventWeight*genWeight);
  Hist->hTwoDZ        ->Fill(leptonTwo.dz                  ,eventWeight*genWeight);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwo.trigger               ,eventWeight*genWeight);
  double twoMetDelPhi  = std::fabs(leptonTwo.p4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI) twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);

  /////////////////////////////////////
  // Z pT spectrum specific histograms

}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t ZPtHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  //object pT thresholds
  const float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  //ignore all but mumu/ee for now
  if(!(mumu || ee)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //5

  if(leptonOne.isElectron() && leptonOne.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonOne.isTau     () && leptonOne.p4->Pt() <= tau_pt     ) return kTRUE;
  if(leptonTwo.isTau     () && leptonTwo.p4->Pt() <= tau_pt     ) return kTRUE;

  //leptons must satisfy the pt requirements and fire a trigger
  mumu  &= ((leptonOne.p4->Pt() > muon_trig_pt_ && leptonOne.fired) ||
            (leptonTwo.p4->Pt() > muon_trig_pt_ && leptonTwo.fired));
  ee    &= ((leptonOne.p4->Pt() > electron_trig_pt_ && leptonOne.fired) ||
            (leptonTwo.p4->Pt() > electron_trig_pt_ && leptonTwo.fired));

  if(!(mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //6

  //eta region cuts
  const double electron_eta_max = (fUseEmbedCuts) ? 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.2 : 2.3;
  if(leptonOne.isElectron() && std::fabs(leptonOne.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonOne.isMuon    () && std::fabs(leptonOne.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonTwo.isMuon    () && std::fabs(leptonTwo.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonOne.isTau     () && std::fabs(leptonOne.p4->Eta()) >= tau_eta_max     ) return kTRUE;
  if(leptonTwo.isTau     () && std::fabs(leptonTwo.p4->Eta()) >= tau_eta_max     ) return kTRUE;

  //reject electrons in the barrel/endcap gap region
  const float elec_gap_low(1.4442), elec_gap_high(1.566);
  if(leptonOne.isElectron() && std::fabs(leptonOne.scEta) >= elec_gap_low && std::fabs(leptonOne.scEta) <= elec_gap_high) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.scEta) >= elec_gap_low && std::fabs(leptonTwo.scEta) <= elec_gap_high) return kTRUE;

  //enforce the leptons are separated
  if(std::fabs(Utilities::DeltaR(*leptonOne.p4, *leptonTwo.p4)) < 0.3) return kTRUE;

  //apply reasonable lepton isolation cuts
  if(leptonOne.isElectron() && leptonOne.relIso >= 0.5) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.relIso >= 0.5) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.relIso >= 0.5) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.relIso >= 0.5) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //7

  //FIXME: Decide on mass range to use, should low/high mass be included?
  const double mll = (*leptonOne.p4+*leptonTwo.p4).M();
  if(mll <= 51. || mll >= 170.) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //8

  if(!(leptonOne.fired || leptonTwo.fired)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //9

  //ensure reasonable dxy/dz cuts for electrons/muons
  const float max_dz(0.5), max_dxy(0.2);
  mumu &= std::fabs(leptonOne.dxy) < max_dxy;
  mumu &= std::fabs(leptonOne.dz ) < max_dz ;
  mumu &= std::fabs(leptonTwo.dxy) < max_dxy;
  mumu &= std::fabs(leptonTwo.dz ) < max_dz ;
  ee   &= std::fabs(leptonOne.dxy) < max_dxy;
  ee   &= std::fabs(leptonOne.dz ) < max_dz ;
  ee   &= std::fabs(leptonTwo.dxy) < max_dxy;
  ee   &= std::fabs(leptonTwo.dz ) < max_dz ;

  fCutFlow->Fill(icutflow); ++icutflow; //9
  if(!(mumu || ee)) return kTRUE;

  //remove additional leptons
  mumu &= nElectrons < 2;
  ee   &= nMuons     < 2;

  fCutFlow->Fill(icutflow); ++icutflow; //10
  if(!(mumu || ee)) return kTRUE;

  //remove anti-isolated lepton events
  mumu &= !isLooseMuon;
  ee   &= !isLooseElectron;

  fCutFlow->Fill(icutflow); ++icutflow; //11
  if(!(mumu || ee)) return kTRUE;

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //12
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //13
  ++icutflow;

  //cut-flow for fake leptons
  if(fIsData) fCutFlow->Fill(icutflow); //13
  else {
    if((std::abs(leptonOne.flavor) == std::abs(leptonOne.genFlavor))
       && (std::abs(leptonTwo.flavor) == std::abs(leptonTwo.genFlavor))) {
      fCutFlow->Fill(icutflow);
    }
  }
  ++icutflow;

  //remove MC jet -> light lepton contribution
  if(!fUseMCEstimatedFakeLep && !fIsData) {
    mumu   &= !isFakeMuon;
    ee     &= !isFakeElectron;
  }

  if(!(mumu || ee)) return kTRUE;

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  // const double met_cut         = 60.;
  // const double mtlep_cut       = 70.;

  // mumu  &= met < met_cut;
  // ee    &= met < met_cut;

  //Add W+Jets selection orthogonality condition
  // mumu  &= fTreeVars.mtlep < mtlep_cut;
  // ee    &= fTreeVars.mtlep < mtlep_cut;

  if(!(mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //16
  ++icutflow;

  ////////////////////////////////////////////////////////////
  // Set 7 + selection offset: preselection without b-jet cut
  ////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 7);

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  mumu  &= nBJetsUse == 0;
  ee    &= nBJetsUse == 0;

  if(isLooseElectron)               {fCutFlow->Fill(icutflow);} //17
  ++icutflow;
  if(isLooseElectron && chargeTest) {fCutFlow->Fill(icutflow);} //18
  ++icutflow;

  if(!looseQCDSelection && chargeTest)                                          {fCutFlow->Fill(icutflow);} //19
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight) > 0.)             {fCutFlow->Fill(icutflow);} //20
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight*eventWeight) > 0.) {fCutFlow->Fill(icutflow);} //21
  ++icutflow;

  ////////////////////////////////////////////////////////////
  // Set 8 + selection offset: preselection
  ////////////////////////////////////////////////////////////
  fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
  FillAllHistograms(set_offset + 8);
  IncrementTimer("SingleFill", true);

  return kTRUE;
}
