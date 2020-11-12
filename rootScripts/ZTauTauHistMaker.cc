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
  // cutstring 10/05/2020:
  // mtone>1.7541851387703586&&mtone<66.508243311050904&&mttwo<68.67656032242067&&met>1.8312383752068708&&met<31.529109883214133&&
  // lepdeltaphi>2.7438808940798096&&leponept>37.357014171774139&&leponept<51.988274780189428&&leptwopt>37.402735145255065&&
  // leptwopt<52.441135442770928&&leppt<18.547911483828408&&njets<1.9999796524643898&&jetpt<41.90139627456665
  // limit gain: 1.83934 sig_eff: 0.256185 bkg_rej: 0.994083
  fEventId[20]->fMinLepm = 60.;        fEventId[20]->fMaxLepm = 170.;
  fEventId[20]->fMaxMtone = 66.508;    fEventId[20]->fMaxMttwo = 68.677;
  fEventId[20]->fMinMtone = 1.754;     fEventId[20]->fMinMttwo = 1.831;
  fEventId[20]->fMaxLeppt = 18.548;    fEventId[20]->fMinLepdeltaphi = 2.744;
  fEventId[20]->fMinLeponept = 37.357; fEventId[20]->fMaxLeponept = 51.988;
  fEventId[20]->fMinLeptwopt = 37.402; fEventId[20]->fMaxLeptwopt = 52.441;
  fEventId[20]->fMaxNJets = 1;         fEventId[20]->fMaxJetpt = 41.901;
  fEventId[20]->fMaxMet = 31.529;      fEventId[20]->fMinMet = 1.831;
  
  fEventId[21] = new TEventID(); //zemu
  // cutstring 10/05/2020:
  //mtone>1.7541851387703586&&mtone<75.511866527441413&&mttwo<78.35751427777079&&met>0.99635845329502015&&
  //met<38.003302896347222&&lepdeltaphi>2.458847083857167&&leponept>34.64483731089453&&leponept<55.055144350598191&&
  //leptwopt>35.279645242822426&&leptwopt<57.063637382693173&&leppt<31.93547906614549&&njets<1.9999796524643898&&
  //jetpt<41.90139627456665
  //limit gain: 1.57032 sig_eff: 0.409239 bkg_rej: 0.978983
  
  fEventId[21]->fMinLepm = 60.;        fEventId[21]->fMaxLepm = 170.;
  fEventId[21]->fMaxMtone = 75.512;    fEventId[21]->fMaxMttwo = 78.357;
  fEventId[21]->fMinMtone = 1.754;     
  fEventId[21]->fMaxLeppt = 31.935;    fEventId[21]->fMinLepdeltaphi = 2.459;
  fEventId[21]->fMinLeponept = 34.645; fEventId[21]->fMaxLeponept = 55.055;
  fEventId[21]->fMinLeptwopt = 35.280; fEventId[21]->fMaxLeptwopt = 57.063;
  fEventId[21]->fMaxNJets = 1;         fEventId[21]->fMaxJetpt = 41.901;
  fEventId[21]->fMaxMet = 38.003;      fEventId[21]->fMinMet = 0.996;
  
  fEventId[50] = new TEventID(); //hemu
  // cutstring lepm>121.98691899130384&&lepm<127.1750722802732&&mtone<88.949126707707563&&mttwo<80.913400356866262
  // &&leponept>35.854765333337127&&leptwopt>30.543996519263601
  // limit gain: 2.5026 sig_eff: 0.448063 bkg_rej: 0.996876
  fEventId[50]->fMinLepm = 121.98692;    fEventId[50]->fMaxLepm = 127.17507;
  fEventId[50]->fMaxMtone = 88.94913;    fEventId[50]->fMaxMttwo = 80.91340;
  fEventId[50]->fMinLeponept = 35.85477; fEventId[50]->fMinLeptwopt = 30.54400;

  fRnd = new TRandom(fRndSeed);
  if(fRemoveZPtWeights == 2) {
    TString hname = Form("%s%i.root", fZPtHistPath.Data(),fYear);
    if(fVerbose > 0) std::cout << "Opening Drell-Yan Z pT vs Mass weights file: " << hname.Data() << std::endl;
    TFile* ZPtFile = TFile::Open(hname.Data(), "READ");
    if(!ZPtFile) std::cout << "WARNING! No Z pT weight file found!\n";
    else {
      fZPtScales = (TH2D*) ZPtFile->Get("hGenRatio");
      if(!fZPtScales) std::cout << "WARNING! No Z pT weight histogram found!\n";
      fZPtRecoScales = (TH2D*) ZPtFile->Get("hRatioNorm");
      if(!fZPtRecoScales) std::cout << "WARNING! No reco Z pT weight histogram found!\n";
    }
  }
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
    if(fVerbose > 0) std::cout << "Filling histograms for set " << index
			       << " with event weight = " << eventWeight
			       << " and gen weight = " << genWeight << " !\n";
    FillLepHistogram(   fLepHist   [index]);
    if(fDYTesting) return;
    FillPhotonHistogram(fPhotonHist[index]);
  } else
    printf("WARNING! Entry %lld, attempted to fill un-initialized histogram set %i!\n", fentry, index);
  if(fDYTesting) return;
  if(fWriteTrees && fTreeSets[index])
    fTrees[index]->Fill();
}

void ZTauTauHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  BookPhotonHistograms();
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
      // fEventHist[i]->hLumiSection            = new TH1D("lumiSection"         , Form("%s: LumiSection"         ,dirname)  , 200,    0, 4e6);
      // fEventHist[i]->hTriggerStatus          = new TH1D("triggerStatus"       , Form("%s: TriggerStatus"       ,dirname)  ,   3, -1.5, 1.5);
      fEventHist[i]->hEventWeight            = new TH1D("eventweight"         , Form("%s: EventWeight"         ,dirname)  , 100,   -5,   5);     
      fEventHist[i]->hGenWeight              = new TH1D("genweight"           , Form("%s: GenWeight"           ,dirname)  ,   5, -2.5, 2.5);     
      fEventHist[i]->hGenTauFlavorWeight     = new TH1D("gentauflavorweight"  , Form("%s: GenTauFlavorWeight"  ,dirname)  ,  40,    0,   2);     
      fEventHist[i]->hPhotonIDWeight         = new TH1D("photonidweight"      , Form("%s: PhotonIDWeight"      ,dirname)  ,  40,    0,   2);     
      fEventHist[i]->hNPV                    = new TH1D("npv"                 , Form("%s: NPV"                 ,dirname)  , 200,  0, 200); 
      fEventHist[i]->hNPU                    = new TH1D("npu"                 , Form("%s: NPU"                 ,dirname)  , 100,  0, 100); 
      fEventHist[i]->hNPartons               = new TH1D("npartons"            , Form("%s: NPartons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNMuons                 = new TH1D("nmuons"              , Form("%s: NMuons"              ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNSlimMuons             = new TH1D("nslimmuons"          , Form("%s: NSlimMuons"          ,dirname)  ,  10,  0,  10); 
      // for(int icount = 0; icount < kMaxCounts; ++icount)
      // 	fEventHist[i]->hNMuonCounts[icount]  = new TH1D(Form("nmuoncounts%i",icount), Form("%s: NMuonCounts %i",dirname,icount),  10,  0,  10); 
      fEventHist[i]->hNElectrons             = new TH1D("nelectrons"          , Form("%s: NElectrons"          ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNSlimElectrons         = new TH1D("nslimelectrons"      , Form("%s: NSlimElectrons"      ,dirname)  ,  10,  0,  10); 
      // for(int icount = 0; icount < kMaxCounts; ++icount)
      // 	fEventHist[i]->hNElectronCounts[icount]= new TH1D(Form("nelectroncounts%i",icount), Form("%s: NElectronCounts %i",dirname,icount),  10,  0,  10); 
      fEventHist[i]->hNLowPtElectrons        = new TH1D("nlowptelectrons"     , Form("%s: NLowPtElectrons"     ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNTaus                  = new TH1D("ntaus"               , Form("%s: NTaus"               ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNSlimTaus              = new TH1D("nslimtaus"           , Form("%s: NSlimTaus"           ,dirname)  ,  10,  0,  10); 
      // for(int icount = 0; icount < kMaxCounts; ++icount)
      // 	fEventHist[i]->hNTauCounts[icount]   = new TH1D(Form("ntaucounts%i",icount), Form("%s: NTauCounts %i",dirname,icount),  10,  0,  10); 
      fEventHist[i]->hNSlimPhotons           = new TH1D("nslimphotons"        , Form("%s: NSlimPhotons"        ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNPhotons               = new TH1D("nphotons"            , Form("%s: NPhotons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausHad            = new TH1D("ngentaushad"         , Form("%s: NGenTausHad"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausLep            = new TH1D("ngentauslep"         , Form("%s: NGenTausLep"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTaus               = new TH1D("ngentaus"            , Form("%s: NGenTaus"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenElectrons          = new TH1D("ngenelectrons"       , Form("%s: NGenElectrons"       ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenMuons              = new TH1D("ngenmuons"           , Form("%s: NGenMuons"           ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNSlimJets              = new TH1D("nslimjets"           , Form("%s: NSlimJets"           ,dirname)  ,  30,  0,  30); 
      fEventHist[i]->hNJets                  = new TH1D("njets"               , Form("%s: NJets"               ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNJets20                = new TH1D("njets20"             , Form("%s: NJets20"             ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNFwdJets               = new TH1D("nfwdjets"            , Form("%s: NFwdJets"            ,dirname)  ,  20,  0,  20); 
      fEventHist[i]->hNBJets                 = new TH1D("nbjets"              , Form("%s: NBJets"              ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsM                = new TH1D("nbjetsm"             , Form("%s: NBJetsM"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsL                = new TH1D("nbjetsl"             , Form("%s: NBJetsL"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20               = new TH1D("nbjets20"            , Form("%s: NBJets20"            ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20M              = new TH1D("nbjets20m"           , Form("%s: NBJets20M"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20L              = new TH1D("nbjets20l"           , Form("%s: NBJets20L"           ,dirname)  ,  20,  0,  20);

      fEventHist[i]->hJetsFlavor             = new TH1D("jetsflavor"          , Form("%s: JetsFlavor"          ,dirname)  ,  50, -20,  30);
      int njetspt = 6;
      double jetspt[] = {0.  , 30. , 40. , 50.  , 70.,
			 100., 
			 1000.};
      int njetseta = 10;
      double jetseta[] = {-2.5 , -2. , -1.5 , -1.,
			  -0.5 , 0.   , 0.5 , 1.   , 1.5,
			  2.   ,
			  2.5};
			 
      fEventHist[i]->hJetsPtVsEta[0]         = new TH2D("jetsptvseta0"        , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hJetsPtVsEta[1]         = new TH2D("jetsptvseta1"        , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hJetsPtVsEta[2]         = new TH2D("jetsptvseta2"        , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsPtVsEta[0]        = new TH2D("bjetsptvseta0"       , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsPtVsEta[1]        = new TH2D("bjetsptvseta1"       , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsPtVsEta[2]        = new TH2D("bjetsptvseta2"       , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsMPtVsEta[0]       = new TH2D("bjetsmptvseta0"      , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsMPtVsEta[1]       = new TH2D("bjetsmptvseta1"      , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsMPtVsEta[2]       = new TH2D("bjetsmptvseta2"      , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsLPtVsEta[0]       = new TH2D("bjetslptvseta0"      , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsLPtVsEta[1]       = new TH2D("bjetslptvseta1"      , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);
      fEventHist[i]->hBJetsLPtVsEta[2]       = new TH2D("bjetslptvseta2"      , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt);

      fEventHist[i]->hMcEra                  = new TH1D("mcera"               , Form("%s: McEra"               ,dirname) ,   5,   0,  5);
      fEventHist[i]->hTriggerLeptonStatus    = new TH1D("triggerleptonstatus" , Form("%s: TriggerLeptonStatus" ,dirname) ,  10,   0, 10);
      fEventHist[i]->hNTriggered             = new TH1D("ntriggered"          , Form("%s: NTriggered"          ,dirname) ,  10,   0, 10);
      fEventHist[i]->hPuWeight		     = new TH1D("puweight"	      , Form("%s: PuWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTopPtWeight	     = new TH1D("topptweight"	      , Form("%s: TopPtWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hZPtWeight	             = new TH1D("zptweight"	      , Form("%s: ZPtWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTauDecayMode	     = new TH1D("taudecaymode"	      , Form("%s: TauDecayMode"	       ,dirname) ,  15,   0, 15);
      fEventHist[i]->hTauMVA		     = new TH1D("taumva"              , Form("%s: TauMVA"              ,dirname) , 100,   0,  1);
      fEventHist[i]->hTauGenFlavor	     = new TH1D("taugenflavor"	      , Form("%s: TauGenFlavor"	       ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauGenFlavorHad	     = new TH1D("taugenflavorhad"     , Form("%s: TauGenFlavorHad"     ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauDeepAntiEle         = new TH1D("taudeepantiele"      , Form("%s: TauDeepAntiEle"      ,dirname) ,  300,  0,300);
      fEventHist[i]->hTauDeepAntiMu          = new TH1D("taudeepantimu"       , Form("%s: TauDeepAntiMu"       ,dirname) ,   30,  0, 30);
      fEventHist[i]->hTauDeepAntiJet         = new TH1D("taudeepantijet"      , Form("%s: TauDeepAntiJet"      ,dirname) ,  300,  0,300);
      // fEventHist[i]->hTauVetoedJetPt	     = new TH1D("tauvetoedjetpt"      , Form("%s: TauVetoedJetPt"      ,dirname) , 210, -10,200);
      // fEventHist[i]->hTauVetoedJetPtUnc	     = new TH1D("tauvetoedjetptunc"   , Form("%s: TauVetoedJetPtUnc"   ,dirname) , 110,  -1, 10);
      fEventHist[i]->hHtSum		     = new TH1D("htsum"		      , Form("%s: HtSum"	       ,dirname) , 200,   0,800);
      fEventHist[i]->hHt		     = new TH1D("ht"		      , Form("%s: Ht"		       ,dirname) , 200,   0,800);
      fEventHist[i]->hHtPhi                  = new TH1D("htphi"               , Form("%s: HtPhi"               ,dirname) , 100,  -4,  4);
      fEventHist[i]->hJetPt                  = new TH1D("jetpt"               , Form("%s: JetPt"               ,dirname)  , 300,   0, 300);
      fEventHist[i]->hJetM                   = new TH1D("jetm"                , Form("%s: JetM"                ,dirname)  , 150,   0, 300);
      fEventHist[i]->hJetEta                 = new TH1D("jeteta"              , Form("%s: JetEta"              ,dirname)  , 200, -10,  10);
      fEventHist[i]->hJetPhi                 = new TH1D("jetphi"              , Form("%s: JetPhi"              ,dirname)  , 100,  -4,  4);
      fEventHist[i]->hJetBMVA                = new TH1D("jetbmva"             , Form("%s: JetBMVA"             ,dirname)  , 300, -1.,  2.);
      fEventHist[i]->hJetBTag                = new TH1D("jetbtag"             , Form("%s: JetBTag"             ,dirname)  ,   2,   0,   2);
      fEventHist[i]->hTauPt                  = new TH1D("taupt"               , Form("%s: TauPt"               ,dirname)  , 300,   0, 300);
      fEventHist[i]->hTauM                   = new TH1D("taum"                , Form("%s: TauM"                ,dirname)  , 100,   0, 4.);
      fEventHist[i]->hTauEta                 = new TH1D("taueta"              , Form("%s: TauEta"              ,dirname)  , 200, -10,  10);
      fEventHist[i]->hTauPhi                 = new TH1D("tauphi"              , Form("%s: TauPhi"              ,dirname)  , 100,  -4,  4);

      fEventHist[i]->hPFMet                  = new TH1D("pfmet"               , Form("%s: PF Met"              ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hPFMetPhi               = new TH1D("pfmetphi"            , Form("%s: PF MetPhi"           ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hPFCovMet00             = new TH1D("pfcovmet00"          , Form("%s: PF CovMet00"         ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPFCovMet01	     = new TH1D("pfcovmet01"	      , Form("%s: PF CovMet01"         ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hPFCovMet11	     = new TH1D("pfcovmet11"	      , Form("%s: PF CovMet11"         ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPuppMet                = new TH1D("puppmet"             , Form("%s: PUPPI Met"           ,dirname)  , 200,  0, 400); 
      // fEventHist[i]->hPuppMetPhi             = new TH1D("puppmetphi"          , Form("%s: PUPPI MetPhi"        ,dirname)  ,  80, -4,   4); 
      // fEventHist[i]->hPuppCovMet00           = new TH1D("puppcovmet00"        , Form("%s: PUPPI CovMet00"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPuppCovMet01	     = new TH1D("puppcovmet01"	      , Form("%s: PUPPI CovMet01"      ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hPuppCovMet11	     = new TH1D("puppcovmet11"	      , Form("%s: PUPPI CovMet11"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hTrkMet                 = new TH1D("trkmet"              , Form("%s: Trk Met"             ,dirname)  , 200,  0, 400); 
      // fEventHist[i]->hTrkMetPhi              = new TH1D("trkmetphi"           , Form("%s: Trk MetPhi"          ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hMet                    = new TH1D("met"                 , Form("%s: Met"                 ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hMetPhi                 = new TH1D("metphi"              , Form("%s: MetPhi"              ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hMetCorr                = new TH1D("metcorr"             , Form("%s: Met Correction"      ,dirname)  , 200,  0, 40); 
      fEventHist[i]->hMetCorrPhi             = new TH1D("metcorrphi"          , Form("%s: MetPhi Correction"   ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hCovMet00               = new TH1D("covmet00"            , Form("%s: CovMet00"            ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hCovMet01		     = new TH1D("covmet01"	      , Form("%s: CovMet01"            ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hCovMet11		     = new TH1D("covmet11"	      , Form("%s: CovMet11"            ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hMetVsPt                = new TH2D("metvspt"             , Form("%s: MetVsPt"             ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetVsM                 = new TH2D("metvsm"              , Form("%s: MetVsM"              ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetVsHtSum             = new TH2D("metvshtsum"          , Form("%s: MetVsHtSum"          ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetOverSqrtHtSum       = new TH1D("metoversqrthtsum"    , Form("%s: MetOverSqrtHtSum"    ,dirname) ,  200,    0., 400.);
      // fEventHist[i]->hMassSVFit		     = new TH1D("masssvfit"	      , Form("%s: MassSVFit"           ,dirname) , 1000,    0., 200.);
      // fEventHist[i]->hMassErrSVFit	     = new TH1D("masserrsvfit"        , Form("%s: MassErrSVFit"        ,dirname) , 1000,    0., 100.);
      // fEventHist[i]->hSVFitStatus            = new TH1D("svfitstatus"         , Form("%s: SVFitStatus"         ,dirname) ,   10,    0.,  10.);

							       
      fEventHist[i]->hLepPt[0]      = new TH1D("leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepPt[1]      = new TH1D("leppt1"        , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepPt[2]      = new TH1D("leppt2"        , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepP          = new TH1D("lepp"          , Form("%s: Lepton P"       ,dirname)  , 200,   0, 400);
      // fEventHist[i]->hLepE          = new TH1D("lepe"          , Form("%s: Lepton E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepM[0]       = new TH1D("lepm"          , Form("%s: Lepton M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepM[1]       = new TH1D("lepm1"         , Form("%s: Lepton M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepM[2]       = new TH1D("lepm2"         , Form("%s: Lepton M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepEta        = new TH1D("lepeta"        , Form("%s: Lepton Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepPhi        = new TH1D("lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  80,  -4,   4);
      //variable width bins for pT vs mass
      const int nmbins = 63;
      const double mbins[nmbins+1] = { 0.   , 55.  , 60.  , 65.  , 70.  ,
				       74.  , 77.  , 79.  , 81.  , 82.  ,
				       83.  , 84.  , 84.75, 85.5 , 86.  ,
				       86.5 , 87.  , 87.5 , 87.75, 88.  ,
				       88.25, 88.5 , 88.75, 89.  , 89.25,
				       89.5 , 89.75, 90.  , 90.15, 90.3 ,
				       90.45, 90.6 , 90.75, 90.9 , 91.05,
				       91.2 , 91.35, 91.5 , 91.75, 92.  ,
				       92.25, 92.5 , 92.75, 93.  , 93.25,
				       93.5 , 94.  , 94.5 , 95.  , 95.5 ,
				       96.  , 96.5 , 97.  , 98.  , 99.  ,
				       100. , 103. , 106. , 110. , 115. ,
				       120. , 130. , 140. , 
				       1000.};
      if(nmbins != sizeof(mbins)/sizeof(*mbins)-1) std::cout << "WARNING! N(pT bins) for 2D histograms is off! nbins = "
							     << nmbins << " size calc = " << sizeof(mbins)/sizeof(*mbins)-1
							     << std::endl;
      const int npbins = 67;
      const double pbins[npbins+1] = { 0.  , 0.5 , 1.  , 1.5 , 2.  ,
				       2.5 , 2.75, 3.  , 3.25, 3.5 ,
				       3.75, 4.  , 4.25, 4.5 , 4.75,
				       5.  , 5.25, 5.5 , 5.75, 6.  ,
				       6.25, 6.5 , 6.75, 7.  , 7.25,
				       7.5 , 7.75, 8.  , 8.5 , 9.  ,
				       9.5 , 10. , 10.5, 11. , 11.5,
				       12. , 12.5, 13. , 13.5, 14. ,
				       14.5, 15. , 16. , 17. , 18. ,
				       19. , 20. , 22. , 24. , 26. ,
				       28. , 30. , 33. , 36. , 40. ,
				       43. , 46. , 50. , 55. , 60. ,
				       65. , 70. , 75. , 80. , 88. ,
				       105., 150., 
				       1000.};
      
      if(npbins != sizeof(pbins)/sizeof(*pbins)-1) std::cout << "WARNING! N(pT bins) for 2D histograms is off! nbins = "
							     << npbins << " size calc = " << sizeof(pbins)/sizeof(*pbins)-1
							     << std::endl;

      fEventHist[i]->hLepPtVsM[0]   = new TH2D("lepptvsm0"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hLepPtVsM[1]   = new TH2D("lepptvsm1"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hLepPtVsM[2]   = new TH2D("lepptvsm2"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[0]     = new TH2D("zptvsm0"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[1]     = new TH2D("zptvsm1"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[2]     = new TH2D("zptvsm2"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPt[0]        = new TH1D("zpt"           , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZPt[1]        = new TH1D("zpt1"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZPt[2]        = new TH1D("zpt2"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZMass[0]      = new TH1D("zmass"         , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);
      fEventHist[i]->hZMass[1]      = new TH1D("zmass1"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);
      fEventHist[i]->hZMass[2]      = new TH1D("zmass2"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);

      fEventHist[i]->hLepDeltaPhi   = new TH1D("lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname)  ,  50,   0,   5);
      fEventHist[i]->hLepDeltaEta   = new TH1D("lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDeltaR     = new TH1D("lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDelRVsPhi  = new TH2D("lepdelrvsphi"  , Form("%s: LepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hLepPtOverM    = new TH1D("lepptoverm"    , Form("%s: Lepton Pt / M"  ,dirname)  , 100,   0,  10);
      fEventHist[i]->hAlpha[0]      = new TH1D("alpha0"        , Form("%s: Alpha (Z) 0"    ,dirname)  , 100,   0,   5);
      fEventHist[i]->hAlpha[1]      = new TH1D("alpha1"        , Form("%s: Alpha (H) 0"    ,dirname)  , 100,   0,   5);
      fEventHist[i]->hAlpha[2]      = new TH1D("alpha2"        , Form("%s: Alpha 1"        ,dirname)  , 100,   0,   5);
      fEventHist[i]->hAlpha[3]      = new TH1D("alpha3"        , Form("%s: Alpha 2"        ,dirname)  , 100,   0,   5);
      fEventHist[i]->hDeltaAlpha[0] = new TH1D("deltaalpha0"   , Form("%s: Delta Alpha (Z) 0"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlpha[1] = new TH1D("deltaalpha1"   , Form("%s: Delta Alpha (Z) 1"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlpha[2] = new TH1D("deltaalpha2"   , Form("%s: Delta Alpha (H) 0"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlpha[3] = new TH1D("deltaalpha3"   , Form("%s: Delta Alpha (H) 1"  ,dirname)  , 200, -10,  10);

      fEventHist[i]->hHtDeltaPhi    = new TH1D("htdeltaphi"    , Form("%s: Ht Lep Delta Phi",dirname) ,  50,   0,   5);
      fEventHist[i]->hMetDeltaPhi   = new TH1D("metdeltaphi"   , Form("%s: Met Lep Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hJetDeltaPhi   = new TH1D("jetdeltaphi"   , Form("%s: Jet Lep Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepOneDeltaPhi   = new TH1D("leponedeltaphi"   , Form("%s: Lep One vs Sys Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepTwoDeltaPhi   = new TH1D("leptwodeltaphi"   , Form("%s: Lep Two vs Sys Delta Phi",dirname),  50,   0,   5);

      fEventHist[i]->hLepOneJetDeltaR     = new TH1D("leponejetdeltar"     , Form("%s: Lep One vs Jet Delta R"  ,dirname),  60,   0,   6);
      fEventHist[i]->hLepOneJetDeltaPhi   = new TH1D("leponejetdeltaphi"   , Form("%s: Lep One vs Jet Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepOneJetDeltaEta   = new TH1D("leponejetdeltaeta"   , Form("%s: Lep One vs Jet Delta Eta",dirname),  60,   0,   6);
      fEventHist[i]->hLepTwoJetDeltaR     = new TH1D("leptwojetdeltar"     , Form("%s: Lep One vs Jet Delta R"  ,dirname),  60,   0,   6);
      fEventHist[i]->hLepTwoJetDeltaPhi   = new TH1D("leptwojetdeltaphi"   , Form("%s: Lep One vs Jet Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepTwoJetDeltaEta   = new TH1D("leptwojetdeltaeta"   , Form("%s: Lep One vs Jet Delta Eta",dirname),  60,   0,   6);

      // fEventHist[i]->hLepSVPt       = new TH1D("lepsvpt"       , Form("%s: Lepton SVFit Pt"      ,dirname)  , 200,   0, 400);
      // fEventHist[i]->hLepSVM        = new TH1D("lepsvm"        , Form("%s: Lepton SVFit M"       ,dirname)  , 400,   0, 400);
      // fEventHist[i]->hLepSVEta      = new TH1D("lepsveta"      , Form("%s: Lepton SVFit Eta"     ,dirname)  , 200, -10,  10);
      // fEventHist[i]->hLepSVPhi      = new TH1D("lepsvphi"      , Form("%s: Lepton SVFit Phi"     ,dirname)  ,  80,  -4,   4);
      // fEventHist[i]->hLepSVDeltaPhi = new TH1D("lepsvdeltaphi" , Form("%s: Lepton SVFit DeltaPhi",dirname)  ,  50,   0,   5);
      // fEventHist[i]->hLepSVDeltaEta = new TH1D("lepsvdeltaeta" , Form("%s: Lepton SVFit DeltaEta",dirname)  , 100,   0,   5);
      // fEventHist[i]->hLepSVDeltaM   = new TH1D("lepsvdeltam"   , Form("%s: Lepton SVFit DeltaM"  ,dirname)  , 200, -10,  90);
      // fEventHist[i]->hLepSVDeltaPt  = new TH1D("lepsvdeltapt"  , Form("%s: Lepton SVFit DeltaPt" ,dirname)  , 200, -10,  90);
      // fEventHist[i]->hLepSVPtOverM  = new TH1D("lepsvptoverm"  , Form("%s: Lepton SVFit Pt / M"  ,dirname)  , 100,   0,  10);
      
      fEventHist[i]->hSysM          = new TH1D("sysm"          , Form("%s: SysM"          ,dirname)  ,1000,  0, 1e3);     
      fEventHist[i]->hSysPt         = new TH1D("syspt"         , Form("%s: SysPt"         ,dirname)  , 200,  0, 400);     
      fEventHist[i]->hSysEta        = new TH1D("syseta"        , Form("%s: SysEta"        ,dirname)  , 100, -5,   5);     
      fEventHist[i]->hSysMvsLepM    = new TH2D("sysmvslepm"    , Form("%s: SysMvsLepM"    ,dirname)  , 200, 0., 200., 200, 0., 200.);     

      fEventHist[i]->hMTOne         = new TH1D("mtone"         , Form("%s: MTOne"         ,dirname)  , 200, 0.,   200.);     
      fEventHist[i]->hMTTwo         = new TH1D("mttwo"         , Form("%s: MTTwo"         ,dirname)  , 200, 0.,   200.);     

      fEventHist[i]->hPXiVis[0]        = new TH1D("pxivis0"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[0]        = new TH1D("pxiinv0"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[0] = new TH1D("pxivisoverinv0" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[0]   = new TH2D("pxiinvvsvis0"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[0]       = new TH1D("pxidiff0"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[0]      = new TH1D("pxidiff20"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff3[0]      = new TH1D("pxidiff30"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);     
      fEventHist[i]->hPXiVis[1]        = new TH1D("pxivis1"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[1]        = new TH1D("pxiinv1"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[1] = new TH1D("pxivisoverinv1" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[1]   = new TH2D("pxiinvvsvis1"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[1]       = new TH1D("pxidiff1"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[1]      = new TH1D("pxidiff21"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff3[1]      = new TH1D("pxidiff31"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);     
      fEventHist[i]->hPXiVis[2]        = new TH1D("pxivis2"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[2]        = new TH1D("pxiinv2"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[2] = new TH1D("pxivisoverinv2" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[2]   = new TH2D("pxiinvvsvis2"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[2]       = new TH1D("pxidiff2"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[2]      = new TH1D("pxidiff22"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff3[2]      = new TH1D("pxidiff32"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);     

      fEventHist[i]->hPTauVisFrac      = new TH1D("ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  ,300,0.,  1.5);     
      fEventHist[i]->hLepMEstimate     = new TH1D("lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);     
      fEventHist[i]->hLepMEstimateTwo  = new TH1D("lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);     

      fEventHist[i]->hPtSum[0]         = new TH1D("ptsum0"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPtSum[1]         = new TH1D("ptsum1"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[0]        = new TH1D("pt1sum0"        , Form("%s: Scalar Pt sum Lepton 1 + MET"     ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[1]        = new TH1D("pt1sum1"        , Form("%s: Scalar Pt sum Lepton 2 + MET"     ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[2]        = new TH1D("pt1sum2"        , Form("%s: Scalar Pt sum Lepton 1 + 2"       ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[3]        = new TH1D("pt1sum3"        , Form("%s: Scalar Pt sum Lepton 1 + 2 - MET" ,dirname)    ,1000,  0.,  1000.);     
      for(unsigned j = 0; j < fMvaNames.size(); ++j)  {
	//high mva score binning to improve cdf making
	fEventHist[i]->hMVA[j]        = new TH1D(Form("mva%i",j)     , Form("%s: %s MVA" ,dirname, fMvaNames[j].Data()) ,10000, -1.,  2.); 
	fEventHist[i]->hMVATrain[j]   = new TH1D(Form("mvatrain%i",j), Form("%s: %s MVA (train)" ,dirname, fMvaNames[j].Data()) ,500, -3.,  2.); 
	fEventHist[i]->hMVATest[j]    = new TH1D(Form("mvatest%i",j) , Form("%s: %s MVA (test)" ,dirname, fMvaNames[j].Data())  ,500, -3.,  2.); 
      }      
      if(fFolderName == "llg_study") {
	// //llg study histograms
	// TH1D* hObjMasses[14]; //jets, jets+gamma, jet1/2 + gamma, jets + l1/2, jet1/2 + l1/2, jets+l1+l2, jets + gamma + l1/2, jets + gamma + l1 + l2
	// //masses
	// fEventHist[i]->hObjMasses[0 ]     = new TH1D("objmasses0"        , Form("%s: ObjMasses0        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[1 ]     = new TH1D("objmasses1"        , Form("%s: ObjMasses1        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[2 ]     = new TH1D("objmasses2"        , Form("%s: ObjMasses2        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[3 ]     = new TH1D("objmasses3"        , Form("%s: ObjMasses3        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[4 ]     = new TH1D("objmasses4"        , Form("%s: ObjMasses4        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[5 ]     = new TH1D("objmasses5"        , Form("%s: ObjMasses5        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[6 ]     = new TH1D("objmasses6"        , Form("%s: ObjMasses6        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[7 ]     = new TH1D("objmasses7"        , Form("%s: ObjMasses7        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[8 ]     = new TH1D("objmasses8"        , Form("%s: ObjMasses8        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[9 ]     = new TH1D("objmasses9"        , Form("%s: ObjMasses9        ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[10]     = new TH1D("objmasses10"       , Form("%s: ObjMasses10       ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[11]     = new TH1D("objmasses11"       , Form("%s: ObjMasses11       ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[12]     = new TH1D("objmasses12"       , Form("%s: ObjMasses12       ", dirname), 300, 0., 600.);
	// fEventHist[i]->hObjMasses[13]     = new TH1D("objmasses13"       , Form("%s: ObjMasses13       ", dirname), 300, 0., 600.);
	// fEventHist[i]->hJetTwoM           = new TH1D("jettwom"           , Form("%s: JetTwoM           ", dirname), 100, 0., 100.);
	// fEventHist[i]->hJetTwoPt          = new TH1D("jettwopt"          , Form("%s: JetTwoPt          ", dirname), 300, 0., 300.);
	// fEventHist[i]->hJetTwoEta         = new TH1D("jettwoeta"         , Form("%s: JetTwoEta         ", dirname), 200,-10., 10.);
	// fEventHist[i]->hJetTwoBMVA        = new TH1D("jettwobmva"        , Form("%s: JetTwoBMVA"        ,dirname) , 300, -1.,  2.);
	// fEventHist[i]->hJetTwoBTag        = new TH1D("jettwobtag"        , Form("%s: JetTwoBTag"        ,dirname) ,   2,   0,   2);
	// fEventHist[i]->hJetsDeltaR        = new TH1D("jetsdeltar"        , Form("%s: JetsDeltaR        ", dirname), 120, 0.,  12.);
	// fEventHist[i]->hJetsDeltaEta      = new TH1D("jetsdeltaeta"      , Form("%s: JetsDeltaEta      ", dirname), 120, 0.,  12.);
	// fEventHist[i]->hJetsDeltaPhi      = new TH1D("jetsdeltaphi"      , Form("%s: JetsDeltaPhi      ", dirname),  40, 0.,   4.);
	// fEventHist[i]->hJetsPt            = new TH1D("jetspt"            , Form("%s: JetsPt            ", dirname), 200, 0., 400.);
	// fEventHist[i]->hJetsEta           = new TH1D("jetseta"           , Form("%s: JetsEta           ", dirname), 200,-10., 10.);
	// fEventHist[i]->hJetsGammaDeltaR   = new TH1D("jetsgammadeltar"   , Form("%s: JetsGammaDeltaR   ", dirname), 120, 0.,  12.);
	// fEventHist[i]->hJetsGammaDeltaEta = new TH1D("jetsgammadeltaeta" , Form("%s: JetsGammaDeltaEta ", dirname), 120, 0.,  12.);
	// fEventHist[i]->hJetsGammaDeltaPhi = new TH1D("jetsgammadeltaphi" , Form("%s: JetsGammaDeltaPhi ", dirname),  40, 0.,   4.);
	// fEventHist[i]->hJetsGammaPt       = new TH1D("jetsgammapt"       , Form("%s: JetsGammaPt       ", dirname), 200, 0., 400.);
	// fEventHist[i]->hJetsGammaEta      = new TH1D("jetsgammaeta"      , Form("%s: JetsGammaEta      ", dirname), 200,-10., 10.);
      }
    }
  }
}

void ZTauTauHistMaker::BookPhotonHistograms() {
  if(fDYTesting) return;
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"photon_%i",i);
      fDirectories[1*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[1*fn + i]->cd();
      fPhotonHist[i] = new PhotonHist_t;
      fPhotonHist[i]->hPz        = new TH1D("pz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPt        = new TH1D("pt"      , Form("%s: Pt"      ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hP         = new TH1D("p"       , Form("%s: P"       ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hEta       = new TH1D("eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fPhotonHist[i]->hPhi       = new TH1D("phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fPhotonHist[i]->hMVA       = new TH1D("mva"     , Form("%s: MVA"     ,dirname)  , 300,  -1,   2);
      // fPhotonHist[i]->hIso       = new TH1D("iso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      // fPhotonHist[i]->hRelIso    = new TH1D("reliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      // fPhotonHist[i]->hTrigger   = new TH1D("trigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
    }
  }
}

void ZTauTauHistMaker::BookLepHistograms() {
  // if(fDYTesting) return;
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      fDirectories[2*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;
      fLepHist[i]->hOnePz        = new TH1D("onepz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePt        = new TH1D("onept"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneP         = new TH1D("onep"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneM         = new TH1D("onem"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOneEta       = new TH1D("oneeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOnePhi       = new TH1D("onephi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hOneD0        = new TH1D("oned0"      , Form("%s: D0"      ,dirname)  , 400,-0.1, 0.1);
      fLepHist[i]->hOneIso       = new TH1D("oneiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fLepHist[i]->hOneID1       = new TH1D("oneid1"     , Form("%s: ID1"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hOneID2       = new TH1D("oneid2"     , Form("%s: ID2"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hOneRelIso    = new TH1D("onereliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hOneFlavor    = new TH1D("oneflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hOneQ         = new TH1D("oneq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hOneTrigger   = new TH1D("onetrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      fLepHist[i]->hOneWeight    = new TH1D("oneweight"  , Form("%s: Weight"  ,dirname)  , 100, -1.,  3.);
      fLepHist[i]->hOneTrigWeight= new TH1D("onetrigweight", Form("%s: Trig Weight"  ,dirname)  , 100, -1.,  3.);
      //Gen Info
      fLepHist[i]->hOneGenPt     = new TH1D("onegenpt"   , Form("%s: Gen Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneGenE      = new TH1D("onegene"    , Form("%s: Gen E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneGenEta    = new TH1D("onegeneta"  , Form("%s: Gen Eta"  ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOneDeltaPt   = new TH1D("onedeltapt" , Form("%s: Gen Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hOneDeltaE    = new TH1D("onedeltae"  , Form("%s: Gen Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneDeltaEta  = new TH1D("onedeltaeta", Form("%s: Gen Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hOneMetDeltaPhi  = new TH1D("onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,   0,   5);
      //  //SVFit Info
      // fLepHist[i]->hOneSVPt      = new TH1D("onesvpt"    , Form("%s: SV Pt"   ,dirname)  , 200,   0, 200);
      // fLepHist[i]->hOneSVM       = new TH1D("onesvm"     , Form("%s: SV M"    ,dirname)  , 200,   0, 1e1);
      // fLepHist[i]->hOneSVEta     = new TH1D("onesveta"   , Form("%s: SV Eta"  ,dirname)  , 200, -10., 10.);
      // fLepHist[i]->hOneSVDeltaPt = new TH1D("onesvdeltapt"    , Form("%s: SV Delta Pt"   ,dirname)  , 200,-100, 100);
      // fLepHist[i]->hOneSVDeltaP  = new TH1D("onesvdeltap"     , Form("%s: SV Delta P"    ,dirname)  , 200,-500, 500);
      // fLepHist[i]->hOneSVDeltaE  = new TH1D("onesvdeltae"     , Form("%s: SV Delta E"    ,dirname)  , 200,-500, 500);
      // fLepHist[i]->hOneSVDeltaEta= new TH1D("onesvdeltaeta"   , Form("%s: SV Delta Eta"  ,dirname)  , 200, -10., 10.);

      // fLepHist[i]->hOneSlimEQ    = new TH1D("oneslimeq"    , Form("%s: OneSlimEQ"    ,dirname),   8,  -4,   4);
      // fLepHist[i]->hOneSlimMuQ   = new TH1D("oneslimmuq"   , Form("%s: OneSlimMuQ"   ,dirname),   8,  -4,   4);
      // fLepHist[i]->hOneSlimTauQ  = new TH1D("oneslimtauq"  , Form("%s: OneSlimTauQ"  ,dirname),   8,  -4,   4);
      // fLepHist[i]->hOneSlimEM    = new TH1D("oneslimem"    , Form("%s: OneSlimEM"    ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimEMSS  = new TH1D("oneslimemss"  , Form("%s: OneSlimEMSS"  ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimEMOS  = new TH1D("oneslimemos"  , Form("%s: OneSlimEMOS"  ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimMuM   = new TH1D("oneslimmum"   , Form("%s: OneSlimMuM"   ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimMuMSS = new TH1D("oneslimmumss" , Form("%s: OneSlimMuMSS" ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimMuMOS = new TH1D("oneslimmumos" , Form("%s: OneSlimMuMOS" ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimTauM  = new TH1D("oneslimtaum"  , Form("%s: OneSlimTauM"  ,dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimTauMSS= new TH1D("oneslimtaumss", Form("%s: OneSlimTauMSS",dirname), 100,  0.,200.);
      // fLepHist[i]->hOneSlimTauMOS= new TH1D("oneslimtaumos", Form("%s: OneSlimTauMOS",dirname), 100,  0.,200.);

      fLepHist[i]->hTwoPz        = new TH1D("twopz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPt        = new TH1D("twopt"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoP         = new TH1D("twop"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoM         = new TH1D("twom"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoEta       = new TH1D("twoeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoPhi       = new TH1D("twophi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hTwoD0        = new TH1D("twod0"      , Form("%s: D0"      ,dirname)  , 400,-0.1, 0.1);
      fLepHist[i]->hTwoIso       = new TH1D("twoiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  20);
      fLepHist[i]->hTwoID1       = new TH1D("twoid1"     , Form("%s: ID1"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hTwoID2       = new TH1D("twoid2"     , Form("%s: ID2"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hTwoRelIso    = new TH1D("tworeliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hTwoFlavor    = new TH1D("twoflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hTwoQ         = new TH1D("twoq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hTwoTrigger   = new TH1D("twotrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      fLepHist[i]->hTwoWeight    = new TH1D("twoweight"  , Form("%s: Weight"  ,dirname)  , 100, -1.,  3.);
      fLepHist[i]->hTwoTrigWeight= new TH1D("twotrigweight", Form("%s: Trig Weight"  ,dirname)  , 100, -1.,  3.);
      //Gen Info
      fLepHist[i]->hTwoGenPt     = new TH1D("twogenpt"   , Form("%s: Gen Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoGenE      = new TH1D("twogene"    , Form("%s: Gen E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoGenEta    = new TH1D("twogeneta"  , Form("%s: Gen Eta"  ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoDeltaPt   = new TH1D("twodeltapt" , Form("%s: Gen Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hTwoDeltaE    = new TH1D("twodeltae"  , Form("%s: Gen Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoDeltaEta  = new TH1D("twodeltaeta", Form("%s: Gen Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hTwoMetDeltaPhi  = new TH1D("twometdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,   0,   5);
      // //SVFit Info
      // fLepHist[i]->hTwoSVPt      = new TH1D("twosvpt"    , Form("%s: SV Pt"   ,dirname)  , 200,   0, 200);
      // fLepHist[i]->hTwoSVM       = new TH1D("twosvm"     , Form("%s: SV M"    ,dirname)  , 200,   0, 1e1);
      // fLepHist[i]->hTwoSVEta     = new TH1D("twosveta"   , Form("%s: SV Eta"  ,dirname)  , 200, -10., 10.);
      // fLepHist[i]->hTwoSVDeltaPt      = new TH1D("twosvdeltapt"    , Form("%s: SV Delta Pt"   ,dirname)  , 200,-100, 100);
      // fLepHist[i]->hTwoSVDeltaP       = new TH1D("twosvdeltap"     , Form("%s: SV Delta P"    ,dirname)  , 200,-500, 500);
      // fLepHist[i]->hTwoSVDeltaE       = new TH1D("twosvdeltae"     , Form("%s: SV Delta E"    ,dirname)  , 200,-500, 500);
      // fLepHist[i]->hTwoSVDeltaEta     = new TH1D("twosvdeltaeta"   , Form("%s: SV Delta Eta"  ,dirname)  , 200, -10., 10.);
      // fLepHist[i]->hTwoSlimEQ    = new TH1D("twoslimeq"  , Form("%s: TwoSlimEQ"  ,dirname),   8,  -4,   4);
      // fLepHist[i]->hTwoSlimMuQ   = new TH1D("twoslimmuq" , Form("%s: TwoSlimMuQ" ,dirname),   8,  -4,   4);
      // fLepHist[i]->hTwoSlimTauQ  = new TH1D("twoslimtauq", Form("%s: TwoSlimTauQ",dirname),   8,  -4,   4);
      // fLepHist[i]->hTwoSlimEM    = new TH1D("twoslimem"  , Form("%s: TwoSlimEM"  ,dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimEMSS  = new TH1D("twoslimemss"  , Form("%s: TwoSlimEMSS"  ,dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimEMOS  = new TH1D("twoslimemos"  , Form("%s: TwoSlimEMOS"  ,dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimMuM   = new TH1D("twoslimmum" , Form("%s: TwoSlimMuM" ,dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimMuMSS = new TH1D("twoslimmumss" , Form("%s: TwoSlimMuMSS" ,dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimMuMOS = new TH1D("twoslimmumos" , Form("%s: TwoSlimMuMOS" ,dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimTauM  = new TH1D("twoslimtaum", Form("%s: TwoSlimTauM",dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimTauMSS= new TH1D("twoslimtaumss", Form("%s: TwoSlimTauMSS",dirname), 100,  0.,200.);
      // fLepHist[i]->hTwoSlimTauMOS= new TH1D("twoslimtaumos", Form("%s: TwoSlimTauMOS",dirname), 100,  0.,200.);

      fLepHist[i]->hPtDiff        = new TH1D("ptdiff"      , Form("%s: 1 pT - 2 pT"      ,dirname)  , 200,-100., 100.);
      fLepHist[i]->hD0Diff        = new TH1D("d0diff"      , Form("%s: 2 D0 - 1 D0"      ,dirname)  , 400,-0.2, 0.2);

      //2D distributions
      fLepHist[i]->hTwoPtVsOnePt      = new TH2D("twoptvsonept", Form("%s: Two pT vs One pT", dirname), 200, 0, 200, 200, 0, 200);
    }
  }
}

void ZTauTauHistMaker::BookTrees() {
  if(fDYTesting) return;
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
      fTrees[i]->Branch("leponeidone",     &fTreeVars.leponeidone    );
      fTrees[i]->Branch("leponeidtwo",     &fTreeVars.leponeidtwo    );
      fTrees[i]->Branch("leponeidthree",   &fTreeVars.leponeidthree  );
      fTrees[i]->Branch("leptwopt",        &fTreeVars.leptwopt       );  
      fTrees[i]->Branch("leptwom",         &fTreeVars.leptwom	     );   
      fTrees[i]->Branch("leptwoeta",       &fTreeVars.leptwoeta      );  
      fTrees[i]->Branch("leptwod0",        &fTreeVars.leptwod0       );  
      fTrees[i]->Branch("leptwoiso",       &fTreeVars.leptwoiso      );  
      fTrees[i]->Branch("leptwoidone",     &fTreeVars.leptwoidone    );
      fTrees[i]->Branch("leptwoidtwo",     &fTreeVars.leptwoidtwo    );
      fTrees[i]->Branch("leptwoidthree",   &fTreeVars.leptwoidthree  );
      fTrees[i]->Branch("lepp",            &fTreeVars.lepp	     );   
      fTrees[i]->Branch("leppt",           &fTreeVars.leppt	     );   
      fTrees[i]->Branch("lepm",            &fTreeVars.lepm	     );   
      fTrees[i]->Branch("lepeta",          &fTreeVars.lepeta	     );   
      fTrees[i]->Branch("lepdeltaeta",     &fTreeVars.lepdeltaeta    );  
      fTrees[i]->Branch("lepdeltar",       &fTreeVars.lepdeltar      );  
      fTrees[i]->Branch("lepdeltaphi",     &fTreeVars.lepdeltaphi    );  
      fTrees[i]->Branch("deltaalphaz1",    &fTreeVars.deltaalphaz1   );
      fTrees[i]->Branch("deltaalphaz2",    &fTreeVars.deltaalphaz2   );
      fTrees[i]->Branch("deltaalphah1",    &fTreeVars.deltaalphah1   );
      fTrees[i]->Branch("deltaalphah2",    &fTreeVars.deltaalphah2   );
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
      fTrees[i]->Branch("jetpt",           &fTreeVars.jetpt	     );   
      fTrees[i]->Branch("njets",           &fTreeVars.njets	     );   
      fTrees[i]->Branch("nbjets",          &fTreeVars.nbjets	     );   
      fTrees[i]->Branch("nbjetsm",         &fTreeVars.nbjetsm	     );   
      fTrees[i]->Branch("nbjetsl",         &fTreeVars.nbjetsl	     );   
      fTrees[i]->Branch("nbjets20",        &fTreeVars.nbjets20       );   
      fTrees[i]->Branch("nbjets20m",       &fTreeVars.nbjets20m      );   
      fTrees[i]->Branch("nbjets20l",       &fTreeVars.nbjets20l      );   
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
  //force it to be that nbjets loose >= nbjets medium etc
  if(fForceBJetSense) {
    if(nBJets  > nBJetsM) nBJetsM = nBJets;
    if(nBJetsM > nBJetsL) nBJetsL = nBJetsM;
    if(nBJets20   > nBJets20M ) nBJets20M = nBJets20;
    if(nBJets20M  > nBJets20L ) nBJets20L = nBJets20M;
  }
  fTreeVars.leponept  = leptonOneP4->Pt();
  fTreeVars.leponem   = leptonOneP4->M();
  fTreeVars.leponeeta = leptonOneP4->Eta();
  fTreeVars.leponed0  = leptonOneD0;
  fTreeVars.leponeiso = leptonOneIso;
  fTreeVars.leponeidone   = 0.;
  fTreeVars.leponeidtwo   = 0.; 
  fTreeVars.leponeidthree = 0.;
  if(selection < 5) {//tau selection
    fTreeVars.leptwoidone   = tauDeepAntiEle;
    fTreeVars.leptwoidtwo   = tauDeepAntiMu; 
    fTreeVars.leptwoidthree = tauDeepAntiJet;
  } else { //no other ids for now
    fTreeVars.leptwoidone   = 0.;
    fTreeVars.leptwoidtwo   = 0.; 
    fTreeVars.leptwoidthree = 0.;
  }
  fTreeVars.leptwopt  = leptonTwoP4->Pt();
  fTreeVars.leptwom   = leptonTwoP4->M();
  fTreeVars.leptwoeta = leptonTwoP4->Eta();
  fTreeVars.leptwod0  = leptonTwoD0;
  fTreeVars.leptwoiso = leptonTwoIso;
  TLorentzVector lep = *leptonOneP4 + *leptonTwoP4;
  fTreeVars.lepp   = lep.P();
  fTreeVars.leppt  = lep.Pt();
  fTreeVars.lepm   = lep.M();
  fTreeVars.lepptoverm    = fTreeVars.leppt   /fTreeVars.lepm;
  fTreeVars.leponeptoverm = fTreeVars.leponept/fTreeVars.lepm;
  fTreeVars.leptwoptoverm = fTreeVars.leptwopt/fTreeVars.lepm;
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

  //definition from (14) and (16) of arxiv:1207.4894
  double hmass(125.), zmass(91.2), tmass(1.78);
  fTreeVars.alphaz1 = (zmass*zmass-tmass*tmass)/(2.*(*leptonOneP4)*(*leptonTwoP4));
  fTreeVars.alphah1 = (hmass*hmass-tmass*tmass)/(2.*(*leptonOneP4)*(*leptonTwoP4));
  fTreeVars.alpha2 = leptonTwoP4->Pt()/leptonOneP4->Pt(); //for lep 1 = tau, lep 2 = non-tau
  fTreeVars.alpha3 = leptonOneP4->Pt()/leptonTwoP4->Pt(); //for lep 1 = non-tau, lep 2 = tau
  fTreeVars.deltaalphaz1 = fTreeVars.alphaz1 - fTreeVars.alpha2;
  fTreeVars.deltaalphaz2 = fTreeVars.alphaz1 - fTreeVars.alpha3;
  fTreeVars.deltaalphah1 = fTreeVars.alphah1 - fTreeVars.alpha2;
  fTreeVars.deltaalphah2 = fTreeVars.alphah1 - fTreeVars.alpha3;

  //event variables
  fTreeVars.ht       = ht;
  fTreeVars.htsum    = htSum;
  fTreeVars.jetpt    = jetOneP4->Pt();
  fTreeVars.njets    = nJets;
  fTreeVars.nbjets   = nBJets;
  fTreeVars.nbjetsm  = nBJetsM;
  fTreeVars.nbjetsl  = nBJetsL;
  fTreeVars.nbjets20   = nBJets20;
  fTreeVars.nbjets20m  = nBJets20M;
  fTreeVars.nbjets20l  = nBJets20L;
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
  else if(selection == 9) selecName = "mumu";
  else if(selection == 18)selecName = "ee";
  else                    {
    std::cout << "---Warning! Entry " << fentry
	      << " has undentified selection " << selection
	      << " in " << __func__ << "!\n";
    selecName = "unknown";
  }

  if(fReprocessMVAs && !fDYTesting) {
    for(unsigned i = 0; i < fMvaNames.size(); ++i) {
      if((fMvaNames[i].Contains(selecName.Data()) || //is this selection
	  (selecName == "emu" && (fMvaNames[i].Contains("_e") || fMvaNames[i].Contains("_mu")))) && //or leptonic tau category
	 (fIsJetBinnedMVAs[i] < 0 || fIsJetBinnedMVAs[i] == min((int) nJets,2))) //and either not jet binned or right number of jets
	fMvaOutputs[i] = mva[i]->EvaluateMVA(fMvaNames[i].Data());
      else
	fMvaOutputs[i] = -2.;
      
      if(fMvaOutputs[i] < -2.1) 
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
float ZTauTauHistMaker::GetZPtWeight(double pt, double mass, int doReco) {
  float weight = 1.;
  TH2D* h = (doReco > 0) ? fZPtRecoScales : fZPtScales;
  if(!h) return weight;
  //ensure the values are within the bounds
  if(pt >= 1000.) pt = 999.;
  if(mass >= 1000.) mass = 999.;
  int binx = h->GetXaxis()->FindBin(mass);
  int biny = h->GetYaxis()->FindBin(pt);
  weight = h->GetBinContent(binx, biny);
  if(weight <= 0.) {
    cout << "WARNING! Z pT weight <= 0 = " << weight << " (pt, mass) = ("
	 << pt << ", " << mass << ") using doReco = " << doReco << " in entry = "<< fentry << "! Returning 1...\n";
    return 1.;
  }
  return weight;
}

void ZTauTauHistMaker::FillEventHistogram(EventHist_t* Hist) {
  // Hist->hLumiSection         ->Fill(lumiSection        , genWeight*eventWeight)      ;
  // Hist->hTriggerStatus       ->Fill(triggerStatus      , genWeight*eventWeight)      ;
  Hist->hEventWeight         ->Fill(eventWeight        );
  Hist->hGenWeight           ->Fill(genWeight          );
  if(!fDYTesting) {
    Hist->hMcEra               ->Fill(mcEra              , genWeight*eventWeight)   ;
    Hist->hGenTauFlavorWeight  ->Fill(genTauFlavorWeight );
    Hist->hPhotonIDWeight      ->Fill(photonIDWeight );
    Hist->hNPV                 ->Fill(nPV                , genWeight*eventWeight)      ;
    Hist->hNPU                 ->Fill(nPU                , genWeight*eventWeight)      ;
    Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
    Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
    Hist->hNSlimMuons          ->Fill(nSlimMuons         , genWeight*eventWeight)      ;
    // for(int icount = 0; icount < kMaxCounts; ++icount)
    //   Hist->hNMuonCounts[icount]->Fill(nMuonCounts[icount], genWeight*eventWeight)     ;
    Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
    Hist->hNSlimElectrons      ->Fill(nSlimElectrons     , genWeight*eventWeight)      ;
    // for(int icount = 0; icount < kMaxCounts; ++icount)
    //   Hist->hNElectronCounts[icount]->Fill(nElectronCounts[icount], genWeight*eventWeight);
    Hist->hNLowPtElectrons     ->Fill(nLowPtElectrons    , genWeight*eventWeight)      ;
    Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
    Hist->hNSlimTaus           ->Fill(nSlimTaus          , genWeight*eventWeight)      ;
    // for(int icount = 0; icount < kMaxCounts; ++icount)
    //   Hist->hNTauCounts[icount]->Fill(nTauCounts[icount] , genWeight*eventWeight)      ;
    Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
    Hist->hNSlimPhotons        ->Fill(nSlimPhotons       , genWeight*eventWeight)      ;
    Hist->hNSlimJets           ->Fill(nSlimJets          , genWeight*eventWeight)      ;
    Hist->hNGenTausHad         ->Fill(nGenTausHad        , genWeight*eventWeight)      ;
    Hist->hNGenTausLep         ->Fill(nGenTausLep        , genWeight*eventWeight)      ;
    Hist->hNGenTaus            ->Fill(nGenTausLep+nGenTausHad, genWeight*eventWeight)      ;
    Hist->hNGenElectrons       ->Fill(nGenElectrons      , genWeight*eventWeight)      ;
    Hist->hNGenMuons           ->Fill(nGenMuons          , genWeight*eventWeight)      ;

  }
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  Hist->hNJets20             ->Fill(nJets20            , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets              ->Fill(nBJets             , genWeight*eventWeight)      ;
  Hist->hNBJetsM             ->Fill(nBJetsM            , genWeight*eventWeight)      ;
  Hist->hNBJetsL             ->Fill(nBJetsL            , genWeight*eventWeight)      ;
  Hist->hNBJets20            ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20M           ->Fill(nBJets20M          , genWeight*eventWeight)      ;
  Hist->hNBJets20L           ->Fill(nBJets20L          , genWeight*eventWeight)      ;
  //Store basic info for all accepted jets
  for(UInt_t jet = 0; jet < nJets20; ++jet) {
    Hist->hJetsFlavor->Fill(jetsFlavor[jet], genWeight*eventWeight);
    int flavor = abs(jetsFlavor[jet]);
    if(flavor > 20) continue; //unknown parent
    
    int index = 0; //light jets
    if(flavor == 4)      index = 1; // c-jet
    else if(flavor == 5) index = 2; // b-jet
    double jpt = jetsPt[jet], jeta = jetsEta[jet], wt = genWeight*eventWeight;
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
    
  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hNTriggered          ->Fill(leptonOneFired+leptonTwoFired, genWeight*eventWeight)   ;
  Hist->hPuWeight	     ->Fill(puWeight	       , genWeight*eventWeight)   ;
  Hist->hTopPtWeight	     ->Fill(topPtWeight	       , genWeight*eventWeight)   ;
  Hist->hZPtWeight	     ->Fill(zPtWeight	       , genWeight*eventWeight)   ;
  if(!fDYTesting) {
    Hist->hTauDecayMode	     ->Fill(tauDecayMode       , genWeight*eventWeight)   ;
    Hist->hTauMVA		     ->Fill(tauMVA	       , genWeight*eventWeight)   ;
    Hist->hTauGenFlavor	     ->Fill(tauGenFlavor       , genWeight*eventWeight)   ;
    Hist->hTauGenFlavorHad     ->Fill(tauGenFlavorHad    , genWeight*eventWeight)   ;
    Hist->hTauDeepAntiEle      ->Fill(tauDeepAntiEle     , genWeight*eventWeight)   ;
    Hist->hTauDeepAntiMu       ->Fill(tauDeepAntiMu      , genWeight*eventWeight)   ;
    Hist->hTauDeepAntiJet      ->Fill(tauDeepAntiJet     , genWeight*eventWeight)   ;
  
    // Hist->hTauVetoedJetPt	     ->Fill(tauVetoedJetPt     , genWeight*eventWeight)   ;
    // Hist->hTauVetoedJetPtUnc   ->Fill(tauVetoedJetPtUnc  , genWeight*eventWeight)   ;
    Hist->hHtSum		     ->Fill(htSum	       , genWeight*eventWeight)   ;
    Hist->hHt		     ->Fill(ht		       , genWeight*eventWeight)   ;
    Hist->hHtPhi               ->Fill(htPhi              , genWeight*eventWeight)   ;

    if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
      Hist->hJetPt               ->Fill(jetOneP4->Pt()        , genWeight*eventWeight)   ;
      Hist->hJetM                ->Fill(jetOneP4->M()         , genWeight*eventWeight)   ;
      Hist->hJetEta              ->Fill(jetOneP4->Eta()       , genWeight*eventWeight)   ;
      Hist->hJetPhi              ->Fill(jetOneP4->Phi()       , genWeight*eventWeight)   ;
      Hist->hJetBTag             ->Fill(jetOneBTag            , genWeight*eventWeight)   ;
      Hist->hJetBMVA             ->Fill(jetOneBMVA            , genWeight*eventWeight)   ;
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
  } //end if(!fDYTesting)

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);

  if(!fDYTesting) {
    Hist->hMetVsPt             ->Fill(lepSys.Pt(), met   , genWeight*eventWeight)   ; 
    Hist->hMetVsM              ->Fill(lepSys.M(), met    , genWeight*eventWeight)   ; 
    Hist->hMetVsHtSum          ->Fill(htSum     , met    , genWeight*eventWeight)   ; 
    if(htSum > 0.) //avoid divide by 0 issue
      Hist->hMetOverSqrtHtSum  ->Fill(met/sqrt(htSum)    , genWeight*eventWeight)   ; 

    // Hist->hMassSVFit	     ->Fill(massSVFit          , genWeight*eventWeight)   ; 
    // Hist->hMassErrSVFit        ->Fill(massErrSVFit       , genWeight*eventWeight)   ;
    // Hist->hSVFitStatus         ->Fill(svFitStatus        , genWeight*eventWeight)   ;
  } //end if(!fDYTesting)
  
  TLorentzVector sys    = (*photonP4) + lepSys;
  TLorentzVector svLepSys = (leptonOneSVP4 && leptonTwoSVP4) ? (*leptonOneSVP4) + (*leptonTwoSVP4) : TLorentzVector(0.,0.,0.,0.);
  TLorentzVector svSys    = (leptonOneSVP4 && leptonTwoSVP4) ? (*photonP4) + svLepSys              : TLorentzVector(0.,0.,0.,0.);
  
  double lepDelR   = leptonOneP4->DeltaR(*leptonTwoP4);
  double lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  double lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  double htDelPhi  = abs(lepSys.Phi() - htPhi);
  double jetDelPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(lepSys.DeltaPhi(*jetOneP4)) : -1.;
  if(htDelPhi > M_PI)
    htDelPhi = abs(2.*M_PI - htDelPhi);
  double metDelPhi  = abs(lepSys.Phi() - metPhi);
  if(metDelPhi > M_PI)
    metDelPhi = abs(2.*M_PI - metDelPhi);
  double lepOneDelPhi  = abs(lepSys.Phi() - leptonOneP4->Phi());
  if(lepOneDelPhi > M_PI)
    lepOneDelPhi = abs(2.*M_PI - lepOneDelPhi);
  double lepTwoDelPhi  = abs(lepSys.Phi() - leptonTwoP4->Phi());
  if(lepTwoDelPhi > M_PI)
    lepTwoDelPhi = abs(2.*M_PI - lepTwoDelPhi);

  //angles between leptons and leading jet
  double lepOneJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonOneP4->DeltaR(*jetOneP4)            : -1.;
  double lepOneJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonOneP4->DeltaPhi(*jetOneP4))     : -1.;
  double lepOneJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonOneP4->Eta() - jetOneP4->Eta()) : -1.;
  double lepTwoJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonTwoP4->DeltaR(*jetOneP4)            : -1.;
  double lepTwoJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonTwoP4->DeltaPhi(*jetOneP4))     : -1.;
  double lepTwoJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonTwoP4->Eta() - jetOneP4->Eta()) : -1.;

  // float lepSVDelR   = -1.;
  // float lepSVDelPhi = -1.;
  // float lepSVDelEta = -1.;
  // float lepSVDelM   = -20.;
  // float lepSVDelPt  = -20.;
  // if(leptonOneSVP4 && leptonTwoSVP4
  //    && leptonOneSVP4->Pt() > 1.e-5 && leptonTwoSVP4->Pt() > 1.e-5) {
  //   lepSVDelR   = leptonOneSVP4->DeltaR(*leptonTwoSVP4)           ;
  //   lepSVDelPhi = abs(leptonOneSVP4->DeltaPhi(*leptonTwoSVP4))    ;
  //   lepSVDelEta = abs(leptonOneSVP4->Eta() - leptonTwoSVP4->Eta());
  //   lepSVDelM = svLepSys.M() - lepSys.M();
  //   lepSVDelPt = svLepSys.Pt() - lepSys.Pt();
  // }
  
  //for removing or alternate DY reweighting weights
  double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight;
  double recoweight = (fIsDY && zPtWeight > 0.) ? bareweight*GetZPtWeight(lepSys.Pt(), lepSys.M(), /*use reco weights*/ 1) : bareweight;
  double zpt   = (!fIsDY || zPt <  0.  ) ? lepSys.Pt() : zPt; //for DY to use given Z pT and Mass
  double zmass = (!fIsDY || zMass <  0.) ? lepSys.M()  : zMass;
  if(zpt < 0.) zpt = 0.;
  if(zmass < 0.) zmass = 0.;
  
  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepPt[1]     ->Fill(lepSys.Pt()            ,bareweight);
  Hist->hLepPt[2]     ->Fill(lepSys.Pt()            ,recoweight);
  Hist->hLepP         ->Fill(lepSys.P()             ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepM[1]      ->Fill(lepSys.M()             ,bareweight);
  Hist->hLepM[2]      ->Fill(lepSys.M()             ,recoweight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);

  //2D histograms for DY reweighting
  Hist->hLepPtVsM[0]  ->Fill(max(lepSys.M(), 0.), max(lepSys.Pt(), 0.),eventWeight*genWeight);
  Hist->hLepPtVsM[1]  ->Fill(max(lepSys.M(), 0.), max(lepSys.Pt(), 0.),bareweight);
  Hist->hLepPtVsM[2]  ->Fill(max(lepSys.M(), 0.), max(lepSys.Pt(), 0.),recoweight);
  Hist->hZPtVsM[0]    ->Fill(zmass, zpt  ,eventWeight*genWeight);
  Hist->hZPtVsM[1]    ->Fill(zmass, zpt  ,bareweight);
  Hist->hZPtVsM[2]    ->Fill(zmass, zpt  ,recoweight);
  Hist->hZPt[0]       ->Fill(zpt  ,eventWeight*genWeight);
  Hist->hZPt[1]       ->Fill(zpt  ,bareweight);
  Hist->hZPt[2]       ->Fill(zpt  ,recoweight);
  Hist->hZMass[0]     ->Fill(zmass,eventWeight*genWeight);
  Hist->hZMass[1]     ->Fill(zmass,bareweight);
  Hist->hZMass[2]     ->Fill(zmass,recoweight);
  
  Hist->hLepDeltaPhi  ->Fill(lepDelPhi              ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR    ->Fill(lepDelR                ,eventWeight*genWeight);
  Hist->hLepDelRVsPhi ->Fill(lepDelR , lepDelPhi    ,eventWeight*genWeight);  
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);

  if(fDYTesting) return;
  
  Hist->hAlpha[0]->Fill((double) fTreeVars.alphaz1, eventWeight*genWeight);
  Hist->hAlpha[1]->Fill((double) fTreeVars.alphah1, eventWeight*genWeight);
  Hist->hAlpha[2]->Fill((double) fTreeVars.alpha2, eventWeight*genWeight);
  Hist->hAlpha[3]->Fill((double) fTreeVars.alpha3, eventWeight*genWeight);
  Hist->hDeltaAlpha[0]->Fill((double) fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill((double) fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill((double) fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill((double) fTreeVars.deltaalphah2, eventWeight*genWeight);

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
  
  // Hist->hLepSVPt      ->Fill(svLepSys.Pt()            ,eventWeight*genWeight);
  // Hist->hLepSVM       ->Fill(svLepSys.M()             ,eventWeight*genWeight);
  // Hist->hLepSVEta     ->Fill(((svLepSys.Pt() > 1.e-5) ? svLepSys.Eta()  : -1e6),eventWeight*genWeight);
  // Hist->hLepSVPhi     ->Fill(svLepSys.Phi()           ,eventWeight*genWeight);

  // Hist->hLepSVDeltaPhi ->Fill(lepSVDelPhi              ,eventWeight*genWeight);
  // Hist->hLepSVDeltaEta ->Fill(lepSVDelEta              ,eventWeight*genWeight);
  // Hist->hLepSVDeltaM   ->Fill(lepSVDelM                ,eventWeight*genWeight);
  // Hist->hLepSVDeltaPt  ->Fill(lepSVDelPt               ,eventWeight*genWeight);
  // Hist->hLepSVPtOverM  ->Fill(svLepSys.Pt()/svLepSys.M() ,eventWeight*genWeight);
  
  		                
  Hist->hSysM          ->Fill(sys.M()      ,eventWeight*genWeight);
  Hist->hSysPt         ->Fill(sys.Pt()     ,eventWeight*genWeight);
  Hist->hSysEta        ->Fill(sys.Eta()    ,eventWeight*genWeight);  
  Hist->hSysMvsLepM    ->Fill(lepSys.M(), sys.M(), eventWeight*genWeight);
  
  Hist->hMTOne         ->Fill((double) fTreeVars.mtone  ,eventWeight*genWeight);
  Hist->hMTTwo         ->Fill((double) fTreeVars.mttwo  ,eventWeight*genWeight);

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

  Hist->hPTauVisFrac    ->Fill(1.*fTreeVars.ptauvisfrac , eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(1.*fTreeVars.mestimatetwo, eventWeight*genWeight);

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
    Hist->hMVA[i]      ->Fill(fMvaOutputs[i], eventWeight*genWeight);
    if (fTreeVars.train > 0) Hist->hMVATrain[i]->Fill(fMvaOutputs[i], fTreeVars.eventweight*((fFractionMVA > 0.) ? 1./fFractionMVA : 1.));
    if (fTreeVars.train < 0) Hist->hMVATest[i] ->Fill(fMvaOutputs[i], fTreeVars.eventweight*((fFractionMVA < 1.) ? 1./(1.-fFractionMVA) : 1.));
  }
  if(fFolderName != "llg_study") return;
  // TLorentzVector jets = *jetOneP4 + *jetTwoP4, jet1g = *jetOneP4 + *photonP4, jet2g = *jetTwoP4 + *photonP4;
  // TLorentzVector jetsg = jets + *photonP4, jet1l1 = *jetOneP4 + *leptonOneP4, jet1l2 = *jetOneP4 + *leptonTwoP4;
  // TLorentzVector jet2l1 = *jetTwoP4 + *leptonOneP4, jet2l2 = *jetTwoP4 + *leptonTwoP4;
  // TLorentzVector jetsl1 = jets + *leptonOneP4, jetsl2 = jets + *leptonTwoP4;
  // TLorentzVector jetsl1g = jetsl1 + *photonP4, jetsl2g = jetsl2 + *photonP4;
  // TLorentzVector jetsllg = jetsl1g + *leptonTwoP4;
  
  // Hist->hObjMasses[0 ]     ->Fill(jets.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[1 ]     ->Fill(jet1g.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[2 ]     ->Fill(jet2g.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[3 ]     ->Fill(jetsg.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[4 ]     ->Fill(jet1l1.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[5 ]     ->Fill(jet2l1.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[6 ]     ->Fill(jet1l2.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[7 ]     ->Fill(jet2l2.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[8 ]     ->Fill(jetsl1.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[9 ]     ->Fill(jetsl2.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[10]     ->Fill(jetsl1g.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[11]     ->Fill(jetsl2g.M() ,eventWeight*genWeight);
  // Hist->hObjMasses[12]     ->Fill(jetsllg.M() ,eventWeight*genWeight);
  // // Hist->hObjMasses[13]     ->Fill( ,eventWeight*genWeight);
  // Hist->hJetTwoM           ->Fill(jetTwoP4->M(),eventWeight*genWeight);
  // Hist->hJetTwoPt          ->Fill(jetTwoP4->Pt(),eventWeight*genWeight);
  // Hist->hJetTwoEta         ->Fill(jetTwoP4->Eta(),eventWeight*genWeight);
  // Hist->hJetTwoBTag        ->Fill(jetTwoBTag, genWeight*eventWeight)   ;
  // Hist->hJetTwoBMVA        ->Fill(jetTwoBMVA, genWeight*eventWeight)   ;
  // Hist->hJetsDeltaR        ->Fill(jetTwoP4->DeltaR(*jetOneP4),eventWeight*genWeight);
  // Hist->hJetsDeltaEta      ->Fill(abs(jetTwoP4->Eta()-jetOneP4->Eta()),eventWeight*genWeight);
  // Hist->hJetsDeltaPhi      ->Fill(abs(jetTwoP4->DeltaPhi(*jetOneP4)),eventWeight*genWeight);
  // Hist->hJetsPt            ->Fill(jets.Pt() ,eventWeight*genWeight);
  // Hist->hJetsEta           ->Fill(jets.Eta() ,eventWeight*genWeight);
  // Hist->hJetsGammaDeltaR   ->Fill(jets.DeltaR(*photonP4) ,eventWeight*genWeight);
  // Hist->hJetsGammaDeltaEta ->Fill(abs(jets.Eta() - photonP4->Eta()) ,eventWeight*genWeight);
  // Hist->hJetsGammaDeltaPhi ->Fill(abs(jets.DeltaPhi(*photonP4)) ,eventWeight*genWeight);
  // Hist->hJetsGammaPt       ->Fill(jetsg.Pt() ,eventWeight*genWeight);
  // Hist->hJetsGammaEta      ->Fill(jetsg.Eta() ,eventWeight*genWeight);
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
  /////////////
  //  Lep 1  //
  /////////////
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()            ,eventWeight*genWeight);
  Hist->hOnePt        ->Fill(leptonOneP4->Pt()            ,eventWeight*genWeight);
  Hist->hOneP         ->Fill(leptonOneP4->P()             ,eventWeight*genWeight);
  Hist->hOneM         ->Fill(leptonOneP4->M()             ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()           ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOneP4->Phi()           ,eventWeight*genWeight);
  Hist->hOneD0        ->Fill(leptonOneD0                  ,eventWeight*genWeight);
  Hist->hOneIso       ->Fill(leptonOneIso                 ,eventWeight*genWeight);
  Hist->hOneID1       ->Fill(leptonOneID1                 ,eventWeight*genWeight);
  Hist->hOneID2       ->Fill(leptonOneID2                 ,eventWeight*genWeight);
  double relIso1 = leptonOneIso/ leptonOneP4->Pt();
  Hist->hOneRelIso    ->Fill(relIso1                      ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(fabs(leptonOneFlavor)        ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOneFired               ,eventWeight*genWeight);
  Hist->hOneWeight    ->Fill(leptonOneWeight              ,eventWeight*genWeight);
  Hist->hOneTrigWeight->Fill(leptonOneTrigWeight          ,eventWeight*genWeight);

  double oneMetDelPhi  = abs(leptonOneP4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI)
    oneMetDelPhi = abs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);


  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()            ,eventWeight*genWeight);
  Hist->hTwoPt        ->Fill(leptonTwoP4->Pt()            ,eventWeight*genWeight);
  Hist->hTwoP         ->Fill(leptonTwoP4->P()             ,eventWeight*genWeight);
  Hist->hTwoM         ->Fill(leptonTwoP4->M()             ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()           ,eventWeight*genWeight);
  Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()           ,eventWeight*genWeight);
  Hist->hTwoD0        ->Fill(leptonTwoD0                  ,eventWeight*genWeight);
  Hist->hTwoIso       ->Fill(leptonTwoIso                 ,eventWeight*genWeight);
  Hist->hTwoID1       ->Fill(leptonTwoID1                 ,eventWeight*genWeight);
  Hist->hTwoID2       ->Fill(leptonTwoID2                 ,eventWeight*genWeight);
  double relIso2 = leptonTwoIso/ leptonTwoP4->Pt();
  Hist->hTwoRelIso    ->Fill(relIso2                      ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(fabs(leptonTwoFlavor)        ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwoFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwoFired               ,eventWeight*genWeight);
  Hist->hTwoWeight    ->Fill(leptonTwoWeight              ,eventWeight*genWeight);
  Hist->hTwoTrigWeight->Fill(leptonTwoTrigWeight          ,eventWeight*genWeight);

  double twoMetDelPhi  = abs(leptonTwoP4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI)
    twoMetDelPhi = abs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  Hist->hPtDiff      ->Fill(leptonOneP4->Pt()-leptonTwoP4->Pt() ,eventWeight*genWeight);
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

  fChain->GetEntry(entry,0);
  if(fVerbose > 0 || entry%50000 == 0) printf("Processing event: %12lld (%5.1f%%)\n", entry, entry*100./fChain->GetEntriesFast());

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

  //skip if it's data and lepton status doesn't match data set ( 1 = electron 2 = muon) unless allowing overlap and it passes bothl
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
  if(fVerbose > 0 ) std::cout << "MET = " << met << std::endl;


  // Trigger weights
  if(fRemoveTriggerWeights > 0) {
    if(leptonOneTrigWeight > 0.) eventWeight /= leptonOneTrigWeight;
    if(leptonTwoTrigWeight > 0.) eventWeight /= leptonTwoTrigWeight;
  }
  
  // DY z pT weights
  if(fIsDY && fRemoveZPtWeights > 0 && zPtWeight > 0.) {
    if(fVerbose > 0) std::cout << "Removing zPtWeight = " << zPtWeight << " from eventWeight = " << eventWeight;
    eventWeight /= zPtWeight;
    if(fVerbose > 0) std::cout << " --> " << eventWeight << std::endl;
  }
  //only re-weight the DY sample(s)
  if(fRemoveZPtWeights > 1 && fIsDY) {
    double zpt   = (!fIsDY || zPt < 0.  ) ? (*leptonOneP4+*leptonTwoP4).Pt() : zPt;
    double zmass = (!fIsDY || zMass < 0.) ? (*leptonOneP4+*leptonTwoP4).M()  : zMass;
    zpt = zpt < 0. ? 0. : zpt;
    zmass = zmass < 0. ? 0. : zmass;
    zPtWeight = GetZPtWeight(zpt, zmass);
    eventWeight *= zPtWeight;
    if(fVerbose > 0) std::cout << "For Z pT = " << zpt << " and Mass = " << zmass << " using Data/MC weight " << zPtWeight
			       << "--> event weight = " << eventWeight << std::endl;
  }

  //very different setup, process separately
  if(fFolderName == "llg_study") {
    ProcessLLGStudy();
    // return kTRUE;
  }

  //No weights in data
  if(fIsData) { eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.; genTauFlavorWeight = 1.; }
  
  //selections
  //use the tree name to choose the selection
  bool mutau = (fFolderName == "mutau") && nTaus == 1  && nMuons == 1;
  bool etau  = (fFolderName == "etau")  && nTaus == 1  && nElectrons == 1;
  bool emu   = (fFolderName == "emu")   && nMuons == 1 && nElectrons == 1;
  //reject overlaps
  if(mutau && etau) {mutau = false; etau = false;}
  if(emu && (mutau || etau)) {mutau = false; etau = false;}
  bool mumu  = fFolderName == "mumu" && nMuons == 2; //no other requirement
  bool ee    = fFolderName == "ee" && nElectrons == 2; //no other requirement
  bool llg_study = fFolderName == "llg_study" && (nElectrons + nMuons > 0) && nJets > 1 && nPhotons > 0 && (nElectrons + nMuons < 3);
  if(fFolderName == "llg_study" && llg_study && (nMuons + nElectrons == 1) && nTaus > 0) {
    *leptonTwoP4 = *tauP4;
    leptonTwoFlavor = tauFlavor;
  }
  if(fVerbose > 0) std::cout << "Event has selection statuses: mutau = " << mutau
			     << " etau = " << etau << " emu = " << emu
			     << " mumu = " << mumu << " and ee = " << ee << std::endl
			     << "eventWeight*genWeight = " << eventWeight*genWeight << std::endl;

  if(!(mutau || etau || emu || mumu || ee || llg_study)) {
    std::cout << "WARNING! Entry " << entry << " passes no selections!\n";    
    return kTRUE;
  }
  
  if((mutau + etau + emu + mumu + ee) > 1)
    std::cout << "WARNING! Entry " << entry << " passes multiple selections!\n";
  
  InitializeTreeVariables(mutau+2*etau+5*emu+9*mumu+18*ee+36*llg_study);
  //if splitting testing/training samples
  if(!fDYTesting && fFractionMVA > 0. && fIsData == 0)
    eventWeight *= (fTreeVars.train > 0.) ? 0. : 1./(1.-fFractionMVA); //if training, ignore, else rescale to account for training sample removed
  
  //FIXME temporary fix to hadronic tau ID if is not data
  // if((mutau || etau)&&fIsData==0) {
  //   eventWeight *= 1./0.95; //remove overall tau ID weight that didn't include if gen tau or not in the total event weight
  //   if(abs(tauGenFlavor) == 15 && fUseTauFakeSF == 1) genTauFlavorWeight *= 0.95; //apply to real taus to original weights
  // }
  
  //use locally computed weight 
  if(fUseTauFakeSF > 1 && fIsData == 0) genTauFlavorWeight = GetTauFakeSF(tauGenFlavor);
  
  //apply fake tau SF
  if(!fDYTesting && fUseTauFakeSF && fIsData == 0) eventWeight *= genTauFlavorWeight;

  //No weights in data
  if(fIsData) { eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.; genTauFlavorWeight = 1.; }

  bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  if(bisector.Mag() > 0.) bisector.SetMag(1.);
  double pxi_vis = (lp1+lp2)*bisector;
  double pxi_inv = missing*bisector;

  //count number of electrons, muons, and taus
  // if(fIsNano) CountSlimObjects();
  

  if(fVerbose > 0) std::cout << "Event has selection statuses: mutau = " << mutau
			     << " etau = " << etau << " emu = " << emu
			     << " mumu = " << mumu << " and ee = " << ee << std::endl;
  
  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: object number selection
  ////////////////////////////////////////////////////////////  
  if(!fDYTesting) {
    if(mutau && chargeTest) FillAllHistograms(kMuTau + 1);
    else if(mutau)          FillAllHistograms(kMuTau + 1 + fQcdOffset);
    if(etau  && chargeTest) FillAllHistograms(kETau  + 1);
    else if(etau)           FillAllHistograms(kETau  + 1 + fQcdOffset);
    if(emu   && chargeTest) FillAllHistograms(kEMu   + 1);
    else if(emu)            FillAllHistograms(kEMu   + 1 + fQcdOffset);
    if(mumu  && chargeTest) FillAllHistograms(kMuMu  + 1);
    else if(mumu)           FillAllHistograms(kMuMu  + 1 + fQcdOffset);
    if(ee    && chargeTest) FillAllHistograms(kEE    + 1);
    else if(ee)             FillAllHistograms(kEE    + 1 + fQcdOffset);
  }
  
  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  TLorentzVector* muon_2 = 0;
  TLorentzVector* electron_2 = 0;
  //same flavor categories
  if(abs(leptonOneFlavor) == abs(leptonTwoFlavor)) {
    if(abs(leptonOneFlavor) == 13) { //mumu
      muon = leptonOneP4;
      muon_2 = leptonTwoP4;
    } else if(abs(leptonOneFlavor) == 11) { //ee
      electron = leptonOneP4;
      electron_2 = leptonTwoP4;
    }
  } else {
    if(abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
    else if(abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
    if(abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
    else if(abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
    if(abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
    else if(abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;
  }
  mutau = mutau && (tau != 0) && (muon != 0);
  etau  = etau  && (tau != 0) && (electron != 0);
  emu   = emu   && (muon != 0) && (electron != 0);
  mumu  = mumu  && (muon != 0) && (muon_2 != 0);
  ee    = ee    && (electron != 0) && (electron_2 != 0);

  mutau = mutau && muon->Pt() > 25. && tau->Pt() > 20.;
  etau  = etau  && electron->Pt() > 28. && tau->Pt() > 20.;
  emu   = emu   && ((electron->Pt() > 28. && muon->Pt() > 5.) ||
		    (electron->Pt() > 10. && muon->Pt() > 25.));
  mumu  = mumu  && ((muon->Pt() > 25. && muon_2->Pt() > 5.) ||
		    (muon->Pt() > 10.  && muon_2->Pt() > 25.));
  ee    = ee    && ((electron->Pt() > 28. && electron_2->Pt() > 10.) ||
		    (electron->Pt() > 10. && electron_2->Pt() > 28.));

  ////////////////////////////////////////////////////////////
  // Set 2 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(!fDYTesting) {
    if(mutau && chargeTest) FillAllHistograms(kMuTau + 2);
    else if(mutau)          FillAllHistograms(kMuTau + 2 + fQcdOffset);
    if(etau  && chargeTest) FillAllHistograms(kETau  + 2);
    else if(etau)           FillAllHistograms(kETau  + 2 + fQcdOffset);
    if(emu  && chargeTest)  FillAllHistograms(kEMu   + 2);
    else if(emu)            FillAllHistograms(kEMu   + 2 + fQcdOffset);
    if(mumu  && chargeTest) FillAllHistograms(kMuMu  + 2);
    else if(mumu)           FillAllHistograms(kMuMu  + 2 + fQcdOffset);
    if(ee    && chargeTest) FillAllHistograms(kEE    + 2);
    else if(ee)             FillAllHistograms(kEE    + 2 + fQcdOffset);
  }
  
  mutau = mutau && abs(muon->Eta()) < 2.4;
  mutau = mutau && abs(tau->Eta()) < 2.3;
  mutau = mutau && abs(tau->DeltaR(*muon)) > 0.3;

  etau  = etau  && abs(electron->Eta()) < 2.5;
  etau  = etau  && abs(tau->Eta()) < 2.3;
  etau  = etau  && abs(tau->DeltaR(*electron)) > 0.3;

  emu   = emu   && abs(electron->Eta()) < 2.5;
  emu   = emu   && abs(muon->Eta()) < 2.4;
  emu   = emu   && abs(muon->DeltaR(*electron)) > 0.3;

  mumu  = mumu  && abs(muon->Eta()) < 2.4;
  mumu  = mumu  && abs(muon_2->Eta()) < 2.4;
  mumu  = mumu  && abs(muon->DeltaR(*muon_2)) > 0.3;
  
  ee    = ee    && abs(electron->Eta()) < 2.5;
  ee    = ee    && abs(electron_2->Eta()) < 2.5;
  ee    = ee    && abs(electron->DeltaR(*electron_2)) > 0.3;

  //mass cuts, due to generation ranges need > 50 GeV for all sets
  double mll = (*leptonOneP4+*leptonTwoP4).M();
  mutau &= mll > 51. && mll < 169.;
  etau  &= mll > 51. && mll < 169.;
  emu   &= mll > 51. && mll < 169.;
  mumu  &= mll > 51. && mll < 169.;
  ee    &= mll > 51. && mll < 169.;

  //ensure a trigger was fired
  mutau &= leptonOneFired || leptonTwoFired;
  etau  &= leptonOneFired || leptonTwoFired;
  emu   &= leptonOneFired || leptonTwoFired;
  mumu  &= leptonOneFired || leptonTwoFired;
  ee    &= leptonOneFired || leptonTwoFired;

  ////////////////////////////////////////////////////////////
  // Set 7 + selection offset: object eta + mass cuts
  ////////////////////////////////////////////////////////////

  if(mutau && chargeTest) FillAllHistograms(kMuTau + 7);
  else if(mutau)          FillAllHistograms(kMuTau + 7 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(kETau  + 7);
  else if(etau)           FillAllHistograms(kETau  + 7 + fQcdOffset);
  if(emu  && chargeTest)  FillAllHistograms(kEMu   + 7);
  else if(emu)            FillAllHistograms(kEMu   + 7 + fQcdOffset);
  if(mumu  && chargeTest) FillAllHistograms(kMuMu  + 7);
  else if(mumu)           FillAllHistograms(kMuMu  + 7 + fQcdOffset);
  if(mumu  && chargeTest) FillAllHistograms(kMuMu  + 7);
  else if(mumu)           FillAllHistograms(kMuMu  + 7 + fQcdOffset);
  if(ee    && chargeTest) FillAllHistograms(kEE    + 7);
  else if(ee)             FillAllHistograms(kEE    + 7 + fQcdOffset);


  if(fDYTesting) return kTRUE;
  
  //////////////////////////////////////////////////////////////
  //
  // Analysis cut section
  //
  //////////////////////////////////////////////////////////////
  //configure bjet counting based on selection
  fBJetCounting = 2; //use pT > 20 GeV/c
  if(emu)       fBJetTightness = 2; //loose b-jets
  else if(etau) fBJetTightness = 0; //-1; //no cut on b-jets
  else if(mutau)fBJetTightness = 0; //-1; //no cut on b-jets
  else if(mumu) fBJetTightness = 2; //loose b-jets
  else if(ee)   fBJetTightness = 2; //loose b-jets

  //define how we're counting bjets, ID and pT threshold
  if(fBJetCounting == 0) {
    if(fBJetTightness == 0) nBJetsUse = nBJets;
    else if(fBJetTightness == 1) nBJetsUse = nBJetsM;
    else if(fBJetTightness == 2) nBJetsUse = nBJetsL;
  // } else if(fBJetCounting == 1) {
  //   if(fBJetTightness == 0) nBJetsUse = nBJets25;
  //   else if(fBJetTightness == 1) nBJetsUse = nBJets25M;
  //   else if(fBJetTightness == 2) nBJetsUse = nBJets25L;
  } else if(fBJetCounting == 2) {
    if(fBJetTightness == 0) nBJetsUse = nBJets20;
    else if(fBJetTightness == 1) nBJetsUse = nBJets20M;
    else if(fBJetTightness == 2) nBJetsUse = nBJets20L;
  } else if(fBJetTightness == -1) //no b-jet cut
    nBJetsUse = 0;
  else {
    if(entry % 50000 == 0) printf("Bad bJetUse definition, Count = %i Tight = %i!\n", fBJetCounting, fBJetTightness);
  }

  ////////////////////////////////////////////////////////////
  // Set 5 + selection offset: add deep NN IDs              //
  ////////////////////////////////////////////////////////////

  mutau &= tauDeepAntiJet >= 50; //63 = tight
  mutau &= tauDeepAntiMu  >= 10; //15 = tight
  mutau &= tauDeepAntiEle >= 10; //15 = loose
  etau  &= tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //
  etau  &= tauDeepAntiEle >= 50; //63 = tight

  if(!fDYTesting) {
    if(mutau && chargeTest) FillAllHistograms(kMuTau + 5);
    else if(mutau)          FillAllHistograms(kMuTau + 5 + fQcdOffset);
    if(etau && chargeTest)  FillAllHistograms(kETau  + 5);
    else if(etau)           FillAllHistograms(kETau  + 5 + fQcdOffset);
    if(emu && chargeTest)   FillAllHistograms(kEMu   + 5);
    else if(emu)            FillAllHistograms(kEMu   + 5 + fQcdOffset);
  }  
  
  ////////////////////////////////////////////////////////////
  // Set 6 + selection offset: add counting veto            //
  ////////////////////////////////////////////////////////////
  // bool mct = nMuonCounts[0] == 1; //loose ID
  // bool ect = nElectronCounts[0] == 1; //WPL
  // bool tct = nTauCounts[37] == 1; //same as mutau selection requirements

  // mutau &= mct && tct;
  // etau  &= ect && tct;
  // emu   &= mct && ect;
  
  // if(mutau && chargeTest) FillAllHistograms(kMuTau + 6);
  // else if(mutau)          FillAllHistograms(kMuTau + 6 + fQcdOffset);
  // if(etau && chargeTest)  FillAllHistograms(kETau  + 6);
  // else if(etau)           FillAllHistograms(kETau  + 6 + fQcdOffset);
  // if(emu && chargeTest)   FillAllHistograms(kEMu   + 6);
  // else if(emu)            FillAllHistograms(kEMu   + 6 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 13 + selection offset: nBJets >= 1 (Top backgrounds)
  ////////////////////////////////////////////////////////////////////////////  
  if(!fDYTesting) {
    if(mutau && nBJetsUse > 0 && chargeTest) FillAllHistograms(kMuTau + 13);
    else if(mutau && nBJetsUse > 0)          FillAllHistograms(kMuTau + 13 + fQcdOffset);
    if(etau && nBJetsUse > 0 && chargeTest)  FillAllHistograms(kETau  + 13);
    else if(etau && nBJetsUse > 0)           FillAllHistograms(kETau  + 13 + fQcdOffset);
    if(emu && nBJetsUse > 0 && chargeTest)   FillAllHistograms(kEMu   + 13);
    else if(emu && nBJetsUse > 0)            FillAllHistograms(kEMu   + 13 + fQcdOffset);
  }
  
  //require no bjets
  mutau &= nBJetsUse == 0;
  etau  &= nBJetsUse == 0;
  emu   &= nBJetsUse == 0;
  mumu  &= nBJetsUse == 0;
  ee    &= nBJetsUse == 0;

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
  if(!fDYTesting) {
    bool wregion = ((etau && mTE > 80. && mTTau > 50.)
		    || (mutau && mTMu > 80. && mTTau > 50.)
		    || (emu && mTE > 80. && mTMu > 80.));
  
    if(mutau && wregion && chargeTest) FillAllHistograms(kMuTau + 14);
    else if(mutau && wregion)          FillAllHistograms(kMuTau + 14 + fQcdOffset);
    if(etau && wregion && chargeTest)  FillAllHistograms(kETau  + 14);
    else if(etau && wregion)           FillAllHistograms(kETau  + 14 + fQcdOffset);
    if(emu && wregion && chargeTest)   FillAllHistograms(kEMu   + 14);
    else if(emu && wregion)            FillAllHistograms(kEMu   + 14 + fQcdOffset);
  }
  
  ////////////////////////////////////////////////////////////////////////////
  // Set 15 + selection offset: Z backgrounds
  ////////////////////////////////////////////////////////////////////////////  
  if(!fDYTesting) {
    bool zregion = ((etau && mTE < 20. && fTreeVars.lepm > 40. && fTreeVars.lepm < 100.)
		    || (mutau && mTMu < 20. && fTreeVars.lepm > 40. && fTreeVars.lepm < 100.)
		    || (emu && mTE < 30. && mTMu < 50. && fTreeVars.lepm > 40. && fTreeVars.lepm < 80.));
  
    if(mutau && zregion && chargeTest) FillAllHistograms(kMuTau + 15);
    else if(mutau && zregion)          FillAllHistograms(kMuTau + 15 + fQcdOffset);
    if(etau && zregion && chargeTest)  FillAllHistograms(kETau  + 15);
    else if(etau && zregion)           FillAllHistograms(kETau  + 15 + fQcdOffset);
    if(emu && zregion && chargeTest)   FillAllHistograms(kEMu   + 15);
    else if(emu && zregion)            FillAllHistograms(kEMu   + 15 + fQcdOffset);
  }
  
  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(kMuTau + 8);
  else if(mutau)          FillAllHistograms(kMuTau + 8 + fQcdOffset);
  if(etau && chargeTest)  FillAllHistograms(kETau  + 8);
  else if(etau)           FillAllHistograms(kETau  + 8 + fQcdOffset);
  if(emu && chargeTest)   FillAllHistograms(kEMu   + 8);
  else if(emu)            FillAllHistograms(kEMu   + 8 + fQcdOffset);
  if(mumu && chargeTest)  FillAllHistograms(kMuMu  + 8);
  else if(mumu)           FillAllHistograms(kMuMu  + 8 + fQcdOffset);
  if(ee    && chargeTest) FillAllHistograms(kEE    + 8);
  else if(ee)             FillAllHistograms(kEE    + 8 + fQcdOffset);

  
  if(fDYTesting) return kTRUE;

  
  //just Z->mu+mu, for Z pT weights study
  // if(mumu && nJets == 0 && nPhotons == 0 && nTaus == 0 && chargeTest)  FillAllHistograms(kMuMu  + 9);
  // else if(mumu && nJets == 0 && nPhotons == 0 && nTaus == 0)           FillAllHistograms(kMuMu  + 9 + fQcdOffset);
  
  
  ////////////////////////////////////////////////////////////////////////////
  // Set 24-29 + selection offset: For testing purposes
  ////////////////////////////////////////////////////////////////////////////
  bool mutau_test = mutau && tauDeepAntiEle >  50;
  bool etau_test  = etau  && tauDeepAntiEle > 100;

  if(mutau_test && chargeTest) FillAllHistograms(kMuTau + 24);
  else if(mutau_test)          FillAllHistograms(kMuTau + 24 + fQcdOffset);
  if(etau_test && chargeTest)  FillAllHistograms(kETau  + 24);
  else if(etau_test)           FillAllHistograms(kETau  + 24 + fQcdOffset);

  // mutau_test &= tauDeepAntiEle > 50;
  // etau_test  &= tauDeepAntiEle > 100;

  // if(mutau_test && chargeTest) FillAllHistograms(kMuTau + 25);
  // else if(mutau_test)          FillAllHistograms(kMuTau + 25 + fQcdOffset);
  // if(etau_test && chargeTest)  FillAllHistograms(kETau  + 25);
  // else if(etau_test)           FillAllHistograms(kETau  + 25 + fQcdOffset);
  
  ////////////////////////////////////////////////////////////////////////////
  // Set 9-12 : BDT Cut
  ////////////////////////////////////////////////////////////////////////////
  //Total background Z0 MVAs
  if(mutau && chargeTest && fMvaOutputs[1] > fMvaCuts[1]) FillAllHistograms(kMuTau + 9);
  else if(mutau && fMvaOutputs[1] > fMvaCuts[1])          FillAllHistograms(kMuTau + 9 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[3] > fMvaCuts[3])  FillAllHistograms(kETau  + 9);
  else if(etau && fMvaOutputs[3] > fMvaCuts[3])           FillAllHistograms(kETau  + 9 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[5] > fMvaCuts[5])   FillAllHistograms(kEMu   + 9);
  else if(emu && fMvaOutputs[5] > fMvaCuts[5])            FillAllHistograms(kEMu   + 9 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[7] > fMvaCuts[7])   FillAllHistograms(kMuTauE+ 9);
  else if(emu && fMvaOutputs[7] > fMvaCuts[7])            FillAllHistograms(kMuTauE+ 9 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[9] > fMvaCuts[9])   FillAllHistograms(kETauMu+ 9);
  else if(emu && fMvaOutputs[9] > fMvaCuts[9])            FillAllHistograms(kETauMu+ 9 + fQcdOffset);
  if(mumu && chargeTest && fMvaOutputs[5] > fMvaCuts[5])  FillAllHistograms(kMuMu  + 9);
  else if(mumu && fMvaOutputs[5] > fMvaCuts[5])           FillAllHistograms(kMuMu  + 9 + fQcdOffset);
  if(ee && chargeTest && fMvaOutputs[5] > fMvaCuts[5])    FillAllHistograms(kEE    + 9);
  else if(ee && fMvaOutputs[5] > fMvaCuts[5])             FillAllHistograms(kEE    + 9 + fQcdOffset);
  //Total background higgs MVAs
  if(mutau && chargeTest && fMvaOutputs[0] > fMvaCuts[0]) FillAllHistograms(kMuTau + 10);
  else if(mutau && fMvaOutputs[0] > fMvaCuts[0])          FillAllHistograms(kMuTau + 10 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[2] > fMvaCuts[2])  FillAllHistograms(kETau  + 10);
  else if(etau && fMvaOutputs[2] > fMvaCuts[2])           FillAllHistograms(kETau  + 10 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[4] > fMvaCuts[4])   FillAllHistograms(kEMu   + 10);
  else if(emu && fMvaOutputs[4] > fMvaCuts[4])            FillAllHistograms(kEMu   + 10 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[6] > fMvaCuts[6])   FillAllHistograms(kMuTauE+ 10);
  else if(emu && fMvaOutputs[6] > fMvaCuts[6])            FillAllHistograms(kMuTauE+ 10 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[8] > fMvaCuts[8])   FillAllHistograms(kETauMu+ 10);
  if(mumu && chargeTest && fMvaOutputs[4] > fMvaCuts[4])  FillAllHistograms(kMuMu  + 10);
  else if(mumu && fMvaOutputs[4] > fMvaCuts[4])           FillAllHistograms(kMuMu  + 10 + fQcdOffset);
  if(ee && chargeTest && fMvaOutputs[4] > fMvaCuts[4])    FillAllHistograms(kEE    + 10);
  else if(ee && fMvaOutputs[4] > fMvaCuts[4])             FillAllHistograms(kEE    + 10 + fQcdOffset);

  // Mass window sets, before cuts
  // double mgll = (*photonP4 + (*leptonOneP4+*leptonTwoP4)).M();
  //Z0 window
  if(mutau && chargeTest && mll < 95. && mll > 60.)  FillAllHistograms(kMuTau + 11);
  else if(mutau && mll < 105. && mll > 60.)          FillAllHistograms(kMuTau + 11 + fQcdOffset);
  if(etau && chargeTest && mll < 95. && mll > 60.)   FillAllHistograms(kETau  + 11);
  else if(etau && mll < 95. && mll > 60.)            FillAllHistograms(kETau  + 11 + fQcdOffset);
  if(emu && chargeTest && mll < 100. && mll > 80.)   FillAllHistograms(kEMu   + 11);
  else if(emu && mll < 100. && mll > 80.)            FillAllHistograms(kEMu   + 11 + fQcdOffset);
  //higgs window
  if(mutau && chargeTest && mll < 130. && mll > 75.) FillAllHistograms(kMuTau + 12);
  else if(mutau && mll < 130. && mll > 75.)          FillAllHistograms(kMuTau + 12 + fQcdOffset);
  if(etau && chargeTest && mll < 130. && mll > 75.)  FillAllHistograms(kETau  + 12);
  else if(etau && mll < 130. && mll > 75.)           FillAllHistograms(kETau  + 12 + fQcdOffset);
  if(emu && chargeTest && mll < 130. && mll > 115.)  FillAllHistograms(kEMu   + 12);
  else if(emu && mll < 130. && mll > 115.)           FillAllHistograms(kEMu   + 12 + fQcdOffset);


  ////////////////////////////////////////////////////////////////////////////
  // Set 16-17 + selection offset: Box cuts
  ////////////////////////////////////////////////////////////////////////////
  //z ids
  // if(mutau && chargeTest && fEventId[0] && fEventId[0]->IDWord(fTreeVars) == 0)   FillAllHistograms(16);
  // else if(mutau && fEventId[0] && fEventId[0]->IDWord(fTreeVars) == 0)            FillAllHistograms(16 + fQcdOffset);
  // if(etau && chargeTest && fEventId[10] && fEventId[10]->IDWord(fTreeVars) == 0)  FillAllHistograms(36);
  // else if(etau && fEventId[10] && fEventId[10]->IDWord(fTreeVars) == 0)           FillAllHistograms(36 + fQcdOffset);
  if(emu && chargeTest && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)   FillAllHistograms(kEMu   + 16);
  else if(emu && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)            FillAllHistograms(kEMu   + 16 + fQcdOffset);
  if(mumu && chargeTest && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)  FillAllHistograms(kMuMu  + 16);
  else if(mumu && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)           FillAllHistograms(kMuMu  + 16 + fQcdOffset);
  if(ee && chargeTest && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)    FillAllHistograms(kEE    + 16);
  else if(ee && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)             FillAllHistograms(kEE    + 16 + fQcdOffset);
  if(emu && chargeTest && fEventId[21] && fEventId[21]->IDWord(fTreeVars) == 0)   FillAllHistograms(kEMu   + 18);
  else if(emu && fEventId[21] && fEventId[21]->IDWord(fTreeVars) == 0)            FillAllHistograms(kEMu   + 18 + fQcdOffset);
  if(mumu && chargeTest && fEventId[21] && fEventId[21]->IDWord(fTreeVars) == 0)  FillAllHistograms(kMuMu  + 18);
  else if(mumu && fEventId[21] && fEventId[21]->IDWord(fTreeVars) == 0)           FillAllHistograms(kMuMu  + 18 + fQcdOffset);
  if(ee && chargeTest && fEventId[21] && fEventId[21]->IDWord(fTreeVars) == 0)    FillAllHistograms(kEE    + 18);
  else if(ee && fEventId[21] && fEventId[21]->IDWord(fTreeVars) == 0)             FillAllHistograms(kEE    + 18 + fQcdOffset);
  //higgs ids
  // if(mutau && chargeTest && fEventId[30] && fEventId[30]->IDWord(fTreeVars) == 0) FillAllHistograms(17);
  // else if(mutau && fEventId[30] && fEventId[30]->IDWord(fTreeVars) == 0)          FillAllHistograms(17 + fQcdOffset);
  // if(etau && chargeTest && fEventId[40] && fEventId[40]->IDWord(fTreeVars) == 0)  FillAllHistograms(37);
  // else if(etau && fEventId[40] && fEventId[40]->IDWord(fTreeVars) == 0)           FillAllHistograms(37 + fQcdOffset);
  if(emu && chargeTest && fEventId[50] && fEventId[50]->IDWord(fTreeVars) == 0)   FillAllHistograms(kEMu   + 17);
  else if(emu && fEventId[50] && fEventId[50]->IDWord(fTreeVars) == 0)            FillAllHistograms(kEMu   + 17 + fQcdOffset);


  ////////////////////////////////////////////////////////////////////////////
  // Set 18-20 + selection offset: Jet categories 0, 1, >1
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && nJets == 0 && chargeTest) FillAllHistograms(kMuTau + 18);
  else if(mutau && nJets == 0)          FillAllHistograms(kMuTau + 18 + fQcdOffset);
  if(etau && nJets == 0 && chargeTest)  FillAllHistograms(kETau  + 18);
  else if(etau && nJets == 0)           FillAllHistograms(kETau  + 18 + fQcdOffset);
  // if(emu && nJets == 0 && chargeTest)   FillAllHistograms(kEMu   + 18);
  // else if(emu && nJets == 0)            FillAllHistograms(kEMu   + 18 + fQcdOffset);
  // if(mumu && nJets == 0 && chargeTest)  FillAllHistograms(kMuMu  + 18);
  // else if(mumu && nJets == 0)           FillAllHistograms(kMuMu  + 18 + fQcdOffset);

  if(mutau && nJets == 1 && chargeTest) FillAllHistograms(kMuTau + 19);
  else if(mutau && nJets == 1)          FillAllHistograms(kMuTau + 19 + fQcdOffset);
  if(etau && nJets == 1 && chargeTest)  FillAllHistograms(kETau  + 19);
  else if(etau && nJets == 1)           FillAllHistograms(kETau  + 19 + fQcdOffset);
  if(emu && nJets == 1 && chargeTest)   FillAllHistograms(kEMu   + 19);
  else if(emu && nJets == 1)            FillAllHistograms(kEMu   + 19 + fQcdOffset);
  if(mumu && nJets == 1 && chargeTest)  FillAllHistograms(kMuMu  + 19);
  else if(mumu && nJets == 1)           FillAllHistograms(kMuMu  + 19 + fQcdOffset);

  if(mutau && nJets > 1  && chargeTest) FillAllHistograms(kMuTau + 20);
  else if(mutau && nJets > 1 )          FillAllHistograms(kMuTau + 20 + fQcdOffset);
  if(etau && nJets > 1  && chargeTest)  FillAllHistograms(kETau  + 20);
  else if(etau && nJets > 1 )           FillAllHistograms(kETau  + 20 + fQcdOffset);
  if(emu && nJets > 1  && chargeTest)   FillAllHistograms(kEMu   + 20);
  else if(emu && nJets > 1 )            FillAllHistograms(kEMu   + 20 + fQcdOffset);
  if(mumu && nJets > 1 && chargeTest)   FillAllHistograms(kMuMu  + 20);
  else if(mumu && nJets > 1)            FillAllHistograms(kMuMu  + 20 + fQcdOffset);
  

  ////////////////////////////////////////////////////////////////////////////
  // Set 21-22 + selection offset: Photon categories 0, >1
  ////////////////////////////////////////////////////////////////////////////
  // if(mutau && nPhotons == 0 && chargeTest) FillAllHistograms(kMuTau + 21);
  // else if(mutau && nPhotons == 0)          FillAllHistograms(kMuTau + 21 + fQcdOffset);
  // if(etau && nPhotons == 0 && chargeTest)  FillAllHistograms(kETau  + 21);
  // else if(etau && nPhotons == 0)           FillAllHistograms(kETau  + 21 + fQcdOffset);
  // if(emu && nPhotons == 0 && chargeTest)   FillAllHistograms(kEMu   + 21);
  // else if(emu && nPhotons == 0)            FillAllHistograms(kEMu   + 21 + fQcdOffset);
  // if(mutau && nPhotons >= 1 && chargeTest) FillAllHistograms(kMuTau + 22);
  // else if(mutau && nPhotons >= 1)          FillAllHistograms(kMuTau + 22 + fQcdOffset);
  // if(etau && nPhotons >= 1 && chargeTest)  FillAllHistograms(kETau  + 22);
  // else if(etau && nPhotons >= 1)           FillAllHistograms(kETau  + 22 + fQcdOffset);
  // if(emu && nPhotons >= 1 && chargeTest)   FillAllHistograms(kEMu   + 22);
  // else if(emu && nPhotons >= 1)            FillAllHistograms(kEMu   + 22 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 23-24 JetMisID (tau) 23 Mario's cut (emu)
  ////////////////////////////////////////////////////////////////////////////
  //jets
  // if(mutau && tauGenFlavor < 7 && chargeTest) FillAllHistograms(kMuTau + 23); 
  // else if(mutau && tauGenFlavor < 7)          FillAllHistograms(kMuTau + 23 + fQcdOffset); //not necessarily for QCD, but maybe a control region
  // if(etau && tauGenFlavor < 7 && chargeTest)  FillAllHistograms(kETau  + 23);
  // else if(etau && tauGenFlavor < 7)           FillAllHistograms(kETau  + 23 + fQcdOffset);
  //leptons
  // if(mutau && abs(tauGenFlavor - 12) == 1 && chargeTest) FillAllHistograms(kMuTau + 24); 
  // else if(mutau && abs(tauGenFlavor - 12) == 1)          FillAllHistograms(kMuTau + 24 + fQcdOffset); 
  // if(etau && abs(tauGenFlavor - 12) == 1 && chargeTest)  FillAllHistograms(kETau  + 24);
  // else if(etau && abs(tauGenFlavor - 12) == 1)           FillAllHistograms(kETau  + 24 + fQcdOffset);

  bool marioID = (fFolderName == "emu") && nElectrons == 1 && nMuons == 1; //e+mu
  marioID = marioID && ((electron->Pt() > 30. && muon->Pt() > 10.) ||
			(electron->Pt() > 15. && muon->Pt() > 25.)); //trigger selection thresholds
  marioID = marioID && abs(electron->Eta()) < 2.5; //angular acceptances
  marioID = marioID && abs(muon->Eta()) < 2.4;
  marioID = marioID && abs(muon->DeltaR(*electron)) > 0.3;

  marioID &= (nBJetsM) == 0; //medium ID
  marioID &= (nJets == 0) || jetOneP4->Pt() < 78.; //highest pT jet cut
  marioID &= puppMETC < 28.; //MET cut (he used PUPPI, unclear if same correction)
  marioID = marioID && (electron->Pt() > 32. && muon->Pt() > 28.); //higher electron and muon pT threshold
  marioID &= fTreeVars.lepm > 75. && fTreeVars.lepm < 110.; //mass window
  
  if(emu && marioID && chargeTest)  FillAllHistograms(kEMu   + 23);
  else if(emu && marioID)           FillAllHistograms(kEMu   + 23 + fQcdOffset);

  
  return kTRUE;
}

void ZTauTauHistMaker::CountSlimObjects() {
  //reset counters
  for(int index = 0; index < kMaxCounts; ++index) {
    nMuonCounts[index] = 0;
    nElectronCounts[index] = 0;
    nTauCounts[index] = 0;
  }
  //count muons
  for(UInt_t index = 0; index < nSlimMuons; ++index) {
    if((*slimMuons)[index].looseId) {
      ++nMuonCounts[0];
      if((*slimMuons)[index].pfIsoId > 0) ++nMuonCounts[3 ];
      if((*slimMuons)[index].pfIsoId > 1) ++nMuonCounts[6 ];
      if((*slimMuons)[index].pfIsoId > 2) ++nMuonCounts[9 ];
      if((*slimMuons)[index].pfIsoId > 3) ++nMuonCounts[12];
      if((*slimMuons)[index].pfIsoId > 4) ++nMuonCounts[15];
      if((*slimMuons)[index].pfIsoId > 5) ++nMuonCounts[18];
      if((*slimMuons)[index].pt > 10.)    ++nMuonCounts[21];
      
    }
    if((*slimMuons)[index].mediumId) {
      ++nMuonCounts[1];
      if((*slimMuons)[index].pfIsoId > 0) ++nMuonCounts[4 ];
      if((*slimMuons)[index].pfIsoId > 1) ++nMuonCounts[7 ];
      if((*slimMuons)[index].pfIsoId > 2) ++nMuonCounts[10];
      if((*slimMuons)[index].pfIsoId > 3) ++nMuonCounts[13];
      if((*slimMuons)[index].pfIsoId > 4) ++nMuonCounts[16];
      if((*slimMuons)[index].pfIsoId > 5) ++nMuonCounts[19];
      if((*slimMuons)[index].pt > 10.)    ++nMuonCounts[22];
    }
    if((*slimMuons)[index].tightId) {
      ++nMuonCounts[2];
      if((*slimMuons)[index].pfIsoId > 0) ++nMuonCounts[5 ];
      if((*slimMuons)[index].pfIsoId > 1) ++nMuonCounts[8 ];
      if((*slimMuons)[index].pfIsoId > 2) ++nMuonCounts[11];
      if((*slimMuons)[index].pfIsoId > 3) ++nMuonCounts[14];
      if((*slimMuons)[index].pfIsoId > 4) ++nMuonCounts[17];
      if((*slimMuons)[index].pfIsoId > 5) ++nMuonCounts[20];
      if((*slimMuons)[index].pt > 10.)    ++nMuonCounts[23];
    }    
  }
  //count electrons
  for(UInt_t index = 0; index < nSlimElectrons; ++index) {
    if((*slimElectrons)[index].WPL) {
      ++nElectronCounts[0];
      if((*slimElectrons)[index].pt > 15.) ++nElectronCounts[3];
    }
    if((*slimElectrons)[index].WP80) {
      ++nElectronCounts[1];
      if((*slimElectrons)[index].pt > 15.) ++nElectronCounts[4];
    }
    if((*slimElectrons)[index].WP90) {
      ++nElectronCounts[2];
      if((*slimElectrons)[index].pt > 15.) ++nElectronCounts[5];
    }    
  }
  //count taus
  for(UInt_t index = 0; index < nSlimTaus; ++index) {
    TLorentzVector lv;
    lv.SetPtEtaPhiM((*slimTaus)[index].pt,
		    (*slimTaus)[index].eta,
		    (*slimTaus)[index].phi,
		    (*slimTaus)[index].mass);
    if((*slimTaus)[index].deepAntiJet > 0) {
      if((*slimTaus)[index].deepAntiMu > 0) {
	++nTauCounts[0];
	if((*slimTaus)[index].deepAntiEle > 0)  ++nTauCounts[ 2];
	if((*slimTaus)[index].deepAntiEle > 1)  ++nTauCounts[ 4];
	if((*slimTaus)[index].deepAntiEle > 3)  ++nTauCounts[ 6];
	if((*slimTaus)[index].deepAntiEle > 7)  ++nTauCounts[ 8];
	if((*slimTaus)[index].deepAntiEle > 15) ++nTauCounts[10];
	if(leptonOneP4->DeltaR(lv) > 0.3) {
	  ++nTauCounts[12];
	  if((*slimTaus)[index].deepAntiEle > 0) {
	    ++nTauCounts[16];
	    if((*slimTaus)[index].deepAntiJet > 1) ++nTauCounts[31];
	    if((*slimTaus)[index].deepAntiJet > 3) ++nTauCounts[33];
	    if((*slimTaus)[index].deepAntiJet > 7) ++nTauCounts[35];
	  }
	  if((*slimTaus)[index].deepAntiJet > 1) ++nTauCounts[25];
	  if((*slimTaus)[index].deepAntiJet > 3) ++nTauCounts[27];
	  if((*slimTaus)[index].deepAntiJet > 7) ++nTauCounts[29];
	}
	if(leptonTwoP4->DeltaR(lv) > 0.3)   ++nTauCounts[14];
      }
      if((*slimTaus)[index].deepAntiMu > 1) {
	++nTauCounts[1];
	if((*slimTaus)[index].deepAntiEle > 0)  ++nTauCounts[ 3];
	if((*slimTaus)[index].deepAntiEle > 1)  ++nTauCounts[ 5];
	if((*slimTaus)[index].deepAntiEle > 3)  ++nTauCounts[ 7];
	if((*slimTaus)[index].deepAntiEle > 7)  ++nTauCounts[ 9];
	if((*slimTaus)[index].deepAntiEle > 15) ++nTauCounts[11];
	if(leptonOneP4->DeltaR(lv) > 0.3) {
	  ++nTauCounts[13];
	  if((*slimTaus)[index].deepAntiEle > 0) {
	    ++nTauCounts[17];
	    if((*slimTaus)[index].deepAntiJet > 1) ++nTauCounts[32];
	    if((*slimTaus)[index].deepAntiJet > 3) ++nTauCounts[34];
	    if((*slimTaus)[index].deepAntiJet > 7) ++nTauCounts[36];
	    //same as ID criteria for selection tau:
	    if((*slimTaus)[index].deepAntiJet >= 50 &&
	       (*slimTaus)[index].deepAntiMu >= 10 &&
	       (*slimTaus)[index].deepAntiEle >= 10) ++nTauCounts[37];
	  }
	  if((*slimTaus)[index].deepAntiJet > 1) ++nTauCounts[26];
	  if((*slimTaus)[index].deepAntiJet > 3) ++nTauCounts[28];
	  if((*slimTaus)[index].deepAntiJet > 7) ++nTauCounts[30];
	}
	if(leptonTwoP4->DeltaR(lv) > 0.3)   ++nTauCounts[15];
      }
      if((*slimTaus)[index].deepAntiEle > 0) {
	++nTauCounts[18];
	if(leptonOneP4->DeltaR(lv) > 0.3)   ++nTauCounts[19];
	if(leptonTwoP4->DeltaR(lv) > 0.3) {
	  ++nTauCounts[20];
	  if(leptonOneP4->DeltaR(lv) > 0.3)   ++nTauCounts[21];
	}
      }
      if(leptonOneP4->DeltaR(lv) > 0.3)   ++nTauCounts[22];
      if(leptonTwoP4->DeltaR(lv) > 0.3) {
	++nTauCounts[23];
	if(leptonOneP4->DeltaR(lv) > 0.3)   ++nTauCounts[24];
      }
    }    
  }
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
