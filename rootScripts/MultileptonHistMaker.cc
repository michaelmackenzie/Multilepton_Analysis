#define MultileptonHistMaker_cxx
// The class definition in MultileptonHistMaker.h has been generated automatically
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
// root> T->Process("MultileptonHistMaker.C")
// root> T->Process("MultileptonHistMaker.C","some options")
// root> T->Process("MultileptonHistMaker.C+")
//


#include "MultileptonHistMaker.hh"
#include <TStyle.h>

void MultileptonHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  TString option = GetOption();
  printf("MultileptonHistMaker::Begin\n");
  timer->Start();
  fChain = 0;
}

void MultileptonHistMaker::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).
  // Readers to access the data (delete the ones you do not need).

  printf("MultileptonHistMaker::SlaveBegin\n");
  TString option = GetOption();

}

void MultileptonHistMaker::FillAllHistograms(Int_t index) {
  if(fEventSets[index]) FillEventHistogram(fEventHist[index]);
  if(fJetSets  [index]) FillJetHistogram(  fJetHist  [index]);
  if(fBJetSets [index]) FillBJetHistogram( fBJetHist [index]);
  if(fLepSets  [index]) FillLepHistogram(  fLepHist  [index]);
}

void MultileptonHistMaker::BookHistograms() {
  BookEventHistograms();
  BookJetHistograms();
  BookBJetHistograms();
  BookLepHistograms();
}

void MultileptonHistMaker::BookEventHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"event_%i",i);
      fDirectories[0*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i]->cd();
      fEventHist[i] = new EventHist_t;
      fEventHist[i]->hLumiSection   = new TH1F("lumiSection"   , Form("%s: LumiSection"   ,dirname)  , 200,    0, 4e6);
      fEventHist[i]->hTriggerStatus = new TH1F("triggerStatus" , Form("%s: TriggerStatus" ,dirname)  ,   3, -1.5, 1.5);
      fEventHist[i]->hEventWeight   = new TH1F("eventweight"   , Form("%s: EventWeight"   ,dirname)  , 100,   -5,   5);     
      fEventHist[i]->hGenWeight     = new TH1F("genweight"     , Form("%s: GenWeight"     ,dirname)  ,   5, -2.5, 2.5);     
      fEventHist[i]->hNPV           = new TH1F("npv"           , Form("%s: NPV"           ,dirname)  , 200,  0, 200); 
      fEventHist[i]->hNPU           = new TH1F("npu"           , Form("%s: NPU"           ,dirname)  , 100,  0, 100); 
      fEventHist[i]->hNPartons      = new TH1F("npartons"      , Form("%s: NPartons"      ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hMet           = new TH1F("met"           , Form("%s: Met"           ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hMetPhi        = new TH1F("metphi"        , Form("%s: MetPhi"        ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hNMuons        = new TH1F("nmuons"        , Form("%s: NMuons"        ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNElectrons    = new TH1F("nelectrons"    , Form("%s: NElectrons"    ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNJets         = new TH1F("njets"         , Form("%s: NJets"         ,dirname)  ,  50,  0,  50); 
      fEventHist[i]->hNFwdJets      = new TH1F("nfwdjets"      , Form("%s: NFwdJets"      ,dirname)  ,  50,  0,  50); 
      fEventHist[i]->hNBJets        = new TH1F("nbjets"        , Form("%s: NBJets"        ,dirname)  ,  50,  0,  50);
							       
      fEventHist[i]->hJetsM         = new TH1F("jetsm"         , Form("%s: JetsM"         ,dirname)  , 200,  0, 1e3);     
      fEventHist[i]->hJetsPt        = new TH1F("jetspt"        , Form("%s: JetsPt"        ,dirname)  , 200,  0, 1e3);     
      fEventHist[i]->hJetsEta       = new TH1F("jetseta"       , Form("%s: JetsEta"       ,dirname)  , 100, -5,   5);     
      fEventHist[i]->hJetsDelPhi    = new TH1F("jetsdelphi"    , Form("%s: JetsDelPhi"    ,dirname)  ,  40,  0,   4);     
      fEventHist[i]->hJetsDelEta    = new TH1F("jetsdeleta"    , Form("%s: JetsDelEta"    ,dirname)  , 100,  0,   5);     
      fEventHist[i]->hJetsDelR      = new TH1F("jetsdelr"      , Form("%s: JetsDelR"      ,dirname)  , 100,  0,   5);     
      fEventHist[i]->hJetsDelRVsPhi = new TH2F("jetsdelrvsphi" , Form("%s: JetsDelRVsPhi" ,dirname)  ,  40,  0,   4, 100,  0,   5);     

      fEventHist[i]->hLepPt         = new TH1F("leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepP          = new TH1F("lepp"          , Form("%s: Lepton P"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepE          = new TH1F("lepe"          , Form("%s: Lepton E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepM          = new TH1F("lepm"          , Form("%s: Lepton M"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepEta        = new TH1F("lepeta"        , Form("%s: Lepton Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepPhi        = new TH1F("lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepDeltaPhi   = new TH1F("lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname)  ,  40,   0,   4);
      fEventHist[i]->hLepDeltaEta   = new TH1F("lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDeltaR     = new TH1F("lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDelRVsPhi  = new TH2F("lepdelrvsphi"  , Form("%s: LepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hLepPtOverM    = new TH1F("lepptoverm"    , Form("%s: Lepton Pt / M"  ,dirname)  , 100,   0,  20);

      fEventHist[i]->hSysM          = new TH1F("sysm"          , Form("%s: SysM"          ,dirname)  , 200,  0, 1e3);     
      fEventHist[i]->hSysPt         = new TH1F("syspt"         , Form("%s: SysPt"         ,dirname)  , 200,  0, 400);     
      fEventHist[i]->hSysEta        = new TH1F("syseta"        , Form("%s: SysEta"        ,dirname)  , 100, -5,   5);     
      fEventHist[i]->hJetsLepDelPhi = new TH1F("jetslepdelphi" , Form("%s: JetsLepDelPhi" ,dirname)  ,  40,  0,   4);     
      fEventHist[i]->hJetsLepDelEta = new TH1F("jetslepdeleta" , Form("%s: JetsLepDelEta" ,dirname)  , 100,  0,  10);     
      fEventHist[i]->hJetsLepDelR   = new TH1F("jetslepdelr"   , Form("%s: JetsLepDelR"   ,dirname)  , 100,  0,  10);     
      fEventHist[i]->hJetsLepDelRVsPhi = new TH2F("jetslepdelrvsphi"  , Form("%s: JetsLepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hJetLepDelPhi  = new TH1F("jetlepdelphi"  , Form("%s: JetLepDelPhi"  ,dirname)  ,  40,  0,   4);     
      fEventHist[i]->hJetLepDelEta  = new TH1F("jetlepdeleta"  , Form("%s: JetLepDelEta"  ,dirname)  , 100,  0,  10);     
      fEventHist[i]->hJetLepDelR    = new TH1F("jetlepdelr"    , Form("%s: JetLepDelR"    ,dirname)  , 100,  0,  10);     
      fEventHist[i]->hJetLepDelRVsPhi = new TH2F("jetlepdelrvsphi"  , Form("%s: JetaLepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     

      fEventHist[i]->hBJetLepM      = new TH1F("bjetlepm"      , Form("%s: BJetLepM"      ,dirname)  , 200,  0, 1e3);     
      fEventHist[i]->hBJetLepPt     = new TH1F("bjetleppt"     , Form("%s: BJetLepPt"     ,dirname)  , 200,  0, 1e3);     
      fEventHist[i]->hBJetLepDelPhi = new TH1F("bjetlepdelphi" , Form("%s: BJetLepDelPhi" ,dirname)  ,  40,  0,   4);     
      fEventHist[i]->hBJetLepDelEta = new TH1F("bjetlepdeleta" , Form("%s: BJetLepDelEta" ,dirname)  , 100,  0,  10);     
      fEventHist[i]->hBJetLepDelR   = new TH1F("bjetlepdelr"   , Form("%s: BJetLepDelR"   ,dirname)  , 100,  0,  10);     
      fEventHist[i]->hBJetLepDelRVsPhi = new TH2F("bjetlepdelrvsphi"  , Form("%s: BJetaLepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);
      fEventHist[i]->hBJetLepMVsLepM = new TH2F("bjetlepmvslepm", Form("%s: BJetaLepMVsLepM"   ,dirname)  ,  200,  0, 1e3, 200,  0, 400); 
    }
  }
}

void MultileptonHistMaker::BookJetHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fJetSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"jet_%i",i);
      fDirectories[1*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[1*fn + i]->cd();
      fJetHist[i] = new JetHist_t;
      fJetHist[i]->hPx        = new TH1F("px"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fJetHist[i]->hPy        = new TH1F("py"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fJetHist[i]->hPz        = new TH1F("pz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fJetHist[i]->hPt        = new TH1F("pt"      , Form("%s: Pt"      ,dirname)  , 200,   0, 1e3);
      fJetHist[i]->hP         = new TH1F("p"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fJetHist[i]->hE         = new TH1F("e"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fJetHist[i]->hM         = new TH1F("m"       , Form("%s: M"       ,dirname)  , 200,   0, 1e3);
      fJetHist[i]->hEta       = new TH1F("eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fJetHist[i]->hPhi       = new TH1F("phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fJetHist[i]->hFlavor    = new TH1F("flavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fJetHist[i]->hJetD0     = new TH1F("jetd0"   , Form("%s: JetD0"   ,dirname)  , 200, -10,  10);
      fJetHist[i]->hJetTag    = new TH1F("jettag"  , Form("%s: JetTag"  ,dirname)  , 100,   0,   1);
      fJetHist[i]->hJetPUID   = new TH1F("jetpuid" , Form("%s: JetPUID" ,dirname)  , 200,  -1,   1);
    }
  }
}

void MultileptonHistMaker::BookBJetHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fBJetSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"bjet_%i",i);
      fDirectories[2*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i]->cd();
      fBJetHist[i] = new JetHist_t;
      fBJetHist[i]->hPx        = new TH1F("px"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fBJetHist[i]->hPy        = new TH1F("py"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fBJetHist[i]->hPz        = new TH1F("pz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fBJetHist[i]->hPt        = new TH1F("pt"      , Form("%s: Pt"      ,dirname)  , 200,   0, 1e3);
      fBJetHist[i]->hP         = new TH1F("p"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fBJetHist[i]->hE         = new TH1F("e"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fBJetHist[i]->hM         = new TH1F("m"       , Form("%s: M"       ,dirname)  , 200,   0, 1e3);
      fBJetHist[i]->hEta       = new TH1F("eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fBJetHist[i]->hPhi       = new TH1F("phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fBJetHist[i]->hFlavor    = new TH1F("flavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fBJetHist[i]->hJetD0     = new TH1F("jetd0"   , Form("%s: JetD0"   ,dirname)  , 200, -10,  10);
      fBJetHist[i]->hJetTag    = new TH1F("jettag"  , Form("%s: JetTag"  ,dirname)  , 100,   0,   1);
      fBJetHist[i]->hJetPUID   = new TH1F("jetpuid" , Form("%s: JetPUID" ,dirname)  , 200,  -1,   1);
    }
  }
}

void MultileptonHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fLepSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      fLepHist[i] = new LepHist_t;
      fLepHist[i]->hOnePx        = new TH1F("onepx"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePy        = new TH1F("onepy"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePz        = new TH1F("onepz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePt        = new TH1F("onept"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneP         = new TH1F("onep"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneE         = new TH1F("onee"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneM         = new TH1F("onem"       , Form("%s: M"       ,dirname)  , 200,   0, 1e3);
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
      fLepHist[i]->hTwoM         = new TH1F("twom"       , Form("%s: M"       ,dirname)  , 200,   0, 1e3);
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

void MultileptonHistMaker::FillEventHistogram(EventHist_t* Hist) {
  Hist->hLumiSection   ->Fill(lumiSection  ,eventWeight*genWeight);
  Hist->hTriggerStatus ->Fill(triggerStatus,eventWeight*genWeight);
  Hist->hEventWeight   ->Fill(eventWeight*genWeight              );
  Hist->hGenWeight     ->Fill(genWeight                          );
  Hist->hNPV           ->Fill(nPV          ,eventWeight*genWeight);
  Hist->hNPU           ->Fill(nPU          ,eventWeight*genWeight);
  Hist->hNPartons      ->Fill(nPartons     ,eventWeight*genWeight);
  Hist->hMet           ->Fill(met          ,eventWeight*genWeight);
  Hist->hMetPhi        ->Fill(metPhi       ,eventWeight*genWeight);
  Hist->hNMuons        ->Fill(nMuons       ,eventWeight*genWeight);
  Hist->hNElectrons    ->Fill(nElectrons   ,eventWeight*genWeight);
  Hist->hNJets         ->Fill(nJets        ,eventWeight*genWeight);
  Hist->hNFwdJets      ->Fill(nFwdJets     ,eventWeight*genWeight);
  Hist->hNBJets        ->Fill(nBJets       ,eventWeight*genWeight);

  TLorentzVector jetSys = (*jetP4) + (*bjetP4);
  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  TLorentzVector sys    = jetSys + lepSys;
  
  float jbDelR   = jetP4->DeltaR(*bjetP4);
  float jbDelPhi = abs(jetP4->DeltaPhi(*bjetP4));
  float jbDelEta = abs(jetP4->Eta() - bjetP4->Eta());

  float lepDelR   = leptonOneP4->DeltaR(*leptonTwoP4);
  float lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  float lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  Hist->hJetsM        ->Fill(jetSys.M()             ,eventWeight*genWeight);
  Hist->hJetsPt       ->Fill(jetSys.Pt()            ,eventWeight*genWeight);
  Hist->hJetsEta      ->Fill(jetSys.Eta()           ,eventWeight*genWeight);
  Hist->hJetsDelPhi   ->Fill(jbDelPhi               ,eventWeight*genWeight);
  Hist->hJetsDelEta   ->Fill(jbDelEta               ,eventWeight*genWeight);
  Hist->hJetsDelR     ->Fill(jbDelR                 ,eventWeight*genWeight);
  Hist->hJetsDelRVsPhi->Fill(jbDelR , jbDelPhi      ,eventWeight*genWeight);
							       
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




  float jslDelR   = jetSys.DeltaR(lepSys);
  float jslDelPhi = abs(jetSys.DeltaPhi(lepSys));
  float jslDelEta = abs(jetSys.Eta() - lepSys.Eta());
  float jlDelR   = jetSys.DeltaR(lepSys);
  float jlDelPhi = abs(jetSys.DeltaPhi(lepSys));
  float jlDelEta = abs(jetSys.Eta() - lepSys.Eta());

  TLorentzVector blepSys = (*bjetP4)+lepSys;
  float blM = blepSys.M();
  float blPt = blepSys.Pt();  
  float blDelR   = bjetP4->DeltaR(lepSys);
  float blDelPhi = abs(bjetP4->DeltaPhi(lepSys));
  float blDelEta = abs(bjetP4->Eta() - lepSys.Eta());
  
  Hist->hSysM          ->Fill(sys.M()      ,eventWeight*genWeight);
  Hist->hSysPt         ->Fill(sys.Pt()     ,eventWeight*genWeight);
  Hist->hSysEta        ->Fill(sys.Eta()    ,eventWeight*genWeight);
  Hist->hJetsLepDelPhi ->Fill(jslDelPhi    ,eventWeight*genWeight);
  Hist->hJetsLepDelEta ->Fill(jslDelEta    ,eventWeight*genWeight);
  Hist->hJetsLepDelR   ->Fill(jslDelR      ,eventWeight*genWeight);
  Hist->hJetsLepDelRVsPhi ->Fill(jslDelR , jslDelPhi    ,eventWeight*genWeight);
  
  Hist->hJetLepDelPhi  ->Fill(jlDelPhi     ,eventWeight*genWeight);
  Hist->hJetLepDelEta  ->Fill(jlDelEta     ,eventWeight*genWeight);
  Hist->hJetLepDelR    ->Fill(jlDelR       ,eventWeight*genWeight);
  Hist->hJetLepDelRVsPhi ->Fill(jlDelR , jlDelPhi    ,eventWeight*genWeight);

  Hist->hBJetLepM      ->Fill(blM          ,eventWeight*genWeight);
  Hist->hBJetLepPt     ->Fill(blPt         ,eventWeight*genWeight);
  Hist->hBJetLepDelPhi ->Fill(blDelPhi     ,eventWeight*genWeight);
  Hist->hBJetLepDelEta ->Fill(blDelEta     ,eventWeight*genWeight);
  Hist->hBJetLepDelR   ->Fill(blDelR       ,eventWeight*genWeight);
  Hist->hBJetLepDelRVsPhi ->Fill(blDelR , blDelPhi    ,eventWeight*genWeight);

  Hist->hBJetLepMVsLepM ->Fill(blM , lepSys.M()   ,eventWeight*genWeight);
  
}

void MultileptonHistMaker::FillJetHistogram(JetHist_t* Hist) {
  Hist->hPx        ->Fill(jetP4->Px()      ,eventWeight*genWeight);
  Hist->hPy        ->Fill(jetP4->Py()      ,eventWeight*genWeight);
  Hist->hPz        ->Fill(jetP4->Pz()      ,eventWeight*genWeight);
  Hist->hPt        ->Fill(jetP4->Pt()      ,eventWeight*genWeight);
  Hist->hP         ->Fill(jetP4->P()       ,eventWeight*genWeight);
  Hist->hE         ->Fill(jetP4->E()       ,eventWeight*genWeight);
  Hist->hM         ->Fill(jetP4->M()       ,eventWeight*genWeight);
  Hist->hEta       ->Fill(jetP4->Eta()     ,eventWeight*genWeight);
  Hist->hPhi       ->Fill(jetP4->Phi()     ,eventWeight*genWeight);
  Hist->hFlavor    ->Fill(jetFlavor        ,eventWeight*genWeight);
  Hist->hJetD0     ->Fill(jetD0            ,eventWeight*genWeight);
  Hist->hJetTag    ->Fill(jetTag           ,eventWeight*genWeight);
  Hist->hJetPUID   ->Fill(jetPUID          ,eventWeight*genWeight);
}

void MultileptonHistMaker::FillBJetHistogram(JetHist_t* Hist) {
  Hist->hPx        ->Fill(bjetP4->Px()      ,eventWeight*genWeight);
  Hist->hPy        ->Fill(bjetP4->Py()      ,eventWeight*genWeight);
  Hist->hPz        ->Fill(bjetP4->Pz()      ,eventWeight*genWeight);
  Hist->hPt        ->Fill(bjetP4->Pt()      ,eventWeight*genWeight);
  Hist->hP         ->Fill(bjetP4->P()       ,eventWeight*genWeight);
  Hist->hE         ->Fill(bjetP4->E()       ,eventWeight*genWeight);
  Hist->hM         ->Fill(bjetP4->M()       ,eventWeight*genWeight);
  Hist->hEta       ->Fill(bjetP4->Eta()     ,eventWeight*genWeight);
  Hist->hPhi       ->Fill(bjetP4->Phi()     ,eventWeight*genWeight);
  Hist->hFlavor    ->Fill(bjetFlavor        ,eventWeight*genWeight);
  Hist->hJetD0     ->Fill(bjetD0            ,eventWeight*genWeight);
  Hist->hJetTag    ->Fill(bjetTag           ,eventWeight*genWeight);
  Hist->hJetPUID   ->Fill(bjetPUID          ,eventWeight*genWeight);
}

void MultileptonHistMaker::FillLepHistogram(LepHist_t* Hist) {
  Hist->hOnePx        ->Fill(leptonOneP4->Px()      ,eventWeight*genWeight);
  Hist->hOnePy        ->Fill(leptonOneP4->Py()      ,eventWeight*genWeight);
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()      ,eventWeight*genWeight);
  Hist->hOnePt        ->Fill(leptonOneP4->Pt()      ,eventWeight*genWeight);
  Hist->hOneP         ->Fill(leptonOneP4->P()       ,eventWeight*genWeight);
  Hist->hOneE         ->Fill(leptonOneP4->E()       ,eventWeight*genWeight);
  Hist->hOneM         ->Fill(leptonOneP4->M()       ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()     ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOneP4->Phi()     ,eventWeight*genWeight);
  Hist->hOneIso       ->Fill(leptonOneIso           ,eventWeight*genWeight);
  float relIso1 = leptonOneIso/ leptonOneP4->Pt();
  Hist->hOneRelIso    ->Fill(relIso1                ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(leptonOneFlavor        ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOneQ             ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOneTrigger       ,eventWeight*genWeight);
  
  Hist->hTwoPx        ->Fill(leptonTwoP4->Px()      ,eventWeight*genWeight);
  Hist->hTwoPy        ->Fill(leptonTwoP4->Py()      ,eventWeight*genWeight);
  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()      ,eventWeight*genWeight);
  Hist->hTwoPt        ->Fill(leptonTwoP4->Pt()      ,eventWeight*genWeight);
  Hist->hTwoP         ->Fill(leptonTwoP4->P()       ,eventWeight*genWeight);
  Hist->hTwoE         ->Fill(leptonTwoP4->E()       ,eventWeight*genWeight);
  Hist->hTwoM         ->Fill(leptonTwoP4->M()       ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()     ,eventWeight*genWeight);
  Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()     ,eventWeight*genWeight);
  Hist->hTwoIso       ->Fill(leptonTwoIso           ,eventWeight*genWeight);
  float relIso2 = leptonTwoIso/ leptonTwoP4->Pt();
  Hist->hTwoRelIso    ->Fill(relIso2                ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(leptonTwoFlavor        ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwoQ             ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwoTrigger       ,eventWeight*genWeight);

}

//EventID Definitions
MultileptonHistMaker::TEventID::TEventID(MultileptonHistMaker & Owner) :fOwner(&Owner){
  fUseMask = 0xffffffff;
}
MultileptonHistMaker::TEventID::~TEventID() {
}

int MultileptonHistMaker::TEventID::IDWord() {

  int id_word(0);

  TLorentzVector lepSys = *(fOwner->leptonOneP4) + *(fOwner->leptonTwoP4);
  TLorentzVector jetSys = *(fOwner->jetP4) + *(fOwner->bjetP4);

  Float_t lepM = lepSys.M();
  Float_t LepEta = abs(lepSys.Eta());
  Float_t PtOverM = lepSys.Pt() / lepM; 

  Float_t JetsLepDPhi   = abs(jetSys.DeltaPhi(lepSys));
  Float_t JetsLepDR     = jetSys.DeltaR(lepSys);
  Float_t BJetLepDPhi   = abs(fOwner->bjetP4->DeltaPhi(lepSys));
  Float_t BJetLepDR     = fOwner->bjetP4->DeltaR(lepSys);
  Float_t LeadingPt     = fOwner->leptonOneP4->Pt();
  Float_t TrailingPt    = fOwner->leptonTwoP4->Pt();

  if (PtOverM       < fMinPtOverM    ) id_word |= kPtOverM    ;
  if (PtOverM       > fMaxPtOverM    ) id_word |= kPtOverM    ;
  if (fOwner->met   < fMinMet        ) id_word |= kMet        ;
  if (fOwner->met   > fMaxMet        ) id_word |= kMet        ;
  if (JetsLepDPhi   < fMinJetsLepDPhi) id_word |= kJetsLepDPhi;
  if (JetsLepDPhi   > fMaxJetsLepDPhi) id_word |= kJetsLepDPhi;
  if (JetsLepDR     < fMinJetsLepDR  ) id_word |= kJetsLepDR  ;
  if (JetsLepDR     > fMaxJetsLepDR  ) id_word |= kJetsLepDR  ;
  if (BJetLepDPhi   < fMinBJetLepDPhi) id_word |= kBJetLepDPhi;
  if (BJetLepDPhi   > fMaxBJetLepDPhi) id_word |= kBJetLepDPhi;
  if (BJetLepDR     < fMinBJetLepDR  ) id_word |= kBJetLepDR  ;
  if (BJetLepDR     > fMaxBJetLepDR  ) id_word |= kBJetLepDR  ;
  if (LeadingPt     < fMinLeadingPt  ) id_word |= kLeadingPt  ;
  if (LeadingPt     > fMaxLeadingPt  ) id_word |= kLeadingPt  ;
  if (TrailingPt    < fMinTrailingPt ) id_word |= kTrailingPt ;
  if (TrailingPt    > fMaxTrailingPt ) id_word |= kTrailingPt ;
  if (LepEta        < fMinLepEta     ) id_word |= kLepEta     ;
  if (LepEta        > fMaxLepEta     ) id_word |= kLepEta     ;
  if (fOwner->leptonOneIso  < fMinLeadingIso ) id_word |= kLeadingIso ;
  if (fOwner->leptonOneIso  > fMaxLeadingIso ) id_word |= kLeadingIso ;
  if (fOwner->leptonTwoIso  < fMinTrailingIso) id_word |= kTrailingIso;
  if (fOwner->leptonTwoIso  > fMaxTrailingIso) id_word |= kTrailingIso;

  return (id_word & fUseMask);
}


Bool_t MultileptonHistMaker::Process(Long64_t entry)
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
  
  if(entry%50000 == 0) printf("Processing event: %12lld\n", entry);
  
  fChain->GetEntry(entry);
  // cout << "Before Set Entry" << endl;
  // TTreeReader::EEntryStatus ees = fReader.SetEntry(entry);
  // if(ees == TTreeReader::kEntryValid) cout <<"valid entry\n";
  // if(ees == TTreeReader::kEntryNotLoaded) cout <<"not loaded entry\n";
  // if(ees == TTreeReader::kEntryNoTree) cout <<"no tree entry\n";
  // if(ees == TTreeReader::kEntryNotFound) cout <<"not found entry\n";
  // if(ees == TTreeReader::kEntryChainSetupError) cout <<"Chain Setup Error\n";
  // if(ees == TTreeReader::kEntryChainFileError) cout <<"Chain File Error\n";
  // if(ees == TTreeReader::kEntryDictionaryError) cout <<"Dictionary Problem entry\n";
  // else cout << ees << " EES Not Found \n"<< endl;

  if(fEventSets[0]) FillEventHistogram(fEventHist[0]);
  if(fJetSets[0])   FillJetHistogram(fJetHist[0]);
  if(fBJetSets[0])  FillBJetHistogram(fBJetHist[0]);
  if(fLepSets[0])   FillLepHistogram(fLepHist[0]);

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  Double_t lepM = lepSys.M();
  bool massTest = (12. < lepM && 70. > lepM);
  bool chargeTest = leptonOneQ*leptonTwoQ < 0; //check if oppositely signed
  
  bool b1f = (massTest && nFwdJets > 0 && nBJets == 1 && nJets == 0);
  b1f = b1f && chargeTest;
  b1f = b1f && jetP4->Pt() > 30. && abs(jetP4->Eta()) > 2.4;
  TLorentzVector jetSys = (*jetP4) + (*bjetP4);
  double jslDelPhi = abs(jetSys.DeltaPhi(lepSys));
  bool b1c = massTest && nFwdJets == 0 && met < 40. && jslDelPhi > 2.5 && nBJets > 0;
  b1c = b1c && chargeTest;
  b1c = b1c && jetP4->Pt() > 30. && abs(jetP4->Eta()) < 2.4 && (nJets + nBJets) == 2;
  if(fEventSets[1] && massTest)     FillEventHistogram(fEventHist[1]);
  if(fEventSets[2] && b1c)          FillEventHistogram(fEventHist[2]);
  if(fEventSets[3] && b1f)          FillEventHistogram(fEventHist[3]);
  if(fEventSets[4] && (b1c || b1f)) FillEventHistogram(fEventHist[4]);
  if(fEventSets[5] && (b1c || b1f) && lepSys.Pt()/lepSys.M() > 2.) FillEventHistogram(fEventHist[5]);
  if(fEventSets[6] && (b1c)&& lepSys.Pt()/lepSys.M() > 2.) FillEventHistogram(fEventHist[6]);
  if(fEventSets[7] && (b1f) && lepSys.Pt()/lepSys.M() > 2.) FillEventHistogram(fEventHist[7]);

  if(fJetSets[1]   && massTest)   FillJetHistogram(fJetHist[1]);
  if(fBJetSets[1]  && massTest)   FillBJetHistogram(fBJetHist[1]);
  if(fLepSets[1]   && massTest)   FillLepHistogram(fLepHist[1]);


  /*
    B2bX in 0.02 signal loss steps Optimization:
    1) pt/M > 1.6 ctr met < 65 fwd 2) met < 65 ctr pt/M > 1.6 fwd
    3) jslDR > 2.8 ctr > 3.1 fwd 4) pt/M > 2 ctr jslDPhi > 2.5
    5) met < 55 ctr jslDR > 3.2 6) bjlDR < 3.2 ctr + fwd
    7) lep1Pt > 35 ctr blDR > 1.5  //8) pt/M > 2.2 ctr lep1Pt < 105 fwd

    B2bX in 0.02 signal loss steps Optimization without pt/M:
    1) met < 65 ctr < 65 fwd 2) jslDPhi > 2.5 ctr jslDR > 3.1 fwd
    3) jslDR > 2.8 ctr jslDPhi > 2.5 4) lepSysPt > 52 ctr jslDR > 3.2
    5) met < 55 ctr bjlDR > 1.5 6) bjlDR < 3.2 ctr met < 55 fwd
    7) jslDR > 2.9 ctr bjlDR < 3.2 //8) lepSysPt > 58 ctr > 54 fwd

    B2bX in 0.02 signal loss steps Optimization for S/B with 20 GeV < M < 40 GeV (no lepSysPt cut):
    1) met < 64 ctr      met < 64 fwd  2) jslDR > 2.8 ctr jslDR > 3.1 fwd
    3) met < 56 ctr      jslDPhi > 2.5 4) pt/M > 1.8 ctr jslDR > 3.2 fwd
    4) jslDPhi > 2.7 ctr met < 56 fwd  5) bjlDR < 3.2 ctr pt/M > 1.8 fwd

    B2bX in 0.02 signal loss steps Optimization for S/B with 20 GeV < M < 40 GeV (no lepSysPt cut):
    Done in the Pt study where no pt requirement in the selector is added
    1) met < 62 ctr met < 70 fwd 2) jslDPhi > 2.1 ctr jslDR > 3 fwd
    3) pt/M > 1 ctr pt/M > 1 fwd 4) jslDR > 2.7 ctr jslDPhi > 2.3 fwd
    5) l1Pt > 27 ctr l1Pt > 27 fwd 6) met < 54 ctr blDR < 3.3 fwd
    6) blDR < 3.3 ctr jslDPhi > 2.5 fwd 7) jslDR > 2.9 ctr pt/M > 1.6 fwd
    8) pt/M > 1.6 ctr jslDR > 3.2 fwd 9) jslDR > 3 ctr met < 58 fwd
    //10) met < 48 ctr jslDR > 3.3 fwd
   */
  double blDelR    = bjetP4->DeltaR(lepSys);
  double jslDelR   = jetSys.DeltaR(lepSys);

  TEventID* boxCutID[2];
  boxCutID[0] = new TEventID(*this); //central jet selection
  boxCutID[1] = new TEventID(*this); //forward jet selection

  boxCutID[0]->SetMinPtOverM(1.6);
  boxCutID[0]->SetMinJetsLepDR(3.);
  boxCutID[0]->SetMaxBJetLepDR(3.3);
  boxCutID[0]->SetMaxMet(54.);
  boxCutID[0]->SetMinLeadingPt(27.);
  boxCutID[0]->SetMinJetsLepDPhi(2.1);

  boxCutID[1]->SetMinPtOverM(1.6);
  boxCutID[1]->SetMinJetsLepDR(3.2);
  boxCutID[1]->SetMaxBJetLepDR(3.3);
  boxCutID[1]->SetMaxMet(58.);
  boxCutID[1]->SetMinLeadingPt(27.);
  boxCutID[1]->SetMinJetsLepDPhi(2.5);

  int id_word[2];
  id_word[0] = boxCutID[0]->IDWord();
  id_word[1] = boxCutID[1]->IDWord();

  bool boxCuts[] = {kTRUE,kTRUE};
  boxCuts[0] = boxCuts[0] && massTest && nBJets>0 && (nJets+nBJets) == 2 && nFwdJets==0;// && lepM > 20. && lepM < 40.;
  boxCuts[1] = boxCuts[1] && massTest && nBJets==1 && nJets==0;// && lepM > 20. && lepM < 40.;

  
  if(boxCuts[0]) {
    FillAllHistograms(8);
    if(id_word[0] == 0) {
      if(fEventSets[20])  FillEventHistogram(fEventHist[20]);
      if(fJetSets  [20])  FillJetHistogram(  fJetHist  [20]);
      if(fBJetSets [20])  FillBJetHistogram( fBJetHist [20]);
      if(fLepSets  [20])  FillLepHistogram(  fLepHist  [20]);
    } else { // studying which fail only 1 cut
      if((id_word[0] & ~boxCutID[0]->kPtOverM     )== 0) FillEventHistogram(fEventHist[30]);
      if((id_word[0] & ~boxCutID[0]->kMet         )== 0) FillEventHistogram(fEventHist[31]);
      if((id_word[0] & ~boxCutID[0]->kJetsLepDPhi )== 0) FillEventHistogram(fEventHist[32]);
      if((id_word[0] & ~boxCutID[0]->kJetsLepDR   )== 0) FillEventHistogram(fEventHist[33]);
      if((id_word[0] & ~boxCutID[0]->kBJetLepDR   )== 0) FillEventHistogram(fEventHist[34]);
      if((id_word[0] & ~boxCutID[0]->kBJetLepDPhi )== 0) FillEventHistogram(fEventHist[35]);
      if((id_word[0] & ~boxCutID[0]->kLeadingPt   )== 0) FillEventHistogram(fEventHist[36]);
      if((id_word[0] & ~boxCutID[0]->kTrailingPt  )== 0) FillEventHistogram(fEventHist[37]);
      if((id_word[0] & ~boxCutID[0]->kLepEta      )== 0) FillEventHistogram(fEventHist[38]);
      if((id_word[0] & ~boxCutID[0]->kLeadingIso  )== 0) FillEventHistogram(fEventHist[39]);
    }
  }
  if(boxCuts[1]) {
    FillAllHistograms(9);
    if(id_word[1] == 0) {
      if(fEventSets[21])  FillEventHistogram(fEventHist[21]);
      if(fJetSets  [21])  FillJetHistogram(  fJetHist  [21]);
      if(fBJetSets [21])  FillBJetHistogram( fBJetHist [21]);
      if(fLepSets  [21])  FillLepHistogram(  fLepHist  [21]);
    } else { // studying which fail only 1 cut
      if((id_word[1] & ~boxCutID[1]->kPtOverM     )== 0) FillEventHistogram(fEventHist[40]);
      if((id_word[1] & ~boxCutID[1]->kMet         )== 0) FillEventHistogram(fEventHist[41]);
      if((id_word[1] & ~boxCutID[1]->kJetsLepDPhi )== 0) FillEventHistogram(fEventHist[42]);
      if((id_word[1] & ~boxCutID[1]->kJetsLepDR   )== 0) FillEventHistogram(fEventHist[43]);
      if((id_word[1] & ~boxCutID[1]->kBJetLepDR   )== 0) FillEventHistogram(fEventHist[44]);
      if((id_word[1] & ~boxCutID[1]->kBJetLepDPhi )== 0) FillEventHistogram(fEventHist[45]);
      if((id_word[1] & ~boxCutID[1]->kLeadingPt   )== 0) FillEventHistogram(fEventHist[46]);
      if((id_word[1] & ~boxCutID[1]->kTrailingPt  )== 0) FillEventHistogram(fEventHist[47]);
      if((id_word[1] & ~boxCutID[1]->kLepEta      )== 0) FillEventHistogram(fEventHist[48]);
      if((id_word[1] & ~boxCutID[1]->kLeadingIso  )== 0) FillEventHistogram(fEventHist[49]);
    }
  }
  if((boxCuts[0] && id_word[0] == 0) || (boxCuts[1] && id_word[1] == 0)) {
    if(fEventSets[22])  FillEventHistogram(fEventHist[22]);
    if(fJetSets  [22])  FillJetHistogram(  fJetHist  [22]);
    if(fBJetSets [22])  FillBJetHistogram( fBJetHist [22]);
    if(fLepSets  [22])  FillLepHistogram(  fLepHist  [22]);
  }

  //Box cuts defined without Pt/M:
  bool set12CTest = massTest && nBJets>0 && (nJets+nBJets) == 2 && nFwdJets==0;
  set12CTest = set12CTest && abs(lepSys.Eta()) < 1.9;
  set12CTest = set12CTest && blDelR < 3.2;
  set12CTest = set12CTest && jslDelR > 2.9;
  set12CTest = set12CTest && met < 54.;
  set12CTest = set12CTest && jslDelPhi > 2.5;

  bool set12FTest = massTest && nBJets==1 && nJets==0;
  // set12FTest = set12FTest && lepSys.Pt() > 54.;
  set12FTest = set12FTest && blDelR < 3.2;
  set12FTest = set12FTest && blDelR > 1.5;
  set12FTest = set12FTest && jslDelR > 3.2;
  set12FTest = set12FTest && met < 55.;
  set12FTest = set12FTest && jslDelPhi > 2.5;
  if(set12CTest) {
    if(fEventSets[10])  FillEventHistogram(fEventHist[10]);
    if(fJetSets  [10])  FillJetHistogram(  fJetHist  [10]);
    if(fBJetSets [10])  FillBJetHistogram( fBJetHist [10]);
    if(fLepSets  [10])  FillLepHistogram(  fLepHist  [10]);
  }
  if(set12FTest) {
    if(fEventSets[11])  FillEventHistogram(fEventHist[11]);
    if(fJetSets  [11])  FillJetHistogram(  fJetHist  [11]);
    if(fBJetSets [11])  FillBJetHistogram( fBJetHist [11]);
    if(fLepSets  [11])  FillLepHistogram(  fLepHist  [11]);
  }
  if(set12CTest || set12FTest) {
    if(fEventSets[12])  FillEventHistogram(fEventHist[12]);
    if(fJetSets  [12])  FillJetHistogram(  fJetHist  [12]);
    if(fBJetSets [12])  FillBJetHistogram( fBJetHist [12]);
    if(fLepSets  [12])  FillLepHistogram(  fLepHist  [12]);
  }

  //Box cuts defined with Pt/M
  bool set16CTest = massTest && nBJets>0 && (nJets+nBJets) == 2 && nFwdJets==0;
  set16CTest = set16CTest && lepSys.Pt()/lepSys.M() > 1.8;
  set16CTest = set16CTest && blDelR < 3.2;
  set16CTest = set16CTest && jslDelR > 2.8;
  set16CTest = set16CTest && met < 56.;
  set16CTest = set16CTest && jslDelPhi > 2.7;

  bool set16FTest = massTest && nBJets==1 && nJets==0;
  set16FTest = set16FTest && lepSys.Pt()/lepSys.M() > 1.8;
  set16FTest = set16FTest && jslDelR > 3.2;
  set16FTest = set16FTest && met < 56.;
  set16FTest = set16FTest && jslDelPhi > 2.5;

  if(set16CTest) {
    if(fEventSets[14])  FillEventHistogram(fEventHist[14]);
    if(fJetSets  [14])  FillJetHistogram(  fJetHist  [14]);
    if(fBJetSets [14])  FillBJetHistogram( fBJetHist [14]);
    if(fLepSets  [14])  FillLepHistogram(  fLepHist  [14]);
  }
  if(set16FTest) {
    if(fEventSets[15])  FillEventHistogram(fEventHist[15]);
    if(fJetSets  [15])  FillJetHistogram(  fJetHist  [15]);
    if(fBJetSets [15])  FillBJetHistogram( fBJetHist [15]);
    if(fLepSets  [15])  FillLepHistogram(  fLepHist  [15]);
  }
  if(set16CTest || set16FTest) {
    if(fEventSets[16])  FillEventHistogram(fEventHist[16]);
    if(fJetSets  [16])  FillJetHistogram(  fJetHist  [16]);
    if(fBJetSets [16])  FillBJetHistogram( fBJetHist [16]);
    if(fLepSets  [16])  FillLepHistogram(  fLepHist  [16]);
  }

  
  return kTRUE;
}

void MultileptonHistMaker::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void MultileptonHistMaker::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s.hist\n",fChain->GetName());
  fOut->Write();//Form("%s_OutFile.root",fChain->GetName()));
  delete fOut;
  // timer->Stop();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

}
