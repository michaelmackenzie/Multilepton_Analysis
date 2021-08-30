#define ZTauTauHistMaker_cxx
// The class definition in ZTauTauHistMaker.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.n


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


#include "interface/ZTauTauHistMaker.hh"
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
  fElectronIDWeight.verbose_ = fVerbose;
  fMuonIDWeight.verbose_ = fVerbose;
  fBTagWeight.verbose_ = fVerbose;

  fMuonJetToTauWeights[JetToTauComposition::kWJets] = new JetToTauWeight("MuonWJets", "mutau",   31,  301300, fSystematicSeed, 0);
  fMuonJetToTauWeights[JetToTauComposition::kZJets] = new JetToTauWeight("MuonZJets", "mutau",   31,  301300, fSystematicSeed, 0);
  fMuonJetToTauWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("MuonTop"  , "mutau",   38, 1101302, fSystematicSeed, 0);
  fMuonJetToTauWeights[JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonQCD"  , "mutau", 1030,  101300, fSystematicSeed, 0);
  // fMuonJetToTauWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("MuonTop"  , "mutau",   32, 1100300, fSystematicSeed, 0);

  fElectronJetToTauWeights[JetToTauComposition::kWJets] = new JetToTauWeight("ElectronWJets", "etau",   31,  301300, fSystematicSeed, 0);
  fElectronJetToTauWeights[JetToTauComposition::kZJets] = new JetToTauWeight("ElectronZJets", "etau",   31,  301300, fSystematicSeed, 0);
  fElectronJetToTauWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronTop"  , "etau",   38, 1101302, fSystematicSeed, 0);
  fElectronJetToTauWeights[JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronQCD"  , "etau", 1030,  101300, fSystematicSeed, 0);
  // fElectronJetToTauWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronTop"  , "etau",   32, 1100300, fSystematicSeed, 0);

  for(int itrig = 0; itrig < 3; ++itrig) triggerWeights[itrig] = 1.f;

  fSystematicShifts = new SystematicShifts(fSystematicSeed);
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
    if(fDoSystematics && fSysSets[index]) FillSystematicHistogram(fSystematicHist[index]);
    // if(!fDYTesting) FillPhotonHistogram(fPhotonHist[index]);
  } else
    printf("WARNING! Entry %lld, attempted to fill un-initialized histogram set %i!\n", fentry, index);
  if(fDoSystematics >= 0 && fWriteTrees && fTreeSets[index])
    fTrees[index]->Fill();
}

void ZTauTauHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  // BookPhotonHistograms();
  if(fWriteTrees && fDoSystematics >= 0) BookTrees();
  if(fDoSystematics) BookSystematicHistograms();
}

void ZTauTauHistMaker::BookEventHistograms() {
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
      fDirectories[0*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i]->cd();
      fEventHist[i] = new EventHist_t;
      // fEventHist[i]->hLumiSection            = new TH1D("lumiSection"         , Form("%s: LumiSection"         ,dirname)  , 200,    0, 4e6);
      // fEventHist[i]->hTriggerStatus          = new TH1D("triggerStatus"       , Form("%s: TriggerStatus"       ,dirname)  ,   3, -1.5, 1.5);
      fEventHist[i]->hEventWeight            = new TH1D("eventweight"         , Form("%s: EventWeight"         ,dirname)  , 100,   -5,   5);
      fEventHist[i]->hEventWeightMVA         = new TH1D("eventweightmva"      , Form("%s: EventWeightMVA"      ,dirname)  , 100,   -5,   5);
      fEventHist[i]->hGenWeight              = new TH1D("genweight"           , Form("%s: GenWeight"           ,dirname)  ,   5, -2.5, 2.5);
      fEventHist[i]->hGenTauFlavorWeight     = new TH1D("gentauflavorweight"  , Form("%s: GenTauFlavorWeight"  ,dirname)  ,  40,    0,   2);
      // fEventHist[i]->hPhotonIDWeight         = new TH1D("photonidweight"      , Form("%s: PhotonIDWeight"      ,dirname)  ,  40,    0,   2);
      fEventHist[i]->hJetToTauWeight         = new TH1D("jettotauweight"      , Form("%s: JetToTauWeight"      ,dirname)  ,  40,  0,   2);
      int jstart = SystematicGrouping::kJetToTau;
      fEventHist[i]->hJetToTauWeightGroup    = new TH1D("jettotauweightgroup" , Form("%s: JetToTauWeightGroup" ,dirname)  ,  50, jstart, 50+jstart);
      fEventHist[i]->hJetToTauWeightCorr     = new TH1D("jettotauweightcorr"  , Form("%s: JetToTauWeightCorr"  ,dirname)  , 100,  0,   5);
      for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
        fEventHist[i]->hJetToTauComps[ji] = new TH1D(Form("jettotaucomps_%i", ji), Form("%s: JetToTauComps %i",dirname, ji),  50,  0, 2);
        fEventHist[i]->hJetToTauWts  [ji] = new TH1D(Form("jettotauwts_%i"  , ji), Form("%s: JetToTauWts %i"  ,dirname, ji),  50,  0, 2);
      }
      fEventHist[i]->hIsSignal               = new TH1D("issignal"            , Form("%s: IsSignal"            ,dirname)  ,   5, -2,   3);
      fEventHist[i]->hNPV[0]                 = new TH1D("npv"                 , Form("%s: NPV"                 ,dirname)  , 200,  0, 200);
      fEventHist[i]->hNPV[1]                 = new TH1D("npv1"                , Form("%s: NPV"                 ,dirname)  , 200,  0, 200);
      fEventHist[i]->hNPU[0]                 = new TH1D("npu"                 , Form("%s: NPU"                 ,dirname)  , 100,  0, 100);
      fEventHist[i]->hNPU[1]                 = new TH1D("npu1"                , Form("%s: NPU"                 ,dirname)  , 100,  0, 100);
      fEventHist[i]->hNPartons               = new TH1D("npartons"            , Form("%s: NPartons"            ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hLHENJets               = new TH1D("lhenjets"            , Form("%s: LHE N(jets)"         ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNMuons                 = new TH1D("nmuons"              , Form("%s: NMuons"              ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNSlimMuons             = new TH1D("nslimmuons"          , Form("%s: NSlimMuons"          ,dirname)  ,  10,  0,  10);
      // for(int icount = 0; icount < kMaxCounts; ++icount)
      //        fEventHist[i]->hNMuonCounts[icount]  = new TH1D(Form("nmuoncounts%i",icount), Form("%s: NMuonCounts %i",dirname,icount),  10,  0,  10);
      fEventHist[i]->hNElectrons             = new TH1D("nelectrons"          , Form("%s: NElectrons"          ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNSlimElectrons         = new TH1D("nslimelectrons"      , Form("%s: NSlimElectrons"      ,dirname)  ,  10,  0,  10);
      // for(int icount = 0; icount < kMaxCounts; ++icount)
      //        fEventHist[i]->hNElectronCounts[icount]= new TH1D(Form("nelectroncounts%i",icount), Form("%s: NElectronCounts %i",dirname,icount),  10,  0,  10);
      // fEventHist[i]->hNLowPtElectrons        = new TH1D("nlowptelectrons"     , Form("%s: NLowPtElectrons"     ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNTaus                  = new TH1D("ntaus"               , Form("%s: NTaus"               ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNSlimTaus              = new TH1D("nslimtaus"           , Form("%s: NSlimTaus"           ,dirname)  ,  10,  0,  10);
      // for(int icount = 0; icount < kMaxCounts; ++icount)
      //        fEventHist[i]->hNTauCounts[icount]   = new TH1D(Form("ntaucounts%i",icount), Form("%s: NTauCounts %i",dirname,icount),  10,  0,  10);
      // fEventHist[i]->hNSlimPhotons           = new TH1D("nslimphotons"        , Form("%s: NSlimPhotons"        ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNPhotons               = new TH1D("nphotons"            , Form("%s: NPhotons"            ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNGenTausHad            = new TH1D("ngentaushad"         , Form("%s: NGenTausHad"         ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNGenTausLep            = new TH1D("ngentauslep"         , Form("%s: NGenTausLep"         ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNGenTaus               = new TH1D("ngentaus"            , Form("%s: NGenTaus"            ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNGenElectrons          = new TH1D("ngenelectrons"       , Form("%s: NGenElectrons"       ,dirname)  ,  10,  0,  10);
      fEventHist[i]->hNGenMuons              = new TH1D("ngenmuons"           , Form("%s: NGenMuons"           ,dirname)  ,  10,  0,  10);
      // fEventHist[i]->hNSlimJets              = new TH1D("nslimjets"           , Form("%s: NSlimJets"           ,dirname)  ,  30,  0,  30);
      fEventHist[i]->hNJets                  = new TH1D("njets"               , Form("%s: NJets"               ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNJets20[0]             = new TH1D("njets20"             , Form("%s: NJets20"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNJets20[1]             = new TH1D("njets201"            , Form("%s: NJets20"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNJets20Rej[0]          = new TH1D("njets20rej"          , Form("%s: NJets20Rej"          ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNJets20Rej[1]          = new TH1D("njets20rej1"         , Form("%s: NJets20Rej"          ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNFwdJets               = new TH1D("nfwdjets"            , Form("%s: NFwdJets"            ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets                 = new TH1D("nbjets"              , Form("%s: NBJets"              ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsM                = new TH1D("nbjetsm"             , Form("%s: NBJetsM"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJetsL                = new TH1D("nbjetsl"             , Form("%s: NBJetsL"             ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20[0]            = new TH1D("nbjets20"            , Form("%s: NBJets20"            ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20M[0]           = new TH1D("nbjets20m"           , Form("%s: NBJets20M"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20L[0]           = new TH1D("nbjets20l"           , Form("%s: NBJets20L"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20[1]            = new TH1D("nbjets201"           , Form("%s: NBJets20"            ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20M[1]           = new TH1D("nbjets20m1"          , Form("%s: NBJets20M"           ,dirname)  ,  20,  0,  20);
      fEventHist[i]->hNBJets20L[1]           = new TH1D("nbjets20l1"          , Form("%s: NBJets20L"           ,dirname)  ,  20,  0,  20);

      fEventHist[i]->hJetsFlavor             = new TH1D("jetsflavor"          , Form("%s: JetsFlavor"          ,dirname)  ,  60, -10,  50);
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
      fEventHist[i]->hMuonTriggerStatus      = new TH1D("muontriggerstatus"   , Form("%s: MuonTriggerStatus"   ,dirname) ,  10,   0, 10);
      fEventHist[i]->hNTriggered             = new TH1D("ntriggered"          , Form("%s: NTriggered"          ,dirname) ,  10,   0, 10);
      fEventHist[i]->hPuWeight               = new TH1D("puweight"            , Form("%s: PuWeight"            ,dirname) , 200,   0,  2);
      fEventHist[i]->hJetPUIDWeight          = new TH1D("jetpuidweight"       , Form("%s: JetPUIDWeight"       ,dirname) , 100,   0,  2);
      fEventHist[i]->hPrefireWeight          = new TH1D("prefireweight"       , Form("%s: PrefireWeight"       ,dirname) , 100,   0,  2);
      fEventHist[i]->hTopPtWeight            = new TH1D("topptweight"         , Form("%s: TopPtWeight"         ,dirname) , 200,   0,  2);
      fEventHist[i]->hBTagWeight             = new TH1D("btagweight"          , Form("%s: BTagWeight"          ,dirname) , 200,   0,  2);
      fEventHist[i]->hZPtWeight              = new TH1D("zptweight"           , Form("%s: ZPtWeight"           ,dirname) , 200,   0,  2);
      fEventHist[i]->hTauDecayMode[0]        = new TH1D("taudecaymode"        , Form("%s: TauDecayMode"        ,dirname) ,  15,   0, 15);
      fEventHist[i]->hTauDecayMode[1]        = new TH1D("taudecaymode1"       , Form("%s: TauDecayMode"        ,dirname) ,  15,   0, 15);
      // fEventHist[i]->hTauMVA                 = new TH1D("taumva"              , Form("%s: TauMVA"              ,dirname) , 100,   0,  1);
      fEventHist[i]->hTauGenFlavor           = new TH1D("taugenflavor"        , Form("%s: TauGenFlavor"        ,dirname) ,  50,   0, 50);
      // fEventHist[i]->hTauGenFlavorHad        = new TH1D("taugenflavorhad"     , Form("%s: TauGenFlavorHad"     ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauDeepAntiEle         = new TH1D("taudeepantiele"      , Form("%s: TauDeepAntiEle"      ,dirname) ,   30,  0, 30);
      fEventHist[i]->hTauDeepAntiMu          = new TH1D("taudeepantimu"       , Form("%s: TauDeepAntiMu"       ,dirname) ,   30,  0, 30);
      fEventHist[i]->hTauDeepAntiJet         = new TH1D("taudeepantijet"      , Form("%s: TauDeepAntiJet"      ,dirname) ,   30,  0, 30);
      // fEventHist[i]->hTauVetoedJetPt      = new TH1D("tauvetoedjetpt"      , Form("%s: TauVetoedJetPt"      ,dirname) , 210, -10,200);
      // fEventHist[i]->hTauVetoedJetPtUnc           = new TH1D("tauvetoedjetptunc"   , Form("%s: TauVetoedJetPtUnc"   ,dirname) , 110,  -1, 10);
      fEventHist[i]->hHtSum                  = new TH1D("htsum"               , Form("%s: HtSum"               ,dirname) , 200,   0,800);
      fEventHist[i]->hHt                     = new TH1D("ht"                  , Form("%s: Ht"                  ,dirname) , 200,   0,800);
      fEventHist[i]->hHtPhi                  = new TH1D("htphi"               , Form("%s: HtPhi"               ,dirname) , 100,  -4,  4);
      fEventHist[i]->hJetPt[0]               = new TH1D("jetpt"               , Form("%s: JetPt"               ,dirname) , 150,   0, 300);
      fEventHist[i]->hJetPt[1]               = new TH1D("jetpt1"              , Form("%s: JetPt"               ,dirname) , 150,   0, 300);
      // fEventHist[i]->hJetM                   = new TH1D("jetm"                , Form("%s: JetM"                ,dirname)  , 150,   0, 300);
      fEventHist[i]->hJetEta                 = new TH1D("jeteta"              , Form("%s: JetEta"              ,dirname)  , 200, -10,  10);
      // fEventHist[i]->hJetPhi                 = new TH1D("jetphi"              , Form("%s: JetPhi"              ,dirname)  , 100,  -4,  4);
      // fEventHist[i]->hJetBMVA                = new TH1D("jetbmva"             , Form("%s: JetBMVA"             ,dirname)  , 300, -1.,  2.);
      // fEventHist[i]->hJetBTag                = new TH1D("jetbtag"             , Form("%s: JetBTag"             ,dirname)  ,   2,   0,   2);
      fEventHist[i]->hTauPt                  = new TH1D("taupt"               , Form("%s: TauPt"               ,dirname)  , 300,   0, 300);
      // fEventHist[i]->hTauM                   = new TH1D("taum"                , Form("%s: TauM"                ,dirname)  , 100,   0, 4.);
      fEventHist[i]->hTauEta                 = new TH1D("taueta"              , Form("%s: TauEta"              ,dirname)  , 200, -10,  10);
      // fEventHist[i]->hTauPhi                 = new TH1D("tauphi"              , Form("%s: TauPhi"              ,dirname)  , 100,  -4,  4);

      // fEventHist[i]->hPFMet                  = new TH1D("pfmet"               , Form("%s: PF Met"              ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hPFMetPhi               = new TH1D("pfmetphi"            , Form("%s: PF MetPhi"           ,dirname)  ,  80, -4,   4);
      // fEventHist[i]->hPFCovMet00             = new TH1D("pfcovmet00"          , Form("%s: PF CovMet00"         ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPFCovMet01             = new TH1D("pfcovmet01"          , Form("%s: PF CovMet01"         ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hPFCovMet11             = new TH1D("pfcovmet11"          , Form("%s: PF CovMet11"         ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPuppMet                = new TH1D("puppmet"             , Form("%s: PUPPI Met"           ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hPuppMetPhi             = new TH1D("puppmetphi"          , Form("%s: PUPPI MetPhi"        ,dirname)  ,  80, -4,   4);
      // fEventHist[i]->hPuppCovMet00           = new TH1D("puppcovmet00"        , Form("%s: PUPPI CovMet00"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPuppCovMet01        = new TH1D("puppcovmet01"        , Form("%s: PUPPI CovMet01"      ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hPuppCovMet11        = new TH1D("puppcovmet11"        , Form("%s: PUPPI CovMet11"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hTrkMet                 = new TH1D("trkmet"              , Form("%s: Trk Met"             ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hTrkMetPhi              = new TH1D("trkmetphi"           , Form("%s: Trk MetPhi"          ,dirname)  ,  80, -4,   4);
      fEventHist[i]->hMet                    = new TH1D("met"                 , Form("%s: Met"                 ,dirname)  , 200,  0, 400);
      fEventHist[i]->hMetPhi                 = new TH1D("metphi"              , Form("%s: MetPhi"              ,dirname)  ,  80, -4,   4);
      fEventHist[i]->hMetCorr                = new TH1D("metcorr"             , Form("%s: Met Correction"      ,dirname)  , 200,  0, 40);
      fEventHist[i]->hMetCorrPhi             = new TH1D("metcorrphi"          , Form("%s: MetPhi Correction"   ,dirname)  ,  80, -4,   4);
      // fEventHist[i]->hCovMet00               = new TH1D("covmet00"            , Form("%s: CovMet00"            ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hCovMet01               = new TH1D("covmet01"            , Form("%s: CovMet01"            ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hCovMet11               = new TH1D("covmet11"            , Form("%s: CovMet11"            ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hMetVsPt                = new TH2D("metvspt"             , Form("%s: MetVsPt"             ,dirname) ,200,0,400, 200,0,400);
      // fEventHist[i]->hMetVsM                 = new TH2D("metvsm"              , Form("%s: MetVsM"              ,dirname) ,200,0,400, 200,0,400);
      // fEventHist[i]->hMetVsHtSum             = new TH2D("metvshtsum"          , Form("%s: MetVsHtSum"          ,dirname) ,200,0,400, 200,0,400);
      // fEventHist[i]->hMetOverSqrtHtSum       = new TH1D("metoversqrthtsum"    , Form("%s: MetOverSqrtHtSum"    ,dirname) ,  200,    0., 400.);
      // fEventHist[i]->hMassSVFit                   = new TH1D("masssvfit"           , Form("%s: MassSVFit"           ,dirname) , 1000,    0., 200.);
      // fEventHist[i]->hMassErrSVFit        = new TH1D("masserrsvfit"        , Form("%s: MassErrSVFit"        ,dirname) , 1000,    0., 100.);
      // fEventHist[i]->hSVFitStatus            = new TH1D("svfitstatus"         , Form("%s: SVFitStatus"         ,dirname) ,   10,    0.,  10.);


      fEventHist[i]->hLepPt[0]      = new TH1D("leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepPt[1]      = new TH1D("leppt1"        , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepPt[2]      = new TH1D("leppt2"        , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      // fEventHist[i]->hLepP          = new TH1D("lepp"          , Form("%s: Lepton P"       ,dirname)  , 200,   0, 400);
      // fEventHist[i]->hLepE          = new TH1D("lepe"          , Form("%s: Lepton E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepM[0]       = new TH1D("lepm"          , Form("%s: Lepton M"       ,dirname)  , 400,   0, 200);
      fEventHist[i]->hLepM[1]       = new TH1D("lepm1"         , Form("%s: Lepton M"       ,dirname)  , 400,   0, 200);
      fEventHist[i]->hLepM[2]       = new TH1D("lepm2"         , Form("%s: Lepton M"       ,dirname)  , 400,   0, 200);
      fEventHist[i]->hLepM[3]       = new TH1D("lepm3"         , Form("%s: Lepton M"       ,dirname)  ,  40,  70, 110);
      fEventHist[i]->hLepM[4]       = new TH1D("lepm4"         , Form("%s: Lepton M"       ,dirname)  ,  40, 105, 145);
      fEventHist[i]->hLepMt         = new TH1D("lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepEta        = new TH1D("lepeta"        , Form("%s: Lepton Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepPhi        = new TH1D("lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepMVsMVA[0]  = new TH2D("lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  , 200, -1., 1., 100,   50, 150);
      fEventHist[i]->hLepMVsMVA[1]  = new TH2D("lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  , 200, -1., 1., 100,   50, 150);

      //variable width bins for pT vs mass
      const int nmbins = 12;
      const double mbins[nmbins+1] = { 0.   , 70.  , 75.  , 80.  , 85.  ,
                                       90.  , 95.  , 100. , 105. , 110. ,
                                       120. , 140. ,
                                       1000.};
      // const int nmbins = 63;
      // const double mbins[nmbins+1] = { 0.   , 55.  , 60.  , 65.  , 70.  ,
      //                                  74.  , 77.  , 79.  , 81.  , 82.  ,
      //                                  83.  , 84.  , 84.75, 85.5 , 86.  ,
      //                                  86.5 , 87.  , 87.5 , 87.75, 88.  ,
      //                                  88.25, 88.5 , 88.75, 89.  , 89.25,
      //                                  89.5 , 89.75, 90.  , 90.15, 90.3 ,
      //                                  90.45, 90.6 , 90.75, 90.9 , 91.05,
      //                                  91.2 , 91.35, 91.5 , 91.75, 92.  ,
      //                                  92.25, 92.5 , 92.75, 93.  , 93.25,
      //                                  93.5 , 94.  , 94.5 , 95.  , 95.5 ,
      //                                  96.  , 96.5 , 97.  , 98.  , 99.  ,
      //                                  100. , 103. , 106. , 110. , 115. ,
      //                                  120. , 130. , 140. ,
      //                                  1000.};
      if(nmbins != sizeof(mbins)/sizeof(*mbins)-1) std::cout << "WARNING! N(pT bins) for 2D histograms is off! nbins = "
                                                             << nmbins << " size calc = " << sizeof(mbins)/sizeof(*mbins)-1
                                                             << std::endl;
      const int npbins = 27;
      const double pbins[npbins+1] = { 0.  , 1.  , 2.  , 3.  , 4.  ,
                                       5.  , 6.  , 7.  , 8.  , 9.  ,
                                       10. , 11. , 12. , 14. , 16. ,
                                       20. , 25. , 30. , 35. , 40. ,
                                       45. , 50. , 60. , 70. , 80. ,
                                       100., 150.,
                                       1000.};
      // const int npbins = 67;
      // const double pbins[npbins+1] = { 0.  , 0.5 , 1.  , 1.5 , 2.  ,
      //                                  2.5 , 2.75, 3.  , 3.25, 3.5 ,
      //                                  3.75, 4.  , 4.25, 4.5 , 4.75,
      //                                  5.  , 5.25, 5.5 , 5.75, 6.  ,
      //                                  6.25, 6.5 , 6.75, 7.  , 7.25,
      //                                  7.5 , 7.75, 8.  , 8.5 , 9.  ,
      //                                  9.5 , 10. , 10.5, 11. , 11.5,
      //                                  12. , 12.5, 13. , 13.5, 14. ,
      //                                  14.5, 15. , 16. , 17. , 18. ,
      //                                  19. , 20. , 22. , 24. , 26. ,
      //                                  28. , 30. , 33. , 36. , 40. ,
      //                                  43. , 46. , 50. , 55. , 60. ,
      //                                  65. , 70. , 75. , 80. , 88. ,
      //                                  105., 150.,
      //                                  1000.};

      if(npbins != sizeof(pbins)/sizeof(*pbins)-1) std::cout << "WARNING! N(pT bins) for 2D histograms is off! nbins = "
                                                             << npbins << " size calc = " << sizeof(pbins)/sizeof(*pbins)-1
                                                             << std::endl;

      fEventHist[i]->hLepPtVsM[0]   = new TH2D("lepptvsm0"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hLepPtVsM[1]   = new TH2D("lepptvsm1"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hLepPtVsM[2]   = new TH2D("lepptvsm2"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hLepPtVsM[3]   = new TH2D("lepptvsm3"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hLepPtVsM[4]   = new TH2D("lepptvsm4"     , Form("%s: Lepton Pt vs M" ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[0]     = new TH2D("zptvsm0"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[1]     = new TH2D("zptvsm1"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[2]     = new TH2D("zptvsm2"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[3]     = new TH2D("zptvsm3"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPtVsM[4]     = new TH2D("zptvsm4"       , Form("%s: Gen Z Pt vs M"  ,dirname)  , nmbins, mbins, npbins, pbins);
      fEventHist[i]->hZPt[0]        = new TH1D("zpt"           , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZPt[1]        = new TH1D("zpt1"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZPt[2]        = new TH1D("zpt2"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZPt[3]        = new TH1D("zpt3"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZPt[4]        = new TH1D("zpt4"          , Form("%s: Z Pt"           ,dirname)  , npbins,   pbins);
      fEventHist[i]->hZMass[0]      = new TH1D("zmass"         , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);
      fEventHist[i]->hZMass[1]      = new TH1D("zmass1"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);
      fEventHist[i]->hZMass[2]      = new TH1D("zmass2"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);
      fEventHist[i]->hZMass[3]      = new TH1D("zmass3"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);
      fEventHist[i]->hZMass[4]      = new TH1D("zmass4"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins);

      //Jet --> tau_h histograms
      fEventHist[i]->hLooseLep      = new TH1D("looselep"      , Form("%s: LooseLep"       ,dirname)  , 15, 0, 15);

      //variable width bins for eta vs pT
      const int ntetabins = 2;
      const double tetabins[ntetabins+1] = { 0., 1.5,
                                             2.3};

      if(ntetabins != sizeof(tetabins)/sizeof(*tetabins)-1) std::cout << "WARNING! N(tau eta bins) for 2D histograms is off! nbins = "
                                                                << ntetabins << " size calc = " << sizeof(tetabins)/sizeof(*tetabins)-1
                                                                << std::endl;
      const int ntpbins = 11;
      const double tpbins[ntpbins+1] = { 20., 22., 24., 26., 29.,
                                         33., 37., 42., 50., 60.,
                                         100.,
                                         200.};

      if(ntpbins != sizeof(tpbins)/sizeof(*tpbins)-1) std::cout << "WARNING! N(tau pT bins) for 2D histograms is off! nbins = "
                                                                << ntpbins << " size calc = " << sizeof(tpbins)/sizeof(*tpbins)-1
                                                                << std::endl;
      const int ndms = 4;
      const int njetsmax = 4;
      const int nptregions = 5;
      const int ncats = 3;
      for(int cat = 0; cat < ncats; ++cat) {
        for(int dm = 0; dm < ndms; ++dm) {
          for(int njets = 0; njets <= njetsmax; ++njets) {
            for(int iptr = 0; iptr < nptregions; ++iptr) {
              // std::cout << "Initializing iptr = " << iptr << " njets = " << njets << " cat = " << cat << " and dm = " << dm << std::endl;
              const char* hname = (iptr == 0) ? Form("faketau%ijet%idm_%i", njets, dm, cat) : Form("faketau%ijet%idm_%i_%i", njets, dm, cat, iptr);
              fEventHist[i]->hFakeTauNJetDMPtEta[iptr][cat][njets][dm]   = new TH2D(hname, Form("%s: Fake tau Eta vs Pt" ,dirname),
                                                                                  ntpbins, tpbins, ntetabins, tetabins);
              const char* hmcname = (iptr == 0) ? Form("faketaumc%ijet%idm_%i", njets, dm, cat) : Form("faketaumc%ijet%idm_%i_%i", njets, dm, cat, iptr);
              fEventHist[i]->hFakeTauMCNJetDMPtEta[iptr][cat][njets][dm] = new TH2D(hmcname, Form("%s: Fake tau MC Eta vs Pt" ,dirname),
                                                                                    ntpbins, tpbins, ntetabins, tetabins);
            }
          }
        }
      }

      fEventHist[i]->hTausPt        = new TH1D("tauspt"       , Form("%s: TausPt     "  ,dirname), 100,  0., 200.);
      fEventHist[i]->hTausEta       = new TH1D("tauseta"      , Form("%s: TausEta    "  ,dirname), 100, -5.,   5.);
      fEventHist[i]->hTausDM        = new TH1D("tausdm"       , Form("%s: TausDM     "  ,dirname),  15,  0.,  15.);
      fEventHist[i]->hTausAntiJet   = new TH1D("tausantijet"  , Form("%s: TausAntiJet"  ,dirname),  30,  0.,  30.);
      fEventHist[i]->hTausAntiEle   = new TH1D("tausantiele"  , Form("%s: TausAntiEle"  ,dirname),  30,  0.,  30.);
      fEventHist[i]->hTausAntiMu    = new TH1D("tausantimu"   , Form("%s: TausAntiMu"   ,dirname),  30,  0.,  30.);
      fEventHist[i]->hTausMVAAntiMu = new TH1D("tausmvaantimu", Form("%s: TausMVAAntiMu",dirname),  30,  0.,  30.);
      fEventHist[i]->hTausGenFlavor = new TH1D("tausgenflavor", Form("%s: TausGenFlavor",dirname),  40,  0.,  40.);
      fEventHist[i]->hTausDeltaR    = new TH1D("tausdeltar"   , Form("%s: TausDeltaR"   ,dirname), 100,  0.,   5.);
      fEventHist[i]->hFakeTausPt    = new TH1D("faketauspt"   , Form("%s: TausPt     "  ,dirname), 100,  0., 200.);
      fEventHist[i]->hFakeTausEta   = new TH1D("faketauseta"  , Form("%s: TausEta    "  ,dirname), 100, -5.,   5.);
      fEventHist[i]->hFakeTausDM    = new TH1D("faketausdm"   , Form("%s: TausDM     "  ,dirname),  15,  0.,  15.);

      //Jet --> light lep histograms
      //variable width bins for eta vs pT
      const int nletabins = 2;
      const double letabins[nletabins+1] = { 0., 1.5, 2.5};

      if(nletabins != sizeof(letabins)/sizeof(*letabins)-1) std::cout << "WARNING! N(light lep eta bins) for 2D histograms is off! nbins = "
                                                                << nletabins << " size calc = " << sizeof(letabins)/sizeof(*letabins)-1
                                                                << std::endl;
      const int nlpbins = 6;
      const double lpbins[nlpbins+1] = { 10., 15., 20., 25., 35.,
                                         50.,
                                         200.};

      if(nlpbins != sizeof(lpbins)/sizeof(*lpbins)-1) std::cout << "WARNING! N(light lep pT bins) for 2D histograms is off! nbins = "
                                                                << nlpbins << " size calc = " << sizeof(lpbins)/sizeof(*lpbins)-1
                                                                << std::endl;

      for(int cat = 0; cat < 3; ++cat)
        fEventHist[i]->hFakeLepPtEta[cat] = new TH2D(Form("fakeleppteta_%i", cat), Form("%s: Fake lepton Eta vs Pt" ,dirname), nlpbins, lpbins, nletabins, letabins);

      fEventHist[i]->hLeptonsPt   = new TH1D("leptonspt"  , Form("%s: LeptonsPt  ",dirname), 100,  0., 200.);
      fEventHist[i]->hLeptonsEta  = new TH1D("leptonseta" , Form("%s: LeptonsEta ",dirname), 100, -5.,   5.);
      fEventHist[i]->hLeptonsID   = new TH1D("leptonsid"  , Form("%s: LeptonsID  ",dirname),  15,  0.,  15.);
      fEventHist[i]->hLeptonsIsoID= new TH1D("leptonsisoid",Form("%s: LeptonsIsoID",dirname), 15,  0.,  15.);
      fEventHist[i]->hLeptonsGenFlavor= new TH1D("leptonsgenflavor",Form("%s: LeptonsGenFlavor",dirname), 50,  0.,  50.);

      //end light lep fake histograms

      fEventHist[i]->hLepDeltaPhi[0]= new TH1D("lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname)  ,  50,   0,   5);
      fEventHist[i]->hLepDeltaPhi[1]= new TH1D("lepdeltaphi1"  , Form("%s: Lepton DeltaPhi",dirname)  ,  50,   0,   5);
      fEventHist[i]->hLepDeltaEta   = new TH1D("lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDeltaR[0]  = new TH1D("lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname)  , 100,   0,   5);
      double drbins[] = {0.  , 1.2 , 2.  , 2.5 , 2.8 ,
                         3.  , 3.2 , 3.4 , 3.6 , 3.8 ,
                         4.  , 4.3 , 4.6 , 4.9 ,
                         6.};
      const int ndrbins = (sizeof(drbins)/sizeof(*drbins) - 1);
      fEventHist[i]->hLepDeltaR[1]  = new TH1D("lepdeltar1"    , Form("%s: Lepton DeltaR"  ,dirname)  , ndrbins, drbins);
      fEventHist[i]->hLepDeltaR[2]  = new TH1D("lepdeltar2"    , Form("%s: Lepton DeltaR"  ,dirname)  , ndrbins, drbins);
      fEventHist[i]->hLepDelRVsOneEta[0] = new TH2D("lepdelrvsoneeta" , Form("%s: LepDelRVsOneEta",dirname),  15, 0., 2.5, ndrbins, drbins);
      fEventHist[i]->hLepDelRVsOneEta[1] = new TH2D("lepdelrvsoneeta1", Form("%s: LepDelRVsOneEta",dirname),  15, 0., 2.5, ndrbins, drbins);
      fEventHist[i]->hLepDelPhiVsOneEta[0] = new TH2D("lepdelphivsoneeta" , Form("%s: LepDelPhiVsOneEta",dirname), 15, 0., 2.5, 15, 0., 3.5);
      fEventHist[i]->hLepDelPhiVsOneEta[1] = new TH2D("lepdelphivsoneeta1", Form("%s: LepDelPhiVsOneEta",dirname), 15, 0., 2.5, 15, 0., 3.5);

      // fEventHist[i]->hLepDelRVsPhi  = new TH2D("lepdelrvsphi"  , Form("%s: LepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);
      fEventHist[i]->hLepPtOverM    = new TH1D("lepptoverm"    , Form("%s: Lepton Pt / M"  ,dirname)  , 100,   0,  10);
      fEventHist[i]->hAlpha[0]      = new TH1D("alpha0"        , Form("%s: Alpha (Z) 0"    ,dirname)  , 100,   0,   5);
      fEventHist[i]->hAlpha[1]      = new TH1D("alpha1"        , Form("%s: Alpha (H) 0"    ,dirname)  , 100,   0,   5);
      fEventHist[i]->hAlpha[2]      = new TH1D("alpha2"        , Form("%s: Alpha 1"        ,dirname)  , 100,   0,   5);
      fEventHist[i]->hAlpha[3]      = new TH1D("alpha3"        , Form("%s: Alpha 2"        ,dirname)  , 100,   0,   5);
      fEventHist[i]->hDeltaAlpha[0] = new TH1D("deltaalpha0"   , Form("%s: Delta Alpha (Z) 0"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlpha[1] = new TH1D("deltaalpha1"   , Form("%s: Delta Alpha (Z) 1"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlpha[2] = new TH1D("deltaalpha2"   , Form("%s: Delta Alpha (H) 0"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlpha[3] = new TH1D("deltaalpha3"   , Form("%s: Delta Alpha (H) 1"  ,dirname)  , 200, -10,  10);
      fEventHist[i]->hDeltaAlphaM[0]= new TH1D("deltaalpham0"  , Form("%s: Delta Alpha Mass 0" ,dirname)  , 400,   0, 400);
      fEventHist[i]->hDeltaAlphaM[1]= new TH1D("deltaalpham1"  , Form("%s: Delta Alpha Mass 1" ,dirname)  , 400,   0, 400);
      fEventHist[i]->hDeltaAlphaMColM[0]= new TH1D("deltaalphamcolm0"  , Form("%s: MCol - Alpha Mass 0" ,dirname)  , 200,   -100, 100);
      fEventHist[i]->hDeltaAlphaMColM[1]= new TH1D("deltaalphamcolm1"  , Form("%s: MCol - Alpha Mass 1" ,dirname)  , 200,   -100, 100);

      // fEventHist[i]->hHtDeltaPhi    = new TH1D("htdeltaphi"    , Form("%s: Ht Lep Delta Phi",dirname) ,  50,   0,   5);
      fEventHist[i]->hMetDeltaPhi   = new TH1D("metdeltaphi"   , Form("%s: Met Lep Delta Phi",dirname),  50,   0,   5);
      // fEventHist[i]->hJetDeltaPhi   = new TH1D("jetdeltaphi"   , Form("%s: Jet Lep Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepOneDeltaPhi   = new TH1D("leponedeltaphi"   , Form("%s: Lep One vs Sys Delta Phi",dirname),  50,   0,   5);
      fEventHist[i]->hLepTwoDeltaPhi   = new TH1D("leptwodeltaphi"   , Form("%s: Lep Two vs Sys Delta Phi",dirname),  50,   0,   5);

      // fEventHist[i]->hLepOneJetDeltaR     = new TH1D("leponejetdeltar"     , Form("%s: Lep One vs Jet Delta R"  ,dirname),  60,   0,   6);
      // fEventHist[i]->hLepOneJetDeltaPhi   = new TH1D("leponejetdeltaphi"   , Form("%s: Lep One vs Jet Delta Phi",dirname),  50,   0,   5);
      // fEventHist[i]->hLepOneJetDeltaEta   = new TH1D("leponejetdeltaeta"   , Form("%s: Lep One vs Jet Delta Eta",dirname),  60,   0,   6);
      // fEventHist[i]->hLepTwoJetDeltaR     = new TH1D("leptwojetdeltar"     , Form("%s: Lep One vs Jet Delta R"  ,dirname),  60,   0,   6);
      // fEventHist[i]->hLepTwoJetDeltaPhi   = new TH1D("leptwojetdeltaphi"   , Form("%s: Lep One vs Jet Delta Phi",dirname),  50,   0,   5);
      // fEventHist[i]->hLepTwoJetDeltaEta   = new TH1D("leptwojetdeltaeta"   , Form("%s: Lep One vs Jet Delta Eta",dirname),  60,   0,   6);

      // fEventHist[i]->hLepSVPt       = new TH1D("lepsvpt"       , Form("%s: Lepton SVFit Pt"      ,dirname)  , 200,   0, 400);
      // fEventHist[i]->hLepSVM        = new TH1D("lepsvm"        , Form("%s: Lepton SVFit M"       ,dirname)  , 400,   0, 400);
      // fEventHist[i]->hLepSVEta      = new TH1D("lepsveta"      , Form("%s: Lepton SVFit Eta"     ,dirname)  , 200, -10,  10);
      // fEventHist[i]->hLepSVPhi      = new TH1D("lepsvphi"      , Form("%s: Lepton SVFit Phi"     ,dirname)  ,  80,  -4,   4);
      // fEventHist[i]->hLepSVDeltaPhi = new TH1D("lepsvdeltaphi" , Form("%s: Lepton SVFit DeltaPhi",dirname)  ,  50,   0,   5);
      // fEventHist[i]->hLepSVDeltaEta = new TH1D("lepsvdeltaeta" , Form("%s: Lepton SVFit DeltaEta",dirname)  , 100,   0,   5);
      // fEventHist[i]->hLepSVDeltaM   = new TH1D("lepsvdeltam"   , Form("%s: Lepton SVFit DeltaM"  ,dirname)  , 200, -10,  90);
      // fEventHist[i]->hLepSVDeltaPt  = new TH1D("lepsvdeltapt"  , Form("%s: Lepton SVFit DeltaPt" ,dirname)  , 200, -10,  90);
      // fEventHist[i]->hLepSVPtOverM  = new TH1D("lepsvptoverm"  , Form("%s: Lepton SVFit Pt / M"  ,dirname)  , 100,   0,  10);

      // fEventHist[i]->hSysM          = new TH1D("sysm"          , Form("%s: SysM"          ,dirname)  ,1000,  0, 1e3);
      // fEventHist[i]->hSysPt         = new TH1D("syspt"         , Form("%s: SysPt"         ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hSysEta        = new TH1D("syseta"        , Form("%s: SysEta"        ,dirname)  , 100, -5,   5);
      // fEventHist[i]->hSysMvsLepM    = new TH2D("sysmvslepm"    , Form("%s: SysMvsLepM"    ,dirname)  , 200, 0., 200., 200, 0., 200.);

      fEventHist[i]->hMTOne         = new TH1D("mtone"         , Form("%s: MTOne"         ,dirname)  , 200, 0.,   200.);
      fEventHist[i]->hMTTwo         = new TH1D("mttwo"         , Form("%s: MTTwo"         ,dirname)  , 200, 0.,   200.);
      fEventHist[i]->hMTLep         = new TH1D("mtlep"         , Form("%s: MTLep"         ,dirname)  , 200, 0.,   200.);
      fEventHist[i]->hMTOneOverM    = new TH1D("mtoneoverm"    , Form("%s: MTOneOverM"    ,dirname)  , 100, 0.,   10.);
      fEventHist[i]->hMTTwoOverM    = new TH1D("mttwooverm"    , Form("%s: MTTwoOverM"    ,dirname)  , 100, 0.,   10.);

      fEventHist[i]->hPXiVis[0]        = new TH1D("pxivis0"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);
      fEventHist[i]->hPXiInv[0]        = new TH1D("pxiinv0"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);
      fEventHist[i]->hPXiVisOverInv[0] = new TH1D("pxivisoverinv0" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);
      fEventHist[i]->hPXiInvVsVis[0]   = new TH2D("pxiinvvsvis0"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);
      fEventHist[i]->hPXiDiff[0]       = new TH1D("pxidiff0"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);
      fEventHist[i]->hPXiDiff2[0]      = new TH1D("pxidiff20"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);
      fEventHist[i]->hPXiDiff3[0]      = new TH1D("pxidiff30"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);
      // fEventHist[i]->hPXiVis[1]        = new TH1D("pxivis1"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);
      // fEventHist[i]->hPXiInv[1]        = new TH1D("pxiinv1"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);
      // fEventHist[i]->hPXiVisOverInv[1] = new TH1D("pxivisoverinv1" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);
      // fEventHist[i]->hPXiInvVsVis[1]   = new TH2D("pxiinvvsvis1"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);
      // fEventHist[i]->hPXiDiff[1]       = new TH1D("pxidiff1"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);
      // fEventHist[i]->hPXiDiff2[1]      = new TH1D("pxidiff21"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);
      // fEventHist[i]->hPXiDiff3[1]      = new TH1D("pxidiff31"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);
      // fEventHist[i]->hPXiVis[2]        = new TH1D("pxivis2"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);
      // fEventHist[i]->hPXiInv[2]        = new TH1D("pxiinv2"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);
      // fEventHist[i]->hPXiVisOverInv[2] = new TH1D("pxivisoverinv2" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);
      // fEventHist[i]->hPXiInvVsVis[2]   = new TH2D("pxiinvvsvis2"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);
      // fEventHist[i]->hPXiDiff[2]       = new TH1D("pxidiff2"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);
      // fEventHist[i]->hPXiDiff2[2]      = new TH1D("pxidiff22"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);
      // fEventHist[i]->hPXiDiff3[2]      = new TH1D("pxidiff32"      , Form("%s: PXiInv + (1-a)*PXiVis + b" ,dirname)  ,1000,-500., 500.);

      fEventHist[i]->hPTauVisFrac      = new TH1D("ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  ,300,0.,  1.5);
      fEventHist[i]->hLepMEstimate     = new TH1D("lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);
      fEventHist[i]->hLepMEstimateTwo  = new TH1D("lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);
      fEventHist[i]->hLepDot           = new TH1D("lepdot"         , Form("%s: sqrt(p1 dot p2)"          ,dirname)  ,200,0.,  400.);

      for(int mode = 0; mode < 3; ++mode) {
        fEventHist[i]->hLepOnePrimePx[mode] = new TH1D(Form("leponeprimepx%i", mode), Form("%s: l1 px" , dirname), 200, -200., 200.);
        fEventHist[i]->hLepTwoPrimePx[mode] = new TH1D(Form("leptwoprimepx%i", mode), Form("%s: l2 px" , dirname), 200, -200., 200.);
        fEventHist[i]->hMetPrimePx   [mode] = new TH1D(Form("metprimepx%i"   , mode), Form("%s: met px", dirname), 200, -200., 200.);
        fEventHist[i]->hLepOnePrimePy[mode] = new TH1D(Form("leponeprimepy%i", mode), Form("%s: l1 py" , dirname), 200, -200., 200.);
        fEventHist[i]->hLepTwoPrimePy[mode] = new TH1D(Form("leptwoprimepy%i", mode), Form("%s: l2 py" , dirname), 200, -200., 200.);
        fEventHist[i]->hMetPrimePy   [mode] = new TH1D(Form("metprimepy%i"   , mode), Form("%s: met py", dirname), 200, -200., 200.);
        fEventHist[i]->hLepOnePrimePz[mode] = new TH1D(Form("leponeprimepz%i", mode), Form("%s: l1 pz" , dirname), 200, -200., 200.);
        fEventHist[i]->hLepTwoPrimePz[mode] = new TH1D(Form("leptwoprimepz%i", mode), Form("%s: l2 pz" , dirname), 200, -200., 200.);
        fEventHist[i]->hMetPrimePz   [mode] = new TH1D(Form("metprimepz%i"   , mode), Form("%s: met pz", dirname), 200, -200., 200.);
        fEventHist[i]->hLepOnePrimeE [mode] = new TH1D(Form("leponeprimee%i" , mode), Form("%s: l1 e " , dirname), 200,    0., 200.);
        fEventHist[i]->hLepTwoPrimeE [mode] = new TH1D(Form("leptwoprimee%i" , mode), Form("%s: l2 e " , dirname), 200,    0., 200.);
        fEventHist[i]->hMetPrimeE    [mode] = new TH1D(Form("metprimee%i"    , mode), Form("%s: met e ", dirname), 200,    0., 200.);
      }

      fEventHist[i]->hPtSum[0]         = new TH1D("ptsum0"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);
      // fEventHist[i]->hPtSum[1]         = new TH1D("ptsum1"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);
      // fEventHist[i]->hPt1Sum[0]        = new TH1D("pt1sum0"        , Form("%s: Scalar Pt sum Lepton 1 + MET"     ,dirname)    ,1000,  0.,  1000.);
      // fEventHist[i]->hPt1Sum[1]        = new TH1D("pt1sum1"        , Form("%s: Scalar Pt sum Lepton 2 + MET"     ,dirname)    ,1000,  0.,  1000.);
      // fEventHist[i]->hPt1Sum[2]        = new TH1D("pt1sum2"        , Form("%s: Scalar Pt sum Lepton 1 + 2"       ,dirname)    ,1000,  0.,  1000.);
      // fEventHist[i]->hPt1Sum[3]        = new TH1D("pt1sum3"        , Form("%s: Scalar Pt sum Lepton 1 + 2 - MET" ,dirname)    ,1000,  0.,  1000.);
      for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
        //high mva score binning to improve cdf making
        fEventHist[i]->hMVA[j][0]     = new TH1D(Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig.names_[j].Data()) ,
                                                 fMVAConfig.NBins(j), fMVAConfig.Bins(j).data());
        fEventHist[i]->hMVA[j][1]     = new TH1D(Form("mva%i_1",j)     , Form("%s: %s MVA" ,dirname, fMVAConfig.names_[j].Data()) , 3000, -1.,  2.);
        fEventHist[i]->hMVATrain[j]   = new TH1D(Form("mvatrain%i",j), Form("%s: %s MVA (train)" ,dirname, fMVAConfig.names_[j].Data()) ,500, -3.,  2.);
        fEventHist[i]->hMVATest[j]    = new TH1D(Form("mvatest%i",j) , Form("%s: %s MVA (test)" ,dirname, fMVAConfig.names_[j].Data())  ,500, -3.,  2.);
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
      // int nbins_pt = 11; //for correcting jet -> tau scale factors
      // float pts[] = {0.  , 35. , 40. , 45. , 50. ,
      //                55. , 60. , 65. , 70. , 80. ,
      //                95. ,
      //                200.};
      int nbins_pt = 14; //for correcting jet -> tau scale factors
      float pts[] = {0.  , 20. , 25. , 30. , 35. ,
                     40. , 45. , 50. , 60. , 70. ,
                     80. , 90. , 120., 150.,
                     200.};

      float rbins[] = {0.  , 1.  , 1.5 , 2.  , 2.5,
                       3.  , 3.5 ,
                       5.};
      int nrbins = 7;
      fLepHist[i]->hOnePz        = new TH1D("onepz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      for(int j = 0; j < 12; ++j) {
        TString name = "onept"; if(j > 0) name += j;
        fLepHist[i]->hOnePt[j]     = new TH1D(name.Data(), Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      }
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettauoneptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        fLepHist[i]->hJetTauOnePtVsR[dmregion] = new TH2D(name_r.Data() , Form("%s: Delta R Vs One Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins);
        for(int ptregion = 0; ptregion < 4; ++ptregion) {
          TString name = "jettauonept";
          if(ptregion > 0) name += ptregion;
          if(dmregion > 0) {name += "_"; name += dmregion;}
          fLepHist[i]->hJetTauOnePt[dmregion][ptregion] = new TH1D(name.Data() , Form("%s: One Pt"   ,dirname)  , nbins_pt, pts);
        }
      }
      fLepHist[i]->hJetTauOneR   = new TH1D("jettauoner" , Form("%s: Delta R" ,dirname)  , nrbins, rbins);
      fLepHist[i]->hJetTauOneEta = new TH1D("jettauoneeta", Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5);
      fLepHist[i]->hJetTauOneMetDeltaPhi = new TH1D("jettauonemetdeltaphi", Form("%s: OneMetDeltaPhi"  ,dirname)  , 40,    0, 4.);
      fLepHist[i]->hOneP         = new TH1D("onep"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneM         = new TH1D("onem"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOnePtOverM   = new TH1D("oneptoverm" , Form("%s: Pt / M_{ll}",dirname)  , 100,   0,  10);
      fLepHist[i]->hOneEta       = new TH1D("oneeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOnePhi       = new TH1D("onephi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hOneD0        = new TH1D("oned0"      , Form("%s: D0"      ,dirname)  , 400,-0.1, 0.1);
      fLepHist[i]->hOneIso       = new TH1D("oneiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fLepHist[i]->hOneID1       = new TH1D("oneid1"     , Form("%s: ID1"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hOneID2       = new TH1D("oneid2"     , Form("%s: ID2"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hOneRelIso    = new TH1D("onereliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hOneFlavor    = new TH1D("oneflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hOneGenFlavor = new TH1D("onegenflavor", Form("%s: Gen Flavor"  ,dirname),  40,   0,  40);
      fLepHist[i]->hOneQ         = new TH1D("oneq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hOneTrigger   = new TH1D("onetrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      fLepHist[i]->hOneWeight    = new TH1D("oneweight"  , Form("%s: Weight"  ,dirname)  , 100,   0,   2);
      fLepHist[i]->hOneTrigWeight= new TH1D("onetrigweight", Form("%s: Trig Weight"  ,dirname)  , 100, 0.,  2.);
      fLepHist[i]->hOneWeight1Group = new TH1D("oneweight1group"  , Form("%s: Weight group"  ,dirname)  , 50, 0,  50);
      fLepHist[i]->hOneWeight2Group = new TH1D("oneweight2group"  , Form("%s: Weight group"  ,dirname)  , 50, 0,  50);
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
      for(int j = 0; j < 12; ++j) {
        TString name = "twopt"; if(j > 0) name += j;
        fLepHist[i]->hTwoPt[j]     = new TH1D(name.Data(), Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      }
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettautwoptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        fLepHist[i]->hJetTauTwoPtVsR[dmregion] = new TH2D(name_r.Data() , Form("%s: Delta R Vs Two Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins);
        for(int ptregion = 0; ptregion < 4; ++ptregion) {
          TString name = "jettautwopt";
          if(ptregion > 0) name += ptregion;
          if(dmregion > 0) {name += "_"; name += dmregion;}
          fLepHist[i]->hJetTauTwoPt[dmregion][ptregion] = new TH1D(name.Data() , Form("%s: Two Pt"   ,dirname)  , nbins_pt, pts);
        }
      }
      fLepHist[i]->hJetTauTwoR   = new TH1D("jettautwor" , Form("%s: Delta R" ,dirname)  , nrbins, rbins);
      fLepHist[i]->hJetTauTwoEta = new TH1D("jettautwoeta", Form("%s: |Eta|"  ,dirname)  ,  20,   0, 2.5);
      fLepHist[i]->hJetTauTwoMetDeltaPhi = new TH1D("jettautwometdeltaphi", Form("%s: TwoMetDeltaPhi"  ,dirname)  , 40,    0, 4.);
      fLepHist[i]->hTwoP         = new TH1D("twop"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoM         = new TH1D("twom"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoPtOverM   = new TH1D("twoptoverm" , Form("%s: Pt / M_{ll}",dirname)  , 100,   0,  10);
      fLepHist[i]->hTwoEta       = new TH1D("twoeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoPhi       = new TH1D("twophi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hTwoD0        = new TH1D("twod0"      , Form("%s: D0"      ,dirname)  , 400,-0.1, 0.1);
      fLepHist[i]->hTwoIso       = new TH1D("twoiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  20);
      fLepHist[i]->hTwoID1       = new TH1D("twoid1"     , Form("%s: ID1"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hTwoID2       = new TH1D("twoid2"     , Form("%s: ID2"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hTwoID3       = new TH1D("twoid3"     , Form("%s: ID3"     ,dirname)  ,  80,  -1,  79);
      fLepHist[i]->hTwoRelIso    = new TH1D("tworeliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hTwoFlavor    = new TH1D("twoflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hTwoGenFlavor = new TH1D("twogenflavor", Form("%s: Gen Flavor"  ,dirname),  40,   0,  40);
      fLepHist[i]->hTwoQ         = new TH1D("twoq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hTwoTrigger   = new TH1D("twotrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      fLepHist[i]->hTwoWeight1Group = new TH1D("twoweight1group"  , Form("%s: Weight group"  ,dirname)  , 50, 0,  50);
      fLepHist[i]->hTwoWeight2Group = new TH1D("twoweight2group"  , Form("%s: Weight group"  ,dirname)  , 50, 0,  50);
      fLepHist[i]->hTwoWeight    = new TH1D("twoweight"  , Form("%s: Weight"  ,dirname)  , 100,   0,   2);
      fLepHist[i]->hTwoTrigWeight= new TH1D("twotrigweight", Form("%s: Trig Weight"  ,dirname)  , 100, 0,  2);
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
      fLepHist[i]->hTwoPtVsOnePt       = new TH2D("twoptvsonept", Form("%s: Two pT vs One pT", dirname), 100, 0, 200, 100, 0, 200);
      int nbins_tau_pt = 6;
      float tau_pts[] = {0., 25., 30., 35., 40.,
                         50.,
                         150.};
      fLepHist[i]->hJetTauTwoPtVsOnePt = new TH2D("jettautwoptvsonept", Form("%s: Two pT vs One pT", dirname), nbins_pt, pts, nbins_tau_pt, tau_pts);
    }
  }
}

void ZTauTauHistMaker::BookSystematicHistograms() {
  for(int i = 0; i < fQcdOffset; ++i) {
    if(fSysSets[i]) { //turn on all offset histogram sets
      fSysSets[i+fQcdOffset] = 1;
      fSysSets[i+fMisIDOffset] = 1;
      fSysSets[i+fQcdOffset+fMisIDOffset] = 1;
    }
  }
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0 && fSysSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"systematic_%i",i);
      fDirectories[4*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[4*fn + i]->cd();
      fSystematicHist[i] = new SystematicHist_t;
      for(int sys = 0; sys < kMaxSystematics; ++sys) {
        fSystematicHist[i]->hLepM  [sys] = new TH1D(Form("lepm_%i"  , sys), Form("%s: LepM %i" , dirname, sys) , 400,   0, 200);
        fSystematicHist[i]->hLepPt [sys] = new TH1D(Form("leppt_%i" , sys), Form("%s: LepPt %i", dirname, sys) ,  50,   0, 200);
        fSystematicHist[i]->hOnePt [sys] = new TH1D(Form("onept_%i" , sys), Form("%s: Pt %i"   , dirname, sys) ,  50,   0, 200);
        fSystematicHist[i]->hOneEta[sys] = new TH1D(Form("oneeta_%i", sys), Form("%s: Eta %i"  , dirname, sys) ,  30,  -3,   3);
        fSystematicHist[i]->hTwoPt [sys] = new TH1D(Form("twopt_%i" , sys), Form("%s: Pt %i"   , dirname, sys) ,  50,   0, 200);
        fSystematicHist[i]->hTwoEta[sys] = new TH1D(Form("twoeta_%i", sys), Form("%s: Eta %i"  , dirname, sys) ,  30,  -3,   3);
        fSystematicHist[i]->hWeightChange[sys] = new TH1D(Form("weightchange_%i", sys), Form("%s: Relative weight change %i"  , dirname, sys) ,  50, -2.,  2.);
        for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
          fSystematicHist[i]->hMVA[j][sys] = new TH1D(Form("mva%i_%i",j, sys)     , Form("%s: %s MVA %i" ,dirname, fMVAConfig.names_[j].Data(), sys),
                                                      fMVAConfig.NBins(j), fMVAConfig.Bins(j).data());
                                                      // 300, -1.,  2.);
        }
      }
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
      fTrees[i]->Branch("leponept",           &fTreeVars.leponept          );
      fTrees[i]->Branch("leponeptoverm",      &fTreeVars.leponeptoverm     );
      // fTrees[i]->Branch("leponem",            &fTreeVars.leponem           );
      fTrees[i]->Branch("leponeeta",          &fTreeVars.leponeeta         );
      fTrees[i]->Branch("leponed0",           &fTreeVars.leponed0          );
      // fTrees[i]->Branch("leponeiso",          &fTreeVars.leponeiso         );
      // fTrees[i]->Branch("leponeidone",        &fTreeVars.leponeidone       );
      // fTrees[i]->Branch("leponeidtwo",        &fTreeVars.leponeidtwo       );
      // fTrees[i]->Branch("leponeidthree",      &fTreeVars.leponeidthree     );
      fTrees[i]->Branch("leptwopt",           &fTreeVars.leptwopt          );
      fTrees[i]->Branch("leptwoptoverm",      &fTreeVars.leptwoptoverm     );
      // fTrees[i]->Branch("leptwom",            &fTreeVars.leptwom           );
      fTrees[i]->Branch("leptwoeta",          &fTreeVars.leptwoeta         );
      fTrees[i]->Branch("leptwod0",           &fTreeVars.leptwod0          );
      // fTrees[i]->Branch("leptwoiso",          &fTreeVars.leptwoiso         );
      // fTrees[i]->Branch("leptwoidone",        &fTreeVars.leptwoidone       );
      // fTrees[i]->Branch("leptwoidtwo",        &fTreeVars.leptwoidtwo       );
      // fTrees[i]->Branch("leptwoidthree",      &fTreeVars.leptwoidthree     );
      // fTrees[i]->Branch("lepp",               &fTreeVars.lepp              );
      fTrees[i]->Branch("leppt",              &fTreeVars.leppt             );
      fTrees[i]->Branch("lepm",               &fTreeVars.lepm              );
      fTrees[i]->Branch("lepptoverm",         &fTreeVars.lepptoverm        );
      fTrees[i]->Branch("lepeta",             &fTreeVars.lepeta            );
      fTrees[i]->Branch("lepdeltaeta",        &fTreeVars.lepdeltaeta       );
      fTrees[i]->Branch("lepdeltar",          &fTreeVars.lepdeltar         );
      fTrees[i]->Branch("lepdeltaphi",        &fTreeVars.lepdeltaphi       );
      fTrees[i]->Branch("deltaalphaz1",       &fTreeVars.deltaalphaz1      );
      fTrees[i]->Branch("deltaalphaz2",       &fTreeVars.deltaalphaz2      );
      fTrees[i]->Branch("deltaalphah1",       &fTreeVars.deltaalphah1      );
      fTrees[i]->Branch("deltaalphah2",       &fTreeVars.deltaalphah2      );
      fTrees[i]->Branch("deltaalpham1",       &fTreeVars.deltaalpham1      );
      fTrees[i]->Branch("deltaalpham2",       &fTreeVars.deltaalpham2      );
      fTrees[i]->Branch("htdeltaphi",         &fTreeVars.htdeltaphi        );
      fTrees[i]->Branch("metdeltaphi",        &fTreeVars.metdeltaphi       );
      fTrees[i]->Branch("leponedeltaphi",     &fTreeVars.leponedeltaphi    );
      fTrees[i]->Branch("leptwodeltaphi",     &fTreeVars.leptwodeltaphi    );
      fTrees[i]->Branch("onemetdeltaphi",     &fTreeVars.onemetdeltaphi    );
      fTrees[i]->Branch("twometdeltaphi",     &fTreeVars.twometdeltaphi    );
      fTrees[i]->Branch("lepmestimate",       &fTreeVars.mestimate         );
      fTrees[i]->Branch("lepmestimatetwo",    &fTreeVars.mestimatetwo      );
      fTrees[i]->Branch("met",                &fTreeVars.met               );
      fTrees[i]->Branch("mtone",              &fTreeVars.mtone             );
      fTrees[i]->Branch("mttwo",              &fTreeVars.mttwo             );
      fTrees[i]->Branch("mtoneoverm",         &fTreeVars.mtoneoverm        );
      fTrees[i]->Branch("mttwooverm",         &fTreeVars.mttwooverm        );
      fTrees[i]->Branch("pxivis",             &fTreeVars.pxivis            );
      fTrees[i]->Branch("pxiinv",             &fTreeVars.pxiinv            );
      fTrees[i]->Branch("ht",                 &fTreeVars.ht                );
      fTrees[i]->Branch("htsum",              &fTreeVars.htsum             );
      fTrees[i]->Branch("jetpt",              &fTreeVars.jetpt             );
      fTrees[i]->Branch("njets",              &fTreeVars.njets             );

      //boosted frame variables
      for(int mode = 0; mode < 3; ++mode) {
        fTrees[i]->Branch(Form("leponeprimepx%i", mode), &fTreeVars.leponeprimepx[mode]);
        fTrees[i]->Branch(Form("leptwoprimepx%i", mode), &fTreeVars.leptwoprimepx[mode]);
        fTrees[i]->Branch(Form("metprimepx%i"   , mode), &fTreeVars.metprimepx   [mode]);
        fTrees[i]->Branch(Form("leponeprimepy%i", mode), &fTreeVars.leponeprimepy[mode]);
        fTrees[i]->Branch(Form("leptwoprimepy%i", mode), &fTreeVars.leptwoprimepy[mode]);
        fTrees[i]->Branch(Form("metprimepy%i"   , mode), &fTreeVars.metprimepy   [mode]);
        fTrees[i]->Branch(Form("leponeprimepz%i", mode), &fTreeVars.leponeprimepz[mode]);
        fTrees[i]->Branch(Form("leptwoprimepz%i", mode), &fTreeVars.leptwoprimepz[mode]);
        fTrees[i]->Branch(Form("metprimepz%i"   , mode), &fTreeVars.metprimepz   [mode]);
        fTrees[i]->Branch(Form("leponeprimee%i" , mode), &fTreeVars.leponeprimee [mode]);
        fTrees[i]->Branch(Form("leptwoprimee%i" , mode), &fTreeVars.leptwoprimee [mode]);
        fTrees[i]->Branch(Form("metprimee%i"    , mode), &fTreeVars.metprimee    [mode]);
      }

      fTrees[i]->Branch("eventweight",        &fTreeVars.eventweightMVA    );
      fTrees[i]->Branch("fulleventweight",    &fTreeVars.fulleventweight   );
      fTrees[i]->Branch("fulleventweightlum", &fTreeVars.fulleventweightlum);
      fTrees[i]->Branch("eventcategory",      &fTreeVars.eventcategory     );
      fTrees[i]->Branch("category",           &fTreeVars.category          );
      fTrees[i]->Branch("train",              &fTreeVars.train             );
      fTrees[i]->Branch("issignal",           &fTreeVars.issignal          );

      //add MVA score branches
      for(unsigned index = 0; index < fMVAConfig.names_.size(); ++index) {
        fTrees[i]->Branch(Form("mva%i", index), &fMvaOutputs[index]);
      }
    }
  }
}

//selections: 1 = mutau, 2 = etau, 5 = emu, 9 = mumu, 18 = ee
void ZTauTauHistMaker::InitializeTreeVariables(Int_t selection) {
  //force it to be that nbjets loose >= nbjets medium etc
  if(fForceBJetSense) {
    if(nBJets  > nBJetsM) nBJetsM = nBJets;
    if(nBJetsM > nBJetsL) nBJetsL = nBJetsM;
    if(nBJets20   > nBJets20M ) nBJets20M = nBJets20;
    if(nBJets20M  > nBJets20L ) nBJets20L = nBJets20M;
  }
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    fJetToTauWts      [proc] = 1.f;
    fJetToTauCorrs    [proc] = 1.f;
    fJetToTauComps    [proc] = 1.f;
    fJetToTauCompsUp  [proc] = 1.f;
    fJetToTauCompsDown[proc] = 1.f;
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
  fTreeVars.lepmt  = lep.Mt();
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
  fTreeVars.mtlep = sqrt(2.*met*lep.Pt()*(1.-cos(lep.Phi() - metPhi)));
  fTreeVars.mtoneoverm = fTreeVars.mtone / fTreeVars.lepm;
  fTreeVars.mttwooverm = fTreeVars.mttwo / fTreeVars.lepm;

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
  double pnuest    = std::max(1.e-8,lp2*missing/lp2.Mag()); //inv pT along tau = lep 2 pt unit vector dot missing
  double pnuesttwo = std::max(1.e-8,lp1*missing/lp1.Mag()); //inv pT along tau = lep 1 (for emu case with tau decay)
  fTreeVars.ptauvisfrac  = lp2.Mag() / (lp2.Mag() + pnuest);
  fTreeVars.mestimate    = fTreeVars.lepm/sqrt(fTreeVars.ptauvisfrac);
  fTreeVars.mestimatetwo = fTreeVars.lepm/sqrt(lp1.Mag() / (lp1.Mag() + pnuesttwo));

  //definition from (14) and (16) of arxiv:1207.4894
  double hmass(125.), zmass(91.2), tmass(1.78), lepdot(2.*((*leptonOneP4)*(*leptonTwoP4)));
  //delta alpha 1 = (m_boson^2 - m_tau^2) / (p(l1)\cdot p(l2))
  //delta alpha 2 = pT(l1) / pT(l2) (l1 = tau)
  //delta alpha 3 = pT(l2) / pT(l1) (l2 = tau)
  fTreeVars.alphaz1 = (zmass*zmass-tmass*tmass)/(lepdot);
  fTreeVars.alphah1 = (hmass*hmass-tmass*tmass)/(lepdot);
  fTreeVars.alpha2 = leptonTwoP4->Pt()/leptonOneP4->Pt(); //for lep 1 = tau, lep 2 = non-tau
  fTreeVars.alpha3 = leptonOneP4->Pt()/leptonTwoP4->Pt(); //for lep 2 = non-tau, lep 1 = tau
  fTreeVars.deltaalphaz1 = fTreeVars.alphaz1 - fTreeVars.alpha2;
  fTreeVars.deltaalphaz2 = fTreeVars.alphaz1 - fTreeVars.alpha3;
  fTreeVars.deltaalphah1 = fTreeVars.alphah1 - fTreeVars.alpha2;
  fTreeVars.deltaalphah2 = fTreeVars.alphah1 - fTreeVars.alpha3;
  //mass from delta alpha equation: m_boson = sqrt(m_tau^2 + pT(lep)/pT(tau) * p(l1) \cdot p(l2))
  fTreeVars.deltaalpham1 = std::sqrt(tmass*tmass + fTreeVars.alpha2 * lepdot); //lep 1 = tau
  fTreeVars.deltaalpham2 = std::sqrt(tmass*tmass + fTreeVars.alpha3 * lepdot); //lep 2 = tau
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
  fTreeVars.eventweightMVA = genWeight*eventWeight;
  fTreeVars.fulleventweight = genWeight*eventWeight*fXsec;
  fTreeVars.fulleventweightlum = fTreeVars.fulleventweight*fLum;

  //Perform a transform to the Z/H boson frame
  //Requires knowledge of which lepton has associated neutrinos (or if none do)
  TLorentzVector metP4;
  metP4.SetPtEtaPhiE(missing.Pt(), 0., missing.Phi(), missing.Pt());
  for(int imode = 0; imode < 3; ++imode) {
    TLorentzVector system;
    system = *leptonOneP4 + *leptonTwoP4;
    if(imode < 2) system += metP4;
    TVector3 boost = -1*system.BoostVector();
    boost.SetZ(0.); //only transform pT to 0
    TLorentzVector lepOnePrime(*leptonOneP4);
    TLorentzVector lepTwoPrime(*leptonTwoP4);
    TLorentzVector metPrime(missing, missing.Mag());
    lepOnePrime.Boost(boost); lepTwoPrime.Boost(boost); metPrime.Boost(boost); //boost so system pT = 0
    double phiRot;
    if(imode < 2) { //set MET along x axis
      phiRot = -metPrime.Phi();
    } else { //set lepton two pT (muon in emu) along the x-axis
      phiRot = -lepTwoPrime.Phi();
    }
    lepOnePrime.RotateZ(phiRot); lepTwoPrime.RotateZ(phiRot); metPrime.RotateZ(phiRot);

    //if the tau has a negative momentum, rotate about x 180 degrees to make it positive
    if((imode != 1 && lepTwoPrime.Pz() < 0.) || (imode == 1 && lepOnePrime.Pz() < 0.)) { //for emu, use the muon
      lepOnePrime.RotateX(M_PI); lepTwoPrime.RotateX(M_PI); metPrime.RotateX(M_PI);
    }
    if(std::isnan(lepOnePrime.Px()) || std::isnan(lepOnePrime.Py()) || std::isnan(lepOnePrime.Pz()) || std::isnan(lepOnePrime.E())) {
      std::cout << "!!! Entry " << fentry << " has NaN boosted lepton p4 components!\n ";
      leptonOneP4->Print();
      leptonTwoP4->Print();
      metP4.Print();
      system.Print();
      boost.Print();
      lepOnePrime.Print();
      lepTwoPrime.Print();
      metPrime.Print();
      fTreeVars.leponeprimepx[imode] = 0.;
      fTreeVars.leptwoprimepx[imode] = 0.;
      fTreeVars.metprimepx   [imode] = 0.;
      fTreeVars.leponeprimepy[imode] = 0.;
      fTreeVars.leptwoprimepy[imode] = 0.;
      fTreeVars.metprimepy   [imode] = 0.;
      fTreeVars.leponeprimepz[imode] = 0.;
      fTreeVars.leptwoprimepz[imode] = 0.;
      fTreeVars.metprimepz   [imode] = 0.;
      fTreeVars.leponeprimee [imode] = 0.;
      fTreeVars.leptwoprimee [imode] = 0.;
      fTreeVars.metprimee    [imode] = 0.;
    } else {
      fTreeVars.leponeprimepx[imode] = lepOnePrime.Px();
      fTreeVars.leptwoprimepx[imode] = lepTwoPrime.Px();
      fTreeVars.metprimepx   [imode] =    metPrime.Px();
      fTreeVars.leponeprimepy[imode] = lepOnePrime.Py();
      fTreeVars.leptwoprimepy[imode] = lepTwoPrime.Py();
      fTreeVars.metprimepy   [imode] =    metPrime.Py();
      fTreeVars.leponeprimepz[imode] = lepOnePrime.Pz();
      fTreeVars.leptwoprimepz[imode] = lepTwoPrime.Pz();
      fTreeVars.metprimepz   [imode] =    metPrime.Pz();
      fTreeVars.leponeprimee [imode] = lepOnePrime.E();
      fTreeVars.leptwoprimee [imode] = lepTwoPrime.E();
      fTreeVars.metprimee    [imode] =    metPrime.E();
    }
  }

  fTreeVars.eventcategory = fEventCategory;
  if(fFractionMVA > 0.) fTreeVars.train = (fRnd->Uniform() < fFractionMVA) ? 1. : -1.; //whether or not it is in the training sample
  fTreeVars.issignal = (fIsData == 0) ? (2.*(fIsSignal) - 1.) : 0.; //signal = 1, background = -1, data = 0
  //if splitting testing/training samples
  if(!fDYTesting)
    fTreeVars.eventweightMVA *= (fTreeVars.train > 0.) ? 0. : 1./(1.-fFractionMVA); //if training, ignore, else rescale to account for training sample removed

  TString selecName = "";
  if(selection == 1)       { fTreeVars.category = 1; selecName = "mutau"; }
  else if(selection == 2)  { fTreeVars.category = 2; selecName = "etau" ; }
  else if(selection == 5)  { fTreeVars.category = 3; selecName = "emu"  ; }
  else if(selection == 9)  { fTreeVars.category = 4; selecName = "mumu" ; }
  else if(selection == 18) { fTreeVars.category = 5; selecName = "ee"   ; }
  else                    {
    std::cout << "---Warning! Entry " << fentry
              << " has undentified selection " << selection
              << " in " << __func__ << "!\n";
    selecName = "unknown";
    fTreeVars.category = -1;
  }

  if(fReprocessMVAs && !fDYTesting) {
    for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
      if((fMVAConfig.names_[i].Contains(selecName.Data()) || //is this selection
          (selecName == "emu" && (fMVAConfig.names_[i].Contains("_e") || fMVAConfig.names_[i].Contains("_mu")))) && //or leptonic tau category
         (fIsJetBinnedMVAs[i] < 0 || fIsJetBinnedMVAs[i] == std::min((int) nJets,2))) //and either not jet binned or right number of jets
        fMvaOutputs[i] = mva[i]->EvaluateMVA(fMVAConfig.names_[i].Data());
      else
        fMvaOutputs[i] = -2.;

      if(fMvaOutputs[i] < -2.1)
        std::cout << "Error value returned for MVA " << fMVAConfig.names_[i].Data()
             << " evaluation, Entry = " << fentry << std::endl;
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
  if(selection > 0) return met;
  float corrected = met;

  // switch(abs(selection)) {
  // case 0 : weight = (1.1 - 0.2/150.*met); break; // mu+tau
  // case 1 : weight = 1.; break; // e+tau
  // case 2 : weight = 1.; break; // e+mu
  // }
  return corrected;
}

void ZTauTauHistMaker::FillEventHistogram(EventHist_t* Hist) {
  // Hist->hLumiSection         ->Fill(lumiSection        , genWeight*eventWeight)      ;
  // Hist->hTriggerStatus       ->Fill(triggerStatus      , genWeight*eventWeight)      ;
  Hist->hEventWeight         ->Fill(eventWeight             );
  Hist->hGenWeight           ->Fill(genWeight               );
  if(fDoSystematics >= 0)
    Hist->hEventWeightMVA      ->Fill(fTreeVars.eventweightMVA);
  Hist->hJetToTauWeight      ->Fill(jetToTauWeight                            );
  Hist->hJetToTauWeightGroup ->Fill(jetToTauWeightGroup, genWeight*eventWeight);
  Hist->hJetToTauWeightCorr  ->Fill((jetToTauWeight > 0.) ? jetToTauWeightCorr/jetToTauWeight : 0.);
  for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
    Hist->hJetToTauComps[ji]->Fill(fJetToTauComps[ji]);
    Hist->hJetToTauWts  [ji]->Fill(fJetToTauWts  [ji]);
  }
  if(fDoSystematics >= 0) {
    Hist->hIsSignal            ->Fill(fTreeVars.issignal      );
    Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
    Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
    Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  }
  Hist->hNPV[0]              ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hNPV[1]              ->Fill(nPV                , genWeight*eventWeight*((puWeight > 0.) ? 1./puWeight : 1.));
  Hist->hNPU[0]              ->Fill(nPU                , genWeight*eventWeight)      ;
  Hist->hNPU[1]              ->Fill(nPU                , genWeight*eventWeight*((puWeight > 0.) ? 1./puWeight : 1.));
  Hist->hLHENJets            ->Fill(LHE_Njets          , genWeight*eventWeight)      ;
  if(!fDYTesting && fDoSystematics >= 0) {
    Hist->hMcEra               ->Fill(mcEra              , genWeight*eventWeight)   ;
    Hist->hGenTauFlavorWeight  ->Fill(genTauFlavorWeight );
    // Hist->hPhotonIDWeight      ->Fill(photonIDWeight );
    Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
    Hist->hNSlimMuons          ->Fill(nSlimMuons         , genWeight*eventWeight)      ;
    // for(int icount = 0; icount < kMaxCounts; ++icount)
    //   Hist->hNMuonCounts[icount]->Fill(nMuonCounts[icount], genWeight*eventWeight)     ;
    Hist->hNSlimElectrons      ->Fill(nSlimElectrons     , genWeight*eventWeight)      ;
    // for(int icount = 0; icount < kMaxCounts; ++icount)
    //   Hist->hNElectronCounts[icount]->Fill(nElectronCounts[icount], genWeight*eventWeight);
    // Hist->hNLowPtElectrons     ->Fill(nLowPtElectrons    , genWeight*eventWeight)      ;
    Hist->hNSlimTaus           ->Fill(nSlimTaus          , genWeight*eventWeight)      ;
    // for(int icount = 0; icount < kMaxCounts; ++icount)
    //   Hist->hNTauCounts[icount]->Fill(nTauCounts[icount] , genWeight*eventWeight)      ;
    Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
    // Hist->hNSlimPhotons        ->Fill(nSlimPhotons       , genWeight*eventWeight)      ;
    // Hist->hNSlimJets           ->Fill(nSlimJets          , genWeight*eventWeight)      ;
    Hist->hNGenTausHad         ->Fill(nGenTausHad        , genWeight*eventWeight)      ;
    Hist->hNGenTausLep         ->Fill(nGenTausLep        , genWeight*eventWeight)      ;
  }
  Hist->hNGenTaus            ->Fill(nGenHardTaus       , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenHardElectrons  , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenHardMuons      , genWeight*eventWeight)      ;
  if(fDoSystematics >= 0) {
    Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
    Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
    Hist->hNJets20[1]          ->Fill(nJets20            , genWeight*eventWeight/jetPUIDWeight);
    Hist->hNJets20Rej[0]       ->Fill(nJets20Rej         , genWeight*eventWeight)      ;
    Hist->hNJets20Rej[1]       ->Fill(nJets20Rej         , genWeight*eventWeight/jetPUIDWeight);
    Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
    Hist->hNBJets              ->Fill(nBJets             , genWeight*eventWeight)      ;
    Hist->hNBJetsM             ->Fill(nBJetsM            , genWeight*eventWeight)      ;
    Hist->hNBJetsL             ->Fill(nBJetsL            , genWeight*eventWeight)      ;
    Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
    Hist->hNBJets20M[0]        ->Fill(nBJets20M          , genWeight*eventWeight)      ;
    Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;
    float nobwt = genWeight*eventWeight;
    if(btagWeight > 0.) nobwt /= btagWeight;
    Hist->hNBJets20[1]         ->Fill(nBJets20           , nobwt);
    Hist->hNBJets20M[1]        ->Fill(nBJets20M          , nobwt);
    Hist->hNBJets20L[1]        ->Fill(nBJets20L          , nobwt);
    //Store basic info for all accepted jets
    for(UInt_t jet = 0; jet < nJets20; ++jet) {
      Hist->hJetsFlavor->Fill(jetsFlavor[jet], genWeight*eventWeight);
      int flavor = abs(jetsFlavor[jet]);
      if(flavor > 20) flavor = 1; //unknown parent defaults to light jet

      int index = 0; //light jets
      if(flavor == 4)      index = 1; // c-jet
      else if(flavor == 5) index = 2; // b-jet
      float jpt = jetsPt[jet], jeta = jetsEta[jet], wt = genWeight*eventWeight;
      if(btagWeight > 0.) wt /= btagWeight; //remove previous correction
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
  }
  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hMuonTriggerStatus   ->Fill(muonTriggerStatus, genWeight*eventWeight)   ;
  Hist->hNTriggered          ->Fill(leptonOneFired+leptonTwoFired, genWeight*eventWeight)   ;
  if(fDoSystematics >= 0) {
    Hist->hPuWeight            ->Fill(puWeight)     ;
    Hist->hJetPUIDWeight       ->Fill(jetPUIDWeight);
    Hist->hPrefireWeight       ->Fill(prefireWeight);
    Hist->hTopPtWeight         ->Fill(topPtWeight)  ;
    Hist->hBTagWeight          ->Fill(btagWeight)   ;
    Hist->hZPtWeight           ->Fill(zPtWeight)    ;
    if(tauP4 && tauP4->Pt() > 0.) { //if 0 then no additional tau stored
      Hist->hTauPt           ->Fill(tauP4->Pt()        , genWeight*eventWeight)   ;
      // Hist->hTauM            ->Fill(tauP4->M()         , genWeight*eventWeight)   ;
      Hist->hTauEta          ->Fill(tauP4->Eta()       , genWeight*eventWeight)   ;
      // Hist->hTauPhi          ->Fill(tauP4->Phi()       , genWeight*eventWeight)   ;
    }
    Hist->hTauGenFlavor      ->Fill(tauGenFlavor               , genWeight*eventWeight)   ;
    Hist->hTauDecayMode[0]   ->Fill(tauDecayMode               , genWeight*eventWeight)   ;
    Hist->hTauDecayMode[1]   ->Fill(tauDecayMode);
    // Hist->hTauMVA            ->Fill(tauMVA                     , genWeight*eventWeight)   ;
    // Hist->hTauGenFlavorHad   ->Fill(tauGenFlavorHad            , genWeight*eventWeight)   ;
    Hist->hTauDeepAntiEle    ->Fill(std::log2(tauDeepAntiEle+1), genWeight*eventWeight)   ;
    Hist->hTauDeepAntiMu     ->Fill(tauDeepAntiMu              , genWeight*eventWeight)   ;
    Hist->hTauDeepAntiJet    ->Fill(std::log2(tauDeepAntiJet+1), genWeight*eventWeight)   ;

    if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
      Hist->hJetPt[0]        ->Fill(jetOneP4->Pt()             , genWeight*eventWeight)   ;
      Hist->hJetPt[1]        ->Fill(jetOneP4->Pt()             , genWeight*eventWeight/jetPUIDWeight)   ;
    }
  }

  if(!fDYTesting && fDoSystematics >= 0) {

    Hist->hHtSum             ->Fill(htSum              , genWeight*eventWeight)   ;
    Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;
    Hist->hHtPhi             ->Fill(htPhi              , genWeight*eventWeight)   ;

    if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
      // Hist->hJetM            ->Fill(jetOneP4->M()         , genWeight*eventWeight)   ;
      Hist->hJetEta          ->Fill(jetOneP4->Eta()       , genWeight*eventWeight)   ;
      // Hist->hJetPhi          ->Fill(jetOneP4->Phi()       , genWeight*eventWeight)   ;
      // Hist->hJetBTag         ->Fill(jetOneBTag            , genWeight*eventWeight)   ;
      // Hist->hJetBMVA         ->Fill(jetOneBMVA            , genWeight*eventWeight)   ;
    }

    // Hist->hPuppMet           ->Fill(puppMETC              , genWeight*eventWeight)      ;
    // Hist->hPFMet             ->Fill(pfMETC                , genWeight*eventWeight)      ;
    // Hist->hPFMetPhi          ->Fill(pfMETCphi             , genWeight*eventWeight)      ;
    // Hist->hPFCovMet00        ->Fill(pfMETCCov00           , genWeight*eventWeight)   ;
    // Hist->hPFCovMet01        ->Fill(pfMETCCov01           , genWeight*eventWeight)   ;
    // Hist->hPFCovMet11        ->Fill(pfMETCov11           , genWeight*eventWeight)   ;
    // Hist->hTrkMet              ->Fill(trkMET                , genWeight*eventWeight)      ;
    // Hist->hTrkMetPhi           ->Fill(trkMETphi             , genWeight*eventWeight)      ;
  } //end if(!fDYTesting)

  if(fDoSystematics >= 0) {
    Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
    Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
    Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
    Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
  }

  if(!fDYTesting && fDoSystematics >= 0) {
    Hist->hMetPhi            ->Fill(metPhi             , genWeight*eventWeight)      ;
    Hist->hMetCorr           ->Fill(metCorr            , genWeight*eventWeight)      ;
    Hist->hMetCorrPhi        ->Fill(metCorrPhi         , genWeight*eventWeight)      ;
    // Hist->hCovMet00          ->Fill(covMet00           , genWeight*eventWeight)   ;
    // Hist->hCovMet01          ->Fill(covMet01           , genWeight*eventWeight)   ;
    // Hist->hCovMet11          ->Fill(covMet11           , genWeight*eventWeight)   ;
  } //end if(!fDYTesting)
  if(fDoSystematics >= 0) {
    Hist->hMTOneOverM        ->Fill(fTreeVars.mtoneoverm  ,eventWeight*genWeight);
    Hist->hMTTwoOverM        ->Fill(fTreeVars.mttwooverm  ,eventWeight*genWeight);
  }

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);

  // if(!fDYTesting && fDoSystematics >= 0) {
  //   Hist->hMetVsPt             ->Fill(lepSys.Pt(), met   , genWeight*eventWeight)   ;
  //   Hist->hMetVsM              ->Fill(lepSys.M(), met    , genWeight*eventWeight)   ;
  //   Hist->hMetVsHtSum          ->Fill(htSum     , met    , genWeight*eventWeight)   ;
  //   if(htSum > 0.) //avoid divide by 0 issue
  //     Hist->hMetOverSqrtHtSum  ->Fill(met/sqrt(htSum)    , genWeight*eventWeight)   ;
  // }

  double lepDelR   = abs(leptonOneP4->DeltaR(*leptonTwoP4));
  double lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  double lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  if(fDoSystematics >= 0) {
    TLorentzVector sys    = (*photonP4) + lepSys;
    TLorentzVector svLepSys = (leptonOneSVP4 && leptonTwoSVP4) ? (*leptonOneSVP4) + (*leptonTwoSVP4) : TLorentzVector(0.,0.,0.,0.);
    TLorentzVector svSys    = (leptonOneSVP4 && leptonTwoSVP4) ? (*photonP4) + svLepSys              : TLorentzVector(0.,0.,0.,0.);

    double htDelPhi  = abs(lepSys.Phi() - htPhi);
    // double jetDelPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(lepSys.DeltaPhi(*jetOneP4)) : -1.;
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

    // Hist->hHtDeltaPhi   ->Fill(htDelPhi               ,eventWeight*genWeight);
    Hist->hMetDeltaPhi  ->Fill(metDelPhi              ,eventWeight*genWeight);
    // if(jetDelPhi >= 0.) Hist->hJetDeltaPhi  ->Fill(jetDelPhi,eventWeight*genWeight);
    Hist->hLepOneDeltaPhi->Fill(lepOneDelPhi          ,eventWeight*genWeight);
    Hist->hLepTwoDeltaPhi->Fill(lepTwoDelPhi          ,eventWeight*genWeight);

    //angles between leptons and leading jet
    // double lepOneJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonOneP4->DeltaR(*jetOneP4)            : -1.;
    // double lepOneJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonOneP4->DeltaPhi(*jetOneP4))     : -1.;
    // double lepOneJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonOneP4->Eta() - jetOneP4->Eta()) : -1.;
    // double lepTwoJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonTwoP4->DeltaR(*jetOneP4)            : -1.;
    // double lepTwoJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonTwoP4->DeltaPhi(*jetOneP4))     : -1.;
    // double lepTwoJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? abs(leptonTwoP4->Eta() - jetOneP4->Eta()) : -1.;

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
  }
  //for removing or alternate DY reweighting weights
  double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight;
  float tmp_1, tmp_2, sys_reco_weight(1.);
  double recoweight = (fIsDY) ? bareweight*fZPtWeight.GetWeight(fYear, lepSys.Pt(), lepSys.M(), /*use reco weights*/ true, tmp_1, tmp_2, sys_reco_weight) : bareweight;
  sys_reco_weight *= bareweight;
  double zpt   = (!fIsDY || zPt <  0.  ) ? lepSys.Pt() : zPt; //for DY to use given Z pT and Mass
  double zmass = (!fIsDY || zMass <  0.) ? lepSys.M()  : zMass;
  if(zpt < 0.) zpt = 0.;
  if(zmass < 0.) zmass = 0.;

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    Hist->hLepPt[1]     ->Fill(lepSys.Pt()            ,bareweight);
    Hist->hLepPt[2]     ->Fill(lepSys.Pt()            ,recoweight);
    // Hist->hLepP         ->Fill(lepSys.P()             ,eventWeight*genWeight);
    Hist->hLepM[1]      ->Fill(lepSys.M()             ,bareweight);
    Hist->hLepM[2]      ->Fill(lepSys.M()             ,recoweight);
    Hist->hLepM[3]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
    Hist->hLepM[4]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
    Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
    Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
    Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);

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
    Hist->hZMass[0]     ->Fill(zmass, eventWeight*genWeight);
    Hist->hZMass[1]     ->Fill(zmass, bareweight);
    Hist->hZMass[2]     ->Fill(zmass, recoweight);
    Hist->hZMass[3]     ->Fill(zmass, bareweight*zPtWeightSys);
    Hist->hZMass[4]     ->Fill(zmass, sys_reco_weight);

    Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,eventWeight*genWeight);
    Hist->hLepDeltaPhi[1]->Fill(lepDelPhi             ,(qcdWeight > 0.) ? eventWeight*genWeight/qcdWeight : eventWeight*genWeight);
    Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
    Hist->hLepDeltaR[0] ->Fill(lepDelR                ,eventWeight*genWeight);
    Hist->hLepDeltaR[1] ->Fill(lepDelR                ,(qcdWeight > 0.) ? eventWeight*genWeight/qcdWeight : eventWeight*genWeight);
    Hist->hLepDeltaR[2] ->Fill(lepDelR                ,eventWeight*genWeight); //same binning as scale factor measurement
    Hist->hLepDelRVsOneEta[0]->Fill(fabs(fTreeVars.leponeeta), lepDelR, eventWeight*genWeight);
    Hist->hLepDelRVsOneEta[1]->Fill(fabs(fTreeVars.leponeeta), lepDelR, (qcdWeight > 0.) ? eventWeight*genWeight/qcdWeight : eventWeight*genWeight);
    Hist->hLepDelPhiVsOneEta[0]->Fill(fabs(fTreeVars.leponeeta), lepDelPhi, eventWeight*genWeight);
    Hist->hLepDelPhiVsOneEta[1]->Fill(fabs(fTreeVars.leponeeta), lepDelPhi, (qcdWeight > 0.) ? eventWeight*genWeight/qcdWeight : eventWeight*genWeight);

    // Hist->hLepDelRVsPhi ->Fill(lepDelR , lepDelPhi    ,eventWeight*genWeight);
    Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);

    //Histograms for jet --> tau_h scale factors
    Hist->hLooseLep     ->Fill(isLooseElectron + 2*isLooseMuon + 4*isLooseTau, eventWeight*genWeight);
    if(nTaus == 1) {
      for(UInt_t itau = 0; itau < nTaus; ++itau) {
        int dm = -1; int njets = std::min(4, (int) nJets);
        bool fakeMC = fIsData == 0 && abs(tausGenFlavor[itau]) == 26;
        if(fabs(tausEta[itau]) > 2.3) continue;
        if(tausMVAAntiMu[itau] < 2) continue; //ignore ones that fail the old MVA anti-mu tight ID
        if(tausAntiEle[itau] < 10) continue; //ignore ones that have low anti-ele ID FIXME: Ensure this is done before here!
        float tau_wt = tausWeight[itau];
        if(fIsData || tau_wt <= 0.) tau_wt = 1.;
        //remove j->tau weights if set
        // if(jetToTauWeight     > 0.) tau_wt *= 1./jetToTauWeight;
        if(jetToTauWeightCorr > 0.) tau_wt *= 1./jetToTauWeightCorr;
        if(!fakeMC) {
          Hist->hTausPt->Fill(tausPt[itau], tau_wt*eventWeight*genWeight);
          Hist->hTausEta->Fill(tausEta[itau], tau_wt*eventWeight*genWeight);
          Hist->hTausDM->Fill(tausDM[itau], tau_wt*eventWeight*genWeight);
          Hist->hTausAntiJet->Fill(std::log2(tausAntiJet[itau]+1), tau_wt*eventWeight*genWeight);
          Hist->hTausAntiEle->Fill(std::log2(tausAntiEle[itau]+1), tau_wt*eventWeight*genWeight);
          Hist->hTausAntiMu ->Fill(tausAntiMu[itau]              , tau_wt*eventWeight*genWeight);
          Hist->hTausMVAAntiMu->Fill(tausMVAAntiMu[itau]         , tau_wt*eventWeight*genWeight);
          Hist->hTausGenFlavor->Fill(tausGenFlavor[itau], tau_wt*eventWeight*genWeight);
          TLorentzVector tausLV;
          tausLV.SetPtEtaPhiM(tausPt[itau], tausEta[itau], tausPhi[itau], 1.777);
          Hist->hTausDeltaR->Fill(abs(tausLV.DeltaR(*leptonOneP4)), tau_wt*eventWeight*genWeight);
        } else {
          Hist->hFakeTausPt->Fill(tausPt[itau], tau_wt*eventWeight*genWeight);
          Hist->hFakeTausEta->Fill(tausEta[itau], tau_wt*eventWeight*genWeight);
          Hist->hFakeTausDM->Fill(tausDM[itau], tau_wt*eventWeight*genWeight);
        }

        if     (tausDM[itau] == 0 ) dm = 0;
        else if(tausDM[itau] == 1 ) dm = 1;
        else if(tausDM[itau] == 10) dm = 2;
        else if(tausDM[itau] == 11) dm = 3;
        else if(tausDM[itau] < 0  ) dm = 0; //only happens in tree version with bug --> default to progress until fixed
        else continue; //non-accepted decay mode

        const int nptregions = 5; //to measure scale factor in regions of the lead muon pT as a cross-check
        bool ptregions[nptregions] = {true,
                                      fTreeVars.leponept > 55.,
                                      fTreeVars.leponept > 60.,
                                      fTreeVars.leponept < 55.,
                                      fTreeVars.leponept < 60.};
        for(int iptr = 0; iptr < nptregions; ++iptr) {
          if(!ptregions[iptr]) continue;
          if(fakeMC) {
            Hist->hFakeTauMCNJetDMPtEta[iptr][0][njets][dm]->Fill(tausPt[itau], abs(tausEta[itau]), tau_wt*eventWeight*genWeight); //all taus
          } else {
            Hist->hFakeTauNJetDMPtEta[iptr][0][njets][dm]  ->Fill(tausPt[itau], abs(tausEta[itau]), tau_wt*eventWeight*genWeight); //all taus
          }
          if(tausAntiJet[itau] > fFakeTauIsoCut) {
            if(fakeMC) {
              Hist->hFakeTauMCNJetDMPtEta[iptr][1][njets][dm]->Fill(tausPt[itau], abs(tausEta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
            } else {
              Hist->hFakeTauNJetDMPtEta[iptr][1][njets][dm]  ->Fill(tausPt[itau], abs(tausEta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
            }
          } else {
            if(fakeMC) {
              Hist->hFakeTauMCNJetDMPtEta[iptr][2][njets][dm]->Fill(tausPt[itau], abs(tausEta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
            } else {
              Hist->hFakeTauNJetDMPtEta[iptr][2][njets][dm]  ->Fill(tausPt[itau], abs(tausEta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
            }
          }
        }
      }
    } //end j-->tau scale factor section

    //Histograms for jet --> lepton scale factors
    for(UInt_t ilep = 0; ilep < nExtraLep; ++ilep) {
      double eta = fabs(leptonsEta[ilep]);
      if(leptonsTriggered[ilep] == 0) continue; //only look at triggered leptons
      if(!leptonsIsMuon[ilep] && eta > 2.5) continue; //skip electrons above 2.5 eta
      if(leptonsIsMuon[ilep] && eta > 2.4) continue; //skip muons above 2.4 eta
      if(fIsData == 0 && leptonsGenFlavor[ilep] < 6) continue; //MC light lep from jets or unknown, also skipping unmatched

      Hist->hLeptonsPt->Fill(leptonsPt[ilep], eventWeight*genWeight);
      Hist->hLeptonsEta->Fill(leptonsEta[ilep], eventWeight*genWeight);
      Hist->hLeptonsID->Fill(leptonsID[ilep], eventWeight*genWeight);
      Hist->hLeptonsIsoID->Fill(leptonsIsoID[ilep], eventWeight*genWeight);
      Hist->hLeptonsGenFlavor->Fill(leptonsGenFlavor[ilep], eventWeight*genWeight);
      Hist->hFakeLepPtEta[0]->Fill(leptonsPt[ilep], eta, eventWeight*genWeight); //all leptons
      if((leptonsIsMuon[ilep] && leptonsIsoID[ilep] > fFakeMuonIsoCut) || (!leptonsIsMuon[ilep] && leptonsID[ilep] > fFakeElectronIsoCut))
        Hist->hFakeLepPtEta[1]->Fill(leptonsPt[ilep], eta, eventWeight*genWeight); //tight leptons
      else
        Hist->hFakeLepPtEta[2]->Fill(leptonsPt[ilep], eta, eventWeight*genWeight); //loose leptons
    } //end j-->lep scale factor section
  } //end if(fDoSystematics >= 0)

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    //MVA score vs mass for MVAs not correlated with mass
    if(fMVAConfig.names_[i].Contains("emu")) {
      if(fMVAConfig.names_[i].Contains("Z0")) Hist->hLepMVsMVA[0]->Fill(fMvaOutputs[i], fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
      else                                    Hist->hLepMVsMVA[1]->Fill(fMvaOutputs[i], fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
    }
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    if (fTreeVars.train > 0) Hist->hMVATrain[i]->Fill(fMvaOutputs[i], fTreeVars.eventweight*((!fDYTesting && fFractionMVA > 0.) ? 1./fFractionMVA : 1.));
    if (fTreeVars.train < 0) Hist->hMVATest[i] ->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA);
  }
  if(fDoSystematics < 0) return;

  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);
  for(int mode = 0; mode < 3; ++mode) {
    Hist->hLepOnePrimePx[mode]->Fill(fTreeVars.leponeprimepx[mode], eventWeight*genWeight);
    Hist->hLepTwoPrimePx[mode]->Fill(fTreeVars.leptwoprimepx[mode], eventWeight*genWeight);
    Hist->hMetPrimePx   [mode]->Fill(fTreeVars.metprimepx   [mode], eventWeight*genWeight);
    Hist->hLepOnePrimePy[mode]->Fill(fTreeVars.leponeprimepy[mode], eventWeight*genWeight);
    Hist->hLepTwoPrimePy[mode]->Fill(fTreeVars.leptwoprimepy[mode], eventWeight*genWeight);
    Hist->hMetPrimePy   [mode]->Fill(fTreeVars.metprimepy   [mode], eventWeight*genWeight);
    Hist->hLepOnePrimePz[mode]->Fill(fTreeVars.leponeprimepz[mode], eventWeight*genWeight);
    Hist->hLepTwoPrimePz[mode]->Fill(fTreeVars.leptwoprimepz[mode], eventWeight*genWeight);
    Hist->hMetPrimePz   [mode]->Fill(fTreeVars.metprimepz   [mode], eventWeight*genWeight);
    Hist->hLepOnePrimeE [mode]->Fill(fTreeVars.leponeprimee [mode], eventWeight*genWeight);
    Hist->hLepTwoPrimeE [mode]->Fill(fTreeVars.leptwoprimee [mode], eventWeight*genWeight);
    Hist->hMetPrimeE    [mode]->Fill(fTreeVars.metprimee    [mode], eventWeight*genWeight);
  }

  Hist->hLepDot  ->Fill(sqrt(2.*((*leptonOneP4)*(*leptonTwoP4))), eventWeight*genWeight);
  Hist->hAlpha[0]->Fill((double) fTreeVars.alphaz1, eventWeight*genWeight);
  Hist->hAlpha[1]->Fill((double) fTreeVars.alphah1, eventWeight*genWeight);
  Hist->hAlpha[2]->Fill((double) fTreeVars.alpha2, eventWeight*genWeight);
  Hist->hAlpha[3]->Fill((double) fTreeVars.alpha3, eventWeight*genWeight);
  Hist->hDeltaAlpha[0]->Fill((double) fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill((double) fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill((double) fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill((double) fTreeVars.deltaalphah2, eventWeight*genWeight);
  Hist->hDeltaAlphaM[0]->Fill((double) fTreeVars.deltaalpham1, eventWeight*genWeight);
  Hist->hDeltaAlphaM[1]->Fill((double) fTreeVars.deltaalpham2, eventWeight*genWeight);
  Hist->hDeltaAlphaMColM[0]->Fill(fTreeVars.mestimate    - fTreeVars.deltaalpham2, eventWeight*genWeight);
  Hist->hDeltaAlphaMColM[1]->Fill(fTreeVars.mestimatetwo - fTreeVars.deltaalpham1, eventWeight*genWeight);

  if(fDYTesting) return;

  // Hist->hLepSVPt      ->Fill(svLepSys.Pt()            ,eventWeight*genWeight);
  // Hist->hLepSVM       ->Fill(svLepSys.M()             ,eventWeight*genWeight);
  // Hist->hLepSVEta     ->Fill(((svLepSys.Pt() > 1.e-5) ? svLepSys.Eta()  : -1e6),eventWeight*genWeight);
  // Hist->hLepSVPhi     ->Fill(svLepSys.Phi()           ,eventWeight*genWeight);

  // Hist->hLepSVDeltaPhi ->Fill(lepSVDelPhi              ,eventWeight*genWeight);
  // Hist->hLepSVDeltaEta ->Fill(lepSVDelEta              ,eventWeight*genWeight);
  // Hist->hLepSVDeltaM   ->Fill(lepSVDelM                ,eventWeight*genWeight);
  // Hist->hLepSVDeltaPt  ->Fill(lepSVDelPt               ,eventWeight*genWeight);
  // Hist->hLepSVPtOverM  ->Fill(svLepSys.Pt()/svLepSys.M() ,eventWeight*genWeight);


  // Hist->hSysM          ->Fill(sys.M()      ,eventWeight*genWeight);
  // Hist->hSysPt         ->Fill(sys.Pt()     ,eventWeight*genWeight);
  // Hist->hSysEta        ->Fill(sys.Eta()    ,eventWeight*genWeight);
  // Hist->hSysMvsLepM    ->Fill(lepSys.M(), sys.M(), eventWeight*genWeight);

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  TVector3 pp  = photonP4->Vect();
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  pp.SetZ(0.);
  TVector3 bisector0 = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector0.Mag() > 0.) bisector0.SetMag(1.);

  //project onto the bisectors
  //ignore photon
  double pxi_vis0 = (lp1+lp2)*bisector0;
  double pxi_inv0 = missing*bisector0;

  Hist->hPXiVis[0]     ->Fill(pxi_vis0         ,eventWeight*genWeight);
  Hist->hPXiInv[0]     ->Fill(pxi_inv0         ,eventWeight*genWeight);
  Hist->hPXiVisOverInv[0]->Fill((pxi_inv0 != 0.) ? pxi_vis0/pxi_inv0 : 1.e6 ,eventWeight*genWeight);
  Hist->hPXiInvVsVis[0]->Fill(pxi_vis0, pxi_inv0,eventWeight*genWeight);
  Hist->hPXiDiff[0]    ->Fill(pxi_vis0-pxi_inv0 ,eventWeight*genWeight);

  Hist->hPTauVisFrac    ->Fill(1.*fTreeVars.ptauvisfrac , eventWeight*genWeight);

  double coeff = 0.85 - 1.; // match 2016 clfv higgs paper (defined pxi inv = xi dot (lep1 + lep2 + met) = my pxi inv + pxi vis)
  double offset = -60.;
  Hist->hPXiDiff3[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);

  coeff = 0.6;
  offset = -40.;
  Hist->hPXiDiff2[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);

  Hist->hPtSum[0]      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+met ,eventWeight*genWeight);
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
  Hist->hOnePt[0]     ->Fill(leptonOneP4->Pt()            ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    //trigger weight variations
    float wt = (fIsData) ? 1.f : std::max(0.001f, leptonOneTrigWeight * leptonTwoTrigWeight);
    Hist->hOnePt[1]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);
    Hist->hOnePt[2]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt*triggerWeights[0]);
    Hist->hOnePt[3]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt*triggerWeights[1]);
    Hist->hOnePt[4]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt*triggerWeights[2]);
    Hist->hOnePt[5]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/leptonOneTrigWeight);
    //remove ID weights
    wt = leptonOneWeight1;
    Hist->hOnePt[6]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);
    wt = leptonOneWeight2;
    Hist->hOnePt[7]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);
    //remove Z pT weight
    wt = zPtWeight;
    Hist->hOnePt[8]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);
    //remove b-tag weight
    wt = btagWeight;
    Hist->hOnePt[9]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);
    // //remove j->tau weight
    // wt = jetToTauWeight*jetToTauWeightCorr;
    // Hist->hOnePt[10]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);
    //remove j->tau weight correction
    wt = jetToTauWeightCorr/jetToTauWeight;
    Hist->hOnePt[11]->Fill(leptonOneP4->Pt(), eventWeight*genWeight/wt);

    if(nTaus == 1 && ((fabs(leptonTwoFlavor) == 15 && tauDecayMode%10 < 2) || (tausDM[0] < 2 || tausDM[0] > 9))) {
      int dmr = (fabs(leptonTwoFlavor) == 15) ? tauDecayMode : tausDM[0];
      if(dmr > 9) dmr -= (10 - 2); //10,11 --> 2,3
      dmr += 1; //dmr = DM ID + 1, so 0 can be inclusive
      float taupt = (fabs(leptonTwoFlavor) == 15) ? leptonTwoP4->Pt() : tausPt[0];
      int ptr;
      TLorentzVector taulv;
      if(fabs(leptonTwoFlavor) == 15) taulv = *leptonTwoP4;
      else taulv.SetPtEtaPhiM(tausPt[0], tausEta[0], tausPhi[0], 1.777);

      if(taupt < 30.)      ptr = 1;
      else if(taupt < 45.) ptr = 2;
      else                 ptr = 3;
      float wt = eventWeight*genWeight;
      if(jetToTauWeightCorr > 0.) wt *= jetToTauWeight / jetToTauWeightCorr; // remove the pT correction
      double dr = abs(leptonOneP4->DeltaR(taulv));
      Hist->hJetTauOnePt[0][0]    ->Fill(leptonOneP4->Pt(), wt);
      Hist->hJetTauOnePt[dmr][0]  ->Fill(leptonOneP4->Pt(), wt);
      Hist->hJetTauOnePt[0][ptr]  ->Fill(leptonOneP4->Pt(), wt);
      Hist->hJetTauOnePt[dmr][ptr]->Fill(leptonOneP4->Pt(), wt);
      Hist->hJetTauOnePtVsR[0]    ->Fill(leptonOneP4->Pt(), dr, wt);
      Hist->hJetTauOnePtVsR[dmr]  ->Fill(leptonOneP4->Pt(), dr, wt);
      Hist->hJetTauOneR->Fill(dr, wt);
      Hist->hJetTauOneEta->Fill(fabs(leptonOneP4->Eta()), wt);
      Hist->hJetTauOneMetDeltaPhi->Fill(fTreeVars.onemetdeltaphi, wt);

      dr = abs(leptonTwoP4->DeltaR(taulv));
      Hist->hJetTauTwoPt[0][0]    ->Fill(leptonTwoP4->Pt(), wt);
      Hist->hJetTauTwoPt[dmr][0]  ->Fill(leptonTwoP4->Pt(), wt);
      Hist->hJetTauTwoPt[0][ptr]  ->Fill(leptonTwoP4->Pt(), wt);
      Hist->hJetTauTwoPt[dmr][ptr]->Fill(leptonTwoP4->Pt(), wt);
      Hist->hJetTauTwoPtVsR[0]    ->Fill(leptonTwoP4->Pt(), dr, wt);
      Hist->hJetTauTwoPtVsR[dmr]  ->Fill(leptonTwoP4->Pt(), dr, wt);
      Hist->hJetTauTwoR->Fill(dr, wt);
      Hist->hJetTauTwoEta->Fill(fabs(leptonTwoP4->Eta()), wt);
      Hist->hJetTauTwoPtVsOnePt->Fill(leptonOneP4->Pt(), taulv.Pt(), wt);
      Hist->hJetTauTwoMetDeltaPhi->Fill(fTreeVars.twometdeltaphi, wt);
    }
    Hist->hOneP         ->Fill(leptonOneP4->P()             ,eventWeight*genWeight);
    Hist->hOneM         ->Fill(leptonOneP4->M()             ,eventWeight*genWeight);
    Hist->hOnePtOverM   ->Fill(leptonOneP4->Pt() / fTreeVars.lepm, eventWeight*genWeight);
    Hist->hOneEta       ->Fill(leptonOneP4->Eta()           ,eventWeight*genWeight);
    Hist->hOnePhi       ->Fill(leptonOneP4->Phi()           ,eventWeight*genWeight);
    Hist->hOneD0        ->Fill(leptonOneD0                  ,eventWeight*genWeight);
    Hist->hOneIso       ->Fill(leptonOneIso                 ,eventWeight*genWeight);
    Hist->hOneID1       ->Fill(leptonOneID1                 ,eventWeight*genWeight);
    Hist->hOneID2       ->Fill(leptonOneID2                 ,eventWeight*genWeight);
    double relIso1 = leptonOneIso/ leptonOneP4->Pt();
    Hist->hOneRelIso    ->Fill(relIso1                      ,eventWeight*genWeight);
    Hist->hOneFlavor    ->Fill(fabs(leptonOneFlavor)        ,eventWeight*genWeight);
    Hist->hOneGenFlavor ->Fill(leptonOneGenFlavor           ,eventWeight*genWeight);
    Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  }
  Hist->hOneTrigger   ->Fill(leptonOneTrigger               ,eventWeight*genWeight);
  int offset_1(0), offset_2(0);
  if(abs(leptonOneFlavor) == 11) {
    offset_1 = SystematicGrouping::kElectronID;
    offset_2 = SystematicGrouping::kElectronRecoID;
  } else if(abs(leptonOneFlavor) == 13) {
    offset_1 = SystematicGrouping::kMuonID;
    offset_2 = SystematicGrouping::kMuonIsoID;
  }
  Hist->hOneWeight1Group->Fill(leptonOneWeight1_group - offset_1     ,eventWeight*genWeight);
  Hist->hOneWeight2Group->Fill(leptonOneWeight2_group - offset_2     ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    Hist->hOneWeight    ->Fill(leptonOneWeight1*leptonOneWeight2,eventWeight*genWeight);
    Hist->hOneTrigWeight->Fill(leptonOneTrigWeight          ,eventWeight*genWeight);

    double oneMetDelPhi  = abs(leptonOneP4->Phi() - metPhi);
    if(oneMetDelPhi > M_PI)
      oneMetDelPhi = abs(2.*M_PI - oneMetDelPhi);
    Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);
  }

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()            ,eventWeight*genWeight);
  Hist->hTwoPt[0]     ->Fill(leptonTwoP4->Pt()            ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    //trigger weight variations
    float wt = (fIsData) ? 1.f : std::max(0.001f, leptonOneTrigWeight * leptonTwoTrigWeight);
    Hist->hTwoPt[1]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);
    Hist->hTwoPt[2]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt*triggerWeights[0]);
    Hist->hTwoPt[3]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt*triggerWeights[1]);
    Hist->hTwoPt[4]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt*triggerWeights[2]);
    Hist->hTwoPt[5]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/leptonTwoTrigWeight);
    //remove ID weights
    wt = leptonTwoWeight1;
    Hist->hTwoPt[6]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);
    wt = leptonTwoWeight2;
    Hist->hTwoPt[7]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);
    //remove Z pT weight
    wt = zPtWeight;
    Hist->hTwoPt[8]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);
    //remove b-tag weight
    wt = btagWeight;
    Hist->hTwoPt[9]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);
    //remove j->tau weight
    wt = jetToTauWeightCorr;
    Hist->hTwoPt[10]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);
    //remove j->tau weight correction
    wt = jetToTauWeightCorr/jetToTauWeight;
    Hist->hTwoPt[11]->Fill(leptonTwoP4->Pt(), eventWeight*genWeight/wt);

    Hist->hTwoP         ->Fill(leptonTwoP4->P()             ,eventWeight*genWeight);
    Hist->hTwoM         ->Fill(leptonTwoP4->M()             ,eventWeight*genWeight);
    Hist->hTwoPtOverM   ->Fill(leptonTwoP4->Pt() / fTreeVars.lepm, eventWeight*genWeight);
    Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()           ,eventWeight*genWeight);
    Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()           ,eventWeight*genWeight);
    Hist->hTwoD0        ->Fill(leptonTwoD0                  ,eventWeight*genWeight);
    Hist->hTwoIso       ->Fill(leptonTwoIso                 ,eventWeight*genWeight);
    Hist->hTwoID1       ->Fill(leptonTwoID1                 ,eventWeight*genWeight);
    Hist->hTwoID2       ->Fill(leptonTwoID2                 ,eventWeight*genWeight);
    Hist->hTwoID3       ->Fill(leptonTwoID3                 ,eventWeight*genWeight);
    double relIso2 = leptonTwoIso/ leptonTwoP4->Pt();
    Hist->hTwoRelIso    ->Fill(relIso2                      ,eventWeight*genWeight);
    Hist->hTwoFlavor    ->Fill(fabs(leptonTwoFlavor)        ,eventWeight*genWeight);
    Hist->hTwoGenFlavor ->Fill(leptonTwoGenFlavor           ,eventWeight*genWeight);
    Hist->hTwoQ         ->Fill(leptonTwoFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  }
  Hist->hTwoTrigger   ->Fill(leptonTwoTrigger               ,eventWeight*genWeight);
  offset_1 = 0; offset_2 = 0;
  if(abs(leptonTwoFlavor) == 11) {
    offset_1 = SystematicGrouping::kElectronID;
    offset_2 = SystematicGrouping::kElectronRecoID;
  } else if(abs(leptonTwoFlavor) == 13) {
    offset_1 = SystematicGrouping::kMuonID;
    offset_2 = SystematicGrouping::kMuonIsoID;
  }
  Hist->hTwoWeight1Group->Fill(leptonTwoWeight1_group - offset_1     ,eventWeight*genWeight);
  Hist->hTwoWeight2Group->Fill(leptonTwoWeight2_group - offset_2     ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    Hist->hTwoWeight    ->Fill(leptonTwoWeight1*leptonTwoWeight2,eventWeight*genWeight);
    Hist->hTwoTrigWeight->Fill(leptonTwoTrigWeight          ,eventWeight*genWeight);

    double twoMetDelPhi  = abs(leptonTwoP4->Phi() - metPhi);
    if(twoMetDelPhi > M_PI)
      twoMetDelPhi = abs(2.*M_PI - twoMetDelPhi);
    Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

    Hist->hPtDiff      ->Fill(leptonOneP4->Pt()-leptonTwoP4->Pt() ,eventWeight*genWeight);
    Hist->hD0Diff      ->Fill(leptonTwoD0-leptonOneD0             ,eventWeight*genWeight);
    Hist->hTwoPtVsOnePt->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(),eventWeight*genWeight);
  }
}

void ZTauTauHistMaker::FillSystematicHistogram(SystematicHist_t* Hist) {
  bool isSameFlavor = fFolderName == "ee" || fFolderName == "mumu";
  bool isMuTau = fFolderName == "mutau";
  bool isETau  = fFolderName == "etau";
  for(int sys = 0; sys < kMaxSystematics; ++sys) {
    float weight = eventWeight*genWeight;
    if(weight == 0.) continue; //no way to re-scale 0, contributes nothing to histograms so can just skip
    TLorentzVector lv1 = *leptonOneP4;
    TLorentzVector lv2 = *leptonTwoP4;
    if(sys == 0) weight = weight;                                          //do nothing
    else if  (sys ==  1) {                                                 //electron ID scale factors
      if(abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  2) {
      if(abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  3) {
      if(abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_sys / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
    } else if(sys ==  4) {                                                 //muon ID scale factors
      if(abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  5) {
      if(abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  6) {
      if(abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_sys  / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_sys  / leptonTwoWeight1;
    } else if(sys ==  7) {                                                 //tau ID scale factors
      if(abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  8) {
      if(abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  9) {
      if(abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_sys  / leptonOneWeight1;
      if(abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_sys  / leptonTwoWeight1;
    } else if(sys == 10) weight *= jetToTauWeightUp     / jetToTauWeight ; //Jet --> tau weights
    else if  (sys == 11) weight *= jetToTauWeightDown   / jetToTauWeight ;
    else if  (sys == 12) weight *= jetToTauWeightSys    / jetToTauWeight ;
    else if  (sys == 13) weight *= zPtWeightUp          / zPtWeight      ; //Drell-Yan pT weights
    else if  (sys == 14) weight *= zPtWeightDown        / zPtWeight      ;
    else if  (sys == 15) weight *= zPtWeightSys         / zPtWeight      ;
    else if  (sys == 16) {                                                 //electron reco ID scale factors
      if(abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_up / leptonOneWeight2;
      if(abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_up / leptonTwoWeight2;
    } else if(sys == 17) {
      if(abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_down / leptonOneWeight2;
      if(abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    } else if(sys == 18) {
      if(abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_sys / leptonOneWeight2;
      if(abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_sys / leptonTwoWeight2;
    } else if(sys == 19) {                                                 //muon iso ID scale factors
      if(abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_up / leptonOneWeight2;
      if(abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_up / leptonTwoWeight2;
    } else if(sys == 20) {
      if(abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_down / leptonOneWeight2;
      if(abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    } else if(sys == 21) {
      if(abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_sys  / leptonOneWeight2;
      if(abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_sys  / leptonTwoWeight2;
    }
    //FIXME: Need to re-evaluate variables using the updated tau four momentum
    else if  (sys == 22) {                                                 //Tau ES
      if(abs(leptonTwoFlavor) == 15 && tauES > 0. && tauESUp > 0.) {
        lv2 *= (tauESUp / tauES);
      }
    } else if(sys == 23) {
      if(abs(leptonTwoFlavor) == 15 && tauES > 0. && tauESDown > 0.) {
        lv2 *= (tauESDown / tauES);
      }
    }
    // else if(sys == 24) {
    //   if(abs(leptonTwoFlavor) == 15 && tauES > 0. && tauESDown > 0.) {
    //     lv2 *= (tauESSys  / tauES);
    //   }
    // }                                                                   //End tau ES
    else if  (sys == 25) weight *= (qcdWeight > 0.) ? qcdWeightUp  / qcdWeight : 0.; //SS --> OS weights
    else if  (sys == 26) weight *= (qcdWeight > 0.) ? qcdWeightDown/ qcdWeight : 0.;
    else if  (sys == 27) weight *= (qcdWeight > 0.) ? qcdWeightSys / qcdWeight : 0.;
    else if  (sys == 28) weight *= jetToTauWeightCorrUp     / jetToTauWeightCorr   ; //Jet --> tau weight pT corrections
    else if  (sys == 29) weight *= jetToTauWeightCorrDown   / jetToTauWeightCorr   ;
    else if  (sys == 30) weight *= jetToTauWeightCorrSys    / jetToTauWeightCorr   ;
    else if  (sys == 31) weight *= jetToTauWeight_compUp    / jetToTauWeightCorr   ; //Jet --> tau weight composition
    else if  (sys == 32) weight *= jetToTauWeight_compDown  / jetToTauWeightCorr   ;
    else if  (sys == 33) weight *= jetToTauWeight_compUp    / jetToTauWeightCorr   ;
    else if(sys > 33 && sys < 43) {                                                // tau IDs by PDG ID
      if(abs(leptonTwoFlavor) == 15) {
        if(sys == 34 && abs(tauGenFlavor) == 15)       //tau anti-jet ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 35 && abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 36 && abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
        else if(sys == 37 && abs(tauGenFlavor) == 13)  //tau anti-mu ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 38 && abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 39 && abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
        else if(sys == 40 && abs(tauGenFlavor) == 11)  //tau anti-ele ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 41 && abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 42 && abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
      }
    } else if  (sys == 43) { weight *= (fIsData) ? 1. : 1. + 0.016; //luminosity uncertainty
    } else if  (sys == 44) { weight *= (fIsData) ? 1. : 1. - 0.016;
    } else if  (sys == 45) { weight *= (btagWeight > 0.) ? btagWeightUp   / btagWeight : 1.; //btag uncertainty
    } else if  (sys == 46) { weight *= (btagWeight > 0.) ? btagWeightDown / btagWeight : 1.;

    // } else if  (sys >= 50 && sys < 56) { //jet->tau statistical, separated by years
    //   int base = 50;
    //   if(abs(leptonTwoFlavor) == 15) {
    //     int bin = 2*(fYear - 2016); //uncorrelated between years
    //     if((sys == base + bin) && abs(tauGenFlavor) == 15)
    //       weight *= jetToTauWeightUp   / jetToTauWeight;
    //     else if((sys == base + 1 + bin) && abs(tauGenFlavor) == 15)
    //       weight *= jetToTauWeightDown / jetToTauWeight;
    //   }
      //Removed j->tau groupings since now using fitter errors uncorrelated by years
    } else if  (sys >= SystematicGrouping::kJetToTau && sys < 100) { //Jet --> tau systematic groupings, (4 DM)*(2 eta)*(3 years)*(2 up/down) = 48 histograms
      if(fFolderName == "mumu" || fFolderName == "ee") continue;
      int offset = SystematicGrouping::kJetToTau;
      if(2*(jetToTauWeightGroup) - offset     == sys) weight *= jetToTauWeightUp   / jetToTauWeight;
      if(2*(jetToTauWeightGroup) - offset + 1 == sys) weight *= jetToTauWeightDown / jetToTauWeight;
      //Removed muon ID groupings since it appears to be small enough to treat correlated
    // } else if  (sys >= SystematicGrouping::kMuonID && sys < 150) { //Muon ID
    //   int offset = SystematicGrouping::kMuonID;
    //   if(abs(leptonOneFlavor) == 13) {
    //     if(2*(leptonOneWeight1_group) - offset     == sys) weight *= leptonOneWeight1_up   / leptonOneWeight1;
    //     if(2*(leptonOneWeight1_group) - offset + 1 == sys) weight *= leptonOneWeight1_down / leptonOneWeight1;
    //   }
    //   if(abs(leptonTwoFlavor) == 13) {
    //     if(2*(leptonTwoWeight1_group) - offset     == sys) weight *= leptonTwoWeight1_up   / leptonTwoWeight1;
    //     if(2*(leptonTwoWeight1_group) - offset + 1 == sys) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    //   }
      //Removed muon iso ID groupings since it appears to be small enough to treat correlated
    // } else if  (sys >= SystematicGrouping::kMuonIsoID && sys < 200) { //Muon ID
    //   int offset = SystematicGrouping::kMuonIsoID;
    //   if(abs(leptonOneFlavor) == 13) {
    //     if(2*(leptonOneWeight2_group) - offset     == sys) weight *= leptonOneWeight2_up   / leptonOneWeight2;
    //     if(2*(leptonOneWeight2_group) - offset + 1 == sys) weight *= leptonOneWeight2_down / leptonOneWeight2;
    //   }
    //   if(abs(leptonTwoFlavor) == 13) {
    //     if(2*(leptonTwoWeight2_group) - offset     == sys) weight *= leptonTwoWeight2_up   / leptonTwoWeight2;
    //     if(2*(leptonTwoWeight2_group) - offset + 1 == sys) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    //   }
    } else if  (sys >= 100 && sys < 106) { //tau anti-jet ID, separated by years
      if(fFolderName == "mumu" || fFolderName == "ee") continue;
      int base = 100;
      if(abs(leptonTwoFlavor) == 15) {
        int bin = 2*(fYear - 2016); //uncorrelated between years
        if((sys == base + bin) && abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + bin) && abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    } else if  (sys >= 110 && sys < 140) { //tau anti-mu ID, separated by 5 bins per year --> 15 histograms per up/down
      if(fFolderName == "mumu" || fFolderName == "ee") continue;
      int base = 110;
      if(abs(leptonTwoFlavor) == 15) {
        int bin = 5*(fYear - 2016) + leptonTwoWeight1_bin; //uncorrelated between years and bins
        if((sys == base + 2*bin) && abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + 2*bin) && abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    } else if  (sys >= 140 && sys < 158) { //tau anti-ele ID, separated by 3 bins per year --> 9 histograms per up/down
      if(fFolderName == "mumu" || fFolderName == "ee") continue;
      int base = 140;
      if(abs(leptonTwoFlavor) == 15) {
        int bin = 3*(fYear - 2016) + leptonTwoWeight1_bin; //uncorrelated between years and bins
        if((sys == base + 2*bin) && abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + 2*bin) && abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
      //Remove electron ID and Reco ID groupings, as generally systematically correlated errors
    // } else if  (sys >= SystematicGrouping::kElectronID && sys < 250) { //Electron ID
    //   int offset = SystematicGrouping::kElectronID;
    //   if(abs(leptonOneFlavor) == 11) {
    //     if(2*(leptonOneWeight1_group) - offset     == sys) weight *= leptonOneWeight1_up   / leptonOneWeight1;
    //     if(2*(leptonOneWeight1_group) - offset + 1 == sys) weight *= leptonOneWeight1_down / leptonOneWeight1;
    //   }
    //   if(abs(leptonTwoFlavor) == 11) {
    //     if(2*(leptonTwoWeight1_group) - offset     == sys) weight *= leptonTwoWeight1_up   / leptonTwoWeight1;
    //     if(2*(leptonTwoWeight1_group) - offset + 1 == sys) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    //   }
    // } else if  (sys >= SystematicGrouping::kElectronRecoID && sys < 300) { //Electron Reco ID
    //   int offset = SystematicGrouping::kElectronRecoID;
    //   if(abs(leptonOneFlavor) == 11) {
    //     if(2*(leptonOneWeight2_group) - offset     == sys) weight *= leptonOneWeight2_up   / leptonOneWeight2;
    //     if(2*(leptonOneWeight2_group) - offset + 1 == sys) weight *= leptonOneWeight2_down / leptonOneWeight2;
    //   }
    //   if(abs(leptonTwoFlavor) == 11) {
    //     if(2*(leptonTwoWeight2_group) - offset     == sys) weight *= leptonTwoWeight2_up   / leptonTwoWeight2;
    //     if(2*(leptonTwoWeight2_group) - offset + 1 == sys) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    //   }
    } else continue; //no need to fill undefined systematics

    if(std::isnan(weight)) {
      std::cout << "ZTauTauHistMaker::" << __func__ << ": Entry " << fentry << " Systematic " << sys << " weight is NaN! event weight = "
                << eventWeight << ", setting to 0...\n";
      weight = 0.;
    }

    TLorentzVector lepSys = lv1 + lv2;

    Hist->hLepM  [sys]->Fill(lepSys.M()  , weight);
    //skip all other histograms in same-flavor selection, only using the M_{ll} histogram
    if(isSameFlavor) continue;
    Hist->hOnePt [sys]->Fill(lv1.Pt()    , weight);
    Hist->hTwoPt [sys]->Fill(lv2.Pt()    , weight);
    if(!fIsData)
      Hist->hWeightChange[sys]->Fill((eventWeight*genWeight != 0.) ? (eventWeight*genWeight - weight) / (eventWeight*genWeight) : 0.);
    //MVA outputs
    float mvaweight = fTreeVars.eventweightMVA*(weight/(eventWeight*genWeight));
    if(std::isnan(mvaweight)) {
      std::cout << "ZTauTauHistMaker::" << __func__ << ": Entry " << fentry << " MVA weight is NaN! eventweightMVA = "
                << fTreeVars.eventweightMVA << ", setting to 0...\n";
      mvaweight = 0.;
    }
    for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
      int category = fMVAConfig.data_cat_[i];
      //Only fill relevant MVA histograms for the datset
      if(!((category == 1 && isMuTau) ||
           (category == 2 && isETau) ||
           (category == 0 && !(isMuTau || isETau)))
         ) continue;
      float mvascore = fMvaOutputs[i];
      if(std::isnan(mvascore) && fVerbose > 0) {
        std::cout << "ZTauTauHistMaker::" << __func__ << ": Entry " << fentry << " MVA " << i << " score is NaN! Setting to -2...\n";
        mvascore = -2.;
      }
      Hist->hMVA[i][sys]->Fill(mvascore, mvaweight);
    }
  }
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
      std::cout << "Warning! Unable to identify type of DY event!" << std::endl
           << "nGenHardTaus = " << nGenHardTaus << std::endl
           << "nGenHardMuons = " << nGenHardMuons << std::endl
           << "nGenHardElectrons = " << nGenHardElectrons << std::endl
           << "fDYType = " << fDYType << std::endl
           << "Entry " << fentry << std::endl;
      return kTRUE;
    }
  }
  //split W+Jets into N(jets) generated for binned sample combination
  if(fWNJets > -1 && LHE_Njets != fWNJets) {
    return kTRUE;
  }

  ////////////////////////
  // Data overlap check //
  ////////////////////////

  //skip if it's data and lepton status doesn't match data set ( 1 = electron 2 = muon) unless allowing overlap and it passes both
  if(fIsData > 0) {
    int pdgid = (fIsData == 1) ? 11 : 13; //pdg ID for the data stream
    //if no selected lepton fired this trigger, continue
    if(!((abs(leptonOneFlavor) == pdgid && leptonOneFired) || (abs(leptonTwoFlavor) == pdgid && leptonTwoFired)))
      return kTRUE;

    if(triggerLeptonStatus != ((UInt_t) fIsData)) { //trigger doesn't match data stream
      if(triggerLeptonStatus != 3) return kTRUE; //triggered only for the other stream
      if(fSkipDoubleTrigger) { //don't allow double triggers
        int other_pdgid = (fIsData == 1) ? 13 : 11; //pdg ID for the other data stream
        //only skip if the selected lepton actually fired the trigger
        if((abs(leptonOneFlavor) == other_pdgid && leptonOneFired) ||(abs(leptonTwoFlavor) == other_pdgid && leptonTwoFired)) return kTRUE;
      }
    }
  }

  //Print debug info
  if(fVerbose > 0 ) {
    std::cout << " lep_1: pdg_id = " << leptonOneFlavor << " p4 = "; leptonOneP4->Print();
    std::cout << " lep_2: pdg_id = " << leptonTwoFlavor << " p4 = "; leptonTwoP4->Print();
  }

  /////////////////
  // Correct MET //
  /////////////////

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
  if(fVerbose > 0 ) std::cout << " MET = " << met << std::endl;

  /////////////////////////////////
  // Remove weights if requested //
  /////////////////////////////////

  bool trigError = nTrigModes > 0 && !fIsData && abs(triggerWeights[0] - leptonOneTrigWeight*leptonTwoTrigWeight) > 0.001;
  if(trigError) std::cout << "!!! Warning! Entry " << fentry;
  if(trigError || fVerbose > 0) std::cout << " TriggerWeights[0] = " << triggerWeights[0]
                                          << " vs leptonOneTrigWeight*leptonTwoTrigWeight = "
                                          << leptonOneTrigWeight*leptonTwoTrigWeight << std::endl;
  //photon ID weights
  if(fRemovePhotonIDWeights > 0) {
    if(photonIDWeight > 0.) eventWeight /= photonIDWeight;
  }

  //b-tag weights
  if(fRemoveBTagWeights > 0 && btagWeight > 0. && !fIsData) {
    eventWeight /= btagWeight;
    if(fVerbose > 0) std::cout << " B-tag weight originally: " << btagWeight << std::endl;
    if(fRemoveBTagWeights > 1) { //replace the weights
      int wp = BTagWeight::kLooseBTag;
      if(fFolderName == "mutau" || fFolderName == "etau") wp = BTagWeight::kTightBTag;
      btagWeight = fBTagWeight.GetWeight(wp, fYear, nJets20, jetsPt, jetsEta, jetsFlavor, jetsBTag, btagWeightUp, btagWeightDown);
      eventWeight *= btagWeight;
      if((btagWeight - btagWeightUp) * (btagWeight - btagWeightDown) > 1.e-3 && fVerbose > 0) {
        std::cout << "!!! Warning: Entry " << fentry << " B-tag weight up/down are on the same side of the weight: Wt = "
                  << btagWeight << " Up = " << btagWeightUp << " Down = " << btagWeightDown << std::endl;
      }
    } else {
      btagWeight = 1.;
      btagWeightUp = 1.;
      btagWeightDown = 1.;
    }
    if(fVerbose > 0) std::cout << " B-tag weight updated: " << btagWeight << std::endl;
  }

  //pileup weights
  if(fRemovePUWeights > 0 && puWeight > 0. && !fIsData) {
    eventWeight /= puWeight;
    //replace weight
    if(fRemovePUWeights > 1) {
      puWeight = fPUWeight.GetWeight(nPU, fYear);
      eventWeight *= puWeight;
    } else
      puWeight = 1.;
  }

  //jet PU ID weight
  jetPUIDWeight = 1.;
  if(fUseJetPUIDWeights > 0 && !fIsData) {
    jetPUIDWeight = fJetPUWeight.GetWeight(fYear, nJets20, jetsPt, jetsEta, nJets20Rej, jetsRejPt, jetsRejEta);
  }
  eventWeight *= jetPUIDWeight;

  //pre-fire weight
  prefireWeight = 1.;
  if(fUsePrefireWeights > 0 && !fIsData) {
    prefireWeight = fPrefireWeight.GetWeight(fYear, nJets20, jetsPt, jetsEta);
  }
  eventWeight *= jetPUIDWeight;

  // DY z pT weights
  if(!fIsDY) {zPtWeight = 1.; zPtWeightUp = 1.; zPtWeightDown = 1.; zPtWeightSys = 1.;}
  if(fIsDY && fRemoveZPtWeights > 0 && zPtWeight > 0.) {
    if(fVerbose > 0) std::cout << " Removing zPtWeight = " << zPtWeight << " from eventWeight = " << eventWeight;
    eventWeight /= zPtWeight;
    zPtWeight = 1.;
    if(fVerbose > 0) std::cout << " --> " << eventWeight << std::endl;
  }
  //only re-weight the DY sample(s)
  if(fRemoveZPtWeights > 1 && fIsDY) {
    double zpt   = (!fIsDY || zPt < 0.  ) ? (*leptonOneP4+*leptonTwoP4).Pt() : zPt;
    double zmass = (!fIsDY || zMass < 0.) ? (*leptonOneP4+*leptonTwoP4).M()  : zMass;
    zpt = zpt < 0. ? 0. : zpt;
    zmass = zmass < 0. ? 0. : zmass;
    zPtWeight = fZPtWeight.GetWeight(fYear, zpt, zmass, false /*Use Gen level weights*/, zPtWeightUp, zPtWeightDown, zPtWeightSys);
    eventWeight *= zPtWeight;
    if(fVerbose > 0) std::cout << " For Z pT = " << zpt << " and Mass = " << zmass << " using Data/MC weight " << zPtWeight
                               << "--> event weight = " << eventWeight << std::endl;
  }


  //No weights in data
  if(fIsData) {
    eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.;
    genTauFlavorWeight = 1.; jetPUIDWeight = 1.; btagWeight = 1.;
    for(int itrig = 0; itrig < nTrigModes; ++itrig) triggerWeights[itrig] = 1.;
  }


  //Initialize systematic variation weights
  InitializeSystematics();


  //selections
  //use the tree name to choose the selection
  bool mutau = (fFolderName == "mutau") && nTaus == 1  && nMuons == 1;
  bool etau  = (fFolderName == "etau")  && nTaus == 1  && nElectrons == 1;
  bool emu   = (fFolderName == "emu")   && nMuons == 1 && nElectrons == 1;
  bool mumu  = fFolderName == "mumu" && nMuons == 2;
  bool ee    = fFolderName == "ee" && nElectrons == 2;
  //reject overlaps
  if(mutau && etau) {mutau = false; etau = false;}
  if(emu && (mutau || etau)) {mutau = false; etau = false;}
  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl
                             << " eventWeight*genWeight = " << eventWeight*genWeight << std::endl;

  if(!(mutau || etau || emu || mumu || ee)) {
    std::cout << "WARNING! Entry " << entry << " passes no selections!\n";
    return kTRUE;
  }

  if((mutau + etau + emu + mumu + ee) > 1)
    std::cout << "WARNING! Entry " << entry << " passes multiple selections!\n";

  //trigger and object pT thresholds
  float muon_trig_pt(25.), electron_trig_pt(29.), muon_pt(10.), electron_pt(15.), tau_pt(20.);
  if(fYear == 2017) muon_trig_pt = 28.;
  if(fYear != 2016) electron_trig_pt = 34.;

  //trigger weights
  if(fRemoveTriggerWeights > 0 && !fIsData) {
    if(leptonOneTrigWeight > 0.) eventWeight /= leptonOneTrigWeight;
    if(leptonTwoTrigWeight > 0.) eventWeight /= leptonTwoTrigWeight;
    leptonOneTrigWeight = 1.; leptonTwoTrigWeight = 1.;
    if(fRemoveTriggerWeights > 1) {
      float data_eff[2] = {0.5f, 0.5f}; //set to 0.5 so no danger in doing the ratio of eff or 1 - eff
      float mc_eff[2]   = {0.5f, 0.5f};
      if(ee) {
        fElectronIDWeight.TriggerEff(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, data_eff[0], mc_eff[0]);
        fElectronIDWeight.TriggerEff(leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, data_eff[1], mc_eff[1]);
      }
      if(emu) {
        fElectronIDWeight.TriggerEff(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, data_eff[0], mc_eff[0]);
        //use low trigger in case the lepton didn't fire a trigger
        fMuonIDWeight.TriggerEff(leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, !leptonTwoFired || muonTriggerStatus != 2, data_eff[1], mc_eff[1]);
      }
      if(etau) {
        fElectronIDWeight.TriggerEff(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, data_eff[0], mc_eff[0]);
        data_eff[1] = 0.f; mc_eff[1] = 0.f; //tau can't trigger
      }
      if(mumu) {
        //use low trigger in case the lepton didn't fire a trigger
        fMuonIDWeight.TriggerEff(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, !leptonOneFired || muonTriggerStatus != 2, data_eff[0], mc_eff[0]);
        fMuonIDWeight.TriggerEff(leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, !leptonTwoFired || muonTriggerStatus != 2, data_eff[1], mc_eff[1]);
      }
      if(mutau) {
        fMuonIDWeight.TriggerEff(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, muonTriggerStatus != 2, data_eff[0], mc_eff[0]);
        data_eff[1] = 0.f; mc_eff[1] = 0.f; //tau can't trigger
      }
      //use just the muon if there's a muon or the leading firing lepton otherwise
      if(fRemoveTriggerWeights == 2) {
        if(leptonOneFired && (abs(leptonOneFlavor) == 13 || !leptonTwoFired || abs(leptonTwoFlavor) != 13)) {
          leptonOneTrigWeight = data_eff[0] / mc_eff[0];
        } else {
          leptonTwoTrigWeight = data_eff[1] / mc_eff[1];
        }
      }
      //use the probability of each lepton triggering or not triggering
      else if(fRemoveTriggerWeights == 3) {
        //if unable to fire the trigger, mc/data efficiency set equal at 0.5, so division is safe
        leptonOneTrigWeight = (leptonOneFired) ? data_eff[0] / mc_eff[0] : (1.-data_eff[0])/(1.-mc_eff[0]);
        leptonTwoTrigWeight = (leptonTwoFired) ? data_eff[1] / mc_eff[1] : (1.-data_eff[1])/(1.-mc_eff[1]);
        if(leptonTwoP4->Pt() < ((abs(leptonTwoFlavor) == 13) ? muon_trig_pt : electron_trig_pt))
          leptonTwoTrigWeight = 1.; //couldn't fire, so ensure no weight effect
      }
      //use the probability that at least one fires
      else if(fRemoveTriggerWeights == 4) {
        float prob_data(1.f), prob_mc(1.f);
        float pt_1(leptonOneP4->Pt()), pt_2(leptonTwoP4->Pt());
        //check if each is triggerable
        if(fabs(leptonOneFlavor) == 13 && pt_1 > muon_trig_pt) {
          prob_data *= (1.f - data_eff[0]);
          prob_mc   *= (1.f - mc_eff[0]  );
        }
        if(fabs(leptonTwoFlavor) == 13 && pt_2 > muon_trig_pt) {
          prob_data *= (1.f - data_eff[1]);
          prob_mc   *= (1.f - mc_eff[1]  );
        }
        if(fabs(leptonOneFlavor) == 11 && pt_1 > electron_trig_pt) {
          prob_data *= (1.f - data_eff[0]);
          prob_mc   *= (1.f - mc_eff[0]  );
        }
        if(fabs(leptonTwoFlavor) == 11 && pt_2 > electron_trig_pt) {
          prob_data *= (1.f - data_eff[1]);
          prob_mc   *= (1.f - mc_eff[1]  );
        }
        if(prob_data >= 1.f || prob_mc >= 1.f) {
          if(fVerbose > 0) { //often due to changed trigger thresholds, so only warn if debugging
            std::cout << "!!! " << fentry << " Warning! Inefficiency calculation >= 1!"
                      << " 1-eff(data) = " << prob_data
                      << " 1-eff(MC) = " << prob_mc
                      << std::endl;
          }
        }
        prob_data = std::min(0.9999f, prob_data);
        prob_mc   = std::min(0.9999f, prob_mc  );
        leptonOneTrigWeight = sqrt((1.f - prob_data) / (1.f - prob_mc));
        leptonTwoTrigWeight = sqrt((1.f - prob_data) / (1.f - prob_mc));
      }

      if(leptonOneTrigWeight < 0. || leptonTwoTrigWeight < 0. || std::isnan(leptonOneTrigWeight) || std::isnan(leptonTwoTrigWeight)) {
        std::cout << "!!! " << fentry << " Warning! Trigger efficiency calculation failed: wt_1 = "
                  << leptonOneTrigWeight << " wt_2 = " << leptonTwoTrigWeight
                  << " P(l1) = (" << data_eff[0] << ", " << mc_eff[0] << ")"
                  << " P(l2) = (" << data_eff[1] << ", " << mc_eff[1] << ")"
                  << " fired(l1) = " << leptonOneFired
                  << " fired(l2) = " << leptonTwoFired
                  << std::endl;
        leptonOneTrigWeight = 1.; leptonTwoTrigWeight = 1.;
      }
      //keep this the same for now, to see the comparison plot using this trigger weight array
      // triggerWeights[0] = leptonOneTrigWeight * leptonTwoTrigWeight;
      eventWeight *= leptonOneTrigWeight * leptonTwoTrigWeight;
    }
  }


  //use locally computed weight
  if(fUseTauFakeSF > 1 && fIsData == 0) genTauFlavorWeight = GetTauFakeSF(tauGenFlavor);

  //apply fake tau SF
  if(!fDYTesting && fUseTauFakeSF && fIsData == 0) eventWeight *= genTauFlavorWeight;

  //No weights in data
  if(fIsData) { eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.; genTauFlavorWeight = 1.; jetPUIDWeight = 1.; btagWeight = 1.; }


  InitializeTreeVariables(mutau+2*etau+5*emu+9*mumu+18*ee);

  if(std::isnan(eventWeight)) {
    std::cout << "!!! " << fentry << " Warning! Event weight is NaN: wt = "
              << eventWeight << " gen_wt = " << genWeight << std::endl;
    eventWeight = 1.;
  }

  bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  if(bisector.Mag() > 0.) bisector.SetMag(1.);

  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl;

  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////
  // Check if anti-iso lepton category
  //////////////////////////////////////////////////////////////

  isLooseElectron = (ee || emu || etau) && looseQCDSelection;
  isLooseMuon     = (mumu || emu || mutau) && looseQCDSelection;
  isLooseTau      = (etau || mutau) && tauDeepAntiJet < 50 && looseQCDSelection;
  //Ignore loose lepton selection in same flavor category for now
  ee   &= !isLooseElectron;
  mumu &= !isLooseMuon;

  if(isLooseElectron) {
    if(emu || etau) isLooseElectron &= leptonOneID1 < 4; //Not WP80
    if(ee)          isLooseElectron &= leptonOneID1 < 4 || leptonTwoID1 < 4; //Not WP80
  }
  if(isLooseMuon) {
    if(emu)   isLooseMuon &= leptonTwoID1 < 4; //Not Tight IsoID
    if(mutau) isLooseMuon &= leptonOneID1 < 4; //Not Tight IsoID
    if(mumu)  isLooseMuon &= leptonOneID1 < 4 || leptonTwoID1 < 4; //Not Tight IsoID
  }

  isFakeElectron  = !fIsData && ((fabs(leptonOneFlavor) == 11 && leptonOneGenFlavor == 26) ||
                                 (fabs(leptonTwoFlavor) == 11 && leptonTwoGenFlavor == 26));
  isFakeMuon      = !fIsData && ((fabs(leptonOneFlavor) == 13 && leptonOneGenFlavor == 26) ||
                                 (fabs(leptonTwoFlavor) == 13 && leptonTwoGenFlavor == 26));

  jetToTauWeight = 1.; jetToTauWeightUp = 1.; jetToTauWeightDown = 1.; jetToTauWeightSys = 1.; jetToTauWeightGroup = 0;
  jetToTauWeightCorr = 1.; jetToTauWeightCorrUp = 1.; jetToTauWeightCorrDown = 1.; jetToTauWeightCorrSys = 1.;
  jetToTauWeight_compUp = 1.; jetToTauWeight_compDown = 1.;


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
  if(!fDYTesting || fCutFlowTesting) {
    FillAllHistograms(set_offset + 1);
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


  mutau = mutau && muon->Pt() > muon_trig_pt && tau->Pt() > tau_pt;
  etau  = etau  && electron->Pt() > electron_trig_pt && tau->Pt() > tau_pt;
  emu   = emu   && ((electron->Pt() > electron_trig_pt && muon->Pt() > muon_pt && leptonOneFired) ||
                    (electron->Pt() > electron_pt && muon->Pt() > muon_trig_pt && leptonTwoFired));
  mumu  = mumu  && ((muon->Pt() > muon_trig_pt && muon_2->Pt() > muon_pt && leptonOneFired) ||
                    (muon->Pt() > muon_pt  && muon_2->Pt() > muon_trig_pt && leptonTwoFired));
  ee    = ee    && ((electron->Pt() > electron_trig_pt && electron_2->Pt() > electron_pt && leptonOneFired) ||
                    (electron->Pt() > electron_pt && electron_2->Pt() > electron_trig_pt && leptonTwoFired));

  mumu &= nMuons     == 2;
  ee   &= nElectrons == 2;

  ////////////////////////////////////////////////////////////
  // Set 2 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    FillAllHistograms(set_offset + 2);
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
  mutau &= mll > 51. && mll < 170.;
  etau  &= mll > 51. && mll < 170.;
  emu   &= mll > 51. && mll < 170.;
  mumu  &= mll > 51. && mll < 170.;
  ee    &= mll > 51. && mll < 170.;

  //ensure a trigger was fired
  mutau &= leptonOneFired || leptonTwoFired;
  etau  &= leptonOneFired || leptonTwoFired;
  emu   &= leptonOneFired || leptonTwoFired;
  mumu  &= leptonOneFired || leptonTwoFired;
  ee    &= leptonOneFired || leptonTwoFired;

  // //ignore Mu50 trigger
  // mutau &= muonTriggerStatus != 2;
  // emu   &= !(muonTriggerStatus == 2 && triggerLeptonStatus == 2);
  // mumu  &= muonTriggerStatus != 2;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 3 + selection offset: eta, mass, and trigger cuts
  ////////////////////////////////////////////////////////////

  if(!fDYTesting || fCutFlowTesting) {
    FillAllHistograms(set_offset + 3);
  }

  ////////////////////////////////////////////////////////////
  // Set 4 + selection offset: add tau IDs
  ////////////////////////////////////////////////////////////

  mutau &= isLooseTau || tauDeepAntiJet >= 50; //63 = tight
  mutau &= tauDeepAntiMu  >= 10; //15 = tight
  mutau &= tauDeepAntiEle >= 10; //15 = loose
  mutau &= leptonTwoID2   >=  2; // 3 = tight MVA ID

  etau  &= isLooseTau || tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //15 = tight
  etau  &= tauDeepAntiEle >= 50; //63 = tight
  etau  &= leptonTwoID2   >=  2; // 3 = tight MVA ID

  //remove tau decay modes not interested in
  mutau &= tauDecayMode != 5 && tauDecayMode != 6;
  etau  &= tauDecayMode != 5 && tauDecayMode != 6;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    FillAllHistograms(set_offset + 4);
  }

  ////////////////////////////////////////////////////////////
  // Set 34 + selection offset: High pT lepton one region
  ////////////////////////////////////////////////////////////
  // if((!fDYTesting || mutau || etau) && fTreeVars.leponept > 60.)
  //   FillAllHistograms(set_offset + 34);

  //////////////////////////////////////////////////////////////
  //
  // Analysis cut section
  //
  //////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////
  // Set 29 + selection offset: Z mass + Jets selection
  ////////////////////////////////////////////////////////////////////////////
  // bool isZRegion = mll > 70. && mll < 110. && nJets > 0;
  // if(ee || etau || emu) isZRegion &= electron->Pt() > 28.;
  // if(mumu || mutau || emu) isZRegion &= muon->Pt() > 25.;
  // if(mumu) isZRegion &= muon_2->Pt() > 25.;
  // if(ee) isZRegion &= electron_2->Pt() > 28.;
  // if(isZRegion&&(fDYFakeTauTesting || !fDYTesting))
  //   FillAllHistograms(set_offset + 29);

  // if(fDYFakeTauTesting&&!fQCDFakeTauTesting&&!fWJFakeTauTesting&&!fTTFakeTauTesting) return kTRUE;


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
  } else if(fBJetCounting == 2) {
    if(fBJetTightness == 0) nBJetsUse = nBJets20;
    else if(fBJetTightness == 1) nBJetsUse = nBJets20M;
    else if(fBJetTightness == 2) nBJetsUse = nBJets20L;
  } else if(fBJetTightness == -1) //no b-jet cut
    nBJetsUse = 0;
  else {
    if(entry % 50000 == 0) printf("Bad bJetUse definition, Count = %i Tight = %i!\n", fBJetCounting, fBJetTightness);
  }

  //////////////////////////
  // Handle loose leptons //
  //////////////////////////

  //FIXME: add back in loose light leptons
  mutau &= !isLooseMuon;
  etau  &= !isLooseElectron;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 35 + selection offset: last set with MC estimated taus and leptons
  ////////////////////////////////////////////////////////////

  if((!fDYTesting || !(mumu || ee)) && nBJetsUse == 0) {
    FillAllHistograms(set_offset + 35);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 36 + selection offset: QCD selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  bool qcdSelection = (etau || (mutau && leptonOneIso/fTreeVars.leponept > 0.05)) && fTreeVars.mtlep < 40. && met < 60. && nBJetsUse == 0;
  if((fQCDFakeTauTesting || !fDYTesting) && qcdSelection)
    FillAllHistograms(set_offset + 36);

  ////////////////////////////////////////////////////////////////////////////
  // Set 37 + selection offset: W+Jets selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  bool wjetsSelection = (etau || mutau) && fTreeVars.mtlep > 70. && nBJetsUse == 0;
  if((fWJFakeTauTesting || !fDYTesting) && wjetsSelection)
    FillAllHistograms(set_offset + 37);

  ////////////////////////////////////////////////////////////////////////////
  // Set 38 + selection offset: ttbar selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  bool topSelection = (etau || mutau) && nBJetsUse >= 1;
  if((fTTFakeTauTesting || !fDYTesting) && topSelection)
    FillAllHistograms(set_offset + 38);

  // ////////////////////////////////////////////////////////////
  // // Set 38 + selection offset: MC estimated j->tau only
  // ////////////////////////////////////////////////////////////

  // if((!fDYTesting || !(mumu || ee || emu)) && (fIsData || tauGenFlavor == 26) && nBJetsUse == 0) {
  //   FillAllHistograms(set_offset + 38);
  // }

  ////////////////////////////////////////////////////////////
  // Set 40-41 + selection offset: Fake taus estimated with MC estimated scale factors
  ////////////////////////////////////////////////////////////
  Float_t tmp_evt_wt = eventWeight; //recored weight before corrections to update all weights at the end

  if((!fDYTesting || fJetTauTesting) && (mutau || etau) && nBJetsUse == 0) {
    //don't care about the MC uncertainties in MC study
    if(isLooseTau && mutau) {
      jetToTauWeight = fMuonJetToTauMCWeight.GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
                                                           fTreeVars.onemetdeltaphi,
                                                           jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                           jetToTauWeightCorr, jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                           jetToTauWeightCorrSys);
      jetToTauWeightCorr = jetToTauWeight;
      eventWeight *= jetToTauWeightCorr;
    } else {
      jetToTauWeight = 1.;
      jetToTauWeightCorr = 1.;
    }
    if(tauGenFlavor != 26) {
      FillAllHistograms(set_offset + 40); //true tight taus or loose taus
    } else {
      FillAllHistograms(set_offset + 41); //fake tight taus or loose taus
    }
  }
  eventWeight = tmp_evt_wt; //restore the proper event weight

  if((mutau || etau) && nBJetsUse == 0 && tauGenFlavor == 26) {
    FillAllHistograms(set_offset + 42); //fake tight taus or loose taus
  }

  if(!fUseMCEstimatedFakeLep && !fIsData) {
    emu   &= !isFakeMuon;
    emu   &= !isFakeElectron;
    mumu  &= !isFakeMuon;
    ee    &= !isFakeElectron;
    mutau &= !isFakeMuon;
    etau  &= !isFakeElectron;
  }

  //remove MC estimated jet --> tau component
  mutau &= fIsData > 0 || abs(tauGenFlavor) != 26;
  etau  &= fIsData > 0 || abs(tauGenFlavor) != 26;


  /////////////////////////
  // Jet --> tau weights //
  /////////////////////////

  //weigh anti-iso tau region by anti-iso --> tight iso weight
  if((etau || mutau) && isLooseTau) {
    //use data factor for MC and Data, since not using MC estimated fake tau rates
    if(fUseJetToTauComposition) {
      jetToTauWeight = 0.;
      jetToTauWeightCorr = 0.;
      Float_t jttUp(0.), jttDown(0.), jttSys(0.);
      if(mutau) {
        if(chargeTest)
          fMuonJetToTauComp.GetComposition(tau->Pt(), muon->Pt(), fTreeVars.onemetdeltaphi, fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        else
          fMuonJetToTauSSComp.GetComposition(tau->Pt(), muon->Pt(), fTreeVars.onemetdeltaphi, fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
      } else {
        if(chargeTest)
          fElectronJetToTauComp.GetComposition(tau->Pt(), electron->Pt(), fTreeVars.onemetdeltaphi, fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        else
          fElectronJetToTauSSComp.GetComposition(tau->Pt(), electron->Pt(), fTreeVars.onemetdeltaphi, fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
      }
      for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
        if(mutau) {
          fJetToTauWts[proc] = (fMuonJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
                                                                          fTreeVars.onemetdeltaphi,
                                                                          jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                                          fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                          jetToTauWeightCorrSys));
        } else {
          fJetToTauWts[proc] = (fElectronJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), electron->Pt(), electron->DeltaR(*tau),
                                                                              fTreeVars.onemetdeltaphi,
                                                                              jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                                              fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                              jetToTauWeightCorrSys));
        }
        jetToTauWeight     += fJetToTauComps[proc] * fJetToTauWts[proc];
        jetToTauWeightCorr += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc]; //weight with the correction for this process
        // jetToTauWeightCorr += fJetToTauComps[proc] * fJetToTauCorrs[proc];
        //store systematic effects
        jttUp   += fJetToTauComps[proc] * jetToTauWeightUp  ;
        jttDown += fJetToTauComps[proc] * jetToTauWeightDown;
        jttSys  += fJetToTauComps[proc] * jetToTauWeightSys ;
        jetToTauWeight_compUp       += fJetToTauCompsUp  [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
        jetToTauWeight_compDown     += fJetToTauCompsDown[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
      }
      jetToTauWeightUp   = jttUp  ;
      jetToTauWeightDown = jttDown;
      jetToTauWeightSys  = jttSys ;
      jetToTauWeightCorrUp   = jetToTauWeight; //set correction up to be ignoring the correction
      jetToTauWeightCorrDown = 2.*jetToTauWeightCorr - jetToTauWeight; //size of the weight in the other direction from 1
      jetToTauWeightCorrSys  = jetToTauWeight;
    } else if(mutau) { //don't use j->tau composition
      jetToTauWeight = fMuonJetToTauWeight.GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
                                                         fTreeVars.onemetdeltaphi,
                                                         jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                         jetToTauWeightCorr, jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                         jetToTauWeightCorrSys);
      jetToTauWeightCorr *= jetToTauWeight;
    } else if(etau) { //don't use j->tau composition
      jetToTauWeight = fElectronJetToTauWeight.GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), electron->Pt(), electron->DeltaR(*tau),
                                                             fTreeVars.onemetdeltaphi,
                                                             jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                             jetToTauWeightCorr, jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                             jetToTauWeightCorrSys);
      jetToTauWeightCorr *= jetToTauWeight;
    }
    jetToTauWeightGroup += SystematicGrouping::kJetToTau;
  }

  eventWeight *= jetToTauWeightCorr;

  ///////////////////////
  // SS --> OS weights //
  ///////////////////////

  qcdWeight = 1.; qcdWeightUp = 1.; qcdWeightDown = 1.; qcdWeightSys = 1.;
  //get scale factor for same sign --> opposite sign
  if(emu && !chargeTest) {
    qcdWeight = fQCDWeight.GetWeight(fTreeVars.lepdeltar, fTreeVars.lepdeltaphi, fTreeVars.leponeeta, fYear, qcdWeightUp, qcdWeightDown, qcdWeightSys);
  }

  eventWeight *= qcdWeight;

  /////////////////////////
  // Jet --> lep weights //
  /////////////////////////

  if(mutau && isLooseMuon) {
    //use data factor for MC and Data, since not using MC estimated fake tau rates
    eventWeight *= fJetToMuonWeight.GetDataFactor(fYear, muon->Pt(), muon->Eta());
  } else if(etau && isLooseElectron) {
    //use data factor for MC and Data, since not using MC estimated fake tau rates
    eventWeight *= fJetToElectronWeight.GetDataFactor(fYear, electron->Pt(), electron->Eta());
  }

  if(eventWeight != tmp_evt_wt) { //update tree var weights if needed
    fTreeVars.eventweight        *= eventWeight/tmp_evt_wt;
    fTreeVars.fulleventweight    *= eventWeight/tmp_evt_wt;
    fTreeVars.fulleventweightlum *= eventWeight/tmp_evt_wt;
    fTreeVars.eventweightMVA     *= eventWeight/tmp_evt_wt;
  }

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 30 + selection offset: QCD selection
  ////////////////////////////////////////////////////////////////////////////
  if((fQCDFakeTauTesting || !fDYTesting) && qcdSelection) {
    Float_t prev_evt_wt = eventWeight;
    Float_t prev_jtt_wt = jetToTauWeight;
    Float_t prev_jtt_cr = jetToTauWeightCorr;
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts  [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight *fJetToTauCorrs[JetToTauComposition::kQCD];
      eventWeight *= (jetToTauWeightCorr / prev_jtt_cr);
    }
    FillAllHistograms(set_offset + 30);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 31 + selection offset: W+Jets selection
  ////////////////////////////////////////////////////////////////////////////
  if((fWJFakeTauTesting || !fDYTesting) && wjetsSelection) {
    Float_t prev_evt_wt = eventWeight;
    Float_t prev_jtt_wt = jetToTauWeight;
    Float_t prev_jtt_cr = jetToTauWeightCorr;
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts  [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight *fJetToTauCorrs[JetToTauComposition::kWJets];
      eventWeight *= (jetToTauWeightCorr / prev_jtt_cr);
    }
    FillAllHistograms(set_offset + 31);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 32 + selection offset: Top selection
  ////////////////////////////////////////////////////////////////////////////
  if((fTTFakeTauTesting || !fDYTesting) && topSelection) {
    Float_t prev_evt_wt = eventWeight;
    Float_t prev_jtt_wt = jetToTauWeight;
    Float_t prev_jtt_cr = jetToTauWeightCorr;
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts  [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight *fJetToTauCorrs[JetToTauComposition::kTop];
      eventWeight *= (jetToTauWeightCorr / prev_jtt_cr);
    }
    FillAllHistograms(set_offset + 32);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
  }

  ////////////////////////////////////////////////////////////
  // Set 7 + selection offset: No MC estimated fake taus
  ////////////////////////////////////////////////////////////
  if((!fDYTesting || (mumu || ee)) && fDoSystematics >= 0)
    FillAllHistograms(set_offset + 7);

  if(fCutFlowTesting) return kTRUE;

  //Test the jet --> tau scale factors in mumu/ee
  if((mumu || ee) && nTaus == 1 && tausDM[0] != 5 && tausDM[0] != 6 && (fIsData || abs(tausGenFlavor[0]) != 26)) {
    if(tausAntiJet[0] <= fFakeTauIsoCut) { //loose ID tau
      //add loose --> tight tau weight, without leading lepton pT closure correction
      Float_t temp_event_weight = eventWeight;
      TLorentzVector taulv;
      taulv.SetPtEtaPhiM(tausPt[0], tausEta[0], tausPhi[0], 1.777);
      if(mumu) jetToTauWeight = fMuonJetToTauWeight.GetDataFactor    (tausDM[0], fYear, tausPt[0], tausEta[0],
                                                                      leptonOneP4->Pt(), leptonOneP4->DeltaR(taulv),
                                                                      fTreeVars.onemetdeltaphi, jetToTauWeightCorr);
      if(ee)   jetToTauWeight = fElectronJetToTauWeight.GetDataFactor(tausDM[0], fYear, tausPt[0], tausEta[0],
                                                                      leptonOneP4->Pt(), leptonOneP4->DeltaR(taulv),
                                                                      fTreeVars.onemetdeltaphi, jetToTauWeightCorr);
      jetToTauWeightCorr *= jetToTauWeight;
      eventWeight *= jetToTauWeightCorr;
      FillAllHistograms(set_offset + 51);
      eventWeight = temp_event_weight; //restore the event weight
      jetToTauWeight     = 1.;
      jetToTauWeightCorr = 1.;
    } else{
      //tight taus --> no weight
      FillAllHistograms(set_offset + 50);
    }
    //ensure jet to tau weights are removed if used in j->tau testing in ee/mumu channels
    jetToTauWeight = 1.; jetToTauWeightUp = 1.; jetToTauWeightDown = 1.; jetToTauWeightSys = 1.; jetToTauWeightGroup = 0;
    jetToTauWeightCorr = 1.; jetToTauWeightCorrUp = 1.; jetToTauWeightCorrDown = 1.; jetToTauWeightCorrSys = 1.;
  }

  ////////////////////////////////////////////////////////////
  // Set 36 + selection offset: High pT lepton one region
  ////////////////////////////////////////////////////////////
  // if((!fDYTesting || mutau || etau) && fTreeVars.leponept > 60.)
  //   FillAllHistograms(set_offset + 36);

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  mutau &= nBJetsUse == 0;
  etau  &= nBJetsUse == 0;
  emu   &= nBJetsUse == 0;
  mumu  &= nBJetsUse == 0;
  ee    &= nBJetsUse == 0;

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  mutau &= met < 60.;
  etau  &= met < 60.;
  emu   &= met < 60.;
  // mumu  &= met < 60.;
  // ee    &= met < 60.;

  //Add W+Jets selection orthogonality condition
  mutau &= fTreeVars.mtlep < 70.;
  etau  &= fTreeVars.mtlep < 70.;
  emu   &= fTreeVars.mtlep < 70.;
  // mumu  &= fTreeVars.mtlep < 70.;
  // ee    &= fTreeVars.mtlep < 70.;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 8);

  ////////////////////////////////////////////////////////////////////////////
  // Set 45-46 + selection offset: Tau eta region categories
  ////////////////////////////////////////////////////////////////////////////
  // if((emu || ((mutau || etau) && fabs(tau->Eta()) < 1.5))) //no tau in emu, put all with barrel category
  //   FillAllHistograms(set_offset + 45);
  // else if((mutau || etau) && fabs(tau->Eta()) >= 1.5)
  //   FillAllHistograms(set_offset + 46);

  ////////////////////////////////////////////////////////////////////////////
  // Set 22-23 + selection offset: Jet categories 0, > 0
  ////////////////////////////////////////////////////////////////////////////
  // if((mutau || etau || emu || mumu || ee) && nJets == 0)
  //   FillAllHistograms(set_offset + 22);
  // else if((mutau || etau || emu || mumu || ee) && nJets > 0)
  //   FillAllHistograms(set_offset + 23);


  ////////////////////////////////////////////////////////////////////////////
  // Set 24-25 : Mass window sets
  ////////////////////////////////////////////////////////////////////////////
  //Z0 window
  // if(mutau && mll < 95. && mll > 60.)  FillAllHistograms(set_offset + 24);
  // if(etau && mll < 95. && mll > 60.)   FillAllHistograms(set_offset + 24);
  if(emu && mll < 96. && mll > 85.)    FillAllHistograms(set_offset + 24);
  //higgs window
  // if(mutau && mll < 130. && mll > 75.) FillAllHistograms(set_offset + 25);
  // if(etau && mll < 130. && mll > 75.)  FillAllHistograms(set_offset + 25);
  if(emu && mll < 130. && mll > 120.)  FillAllHistograms(set_offset + 25);

  if(fDYTesting && !fDoMVASets) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 9-18 : BDT Cut categories
  ////////////////////////////////////////////////////////////////////////////
  //Set event weight to ignore training sample
  Float_t prev_wt = eventWeight;
  //only use weight if MC or is same sign data
  eventWeight = (fIsData == 0 || !chargeTest) ? fabs(fTreeVars.eventweightMVA) : prev_wt; //use abs to remove gen weight sign
  bool doMVASets = !fDYTesting || fDoMVASets;
  int category = -1; //histogram set to use, based on MVA score
  if(doMVASets) {
    //Total background Z0 MVAs: 9 - 13
    if(mutau) {
      category = Category("zmutau");
    } else if(etau) {
      category = Category("zetau");
    } else if(emu) {
      category = Category("zemu");
      FillAllHistograms(set_offset + 9 + category);
      category = Category("zmutau_e");
      FillAllHistograms(set_offset + kMuTauE - kEMu + 9 + category);
      category = Category("zetau_mu");
      FillAllHistograms(set_offset + kETauMu - kEMu + 9 + category);
    } else if(mumu) {
      category = Category("zemu");
    } else if(ee) {
      category = Category("zemu");
    }
  }
  if(category >= 0 && !emu) //do emu separately, rest do here
    FillAllHistograms(set_offset + 9 + category);

  //Total background higgs MVAs 14 - 18
  category = -1;
  if(doMVASets) {
    if(mutau) {
      category = Category("hmutau");
    } else if(etau) {
      category = Category("hetau");
    } else if(emu) {
      category = Category("hemu");
      FillAllHistograms(set_offset + 14 + category);
      category = Category("hmutau_e");
      FillAllHistograms(set_offset + kMuTauE - kEMu + 14 + category);
      category = Category("hetau_mu");
      FillAllHistograms(set_offset + kETauMu - kEMu + 14 + category);
    } else if(mumu) {
      category = Category("hemu");
    } else if(ee) {
      category = Category("hemu");
    }
  }

  if(category >= 0 && !emu) //do emu separately, rest do here
    FillAllHistograms(set_offset + 14 + category);

  //restore event weight
  eventWeight = prev_wt;

  if(fDYTesting) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 20-21 + selection offset: Box cuts
  ////////////////////////////////////////////////////////////////////////////
  //z ids
  // if(mutau && fEventId[0] && fEventId[0]->IDWord(fTreeVars) == 0)   FillAllHistograms(set_offset + 20);
  // if(etau && fEventId[10] && fEventId[10]->IDWord(fTreeVars) == 0)  FillAllHistograms(set_offset + 20);
  // if(emu && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)   FillAllHistograms(set_offset + 20);
  // if(mumu && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)  FillAllHistograms(set_offset + 20);
  // if(ee && fEventId[20] && fEventId[20]->IDWord(fTreeVars) == 0)    FillAllHistograms(set_offset + 20);
  // //higgs ids
  // if(emu && fEventId[50] && fEventId[50]->IDWord(fTreeVars) == 0)   FillAllHistograms(set_offset + 21);



  ////////////////////////////////////////////////////////////////////////////
  // Set 27 + selection offset: Genuine hadronic taus
  ////////////////////////////////////////////////////////////////////////////
  bool genuineTau = fIsData != 0 || abs(tauGenFlavor) == 15;
  genuineTau &= mutau || etau;
  if(genuineTau)
    FillAllHistograms(set_offset + 27);

  ////////////////////////////////////////////////////////////////////////////
  // Set 28 + selection offset: Non-jet based hadronic taus
  ////////////////////////////////////////////////////////////////////////////
  // bool nonJetTau = fIsData != 0 || abs(tauGenFlavor) != 26;
  // nonJetTau &= mutau || etau;
  // if(nonJetTau)
  //   FillAllHistograms(set_offset + 28);


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

//determine MVA category
int ZTauTauHistMaker::Category(TString selection) {
  int category = 0;
  //get MVA output index by selection
  int mva_i = fMVAConfig.GetIndexBySelection(selection);
  if(mva_i < 0) return category;
  std::vector<double> mvaCuts = fMVAConfig.categories_[selection];
  for(unsigned index = 0; index < mvaCuts.size(); ++index) {
    if(fMvaOutputs[mva_i] > mvaCuts[index]) ++category; //passes category
    else break; //fails
  }
  return category;
}

//initialize the randomly assigned systematic shifts
void ZTauTauHistMaker::InitializeSystematics() {
  leptonOneWeight1_group = 0; leptonOneWeight2_group = 0;
  leptonTwoWeight1_group = 0; leptonTwoWeight2_group = 0;
  if(abs(leptonOneFlavor) == 11) {
    leptonOneWeight1_sys = fSystematicShifts->ElectronID    (fYear, leptonOneWeight1_bin) ? leptonOneWeight1_up : leptonOneWeight1_down;
    leptonOneWeight2_sys = fSystematicShifts->ElectronRecoID(fYear, leptonOneWeight2_bin) ? leptonOneWeight2_up : leptonOneWeight2_down;
    leptonOneWeight1_group = fElectronIDWeight.GetIDGroup(leptonOneWeight1_bin, fYear) + SystematicGrouping::kElectronID;
    leptonOneWeight2_group = fElectronIDWeight.GetRecoGroup(leptonOneWeight2_bin, fYear) + SystematicGrouping::kElectronRecoID;
  } else if(abs(leptonOneFlavor) == 13) {
    if(fYear == 2018) { //remove the period dependence for 2018, as it is using inclusive weights
      leptonOneWeight1_bin %= 10000;
      leptonTwoWeight1_bin %= 10000;
      leptonOneWeight2_bin %= 10000;
      leptonTwoWeight2_bin %= 10000;
    }
    leptonOneWeight1_sys = fSystematicShifts->MuonID   (fYear, leptonOneWeight1_bin) ? leptonOneWeight1_up : leptonOneWeight1_down;
    leptonOneWeight2_sys = fSystematicShifts->MuonIsoID(fYear, leptonOneWeight2_bin) ? leptonOneWeight2_up : leptonOneWeight2_down;
    leptonOneWeight1_group = fMuonIDWeight.GetIDGroup(leptonOneWeight1_bin, fYear) + SystematicGrouping::kMuonID;
    leptonOneWeight2_group = fMuonIDWeight.GetIsoGroup(leptonOneWeight2_bin, fYear) + SystematicGrouping::kMuonIsoID;
  } else if(abs(leptonOneFlavor) == 15) {
    int tauGenID = 0;
    if     (abs(tauGenFlavor) == 15) tauGenID = 5;
    else if(abs(tauGenFlavor) == 13) tauGenID = 2;
    else if(abs(tauGenFlavor) == 11) tauGenID = 1;
    leptonOneWeight1_sys = fSystematicShifts->TauID(fYear, tauGenID, leptonOneWeight1_bin) ? leptonOneWeight1_up : leptonOneWeight1_down;
    leptonOneWeight2_sys = leptonOneWeight2; //not defined
  }
  if(abs(leptonTwoFlavor) == 11) {
    leptonTwoWeight1_sys = fSystematicShifts->ElectronID    (fYear, leptonTwoWeight1_bin) ? leptonTwoWeight1_up : leptonTwoWeight1_down;
    leptonTwoWeight2_sys = fSystematicShifts->ElectronRecoID(fYear, leptonTwoWeight2_bin) ? leptonTwoWeight2_up : leptonTwoWeight2_down;
    leptonTwoWeight1_group = fElectronIDWeight.GetIDGroup(leptonTwoWeight1_bin, fYear) + SystematicGrouping::kElectronID;
    leptonTwoWeight2_group = fElectronIDWeight.GetRecoGroup(leptonTwoWeight2_bin, fYear) + SystematicGrouping::kElectronRecoID;
  } else if(abs(leptonTwoFlavor) == 13) {
    leptonTwoWeight1_sys = fSystematicShifts->MuonID   (fYear, leptonTwoWeight1_bin) ? leptonTwoWeight1_up : leptonTwoWeight1_down;
    leptonTwoWeight2_sys = fSystematicShifts->MuonIsoID(fYear, leptonTwoWeight2_bin) ? leptonTwoWeight2_up : leptonTwoWeight2_down;
    leptonTwoWeight1_group = fMuonIDWeight.GetIDGroup(leptonOneWeight1_bin, fYear) + SystematicGrouping::kMuonID;
    leptonTwoWeight2_group = fMuonIDWeight.GetIsoGroup(leptonOneWeight2_bin, fYear) + SystematicGrouping::kMuonIsoID;
  } else if(abs(leptonTwoFlavor) == 15) {
    int tauGenID = 0;
    if     (abs(tauGenFlavor) == 15) tauGenID = 5;
    else if(abs(tauGenFlavor) == 13) tauGenID = 2;
    else if(abs(tauGenFlavor) == 11) tauGenID = 1;
    leptonTwoWeight1_sys = fSystematicShifts->TauID(fYear, tauGenID, leptonTwoWeight1_bin) ? leptonTwoWeight1_up : leptonTwoWeight1_down;
    leptonTwoWeight2_sys = leptonTwoWeight2; //not defined
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
