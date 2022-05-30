#define CLFVHistMaker_cxx

#include "interface/CLFVHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
CLFVHistMaker::CLFVHistMaker(int seed, TTree * /*tree*/) : fSystematicSeed(seed),
                                                           fMuonJetToTauComp("mutau", 2035, 3, 0), fMuonJetToTauSSComp("mutau", 3035, 3, 0),
                                                           fElectronJetToTauComp("etau", 2035, 3, 0), fElectronJetToTauSSComp("etau", 3035, 3, 0),
                                                           fJetToMuonWeight("mumu"), fJetToElectronWeight("ee"),
                                                           fQCDWeight("emu", /*11010*/ 1100200/*anti-iso, jet binned, no fits*/, seed, 0),
                                                           fMuonIDWeight(seed),
                                                           //FIXME: Turn on or keep off electron trigger interpolation
                                                           fElectronIDWeight(0, seed), fZPtWeight("MuMu", seed),
                                                           //FIXME: Turn on or keep off embedding trigger interpolation
                                                           fEmbeddingWeight(), fEmbeddingTnPWeight(10/*10*(use 2016 BF/GH scales) + 1*(interpolate scales or not)*/) {

  //ensure pointers set to null to not attempt to delete if never initialized
  fCutFlow = nullptr;
  for(int i = 0; i < fn; i++) {
    fEventHist     [i] = nullptr;
    fLepHist       [i] = nullptr;
    fSystematicHist[i] = nullptr;
  }
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    fMuonJetToTauWeights    [proc] = nullptr;
    fElectronJetToTauWeights[proc] = nullptr;
  }
  fRnd = nullptr;
  fSystematicShifts = nullptr;
  for(int i = 0; i <kIds; ++i) fEventId[i] = nullptr;

  leptonOneP4 = new TLorentzVector();
  leptonTwoP4 = new TLorentzVector();
  jetOneP4    = new TLorentzVector();
}

//--------------------------------------------------------------------------------------------------------------
CLFVHistMaker::~CLFVHistMaker() {
  if(fCutFlow) delete fCutFlow;
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    if(fMuonJetToTauWeights      [proc]) {delete fMuonJetToTauWeights      [proc];}
    if(fMuonJetToTauMCWeights    [proc]) {delete fMuonJetToTauMCWeights    [proc];}
    if(fElectronJetToTauWeights  [proc]) {delete fElectronJetToTauWeights  [proc];}
    if(fElectronJetToTauMCWeights[proc]) {delete fElectronJetToTauMCWeights[proc];}
  }
  DeleteHistograms();

  if (fTauIDWeight     ) delete fTauIDWeight     ;
  if (fSystematicShifts) delete fSystematicShifts;
  if (fRnd             ) delete fRnd             ;
  for(int i = 0; i <kIds; ++i) {if(fEventId[i]) delete fEventId[i];}
  if(leptonOneP4) delete leptonOneP4;
  if(leptonTwoP4) delete leptonTwoP4;
  if(jetOneP4)    delete jetOneP4;
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();
  std::setbuf(stdout, NULL); //don't buffer the stdout stream
  printf("CLFVHistMaker::Begin\n");
  timer->Start();
  fChain = 0;
  fElectronIDWeight.verbose_ = fVerbose;
  fMuonIDWeight.verbose_ = fVerbose;
  fBTagWeight.verbose_ = fVerbose;
  fCutFlow = new TH1D("hcutflow", "Cut-flow", 100, 0, 100);

  fMuonJetToTauWeights      [JetToTauComposition::kWJets] = new JetToTauWeight("MuonWJets"      , "mutau", "WJets",   31, 20300300, fSystematicSeed, 0);
  fMuonJetToTauWeights      [JetToTauComposition::kZJets] = new JetToTauWeight("MuonZJets"      , "mutau", "WJets",   31, 20300300, fSystematicSeed, 0);
  fMuonJetToTauWeights      [JetToTauComposition::kTop  ] = new JetToTauWeight("MuonTop"        , "mutau", "Top"  ,   82,  1100301, fSystematicSeed, 0);
  fMuonJetToTauWeights      [JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonQCD"        , "mutau", "QCD"  , 1030, 40300300, fSystematicSeed, 0);

  fMuonJetToTauMCWeights    [JetToTauComposition::kWJets] = new JetToTauWeight("MuonMCWJets"    , "mutau", "WJets",   88, 20300301, fSystematicSeed, 0); //MC weights with MC non-closure + bias
  fMuonJetToTauMCWeights    [JetToTauComposition::kZJets] = new JetToTauWeight("MuonMCZJets"    , "mutau", "WJets",   88, 20300301, fSystematicSeed, 0);
  fMuonJetToTauMCWeights    [JetToTauComposition::kTop  ] = new JetToTauWeight("MuonMCTop"      , "mutau", "Top"  ,   82,  1100301, fSystematicSeed, 0); //Normal weights
  fMuonJetToTauMCWeights    [JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonMCQCD"      , "mutau", "QCD"  , 1095,   300300, fSystematicSeed, 0); //high iso weights for SS --> OS bias

  //FIXME: Add back W+Jets MC bias (Mode = 10300300, 60300300 for only MC bias shape)
  fElectronJetToTauWeights  [JetToTauComposition::kWJets] = new JetToTauWeight("ElectronWJets"  , "etau" , "WJets",   31, 10300300, fSystematicSeed, 0);
  fElectronJetToTauWeights  [JetToTauComposition::kZJets] = new JetToTauWeight("ElectronZJets"  , "etau" , "WJets",   31, 10300300, fSystematicSeed, 0);
  fElectronJetToTauWeights  [JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronTop"    , "etau" , "Top"  ,   82,  1100301, fSystematicSeed, 0);
  fElectronJetToTauWeights  [JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronQCD"    , "etau" , "QCD"  , 1030, 50300300, fSystematicSeed, 0);

  fElectronJetToTauMCWeights[JetToTauComposition::kWJets] = new JetToTauWeight("ElectronMCWJets", "etau" , "WJets",   88, 10300301, fSystematicSeed, 0); //MC weights with MC non-closure + bias
  fElectronJetToTauMCWeights[JetToTauComposition::kZJets] = new JetToTauWeight("ElectronMCZJets", "etau" , "WJets",   88, 10300301, fSystematicSeed, 0);
  fElectronJetToTauMCWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronMCTop"  , "etau" , "Top"  ,   82,  1100301, fSystematicSeed, 0); //Normal weights
  fElectronJetToTauMCWeights[JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronMCQCD"  , "etau" , "QCD"  , 1095,   300300, fSystematicSeed, 0); //loose electron ID weights for SS --> OS bias

  fTauIDWeight = new TauIDWeight(fIsEmbed, fVerbose);

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
  fTreeVars.type = -1;

}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("CLFVHistMaker::SlaveBegin\n");
  TString option = GetOption();

}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillAllHistograms(Int_t index) {
  if(eventWeight < 0. || !std::isfinite(eventWeight*genWeight)) {
    std::cout << "WARNING! Event " << fentry << " set " << index << " has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << genWeight << "; puWeight = " << puWeight
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOneTrigWeight*leptonTwoTrigWeight
              << "; jetPUIDWeight = " << jetPUIDWeight << "; zPtWeight = " << zPtWeight
              << std::endl;
  }
  if(!std::isfinite(eventWeight*genWeight)) {
    // std::cout << fentry << ": Warning! Set " << index << " has non-finited event weight = " << eventWeight*genWeight
    //           << ", setting to 0" << std::endl;
    eventWeight = 0.;
    genWeight = 1.;
  }
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

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  // BookPhotonHistograms();
  if(fWriteTrees && fDoSystematics >= 0) BookTrees();
  if(fDoSystematics) BookSystematicHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookEventHistograms() {
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
      // fEventHist[i]->hLumiSection            = new TH1F("lumiSection"         , Form("%s: LumiSection"         ,dirname)  , 200,    0, 4e6);
      Utilities::BookH1F(fEventHist[i]->hEventWeight             , "eventweight"             , Form("%s: EventWeight"                 ,dirname), 100,   -1,   3, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEventWeight          , "logeventweight"          , Form("%s: LogEventWeight"              ,dirname),  50,  -10,   1, folder);
      Utilities::BookH1F(fEventHist[i]->hEventWeightMVA          , "eventweightmva"          , Form("%s: EventWeightMVA"              ,dirname), 100,   -5,   5, folder);
      Utilities::BookH1D(fEventHist[i]->hGenWeight               , "genweight"               , Form("%s: GenWeight"                   ,dirname),   5, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hFullEventWeightLum      , "fulleventweightlum"      , Form("%s: abs(FullEventWeightLum)"     ,dirname), 100,    0,  15, folder);
      Utilities::BookH1F(fEventHist[i]->hLogFullEventWeightLum   , "logfulleventweightlum"   , Form("%s: log(abs(FullEventWeightLum))",dirname),  50,  -10,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hEmbeddingWeight         , "embeddingweight"         , Form("%s: EmbeddingWeight"             ,dirname), 100,    0, 0.5, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEmbeddingWeight      , "logembeddingweight"      , Form("%s: Log10(EmbeddingWeight)"      ,dirname),  40,   -9,   1, folder);
      Utilities::BookH1F(fEventHist[i]->hEmbeddingUnfoldingWeight, "embeddingunfoldingweight", Form("%s: EmbeddingUnfoldingWeight"    ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeight          , "jettotauweight"          , Form("%s: JetToTauWeight"              ,dirname),  40,    0,   2, folder);
      int jstart = SystematicGrouping::kJetToTau;
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightGroup     , "jettotauweightgroup"     , Form("%s: JetToTauWeightGroup"         ,dirname),  50, jstart, 50+jstart, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightCorr      , "jettotauweightcorr"      , Form("%s: JetToTauWeightCorr"          ,dirname),  50,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightBias      , "jettotauweightbias"      , Form("%s: JetToTauWeightBias"          ,dirname),  50,    0,   5, folder);
      for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
        Utilities::BookH1F(fEventHist[i]->hJetToTauComps[ji]     , Form("jettotaucomps_%i", ji), Form("%s: JetToTauComps %i"      ,dirname, ji),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hJetToTauWts  [ji]     , Form("jettotauwts_%i"  , ji), Form("%s: JetToTauWts %i"        ,dirname, ji),  50,    0,   2, folder);
      }
      Utilities::BookH1F(fEventHist[i]->hJetToTauCompEffect      , "jettotaucompeffect"      , Form("%s: JetToTauCompEffect"          ,dirname),  50,    0,   2, folder);
      Utilities::BookH1D(fEventHist[i]->hIsSignal                , "issignal"                , Form("%s: IsSignal"                    ,dirname),   5,   -2,   3, folder);
      Utilities::BookH1F(fEventHist[i]->hNPV[0]                  , "npv"                     , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1F(fEventHist[i]->hNPV[1]                  , "npv1"                    , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1F(fEventHist[i]->hNPU[0]                  , "npu"                     , Form("%s: NPU"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1F(fEventHist[i]->hNPU[1]                  , "npu1"                    , Form("%s: NPU"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1D(fEventHist[i]->hNPartons                , "npartons"                , Form("%s: NPartons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hLHENJets                , "lhenjets"                , Form("%s: LHE N(jets)"                 ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNMuons                  , "nmuons"                  , Form("%s: NMuons"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNElectrons              , "nelectrons"              , Form("%s: NElectrons"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTaus                   , "ntaus"                   , Form("%s: NTaus"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNPhotons                , "nphotons"                , Form("%s: NPhotons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTausHad             , "ngentaushad"             , Form("%s: NGenTausHad"                 ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTausLep             , "ngentauslep"             , Form("%s: NGenTausLep"                 ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets                   , "njets"                   , Form("%s: NJets"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[1]              , "njets201"                , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20Rej[0]           , "njets20rej"              , Form("%s: NJets20Rej"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20Rej[1]           , "njets20rej1"             , Form("%s: NJets20Rej"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNFwdJets                , "nfwdjets"                , Form("%s: NFwdJets"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets                  , "nbjets"                  , Form("%s: NBJets"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJetsM                 , "nbjetsm"                 , Form("%s: NBJetsM"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJetsL                 , "nbjetsl"                 , Form("%s: NBJetsL"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20M[0]            , "nbjets20m"               , Form("%s: NBJets20M"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[1]             , "nbjets201"               , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20M[1]            , "nbjets20m1"              , Form("%s: NBJets20M"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[1]            , "nbjets20l1"              , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);

      // fEventHist[i]->hJetsFlavor             = new TH1D("jetsflavor"          , Form("%s: JetsFlavor"          ,dirname)  ,  60, -10,  50);
      int njetspt = 6;
      double jetspt[] = {0.  , 30. , 40. , 50.  , 70.,
                         100.,
                         1000.};
      int njetseta = 10;
      double jetseta[] = {-2.5 , -2. , -1.5 , -1.,
                          -0.5 , 0.   , 0.5 , 1.   , 1.5,
                          2.   ,
                          2.5};

      Utilities::BookH2F(fEventHist[i]->hJetsPtVsEta[0]  , "jetsptvseta0"        , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hJetsPtVsEta[1]  , "jetsptvseta1"        , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hJetsPtVsEta[2]  , "jetsptvseta2"        , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsPtVsEta[0] , "bjetsptvseta0"       , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsPtVsEta[1] , "bjetsptvseta1"       , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsPtVsEta[2] , "bjetsptvseta2"       , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsMPtVsEta[0], "bjetsmptvseta0"      , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsMPtVsEta[1], "bjetsmptvseta1"      , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsMPtVsEta[2], "bjetsmptvseta2"      , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsLPtVsEta[0], "bjetslptvseta0"      , Form("%s: Jet pT vs eta [0]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsLPtVsEta[1], "bjetslptvseta1"      , Form("%s: Jet pT vs eta [1]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);
      Utilities::BookH2F(fEventHist[i]->hBJetsLPtVsEta[2], "bjetslptvseta2"      , Form("%s: Jet pT vs eta [2]"   ,dirname),  njetseta, jetseta, njetspt, jetspt, folder);

      Utilities::BookH1D(fEventHist[i]->hMcEra              , "mcera"               , Form("%s: MCEra + 2*(year-2016)",dirname),  8,    0,   8, folder);
      Utilities::BookH1D(fEventHist[i]->hTriggerLeptonStatus, "triggerleptonstatus" , Form("%s: TriggerLeptonStatus" ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hMuonTriggerStatus  , "muontriggerstatus"   , Form("%s: MuonTriggerStatus"   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTriggered         , "ntriggered"          , Form("%s: NTriggered"          ,dirname),   5,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hPuWeight           , "puweight"            , Form("%s: PuWeight"            ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPUIDWeight      , "jetpuidweight"       , Form("%s: JetPUIDWeight"       ,dirname),  50,    0,   2, folder);
      Utilities::BookH1D(fEventHist[i]->hPrefireWeight      , "prefireweight"       , Form("%s: PrefireWeight"       ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hBTagWeight         , "btagweight"          , Form("%s: BTagWeight"          ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hZPtWeight          , "zptweight"           , Form("%s: ZPtWeight"           ,dirname),  50,    0,   2, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDecayMode[0]    , "taudecaymode"        , Form("%s: TauDecayMode"        ,dirname),  15,    0,  15, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDecayMode[1]    , "taudecaymode1"       , Form("%s: TauDecayMode"        ,dirname),  15,    0,  15, folder);
      Utilities::BookH1D(fEventHist[i]->hTauGenFlavor       , "taugenflavor"        , Form("%s: TauGenFlavor"        ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiEle     , "taudeepantiele"      , Form("%s: TauDeepAntiEle"      ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiMu      , "taudeepantimu"       , Form("%s: TauDeepAntiMu"       ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiJet     , "taudeepantijet"      , Form("%s: TauDeepAntiJet"      ,dirname),  30,    0,  30, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPt[1]           , "jetpt1"              , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hJetEta             , "jeteta"              , Form("%s: JetEta"              ,dirname), 100,   -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hTauPt              , "taupt"               , Form("%s: TauPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hTauEta             , "taueta"              , Form("%s: TauEta"              ,dirname),  50, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hHtSum              , "htsum"               , Form("%s: HtSum"               ,dirname), 100,    0, 400, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                 , "ht"                  , Form("%s: Ht"                  ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hHtPhi              , "htphi"               , Form("%s: HtPhi"               ,dirname),  50,   -4,   4, folder);

      // fEventHist[i]->hPFMet                  = new TH1F("pfmet"               , Form("%s: PF Met"              ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hPFMetPhi               = new TH1F("pfmetphi"            , Form("%s: PF MetPhi"           ,dirname)  ,  80, -4,   4);
      // fEventHist[i]->hPFCovMet00             = new TH1F("pfcovmet00"          , Form("%s: PF CovMet00"         ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPFCovMet01             = new TH1F("pfcovmet01"          , Form("%s: PF CovMet01"         ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hPFCovMet11             = new TH1F("pfcovmet11"          , Form("%s: PF CovMet11"         ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPuppMet                = new TH1F("puppmet"             , Form("%s: PUPPI Met"           ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hPuppMetPhi             = new TH1F("puppmetphi"          , Form("%s: PUPPI MetPhi"        ,dirname)  ,  80, -4,   4);
      // fEventHist[i]->hPuppCovMet00           = new TH1F("puppcovmet00"        , Form("%s: PUPPI CovMet00"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hPuppCovMet01        = new TH1F("puppcovmet01"        , Form("%s: PUPPI CovMet01"      ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hPuppCovMet11        = new TH1F("puppcovmet11"        , Form("%s: PUPPI CovMet11"      ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hTrkMet                 = new TH1F("trkmet"              , Form("%s: Trk Met"             ,dirname)  , 200,  0, 400);
      // fEventHist[i]->hTrkMetPhi              = new TH1F("trkmetphi"           , Form("%s: Trk MetPhi"          ,dirname)  ,  80, -4,   4);
      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                 ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMetPhi              , "metphi"              , Form("%s: MetPhi"              ,dirname)  ,  40, -4,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hMetCorr             , "metcorr"             , Form("%s: Met Correction"      ,dirname)  ,  50,  0,  25, folder);
      Utilities::BookH1F(fEventHist[i]->hMetCorrPhi          , "metcorrphi"          , Form("%s: MetPhi Correction"   ,dirname)  ,  40, -4,   4, folder);
      // fEventHist[i]->hCovMet00               = new TH1F("covmet00"            , Form("%s: CovMet00"            ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hCovMet01               = new TH1F("covmet01"            , Form("%s: CovMet01"            ,dirname) , 1000,-1000.,1000.);
      // fEventHist[i]->hCovMet11               = new TH1F("covmet11"            , Form("%s: CovMet11"            ,dirname) , 1000,    0.,1000.);
      // fEventHist[i]->hMetVsPt                = new TH2F("metvspt"             , Form("%s: MetVsPt"             ,dirname) ,200,0,400, 200,0,400);
      // fEventHist[i]->hMetVsM                 = new TH2F("metvsm"              , Form("%s: MetVsM"              ,dirname) ,200,0,400, 200,0,400);
      // fEventHist[i]->hMetVsHtSum             = new TH2F("metvshtsum"          , Form("%s: MetVsHtSum"          ,dirname) ,200,0,400, 200,0,400);
      // fEventHist[i]->hMetOverSqrtHtSum       = new TH1F("metoversqrthtsum"    , Form("%s: MetOverSqrtHtSum"    ,dirname) ,  200,    0., 400.);
      // fEventHist[i]->hMassSVFit                   = new TH1F("masssvfit"           , Form("%s: MassSVFit"           ,dirname) , 1000,    0., 200.);
      // fEventHist[i]->hMassErrSVFit        = new TH1F("masserrsvfit"        , Form("%s: MassErrSVFit"        ,dirname) , 1000,    0., 100.);
      // fEventHist[i]->hSVFitStatus            = new TH1D("svfitstatus"         , Form("%s: SVFitStatus"         ,dirname) ,   10,    0.,  10.);


      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPt[1], "leppt1"        , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPt[2], "leppt2"        , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[1] , "lepm1"         , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[2] , "lepm2"         , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[3] , "lepm3"         , Form("%s: Lepton M"       ,dirname)  ,  80,  70, 110, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[4] , "lepm4"         , Form("%s: Lepton M"       ,dirname)  ,  40, 105, 145, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPhi  , "lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  40,  -4,   4, folder);
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[0], "lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[1], "lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);

      //variable width bins for pT vs mass
      // const double mbins[] = { 0.   , 70.  , 75.  , 80.  , 85.  ,
      //                          90.  , 95.  , 100. , 105. , 110. ,
      //                          120. , 140. ,
      //                          1000.};
      const double mbins[] = {50., 80., 100., 130., 500.}; //modified from H->tautau
      const int nmbins = sizeof(mbins) / sizeof(*mbins) - 1;
      // const double pbins[] = { 0.  , 1.  , 2.  , 3.  , 4.  ,
      //                          5.  , 6.  , 7.  , 8.  , 9.  ,
      //                          10. , 11. , 12. , 14. , 16. ,
      //                          20. , 25. , 30. , 35. , 40. ,
      //                          45. , 50. , 60. , 70. , 80. ,
      //                          100., 150.,
      //                          1000.};
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
      Utilities::BookH1F(fEventHist[i]->hZMass[0]   , "zmass"         , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[1]   , "zmass1"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[2]   , "zmass2"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[3]   , "zmass3"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);
      Utilities::BookH1F(fEventHist[i]->hZMass[4]   , "zmass4"        , Form("%s: Z Mass"         ,dirname)  , nmbins,   mbins, folder);

      Utilities::BookH1F(fEventHist[i]->hZLepOnePt  , "zleponept"     , Form("%s: ZLepOnePt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoPt  , "zleptwopt"     , Form("%s: ZLepTwoPt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepOneEta , "zleponeeta"    , Form("%s: ZLepOneEta"     ,dirname)  ,  50, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoEta , "zleptwoeta"    , Form("%s: ZLepTwoEta"     ,dirname)  ,  50, -2.5, 2.5, folder);


      if(fSelection == "" || fSelection == "mutau" || fSelection == "etau") { //j->tau related histograms
        //variable width bins for eta vs pT
        const double tetabins[] = { 0., 1.5, 2.3};
        const int ntetabins = sizeof(tetabins) / sizeof(*tetabins) - 1;
        const double tpbins[] = { 20., 22., 24., 26., 29.,
                                  33., 37., 42., 50., 60.,
                                  100.,
                                  200.};
        const int ntpbins = sizeof(tpbins) / sizeof(*tpbins) - 1;

        const int ndms = 4; //decay mode categories
        const int njetsmax = 2; //N(jets) categories: 0, 1, and >= 2 jets
        const int ncats = 3; //tau isolation categories: 0 = inclusive; 1 = anti-isolated; 2 = isolated
        for(int cat = 0; cat < ncats; ++cat) {
          for(int dm = 0; dm < ndms; ++dm) {
            for(int njets = 0; njets <= njetsmax; ++njets) {
              const char* hname = Form("faketau%ijet%idm_%i", njets, dm, cat);
              Utilities::BookH2F(fEventHist[i]->hFakeTauNJetDMPtEta[cat][njets][dm], hname, Form("%s: Fake tau Eta vs Pt" ,dirname),
                                 ntpbins, tpbins, ntetabins, tetabins, folder);
              const char* hmcname = Form("faketaumc%ijet%idm_%i", njets, dm, cat);
              Utilities::BookH2F(fEventHist[i]->hFakeTauMCNJetDMPtEta[cat][njets][dm], hmcname, Form("%s: Fake tau MC Eta vs Pt" ,dirname),
                                 ntpbins, tpbins, ntetabins, tetabins, folder);
            }
          }
        }

        Utilities::BookH1F(fEventHist[i]->hTausPt       , "tauspt"       , Form("%s: TausPt     "  ,dirname), 100,  0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hTausEta      , "tauseta"      , Form("%s: TausEta    "  ,dirname),  50, -2.5, 2.5, folder);
        Utilities::BookH1D(fEventHist[i]->hTausDM       , "tausdm"       , Form("%s: TausDM     "  ,dirname),  15,  0.,  15., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiJet  , "tausantijet"  , Form("%s: TausAntiJet"  ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiEle  , "tausantiele"  , Form("%s: TausAntiEle"  ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiMu   , "tausantimu"   , Form("%s: TausAntiMu"   ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausMVAAntiMu, "tausmvaantimu", Form("%s: TausMVAAntiMu",dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausGenFlavor, "tausgenflavor", Form("%s: TausGenFlavor",dirname),  40,  0.,  40., folder);
        Utilities::BookH1F(fEventHist[i]->hTausDeltaR   , "tausdeltar"   , Form("%s: TausDeltaR"   ,dirname),  50,  0.,   5., folder);
        Utilities::BookH1F(fEventHist[i]->hFakeTausPt   , "faketauspt"   , Form("%s: TausPt     "  ,dirname), 100,  0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hFakeTausEta  , "faketauseta"  , Form("%s: TausEta    "  ,dirname),  50, -2.5, 2.5, folder);
        Utilities::BookH1D(fEventHist[i]->hFakeTausDM   , "faketausdm"   , Form("%s: TausDM     "  ,dirname),  15,  0.,  15., folder);

        // jet --> tau non-closure histograms
        const double jt_lepm_bins[] = {50., 60., 70., 80., 100., 120., 140., 200.};
        const int    jt_nlepm_bins  = sizeof(jt_lepm_bins) / sizeof(*jt_lepm_bins) - 1;
        const double jt_dr_bins[]   = {0., 2., 2.5, 3., 3.5, 4., 5.};
        const int    jt_ndr_bins    = sizeof(jt_dr_bins) / sizeof(*jt_dr_bins) - 1;
        const double jt_mt_bins[]   = {0., 20., 35., 45., 55., 65., 75., 85., 150.};
        const int    jt_nmt_bins    = sizeof(jt_mt_bins) / sizeof(*jt_mt_bins) - 1;
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[0], "jettaudeltar0", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[1], "jettaudeltar1", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[2], "jettaudeltar2", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [0], "jettaulepm0"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [1], "jettaulepm1"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [2], "jettaulepm2"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [0], "jettaumtlep0" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [1], "jettaumtlep1" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [2], "jettaumtlep2" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [0], "jettaumtone0" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [1], "jettaumtone1" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [2], "jettaumtone2" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [0], "jettaumttwo0" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [1], "jettaumttwo1" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [2], "jettaumttwo2" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
      }

      //Jet --> light lep histograms
      //variable width bins for eta vs pT
      const double letabins[] = { 0., 1.5, 2.5};
      const int nletabins = sizeof(letabins) / sizeof(*letabins) - 1;
      const double lpbins[] = { 10., 15., 20., 25., 35.,
                                         50.,
                                         200.};
      const int nlpbins = sizeof(lpbins) / sizeof(*lpbins) - 1;

      for(int cat = 0; cat < 3; ++cat) {
        Utilities::BookH2F(fEventHist[i]->hFakeLepPtEta[cat], Form("fakeleppteta_%i", cat), Form("%s: Fake lepton Eta vs Pt" ,dirname), nlpbins, lpbins, nletabins, letabins, folder);
      }
      Utilities::BookH1F(fEventHist[i]->hLeptonsPt       , "leptonspt"       , Form("%s: LeptonsPt  "    ,dirname), 100,   0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLeptonsEta      , "leptonseta"      , Form("%s: LeptonsEta "    ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1D(fEventHist[i]->hLeptonsID       , "leptonsid"       , Form("%s: LeptonsID  "    ,dirname),  15,   0.,  15., folder);
      Utilities::BookH1D(fEventHist[i]->hLeptonsIsoID    , "leptonsisoid"    ,Form("%s: LeptonsIsoID"    ,dirname),  15,   0.,  15., folder);
      Utilities::BookH1D(fEventHist[i]->hLeptonsGenFlavor, "leptonsgenflavor",Form("%s: LeptonsGenFlavor",dirname),  50,   0.,  50., folder);

      //end light lep fake histograms

      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[0], "lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[1], "lepdeltaphi1"  , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaEta   , "lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[0]  , "lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname), 50,   0,   5, folder);
      const double drbins[] = {0.  , 1.5 , 2.  , 2.5 , 2.8 ,
                               3.  , 3.2 , 3.4 , 3.6 , 3.8 ,
                               4.  , 4.3 , 4.6 , 4.9 ,
                               6.};
      const int ndrbins = (sizeof(drbins)/sizeof(*drbins) - 1);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[1]        , "lepdeltar1"        , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[2]        , "lepdeltar2"        , Form("%s: Lepton DeltaR"    ,dirname), ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelRVsOneEta[0]  , "lepdelrvsoneeta"   , Form("%s: LepDelRVsOneEta"  ,dirname),  15, 0., 2.5, ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelRVsOneEta[1]  , "lepdelrvsoneeta1"  , Form("%s: LepDelRVsOneEta"  ,dirname),  15, 0., 2.5, ndrbins, drbins, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelPhiVsOneEta[0], "lepdelphivsoneeta" , Form("%s: LepDelPhiVsOneEta",dirname), 15, 0., 2.5, 15, 0., 3.5, folder);
      Utilities::BookH2F(fEventHist[i]->hLepDelPhiVsOneEta[1], "lepdelphivsoneeta1", Form("%s: LepDelPhiVsOneEta",dirname), 15, 0., 2.5, 15, 0., 3.5, folder);
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

      Utilities::BookH1F(fEventHist[i]->hLepPtOverM        , "lepptoverm"       , Form("%s: Lepton Pt / M"       ,dirname),  40,   0,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[0]    , "deltaalpham0"     , Form("%s: Delta Alpha Mass 0"  ,dirname), 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[1]    , "deltaalpham1"     , Form("%s: Delta Alpha Mass 1"  ,dirname), 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaMColM[0], "deltaalphamcolm0" , Form("%s: MCol - Alpha Mass 0" ,dirname),  50, -50,  50, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaMColM[1], "deltaalphamcolm1" , Form("%s: MCol - Alpha Mass 1" ,dirname),  50, -50,  50, folder);

      Utilities::BookH1F(fEventHist[i]->hMetDeltaPhi       , "metdeltaphi"      , Form("%s: Met Lep Delta Phi"       ,dirname),  50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepOneDeltaPhi    , "leponedeltaphi"   , Form("%s: Lep One vs Sys Delta Phi",dirname),  50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepTwoDeltaPhi    , "leptwodeltaphi"   , Form("%s: Lep Two vs Sys Delta Phi",dirname),  50,   0,   5, folder);


      Utilities::BookH1F(fEventHist[i]->hMTOne     , "mtone"         , Form("%s: MTOne"         ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo     , "mttwo"         , Form("%s: MTTwo"         ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep     , "mtlep"         , Form("%s: MTLep"         ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTOneOverM, "mtoneoverm"    , Form("%s: MTOneOverM"    ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwoOverM, "mttwooverm"    , Form("%s: MTTwoOverM"    ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH2F(fEventHist[i]->hMTTwoVsOne, "mttwovsone"    , Form("%s: MTTwoVsOne"    ,dirname)  ,  30, 0.,   120., 30, 0., 120., folder);

      Utilities::BookH1F(fEventHist[i]->hPTauVisFrac    , "ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  , 50,0.,  1.5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDot         , "lepdot"         , Form("%s: sqrt(p1 dot p2)"          ,dirname)  ,100,0.,  200, folder);

      for(int mode = 0; mode < 3; ++mode) {
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePx[mode], Form("leponeprimepx%i", mode), Form("%s: l1 px" , dirname),  50, -100., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePx[mode], Form("leptwoprimepx%i", mode), Form("%s: l2 px" , dirname),  50, -100., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePx   [mode], Form("metprimepx%i"   , mode), Form("%s: met px", dirname),  50,    0., 100., folder);
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePy[mode], Form("leponeprimepy%i", mode), Form("%s: l1 py" , dirname),  50, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePy[mode], Form("leptwoprimepy%i", mode), Form("%s: l2 py" , dirname),  50, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePy   [mode], Form("metprimepy%i"   , mode), Form("%s: met py", dirname),  50, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimePz[mode], Form("leponeprimepz%i", mode), Form("%s: l1 pz" , dirname), 100, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePz[mode], Form("leptwoprimepz%i", mode), Form("%s: l2 pz" , dirname), 100, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimePz   [mode], Form("metprimepz%i"   , mode), Form("%s: met pz", dirname), 100, -200., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepOnePrimeE [mode], Form("leponeprimee%i" , mode), Form("%s: l1 e " , dirname), 100,    0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoPrimeE [mode], Form("leptwoprimee%i" , mode), Form("%s: l2 e " , dirname), 100,    0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hMetPrimeE    [mode], Form("metprimee%i"    , mode), Form("%s: met e ", dirname), 100,    0., 200., folder);
      }

      for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig.names_[j].Data()) ,
                           fMVAConfig.NBins(j), fMVAConfig.Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig.names_[j].Data()), 3000, -1.,  2., folder);
        Utilities::BookH1F(fEventHist[i]->hMVATrain[j], Form("mvatrain%i",j), Form("%s: %s MVA (train)" ,dirname, fMVAConfig.names_[j].Data()),  100, -3.,  2., folder);
        Utilities::BookH1F(fEventHist[i]->hMVATest[j] , Form("mvatest%i",j) , Form("%s: %s MVA (test)"  ,dirname, fMVAConfig.names_[j].Data()),  100, -3.,  2., folder);
      }
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookPhotonHistograms() {
  if(fDYTesting) return;
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"photon_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[1*fn + i] = folder;
      fDirectories[1*fn + i]->cd();
      fPhotonHist[i] = new PhotonHist_t;
      Utilities::BookH1F(fPhotonHist[i]->hPz , "pz"      , Form("%s: Pz"      ,dirname)  , 100,-200, 200, folder);
      Utilities::BookH1F(fPhotonHist[i]->hPt , "pt"      , Form("%s: Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fPhotonHist[i]->hP  , "p"       , Form("%s: P"       ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fPhotonHist[i]->hEta, "eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10, folder);
      Utilities::BookH1F(fPhotonHist[i]->hPhi, "phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4, folder);
      Utilities::BookH1F(fPhotonHist[i]->hMVA, "mva"     , Form("%s: MVA"     ,dirname)  , 300,  -1,   2, folder);
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookLepHistograms() {
  // if(fDYTesting) return;
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i] = folder;
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;

      //for correcting jet -> tau scale factors
      const double pts[] = {0.  , 20. , 25. , 30. , 35. ,
                            40. , 45. , 50. , 55. , 65. ,
                            80. , 100. , 120.,
                            200.};
      const int nbins_pt = sizeof(pts)/sizeof(*pts) - 1;

      const double rbins[] = {0.  , 1.  , 1.5 , 2.  , 2.5,
                              3.  , 3.5 ,
                              5.};
      const int nrbins = sizeof(rbins)/sizeof(*rbins) - 1;
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettauoneptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        Utilities::BookH2F(fLepHist[i]->hJetTauOnePtVsR[dmregion], name_r.Data() , Form("%s: Delta R Vs One Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins, folder);
        TString name = "jettauonept";
        if(dmregion > 0) {name += "_"; name += dmregion;}
        Utilities::BookH1F(fLepHist[i]->hJetTauOnePt[dmregion], name.Data() , Form("%s: One Pt"   ,dirname)  , nbins_pt, pts, folder);
      }
      Utilities::BookH1F(fLepHist[i]->hJetTauOneR  , "jettauoner" , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);

      const double pts_qcd[] = {0.  , 20. , 25. , 30. , 35. ,
                                40. , 50. , 60. ,
                                200.};
      const int nbins_pt_qcd = sizeof(pts_qcd)/sizeof(*pts_qcd) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtQCD[0], "jettauoneptqcd_0" , Form("%s: One Pt"   ,dirname)  , nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtQCD[1], "jettauoneptqcd_1" , Form("%s: One Pt"   ,dirname)  , nbins_pt_qcd, pts_qcd, folder);

      Utilities::BookH1F(fLepHist[i]->hJetTauOneEta, "jettauoneeta", Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      const double metbins[] = {0., 0.5, 0.9, 1.2, 1.4, 1.6, 1.8, 2., 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 4., 5.};
      const int nmetbins = sizeof(metbins)/sizeof(*metbins) - 1;
      for(int idm = -1; idm < 4; ++idm) {
        TString nm = (idm > -1) ? Form("jettauonemetdeltaphi_%i", idm) : "jettauonemetdeltaphi";
        Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDeltaPhi[idm+1], nm.Data(), Form("%s: OneMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      }
      const double metqcdbins[] = {0., 0.3, 0.6, 0.9, 1.3, 1.8, 2.5, 3.2, 4.};
      const int nmetqcdbins = sizeof(metqcdbins)/sizeof(*metqcdbins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiQCD[0], "jettauonemetdphiqcd0", Form("%s: OneMetDeltaPhi"  ,dirname), nmetqcdbins, metqcdbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiQCD[1], "jettauonemetdphiqcd1", Form("%s: OneMetDeltaPhi"  ,dirname), nmetqcdbins, metqcdbins, folder);
      const double isobins[] = {0., 0.02, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.40, 0.50};
      const int nisobins = sizeof(isobins)/sizeof(*isobins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneRelIso[0], "jettauonereliso_0", Form("%s: OneRelIso"  ,dirname), nisobins, isobins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOneRelIso[1], "jettauonereliso_1", Form("%s: OneRelIso"  ,dirname), nisobins, isobins, folder);

      //j-->tau compositions
      const double mtbins[] = {0., 10., 20., 30., 40., 50., 65., 90., 150.};
      const int    nmtbins  = sizeof(mtbins) / sizeof(*mtbins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiComp, "jettauonemetdphicomp", Form("%s: OneMetDeltaPhi"  ,dirname), nmetbins    , metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDPhiComp, "jettautwometdphicomp", Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins    , metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtComp     , "jettauoneptcomp"     , Form("%s: OnePt"           ,dirname), nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoPtComp     , "jettautwoptcomp"     , Form("%s: TwoPt"           ,dirname), nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauMTOneComp     , "jettaumtonecomp"     , Form("%s: MTOne"           ,dirname), nmtbins     , mtbins , folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauMTTwoComp     , "jettaumttwocomp"     , Form("%s: MTTwo"           ,dirname), nmtbins     , mtbins , folder);

      //end j-->tau histograms

      for(int j = 0; j < 12; ++j) { //lepton pT with/without various weights
        TString name = "onept"; if(j > 0) name += j;
        Utilities::BookH1F(fLepHist[i]->hOnePt[j], name.Data(), Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      }
      Utilities::BookH1F(fLepHist[i]->hOnePz          , "onepz"            , Form("%s: Pz"           ,dirname), 100, -100,  100, folder);
      Utilities::BookH1F(fLepHist[i]->hOneEta         , "oneeta"           , Form("%s: Eta"          ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePhi         , "onephi"           , Form("%s: Phi"          ,dirname),  80,   -4,    4, folder);
      Utilities::BookH1F(fLepHist[i]->hOneD0          , "oned0"            , Form("%s: D0"           ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDXY         , "onedxy"           , Form("%s: DXY"          ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDZ          , "onedz"            , Form("%s: DZ"           ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneIso         , "oneiso"           , Form("%s: Iso"          ,dirname),  50,    0,    5, folder);
      Utilities::BookH1D(fLepHist[i]->hOneID1         , "oneid1"           , Form("%s: ID1"          ,dirname),  80,   -1,   79, folder);
      Utilities::BookH1D(fLepHist[i]->hOneID2         , "oneid2"           , Form("%s: ID2"          ,dirname),  80,   -1,   79, folder);
      Utilities::BookH1F(fLepHist[i]->hOneRelIso      , "onereliso"        , Form("%s: Iso / Pt"     ,dirname),  50,    0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hOneFlavor      , "oneflavor"        , Form("%s: Flavor"       ,dirname),  20,    0,   20, folder);
      Utilities::BookH1D(fLepHist[i]->hOneGenFlavor   , "onegenflavor      ", Form("%s: Gen Flavor"  ,dirname),  40,    0,   40, folder);
      Utilities::BookH1D(fLepHist[i]->hOneQ           , "oneq"             , Form("%s: Q"            ,dirname),   5,   -2,    2, folder);
      Utilities::BookH1D(fLepHist[i]->hOneTrigger     , "onetrigger"       , Form("%s: Trigger"      ,dirname),  10,    0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hOneWeight      , "oneweight"        , Form("%s: Weight"       ,dirname),  50,    0,    2, folder);
      Utilities::BookH1F(fLepHist[i]->hOneTrigWeight  , "onetrigweight"    , Form("%s: Trig Weight"  ,dirname),  50,    0,    2, folder);
      Utilities::BookH1D(fLepHist[i]->hOneWeight1Group, "oneweight1group"  , Form("%s: Weight group" ,dirname),  50,    0,   50, folder);
      Utilities::BookH1D(fLepHist[i]->hOneWeight2Group, "oneweight2group"  , Form("%s: Weight group" ,dirname),  50,    0,   50, folder);
      //Gen Info
      Utilities::BookH1F(fLepHist[i]->hOneGenPt       , "onegenpt"         , Form("%s: Gen Pt"       ,dirname), 100,    0,  150, folder);
      Utilities::BookH1F(fLepHist[i]->hOneGenE        , "onegene"          , Form("%s: Gen E"        ,dirname), 100,    0,  150, folder);
      Utilities::BookH1F(fLepHist[i]->hOneGenEta      , "onegeneta"        , Form("%s: Gen Eta"      ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneMetDeltaPhi , "onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,    0,    5, folder);

      // Lepton two info //
      Utilities::BookH1F(fLepHist[i]->hTwoPz          , "twopz"            , Form("%s: Pz"           ,dirname), 100, -100,  100, folder);
      for(int j = 0; j < 12; ++j) {
        TString name = "twopt"; if(j > 0) name += j;
        Utilities::BookH1F(fLepHist[i]->hTwoPt[j]     , name.Data(), Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      }
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettautwoptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        Utilities::BookH2F(fLepHist[i]->hJetTauTwoPtVsR[dmregion], name_r.Data() , Form("%s: Delta R Vs Two Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins, folder);
        TString name = "jettautwopt";
        if(dmregion > 0) {name += "_"; name += dmregion;}
        Utilities::BookH1F(fLepHist[i]->hJetTauTwoPt[dmregion], name.Data() , Form("%s: Two Pt"   ,dirname)  , nbins_pt, pts, folder);
      }
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoR[0]     , "jettautwor0"     , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoR[1]     , "jettautwor"      , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEta[0]   , "jettautwoeta0"   , Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEta[1]   , "jettautwoeta"    , Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEtaQCD[0], "jettautwoetaqcd0", Form("%s: |Eta|"  ,dirname)  , 10,    0, 2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEtaQCD[1], "jettautwoetaqcd1", Form("%s: |Eta|"  ,dirname)  , 10,    0, 2.5, folder);

      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDeltaPhi[0], "jettautwometdeltaphi0", Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDeltaPhi[1], "jettautwometdeltaphi" , Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoRelIso[0], "jettautworeliso_0", Form("%s: TwoRelIso"  ,dirname), nisobins, isobins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoRelIso[1], "jettautworeliso_1", Form("%s: TwoRelIso"  ,dirname), nisobins, isobins, folder);

      Utilities::BookH1F(fLepHist[i]->hTwoEta         , "twoeta"           , Form("%s: Eta"         ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPhi         , "twophi"           , Form("%s: Phi"         ,dirname),  80,  -4,    4, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoD0          , "twod0"            , Form("%s: D0"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXY         , "twodxy"           , Form("%s: DXY"         ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZ          , "twodz"            , Form("%s: DZ"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoIso         , "twoiso"           , Form("%s: Iso"         ,dirname),  50,   0,    5, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoID1         , "twoid1"           , Form("%s: ID1"         ,dirname),  80,  -1,   79, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoID2         , "twoid2"           , Form("%s: ID2"         ,dirname),  80,  -1,   79, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoID3         , "twoid3"           , Form("%s: ID3"         ,dirname),  80,  -1,   79, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoRelIso      , "tworeliso"        , Form("%s: Iso / Pt"    ,dirname),  50,   0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoFlavor      , "twoflavor"        , Form("%s: Flavor"      ,dirname),  20,   0,   20, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoGenFlavor   , "twogenflavor"     , Form("%s: Gen Flavor"  ,dirname),  40,   0,   40, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoQ           , "twoq"             , Form("%s: Q"           ,dirname),   5,  -2,    2, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoTrigger     , "twotrigger"       , Form("%s: Trigger"     ,dirname),  10,   0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoWeight      , "twoweight"        , Form("%s: Weight"      ,dirname),  50,   0,    2, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoTrigWeight  , "twotrigweight"    , Form("%s: Trig Weight" ,dirname),  50,   0,    2, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoWeight1Group, "twoweight1group"  , Form("%s: Weight group",dirname),  50,   0,   50, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoWeight2Group, "twoweight2group"  , Form("%s: Weight group",dirname),  50,   0,   50, folder);
      //Gen Info
      Utilities::BookH1F(fLepHist[i]->hTwoGenPt       , "twogenpt"         , Form("%s: Gen Pt"      ,dirname), 100,   0,  150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoGenE        , "twogene"          , Form("%s: Gen E"       ,dirname), 100,   0,  150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoGenEta      , "twogeneta"        , Form("%s: Gen Eta"     ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoMetDeltaPhi , "twometdeltaphi"   , Form("%s: Met Delta Phi",dirname), 50,   0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hPtDiff         , "ptdiff"           , Form("%s: 1 pT - 2 pT"  ,dirname), 100,  -80,   80, folder);
      Utilities::BookH1F(fLepHist[i]->hD0Diff         , "d0diff"           , Form("%s: 2 D0 - 1 D0"  ,dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDXYDiff        , "dxydiff"          , Form("%s: 2 DXY - 1 DXY",dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDZDiff         , "dzdiff"           , Form("%s: 2 DZ - 1 DZ"  ,dirname), 100,-0.08, 0.08, folder);

      //2D distributions
      Utilities::BookH2F(fLepHist[i]->hTwoPtVsOnePt   , "twoptvsonept"     , Form("%s: Two pT vs One pT", dirname), 30, 0, 150, 30, 0, 150, folder);
      const double tau_pts[] = {0., 25., 30., 35., 40.,
                                50.,
                                150.};
      const int nbins_tau_pt = sizeof(tau_pts) / sizeof(*tau_pts) - 1;
      Utilities::BookH2F(fLepHist[i]->hJetTauTwoPtVsOnePt, "jettautwoptvsonept", Form("%s: Two pT vs One pT", dirname), nbins_pt, pts, nbins_tau_pt, tau_pts, folder);
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookSystematicHistograms() {
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
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[4*fn + i] = folder;
      fDirectories[4*fn + i]->cd();
      fSystematicHist[i] = new SystematicHist_t;
      for(int sys = 0; sys < kMaxSystematics; ++sys) {
        Utilities::BookH1F(fSystematicHist[i]->hLepM        [sys], Form("lepm_%i"        , sys), Form("%s: LepM %i"                   , dirname, sys) , 280,  40, 180, folder);
        Utilities::BookH1F(fSystematicHist[i]->hLepPt       [sys], Form("leppt_%i"       , sys), Form("%s: LepPt %i"                  , dirname, sys) ,  30,   0, 100, folder);
        Utilities::BookH1F(fSystematicHist[i]->hOnePt       [sys], Form("onept_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
        Utilities::BookH1F(fSystematicHist[i]->hOneEta      [sys], Form("oneeta_%i"      , sys), Form("%s: Eta %i"                    , dirname, sys) ,  30,  -3,   3, folder);
        Utilities::BookH1F(fSystematicHist[i]->hTwoPt       [sys], Form("twopt_%i"       , sys), Form("%s: Pt %i"                     , dirname, sys) ,  40,   0, 100, folder);
        Utilities::BookH1F(fSystematicHist[i]->hTwoEta      [sys], Form("twoeta_%i"      , sys), Form("%s: Eta %i"                    , dirname, sys) ,  30,  -3,   3, folder);
        Utilities::BookH1F(fSystematicHist[i]->hWeightChange[sys], Form("weightchange_%i", sys), Form("%s: Relative weight change %i" , dirname, sys) ,  30, -2.,   2, folder);
        for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
          Utilities::BookH1D(fSystematicHist[i]->hMVA[j][sys], Form("mva%i_%i",j, sys)     , Form("%s: %s MVA %i" ,dirname, fMVAConfig.names_[j].Data(), sys),
                             fMVAConfig.NBins(j), fMVAConfig.Bins(j).data(), folder);
        }
      }
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      delete dirname;
      fTrees[i] = new TTree(Form("tree_%i",i),Form("CLFVHistMaker TTree %i",i));
      fTrees[i]->Branch("leponept",           &fTreeVars.leponept          );
      fTrees[i]->Branch("leponeptoverm",      &fTreeVars.leponeptoverm     );
      // fTrees[i]->Branch("leponem",            &fTreeVars.leponem           );
      // fTrees[i]->Branch("leponeeta",          &fTreeVars.leponeeta         );
      // fTrees[i]->Branch("leponed0",           &fTreeVars.leponed0          );
      // fTrees[i]->Branch("leponeiso",          &fTreeVars.leponeiso         );
      // fTrees[i]->Branch("leponeidone",        &fTreeVars.leponeidone       );
      // fTrees[i]->Branch("leponeidtwo",        &fTreeVars.leponeidtwo       );
      // fTrees[i]->Branch("leponeidthree",      &fTreeVars.leponeidthree     );
      fTrees[i]->Branch("leptwopt",           &fTreeVars.leptwopt          );
      fTrees[i]->Branch("leptwoptoverm",      &fTreeVars.leptwoptoverm     );
      // fTrees[i]->Branch("leptwom",            &fTreeVars.leptwom           );
      // fTrees[i]->Branch("leptwoeta",          &fTreeVars.leptwoeta         );
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
      // fTrees[i]->Branch("htdeltaphi",         &fTreeVars.htdeltaphi        );
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
      // fTrees[i]->Branch("pxivis",             &fTreeVars.pxivis            );
      // fTrees[i]->Branch("pxiinv",             &fTreeVars.pxiinv            );
      // fTrees[i]->Branch("ht",                 &fTreeVars.ht                );
      // fTrees[i]->Branch("htsum",              &fTreeVars.htsum             );
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
      fTrees[i]->Branch("type",               &fTreeVars.type              );

      fTrees[i]->Branch("jettotaunonclosure", &fTreeVars.jettotaunonclosure);
      fTrees[i]->Branch("zptup"             , &fTreeVars.zptup             );
      fTrees[i]->Branch("zptdown"           , &fTreeVars.zptdown           );

      //add MVA score branches
      for(unsigned index = 0; index < fMVAConfig.names_.size(); ++index) {
        fTrees[i]->Branch(Form("mva%i", index), &fMvaOutputs[index]);
      }
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::DeleteHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      if(fEventHist[i]) delete fEventHist[i];
      if(fLepHist  [i]) delete fLepHist  [i];
      if(fDoSystematics && fSysSets[i] && fSystematicHist[i]) delete fSystematicHist[i];
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
// Initialize branch structure of the input tree
void CLFVHistMaker::InitializeInputTree(TTree* tree) {
  tree->SetBranchAddress("nMuon"               , &nMuon               );
  tree->SetBranchAddress("Muon_pt"             , &Muon_pt             );
  tree->SetBranchAddress("Muon_eta"            , &Muon_eta            );
  tree->SetBranchAddress("Muon_phi"            , &Muon_phi            );
  tree->SetBranchAddress("Muon_charge"         , &Muon_charge         );
  tree->SetBranchAddress("Muon_looseId"        , &Muon_looseId        );
  tree->SetBranchAddress("Muon_mediumId"       , &Muon_mediumId       );
  tree->SetBranchAddress("Muon_tightId"        , &Muon_tightId        );
  tree->SetBranchAddress("Muon_pfRelIso04_all" , &Muon_pfRelIso04_all );
  tree->SetBranchAddress("Muon_dxy"            , &Muon_dxy            );
  tree->SetBranchAddress("Muon_dxyErr"         , &Muon_dxyErr         );
  tree->SetBranchAddress("Muon_dz"             , &Muon_dz             );
  tree->SetBranchAddress("Muon_dzErr"          , &Muon_dzErr          );
  tree->SetBranchAddress("Muon_nTrackerLayers" , &Muon_nTrackerLayers );
  tree->SetBranchAddress("Muon_TaggedAsRemoved", &Muon_TaggedAsRemoved);
  tree->SetBranchAddress("Muon_genPartFlav"    , &Muon_genPartFlav    );
  tree->SetBranchAddress("Muon_genPartIdx"     , &Muon_genPartIdx     );

  tree->SetBranchAddress("nElectron"                     , &nElectron                     );
  tree->SetBranchAddress("Electron_pt"                   , &Electron_pt                   );
  tree->SetBranchAddress("Electron_eta"                  , &Electron_eta                  );
  tree->SetBranchAddress("Electron_phi"                  , &Electron_phi                  );
  tree->SetBranchAddress("Electron_charge"               , &Electron_charge               );
  tree->SetBranchAddress("Electron_deltaEtaSC"           , &Electron_deltaEtaSC           );
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WPL"   , &Electron_mvaFall17V2Iso_WPL   );
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WP90"  , &Electron_mvaFall17V2Iso_WP90  );
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WP80"  , &Electron_mvaFall17V2Iso_WP80  );
  tree->SetBranchAddress("Electron_mvaFall17V2noIso_WPL" , &Electron_mvaFall17V2noIso_WPL );
  tree->SetBranchAddress("Electron_mvaFall17V2noIso_WP90", &Electron_mvaFall17V2noIso_WP90);
  tree->SetBranchAddress("Electron_mvaFall17V2noIso_WP80", &Electron_mvaFall17V2noIso_WP80);
  tree->SetBranchAddress("Electron_pfRelIso03_all"       , &Electron_pfRelIso03_all       );
  tree->SetBranchAddress("Electron_eCorr"                , &Electron_eCorr                );
  tree->SetBranchAddress("Electron_dxy"                  , &Electron_dxy                  );
  tree->SetBranchAddress("Electron_dxyErr"               , &Electron_dxyErr               );
  tree->SetBranchAddress("Electron_dz"                   , &Electron_dz                   );
  tree->SetBranchAddress("Electron_dzErr"                , &Electron_dzErr                );
  tree->SetBranchAddress("Electron_TaggedAsRemoved"      , &Electron_TaggedAsRemoved      );
  tree->SetBranchAddress("Electron_genPartFlav"          , &Electron_genPartFlav          );
  tree->SetBranchAddress("Electron_genPartIdx"           , &Electron_genPartIdx           );

  tree->SetBranchAddress("nTau"                          , &nTau                          );
  tree->SetBranchAddress("Tau_pt"                        , &Tau_pt                        );
  tree->SetBranchAddress("Tau_eta"                       , &Tau_eta                       );
  tree->SetBranchAddress("Tau_phi"                       , &Tau_phi                       );
  tree->SetBranchAddress("Tau_mass"                      , &Tau_mass                      );
  tree->SetBranchAddress("Tau_charge"                    , &Tau_charge                    );
  tree->SetBranchAddress("Tau_idDeepTau2017v2p1VSe"      , &Tau_idDeepTau2017v2p1VSe      );
  tree->SetBranchAddress("Tau_idDeepTau2017v2p1VSmu"     , &Tau_idDeepTau2017v2p1VSmu     );
  tree->SetBranchAddress("Tau_idDeepTau2017v2p1VSjet"    , &Tau_idDeepTau2017v2p1VSjet    );
  tree->SetBranchAddress("Tau_idAntiEle"                 , &Tau_idAntiEle                 );
  tree->SetBranchAddress("Tau_idAntiEle2018"             , &Tau_idAntiEle2018             );
  tree->SetBranchAddress("Tau_idAntiMu"                  , &Tau_idAntiMu                  );
  tree->SetBranchAddress("Tau_idMVAnewDM2017v2"          , &Tau_idAntiJet                 );
  tree->SetBranchAddress("Tau_decayMode"                 , &Tau_decayMode                 );
  tree->SetBranchAddress("Tau_idDecayMode"               , &Tau_idDecayMode               );
  tree->SetBranchAddress("Tau_idDecayModeNewDMs"         , &Tau_idDecayModeNewDMs         );
  tree->SetBranchAddress("Tau_dxy"                       , &Tau_dxy                       );
  // tree->SetBranchAddress("Tau_dxyErr"                    , &Tau_dxyErr                    );
  tree->SetBranchAddress("Tau_dz"                        , &Tau_dz                        );
  // tree->SetBranchAddress("Tau_dzErr"                     , &Tau_dzErr                     );
  tree->SetBranchAddress("Tau_genPartFlav"               , &Tau_genPartFlav               );
  tree->SetBranchAddress("Tau_genPartIdx"                , &Tau_genPartIdx                );

  tree->SetBranchAddress("nJet"                          , &nJet                          );
  tree->SetBranchAddress("Jet_pt"                        , &Jet_pt                        );
  tree->SetBranchAddress("Jet_eta"                       , &Jet_eta                       );
  tree->SetBranchAddress("Jet_phi"                       , &Jet_phi                       );
  tree->SetBranchAddress("Jet_mass"                      , &Jet_mass                      );
  // tree->SetBranchAddress("Jet_charge"                    , &Jet_charge                    );
  tree->SetBranchAddress("Jet_jetId"                     , &Jet_jetId                     );
  tree->SetBranchAddress("Jet_puId"                      , &Jet_puId                      );
  // tree->SetBranchAddress("Jet_pfRelIso03_all"            , &Jet_pfRelIso03_al           l );
  tree->SetBranchAddress("Jet_btagDeepB"                 , &Jet_btagDeepB                 );
  tree->SetBranchAddress("Jet_btagCMVA"                  , &Jet_btagCMVA                  );
  // tree->SetBranchAddress("Jet_eCorr"                     , &Jet_eCorr                     );
  // tree->SetBranchAddress("Jet_dxy"                       , &Jet_dxy                       );
  // tree->SetBranchAddress("Jet_dxyErr"                    , &Jet_dxyErr                    );
  // tree->SetBranchAddress("Jet_dz"                        , &Jet_dz                        );
  // tree->SetBranchAddress("Jet_dzErr"                     , &Jet_dzErr                     );
  tree->SetBranchAddress("Jet_partonFlavour"             , &Jet_partonFlavour             );
  tree->SetBranchAddress("Jet_TaggedAsRemovedByMuon"     , &Jet_TaggedAsRemovedByMuon     );
  tree->SetBranchAddress("Jet_TaggedAsRemovedByElectron" , &Jet_TaggedAsRemovedByElectron );

  tree->SetBranchAddress("nTrigObj"                      , &nTrigObj                      );
  tree->SetBranchAddress("TrigObj_pt"                    , &TrigObj_pt                    );
  tree->SetBranchAddress("TrigObj_eta"                   , &TrigObj_eta                   );
  tree->SetBranchAddress("TrigObj_phi"                   , &TrigObj_phi                   );
  tree->SetBranchAddress("TrigObj_id"                    , &TrigObj_id                    );
  tree->SetBranchAddress("TrigObj_filterBits"            , &TrigObj_filterBits            );

  //Z information
  //FIXME: These fields should be for a general Z, not channel specific
  tree->SetBranchAddress("GenZll_pt"                     , &zPt                           );
  tree->SetBranchAddress("GenZll_mass"                   , &zMass                         );
  tree->SetBranchAddress("GenZll_LepOne_pdgId"           , &zLepOneID                     );
  tree->SetBranchAddress("GenZll_LepTwo_pdgId"           , &zLepTwoID                     );
  tree->SetBranchAddress("GenZll_LepOne_pt"              , &zLepOnePt                     );
  tree->SetBranchAddress("GenZll_LepTwo_pt"              , &zLepTwoPt                     );
  tree->SetBranchAddress("GenZll_LepOne_eta"             , &zLepOneEta                    );
  tree->SetBranchAddress("GenZll_LepTwo_eta"             , &zLepTwoEta                    );

  // if(tree->GetBranch("GenZMuE_pt")) {
  //   tree->SetBranchAddress("GenZMuE_pt"                    , &zPt                           );
  //   tree->SetBranchAddress("GenZMuE_mass"                  , &zMass                         );
  // } else if(tree->GetBranch("GenZTauTau_pt")) {
  //   tree->SetBranchAddress("GenZTauTau_pt"                 , &zPt                           );
  //   tree->SetBranchAddress("GenZTauTau_mass"               , &zMass                         );
  // }

  //Event information
  tree->SetBranchAddress("genWeight"                     , &genWeight                     );
  tree->SetBranchAddress("HT"                            , &ht                            );
  tree->SetBranchAddress("PuppiMET_pt"                   , &puppMET                       );
  tree->SetBranchAddress("PuppiMET_phi"                  , &puppMETphi                    );
  // tree->SetBranchAddress("Pileup_nPU"                    , &nPUAdded                      );
  tree->SetBranchAddress("Pileup_nTrueInt"               , &nPU                           );
  tree->SetBranchAddress("PV_npvsGood"                   , &nPV                           ) ;
}


//--------------------------------------------------------------------------------------------------------------
// Determine and apply the global event weights (i.e. not selection specific)
void CLFVHistMaker::InitializeEventWeights() {
  //reset to-be-defined weights
  eventWeight = 1.f;

  leptonOneWeight1      = 1.; leptonTwoWeight1      = 1.;
  leptonOneWeight1_up   = 1.; leptonTwoWeight1_up   = 1.;
  leptonOneWeight1_down = 1.; leptonTwoWeight1_down = 1.;
  leptonOneWeight1_bin  = 0 ; leptonTwoWeight1_bin  = 0 ;

  leptonOneWeight2      = 1.; leptonTwoWeight2      = 1.;
  leptonOneWeight2_up   = 1.; leptonTwoWeight2_up   = 1.;
  leptonOneWeight2_down = 1.; leptonTwoWeight2_down = 1.;
  leptonOneWeight2_bin  = 0 ; leptonTwoWeight2_bin  = 0 ;

  leptonOneTrigWeight   = 1.; leptonTwoTrigWeight   = 1.;
  leptonOneFired        = 0 ; leptonTwoFired        = 0 ;

  leptonOnePtSF = 1.; leptonTwoPtSF = 1.;

  tauES = 1.; tauES_up = 1.; tauES_down = 1.;
  eleES = 1.; eleES_up = 1.; eleES_down = 1.;

  ////////////////////////////////////////////////////////////////////
  //   Generator/Embedding Weight
  ////////////////////////////////////////////////////////////////////

  embeddingWeight = 1.f; embeddingUnfoldingWeight = 1.f;
  if(fIsEmbed) {
    genWeight = std::fabs(genWeight);
    if(genWeight > 1.) { //undefined generation weight (must be < 1) --> remove the event
      genWeight = 0.f;
    } else if(genWeight == 1.f) {
      std::cout << "!!! Warning! Entry = " << fentry << ": Unit input embedding weight = " << genWeight
                << " in event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                << std::endl;
      genWeight = 0.f;
    } else { //move the weight value to the embeddingWeight variable
      embeddingWeight = genWeight; //move the embedding weight from genWeight to embeddingWeight
      genWeight = 1.;
    }
    embeddingUnfoldingWeight = fEmbeddingWeight.UnfoldingWeight(zLepOnePt, zLepOneEta, zLepTwoPt, zLepTwoEta, fYear);
    eventWeight *= embeddingUnfoldingWeight;
  } else if(!fIsData) {
    genWeight = (genWeight < 0.) ? -1.f : 1.f;
  } else { //Data
    genWeight = 1.f;
  }

  ////////////////////////////////////////////////////////////////////
  //   Z info + re-weight
  ////////////////////////////////////////////////////////////////////

  //Z pT/mass info (DY and Z signals only)
  zPtWeight = 1.f;
  if(fIsDY && !fIsEmbed) {
    //re-weight the Z pt vs mass spectrum
    zPtWeight = fZPtWeight.GetWeight(fYear, zPt, zMass, false /*Use Gen level weights*/, zPtWeightUp, zPtWeightDown, zPtWeightSys);
    eventWeight *= zPtWeight;
    if(fVerbose > 0) std::cout << " For Z pT = " << zPt << " and Mass = " << zMass << " using Data/MC weight " << zPtWeight
                               << "--> event weight = " << eventWeight << std::endl;

    //store generator level Z->ll lepton ids
    nGenHardElectrons = 0; nGenHardMuons = 0; nGenHardTaus = 0;
    //lepton one
    if(std::abs(zLepOneID) == 11)      ++nGenHardElectrons;
    else if(std::abs(zLepOneID) == 13) ++nGenHardMuons;
    else if(std::abs(zLepOneID) == 15) ++nGenHardTaus;
    //lepton two
    if(std::abs(zLepTwoID) == 11)      ++nGenHardElectrons;
    else if(std::abs(zLepTwoID) == 13) ++nGenHardMuons;
    else if(std::abs(zLepTwoID) == 15) ++nGenHardTaus;
  } else {
    zPtWeight = 1.; zPtWeightUp = 1.; zPtWeightDown = 1.; zPtWeightSys = 1.;
  }

  ////////////////////////////////////////////////////////////////////
  //   Trigger weights
  ////////////////////////////////////////////////////////////////////

  //Trigger thresholds (mu, ele): 2016 = (24, 27); 2017 = (27, 32); 2018 = (24, 32)
  //Use 1 GeV/c above muon threshold and 3 GeV/c above electron threshold (except for 2016)
  muon_trig_pt_ = 25.; electron_trig_pt_ = 35.;
  if(fYear == 2017) muon_trig_pt_ = 28.;
  if(fYear == 2016) electron_trig_pt_ = 29.; //2 GeV/c above threshold, since better behaved near threshold

  MatchTriggers();
  if(!fIsData) ApplyTriggerWeights();

  ////////////////////////////////////////////////////////////////////
  //   Pileup weights
  ////////////////////////////////////////////////////////////////////

  //pileup weights
  if(fIsEmbed || fIsData) { //pile-up is from data
    puWeight   = 1.;
    zPtWeight  = 1.f;
    btagWeight = 1.f;
  } else {
    if(fRemovePUWeights > 0) {
      puWeight = 1.f;
      //replace weight
      if(fRemovePUWeights > 1) {
        puWeight = fPUWeight.GetWeight(nPU, fYear);
      }
    }
    eventWeight *= puWeight;
  }

  ////////////////////////////////////////////////////////////////////
  //   Pre-fire weights
  ////////////////////////////////////////////////////////////////////

  prefireWeight = 1.f;
  if(fUsePrefireWeights > 0 && !fIsData && !fIsEmbed) {
    prefireWeight = fPrefireWeight.GetWeight(fYear, nJets20, jetsPt, jetsEta);
  }
  eventWeight *= prefireWeight;

  ////////////////////////////////////////////////////////////////////
  //   Jet PU ID weights
  ////////////////////////////////////////////////////////////////////

  jetPUIDWeight = 1.f;
  if(fUseJetPUIDWeights > 0 && !fIsData && !fIsEmbed) {
    jetPUIDWeight = fJetPUWeight.GetWeight(fYear, nJets20, jetsPt, jetsEta, nJets20Rej, jetsRejPt, jetsRejEta);
  }
  eventWeight *= jetPUIDWeight;

  ////////////////////////////////////////////////////////////////////
  //   B-tag ID weights
  ////////////////////////////////////////////////////////////////////

  btagWeight = 1.f;
  if(fUseBTagWeights && !fIsData && !fIsEmbed) {
    //FIXME: Add a category with Tight ID b-tag in ee/mumu for Z->x+tau_h normalization?
    int wp = BTagWeight::kLooseBTag; //ee/mumu/emu use loose ID b-tag
    if(std::fabs(leptonTwoFlavor) == 15) wp = BTagWeight::kTightBTag;
    btagWeight = fBTagWeight.GetWeight(wp, fYear, nJets20, jetsPt, jetsEta, jetsFlavor, jetsBTag, btagWeightUp, btagWeightDown);
    eventWeight *= btagWeight;
    if((btagWeight - btagWeightUp) * (btagWeight - btagWeightDown) > 1.e-3 && fVerbose > 0) {
      std::cout << "!!! Warning: Entry " << fentry << " B-tag weight up/down are on the same side of the weight: Wt = "
                << btagWeight << " Up = " << btagWeightUp << " Down = " << btagWeightDown << std::endl;
    }
  } else { //don't apply b-tag weights
    btagWeight = 1.;
    btagWeightUp = 1.;
    btagWeightDown = 1.;
  }

  //No weights in data
  if(fIsData) {
    eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.; zPtWeightUp = 1.; zPtWeightDown = 1.;
    genTauFlavorWeight = 1.; jetPUIDWeight = 1.; btagWeight = 1.; embeddingWeight = 1.; embeddingUnfoldingWeight = 1.;
    for(int itrig = 0; itrig < nTrigModes; ++itrig) triggerWeights[itrig] = 1.;
  }
}

//--------------------------------------------------------------------------------------------------------------
// Count objects in the event (leptons, jets, etc.) and initialize base variables
void CLFVHistMaker::CountObjects() {
  nMuons = nMuon; nElectrons = nElectron; nTaus = nTau; nPhotons = 0;

  if(nElectron == 1 && nMuon == 1) { //e+mu selection
    leptonOneP4->SetPtEtaPhiM(Electron_pt[0], Electron_eta[0], Electron_phi[0], ELECMASS);
    leptonTwoP4->SetPtEtaPhiM(Muon_pt    [0], Muon_eta    [0], Muon_phi    [0], MUONMASS);
    leptonOneSCEta = Electron_eta[0] + Electron_deltaEtaSC[0];
    leptonTwoSCEta = Muon_eta[0];
    leptonOneDXY   = Electron_dxy[0];
    leptonTwoDXY   = Muon_dxy    [0];
    leptonOneDZ    = Electron_dz [0];
    leptonTwoDZ    = Muon_dz     [0];
    leptonOneIso   = Electron_pfRelIso03_all[0]*Electron_pt[0];
    leptonTwoIso   = Muon_pfRelIso04_all[0]*Muon_pt[0];
    leptonOneFlavor = -11*Electron_charge[0];
    leptonTwoFlavor = -13*Muon_charge[0];
    leptonOneGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwoGenFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonOneID1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOneID2       = 0;
    leptonTwoID1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonTwoID2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
  } else if(nElectron == 1 && nMuon == 0 && nTau == 1) { //e+tau
    leptonOneP4->SetPtEtaPhiM(Electron_pt[0], Electron_eta[0], Electron_phi[0], ELECMASS);
    leptonTwoP4->SetPtEtaPhiM(Tau_pt     [0], Tau_eta     [0], Tau_phi     [0], Tau_mass[0]);
    leptonOneSCEta = Electron_eta[0] + Electron_deltaEtaSC[0];
    leptonTwoSCEta = Tau_eta[0];
    leptonOneDXY   = Electron_dxy[0];
    leptonTwoDXY   = Tau_dxy     [0];
    leptonOneDZ    = Electron_dz [0];
    leptonTwoDZ    = Tau_dz      [0];
    leptonOneIso   = Electron_pfRelIso03_all[0]*Electron_pt[0];
    leptonTwoIso   = 0.f;
    leptonOneFlavor = -11*Electron_charge[0];
    leptonTwoFlavor = -15*Tau_charge[0];
    leptonOneGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwoGenFlavor = TauFlavorFromID(Tau_genPartFlav[0]);
    leptonOneID1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOneID2       = 0;
    leptonTwoID1       = Tau_idAntiEle[0];
    leptonTwoID2       = Tau_idAntiMu[0];
  } else if(nElectron == 0 && nMuon == 1 && nTau == 1) { //mu+tau
    leptonOneP4->SetPtEtaPhiM(Muon_pt    [0], Muon_eta    [0], Muon_phi    [0], MUONMASS);
    leptonTwoP4->SetPtEtaPhiM(Tau_pt     [0], Tau_eta     [0], Tau_phi     [0], Tau_mass[0]);
    leptonOneSCEta = Muon_eta[0];
    leptonTwoSCEta = Tau_eta [1];
    leptonOneDXY   = Muon_dxy    [0];
    leptonTwoDXY   = Tau_dxy     [0];
    leptonOneDZ    = Muon_dz     [0];
    leptonTwoDZ    = Tau_dz      [0];
    leptonOneIso   = Muon_pfRelIso04_all[0]*Muon_pt[0];
    leptonTwoIso   = 0.f;
    leptonOneFlavor = -13*Muon_charge[0];
    leptonTwoFlavor = -15*Tau_charge[0];
    leptonOneGenFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonTwoGenFlavor = TauFlavorFromID(Tau_genPartFlav[0]);
    leptonOneID1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOneID2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwoID1       = Tau_idAntiEle[0];
    leptonTwoID2       = Tau_idAntiMu[0];
  } else if(nElectron == 0 && nMuon == 2) { //mu+mu
    leptonOneP4->SetPtEtaPhiM(Muon_pt    [0], Muon_eta    [0], Muon_phi    [0], MUONMASS);
    leptonTwoP4->SetPtEtaPhiM(Muon_pt    [1], Muon_eta    [1], Muon_phi    [1], MUONMASS);
    leptonOneSCEta = Muon_eta[0];
    leptonTwoSCEta = Muon_eta[1];
    leptonOneDXY   = Muon_dxy    [0];
    leptonTwoDXY   = Muon_dxy    [1];
    leptonOneDZ    = Muon_dz     [0];
    leptonTwoDZ    = Muon_dz     [1];
    leptonOneIso   = Muon_pfRelIso04_all[0]*Muon_pt[0];
    leptonTwoIso   = Muon_pfRelIso04_all[1]*Muon_pt[1];
    leptonOneFlavor = -13*Muon_charge[0];
    leptonTwoFlavor = -13*Muon_charge[1];
    leptonOneGenFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonTwoGenFlavor = MuonFlavorFromID(Muon_genPartFlav[1]);
    leptonOneID1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOneID2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwoID1       = Muon_looseId[1] + Muon_mediumId[1] + Muon_tightId[1];
    leptonTwoID2       = MuonRelIsoID(Muon_pfRelIso04_all[1]);
  } else if(nElectron == 2 && nMuon == 0) { //e+e
    leptonOneP4->SetPtEtaPhiM(Electron_pt[0], Electron_eta[0], Electron_phi[0], ELECMASS);
    leptonTwoP4->SetPtEtaPhiM(Electron_pt[1], Electron_eta[1], Electron_phi[1], ELECMASS);
    leptonOneSCEta  = Electron_eta[0] + Electron_deltaEtaSC[0];
    leptonTwoSCEta  = Electron_eta[1] + Electron_deltaEtaSC[1];
    leptonOneDXY    = Electron_dxy[0];
    leptonTwoDXY    = Electron_dxy[1];
    leptonOneDZ     = Electron_dz [0];
    leptonTwoDZ     = Electron_dz [1];
    leptonOneIso    = Electron_pfRelIso03_all[0]*Electron_pt[0];
    leptonTwoIso    = Electron_pfRelIso03_all[1]*Electron_pt[1];
    leptonOneFlavor = -11*Electron_charge[0];
    leptonTwoFlavor = -11*Electron_charge[1];
    leptonOneGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwoGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[1]);
    leptonOneID1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOneID2       = 0;
    leptonTwoID1       = Electron_mvaFall17V2noIso_WPL[1] + Electron_mvaFall17V2noIso_WP90[1] + Electron_mvaFall17V2noIso_WP80[1];
    leptonTwoID2       = 0;
  }
  if(nTau > 0) { //store information of the leading pT tau
    tauDecayMode = Tau_decayMode[0];
    tauDeepAntiEle = Tau_idDeepTau2017v2p1VSe  [0];
    tauDeepAntiMu  = Tau_idDeepTau2017v2p1VSmu [0];
    tauDeepAntiJet = Tau_idDeepTau2017v2p1VSjet[0];
  }

  //Add MET information
  met = puppMET; //use PUPPI MET
  metPhi = puppMETphi;
  metCorr = 0.;
  metCorrPhi = 0.;

  //FIXME: Add actual Z daughter information
  if(zPt < 0 || zMass < 0) {
    zPt = ((*leptonOneP4) + (*leptonTwoP4)).Pt();
    zMass = ((*leptonOneP4) + (*leptonTwoP4)).M();
  }
  // zLepOnePt  = leptonOneP4->Pt();
  // zLepTwoPt  = leptonTwoP4->Pt();
  // zLepOneEta = leptonOneP4->Eta();
  // zLepTwoEta = leptonTwoP4->Eta();
  // zLepOneID  = leptonOneFlavor;
  // zLepTwoID  = leptonTwoFlavor;

  ///////////////////////////////////////////////////////
  // Add Jet information
  ///////////////////////////////////////////////////////

  CountJets();

  ///////////////////////////////////////////////////////
  //Set the data era
  ///////////////////////////////////////////////////////

  if(!fIsData && !fIsEmbed) { // use random numbers for MC not split by era
    mcEra = 0;
    if(fYear == 2016) {
      const double rand = fRnd->Uniform();
      const double frac_first = 19.72 / 35.86;
      mcEra = rand > frac_first; //0 if first, 1 if second
    } else if(fYear == 2018) {
      const double rand = fRnd->Uniform();
      const double frac_first = 8.98 / 59.59;
      mcEra = rand > frac_first; //0 if first, 1 if second
    }
  } else if(fIsEmbed) { //decide by run period in the file name
    if(fYear == 2016) { //split 2016 into B-F and GH
      if(TString(fChain->GetName()).Contains("-G") || TString(fChain->GetName()).Contains("-H")) mcEra = 1;
    } else if(fYear == 2018) { //split 2018 into ABC and D
      if(TString(fChain->GetName()).Contains("-D")) mcEra = 1;
    }
  } else if(fIsData) { //decide based on the run number
    if(fYear == 2016) mcEra = (runNumber > 278808); //split on period B-F and GH
    if(fYear == 2018) mcEra = (runNumber > 320065); //split on period ABC and D
  }

}

//--------------------------------------------------------------------------------------------------------------
// Count the jets in the event
void CLFVHistMaker::CountJets() {
  //reset counters
  nJets    = 0; nJets20   = 0; nJets20Rej = 0;
  nBJets   = 0; nBJetsM   = 0; nBJetsL    = 0;
  nBJets20 = 0; nBJets20M = 0; nBJets20L  = 0;

  //reset HT calculation
  ht = 0.f; htPhi = 0.f; htSum = 0.f;

  //jet ids
  const int min_jet_id    = 2;
  const int min_jet_pu_id = 6;
  const float min_jet_pt = 20.;

  TLorentzVector htLV;
  TLorentzVector jetLoop; //for checking delta R
  float jetptmax = -1; //track highest pt jet
  jetOneP4->SetPtEtaPhiM(0.,0.,0.,0.); //reset to no jet found
  for(UInt_t ijet = 0; ijet < nJet; ++ijet) {
    const float jetpt  = Jet_pt [ijet];
    const float jeteta = Jet_eta[ijet];
    if(jetpt < min_jet_pt) continue; //too low of jet pt
    if(Jet_jetId[ijet] < min_jet_id) continue; //bad jet
    if(Jet_TaggedAsRemovedByMuon[ijet]) continue; //overlapping a muon
    if(Jet_TaggedAsRemovedByElectron[ijet]) continue; //overlapping a electron

    //Restrict to less-forward jets
    if(std::fabs(jeteta) < 3.) {
      //check if jet fails PU ID or not
      if(jetpt < 50. && Jet_puId[ijet] < min_jet_pu_id) {
        jetsRejPt    [nJets20Rej] = jetpt;
        jetsRejEta   [nJets20Rej] = jeteta;
        ++nJets20Rej;
        continue;
      } else {
        ++nJets20;
        if(jetpt > 25.) ++nJets; //higher pT threshold counting
        jetLoop.SetPtEtaPhiM(jetpt, jeteta, Jet_phi[ijet], Jet_mass[ijet]);

        //add HT information
        htLV  += jetLoop;
        htSum += jetpt;

        //store the highest pT jet
        if(jetpt > jetptmax) {*jetOneP4 = jetLoop; jetptmax = jetpt;}

        int jetBTag = 0;
        //check if b-tagged, must have eta < 2.4
        if(std::fabs(jeteta) < 2.4) {
          if(Jet_btagDeepB[ijet] > fBTagWeight.BTagCut(BTagWeight::kLooseBTag, fYear)) {
            ++jetBTag; ++nBJets20L; if(jetpt > 25.) ++nBJetsL;
            if(Jet_btagDeepB[ijet] > fBTagWeight.BTagCut(BTagWeight::kMediumBTag, fYear)) {
              ++jetBTag; ++nBJets20M; if(jetpt > 25.) ++nBJetsM;
              if(Jet_btagDeepB[ijet] > fBTagWeight.BTagCut(BTagWeight::kTightBTag, fYear)) {
                ++jetBTag; ++nBJets20; if(jetpt > 25.) ++nBJets;
              }
            }
          }
        }
        //save info for jet b-tag efficiency
        jetsPt    [nJets20-1] = jetpt;
        jetsEta   [nJets20-1] = jeteta;
        jetsBTag  [nJets20-1] = jetBTag;
        jetsFlavor[nJets20-1] = Jet_partonFlavour[ijet];
      }
    }
  }

  //Record HT result
  ht    = htLV.Pt();
  htPhi = htLV.Phi();
}

//--------------------------------------------------------------------------------------------------------------
//selections: 1 = mutau, 2 = etau, 5 = emu, 9 = mumu, 18 = ee
void CLFVHistMaker::InitializeTreeVariables(Int_t selection) {
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
    fJetToTauBiases   [proc] = 1.f;
    fJetToTauComps    [proc] = 1.f;
    fJetToTauCompsUp  [proc] = 1.f;
    fJetToTauCompsDown[proc] = 1.f;
    fJetToTauMCWts    [proc] = 1.f;
    fJetToTauMCCorrs  [proc] = 1.f;
  }
  leptonOneD0 = std::sqrt(leptonOneDXY*leptonOneDXY + leptonOneDZ*leptonOneDZ);
  leptonTwoD0 = std::sqrt(leptonTwoDXY*leptonTwoDXY + leptonTwoDZ*leptonTwoDZ);
  fTreeVars.leponept  = leptonOneP4->Pt();
  fTreeVars.leponem   = leptonOneP4->M();
  fTreeVars.leponeeta = leptonOneP4->Eta();
  fTreeVars.leponed0  = leptonOneD0;
  fTreeVars.leponeiso = leptonOneIso;
  fTreeVars.leponereliso = leptonOneIso / leptonOneP4->Pt();
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
  fTreeVars.leptworeliso = leptonTwoIso / leptonTwoP4->Pt();
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
  fTreeVars.lepdeltaphi = std::fabs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  fTreeVars.lepdeltaeta = std::fabs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  //phi differences
  fTreeVars.htdeltaphi = std::fabs(lep.Phi() - htPhi);
  if(fTreeVars.htdeltaphi > M_PI)
    fTreeVars.htdeltaphi = std::fabs(2.*M_PI - fTreeVars.htdeltaphi);
  fTreeVars.metdeltaphi = std::fabs(lep.Phi() - metPhi);
  if(fTreeVars.metdeltaphi > M_PI)
    fTreeVars.metdeltaphi = std::fabs(2.*M_PI - fTreeVars.metdeltaphi);
  fTreeVars.leponedeltaphi = std::fabs(leptonOneP4->Phi() - lep.Phi());
  if(fTreeVars.leponedeltaphi > M_PI)
    fTreeVars.leponedeltaphi = std::fabs(2.*M_PI - fTreeVars.leponedeltaphi);
  fTreeVars.leptwodeltaphi = std::fabs(leptonTwoP4->Phi() - lep.Phi());
  if(fTreeVars.leptwodeltaphi > M_PI)
    fTreeVars.leptwodeltaphi = std::fabs(2.*M_PI - fTreeVars.leptwodeltaphi);
  fTreeVars.onemetdeltaphi = std::fabs(leptonOneP4->Phi() - metPhi);
  if(fTreeVars.onemetdeltaphi > M_PI)
    fTreeVars.onemetdeltaphi = std::fabs(2.*M_PI - fTreeVars.onemetdeltaphi);
  fTreeVars.twometdeltaphi = std::fabs(leptonTwoP4->Phi() - metPhi);
  if(fTreeVars.twometdeltaphi > M_PI)
    fTreeVars.twometdeltaphi = std::fabs(2.*M_PI - fTreeVars.twometdeltaphi);

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
  double hmass(HIGGSMASS), zmass(ZMASS), tmass(TAUMASS), lepdot(2.*((*leptonOneP4)*(*leptonTwoP4)));
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
    if(!std::isfinite(lepOnePrime.Px()) || !std::isfinite(lepOnePrime.Py()) || !std::isfinite(lepOnePrime.Pz()) || !std::isfinite(lepOnePrime.E())) {
      std::cout << "!!! Entry " << fentry << " has non-finite boosted lepton p4 components!\n ";
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
  if(fTreeVars.type < -0.5) { //doesn't change for a given file
    if(fIsSignal) fTreeVars.type = 0;
    else if(fIsData) fTreeVars.type = 1;
    else {
      TString filename = GetOutputName();
      if(filename.Contains("DY") || filename.Contains("Embed")) {
        if(fDYType == 2 || !filename.Contains("Embed")) fTreeVars.type = 2; //Z->ll
        else                                            fTreeVars.type = 3; //Z->tautau or inclusive or Z->tautau embedded
      } else if(filename.Contains("Top") || filename.Contains("ttbar")) fTreeVars.type = 4;
      else if(filename.Contains("Wlnu")) fTreeVars.type = 5; //W+(n)Jets
      else fTreeVars.type = 6; //Di-boson/whatever is leftover
    }
  }
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

//--------------------------------------------------------------------------------------------------------------
float CLFVHistMaker::GetTauFakeSF(int genFlavor) {
  float weight = 1.;
  switch(std::abs(genFlavor)) {
  case 15 : weight = 0.95; break;
  case 11 : weight = genTauFlavorWeight; break;
  case 13 : weight = genTauFlavorWeight; break;
  }
  //jets are given weight 1
  return weight;
}

//--------------------------------------------------------------------------------------------------------------
float CLFVHistMaker::CorrectMET(const int selection, const float met) {
  if(selection < 0) return met;
  float corrected = met;

  // switch(std::abs(selection)) {
  // case 0 : weight = (1.1 - 0.2/150.*met); break; // mu+tau
  // case 1 : weight = 1.; break; // e+tau
  // case 2 : weight = 1.; break; // e+mu
  // }
  return corrected;
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillEventHistogram(EventHist_t* Hist) {
  // Hist->hLumiSection         ->Fill(lumiSection        , genWeight*eventWeight)      ;
  // Hist->hTriggerStatus       ->Fill(triggerStatus      , genWeight*eventWeight)      ;
  Hist->hEventWeight         ->Fill(eventWeight             );
  Hist->hLogEventWeight      ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hFullEventWeightLum  ->Fill(std::fabs(fTreeVars.fulleventweightlum), genWeight*eventWeight);
  Hist->hLogFullEventWeightLum->Fill((std::fabs(fTreeVars.fulleventweightlum) > 1.e-10) ? std::log10(std::fabs(fTreeVars.fulleventweightlum)) : -999., genWeight*eventWeight);
  Hist->hGenWeight           ->Fill(genWeight                   , eventWeight          );
  Hist->hEmbeddingWeight     ->Fill(embeddingWeight             , eventWeight*genWeight);
  Hist->hLogEmbeddingWeight  ->Fill((embeddingWeight > 1.e-10) ? std::log10(embeddingWeight) : -999. , eventWeight*genWeight     );
  Hist->hEmbeddingUnfoldingWeight->Fill(embeddingUnfoldingWeight);
  if(fDoSystematics >= 0)
    Hist->hEventWeightMVA      ->Fill(fTreeVars.eventweightMVA);
  Hist->hJetToTauWeight      ->Fill(jetToTauWeight, genWeight*eventWeight);
  Hist->hJetToTauWeightGroup ->Fill(jetToTauWeightGroup, genWeight*eventWeight);
  Hist->hJetToTauWeightCorr  ->Fill((jetToTauWeight     > 0.) ? jetToTauWeightCorr/jetToTauWeight     : -1., genWeight*eventWeight);
  Hist->hJetToTauWeightBias  ->Fill((jetToTauWeightCorr > 0.) ? jetToTauWeightBias/jetToTauWeightCorr : -1., genWeight*eventWeight);
  for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
    Hist->hJetToTauComps[ji]->Fill(fJetToTauComps[ji]);
    Hist->hJetToTauWts  [ji]->Fill(fJetToTauWts  [ji]);
  }
  //quantify the effect of using composition by the size of the full weight / W+Jets weight alone
  const float wj_jtau_wt = fJetToTauWts[JetToTauComposition::kWJets]*fJetToTauCorrs[JetToTauComposition::kWJets]*fJetToTauBiases[JetToTauComposition::kWJets];
  Hist->hJetToTauCompEffect->Fill((wj_jtau_wt > 0.) ? jetToTauWeightBias / wj_jtau_wt : 0., eventWeight*genWeight);
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
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight)   ;
  Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
  Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  if(!fDYTesting && fDoSystematics >= 0) {
    // Hist->hGenTauFlavorWeight  ->Fill(genTauFlavorWeight );
    // Hist->hPhotonIDWeight      ->Fill(photonIDWeight );
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
  //   for(UInt_t jet = 0; jet < nJets20; ++jet) {
  //     // Hist->hJetsFlavor->Fill(jetsFlavor[jet], genWeight*eventWeight);
  //     int flavor = std::abs(jetsFlavor[jet]);
  //     if(flavor > 20) flavor = 1; //unknown parent defaults to light jet

  //     int index = 0; //light jets
  //     if(flavor == 4)      index = 1; // c-jet
  //     else if(flavor == 5) index = 2; // b-jet
  //     float jpt = jetsPt[jet], jeta = jetsEta[jet], wt = genWeight*eventWeight;
  //     if(btagWeight > 0.) wt /= btagWeight; //remove previous correction
  //     Hist->hJetsPtVsEta[index]->Fill(jeta, jpt, wt);
  //     if(jetsBTag[jet] > 0) { //Loose b-tag
  //       Hist->hBJetsLPtVsEta[index]->Fill(jeta, jpt, wt);
  //       if(jetsBTag[jet] > 1) { //Medium b-tag
  //         Hist->hBJetsMPtVsEta[index]->Fill(jeta, jpt, wt);
  //         if(jetsBTag[jet] > 2) { //Tight b-tag
  //           Hist->hBJetsPtVsEta[index]->Fill(jeta, jpt, wt);
  //         }
  //       }
  //     }
  //   }
  }
  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hMuonTriggerStatus   ->Fill(muonTriggerStatus, genWeight*eventWeight)   ;
  Hist->hNTriggered          ->Fill(leptonOneFired+leptonTwoFired, genWeight*eventWeight)   ;
  if(fDoSystematics >= 0) {
    Hist->hPuWeight            ->Fill(puWeight)     ;
    Hist->hJetPUIDWeight       ->Fill(jetPUIDWeight);
    Hist->hPrefireWeight       ->Fill(prefireWeight);
    // Hist->hTopPtWeight         ->Fill(topPtWeight)  ;
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
      Hist->hJetEta          ->Fill(jetOneP4->Eta()            , genWeight*eventWeight)   ;
    }
  }

  // if(!fDYTesting && fDoSystematics >= 0) {


    // if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
      // Hist->hJetM            ->Fill(jetOneP4->M()         , genWeight*eventWeight)   ;
      // Hist->hJetPhi          ->Fill(jetOneP4->Phi()       , genWeight*eventWeight)   ;
      // Hist->hJetBTag         ->Fill(jetOneBTag            , genWeight*eventWeight)   ;
      // Hist->hJetBMVA         ->Fill(jetOneBMVA            , genWeight*eventWeight)   ;
    // }

    // Hist->hPuppMet           ->Fill(puppMETC              , genWeight*eventWeight)      ;
    // Hist->hPFMet             ->Fill(pfMETC                , genWeight*eventWeight)      ;
    // Hist->hPFMetPhi          ->Fill(pfMETCphi             , genWeight*eventWeight)      ;
    // Hist->hPFCovMet00        ->Fill(pfMETCCov00           , genWeight*eventWeight)   ;
    // Hist->hPFCovMet01        ->Fill(pfMETCCov01           , genWeight*eventWeight)   ;
    // Hist->hPFCovMet11        ->Fill(pfMETCov11           , genWeight*eventWeight)   ;
    // Hist->hTrkMet              ->Fill(trkMET                , genWeight*eventWeight)      ;
    // Hist->hTrkMetPhi           ->Fill(trkMETphi             , genWeight*eventWeight)      ;
  // } //end if(!fDYTesting)

  if(fDoSystematics >= 0) {
    Hist->hHtSum             ->Fill(htSum              , genWeight*eventWeight)   ;
    Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;
    Hist->hHtPhi             ->Fill(htPhi              , genWeight*eventWeight)   ;
    Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
    Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
    Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
    Hist->hMTTwoVsOne        ->Fill(fTreeVars.mtone, fTreeVars.mttwo, eventWeight*genWeight);
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

  const double lepDelR   = std::fabs(leptonOneP4->DeltaR(*leptonTwoP4));
  const double lepDelPhi = std::fabs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  const double lepDelEta = std::fabs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  if(fDoSystematics >= 0) {
    TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;
    TLorentzVector svLepSys = (leptonOneSVP4 && leptonTwoSVP4) ? (*leptonOneSVP4) + (*leptonTwoSVP4) : TLorentzVector(0.,0.,0.,0.);
    TLorentzVector svSys    = (leptonOneSVP4 && leptonTwoSVP4 && photonP4) ? (*photonP4) + svLepSys  : TLorentzVector(0.,0.,0.,0.);

    double htDelPhi  = std::fabs(lepSys.Phi() - htPhi);
    // double jetDelPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? std::fabs(lepSys.DeltaPhi(*jetOneP4)) : -1.;
    if(htDelPhi > M_PI)
      htDelPhi = std::fabs(2.*M_PI - htDelPhi);
    double metDelPhi  = std::fabs(lepSys.Phi() - metPhi);
    if(metDelPhi > M_PI)
      metDelPhi = std::fabs(2.*M_PI - metDelPhi);
    double lepOneDelPhi  = std::fabs(lepSys.Phi() - leptonOneP4->Phi());
    if(lepOneDelPhi > M_PI)
      lepOneDelPhi = std::fabs(2.*M_PI - lepOneDelPhi);
    double lepTwoDelPhi  = std::fabs(lepSys.Phi() - leptonTwoP4->Phi());
    if(lepTwoDelPhi > M_PI)
      lepTwoDelPhi = std::fabs(2.*M_PI - lepTwoDelPhi);

    // Hist->hHtDeltaPhi   ->Fill(htDelPhi               ,eventWeight*genWeight);
    Hist->hMetDeltaPhi  ->Fill(metDelPhi              ,eventWeight*genWeight);
    // if(jetDelPhi >= 0.) Hist->hJetDeltaPhi  ->Fill(jetDelPhi,eventWeight*genWeight);
    Hist->hLepOneDeltaPhi->Fill(lepOneDelPhi          ,eventWeight*genWeight);
    Hist->hLepTwoDeltaPhi->Fill(lepTwoDelPhi          ,eventWeight*genWeight);

    //angles between leptons and leading jet
    // double lepOneJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonOneP4->DeltaR(*jetOneP4)            : -1.;
    // double lepOneJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? std::fabs(leptonOneP4->DeltaPhi(*jetOneP4))     : -1.;
    // double lepOneJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? std::fabs(leptonOneP4->Eta() - jetOneP4->Eta()) : -1.;
    // double lepTwoJetDeltaR   = (jetOneP4 && jetOneP4->Pt() > 0.) ? leptonTwoP4->DeltaR(*jetOneP4)            : -1.;
    // double lepTwoJetDeltaPhi = (jetOneP4 && jetOneP4->Pt() > 0.) ? std::fabs(leptonTwoP4->DeltaPhi(*jetOneP4))     : -1.;
    // double lepTwoJetDeltaEta = (jetOneP4 && jetOneP4->Pt() > 0.) ? std::fabs(leptonTwoP4->Eta() - jetOneP4->Eta()) : -1.;

    // float lepSVDelR   = -1.;
    // float lepSVDelPhi = -1.;
    // float lepSVDelEta = -1.;
    // float lepSVDelM   = -20.;
    // float lepSVDelPt  = -20.;
    // if(leptonOneSVP4 && leptonTwoSVP4
    //    && leptonOneSVP4->Pt() > 1.e-5 && leptonTwoSVP4->Pt() > 1.e-5) {
    //   lepSVDelR   = leptonOneSVP4->DeltaR(*leptonTwoSVP4)           ;
    //   lepSVDelPhi = std::fabs(leptonOneSVP4->DeltaPhi(*leptonTwoSVP4))    ;
    //   lepSVDelEta = std::fabs(leptonOneSVP4->Eta() - leptonTwoSVP4->Eta());
    //   lepSVDelM = svLepSys.M() - lepSys.M();
    //   lepSVDelPt = svLepSys.Pt() - lepSys.Pt();
    // }
  }
  //for removing or alternate DY reweighting weights
  double bareweight = (fIsDY && zPtWeight > 0.) ? eventWeight*genWeight/zPtWeight : eventWeight*genWeight; //no DY Z pT vs M weights
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

    Hist->hZLepOnePt    ->Fill(zLepOnePt , eventWeight*genWeight);
    Hist->hZLepTwoPt    ->Fill(zLepTwoPt , eventWeight*genWeight);
    Hist->hZLepOneEta   ->Fill(zLepOneEta, eventWeight*genWeight);
    Hist->hZLepTwoEta   ->Fill(zLepTwoEta, eventWeight*genWeight);

    const Float_t wt_noqcd     = (qcdWeight > 0.) ? eventWeight*genWeight/qcdWeight : eventWeight*genWeight;
    const Float_t wt_noqcdcl   = (qcdClosure > 0.) ? eventWeight*genWeight/qcdClosure : eventWeight*genWeight;
    const Float_t wt_noantiiso = (qcdIsoScale > 0.) ? eventWeight*genWeight/qcdIsoScale : eventWeight*genWeight;
    Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,eventWeight*genWeight);
    Hist->hLepDeltaPhi[1]->Fill(lepDelPhi             ,wt_noqcd);
    Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
    Hist->hLepDeltaR[0] ->Fill(lepDelR                ,eventWeight*genWeight);
    Hist->hLepDeltaR[1] ->Fill(lepDelR                ,wt_noqcd);
    Hist->hLepDeltaR[2] ->Fill(lepDelR                ,eventWeight*genWeight); //same binning as scale factor measurement
    Hist->hLepDelRVsOneEta[0]->Fill(std::fabs(fTreeVars.leponeeta), lepDelR, eventWeight*genWeight);
    Hist->hLepDelRVsOneEta[1]->Fill(std::fabs(fTreeVars.leponeeta), lepDelR, wt_noqcd);
    Hist->hLepDelPhiVsOneEta[0]->Fill(std::fabs(fTreeVars.leponeeta), lepDelPhi, eventWeight*genWeight);
    Hist->hLepDelPhiVsOneEta[1]->Fill(std::fabs(fTreeVars.leponeeta), lepDelPhi, wt_noqcd);
    //Jet-binned QCD transfer factor measurement histograms
    if(nJets == 0) {
      Hist->hQCDDelRJ[0] ->Fill(lepDelR                ,wt_noqcd);
      Hist->hQCDOnePtVsTwoPtJ[0]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl);
    } else if(nJets == 1) {
      Hist->hQCDDelRJ[1] ->Fill(lepDelR                ,wt_noqcd);
      Hist->hQCDOnePtVsTwoPtJ[1]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl);
    } else {
      Hist->hQCDDelRJ[2] ->Fill(lepDelR                ,wt_noqcd);
      Hist->hQCDOnePtVsTwoPtJ[2]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl);
    }
    Hist->hQCDOnePtVsTwoPtJ[3]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcdcl); //jet-inclusive correction
    Hist->hQCDOnePtVsTwoPtIso[0]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noqcd); //no QCD SS-->OS weights
    Hist->hQCDOnePtVsTwoPtIso[1]->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), wt_noantiiso); //no muon anti-iso --> iso weight

    // Hist->hLepDelRVsPhi ->Fill(lepDelR , lepDelPhi    ,eventWeight*genWeight);
    Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);

    //Histograms for jet --> tau_h scale factors
    // Hist->hLooseLep     ->Fill(isLooseElectron + 2*isLooseMuon + 4*isLooseTau, eventWeight*genWeight);
    if(nTaus == 1 && (std::abs(leptonTwoFlavor) == 15) ) {
      for(UInt_t itau = 0; itau < nTaus; ++itau) {
        int dm = -1;
        const int njets = std::min(2, (int) nJets); //0, 1, or >= 2 jets
        bool fakeMC = fIsData == 0 && std::abs(tausGenFlavor[itau]) == 26;
        if(std::fabs(tausEta[itau]) > 2.3) continue;
        if(tausMVAAntiMu[itau] < 2) continue; //ignore ones that fail the old MVA anti-mu tight ID
        if(tausAntiEle[itau] < 10) continue; //ignore ones that have low anti-ele ID FIXME: Ensure this is done before here!
        float tau_wt = tausWeight[itau];
        if(fIsData || tau_wt <= 0.) tau_wt = 1.;
        //remove j->tau weights if set
        if(jetToTauWeightBias > 0.) tau_wt *= 1./jetToTauWeightBias;
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
          tausLV.SetPtEtaPhiM(tausPt[itau], tausEta[itau], tausPhi[itau], TAUMASS);
          Hist->hTausDeltaR->Fill(std::fabs(tausLV.DeltaR(*leptonOneP4)), tau_wt*eventWeight*genWeight);
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

        if(fakeMC) { //MC fake tau
          Hist->hFakeTauMCNJetDMPtEta[0][njets][dm]->Fill(tausPt[itau], std::fabs(tausEta[itau]), tau_wt*eventWeight*genWeight); //all taus
        } else { //genuine MC tau or data tau
          Hist->hFakeTauNJetDMPtEta[0][njets][dm]  ->Fill(tausPt[itau], std::fabs(tausEta[itau]), tau_wt*eventWeight*genWeight); //all taus
        }
        if(tausAntiJet[itau] > fFakeTauIsoCut) { //anti-isolated tau
          if(fakeMC) { //MC fake tau
            Hist->hFakeTauMCNJetDMPtEta[1][njets][dm]->Fill(tausPt[itau], std::fabs(tausEta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
          } else {
            Hist->hFakeTauNJetDMPtEta[1][njets][dm]  ->Fill(tausPt[itau], std::fabs(tausEta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
          }
        } else { //isolated tau
          if(fakeMC) { //MC fake tau
            Hist->hFakeTauMCNJetDMPtEta[2][njets][dm]->Fill(tausPt[itau], std::fabs(tausEta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
          } else { //genuine MC tau or data tau
            Hist->hFakeTauNJetDMPtEta[2][njets][dm]  ->Fill(tausPt[itau], std::fabs(tausEta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
          }
        }
        // jet --> tau non-closure histograms
        Float_t wt_no_nonclosure(eventWeight*genWeight), wt_no_biascorr(eventWeight*genWeight);
        if(jetToTauWeightCorr > 0.) wt_no_nonclosure *= jetToTauWeight / jetToTauWeightBias; //remove non-closure and bias corrections
        if(jetToTauWeightBias > 0.) wt_no_biascorr   *= jetToTauWeightCorr / jetToTauWeightBias; //remove the bias correction

        Hist->hJetTauDeltaR[0]->Fill(fTreeVars.lepdeltar, eventWeight*genWeight);
        Hist->hJetTauDeltaR[1]->Fill(fTreeVars.lepdeltar, wt_no_nonclosure     );
        Hist->hJetTauDeltaR[2]->Fill(fTreeVars.lepdeltar, wt_no_biascorr       );
        Hist->hJetTauLepM  [0]->Fill(fTreeVars.lepm     , eventWeight*genWeight);
        Hist->hJetTauLepM  [1]->Fill(fTreeVars.lepm     , wt_no_nonclosure     );
        Hist->hJetTauLepM  [2]->Fill(fTreeVars.lepm     , wt_no_biascorr       );
        Hist->hJetTauMTLep [0]->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
        Hist->hJetTauMTLep [1]->Fill(fTreeVars.mtlep    , wt_no_nonclosure     );
        Hist->hJetTauMTLep [2]->Fill(fTreeVars.mtlep    , wt_no_biascorr       );
        Hist->hJetTauMTOne [0]->Fill(fTreeVars.mtone    , eventWeight*genWeight);
        Hist->hJetTauMTOne [1]->Fill(fTreeVars.mtone    , wt_no_nonclosure     );
        Hist->hJetTauMTOne [2]->Fill(fTreeVars.mtone    , wt_no_biascorr       );
        Hist->hJetTauMTTwo [0]->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
        Hist->hJetTauMTTwo [1]->Fill(fTreeVars.mttwo    , wt_no_nonclosure     );
        Hist->hJetTauMTTwo [2]->Fill(fTreeVars.mttwo    , wt_no_biascorr       );
      }
    } //end j-->tau scale factor section

    //Histograms for jet --> lepton scale factors
    for(UInt_t ilep = 0; ilep < nExtraLep; ++ilep) {
      const double eta = std::fabs(leptonsEta[ilep]);
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
  // Hist->hAlpha[0]->Fill((double) fTreeVars.alphaz1, eventWeight*genWeight);
  // Hist->hAlpha[1]->Fill((double) fTreeVars.alphah1, eventWeight*genWeight);
  // Hist->hAlpha[2]->Fill((double) fTreeVars.alpha2, eventWeight*genWeight);
  // Hist->hAlpha[3]->Fill((double) fTreeVars.alpha3, eventWeight*genWeight);
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
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector0 = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector0.Mag() > 0.) bisector0.SetMag(1.);

  //project onto the bisectors
  // double pxi_vis0 = (lp1+lp2)*bisector0;
  // double pxi_inv0 = missing*bisector0;

  // Hist->hPXiVis[0]     ->Fill(pxi_vis0         ,eventWeight*genWeight);
  // Hist->hPXiInv[0]     ->Fill(pxi_inv0         ,eventWeight*genWeight);
  // Hist->hPXiVisOverInv[0]->Fill((pxi_inv0 != 0.) ? pxi_vis0/pxi_inv0 : 1.e6 ,eventWeight*genWeight);
  // Hist->hPXiInvVsVis[0]->Fill(pxi_vis0, pxi_inv0,eventWeight*genWeight);
  // Hist->hPXiDiff[0]    ->Fill(pxi_vis0-pxi_inv0 ,eventWeight*genWeight);

  Hist->hPTauVisFrac    ->Fill(1.*fTreeVars.ptauvisfrac , eventWeight*genWeight);

  // double coeff = 0.85 - 1.; // match 2016 clfv higgs paper (defined pxi inv = xi dot (lep1 + lep2 + met) = my pxi inv + pxi vis)
  // double offset = -60.;
  // Hist->hPXiDiff3[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);

  // coeff = 0.6;
  // offset = -40.;
  // Hist->hPXiDiff2[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);

  // Hist->hPtSum[0]      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+met ,eventWeight*genWeight);
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
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

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillLepHistogram(LepHist_t* Hist) {
  /////////////
  //  Lep 1  //
  /////////////
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()            ,eventWeight*genWeight);
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    //trigger weight variations
    float wt = (fIsData) ? 1.f : std::max(0.001f, leptonOneTrigWeight * leptonTwoTrigWeight);
    Hist->hOnePt[1]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
    Hist->hOnePt[2]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[0]);
    Hist->hOnePt[3]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[1]);
    Hist->hOnePt[4]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt*triggerWeights[2]);
    Hist->hOnePt[5]->Fill(fTreeVars.leponept, eventWeight*genWeight/leptonOneTrigWeight);
    //remove ID weights
    wt = (leptonOneWeight1 > 0.) ? leptonOneWeight1 : 1.;
    Hist->hOnePt[6]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
    wt = (leptonOneWeight2 > 0.) ? leptonOneWeight2 : 1.;
    Hist->hOnePt[7]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
    //remove Z pT weight
    wt = (zPtWeight > 0.) ? zPtWeight : 1.;
    Hist->hOnePt[8]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
    //remove b-tag weight
    wt = (btagWeight > 0.) ? btagWeight : 1.;
    Hist->hOnePt[9]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
    // //remove j->tau weight
    wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
    Hist->hOnePt[10]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
    //remove j->tau weight correction + bias correction
    wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
    Hist->hOnePt[11]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);

    if(nTaus == 1 && ((std::abs(leptonTwoFlavor) == 15 && tauDecayMode%10 < 2) || (Tau_decayMode[0]%10 < 2))) {
      int dmr = (std::abs(leptonTwoFlavor) == 15) ? tauDecayMode : Tau_decayMode[0];
      if(dmr > 9) dmr -= (10 - 2); //10,11 --> 2,3
      dmr += 1; //dmr = DM ID + 1, so 0 can be inclusive
      dmr = std::min(4, dmr); //ensure it's not out of bounds
      TLorentzVector taulv;
      if(std::abs(leptonTwoFlavor) == 15) taulv = *leptonTwoP4;
      else taulv.SetPtEtaPhiM(Tau_pt[0], Tau_eta[0], Tau_phi[0], Tau_mass[0]);

      //FIXME: Remove tau pT region histograms
      float wt_nojt(eventWeight*genWeight), wt_nocorr(eventWeight*genWeight), wt_nobias(eventWeight*genWeight);
      if(jetToTauWeightBias > 0.) wt_nojt   *= 1.                 / jetToTauWeightBias; // remove the entire weight
      if(jetToTauWeightBias > 0.) wt_nocorr *= jetToTauWeight     / jetToTauWeightBias; // remove the non-closure and bias corrections
      if(jetToTauWeightBias > 0.) wt_nobias *= jetToTauWeightCorr / jetToTauWeightBias; // remove the bias corrections
      double dr = std::fabs(leptonOneP4->DeltaR(taulv));
      Hist->hJetTauOnePt[0]       ->Fill(fTreeVars.leponept, wt_nocorr);
      Hist->hJetTauOnePt[dmr]     ->Fill(fTreeVars.leponept, wt_nocorr);
      Hist->hJetTauOnePtQCD[0]    ->Fill(fTreeVars.leponept, eventWeight*genWeight);
      Hist->hJetTauOnePtQCD[1]    ->Fill(fTreeVars.leponept, wt_nocorr);
      Hist->hJetTauOnePtVsR[0]    ->Fill(fTreeVars.leponept, dr, wt_nocorr);
      Hist->hJetTauOnePtVsR[dmr]  ->Fill(fTreeVars.leponept, dr, wt_nocorr);
      Hist->hJetTauOneR->Fill(dr, wt_nocorr);
      Hist->hJetTauOneEta->Fill(std::fabs(leptonOneP4->Eta()), wt_nocorr);
      Hist->hJetTauOneMetDeltaPhi[0]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr); //inclusive
      Hist->hJetTauOneMetDeltaPhi[dmr]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr);
      Hist->hJetTauOneMetDPhiQCD[0]->Fill(fTreeVars.onemetdeltaphi, eventWeight*genWeight); //inclusive
      Hist->hJetTauOneMetDPhiQCD[1]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr); //inclusive
      Hist->hJetTauOneRelIso[0]->Fill(fTreeVars.leponereliso, eventWeight*genWeight);
      Hist->hJetTauOneRelIso[1]->Fill(fTreeVars.leponereliso, wt_nobias);

      Hist->hJetTauOneMetDPhiComp->Fill(fTreeVars.onemetdeltaphi, wt_nojt); //compositions
      Hist->hJetTauTwoMetDPhiComp->Fill(fTreeVars.twometdeltaphi, wt_nojt);
      Hist->hJetTauOnePtComp     ->Fill(fTreeVars.leponept       , wt_nojt);
      Hist->hJetTauTwoPtComp     ->Fill(fTreeVars.leptwopt       , wt_nojt);
      Hist->hJetTauMTOneComp     ->Fill(fTreeVars.mtone         , wt_nojt);
      Hist->hJetTauMTTwoComp     ->Fill(fTreeVars.mttwo         , wt_nojt);

      dr = std::fabs(leptonTwoP4->DeltaR(taulv));
      Hist->hJetTauTwoPt[0]       ->Fill(fTreeVars.leptwopt, wt_nocorr);
      Hist->hJetTauTwoPt[dmr]     ->Fill(fTreeVars.leptwopt, wt_nocorr);
      Hist->hJetTauTwoPtVsR[0]    ->Fill(fTreeVars.leptwopt, dr, wt_nocorr);
      Hist->hJetTauTwoPtVsR[dmr]  ->Fill(fTreeVars.leptwopt, dr, wt_nocorr);
      Hist->hJetTauTwoR[0]->Fill(dr, eventWeight*genWeight);
      Hist->hJetTauTwoR[1]->Fill(dr, wt_nocorr);
      Hist->hJetTauTwoEta[0]->Fill(std::fabs(leptonTwoP4->Eta()), eventWeight*genWeight);
      Hist->hJetTauTwoEta[1]->Fill(std::fabs(leptonTwoP4->Eta()), wt_nocorr);
      Hist->hJetTauTwoEtaQCD[0]->Fill(std::fabs(leptonTwoP4->Eta()), eventWeight*genWeight);
      Hist->hJetTauTwoEtaQCD[1]->Fill(std::fabs(leptonTwoP4->Eta()), wt_nocorr);
      Hist->hJetTauTwoPtVsOnePt->Fill(fTreeVars.leponept, taulv.Pt(), wt_nocorr);
      Hist->hJetTauTwoMetDeltaPhi[0]->Fill(fTreeVars.twometdeltaphi, eventWeight*genWeight);
      Hist->hJetTauTwoMetDeltaPhi[1]->Fill(fTreeVars.twometdeltaphi, wt_nocorr);
      Hist->hJetTauTwoRelIso[0]->Fill(fTreeVars.leptworeliso, eventWeight*genWeight);
      Hist->hJetTauTwoRelIso[1]->Fill(fTreeVars.leptworeliso, wt_nobias);
    }
    // Hist->hOneP         ->Fill(leptonOneP4->P()             ,eventWeight*genWeight);
    // Hist->hOneM         ->Fill(leptonOneP4->M()             ,eventWeight*genWeight);
    // Hist->hOnePtOverM   ->Fill(fTreeVars.leponept / fTreeVars.lepm, eventWeight*genWeight);
    Hist->hOneEta       ->Fill(leptonOneP4->Eta()           ,eventWeight*genWeight);
    // Hist->hOneSCEta     ->Fill(leptonOneSCEta               ,eventWeight*genWeight);
    Hist->hOnePhi       ->Fill(leptonOneP4->Phi()           ,eventWeight*genWeight);
    Hist->hOneD0        ->Fill(leptonOneD0                  ,eventWeight*genWeight);
    Hist->hOneDXY       ->Fill(leptonOneDXY                 ,eventWeight*genWeight);
    Hist->hOneDZ        ->Fill(leptonOneDZ                  ,eventWeight*genWeight);
    Hist->hOneIso       ->Fill(leptonOneIso                 ,eventWeight*genWeight);
    Hist->hOneID1       ->Fill(leptonOneID1                 ,eventWeight*genWeight);
    Hist->hOneID2       ->Fill(leptonOneID2                 ,eventWeight*genWeight);
    Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
    Hist->hOneFlavor    ->Fill(std::abs(leptonOneFlavor)        ,eventWeight*genWeight);
    Hist->hOneGenFlavor ->Fill(leptonOneGenFlavor           ,eventWeight*genWeight);
    Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  }
  Hist->hOneTrigger   ->Fill(leptonOneTrigger               ,eventWeight*genWeight);
  int offset_1(0), offset_2(0);
  if(std::abs(leptonOneFlavor) == 11) {
    offset_1 = SystematicGrouping::kElectronID;
    offset_2 = SystematicGrouping::kElectronRecoID;
  } else if(std::abs(leptonOneFlavor) == 13) {
    offset_1 = SystematicGrouping::kMuonID;
    offset_2 = SystematicGrouping::kMuonIsoID;
  }
  Hist->hOneWeight1Group->Fill(leptonOneWeight1_group - offset_1     ,eventWeight*genWeight);
  Hist->hOneWeight2Group->Fill(leptonOneWeight2_group - offset_2     ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    Hist->hOneWeight    ->Fill(leptonOneWeight1*leptonOneWeight2,eventWeight*genWeight);
    Hist->hOneTrigWeight->Fill(leptonOneTrigWeight          ,eventWeight*genWeight);

    double oneMetDelPhi  = std::fabs(leptonOneP4->Phi() - metPhi);
    if(oneMetDelPhi > M_PI)
      oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
    Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);
  }

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()            ,eventWeight*genWeight);
  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    //trigger weight variations
    float wt = (fIsData) ? 1.f : std::max(0.001f, leptonOneTrigWeight * leptonTwoTrigWeight);
    Hist->hTwoPt[1]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
    Hist->hTwoPt[2]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[0]);
    Hist->hTwoPt[3]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[1]);
    Hist->hTwoPt[4]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt*triggerWeights[2]);
    Hist->hTwoPt[5]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/leptonTwoTrigWeight);
    //remove ID weights
    wt = leptonTwoWeight1;
    Hist->hTwoPt[6]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
    wt = leptonTwoWeight2;
    Hist->hTwoPt[7]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
    //remove Z pT weight
    wt = zPtWeight;
    Hist->hTwoPt[8]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
    //remove b-tag weight
    wt = btagWeight;
    Hist->hTwoPt[9]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
    //remove j->tau weight
    wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
    Hist->hTwoPt[10]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
    //remove j->tau weight correction
    wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
    Hist->hTwoPt[11]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);

    // Hist->hTwoP         ->Fill(leptonTwoP4->P()             ,eventWeight*genWeight);
    // Hist->hTwoM         ->Fill(leptonTwoP4->M()             ,eventWeight*genWeight);
    // Hist->hTwoPtOverM   ->Fill(fTreeVars.leptwopt / fTreeVars.lepm, eventWeight*genWeight);
    Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()           ,eventWeight*genWeight);
    Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()           ,eventWeight*genWeight);
    Hist->hTwoD0        ->Fill(leptonTwoD0                  ,eventWeight*genWeight);
    Hist->hTwoDXY       ->Fill(leptonTwoDXY                 ,eventWeight*genWeight);
    Hist->hTwoDZ        ->Fill(leptonTwoDZ                  ,eventWeight*genWeight);
    Hist->hTwoIso       ->Fill(leptonTwoIso                 ,eventWeight*genWeight);
    Hist->hTwoID1       ->Fill(leptonTwoID1                 ,eventWeight*genWeight);
    Hist->hTwoID2       ->Fill(leptonTwoID2                 ,eventWeight*genWeight);
    Hist->hTwoID3       ->Fill(leptonTwoID3                 ,eventWeight*genWeight);
    Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
    Hist->hTwoFlavor    ->Fill(std::abs(leptonTwoFlavor)        ,eventWeight*genWeight);
    Hist->hTwoGenFlavor ->Fill(leptonTwoGenFlavor           ,eventWeight*genWeight);
    Hist->hTwoQ         ->Fill(leptonTwoFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  }
  Hist->hTwoTrigger   ->Fill(leptonTwoTrigger               ,eventWeight*genWeight);
  offset_1 = 0; offset_2 = 0;
  if(std::abs(leptonTwoFlavor) == 11) {
    offset_1 = SystematicGrouping::kElectronID;
    offset_2 = SystematicGrouping::kElectronRecoID;
  } else if(std::abs(leptonTwoFlavor) == 13) {
    offset_1 = SystematicGrouping::kMuonID;
    offset_2 = SystematicGrouping::kMuonIsoID;
  }
  Hist->hTwoWeight1Group->Fill(leptonTwoWeight1_group - offset_1     ,eventWeight*genWeight);
  Hist->hTwoWeight2Group->Fill(leptonTwoWeight2_group - offset_2     ,eventWeight*genWeight);
  if(fDoSystematics >= 0) {
    Hist->hTwoWeight    ->Fill(leptonTwoWeight1*leptonTwoWeight2,eventWeight*genWeight);
    Hist->hTwoTrigWeight->Fill(leptonTwoTrigWeight          ,eventWeight*genWeight);

    double twoMetDelPhi  = std::fabs(leptonTwoP4->Phi() - metPhi);
    if(twoMetDelPhi > M_PI)
      twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
    Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

    Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);
    Hist->hD0Diff      ->Fill(leptonTwoD0-leptonOneD0             ,eventWeight*genWeight);
    Hist->hDXYDiff     ->Fill(leptonTwoDXY-leptonOneDXY           ,eventWeight*genWeight);
    Hist->hDZDiff      ->Fill(leptonTwoDZ-leptonOneDZ             ,eventWeight*genWeight);
    Hist->hTwoPtVsOnePt->Fill(fTreeVars.leponept, fTreeVars.leptwopt,eventWeight*genWeight);
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::FillSystematicHistogram(SystematicHist_t* Hist) {
  bool isSameFlavor = std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor);
  bool isMuTau = std::abs(leptonOneFlavor) == 13 && std::abs(leptonTwoFlavor) == 15;
  bool isETau  = std::abs(leptonOneFlavor) == 13 && std::abs(leptonTwoFlavor) == 15;
  for(int sys = 0; sys < kMaxSystematics; ++sys) {
    float weight = eventWeight*genWeight;
    if(weight == 0.) continue; //no way to re-scale 0, contributes nothing to histograms so can just skip
    TLorentzVector lv1 = *leptonOneP4;
    TLorentzVector lv2 = *leptonTwoP4;
    if(sys == 0) weight = weight;                                          //do nothing
    else if  (sys ==  1) {                                                 //electron ID scale factors
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  2) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  3) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight1_sys / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
    } else if(sys ==  4) {                                                 //muon ID scale factors
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  5) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  6) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight1_sys  / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight1_sys  / leptonTwoWeight1;
    } else if(sys ==  7) {                                                 //tau ID scale factors
      if(std::abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_up / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_up / leptonTwoWeight1;
    } else if(sys ==  8) {
      if(std::abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_down / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    } else if(sys ==  9) {
      if(std::abs(leptonOneFlavor) == 15) weight *= leptonOneWeight1_sys  / leptonOneWeight1;
      if(std::abs(leptonTwoFlavor) == 15) weight *= leptonTwoWeight1_sys  / leptonTwoWeight1;
    } else if(sys == 10) weight *= jetToTauWeightUp     / jetToTauWeight ; //Jet --> tau weights
    else if  (sys == 11) weight *= jetToTauWeightDown   / jetToTauWeight ;
    else if  (sys == 12) weight *= jetToTauWeightSys    / jetToTauWeight ;
    else if  (sys == 13) weight *= zPtWeightUp          / zPtWeight      ; //Drell-Yan pT weights
    else if  (sys == 14) weight *= zPtWeightDown        / zPtWeight      ;
    else if  (sys == 15) weight *= zPtWeightSys         / zPtWeight      ;
    else if  (sys == 16) {                                                 //electron reco ID scale factors
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_up / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_up / leptonTwoWeight2;
    } else if(sys == 17) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_down / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    } else if(sys == 18) {
      if(std::abs(leptonOneFlavor) == 11) weight *= leptonOneWeight2_sys / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 11) weight *= leptonTwoWeight2_sys / leptonTwoWeight2;
    } else if(sys == 19) {                                                 //muon iso ID scale factors
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_up / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_up / leptonTwoWeight2;
    } else if(sys == 20) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_down / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    } else if(sys == 21) {
      if(std::abs(leptonOneFlavor) == 13) weight *= leptonOneWeight2_sys  / leptonOneWeight2;
      if(std::abs(leptonTwoFlavor) == 13) weight *= leptonTwoWeight2_sys  / leptonTwoWeight2;
    }
    //FIXME: Need to re-evaluate variables using the updated tau four momentum
    else if  (sys == 22) {                                                 //Tau ES
      if(std::abs(leptonTwoFlavor) == 15 && tauES > 0. && tauES_up > 0.) {
        lv2 *= (tauES_up / tauES);
      }
    } else if(sys == 23) {
      if(std::abs(leptonTwoFlavor) == 15 && tauES > 0. && tauES_down > 0.) {
        lv2 *= (tauES_down / tauES);
      }
    }
    else if  (sys == 25) weight *= (qcdWeight > 0.) ? qcdWeightUp  / qcdWeight : 0.; //SS --> OS weights
    else if  (sys == 26) weight *= (qcdWeight > 0.) ? qcdWeightDown/ qcdWeight : 0.;
    else if  (sys == 27) weight *= (qcdWeight > 0.) ? qcdWeightSys / qcdWeight : 0.;
    else if  (sys == 28) weight *= jetToTauWeightCorrUp     / jetToTauWeightCorr   ; //Jet --> tau weight pT corrections
    else if  (sys == 29) weight *= jetToTauWeightCorrDown   / jetToTauWeightCorr   ;
    else if  (sys == 30) weight *= jetToTauWeightCorrSys    / jetToTauWeightCorr   ;
    else if  (sys == 31) weight *= jetToTauWeight_compUp    / jetToTauWeightBias   ; //Jet --> tau weight composition
    else if  (sys == 32) weight *= jetToTauWeight_compDown  / jetToTauWeightBias   ;
    else if  (sys == 33) weight *= jetToTauWeight_compUp    / jetToTauWeightBias   ;
    else if(sys > 33 && sys < 43) {                                                // tau IDs by PDG ID
      if(std::abs(leptonTwoFlavor) == 15) {
        if(sys == 34 && std::abs(tauGenFlavor) == 15)       //tau anti-jet ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 35 && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 36 && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
        else if(sys == 37 && std::abs(tauGenFlavor) == 13)  //tau anti-mu ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 38 && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 39 && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
        else if(sys == 40 && std::abs(tauGenFlavor) == 11)  //tau anti-ele ID scale factors
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if(sys == 41 && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
        else if(sys == 42 && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_sys / leptonTwoWeight1;
      }
    } else if  (sys == 43) { weight *= (fIsData || fIsEmbed) ? 1. : 1. + 0.016; //luminosity uncertainty
    } else if  (sys == 44) { weight *= (fIsData || fIsEmbed) ? 1. : 1. - 0.016;
    } else if  (sys == 45) { weight *= (btagWeight > 0.) ? btagWeightUp   / btagWeight : 1.; //btag uncertainty
    } else if  (sys == 46) { weight *= (btagWeight > 0.) ? btagWeightDown / btagWeight : 1.;
    } else if  (sys == 47) { weight *= jetToTauWeightBiasUp     / jetToTauWeightBias   ; //Jet --> tau weight bias corrections
    } else if  (sys == 48) { weight *= jetToTauWeightBiasDown   / jetToTauWeightBias   ;

      //Removed j->tau groupings since now using fitter errors uncorrelated by years
    } else if  (sys >= SystematicGrouping::kJetToTau && sys < 100) { //Jet --> tau systematic groupings, (4 DM)*(2 eta)*(3 years)*(2 up/down) = 48 histograms
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int offset = SystematicGrouping::kJetToTau;
      if(2*(jetToTauWeightGroup) - offset     == sys) weight *= jetToTauWeightUp   / jetToTauWeight;
      if(2*(jetToTauWeightGroup) - offset + 1 == sys) weight *= jetToTauWeightDown / jetToTauWeight;
      //Removed muon ID groupings since it appears to be small enough to treat correlated
    // } else if  (sys >= SystematicGrouping::kMuonID && sys < 150) { //Muon ID
    //   int offset = SystematicGrouping::kMuonID;
    //   if(std::abs(leptonOneFlavor) == 13) {
    //     if(2*(leptonOneWeight1_group) - offset     == sys) weight *= leptonOneWeight1_up   / leptonOneWeight1;
    //     if(2*(leptonOneWeight1_group) - offset + 1 == sys) weight *= leptonOneWeight1_down / leptonOneWeight1;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 13) {
    //     if(2*(leptonTwoWeight1_group) - offset     == sys) weight *= leptonTwoWeight1_up   / leptonTwoWeight1;
    //     if(2*(leptonTwoWeight1_group) - offset + 1 == sys) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    //   }
      //Removed muon iso ID groupings since it appears to be small enough to treat correlated
    // } else if  (sys >= SystematicGrouping::kMuonIsoID && sys < 200) { //Muon ID
    //   int offset = SystematicGrouping::kMuonIsoID;
    //   if(std::abs(leptonOneFlavor) == 13) {
    //     if(2*(leptonOneWeight2_group) - offset     == sys) weight *= leptonOneWeight2_up   / leptonOneWeight2;
    //     if(2*(leptonOneWeight2_group) - offset + 1 == sys) weight *= leptonOneWeight2_down / leptonOneWeight2;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 13) {
    //     if(2*(leptonTwoWeight2_group) - offset     == sys) weight *= leptonTwoWeight2_up   / leptonTwoWeight2;
    //     if(2*(leptonTwoWeight2_group) - offset + 1 == sys) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    //   }
    } else if  (sys >= 100 && sys < 106) { //tau anti-jet ID, separated by years
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int base = 100;
      if(std::abs(leptonTwoFlavor) == 15) {
        int bin = 2*(fYear - 2016); //uncorrelated between years
        if((sys == base + bin) && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + bin) && std::abs(tauGenFlavor) == 15)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    } else if  (sys >= 110 && sys < 140) { //tau anti-mu ID, separated by 5 bins per year --> 15 histograms per up/down
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int base = 110;
      if(std::abs(leptonTwoFlavor) == 15) {
        int bin = 5*(fYear - 2016) + leptonTwoWeight1_bin; //uncorrelated between years and bins
        if((sys == base + 2*bin) && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + 2*bin) && std::abs(tauGenFlavor) == 13)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    } else if  (sys >= 140 && sys < 158) { //tau anti-ele ID, separated by 3 bins per year --> 9 histograms per up/down
      if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) continue;
      int base = 140;
      if(std::abs(leptonTwoFlavor) == 15) {
        int bin = 3*(fYear - 2016) + leptonTwoWeight1_bin; //uncorrelated between years and bins
        if((sys == base + 2*bin) && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_up / leptonTwoWeight1;
        else if((sys == base + 1 + 2*bin) && std::abs(tauGenFlavor) == 11)
          weight *= leptonTwoWeight1_down / leptonTwoWeight1;
      }
    //  //Remove electron ID and Reco ID groupings, as generally systematically correlated errors
    // } else if  (sys >= SystematicGrouping::kElectronID && sys < 250) { //Electron ID
    //   int offset = SystematicGrouping::kElectronID;
    //   if(std::abs(leptonOneFlavor) == 11) {
    //     if(2*(leptonOneWeight1_group) - offset     == sys) weight *= leptonOneWeight1_up   / leptonOneWeight1;
    //     if(2*(leptonOneWeight1_group) - offset + 1 == sys) weight *= leptonOneWeight1_down / leptonOneWeight1;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 11) {
    //     if(2*(leptonTwoWeight1_group) - offset     == sys) weight *= leptonTwoWeight1_up   / leptonTwoWeight1;
    //     if(2*(leptonTwoWeight1_group) - offset + 1 == sys) weight *= leptonTwoWeight1_down / leptonTwoWeight1;
    //   }
    // } else if  (sys >= SystematicGrouping::kElectronRecoID && sys < 300) { //Electron Reco ID
    //   int offset = SystematicGrouping::kElectronRecoID;
    //   if(std::abs(leptonOneFlavor) == 11) {
    //     if(2*(leptonOneWeight2_group) - offset     == sys) weight *= leptonOneWeight2_up   / leptonOneWeight2;
    //     if(2*(leptonOneWeight2_group) - offset + 1 == sys) weight *= leptonOneWeight2_down / leptonOneWeight2;
    //   }
    //   if(std::abs(leptonTwoFlavor) == 11) {
    //     if(2*(leptonTwoWeight2_group) - offset     == sys) weight *= leptonTwoWeight2_up   / leptonTwoWeight2;
    //     if(2*(leptonTwoWeight2_group) - offset + 1 == sys) weight *= leptonTwoWeight2_down / leptonTwoWeight2;
    //   }
    } else continue; //no need to fill undefined systematics

    if(!std::isfinite(weight)) {
      std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << " Systematic " << sys << ": Weight is not finite! weight = " << weight << " event weight = "
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
    if(!std::isfinite(mvaweight)) {
      std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << " MVA weight is not finite! mvaweight = " << mvaweight << " eventweightMVA = "
                << fTreeVars.eventweightMVA << ", setting to 0...\n";
      mvaweight = 0.;
    }
    for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
      const int category = fMVAConfig.data_cat_[i];
      //Only fill relevant MVA histograms for the datset
      if(!((category == 1 && isMuTau) ||
           (category == 2 && isETau) ||
           (category == 0 && !(isMuTau || isETau)))
         ) continue;
      float mvascore = fMvaOutputs[i];
      if(!std::isfinite(mvascore) && fVerbose > 0) {
        std::cout << "CLFVHistMaker::" << __func__ << ": Entry " << fentry << " MVA " << i << " score is not finite = " << mvascore << "! Setting to -2...\n";
        mvascore = -2.;
      }
      Hist->hMVA[i][sys]->Fill(mvascore, mvaweight);
    }
  }
}


//--------------------------------------------------------------------------------------------------------------
Bool_t CLFVHistMaker::Process(Long64_t entry)
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
  if(fVerbose > 0 || entry%50000 == 0) std::cout << Form("Processing event: %12lld (%5.1f%%)\n", entry, entry*100./fChain->GetEntriesFast());
  int icutflow = 0;
  fCutFlow->Fill(icutflow); ++icutflow; //0

  //Initialize base object information
  CountObjects();

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

  fCutFlow->Fill(icutflow); ++icutflow; //1
  //split W+Jets into N(jets) generated for binned sample combination
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

  // /////////////////
  // // Correct MET //
  // /////////////////

  // //add the met correction to the met
  // TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  // TVector3 missingPUPPI(puppMETC*cos(puppMETCphi), puppMETC*sin(puppMETCphi), 0.);
  // TVector3 missingCorr(metCorr*cos(metCorrPhi), metCorr*sin(metCorrPhi), 0.);
  // missing = missing + missingCorr;
  // missingPUPPI = missingPUPPI + missingCorr;
  // puppMETC = missingPUPPI.Mag();
  // if(fMETType == 0) {
  //   met = missing.Mag();
  //   metPhi = missing.Phi();
  // } else if(fMETType == 1) {
  //   met = missingPUPPI.Mag();
  //   metPhi = missingPUPPI.Phi();
  // }
  // if(fVerbose > 0 ) std::cout << " MET = " << met << std::endl;

  //Initialize systematic variation weights
  InitializeSystematics();


  //selections
  //use the tree name to choose the selection
  bool mutau = nElectron == 0 && nMuon == 1 && nTau == 1 && (fSelection == "" || fSelection == "mutau");
  bool etau  = nElectron == 1 && nMuon == 0 && nTau == 1 && (fSelection == "" || fSelection == "etau" );
  bool emu   = nElectron == 1 && nMuon == 1              && (fSelection == "" || fSelection == "emu"  );
  bool mumu  = nElectron == 0 && nMuon == 2              && (fSelection == "" || fSelection == "mumu" );
  bool ee    = nElectron == 2 && nMuon == 0              && (fSelection == "" || fSelection == "ee"   );


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

  //////////////////////////////////////////////////////////////
  // Check if anti-iso lepton category
  //////////////////////////////////////////////////////////////

  isLooseElectron = (ee || emu || etau) && looseQCDSelection;
  isLooseMuon     = (mumu || emu || mutau) && looseQCDSelection;
  isLooseTau      = (etau || mutau) && tauDeepAntiJet < 50 && looseQCDSelection;

  if(isLooseElectron) {
    if(emu || etau) isLooseElectron &= leptonOneID1 < 4; //Not WP80
    if(ee)          isLooseElectron &= leptonOneID1 < 4 || leptonTwoID1 < 4; //Not WP80
  }
  if(isLooseMuon) {
    if(emu)   isLooseMuon &= leptonTwoID1 <= ParticleCorrections::kTightMuIso; //Not Tight IsoID
    if(mutau) isLooseMuon &= leptonOneID1 <= ParticleCorrections::kTightMuIso; //Not Tight IsoID
    if(mumu)  isLooseMuon &= leptonOneID1 <= ParticleCorrections::kTightMuIso || leptonTwoID1 < ParticleCorrections::kTightMuIso; //Not Tight IsoID
  }

  isFakeElectron  = !fIsData && ((std::abs(leptonOneFlavor) == 11 && leptonOneGenFlavor == 26) ||
                                 (std::abs(leptonTwoFlavor) == 11 && leptonTwoGenFlavor == 26));
  isFakeMuon      = !fIsData && ((std::abs(leptonOneFlavor) == 13 && leptonOneGenFlavor == 26) ||
                                 (std::abs(leptonTwoFlavor) == 13 && leptonTwoGenFlavor == 26));


  //////////////////////////////////////////////////////////////
  // Apply or remove scale factors
  //////////////////////////////////////////////////////////////

  //object pT thresholds
  float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  //apply embedding unfolding correction to embedded samples
  if(fIsEmbed) {
    if(fIsEmbed && embeddingWeight > 0.9) { //as branching fraction is in the weight, should never be this large
      if(std::fabs(genWeight) > 0.5) {
        std::cout << "!!! Warning! Event " << fentry << ": Embedding generation weight is large but gen-weight non-zero! Embedding weight = "
                  << embeddingWeight << " genWeight = " << genWeight
                  << " event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                  << std::endl;
      }
      eventWeight /= embeddingWeight; //remove the previous weight
      embeddingWeight = 1.;
      genWeight = 0.; //remove the event by using the gen weight
    }
    embeddingUnfoldingWeight = fEmbeddingWeight.UnfoldingWeight(zLepOnePt, zLepOneEta, zLepTwoPt, zLepTwoEta, fYear);
    eventWeight *= embeddingUnfoldingWeight;
    eventWeight /= leptonOneWeight1; eventWeight /= leptonOneWeight2;
    eventWeight /= leptonTwoWeight1; eventWeight /= leptonTwoWeight2;
    leptonOneWeight1      = 1.; leptonOneWeight2      = 1.;
    leptonOneWeight1_up   = 1.; leptonOneWeight2_up   = 1.;
    leptonOneWeight1_down = 1.; leptonOneWeight2_down = 1.;
    leptonTwoWeight1      = 1.; leptonTwoWeight2      = 1.;
    leptonTwoWeight1_up   = 1.; leptonTwoWeight2_up   = 1.;
    leptonTwoWeight1_down = 1.; leptonTwoWeight2_down = 1.;
    int tauGenID = 0;
    if     (std::abs(tauGenFlavor) == 15) tauGenID = 5;
    else if(std::abs(tauGenFlavor) == 13) tauGenID = 2;
    else if(std::abs(tauGenFlavor) == 11) tauGenID = 1;
    if     (std::abs(leptonOneFlavor) == 11) {
      if(fUseEmbedTnPWeights) {
        leptonOneWeight1 = fEmbeddingTnPWeight.ElectronIDWeight(leptonOneP4->Pt(), leptonOneSCEta, fYear, isLooseElectron, mcEra);
      } else {
        leptonOneWeight1 = fEmbeddingWeight.ElectronIDWeight(leptonOneP4->Pt(), leptonOneSCEta, fYear);
      }
    } else if(std::abs(leptonOneFlavor) == 13) {
      if(fUseEmbedTnPWeights) {
        leptonOneWeight1 = fEmbeddingTnPWeight.MuonIDWeight(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, isLooseMuon, mcEra);
      } else {
        leptonOneWeight1 = fEmbeddingWeight.MuonIDWeight(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear);
      }
    } else if(std::abs(leptonOneFlavor) == 15) leptonOneWeight1 = fTauIDWeight->IDWeight(leptonOneP4->Pt(), leptonOneP4->Eta(), tauGenID, tauDeepAntiJet,
                                                                                  fYear, leptonOneWeight1_up, leptonOneWeight1_down);
    if     (std::abs(leptonTwoFlavor) == 11) {
      if(fUseEmbedTnPWeights) {
        leptonTwoWeight1 = fEmbeddingTnPWeight.ElectronIDWeight(leptonTwoP4->Pt(), leptonTwoSCEta, fYear, isLooseElectron, mcEra);
      } else {
        leptonTwoWeight1 = fEmbeddingWeight.ElectronIDWeight(leptonTwoP4->Pt(), leptonTwoSCEta, fYear);
      }
    } else if(std::abs(leptonTwoFlavor) == 13) {
      if(fUseEmbedTnPWeights) {
        leptonTwoWeight1 = fEmbeddingTnPWeight.MuonIDWeight(leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, isLooseMuon, mcEra);
      } else {
        leptonTwoWeight1 = fEmbeddingWeight.MuonIDWeight(leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear);
      }
    } else if(std::abs(leptonTwoFlavor) == 15) leptonTwoWeight1 = fTauIDWeight->IDWeight(leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauGenID, tauDeepAntiJet,
                                                                                         fYear, leptonTwoWeight1_up, leptonTwoWeight1_down);

    eventWeight *= leptonOneWeight1; eventWeight *= leptonOneWeight2;
    eventWeight *= leptonTwoWeight1; eventWeight *= leptonTwoWeight2;
  }

  //No weights in data
  if(fIsData) {
    eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.; genTauFlavorWeight = 1.;
    jetPUIDWeight = 1.; btagWeight = 1.; embeddingWeight = 1.; embeddingUnfoldingWeight = 1.;
  }


  InitializeTreeVariables(mutau+2*etau+5*emu+9*mumu+18*ee);


  const bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  if(bisector.Mag() > 0.) bisector.SetMag(1.);

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

  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////

  //Ignore loose lepton selection in same flavor category for now
  ee   &= !isLooseElectron;
  mumu &= !isLooseMuon;


  jetToTauWeight = 1.; jetToTauWeightUp = 1.; jetToTauWeightDown = 1.; jetToTauWeightSys = 1.; jetToTauWeightGroup = 0;
  jetToTauWeightCorr = 1.; jetToTauWeightCorrUp = 1.; jetToTauWeightCorrDown = 1.; jetToTauWeightCorrSys = 1.;
  jetToTauWeightBias = 1.; jetToTauWeightBiasUp = 1.; jetToTauWeightBiasDown = 1.;
  jetToTauWeight_compUp = 1.; jetToTauWeight_compDown = 1.;

  qcdWeight = 1.; qcdWeightUp = 1.; qcdWeightDown = 1.; qcdWeightSys = 1.; qcdClosure = 1.;


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
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 1);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //5

  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  TLorentzVector* muon_2 = 0;
  TLorentzVector* electron_2 = 0;
  //same flavor categories
  if(std::abs(leptonOneFlavor) == std::abs(leptonTwoFlavor)) {
    if(std::abs(leptonOneFlavor) == 13) { //mumu
      muon = leptonOneP4;
      muon_2 = leptonTwoP4;
    } else if(std::abs(leptonOneFlavor) == 11) { //ee
      electron = leptonOneP4;
      electron_2 = leptonTwoP4;
    }
  } else {
    if(std::abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
    else if(std::abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
    if(std::abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
    else if(std::abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
    if(std::abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
    else if(std::abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;
  }

  //lepton vectors must be found
  mutau = mutau && (tau != 0) && (muon != 0);
  etau  = etau  && (tau != 0) && (electron != 0);
  emu   = emu   && (muon != 0) && (electron != 0);
  mumu  = mumu  && (muon != 0) && (muon_2 != 0);
  ee    = ee    && (electron != 0) && (electron_2 != 0);


  //leptons must satisfy the pt requirements and fire a trigger
  mutau = mutau && muon->Pt() > muon_trig_pt_ && tau->Pt() > tau_pt && leptonOneFired;
  etau  = etau  && electron->Pt() > electron_trig_pt_ && tau->Pt() > tau_pt && leptonOneFired;
  emu   = emu   && ((electron->Pt() > electron_trig_pt_ && muon->Pt() > muon_pt && leptonOneFired) ||
                    (electron->Pt() > electron_pt && muon->Pt() > muon_trig_pt_ && leptonTwoFired));
  mumu  = mumu  && ((muon->Pt() > muon_trig_pt_ && muon_2->Pt() > muon_pt && leptonOneFired) ||
                    (muon->Pt() > muon_pt  && muon_2->Pt() > muon_trig_pt_ && leptonTwoFired));
  ee    = ee    && ((electron->Pt() > electron_trig_pt_ && electron_2->Pt() > electron_pt && leptonOneFired) ||
                    (electron->Pt() > electron_pt && electron_2->Pt() > electron_trig_pt_ && leptonTwoFired));

  //don't allow multiple muons/electrons in mumu/ee selections
  mumu &= nMuons     == 2;
  ee   &= nElectrons == 2;

  ////////////////////////////////////////////////////////////
  // Set 2 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 2);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //6

  const double electron_eta_max = (fUseEmbedCuts) ? 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.2 : 2.3;

  mutau = mutau && std::fabs(muon->Eta()) < muon_eta_max;
  mutau = mutau && std::fabs(tau->Eta()) < tau_eta_max;
  mutau = mutau && std::fabs(tau->DeltaR(*muon)) > 0.3;

  etau  = etau  && std::fabs(electron->Eta()) < electron_eta_max;
  etau  = etau  && std::fabs(tau->Eta()) < tau_eta_max;
  etau  = etau  && std::fabs(tau->DeltaR(*electron)) > 0.3;

  emu   = emu   && std::fabs(electron->Eta()) < electron_eta_max;
  emu   = emu   && std::fabs(muon->Eta()) < muon_eta_max;
  emu   = emu   && std::fabs(muon->DeltaR(*electron)) > 0.3;

  mumu  = mumu  && std::fabs(muon->Eta()) < muon_eta_max;
  mumu  = mumu  && std::fabs(muon_2->Eta()) < muon_eta_max;
  mumu  = mumu  && std::fabs(muon->DeltaR(*muon_2)) > 0.3;

  ee    = ee    && std::fabs(electron->Eta()) < electron_eta_max;
  ee    = ee    && std::fabs(electron_2->Eta()) < electron_eta_max;
  ee    = ee    && std::fabs(electron->DeltaR(*electron_2)) > 0.3;

  //mass cuts, due to generation ranges need > 50 GeV for all sets
  double mll = (*leptonOneP4+*leptonTwoP4).M();
  mutau &= mll > 51. && mll < 170.;
  etau  &= mll > 51. && mll < 170.;
  emu   &= mll > 51. && mll < 170.;
  mumu  &= mll > 51. && mll < 170.;
  ee    &= mll > 51. && mll < 170.;

  //Remove Mu50 muon trigger if used in the sample
  //FIXME: For mu-mu this removes events with one firing Mu50 and one firing IsoMu due to overwrite of muonTriggerStatus
  //--> update to lep(1/2) trigger status variables
  if(fabs(leptonOneFlavor) == 13 && muonTriggerStatus == 2) leptonOneFired = false;
  if(fabs(leptonTwoFlavor) == 13 && muonTriggerStatus == 2) leptonTwoFired = false;

  //ensure a trigger was fired
  mutau &= leptonOneFired || leptonTwoFired;
  etau  &= leptonOneFired || leptonTwoFired;
  emu   &= leptonOneFired || leptonTwoFired;
  mumu  &= leptonOneFired || leptonTwoFired;
  ee    &= leptonOneFired || leptonTwoFired;


  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  fCutFlow->Fill(icutflow); ++icutflow; //7

  ////////////////////////////////////////////////////////////
  // Set 3 + selection offset: eta, mass, and trigger cuts
  ////////////////////////////////////////////////////////////

  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 3);
  }

  ////////////////////////////////////////////////////////////
  // Set 4 + selection offset: add tau IDs
  ////////////////////////////////////////////////////////////

  mutau &= isLooseTau || tauDeepAntiJet >= 50; //63 = tight
  mutau &= tauDeepAntiMu  >= 10; //15 = tight
  mutau &= tauDeepAntiEle >= 10; //15 = loose
  mutau &= leptonTwoID2   >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  etau  &= isLooseTau || tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //15 = tight
  etau  &= tauDeepAntiEle >= 50; //63 = tight
  etau  &= leptonTwoID2   >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  //remove tau decay modes not interested in
  mutau &= tauDecayMode != 5 && tauDecayMode != 6;
  etau  &= tauDecayMode != 5 && tauDecayMode != 6;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  if(!fDYTesting || fCutFlowTesting) {
    if(!emu || !isLooseElectron)
      FillAllHistograms(set_offset + 4);
  }

  fCutFlow->Fill(icutflow); ++icutflow; //8

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //9
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //10
  ++icutflow;

  //cut-flow for fake leptons
  if(fIsData) fCutFlow->Fill(icutflow); //11
  else if(emu) {
    if((std::abs(leptonOneFlavor) == std::abs(leptonOneGenFlavor))
       && (std::abs(leptonTwoFlavor) == std::abs(leptonTwoGenFlavor))) {
      fCutFlow->Fill(icutflow);
    }
  }
  ++icutflow;
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

  /////////////////////////
  // Jet --> tau weights //
  /////////////////////////


  Float_t tmp_evt_wt = eventWeight; //recored weight before corrections to update all weights at the end
  //weigh anti-iso tau region by anti-iso --> tight iso weight
  if((etau || mutau) && isLooseTau) {
    //use data factor for MC and Data, since not using MC estimated fake tau rates
    if(fUseJetToTauComposition) {
      jetToTauWeight = 0.;
      jetToTauWeightCorr = 0.;
      jetToTauWeightBias = 0.;
      jetToTauWeight_compUp = 0.;
      jetToTauWeight_compDown = 0.;
      Float_t jttUp(0.), jttDown(0.), jttSys(0.);
      if(mutau) {
        if(chargeTest) {
          fMuonJetToTauComp.GetComposition(tau->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                           muon->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                           fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        } else {
          fMuonJetToTauSSComp.GetComposition(tau->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                             muon->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                             fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        }
      } else {
        if(chargeTest) {
          fElectronJetToTauComp.GetComposition(tau->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                               electron->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                               fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        } else {
          fElectronJetToTauSSComp.GetComposition(tau->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                                 electron->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                                 fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        }
      }
      for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
        if(mutau) {
          fJetToTauWts[proc]   = (fMuonJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
                                                                            fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOneIso/leptonOneP4->Pt(),
                                                                            jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                                            fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                            jetToTauWeightCorrSys, fJetToTauBiases[proc]));
          fJetToTauMCWts[proc] = (fMuonJetToTauMCWeights[proc]->GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
                                                                              fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOneIso/leptonOneP4->Pt(),
                                                                              fJetToTauMCCorrs[proc], fJetToTauMCBiases[proc]));
        } else {
          fJetToTauWts[proc]   = (fElectronJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), electron->Pt(), electron->DeltaR(*tau),
                                                                                fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOneIso/leptonOneP4->Pt(),
                                                                                jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
                                                                                fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                                jetToTauWeightCorrSys, fJetToTauBiases[proc]));
          fJetToTauMCWts[proc] = (fElectronJetToTauMCWeights[proc]->GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), electron->Pt(), electron->DeltaR(*tau),
                                                                                  fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOneIso/leptonOneP4->Pt(),
                                                                                  fJetToTauMCCorrs[proc], fJetToTauMCBiases[proc]));
        }
        jetToTauWeight     += fJetToTauComps[proc] * fJetToTauWts[proc];
        jetToTauWeightCorr += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc]; //weight with the correction for this process
        jetToTauWeightBias += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc]; //weight with the correction and bias for this process
        //store systematic effects
        jttUp   += fJetToTauComps[proc] * jetToTauWeightUp  ;
        jttDown += fJetToTauComps[proc] * jetToTauWeightDown;
        jttSys  += fJetToTauComps[proc] * jetToTauWeightSys ;
        jetToTauWeight_compUp   += fJetToTauCompsUp  [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
        jetToTauWeight_compDown += fJetToTauCompsDown[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
      }
      jetToTauWeightUp   = jttUp  ;
      jetToTauWeightDown = jttDown;
      jetToTauWeightSys  = jttSys ;
      jetToTauWeightCorrUp   = jetToTauWeight; //set correction up to be ignoring the correction
      jetToTauWeightCorrDown = 2.*jetToTauWeightCorr - jetToTauWeight; //size of the weight in the other direction from 1
      jetToTauWeightCorrSys  = jetToTauWeight;
      jetToTauWeightBiasUp   = jetToTauWeightCorr; //set correction up to be ignoring the correction
      jetToTauWeightBiasDown = 2.*jetToTauWeightBias - jetToTauWeightCorr; //size of the weight in the other direction from 1
    // } else if(mutau) { //don't use j->tau composition
    //   jetToTauWeight = fMuonJetToTauWeight.GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
    //                                                      fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOneIso/leptonOneP4->Pt(),
    //                                                      jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
    //                                                      jetToTauWeightCorr, jetToTauWeightCorrUp, jetToTauWeightCorrDown,
    //                                                      jetToTauWeightCorrSys, jetToTauWeightBias);
    //   jetToTauWeightCorr *= jetToTauWeight;
    //   jetToTauWeightBias *= jetToTauWeightCorr;
    // } else if(etau) { //don't use j->tau composition
    //   jetToTauWeight = fElectronJetToTauWeight.GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), electron->Pt(), electron->DeltaR(*tau),
    //                                                          fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOneIso/leptonOneP4->Pt(),
    //                                                          jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
    //                                                          jetToTauWeightCorr, jetToTauWeightCorrUp, jetToTauWeightCorrDown,
    //                                                          jetToTauWeightCorrSys, jetToTauWeightBias);
    //   jetToTauWeightCorr *= jetToTauWeight;
    //   jetToTauWeightBias *= jetToTauWeightCorr;
    } else {
      std::cout << "Error! Jet to Tau weight without composition no longer supported!\n";
      throw 20;
    }
    //set j-->tau bias uncertainty to its size
    const Float_t bias_size = jetToTauWeightBias / jetToTauWeightCorr;
    jetToTauWeightBiasUp = jetToTauWeightCorr;
    jetToTauWeightBiasDown = (2.*bias_size - 1.)*jetToTauWeightCorr;
    jetToTauWeightGroup += SystematicGrouping::kJetToTau;
  }

  eventWeight *= jetToTauWeightBias;

  ///////////////////////
  // SS --> OS weights //
  ///////////////////////

  //get scale factor for same sign --> opposite sign, apply to MC and Data same-sign events
  if(emu && !chargeTest) {
    qcdWeight = fQCDWeight.GetWeight(fTreeVars.lepdeltar, fTreeVars.lepdeltaphi, fTreeVars.leponeeta, fTreeVars.leponept, fTreeVars.leptwopt,
                                     fYear, nJets, isLooseMuon, qcdClosure, qcdIsoScale, qcdWeightUp, qcdWeightDown, qcdWeightSys);
  }

  eventWeight *= qcdWeight;

  if(!std::isfinite(eventWeight) || !std::isfinite(genWeight)) {
    std::cout << __func__ << ": Warning!!! " << fentry << "point 2: Event weight not defined, setting to 0...\n";
    eventWeight = 0.;
    genWeight = 1.;
  }


  // /////////////////////////
  // // Jet --> lep weights //
  // /////////////////////////

  // if(mutau && isLooseMuon) {
  //   //use data factor for MC and Data, since not using MC estimated fake tau rates
  //   eventWeight *= fJetToMuonWeight.GetDataFactor(fYear, muon->Pt(), muon->Eta());
  // } else if(etau && isLooseElectron) {
  //   //use data factor for MC and Data, since not using MC estimated fake tau rates
  //   eventWeight *= fJetToElectronWeight.GetDataFactor(fYear, electron->Pt(), electron->Eta());
  // }

  if(eventWeight != tmp_evt_wt) { //update tree var weights if needed
    fTreeVars.eventweight        *= eventWeight/tmp_evt_wt;
    fTreeVars.fulleventweight    *= eventWeight/tmp_evt_wt;
    fTreeVars.fulleventweightlum *= eventWeight/tmp_evt_wt;
    fTreeVars.eventweightMVA     *= eventWeight/tmp_evt_wt;
  }
  tmp_evt_wt = eventWeight;


  ////////////////////////////////////////////////////////////
  // jet --> tau cuts and region definitions
  ////////////////////////////////////////////////////////////

  const double met_cut       = 60.;
  const double mtlep_cut     = 70.;
  const double qcd_mtlep_cut = (etau) ? 45. : mtlep_cut;
  // const bool qcdSelection     = (etau || (mutau && leptonOneIso/fTreeVars.leponept > 0.05)) && fTreeVars.mtlep < 40. && met < met_cut && nBJetsUse == 0;
  const bool looseQCDRegion    = fTreeVars.mtlep < mtlep_cut && met < met_cut && nBJetsUse == 0; // no isolation cut (mutau, 0 - 0.5 allowed) and electron WPL + tight ID
  const bool qcdSelection      = looseQCDRegion && fTreeVars.mtlep < qcd_mtlep_cut && (etau || (mutau && fTreeVars.leponereliso > 0.05)) && !(isLooseMuon || isLooseElectron);
  const bool wjetsSelection    = (etau || mutau) && fTreeVars.mtlep > 70. && nBJetsUse == 0 && !(isLooseMuon || isLooseElectron);
  const bool topSelection      = (etau || mutau) && nBJetsUse >= 1 && !(isLooseMuon || isLooseElectron);
  const bool nominalSelection  = nBJetsUse == 0 && met < met_cut && fTreeVars.mtlep < mtlep_cut && !(isLooseMuon || isLooseElectron);
  // const bool looseNominal      = nBJetsUse == 0 && !(isLooseMuon || isLooseElectron); //no MET or MT(ll, MET) cuts

  ////////////////////////////////////////////////////////////
  // Set 92 (93) + selection offset: QCD selection without iso/mtlep cut with (without) MC taus
  ////////////////////////////////////////////////////////////

  if((etau || mutau) && looseQCDRegion) {
    int qcd_set_offset = set_offset;
    if(looseQCDSelection && !isLooseTau && (isLooseMuon || isLooseElectron)) qcd_set_offset -= fMisIDOffset; //remove offset if the loose lepton isn't the tau
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = fUseJetToTauComposition && isLooseTau;
    const bool isfake = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);
    //QCD weights from DR
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr;//*fJetToTauBiases[JetToTauComposition::kQCD];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(mutau || (etau && !isLooseElectron)) { //full isolation/mtlep region for isolation/mtlep bias measurement, no bias corrections
      FillAllHistograms(qcd_set_offset + 92);
      if(!isfake) FillAllHistograms(qcd_set_offset + 93);
    }
    if(fTreeVars.mtlep < qcd_mtlep_cut && ((mutau && fTreeVars.leponereliso > 0.15) || (etau && isLooseElectron))) { //high isolation/loose ID region for SS --> OS bias measurement
      //QCD weights from high iso region, no bias corrections (stored in MC weights, but high iso weights)
      if(add_wt) {
        jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kQCD];
        jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kQCD];
        jetToTauWeightBias = jetToTauWeightCorr;//*fJetToTauMCBiases[JetToTauComposition::kQCD];
        eventWeight        = jetToTauWeightBias*evt_wt_bare;
      }
      FillAllHistograms(qcd_set_offset + 94);
      if(!isfake) FillAllHistograms(qcd_set_offset + 95);
    }
    //Restore the previous weights
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }


  //////////////////////////
  // Handle loose leptons //
  //////////////////////////

  //FIXME: add back in loose light leptons
  mutau &= !isLooseMuon;
  etau  &= !isLooseElectron;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //12
  ++icutflow;

  // ////////////////////////////////////////////////////////////
  // // Set 90 (91) + selection offset: loose nominal selection with (without) MC fake taus
  // ////////////////////////////////////////////////////////////

  // if((emu || etau || mutau) && looseNominal) {
  //   const bool isfake = !fIsData && (((mutau || etau) && std::abs(tauGenFlavor) == 26) || isFakeElectron || isFakeMuon);
  //   FillAllHistograms(set_offset + 90);
  //   if(!isfake) FillAllHistograms(set_offset + 91);
  // }

  ////////////////////////////////////////////////////////////
  // Set 35 + selection offset: last set with MC estimated taus and leptons
  ////////////////////////////////////////////////////////////

  if(!(mumu || ee || emu) && nominalSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = fUseJetToTauComposition && isLooseTau;
    const bool isfake = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);

    //Nominal mixture
    FillAllHistograms(set_offset + 35);
    //fake tau only contribution
    if(isfake) FillAllHistograms(set_offset + 42);

    //Nominal mixture, without j-->tau weights
    if(add_wt) {
      jetToTauWeight     = 1.f;
      jetToTauWeightCorr = 1.f;
      jetToTauWeightBias = 1.f;
      eventWeight        = evt_wt_bare;
    }
    if(isLooseTau) {
      FillAllHistograms(set_offset + 33); //only fill for loose ID taus, since non-loose already have no j-->tau weights
    }
    // //QCD weights from DR
    // if(add_wt) {
    //   jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
    //   jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
    //   jetToTauWeightBias = jetToTauWeightCorr*fJetToTauBiases[JetToTauComposition::kQCD];
    //   eventWeight        = jetToTauWeightBias*evt_wt_bare;
    // }
    // FillAllHistograms(set_offset + 80);
    // if(!isfake) { //no fake MC events
    //   FillAllHistograms(set_offset + 85);
    // }

    //W+Jets data based weights from DR to test W+Jets only vs composition
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauBiases[JetToTauComposition::kWJets];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 34);
    }

    //W+Jets MC based weights from DR for bias tests
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauMCBiases[JetToTauComposition::kWJets];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 81);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 86);
    }
    //Top MC based weights from DR (nominal weights are MC in this region anyway)
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauMCBiases[JetToTauComposition::kTop];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 82);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 87);
    }
    //Restore the previous weights
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 36 + selection offset: QCD selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((fQCDFakeTauTesting || !fDYTesting) && qcdSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 36);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 37 + selection offset: W+Jets selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((fWJFakeTauTesting || !fDYTesting) && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool    add_wt      = fUseJetToTauComposition && isLooseTau;
    const bool    isfake      = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);
    //Data measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 37);
    //MC measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                 [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs[JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 88);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 89);
    }
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 38 + selection offset: ttbar selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((fTTFakeTauTesting || !fDYTesting) && (mutau || etau) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 38);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  // ////////////////////////////////////////////////////////////
  // // Set 38 + selection offset: MC estimated j->tau only
  // ////////////////////////////////////////////////////////////

  // if((!fDYTesting || !(mumu || ee || emu)) && (fIsData || tauGenFlavor == 26) && nBJetsUse == 0) {
  //   FillAllHistograms(set_offset + 38);
  // }

  // ////////////////////////////////////////////////////////////
  // // Set 40-41 + selection offset: Fake taus estimated with MC estimated scale factors
  // ////////////////////////////////////////////////////////////

  // if((!fDYTesting || fJetTauTesting) && (mutau || etau) && nBJetsUse == 0) {
  //   //don't care about the MC uncertainties in MC study
  //   Float_t tmp_jwt(jetToTauWeight), tmp_jcr(jetToTauWeightCorr), tmp_jbs(jetToTauWeightBias);
  //   if(isLooseTau && mutau) {
  //     jetToTauWeight = fMuonJetToTauMCWeight.GetDataFactor(tauDecayMode, fYear, tau->Pt(), tau->Eta(), muon->Pt(), muon->DeltaR(*tau),
  //                                                          fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep,
  //                                                          jetToTauWeightUp, jetToTauWeightDown, jetToTauWeightSys, jetToTauWeightGroup,
  //                                                          jetToTauWeightCorr, jetToTauWeightCorrUp, jetToTauWeightCorrDown,
  //                                                          jetToTauWeightCorrSys, jetToTauWeightBias);
  //     jetToTauWeightCorr *= jetToTauWeight;
  //     jetToTauWeightBias *= jetToTauWeightCorr;
  //     eventWeight *= jetToTauWeightBias;
  //   } else {
  //     jetToTauWeight = 1.;
  //     jetToTauWeightCorr = 1.;
  //   }
  //   if(tauGenFlavor != 26) {
  //     FillAllHistograms(set_offset + 40); //true tight taus or loose taus
  //   } else {
  //     FillAllHistograms(set_offset + 41); //fake tight taus or loose taus
  //   }
  //   jetToTauWeight     = tmp_jwt;
  //   jetToTauWeightCorr = tmp_jcr;
  //   jetToTauWeightBias = tmp_jbs;
  // }
  // eventWeight = tmp_evt_wt; //restore the proper event weight

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

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  //initialize systematic weights for TMVA systematic testing, remove non-closure correction as an estimate
  fTreeVars.jettotaunonclosure = (jetToTauWeight / jetToTauWeightCorr) * (jetToTauWeightBias / jetToTauWeightCorr);
  fTreeVars.zptup = zPtWeightUp / zPtWeight;
  fTreeVars.zptdown = zPtWeightDown / zPtWeight;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //13
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 30 + selection offset: QCD selection
  ////////////////////////////////////////////////////////////////////////////
  if((fQCDFakeTauTesting || !fDYTesting) && qcdSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 30);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 31 + selection offset: W+Jets selection
  ////////////////////////////////////////////////////////////////////////////
  if((fWJFakeTauTesting || !fDYTesting) && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 31);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 32 + selection offset: Top selection
  ////////////////////////////////////////////////////////////////////////////
  if((fTTFakeTauTesting || !fDYTesting) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 32);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  mutau &= met < met_cut;
  etau  &= met < met_cut;
  emu   &= met < met_cut;
  // mumu  &= met < 60.;
  // ee    &= met < 60.;

  //Add W+Jets selection orthogonality condition
  mutau &= fTreeVars.mtlep < mtlep_cut;
  etau  &= fTreeVars.mtlep < mtlep_cut;
  emu   &= fTreeVars.mtlep < mtlep_cut;
  // mumu  &= fTreeVars.mtlep < 70.;
  // ee    &= fTreeVars.mtlep < 70.;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  ////////////////////////////////////////////////////////////////////////////
  // Set 7 + selection offset: No MC estimated fake taus, no b-jet cut
  ////////////////////////////////////////////////////////////////////////////
  if((!fDYTesting || (mumu || ee)) && fDoSystematics >= 0)
    FillAllHistograms(set_offset + 7);

  if(fCutFlowTesting) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //14
  ++icutflow;

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  mutau &= nBJetsUse == 0;
  etau  &= nBJetsUse == 0;
  emu   &= nBJetsUse == 0;
  mumu  &= nBJetsUse == 0;
  ee    &= nBJetsUse == 0;

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


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
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;


  if(!looseQCDSelection && chargeTest)                                          {fCutFlow->Fill(icutflow);} //15
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight) > 0.)             {fCutFlow->Fill(icutflow);} //16
  ++icutflow;
  if(!looseQCDSelection && chargeTest && std::fabs(genWeight*eventWeight) > 0.) {fCutFlow->Fill(icutflow);} //17
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 8);

  if((emu || mumu || ee) && (!fDYTesting || fTriggerTesting)) {
    if(emu) { //e+mu trigger testing
      ////////////////////////////////////////////////////////////////////////////
      // Set 60 + selection offset: Electron triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonOneP4->Pt() > electron_trig_pt_) {
        FillAllHistograms(set_offset + 60);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 61 + selection offset: Muon triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoFired && leptonTwoP4->Pt() > muon_trig_pt_) {
        FillAllHistograms(set_offset + 61);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 62 + selection offset: Both leptons triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonTwoFired) {
        FillAllHistograms(set_offset + 62);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 63 + selection offset: Electron triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneP4->Pt() > electron_trig_pt_) {
        FillAllHistograms(set_offset + 63);
      }
      ////////////////////////////////////////////////////////////////////////////
      // Set 66 + selection offset: electron not triggerable
      ////////////////////////////////////////////////////////////////////////////
      else {
        FillAllHistograms(set_offset + 66);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 64 + selection offset: muon triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoP4->Pt() > muon_trig_pt_) {
        FillAllHistograms(set_offset + 64);
      }
      ////////////////////////////////////////////////////////////////////////////
      // Set 67 + selection offset: muon not triggerable
      ////////////////////////////////////////////////////////////////////////////
      else {
        FillAllHistograms(set_offset + 67);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 65 + selection offset: Electron+Muon triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneP4->Pt() > electron_trig_pt_ && leptonTwoP4->Pt() > muon_trig_pt_) {
        FillAllHistograms(set_offset + 65);
      }
    } else { //ee and mumu trigger testing
      const double trig_threshold = (ee) ? electron_trig_pt_ : muon_trig_pt_;

      ////////////////////////////////////////////////////////////////////////////
      // Set 60 + selection offset: lepton one triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonOneP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 60);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 61 + selection offset: lepton two triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoFired && leptonTwoP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 61);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 62 + selection offset: both leptons triggered
      ////////////////////////////////////////////////////////////////////////////
      if(leptonOneFired && leptonOneP4->Pt() > trig_threshold && leptonTwoFired && leptonTwoP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 62);
      }

      ////////////////////////////////////////////////////////////////////////////
      // Set 63 + selection offset: lepton two triggerable
      ////////////////////////////////////////////////////////////////////////////
      if(leptonTwoP4->Pt() > trig_threshold) {
        FillAllHistograms(set_offset + 63);
      }
      ////////////////////////////////////////////////////////////////////////////
      // Set 66 + selection offset: lepton two not triggerable
      ////////////////////////////////////////////////////////////////////////////
      else {
        FillAllHistograms(set_offset + 66);
      }

    }
  }

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
  eventWeight = (fIsData == 0 || !chargeTest) ? std::fabs(fTreeVars.eventweightMVA) : std::fabs(prev_wt); //use abs to remove gen weight sign
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



  // ////////////////////////////////////////////////////////////////////////////
  // // Set 27 + selection offset: Genuine hadronic taus
  // ////////////////////////////////////////////////////////////////////////////
  // bool genuineTau = fIsData != 0 || std::abs(tauGenFlavor) == 15;
  // genuineTau &= mutau || etau;
  // if(genuineTau)
  //   FillAllHistograms(set_offset + 27);

  ////////////////////////////////////////////////////////////////////////////
  // Set 28 + selection offset: Non-jet based hadronic taus
  ////////////////////////////////////////////////////////////////////////////
  // bool nonJetTau = fIsData != 0 || std::abs(tauGenFlavor) != 26;
  // nonJetTau &= mutau || etau;
  // if(nonJetTau)
  //   FillAllHistograms(set_offset + 28);


  return kTRUE;
}


//--------------------------------------------------------------------------------------------------------------
// Match trigger objects to selected leptons
void CLFVHistMaker::MatchTriggers() {
  // int trigMatchOne =  0; //trigger matching result
  // int trigMatchTwo =  0; //trigger matching result
  // int trigIndexOne = -1;
  // int trigIndexTwo = -1;

  //FIXME: Do actual trigger matching
  const bool mu_trig  = ((fYear == 2016 && HLT_IsoMu24) ||
                        ( fYear == 2017 && HLT_IsoMu27) ||
                        ( fYear == 2018 && HLT_IsoMu24));
  const bool ele_trig = ((fYear == 2016 && HLT_Ele27_WPTight_GsF) ||
                        ( fYear == 2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) ||
                        ( fYear == 2018 && HLT_Ele32_WPTight_GsF));

  if(std::abs(leptonOneFlavor) == 15) {
    leptonOneFired = false;
  } else if(std::abs(leptonOneFlavor) == 13) {
    leptonOneFired = mu_trig && leptonOneP4->Pt() > muon_trig_pt_;
  } else if(std::abs(leptonOneFlavor) == 11) {
    leptonOneFired = ele_trig && leptonOneP4->Pt() > electron_trig_pt_;
  }

  if(std::abs(leptonTwoFlavor) == 15) {
    leptonTwoFired = false;
  } else if(std::abs(leptonTwoFlavor) == 13) {
    leptonTwoFired = mu_trig && leptonTwoP4->Pt() > muon_trig_pt_;
  } else if(std::abs(leptonTwoFlavor) == 11) {
    leptonTwoFired = ele_trig && leptonTwoP4->Pt() > electron_trig_pt_;
  }

  triggerLeptonStatus = 0;
  if((std::abs(leptonOneFlavor) == 11 && leptonOneFired) ||
     (std::abs(leptonTwoFlavor) == 11 && leptonTwoFired)) {
    triggerLeptonStatus += 1;
  }
  if((std::abs(leptonOneFlavor) == 13 && leptonOneFired) ||
     (std::abs(leptonTwoFlavor) == 13 && leptonTwoFired)) {
    triggerLeptonStatus += 2;
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::ApplyTriggerWeights() {
  float data_eff[2] = {0.5f, 0.5f}; //set to 0.5 so no danger in doing the ratio of eff or 1 - eff
  float mc_eff[2]   = {0.5f, 0.5f};

  ////////////////////////////////
  // Get the efficiencies
  ////////////////////////////////

  // Electrons
  if(std::abs(leptonOneFlavor) == 11) { //lepton 1 is an electron
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.ElectronTriggerWeight(leptonOneP4->Pt(), leptonOneSCEta, fYear, data_eff[0], mc_eff[0], isLooseElectron, mcEra);
      } else {
        fEmbeddingWeight.ElectronTriggerWeight   (leptonOneP4->Pt(), leptonOneSCEta, fYear, data_eff[0], mc_eff[0]);
      }
    } else {
      fElectronIDWeight.TriggerEff               (leptonOneP4->Pt(), leptonOneSCEta, fYear,
                                                  (leptonOneID1 > 1) ? ElectronIDWeight::kWP80 : ElectronIDWeight::kWPLNotWP80, data_eff[0], mc_eff[0]);
    }
  }
  if(std::abs(leptonTwoFlavor) == 11) { //lepton 2 is an electron
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.ElectronTriggerWeight(leptonTwoP4->Pt(), leptonTwoSCEta, fYear, data_eff[1], mc_eff[1], isLooseElectron, mcEra);
      } else {
        fEmbeddingWeight.ElectronTriggerWeight   (leptonTwoP4->Pt(), leptonTwoSCEta, fYear, data_eff[1], mc_eff[1]);
      }
    } else {
      fElectronIDWeight.TriggerEff               (leptonTwoP4->Pt(), leptonTwoSCEta, fYear,
                                                  (leptonOneID1 > 1) ? ElectronIDWeight::kWP80 : ElectronIDWeight::kWPLNotWP80, data_eff[1], mc_eff[1]);
    }
  }

  // Muons
  if(std::abs(leptonTwoFlavor) == 13) { //lepton 2 is a muon
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.MuonTriggerWeight(leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, data_eff[1], mc_eff[1], isLooseMuon, mcEra);
      } else {
        fEmbeddingWeight.MuonTriggerWeight   (leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, data_eff[1], mc_eff[1]);
      }
    } else {
      //use low trigger in case the lepton didn't fire a trigger
      fMuonIDWeight.TriggerEff               (leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, mcEra, !leptonTwoFired || muonTriggerStatus != 2, data_eff[1], mc_eff[1]);
    }
  }
  if(std::abs(leptonOneFlavor) == 13) { //lepton 1 is a muon
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.MuonTriggerWeight(leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, data_eff[0], mc_eff[0], isLooseMuon, mcEra);
      } else {
        fEmbeddingWeight.MuonTriggerWeight   (leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, data_eff[0], mc_eff[0]);
      }
    } else {
      fMuonIDWeight.TriggerEff               (leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, mcEra, !leptonOneFired || muonTriggerStatus != 2, data_eff[0], mc_eff[0]);
    }
  }

  // Taus
  if(std::abs(leptonTwoFlavor) == 15) {
    data_eff[1] = 0.f; mc_eff[1] = 0.f; //tau can't trigger
  }

  ////////////////////////////////
  // Apply the corrections
  ////////////////////////////////

  const float pt_1(leptonOneP4->Pt()), pt_2(leptonTwoP4->Pt());
  const float min_pt_1 = (std::abs(leptonOneFlavor) == 13) ? muon_trig_pt_ : electron_trig_pt_;
  const float min_pt_2 = (std::abs(leptonTwoFlavor) == 13) ? muon_trig_pt_ : electron_trig_pt_;
  //use just the muon if there's a muon or the leading firing lepton otherwise
  if(fRemoveTriggerWeights == 2) {
    if((leptonOneFired && pt_1 >= min_pt_1) && (std::abs(leptonOneFlavor) == 13 || !leptonTwoFired || std::abs(leptonTwoFlavor) != 13)) {
      leptonOneTrigWeight = data_eff[0] / mc_eff[0];
    } else {
      leptonTwoTrigWeight = data_eff[1] / mc_eff[1];
    }
  }

  //use the probability of the event, given whether or not each lepton triggered
  else if(fRemoveTriggerWeights == 3) {
    //P(event) = P(lep 1 status) * P(lep 2 status)
    //SF = P(event | Data) / P(event | MC) = (P(1 | Data) * P(2 | Data)) / (P(1 | MC) * P(2 | MC))
    //SF = (P(1 | Data) / P(1 | MC)) * (P(2 | Data) / P(2 | MC))

    //if a lepton couldn't fire, ensure no weight effect
    if(pt_1 < min_pt_1) {
      leptonOneTrigWeight = 1.;
    } else {
      leptonOneTrigWeight = (leptonOneFired) ? data_eff[0] / mc_eff[0] : (1.-data_eff[0])/(1.-mc_eff[0]);
    }
    if(std::abs(leptonTwoFlavor) == 15 || pt_2 < min_pt_2) {
      leptonTwoTrigWeight = 1.;
    } else {
      leptonTwoTrigWeight = (leptonTwoFired) ? data_eff[1] / mc_eff[1] : (1.-data_eff[1])/(1.-mc_eff[1]);
    }
  }

  //use the probability that at least one fires
  else if(fRemoveTriggerWeights == 4) {
    // P(at least 1 fires) = 1 - P(neither fires) = 1 - Product of P(doesn't fire)
    float prob_data(1.f), prob_mc(1.f);
    //check if each is triggerable, and if so apply the P(!fire) for MC and Data
    if(pt_1 > min_pt_1) {
      prob_data *= (1.f - data_eff[0]);
      prob_mc   *= (1.f - mc_eff[0]  );
    }
    if(pt_2 > min_pt_2 && std::abs(leptonTwoFlavor) != 15) {
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
    prob_data = std::max(0.f, std::min(0.9999f, prob_data)); //avoid a ~1/0  or sqrt(negative) situation
    prob_mc   = std::max(0.f, std::min(0.9999f, prob_mc  ));
    const float trig_wt = (1.f - prob_data) / (1.f - prob_mc);
    if(std::abs(leptonTwoFlavor) == 15 || pt_2 < min_pt_2) { //no need to split if other lepton can't fire
      leptonOneTrigWeight = trig_wt;
    } else if(pt_1 < min_pt_1) { //only lepton two could (and did) fire
      leptonTwoTrigWeight = trig_wt;
    } else { //both could fire, so equally split the weight between the two
      leptonOneTrigWeight = std::sqrt(trig_wt);
      leptonTwoTrigWeight = std::sqrt(trig_wt);
    }
  }

  if(leptonOneTrigWeight < 0. || leptonTwoTrigWeight < 0. || !std::isfinite(leptonOneTrigWeight) || !std::isfinite(leptonTwoTrigWeight)) {
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

//--------------------------------------------------------------------------------------------------------------
//determine MVA category
int CLFVHistMaker::Category(TString selection) {
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

//--------------------------------------------------------------------------------------------------------------
//initialize the randomly assigned systematic shifts
void CLFVHistMaker::InitializeSystematics() {
  leptonOneWeight1_group = 0; leptonOneWeight2_group = 0;
  leptonTwoWeight1_group = 0; leptonTwoWeight2_group = 0;
  if(std::abs(leptonOneFlavor) == 11) {
    leptonOneWeight1_sys = fSystematicShifts->ElectronID    (fYear, leptonOneWeight1_bin) ? leptonOneWeight1_up : leptonOneWeight1_down;
    leptonOneWeight2_sys = fSystematicShifts->ElectronRecoID(fYear, leptonOneWeight2_bin) ? leptonOneWeight2_up : leptonOneWeight2_down;
    leptonOneWeight1_group = fElectronIDWeight.GetIDGroup(leptonOneWeight1_bin, fYear) + SystematicGrouping::kElectronID;
    leptonOneWeight2_group = fElectronIDWeight.GetRecoGroup(leptonOneWeight2_bin, fYear) + SystematicGrouping::kElectronRecoID;
  } else if(std::abs(leptonOneFlavor) == 13) {
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
  } else if(std::abs(leptonOneFlavor) == 15) {
    int tauGenID = 0;
    if     (std::abs(tauGenFlavor) == 15) tauGenID = 5;
    else if(std::abs(tauGenFlavor) == 13) tauGenID = 2;
    else if(std::abs(tauGenFlavor) == 11) tauGenID = 1;
    leptonOneWeight1_sys = fSystematicShifts->TauID(fYear, tauGenID, leptonOneWeight1_bin) ? leptonOneWeight1_up : leptonOneWeight1_down;
    leptonOneWeight2_sys = leptonOneWeight2; //not defined
  }
  if(std::abs(leptonTwoFlavor) == 11) {
    leptonTwoWeight1_sys = fSystematicShifts->ElectronID    (fYear, leptonTwoWeight1_bin) ? leptonTwoWeight1_up : leptonTwoWeight1_down;
    leptonTwoWeight2_sys = fSystematicShifts->ElectronRecoID(fYear, leptonTwoWeight2_bin) ? leptonTwoWeight2_up : leptonTwoWeight2_down;
    leptonTwoWeight1_group = fElectronIDWeight.GetIDGroup(leptonTwoWeight1_bin, fYear) + SystematicGrouping::kElectronID;
    leptonTwoWeight2_group = fElectronIDWeight.GetRecoGroup(leptonTwoWeight2_bin, fYear) + SystematicGrouping::kElectronRecoID;
  } else if(std::abs(leptonTwoFlavor) == 13) {
    leptonTwoWeight1_sys = fSystematicShifts->MuonID   (fYear, leptonTwoWeight1_bin) ? leptonTwoWeight1_up : leptonTwoWeight1_down;
    leptonTwoWeight2_sys = fSystematicShifts->MuonIsoID(fYear, leptonTwoWeight2_bin) ? leptonTwoWeight2_up : leptonTwoWeight2_down;
    leptonTwoWeight1_group = fMuonIDWeight.GetIDGroup(leptonOneWeight1_bin, fYear) + SystematicGrouping::kMuonID;
    leptonTwoWeight2_group = fMuonIDWeight.GetIsoGroup(leptonOneWeight2_bin, fYear) + SystematicGrouping::kMuonIsoID;
  } else if(std::abs(leptonTwoFlavor) == 15) {
    int tauGenID = 0;
    if     (std::abs(tauGenFlavor) == 15) tauGenID = 5;
    else if(std::abs(tauGenFlavor) == 13) tauGenID = 2;
    else if(std::abs(tauGenFlavor) == 11) tauGenID = 1;
    leptonTwoWeight1_sys = fSystematicShifts->TauID(fYear, tauGenID, leptonTwoWeight1_bin) ? leptonTwoWeight1_up : leptonTwoWeight1_down;
    leptonTwoWeight2_sys = leptonTwoWeight2; //not defined
  }
}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::ProcessLLGStudy() {

}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

//--------------------------------------------------------------------------------------------------------------
void CLFVHistMaker::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s\n",fOut->GetName());
  fTopDir->cd();
  fCutFlow->Write(); //add the cut-flow histogram to the output
  fOut->Write();//Form("%s_OutFile.root",fChain->GetName()));
  fOut->Close();
  delete fOut;
  // timer->Stop();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);


}
