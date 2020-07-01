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
  for(int i = 0; i < kIds; ++i) fEventId[i] = 0;

  fEventId[20] = new TEventID(); //zemu
  // cutstring: lepm>88.245574860124123&&lepm<95.218728460130563&&mtone<86.510608060784762&&mttwo<76.769443816768444
  // &&leppt<23.865836528857464
  // limit gain: 2.33436 sig_eff: 0.392917 bkg_rej: 0.996505
  fEventId[20]->fMinLepm = 88.24557;     fEventId[20]->fMaxLepm = 95.21873;
  fEventId[20]->fMaxMtone = 86.51061;    fEventId[20]->fMaxMttwo = 76.76944;
  fEventId[20]->fMaxLeppt = 23.86584;
  fEventId[50] = new TEventID(); //hemu
  // cutstring lepm>121.98691899130384&&lepm<127.1750722802732&&mtone<88.949126707707563&&mttwo<80.913400356866262
  // &&leponept>35.854765333337127&&leptwopt>30.543996519263601
  // limit gain: 2.5026 sig_eff: 0.448063 bkg_rej: 0.996876
  fEventId[50]->fMinLepm = 121.98692;    fEventId[50]->fMaxLepm = 127.17507;
  fEventId[50]->fMaxMtone = 88.94913;    fEventId[50]->fMaxMttwo = 80.91340;
  fEventId[50]->fMinLeponept = 35.85477; fEventId[50]->fMinLeptwopt = 30.54400;

  fRnd = new TRandom(fRndSeed);
  
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
  } else
    printf("WARNING! Attempted to fill un-initialized histogram set %i!\n", index);
  if(fWriteTrees && fTreeSets[index])
    fTrees[index]->Fill();
}

void ZTauTauHistMaker::BookHistograms() {
  BookEventHistograms();
  BookPhotonHistograms();
  BookLepHistograms();
  if(fWriteTrees) BookTrees();
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
      fEventHist[i]->hGenTauFlavorWeight     = new TH1F("gentauflavorweight"  , Form("%s: GenTauFlavorWeight"  ,dirname)  ,  40,    0,   2);     
      fEventHist[i]->hNPV                    = new TH1F("npv"                 , Form("%s: NPV"                 ,dirname)  , 200,  0, 200); 
      fEventHist[i]->hNPU                    = new TH1F("npu"                 , Form("%s: NPU"                 ,dirname)  , 100,  0, 100); 
      fEventHist[i]->hNPartons               = new TH1F("npartons"            , Form("%s: NPartons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNMuons                 = new TH1F("nmuons"              , Form("%s: NMuons"              ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNElectrons             = new TH1F("nelectrons"          , Form("%s: NElectrons"          ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNLowPtElectrons        = new TH1F("nlowptelectrons"     , Form("%s: NLowPtElectrons"     ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNTaus                  = new TH1F("ntaus"               , Form("%s: NTaus"               ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNPhotons               = new TH1F("nphotons"            , Form("%s: NPhotons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausHad            = new TH1F("ngentaushad"         , Form("%s: NGenTausHad"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausLep            = new TH1F("ngentauslep"         , Form("%s: NGenTausLep"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTaus               = new TH1F("ngentaus"            , Form("%s: NGenTaus"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenElectrons          = new TH1F("ngenelectrons"       , Form("%s: NGenElectrons"       ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenMuons              = new TH1F("ngenmuons"           , Form("%s: NGenMuons"           ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNJets                  = new TH1F("njets"               , Form("%s: NJets"               ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNJets25                = new TH1F("njets25"             , Form("%s: NJets25"             ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNJets20                = new TH1F("njets20"             , Form("%s: NJets20"             ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNJets25Tot             = new TH1F("njets25tot"          , Form("%s: NJets25Tot"          ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNJetsTot               = new TH1F("njetstot"            , Form("%s: NJetsTot"            ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNFwdJets               = new TH1F("nfwdjets"            , Form("%s: NFwdJets"            ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNBJets                 = new TH1F("nbjets"              , Form("%s: NBJets"              ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsM                = new TH1F("nbjetsm"             , Form("%s: NBJetsM"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsL                = new TH1F("nbjetsl"             , Form("%s: NBJetsL"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets25               = new TH1F("nbjets25"            , Form("%s: NBJets25"            ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets25M              = new TH1F("nbjets25m"           , Form("%s: NBJets25M"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets25L              = new TH1F("nbjets25l"           , Form("%s: NBJets25L"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20               = new TH1F("nbjets20"            , Form("%s: NBJets20"            ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20M              = new TH1F("nbjets20m"           , Form("%s: NBJets20M"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20L              = new TH1F("nbjets20l"           , Form("%s: NBJets20L"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets25Tot            = new TH1F("nbjets25tot"         , Form("%s: NBJets25Tot"         ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets25TotM           = new TH1F("nbjets25totm"        , Form("%s: NBJets25TotM"        ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets25TotL           = new TH1F("nbjets25totl"        , Form("%s: NBJets25TotL"        ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsTot              = new TH1F("nbjetstot"           , Form("%s: NBJetsTot"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsTotM             = new TH1F("nbjetstotm"          , Form("%s: NBJetsTotM"          ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsTotL             = new TH1F("nbjetstotl"          , Form("%s: NBJetsTotL"          ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hMcEra                  = new TH1F("mcera"               , Form("%s: McEra"               ,dirname) ,   5,   0,  5);
      fEventHist[i]->hTriggerLeptonStatus    = new TH1F("triggerleptonstatus" , Form("%s: TriggerLeptonStatus" ,dirname) ,   5,   0,  5);
      fEventHist[i]->hPuWeight		     = new TH1F("puweight"	      , Form("%s: PuWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTopPtWeight	     = new TH1F("topptweight"	      , Form("%s: TopPtWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hZPtWeight	             = new TH1F("zptweight"	      , Form("%s: ZPtWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTauDecayMode	     = new TH1F("taudecaymode"	      , Form("%s: TauDecayMode"	       ,dirname) ,  11,   0, 11);
      fEventHist[i]->hTauMVA		     = new TH1F("taumva"              , Form("%s: TauMVA"              ,dirname) , 100,   0,  1);
      fEventHist[i]->hTauGenFlavor	     = new TH1F("taugenflavor"	      , Form("%s: TauGenFlavor"	       ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauGenFlavorHad	     = new TH1F("taugenflavorhad"     , Form("%s: TauGenFlavorHad"     ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauVetoedJetPt	     = new TH1F("tauvetoedjetpt"      , Form("%s: TauVetoedJetPt"      ,dirname) , 210, -10,200);
      fEventHist[i]->hTauVetoedJetPtUnc	     = new TH1F("tauvetoedjetptunc"   , Form("%s: TauVetoedJetPtUnc"   ,dirname) , 110,  -1, 10);
      fEventHist[i]->hHtSum		     = new TH1F("htsum"		      , Form("%s: HtSum"	       ,dirname) , 200,   0,800);
      fEventHist[i]->hHt		     = new TH1F("ht"		      , Form("%s: Ht"		       ,dirname) , 200,   0,800);
      fEventHist[i]->hHtPhi                  = new TH1F("htphi"               , Form("%s: HtPhi"               ,dirname) , 100,  -4,  4);
      fEventHist[i]->hJetPt                  = new TH1F("jetpt"               , Form("%s: JetPt"               ,dirname)  , 300,   0, 300);
      fEventHist[i]->hJetM                   = new TH1F("jetm"                , Form("%s: JetM"                ,dirname)  , 150,   0, 300);
      fEventHist[i]->hJetEta                 = new TH1F("jeteta"              , Form("%s: JetEta"              ,dirname)  , 200, -10,  10);
      fEventHist[i]->hJetPhi                 = new TH1F("jetphi"              , Form("%s: JetPhi"              ,dirname)  , 100,  -4,  4);
      fEventHist[i]->hTauPt                  = new TH1F("taupt"               , Form("%s: TauPt"               ,dirname)  , 300,   0, 300);
      fEventHist[i]->hTauM                   = new TH1F("taum"                , Form("%s: TauM"                ,dirname)  , 100,   0, 4.);
      fEventHist[i]->hTauEta                 = new TH1F("taueta"              , Form("%s: TauEta"              ,dirname)  , 200, -10,  10);
      fEventHist[i]->hTauPhi                 = new TH1F("tauphi"              , Form("%s: TauPhi"              ,dirname)  , 100,  -4,  4);

      fEventHist[i]->hPFMet                  = new TH1F("pfmet"               , Form("%s: PF Met"              ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hPFMetPhi               = new TH1F("pfmetphi"            , Form("%s: PF MetPhi"           ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hPFCovMet00             = new TH1F("pfcovmet00"          , Form("%s: PF CovMet00"         ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPFCovMet01	     = new TH1F("pfcovmet01"	      , Form("%s: PF CovMet01"         ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hPFCovMet11	     = new TH1F("pfcovmet11"	      , Form("%s: PF CovMet11"         ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPuppMet                = new TH1F("puppmet"             , Form("%s: PUPPI Met"           ,dirname)  , 200,  0, 400); 
      // fEventHist[i]->hPuppMetPhi             = new TH1F("puppmetphi"          , Form("%s: PUPPI MetPhi"        ,dirname)  ,  80, -4,   4); 
      // fEventHist[i]->hPuppCovMet00           = new TH1F("puppcovmet00"        , Form("%s: PUPPI CovMet00"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPuppCovMet01	     = new TH1F("puppcovmet01"	      , Form("%s: PUPPI CovMet01"      ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hPuppCovMet11	     = new TH1F("puppcovmet11"	      , Form("%s: PUPPI CovMet11"      ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hTrkMet                 = new TH1F("trkmet"              , Form("%s: Trk Met"             ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hTrkMetPhi              = new TH1F("trkmetphi"           , Form("%s: Trk MetPhi"          ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hMet                    = new TH1F("met"                 , Form("%s: Met"                 ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hMetPhi                 = new TH1F("metphi"              , Form("%s: MetPhi"              ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hMetCorr                = new TH1F("metcorr"             , Form("%s: Met Correction"      ,dirname)  , 200,  0, 40); 
      fEventHist[i]->hMetCorrPhi             = new TH1F("metcorrphi"          , Form("%s: MetPhi Correction"   ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hCovMet00               = new TH1F("covmet00"            , Form("%s: CovMet00"            ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hCovMet01		     = new TH1F("covmet01"	      , Form("%s: CovMet01"            ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hCovMet11		     = new TH1F("covmet11"	      , Form("%s: CovMet11"            ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hMetVsPt                = new TH2F("metvspt"             , Form("%s: MetVsPt"             ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetVsM                 = new TH2F("metvsm"              , Form("%s: MetVsM"              ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetVsHtSum             = new TH2F("metvshtsum"          , Form("%s: MetVsHtSum"          ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetOverSqrtHtSum       = new TH1F("metoversqrthtsum"    , Form("%s: MetOverSqrtHtSum"    ,dirname) ,  200,    0., 400.);
      fEventHist[i]->hMassSVFit		     = new TH1F("masssvfit"	      , Form("%s: MassSVFit"           ,dirname) , 1000,    0., 200.);
      fEventHist[i]->hMassErrSVFit	     = new TH1F("masserrsvfit"        , Form("%s: MassErrSVFit"        ,dirname) , 1000,    0., 100.);
      fEventHist[i]->hSVFitStatus            = new TH1F("svfitstatus"         , Form("%s: SVFitStatus"         ,dirname) ,   10,    0.,  10.);

							       
      fEventHist[i]->hLepPt         = new TH1F("leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepP          = new TH1F("lepp"          , Form("%s: Lepton P"       ,dirname)  , 200,   0, 400);
      // fEventHist[i]->hLepE          = new TH1F("lepe"          , Form("%s: Lepton E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepM          = new TH1F("lepm"          , Form("%s: Lepton M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepEta        = new TH1F("lepeta"        , Form("%s: Lepton Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepPhi        = new TH1F("lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepDeltaPhi   = new TH1F("lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname)  ,  50,   0,   5);
      fEventHist[i]->hLepDeltaEta   = new TH1F("lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDeltaR     = new TH1F("lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDelRVsPhi  = new TH2F("lepdelrvsphi"  , Form("%s: LepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hLepPtOverM    = new TH1F("lepptoverm"    , Form("%s: Lepton Pt / M"  ,dirname)  , 100,   0,  10);
      fEventHist[i]->hHtDeltaPhi    = new TH1F("htdeltaphi"    , Form("%s: Ht Lep Delta Phi",dirname) ,  50,   0,   5);
      fEventHist[i]->hMetDeltaPhi   = new TH1F("metdeltaphi"   , Form("%s: Met Lep Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hJetDeltaPhi   = new TH1F("jetdeltaphi"   , Form("%s: Jet Lep Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepOneDeltaPhi   = new TH1F("leponedeltaphi"   , Form("%s: Lep One vs Sys Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepTwoDeltaPhi   = new TH1F("leptwodeltaphi"   , Form("%s: Lep Two vs Sys Delta Phi",dirname),  50,   0,   5);

      fEventHist[i]->hLepOneJetDeltaR     = new TH1F("leponejetdeltar"     , Form("%s: Lep One vs Jet Delta R"  ,dirname),  60,   0,   6);
      fEventHist[i]->hLepOneJetDeltaPhi   = new TH1F("leponejetdeltaphi"   , Form("%s: Lep One vs Jet Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepOneJetDeltaEta   = new TH1F("leponejetdeltaeta"   , Form("%s: Lep One vs Jet Delta Eta",dirname),  60,   0,   6);
      fEventHist[i]->hLepTwoJetDeltaR     = new TH1F("leptwojetdeltar"     , Form("%s: Lep One vs Jet Delta R"  ,dirname),  60,   0,   6);
      fEventHist[i]->hLepTwoJetDeltaPhi   = new TH1F("leptwojetdeltaphi"   , Form("%s: Lep One vs Jet Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepTwoJetDeltaEta   = new TH1F("leptwojetdeltaeta"   , Form("%s: Lep One vs Jet Delta Eta",dirname),  60,   0,   6);

      fEventHist[i]->hLepSVPt       = new TH1F("lepsvpt"       , Form("%s: Lepton SVFit Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepSVM        = new TH1F("lepsvm"        , Form("%s: Lepton SVFit M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepSVEta      = new TH1F("lepsveta"      , Form("%s: Lepton SVFit Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepSVPhi      = new TH1F("lepsvphi"      , Form("%s: Lepton SVFit Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepSVDeltaPhi = new TH1F("lepsvdeltaphi" , Form("%s: Lepton SVFit DeltaPhi",dirname)  ,  50,   0,   5);
      fEventHist[i]->hLepSVDeltaEta = new TH1F("lepsvdeltaeta" , Form("%s: Lepton SVFit DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepSVDeltaM   = new TH1F("lepsvdeltam"   , Form("%s: Lepton SVFit DeltaM"  ,dirname)  , 200, -10,  90);
      fEventHist[i]->hLepSVDeltaPt  = new TH1F("lepsvdeltapt"  , Form("%s: Lepton SVFit DeltaPt" ,dirname)  , 200, -10,  90);
      fEventHist[i]->hLepSVPtOverM  = new TH1F("lepsvptoverm"  , Form("%s: Lepton SVFit Pt / M"  ,dirname)  , 100,   0,  10);
      
      fEventHist[i]->hSysM          = new TH1F("sysm"          , Form("%s: SysM"          ,dirname)  ,1000,  0, 1e3);     
      fEventHist[i]->hSysPt         = new TH1F("syspt"         , Form("%s: SysPt"         ,dirname)  , 200,  0, 400);     
      fEventHist[i]->hSysEta        = new TH1F("syseta"        , Form("%s: SysEta"        ,dirname)  , 100, -5,   5);     
      fEventHist[i]->hSysMvsLepM    = new TH2F("sysmvslepm"    , Form("%s: SysMvsLepM"    ,dirname)  , 200, 0., 200., 200, 0., 200.);     

      fEventHist[i]->hMTOne         = new TH1F("mtone"         , Form("%s: MTOne"         ,dirname)  , 200, 0.,   200.);     
      fEventHist[i]->hMTTwo         = new TH1F("mttwo"         , Form("%s: MTTwo"         ,dirname)  , 200, 0.,   200.);     

      fEventHist[i]->hPXiVis[0]        = new TH1F("pxivis0"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[0]        = new TH1F("pxiinv0"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[0] = new TH1F("pxivisoverinv0" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[0]   = new TH2F("pxiinvvsvis0"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[0]       = new TH1F("pxidiff0"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[0]      = new TH1F("pxidiff20"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff3[0]      = new TH1F("pxidiff30"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);     
      fEventHist[i]->hPXiVis[1]        = new TH1F("pxivis1"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[1]        = new TH1F("pxiinv1"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[1] = new TH1F("pxivisoverinv1" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[1]   = new TH2F("pxiinvvsvis1"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[1]       = new TH1F("pxidiff1"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[1]      = new TH1F("pxidiff21"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff3[1]      = new TH1F("pxidiff31"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);     
      fEventHist[i]->hPXiVis[2]        = new TH1F("pxivis2"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[2]        = new TH1F("pxiinv2"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[2] = new TH1F("pxivisoverinv2" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[2]   = new TH2F("pxiinvvsvis2"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[2]       = new TH1F("pxidiff2"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[2]      = new TH1F("pxidiff22"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff3[2]      = new TH1F("pxidiff32"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);     

      fEventHist[i]->hPTauVisFrac      = new TH1F("ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  ,300,0.,  1.5);     
      fEventHist[i]->hLepMEstimate     = new TH1F("lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);     
      fEventHist[i]->hLepMEstimateTwo  = new TH1F("lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);     

      fEventHist[i]->hPtSum[0]         = new TH1F("ptsum0"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPtSum[1]         = new TH1F("ptsum1"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[0]        = new TH1F("pt1sum0"        , Form("%s: Scalar Pt sum Lepton 1 + MET"     ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[1]        = new TH1F("pt1sum1"        , Form("%s: Scalar Pt sum Lepton 2 + MET"     ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[2]        = new TH1F("pt1sum2"        , Form("%s: Scalar Pt sum Lepton 1 + 2"       ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[3]        = new TH1F("pt1sum3"        , Form("%s: Scalar Pt sum Lepton 1 + 2 - MET" ,dirname)    ,1000,  0.,  1000.);     
      for(unsigned j = 0; j < fMvaNames.size(); ++j)  {
	//high mva score binning to improve cdf making
	fEventHist[i]->hMVA[j]        = new TH1F(Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMvaNames[j].Data()) ,10000, -1.,  2.); //beyond 1 for space for legend
      }      
      if(fFolderName == "llg_study") {
	//llg study histograms
	TH1F* hObjMasses[14]; //jets, jets+gamma, jet1/2 + gamma, jets + l1/2, jet1/2 + l1/2, jets+l1+l2, jets + gamma + l1/2, jets + gamma + l1 + l2
	//masses
	fEventHist[i]->hObjMasses[0 ]     = new TH1F("objmasses0"       , Form("%s: ObjMasses0        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[1 ]     = new TH1F("objmasses1"       , Form("%s: ObjMasses1        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[2 ]     = new TH1F("objmasses2"       , Form("%s: ObjMasses2        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[3 ]     = new TH1F("objmasses3"       , Form("%s: ObjMasses3        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[4 ]     = new TH1F("objmasses4"       , Form("%s: ObjMasses4        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[5 ]     = new TH1F("objmasses5"       , Form("%s: ObjMasses5        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[6 ]     = new TH1F("objmasses6"       , Form("%s: ObjMasses6        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[7 ]     = new TH1F("objmasses7"       , Form("%s: ObjMasses7        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[8 ]     = new TH1F("objmasses8"       , Form("%s: ObjMasses8        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[9 ]     = new TH1F("objmasses9"       , Form("%s: ObjMasses9        ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[10]     = new TH1F("objmasses10"      , Form("%s: ObjMasses10       ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[11]     = new TH1F("objmasses11"      , Form("%s: ObjMasses11       ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[12]     = new TH1F("objmasses12"      , Form("%s: ObjMasses12       ", dirname), 300, 0., 600.);
	fEventHist[i]->hObjMasses[13]     = new TH1F("objmasses13"      , Form("%s: ObjMasses13       ", dirname), 300, 0., 600.);
	fEventHist[i]->hJetTwoM           = new TH1F("jettwom"           , Form("%s: JetTwoM           ", dirname), 100, 0., 100.);
	fEventHist[i]->hJetTwoPt          = new TH1F("jettwopt"          , Form("%s: JetTwoPt          ", dirname), 300, 0., 300.);
	fEventHist[i]->hJetTwoEta         = new TH1F("jettwoeta"         , Form("%s: JetTwoEta         ", dirname), 200,-10., 10.);
	fEventHist[i]->hJetsDeltaR        = new TH1F("jetsdeltar"        , Form("%s: JetsDeltaR        ", dirname), 120, 0.,  12.);
	fEventHist[i]->hJetsDeltaEta      = new TH1F("jetsdeltaeta"      , Form("%s: JetsDeltaEta      ", dirname), 120, 0.,  12.);
	fEventHist[i]->hJetsDeltaPhi      = new TH1F("jetsdeltaphi"      , Form("%s: JetsDeltaPhi      ", dirname),  40, 0.,   4.);
	fEventHist[i]->hJetsPt            = new TH1F("jetspt"            , Form("%s: JetsPt            ", dirname), 200, 0., 400.);
	fEventHist[i]->hJetsEta           = new TH1F("jetseta"           , Form("%s: JetsEta           ", dirname), 200,-10., 10.);
	fEventHist[i]->hJetsGammaDeltaR   = new TH1F("jetsgammadeltar"   , Form("%s: JetsGammaDeltaR   ", dirname), 120, 0.,  12.);
	fEventHist[i]->hJetsGammaDeltaEta = new TH1F("jetsgammadeltaeta" , Form("%s: JetsGammaDeltaEta ", dirname), 120, 0.,  12.);
	fEventHist[i]->hJetsGammaDeltaPhi = new TH1F("jetsgammadeltaphi" , Form("%s: JetsGammaDeltaPhi ", dirname),  40, 0.,   4.);
	fEventHist[i]->hJetsGammaPt       = new TH1F("jetsgammapt"       , Form("%s: JetsGammaPt       ", dirname), 200, 0., 400.);
	fEventHist[i]->hJetsGammaEta      = new TH1F("jetsgammaeta"      , Form("%s: JetsGammaEta      ", dirname), 200,-10., 10.);
      }
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
      fPhotonHist[i]->hPz        = new TH1F("pz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPt        = new TH1F("pt"      , Form("%s: Pt"      ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hP         = new TH1F("p"       , Form("%s: P"       ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hEta       = new TH1F("eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fPhotonHist[i]->hPhi       = new TH1F("phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fPhotonHist[i]->hMVA       = new TH1F("mva"     , Form("%s: MVA"     ,dirname)  , 200,  -1,   1);
      // fPhotonHist[i]->hIso       = new TH1F("iso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      // fPhotonHist[i]->hRelIso    = new TH1F("reliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      // fPhotonHist[i]->hTrigger   = new TH1F("trigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
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
      fLepHist[i]->hOnePz        = new TH1F("onepz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePt        = new TH1F("onept"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneP         = new TH1F("onep"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneM         = new TH1F("onem"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOneEta       = new TH1F("oneeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOnePhi       = new TH1F("onephi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hOneD0        = new TH1F("oned0"      , Form("%s: D0"      ,dirname)  , 400,-0.1, 0.1);
      fLepHist[i]->hOneIso       = new TH1F("oneiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fLepHist[i]->hOneRelIso    = new TH1F("onereliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hOneFlavor    = new TH1F("oneflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hOneQ         = new TH1F("oneq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hOneTrigger   = new TH1F("onetrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      //Gen Info
      fLepHist[i]->hOneGenPt     = new TH1F("onegenpt"   , Form("%s: Gen Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneGenE      = new TH1F("onegene"    , Form("%s: Gen E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneGenEta    = new TH1F("onegeneta"  , Form("%s: Gen Eta"  ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOneDeltaPt   = new TH1F("onedeltapt" , Form("%s: Gen Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hOneDeltaE    = new TH1F("onedeltae"  , Form("%s: Gen Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneDeltaEta  = new TH1F("onedeltaeta", Form("%s: Gen Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hOneMetDeltaPhi  = new TH1F("onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,   0,   5);
      //SVFit Info
      fLepHist[i]->hOneSVPt      = new TH1F("onesvpt"    , Form("%s: SV Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneSVM       = new TH1F("onesvm"     , Form("%s: SV M"    ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOneSVEta     = new TH1F("onesveta"   , Form("%s: SV Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hOneSVDeltaPt = new TH1F("onesvdeltapt"    , Form("%s: SV Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hOneSVDeltaP  = new TH1F("onesvdeltap"     , Form("%s: SV Delta P"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneSVDeltaE  = new TH1F("onesvdeltae"     , Form("%s: SV Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneSVDeltaEta= new TH1F("onesvdeltaeta"   , Form("%s: SV Delta Eta"  ,dirname)  , 200, -10., 10.);

      fLepHist[i]->hTwoPz        = new TH1F("twopz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPt        = new TH1F("twopt"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoP         = new TH1F("twop"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoM         = new TH1F("twom"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoEta       = new TH1F("twoeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoPhi       = new TH1F("twophi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hTwoD0        = new TH1F("twod0"      , Form("%s: D0"      ,dirname)  , 400,-0.1, 0.1);
      fLepHist[i]->hTwoIso       = new TH1F("twoiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  20);
      fLepHist[i]->hTwoRelIso    = new TH1F("tworeliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hTwoFlavor    = new TH1F("twoflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hTwoQ         = new TH1F("twoq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hTwoTrigger   = new TH1F("twotrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      //Gen Info
      fLepHist[i]->hTwoGenPt     = new TH1F("twogenpt"   , Form("%s: Gen Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoGenE      = new TH1F("twogene"    , Form("%s: Gen E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoGenEta    = new TH1F("twogeneta"  , Form("%s: Gen Eta"  ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoDeltaPt   = new TH1F("twodeltapt" , Form("%s: Gen Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hTwoDeltaE    = new TH1F("twodeltae"  , Form("%s: Gen Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoDeltaEta  = new TH1F("twodeltaeta", Form("%s: Gen Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hTwoMetDeltaPhi  = new TH1F("twometdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,   0,   5);
      //SVFit Info
      fLepHist[i]->hTwoSVPt      = new TH1F("twosvpt"    , Form("%s: SV Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoSVM       = new TH1F("twosvm"     , Form("%s: SV M"    ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoSVEta     = new TH1F("twosveta"   , Form("%s: SV Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hTwoSVDeltaPt      = new TH1F("twosvdeltapt"    , Form("%s: SV Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hTwoSVDeltaP       = new TH1F("twosvdeltap"     , Form("%s: SV Delta P"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoSVDeltaE       = new TH1F("twosvdeltae"     , Form("%s: SV Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoSVDeltaEta     = new TH1F("twosvdeltaeta"   , Form("%s: SV Delta Eta"  ,dirname)  , 200, -10., 10.);

      fLepHist[i]->hD0Diff        = new TH1F("d0diff"      , Form("%s: 2 D0 - 1 D0"      ,dirname)  , 400,-0.2, 0.2);

      //2D distributions
      fLepHist[i]->hTwoPtVsOnePt      = new TH2F("twoptvsonept", Form("%s: Two pT vs One pT", dirname), 200, 0, 200, 200, 0, 200);
    }
  }
}

void ZTauTauHistMaker::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      fTrees[i] = new TTree(Form("tree_%i",i),Form("ZTauTauHistMaker TTree %i",i));
      fTrees[i]->Branch("leponept",        &fTreeVars.leponept       );   
      fTrees[i]->Branch("leponem",         &fTreeVars.leponem	     );   
      fTrees[i]->Branch("leponeeta",       &fTreeVars.leponeeta      );  
      fTrees[i]->Branch("leponed0",        &fTreeVars.leponed0       );  
      fTrees[i]->Branch("leponeiso",       &fTreeVars.leponeiso      );  
      fTrees[i]->Branch("leptwopt",        &fTreeVars.leptwopt       );  
      fTrees[i]->Branch("leptwom",         &fTreeVars.leptwom	     );   
      fTrees[i]->Branch("leptwoeta",       &fTreeVars.leptwoeta      );  
      fTrees[i]->Branch("leptwod0",        &fTreeVars.leptwod0       );  
      fTrees[i]->Branch("leptwoiso",       &fTreeVars.leptwoiso      );  
      fTrees[i]->Branch("lepp",            &fTreeVars.lepp	     );   
      fTrees[i]->Branch("leppt",           &fTreeVars.leppt	     );   
      fTrees[i]->Branch("lepm",            &fTreeVars.lepm	     );   
      fTrees[i]->Branch("lepeta",          &fTreeVars.lepeta	     );   
      fTrees[i]->Branch("lepdeltaeta",     &fTreeVars.lepdeltaeta    );  
      fTrees[i]->Branch("lepdeltar",       &fTreeVars.lepdeltar      );  
      fTrees[i]->Branch("lepdeltaphi",     &fTreeVars.lepdeltaphi    );  
      fTrees[i]->Branch("htdeltaphi",      &fTreeVars.htdeltaphi     );  
      fTrees[i]->Branch("metdeltaphi",     &fTreeVars.metdeltaphi    );  
      fTrees[i]->Branch("leponedeltaphi",  &fTreeVars.leponedeltaphi );  
      fTrees[i]->Branch("leptwodeltaphi",  &fTreeVars.leptwodeltaphi );  
      fTrees[i]->Branch("onemetdeltaphi",  &fTreeVars.onemetdeltaphi );  
      fTrees[i]->Branch("twometdeltaphi",  &fTreeVars.twometdeltaphi );  
      fTrees[i]->Branch("lepmestimate",    &fTreeVars.mestimate	     );   
      fTrees[i]->Branch("lepmestimatetwo", &fTreeVars.mestimatetwo   );   
      fTrees[i]->Branch("met",             &fTreeVars.met            );
      fTrees[i]->Branch("mtone",           &fTreeVars.mtone	     );   
      fTrees[i]->Branch("mttwo",           &fTreeVars.mttwo	     );   
      fTrees[i]->Branch("pxivis",          &fTreeVars.pxivis	     );   
      fTrees[i]->Branch("pxiinv",          &fTreeVars.pxiinv	     );   
      fTrees[i]->Branch("ht",              &fTreeVars.ht	     );   
      fTrees[i]->Branch("htsum",           &fTreeVars.htsum	     );   
      fTrees[i]->Branch("njets",           &fTreeVars.njets	     );   
      fTrees[i]->Branch("nbjets",          &fTreeVars.nbjets	     );   
      fTrees[i]->Branch("nbjetsm",         &fTreeVars.nbjetsm	     );   
      fTrees[i]->Branch("nbjetsl",         &fTreeVars.nbjetsl	     );   
      fTrees[i]->Branch("nphotons",        &fTreeVars.nphotons       );  
      
      fTrees[i]->Branch("eventweight",     &fTreeVars.eventweight    );  
      fTrees[i]->Branch("fulleventweight", &fTreeVars.fulleventweight);
      fTrees[i]->Branch("eventcategory",   &fTreeVars.eventcategory  );
      fTrees[i]->Branch("train",           &fTreeVars.train          );
    }
  }
}

//selections: 1 = mutau, 2 = etau, 5 = emu, 9 = mumu, 18 = llg_study
void ZTauTauHistMaker::InitializeTreeVariables(Int_t selection) {
  nJets25Tot = nJets + nJets25;
  nBJets25Tot = nBJets + nBJets25;
  nBJets25TotM = nBJetsM + nBJets25M;
  nBJets25TotL = nBJetsL + nBJets25L;
  nJetsTot = nJets + nJets25 + nJets20;
  nBJetsTot = nBJets + nBJets25 + nBJets20;
  nBJetsTotM = nBJetsM + nBJets25M + nBJets20M;
  nBJetsTotL = nBJetsL + nBJets25L + nBJets20L;
  fTreeVars.leponept  = leptonOneP4->Pt();
  fTreeVars.leponem   = leptonOneP4->M();
  fTreeVars.leponeeta = leptonOneP4->Eta();
  fTreeVars.leponed0  = leptonOneD0;
  fTreeVars.leponeiso = leptonOneIso;
  fTreeVars.leptwopt  = leptonTwoP4->Pt();
  fTreeVars.leptwom   = leptonTwoP4->M();
  fTreeVars.leptwoeta = leptonTwoP4->Eta();
  fTreeVars.leptwod0  = leptonTwoD0;
  fTreeVars.leptwoiso = leptonTwoIso;
  TLorentzVector lep = *leptonOneP4 + *leptonTwoP4;
  fTreeVars.lepp   = lep.P();
  fTreeVars.leppt  = lep.Pt();
  fTreeVars.lepm   = lep.M();
  fTreeVars.lepeta = lep.Eta();
  fTreeVars.lepdeltar   = leptonOneP4->DeltaR(*leptonTwoP4);
  fTreeVars.lepdeltaphi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  fTreeVars.lepdeltaeta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  //phi differences
  fTreeVars.htdeltaphi = abs(lep.Phi() - htPhi);
  if(fTreeVars.htdeltaphi > M_PI)
    fTreeVars.htdeltaphi = abs(2.*M_PI - fTreeVars.htdeltaphi);
  fTreeVars.metdeltaphi = abs(lep.Phi() - metPhi);
  if(fTreeVars.metdeltaphi > M_PI)
    fTreeVars.metdeltaphi = abs(2.*M_PI - fTreeVars.metdeltaphi);
  fTreeVars.leponedeltaphi = abs(leptonOneP4->Phi() - lep.Phi());
  if(fTreeVars.leponedeltaphi > M_PI)
    fTreeVars.leponedeltaphi = abs(2.*M_PI - fTreeVars.leponedeltaphi);
  fTreeVars.leptwodeltaphi = abs(leptonTwoP4->Phi() - lep.Phi());
  if(fTreeVars.leptwodeltaphi > M_PI)
    fTreeVars.leptwodeltaphi = abs(2.*M_PI - fTreeVars.leptwodeltaphi);
  fTreeVars.onemetdeltaphi = abs(leptonOneP4->Phi() - metPhi);
  if(fTreeVars.onemetdeltaphi > M_PI)
    fTreeVars.onemetdeltaphi = abs(2.*M_PI - fTreeVars.onemetdeltaphi);
  fTreeVars.twometdeltaphi = abs(leptonTwoP4->Phi() - metPhi);
  if(fTreeVars.twometdeltaphi > M_PI)
    fTreeVars.twometdeltaphi = abs(2.*M_PI - fTreeVars.twometdeltaphi);

  //MET variables
  fTreeVars.met = met;
  fTreeVars.mtone = sqrt(2.*met*leptonOneP4->Pt()*(1.-cos(leptonOneP4->Phi() - metPhi)));
  fTreeVars.mttwo = sqrt(2.*met*leptonTwoP4->Pt()*(1.-cos(leptonTwoP4->Phi() - metPhi)));

  //momentum projections onto bisector
  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector.Mag() > 0.) bisector.SetMag(1.);
  
  //project onto the bisectors
  fTreeVars.pxivis = (lp1+lp2)*bisector;
  fTreeVars.pxiinv = missing*bisector;
  double pnuest    = max(0.,lp2*missing/lp2.Mag()); //inv pT along tau = tau pt unit vector dot missing
  double pnuesttwo = max(0.,lp1*missing/lp1.Mag()); //inv pT along other lepton (for emu case with tau decay)
  fTreeVars.ptauvisfrac  = lp2.Mag() / (lp2.Mag() + pnuest);
  fTreeVars.mestimate    = fTreeVars.lepm/sqrt(fTreeVars.ptauvisfrac);
  fTreeVars.mestimatetwo = fTreeVars.lepm/sqrt(lp1.Mag() / (lp1.Mag() + pnuesttwo));
  
  //event variables
  fTreeVars.ht       = ht;
  fTreeVars.htsum    = htSum;
  fTreeVars.njets    = nJets;
  fTreeVars.nbjets   = nBJets;
  fTreeVars.nbjetsm  = nBJetsM;
  fTreeVars.nbjetsl  = nBJetsL;
  fTreeVars.nphotons = nPhotons;
  fTreeVars.eventweight = genWeight*eventWeight;
  fTreeVars.fulleventweight = genWeight*eventWeight*fXsec;
  if(fUseTauFakeSF) fTreeVars.fulleventweight *= genTauFlavorWeight;
  if(fUseTauFakeSF) fTreeVars.eventweight *= genTauFlavorWeight;
  
  fTreeVars.eventcategory = fEventCategory;
  if(fFractionMVA > 0.) fTreeVars.train = (fRnd->Uniform() < fFractionMVA) ? 1. : -1.; //whether or not it is in the training sample
  
  TString selecName = "";
  if(selection == 1)      selecName = "mutau";
  else if(selection == 2) selecName = "etau";
  else if(selection == 5) selecName = "emu";
  else                    selecName = "unknown";
  if(fReprocessMVAs) {
    for(unsigned i = 0; i < fMvaNames.size(); ++i) {
      if((fMvaNames[i].Contains(selecName.Data()) || //is this selection
	  (selecName == "emu" && (fMvaNames[i].Contains("_e") || fMvaNames[i].Contains("_mu")))) && //or leptonic tau category
	 (fIsJetBinnedMVAs[i] < 0 || fIsJetBinnedMVAs[i] == min((int) nJets,2))) //and either not jet binned or right number of jets
	fMvaOutputs[i] = mva[i]->EvaluateMVA(fMvaNames[i].Data());
      else
	fMvaOutputs[i] = -2.;
      
      if(fMvaOutputs[i] < -100.) 
	cout << "Error value returned for MVA " << fMvaNames[i].Data()
	     << " evaluation, Entry = " << fentry << endl;
    }
  }
}

float ZTauTauHistMaker::GetTauFakeSF(int genFlavor) {
  float weight = 1.;
  switch(abs(genFlavor)) {
  case 15 : weight = 0.95; break;
  case 11 : weight = genTauFlavorWeight; break;
  case 13 : weight = genTauFlavorWeight; break;
  }
  //jets are given weight 1
  return weight;
}

float ZTauTauHistMaker::CorrectMET(int selection, float met) {
  float corrected = met;
  
  // switch(abs(selection)) {
  // case 0 : weight = (1.1 - 0.2/150.*met); break; // mu+tau
  // case 1 : weight = 1.; break; // e+tau
  // case 2 : weight = 1.; break; // e+mu
  // }
  return corrected;
}

// re-weight the AMC@NLO DY Z pT
float ZTauTauHistMaker::GetZPtWeight(float pt) {
  float weight = 1.;
  if(pt >= 140.)
    weight = 0.891188;
  else {
    weight = (0.876979 + pt*(4.11598e-3 - pt*2.35520e-5));
    weight *= 1.10211*(0.958512-0.131835*erf((pt-14.1972)/10.1525));
  }
  if(weight < 0.) {
    printf("Warning! Z pT weight < 0: weight = %.3e, pt = %.2f\n", weight, pt);
  }
  return weight;
}

void ZTauTauHistMaker::FillEventHistogram(EventHist_t* Hist) {
  Hist->hLumiSection         ->Fill(lumiSection        , genWeight*eventWeight)      ;
  // Hist->hTriggerStatus       ->Fill(triggerStatus      , genWeight*eventWeight)      ;
  Hist->hEventWeight         ->Fill(eventWeight        );
  Hist->hGenWeight           ->Fill(genWeight          );
  Hist->hGenTauFlavorWeight  ->Fill(genTauFlavorWeight );
  Hist->hNPV                 ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hNPU                 ->Fill(nPU                , genWeight*eventWeight)      ;
  Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
  Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
  Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
  Hist->hNLowPtElectrons     ->Fill(nLowPtElectrons    , genWeight*eventWeight)      ;
  Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  Hist->hNGenTausHad         ->Fill(nGenTausHad        , genWeight*eventWeight)      ;
  Hist->hNGenTausLep         ->Fill(nGenTausLep        , genWeight*eventWeight)      ;
  Hist->hNGenTaus            ->Fill(nGenTausLep+nGenTausHad, genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons      , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons          , genWeight*eventWeight)      ;
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  Hist->hNJets25             ->Fill(nJets25            , genWeight*eventWeight)      ;
  Hist->hNJets20             ->Fill(nJets20            , genWeight*eventWeight)      ;
  Hist->hNJetsTot            ->Fill(nJetsTot           , genWeight*eventWeight)      ;
  Hist->hNJets25Tot          ->Fill(nJets25Tot         , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets              ->Fill(nBJets             , genWeight*eventWeight)      ;
  Hist->hNBJetsM             ->Fill(nBJetsM            , genWeight*eventWeight)      ;
  Hist->hNBJetsL             ->Fill(nBJetsL            , genWeight*eventWeight)      ;
  Hist->hNBJets25            ->Fill(nBJets25           , genWeight*eventWeight)      ;
  Hist->hNBJets25M           ->Fill(nBJets25M          , genWeight*eventWeight)      ;
  Hist->hNBJets25L           ->Fill(nBJets25L          , genWeight*eventWeight)      ;
  Hist->hNBJets20            ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20M           ->Fill(nBJets20M          , genWeight*eventWeight)      ;
  Hist->hNBJets20L           ->Fill(nBJets20L          , genWeight*eventWeight)      ;
  Hist->hNBJets25Tot         ->Fill(nBJets25Tot        , genWeight*eventWeight)      ;
  Hist->hNBJets25TotM        ->Fill(nBJets25TotM       , genWeight*eventWeight)      ;
  Hist->hNBJets25TotL        ->Fill(nBJets25TotL       , genWeight*eventWeight)      ;
  Hist->hNBJetsTot           ->Fill(nBJetsTot          , genWeight*eventWeight)      ;
  Hist->hNBJetsTotM          ->Fill(nBJetsTotM         , genWeight*eventWeight)      ;
  Hist->hNBJetsTotL          ->Fill(nBJetsTotL         , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra              , genWeight*eventWeight)   ;
  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hPuWeight	     ->Fill(puWeight	       , genWeight*eventWeight)   ;
  Hist->hTopPtWeight	     ->Fill(topPtWeight	       , genWeight*eventWeight)   ;
  Hist->hZPtWeight	     ->Fill(zPtWeight	       , genWeight*eventWeight)   ;
  Hist->hTauDecayMode	     ->Fill(tauDecayMode       , genWeight*eventWeight)   ;
  Hist->hTauMVA		     ->Fill(tauMVA	       , genWeight*eventWeight)   ;
  Hist->hTauGenFlavor	     ->Fill(tauGenFlavor       , genWeight*eventWeight)   ;
  Hist->hTauGenFlavorHad     ->Fill(tauGenFlavorHad    , genWeight*eventWeight)   ;
  Hist->hTauVetoedJetPt	     ->Fill(tauVetoedJetPt     , genWeight*eventWeight)   ;
  Hist->hTauVetoedJetPtUnc   ->Fill(tauVetoedJetPtUnc  , genWeight*eventWeight)   ;
  Hist->hHtSum		     ->Fill(htSum	       , genWeight*eventWeight)   ;
  Hist->hHt		     ->Fill(ht		       , genWeight*eventWeight)   ;
  Hist->hHtPhi               ->Fill(htPhi              , genWeight*eventWeight)   ;

  if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt               ->Fill(jetOneP4->Pt()        , genWeight*eventWeight)   ;
    Hist->hJetM                ->Fill(jetOneP4->M()         , genWeight*eventWeight)   ;
    Hist->hJetEta              ->Fill(jetOneP4->Eta()       , genWeight*eventWeight)   ;
    Hist->hJetPhi              ->Fill(jetOneP4->Phi()       , genWeight*eventWeight)   ;
  }

  if(tauP4 && tauP4->Pt() > 0.) { //if 0 then no tau stored
    Hist->hTauPt               ->Fill(tauP4->Pt()        , genWeight*eventWeight)   ;
    Hist->hTauM                ->Fill(tauP4->M()         , genWeight*eventWeight)   ;
    Hist->hTauEta              ->Fill(tauP4->Eta()       , genWeight*eventWeight)   ;
    Hist->hTauPhi              ->Fill(tauP4->Phi()       , genWeight*eventWeight)   ;
  }
  
  Hist->hPuppMet             ->Fill(puppMETC              , genWeight*eventWeight)      ;
  Hist->hPFMet               ->Fill(pfMETC                , genWeight*eventWeight)      ;
  Hist->hPFMetPhi            ->Fill(pfMETCphi             , genWeight*eventWeight)      ;
  Hist->hPFCovMet00          ->Fill(pfMETCCov00           , genWeight*eventWeight)   ;
  Hist->hPFCovMet01	     ->Fill(pfMETCCov01           , genWeight*eventWeight)   ; 
  Hist->hPFCovMet11	     ->Fill(pfMETCov11           , genWeight*eventWeight)   ; 
  // Hist->hTrkMet              ->Fill(trkMET                , genWeight*eventWeight)      ;
  // Hist->hTrkMetPhi           ->Fill(trkMETphi             , genWeight*eventWeight)      ;

  Hist->hMet                 ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMetPhi              ->Fill(metPhi             , genWeight*eventWeight)      ;
  Hist->hMetCorr             ->Fill(metCorr            , genWeight*eventWeight)      ;
  Hist->hMetCorrPhi          ->Fill(metCorrPhi         , genWeight*eventWeight)      ;
  Hist->hCovMet00            ->Fill(covMet00           , genWeight*eventWeight)   ;
  Hist->hCovMet01	     ->Fill(covMet01           , genWeight*eventWeight)   ; 
  Hist->hCovMet11	     ->Fill(covMet11           , genWeight*eventWeight)   ; 
  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  Hist->hMetVsPt             ->Fill(lepSys.Pt(), met   , genWeight*eventWeight)   ; 
  Hist->hMetVsM              ->Fill(lepSys.M(), met    , genWeight*eventWeight)   ; 
  Hist->hMetVsHtSum          ->Fill(htSum     , met    , genWeight*eventWeight)   ; 
  if(htSum > 0.) //avoid divide by 0 issue
    Hist->hMetOverSqrtHtSum  ->Fill(met/sqrt(htSum)    , genWeight*eventWeight)   ; 

  Hist->hMassSVFit	     ->Fill(massSVFit          , genWeight*eventWeight)   ; 
  Hist->hMassErrSVFit        ->Fill(massErrSVFit       , genWeight*eventWeight)   ;
  Hist->hSVFitStatus         ->Fill(svFitStatus        , genWeight*eventWeight)   ;


  TLorentzVector sys    = (*photonP4) + lepSys;
  TLorentzVector svLepSys = (leptonOneSVP4 && leptonTwoSVP4) ? (*leptonOneSVP4) + (*leptonTwoSVP4) : TLorentzVector(0.,0.,0.,0.);
  TLorentzVector svSys    = (leptonOneSVP4 && leptonTwoSVP4) ? (*photonP4) + svLepSys              : TLorentzVector(0.,0.,0.,0.);
  
  float lepDelR   = leptonOneP4->DeltaR(*leptonTwoP4);
  float lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  float lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  float htDelPhi  = abs(lepSys.Phi() - htPhi);
  float jetDelPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(lepSys.DeltaPhi(*jetOneP4)) : -1.;
  if(htDelPhi > M_PI)
    htDelPhi = abs(2.*M_PI - htDelPhi);
  float metDelPhi  = abs(lepSys.Phi() - metPhi);
  if(metDelPhi > M_PI)
    metDelPhi = abs(2.*M_PI - metDelPhi);
  float lepOneDelPhi  = abs(lepSys.Phi() - leptonOneP4->Phi());
  if(lepOneDelPhi > M_PI)
    lepOneDelPhi = abs(2.*M_PI - lepOneDelPhi);
  float lepTwoDelPhi  = abs(lepSys.Phi() - leptonTwoP4->Phi());
  if(lepTwoDelPhi > M_PI)
    lepTwoDelPhi = abs(2.*M_PI - lepTwoDelPhi);

  //angles between leptons and leading jet
  float lepOneJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonOneP4->DeltaR(*jetOneP4)            : -1.;
  float lepOneJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonOneP4->DeltaPhi(*jetOneP4))     : -1.;
  float lepOneJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonOneP4->Eta() - jetOneP4->Eta()) : -1.;
  float lepTwoJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonTwoP4->DeltaR(*jetOneP4)            : -1.;
  float lepTwoJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonTwoP4->DeltaPhi(*jetOneP4))     : -1.;
  float lepTwoJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonTwoP4->Eta() - jetOneP4->Eta()) : -1.;

  float lepSVDelR   = -1.;
  float lepSVDelPhi = -1.;
  float lepSVDelEta = -1.;
  float lepSVDelM   = -20.;
  float lepSVDelPt  = -20.;
  if(leptonOneSVP4 && leptonTwoSVP4
     && leptonOneSVP4->Pt() > 1.e-5 && leptonTwoSVP4->Pt() > 1.e-5) {
    lepSVDelR   = leptonOneSVP4->DeltaR(*leptonTwoSVP4)           ;
    lepSVDelPhi = abs(leptonOneSVP4->DeltaPhi(*leptonTwoSVP4))    ;
    lepSVDelEta = abs(leptonOneSVP4->Eta() - leptonTwoSVP4->Eta());
    lepSVDelM = svLepSys.M() - lepSys.M();
    lepSVDelPt = svLepSys.Pt() - lepSys.Pt();
  }
  Hist->hLepPt        ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepP         ->Fill(lepSys.P()             ,eventWeight*genWeight);
  Hist->hLepM         ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
						    		
  Hist->hLepDeltaPhi  ->Fill(lepDelPhi              ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR    ->Fill(lepDelR                ,eventWeight*genWeight);
  Hist->hLepDelRVsPhi ->Fill(lepDelR , lepDelPhi    ,eventWeight*genWeight);
  
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);
  Hist->hHtDeltaPhi   ->Fill(htDelPhi               ,eventWeight*genWeight);
  Hist->hMetDeltaPhi  ->Fill(metDelPhi              ,eventWeight*genWeight);
  if(jetDelPhi >= 0.) Hist->hJetDeltaPhi  ->Fill(jetDelPhi,eventWeight*genWeight);
  Hist->hLepOneDeltaPhi->Fill(lepOneDelPhi          ,eventWeight*genWeight);
  Hist->hLepTwoDeltaPhi->Fill(lepTwoDelPhi          ,eventWeight*genWeight);

  if(jetOneP4 && jetOneP4->Pt() > 0.) { //only fill if there's at least one jet
    Hist->hLepOneJetDeltaR  ->Fill(lepOneJetDeltaR  ,eventWeight*genWeight);
    Hist->hLepOneJetDeltaPhi->Fill(lepOneJetDeltaPhi,eventWeight*genWeight);
    Hist->hLepOneJetDeltaEta->Fill(lepOneJetDeltaEta,eventWeight*genWeight);
    Hist->hLepTwoJetDeltaR  ->Fill(lepTwoJetDeltaR  ,eventWeight*genWeight);
    Hist->hLepTwoJetDeltaPhi->Fill(lepTwoJetDeltaPhi,eventWeight*genWeight);
    Hist->hLepTwoJetDeltaEta->Fill(lepTwoJetDeltaEta,eventWeight*genWeight);
  }
  
  Hist->hLepSVPt      ->Fill(svLepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepSVM       ->Fill(svLepSys.M()             ,eventWeight*genWeight);
  Hist->hLepSVEta     ->Fill(((svLepSys.Pt() > 1.e-5) ? svLepSys.Eta()  : -1e6),eventWeight*genWeight);
  Hist->hLepSVPhi     ->Fill(svLepSys.Phi()           ,eventWeight*genWeight);

  Hist->hLepSVDeltaPhi ->Fill(lepSVDelPhi              ,eventWeight*genWeight);
  Hist->hLepSVDeltaEta ->Fill(lepSVDelEta              ,eventWeight*genWeight);
  Hist->hLepSVDeltaM   ->Fill(lepSVDelM                ,eventWeight*genWeight);
  Hist->hLepSVDeltaPt  ->Fill(lepSVDelPt               ,eventWeight*genWeight);
  Hist->hLepSVPtOverM  ->Fill(svLepSys.Pt()/svLepSys.M() ,eventWeight*genWeight);
  
  		                
  Hist->hSysM          ->Fill(sys.M()      ,eventWeight*genWeight);
  Hist->hSysPt         ->Fill(sys.Pt()     ,eventWeight*genWeight);
  Hist->hSysEta        ->Fill(sys.Eta()    ,eventWeight*genWeight);  
  Hist->hSysMvsLepM    ->Fill(lepSys.M(), sys.M(), eventWeight*genWeight);
  
  Hist->hMTOne         ->Fill(fTreeVars.mtone  ,eventWeight*genWeight);
  Hist->hMTTwo         ->Fill(fTreeVars.mttwo  ,eventWeight*genWeight);

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  TVector3 pp  = photonP4->Vect();
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  pp.SetZ(0.);
  TVector3 bisector0 = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector0.Mag() > 0.) bisector0.SetMag(1.);

  //combine photon with nearest lepton
  TVector3 lpp = (leptonOneP4->DeltaR(*photonP4) > leptonTwoP4->DeltaR(*photonP4)) ? lp2 : lp1; 
  //  (lp1 + lp2 - lpp) = not nearest to photon lepton
  TVector3 bisector1 = ((lpp+pp).Mag()*(lp1+lp2-lpp) + (lpp+pp)*((lp1+lp2-lpp).Mag())); //divides lepton + photon and lepton
  if(bisector1.Mag() > 0.) bisector1.SetMag(1.);

  //combine leptons together
  TVector3 bisector2 = (pp.Mag())*(lp1+lp2) + (lp1+lp2).Mag()*pp; //divides leptons and photon
  if(bisector2.Mag() > 0.) bisector2.SetMag(1.); //mag = 0 often since no photon often

  //project onto the bisectors
  //ignore photon
  double pxi_vis0 = (lp1+lp2)*bisector0;
  double pxi_inv0 = missing*bisector0;
  //add photon in
  double pxi_vis1 = (lp1+lp2+pp)*bisector1;
  double pxi_inv1 = missing*bisector1;
  double pxi_vis2 = (lp1+lp2+pp)*bisector2;
  double pxi_inv2 = missing*bisector2;

  Hist->hPXiVis[0]     ->Fill(pxi_vis0         ,eventWeight*genWeight);
  Hist->hPXiInv[0]     ->Fill(pxi_inv0         ,eventWeight*genWeight);
  Hist->hPXiVisOverInv[0]->Fill((pxi_inv0 != 0.) ? pxi_vis0/pxi_inv0 : 1.e6 ,eventWeight*genWeight);
  Hist->hPXiInvVsVis[0]->Fill(pxi_vis0, pxi_inv0,eventWeight*genWeight);
  Hist->hPXiDiff[0]    ->Fill(pxi_vis0-pxi_inv0 ,eventWeight*genWeight);

  Hist->hPTauVisFrac    ->Fill(fTreeVars.ptauvisfrac , eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);

  if(nPhotons > 0) {
    Hist->hPXiVis[1]     ->Fill(pxi_vis1         ,eventWeight*genWeight);
    Hist->hPXiInv[1]     ->Fill(pxi_inv1         ,eventWeight*genWeight);
    Hist->hPXiVisOverInv[1]->Fill((pxi_inv1 != 0.) ? pxi_vis1/pxi_inv1 : 1.e6 ,eventWeight*genWeight);
    Hist->hPXiInvVsVis[1]->Fill(pxi_vis1, pxi_inv1,eventWeight*genWeight);
    Hist->hPXiDiff[1]    ->Fill(pxi_vis1-pxi_inv1 ,eventWeight*genWeight);
    Hist->hPXiVis[2]     ->Fill(pxi_vis2         ,eventWeight*genWeight);
    Hist->hPXiInv[2]     ->Fill(pxi_inv2         ,eventWeight*genWeight);
    Hist->hPXiVisOverInv[2]->Fill((pxi_inv2 != 0.) ? pxi_vis2/pxi_inv2 : 1.e6 ,eventWeight*genWeight);
    Hist->hPXiInvVsVis[2]->Fill(pxi_vis2, pxi_inv2,eventWeight*genWeight);
    Hist->hPXiDiff[2]    ->Fill(pxi_vis2-pxi_inv2 ,eventWeight*genWeight);
  }
  
  double coeff = 0.85 - 1.; // match 2016 clfv higgs paper (defined pxi inv = xi dot (lep1 + lep2 + met) = my pxi inv + pxi vis)
  double offset = -60.;
  Hist->hPXiDiff3[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);
  Hist->hPXiDiff3[1]   ->Fill(pxi_inv1 - (coeff*pxi_vis1+offset) ,eventWeight*genWeight);
  Hist->hPXiDiff3[2]   ->Fill(pxi_inv2 - (coeff*pxi_vis2+offset) ,eventWeight*genWeight);

  coeff = 0.6;
  offset = -40.;
  Hist->hPXiDiff2[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);
  Hist->hPXiDiff2[1]   ->Fill(pxi_inv1 - (coeff*pxi_vis1+offset) ,eventWeight*genWeight);
  Hist->hPXiDiff2[2]   ->Fill(pxi_inv2 - (coeff*pxi_vis2+offset) ,eventWeight*genWeight);

  Hist->hPtSum[0]      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPtSum[1]      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+photonP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPt1Sum[0]     ->Fill(leptonOneP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPt1Sum[1]     ->Fill(leptonTwoP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPt1Sum[2]     ->Fill(leptonTwoP4->Pt()+leptonTwoP4->Pt() ,eventWeight*genWeight);
  Hist->hPt1Sum[3]     ->Fill(leptonTwoP4->Pt()+leptonTwoP4->Pt()-met ,eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMvaNames.size(); ++i) {
    Hist->hMVA[i]   ->Fill(fMvaOutputs[i], eventWeight*genWeight);
  }
  if(fFolderName != "llg_study") return;
  TLorentzVector jets = *jetOneP4 + *jetTwoP4, jet1g = *jetOneP4 + *photonP4, jet2g = *jetTwoP4 + *photonP4;
  TLorentzVector jetsg = jets + *photonP4, jet1l1 = *jetOneP4 + *leptonOneP4, jet1l2 = *jetOneP4 + *leptonTwoP4;
  TLorentzVector jet2l1 = *jetTwoP4 + *leptonOneP4, jet2l2 = *jetTwoP4 + *leptonTwoP4;
  TLorentzVector jetsl1 = jets + *leptonOneP4, jetsl2 = jets + *leptonTwoP4;
  TLorentzVector jetsl1g = jetsl1 + *photonP4, jetsl2g = jetsl2 + *photonP4;
  TLorentzVector jetsllg = jetsl1g + *leptonTwoP4;
  
  Hist->hObjMasses[0 ]     ->Fill(jets.M() ,eventWeight*genWeight);
  Hist->hObjMasses[1 ]     ->Fill(jet1g.M() ,eventWeight*genWeight);
  Hist->hObjMasses[2 ]     ->Fill(jet2g.M() ,eventWeight*genWeight);
  Hist->hObjMasses[3 ]     ->Fill(jetsg.M() ,eventWeight*genWeight);
  Hist->hObjMasses[4 ]     ->Fill(jet1l1.M() ,eventWeight*genWeight);
  Hist->hObjMasses[5 ]     ->Fill(jet2l1.M() ,eventWeight*genWeight);
  Hist->hObjMasses[6 ]     ->Fill(jet1l2.M() ,eventWeight*genWeight);
  Hist->hObjMasses[7 ]     ->Fill(jet2l2.M() ,eventWeight*genWeight);
  Hist->hObjMasses[8 ]     ->Fill(jetsl1.M() ,eventWeight*genWeight);
  Hist->hObjMasses[9 ]     ->Fill(jetsl2.M() ,eventWeight*genWeight);
  Hist->hObjMasses[10]     ->Fill(jetsl1g.M() ,eventWeight*genWeight);
  Hist->hObjMasses[11]     ->Fill(jetsl2g.M() ,eventWeight*genWeight);
  Hist->hObjMasses[12]     ->Fill(jetsllg.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[13]     ->Fill( ,eventWeight*genWeight);
  Hist->hJetTwoM           ->Fill(jetTwoP4->M(),eventWeight*genWeight);
  Hist->hJetTwoPt          ->Fill(jetTwoP4->Pt(),eventWeight*genWeight);
  Hist->hJetTwoEta         ->Fill(jetTwoP4->Eta(),eventWeight*genWeight);
  Hist->hJetsDeltaR        ->Fill(jetTwoP4->DeltaR(*jetOneP4),eventWeight*genWeight);
  Hist->hJetsDeltaEta      ->Fill(abs(jetTwoP4->Eta()-jetOneP4->Eta()),eventWeight*genWeight);
  Hist->hJetsDeltaPhi      ->Fill(abs(jetTwoP4->DeltaPhi(*jetOneP4)),eventWeight*genWeight);
  Hist->hJetsPt            ->Fill(jets.Pt() ,eventWeight*genWeight);
  Hist->hJetsEta           ->Fill(jets.Eta() ,eventWeight*genWeight);
  Hist->hJetsGammaDeltaR   ->Fill(jets.DeltaR(*photonP4) ,eventWeight*genWeight);
  Hist->hJetsGammaDeltaEta ->Fill(abs(jets.Eta() - photonP4->Eta()) ,eventWeight*genWeight);
  Hist->hJetsGammaDeltaPhi ->Fill(abs(jets.DeltaPhi(*photonP4)) ,eventWeight*genWeight);
  Hist->hJetsGammaPt       ->Fill(jetsg.Pt() ,eventWeight*genWeight);
  Hist->hJetsGammaEta      ->Fill(jetsg.Eta() ,eventWeight*genWeight);
}

void ZTauTauHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  Hist->hPz  ->Fill(photonP4->Pz() , eventWeight*genWeight );
  Hist->hPt  ->Fill(photonP4->Pt() , eventWeight*genWeight );
  Hist->hP   ->Fill(photonP4->P()  , eventWeight*genWeight );
  Hist->hEta ->Fill((photonP4->Pt() > 0.) ? photonP4->Eta() : -1.e6, eventWeight*genWeight );
  Hist->hPhi ->Fill(photonP4->Phi(), eventWeight*genWeight );
  Hist->hMVA ->Fill(photonMVA, eventWeight*genWeight );
  // Hist->hIso       ;
  // Hist->hRelIso    ;
  // Hist->hTrigger   ;
}

void ZTauTauHistMaker::FillLepHistogram(LepHist_t* Hist) {
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()      ,eventWeight*genWeight);
  Hist->hOnePt        ->Fill(leptonOneP4->Pt()      ,eventWeight*genWeight);
  Hist->hOneP         ->Fill(leptonOneP4->P()       ,eventWeight*genWeight);
  Hist->hOneM         ->Fill(leptonOneP4->M()       ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()     ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOneP4->Phi()     ,eventWeight*genWeight);
  Hist->hOneD0        ->Fill(leptonOneD0            ,eventWeight*genWeight);
  Hist->hOneIso       ->Fill(leptonOneIso           ,eventWeight*genWeight);
  float relIso1 = leptonOneIso/ leptonOneP4->Pt();
  Hist->hOneRelIso    ->Fill(relIso1                ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(fabs(leptonOneFlavor)        ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  // Hist->hOneTrigger   ->Fill(leptonOneTrigger       ,eventWeight*genWeight);

  if(genLeptonOneP4) {
    Hist->hOneGenPt       ->Fill(genLeptonOneP4->Pt()    ,eventWeight*genWeight);
    Hist->hOneGenE        ->Fill(genLeptonOneP4->E()     ,eventWeight*genWeight);
    Hist->hOneGenEta      ->Fill(genLeptonOneP4->Eta()   ,eventWeight*genWeight);
    Hist->hOneDeltaPt     ->Fill(leptonOneP4->Pt()  - genLeptonOneP4->Pt()    ,eventWeight*genWeight);
    Hist->hOneDeltaE      ->Fill(leptonOneP4->E()   - genLeptonOneP4->E()     ,eventWeight*genWeight);
    Hist->hOneDeltaEta    ->Fill(leptonOneP4->Eta() - genLeptonOneP4->Eta()   ,eventWeight*genWeight);
  }
  float oneMetDelPhi  = abs(leptonOneP4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI)
    oneMetDelPhi = abs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  if(leptonOneSVP4) {
    Hist->hOneSVPt       ->Fill(leptonOneSVP4->Pt()    ,eventWeight*genWeight);
    Hist->hOneSVM        ->Fill(leptonOneSVP4->M()     ,eventWeight*genWeight);
    if(leptonOneSVP4->Pt() > 1.e-5)
      Hist->hOneSVEta      ->Fill(leptonOneSVP4->Eta()   ,eventWeight*genWeight);
    else
      Hist->hOneSVEta      ->Fill(-1.e6   ,eventWeight*genWeight);
    Hist->hOneSVDeltaPt  ->Fill(leptonOneSVP4->Pt()  -  leptonOneP4->Pt()  ,eventWeight*genWeight);
    Hist->hOneSVDeltaP   ->Fill(leptonOneSVP4->P()   -  leptonOneP4->P()   ,eventWeight*genWeight);
    Hist->hOneSVDeltaE   ->Fill(leptonOneSVP4->E()   -  leptonOneP4->E()   ,eventWeight*genWeight);
    if(leptonOneSVP4->Pt() > 1.e-5)
      Hist->hOneSVDeltaEta ->Fill(leptonOneSVP4->Eta() -  leptonOneP4->Eta() ,eventWeight*genWeight);
    else
      Hist->hOneSVDeltaEta ->Fill(1.e6 ,eventWeight*genWeight);
  }  
  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()      ,eventWeight*genWeight);
  Hist->hTwoPt        ->Fill(leptonTwoP4->Pt()      ,eventWeight*genWeight);
  Hist->hTwoP         ->Fill(leptonTwoP4->P()       ,eventWeight*genWeight);
  Hist->hTwoM         ->Fill(leptonTwoP4->M()       ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()     ,eventWeight*genWeight);
  Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()     ,eventWeight*genWeight);
  Hist->hTwoD0        ->Fill(leptonTwoD0            ,eventWeight*genWeight);
  Hist->hTwoIso       ->Fill(leptonTwoIso           ,eventWeight*genWeight);
  float relIso2 = leptonTwoIso/ leptonTwoP4->Pt();
  Hist->hTwoRelIso    ->Fill(relIso2                ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(fabs(leptonTwoFlavor)        ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwoFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  // Hist->hTwoTrigger   ->Fill(leptonTwoTrigger       ,eventWeight*genWeight);

  if(genLeptonTwoP4) {
    Hist->hTwoGenPt       ->Fill(genLeptonTwoP4->Pt()    ,eventWeight*genWeight);
    Hist->hTwoGenE        ->Fill(genLeptonTwoP4->E()     ,eventWeight*genWeight);
    Hist->hTwoGenEta      ->Fill(genLeptonTwoP4->Eta()   ,eventWeight*genWeight);
    Hist->hTwoDeltaPt     ->Fill(leptonTwoP4->Pt()  - genLeptonTwoP4->Pt()    ,eventWeight*genWeight);
    Hist->hTwoDeltaE      ->Fill(leptonTwoP4->E()   - genLeptonTwoP4->E()     ,eventWeight*genWeight);
    Hist->hTwoDeltaEta    ->Fill(leptonTwoP4->Eta() - genLeptonTwoP4->Eta()   ,eventWeight*genWeight);
  }

  float twoMetDelPhi  = abs(leptonTwoP4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI)
    twoMetDelPhi = abs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  if(leptonTwoSVP4) {
    Hist->hTwoSVPt      ->Fill(leptonTwoSVP4->Pt()    ,eventWeight*genWeight);
    Hist->hTwoSVM       ->Fill(leptonTwoSVP4->M()     ,eventWeight*genWeight);
    if(leptonTwoSVP4->Pt() > 1.e-5)
      Hist->hTwoSVEta      ->Fill(leptonTwoSVP4->Eta()   ,eventWeight*genWeight);
    else
      Hist->hTwoSVEta      ->Fill(-1.e6   ,eventWeight*genWeight);
    Hist->hTwoSVDeltaPt  ->Fill(leptonTwoSVP4->Pt()  -  leptonTwoP4->Pt()  ,eventWeight*genWeight);
    Hist->hTwoSVDeltaP   ->Fill(leptonTwoSVP4->P()   -  leptonTwoP4->P()   ,eventWeight*genWeight);
    Hist->hTwoSVDeltaE   ->Fill(leptonTwoSVP4->E()   -  leptonTwoP4->E()   ,eventWeight*genWeight);
    if(leptonTwoSVP4->Pt() > 1.e-5)    
      Hist->hTwoSVDeltaEta ->Fill(leptonTwoSVP4->Eta() -  leptonTwoP4->Eta() ,eventWeight*genWeight);
    else
      Hist->hTwoSVDeltaEta ->Fill(-1.e6 ,eventWeight*genWeight);

  }

  Hist->hD0Diff      ->Fill(leptonTwoD0-leptonOneD0             ,eventWeight*genWeight);
  Hist->hTwoPtVsOnePt->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(),eventWeight*genWeight);
}


Bool_t ZTauTauHistMaker::Process(Long64_t entry)
{
  fentry = entry;
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

  //DY Splitting
  if(fDYType > 0) {
    // 1 = tau, 2 = muon or electron channel
    if(fDYType == 1 && nGenHardTaus < 2) return kTRUE; // gen counting currently odd, so ask at least 2
    else if(fDYType == 2 && ((nGenHardMuons + nGenHardElectrons) < 2
			     || nGenHardTaus >= 2)) return kTRUE;  //add not tau condition, to avoid double counting
    else if(nGenHardMuons + nGenHardElectrons < 2 && nGenHardTaus < 2) {
      cout << "Warning! Unable to identify type of DY event!" << endl
	   << "nGenHardTaus = " << nGenHardTaus << endl
	   << "nGenHardMuons = " << nGenHardMuons << endl
	   << "nGenHardElectrons = " << nGenHardElectrons << endl
	   << "fDYType = " << fDYType << endl
	   << "Entry " << fentry << endl;
      return kTRUE;
    }
  }

  //skip if it's data and lepton status doesn't match data set ( 1 = electron 2 = muon) unless allowing overlap and it passes both
  if(fIsData>0 && triggerLeptonStatus != ((UInt_t) fIsData) &&
     (fSkipDoubleTrigger || triggerLeptonStatus != 3)) return kTRUE; //avoid double counting data events and events triggered for the other dataset
  
  //add the met correction to the met
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  TVector3 missingPUPPI(puppMETC*cos(puppMETCphi), puppMETC*sin(puppMETCphi), 0.);
  TVector3 missingCorr(metCorr*cos(metCorrPhi), metCorr*sin(metCorrPhi), 0.);
  missing = missing + missingCorr;
  missingPUPPI = missingPUPPI + missingCorr;
  puppMETC = missingPUPPI.Mag();
  if(fMETType == 0) {
    met = missing.Mag();
    metPhi = missing.Phi();
  } else if(fMETType == 1) {
    met = missingPUPPI.Mag();
    metPhi = missingPUPPI.Phi();
  }


  // DY z pT weights
  if(fRemoveZPtWeights > 0 && zPtWeight > 0.) eventWeight /= zPtWeight;
  //only re-weight the DY sample
  if(fRemoveZPtWeights > 1 && fDYType > 0) {
    zPtWeight = (leptonTwoP4->E() > 0.1) ? GetZPtWeight((*leptonOneP4+*leptonTwoP4).Pt()) : GetZPtWeight(zPt);
    eventWeight *= zPtWeight;
  }

  //very different setup, process separately
  if(fFolderName == "llg_study") {
    ProcessLLGStudy();
    // return kTRUE;
  }
  
  //selections
  bool mutau = nTaus == 1  && nMuons == 1 && nElectrons == 0;
  bool etau  = nTaus == 1  && nMuons == 0 && nElectrons == 1;
  bool emu   = nTaus == 0  && nMuons == 1 && nElectrons == 1;
  bool mumu  = nMuons == 2; //no other requirement
  bool llg_study = (nElectrons + nMuons > 0) && nJets > 1 && nPhotons > 0 && (nElectrons + nMuons < 3);
  double l2pt_prev = leptonTwoP4->Pt();
  if(fFolderName == "llg_study" && llg_study && (nMuons + nElectrons == 1) && nTaus > 0) {
    *leptonTwoP4 = *tauP4;
    leptonTwoFlavor = tauFlavor;
  }
  if(!(mutau || etau || emu || mumu || llg_study))
    return kTRUE;

  InitializeTreeVariables(mutau+2*etau+5*emu+9*mumu+18*llg_study);

  //if splitting testing/training samples
  if(fFractionMVA > 0.)
    eventWeight *= (fTreeVars.train > 0.) ? 0. : 1./(1.-fFractionMVA); //if training, ignore, else rescale to account for training sample removed
  
  mumu &= fTreeVars.lepm > 15.; //DY MC not defined below 10 GeV/c^2

  //FIXME temporary fix to hadronic tau ID if is not data
  // if((mutau || etau)&&fIsData==0) {
  //   eventWeight *= 1./0.95; //remove overall tau ID weight that didn't include if gen tau or not in the total event weight
  //   if(abs(tauGenFlavor) == 15 && fUseTauFakeSF == 1) genTauFlavorWeight *= 0.95; //apply to real taus to original weights
  // }
  
  //use locally computed weight 
  if(fUseTauFakeSF > 1 && fIsData == 0) genTauFlavorWeight = GetTauFakeSF(tauGenFlavor);
  
  //apply fake tau SF
  if(fUseTauFakeSF && fIsData == 0) eventWeight *= genTauFlavorWeight;

  bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  if(bisector.Mag() > 0.) bisector.SetMag(1.);
  double pxi_vis = (lp1+lp2)*bisector;
  double pxi_inv = missing*bisector;
  
  // if(chargeTest) FillAllHistograms(1);
  // else           FillAllHistograms(1 + fQcdOffset);

  // if(nPhotons > 0  && chargeTest) FillAllHistograms(2);
  // else if(nPhotons > 0)           FillAllHistograms(2 + fQcdOffset);
  // if(nPhotons == 1 && chargeTest) FillAllHistograms(3);
  // else if(nPhotons == 1)          FillAllHistograms(3 + fQcdOffset);


  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////
  // Set 5 + selection offset: object number selection
  ////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(5);
  else if(mutau)          FillAllHistograms(5 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(25);
  else if(etau)           FillAllHistograms(25 + fQcdOffset);
  if(emu   && chargeTest) FillAllHistograms(45);
  else if(emu)            FillAllHistograms(45 + fQcdOffset);

  // llg study sets
  if(llg_study)                                       FillAllHistograms(95);
  if(llg_study && nMuons == 2 && chargeTest)          FillAllHistograms(96);
  else if(llg_study && nMuons == 2)                   FillAllHistograms(96 + fQcdOffset);
  if(llg_study && nElectrons == 2 && chargeTest)      FillAllHistograms(97);
  else if(llg_study && nElectrons == 2)               FillAllHistograms(97 + fQcdOffset);
  if(llg_study && nMuons == 1 && nElectrons == 0)     FillAllHistograms(98);
  if(llg_study && nElectrons == 1 && nMuons== 0)      FillAllHistograms(99);
  if(llg_study && nElectrons == 1 && nMuons == 1 && chargeTest)      FillAllHistograms(100);
  else if(llg_study && nElectrons == 1 && nMuons == 1) FillAllHistograms(100+fQcdOffset);
  //tau llg sets
  if(llg_study && nElectrons == 1 && nMuons == 0 && nTaus == 1 && chargeTest) FillAllHistograms(101);
  else if(llg_study && nElectrons == 1 && nMuons == 0 && nTaus == 1)          FillAllHistograms(101+fQcdOffset);
  if(llg_study && nElectrons == 0 && nMuons == 1 && nTaus == 1 && chargeTest) FillAllHistograms(102);
  else if(llg_study && nElectrons == 0 && nMuons == 1 && nTaus == 1)          FillAllHistograms(102+fQcdOffset);
  
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
  emu   = emu   && ((electron->Pt() > 30. && muon->Pt() > 10.) ||
		    (electron->Pt() > 15. && muon->Pt() > 25.));

  ////////////////////////////////////////////////////////////
  // Set 6 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(6);
  else if(mutau)          FillAllHistograms(6 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(26);
  else if(etau)           FillAllHistograms(26 + fQcdOffset);
  if(emu  && chargeTest)  FillAllHistograms(46);
  else if(emu)            FillAllHistograms(46 + fQcdOffset);
  
  mutau = mutau && abs(muon->Eta()) < 2.4;
  mutau = mutau && abs(tau->Eta()) < 2.3;
  mutau = mutau && abs(tau->DeltaR(*muon)) > 0.3;

  etau  = etau  && abs(electron->Eta()) < 2.5;
  etau  = etau  && abs(tau->Eta()) < 2.3;
  etau  = etau  && abs(tau->DeltaR(*electron)) > 0.3;

  emu  = emu  && abs(electron->Eta()) < 2.5;
  emu  = emu  && abs(muon->Eta()) < 2.4;
  emu  = emu  && abs(muon->DeltaR(*electron)) > 0.3;

  ////////////////////////////////////////////////////////////
  // Set 7 + selection offset: object eta cuts
  ////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(7);
  else if(mutau)          FillAllHistograms(7 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(27);
  else if(etau)           FillAllHistograms(27 + fQcdOffset);
  if(emu  && chargeTest)  FillAllHistograms(47);
  else if(emu)            FillAllHistograms(47 + fQcdOffset);
  if(mumu  && chargeTest) FillAllHistograms(67);
  else if(mumu)           FillAllHistograms(67 + fQcdOffset);

  //////////////////////////////////////////////////////////////
  //
  // Analysis cut section
  //
  //////////////////////////////////////////////////////////////

  //define how we're counting bjets, ID and pT threshold
  if(fBJetCounting == 0) {
    if(fBJetTightness == 0) nBJetsUse = nBJets;
    else if(fBJetTightness == 1) nBJetsUse = nBJetsM;
    else if(fBJetTightness == 2) nBJetsUse = nBJetsL;
  } else if(fBJetCounting == 1) {
    if(fBJetTightness == 0) nBJetsUse = nBJets + nBJets25;
    else if(fBJetTightness == 1) nBJetsUse = nBJetsM + nBJets25M;
    else if(fBJetTightness == 2) nBJetsUse = nBJetsL + nBJets25L;
  } else if(fBJetCounting == 2) {
    if(fBJetTightness == 0) nBJetsUse = nBJets + nBJets25 + nBJets20;
    else if(fBJetTightness == 1) nBJetsUse = nBJetsM + nBJets25M + nBJets20M;
    else if(fBJetTightness == 2) nBJetsUse = nBJetsL + nBJets25L + nBJets20L;
  } else {
    if(entry % 50000 == 0) printf("Bad bJetUse definition!\n");
    
  }
  ////////////////////////////////////////////////////////////////////////////
  // Set 13 + selection offset: nBJets >= 1 (Top backgrounds)
  ////////////////////////////////////////////////////////////////////////////  
  if(mutau && nBJetsUse > 0 && chargeTest) FillAllHistograms(13);
  else if(mutau && nBJetsUse > 0)          FillAllHistograms(13 + fQcdOffset);
  if(etau && nBJetsUse > 0 && chargeTest)  FillAllHistograms(33);
  else if(etau && nBJetsUse > 0)           FillAllHistograms(33 + fQcdOffset);
  if(emu && nBJetsUse > 0 && chargeTest)   FillAllHistograms(53);
  else if(emu && nBJetsUse > 0)            FillAllHistograms(53 + fQcdOffset);

  
  mutau &= nBJetsUse == 0;
  etau  &= nBJetsUse == 0;
  emu   &= nBJetsUse == 0;
  mumu  &= nBJetsUse == 0;

  //transverse masses with MET
  double mTTau = (tau != 0) ? 2.*met*tau->Pt() : 0.;
  mTTau = (tau != 0) ? sqrt(mTTau*(1.-cos(tau->Phi() - metPhi))) : 0.;
  double mTMu = (muon != 0) ? 2.*met*muon->Pt() : 0.;
  mTMu = (muon != 0) ? sqrt(mTMu*(1.-cos(muon->Phi() - metPhi))) : 0.;
  double mTE = (electron != 0) ? 2.*met*electron->Pt() : 0.;
  mTE  = (electron != 0) ? sqrt(mTE*(1.-cos(electron->Phi() - metPhi))) : 0.;

  ////////////////////////////////////////////////////////////////////////////
  // Set 14 + selection offset: MT(e/mu) > 60, MT(tau) > 50 (W/Top backgrounds)
  ////////////////////////////////////////////////////////////////////////////  
  bool wregion = ((etau && mTE > 80. && mTTau > 50.)
		  || (mutau && mTMu > 80. && mTTau > 50.)
		  || (emu && mTE > 80. && mTMu > 80.));
  
  if(mutau && wregion && chargeTest) FillAllHistograms(14);
  else if(mutau && wregion)          FillAllHistograms(14 + fQcdOffset);
  if(etau && wregion && chargeTest)  FillAllHistograms(34);
  else if(etau && wregion)           FillAllHistograms(34 + fQcdOffset);
  if(emu && wregion && chargeTest)   FillAllHistograms(54);
  else if(emu && wregion)            FillAllHistograms(54 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 15 + selection offset: Z backgrounds
  ////////////////////////////////////////////////////////////////////////////  
  bool zregion = ((etau && mTE < 20. && fTreeVars.lepm > 40. && fTreeVars.lepm < 100.)
		  || (mutau && mTMu < 20. && fTreeVars.lepm > 40. && fTreeVars.lepm < 100.)
		  || (emu && mTE < 30. && mTMu < 50. && fTreeVars.lepm > 40. && fTreeVars.lepm < 80.));
  
  if(mutau && zregion && chargeTest) FillAllHistograms(15);
  else if(mutau && zregion)          FillAllHistograms(15 + fQcdOffset);
  if(etau && zregion && chargeTest)  FillAllHistograms(35);
  else if(etau && zregion)           FillAllHistograms(35 + fQcdOffset);
  if(emu && zregion && chargeTest)   FillAllHistograms(55);
  else if(emu && zregion)            FillAllHistograms(55 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(8);
  else if(mutau)          FillAllHistograms(8 + fQcdOffset);
  if(etau && chargeTest)  FillAllHistograms(28);
  else if(etau)           FillAllHistograms(28 + fQcdOffset);
  if(emu && chargeTest)   FillAllHistograms(48);
  else if(emu)            FillAllHistograms(48 + fQcdOffset);
  if(mumu && chargeTest)  FillAllHistograms(68);
  else if(mumu)           FillAllHistograms(68 + fQcdOffset);

  //just Z->mu+mu, for Z pT weights study
  if(mumu && nJets == 0 && nPhotons == 0 && nTaus == 0 && chargeTest)  FillAllHistograms(69);
  else if(mumu && nJets == 0 && nPhotons == 0 && nTaus == 0)           FillAllHistograms(69 + fQcdOffset);
  
  
  ////////////////////////////////////////////////////////////////////////////
  // Set 9-12 : BDT Cut
  ////////////////////////////////////////////////////////////////////////////
  //Total background Z0 MVAs
  if(mutau && chargeTest && fMvaOutputs[1] > fMvaCuts[1]) FillAllHistograms(9);
  else if(mutau && fMvaOutputs[1] > fMvaCuts[1])          FillAllHistograms(9 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[3] > fMvaCuts[3])  FillAllHistograms(29);
  else if(etau && fMvaOutputs[3] > fMvaCuts[3])           FillAllHistograms(29 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[5] > fMvaCuts[5])   FillAllHistograms(49);
  else if(emu && fMvaOutputs[5] > fMvaCuts[5])            FillAllHistograms(49 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[7] > fMvaCuts[7])   FillAllHistograms(89); //mutau_e
  else if(emu && fMvaOutputs[7] > fMvaCuts[7])            FillAllHistograms(89 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[9] > fMvaCuts[9])   FillAllHistograms(91); //etau_mu
  else if(emu && fMvaOutputs[9] > fMvaCuts[9])            FillAllHistograms(91 + fQcdOffset);
  //Total background higgs MVAs
  if(mutau && chargeTest && fMvaOutputs[0] > fMvaCuts[0]) FillAllHistograms(10);
  else if(mutau && fMvaOutputs[0] > fMvaCuts[0])          FillAllHistograms(10 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[2] > fMvaCuts[2])  FillAllHistograms(30);
  else if(etau && fMvaOutputs[2] > fMvaCuts[2])           FillAllHistograms(30 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[4] > fMvaCuts[4])   FillAllHistograms(50);
  else if(emu && fMvaOutputs[4] > fMvaCuts[4])            FillAllHistograms(50 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[6] > fMvaCuts[6])   FillAllHistograms(90); //mutau_e
  else if(emu && fMvaOutputs[6] > fMvaCuts[6])            FillAllHistograms(90 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[8] > fMvaCuts[8])   FillAllHistograms(92); //etau_mu
  else if(emu && fMvaOutputs[8] > fMvaCuts[8])            FillAllHistograms(92 + fQcdOffset);

  // Mass window sets, before cuts
  double mll = (*leptonOneP4+*leptonTwoP4).M();
  double mgll = (*photonP4 + (*leptonOneP4+*leptonTwoP4)).M();
  //Z0 window
  if(mutau && chargeTest && mll < 95. && mll > 60.)  FillAllHistograms(11);
  else if(mutau && mll < 105. && mll > 60.)          FillAllHistograms(11 + fQcdOffset);
  if(etau && chargeTest && mll < 95. && mll > 60.)   FillAllHistograms(31);
  else if(etau && mll < 95. && mll > 60.)            FillAllHistograms(31 + fQcdOffset);
  if(emu && chargeTest && mll < 100. && mll > 80.)   FillAllHistograms(51);
  else if(emu && mll < 100. && mll > 80.)            FillAllHistograms(51 + fQcdOffset);
  //higgs window
  if(mutau && chargeTest && mll < 130. && mll > 75.) FillAllHistograms(12);
  else if(mutau && mll < 130. && mll > 75.)          FillAllHistograms(12 + fQcdOffset);
  if(etau && chargeTest && mll < 130. && mll > 75.)  FillAllHistograms(32);
  else if(etau && mll < 130. && mll > 75.)           FillAllHistograms(32 + fQcdOffset);
  if(emu && chargeTest && mll < 130. && mll > 115.)  FillAllHistograms(52);
  else if(emu && mll < 130. && mll > 115.)           FillAllHistograms(52 + fQcdOffset);


  ////////////////////////////////////////////////////////////////////////////
  // Set 16-17 + selection offset: Box cuts
  ////////////////////////////////////////////////////////////////////////////
  //z ids
  if(mutau && chargeTest && fEventId[0] && fEventId[0]->IDWord(fTreeVars) == 0)   FillAllHistograms(16);
  else if(mutau && fEventId[0] && fEventId[0]->IDWord(fTreeVars) == 0)            FillAllHistograms(16 + fQcdOffset);
  if(etau && chargeTest && fEventId[10] && fEventId[10]->IDWord(fTreeVars) == 0)  FillAllHistograms(36);
  else if(etau && fEventId[10] && fEventId[10]->IDWord(fTreeVars) == 0)           FillAllHistograms(36 + fQcdOffset);
  if(emu && chargeTest && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)   FillAllHistograms(56);
  else if(emu && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)            FillAllHistograms(56 + fQcdOffset);
  //higgs ids
  if(mutau && chargeTest && fEventId[30] && fEventId[30]->IDWord(fTreeVars) == 0) FillAllHistograms(17);
  else if(mutau && fEventId[30] && fEventId[30]->IDWord(fTreeVars) == 0)          FillAllHistograms(17 + fQcdOffset);
  if(etau && chargeTest && fEventId[40] && fEventId[40]->IDWord(fTreeVars) == 0)  FillAllHistograms(37);
  else if(etau && fEventId[40] && fEventId[40]->IDWord(fTreeVars) == 0)           FillAllHistograms(37 + fQcdOffset);
  if(emu && chargeTest && fEventId[50] && fEventId[50]->IDWord(fTreeVars) == 0)   FillAllHistograms(57);
  else if(emu && fEventId[50] && fEventId[50]->IDWord(fTreeVars) == 0)            FillAllHistograms(57 + fQcdOffset);


  ////////////////////////////////////////////////////////////////////////////
  // Set 18-20 + selection offset: Jet categories 0, 1, >1
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && nJets == 0 && chargeTest) FillAllHistograms(18);
  else if(mutau && nJets == 0)          FillAllHistograms(18 + fQcdOffset);
  if(etau && nJets == 0 && chargeTest)  FillAllHistograms(38);
  else if(etau && nJets == 0)           FillAllHistograms(38 + fQcdOffset);
  if(emu && nJets == 0 && chargeTest)   FillAllHistograms(58);
  else if(emu && nJets == 0)            FillAllHistograms(58 + fQcdOffset);
  if(mumu && nJets == 0 && chargeTest)  FillAllHistograms(78);
  else if(mumu && nJets == 0)           FillAllHistograms(78 + fQcdOffset);

  if(mutau && nJets == 1 && chargeTest) FillAllHistograms(19);
  else if(mutau && nJets == 1)          FillAllHistograms(19 + fQcdOffset);
  if(etau && nJets == 1 && chargeTest)  FillAllHistograms(39);
  else if(etau && nJets == 1)           FillAllHistograms(39 + fQcdOffset);
  if(emu && nJets == 1 && chargeTest)   FillAllHistograms(59);
  else if(emu && nJets == 1)            FillAllHistograms(59 + fQcdOffset);
  if(mumu && nJets == 1 && chargeTest)  FillAllHistograms(79);
  else if(mumu && nJets == 1)           FillAllHistograms(79 + fQcdOffset);

  if(mutau && nJets > 1  && chargeTest) FillAllHistograms(20);
  else if(mutau && nJets > 1 )          FillAllHistograms(20 + fQcdOffset);
  if(etau && nJets > 1  && chargeTest)  FillAllHistograms(40);
  else if(etau && nJets > 1 )           FillAllHistograms(40 + fQcdOffset);
  if(emu && nJets > 1  && chargeTest)   FillAllHistograms(60);
  else if(emu && nJets > 1 )            FillAllHistograms(60 + fQcdOffset);
  if(mumu && nJets > 1 && chargeTest)   FillAllHistograms(80);
  else if(mumu && nJets > 1)            FillAllHistograms(80 + fQcdOffset);
  

  ////////////////////////////////////////////////////////////////////////////
  // Set 21-22 + selection offset: Photon categories 0, >1
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && nPhotons == 0 && chargeTest) FillAllHistograms(21);
  else if(mutau && nPhotons == 0)          FillAllHistograms(21 + fQcdOffset);
  if(etau && nPhotons == 0 && chargeTest)  FillAllHistograms(41);
  else if(etau && nPhotons == 0)           FillAllHistograms(41 + fQcdOffset);
  if(emu && nPhotons == 0 && chargeTest)   FillAllHistograms(61);
  else if(emu && nPhotons == 0)            FillAllHistograms(61 + fQcdOffset);
  if(mutau && nPhotons >= 1 && chargeTest) FillAllHistograms(22);
  else if(mutau && nPhotons >= 1)          FillAllHistograms(22 + fQcdOffset);
  if(etau && nPhotons >= 1 && chargeTest)  FillAllHistograms(42);
  else if(etau && nPhotons >= 1)           FillAllHistograms(42 + fQcdOffset);
  if(emu && nPhotons >= 1 && chargeTest)   FillAllHistograms(62);
  else if(emu && nPhotons >= 1)            FillAllHistograms(62 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 23-24 JetMisID (tau) 23 Mario's cut (emu)
  ////////////////////////////////////////////////////////////////////////////
  //jets
  if(mutau && tauGenFlavor < 7 && chargeTest) FillAllHistograms(23); 
  else if(mutau && tauGenFlavor < 7)          FillAllHistograms(23 + fQcdOffset); //not necessarily for QCD, but maybe a control region
  if(etau && tauGenFlavor < 7 && chargeTest)  FillAllHistograms(43);
  else if(etau && tauGenFlavor < 7)           FillAllHistograms(43 + fQcdOffset);
  //leptons
  if(mutau && abs(tauGenFlavor - 12) == 1 && chargeTest) FillAllHistograms(24); 
  else if(mutau && abs(tauGenFlavor - 12) == 1)          FillAllHistograms(24 + fQcdOffset); 
  if(etau && abs(tauGenFlavor - 12) == 1 && chargeTest)  FillAllHistograms(44);
  else if(etau && abs(tauGenFlavor - 12) == 1)           FillAllHistograms(44 + fQcdOffset);

  bool marioID = nElectrons == 1 && nMuons == 1; //e+mu
  marioID = marioID && ((electron->Pt() > 30. && muon->Pt() > 10.) ||
			(electron->Pt() > 15. && muon->Pt() > 25.)); //trigger selection thresholds
  marioID = marioID && abs(electron->Eta()) < 2.5; //angular acceptances
  marioID = marioID && abs(muon->Eta()) < 2.4;
  marioID = marioID && abs(muon->DeltaR(*electron)) > 0.3;

  marioID &= (nBJetsM+nBJets25M) == 0; //medium ID
  marioID &= (nJets == 0) || jetOneP4->Pt() < 78.; //highest pT jet cut
  marioID &= puppMETC < 28.; //MET cut (he used PUPPI, unclear if same correction)
  marioID = marioID && (electron->Pt() > 32. && muon->Pt() > 28.); //higher electron and muon pT threshold
  marioID &= fTreeVars.lepm > 75. && fTreeVars.lepm < 110.; //mass window
  
  if(emu && marioID && chargeTest)  FillAllHistograms(63);
  else if(emu && marioID)           FillAllHistograms(63 + fQcdOffset);

  // else if(etau && tauGenFlavor)           FillAllHistograms(43 + fQcdOffset);
  // if(emu && nPhotons == 0 && chargeTest)   FillAllHistograms(61);
  // else if(emu && nPhotons == 0)            FillAllHistograms(61 + fQcdOffset);
  
  return kTRUE;
}

void ZTauTauHistMaker::ProcessLLGStudy() {

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
