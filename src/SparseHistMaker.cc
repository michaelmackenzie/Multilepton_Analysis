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
  }
  if(etau) {
    fEventSets [kETau + 1] = 1; // all events
  }
  if(emu) {
    fEventSets [kEMu  + 1] = 1; // all events
    //Leptonic tau channels
    fEventSets[kMuTauE + 1] = 1;
    fEventSets[kETauMu + 1] = 1;
  }
  if(mumu) {
    fEventSets [kMuMu + 1] = 1; // all events
  }
  if(ee) {
    fEventSets [kEE   + 1] = 1; // all events
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
  if(!fChain) {
    printf("HistMaker::%s: Error! TChain not found\n", __func__);
    throw 1;
  }
  fentry = entry;
  fChain->GetEntry(entry,0);
  if(fVerbose > 0 || entry%50000 == 0) std::cout << Form("Processing event: %12lld (%5.1f%%)\n", entry, entry*100./fChain->GetEntriesFast());
  int icutflow = 0;
  fCutFlow->Fill(icutflow); ++icutflow; //0

  //Initialize base object information
  CountObjects();

  //DY Splitting
  if(fDYType > 0) {
    // 1 = tau, 2 = muon or electron channel
    if(nGenHardTaus == 0) { //Z->ee/mumu
      if(fDYType == 1) return kTRUE;
    } else if(nGenHardTaus == 2) { //Z->tautau
      if(fDYType == 2) return kTRUE;
    } else {
      std::cout << "Warning! Unable to identify type of DY event!" << std::endl
           << "nGenHardTaus = " << nGenHardTaus << std::endl
           << "nGenHardMuons = " << nGenHardMuons << std::endl
           << "nGenHardElectrons = " << nGenHardElectrons << std::endl
           << "fDYType = " << fDYType << std::endl
           << "Entry " << fentry << std::endl;
      return kTRUE;
    }
  }

  fCutFlow->Fill(icutflow); ++icutflow; //1
  //split W+Jets into N(LHE jets) generated for binned sample combination
  if(fWNJets > -1 && LHE_Njets != fWNJets) {
    return kTRUE;
  }

  //If running embedding, reject di-tau production from non-embedding MC (except tau-tau DY MC, which is already separated by histogram files)
  if(fDYType != 1 && fUseEmbedCuts && !fIsEmbed && !fIsData && nGenTaus == 2) {
    return kTRUE;
  }

  fCutFlow->Fill(icutflow); ++icutflow; //2

  /////////////////////////
  // Apply event weights //
  /////////////////////////

  InitializeEventWeights();
  if(eventWeight < 0. || !std::isfinite(eventWeight*genWeight)) {
    std::cout << "WARNING! Skipping event " << fentry << ", as it has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << genWeight << "; puWeight = " << puWeight
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOneTrigWeight*leptonTwoTrigWeight
              << "; jetPUIDWeight = " << jetPUIDWeight << std::endl;
    return kTRUE;
  }

  ////////////////////////
  // Data overlap check //
  ////////////////////////

  //skip if it's data and lepton status doesn't match data set ( 1 = electron 2 = muon) unless allowing overlap and it passes both
  if(fIsData > 0) {
    int pdgid = (fIsData == 1) ? 11 : 13; //pdg ID for the data stream
    //if no selected lepton fired this trigger, continue
    if(!((std::abs(leptonOneFlavor) == pdgid && leptonOneFired) || (std::abs(leptonTwoFlavor) == pdgid && leptonTwoFired)))
      return kTRUE;

    if(triggerLeptonStatus != ((UInt_t) fIsData)) { //trigger doesn't match data stream
      if(triggerLeptonStatus != 3) return kTRUE; //triggered only for the other stream
      if(fSkipDoubleTrigger) { //don't allow double triggers
        int other_pdgid = (fIsData == 1) ? 13 : 11; //pdg ID for the other data stream
        //only skip if the selected lepton actually fired the trigger
        if((std::abs(leptonOneFlavor) == other_pdgid && leptonOneFired) ||(std::abs(leptonTwoFlavor) == other_pdgid && leptonTwoFired)) return kTRUE;
      }
    }
  }

  fCutFlow->Fill(icutflow); ++icutflow; //3

  //Print debug info
  if(fVerbose > 0 ) {
    std::cout << " lep_1: pdg_id = " << leptonOneFlavor << " p4 = "; leptonOneP4->Print();
    std::cout << " lep_2: pdg_id = " << leptonTwoFlavor << " p4 = "; leptonTwoP4->Print();
  }

  //Initialize systematic variation weights
  InitializeSystematics();


  //selections
  //use the tree name to choose the selection
  bool mutau =                    nMuons == 1 && nTaus == 1 && (fSelection == "" || fSelection == "mutau");
  bool etau  = nElectrons == 1 &&                nTaus == 1 && (fSelection == "" || fSelection == "etau" );
  bool emu   = nElectrons == 1 && nMuons == 1               && (fSelection == "" || fSelection == "emu"  );
  bool mumu  = nElectrons <  2 && nMuons == 2               && (fSelection == "" || fSelection == "mumu" );
  bool ee    = nElectrons == 2 && nMuons <  2               && (fSelection == "" || fSelection == "ee"   );


  //reject overlaps
  if(mutau && etau) {mutau = false; etau = false;}
  if(emu && (mutau || etau)) {mutau = false; etau = false;}
  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl
                             << " eventWeight*genWeight = " << eventWeight*genWeight << std::endl;

  if(!(mutau || etau || emu || mumu || ee)) {
    std::cout << "WARNING! Entry " << entry << " passes no selections! N(e) = " << nElectron
              << " N(mu) = " << nMuon << " N(tau) = " << nTau << std::endl;
    return kTRUE;
  }

  fCutFlow->Fill(icutflow); ++icutflow; //4

  if((mutau + etau + emu + mumu + ee) > 1)
    std::cout << "WARNING! Entry " << entry << " passes multiple selections!\n";

  isFakeElectron  = !fIsData && ((std::abs(leptonOneFlavor) == 11 && leptonOneGenFlavor == 26) ||
                                 (std::abs(leptonTwoFlavor) == 11 && leptonTwoGenFlavor == 26));
  isFakeMuon      = !fIsData && ((std::abs(leptonOneFlavor) == 13 && leptonOneGenFlavor == 26) ||
                                 (std::abs(leptonTwoFlavor) == 13 && leptonTwoGenFlavor == 26));

  isLooseElectron = (ee || emu || etau) && leptonOneIso / leptonOneP4->Pt() >= 0.15;
  isLooseMuon     = (mutau || mumu)     && leptonOneIso / leptonOneP4->Pt() >= 0.15;
  isLooseMuon    |= (emu   || mumu)     && leptonTwoIso / leptonTwoP4->Pt() >= 0.15;
  isLooseTau      = (etau  || mutau) && tauDeepAntiJet < 50;
  looseQCDSelection = isLooseElectron || isLooseMuon || isLooseTau;


  //////////////////////////////////////////////////////////////
  // Apply or remove scale factors
  //////////////////////////////////////////////////////////////

  //No weights in data
  if(fIsData) {
    eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.;
    jetPUIDWeight = 1.; btagWeight = 1.; embeddingWeight = 1.; embeddingUnfoldingWeight = 1.;
  }


  InitializeTreeVariables();

  //////////////////////////////////////////////////////////////
  // Check if anti-iso/same-sign lepton category
  //////////////////////////////////////////////////////////////


  const bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;

  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl;

  if(!std::isfinite(eventWeight) || !std::isfinite(genWeight)) {
    std::cout << __func__ << ": Warning!!! " << fentry << " point 1: Event weight not defined (" << eventWeight*genWeight << "), setting to 0. Event weights:\n"
              << " btag = " << btagWeight << "; embedding = " << embeddingWeight << "; embed_unfold = " << embeddingUnfoldingWeight
              << "; genWeight = " << genWeight << "; zPtWeight = " << zPtWeight << "; jetPUIDWt = " << jetPUIDWeight
              << "; trig_wt = " << leptonOneTrigWeight*leptonTwoTrigWeight << "; lep1_wt = " << leptonOneWeight1*leptonOneWeight2
              << "; lep2_wt = " << leptonTwoWeight1*leptonTwoWeight2
              << std::endl;
    eventWeight = 0.;
    genWeight = 1.;
  }

  ////////////////////////////////////////////////////////////
  // Define the selection set for typical histogramming
  ////////////////////////////////////////////////////////////
  int set_offset = 0;
  if(mutau)     set_offset = kMuTau;
  else if(etau) set_offset = kETau;
  else if(emu ) set_offset = kEMu;
  else if(mumu) set_offset = kMuMu;
  else if(ee  ) set_offset = kEE;

  if(!chargeTest) set_offset += fQcdOffset;
  if(looseQCDSelection) set_offset += fMisIDOffset;

  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: base selection
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  FillAllHistograms(set_offset + 1);

  fCutFlow->Fill(icutflow); ++icutflow; //5
  return kTRUE;
}
