#define ZTauTauHistMaker_cxx
// The class definition in ZTauTauHistMaker.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("ZTauTauHistMaker.C")
// root> T->Process("ZTauTauHistMaker.C","some options")
// root> T->Process("ZTauTauHistMaker.C+")
//


#include "ZTauTauHistMaker.hh"
#include <TStyle.h>

void ZTauTauHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();
  printf("ZTauTauHistMaker::Begin\n");
  timer->Start();
  fChain = 0;
}

void ZTauTauHistMaker::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("ZTauTauHistMaker::SlaveBegin\n");
  TString option = GetOption();

}

void ZTauTauHistMaker::FillAllHistograms(Int_t index) {
  if(fEventSets [index]) {
    FillEventHistogram( fEventHist [index]);
    FillPhotonHistogram(fPhotonHist[index]);
    FillLepHistogram(   fLepHist   [index]);
  }
}

void ZTauTauHistMaker::BookHistograms() {
  BookEventHistograms();
  BookPhotonHistograms();
  BookLepHistograms();
}

void ZTauTauHistMaker::BookEventHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"event_%i",i);
      fDirectories[0*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i]->cd();
      fEventHist[i] = new EventHist_t;
      fEventHist[i]->hLumiSection            = new TH1F("lumiSection"         , Form("%s: LumiSection"         ,dirname)  , 200,    0, 4e6);
      fEventHist[i]->hTriggerStatus          = new TH1F("triggerStatus"       , Form("%s: TriggerStatus"       ,dirname)  ,   3, -1.5, 1.5);
      fEventHist[i]->hEventWeight            = new TH1F("eventweight"         , Form("%s: EventWeight"         ,dirname)  , 100,   -5,   5);     
      fEventHist[i]->hGenWeight              = new TH1F("genweight"           , Form("%s: GenWeight"           ,dirname)  ,   5, -2.5, 2.5);     
      fEventHist[i]->hNPV                    = new TH1F("npv"                 , Form("%s: NPV"                 ,dirname)  , 200,  0, 200); 
      fEventHist[i]->hNPU                    = new TH1F("npu"                 , Form("%s: NPU"                 ,dirname)  , 100,  0, 100); 
      fEventHist[i]->hNPartons               = new TH1F("npartons"            , Form("%s: NPartons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hMet                    = new TH1F("met"                 , Form("%s: Met"                 ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hMetPhi                 = new TH1F("metphi"              , Form("%s: MetPhi"              ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hNMuons                 = new TH1F("nmuons"              , Form("%s: NMuons"              ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNElectrons             = new TH1F("nelectrons"          , Form("%s: NElectrons"          ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNTaus                  = new TH1F("ntaus"               , Form("%s: NTaus"               ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNPhotons               = new TH1F("nphotons"            , Form("%s: NPhotons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausHad            = new TH1F("ngentaushad"         , Form("%s: NGenTausHad"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausLep            = new TH1F("ngentauslep"         , Form("%s: NGenTausLep"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNJets                  = new TH1F("njets"               , Form("%s: NJets"               ,dirname)  ,  50,  0,  50); 
      fEventHist[i]->hNFwdJets               = new TH1F("nfwdjets"            , Form("%s: NFwdJets"            ,dirname)  ,  50,  0,  50); 
      fEventHist[i]->hNBJets                 = new TH1F("nbjets"              , Form("%s: NBJets"              ,dirname)  ,  50,  0,  50);
      fEventHist[i]->hMcEra                  = new TH1F("mcEra"               , Form("%s: McEra"               ,dirname) ,   5,   0,  5);
      fEventHist[i]->hTriggerLeptonStatus    = new TH1F("triggerLeptonStatus" , Form("%s: TriggerLeptonStatus" ,dirname) ,   5,   0,  5);
      fEventHist[i]->hPuWeight		     = new TH1F("puWeight"	      , Form("%s: PuWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTopPtWeight	     = new TH1F("topPtWeight"	      , Form("%s: TopPtWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTauDecayMode	     = new TH1F("tauDecayMode"	      , Form("%s: TauDecayMode"	       ,dirname) ,  11,   0, 11);
      fEventHist[i]->hTauMVA		     = new TH1F("tauMVA"              , Form("%s: TauMVA"              ,dirname) , 100,   0,  1);
      fEventHist[i]->hTauGenFlavor	     = new TH1F("tauGenFlavor"	      , Form("%s: TauGenFlavor"	       ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauGenFlavorHad	     = new TH1F("tauGenFlavorHad"     , Form("%s: TauGenFlavorHad"     ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauVetoedJetPt	     = new TH1F("tauVetoedJetPt"      , Form("%s: TauVetoedJetPt"      ,dirname) , 210, -10,200);
      fEventHist[i]->hTauVetoedJetPtUnc	     = new TH1F("tauVetoedJetPtUnc"   , Form("%s: TauVetoedJetPtUnc"   ,dirname) , 110,  -1, 10);
      fEventHist[i]->hHtSum		     = new TH1F("htSum"		      , Form("%s: HtSum"	       ,dirname) , 200,   0,800);
      fEventHist[i]->hHt		     = new TH1F("ht"		      , Form("%s: Ht"		       ,dirname) , 200,   0,800);
      fEventHist[i]->hHtPhi                  = new TH1F("htPhi"               , Form("%s: HtPhi"               ,dirname) , 100,  -4,  4);

							       
      fEventHist[i]->hLepPt         = new TH1F("leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepP          = new TH1F("lepp"          , Form("%s: Lepton P"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepE          = new TH1F("lepe"          , Form("%s: Lepton E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepM          = new TH1F("lepm"          , Form("%s: Lepton M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepEta        = new TH1F("lepeta"        , Form("%s: Lepton Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepPhi        = new TH1F("lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepDeltaPhi   = new TH1F("lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname)  ,  40,   0,   4);
      fEventHist[i]->hLepDeltaEta   = new TH1F("lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDeltaR     = new TH1F("lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDelRVsPhi  = new TH2F("lepdelrvsphi"  , Form("%s: LepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hLepPtOverM    = new TH1F("lepptoverm"    , Form("%s: Lepton Pt / M"  ,dirname)  , 100,   0,  20);

      fEventHist[i]->hSysM          = new TH1F("sysm"          , Form("%s: SysM"          ,dirname)  ,1000,  0, 1e3);     
      fEventHist[i]->hSysPt         = new TH1F("syspt"         , Form("%s: SysPt"         ,dirname)  , 200,  0, 400);     
      fEventHist[i]->hSysEta        = new TH1F("syseta"        , Form("%s: SysEta"        ,dirname)  , 100, -5,   5);     

      fEventHist[i]->hMTMu          = new TH1F("mtmu"          , Form("%s: MT_Mu"         ,dirname)  , 100, 0.,   100.);     
      fEventHist[i]->hMTE           = new TH1F("mte"           , Form("%s: MT_E"          ,dirname)  , 100, 0.,   100.);     
      fEventHist[i]->hMTTau         = new TH1F("mttau"         , Form("%s: MT_Tau"        ,dirname)  , 100, 0.,   100.);     

      fEventHist[i]->hMDiff         = new TH1F("mdiff"         , Form("%s: 0.85M_gll - M_ll" ,dirname)  , 100, -50.,   50.);     

      fEventHist[i]->hBisectorPhi   = new TH1F("bisectorphi"   , Form("%s: Bisector Phi"    ,dirname)  , 100,  -4.,     4.);      
      fEventHist[i]->hPXiVis        = new TH1F("pxivis"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv        = new TH1F("pxiinv"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv = new TH1F("pxivisoverinv" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis   = new TH2F("pxiinvvsvis"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff       = new TH1F("pxidiff"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2      = new TH1F("pxidiff2"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     

      fEventHist[i]->hPtSum         = new TH1F("ptsum"         , Form("%s: Scalar Pt sum" ,dirname)    ,1000,  0.,  1000.);     
      
    }
  }
}

void ZTauTauHistMaker::BookPhotonHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"photon_%i",i);
      fDirectories[1*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[1*fn + i]->cd();
      fPhotonHist[i] = new PhotonHist_t;
      fPhotonHist[i]->hPx        = new TH1F("px"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPy        = new TH1F("py"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPz        = new TH1F("pz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPt        = new TH1F("pt"      , Form("%s: Pt"      ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hP         = new TH1F("p"       , Form("%s: P"       ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hE         = new TH1F("e"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fPhotonHist[i]->hEta       = new TH1F("eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fPhotonHist[i]->hPhi       = new TH1F("phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fPhotonHist[i]->hIso       = new TH1F("iso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fPhotonHist[i]->hRelIso    = new TH1F("reliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fPhotonHist[i]->hTrigger   = new TH1F("trigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
    }
  }
}

void ZTauTauHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      fDirectories[2*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;
      fLepHist[i]->hOnePx        = new TH1F("onepx"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePy        = new TH1F("onepy"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePz        = new TH1F("onepz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePt        = new TH1F("onept"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneP         = new TH1F("onep"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneE         = new TH1F("onee"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneM         = new TH1F("onem"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOneEta       = new TH1F("oneeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOnePhi       = new TH1F("onephi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hOneIso       = new TH1F("oneiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fLepHist[i]->hOneRelIso    = new TH1F("onereliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hOneFlavor    = new TH1F("oneflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hOneQ         = new TH1F("oneq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hOneTrigger   = new TH1F("onetrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      fLepHist[i]->hTwoPx        = new TH1F("twopx"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPy        = new TH1F("twopy"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPz        = new TH1F("twopz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPt        = new TH1F("twopt"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoP         = new TH1F("twop"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoE         = new TH1F("twoe"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoM         = new TH1F("twom"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoEta       = new TH1F("twoeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoPhi       = new TH1F("twophi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hTwoIso       = new TH1F("twoiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fLepHist[i]->hTwoRelIso    = new TH1F("tworeliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hTwoFlavor    = new TH1F("twoflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hTwoQ         = new TH1F("twoq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hTwoTrigger   = new TH1F("twotrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);

    }
  }
}


void ZTauTauHistMaker::FillEventHistogram(EventHist_t* Hist) {
  Hist->hLumiSection         ->Fill(lumiSection        , genWeight*eventWeight)      ;
  // Hist->hTriggerStatus       ->Fill(triggerStatus      , genWeight*eventWeight)      ;
  Hist->hEventWeight         ->Fill(eventWeight        );
  Hist->hGenWeight           ->Fill(genWeight          );
  Hist->hNPV                 ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hNPU                 ->Fill(nPU                , genWeight*eventWeight)      ;
  // Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
  Hist->hMet                 ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMetPhi              ->Fill(metPhi             , genWeight*eventWeight)      ;
  Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
  Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
  Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  Hist->hNGenTausHad         ->Fill(nGenTausHad        , genWeight*eventWeight)      ;
  Hist->hNGenTausLep         ->Fill(nGenTausLep        , genWeight*eventWeight)      ;
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  // Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets              ->Fill(nBJets             , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra              , genWeight*eventWeight)   ;
  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hPuWeight	     ->Fill(puWeight	       , genWeight*eventWeight)   ;
  Hist->hTopPtWeight	     ->Fill(topPtWeight	       , genWeight*eventWeight)   ;
  Hist->hTauDecayMode	     ->Fill(tauDecayMode       , genWeight*eventWeight)   ;
  Hist->hTauMVA		     ->Fill(tauMVA	       , genWeight*eventWeight)   ;
  Hist->hTauGenFlavor	     ->Fill(tauGenFlavor       , genWeight*eventWeight)   ;
  Hist->hTauGenFlavorHad     ->Fill(tauGenFlavorHad    , genWeight*eventWeight)   ;
  Hist->hTauVetoedJetPt	     ->Fill(tauVetoedJetPt     , genWeight*eventWeight)   ;
  Hist->hTauVetoedJetPtUnc   ->Fill(tauVetoedJetPtUnc  , genWeight*eventWeight)   ;
  Hist->hHtSum		     ->Fill(htSum	       , genWeight*eventWeight)   ;
  Hist->hHt		     ->Fill(ht		       , genWeight*eventWeight)   ;
  Hist->hHtPhi               ->Fill(htPhi              , genWeight*eventWeight)   ;


  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  TLorentzVector sys    = (*photonP4) + lepSys;
  
  float lepDelR   = leptonOneP4->DeltaR(*leptonTwoP4);
  float lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  float lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  
  Hist->hLepPt        ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepP         ->Fill(lepSys.P()             ,eventWeight*genWeight);
  Hist->hLepE         ->Fill(lepSys.E()             ,eventWeight*genWeight);
  Hist->hLepM         ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
						    		
  Hist->hLepDeltaPhi  ->Fill(lepDelPhi              ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR    ->Fill(lepDelR                ,eventWeight*genWeight);
  Hist->hLepDelRVsPhi ->Fill(lepDelR , lepDelPhi    ,eventWeight*genWeight);
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);

  		                
  Hist->hSysM          ->Fill(sys.M()      ,eventWeight*genWeight);
  Hist->hSysPt         ->Fill(sys.Pt()     ,eventWeight*genWeight);
  Hist->hSysEta        ->Fill(sys.Eta()    ,eventWeight*genWeight);

  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  if(abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
  if(abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
  if(abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;

  double mTMu = (muon != 0) ? 2.*met*muon->Pt() : 0.;
  mTMu = (muon != 0) ? sqrt(mTMu*(1.-cos(muon->Phi() - metPhi))) : 0.;
  double mTE = (electron != 0) ? 2.*met*electron->Pt() : 0.;
  mTE  = (electron != 0) ? sqrt(mTE*(1.-cos(electron->Phi() - metPhi))) : 0.;
  double mTTau = (tau != 0) ? 2.*met*tau->Pt() : 0.;
  mTTau  = (tau != 0) ? sqrt(mTTau*(1.-cos(tau->Phi() - metPhi))) : 0.;

  Hist->hMTMu          ->Fill(mTMu       ,eventWeight*genWeight);
  Hist->hMTE           ->Fill(mTE        ,eventWeight*genWeight);
  Hist->hMTTau         ->Fill(mTTau      ,eventWeight*genWeight);

  Hist->hMDiff         ->Fill(0.85*sys.M() - lepSys.M(),eventWeight*genWeight);

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  bisector.SetMag(1.);
  double pxi_vis = (lp1+lp2)*bisector;
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  double pxi_inv = missing*bisector;
  Hist->hBisectorPhi->Fill(bisector.Phi()  ,eventWeight*genWeight);
  Hist->hPXiVis     ->Fill(pxi_vis         ,eventWeight*genWeight);
  Hist->hPXiInv     ->Fill(pxi_inv         ,eventWeight*genWeight);
  Hist->hPXiVisOverInv->Fill((pxi_inv != 0.) ? pxi_vis/pxi_inv : 1.e6 ,eventWeight*genWeight);
  Hist->hPXiInvVsVis->Fill(pxi_vis, pxi_inv,eventWeight*genWeight);
  Hist->hPXiDiff    ->Fill(pxi_vis-pxi_inv ,eventWeight*genWeight);
  double coeff = 0.6;
  double offset = -40.;
  Hist->hPXiDiff2   ->Fill(pxi_inv - (coeff*pxi_vis+offset) ,eventWeight*genWeight);

  Hist->hPtSum      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+met ,eventWeight*genWeight);
}

void ZTauTauHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  Hist->hPx  ->Fill(photonP4->Px() , eventWeight*genWeight );
  Hist->hPy  ->Fill(photonP4->Py() , eventWeight*genWeight );
  Hist->hPz  ->Fill(photonP4->Pz() , eventWeight*genWeight );
  Hist->hPt  ->Fill(photonP4->Pt() , eventWeight*genWeight );
  Hist->hP   ->Fill(photonP4->P()  , eventWeight*genWeight );
  Hist->hE   ->Fill(photonP4->E()  , eventWeight*genWeight );
  Hist->hEta ->Fill(photonP4->Eta(), eventWeight*genWeight );
  Hist->hPhi ->Fill(photonP4->Phi(), eventWeight*genWeight );
  // Hist->hIso       ;
  // Hist->hRelIso    ;
  // Hist->hTrigger   ;
}

void ZTauTauHistMaker::FillLepHistogram(LepHist_t* Hist) {
  Hist->hOnePx        ->Fill(leptonOneP4->Px()      ,eventWeight*genWeight);
  Hist->hOnePy        ->Fill(leptonOneP4->Py()      ,eventWeight*genWeight);
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()      ,eventWeight*genWeight);
  Hist->hOnePt        ->Fill(leptonOneP4->Pt()      ,eventWeight*genWeight);
  Hist->hOneP         ->Fill(leptonOneP4->P()       ,eventWeight*genWeight);
  Hist->hOneE         ->Fill(leptonOneP4->E()       ,eventWeight*genWeight);
  Hist->hOneM         ->Fill(leptonOneP4->M()       ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()     ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOneP4->Phi()     ,eventWeight*genWeight);
  // Hist->hOneIso       ->Fill(leptonOneIso           ,eventWeight*genWeight);
  // float relIso1 = leptonOneIso/ leptonOneP4->Pt();
  // Hist->hOneRelIso    ->Fill(relIso1                ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(fabs(leptonOneFlavor)        ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  // Hist->hOneTrigger   ->Fill(leptonOneTrigger       ,eventWeight*genWeight);
  
  Hist->hTwoPx        ->Fill(leptonTwoP4->Px()      ,eventWeight*genWeight);
  Hist->hTwoPy        ->Fill(leptonTwoP4->Py()      ,eventWeight*genWeight);
  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()      ,eventWeight*genWeight);
  Hist->hTwoPt        ->Fill(leptonTwoP4->Pt()      ,eventWeight*genWeight);
  Hist->hTwoP         ->Fill(leptonTwoP4->P()       ,eventWeight*genWeight);
  Hist->hTwoE         ->Fill(leptonTwoP4->E()       ,eventWeight*genWeight);
  Hist->hTwoM         ->Fill(leptonTwoP4->M()       ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()     ,eventWeight*genWeight);
  Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()     ,eventWeight*genWeight);
  // Hist->hTwoIso       ->Fill(leptonTwoIso           ,eventWeight*genWeight);
  // float relIso2 = leptonTwoIso/ leptonTwoP4->Pt();
  // Hist->hTwoRelIso    ->Fill(relIso2                ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(fabs(leptonTwoFlavor)        ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwoFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  // Hist->hTwoTrigger   ->Fill(leptonTwoTrigger       ,eventWeight*genWeight);

}


Bool_t ZTauTauHistMaker::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // When processing keyed objects with PROOF, the object is already loaded
  // and is available via the fObject pointer.
  //
  // This function should contain the \"body\" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.

  fChain->GetEntry(entry);
  if(entry%50000 == 0) printf("Processing event: %12lld\n", entry);
  bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;
  FillAllHistograms(0);

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  bisector.SetMag(1.);
  double pxi_vis = (lp1+lp2)*bisector;
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  double pxi_inv = missing*bisector;
  
  if(chargeTest) FillAllHistograms(1);
  else           FillAllHistograms(1 + fQcdOffset);

  if(nPhotons > 0  && chargeTest) FillAllHistograms(2);
  else if(nPhotons > 0)           FillAllHistograms(2 + fQcdOffset);
  if(nPhotons == 1 && chargeTest) FillAllHistograms(3);
  else if(nPhotons == 1)          FillAllHistograms(3 + fQcdOffset);

  bool mutau = nTaus == 1  && nMuons == 1;
  bool etau  = nTaus == 1  && nElectrons == 1;
  bool emu   = nMuons == 1 && nElectrons == 1;
  
  if(mutau && chargeTest) FillAllHistograms(5);
  else if(mutau)          FillAllHistograms(5 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(15);
  else if(etau)           FillAllHistograms(15 + fQcdOffset);
  if(emu   && chargeTest) FillAllHistograms(25);
  else if(emu)            FillAllHistograms(25 + fQcdOffset);
  
  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  if(abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
  if(abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
  if(abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;

  mutau = mutau && (tau != 0) && (muon != 0);
  etau  = etau  && (tau != 0) && (electron != 0);
  emu   = emu   && (muon != 0) && (electron != 0);

  mutau = mutau && muon->Pt() > 25. && tau->Pt() > 20.;
  etau  = etau  && electron->Pt() > 30. && tau->Pt() > 20.;

  if(mutau && chargeTest) FillAllHistograms(6);
  else if(mutau)          FillAllHistograms(6 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(16);
  else if(etau)           FillAllHistograms(16 + fQcdOffset);
  
  mutau = mutau && abs(muon->Eta()) < 2.4;
  mutau = mutau && abs(tau->Eta()) < 2.3;
  mutau = mutau && abs(tau->DeltaR(*muon)) > 0.3;

  etau  = etau  && abs(electron->Eta()) < 2.5;
  etau  = etau  && abs(tau->Eta()) < 2.3;
  etau  = etau  && abs(tau->DeltaR(*electron)) > 0.3;

  if(mutau && chargeTest) FillAllHistograms(7);
  else if(mutau)          FillAllHistograms(7 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(17);
  else if(etau)           FillAllHistograms(17 + fQcdOffset);

  //adding visible/invisible pT cuts
  double offset = -40.;
  double coeff = 0.6;
  double visShift = coeff*pxi_vis + offset;
  if(mutau && pxi_inv > visShift && chargeTest) FillAllHistograms(11);
  else if(mutau && pxi_inv > visShift)          FillAllHistograms(11 + fQcdOffset);
  if(etau && pxi_inv > visShift && chargeTest)  FillAllHistograms(21);
  else if(etau && pxi_inv > visShift)           FillAllHistograms(21 + fQcdOffset);

  offset = -20.;
  visShift = coeff*pxi_vis + offset;
  if(mutau && pxi_inv > visShift && chargeTest) FillAllHistograms(12);
  else if(mutau && pxi_inv > visShift)          FillAllHistograms(12 + fQcdOffset);
  if(etau && pxi_inv > visShift && chargeTest)  FillAllHistograms(22);
  else if(etau && pxi_inv > visShift)           FillAllHistograms(22 + fQcdOffset);
  
  double mll = (*leptonOneP4+*leptonTwoP4).M();
  double mgll = (*photonP4 + (*leptonOneP4+*leptonTwoP4)).M();
  mutau = mutau && mll < 100. && mll > 40.;
  etau  = etau  && mll < 100. && mll > 40.;

  double mTMu = (muon != 0) ? 2.*met*muon->Pt() : 0.;
  mTMu = (muon != 0) ? sqrt(mTMu*(1.-cos(muon->Phi() - metPhi))) : 0.;
  double mTE = (electron != 0) ? 2.*met*electron->Pt() : 0.;
  mTE  = (electron != 0) ? sqrt(mTE*(1.-cos(electron->Phi() - metPhi))) : 0.;
  mutau = mutau && mTMu < 60.;
  etau  = etau  && mTE  < 60.;

  //mutau cuts in first slides
  if(mutau && chargeTest) FillAllHistograms(8);
  else if(mutau)          FillAllHistograms(8 + fQcdOffset);
  if(etau && chargeTest) FillAllHistograms(18);
  else if(etau)          FillAllHistograms(18 + fQcdOffset);

  //add required photon
  mutau = mutau && nPhotons > 0;
  mutau = mutau && (abs(photonP4->Eta()) < 1.4432 || (abs(photonP4->Eta()) > 1.566 && abs(photonP4->Eta()) < 2.5));
  mutau = mutau && photonP4->Pt() > 10.;
  mutau = mutau && photonP4->DeltaR(*muon) > 0.5;
  mutau = mutau && photonP4->DeltaR(*tau) > 0.5;
  if(mutau && chargeTest) FillAllHistograms(9);
  else if(mutau)          FillAllHistograms(9 + fQcdOffset);

  //add required photon
  etau = etau && nPhotons > 0;
  etau = etau && (abs(photonP4->Eta()) < 1.4432 || (abs(photonP4->Eta()) > 1.566 && abs(photonP4->Eta()) < 2.5));
  etau = etau && photonP4->Pt() > 10.;
  etau = etau && photonP4->DeltaR(*electron) > 0.5;
  etau = etau && photonP4->DeltaR(*tau) > 0.5;
  if(etau && chargeTest) FillAllHistograms(19);
  else if(etau)          FillAllHistograms(19 + fQcdOffset);
  
  //extra cuts added
  mutau = mutau && 10. < (0.85*mgll - mll) && 25. > (0.85*mgll - mll);
  mutau = mutau && photonP4->Pt() > 18.;
  if(mutau && chargeTest) FillAllHistograms(10);
  else if(mutau)          FillAllHistograms(10 + fQcdOffset);

  double zmass = 91.1876;
  double mge = (electron) ? (*photonP4 + *electron).M() : 0.;
  etau = etau && 10. < (0.85*mgll - mll) && 25. > (0.85*mgll - mll);
  etau = etau && photonP4->Pt() > 18.;
  etau = etau && abs(mge-zmass) > 10.;
  if(etau && chargeTest) FillAllHistograms(20);
  else if(etau)          FillAllHistograms(20 + fQcdOffset);

  return kTRUE;
}

void ZTauTauHistMaker::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void ZTauTauHistMaker::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s\n",fOut->GetName());
  fOut->Write();//Form("%s_OutFile.root",fChain->GetName()));
  delete fOut;
  // timer->Stop();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);


}
