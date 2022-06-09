#define HISTMAKER_CXX

#include "interface/HistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
HistMaker::HistMaker(int seed, TTree * /*tree*/) : fSystematicSeed(seed),
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
  fChain = nullptr;
  fCutFlow = nullptr;
  for(int i = 0; i < fn; i++) {
    fEventHist     [i] = nullptr;
    fLepHist       [i] = nullptr;
    fPhotonHist    [i] = nullptr;
    fSystematicHist[i] = nullptr;
  }
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    fMuonJetToTauWeights    [proc] = nullptr;
    fElectronJetToTauWeights[proc] = nullptr;
  }
  fRnd = nullptr;
  fSystematicShifts = nullptr;

  leptonOneP4 = new TLorentzVector();
  leptonTwoP4 = new TLorentzVector();
  jetOneP4    = new TLorentzVector();
  photonP4    = nullptr;
}

//--------------------------------------------------------------------------------------------------------------
HistMaker::~HistMaker() {
  DeleteHistograms();
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    if(fMuonJetToTauWeights      [proc]) {delete fMuonJetToTauWeights      [proc]; fMuonJetToTauWeights      [proc] = nullptr;}
    if(fMuonJetToTauMCWeights    [proc]) {delete fMuonJetToTauMCWeights    [proc]; fMuonJetToTauMCWeights    [proc] = nullptr;}
    if(fElectronJetToTauWeights  [proc]) {delete fElectronJetToTauWeights  [proc]; fElectronJetToTauWeights  [proc] = nullptr;}
    if(fElectronJetToTauMCWeights[proc]) {delete fElectronJetToTauMCWeights[proc]; fElectronJetToTauMCWeights[proc] = nullptr;}
  }

  if(fCutFlow          ) {delete fCutFlow         ; fCutFlow          = nullptr;}
  if(fTauIDWeight      ) {delete fTauIDWeight     ; fTauIDWeight      = nullptr;}
  if(fSystematicShifts ) {delete fSystematicShifts; fSystematicShifts = nullptr;}
  if(fRnd              ) {delete fRnd             ; fRnd              = nullptr;}
  if(leptonOneP4       ) {delete leptonOneP4      ; leptonOneP4       = nullptr;}
  if(leptonTwoP4       ) {delete leptonTwoP4      ; leptonTwoP4       = nullptr;}
  if(jetOneP4          ) {delete jetOneP4         ; jetOneP4          = nullptr;}
  if(photonP4          ) {delete photonP4         ; photonP4          = nullptr;}
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();
  std::setbuf(stdout, NULL); //don't buffer the stdout stream
  printf("HistMaker::Begin\n");
  timer->Start();

  //Initialize time analysis fields
  for(int itime = 0; itime < fNTimes; ++itime) {
    fDurations[itime] = 0.; fTimeCounts[itime] = 0;
  }
  fTimes[0] = std::chrono::steady_clock::now();

  TString dir = gSystem->Getenv("CMSSW_BASE");
  if(dir == "") dir = "../"; //if not in a CMSSW environment, assume in a sub-directory of the package
  else dir += "/src/CLFVAnalysis/";
  fRoccoR = new RoccoR(Form("%sscale_factors/RoccoR%i.txt", dir.Data(), fYear));
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

  fRnd = new TRandom3(fRndSeed);
  fTreeVars.type = -1;

  //Initialize MVA tools
  TMVA::Tools::Instance(); //load the TMVA library
  for(int i = 0; i < kMaxMVAs; ++i) mva[i] = 0; //initially 0s

  if(fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {

      mva[mva_i] = new TMVA::Reader("!Color:!Silent");
      TString selection = "";
      if(fMVAConfig.names_[mva_i].Contains("higgs")) selection += "h";
      else selection += "z";
      if(fMVAConfig.names_[mva_i].Contains("mutau")) selection += "mutau";
      else if(fMVAConfig.names_[mva_i].Contains("etau")) selection += "etau";
      else if(fMVAConfig.names_[mva_i].Contains("emu")) selection += "emu";
      else {
        printf ("Warning! Didn't determine mva weight file %s corresponding selection!\n",
                fMVAConfig.names_[mva_i].Data());
        selection += "mutau"; //just to default to something
      }
      //add for leptonic tau channels FIXME: Put as part of original check
      if(fMVAConfig.names_[mva_i].Contains("mutau_e")) selection += "_e";
      else if(fMVAConfig.names_[mva_i].Contains("etau_mu")) selection += "_mu";

      Int_t isJetBinned = -1; // -1 is not binned, 0 = 0 jets, 1 = 1 jet, 2 = >1 jets
      if(fMVAConfig.names_[mva_i].Contains("_18") || //0 jet
         fMVAConfig.names_[mva_i].Contains("_48") ||
         fMVAConfig.names_[mva_i].Contains("_78"))
        isJetBinned = 0;
      else if(fMVAConfig.names_[mva_i].Contains("_19") || //1 jet
              fMVAConfig.names_[mva_i].Contains("_49") ||
              fMVAConfig.names_[mva_i].Contains("_79"))
        isJetBinned = 1;
      else if(fMVAConfig.names_[mva_i].Contains("_20") || //>1 jet
              fMVAConfig.names_[mva_i].Contains("_50") ||
              fMVAConfig.names_[mva_i].Contains("_80"))
        isJetBinned = 2;

      fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling

      printf("Using selection %s\n", selection.Data());
      //use a tool to initialize the MVA variables and spectators
      TrkQualInit trkQualInit(fTrkQualVersion, isJetBinned);
      trkQualInit.InitializeVariables(*(mva[mva_i]), selection, fTreeVars);
      //Initialize MVA weight file
      const char* f = Form("weights/%s.weights.xml",fMVAConfig.names_[mva_i].Data());
      mva[mva_i]->BookMVA(fMVAConfig.names_[mva_i].Data(),f);
      printf("Booked MVA %s with selection %s\n", fMVAConfig.names_[mva_i].Data(), selection.Data());
    }
  }

  //Create the output file
  fOut = new TFile(GetOutputName(), "RECREATE","HistMaker output histogram file");
  fTopDir = fOut->mkdir("Data");
  fTopDir->cd();
  std::cout << "Using output filename " << GetOutputName() << std::endl;

  for(int ihist = 0; ihist < fn; ++ihist) {
    fEventHist     [ihist] = nullptr;
    fLepHist       [ihist] = nullptr;
    fPhotonHist    [ihist] = nullptr;
    fSystematicHist[ihist] = nullptr;
  }

  //Setup output histograms
  InitHistogramFlags();
  BookHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("HistMaker::SlaveBegin\n");
  TString option = GetOption();

}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::FillAllHistograms(Int_t index) {
  if(eventWeight < 0. || !std::isfinite(eventWeight*genWeight)) {
    std::cout << "WARNING! Event " << fentry << ": Set " << index << " has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << genWeight << "; puWeight = " << puWeight
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOneTrigWeight*leptonTwoTrigWeight
              << "; jetPUIDWeight = " << jetPUIDWeight << "; zPtWeight = " << zPtWeight
              << std::endl;
  }
  if(!std::isfinite(eventWeight*genWeight)) {
    eventWeight = 0.;
    genWeight = 1.;
  }
  if(fEventSets [index]) {
    if(fVerbose > 0) std::cout << "Filling histograms for set " << index
                               << " with event weight = " << eventWeight
                               << " and gen weight = " << genWeight << " !\n";
    FillEventHistogram( fEventHist [index]);
    FillLepHistogram(   fLepHist   [index]);
    if(fDoSystematics && fSysSets[index]) FillSystematicHistogram(fSystematicHist[index]);
    // if(fFillPhotonHists) FillPhotonHistogram(fPhotonHist[index]);
  } else
    printf("WARNING! Entry %lld: Attempted to fill un-initialized histogram set %i!\n", fentry, index);
  if(fDoSystematics >= 0 && fWriteTrees && fTreeSets[index])
    fTrees[index]->Fill();
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::InitHistogramFlags() {
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
void HistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  // BookPhotonHistograms();
  if(fWriteTrees && fDoSystematics >= 0) BookTrees();
  if(fDoSystematics) BookSystematicHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::BookBaseEventHistograms(Int_t i, const char* dirname) {
      auto folder = fDirectories[0*fn + i];
      Utilities::BookH1F(fEventHist[i]->hEventWeight             , "eventweight"             , Form("%s: EventWeight"                 ,dirname), 100,   -1,   3, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEventWeight          , "logeventweight"          , Form("%s: LogEventWeight"              ,dirname),  50,  -10,   1, folder);
      Utilities::BookH1F(fEventHist[i]->hFullEventWeightLum      , "fulleventweightlum"      , Form("%s: abs(FullEventWeightLum)"     ,dirname), 100,    0,  15, folder);
      Utilities::BookH1F(fEventHist[i]->hLogFullEventWeightLum   , "logfulleventweightlum"   , Form("%s: log(abs(FullEventWeightLum))",dirname),  50,  -10,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hEventWeightMVA          , "eventweightmva"          , Form("%s: EventWeightMVA"              ,dirname), 100,   -1,   5, folder);
      Utilities::BookH1D(fEventHist[i]->hGenWeight               , "genweight"               , Form("%s: GenWeight"                   ,dirname),   5, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hEmbeddingWeight         , "embeddingweight"         , Form("%s: EmbeddingWeight"             ,dirname), 100,    0, 0.5, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEmbeddingWeight      , "logembeddingweight"      , Form("%s: Log10(EmbeddingWeight)"      ,dirname),  40,   -9,   1, folder);
      Utilities::BookH1F(fEventHist[i]->hEmbeddingUnfoldingWeight, "embeddingunfoldingweight", Form("%s: EmbeddingUnfoldingWeight"    ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hQCDWeight               , "qcdweight"               , Form("%s: QCDWeight"                   ,dirname),  50,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeight          , "jettotauweight"          , Form("%s: JetToTauWeight"              ,dirname),  40,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightCorr      , "jettotauweightcorr"      , Form("%s: JetToTauWeightCorr"          ,dirname),  50,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightBias      , "jettotauweightbias"      , Form("%s: JetToTauWeightBias"          ,dirname),  50,    0,   5, folder);

      Utilities::BookH1D(fEventHist[i]->hIsSignal                , "issignal"                , Form("%s: IsSignal"                    ,dirname),   5,   -2,   3, folder);
      Utilities::BookH1D(fEventHist[i]->hNMuons                  , "nmuons"                  , Form("%s: NMuons"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNElectrons              , "nelectrons"              , Form("%s: NElectrons"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTaus                   , "ntaus"                   , Form("%s: NTaus"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hNPV[0]                  , "npv"                     , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1F(fEventHist[i]->hNPU[0]                  , "npu"                     , Form("%s: NPU"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1D(fEventHist[i]->hNPartons                , "npartons"                , Form("%s: NPartons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hLHENJets                , "lhenjets"                , Form("%s: LHE N(jets)"                 ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);
      Utilities::BookH1D(fEventHist[i]->hNPhotons                , "nphotons"                , Form("%s: NPhotons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets                   , "njets"                   , Form("%s: NJets"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20Rej[0]           , "njets20rej"              , Form("%s: NJets20Rej"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNFwdJets                , "nfwdjets"                , Form("%s: NFwdJets"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);


      Utilities::BookH1D(fEventHist[i]->hTriggerLeptonStatus, "triggerleptonstatus" , Form("%s: TriggerLeptonStatus" ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTriggered         , "ntriggered"          , Form("%s: NTriggered"          ,dirname),   5,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hPuWeight           , "puweight"            , Form("%s: PuWeight"            ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPUIDWeight      , "jetpuidweight"       , Form("%s: JetPUIDWeight"       ,dirname),  50,    0,   2, folder);
      Utilities::BookH1D(fEventHist[i]->hPrefireWeight      , "prefireweight"       , Form("%s: PrefireWeight"       ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hBTagWeight         , "btagweight"          , Form("%s: BTagWeight"          ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hZPtWeight          , "zptweight"           , Form("%s: ZPtWeight"           ,dirname),  50,    0,   2, folder);

      Utilities::BookH1F(fEventHist[i]->hTauPt              , "taupt"               , Form("%s: TauPt"               ,dirname),  50,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hTauEta             , "taueta"              , Form("%s: TauEta"              ,dirname),  30, -2.5, 2.5, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDecayMode[0]    , "taudecaymode"        , Form("%s: TauDecayMode"        ,dirname),  15,    0,  15, folder);
      Utilities::BookH1D(fEventHist[i]->hTauGenFlavor       , "taugenflavor"        , Form("%s: TauGenFlavor"        ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiEle     , "taudeepantiele"      , Form("%s: TauDeepAntiEle"      ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiMu      , "taudeepantimu"       , Form("%s: TauDeepAntiMu"       ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiJet     , "taudeepantijet"      , Form("%s: TauDeepAntiJet"      ,dirname),  30,    0,  30, folder);

      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hJetEta             , "jeteta"              , Form("%s: JetEta"              ,dirname), 100,   -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hHtSum              , "htsum"               , Form("%s: HtSum"               ,dirname), 100,    0, 400, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                 , "ht"                  , Form("%s: Ht"                  ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hHtPhi              , "htphi"               , Form("%s: HtPhi"               ,dirname),  50,   -4,   4, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMetPhi              , "metphi"              , Form("%s: MetPhi"                  ,dirname)  ,  40, -4,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hMetCorr             , "metcorr"             , Form("%s: Met Correction"          ,dirname)  ,  50,  0,  25, folder);
      Utilities::BookH1F(fEventHist[i]->hMetCorrPhi          , "metcorrphi"          , Form("%s: MetPhi Correction"       ,dirname)  ,  40, -4,   4, folder);

      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTOneOverM          , "mtoneoverm"          , Form("%s: MTOneOverM"              ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwoOverM          , "mttwooverm"          , Form("%s: MTTwoOverM"              ,dirname)  , 100, 0.,   10. , folder);

      Utilities::BookH1F(fEventHist[i]->hMetDeltaPhi         , "metdeltaphi"         , Form("%s: Met Lep Delta Phi"       ,dirname)  ,  50,  0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepOneDeltaPhi      , "leponedeltaphi"      , Form("%s: Lep One vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepTwoDeltaPhi      , "leptwodeltaphi"      , Form("%s: Lep Two vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);


      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPhi  , "lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  40,  -4,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPtOverM        , "lepptoverm"       , Form("%s: Lepton Pt / M"       ,dirname),  40,   0,   4, folder);

      Utilities::BookH1F(fEventHist[i]->hLepDeltaEta   , "lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[0]  , "lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname), 50,   0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hZLepOnePt  , "zleponept"     , Form("%s: ZLepOnePt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoPt  , "zleptwopt"     , Form("%s: ZLepTwoPt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepOneEta , "zleponeeta"    , Form("%s: ZLepOneEta"     ,dirname)  ,  50, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoEta , "zleptwoeta"    , Form("%s: ZLepTwoEta"     ,dirname)  ,  50, -2.5, 2.5, folder);

      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[0], "lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[1], "lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[0], "lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[0]    , "deltaalpham0"     , Form("%s: Delta Alpha Mass 0"  ,dirname), 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[1]    , "deltaalpham1"     , Form("%s: Delta Alpha Mass 1"  ,dirname), 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaMColM[0], "deltaalphamcolm0" , Form("%s: MCol - Alpha Mass 0" ,dirname),  50, -50,  50, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaMColM[1], "deltaalphamcolm1" , Form("%s: MCol - Alpha Mass 1" ,dirname),  50, -50,  50, folder);

      Utilities::BookH1F(fEventHist[i]->hPTauVisFrac    , "ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  , 50,0.,  1.5, folder);

      for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig.names_[j].Data()) ,
                           fMVAConfig.NBins(j), fMVAConfig.Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig.names_[j].Data()), 3000, -1.,  2., folder);
        Utilities::BookH1F(fEventHist[i]->hMVATrain[j], Form("mvatrain%i",j), Form("%s: %s MVA (train)" ,dirname, fMVAConfig.names_[j].Data()),  100, -3.,  2., folder);
        Utilities::BookH1F(fEventHist[i]->hMVATest[j] , Form("mvatest%i",j) , Form("%s: %s MVA (test)"  ,dirname, fMVAConfig.names_[j].Data()),  100, -3.,  2., folder);
      }

      // for(int mode = 0; mode < 3; ++mode) {
      //   Utilities::BookH1F(fEventHist[i]->hLepOnePrimePx[mode], Form("leponeprimepx%i", mode), Form("%s: l1 px" , dirname),  50, -100., 100., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePx[mode], Form("leptwoprimepx%i", mode), Form("%s: l2 px" , dirname),  50, -100., 100., folder);
      //   Utilities::BookH1F(fEventHist[i]->hMetPrimePx   [mode], Form("metprimepx%i"   , mode), Form("%s: met px", dirname),  50,    0., 100., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepOnePrimePy[mode], Form("leponeprimepy%i", mode), Form("%s: l1 py" , dirname),  50, -200., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePy[mode], Form("leptwoprimepy%i", mode), Form("%s: l2 py" , dirname),  50, -200., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hMetPrimePy   [mode], Form("metprimepy%i"   , mode), Form("%s: met py", dirname),  50, -200., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepOnePrimePz[mode], Form("leponeprimepz%i", mode), Form("%s: l1 pz" , dirname), 100, -200., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepTwoPrimePz[mode], Form("leptwoprimepz%i", mode), Form("%s: l2 pz" , dirname), 100, -200., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hMetPrimePz   [mode], Form("metprimepz%i"   , mode), Form("%s: met pz", dirname), 100, -200., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepOnePrimeE [mode], Form("leponeprimee%i" , mode), Form("%s: l1 e " , dirname), 100,    0., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hLepTwoPrimeE [mode], Form("leptwoprimee%i" , mode), Form("%s: l2 e " , dirname), 100,    0., 200., folder);
      //   Utilities::BookH1F(fEventHist[i]->hMetPrimeE    [mode], Form("metprimee%i"    , mode), Form("%s: met e ", dirname), 100,    0., 200., folder);
      // }

}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::BookEventHistograms() {
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

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::BookPhotonHistograms() {
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
void HistMaker::BookBaseLepHistograms(Int_t i, const char* dirname) {
      auto folder = fDirectories[2*fn + i];

      // Lepton one info //
      Utilities::BookH1F(fLepHist[i]->hOnePt[0]       , "onept"            , Form("%s: Pt"           ,dirname)  , 150,   0, 150, folder);
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
      Utilities::BookH1D(fLepHist[i]->hOneJetOverlap  , "onejetoverlap"    , Form("%s: OneJetOverlap",dirname),   3,    0,    3, folder);
      Utilities::BookH1D(fLepHist[i]->hOneTrigger     , "onetrigger"       , Form("%s: Trigger"      ,dirname),  10,    0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hOneWeight      , "oneweight"        , Form("%s: Weight"       ,dirname),  50,    0,    2, folder);
      Utilities::BookH1F(fLepHist[i]->hOneTrigWeight  , "onetrigweight"    , Form("%s: Trig Weight"  ,dirname),  50,    0,    2, folder);
      //Gen Info
      Utilities::BookH1F(fLepHist[i]->hOneGenPt       , "onegenpt"         , Form("%s: Gen Pt"       ,dirname), 100,    0,  150, folder);
      Utilities::BookH1F(fLepHist[i]->hOneGenE        , "onegene"          , Form("%s: Gen E"        ,dirname), 100,    0,  150, folder);
      Utilities::BookH1F(fLepHist[i]->hOneGenEta      , "onegeneta"        , Form("%s: Gen Eta"      ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneMetDeltaPhi , "onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,    0,    5, folder);

      // Lepton two info //
      Utilities::BookH1F(fLepHist[i]->hTwoPt[0]       , "twopt"            , Form("%s: Pt"          ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPz          , "twopz"            , Form("%s: Pz"          ,dirname), 100, -100,  100, folder);
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
      Utilities::BookH1D(fLepHist[i]->hTwoJetOverlap  , "twojetoverlap"    , Form("%s: TwoJetOverlap",dirname),  3,   0,    3, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoTrigger     , "twotrigger"       , Form("%s: Trigger"     ,dirname),  10,   0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoWeight      , "twoweight"        , Form("%s: Weight"      ,dirname),  50,   0,    2, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoTrigWeight  , "twotrigweight"    , Form("%s: Trig Weight" ,dirname),  50,   0,    2, folder);
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
}
//--------------------------------------------------------------------------------------------------------------
void HistMaker::BookLepHistograms() {
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
void HistMaker::BookSystematicHistograms() {
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
void HistMaker::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      delete dirname;
      fTrees[i] = new TTree(Form("tree_%i",i),Form("HistMaker TTree %i",i));
      fTrees[i]->Branch("leponept",           &fTreeVars.leponept          );
      fTrees[i]->Branch("leponeptoverm",      &fTreeVars.leponeptoverm     );
      fTrees[i]->Branch("leptwopt",           &fTreeVars.leptwopt          );
      fTrees[i]->Branch("leptwoptoverm",      &fTreeVars.leptwoptoverm     );
      fTrees[i]->Branch("leptwod0",           &fTreeVars.leptwod0          );
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
void HistMaker::DeleteHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i]) {
      if(fEventHist     [i]) {delete fEventHist     [i]; fEventHist     [i] = nullptr;}
      if(fLepHist       [i]) {delete fLepHist       [i]; fLepHist       [i] = nullptr;}
      if(fPhotonHist    [i]) {delete fPhotonHist    [i]; fPhotonHist    [i] = nullptr;}
      if(fSystematicHist[i]) {delete fSystematicHist[i]; fSystematicHist[i] = nullptr;}
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
// Initialize branch structure of the input tree
void HistMaker::InitializeInputTree(TTree* tree) {
  tree->SetBranchAddress("nMuon"                    , &nMuon                     );
  tree->SetBranchAddress("nGenMuon"                 , &nGenMuons                 );
  tree->SetBranchAddress("Muon_pt"                  , &Muon_pt                   );
  tree->SetBranchAddress("Muon_eta"                 , &Muon_eta                  );
  tree->SetBranchAddress("Muon_phi"                 , &Muon_phi                  );
  tree->SetBranchAddress("Muon_charge"              , &Muon_charge               );
  tree->SetBranchAddress("Muon_looseId"             , &Muon_looseId              );
  tree->SetBranchAddress("Muon_mediumId"            , &Muon_mediumId             );
  tree->SetBranchAddress("Muon_tightId"             , &Muon_tightId              );
  tree->SetBranchAddress("Muon_pfRelIso04_all"      , &Muon_pfRelIso04_all       );
  tree->SetBranchAddress("Muon_dxy"                 , &Muon_dxy                  );
  tree->SetBranchAddress("Muon_dxyErr"              , &Muon_dxyErr               );
  tree->SetBranchAddress("Muon_dz"                  , &Muon_dz                   );
  tree->SetBranchAddress("Muon_dzErr"               , &Muon_dzErr                );
  tree->SetBranchAddress("Muon_nTrackerLayers"      , &Muon_nTrackerLayers       );
  tree->SetBranchAddress("Muon_TaggedAsRemovedByJet", &Muon_TaggedAsRemovedByJet );
  if(!fIsData) {
    tree->SetBranchAddress("Muon_genPartFlav"    , &Muon_genPartFlav    );
    tree->SetBranchAddress("Muon_genPartIdx"     , &Muon_genPartIdx     );
  }

  tree->SetBranchAddress("nElectron"                     , &nElectron                     );
  tree->SetBranchAddress("nGenElectron"                  , &nGenElectrons                 );
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
  tree->SetBranchAddress("Electron_TaggedAsRemovedByJet" , &Electron_TaggedAsRemovedByJet );
  if(!fIsData) {
    tree->SetBranchAddress("Electron_genPartFlav"          , &Electron_genPartFlav          );
    tree->SetBranchAddress("Electron_genPartIdx"           , &Electron_genPartIdx           );
  }

  tree->SetBranchAddress("nTau"                          , &nTau                          );
  tree->SetBranchAddress("nGenTau"                       , &nGenTaus                      );
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
  tree->SetBranchAddress("Tau_TaggedAsRemovedByJet"      , &Tau_TaggedAsRemovedByJet      );
  if(!fIsData) {
    tree->SetBranchAddress("Tau_genPartFlav"               , &Tau_genPartFlav               );
    tree->SetBranchAddress("Tau_genPartIdx"                , &Tau_genPartIdx                );
  }

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
  if(!fIsData)
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
  if(!fIsData) {
    tree->SetBranchAddress("GenZll_pt"                     , &zPt                           );
    tree->SetBranchAddress("GenZll_mass"                   , &zMass                         );
    tree->SetBranchAddress("GenZll_LepOne_pdgId"           , &zLepOneID                     );
    tree->SetBranchAddress("GenZll_LepTwo_pdgId"           , &zLepTwoID                     );
    tree->SetBranchAddress("GenZll_LepOne_pt"              , &zLepOnePt                     );
    tree->SetBranchAddress("GenZll_LepTwo_pt"              , &zLepTwoPt                     );
    tree->SetBranchAddress("GenZll_LepOne_eta"             , &zLepOneEta                    );
    tree->SetBranchAddress("GenZll_LepTwo_eta"             , &zLepTwoEta                    );
    tree->SetBranchAddress("GenZll_LepOne_phi"             , &zLepOnePhi                    );
    tree->SetBranchAddress("GenZll_LepTwo_phi"             , &zLepTwoPhi                    );
    tree->SetBranchAddress("GenZll_LepOne_mass"            , &zLepOneMass                   );
    tree->SetBranchAddress("GenZll_LepTwo_mass"            , &zLepTwoMass                   );
  }

  //Event information
  if(!fIsData)
    tree->SetBranchAddress("genWeight"                     , &genWeight                     );
  tree->SetBranchAddress("HT"                            , &ht                            );
  tree->SetBranchAddress("PuppiMET_pt"                   , &puppMET                       );
  tree->SetBranchAddress("PuppiMET_phi"                  , &puppMETphi                    );
  // tree->SetBranchAddress("Pileup_nPU"                    , &nPUAdded                      );
  if(!fIsData) {
    tree->SetBranchAddress("Pileup_nTrueInt"               , &nPU                           );
    tree->SetBranchAddress("LHE_Njets"                     , &LHE_Njets                     ) ;
  }
  tree->SetBranchAddress("PV_npvsGood"                   , &nPV                           ) ;

  //MVA information
  if(!fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {
      fChain->SetBranchAddress(Form("mva%i",mva_i), &fMvaOutputs[mva_i]);
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply/replace electron energy scale corrections
void HistMaker::ApplyElectronCorrections() {
  for(UInt_t index = 0; index < nTau; ++index) {
    float sf = (fIsEmbed) ? 1. : fElectronIDWeight.EmbedEnergyScale(Electron_pt[index], Electron_eta[index],
                                                                    fYear, eleES_up, eleES_down); //assume 1 electron in event where up/down matter
    Electron_energyScale[index] = sf;
    if(fIsEmbed) sf /= Electron_eCorr[index]; //remove the correction applied to embedding electrons
    Electron_pt  [index] *= sf;
    // Electron_mass[index] *= sf;
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply the Rochester corrections to the muon pt for all muons
void HistMaker::ApplyMuonCorrections() {
  const static int s(0), m(0); //error set and member for corrections
  for(UInt_t index = 0; index < nMuon; ++index) {
    const double u = fRnd->Uniform();
    double sf = 1.;
    if(fIsData) {
      sf = fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
    } else {
      // //For muons in embedding events not identified as the main leptons, apply typical data corrections
      // if(fIsEmbed && (abs(leptonOneFlavor) != 13 || (int) index != leptonOneIndex) && (abs(leptonTwoFlavor) != 13 || (int) index != leptonTwoIndex)) {
      //   sf = fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
      // } else {
      //For simulated muons, both in embedding and standard MC, apply given MC corrections
      sf = fRoccoR->kSmearMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], Muon_nTrackerLayers[index], u, s, m);
      // }
    }
    Muon_pt[index] *= sf;
    Muon_RoccoSF[index] = sf;
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply tau energy scale corrections
void HistMaker::ApplyTauCorrections() {
  tauES = 1.f; tauES_up = 1.f; tauES_down = 1.f;
  for(UInt_t index = 0; index < nTau; ++index) {
    float sf = (fIsData) ? 1. : fTauIDWeight->EnergyScale(Tau_pt[index], Tau_eta[index], Tau_decayMode[index],
                                                          Tau_genPartFlav[index], Tau_idDeepTau2017v2p1VSjet[index],
                                                          fYear, tauES_up, tauES_down); //assume 1 tau in event where up/down matter
    Tau_pt  [index] *= sf;
    Tau_mass[index] *= sf;
    Tau_energyScale[index] = sf;
  }
}

//--------------------------------------------------------------------------------------------------------------
// Determine and apply the global event weights (i.e. not selection specific)
void HistMaker::InitializeEventWeights() {
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
    eventWeight *= embeddingUnfoldingWeight*embeddingWeight;
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

  btagWeight = 1.f; btagWeightUp = 1.f; btagWeightDown = 1.f;
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
  }

  ////////////////////////////////////////////////////////////////////
  //   Lepton ID weights
  ////////////////////////////////////////////////////////////////////

  if(fIsEmbed) {
    if     (std::abs(leptonOneFlavor) == 11) leptonOneWeight1 = fEmbeddingTnPWeight.ElectronIDWeight(leptonOneP4->Pt(), leptonOneSCEta, fYear, isLooseElectron, mcEra);
    else if(std::abs(leptonOneFlavor) == 13) leptonOneWeight1 = fEmbeddingTnPWeight.MuonIDWeight    (leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, isLooseMuon, mcEra);
    else if(std::abs(leptonOneFlavor) == 15) leptonOneWeight1 = fTauIDWeight->IDWeight              (leptonOneP4->Pt(), leptonOneP4->Eta(), tauGenID, tauDeepAntiJet,
                                                                                                     fYear, leptonOneWeight1_up, leptonOneWeight1_down);
    if     (std::abs(leptonTwoFlavor) == 11) leptonTwoWeight1 = fEmbeddingTnPWeight.ElectronIDWeight(leptonTwoP4->Pt(), leptonTwoSCEta, fYear, isLooseElectron, mcEra);
    else if(std::abs(leptonTwoFlavor) == 13) leptonTwoWeight1 = fEmbeddingTnPWeight.MuonIDWeight    (leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, isLooseMuon, mcEra);
    else if(std::abs(leptonTwoFlavor) == 15) leptonTwoWeight1 = fTauIDWeight->IDWeight              (leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauGenID, tauDeepAntiJet,
                                                                                                     fYear, leptonTwoWeight1_up, leptonTwoWeight1_down);
  } else if(!fIsData) { //MC simulations
    if     (std::abs(leptonOneFlavor) == 11) fElectronIDWeight.IDWeight(leptonOneP4->Pt(), leptonOneSCEta, fYear,
                                                                        leptonOneWeight1 , leptonOneWeight1_up, leptonOneWeight1_down, leptonOneWeight1_bin,
                                                                        leptonOneWeight2 , leptonOneWeight2_up, leptonOneWeight2_down, leptonOneWeight2_bin);
    else if(std::abs(leptonOneFlavor) == 13) fMuonIDWeight.IDWeight    (leptonOneP4->Pt(), leptonOneP4->Eta(), fYear, mcEra,
                                                                        leptonOneWeight1 , leptonOneWeight1_up, leptonOneWeight1_down, leptonOneWeight1_bin,
                                                                        leptonOneWeight2 , leptonOneWeight2_up, leptonOneWeight2_down, leptonOneWeight2_bin);
    else if(std::abs(leptonOneFlavor) == 15) leptonOneWeight1 = fTauIDWeight->IDWeight              (leptonOneP4->Pt(), leptonOneP4->Eta(), tauGenID, tauDeepAntiJet,
                                                                                                     fYear, leptonOneWeight1_up, leptonOneWeight1_down);
    if     (std::abs(leptonTwoFlavor) == 11) fElectronIDWeight.IDWeight(leptonTwoP4->Pt(), leptonTwoSCEta, fYear,
                                                                        leptonTwoWeight1 , leptonTwoWeight1_up, leptonTwoWeight1_down, leptonTwoWeight1_bin,
                                                                        leptonTwoWeight2 , leptonTwoWeight2_up, leptonTwoWeight2_down, leptonTwoWeight2_bin);
    else if(std::abs(leptonTwoFlavor) == 13) fMuonIDWeight.IDWeight    (leptonTwoP4->Pt(), leptonTwoP4->Eta(), fYear, mcEra,
                                                                        leptonTwoWeight1 , leptonTwoWeight1_up, leptonTwoWeight1_down, leptonTwoWeight1_bin,
                                                                        leptonTwoWeight2 , leptonTwoWeight2_up, leptonTwoWeight2_down, leptonTwoWeight2_bin);
    else if(std::abs(leptonTwoFlavor) == 15) leptonTwoWeight1 = fTauIDWeight->IDWeight              (leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauGenID, tauDeepAntiJet,
                                                                                                     fYear, leptonTwoWeight1_up, leptonTwoWeight1_down);
  }
  eventWeight *= leptonOneWeight1*leptonOneWeight2;
  eventWeight *= leptonTwoWeight1*leptonTwoWeight2;

  //No weights in data
  if(fIsData) {
    eventWeight = 1.; puWeight = 1.; genWeight = 1.; zPtWeight = 1.; zPtWeightUp = 1.; zPtWeightDown = 1.;
    jetPUIDWeight = 1.; btagWeight = 1.; embeddingWeight = 1.; embeddingUnfoldingWeight = 1.;
    const int ntrig = sizeof(triggerWeights)/sizeof(*triggerWeights);
    for(int itrig = 0; itrig < ntrig; ++itrig) triggerWeights[itrig] = 1.;
  }
}

//--------------------------------------------------------------------------------------------------------------
// Count objects in the event (leptons, jets, etc.) and initialize base variables
void HistMaker::CountObjects() {

  ///////////////////////////////////////////////////////
  //Set the data era
  ///////////////////////////////////////////////////////

  mcEra = 0;
  if(!fIsData && !fIsEmbed) { // use random numbers for MC not split by era
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


  ///////////////////////////////////////////////////////
  // Apply object scale corrections
  ///////////////////////////////////////////////////////

  ApplyMuonCorrections();
  ApplyTauCorrections();

  ///////////////////////////////////////////////////////
  // Initialize lepton selection info
  ///////////////////////////////////////////////////////

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
    leptonOneJetOverlap = Electron_TaggedAsRemovedByJet[0];
    leptonTwoJetOverlap = Muon_TaggedAsRemovedByJet[0];
    leptonOneGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwoGenFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonOneID1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOneID2       = 0;
    leptonTwoID1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonTwoID2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    eleES              = Electron_energyScale[0];
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
    leptonOneJetOverlap = Electron_TaggedAsRemovedByJet[0];
    leptonTwoJetOverlap = Tau_TaggedAsRemovedByJet[0];
    leptonOneGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwoGenFlavor = TauFlavorFromID(Tau_genPartFlav[0]);
    leptonOneID1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOneID2       = 0;
    leptonTwoID1       = Tau_idAntiEle[0];
    leptonTwoID2       = Tau_idAntiMu[0];
    eleES              = Electron_energyScale[0];
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
    leptonOneJetOverlap = Muon_TaggedAsRemovedByJet[0];
    leptonTwoJetOverlap = Tau_TaggedAsRemovedByJet[0];
    leptonOneGenFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonTwoGenFlavor = TauFlavorFromID(Tau_genPartFlav[0]);
    leptonOneID1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOneID2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwoID1       = Tau_idAntiEle[0];
    leptonTwoID2       = Tau_idAntiMu[0];
  } else if(nElectron < 2 && nMuon == 2) { //mu+mu
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
    leptonOneJetOverlap = Muon_TaggedAsRemovedByJet[0];
    leptonTwoJetOverlap = Muon_TaggedAsRemovedByJet[1];
    leptonOneGenFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonTwoGenFlavor = MuonFlavorFromID(Muon_genPartFlav[1]);
    leptonOneID1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOneID2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwoID1       = Muon_looseId[1] + Muon_mediumId[1] + Muon_tightId[1];
    leptonTwoID2       = MuonRelIsoID(Muon_pfRelIso04_all[1]);
  } else if(nElectron == 2 && nMuon < 2) { //e+e
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
    leptonOneJetOverlap = Electron_TaggedAsRemovedByJet[0];
    leptonTwoJetOverlap = Electron_TaggedAsRemovedByJet[1];
    leptonOneGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwoGenFlavor = ElectronFlavorFromID(Electron_genPartFlav[1]);
    leptonOneID1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOneID2       = 0;
    leptonTwoID1       = Electron_mvaFall17V2noIso_WPL[1] + Electron_mvaFall17V2noIso_WP90[1] + Electron_mvaFall17V2noIso_WP80[1];
    leptonTwoID2       = 0;
    eleES              = Electron_energyScale[0];
  }
  if(nTau > 0) { //store information of the leading pT tau
    tauDecayMode   = Tau_decayMode[0];
    tauDeepAntiEle = Tau_idDeepTau2017v2p1VSe  [0];
    tauDeepAntiMu  = Tau_idDeepTau2017v2p1VSmu [0];
    tauDeepAntiJet = Tau_idDeepTau2017v2p1VSjet[0];
    tauES          = Tau_energyScale[0];
    tauGenID       = Tau_genPartFlav[0];
    tauGenFlavor   = TauFlavorFromID(tauGenID);
  }

  //Add MET information
  met        = puppMET; //use PUPPI MET
  metPhi     = puppMETphi;
  metCorr    = 0.;
  metCorrPhi = 0.;

  //If no Z information is found, approximate it with the lepton info
  if(zPt < 0 || zMass < 0) {
    if(zLepOnePt < 0 || zLepTwoPt < 0) { //no generator-level leptons
      zPt = ((*leptonOneP4) + (*leptonTwoP4)).Pt();
      zMass = ((*leptonOneP4) + (*leptonTwoP4)).M();
    } else { //generator-level leptons found
      TLorentzVector zlepone, zleptwo;
      zlepone.SetPtEtaPhiM(zLepOnePt, zLepOneEta, zLepOnePhi, zLepOneMass);
      zleptwo.SetPtEtaPhiM(zLepTwoPt, zLepTwoEta, zLepTwoPhi, zLepTwoMass);
      zPt   = (zlepone + zleptwo).Pt();
      zMass = (zlepone + zleptwo).M();
    }
  }
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

  /////////////////////////////////////////////
  // Check info about selected leptons

  isFakeElectron  = !fIsData && ((std::abs(leptonOneFlavor) == 11 && leptonOneGenFlavor == 26) ||
                                 (std::abs(leptonTwoFlavor) == 11 && leptonTwoGenFlavor == 26));
  isFakeMuon      = !fIsData && ((std::abs(leptonOneFlavor) == 13 && leptonOneGenFlavor == 26) ||
                                 (std::abs(leptonTwoFlavor) == 13 && leptonTwoGenFlavor == 26));

  isLooseElectron  = (std::abs(leptonOneFlavor) == 11) && leptonOneIso / leptonOneP4->Pt() >= 0.15;
  isLooseElectron |= (std::abs(leptonTwoFlavor) == 11) && leptonTwoIso / leptonTwoP4->Pt() >= 0.15;
  isLooseMuon      = (std::abs(leptonOneFlavor) == 13) && leptonOneIso / leptonOneP4->Pt() >= 0.15;
  isLooseMuon     |= (std::abs(leptonTwoFlavor) == 13) && leptonTwoIso / leptonTwoP4->Pt() >= 0.15;
  isLooseTau       = (std::abs(leptonTwoFlavor) == 15) && tauDeepAntiJet < 50;
  looseQCDSelection = isLooseElectron || isLooseMuon || isLooseTau;
  chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;

  ///////////////////////////////////////////////////////
  // Add Jet information
  ///////////////////////////////////////////////////////

  CountJets();

}

//--------------------------------------------------------------------------------------------------------------
// Count the jets in the event
void HistMaker::CountJets() {
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
void HistMaker::InitializeTreeVariables() {
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
  if(std::abs(leptonTwoFlavor) == 15) {//tau selection
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
  fTreeVars.eventweightMVA *= (fTreeVars.train > 0.) ? 0. : 1./(1.-fFractionMVA); //if training, ignore, else rescale to account for training sample removed

  TString selecName = "";
  if(std::abs(leptonOneFlavor) == 13 && std::abs(leptonTwoFlavor) == 15)      { fTreeVars.category = 1; selecName = "mutau"; }
  else if(std::abs(leptonOneFlavor) == 11 && std::abs(leptonTwoFlavor) == 15) { fTreeVars.category = 2; selecName = "etau" ; }
  else if(std::abs(leptonOneFlavor) == 11 && std::abs(leptonTwoFlavor) == 13) { fTreeVars.category = 3; selecName = "emu"  ; }
  else if(std::abs(leptonOneFlavor) == 13 && std::abs(leptonTwoFlavor) == 13) { fTreeVars.category = 4; selecName = "mumu" ; }
  else if(std::abs(leptonOneFlavor) == 11 && std::abs(leptonTwoFlavor) == 11) { fTreeVars.category = 5; selecName = "ee"   ; }
  else                    {
    std::cout << "---Warning! Entry " << fentry
              << " has undentified selection in " << __func__ << "!\n";
    selecName = "unknown";
    fTreeVars.category = -1;
  }

  if(fReprocessMVAs) {
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
void HistMaker::FillBaseEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  Hist->hEventWeight              ->Fill(eventWeight);
  Hist->hLogEventWeight           ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hFullEventWeightLum       ->Fill(std::fabs(fTreeVars.fulleventweightlum), genWeight*eventWeight);
  Hist->hLogFullEventWeightLum    ->Fill((std::fabs(fTreeVars.fulleventweightlum) > 1.e-10) ? std::log10(std::fabs(fTreeVars.fulleventweightlum)) : -999., genWeight*eventWeight);
  Hist->hEventWeightMVA           ->Fill(fTreeVars.eventweightMVA);
  Hist->hGenWeight                ->Fill(genWeight                   , eventWeight          );
  Hist->hEmbeddingWeight          ->Fill(embeddingWeight             , eventWeight*genWeight);
  Hist->hLogEmbeddingWeight       ->Fill((embeddingWeight > 1.e-10) ? std::log10(embeddingWeight) : -999. , eventWeight*genWeight     );
  Hist->hEmbeddingUnfoldingWeight ->Fill(embeddingUnfoldingWeight);
  Hist->hQCDWeight                ->Fill(qcdWeight     , genWeight*eventWeight);
  Hist->hJetToTauWeight           ->Fill(jetToTauWeight, genWeight*eventWeight);
  Hist->hJetToTauWeightCorr       ->Fill((jetToTauWeight     > 0.) ? jetToTauWeightCorr/jetToTauWeight     : -1., genWeight*eventWeight);
  Hist->hJetToTauWeightBias       ->Fill((jetToTauWeightCorr > 0.) ? jetToTauWeightBias/jetToTauWeightCorr : -1., genWeight*eventWeight);

  Hist->hIsSignal            ->Fill(fTreeVars.issignal      );
  Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
  Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
  Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  Hist->hNPV[0]              ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hNPU[0]              ->Fill(nPU                , genWeight*eventWeight)      ;
  Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
  Hist->hLHENJets            ->Fill(LHE_Njets          , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight)   ;
  Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  Hist->hNGenTaus            ->Fill(nGenTaus             , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons        , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons            , genWeight*eventWeight)      ;
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  Hist->hNJets20Rej[0]       ->Fill(nJets20Rej         , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;

  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hNTriggered          ->Fill(leptonOneFired+leptonTwoFired, genWeight*eventWeight)   ;
  Hist->hPuWeight            ->Fill(puWeight)     ;
  Hist->hJetPUIDWeight       ->Fill(jetPUIDWeight);
  Hist->hPrefireWeight       ->Fill(prefireWeight);
  Hist->hBTagWeight          ->Fill(btagWeight)   ;
  Hist->hZPtWeight           ->Fill(zPtWeight)    ;

  if(tauP4 && tauP4->Pt() > 0.) { //if 0 then no additional tau stored
    Hist->hTauPt           ->Fill(tauP4->Pt()        , genWeight*eventWeight)   ;
    Hist->hTauEta          ->Fill(tauP4->Eta()       , genWeight*eventWeight)   ;
  }
  Hist->hTauGenFlavor      ->Fill(tauGenFlavor               , genWeight*eventWeight)   ;
  Hist->hTauDecayMode[0]   ->Fill(tauDecayMode               , genWeight*eventWeight)   ;
  Hist->hTauDeepAntiEle    ->Fill(std::log2(tauDeepAntiEle+1), genWeight*eventWeight)   ;
  Hist->hTauDeepAntiMu     ->Fill(tauDeepAntiMu              , genWeight*eventWeight)   ;
  Hist->hTauDeepAntiJet    ->Fill(std::log2(tauDeepAntiJet+1), genWeight*eventWeight)   ;

  if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOneP4->Pt()             , genWeight*eventWeight)   ;
    Hist->hJetEta          ->Fill(jetOneP4->Eta()            , genWeight*eventWeight)   ;
  }
  Hist->hHtSum             ->Fill(htSum              , genWeight*eventWeight)   ;
  Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;
  Hist->hHtPhi             ->Fill(htPhi              , genWeight*eventWeight)   ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMetPhi            ->Fill(metPhi             , genWeight*eventWeight)      ;
  Hist->hMetCorr           ->Fill(metCorr            , genWeight*eventWeight)      ;
  Hist->hMetCorrPhi        ->Fill(metCorrPhi         , genWeight*eventWeight)      ;

  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
  Hist->hMTOneOverM        ->Fill(fTreeVars.mtoneoverm  ,eventWeight*genWeight);
  Hist->hMTTwoOverM        ->Fill(fTreeVars.mttwooverm  ,eventWeight*genWeight);

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  const double lepDelR   = std::fabs(leptonOneP4->DeltaR(*leptonTwoP4));
  const double lepDelPhi = std::fabs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  const double lepDelEta = std::fabs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  double htDelPhi  = std::fabs(lepSys.Phi() - htPhi);
  if(htDelPhi > M_PI) htDelPhi = std::fabs(2.*M_PI - htDelPhi);
  double metDelPhi  = std::fabs(lepSys.Phi() - metPhi);
  if(metDelPhi > M_PI) metDelPhi = std::fabs(2.*M_PI - metDelPhi);
  double lepOneDelPhi  = std::fabs(lepSys.Phi() - leptonOneP4->Phi());
  if(lepOneDelPhi > M_PI) lepOneDelPhi = std::fabs(2.*M_PI - lepOneDelPhi);
  double lepTwoDelPhi  = std::fabs(lepSys.Phi() - leptonTwoP4->Phi());
  if(lepTwoDelPhi > M_PI) lepTwoDelPhi = std::fabs(2.*M_PI - lepTwoDelPhi);

  Hist->hMetDeltaPhi  ->Fill(metDelPhi              ,eventWeight*genWeight);
  Hist->hLepOneDeltaPhi->Fill(lepOneDelPhi          ,eventWeight*genWeight);
  Hist->hLepTwoDeltaPhi->Fill(lepTwoDelPhi          ,eventWeight*genWeight);

  double zpt   = (!fIsDY || zPt <  0.  ) ? lepSys.Pt() : zPt; //for DY to use given Z pT and Mass
  double zmass = (!fIsDY || zMass <  0.) ? lepSys.M()  : zMass;
  if(zpt < 0.) zpt = 0.;
  if(zmass < 0.) zmass = 0.;

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);

  Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR[0] ->Fill(lepDelR                ,eventWeight*genWeight);
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);

  Hist->hZLepOnePt    ->Fill(zLepOnePt , eventWeight*genWeight);
  Hist->hZLepTwoPt    ->Fill(zLepTwoPt , eventWeight*genWeight);
  Hist->hZLepOneEta   ->Fill(zLepOneEta, eventWeight*genWeight);
  Hist->hZLepTwoEta   ->Fill(zLepTwoEta, eventWeight*genWeight);

  Hist->hPTauVisFrac    ->Fill(fTreeVars.ptauvisfrac , eventWeight*genWeight);
  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);
  Hist->hDeltaAlphaM[0]->Fill(fTreeVars.deltaalpham1, eventWeight*genWeight);
  Hist->hDeltaAlphaM[1]->Fill(fTreeVars.deltaalpham2, eventWeight*genWeight);
  Hist->hDeltaAlphaMColM[0]->Fill(fTreeVars.mestimate    - fTreeVars.deltaalpham2, eventWeight*genWeight);
  Hist->hDeltaAlphaMColM[1]->Fill(fTreeVars.mestimatetwo - fTreeVars.deltaalpham1, eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    //MVA score vs mass for MVAs not correlated with mass
    if(fMVAConfig.names_[i].Contains("emu")) {
      if(fMVAConfig.names_[i].Contains("Z0")) Hist->hLepMVsMVA[0]->Fill(fMvaOutputs[i], fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
      else                                    Hist->hLepMVsMVA[1]->Fill(fMvaOutputs[i], fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
    }
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    if (fTreeVars.train > 0) Hist->hMVATrain[i]->Fill(fMvaOutputs[i], fTreeVars.eventweight*((fFractionMVA > 0.) ? 1./fFractionMVA : 1.));
    if (fTreeVars.train < 0) Hist->hMVATest[i] ->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA);
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  FillBaseEventHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  Hist->hPz  ->Fill(photonP4->Pz() , eventWeight*genWeight );
  Hist->hPt  ->Fill(photonP4->Pt() , eventWeight*genWeight );
  Hist->hP   ->Fill(photonP4->P()  , eventWeight*genWeight );
  Hist->hEta ->Fill((photonP4->Pt() > 0.) ? photonP4->Eta() : -1.e6, eventWeight*genWeight );
  Hist->hPhi ->Fill(photonP4->Phi(), eventWeight*genWeight );
  Hist->hMVA ->Fill(photonMVA, eventWeight*genWeight );
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::FillBaseLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  /////////////
  //  Lep 1  //
  /////////////
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()            ,eventWeight*genWeight);
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()           ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOneP4->Phi()           ,eventWeight*genWeight);
  Hist->hOneD0        ->Fill(leptonOneD0                  ,eventWeight*genWeight);
  Hist->hOneDXY       ->Fill(leptonOneDXY                 ,eventWeight*genWeight);
  Hist->hOneDZ        ->Fill(leptonOneDZ                  ,eventWeight*genWeight);
  Hist->hOneIso       ->Fill(leptonOneIso                 ,eventWeight*genWeight);
  Hist->hOneID1       ->Fill(leptonOneID1                 ,eventWeight*genWeight);
  Hist->hOneID2       ->Fill(leptonOneID2                 ,eventWeight*genWeight);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(std::abs(leptonOneFlavor)    ,eventWeight*genWeight);
  Hist->hOneGenFlavor ->Fill(leptonOneGenFlavor           ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hOneJetOverlap->Fill(leptonOneJetOverlap          ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOneTrigger               ,eventWeight*genWeight);
  Hist->hOneWeight    ->Fill(leptonOneWeight1*leptonOneWeight2,eventWeight*genWeight);
  Hist->hOneTrigWeight->Fill(leptonOneTrigWeight          ,eventWeight*genWeight);

  double oneMetDelPhi  = std::fabs(leptonOneP4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI) oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()            ,eventWeight*genWeight);
  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
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
  Hist->hTwoJetOverlap->Fill(leptonTwoJetOverlap          ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwoTrigger               ,eventWeight*genWeight);
  Hist->hTwoWeight    ->Fill(leptonTwoWeight1*leptonTwoWeight2,eventWeight*genWeight);
  Hist->hTwoTrigWeight->Fill(leptonTwoTrigWeight          ,eventWeight*genWeight);

  double twoMetDelPhi  = std::fabs(leptonTwoP4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI) twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);
  Hist->hD0Diff      ->Fill(leptonTwoD0-leptonOneD0             ,eventWeight*genWeight);
  Hist->hDXYDiff     ->Fill(leptonTwoDXY-leptonOneDXY           ,eventWeight*genWeight);
  Hist->hDZDiff      ->Fill(leptonTwoDZ-leptonOneDZ             ,eventWeight*genWeight);
  Hist->hTwoPtVsOnePt->Fill(fTreeVars.leponept, fTreeVars.leptwopt,eventWeight*genWeight);
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::FillLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  FillBaseLepHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::FillSystematicHistogram(SystematicHist_t* Hist) {
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
    } else continue; //no need to fill undefined systematics

    if(!std::isfinite(weight)) {
      std::cout << "HistMaker::" << __func__ << ": Entry " << fentry << " Systematic " << sys << ": Weight is not finite! weight = " << weight << " event weight = "
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
      std::cout << "HistMaker::" << __func__ << ": Entry " << fentry << " MVA weight is not finite! mvaweight = " << mvaweight << " eventweightMVA = "
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
        std::cout << "HistMaker::" << __func__ << ": Entry " << fentry << " MVA " << i << " score is not finite = " << mvascore << "! Setting to -2...\n";
        mvascore = -2.;
      }
      Hist->hMVA[i][sys]->Fill(mvascore, mvaweight);
    }
  }
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t HistMaker::Process(Long64_t entry)
{
  IncrementTimer(1, false); //timer for Process method
  if(!fChain) {
    printf("HistMaker::%s: Error! TChain not found\n", __func__);
    throw 1;
  }
  fentry = entry;
  fTimes[2] = std::chrono::steady_clock::now(); //timer for reading from the tree
  fChain->GetEntry(entry);
  IncrementTimer(2, true);

  //FIXME: Make 50k a field
  if(fVerbose > 0 || entry%50000 == 0) std::cout << Form("Processing event: %12lld (%5.1f%%)\n", entry, entry*100./fChain->GetEntriesFast());
  int icutflow = 0;
  fCutFlow->Fill(icutflow); ++icutflow; //0

  //Initialize base object information
  fTimes[3] = std::chrono::steady_clock::now(); //timer for initializing base objects
  CountObjects();
  IncrementTimer(3, true);

  //MC sample splitting
  if(SplitSampleEvent()) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //1

  /////////////////////////
  // Apply event weights //
  /////////////////////////

  fTimes[4] = std::chrono::steady_clock::now(); //timer for initializing event weights
  InitializeEventWeights();
  IncrementTimer(4, true);
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

  fCutFlow->Fill(icutflow); ++icutflow; //2

  //Print debug info
  if(fVerbose > 0 ) {
    std::cout << " lep_1: pdg_id = " << leptonOneFlavor << " p4 = "; leptonOneP4->Print();
    std::cout << " lep_2: pdg_id = " << leptonTwoFlavor << " p4 = "; leptonTwoP4->Print();
  }

  //Initialize systematic variation weights
  fTimes[5] = std::chrono::steady_clock::now(); //timer for initializing systematic configuration
  InitializeSystematics();
  IncrementTimer(5, true);


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

  fCutFlow->Fill(icutflow); ++icutflow; //3

  if((mutau + etau + emu + mumu + ee) > 1)
    std::cout << "WARNING! Entry " << entry << " passes multiple selections!\n";


  //////////////////////////////////////////////////////////////
  // Apply or remove scale factors
  //////////////////////////////////////////////////////////////



  fTimes[6] = std::chrono::steady_clock::now(); //timer for initializing event variables
  InitializeTreeVariables();
  IncrementTimer(6, true);

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
  fTimes[7] = std::chrono::steady_clock::now(); //timer for filling all histograms
  FillAllHistograms(set_offset + 1);
  IncrementTimer(7, true);

  fCutFlow->Fill(icutflow); ++icutflow; //5
  return kTRUE;
}


//--------------------------------------------------------------------------------------------------------------
// Match trigger objects to selected leptons
void HistMaker::MatchTriggers() {
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
  if((std::abs(leptonOneFlavor) == 11 && leptonOneFired) || //electron triggered
     (std::abs(leptonTwoFlavor) == 11 && leptonTwoFired)) {
    triggerLeptonStatus += 1;
  }
  if((std::abs(leptonOneFlavor) == 13 && leptonOneFired) || //muon triggered
     (std::abs(leptonTwoFlavor) == 13 && leptonTwoFired)) {
    triggerLeptonStatus += 2;
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::ApplyTriggerWeights() {
  float data_eff[2] = {0.5f, 0.5f}; //set to 0.5 so no danger in doing the ratio of eff or 1 - eff
  float mc_eff[2]   = {0.5f, 0.5f};

  ////////////////////////////////
  // Get the efficiencies
  ////////////////////////////////

  // Electrons
  if(std::abs(leptonOneFlavor) == 11 && leptonOneP4->Pt() > electron_trig_pt_) { //lepton 1 is an electron
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.ElectronTriggerWeight(leptonOneP4->Pt(), leptonOneSCEta, fYear, data_eff[0], mc_eff[0], isLooseElectron, mcEra);
      } else {
        fEmbeddingWeight.ElectronTriggerWeight   (leptonOneP4->Pt(), leptonOneSCEta, fYear, data_eff[0], mc_eff[0]);
      }
    } else {
      fElectronIDWeight.TriggerEff               (leptonOneP4->Pt(), leptonOneSCEta, fYear,
                                                  ElectronIDWeight::kWP80,
                                                  // (leptonOneID1 > 1) ? ElectronIDWeight::kWP80 : ElectronIDWeight::kWPLNotWP80,
                                                  data_eff[0], mc_eff[0]);
    }
  }
  if(std::abs(leptonTwoFlavor) == 11 && leptonTwoP4->Pt() > electron_trig_pt_) { //lepton 2 is an electron
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.ElectronTriggerWeight(leptonTwoP4->Pt(), leptonTwoSCEta, fYear, data_eff[1], mc_eff[1], isLooseElectron, mcEra);
      } else {
        fEmbeddingWeight.ElectronTriggerWeight   (leptonTwoP4->Pt(), leptonTwoSCEta, fYear, data_eff[1], mc_eff[1]);
      }
    } else {
      fElectronIDWeight.TriggerEff               (leptonTwoP4->Pt(), leptonTwoSCEta, fYear,
                                                  ElectronIDWeight::kWP80,
                                                  // (leptonOneID1 > 1) ? ElectronIDWeight::kWP80 : ElectronIDWeight::kWPLNotWP80,
                                                  data_eff[1], mc_eff[1]);
    }
  }

  // Muons
  if(std::abs(leptonOneFlavor) == 13 && leptonOneP4->Pt() > muon_trig_pt_) { //lepton 1 is a muon
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
  if(std::abs(leptonTwoFlavor) == 13 && leptonTwoP4->Pt() > muon_trig_pt_) { //lepton 2 is a muon
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
int HistMaker::Category(TString selection) {
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
void HistMaker::InitializeSystematics() {
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
void HistMaker::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::Terminate()
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
  const Double_t cpuTime = timer->CpuTime();
  const Double_t realTime = timer->RealTime();
  if(fPrintTime) {
    printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
    if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  }
  if(fPrintTime > 1) PrintTimerInfo();
  printf("HistMaker::%s: Finished processing!\n", __func__);
}
