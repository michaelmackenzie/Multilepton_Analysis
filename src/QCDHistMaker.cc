#define QCDHISTMAKER_CXX

#include "interface/QCDHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
QCDHistMaker::QCDHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {
  fDoSystematics = 0;
  fWriteTrees = 0;
}

//--------------------------------------------------------------------------------------------------------------
void QCDHistMaker::Begin(TTree * /*tree*/)
{
  HistMaker::Begin(nullptr);
}


//--------------------------------------------------------------------------------------------------------------
void QCDHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i] = 0;
    fSysSets[i]   = 0;
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
    fEventSets [kEMu  + 70] = 1; // loose electron, tight muon
    fEventSets [kEMu  + 71] = 1; // loose electron, loose muon
  }
  if(mumu) {
    fEventSets [kMuMu + 8] = 1; // preselection
  }
  if(ee) {
    fEventSets [kEE   + 8] = 1; // preselection
  }
}

//--------------------------------------------------------------------------------------------------------------
void QCDHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void QCDHistMaker::BookEventHistograms() {
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
      // QCD SS-->OS specific histograms

      //QCD weight testing
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[1], "lepdeltaphi1"  , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);
      const double drbins[] = {0.  , 1.5 , 2.  , 2.5 , 2.8 ,
                               3.  , 3.2 , 3.4 , 3.6 , 3.8 ,
                               4.  , 4.3 , 4.6 , 4.9 ,
                               6.};
      const int ndrbins = (sizeof(drbins)/sizeof(*drbins) - 1);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[1]        , "lepdeltar1"        , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[2]        , "lepdeltar2"        , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelRVsOneEta[0]  , "lepdelrvsoneeta"   , Form("%s: LepDelRVsOneEta"  ,dirname),  5, 0., 2.5, ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelRVsOneEta[1]  , "lepdelrvsoneeta1"  , Form("%s: LepDelRVsOneEta"  ,dirname),  5, 0., 2.5, ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelPhiVsOneEta[0], "lepdelphivsoneeta" , Form("%s: LepDelPhiVsOneEta",dirname),  5, 0., 2.5, 10, 0., 3.5, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelPhiVsOneEta[1], "lepdelphivsoneeta1", Form("%s: LepDelPhiVsOneEta",dirname),  5, 0., 2.5, 10, 0., 3.5, folder);
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

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void QCDHistMaker::BookLepHistograms() {
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
void QCDHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("QCDHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
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
  // QCD SS-->OS specific histograms

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
  if(nJets20 == 0) {
    Hist->hQCDDelRJ[0] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hQCDOnePtVsTwoPtJ[0]->Fill(leptonOne.p4->Pt(), leptonTwo.p4->Pt(), wt_noqcdcl);
  } else if(nJets20 == 1) {
    Hist->hQCDDelRJ[1] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hQCDOnePtVsTwoPtJ[1]->Fill(leptonOne.p4->Pt(), leptonTwo.p4->Pt(), wt_noqcdcl);
  } else {
    Hist->hQCDDelRJ[2] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hQCDOnePtVsTwoPtJ[2]->Fill(leptonOne.p4->Pt(), leptonTwo.p4->Pt(), wt_noqcdcl);
  }
  Hist->hQCDOnePtVsTwoPtJ[3]->Fill(leptonOne.p4->Pt(), leptonTwo.p4->Pt(), wt_noqcdcl); //jet-inclusive correction
  Hist->hQCDOnePtVsTwoPtIso[0]->Fill(leptonOne.p4->Pt(), leptonTwo.p4->Pt(), wt_noqcd); //no QCD SS-->OS weights
  Hist->hQCDOnePtVsTwoPtIso[1]->Fill(leptonOne.p4->Pt(), leptonTwo.p4->Pt(), wt_noantiiso); //no muon anti-iso --> iso weight

}

//--------------------------------------------------------------------------------------------------------------
void QCDHistMaker::FillLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("QCDHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
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

  ////////////////////////////////////////////////
  // QCD SS-->OS histograms

}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t QCDHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  //object pT thresholds
  const float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  //ignore all but emu for now
  if(!emu) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //5

  if(leptonOne.isElectron() && leptonOne.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonOne.isTau     () && leptonOne.p4->Pt() <= tau_pt     ) return kTRUE;
  if(leptonTwo.isTau     () && leptonTwo.p4->Pt() <= tau_pt     ) return kTRUE;

  //leptons must satisfy the pt requirements and fire a trigger
  emu   &= ((leptonOne.p4->Pt() > electron_trig_pt_ && leptonOne.fired) ||
            (leptonTwo.p4->Pt() > muon_trig_pt_ && leptonTwo.fired));

  if(!emu) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //6

  //eta region cuts
  const double electron_eta_max = (fUseEmbedCuts) ? (emu) ? 2.4 : 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? (emu) ? 2.4 : 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.3 : 2.3; //tau eta doesn't change
  const double min_delta_r      = 0.3;

  if(leptonOne.isElectron() && std::fabs(leptonOne.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonOne.isMuon    () && std::fabs(leptonOne.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonTwo.isMuon    () && std::fabs(leptonTwo.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonOne.isTau     () && std::fabs(leptonOne.p4->Eta()) >= tau_eta_max     ) return kTRUE;
  if(leptonTwo.isTau     () && std::fabs(leptonTwo.p4->Eta()) >= tau_eta_max     ) return kTRUE;

  //reject electrons in the barrel/endcap gap region
  const float elec_gap_low(1.444), elec_gap_high(1.566);
  if(leptonOne.isElectron() && std::fabs(leptonOne.scEta) >= elec_gap_low && std::fabs(leptonOne.scEta) <= elec_gap_high) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.scEta) >= elec_gap_low && std::fabs(leptonTwo.scEta) <= elec_gap_high) return kTRUE;

  //enforce the leptons are separated
  if(std::fabs(Utilities::DeltaR(*leptonOne.p4, *leptonTwo.p4)) < min_delta_r) return kTRUE;

  //apply reasonable lepton isolation cuts
  if(leptonOne.isElectron() && leptonOne.relIso >= 0.5) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.relIso >= 0.5) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.relIso >= 0.5) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.relIso >= 0.5) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //7

  //FIXME: Confirm mass cut region needed
  const double mll = (*leptonOne.p4+*leptonTwo.p4).M();
  if(mll <= 50. || mll >= 170.) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //8

  if(!(leptonOne.fired || leptonTwo.fired)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //9

  //ensure reasonable dxy/dz cuts for electrons/muons
  const float max_dz(0.5), max_dxy(0.2);
  emu &= std::fabs(leptonOne.dxy) < max_dxy;
  emu &= std::fabs(leptonOne.dz ) < max_dz ;
  emu &= std::fabs(leptonTwo.dxy) < max_dxy;
  emu &= std::fabs(leptonTwo.dz ) < max_dz ;

  fCutFlow->Fill(icutflow); ++icutflow; //9
  if(!emu) return kTRUE;

  //remove additional leptons
  emu &= nElectrons == 1;
  emu &= nMuons     == 1;

  fCutFlow->Fill(icutflow); ++icutflow; //10
  if(!emu) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //11
  if(!emu) return kTRUE;

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //12
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //13
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

  //remove MC jet -> light lepton contribution
  if(!fUseMCEstimatedFakeLep && !fIsData) {
    emu   &= !isFakeMuon;
    emu   &= !isFakeElectron;
  }

  if(!emu) return kTRUE;

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  const float met_cut         = -1.f;//60.;
  //FIXME: Confirm mtlep cut needed
  const float mtlep_cut       = 70.f;

  emu   &= met_cut < 0.f || met < met_cut;
  emu   &= mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut;

  if(!emu) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //16
  ++icutflow;

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  emu   &= nBJetsUse == 0;

  if(isLooseElectron)               {fCutFlow->Fill(icutflow);} //17
  ++icutflow;
  if(isLooseElectron && chargeTest) {fCutFlow->Fill(icutflow);} //18
  ++icutflow;

  ////////////////////////////////////////////////////////////
  // QCD SS --> OS cuts and region definitions
  ////////////////////////////////////////////////////////////

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
  if(!emu) return kTRUE;

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
