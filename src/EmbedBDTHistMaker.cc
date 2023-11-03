#define EMBEDBDTHISTMAKER_CXX

#include "interface/EmbedBDTHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
EmbedBDTHistMaker::EmbedBDTHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {

}

//--------------------------------------------------------------------------------------------------------------
EmbedBDTHistMaker::~EmbedBDTHistMaker() {
  // HistMaker::~HistMaker();
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::DeleteHistograms() {
  HistMaker::DeleteHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  HistMaker::Begin(nullptr);

  //Initialize all relevant BDTs
  InitializeMVAs();

  //turn off Embedding BDT corrections
  fUseEmbBDTUncertainty = 0;

}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::InitializeMVAs() {
  if(fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig->names_.size(); ++mva_i) {
      if(mva[mva_i]) continue; //skip initialized MVAs
      //initialize all relevant BDTs
      const TString mva_selection = fMVAConfig->GetSelectionByIndex(mva_i);
      if(!fDoHiggs && mva_selection.BeginsWith("h")) continue;
      if(mva_selection.EndsWith("emu")) continue; //Z/H->e+mu don't use embedding
      if(mva_selection != "zmutau") continue; //FIXME: Starting by only testing 1 selection

      //Only use XGBoost in Z->e+mu category
      if(fUseXGBoostBDT && mva_selection == "zemu") {
        if(!fTrkQualInit) fTrkQualInit = new TrkQualInit(fTrkQualVersion, -1, fUseXGBoostBDT);
        mva        [mva_i] = nullptr;
        wrappedBDTs[mva_i] = new BDTWrapper(Form("weights/%s.2016_2017_2018.json", fMVAConfig->names_[mva_i].Data()), 1, fVerbose);
        if(wrappedBDTs[mva_i]->GetStatus()) throw 20;
        wrappedBDTs[mva_i]->InitializeVariables(fTrkQualInit->GetXGBoostVariables(mva_selection, fTreeVars));
      } else {
        //initialize the reader
        mva        [mva_i] = new TMVA::Reader("!Color:!Silent");
        wrappedBDTs[mva_i] = nullptr;

        Int_t isJetBinned = -1; // -1 is not binned, 0 = 0 jets, 1 = 1 jet, 2 = >1 jets
        fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling

        printf("Using selection %s\n", mva_selection.Data());
        //use a tool to initialize the MVA variables and spectators
        if(!fTrkQualInit) fTrkQualInit = new TrkQualInit(fTrkQualVersion, isJetBinned, fUseXGBoostBDT);
        fTrkQualInit->InitializeVariables(*(mva[mva_i]), mva_selection, fTreeVars);
        //Initialize MVA weight file
        const char* f = Form("weights/%s.2016_2017_2018.weights.xml",fMVAConfig->names_[mva_i].Data());
        mva[mva_i]->BookMVA(fMVAConfig->names_[mva_i].Data(),f);
        printf("Booked MVA %s with selection %s\n", fMVAConfig->names_[mva_i].Data(), mva_selection.Data());
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::FillAllHistograms(Int_t index) {
  HistMaker::FillAllHistograms(index);
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i] = 0;
    fSysSets  [i] = 0;
    fTreeSets [i] = 0;
    fSetFills [i] = 0;
  }

  //Event Sets
  const bool mumu  = fSelection == "" || fSelection == "mumu" ;
  const bool ee    = fSelection == "" || fSelection == "ee"   ;

  if(mumu) {
    fEventSets [kMuMu + 7] = 1;
    fEventSets [kMuMu + 8] = 1;
  }
  if(ee) {
    fEventSets [kEE   + 7] = 1;
    fEventSets [kEE   + 8] = 1;
  }
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::BookEventHistograms() {
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

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::BookPhotonHistograms() {
  HistMaker::BookPhotonHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i] = folder;
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;

      BookBaseLepHistograms(i, dirname);

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::BookSystematicHistograms() {
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::BookTrees() {
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("EmbedBDTHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
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
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  HistMaker::FillPhotonHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::FillLepHistogram(LepHist_t* Hist) {
  FillBaseLepHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void EmbedBDTHistMaker::FillSystematicHistogram(SystematicHist_t* Hist) {
  HistMaker::FillSystematicHistogram(Hist);
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t EmbedBDTHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  if(!(ee || mumu)) return kTRUE;

  //object pT thresholds
  const float muon_pt(10.f), electron_pt(15.f);

  if(leptonOne.isElectron()) one_pt_min_ = electron_pt;
  if(leptonTwo.isElectron()) two_pt_min_ = electron_pt;
  if(leptonOne.isMuon    ()) one_pt_min_ = muon_pt;
  if(leptonTwo.isMuon    ()) two_pt_min_ = muon_pt;

  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////


  //Ignore loose lepton selection in same flavor category for now
  ee   &= !isLooseElectron;
  mumu &= !isLooseMuon;

  ///////////////////////////////////////////
  // Re-define N(b-jets) if needed


  fCutFlow->Fill(icutflow); ++icutflow; //4

  ///////////////////////////////////////////////////////////////////
  //ensure the objects PDG IDs make sense

  mumu  &= leptonOne.isMuon    () && leptonTwo.isMuon    ();
  ee    &= leptonOne.isElectron() && leptonTwo.isElectron();

  if(!(mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //5


  ///////////////////////////////////////////////////////////////////
  //leptons must satisfy the pt requirements and fire a trigger

  mumu  &= (leptonOne.pt > muon_pt && leptonTwo.pt > muon_pt &&
            ((leptonOne.pt > muon_trig_pt_ && leptonOne.matched) ||
             (leptonTwo.pt > muon_trig_pt_ && leptonTwo.matched)));
  ee    &= (leptonOne.pt > electron_pt && leptonTwo.pt > electron_pt &&
            ((leptonOne.pt > electron_trig_pt_ && leptonOne.matched) ||
             (leptonTwo.pt > electron_trig_pt_ && leptonTwo.matched)));

  if(!(mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //6

  ///////////////////////////////////////////////////////////////////
  //don't allow multiple muons/electrons in mumu/ee selections

  mumu &= nMuons     == 2;
  ee   &= nElectrons == 2;

  if(!(mumu || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //7

  ///////////////////////////////////////////////////////////////////
  //reject electrons in the barrel/endcap gap region

  const float elec_gap_low(1.444), elec_gap_high(1.566);
  ee   &= std::fabs(leptonOne.scEta) < elec_gap_low || std::fabs(leptonOne.scEta) > elec_gap_high;
  ee   &= std::fabs(leptonTwo.scEta) < elec_gap_low || std::fabs(leptonTwo.scEta) > elec_gap_high;

  if(!(mumu  || ee)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //8

  ///////////////////////////////////////////////////////////////////
  //Apply eta region cuts

  const double electron_eta_max = (fUseEmbedCuts) ? 2.4 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.4 : 2.4;
  const double min_delta_r      = 0.3; //separation between leptons

  mumu  &= std::fabs(leptonOne.eta) < muon_eta_max;
  mumu  &= std::fabs(leptonTwo.eta) < muon_eta_max;
  mumu  &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  ee    &= std::fabs(leptonOne.eta) < electron_eta_max;
  ee    &= std::fabs(leptonTwo.eta) < electron_eta_max;
  ee    &= std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4)) > min_delta_r;

  //apply reasonable lepton isolation cuts
  const float max_rel_iso = 0.5;
  if(leptonOne.isElectron() && leptonOne.relIso >= max_rel_iso) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.relIso >= max_rel_iso) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.relIso >= max_rel_iso) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.relIso >= max_rel_iso) return kTRUE;

  ///////////////////////////////////////////////////////////////////
  //Apply di-lepton mass cuts

  const float mll = fTreeVars.lepm;
  min_mass_ =  40.f;
  max_mass_ = 170.f;

  mumu  &= mll > min_mass_ && mll < max_mass_;
  ee    &= mll > min_mass_ && mll < max_mass_;

  ///////////////////////////////////////////////////////////////////
  //ensure reasonable dxy/dz cuts for electrons/muons

  const float max_dz(0.5), max_dxy(0.2);
  mumu  &= std::fabs(leptonOne.dxy) < max_dxy;
  mumu  &= std::fabs(leptonOne.dz ) < max_dz ;
  mumu  &= std::fabs(leptonTwo.dxy) < max_dxy;
  mumu  &= std::fabs(leptonTwo.dz ) < max_dz ;
  ee    &= std::fabs(leptonOne.dxy) < max_dxy;
  ee    &= std::fabs(leptonOne.dz ) < max_dz ;
  ee    &= std::fabs(leptonTwo.dxy) < max_dxy;
  ee    &= std::fabs(leptonTwo.dz ) < max_dz ;

  if(!(mumu || ee)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //9


  fCutFlow->Fill(icutflow); ++icutflow; //10

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //11
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //12
  ++icutflow;

  //cut-flow for fake leptons
  if(fIsData) fCutFlow->Fill(icutflow); //13
  else{
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

  const double met_cut         = -1.;
  const double mtlep_cut       = -1.;
  mtlep_max_ = mtlep_cut;
  met_max_   = met_cut;

  if(!(mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //14
  ++icutflow;

  ///////////////////////////////////////////////////////////////////
  //remove MC jet -> light lepton contribution

  if(!fUseMCEstimatedFakeLep && !fIsData && !fIsSignal) { //keep signal as even events with fakes are signal from the MC
    mumu  &= !isFakeMuon;
    ee    &= !isFakeElectron;
  }

  if(!(mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //15
  ++icutflow;

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  mumu  &= met_cut < 0.f || met < met_cut;
  ee    &= met_cut < 0.f || met < met_cut;

  mumu  &= mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut;
  ee    &= mtlep_cut < 0.f || fTreeVars.mtlep < mtlep_cut;

  if(!(mumu || ee)) return kTRUE;

  //Remove loose ID + same-sign events
  mumu    &= !looseQCDSelection || chargeTest;
  ee      &= !looseQCDSelection || chargeTest;

  if(!(mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 7 + selection offset: No MC estimated fake taus, no b-jet cut
  ////////////////////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 7);


  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //16
  ++icutflow;

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  mumu  &= nBJetsUse == 0;
  ee    &= nBJetsUse == 0;

  if(!(mumu || ee)) return kTRUE;


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

  ///////////////////////////////////////////////////////////////////////////
  // Add additional selection for leptonic tau channels


  //Z/H->e+mu only
  if(fSameFlavorEMuSelec) {one_pt_min_ = 20.f; two_pt_min_ = 20.f; min_mass_ = 70.f; max_mass_ = 110.f;}

  if(fSameFlavorEMuSelec) {
    mumu    &= std::fabs(leptonTwo.dxySig) < 3.0;
    mumu    &= std::fabs(leptonOne.dxySig) < 3.0;
    mumu    &= std::fabs(leptonTwo.dzSig ) < 4.7;
    mumu    &= std::fabs(leptonOne.dzSig ) < 4.7;
    ee      &= std::fabs(leptonTwo.dxySig) < 3.0;
    ee      &= std::fabs(leptonOne.dxySig) < 3.0;
    ee      &= std::fabs(leptonTwo.dzSig ) < 4.7;
    ee      &= std::fabs(leptonOne.dzSig ) < 4.7;
    ee      &= leptonOne.pt > one_pt_min_;
    ee      &= leptonTwo.pt > two_pt_min_;
    ee      &= mll > min_mass_ && mll < max_mass_;
    mumu    &= leptonOne.pt > one_pt_min_;
    mumu    &= leptonTwo.pt > two_pt_min_;
    mumu    &= mll > min_mass_ && mll < max_mass_;
  }

  if(!(mumu || ee)) return kTRUE;

  fCutFlow->Fill(icutflow); //22
  ++icutflow;
  if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //23
  ++icutflow;

  fCutFlow->Fill(icutflow); //24
  ++icutflow;
  if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //25
  ++icutflow;

  if(PassesCuts()) {
    if(!looseQCDSelection && chargeTest) fCutFlow->Fill(icutflow); //26
    ++icutflow;
    if(looseQCDSelection || !chargeTest) fCutFlow->Fill(icutflow); //27
    ++icutflow;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  if(mll > min_mass_) {
    fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
    FillAllHistograms(set_offset + 8);
    IncrementTimer("SingleFill", true);
  }

  return kTRUE;
}
