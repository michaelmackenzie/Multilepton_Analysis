#define HISTSELECTOR_CXX

#include "interface/HistSelector.hh"

using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------
HistSelector::HistSelector(int seed, TTree * /*tree*/) : fSystematicSeed(seed),
                                                         fMuonJetToTauComp("mutau", 2035, 3, 0), fMuonJetToTauSSComp("mutau", 3035, 3, 0),
                                                         fElectronJetToTauComp("etau", 2035, 3, 0), fElectronJetToTauSSComp("etau", 3035, 3, 0),
                                                         fQCDWeight("emu", 1100201/*anti-iso, jet binned, 2D pt closure, use fits*/, 0),
                                                         fMuonIDWeight(1 /*use medium muon ID*/),
                                                         fElectronIDWeight(110 /*use WP90 electron ID and embed TnP trigger*/),
                                                         fEmbeddingTnPWeight(10/*10*(use 2016 BF/GH and 2018 ABC/D scales) + 1*(interpolate scales or not)*/) {

  //ensure pointers set to null to not attempt to delete if never initialized
  fChain = nullptr;
  for(int i = 0; i < fn; i++) {
    fEventHist     [i] = nullptr;
    fLepHist       [i] = nullptr;
    fPhotonHist    [i] = nullptr;
    fTrees         [i] = nullptr;
    fSystematicHist[i] = nullptr;
  }
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    fMuonJetToTauWeights      [proc] = nullptr;
    fMuonJetToTauMCWeights    [proc] = nullptr;
    fElectronJetToTauWeights  [proc] = nullptr;
    fElectronJetToTauMCWeights[proc] = nullptr;
  }
  fCutFlow = nullptr;
  fBTagWeight = nullptr;
  fPUWeight = nullptr;
  fJetPUWeight = nullptr;
  fPrefireWeight = nullptr;
  fTauIDWeight = nullptr;
  fEmbeddingWeight = nullptr;
  fRnd = nullptr;

  leptonOne.p4 = new TLorentzVector();
  leptonTwo.p4 = new TLorentzVector();
  jetOne.p4    = new TLorentzVector();
  photonP4     = nullptr;

  //Initialize time analysis fields
  for(int itime = 0; itime < fNTimes; ++itime) {
    fDurations[itime] = 0;
    fTimeNames[itime] = "";
    fTimeCounts[itime] = 0;
  }

  //Initialize configuration fields
  fDYType        =  0;
  fIsDY          =  0;
  fWNJets        = -1;
  fIsEmbed       =  0;
  fIsSignal      = false;
  fVerbose       =  0;
  fSelection     = "";
  fDataset       = "";
  fYear          = 2016;
  fReprocessMVAs = 0;
}

//--------------------------------------------------------------------------------------------------------------
HistSelector::~HistSelector() {
  DeleteHistograms();
  if(fJetToTauWts      ) {delete fJetToTauWts      ; fJetToTauWts       = nullptr;}
  if(fJetToTauWtsUp    ) {delete fJetToTauWtsUp    ; fJetToTauWtsUp     = nullptr;}
  if(fJetToTauWtsDown  ) {delete fJetToTauWtsDown  ; fJetToTauWtsDown   = nullptr;}
  if(fJetToTauCorrs    ) {delete fJetToTauCorrs    ; fJetToTauCorrs     = nullptr;}
  if(fJetToTauBiases   ) {delete fJetToTauBiases   ; fJetToTauBiases    = nullptr;}
  if(fJetToTauComps    ) {delete fJetToTauComps    ; fJetToTauComps     = nullptr;}
  if(fJetToTauCompsUp  ) {delete fJetToTauCompsUp  ; fJetToTauCompsUp   = nullptr;}
  if(fJetToTauCompsDown) {delete fJetToTauCompsDown; fJetToTauCompsDown = nullptr;}
  if(fJetToTauMCWts    ) {delete fJetToTauMCWts    ; fJetToTauMCWts     = nullptr;}
  if(fJetToTauMCCorrs  ) {delete fJetToTauMCCorrs  ; fJetToTauMCCorrs   = nullptr;}
  if(fJetToTauMCBiases ) {delete fJetToTauMCBiases ; fJetToTauMCBiases  = nullptr;}
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    if(fMuonJetToTauWeights      [proc]) {delete fMuonJetToTauWeights      [proc]; fMuonJetToTauWeights      [proc] = nullptr;}
    if(fMuonJetToTauMCWeights    [proc]) {delete fMuonJetToTauMCWeights    [proc]; fMuonJetToTauMCWeights    [proc] = nullptr;}
    if(fElectronJetToTauWeights  [proc]) {delete fElectronJetToTauWeights  [proc]; fElectronJetToTauWeights  [proc] = nullptr;}
    if(fElectronJetToTauMCWeights[proc]) {delete fElectronJetToTauMCWeights[proc]; fElectronJetToTauMCWeights[proc] = nullptr;}
  }

  if(fCutFlow          ) {delete fCutFlow         ; fCutFlow          = nullptr;}
  if(fBTagWeight       ) {delete fBTagWeight      ; fBTagWeight       = nullptr;}
  if(fPUWeight         ) {delete fPUWeight        ; fPUWeight         = nullptr;}
  if(fJetPUWeight      ) {delete fJetPUWeight     ; fJetPUWeight      = nullptr;}
  if(fPrefireWeight    ) {delete fPrefireWeight   ; fPrefireWeight    = nullptr;}
  if(fTauIDWeight      ) {delete fTauIDWeight     ; fTauIDWeight      = nullptr;}
  if(fRnd              ) {delete fRnd             ; fRnd              = nullptr;}
  if(leptonOne.p4      ) {delete leptonOne.p4     ; leptonOne.p4      = nullptr;}
  if(leptonTwo.p4      ) {delete leptonTwo.p4     ; leptonTwo.p4      = nullptr;}
  if(jetOne.p4         ) {delete jetOne.p4        ; jetOne.p4         = nullptr;}
  if(photonP4          ) {delete photonP4         ; photonP4          = nullptr;}
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::DeleteHistograms() {
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
void HistSelector::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();
  std::setbuf(stdout, NULL); //don't buffer the stdout stream
  printf("HistSelector::Begin\n");
  timer->Start();

  TString dir = gSystem->Getenv("CMSSW_BASE");
  if(dir == "") dir = "../"; //if not in a CMSSW environment, assume in a sub-directory of the package
  else dir += "/src/CLFVAnalysis/";

  fRoccoR = new RoccoR(Form("%sscale_factors/RoccoR%i.txt", dir.Data(), fYear));
  fElectronIDWeight.verbose_ = fVerbose;
  fMuonIDWeight.SetVerbose(fVerbose);
  fBTagWeight = new BTagWeight();
  fBTagWeight->verbose_ = fVerbose;
  printf("HistSelector::Begin (PU weight)\n");
  fPUWeight = new PUWeight();
  printf("HistSelector::Begin (Jet PU weight)\n");
  fJetPUWeight = new JetPUWeight();
  fPrefireWeight = new CLFV::PrefireWeight();
  fZPtWeight = new ZPtWeight("MuMu", 1);
  fCutFlow = new TH1D("hcutflow", "Cut-flow", 100, 0, 100);

  fMuonJetToTauWeights      [JetToTauComposition::kWJets] = new JetToTauWeight("MuonWJets"      , "mutau", "WJets",   31, 60300300, 0);
  fMuonJetToTauWeights      [JetToTauComposition::kZJets] = new JetToTauWeight("MuonZJets"      , "mutau", "WJets",   31, 60300300, 0);
  fMuonJetToTauWeights      [JetToTauComposition::kTop  ] = new JetToTauWeight("MuonTop"        , "mutau", "Top"  ,   82,  1100301, 0);
  fMuonJetToTauWeights      [JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonQCD"        , "mutau", "QCD"  , 1030, 40300300, 0);

  fMuonJetToTauMCWeights    [JetToTauComposition::kWJets] = new JetToTauWeight("MuonMCWJets"    , "mutau", "WJets",   88, 10300301, 0); //MC weights with MC non-closure + bias
  fMuonJetToTauMCWeights    [JetToTauComposition::kZJets] = new JetToTauWeight("MuonMCZJets"    , "mutau", "WJets",   88, 10300301, 0);
  fMuonJetToTauMCWeights    [JetToTauComposition::kTop  ] = new JetToTauWeight("MuonMCTop"      , "mutau", "Top"  ,   82,  1100301, 0); //Normal weights
  fMuonJetToTauMCWeights    [JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonMCQCD"      , "mutau", "QCD"  , 1095,   300300, 0); //high iso weights for SS --> OS bias

  //FIXME: Add back W+Jets MC bias (Mode = 10300300, 60300300 for only MC bias shape)
  fElectronJetToTauWeights  [JetToTauComposition::kWJets] = new JetToTauWeight("ElectronWJets"  , "etau" , "WJets",   31, 60300300, 0);
  fElectronJetToTauWeights  [JetToTauComposition::kZJets] = new JetToTauWeight("ElectronZJets"  , "etau" , "WJets",   31, 60300300, 0);
  fElectronJetToTauWeights  [JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronTop"    , "etau" , "Top"  ,   82,  1100301, 0);
  fElectronJetToTauWeights  [JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronQCD"    , "etau" , "QCD"  , 1030, 40300300, 0);

  fElectronJetToTauMCWeights[JetToTauComposition::kWJets] = new JetToTauWeight("ElectronMCWJets", "etau" , "WJets",   88, 10300301, 0); //MC weights with MC non-closure + bias
  fElectronJetToTauMCWeights[JetToTauComposition::kZJets] = new JetToTauWeight("ElectronMCZJets", "etau" , "WJets",   88, 10300301, 0);
  fElectronJetToTauMCWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronMCTop"  , "etau" , "Top"  ,   82,  1100301, 0); //Normal weights
  fElectronJetToTauMCWeights[JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronMCQCD"  , "etau" , "QCD"  , 1095,   300300, 0); //loose electron ID weights for SS --> OS bias

  fTauIDWeight = new TauIDWeight(fIsEmbed, (fSelection == "etau" || fSelection == "mutau") ? fSelection : "etau", fVerbose); //default to etau IDs

  //0: normal unfolding; 1: use z eta unfolding; 2: use z (eta, pt) unfolding; 3: mode 2 with tight cuts; 4: 2016 unfolding; 10: IC measured unfolding
  //FIXME: Settle on an embedding unfolding configuration
  fEmbeddingWeight = new EmbeddingWeight((fIsEmbed /*&& fSelection == "emu"*/) ? 10 : 0);

  for(int itrig = 0; itrig < 3; ++itrig) triggerWeights[itrig] = 1.f;

  fRnd = new TRandom3(fRndSeed);
  fTreeVars.type = -1;

  //Initialize MVA tools
  TMVA::Tools::Instance(); //load the TMVA library
  for(int i = 0; i < kMaxMVAs; ++i) mva[i] = nullptr; //initially 0s

  if(fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {
      //only initialize necessary MVAs
      if(fSelection != "" && !fMVAConfig.names_[mva_i].Contains(fSelection.Data())) continue;
      //ignore leptonic tau MVAs in hadronic channels
      if(fSelection.EndsWith("tau") && (fMVAConfig.names_[mva_i].Contains("tau_e") || fMVAConfig.names_[mva_i].Contains("tau_mu"))) continue;

      //determine the selection for defining the variables
      TString selection = "";
      if(fMVAConfig.names_[mva_i].Contains("higgs")) {if(!fDoHiggs) continue; else selection += "h";}
      else selection += "z";
      if(fMVAConfig.names_[mva_i].Contains("mutau")) selection += "mutau";
      else if(fMVAConfig.names_[mva_i].Contains("etau")) selection += "etau";
      else if(fMVAConfig.names_[mva_i].Contains("emu")) selection += "emu";
      else {
        printf ("Warning! Didn't determine mva weight file %s corresponding selection!\n",
                fMVAConfig.names_[mva_i].Data());
        continue;
      }
      //add for leptonic tau channels FIXME: Put as part of original check
      if(fMVAConfig.names_[mva_i].Contains("mutau_e")) selection += "_e";
      else if(fMVAConfig.names_[mva_i].Contains("etau_mu")) selection += "_mu";

      //initialize the reader
      mva[mva_i] = new TMVA::Reader("!Color:!Silent");

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
      const char* f = Form("weights/%s.2016_2017_2018.weights.xml",fMVAConfig.names_[mva_i].Data());
      mva[mva_i]->BookMVA(fMVAConfig.names_[mva_i].Data(),f);
      printf("Booked MVA %s with selection %s\n", fMVAConfig.names_[mva_i].Data(), selection.Data());
    }
  }

  //Create the output file
  fOut = new TFile(GetOutputName(), "RECREATE","HistSelector output histogram file");
  fTopDir = fOut->mkdir("Data");
  fTopDir->cd();
  std::cout << "Using output filename " << GetOutputName() << std::endl;

  for(int ihist = 0; ihist < fn; ++ihist) {
    fEventHist     [ihist] = nullptr;
    fLepHist       [ihist] = nullptr;
    fPhotonHist    [ihist] = nullptr;
    fSystematicHist[ihist] = nullptr;
  }

  lep_tau = fSelection.EndsWith("tau_e") + 2*fSelection.EndsWith("tau_mu");

  //Check that flags agree with the available branches in the tree
  if(fUseBTagWeights == 2 && !fChain->GetBranch("Jet_btagSF_deepcsv_L")) {
    printf("%s: Warning! B-tag SFs not available in tree, will re-calculate on the fly\n", __func__);
    fUseBTagWeights = 1;
  }
  if(fUseRoccoCorr == 2 && !fChain->GetBranch("Muon_corrected_pt")) {
    printf("%s: Warning! Rochester corrections not available in tree, will re-calculate on the fly\n", __func__);
    fUseRoccoCorr = 1;
  }
  if(fUseJetPUIDWeights == 2 && !fChain->GetBranch("JetPUIDWeight")) {
    printf("%s: Warning! Jet PU ID weight not available in tree, will re-calculate on the fly\n", __func__);
    fUseJetPUIDWeights = 1;
  }

  //Setup output histograms
  InitHistogramFlags();
  BookHistograms();
  printf("HistSelector::Begin (end)\n");
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i] = 0;
    fSysSets  [i] = 0;
    fTreeSets [i] = 0;
    fSetFills [i] = 0;
  }

  //Event Sets
  const bool mutau = fSelection == "" || fSelection == "mutau";
  const bool etau  = fSelection == "" || fSelection == "etau" ;
  const bool emu   = fSelection == "" || fSelection == "emu"   || fSelection.Contains("tau_"); //emu data for e+mu, e+tau_mu, and mu+tau_e
  const bool mumu  = fSelection == "" || fSelection == "mumu" ;
  const bool ee    = fSelection == "" || fSelection == "ee"   ;

  if(mutau) {
    fEventSets [kMuTau + 1] = 1; // all events
    fTreeSets  [kMuTau + 1] = 1;
  }
  if(etau) {
    fEventSets [kETau + 1] = 1; // all events
    fTreeSets  [kETau + 1] = 1;
  }
  if(emu) {
    fEventSets [kEMu  + 1] = 1; // all events
    fTreeSets  [kEMu  + 1] = 1;
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
void HistSelector::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
  // BookPhotonHistograms();
  if(fWriteTrees && fDoSystematics >= 0) BookTrees();
  if(fDoSystematics) BookSystematicHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::BookBaseEventHistograms(Int_t i, const char* dirname) {
      auto folder = fDirectories[0*fn + i];
      Utilities::BookH1F(fEventHist[i]->hEventWeight             , "eventweight"             , Form("%s: EventWeight"                 ,dirname), 100,   -1,   3, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEventWeight          , "logeventweight"          , Form("%s: LogEventWeight"              ,dirname),  50,  -10,   1, folder);
      Utilities::BookH1F(fEventHist[i]->hFullEventWeightLum      , "fulleventweightlum"      , Form("%s: abs(FullEventWeightLum)"     ,dirname), 100,    0,  15, folder);
      Utilities::BookH1F(fEventHist[i]->hLogFullEventWeightLum   , "logfulleventweightlum"   , Form("%s: log(abs(FullEventWeightLum))",dirname),  50,  -10,   2, folder);
      if(!fSparseHists) {
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
        Utilities::BookH1D(fEventHist[i]->hLHENJets                , "lhenjets"                , Form("%s: LHE N(jets)"                 ,dirname),  10,    0,  10, folder);
      }
      Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);
      Utilities::BookH1F(fEventHist[i]->hDataRun                 , "datarun"                 , Form("%s: DataRun"                     ,dirname), 100, 2.6e5, 3.3e5, folder);
      // Utilities::BookH1D(fEventHist[i]->hNPhotons                , "nphotons"                , Form("%s: NPhotons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets                   , "njets"                   , Form("%s: NJets"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNFwdJets                , "nfwdjets"                , Form("%s: NFwdJets"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);

      if(!fSparseHists) {
        Utilities::BookH1D(fEventHist[i]->hNTriggered         , "ntriggered"          , Form("%s: NTriggered"          ,dirname),   5,    0,   5, folder);
        Utilities::BookH1F(fEventHist[i]->hPuWeight           , "puweight"            , Form("%s: PUWeight"            ,dirname),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hJetPUIDWeight      , "jetpuidweight"       , Form("%s: JetPUIDWeight"       ,dirname),  50,    0,   2, folder);
        Utilities::BookH1D(fEventHist[i]->hPrefireWeight      , "prefireweight"       , Form("%s: PrefireWeight"       ,dirname),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hBTagWeight         , "btagweight"          , Form("%s: BTagWeight"          ,dirname),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hZPtWeight          , "zptweight"           , Form("%s: ZPtWeight"           ,dirname),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hSignalZWeight      , "signalzweight"       , Form("%s: SignalZWeight"       ,dirname),  50,    0,   2, folder);

        Utilities::BookH1F(fEventHist[i]->hTauPt              , "taupt"               , Form("%s: TauPt"               ,dirname),  50,    0, 200, folder);
        Utilities::BookH1F(fEventHist[i]->hTauEta             , "taueta"              , Form("%s: TauEta"              ,dirname),  30, -2.5, 2.5, folder);
      }

      Utilities::BookH1D(fEventHist[i]->hTauDecayMode[0]    , "taudecaymode"        , Form("%s: TauDecayMode"        ,dirname),  15,    0,  15, folder);
      Utilities::BookH1D(fEventHist[i]->hTauGenFlavor       , "taugenflavor"        , Form("%s: TauGenFlavor"        ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiEle     , "taudeepantiele"      , Form("%s: TauDeepAntiEle"      ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiMu      , "taudeepantimu"       , Form("%s: TauDeepAntiMu"       ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiJet     , "taudeepantijet"      , Form("%s: TauDeepAntiJet"      ,dirname),  30,    0,  30, folder);

      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hJetEta             , "jeteta"              , Form("%s: JetEta"              ,dirname), 100,   -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hHtSum              , "htsum"               , Form("%s: HtSum"               ,dirname), 100,    0, 400, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                 , "ht"                  , Form("%s: Ht"                  ,dirname), 100,    0, 200, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMetPhi              , "metphi"              , Form("%s: MetPhi"                  ,dirname)  ,  40, -4,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hMetCorr             , "metcorr"             , Form("%s: Met Correction"          ,dirname)  ,  40,  0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hMetUp               , "metup"               , Form("%s: Met up"                  ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMetDown             , "metdown"             , Form("%s: Met down"                ,dirname)  , 100,  0, 200, folder);

      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTDiff              , "mtdiff"              , Form("%s: MTDiff"                  ,dirname)  , 100, -100.,  100., folder);
      Utilities::BookH1F(fEventHist[i]->hMTOneOverM          , "mtoneoverm"          , Form("%s: MTOneOverM"              ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwoOverM          , "mttwooverm"          , Form("%s: MTTwoOverM"              ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMTLepOverM          , "mtlepoverm"          , Form("%s: MTLepOverM"              ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMETDotOne           , "metdotone"           , Form("%s: METDotOne"               ,dirname)  ,  50, 0.,   70. , folder);
      Utilities::BookH1F(fEventHist[i]->hMETDotTwo           , "metdottwo"           , Form("%s: METDotTwo"               ,dirname)  ,  50, 0.,   70. , folder);

      Utilities::BookH1F(fEventHist[i]->hMetDeltaPhi         , "metdeltaphi"         , Form("%s: Met Lep Delta Phi"       ,dirname)  ,  50,  0,   5, folder);

      if(!fSparseHists) {
        Utilities::BookH1F(fEventHist[i]->hLepOneDeltaPhi      , "leponedeltaphi"      , Form("%s: Lep One vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);
        Utilities::BookH1F(fEventHist[i]->hLepTwoDeltaPhi      , "leptwodeltaphi"      , Form("%s: Lep Two vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);
      }


      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateThree, "lepmestimatethree", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateFour, "lepmestimatefour", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateCut[0], "lepmestimatecut0"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateCut[1], "lepmestimatecut1"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateAvg[0], "lepmestimateavg0"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateAvg[1], "lepmestimateavg1"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMBalance    , "lepmbalance"    , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMBalanceTwo , "lepmbalancetwo" , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepPtOverM     , "lepptoverm"     , Form("%s: Lepton Pt / M"       ,dirname),  40,   0,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hLepTrkM        , "leptrkm"        , Form("%s: lep+trk mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepTrkDeltaM   , "leptrkdeltam"   , Form("%s: lepm - lep+trk mass"  ,dirname)  ,50,0., 100, folder);

      Utilities::BookH1F(fEventHist[i]->hPZetaVis       , "pzetavis"      , Form("%s: Dilepton pT dot #zeta",dirname), 50, 0., 200, folder);
      Utilities::BookH1F(fEventHist[i]->hPZetaInv       , "pzetainv"      , Form("%s: MET dot #zeta"        ,dirname), 50, -200., 200, folder);
      Utilities::BookH1F(fEventHist[i]->hPZetaAll       , "pzetaall"      , Form("%s: (l1 + l2 + MET) dot #zeta",dirname), 50, -200., 200, folder);
      Utilities::BookH1F(fEventHist[i]->hPZetaDiff      , "pzetadiff"     , Form("%s: (MET - Dilepton pT) dot #zeta",dirname), 50, -150., 100, folder);
      Utilities::BookH1F(fEventHist[i]->hDZeta          , "dzeta"         , Form("%s: (MET - 0.85*Dilepton pT) dot #zeta",dirname), 50, -150., 100, folder);

      Utilities::BookH1F(fEventHist[i]->hMETU1[0]       , "metu1"         , Form("%s: MET along di-lepton pT",dirname), 50, -100., 100, folder);
      Utilities::BookH1F(fEventHist[i]->hMETU2[0]       , "metu2"         , Form("%s: MET perpendiculat to di-lepton pT",dirname), 50, -100., 100, folder);

      Utilities::BookH1F(fEventHist[i]->hLepDeltaEta   , "lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[0]  , "lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[0], "lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hZEta[0]    , "zeta"          , Form("%s: ZEta"           ,dirname)  ,  50,  -10,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepOnePt  , "zleponept"     , Form("%s: ZLepOnePt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoPt  , "zleptwopt"     , Form("%s: ZLepTwoPt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepOneEta , "zleponeeta"    , Form("%s: ZLepOneEta"     ,dirname)  ,  50, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoEta , "zleptwoeta"    , Form("%s: ZLepTwoEta"     ,dirname)  ,  50, -2.5, 2.5, folder);
      Utilities::BookH1D(fEventHist[i]->hZDecayMode , "zdecaymode"    , Form("%s: ZDecayMode"     ,dirname)  ,  20,    0,  20, folder);

      if(!fSparseHists) {
        Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[0], "lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
        Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[1], "lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      }

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[0]    , "deltaalpham0"     , Form("%s: Delta Alpha M 0"     ,dirname),  80,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[1]    , "deltaalpham1"     , Form("%s: Delta Alpha M 1"     ,dirname),  80,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hBeta[0]           , "beta0"            , Form("%s: Beta (Z) 0"          ,dirname),  60,   0,  3., folder);
      Utilities::BookH1F(fEventHist[i]->hBeta[1]           , "beta1"            , Form("%s: Beta (Z) 1"          ,dirname),  60,   0,  3., folder);

      Utilities::BookH1F(fEventHist[i]->hPTauVisFrac    , "ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  , 50,0.,  1.5, folder);

      for(unsigned j = 0; j < fMVAConfig.names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig.names_[j].Data()) ,
                           fMVAConfig.NBins(j), fMVAConfig.Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig.names_[j].Data()), 3000, -1.,  2., folder);
        Utilities::BookH1F(fEventHist[i]->hMVATrain[j], Form("mvatrain%i",j), Form("%s: %s MVA (train)" ,dirname, fMVAConfig.names_[j].Data()),  100, -3.,  2., folder);
        Utilities::BookH1F(fEventHist[i]->hMVATest[j] , Form("mvatest%i",j) , Form("%s: %s MVA (test)"  ,dirname, fMVAConfig.names_[j].Data()),  100, -3.,  2., folder);
      }

}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::BookEventHistograms() {
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
void HistSelector::BookPhotonHistograms() {
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
void HistSelector::BookBaseLepHistograms(Int_t i, const char* dirname) {
      auto folder = fDirectories[2*fn + i];

      const double momerr_bins[] = {0., 5.e-4f, 1.e-3f, 2.e-3f, 5.e-3f, 0.01f, 0.02f, 0.05f};
      const int    n_momerr_bins = sizeof(momerr_bins) / sizeof(*momerr_bins) - 1;

      // Lepton one info //
      Utilities::BookH1F(fLepHist[i]->hOnePt[0]       , "onept"            , Form("%s: Pt"           ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePz          , "onepz"            , Form("%s: Pz"           ,dirname), 100, -100,  100, folder);
      Utilities::BookH1F(fLepHist[i]->hOneMomErr      , "onemomerr"        , Form("%s: Mom. Error"   ,dirname),  n_momerr_bins, momerr_bins, folder);
      Utilities::BookH1F(fLepHist[i]->hOneEta         , "oneeta"           , Form("%s: Eta"          ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneSCEta       , "onesceta"         , Form("%s: SC Eta"       ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePhi         , "onephi"           , Form("%s: Phi"          ,dirname),  80,   -4,    4, folder);
      Utilities::BookH1F(fLepHist[i]->hOneD0          , "oned0"            , Form("%s: D0"           ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDXY         , "onedxy"           , Form("%s: DXY"          ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDZ          , "onedz"            , Form("%s: DZ"           ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDXYSig      , "onedxysig"        , Form("%s: DXY/err"      ,dirname),  30,    0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDZSig       , "onedzsig"         , Form("%s: DZ/err"       ,dirname),  30,    0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneIso         , "oneiso"           , Form("%s: Iso"          ,dirname),  50,    0,    5, folder);
      Utilities::BookH1D(fLepHist[i]->hOneID1         , "oneid1"           , Form("%s: ID1"          ,dirname),  80,   -1,   79, folder);
      Utilities::BookH1D(fLepHist[i]->hOneID2         , "oneid2"           , Form("%s: ID2"          ,dirname),  80,   -1,   79, folder);
      Utilities::BookH1F(fLepHist[i]->hOneRelIso      , "onereliso"        , Form("%s: Iso / Pt"     ,dirname),  50,    0,  0.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneTrkPtOverPt , "onetrkptoverpt"   , Form("%s: Trk Pt / Pt"  ,dirname),  40,    0,  1.2, folder);
      Utilities::BookH1F(fLepHist[i]->hOneTrkDeltaEta , "onetrkdeltaeta"   , Form("%s: Trk Eta - Eta",dirname),  20,    0,  0.2, folder);
      Utilities::BookH1F(fLepHist[i]->hOneTrkDeltaPhi , "onetrkdeltaphi"   , Form("%s: Trk Phi - Phi",dirname),  20,    0,  0.2, folder);
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
      Utilities::BookH1F(fLepHist[i]->hTwoMomErr      , "twomomerr"        , Form("%s: Mom. Error"  ,dirname),  n_momerr_bins, momerr_bins, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoEta         , "twoeta"           , Form("%s: Eta"         ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoSCEta       , "twosceta"         , Form("%s: SC Eta"      ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPhi         , "twophi"           , Form("%s: Phi"         ,dirname),  80,  -4,    4, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoD0          , "twod0"            , Form("%s: D0"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXY         , "twodxy"           , Form("%s: DXY"         ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZ          , "twodz"            , Form("%s: DZ"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXYSig      , "twodxysig"        , Form("%s: DXY/err"     ,dirname),  30,   0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZSig       , "twodzsig"         , Form("%s: DZ/err"      ,dirname),  30,   0,    5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoIso         , "twoiso"           , Form("%s: Iso"         ,dirname),  50,   0,    5, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoID1         , "twoid1"           , Form("%s: ID1"         ,dirname),  80,  -1,   79, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoID2         , "twoid2"           , Form("%s: ID2"         ,dirname),  80,  -1,   79, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoID3         , "twoid3"           , Form("%s: ID3"         ,dirname),  80,  -1,   79, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoRelIso      , "tworeliso"        , Form("%s: Iso / Pt"    ,dirname),  50,   0,  0.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoTrkPtOverPt , "twotrkptoverpt"   , Form("%s: Trk Pt / Pt"  ,dirname),  40,    0,  1.2, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoTrkDeltaEta , "twotrkdeltaeta"   , Form("%s: Trk Eta - Eta",dirname),  20,    0,  0.2, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoTrkDeltaPhi , "twotrkdeltaphi"   , Form("%s: Trk Phi - Phi",dirname),  20,    0,  0.2, folder);
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
      Utilities::BookH1F(fLepHist[i]->hPtRatio        , "ptratio"          , Form("%s: 1 pT / 2 pT"  ,dirname),  30,    0,    6, folder);
      Utilities::BookH1F(fLepHist[i]->hD0Diff         , "d0diff"           , Form("%s: 2 D0 - 1 D0"  ,dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDXYDiff        , "dxydiff"          , Form("%s: 2 DXY - 1 DXY",dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDZDiff         , "dzdiff"           , Form("%s: 2 DZ - 1 DZ"  ,dirname), 100,-0.08, 0.08, folder);

}
//--------------------------------------------------------------------------------------------------------------
void HistSelector::BookLepHistograms() {
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
void HistSelector::BookSystematicHistograms() {
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
void HistSelector::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      fTrees[i] = new TTree(Form("tree_%i",i),Form("HistSelector TTree %i",i));
      BookBaseTree(i);
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::BookBaseTree(Int_t index) {
  fTrees[index]->Branch("leponept"           , &fTreeVars.leponept          );
  fTrees[index]->Branch("leponeptoverm"      , &fTreeVars.leponeptoverm     );
  fTrees[index]->Branch("leptwopt"           , &fTreeVars.leptwopt          );
  fTrees[index]->Branch("leptwoptoverm"      , &fTreeVars.leptwoptoverm     );
  // fTrees[index]->Branch("leptwod0"           , &fTreeVars.leptwod0          ); //FIXME: Remove
  fTrees[index]->Branch("leppt"              , &fTreeVars.leppt             );
  fTrees[index]->Branch("lepm"               , &fTreeVars.lepm              );
  fTrees[index]->Branch("lepptoverm"         , &fTreeVars.lepptoverm        );
  fTrees[index]->Branch("lepeta"             , &fTreeVars.lepeta            );
  fTrees[index]->Branch("lepdeltaeta"        , &fTreeVars.lepdeltaeta       );
  fTrees[index]->Branch("lepdeltar"          , &fTreeVars.lepdeltar         );
  fTrees[index]->Branch("lepdeltaphi"        , &fTreeVars.lepdeltaphi       );
  fTrees[index]->Branch("ptdiff"             , &fTreeVars.ptdiff            );
  fTrees[index]->Branch("ptdiffoverm"        , &fTreeVars.ptdiffoverm       );
  fTrees[index]->Branch("ptratio"            , &fTreeVars.ptratio           );
  fTrees[index]->Branch("deltaalphaz1"       , &fTreeVars.deltaalphaz1      );
  fTrees[index]->Branch("deltaalphaz2"       , &fTreeVars.deltaalphaz2      );
  fTrees[index]->Branch("deltaalphah1"       , &fTreeVars.deltaalphah1      );
  fTrees[index]->Branch("deltaalphah2"       , &fTreeVars.deltaalphah2      );
  fTrees[index]->Branch("deltaalpham1"       , &fTreeVars.deltaalpham1      );
  fTrees[index]->Branch("deltaalpham2"       , &fTreeVars.deltaalpham2      );
  fTrees[index]->Branch("beta1"              , &fTreeVars.beta1             );
  fTrees[index]->Branch("beta2"              , &fTreeVars.beta2             );
  fTrees[index]->Branch("metdeltaphi"        , &fTreeVars.metdeltaphi       );
  fTrees[index]->Branch("onemetdeltaphi"     , &fTreeVars.onemetdeltaphi    );
  fTrees[index]->Branch("twometdeltaphi"     , &fTreeVars.twometdeltaphi    );
  fTrees[index]->Branch("lepmestimate"       , &fTreeVars.mestimate         );
  fTrees[index]->Branch("lepmestimatetwo"    , &fTreeVars.mestimatetwo      );
  fTrees[index]->Branch("lepmestimatethree"  , &fTreeVars.mestimatethree    );
  fTrees[index]->Branch("lepmestimatefour"   , &fTreeVars.mestimatefour     );
  fTrees[index]->Branch("lepmestimatecut1"   , &fTreeVars.mestimate_cut_1   );
  fTrees[index]->Branch("lepmestimatecut2"   , &fTreeVars.mestimate_cut_2   );
  fTrees[index]->Branch("lepmestimateavg1"   , &fTreeVars.mestimate_avg_1   );
  fTrees[index]->Branch("lepmestimateavg2"   , &fTreeVars.mestimate_avg_2   );
  fTrees[index]->Branch("lepmbalance"        , &fTreeVars.mbalance          );
  fTrees[index]->Branch("lepmbalancetwo"     , &fTreeVars.mbalancetwo       );
  fTrees[index]->Branch("ptauvisfrac"        , &fTreeVars.ptauvisfrac       );
  fTrees[index]->Branch("met"                , &fTreeVars.met               );
  fTrees[index]->Branch("mtone"              , &fTreeVars.mtone             );
  fTrees[index]->Branch("mttwo"              , &fTreeVars.mttwo             );
  fTrees[index]->Branch("mtdiff"             , &fTreeVars.mtdiff            );
  fTrees[index]->Branch("mtlep"              , &fTreeVars.mtlep             );
  fTrees[index]->Branch("mtoneoverm"         , &fTreeVars.mtoneoverm        );
  fTrees[index]->Branch("mttwooverm"         , &fTreeVars.mttwooverm        );
  fTrees[index]->Branch("mtlepoverm"         , &fTreeVars.mtlepoverm        );
  fTrees[index]->Branch("jetpt"              , &fTreeVars.jetpt             );
  fTrees[index]->Branch("njets"              , &fTreeVars.njets             );
  fTrees[index]->Branch("ht"                 , &fTreeVars.ht                );
  fTrees[index]->Branch("htsum"              , &fTreeVars.htsum             );
  fTrees[index]->Branch("pzetavis"           , &fTreeVars.pzetavis          );
  fTrees[index]->Branch("pzetainv"           , &fTreeVars.pzetainv          );
  fTrees[index]->Branch("pzetaall"           , &fTreeVars.pzetaall          );
  fTrees[index]->Branch("dzeta"              , &fTreeVars.dzeta             );
  fTrees[index]->Branch("leptrkdeltam"       , &fTreeVars.leptrkdeltam      );
  fTrees[index]->Branch("trkptoverpt"        , &fTreeVars.trkptoverpt       );
  fTrees[index]->Branch("taudecaymode"       , &fTreeVars.taudecaymode      );
  fTrees[index]->Branch("taugenflavor"       , &fTreeVars.taugenflavor      );

  //boosted frame variables
  for(int mode = 0; mode < 3; ++mode) {
    fTrees[index]->Branch(Form("leponeprimepx%i", mode), &fTreeVars.leponeprimepx[mode]);
    fTrees[index]->Branch(Form("leptwoprimepx%i", mode), &fTreeVars.leptwoprimepx[mode]);
    // fTrees[index]->Branch(Form("metprimepx%i"   , mode), &fTreeVars.metprimepx   [mode]);
    // fTrees[index]->Branch(Form("leponeprimepy%i", mode), &fTreeVars.leponeprimepy[mode]);
    // fTrees[index]->Branch(Form("leptwoprimepy%i", mode), &fTreeVars.leptwoprimepy[mode]);
    // fTrees[index]->Branch(Form("metprimepy%i"   , mode), &fTreeVars.metprimepy   [mode]);
    fTrees[index]->Branch(Form("leponeprimepz%i", mode), &fTreeVars.leponeprimepz[mode]);
    fTrees[index]->Branch(Form("leptwoprimepz%i", mode), &fTreeVars.leptwoprimepz[mode]);
    // fTrees[index]->Branch(Form("metprimepz%i"   , mode), &fTreeVars.metprimepz   [mode]);
    fTrees[index]->Branch(Form("leponeprimee%i" , mode), &fTreeVars.leponeprimee [mode]);
    fTrees[index]->Branch(Form("leptwoprimee%i" , mode), &fTreeVars.leptwoprimee [mode]);
    fTrees[index]->Branch(Form("metprimee%i"    , mode), &fTreeVars.metprimee    [mode]);
  }

  fTrees[index]->Branch("eventweight"       , &fTreeVars.eventweightMVA    ); //event weight
  fTrees[index]->Branch("fulleventweight"   , &fTreeVars.fulleventweight   ); //event weight * xsec / N(gen)
  fTrees[index]->Branch("fulleventweightlum", &fTreeVars.fulleventweightlum); //event weight * xsec / N(gen) * luminosity
  fTrees[index]->Branch("category"          , &fTreeVars.category          ); //etau/mutau/etc.
  fTrees[index]->Branch("train"             , &fTreeVars.train             );
  fTrees[index]->Branch("issignal"          , &fTreeVars.issignal          );
  fTrees[index]->Branch("type"              , &fTreeVars.type              ); //background category (Top, DY, etc.)
  fTrees[index]->Branch("year"              , &fTreeVars.year              );

  //FIXME: Add a few more important effects, perhaps bin IDs as well
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    fTrees[index]->Branch(Form("jettotaustatup%i"  , proc), &fTreeVars.jettotaustatup  [proc]);
    fTrees[index]->Branch(Form("jettotaustatdown%i", proc), &fTreeVars.jettotaustatdown[proc]);
    fTrees[index]->Branch(Form("jettotaunoncl%i"   , proc), &fTreeVars.jettotaunoncl   [proc]);
    fTrees[index]->Branch(Form("jettotaubias%i"    , proc), &fTreeVars.jettotaubias    [proc]);
  }
  fTrees[index]->Branch("jettotaucomp"       , &fTreeVars.jettotaucomp         );

  fTrees[index]->Branch("jetantimusys"       , &fTreeVars.jetantimusys         );
  fTrees[index]->Branch("jetantielesys"      , &fTreeVars.jetantielesys        );
  fTrees[index]->Branch("qcdstat"            , &fTreeVars.qcdstat              );
  fTrees[index]->Branch("qcdnc"              , &fTreeVars.qcdnc                );
  fTrees[index]->Branch("qcdbias"            , &fTreeVars.qcdbias              );
  fTrees[index]->Branch("btagsys"            , &fTreeVars.btagsys              );
  fTrees[index]->Branch("lumisys"            , &fTreeVars.lumisys              );
  fTrees[index]->Branch("zptsys"             , &fTreeVars.zptsys               );

  //lepton ID uncertainties
  fTrees[index]->Branch("leponeid1sys"       , &fTreeVars.leponeid1sys         );
  fTrees[index]->Branch("leponeid2sys"       , &fTreeVars.leponeid2sys         );
  fTrees[index]->Branch("leptwoid1sys"       , &fTreeVars.leptwoid1sys         );
  fTrees[index]->Branch("leptwoid2sys"       , &fTreeVars.leptwoid2sys         );
  fTrees[index]->Branch("leponeid1bin"       , &fTreeVars.leponeid1bin         );
  fTrees[index]->Branch("leptwoid1bin"       , &fTreeVars.leptwoid1bin         );

  //add MVA score branches
  for(unsigned imva = 0; imva < fMVAConfig.names_.size(); ++imva) {
    fTrees[index]->Branch(Form("mva%i", imva), &fMvaOutputs[imva]);
  }
}


//--------------------------------------------------------------------------------------------------------------
void HistSelector::FillAllHistograms(Int_t index) {
  if(eventWeight < 0. || !std::isfinite(eventWeight*(*genWeight))) {
    std::cout << "WARNING! Event " << fentry << ": Set " << index << " has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << *genWeight << "; puWeight = " << *puWeight
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOne.trig_wt*leptonTwo.trig_wt
              << "; jetPUIDWeight = " << jetPUIDWeight << "; zPtWeight = " << zPtWeight
              << std::endl;
  }
  if(!std::isfinite(eventWeight*(*genWeight))) {
    eventWeight = 0.;
    *genWeight = 1.;
  }
  //check the event passes the correct cuts
  const bool pass = PassesCuts();
  if(fEventSets [index]) {
    if(fVerbose > 0) std::cout << "Filling histograms for set " << index
                               << " with event weight = " << eventWeight
                               << " and gen weight = " << *genWeight << " !\n";
    fTimes[GetTimerNumber("Filling")] = std::chrono::steady_clock::now(); //timer for reading from the tree

    if(pass) {
      ++fSetFills[index];
      FillEventHistogram( fEventHist [index]);
      FillLepHistogram(   fLepHist   [index]);
    }
    // //allow events outside the window to the systematics filling, to account for shifted events migrating in/out
    // if(fDoSystematics && fSysSets[index]) {
    //   fTimes[GetTimerNumber("SystFill")] = std::chrono::steady_clock::now(); //timer for reading from the tree
    //   FillSystematicHistogram(fSystematicHist[index]);
    //   IncrementTimer("SystFill", true);
    // }
    // if(fFillPhotonHists) FillPhotonHistogram(fPhotonHist[index]);
    IncrementTimer("Filling", true);
  } else {
    printf("WARNING! Entry %lld: Attempted to fill un-initialized histogram set %i!\n", fentry, index);
  }
  if(pass && fDoSystematics >= 0 && fWriteTrees && fTreeSets[index]) {
    if(fVerbose > 0) {
      printf("HistSelector::%s: Filling tree %i\n", __func__, index);
    }
    fDirectories[3*fn + index]->cd();
    fTrees[index]->Fill();
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::FillBaseEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("HistSelector::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  const float tot_wt = eventWeight*(*genWeight);
  Hist->hEventWeight              ->Fill(eventWeight);
  Hist->hLogEventWeight           ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hFullEventWeightLum       ->Fill(std::fabs(fTreeVars.fulleventweightlum), tot_wt);
  Hist->hLogFullEventWeightLum    ->Fill((std::fabs(fTreeVars.fulleventweightlum) > 1.e-10) ? std::log10(std::fabs(fTreeVars.fulleventweightlum)) : -999., tot_wt);
  if(!fSparseHists) {
    Hist->hEventWeightMVA           ->Fill(fTreeVars.eventweightMVA);
    Hist->hGenWeight                ->Fill(*genWeight                  , eventWeight);
    Hist->hEmbeddingWeight          ->Fill(embeddingWeight             , tot_wt);
    Hist->hLogEmbeddingWeight       ->Fill((embeddingWeight > 1.e-10) ? std::log10(embeddingWeight) : -999. , tot_wt     );
    Hist->hEmbeddingUnfoldingWeight ->Fill(embeddingUnfoldingWeight);
    Hist->hQCDWeight                ->Fill(qcdWeight     , tot_wt);
    Hist->hJetToTauWeight           ->Fill(jetToTauWeight, tot_wt);
    Hist->hJetToTauWeightCorr       ->Fill((jetToTauWeight     > 0.) ? jetToTauWeightCorr/jetToTauWeight     : -1., tot_wt);
    Hist->hJetToTauWeightBias       ->Fill((jetToTauWeightCorr > 0.) ? jetToTauWeightBias/jetToTauWeightCorr : -1., tot_wt);

    Hist->hIsSignal            ->Fill(fTreeVars.issignal      );
    Hist->hNMuons              ->Fill(nMuons             , tot_wt)      ;
    Hist->hNElectrons          ->Fill(nElectrons         , tot_wt)      ;
    Hist->hNTaus               ->Fill(nTaus              , tot_wt)      ;
    Hist->hNPV[0]              ->Fill(nPV                , tot_wt)      ;
    Hist->hNPU[0]              ->Fill(nPU                , tot_wt)      ;
    Hist->hLHENJets            ->Fill(LHE_Njets          , tot_wt)      ;
  }
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), tot_wt) ;
  Hist->hDataRun             ->Fill(runNumber          , tot_wt)      ;
  // Hist->hNPhotons            ->Fill(nPhotons           , tot_wt)      ;
  Hist->hNGenTaus            ->Fill(nGenTaus           , tot_wt)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons      , tot_wt)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons          , tot_wt)      ;
  Hist->hNJets               ->Fill(nJets              , tot_wt)      ;
  Hist->hNJets20[0]          ->Fill(nJets20            , tot_wt)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , tot_wt)      ;
  Hist->hNBJets20[0]         ->Fill(nBJets20           , tot_wt)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , tot_wt)      ;

  if(!fSparseHists) {
    Hist->hNTriggered          ->Fill(leptonOne.fired+leptonTwo.fired, tot_wt)   ;
    Hist->hPuWeight            ->Fill(*puWeight)    ;
    Hist->hJetPUIDWeight       ->Fill(jetPUIDWeight);
    Hist->hPrefireWeight       ->Fill(prefireWeight);
    Hist->hBTagWeight          ->Fill(btagWeight)   ;
    Hist->hZPtWeight           ->Fill(zPtWeight)    ;
    Hist->hSignalZWeight       ->Fill(signalZWeight);

    if(tauP4 && tauP4->Pt() > 0.) { //if 0 then no additional tau stored
      Hist->hTauPt           ->Fill(tauP4->Pt()        , tot_wt)   ;
      Hist->hTauEta          ->Fill(tauP4->Eta()       , tot_wt)   ;
    }
  }
  Hist->hTauGenFlavor      ->Fill(tauGenFlavor               , tot_wt)   ;
  Hist->hTauDecayMode[0]   ->Fill(tauDecayMode               , tot_wt)   ;
  Hist->hTauDeepAntiEle    ->Fill(std::log2(tauDeepAntiEle+1), tot_wt)   ;
  Hist->hTauDeepAntiMu     ->Fill(tauDeepAntiMu              , tot_wt)   ;
  Hist->hTauDeepAntiJet    ->Fill(std::log2(tauDeepAntiJet+1), tot_wt)   ;

  if(jetOne.p4 && jetOne.p4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOne.p4->Pt()            , tot_wt)   ;
    Hist->hJetEta          ->Fill(jetOne.p4->Eta()           , tot_wt)   ;
  }
  Hist->hHtSum             ->Fill(htSum              , tot_wt)   ;
  Hist->hHt                ->Fill(ht                 , tot_wt)   ;

  Hist->hMet               ->Fill(met                , tot_wt)      ;
  Hist->hMetPhi            ->Fill(metPhi             , tot_wt)      ;
  Hist->hMetCorr           ->Fill(metCorr            , tot_wt)      ;
  //approximate met uncertainty
  const float met_err   = (fIsData) ? 0.f : std::sqrt(std::pow(*PuppiMET_ptJERUp - *PuppiMET_pt, 2) + std::pow(*PuppiMET_ptJESUp - *PuppiMET_pt, 2))/(*PuppiMET_pt);
  Hist->hMetUp             ->Fill(met*(1.f+met_err)  , tot_wt);
  Hist->hMetDown           ->Fill(met*(1.f-met_err)  , tot_wt);

  Hist->hMTOne             ->Fill(fTreeVars.mtone    , tot_wt);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , tot_wt);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , tot_wt);
  Hist->hMTDiff            ->Fill(fTreeVars.mtdiff   , tot_wt);
  Hist->hMTOneOverM        ->Fill(fTreeVars.mtoneoverm  ,tot_wt);
  Hist->hMTTwoOverM        ->Fill(fTreeVars.mttwooverm  ,tot_wt);
  Hist->hMTLepOverM        ->Fill(fTreeVars.mtlepoverm  ,tot_wt);
  Hist->hMETDotOne         ->Fill(fTreeVars.metdotl1    , tot_wt);
  Hist->hMETDotTwo         ->Fill(fTreeVars.metdotl2    , tot_wt);

  TLorentzVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
  const double lepDelR   = std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4));
  const double lepDelPhi = std::fabs(leptonOne.p4->DeltaPhi(*leptonTwo.p4));
  const double lepDelEta = std::fabs(leptonOne.p4->Eta() - leptonTwo.p4->Eta());
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  Hist->hMetDeltaPhi  ->Fill(fTreeVars.metdeltaphi  ,tot_wt);
  if(!fSparseHists) {
    Hist->hLepOneDeltaPhi->Fill(fTreeVars.leponedeltaphi  ,tot_wt);
    Hist->hLepTwoDeltaPhi->Fill(fTreeVars.leptwodeltaphi  ,tot_wt);
  }

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,tot_wt);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,tot_wt);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,tot_wt);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,tot_wt);
  // Hist->hLepPhi       ->Fill(lepSys.Phi()           ,tot_wt);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , tot_wt);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, tot_wt);
  Hist->hLepMEstimateThree->Fill(fTreeVars.mestimatethree, tot_wt);
  Hist->hLepMEstimateFour->Fill(fTreeVars.mestimatefour, tot_wt);
  Hist->hLepMEstimateCut[0]->Fill(fTreeVars.mestimate_cut_1, tot_wt);
  Hist->hLepMEstimateCut[1]->Fill(fTreeVars.mestimate_cut_2, tot_wt);
  Hist->hLepMEstimateAvg[0]->Fill(fTreeVars.mestimate_avg_1, tot_wt);
  Hist->hLepMEstimateAvg[1]->Fill(fTreeVars.mestimate_avg_2, tot_wt);
  Hist->hLepMBalance    ->Fill(fTreeVars.mbalance    , tot_wt);
  Hist->hLepMBalanceTwo ->Fill(fTreeVars.mbalancetwo , tot_wt);
  Hist->hLepTrkM        ->Fill(fTreeVars.leptrkm     , tot_wt);
  Hist->hLepTrkDeltaM   ->Fill(fTreeVars.lepm - fTreeVars.leptrkm, tot_wt);
  Hist->hPZetaVis       ->Fill(fTreeVars.pzetavis    , tot_wt);
  Hist->hPZetaInv       ->Fill(fTreeVars.pzetainv    , tot_wt);
  Hist->hPZetaAll       ->Fill(fTreeVars.pzetaall    , tot_wt);
  Hist->hPZetaDiff      ->Fill(fTreeVars.pzetainv - fTreeVars.pzetavis     , tot_wt);
  Hist->hDZeta          ->Fill(fTreeVars.dzeta       , tot_wt);

  Hist->hMETU1[0]       ->Fill(fTreeVars.met_u1      , tot_wt);
  Hist->hMETU2[0]       ->Fill(fTreeVars.met_u2      , tot_wt);

  Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,tot_wt);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,tot_wt);
  Hist->hLepDeltaR[0] ->Fill(lepDelR                ,tot_wt);
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,tot_wt);

  Hist->hZEta[0]      ->Fill(zEta      , tot_wt);
  Hist->hZLepOnePt    ->Fill(zLepOnePt , tot_wt);
  Hist->hZLepTwoPt    ->Fill(zLepTwoPt , tot_wt);
  Hist->hZLepOneEta   ->Fill(zLepOneEta, tot_wt);
  Hist->hZLepTwoEta   ->Fill(zLepTwoEta, tot_wt);
  Hist->hZDecayMode   ->Fill(fTreeVars.zdecaymode, tot_wt);

  Hist->hPTauVisFrac  ->Fill(fTreeVars.ptauvisfrac , tot_wt);
  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, tot_wt);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, tot_wt);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, tot_wt);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, tot_wt);
  Hist->hDeltaAlphaM[0]->Fill(fTreeVars.deltaalpham1, tot_wt);
  Hist->hDeltaAlphaM[1]->Fill(fTreeVars.deltaalpham2, tot_wt);
  Hist->hBeta[0]->Fill(fTreeVars.beta1, tot_wt);
  Hist->hBeta[1]->Fill(fTreeVars.beta2, tot_wt);
  // Hist->hDeltaAlphaMColM[0]->Fill(fTreeVars.mestimate    - fTreeVars.deltaalpham2, tot_wt);
  // Hist->hDeltaAlphaMColM[1]->Fill(fTreeVars.mestimatetwo - fTreeVars.deltaalpham1, tot_wt);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    //MVA score vs mass for MVAs not correlated with mass
    if(!fSparseHists) {
      if(fMVAConfig.names_[i].Contains("emu")) {
        if(fMVAConfig.names_[i].Contains("Z0")) Hist->hLepMVsMVA[0]->Fill(fMvaOutputs[i], fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
        else                                    Hist->hLepMVsMVA[1]->Fill(fMvaOutputs[i], fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
      }
    }
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    if (fTreeVars.train > 0) Hist->hMVATrain[i]->Fill(fMvaOutputs[i], fTreeVars.eventweight*((fFractionMVA > 0.f) ? 1.f/fFractionMVA : 1.f));
    if (fTreeVars.train < 0) Hist->hMVATest[i] ->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA);
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("HistSelector::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  FillBaseEventHistogram(Hist);
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::FillBaseLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("HistSelector::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  /////////////
  //  Lep 1  //
  /////////////
  const float tot_wt = eventWeight*(*genWeight);
  Hist->hOnePz        ->Fill(leptonOne.p4->Pz()            ,tot_wt);
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,tot_wt);
  Hist->hOneMomErr    ->Fill(std::fabs(1.f - leptonOne.ES[1]/leptonOne.ES[0]), tot_wt);
  Hist->hOneEta       ->Fill(leptonOne.p4->Eta()           ,tot_wt);
  Hist->hOneSCEta     ->Fill(leptonOne.scEta               ,tot_wt);
  Hist->hOnePhi       ->Fill(leptonOne.p4->Phi()           ,tot_wt);
  Hist->hOneD0        ->Fill(leptonOne.d0                  ,tot_wt);
  Hist->hOneDXY       ->Fill(leptonOne.dxy                 ,tot_wt);
  Hist->hOneDZ        ->Fill(leptonOne.dz                  ,tot_wt);
  Hist->hOneDXYSig    ->Fill(leptonOne.dxySig              ,tot_wt);
  Hist->hOneDZSig     ->Fill(leptonOne.dzSig               ,tot_wt);
  Hist->hOneIso       ->Fill(leptonOne.iso                 ,tot_wt);
  Hist->hOneID1       ->Fill(leptonOne.id1                 ,tot_wt);
  Hist->hOneID2       ->Fill(leptonOne.id2                 ,tot_wt);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,tot_wt);
  Hist->hOneTrkPtOverPt->Fill(leptonOne.trkpt / leptonOne.p4->Pt() ,tot_wt);
  Hist->hOneTrkDeltaEta->Fill(std::fabs(leptonOne.trketa - leptonOne.p4->Eta()) ,tot_wt);
  Hist->hOneTrkDeltaPhi->Fill(std::fabs(Utilities::DeltaPhi(leptonOne.trkphi, leptonOne.p4->Phi())) ,tot_wt);
  Hist->hOneFlavor    ->Fill(std::abs(leptonOne.flavor)    ,tot_wt);
  Hist->hOneGenFlavor ->Fill(leptonOne.genFlavor           ,tot_wt);
  Hist->hOneQ         ->Fill(leptonOne.flavor < 0 ? -1 : 1 ,tot_wt);
  Hist->hOneJetOverlap->Fill(leptonOne.jetOverlap          ,tot_wt);
  Hist->hOneTrigger   ->Fill(leptonOne.trigger             ,tot_wt);
  Hist->hOneWeight    ->Fill(leptonOne.wt1[0]*leptonOne.wt2[0],tot_wt);
  Hist->hOneTrigWeight->Fill(leptonOne.trig_wt          ,tot_wt);

  Hist->hOneMetDeltaPhi   ->Fill(fTreeVars.onemetdeltaphi   ,tot_wt);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwo.p4->Pz()            ,tot_wt);
  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,tot_wt);
  Hist->hTwoMomErr    ->Fill(std::fabs(1.f - leptonTwo.ES[1]/leptonTwo.ES[0]), tot_wt);
  Hist->hTwoEta       ->Fill(leptonTwo.p4->Eta()           ,tot_wt);
  Hist->hTwoSCEta     ->Fill(leptonTwo.scEta               ,tot_wt);
  Hist->hTwoPhi       ->Fill(leptonTwo.p4->Phi()           ,tot_wt);
  Hist->hTwoD0        ->Fill(leptonTwo.d0                  ,tot_wt);
  Hist->hTwoDXY       ->Fill(leptonTwo.dxy                 ,tot_wt);
  Hist->hTwoDZ        ->Fill(leptonTwo.dz                  ,tot_wt);
  Hist->hTwoDXYSig    ->Fill(leptonTwo.dxySig              ,tot_wt);
  Hist->hTwoDZSig     ->Fill(leptonTwo.dzSig               ,tot_wt);
  Hist->hTwoIso       ->Fill(leptonTwo.iso                 ,tot_wt);
  Hist->hTwoID1       ->Fill(leptonTwo.id1                 ,tot_wt);
  Hist->hTwoID2       ->Fill(leptonTwo.id2                 ,tot_wt);
  Hist->hTwoID3       ->Fill(leptonTwo.id3                 ,tot_wt);
  Hist->hTwoTrkPtOverPt->Fill(leptonTwo.trkpt / leptonTwo.p4->Pt() ,tot_wt);
  Hist->hTwoTrkDeltaEta->Fill(std::fabs(leptonTwo.trketa - leptonTwo.p4->Eta()) ,tot_wt);
  Hist->hTwoTrkDeltaPhi->Fill(std::fabs(Utilities::DeltaPhi(leptonTwo.trkphi, leptonTwo.p4->Phi())) ,tot_wt);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,tot_wt);
  Hist->hTwoFlavor    ->Fill(std::abs(leptonTwo.flavor)        ,tot_wt);
  Hist->hTwoGenFlavor ->Fill(leptonTwo.genFlavor           ,tot_wt);
  Hist->hTwoQ         ->Fill(leptonTwo.flavor < 0 ? -1 : 1 ,tot_wt);
  Hist->hTwoJetOverlap->Fill(leptonTwo.jetOverlap          ,tot_wt);
  Hist->hTwoTrigger   ->Fill(leptonTwo.trigger               ,tot_wt);
  Hist->hTwoWeight    ->Fill(leptonTwo.wt1[0]*leptonTwo.wt2[0],tot_wt);
  Hist->hTwoTrigWeight->Fill(leptonTwo.trig_wt          ,tot_wt);

  Hist->hTwoMetDeltaPhi   ->Fill(fTreeVars.twometdeltaphi  ,tot_wt);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.ptdiff                      ,tot_wt);
  Hist->hPtRatio     ->Fill(fTreeVars.ptratio                     ,tot_wt);
  Hist->hD0Diff      ->Fill(leptonTwo.d0-leptonOne.d0             ,tot_wt);
  Hist->hDXYDiff     ->Fill(leptonTwo.dxy-leptonOne.dxy           ,tot_wt);
  Hist->hDZDiff      ->Fill(leptonTwo.dz-leptonOne.dz             ,tot_wt);
  // Hist->hTwoPtVsOnePt->Fill(fTreeVars.leponept, fTreeVars.leptwopt,tot_wt);
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::FillLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("HistSelector::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  FillBaseLepHistogram(Hist);
}

//-----------------------------------------------------------------------------------------------------------------
//apply/replace electron energy scale corrections
void HistSelector::ApplyElectronCorrections() {
  float delta_x(metCorr*std::cos(metCorrPhi)), delta_y(metCorr*std::sin(metCorrPhi));
  for(UInt_t index = 0; index < *nElectron; ++index) {
    float sf = (fIsEmbed) ? fElectronIDWeight.EmbedEnergyScale(Electron_pt[index], Electron_eta[index],
                                                               fYear, Electron_energyScaleUp[index], Electron_energyScaleDown[index]) : 1.f;
    Electron_energyScale[index] = sf;
    if(!fIsEmbed) { //FIXME: Get electron energy scale uncertainty in MC
      TLorentzVector lv; lv.SetPtEtaPhiM(Electron_pt[index], Electron_eta[index], Electron_phi[index], 0.f/*Electron_mass[index]*/);
      Electron_energyScaleUp  [index] = 1.f + Electron_energyErr[index] / lv.E();
      Electron_energyScaleDown[index] = 1.f - Electron_energyErr[index] / lv.E();
    }
    if(fIsEmbed) sf /= Electron_eCorr[index]; //remove the NANOAOD defined correction applied to embedding electrons
    float pt_diff = Electron_pt[index];
    Electron_pt[index] *= sf;
    // Electron_mass[index] *= sf;
    pt_diff = Electron_pt[index] - pt_diff; //New - Old
    //subtract the change from the MET
    delta_x -= pt_diff*std::cos(Electron_phi[index]);
    delta_y -= pt_diff*std::sin(Electron_phi[index]);
  }
  metCorr = std::sqrt(delta_x*delta_x + delta_y*delta_y);
  metCorrPhi = (metCorr > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, delta_x/metCorr)))*(delta_y < 0.f ? -1 : 1) : 0.f;
  if(!std::isfinite(metCorr) || !std::isfinite(metCorrPhi)) {
    printf("HistSelector::%s: Entry %12lld: MET correction undefined! corr = %.2f, phi = %.3f, dx = %.2f, dy = %.2f\n",
           __func__, fentry, metCorr, metCorrPhi, delta_x, delta_y);
    metCorr = 0.f;
    metCorrPhi = 0.f;
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply the Rochester corrections to the muon pt for all muons
void HistSelector::ApplyMuonCorrections() {
  //FIXME: Update to using the Rochester correction uncertainty instead of size
  const static bool use_size(true && fUseRoccoCorr != 0); //use Rochester size or evaluated uncertainty
  float delta_x(metCorr*std::cos(metCorrPhi)), delta_y(metCorr*std::sin(metCorrPhi));
  const static int s(0), m(0); //error set and member for corrections
  const static int sys_s(2), sys_m(0); //FIXME: Decide on a systematic correction set, currently using Zpt
  for(UInt_t index = 0; index < *nMuon; ++index) {
    if((fIsEmbed && !fUseEmbedRocco) || fUseRoccoCorr == 0) {  //don't correct embedding (or any, if selected) to muons
      Muon_RoccoSF[index] = 1.f;
      const float abs_eta = std::fabs(Muon_eta[index]);
      Muon_ESErr[index] = (fIsEmbed) ? (abs_eta < 1.2) ? 0.004 : (abs_eta < 2.1) ? 0.009 : 0.027 : 0.002;
      continue;
    }
    if(fUseRoccoCorr == 1) { //apply locally-evaluated corrections
      const double u = fRnd->Uniform();
      double sf(1.), err(0.);
      if(fIsData) {
        sf = fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
        err = 0.; //ignore error on data
      } else { //MC or Embedding if using Rochester corrections
        //For simulated muons, both in embedding and standard MC, apply given MC corrections
        if(Muon_genPartIdx[index] >= 0) { //matched to a gen particle
          sf   = fRoccoR->kSpreadMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], GenPart_pt[Muon_genPartIdx[index]], s, m);
          err  = fRoccoR->kSpreadMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], GenPart_pt[Muon_genPartIdx[index]], sys_s, sys_m);
          err *= fRoccoR->kScaleDT (Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
          err /= fRoccoR->kScaleDT (Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], sys_s, sys_m);
          err = std::fabs(sf - err);
        } else { //not matched
          sf   = fRoccoR->kSmearMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], Muon_nTrackerLayers[index], u, s, m);
          err  = fRoccoR->kSmearMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], Muon_nTrackerLayers[index], u, sys_s, sys_m);
          err *= fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
          err /= fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], sys_s, sys_m);
          err = std::fabs(sf - err);
        }
      }
      double pt_diff = Muon_pt[index];
      Muon_pt[index] *= sf;
      Muon_RoccoSF[index] = sf;
      pt_diff = Muon_pt[index] - pt_diff;
      //subtract the change from the MET
      delta_x -= pt_diff*std::cos(Muon_phi[index]);
      delta_y -= pt_diff*std::sin(Muon_phi[index]);
      if(use_size) Muon_ESErr[index] = std::fabs(1.f - sf); //size of the correction
      else         Muon_ESErr[index] = err; //evaluated uncertainty
    } else if(fUseRoccoCorr == 2) { //apply ntuple-level corrections
      Muon_RoccoSF[index] = Muon_corrected_pt[index] / Muon_pt[index];
      const double pt_diff = Muon_corrected_pt[index] - Muon_pt[index];
      Muon_pt[index] = Muon_corrected_pt[index];
      delta_x -= pt_diff*std::cos(Muon_phi[index]);
      delta_y -= pt_diff*std::sin(Muon_phi[index]);
      if(use_size) Muon_ESErr[index] = std::fabs(1.f - Muon_RoccoSF[index]); //size of the correction
      else         Muon_ESErr[index] = std::max(std::fabs(1.f - Muon_correctedUp_pt[index]/Muon_pt[index]),
                                                std::fabs(1.f - Muon_correctedDown_pt[index]/Muon_pt[index])); //take the larger fractional uncertainty of the two
    }
  }
  metCorr = std::sqrt(delta_x*delta_x + delta_y*delta_y);
  metCorrPhi = (metCorr > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, delta_x/metCorr)))*(delta_y < 0.f ? -1 : 1) : 0.f;
  if(!std::isfinite(metCorr) || !std::isfinite(metCorrPhi)) {
    printf("HistSelector::%s: Entry %12lld: MET correction undefined! corr = %.2f, phi = %.3f, dx = %.2f, dy = %.2f\n",
           __func__, fentry, metCorr, metCorrPhi, delta_x, delta_y);
    metCorr = 0.f;
    metCorrPhi = 0.f;
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply tau energy scale corrections
void HistSelector::ApplyTauCorrections() {
  tauES = 1.f; tauES_up = 1.f; tauES_down = 1.f;
  float delta_x(metCorr*std::cos(metCorrPhi)), delta_y(metCorr*std::sin(metCorrPhi));
  for(UInt_t index = 0; index < *nTau; ++index) {
    const float sf = (fIsData) ? 1. : fTauIDWeight->EnergyScale(Tau_pt[index], Tau_eta[index], Tau_decayMode[index],
                                                                Tau_genPartFlav[index], Tau_idDeepTau2017v2p1VSjet[index],
                                                                fYear, Tau_energyScaleUp[index], Tau_energyScaleDown[index]);
    double pt_diff = Tau_pt[index];
    Tau_pt  [index] *= sf;
    Tau_mass[index] *= sf;
    Tau_energyScale[index] = sf;
    pt_diff = Tau_pt[index] - pt_diff;
    //subtract the change from the MET
    delta_x -= pt_diff*std::cos(Tau_phi[index]);
    delta_y -= pt_diff*std::sin(Tau_phi[index]);
  }
  metCorr = std::sqrt(delta_x*delta_x + delta_y*delta_y);
  metCorrPhi = (metCorr > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, delta_x/metCorr)))*(delta_y < 0.f ? -1 : 1) : 0.f;
  if(!std::isfinite(metCorr) || !std::isfinite(metCorrPhi)) {
    printf("HistSelector::%s: Entry %12lld: MET correction undefined! corr = %.2f, phi = %.3f, dx = %.2f, dy = %.2f\n",
           __func__, fentry, metCorr, metCorrPhi, delta_x, delta_y);
    metCorr = 0.f;
    metCorrPhi = 0.f;
  }
}

//-----------------------------------------------------------------------------------------------------------------
//Apply a lepton energy scale change, propagating the effect to the MET
void HistSelector::EnergyScale(const float scale, Lepton_t& lep, float* MET, float* METPhi) {
  if(!lep.p4) return;
  EnergyScale(scale, *(lep.p4), MET, METPhi);
  lep.pt   = lep.p4->Pt();
  lep.eta  = lep.p4->Eta();
  lep.phi  = lep.p4->Phi();
  lep.mass = lep.p4->M();
}

//-----------------------------------------------------------------------------------------------------------------
//Apply an energy scale change, propagating the effect to the MET
void HistSelector::EnergyScale(const float scale, TLorentzVector& lv, float* MET, float* METPhi) {
  if(scale <= 0.f) {
    printf("!!! HistSelector::%s: Non-positive energy scale %f\n", __func__, scale);
    return;
  }
  const float pt_shift((scale-1.f)*lv.Pt()), phi(lv.Phi());
  lv *= scale;
  //ensure the vector has a non-imaginary mass
  if(lv.M2() < 0.) lv.SetE(lv.P());

  //adjust the MET following the shift in the lepton pT, if non-null
  if(MET && METPhi) {
    const float dx(pt_shift*std::cos(phi)), dy(pt_shift*std::sin(phi));
    const float x((*MET)*std::cos(*METPhi)+dx), y((*MET)*std::sin(*METPhi)+dy);
    *MET    = std::sqrt(x*x+y*y);
    *METPhi = (*MET > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, x/(*MET))))*(y < 0.f ? -1.f : 1.f) : 0.f;
  }
}

//--------------------------------------------------------------------------------------------------------------
// If the same flavor leptons are not in increasing pT order, swap them
// FIXME: Add a more generic method, e.g. a Muon/Electron object with fields, loop through fields to swap
void HistSelector::SwapSameFlavor() {
  if(*nMuon == 2 && Muon_pt[0] < Muon_pt[1]) {
    std::swap(Muon_pt                       [0], Muon_pt                       [1]);
    std::swap(Muon_eta                      [0], Muon_eta                      [1]);
    std::swap(Muon_phi                      [0], Muon_phi                      [1]);
    std::swap(Muon_charge                   [0], Muon_charge                   [1]);
    std::swap(Muon_looseId                  [0], Muon_looseId                  [1]);
    std::swap(Muon_mediumId                 [0], Muon_mediumId                 [1]);
    std::swap(Muon_tightId                  [0], Muon_tightId                  [1]);
    std::swap(Muon_pfRelIso04_all           [0], Muon_pfRelIso04_all           [1]);
    std::swap(Muon_dxy                      [0], Muon_dxy                      [1]);
    std::swap(Muon_dxyErr                   [0], Muon_dxyErr                   [1]);
    std::swap(Muon_dz                       [0], Muon_dz                       [1]);
    std::swap(Muon_dzErr                    [0], Muon_dzErr                    [1]);
    std::swap(Muon_nTrackerLayers           [0], Muon_nTrackerLayers           [1]);
    std::swap(Muon_TaggedAsRemovedByJet     [0], Muon_TaggedAsRemovedByJet     [1]);
    std::swap(Muon_genPartFlav              [0], Muon_genPartFlav              [1]);
    std::swap(Muon_genPartIdx               [0], Muon_genPartIdx               [1]);
    std::swap(Muon_RoccoSF                  [0], Muon_RoccoSF                  [1]);
  }
  if(*nElectron == 2 && Electron_pt[0] < Electron_pt[1]) {
    std::swap(Electron_pt                   [0], Electron_pt                   [1]);
    std::swap(Electron_eta                  [0], Electron_eta                  [1]);
    std::swap(Electron_phi                  [0], Electron_phi                  [1]);
    std::swap(Electron_charge               [0], Electron_charge               [1]);
    std::swap(Electron_deltaEtaSC           [0], Electron_deltaEtaSC           [1]);
    std::swap(Electron_mvaFall17V2Iso_WPL   [0], Electron_mvaFall17V2Iso_WPL   [1]);
    std::swap(Electron_mvaFall17V2Iso_WP90  [0], Electron_mvaFall17V2Iso_WP90  [1]);
    std::swap(Electron_mvaFall17V2Iso_WP80  [0], Electron_mvaFall17V2Iso_WP80  [1]);
    std::swap(Electron_mvaFall17V2noIso_WPL [0], Electron_mvaFall17V2noIso_WPL [1]);
    std::swap(Electron_mvaFall17V2noIso_WP90[0], Electron_mvaFall17V2noIso_WP90[1]);
    std::swap(Electron_mvaFall17V2noIso_WP80[0], Electron_mvaFall17V2noIso_WP80[1]);
    std::swap(Electron_pfRelIso03_all       [0], Electron_pfRelIso03_all       [1]);
    std::swap(Electron_eCorr                [0], Electron_eCorr                [1]);
    std::swap(Electron_dxy                  [0], Electron_dxy                  [1]);
    std::swap(Electron_dxyErr               [0], Electron_dxyErr               [1]);
    std::swap(Electron_dz                   [0], Electron_dz                   [1]);
    std::swap(Electron_dzErr                [0], Electron_dzErr                [1]);
    std::swap(Electron_TaggedAsRemovedByJet [0], Electron_TaggedAsRemovedByJet [1]);
    std::swap(Electron_genPartFlav          [0], Electron_genPartFlav          [1]);
    std::swap(Electron_genPartIdx           [0], Electron_genPartIdx           [1]);
    std::swap(Electron_energyErr            [0], Electron_energyErr            [1]);
    std::swap(Electron_energyScale          [0], Electron_energyScale          [1]);
    std::swap(Electron_energyScaleUp        [0], Electron_energyScaleUp        [1]);
    std::swap(Electron_energyScaleDown      [0], Electron_energyScaleDown      [1]);
  }
}

//--------------------------------------------------------------------------------------------------------------
//determine MVA category
int HistSelector::Category(TString selection) {
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
void HistSelector::InitializeSystematics() {
  leptonOne.wt1_group = 0; leptonOne.wt2_group = 0;
  leptonTwo.wt1_group = 0; leptonTwo.wt2_group = 0;
}

//--------------------------------------------------------------------------------------------------------------
// Count objects in the event (leptons, jets, etc.) and initialize base variables
void HistSelector::CountObjects() {

  /////////////////////////////////////
  // Check that no arrays are too small

  if(!fIsData && *nGenPart > kMaxGenPart) {
    printf("HistSelector::%s: Error! Too many gen particles (%i), max allowed are %i\n", __func__, *nGenPart, kMaxGenPart);
    throw 20;
  }
  if(*nElectron > kMaxLeptons) {
    printf("HistSelector::%s: Error! Too many electrons (%i), max allowed are %i\n", __func__, *nElectron, kMaxLeptons);
    throw 20;
  }
  if(*nMuon > kMaxLeptons) {
    printf("HistSelector::%s: Error! Too many muons (%i), max allowed are %i\n", __func__, *nMuon, kMaxLeptons);
    throw 20;
  }
  if(*nTau > kMaxLeptons) {
    printf("HistSelector::%s: Error! Too many taus (%i), max allowed are %i\n", __func__, *nTau, kMaxLeptons);
    throw 20;
  }
  if(*nJet > kMaxParticles) {
    printf("HistSelector::%s: Error! Too many jets (%i), max allowed are %i\n", __func__, *nJet, kMaxParticles);
    throw 20;
  }
  if(*nTrigObj > kMaxTriggers) {
    printf("HistSelector::%s: Error! Too many triggers (%i), max allowed are %i\n", __func__, *nTrigObj, kMaxTriggers);
    throw 20;
  }

  /////////////////////////////////////
  // Base selections
  mutau   =                   *nMuon == 1 && *nTau == 1 && (fSelection == "" || fSelection == "mutau");
  etau    = *nElectron == 1               && *nTau == 1 && (fSelection == "" || fSelection == "etau" );
  emu     = *nElectron == 1 && *nMuon == 1              && (fSelection == "" || fSelection == "emu"  || fSelection.Contains("tau_"));
  mumu    = *nElectron <  2 && *nMuon == 2              && (fSelection == "" || fSelection == "mumu" );
  ee      = *nElectron == 2 && *nMuon <  2              && (fSelection == "" || fSelection == "ee"   );
  etau_mu = emu;
  mutau_e = emu;

  //enforce exclusive channels
  if((mutau and etau) or emu) {
    mutau = false;
    etau  = false;
  }
  if((mumu and ee) or mutau or etau or emu) {
    mumu  = false;
    ee    = false;
  }

  ///////////////////////////////////////////////////////
  //Set the data era
  ///////////////////////////////////////////////////////

  mcEra   = 0;
  if(!fIsData && !fIsEmbed) { // use random numbers for MC not split by era
    if(fYear == 2016) {
      const double rand = fRnd->Uniform();
      const double frac_first = 19.72 / 35.86;
      mcEra = rand > frac_first; //0 if first, 1 if second
    } else if(fYear == 2018) {
      const double rand = fRnd->Uniform();
      const double frac_first = 1. -  31.93/59.59;
      mcEra = rand > frac_first; //0 if first, 1 if second
    }
  } else if(fIsEmbed || fIsData) { //decide by run period in the file name
    TString name = GetOutputName();
    if(fYear == 2016) {
      if(name.Contains("-G") || name.Contains("-H")) mcEra = 1;
    } else if(fYear == 2018) { //split 2018 into ABC and D
      if(name.Contains("-D")) mcEra = 1;
    }
  }

  ///////////////////////////////////////////////////////
  // Apply object scale corrections
  ///////////////////////////////////////////////////////

  //Add MET information
  met        = *PuppiMET_pt; //use PUPPI MET
  metPhi     = *PuppiMET_phi;
  metCorr    = 0.f; //record the changes to the MET due to changes in object energy/momentum scales
  metCorrPhi = 0.f;

  ApplyElectronCorrections();
  ApplyMuonCorrections();
  ApplyTauCorrections();

  //Re-sort muon/electron collections in the case where the corrections change the pT order
  SwapSameFlavor();

  //Apply the MET changes
  const float met_x(met*std::cos(metPhi) + metCorr*std::cos(metCorrPhi)), met_y(met*std::sin(metPhi) + metCorr*std::sin(metCorrPhi));
  met = std::sqrt(met_x*met_x + met_y*met_y);
  metPhi = (met > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, met_x/met)))*(met_y < 0.f ? -1 : 1) : 0.f;
  if(!std::isfinite(met) || !std::isfinite(metPhi)) {
    printf("!!! HistSelector::%s: Entry: %12lld: MET elements are undefined! met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f, met_x = %.2f, met_y = %.2f, pupp = %.2f, puppPhi = %.3f\n",
           __func__, fentry, met, metPhi, metCorr, metCorrPhi, met_x, met_y, *PuppiMET_pt, *PuppiMET_phi);
  }

  ///////////////////////////////////////////////////////
  // Initialize lepton selection info
  ///////////////////////////////////////////////////////

  nMuons = *nMuon; nElectrons = *nElectron; nTaus = *nTau; nPhotons = 0;

  //associated track variables not relevant for all leptons, so reset if not used
  leptonOne.trkpt = 0.f; leptonOne.trketa = 0.f; leptonOne.trkphi = 0.f;
  leptonTwo.trkpt = 0.f; leptonTwo.trketa = 0.f; leptonTwo.trkphi = 0.f;

  if(emu) {
    leptonOne.setPtEtaPhiM(Electron_pt[0], Electron_eta[0], Electron_phi[0], ELECMASS);
    leptonTwo.setPtEtaPhiM(Muon_pt    [0], Muon_eta    [0], Muon_phi    [0], MUONMASS);
    leptonOne.scEta  = Electron_eta[0] + Electron_deltaEtaSC[0];
    leptonTwo.scEta  = Muon_eta[0];
    leptonOne.dxy    = Electron_dxy[0];
    leptonTwo.dxy    = Muon_dxy    [0];
    leptonOne.dz     = Electron_dz [0];
    leptonTwo.dz     = Muon_dz     [0];
    leptonOne.dxySig = Electron_dxy[0] / Electron_dxyErr[0];
    leptonTwo.dxySig = Muon_dxy    [0] / Muon_dxyErr    [0];
    leptonOne.dzSig  = Electron_dz [0] / Electron_dzErr [0];
    leptonTwo.dzSig  = Muon_dz     [0] / Muon_dzErr     [0];
    leptonOne.iso    = Electron_pfRelIso03_all[0]*Electron_pt[0];
    leptonTwo.iso    = Muon_pfRelIso04_all[0]*Muon_pt[0];
    leptonOne.relIso = leptonOne.iso/leptonOne.pt;
    leptonTwo.relIso = leptonTwo.iso/leptonTwo.pt;
    leptonOne.flavor = -11*Electron_charge[0];
    leptonTwo.flavor = -13*Muon_charge[0];
    leptonOne.jetOverlap = Electron_TaggedAsRemovedByJet[0];
    leptonTwo.jetOverlap = Muon_TaggedAsRemovedByJet[0];
    leptonOne.genID     = Electron_genPartFlav[0];
    leptonTwo.genID     = Muon_genPartFlav[0];
    leptonOne.genFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwo.genFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonOne.id1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOne.id2       = 0;
    leptonTwo.id1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonTwo.id2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonOne.ES[0]     = Electron_energyScale[0];
    leptonOne.ES[1]     = Electron_energyScaleUp[0];
    leptonOne.ES[2]     = Electron_energyScaleDown[0];
    leptonTwo.ES[0]     = Muon_RoccoSF[0];
    leptonTwo.ES[1]     = Muon_RoccoSF[0]*(1.f + Muon_ESErr[0]);
    leptonTwo.ES[2]     = Muon_RoccoSF[0]*(1.f - Muon_ESErr[0]);
  } else if(etau) {
    leptonOne.setPtEtaPhiM(Electron_pt[0], Electron_eta[0], Electron_phi[0], ELECMASS);
    leptonTwo.setPtEtaPhiM(Tau_pt     [0], Tau_eta     [0], Tau_phi     [0], Tau_mass[0]);
    leptonOne.scEta  = Electron_eta[0] + Electron_deltaEtaSC[0];
    leptonTwo.scEta  = Tau_eta[0];
    leptonOne.dxy    = Electron_dxy[0];
    leptonTwo.dxy    = Tau_dxy     [0];
    leptonOne.dz     = Electron_dz [0];
    leptonTwo.dz     = Tau_dz      [0];
    leptonOne.dxySig = Electron_dxy[0] / Electron_dxyErr[0];
    leptonTwo.dxySig = 0.f;
    leptonOne.dzSig  = Electron_dz [0] / Electron_dzErr [0];
    leptonTwo.dzSig  = 0.f;
    leptonOne.iso    = Electron_pfRelIso03_all[0]*Electron_pt[0];
    leptonTwo.iso    = 0.f;
    leptonOne.relIso = leptonOne.iso/leptonOne.pt;
    leptonTwo.relIso = 0.f;
    leptonOne.flavor = -11*Electron_charge[0];
    leptonTwo.flavor = -15*Tau_charge[0];
    leptonOne.jetOverlap = Electron_TaggedAsRemovedByJet[0];
    leptonTwo.jetOverlap = Tau_TaggedAsRemovedByJet[0];
    leptonOne.genID     = Electron_genPartFlav[0];
    leptonTwo.genID     = Tau_genPartFlav[0];
    leptonOne.genFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwo.genFlavor = TauFlavorFromID(Tau_genPartFlav[0]);
    leptonOne.id1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOne.id2       = 0;
    leptonTwo.id1       = Tau_idAntiEle[0];
    leptonTwo.id2       = Tau_idAntiMu[0];
    leptonTwo.id3       = Tau_idMVAnewDM2017v2[0];
    leptonOne.ES[0]     = Electron_energyScale[0];
    leptonOne.ES[1]     = Electron_energyScaleUp[0];
    leptonOne.ES[2]     = Electron_energyScaleDown[0];
    leptonTwo.ES[0]     = Tau_energyScale[0];
    leptonTwo.ES[1]     = Tau_energyScaleUp[0];
    leptonTwo.ES[2]     = Tau_energyScaleDown[0];
    leptonTwo.trkpt     = Tau_leadTkPtOverTauPt[0]*leptonTwo.p4->Pt();
    leptonTwo.trketa    = Tau_leadTkDeltaEta[0] + leptonTwo.p4->Eta();
    leptonTwo.trkphi    = Tau_leadTkDeltaPhi[0] + leptonTwo.p4->Phi();
  } else if(mutau) {
    leptonOne.setPtEtaPhiM(Muon_pt    [0], Muon_eta    [0], Muon_phi    [0], MUONMASS);
    leptonTwo.setPtEtaPhiM(Tau_pt     [0], Tau_eta     [0], Tau_phi     [0], Tau_mass[0]);
    leptonOne.scEta  = Muon_eta[0];
    leptonTwo.scEta  = Tau_eta [1];
    leptonOne.dxy    = Muon_dxy    [0];
    leptonTwo.dxy    = Tau_dxy     [0];
    leptonOne.dz     = Muon_dz     [0];
    leptonTwo.dz     = Tau_dz      [0];
    leptonOne.dxySig = Muon_dxy    [0] / Muon_dxyErr    [0];
    leptonTwo.dxySig = 0.f;
    leptonOne.dzSig  = Muon_dz     [0] / Muon_dzErr     [0];
    leptonTwo.dzSig  = 0.f;
    leptonOne.iso    = Muon_pfRelIso04_all[0]*Muon_pt[0];
    leptonTwo.iso    = 0.f;
    leptonOne.relIso = leptonOne.iso/leptonOne.pt;
    leptonTwo.relIso = 0.f;
    leptonOne.flavor = -13*Muon_charge[0];
    leptonTwo.flavor = -15*Tau_charge[0];
    leptonOne.jetOverlap = Muon_TaggedAsRemovedByJet[0];
    leptonTwo.jetOverlap = Tau_TaggedAsRemovedByJet[0];
    leptonOne.genID     = Muon_genPartFlav[0];
    leptonTwo.genID     = Tau_genPartFlav[0];
    leptonOne.genFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonTwo.genFlavor = TauFlavorFromID(Tau_genPartFlav[0]);
    leptonOne.id1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOne.id2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwo.id1       = Tau_idAntiEle[0];
    leptonTwo.id2       = Tau_idAntiMu[0];
    leptonTwo.id3       = Tau_idMVAnewDM2017v2[0];
    leptonOne.ES[0]     = Muon_RoccoSF[0];
    leptonOne.ES[1]     = Muon_RoccoSF[0]*(1.f + Muon_ESErr[0]);
    leptonOne.ES[2]     = Muon_RoccoSF[0]*(1.f - Muon_ESErr[0]);
    leptonTwo.ES[0]     = Tau_energyScale[0];
    leptonTwo.ES[1]     = Tau_energyScaleUp[0];
    leptonTwo.ES[2]     = Tau_energyScaleDown[0];
    leptonTwo.trkpt     = Tau_leadTkPtOverTauPt[0]*leptonTwo.p4->Pt();
    leptonTwo.trketa    = Tau_leadTkDeltaEta[0] + leptonTwo.p4->Eta();
    leptonTwo.trkphi    = Tau_leadTkDeltaPhi[0] + leptonTwo.p4->Phi();
  } else if(mumu) {
    leptonOne.setPtEtaPhiM(Muon_pt    [0], Muon_eta    [0], Muon_phi    [0], MUONMASS);
    leptonTwo.setPtEtaPhiM(Muon_pt    [1], Muon_eta    [1], Muon_phi    [1], MUONMASS);
    leptonOne.scEta  = Muon_eta[0];
    leptonTwo.scEta  = Muon_eta[1];
    leptonOne.dxy    = Muon_dxy    [0];
    leptonTwo.dxy    = Muon_dxy    [1];
    leptonOne.dz     = Muon_dz     [0];
    leptonTwo.dz     = Muon_dz     [1];
    leptonOne.dxySig = Muon_dxy    [0] / Muon_dxyErr    [0];
    leptonTwo.dxySig = Muon_dxy    [1] / Muon_dxyErr    [1];
    leptonOne.dzSig  = Muon_dz     [0] / Muon_dzErr     [0];
    leptonTwo.dzSig  = Muon_dz     [1] / Muon_dzErr     [1];
    leptonOne.iso    = Muon_pfRelIso04_all[0]*Muon_pt[0];
    leptonTwo.iso    = Muon_pfRelIso04_all[1]*Muon_pt[1];
    leptonOne.relIso = leptonOne.iso/leptonOne.pt;
    leptonTwo.relIso = leptonTwo.iso/leptonTwo.pt;
    leptonOne.flavor = -13*Muon_charge[0];
    leptonTwo.flavor = -13*Muon_charge[1];
    leptonOne.jetOverlap = Muon_TaggedAsRemovedByJet[0];
    leptonTwo.jetOverlap = Muon_TaggedAsRemovedByJet[1];
    leptonOne.genID     = Muon_genPartFlav[0];
    leptonTwo.genID     = Muon_genPartFlav[1];
    leptonOne.genFlavor = MuonFlavorFromID(Muon_genPartFlav[0]);
    leptonTwo.genFlavor = MuonFlavorFromID(Muon_genPartFlav[1]);
    leptonOne.id1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOne.id2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwo.id1       = Muon_looseId[1] + Muon_mediumId[1] + Muon_tightId[1];
    leptonTwo.id2       = MuonRelIsoID(Muon_pfRelIso04_all[1]);
    leptonOne.ES[0]     = Muon_RoccoSF[0];
    leptonOne.ES[1]     = Muon_RoccoSF[0]*(1.f + Muon_ESErr[0]);
    leptonOne.ES[2]     = Muon_RoccoSF[0]*(1.f - Muon_ESErr[0]);
    leptonTwo.ES[0]     = Muon_RoccoSF[1];
    leptonTwo.ES[1]     = Muon_RoccoSF[1]*(1.f + Muon_ESErr[1]);
    leptonTwo.ES[2]     = Muon_RoccoSF[1]*(1.f - Muon_ESErr[1]);
  } else if(ee) {
    leptonOne.setPtEtaPhiM(Electron_pt[0], Electron_eta[0], Electron_phi[0], ELECMASS);
    leptonTwo.setPtEtaPhiM(Electron_pt[1], Electron_eta[1], Electron_phi[1], ELECMASS);
    leptonOne.scEta  = Electron_eta[0] + Electron_deltaEtaSC[0];
    leptonTwo.scEta  = Electron_eta[1] + Electron_deltaEtaSC[1];
    leptonOne.dxy    = Electron_dxy[0];
    leptonTwo.dxy    = Electron_dxy[1];
    leptonOne.dz     = Electron_dz [0];
    leptonTwo.dz     = Electron_dz [1];
    leptonOne.dxySig = Electron_dxy[0] / Electron_dxyErr[0];
    leptonTwo.dxySig = Electron_dxy[1] / Electron_dxyErr[1];
    leptonOne.dzSig  = Electron_dz [0] / Electron_dzErr [0];
    leptonTwo.dzSig  = Electron_dz [1] / Electron_dzErr [1];
    leptonOne.iso    = Electron_pfRelIso03_all[0]*Electron_pt[0];
    leptonTwo.iso    = Electron_pfRelIso03_all[1]*Electron_pt[1];
    leptonOne.relIso = leptonOne.iso/leptonOne.pt;
    leptonTwo.relIso = leptonTwo.iso/leptonTwo.pt;
    leptonOne.flavor = -11*Electron_charge[0];
    leptonTwo.flavor = -11*Electron_charge[1];
    leptonOne.jetOverlap = Electron_TaggedAsRemovedByJet[0];
    leptonTwo.jetOverlap = Electron_TaggedAsRemovedByJet[1];
    leptonOne.genID     = Electron_genPartFlav[0];
    leptonTwo.genID     = Electron_genPartFlav[1];
    leptonOne.genFlavor = ElectronFlavorFromID(Electron_genPartFlav[0]);
    leptonTwo.genFlavor = ElectronFlavorFromID(Electron_genPartFlav[1]);
    leptonOne.id1       = Electron_mvaFall17V2noIso_WPL[0] + Electron_mvaFall17V2noIso_WP90[0] + Electron_mvaFall17V2noIso_WP80[0];
    leptonOne.id2       = 0;
    leptonTwo.id1       = Electron_mvaFall17V2noIso_WPL[1] + Electron_mvaFall17V2noIso_WP90[1] + Electron_mvaFall17V2noIso_WP80[1];
    leptonTwo.id2       = 0;
    leptonOne.ES[0]     = Electron_energyScale[0];
    leptonOne.ES[1]     = Electron_energyScaleUp[0];
    leptonOne.ES[2]     = Electron_energyScaleDown[0];
    leptonTwo.ES[0]     = Electron_energyScale[1];
    leptonTwo.ES[1]     = Electron_energyScaleUp[1];
    leptonTwo.ES[2]     = Electron_energyScaleDown[1];
  } else {
    std::cout << "HistSelector::" << __func__ << ": Entry " << fentry
              << ": Warning! No event selection identified: N(e) = " << *nElectron
              << " N(mu) = " << *nMuon << " N(tau) = " << *nTau << std::endl;
      return;
  }
  if(*nTau > 0) { //store information of the leading pT tau
    tauDecayMode   = Tau_decayMode[0];
    tauDeepAntiEle = Tau_idDeepTau2017v2p1VSe  [0];
    tauDeepAntiMu  = Tau_idDeepTau2017v2p1VSmu [0];
    tauDeepAntiJet = Tau_idDeepTau2017v2p1VSjet[0];
    tauES          = Tau_energyScale[0];
    tauES_up       = Tau_energyScaleUp[0];
    tauES_down     = Tau_energyScaleDown[0];
    tauGenID       = Tau_genPartFlav[0];
    tauGenFlavor   = TauFlavorFromID(tauGenID);
  }


  if(!std::isfinite(*PuppiMET_ptJERUp) || !std::isfinite(*PuppiMET_phiJERUp)) {
    if(fVerbose) printf("HistSelector::%s: Entry %lld: MET JER Up not defined\n", __func__, fentry);
    *PuppiMET_ptJERUp  = *PuppiMET_pt;
    *PuppiMET_phiJERUp = *PuppiMET_phi;
  }
  if(!std::isfinite(*PuppiMET_ptJESUp) || !std::isfinite(*PuppiMET_phiJESUp)) {
    if(fVerbose) printf("HistSelector::%s: Entry %lld: MET JES Up not defined\n", __func__, fentry);
    *PuppiMET_ptJESUp  = *PuppiMET_pt;
    *PuppiMET_phiJESUp = *PuppiMET_phi;
  }

  //If no Z information is found, approximate it with the lepton info
  if(fIsData || zPt < 0.f || zMass < 0.f) {
    if(fIsData || zLepOnePt < 0.f || zLepTwoPt < 0.f) { //no generator-level leptons
      if((fIsEmbed || fIsDY)) {
        printf("HistSelector::%s: Entry %lld: Z information not properly stored! M(Z) = %.2f, pT(Z) = %.2f, pT(tau_1) = %.2f, pT(tau_2) = %.2f\n",
               __func__, fentry, zMass, zPt, zLepOnePt, zLepTwoPt);
      }
      zPt = ((*leptonOne.p4) + (*leptonTwo.p4)).Pt();
      zMass = ((*leptonOne.p4) + (*leptonTwo.p4)).M();
      zEta = ((*leptonOne.p4) + (*leptonTwo.p4)).Eta();
    } else { //generator-level leptons found
      TLorentzVector zlepone, zleptwo;
      zlepone.SetPtEtaPhiM(zLepOnePt, zLepOneEta, zLepOnePhi, zLepOneMass);
      zleptwo.SetPtEtaPhiM(zLepTwoPt, zLepTwoEta, zLepTwoPhi, zLepTwoMass);
      zPt   = (zlepone + zleptwo).Pt();
      zMass = (zlepone + zleptwo).M();
      zEta  = (zlepone + zleptwo).Eta();
    }
  }

  //store generator level Z->ll lepton ids
  nGenHardElectrons = 0; nGenHardMuons = 0; nGenHardTaus = 0;
  //lepton one
  if     (std::abs(zLepOneID) == 11) ++nGenHardElectrons;
  else if(std::abs(zLepOneID) == 13) ++nGenHardMuons;
  else if(std::abs(zLepOneID) == 15) ++nGenHardTaus;
  //lepton two
  if     (std::abs(zLepTwoID) == 11) ++nGenHardElectrons;
  else if(std::abs(zLepTwoID) == 13) ++nGenHardMuons;
  else if(std::abs(zLepTwoID) == 15) ++nGenHardTaus;

  /////////////////////////////////////////////
  // Check info about selected leptons

  isFakeElectron  = !fIsData && ((leptonOne.isElectron() && leptonOne.genFlavor == 26) ||
                                 (leptonTwo.isElectron() && leptonTwo.genFlavor == 26));
  isFakeMuon      = !fIsData && ((leptonOne.isMuon    () && leptonOne.genFlavor == 26) ||
                                 (leptonTwo.isMuon    () && leptonTwo.genFlavor == 26));

  if     (leptonOne.isElectron()) leptonOne.isLoose = leptonOne.relIso >= 0.15;
  else if(leptonOne.isMuon    ()) leptonOne.isLoose = leptonOne.relIso >= 0.15;
  else if(leptonOne.isTau     ()) leptonOne.isLoose = tauDeepAntiJet < 50; //FIXME: Add tau ID per tau
  if     (leptonTwo.isElectron()) leptonTwo.isLoose = leptonTwo.relIso >= 0.15;
  else if(leptonTwo.isMuon    ()) leptonTwo.isLoose = leptonTwo.relIso >= 0.15;
  else if(leptonTwo.isTau     ()) leptonTwo.isLoose = tauDeepAntiJet < 50; //FIXME: Add tau ID per tau

  isLooseElectron  = leptonOne.isElectron() && leptonOne.isLoose;
  isLooseElectron |= leptonTwo.isElectron() && leptonTwo.isLoose;
  isLooseMuon      = leptonOne.isMuon    () && leptonOne.isLoose;
  isLooseMuon     |= leptonTwo.isMuon    () && leptonTwo.isLoose;
  isLooseTau       = leptonTwo.isTau     () && leptonTwo.isLoose;
  looseQCDSelection = isLooseElectron || isLooseMuon || isLooseTau;
  chargeTest = leptonOne.flavor*leptonTwo.flavor < 0;

  ///////////////////////////////////////////////////////
  // Add Trigger information
  ///////////////////////////////////////////////////////

  //Trigger thresholds (mu, ele): 2016 = (24, 27); 2017 = (27, 32); 2018 = (24, 32)
  //Use 1 GeV/c above muon threshold and 2 GeV/c above electron threshold
  muon_trig_pt_ = 25.f; electron_trig_pt_ = 34.f;
  if(fYear == 2017) muon_trig_pt_ = 28.f;
  if(fYear == 2016) electron_trig_pt_ = 29.f;

  MatchTriggers();

  ///////////////////////////////////////////////////////
  // Add Jet information
  ///////////////////////////////////////////////////////

  CountJets();

  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl
                             << " eventWeight*genWeight = " << eventWeight*(*genWeight) << std::endl;

}

//--------------------------------------------------------------------------------------------------------------
// Count the jets in the event
void HistSelector::CountJets() {
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
  jetOne.setPtEtaPhiM(0.,0.,0.,0.); //reset to no jet found
  for(UInt_t ijet = 0; ijet < *nJet; ++ijet) {
    if(Jet_jetId[ijet] < min_jet_id) continue; //bad jet
    if(Jet_TaggedAsRemovedByMuon[ijet]) continue; //overlapping a muon
    if(Jet_TaggedAsRemovedByElectron[ijet]) continue; //overlapping an electron
    if(Jet_TaggedAsRemovedByTau[ijet]) continue; //overlapping a hadronic tau
    const float jetpt  = Jet_pt [ijet];
    const float jeteta = Jet_eta[ijet];
    if(jetpt < min_jet_pt) continue; //too low of jet pt

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
        if(jetpt > jetptmax) {
          jetOne.setP(jetLoop); jetptmax = jetpt;
          //jet systematic uncertainties
          jetOne.jer_pt_up   = Jet_pt_jerUp       [ijet];
          jetOne.jer_pt_down = Jet_pt_jerDown     [ijet];
          jetOne.jes_pt_up   = Jet_pt_jesTotalUp  [ijet];
          jetOne.jes_pt_down = Jet_pt_jesTotalDown[ijet];
        }

        int jetBTag = 0;
        //check if b-tagged, must have eta < 2.4
        if(std::fabs(jeteta) < 2.4) {
          if(Jet_btagDeepB[ijet] > fBTagWeight->BTagCut(BTagWeight::kLooseBTag, fYear)) {
            ++jetBTag; ++nBJets20L; if(jetpt > 25.) ++nBJetsL;
            if(Jet_btagDeepB[ijet] > fBTagWeight->BTagCut(BTagWeight::kMediumBTag, fYear)) {
              ++jetBTag; ++nBJets20M; if(jetpt > 25.) ++nBJetsM;
              if(Jet_btagDeepB[ijet] > fBTagWeight->BTagCut(BTagWeight::kTightBTag, fYear)) {
                ++jetBTag; ++nBJets20; if(jetpt > 25.) ++nBJets;
              }
            }
          }
        }
        //save info for jet b-tag efficiency
        jetsPt    [nJets20-1] = jetpt;
        jetsEta   [nJets20-1] = jeteta;
        jetsBTag  [nJets20-1] = jetBTag;
        jetsFlavor[nJets20-1] = (fIsData || fIsEmbed) ? 0 : Jet_partonFlavour[ijet];
      }
    }
  }

  //Record HT result
  ht    = htLV.Pt();
  htPhi = htLV.Phi();
}

//-----------------------------------------------------------------------------------------------------------------
//Check if lepton matches to a trigger
int HistSelector::GetTriggerMatch(TLorentzVector* lv, bool isMuon, Int_t& trigIndex) {
  float pt(lv->Pt()), eta(lv->Eta()), phi(lv->Phi()), min_pt_1, min_pt_2;
  int bit_1, bit_2, id; //trigger bits to use and pdgID
  bool flag_1, flag_2;
  if(isMuon) {
    bit_1 = 1; //IsoMu24/IsoMu27
    min_pt_1 = (fYear == 2017) ? 27. : 24.; //muon_trig_pt_;
    bit_2 = bit_1; //10; //Mu50
    min_pt_2 = min_pt_1; //50.;
    id = 13;
    flag_1 = (fYear == 2017) ? *HLT_IsoMu27 : *HLT_IsoMu24;
    flag_2 = flag_1; //HLT_Mu50;
  } else { //electron
    id = 11;
    if(fYear == 2017) {
      bit_1 = 10; //different trigger in 2017 to correct for Ele32 issues
      bit_2 = 10;
    } else {
      bit_1 = 1;
      bit_2 = 1;
    }
    min_pt_1 = (fYear == 2016) ? 27. : 32.; //electron_trig_pt_;
    min_pt_2 = min_pt_1;
    flag_1 = ((fYear == 2016 && *HLT_Ele27_WPTight_Gsf) ||
              (fYear == 2017 && *HLT_Ele32_WPTight_Gsf_L1DoubleEG) ||
              (fYear == 2018 && *HLT_Ele32_WPTight_Gsf));
    flag_2 = flag_1;
  }


  if(fVerbose > 2) std::cout << "Doing trigger match with isMuon = " << isMuon
                             << " and bits " << (1<<bit_1) << " and " << (1<<bit_2) << std::endl
                             << "Given lepton pt, eta, phi = " << pt << ", " << eta << ", " << phi << std::endl;
  if(pt < min_pt_1 && pt < min_pt_2) {
    if(fVerbose > 2) std::cout << " The lepton pT is below the trigger threshold(s)\n";
    return 0; //pT below threshold considered
  }

  const float deltar_match = 0.1;
  const float deltapt_match = 10; //fractional match, > ~10 means don't use
  bool passedBit1 = false;
  bool passedBit2 = false;
  trigIndex = -1;
  for(unsigned trig_i = 0; trig_i < *nTrigObj; ++trig_i) {
    if(std::abs(TrigObj_id[trig_i]) != id) continue;

    //check if passes correct bit and the considered pT threshold for that trigger
    bool passBit1 = ((TrigObj_filterBits[trig_i] & (1<<bit_1)) != 0) && (pt > min_pt_1 && TrigObj_pt[trig_i] > min_pt_1);
    bool passBit2 = ((TrigObj_filterBits[trig_i] & (1<<bit_2)) != 0) && (pt > min_pt_2 && TrigObj_pt[trig_i] > min_pt_2);
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " has bits " << TrigObj_filterBits[trig_i] << std::endl;
    if(!passBit1 && !passBit2) continue;
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " passed bits check, has pt, eta, phi "
                               << TrigObj_pt[trig_i] <<  ", "
                               << TrigObj_eta[trig_i] <<  ", "
                               << TrigObj_phi[trig_i] << std::endl;

    passBit1 &= flag_1;
    passBit2 &= flag_2;
    if(!passBit1 && !passBit2) continue;
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " passed global flag test\n";

    if(std::fabs(pt - TrigObj_pt[trig_i]) > pt*deltapt_match) continue;
    if(fVerbose > 2) std::cout << "  TrigObj " << trig_i << " passed pt check\n";

    const float deltaeta = std::fabs(eta - TrigObj_eta[trig_i]);
    const float deltaphi = std::fabs(Utilities::DeltaPhi(phi, TrigObj_phi[trig_i]));
    if(sqrt(deltaeta*deltaeta + deltaphi*deltaphi) > deltar_match) continue;
    if(fVerbose > 2) std::cout << "   TrigObj " << trig_i << " passed delta r xcheck\n";

    trigIndex = trig_i;
    if(fVerbose > 2) std::cout << "--- Found a matching trigger object! Continuing search for additional matches...\n";
    passedBit1 |= passBit1;
    passedBit2 |= passBit2;
    if(passedBit1 || passedBit2) break;
  }

  return (passedBit1+2*passedBit2);//no matching trigger object found = 0
}

//--------------------------------------------------------------------------------------------------------------
// Match trigger objects to selected leptons
void HistSelector::MatchTriggers() {
  leptonOne.fired = false; leptonTwo.fired = false;

  //FIXME: Do actual trigger matching
  const bool mu_trig  = ((fYear == 2016 && *HLT_IsoMu24) ||
                        ( fYear == 2017 && *HLT_IsoMu27) ||
                        ( fYear == 2018 && *HLT_IsoMu24));
  const bool ele_trig = ((fYear == 2016 && *HLT_Ele27_WPTight_Gsf) ||
                        ( fYear == 2017 && *HLT_Ele32_WPTight_Gsf_L1DoubleEG) ||
                        ( fYear == 2018 && *HLT_Ele32_WPTight_Gsf));

  //FIXME: Perhaps remove reco trig pT cut from matching to better eval energy scale uncertainties
  if(leptonOne.isTau     ()) {
    leptonOne.matched = false; leptonOne.fired = false;
  } else if(leptonOne.isMuon    ()) {
    leptonOne.matched = mu_trig  && (!fDoTriggerMatching || GetTriggerMatch(leptonOne.p4, true , leptonOne.trigger));
    leptonOne.fired = leptonOne.matched && leptonOne.p4->Pt() > muon_trig_pt_;
  } else if(leptonOne.isElectron()) {
    leptonOne.matched = ele_trig && (!fDoTriggerMatching || GetTriggerMatch(leptonOne.p4, false, leptonOne.trigger));
    leptonOne.fired = leptonOne.matched && leptonOne.p4->Pt() > electron_trig_pt_;
  }

  if(leptonTwo.isTau     ()) {
    leptonTwo.matched = false; leptonTwo.fired = false;
  } else if(leptonTwo.isMuon    ()) {
    leptonTwo.matched = mu_trig  && (!fDoTriggerMatching || GetTriggerMatch(leptonTwo.p4, true , leptonTwo.trigger));
    leptonTwo.fired   = leptonTwo.matched && leptonTwo.p4->Pt() > muon_trig_pt_;
  } else if(leptonTwo.isElectron()) {
    leptonTwo.matched = ele_trig && (!fDoTriggerMatching || GetTriggerMatch(leptonTwo.p4, false, leptonTwo.trigger));
    leptonTwo.fired   = leptonTwo.matched && leptonTwo.p4->Pt() > electron_trig_pt_;
  }

  triggerLeptonStatus = 0;
  if((leptonOne.isElectron() && leptonOne.fired) || //electron triggered
     (leptonTwo.isElectron() && leptonTwo.fired)) {
    triggerLeptonStatus += 1;
  }
  if((leptonOne.isMuon    () && leptonOne.fired) || //muon triggered
     (leptonTwo.isMuon    () && leptonTwo.fired)) {
    triggerLeptonStatus += 2;
  }
  //matches are within 0.1 delta R, so both to be matched must be within 0.2 and therefore will be rejected
  if(fVerbose > 0 && leptonOne.trigger == leptonTwo.trigger) {
    printf("HistSelector::%s: Entry %lld: Warning! Both leptons were matched to the same trigger index!\n", __func__, fentry);
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::ApplyTriggerWeights() {
  if(fIsData) return; //not defined for data
  float data_eff[2]  = {0.5f, 0.5f}; //set to 0.5 so no danger in doing the ratio of eff or 1 - eff
  float mc_eff[2]    = {0.5f, 0.5f};
  float data_up[2]   = {0.5f, 0.5f};
  float mc_up[2]     = {0.5f, 0.5f};
  float data_down[2] = {0.5f, 0.5f};
  float mc_down[2]   = {0.5f, 0.5f};

  ////////////////////////////////
  // Get the efficiencies
  ////////////////////////////////

  // Electrons
  if(leptonOne.isElectron() && leptonOne.p4->Pt() > electron_trig_pt_) { //lepton 1 is an electron
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.ElectronTriggerWeight(leptonOne.p4->Pt(), leptonOne.scEta, fYear, data_eff[0], mc_eff[0],
                                                  data_up[0], mc_up[0], data_down[0], mc_down[0],
                                                  leptonOne.isLoose, mcEra);
      } else {
        fEmbeddingWeight->ElectronTriggerWeight   (leptonOne.p4->Pt(), leptonOne.scEta, fYear, data_eff[0], mc_eff[0]);
      }
    } else {
      fElectronIDWeight.TriggerEff               (leptonOne.p4->Pt(), leptonOne.scEta, fYear,
                                                  ElectronIDWeight::kWP80,
                                                  // (leptonOne.id1 > 1) ? ElectronIDWeight::kWP80 : ElectronIDWeight::kWPLNotWP80,
                                                  data_eff[0], mc_eff[0], data_up[0], mc_up[0], data_down[0], mc_down[0]);
    }
  }
  if(leptonTwo.isElectron() && leptonTwo.p4->Pt() > electron_trig_pt_) { //lepton 2 is an electron
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.ElectronTriggerWeight(leptonTwo.p4->Pt(), leptonTwo.scEta, fYear, data_eff[1], mc_eff[1],
                                                  data_up[1], mc_up[1], data_down[1], mc_down[1],
                                                  leptonTwo.isLoose, mcEra);
      } else {
        fEmbeddingWeight->ElectronTriggerWeight   (leptonTwo.p4->Pt(), leptonTwo.scEta, fYear, data_eff[1], mc_eff[1]);
      }
    } else {
      fElectronIDWeight.TriggerEff               (leptonTwo.p4->Pt(), leptonTwo.scEta, fYear,
                                                  ElectronIDWeight::kWP80,
                                                  // (leptonOne.id1 > 1) ? ElectronIDWeight::kWP80 : ElectronIDWeight::kWPLNotWP80,
                                                  data_eff[1], mc_eff[1], data_up[1], mc_up[1], data_down[1], mc_down[1]);
    }
  }

  // Muons
  if(leptonOne.isMuon    () && leptonOne.p4->Pt() > muon_trig_pt_) { //lepton 1 is a muon
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.MuonTriggerWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear, data_eff[0], mc_eff[0],
                                              data_up[0], mc_up[0], data_down[0], mc_down[0],
                                              leptonOne.isLoose, mcEra);
      } else {
        fEmbeddingWeight->MuonTriggerWeight   (leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear, data_eff[0], mc_eff[0]);
      }
    } else {
      fMuonIDWeight.TriggerEff               (leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear, mcEra, !leptonOne.fired || muonTriggerStatus != 2,
                                              data_eff[0], mc_eff[0], data_up[0], mc_up[0], data_down[0], mc_down[0]);
    }
  }
  if(leptonTwo.isMuon    () && leptonTwo.p4->Pt() > muon_trig_pt_) { //lepton 2 is a muon
    if(fIsEmbed) {
      if(fUseEmbedTnPWeights) {
        fEmbeddingTnPWeight.MuonTriggerWeight(leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear, data_eff[1], mc_eff[1],
                                              data_up[1], mc_up[1], data_down[1], mc_down[1],
                                              leptonTwo.isLoose, mcEra);
      } else {
        fEmbeddingWeight->MuonTriggerWeight   (leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear, data_eff[1], mc_eff[1]);
      }
    } else {
      //use low trigger in case the lepton didn't fire a trigger
      fMuonIDWeight.TriggerEff               (leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear, mcEra, !leptonTwo.fired || muonTriggerStatus != 2,
                                              data_eff[1], mc_eff[1], data_up[1], mc_up[1], data_down[1], mc_down[1]);
    }
  }

  // Taus
  if(leptonTwo.isTau     ()) {
    data_eff[1] = 0.f; mc_eff[1] = 0.f; //tau can't trigger
    data_up[1] = 0.f; mc_up[1] = 0.f;
    data_down[1] = 0.f; mc_down[1] = 0.f;

  }

  ////////////////////////////////
  // Apply the corrections
  ////////////////////////////////

  const float pt_1(leptonOne.p4->Pt()), pt_2(leptonTwo.p4->Pt());
  const float min_pt_1 = (leptonOne.isMuon    ()) ? muon_trig_pt_ : electron_trig_pt_;
  const float min_pt_2 = (leptonTwo.isMuon    ()) ? muon_trig_pt_ : electron_trig_pt_;
  //use just the muon if there's a muon or the leading firing lepton otherwise
  if(fRemoveTriggerWeights == 2) {
    if((leptonOne.fired && pt_1 >= min_pt_1) && (leptonOne.isMuon    () || !leptonTwo.fired || std::abs(leptonTwo.flavor) != 13)) {
      leptonOne.trig_wt = data_eff[0] / mc_eff[0];
    } else {
      leptonTwo.trig_wt = data_eff[1] / mc_eff[1];
    }
  }

  //use the probability of the event, given whether or not each lepton triggered
  else if(fRemoveTriggerWeights == 3) {
    //P(event) = P(lep 1 status) * P(lep 2 status)
    //SF = P(event | Data) / P(event | MC) = (P(1 | Data) * P(2 | Data)) / (P(1 | MC) * P(2 | MC))
    //SF = (P(1 | Data) / P(1 | MC)) * (P(2 | Data) / P(2 | MC))

    //if a lepton couldn't fire, ensure no weight effect
    if(pt_1 < min_pt_1) {
      leptonOne.trig_wt = 1.;
    } else {
      leptonOne.trig_wt = (leptonOne.fired) ? data_eff[0] / mc_eff[0] : (1.-data_eff[0])/(1.-mc_eff[0]);
    }
    if(leptonTwo.isTau     () || pt_2 < min_pt_2) {
      leptonTwo.trig_wt = 1.;
    } else {
      leptonTwo.trig_wt = (leptonTwo.fired) ? data_eff[1] / mc_eff[1] : (1.-data_eff[1])/(1.-mc_eff[1]);
    }
  }

  //use the probability that at least one fires
  else if(fRemoveTriggerWeights == 4) {
    // P(at least 1 fires) = 1 - P(neither fires) = 1 - Product of P(doesn't fire)
    float prob_data(1.f), prob_mc(1.f);
    float prob_data_sys[6] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f}; //l_1 up, l_1 down, l_2 up, l_2 down, l_1 and l_2 up, l_1 and l_2 down
    float prob_mc_sys  [6] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    //check if each is triggerable, and if so apply the P(!fire) for MC and Data
    if(pt_1 > min_pt_1) {
      prob_data *= (1.f - data_eff[0]);
      prob_mc   *= (1.f - mc_eff[0]  );

      prob_data_sys[0] *= (1.f - data_up  [0]);
      prob_data_sys[1] *= (1.f - data_down[0]);
      prob_data_sys[2] *= (1.f - data_eff [0]);
      prob_data_sys[3] *= (1.f - data_eff [0]);
      prob_data_sys[4] *= (1.f - data_up  [0]);
      prob_data_sys[5] *= (1.f - data_down[0]);

      prob_mc_sys  [0] *= (1.f - mc_down  [0]); // flip up/down for MC since ~ 1 / p(MC)
      prob_mc_sys  [1] *= (1.f - mc_up    [0]);
      prob_mc_sys  [2] *= (1.f - mc_eff   [0]);
      prob_mc_sys  [3] *= (1.f - mc_eff   [0]);
      prob_mc_sys  [4] *= (1.f - mc_down  [0]);
      prob_mc_sys  [5] *= (1.f - mc_up    [0]);
    }
    if(pt_2 > min_pt_2 && !leptonTwo.isTau()) {
      prob_data *= (1.f - data_eff[1]);
      prob_mc   *= (1.f - mc_eff[1]  );

      prob_data_sys[0] *= (1.f - data_eff [1]);
      prob_data_sys[1] *= (1.f - data_eff [1]);
      prob_data_sys[2] *= (1.f - data_up  [1]);
      prob_data_sys[3] *= (1.f - data_down[1]);
      prob_data_sys[4] *= (1.f - data_up  [1]);
      prob_data_sys[5] *= (1.f - data_down[1]);

      prob_mc_sys  [0] *= (1.f - mc_eff   [1]);
      prob_mc_sys  [1] *= (1.f - mc_eff   [1]);
      prob_mc_sys  [2] *= (1.f - mc_down  [1]); // flip up/down for MC since ~ 1 / p(MC)
      prob_mc_sys  [3] *= (1.f - mc_up    [1]);
      prob_mc_sys  [4] *= (1.f - mc_down  [1]);
      prob_mc_sys  [5] *= (1.f - mc_up    [1]);
    }
    if(prob_data >= 1.f || prob_mc >= 1.f) {
      if(fVerbose > 0) { //often due to changed trigger thresholds, so only warn if debugging
        std::cout << "!!! " << fentry << " Warning! Inefficiency calculation >= 1!"
                  << " 1-eff(data) = " << prob_data
                  << " 1-eff(MC) = " << prob_mc
                  << std::endl;
      }
    }
    const float max_prob = 0.9999f;
    prob_data = std::max(0.f, std::min(max_prob, prob_data)); //avoid a ~1/0  or sqrt(negative) situation
    prob_mc   = std::max(0.f, std::min(max_prob, prob_mc  ));
    const float trig_wt = (1.f - prob_data) / (1.f - prob_mc);
    if(leptonTwo.isTau     () || pt_2 < min_pt_2) { //no need to split if other lepton can't fire
      leptonOne.trig_wt = trig_wt;
    } else if(pt_1 < min_pt_1) { //only lepton two could (and did) fire
      leptonTwo.trig_wt = trig_wt;
    } else { //both could fire, so equally split the weight between the two
      leptonOne.trig_wt = std::sqrt(trig_wt);
      leptonTwo.trig_wt = std::sqrt(trig_wt);
    }
    triggerWeightsSys[0] = (1.f - std::max(0.f, std::min(max_prob, prob_data_sys[0]))) / (1.f - std::max(0.f, std::min(max_prob, prob_mc_sys[0])));
    triggerWeightsSys[1] = (1.f - std::max(0.f, std::min(max_prob, prob_data_sys[1]))) / (1.f - std::max(0.f, std::min(max_prob, prob_mc_sys[1])));
    triggerWeightsSys[2] = (1.f - std::max(0.f, std::min(max_prob, prob_data_sys[2]))) / (1.f - std::max(0.f, std::min(max_prob, prob_mc_sys[2])));
    triggerWeightsSys[3] = (1.f - std::max(0.f, std::min(max_prob, prob_data_sys[3]))) / (1.f - std::max(0.f, std::min(max_prob, prob_mc_sys[3])));
    triggerWeightsSys[4] = (1.f - std::max(0.f, std::min(max_prob, prob_data_sys[4]))) / (1.f - std::max(0.f, std::min(max_prob, prob_mc_sys[4])));
    triggerWeightsSys[5] = (1.f - std::max(0.f, std::min(max_prob, prob_data_sys[5]))) / (1.f - std::max(0.f, std::min(max_prob, prob_mc_sys[5])));
  }

  if(leptonOne.trig_wt < 0. || leptonTwo.trig_wt < 0. || !std::isfinite(leptonOne.trig_wt) || !std::isfinite(leptonTwo.trig_wt)) {
    std::cout << "!!! " << fentry << " Warning! Trigger efficiency calculation failed: wt_1 = "
              << leptonOne.trig_wt << " wt_2 = " << leptonTwo.trig_wt
              << " P(l1) = (" << data_eff[0] << ", " << mc_eff[0] << ")"
              << " P(l2) = (" << data_eff[1] << ", " << mc_eff[1] << ")"
              << " fired(l1) = " << leptonOne.fired
              << " fired(l2) = " << leptonTwo.fired
              << std::endl;
    leptonOne.trig_wt = 1.f; leptonTwo.trig_wt = 1.f;
  }
  //keep this the same for now, to see the comparison plot using this trigger weight array
  // triggerWeights[0] = leptonOne.trig_wt * leptonTwo.trig_wt;
  eventWeight *= leptonOne.trig_wt * leptonTwo.trig_wt;
}

//--------------------------------------------------------------------------------------------------------------
// Evaluate the j-->tau weights
void HistSelector::EvalJetToTauWeights(float& wt, float& wtcorr, float& wtbias) {
  wt = 0.f; wtcorr = 0.f; wtbias = 0.f;
  //j-->tau weight, by process
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    wt     += fJetToTauComps[proc] * fJetToTauWts[proc];
    wtcorr += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
    wtbias += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
  }
}

//--------------------------------------------------------------------------------------------------------------
// Determine and apply the global event weights (i.e. not selection specific)
void HistSelector::InitializeEventWeights() {
  //reset to-be-defined weights
  eventWeight = 1.f;

  leptonOne.wt1[0]  = 1.f; leptonTwo.wt1[0]  = 1.f;
  leptonOne.wt1[1]  = 1.f; leptonTwo.wt1[1]  = 1.f;
  leptonOne.wt1[2]  = 1.f; leptonTwo.wt1[2]  = 1.f;
  leptonOne.wt1[3]  = 1.f; leptonTwo.wt1[3]  = 1.f;
  leptonOne.wt1_bin = 0  ; leptonTwo.wt1_bin = 0  ;

  leptonOne.wt2[0]  = 1.f; leptonTwo.wt2[0]  = 1.f;
  leptonOne.wt2[1]  = 1.f; leptonTwo.wt2[1]  = 1.f;
  leptonOne.wt2[2]  = 1.f; leptonTwo.wt2[2]  = 1.f;
  leptonOne.wt2[3]  = 1.f; leptonTwo.wt2[3]  = 1.f;
  leptonOne.wt2_bin = 0  ; leptonTwo.wt2_bin = 0  ;

  leptonOne.trig_wt = 1.f; leptonTwo.trig_wt = 1.f;
  const int ntrig = sizeof(triggerWeights)/sizeof(*triggerWeights);
  for(int itrig = 0; itrig < ntrig; ++itrig) triggerWeights[itrig] = 1.f;
  const int ntrig_sys = sizeof(triggerWeightsSys)/sizeof(*triggerWeightsSys);
  for(int itrig = 0; itrig < ntrig_sys; ++itrig) triggerWeightsSys[itrig] = 1.f;

  embeddingWeight = 1.f; embeddingUnfoldingWeight = 1.f;
  zPtWeight = 1.f; zPtWeightUp = 1.f; zPtWeightDown = 1.f; zPtWeightSys = 1.f;
  signalZWeight = 1.f;

  jetToTauWeight     = 1.f; jetToTauWeightUp     = 1.f; jetToTauWeightDown     = 1.f;
  jetToTauWeightCorr = 1.f; jetToTauWeightCorrUp = 1.f; jetToTauWeightCorrDown = 1.f;
  jetToTauWeightBias = 1.f; jetToTauWeightBiasUp = 1.f; jetToTauWeightBiasDown = 1.f;
  jetToTauWeight_compUp = 1.f; jetToTauWeight_compDown = 1.f;

  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    fJetToTauWts      [proc] = 1.f;
    fJetToTauWtsUp    [proc] = 1.f;
    fJetToTauWtsDown  [proc] = 1.f;
    fJetToTauCorrs    [proc] = 1.f;
    fJetToTauBiases   [proc] = 1.f;
    fJetToTauComps    [proc] = 1.f;
    fJetToTauCompsUp  [proc] = 1.f;
    fJetToTauCompsDown[proc] = 1.f;
    fJetToTauMCWts    [proc] = 1.f;
    fJetToTauMCCorrs  [proc] = 1.f;
  }

  qcdWeight = 1.f; qcdWeightUp = 1.f; qcdWeightDown = 1.f; qcdWeightSys = 1.f; qcdClosure = 1.f; qcdIsoScale = 1.f;

  ////////////////////////////////////////////////////////////////////
  //   Generator/Embedding Weight
  ////////////////////////////////////////////////////////////////////

  embeddingWeight = 1.f; embeddingUnfoldingWeight = 1.f;
  if(fIsEmbed) {
    *genWeight = std::fabs(*genWeight);
    if(fIsLLEmbed) {
      embeddingWeight = 1.f;
      *genWeight = 1.f;
    } else if(*genWeight > 1.f) { //undefined generation weight (must be < 1) --> remove the event
      *genWeight = 0.f;
    } else if(*genWeight == 1.f) {
      std::cout << "!!! HistSelector::" << __func__ << ": Entry = " << fentry << ": Unit input embedding weight = " << *genWeight
                << " in event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                << std::endl;
      *genWeight = 0.f;
    } else { //move the weight value to the embeddingWeight variable
      embeddingWeight = *genWeight; //move the embedding weight from genWeight to embeddingWeight
      *genWeight = 1.f;
    }
    embeddingUnfoldingWeight = fEmbeddingWeight->UnfoldingWeight(zLepOnePt, zLepOneEta, zLepTwoPt, zLepTwoEta, zEta, zPt, fYear);
    eventWeight *= embeddingUnfoldingWeight*embeddingWeight;
  } else if(!fIsData) { //standard MC --> store only the sign of the generator weight
    *genWeight = (*genWeight < 0.) ? -1.f : 1.f;
  } else { //Data --> no weight
    *genWeight = 1.f;
  }

  ////////////////////////////////////////////////////////////////////
  //   Signal Z info + re-weight
  ////////////////////////////////////////////////////////////////////

  if(fIsSignal && fDataset.Contains("Z")) {
    //re-weight to MC spectrum
    signalZWeight = (fUseSignalZWeights) ? fSignalZWeight.GetWeight(fYear, zPt, zMass) : 1.f;
    //re-weight MC spectrum to data spectrum
    zPtWeight = fZPtWeight->GetWeight(fYear, zPt, zMass, false /*Use Gen level weights*/, zPtWeightUp, zPtWeightDown, zPtWeightSys);
    eventWeight *= zPtWeight*signalZWeight;
    if(fVerbose > 0) std::cout << " For Z pT = " << zPt << " and Mass = " << zMass << " using Data/MC weight " << zPtWeight
                               << " and signal Z weight " << signalZWeight
                               << "--> event weight = " << eventWeight << std::endl;
  }

  ////////////////////////////////////////////////////////////////////
  //   Z info + re-weight
  ////////////////////////////////////////////////////////////////////

  //Z pT/mass info (DY and Z signals only)
  if(fIsDY && !fIsEmbed) {
    //re-weight the Z pt vs mass spectrum
    zPtWeight = fZPtWeight->GetWeight(fYear, zPt, zMass, false /*Use Gen level weights*/, zPtWeightUp, zPtWeightDown, zPtWeightSys);
    eventWeight *= zPtWeight;
    if(fVerbose > 0) std::cout << " For Z pT = " << zPt << " and Mass = " << zMass << " using Data/MC weight " << zPtWeight
                               << "--> event weight = " << eventWeight << std::endl;
  }

  ////////////////////////////////////////////////////////////////////
  //   Trigger weights
  ////////////////////////////////////////////////////////////////////

  if(!fIsData) ApplyTriggerWeights();

  ////////////////////////////////////////////////////////////////////
  //   Pileup weights
  ////////////////////////////////////////////////////////////////////

  //pileup weights
  if(fIsEmbed || fIsData) { //pile-up is from data
    *puWeight  = 1.f;
    zPtWeight  = 1.f;
    btagWeight = 1.f;
  } else {
    if(fRemovePUWeights > 0) {
      *puWeight = 1.f;
      //replace weight
      if(fRemovePUWeights > 1) {
        *puWeight = fPUWeight->GetWeight(nPU, fYear);
        //FIXME: Decide on up/down for PU
        *puWeightUp = std::max(0.f, 2.f*(*puWeight) - 1.f);
        *puWeightDown = 1.f;
      }
    }
    eventWeight *= *puWeight;
  }

  ////////////////////////////////////////////////////////////////////
  //   Pre-fire weights
  ////////////////////////////////////////////////////////////////////

  //weights are defined in the ntupling stage, remove if flag is > 1
  if(fUsePrefireWeights > 1 && !fIsData && !fIsEmbed) {
    prefireWeight = 1.f;
    prefireWeight = fPrefireWeight->GetJetWeight(fYear, nJets20, jetsPt, jetsEta);
  } else if(fUsePrefireWeights && fIsEmbed) {
    //for embedding consider the probability from the jets already included in the data
    //instead only consider the effect from the reconstructed electrons
    float pt[2], eta[2];
    int nelec = 0;
    if(leptonOne.isElectron()) {
      pt [nelec] = leptonOne.p4->Pt();
      eta[nelec] = leptonOne.p4->Eta();
      ++nelec;
    }
    if(leptonTwo.isElectron()) {
      pt [nelec] = leptonTwo.p4->Pt();
      eta[nelec] = leptonTwo.p4->Eta();
      ++nelec;
    }
    prefireWeight = (nelec > 0) ? fPrefireWeight->GetPhotonWeight(fYear, nelec, pt, eta) : 1.f;
    //take the systematic as the size of the weight
    prefireWeight_up   = std::max(0.f, 2.f*prefireWeight - 1.f);
    prefireWeight_down = 1.f;
  } else if(fUsePrefireWeights == 0 || fIsData) { //remove the weights
    prefireWeight = 1.f; prefireWeight_up = 1.f; prefireWeight_down = 1.f;
  }
  eventWeight *= prefireWeight;

  ////////////////////////////////////////////////////////////////////
  //   Jet PU ID weights
  ////////////////////////////////////////////////////////////////////

  if(fUseJetPUIDWeights > 0 && !fIsData && !fIsEmbed) {
    if(fUseJetPUIDWeights == 1) { //calculate the PU ID weight
      jetPUIDWeight = fJetPUWeight->GetWeight(fYear, nJets20, jetsPt, jetsEta, nJets20Rej, jetsRejPt, jetsRejEta);
    }
  } else {
    jetPUIDWeight = 1.f;
  }

  eventWeight *= jetPUIDWeight;

  ////////////////////////////////////////////////////////////////////
  //   B-tag ID weights
  ////////////////////////////////////////////////////////////////////

  btagWeight = 1.f; btagWeightUpBC = 1.f; btagWeightDownBC = 1.f; btagWeightUpL = 1.f; btagWeightDownL = 1.f;
  if(fUseBTagWeights && !fIsData && !fIsEmbed) {
    int wp = BTagWeight::kLooseBTag; //ee/mumu/emu use loose ID b-tag
    if(leptonTwo.isTau()) wp = BTagWeight::kTightBTag;
    if(fUseBTagWeights == 1) { //calculate the weight
      //FIXME: Add a category with Tight ID b-tag in ee/mumu for Z->x+tau_h normalization?
      btagWeight = fBTagWeight->GetWeight(wp, fYear, nJets20, jetsPt, jetsEta, jetsFlavor, jetsBTag, btagWeightUpBC, btagWeightDownBC, btagWeightUpL, btagWeightDownL);
      eventWeight *= btagWeight;
      if((btagWeight - btagWeightUpBC) * (btagWeight - btagWeightDownBC) > 1.e-3 && fVerbose > 0) {
        std::cout << "!!! Warning: Entry " << fentry << " B-tag weight up/down are on the same side of the weight: Wt = "
                  << btagWeight << " Up = " << btagWeightUpBC << " Down = " << btagWeightDownBC << std::endl;
      }
    } else if(fUseBTagWeights == 2) { //loop over the jet collection with the ntuple-level weights
      for(unsigned ijet = 0; ijet < nJets20; ++ijet) {
        if(std::fabs(Jet_eta[ijet]) >= 2.4f) continue; //b-tagging region
        if(std::fabs(Jet_pt [ijet]) <= 20.f) continue; //b-tag jet pt cut
        const float wt      = (wp == BTagWeight::kLooseBTag) ? Jet_btagSF_deepcsv_L     [ijet] : Jet_btagSF_deepcsv_T     [ijet];
        const float wt_up   = (wp == BTagWeight::kLooseBTag) ? Jet_btagSF_deepcsv_L_up  [ijet] : Jet_btagSF_deepcsv_T_up  [ijet];
        const float wt_down = (wp == BTagWeight::kLooseBTag) ? Jet_btagSF_deepcsv_L_down[ijet] : Jet_btagSF_deepcsv_T_down[ijet];
        //split uncertainties by gen-level parton flavor
        const bool  isbc    = std::fabs(Jet_partonFlavour[ijet]) == 4 || std::fabs(Jet_partonFlavour[ijet]) == 5;
        btagWeight       *= wt;
        btagWeightUpBC   *= ( isbc) ? wt_up   : wt;
        btagWeightDownBC *= ( isbc) ? wt_down : wt;
        btagWeightUpL    *= (!isbc) ? wt_up   : wt;
        btagWeightDownL  *= (!isbc) ? wt_down : wt;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////
  //   Lepton ID weights
  ////////////////////////////////////////////////////////////////////

  if(fIsEmbed) {
    //Lepton 1
    if     (leptonOne.isElectron()) fEmbeddingTnPWeight.ElectronIDWeight(leptonOne.p4->Pt(), leptonOne.scEta, fYear,
                                                                         leptonOne.wt1[0], leptonOne.wt1[1], leptonOne.wt1[2], //electron ID
                                                                         leptonOne.wt3[0], leptonOne.wt3[1], leptonOne.wt3[2], //electron iso ID
                                                                         leptonOne.isLoose, mcEra);
    else if(leptonOne.isMuon    ()) fEmbeddingTnPWeight.MuonIDWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear,
                                                                     leptonOne.wt1[0], leptonOne.wt1[1], leptonOne.wt1[2], //muon ID
                                                                     leptonOne.wt2[0], leptonOne.wt2[1], leptonOne.wt2[2], //muon iso ID
                                                                     leptonOne.isLoose, mcEra);
    else if(leptonOne.isTau     ()) leptonOne.wt1[0] = fTauIDWeight->IDWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                              fYear, leptonOne.wt1[1], leptonOne.wt1[2], leptonOne.wt1_bin);
    //Lepton 2
    if     (leptonTwo.isElectron()) fEmbeddingTnPWeight.ElectronIDWeight(leptonTwo.p4->Pt(), leptonTwo.scEta, fYear,
                                                                         leptonTwo.wt1[0], leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                                         leptonTwo.wt3[0], leptonTwo.wt3[1], leptonTwo.wt3[2],
                                                                         leptonTwo.isLoose, mcEra);
    else if(leptonTwo.isMuon    ()) fEmbeddingTnPWeight.MuonIDWeight(leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear,
                                                                     leptonTwo.wt1[0], leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                                     leptonTwo.wt2[0], leptonTwo.wt2[1], leptonTwo.wt2[2],
                                                                     leptonTwo.isLoose, mcEra);
    else if(leptonTwo.isTau     ()) leptonTwo.wt1[0] = fTauIDWeight->IDWeight(leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                              fYear, leptonTwo.wt1[1], leptonTwo.wt1[2], leptonTwo.wt1_bin);

  } else if(!fIsData) { //MC simulations
    //Lepton 1
    if     (leptonOne.isElectron()) fElectronIDWeight.IDWeight(leptonOne.p4->Pt(), leptonOne.scEta, fYear,
                                                               leptonOne.wt1[0] , leptonOne.wt1[1], leptonOne.wt1[2], //electon ID
                                                               leptonOne.wt2[0] , leptonOne.wt2[1], leptonOne.wt2[2]); //electron reco ID
    else if(leptonOne.isMuon    ()) fMuonIDWeight.IDWeight    (leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear, mcEra,
                                                               leptonOne.wt1[0] , leptonOne.wt1[1], leptonOne.wt1[2],
                                                               leptonOne.wt2[0] , leptonOne.wt2[1], leptonOne.wt2[2]);
    else if(leptonOne.isTau     ()) leptonOne.wt1[0] = fTauIDWeight->IDWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                              fYear, leptonOne.wt1[1], leptonOne.wt1[2], leptonOne.wt1_bin);
    //Lepton 2
    if     (leptonTwo.isElectron()) fElectronIDWeight.IDWeight(leptonTwo.p4->Pt(), leptonTwo.scEta, fYear,
                                                               leptonTwo.wt1[0] , leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                               leptonTwo.wt2[0] , leptonTwo.wt2[1], leptonTwo.wt2[2]);
    else if(leptonTwo.isMuon    ()) fMuonIDWeight.IDWeight    (leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear, mcEra,
                                                               leptonTwo.wt1[0] , leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                               leptonTwo.wt2[0] , leptonTwo.wt2[1], leptonTwo.wt2[2]);
    else if(leptonTwo.isTau     ()) leptonTwo.wt1[0] = fTauIDWeight->IDWeight(leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                              fYear, leptonTwo.wt1[1], leptonTwo.wt1[2], leptonTwo.wt1_bin);
  }
  eventWeight *= leptonOne.wt1[0]*leptonOne.wt2[0];
  eventWeight *= leptonTwo.wt1[0]*leptonTwo.wt2[0];

  /////////////////////////
  // Jet --> tau weights //
  /////////////////////////

  //weigh anti-iso tau region by anti-iso --> tight iso weight
  if((etau || mutau) && isLooseTau) {
    //use data factor for MC and Data, since not using MC estimated fake tau rates
    if(fUseJetToTauComposition) {
      jetToTauWeight = 0.f;
      jetToTauWeightCorr = 0.f;
      jetToTauWeightBias = 0.f;
      jetToTauWeight_compUp = 0.f;
      jetToTauWeight_compDown = 0.f;
      Float_t jttUp(0.f), jttDown(0.f);
      if(mutau) {
        if(chargeTest) {
          fMuonJetToTauComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                           leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                           fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        } else {
          fMuonJetToTauSSComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                             leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                             fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        }
      } else {
        if(chargeTest) {
          fElectronJetToTauComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                               leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                               fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        } else {
          fElectronJetToTauSSComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                                 leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone,
                                                 fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        }
      }
      for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
        if(mutau) {
          fJetToTauWts[proc]   = (fMuonJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                            leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                            fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(),
                                                                            fJetToTauWtsUp[proc], fJetToTauWtsDown[proc],
                                                                            fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                            fJetToTauBiases[proc]));
          fJetToTauMCWts[proc] = (fMuonJetToTauMCWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                              leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                              fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(),
                                                                              fJetToTauMCCorrs[proc], fJetToTauMCBiases[proc]));
        } else {
          fJetToTauWts[proc]   = (fElectronJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                                leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                                fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(),
                                                                                fJetToTauWtsUp[proc], fJetToTauWtsDown[proc],
                                                                                fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                                fJetToTauBiases[proc]));
          fJetToTauMCWts[proc] = (fElectronJetToTauMCWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                                  leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                                  fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(),
                                                                                  fJetToTauMCCorrs[proc], fJetToTauMCBiases[proc]));
        }
        if(fJetToTauWtsUp[proc] < fJetToTauWts[proc]) {
          if(fVerbose > -1) printf("HistSelector::%s: j-->tau weight up for proc %i below nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
                                   __func__, proc, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauDecayMode, leptonOne.p4->Pt(), leptonOne.p4->Eta());
          fJetToTauWtsUp[proc] = fJetToTauWts[proc];
        }
        if(fJetToTauWtsDown[proc] > fJetToTauWts[proc]) {
          if(fVerbose > -1) printf("HistSelector::%s: j-->tau weight down for proc %i above nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
                                   __func__, proc, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauDecayMode, leptonOne.p4->Pt(), leptonOne.p4->Eta());
          fJetToTauWtsDown[proc] = fJetToTauWts[proc];
        }
        jetToTauWeight     += fJetToTauComps[proc] * fJetToTauWts[proc];
        jetToTauWeightCorr += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc]; //weight with the correction for this process
        jetToTauWeightBias += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc]; //weight with the correction and bias for this process
        //store systematic effects, added linearly across all processes
        jttUp   += fJetToTauComps[proc] * fJetToTauWtsUp[proc];
        jttDown += fJetToTauComps[proc] * fJetToTauWtsDown[proc];
        jetToTauWeight_compUp   += fJetToTauCompsUp  [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
        jetToTauWeight_compDown += fJetToTauCompsDown[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
      }
      jetToTauWeightUp   = jttUp  ;
      jetToTauWeightDown = jttDown;
      jetToTauWeightCorrUp   = jetToTauWeight; //set correction up to be ignoring the correction
      jetToTauWeightCorrDown = std::max(0.f, 2.f*jetToTauWeightCorr - jetToTauWeight); //size of the weight in the other direction from 1
      jetToTauWeightBiasUp   = jetToTauWeightCorr; //set correction up to be ignoring the correction
      jetToTauWeightBiasDown = std::max(0.f, 2.f*jetToTauWeightBias - jetToTauWeightCorr); //size of the weight in the other direction from 1
      if(jetToTauWeightUp < jetToTauWeight) {
        if(fVerbose > -1) printf("HistSelector::%s: j-->tau weight up below nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
                                 __func__, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauDecayMode, leptonOne.p4->Pt(), leptonOne.p4->Eta());
        jetToTauWeightUp = jetToTauWeight;
      }
      if(jetToTauWeightDown > jetToTauWeight) {
        if(fVerbose > -1) printf("HistSelector::%s: j-->tau weight down above nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
                                 __func__, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauDecayMode, leptonOne.p4->Pt(), leptonOne.p4->Eta());
        jetToTauWeightDown = jetToTauWeight;
      }
    } else {
      std::cout << "Error! Jet --> Tau weight without composition no longer supported!\n";
      throw 20;
    }
    //set j-->tau bias uncertainty to its size
    const Float_t bias_size = jetToTauWeightBias / jetToTauWeightCorr;
    jetToTauWeightBiasUp = jetToTauWeightCorr;
    jetToTauWeightBiasDown = (2.f*bias_size - 1.f)*jetToTauWeightCorr;
  }
  eventWeight *= jetToTauWeightBias;

  ///////////////////////
  // SS --> OS weights //
  ///////////////////////

  //get scale factor for same sign --> opposite sign, apply to MC and Data same-sign events
  if(emu && !chargeTest && fUseQCDWeights) {
    qcdWeight = fQCDWeight.GetWeight(fTreeVars.lepdeltar, fTreeVars.lepdeltaphi, fTreeVars.leponeeta, fTreeVars.leponept, fTreeVars.leptwopt,
                                     fYear, nJets20, isLooseMuon, qcdClosure, qcdIsoScale, qcdWeightUp, qcdWeightDown, qcdWeightSys);
  }

  eventWeight *= qcdWeight;

  //initialize weights for the tree variables
  fTreeVars.eventweight = *genWeight*eventWeight;
  fTreeVars.eventweightMVA = *genWeight*eventWeight;
  fTreeVars.fulleventweight = *genWeight*eventWeight*fXsec;
  fTreeVars.fulleventweightlum = fTreeVars.fulleventweight*fLum;

  //initialize systematic weights for TMVA systematic testing, remove non-closure and bis corrections as an estimate
  fTreeVars.zptsys             = (zPtWeight > 0.f) ? zPtWeightSys / zPtWeight : 1.f;
  fTreeVars.jetantimusys       = (leptonTwo.isTau() && std::abs(tauGenFlavor) == 13) ? leptonTwo.wt1[1] / leptonTwo.wt1[0] : 1.f;
  fTreeVars.jetantielesys      = (leptonTwo.isTau() && std::abs(tauGenFlavor) == 11) ? leptonTwo.wt1[1] / leptonTwo.wt1[0] : 1.f;
  fTreeVars.qcdstat            = (!chargeTest && qcdWeight   > 0.) ? qcdWeightUp / qcdWeight : 1.f;
  fTreeVars.qcdnc              = (!chargeTest && qcdClosure  > 0.) ? 1.f / qcdClosure        : 1.f;
  fTreeVars.qcdbias            = (!chargeTest && qcdIsoScale > 0.) ? 1.f / qcdIsoScale       : 1.f;
  fTreeVars.btagsys            = (btagWeight > 0.f) ? btagWeightUpBC / btagWeight : 1.f;
  fTreeVars.lumisys            = (fIsData || fIsEmbed) ? 1.f : (fYear == 2016) ? 1.012 : (fYear == 2017) ? 1.023 : 1.025;

  //j-->tau uncertainties, by process
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    float prev_wt(fJetToTauWts[proc]), prev_nc(fJetToTauCorrs[proc]),
      prev_bias(fJetToTauBiases[proc]);
    if(isLooseTau) {
      float wt, wtcorr, wtbias;
      //statistical errors
      fJetToTauWts[proc] = fJetToTauWtsUp[proc];
      EvalJetToTauWeights(wt, wtcorr, wtbias); fJetToTauWts[proc] = prev_wt;
      fTreeVars.jettotaustatup  [proc] = wtbias / jetToTauWeightBias;
      fJetToTauWts[proc] = fJetToTauWtsDown[proc];
      EvalJetToTauWeights(wt, wtcorr, wtbias); fJetToTauWts[proc] = prev_wt;
      fTreeVars.jettotaustatdown[proc] = wtbias / jetToTauWeightBias;
      //non-closure error
      fJetToTauCorrs[proc] = 1.f;
      EvalJetToTauWeights(wt, wtcorr, wtbias); fJetToTauCorrs[proc] = prev_nc;
      fTreeVars.jettotaunoncl[proc] = wtbias / jetToTauWeightBias;
      //bias error
      fJetToTauBiases[proc] = 1.f;
      EvalJetToTauWeights(wt, wtcorr, wtbias); fJetToTauBiases[proc] = prev_bias;
      fTreeVars.jettotaubias[proc] = wtbias / jetToTauWeightBias;
    } else {
      fTreeVars.jettotaunoncl   [proc] = 1.f;
      fTreeVars.jettotaustatup  [proc] = 1.f;
      fTreeVars.jettotaustatdown[proc] = 1.f;
      fTreeVars.jettotaubias    [proc] = 1.f;
    }
    //ensure weights are restored
    fJetToTauWts[proc] = prev_wt; fJetToTauCorrs[proc] = prev_nc; fJetToTauBiases[proc] = prev_bias;
  }
  fTreeVars.jettotaucomp = (isLooseTau) ? jetToTauWeight_compUp / jetToTauWeightBias : 1.f;

  //lepton ID uncertainties
  fTreeVars.leponeid1sys       = (leptonOne.wt1[0] > 0.f) ? leptonOne.wt1[1] / leptonOne.wt1[0] : 1.f;
  fTreeVars.leponeid2sys       = (leptonOne.wt2[0] > 0.f) ? leptonOne.wt2[1] / leptonOne.wt2[0] : 1.f;
  fTreeVars.leptwoid1sys       = (leptonTwo.wt1[0] > 0.f) ? leptonTwo.wt1[1] / leptonTwo.wt1[0] : 1.f;
  fTreeVars.leptwoid2sys       = (leptonTwo.wt2[0] > 0.f) ? leptonTwo.wt2[1] / leptonTwo.wt2[0] : 1.f;
  fTreeVars.leponeid1bin       = leptonOne.wt1_bin;
  fTreeVars.leptwoid1bin       = leptonTwo.wt1_bin;

  //if splitting testing/training samples
  //FIXME: Turn back on the removing of MVA training split weights
  if(!fIsData || looseQCDSelection)
    fTreeVars.eventweightMVA *= (fTreeVars.train > 0.f) ? 0.f : 1.f/(1.f-fFractionMVA); //if training, ignore, else rescale to account for training sample removed
}

//--------------------------------------------------------------------------------------------------------------
// Set kinematics relevant to MVAs, to re-evaluate scores after shifted values etc.
void HistSelector::SetKinematics() {
  leptonOne.d0 = std::sqrt(leptonOne.dxy*leptonOne.dxy + leptonOne.dz*leptonOne.dz);
  leptonTwo.d0 = std::sqrt(leptonTwo.dxy*leptonTwo.dxy + leptonTwo.dz*leptonTwo.dz);
  fTreeVars.leponept     = leptonOne.p4->Pt();
  fTreeVars.leponem      = leptonOne.p4->M();
  fTreeVars.leponeeta    = leptonOne.p4->Eta();
  fTreeVars.leponed0     = leptonOne.d0;
  fTreeVars.leponeiso    = leptonOne.iso;
  fTreeVars.leponereliso = leptonOne.relIso;

  fTreeVars.leptwopt     = leptonTwo.p4->Pt();
  fTreeVars.leptwom      = leptonTwo.p4->M();
  fTreeVars.leptwoeta    = leptonTwo.p4->Eta();
  fTreeVars.leptwod0     = leptonTwo.d0;
  fTreeVars.leptwoiso    = leptonTwo.iso;
  fTreeVars.leptworeliso = leptonTwo.relIso;

  TLorentzVector lep = *leptonOne.p4 + *leptonTwo.p4;

  fTreeVars.lepp          = lep.P();
  fTreeVars.leppt         = lep.Pt();
  fTreeVars.lepm          = lep.M();
  fTreeVars.lepmt         = lep.Mt();
  fTreeVars.lepptoverm    = fTreeVars.leppt   /fTreeVars.lepm;
  fTreeVars.leponeptoverm = fTreeVars.leponept/fTreeVars.lepm;
  fTreeVars.leptwoptoverm = fTreeVars.leptwopt/fTreeVars.lepm;
  fTreeVars.lepeta        = lep.Eta();
  fTreeVars.lepdeltar     = leptonOne.p4->DeltaR(*leptonTwo.p4);
  fTreeVars.lepdeltaphi   = std::fabs(leptonOne.p4->DeltaPhi(*leptonTwo.p4));
  fTreeVars.lepdeltaeta   = std::fabs(leptonOne.p4->Eta() - leptonTwo.p4->Eta());
  fTreeVars.ptdiff        = leptonOne.pt - leptonTwo.pt;
  fTreeVars.ptdiffoverm   = fTreeVars.ptdiff / fTreeVars.lepm;
  fTreeVars.ptratio       = leptonOne.p4->Pt() / leptonTwo.p4->Pt();

  //Tau trk variables
  TLorentzVector trk;
  if(leptonTwo.isTau()) trk.SetPtEtaPhiM(leptonTwo.trkpt, leptonTwo.trketa, leptonTwo.trkphi, TAUMASS);
  else                  trk = *leptonTwo.p4;
  if(trk.M2() < 0.)     trk.SetE(trk.P());
  fTreeVars.trkpt   = trk.Pt();
  fTreeVars.trketa  = trk.Eta();
  fTreeVars.trkphi  = trk.Phi();
  fTreeVars.leptrkm = (*leptonOne.p4 + trk).M();
  fTreeVars.leptrkdeltam = fTreeVars.lepm - fTreeVars.leptrkm;
  fTreeVars.trkptoverpt  = fTreeVars.trkpt / leptonTwo.p4->Pt();

  //phi differences
  fTreeVars.htdeltaphi     = std::fabs(Utilities::DeltaPhi(lep.Phi(), htPhi));
  fTreeVars.metdeltaphi    = std::fabs(Utilities::DeltaPhi(lep.Phi(), metPhi));
  fTreeVars.leponedeltaphi = std::fabs(Utilities::DeltaPhi(leptonOne.p4->Phi(), lep.Phi()));
  fTreeVars.leptwodeltaphi = std::fabs(Utilities::DeltaPhi(leptonTwo.p4->Phi(), lep.Phi()));
  fTreeVars.onemetdeltaphi = std::fabs(Utilities::DeltaPhi(leptonOne.p4->Phi(), metPhi));
  fTreeVars.twometdeltaphi = std::fabs(Utilities::DeltaPhi(leptonTwo.p4->Phi(), metPhi));

  //MET variables
  fTreeVars.met        = met;
  fTreeVars.mtone      = std::sqrt(2.*met*leptonOne.p4->Pt()*(1.-cos(leptonOne.p4->Phi() - metPhi)));
  fTreeVars.mttwo      = std::sqrt(2.*met*leptonTwo.p4->Pt()*(1.-cos(leptonTwo.p4->Phi() - metPhi)));
  fTreeVars.mtlep      = std::sqrt(2.*met*lep.Pt()*(1.-cos(lep.Phi() - metPhi)));
  fTreeVars.mtdiff     = fTreeVars.mtone - fTreeVars.mttwo;
  fTreeVars.mtoneoverm = fTreeVars.mtone / fTreeVars.lepm;
  fTreeVars.mttwooverm = fTreeVars.mttwo / fTreeVars.lepm;
  fTreeVars.mtlepoverm = fTreeVars.mtlep / fTreeVars.lepm;

  //momentum projections onto bisector
  TVector3 lp1 = leptonOne.p4->Vect();
  TVector3 lp2 = leptonTwo.p4->Vect();
  TVector3 missing(met*std::cos(metPhi), met*std::sin(metPhi), 0.);
  if(!std::isfinite(missing.Pt()) || !std::isfinite(met) || !std::isfinite(metPhi)) {
    printf("!!! HistSelector::%s: Entry: %12lld: MET vector is undefined! met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f\n",
           __func__, fentry, met, metPhi, metCorr, metCorrPhi);
  }
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector.Mag() > 0.) bisector.SetMag(1.);

  //////////////////////////////////////////////
  // MET projections and neutrino estimates

  //project MET onto leptons
  fTreeVars.metdotl1 = missing*lp1/lp1.Mag(); //MET along lep 1
  fTreeVars.metdotl2 = missing*lp2/lp2.Mag(); //MET along lep 2

  //project onto the bisectors
  fTreeVars.pzetavis     = (lp1+lp2)*bisector;
  fTreeVars.pzetainv     = missing*bisector;
  fTreeVars.pzetaall     = (lp1 + lp2 + missing)*bisector;
  fTreeVars.dzeta        = fTreeVars.pzetaall - 0.85*fTreeVars.pzetavis;

  EstimateNeutrinos();

  //Perform a transform to the Z/H boson frame
  //Requires knowledge of which lepton has associated neutrinos (or if none do)
  //modes:
  // 0: MET defines x-axis, lepton two has positive Pz (mutau_h, etau_h, etau_mu)
  // 1: MET defines x-axis, lepton one has positive Pz (mutau_e);
  // 2: lepton 2 defines x-axis, lepton one has positive Pz (emu)
  TLorentzVector metP4;
  metP4.SetPtEtaPhiE(missing.Pt(), 0., missing.Phi(), missing.Pt());
  for(int imode = 0; imode < 3; ++imode) {
    TLorentzVector system;
    system = *leptonOne.p4 + *leptonTwo.p4;
    if(imode < 2) system += metP4;
    TVector3 boost = -1*system.BoostVector(); //boost to the center of mass system
    boost.SetZ(0.); //only transform pT to 0, since MET is only in pT
    TLorentzVector lepOnePrime(*leptonOne.p4);
    TLorentzVector lepTwoPrime(*leptonTwo.p4);
    TLorentzVector metPrime(missing, missing.Mag());
    lepOnePrime.Boost(boost); lepTwoPrime.Boost(boost); metPrime.Boost(boost); //boost so system pT = 0
    double phiRot;
    if(imode < 2) { //set MET along x axis
      phiRot = -metPrime.Phi();
    } else { //set lepton two pT (muon in emu) along the x-axis
      phiRot = -lepTwoPrime.Phi();
    }
    lepOnePrime.RotateZ(phiRot); lepTwoPrime.RotateZ(phiRot); metPrime.RotateZ(phiRot);

    //if the tau (or electron in Z->e+mu mode) has a negative momentum, rotate about x 180 degrees to make it positive
    if((imode == 0 && lepTwoPrime.Pz() < 0.) || (imode != 0 && lepOnePrime.Pz() < 0.)) { //for emu, set electron in positive Pz
      lepOnePrime.RotateX(M_PI); lepTwoPrime.RotateX(M_PI); metPrime.RotateX(M_PI);
    }
    if(!std::isfinite(lepOnePrime.Px()) || !std::isfinite(lepOnePrime.Py()) || !std::isfinite(lepOnePrime.Pz()) || !std::isfinite(lepOnePrime.E())) {
      printf("!!! HistSelector::%s: Entry %lld : Non-finite boosted lepton p4 components!\n", __func__, fentry);
      printf("lepone: (pt, eta, phi, E, M^2) = (%.2f, %.3f, %.3f, %.2f, %.3e)\n",
             leptonOne.p4->Pt(), leptonOne.p4->Eta(), leptonOne.p4->Phi(), leptonOne.p4->E(), leptonOne.p4->M2());
      printf("leptwo: (pt, eta, phi, E, M^2) = (%.2f, %.3f, %.3f, %.2f, %.3e)\n",
             leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonTwo.p4->Phi(), leptonTwo.p4->E(), leptonTwo.p4->M2());
      printf("met   : (pt, eta, phi, E, M^2) = (%.2f, %.3f, %.3f, %.2f, %.3e), met = %.2f, metPhi = %.3f\n",
             metP4.Pt(), metP4.Eta(), metP4.Phi(), metP4.E(), metP4.M2(), met, metPhi);
      printf("system: (pt, eta, phi, E, M^2) = (%.2f, %.3f, %.3f, %.2f, %.3e)\n",
             system.Pt(), system.Eta(), system.Phi(), system.E(), system.M2());
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

  ////////////////////////////////////////
  // Additional Event Variables

  fTreeVars.ht       = ht;
  fTreeVars.htsum    = htSum;
  fTreeVars.jetpt    = jetOne.p4->Pt();
}

//--------------------------------------------------------------------------------------------------------------
// Estimate the neutrino contributions with different methods/hypotheses
void HistSelector::EstimateNeutrinos() {
  //Get pT vectors for leptons and MET
  TLorentzVector lep = *leptonOne.p4 + *leptonTwo.p4;
  TVector3 lp1 = leptonOne.p4->Vect();
  TVector3 lp2 = leptonTwo.p4->Vect();
  TVector3 missing(met*std::cos(metPhi), met*std::sin(metPhi), 0.);
  if(!std::isfinite(missing.Pt()) || !std::isfinite(met) || !std::isfinite(metPhi)) {
    printf("!!! HistSelector::%s: Entry: %12lld: MET vector is undefined! met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f\n",
           __func__, fentry, met, metPhi, metCorr, metCorrPhi);
  }
  lp1.SetZ(0.);
  lp2.SetZ(0.);

  const double pnuest    = std::max(-0.999*lp2.Mag(),(double) fTreeVars.metdotl2); //inv pT along tau = lep 2 pt unit vector dot missing
  fTreeVars.ptauvisfrac  = lp2.Mag() / (std::max(1.e-5, lp2.Mag() + pnuest));

  //M(collinear), only using >= 0 MET along tau
  fTreeVars.mestimate_cut_1 = fTreeVars.lepm*((fTreeVars.metdotl2 > 0.f) ? std::sqrt((fTreeVars.metdotl2 + lp2.Mag())/lp2.Mag()) : 1.f); //tau = lep 2
  fTreeVars.mestimate_cut_2 = fTreeVars.lepm*((fTreeVars.metdotl1 > 0.f) ? std::sqrt((fTreeVars.metdotl1 + lp1.Mag())/lp1.Mag()) : 1.f); //tau = lep 1

  //M(collinear), using neutrino = MET along tau, including larger pT than tau in opposite direction
  //if pT neutrino is negative, make it anti-parallel instead
  float nu_coll_pt_1(fTreeVars.metdotl2), nu_coll_pt_2(fTreeVars.metdotl1); //1 = tau cand. is lep 2, 2 = tau cand. is lep 1 (mutau_e only)
  fTreeVars.p_nu_col_1 = nu_coll_pt_1;
  fTreeVars.p_nu_col_2 = nu_coll_pt_2;
  TLorentzVector nu_coll_1, nu_coll_2;
  nu_coll_1.SetPtEtaPhiM(std::fabs(nu_coll_pt_1), leptonTwo.eta*((nu_coll_pt_1 < 0.f) ? -1.f : 1.f), leptonTwo.phi + ((nu_coll_pt_1 < 0.f) ? M_PI : 0.f), 0.);
  nu_coll_2.SetPtEtaPhiM(std::fabs(nu_coll_pt_2), leptonOne.eta*((nu_coll_pt_2 < 0.f) ? -1.f : 1.f), leptonOne.phi + ((nu_coll_pt_2 < 0.f) ? M_PI : 0.f), 0.);
  fTreeVars.mestimate    = (lep + nu_coll_1).M();
  fTreeVars.mestimatetwo = (lep + nu_coll_2).M();

  //get the neutrino momentum by assuming the light lepton and tau had the same pT to begin with, and it's collinear
  //if pT neutrino is negative, make it anti-parallel instead
  TLorentzVector nu_bal_1, nu_bal_2;
  nu_bal_1.SetPtEtaPhiM(std::fabs(leptonOne.pt - leptonTwo.pt), leptonTwo.eta*((leptonOne.pt < leptonTwo.pt) ? -1.f : 1.f), leptonTwo.phi + (leptonOne.pt < leptonTwo.pt) ? M_PI : 0.f, 0.f);
  nu_bal_2.SetPtEtaPhiM(std::fabs(leptonTwo.pt - leptonOne.pt), leptonOne.eta*((leptonTwo.pt < leptonOne.pt) ? -1.f : 1.f), leptonOne.phi + (leptonTwo.pt < leptonOne.pt) ? M_PI : 0.f, 0.f);
  fTreeVars.mbalance    = (nu_bal_1 + lep).M();
  fTreeVars.mbalancetwo = (nu_bal_2 + lep).M();
  fTreeVars.p_nu_bal_1 = nu_bal_1.Pt();
  fTreeVars.p_nu_bal_2 = nu_bal_2.Pt();

  //average the neutrino estimates from the pT balancing and MET projection strategies
  TVector3 nu_avg_1_p = 0.5*(nu_coll_1.Vect() + nu_bal_1.Vect());
  TVector3 nu_avg_2_p = 0.5*(nu_coll_2.Vect() + nu_bal_2.Vect());
  TLorentzVector nu_avg_1(nu_avg_1_p, nu_avg_1_p.Mag()), nu_avg_2(nu_avg_2_p, nu_avg_2_p.Mag());
  fTreeVars.mestimate_avg_1 = (nu_avg_1 + lep).M();
  fTreeVars.mestimate_avg_2 = (nu_avg_2 + lep).M();
  fTreeVars.p_nu_avg_1 = nu_avg_1.Pt();
  fTreeVars.p_nu_avg_2 = nu_avg_2.Pt();

  //get the neutrino eta from the tau eta, but the pT/phi from the MET
  TLorentzVector nu_met_1, nu_met_2;
  nu_met_1.SetPtEtaPhiM(met, leptonTwo.eta, metPhi, 0.);
  nu_met_2.SetPtEtaPhiM(met, leptonOne.eta, metPhi, 0.);
  fTreeVars.mestimatethree = (nu_met_1 + lep).M();
  fTreeVars.mestimatefour  = (nu_met_2 + lep).M();


  /////////////////////////////
  // delta alpha calculations
  //definition from (14) and (16) of arxiv:1207.4894

  //alpha * pT(vis-tau) = beta * pT(vis-lep) (For signal, beta = 1, alpha = pT(lep) / pT(tau))
  //(M^2(boson) - (1 for signal, 2 for DY)*M^2(tau)) ~ M^2(boson) = 2*alpha*beta*(p1.p2)
  // --> for signal, alpha = pT(lep)/pT(tau) = (M^2(boson) - M^2(tau))/(2*p1.p2)
  //alpha 1 = (m_boson^2 - m_tau^2) / (p(l1)\cdot p(l2))
  //alpha 2 = pT(l2) / pT(l1) (l1 = tau)
  //alpha 3 = pT(l1) / pT(l2) (l2 = tau)

  //if beta != 0
  // alpha = beta * pT(vis-lep)/pT(vis-tau)
  // M^2(boson) - 2*M^2(tau) = 2*beta^2*(pT(vis-lep)/pT(vis-tau))*(p1.p2)
  //--> beta = ~M(boson)*sqrt(pT(vis-tau)/pT(vis-lep)/p1.p2)

  const double hmass(HIGGSMASS), zmass(ZMASS), tmass(TAUMASS), lepdot(2.*((*leptonOne.p4)*(*leptonTwo.p4)));
  fTreeVars.alphaz1 = (zmass*zmass-tmass*tmass)/(lepdot);
  fTreeVars.alphah1 = (hmass*hmass-tmass*tmass)/(lepdot);
  fTreeVars.alpha2 = leptonTwo.pt/leptonOne.pt; //for lep 1 = tau, lep 2 = non-tau
  fTreeVars.alpha3 = leptonOne.pt/leptonTwo.pt; //for lep 2 = non-tau, lep 1 = tau
  fTreeVars.beta1  = zmass*(std::sqrt(leptonOne.pt/leptonTwo.pt/lepdot)); //for lep 1 = tau, lep 2 = non-tau
  fTreeVars.beta2  = zmass*(std::sqrt(leptonTwo.pt/leptonOne.pt/lepdot)); //for lep 2 = non-tau, lep 1 = tau
  fTreeVars.deltaalphaz1 = fTreeVars.alphaz1 - fTreeVars.alpha2;
  fTreeVars.deltaalphaz2 = fTreeVars.alphaz1 - fTreeVars.alpha3;
  fTreeVars.deltaalphah1 = fTreeVars.alphah1 - fTreeVars.alpha2;
  fTreeVars.deltaalphah2 = fTreeVars.alphah1 - fTreeVars.alpha3;
  //mass from delta alpha equation: m_boson = sqrt(m_tau^2 + pT(lep)/pT(tau) * (p(l1) dot p(l2)))
  fTreeVars.deltaalpham1 = std::sqrt(tmass*tmass + fTreeVars.alpha2 * lepdot); //lep 1 = tau
  fTreeVars.deltaalpham2 = std::sqrt(tmass*tmass + fTreeVars.alpha3 * lepdot); //lep 2 = tau

}

//--------------------------------------------------------------------------------------------------------------
// Evaluate each MVA that is relevant to the current selection
void HistSelector::EvalMVAs(TString TimerName) {
  if(!fReprocessMVAs) return;
  const bool eval_timer = TimerName != "";
  if(eval_timer) fTimes[GetTimerNumber(TimerName.Data())] = std::chrono::steady_clock::now(); //timer for evaluating the MVAs
  //information about the event selection
  TString selecName = "";
  if     (leptonOne.isMuon    () && leptonTwo.isTau     ()) selecName = "mutau";
  else if(leptonOne.isElectron() && leptonTwo.isTau     ()) selecName = "etau" ;
  else if(leptonOne.isElectron() && leptonTwo.isMuon    ()) selecName = "emu"  ;
  else if(leptonOne.isMuon    () && leptonTwo.isMuon    ()) selecName = "mumu" ;
  else if(leptonOne.isElectron() && leptonTwo.isElectron()) selecName = "ee"   ;
  else                    {
    std::cout << "---Warning! Entry " << fentry
              << " has undentified selection in " << __func__ << "!\n";
    selecName = "unknown";
  }
  for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
    //assume only relevant MVAs are configured
    fMvaOutputs[i] = -2.f;
    if(!mva[i]) continue;
    fMvaOutputs[i] = mva[i]->EvaluateMVA(fMVAConfig.names_[i].Data());

    if(fMvaOutputs[i] < -2.1f) {
      std::cout << "Error value returned for MVA " << fMVAConfig.names_[i].Data()
                << " evaluation, Entry = " << fentry << std::endl;
    }
  }
  if(eval_timer) IncrementTimer(TimerName.Data(), true);
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::InitializeTreeVariables() {
  //force it to be that nbjets loose >= nbjets medium etc
  if(fForceBJetSense) {
    if(nBJets  > nBJetsM) nBJetsM = nBJets;
    if(nBJetsM > nBJetsL) nBJetsL = nBJetsM;
    if(nBJets20   > nBJets20M ) nBJets20M = nBJets20;
    if(nBJets20M  > nBJets20L ) nBJets20L = nBJets20M;
  }

  //Initialize variables that can be systematically varied
  SetKinematics();

  //Initialize variables not varied
  fTreeVars.leponeidone   = 0.;
  fTreeVars.leponeidtwo   = 0.;
  fTreeVars.leponeidthree = 0.;
  if(leptonTwo.isTau     ()) {//tau selection
    fTreeVars.leptwoidone   = tauDeepAntiEle;
    fTreeVars.leptwoidtwo   = tauDeepAntiMu;
    fTreeVars.leptwoidthree = tauDeepAntiJet;
    fTreeVars.taudecaymode  = tauDecayMode;
    fTreeVars.taugenflavor  = std::abs(tauGenFlavor);
  } else { //no other ids for now
    fTreeVars.leptwoidone   = 0.f;
    fTreeVars.leptwoidtwo   = 0.f;
    fTreeVars.leptwoidthree = 0.f;
    fTreeVars.taudecaymode  = 0.f;
  }

  //Project the MET onto the di-lepton pT system direction for recoil definitions
  TVector3 missing(met*std::cos(metPhi), met*std::sin(metPhi), 0.);
  TVector3 sys_pt_dir = (*leptonOne.p4 + *leptonTwo.p4).Vect();
  sys_pt_dir.SetZ(0.);
  sys_pt_dir.SetMag(1.); //unit vector
  fTreeVars.met_u1 = missing*sys_pt_dir; //MET projected onto di-lepton system
  fTreeVars.met_u2 = std::sqrt(met*met - fTreeVars.met_u1*fTreeVars.met_u1)*((sys_pt_dir.DeltaPhi(missing) > 0.) ? 1. : -1.); //MET perpendicular to di-lepton system

  //event variables
  fTreeVars.njets      = nJets20;
  fTreeVars.nbjets     = nBJetsUse;
  fTreeVars.nphotons   = nPhotons;

  //dataset/event information
  fTreeVars.eventcategory = fEventCategory;
  if(!fIsData || looseQCDSelection) {
    if(fFractionMVA > 0.f) fTreeVars.train = (fRnd->Uniform() < fFractionMVA) ? 1.f : -1.f; //whether or not it is in the training sample
    else                   fTreeVars.train = -1.f;
  } else                   fTreeVars.train = -1.f;

  fTreeVars.issignal = (fIsData == 0) ? (2.f*(fIsSignal) - 1.f) : 0.f; //signal = 1, background = -1, data = 0
  fTreeVars.year = fYear;
  if(fTreeVars.type < -0.5) { //doesn't change for a given file, so only set once
    if(fIsSignal) fTreeVars.type = 0;
    else if(fIsData) fTreeVars.type = 1;
    else {
      TString filename = GetOutputName();
      if(filename.Contains("DY") || filename.Contains("Embed")) {
        if(fDYType == 2 && !filename.Contains("Embed")) fTreeVars.type = 2; //Z->ll
        else                                            fTreeVars.type = 3; //Z->tautau or inclusive or Z->tautau embedded
      } else if(filename.Contains("Top") || filename.Contains("ttbar")) fTreeVars.type = 4;
      else if(filename.Contains("Wlnu")) fTreeVars.type = 5; //W+(n)Jets
      else fTreeVars.type = 6; //Di-boson/whatever is leftover
    }
  }

  //Information about the Z decay, if applicable
  int zdecaymode = 0;
  if(fIsEmbed || fIsDY) {
    //zdecaymode = 0: unknown; 1: ee; 2: mumu; 3: tau_e tau_e; 4: tau_mu tau_mu; 5: tau_e tau_mu; 6: tau_e tau_h; 7: tau_mu tau_h; 8: tau_h tau_h
    const int pdg_1 = 0.5 + std::fabs(zLepOneID);
    const int pdg_2 = 0.5 + std::fabs(zLepTwoID);
    if     (pdg_1 == 11 && pdg_2 == 11) zdecaymode = 1;
    else if(pdg_1 == 13 && pdg_2 == 13) zdecaymode = 2;
    else if(pdg_1 == 15 && pdg_2 == 15) { //get tau decay info
      //hadronic decay Idx = -99, label as just a tau pdgId
      const int decay_1 = (zLepOneDecayIdx >= 0 ) ? std::abs(GenPart_pdgId[zLepOneDecayIdx]) : 15;
      const int decay_2 = (zLepTwoDecayIdx >= 0 ) ? std::abs(GenPart_pdgId[zLepTwoDecayIdx]) : 15;
      if     (decay_1 == 11 && decay_2 == 11) zdecaymode = 3;
      else if(decay_1 == 13 && decay_2 == 13) zdecaymode = 4;
      else if(decay_1 == 11 && decay_2 == 13) zdecaymode = 5;
      else if(decay_1 == 13 && decay_2 == 11) zdecaymode = 5;
      else if(decay_1 == 11 && decay_2 == 15) zdecaymode = 6;
      else if(decay_1 == 15 && decay_2 == 11) zdecaymode = 6;
      else if(decay_1 == 13 && decay_2 == 15) zdecaymode = 7;
      else if(decay_1 == 15 && decay_2 == 13) zdecaymode = 7;
      else if(decay_1 == 15 && decay_2 == 15) zdecaymode = 8;
      else {
        printf("HistSelector::%s: Entry %lld: Unknown Z decay mode! lep_1 ID = %i, lep_2 ID = %i, lep_1_decay = %i, lep_2_decay = %i, Idx(decay 1) = %i, Idx(decay 2) = %i, nGenPart = %i\n",
               __func__, fentry, pdg_1, pdg_2, decay_1, decay_2, zLepOneDecayIdx, zLepTwoDecayIdx, *nGenPart);
        if(fVerbose > 0) {
          printf("Gen-collection:\nIdx pdgId mother mother-ID\n");
          for(UInt_t npart = 0; npart < *nGenPart; ++npart) {
            const int mother = GenPart_genPartIdxMother[npart];
            printf(" %3i: %6i %3i %6i\n", npart, GenPart_pdgId[npart], mother,
                   (mother >= 0) ? GenPart_pdgId[mother] : 0);
          }
        }
      }
    }
    else if(pdg_1 == 11 && pdg_2 == 13) zdecaymode = 9;
    else if(pdg_1 == 13 && pdg_2 == 11) zdecaymode = 9;
    else if(pdg_1 == 11 && pdg_2 == 15) zdecaymode = 10;
    else if(pdg_1 == 15 && pdg_2 == 11) zdecaymode = 10;
    else if(pdg_1 == 13 && pdg_2 == 15) zdecaymode = 11;
    else if(pdg_1 == 15 && pdg_2 == 13) zdecaymode = 11;
    else {
      printf("HistSelector::%s: Unknown Z decay mode! lep_1 ID = %i, lep_2 ID = %i\n",
             __func__, pdg_1, pdg_2);
    }
  }
  fTreeVars.zdecaymode = zdecaymode;
  //information about the event selection
  if     (leptonOne.isMuon    () && leptonTwo.isTau     ()) fTreeVars.category = 1;
  else if(leptonOne.isElectron() && leptonTwo.isTau     ()) fTreeVars.category = 2;
  else if(leptonOne.isElectron() && leptonTwo.isMuon    ()) fTreeVars.category = 3;
  else if(leptonOne.isMuon    () && leptonTwo.isMuon    ()) fTreeVars.category = 4;
  else if(leptonOne.isElectron() && leptonTwo.isElectron()) fTreeVars.category = 5;
  else                    {
    std::cout << "---Warning! Entry " << fentry
              << " has undentified selection in " << __func__ << "!\n";
    fTreeVars.category = -1;
  }

  if(fReprocessMVAs) EvalMVAs("InitMVAs");
}

//--------------------------------------------------------------------------------------------------------------
// Setup the event for processes
Bool_t HistSelector::InitializeEvent(Long64_t entry)
{
  IncrementTimer("Total", false); //timer for Process method
  if(!fChain) {
    printf("HistSelector::%s: Error! TChain not found\n", __func__);
    throw 1;
  }
  fentry = entry;
  fTimes[GetTimerNumber("Reading")] = std::chrono::steady_clock::now(); //timer for reading from the tree
  // fReader.SetEntry(entry);
  if(fReader.SetLocalEntry(entry) != TTreeReader::kEntryValid) {
    printf("HistSelector::%s: Failed to load entry %lld!\n", __func__, fentry);
    throw 20;
  }

  fChain->GetTree()->GetEntry(entry);
  IncrementTimer("Reading", true);

  fTimes[GetTimerNumber("EventInit")] = std::chrono::steady_clock::now(); //timer for initializing event info
  if(fVerbose > 0 || (fTimeCounts[GetTimerNumber("Total")]-1)%fNotifyCount == 0) {
    const double dur = fDurations[GetTimerNumber("Total")]/1.e6;
    const Long64_t entries = fChain->GetEntriesFast();
    const int seen = fTimeCounts[GetTimerNumber("Total")] - 1;
    const double rate = (dur > 0.) ? seen/dur : 0.;
    const double remain = (entries - seen)/rate;
    printf("%s: Processing event: %12i (%5.1f%%), entry %12lld, overall rate = %9.1f Hz (%6.1f %s remaining)\n", __func__,
           seen,
           seen*100./entries,
           entry,
           rate,
           (rate > 0.) ? (remain > 180.) ? remain/60. : remain : 0.,
           (rate > 0.) ? (remain > 180.) ?    "min"   :   "s"  : "min"
           );
  }

  icutflow = 0;
  fCutFlow->Fill(icutflow); ++icutflow; //0

  //Filter ntuple-level details
  if(fMaxLepM > 0.f && *SelectionFilter_LepM > 0.f && *SelectionFilter_LepM > fMaxLepM) {
    IncrementTimer("EventInit", true);
    return kTRUE;
  }
  if(fMinLepM > 0.f && *SelectionFilter_LepM > 0.f && *SelectionFilter_LepM < fMinLepM) {
    IncrementTimer("EventInit", true);
    return kTRUE;
  }

  //reset the event cuts
  one_pt_min_ = 0.f; two_pt_min_ = 0.f;
  ptdiff_min_ = -1.e10f; ptdiff_max_ = +1.e10f;
  min_mass_ = 0.f; max_mass_ = -1.f;
  met_max_ = -1.f; mtone_max_ = -1.f; mttwo_max_ = -1.f; mtlep_max_ = -1.f;

  //Initialize base object information
  CountObjects(); // > 100 kHz
  if(!(mutau or etau or emu or mumu or ee)) {
    IncrementTimer("EventInit", true);
    return kTRUE;
  }

  //MC sample splitting
  if(SplitSampleEvent()) {
    IncrementTimer("EventInit", true);
    return kTRUE;
  }

  fCutFlow->Fill(icutflow); ++icutflow; //1

  ////////////////////////
  // Data overlap check //
  ////////////////////////

  //skip if it's data and lepton status doesn't match data set ( 1 = electron 2 = muon) unless allowing overlap and it passes both
  if(fIsData > 0) {
    const int pdgid = (fIsData == 1) ? 11 : 13; //pdg ID for the data stream
    //if no selected lepton fired this trigger, continue
    if(!((std::abs(leptonOne.flavor) == pdgid && leptonOne.fired) || (std::abs(leptonTwo.flavor) == pdgid && leptonTwo.fired))) {
      IncrementTimer("EventInit", true);
      return kTRUE;
    }

    if(triggerLeptonStatus != ((UInt_t) fIsData)) { //trigger doesn't match data stream
      if(triggerLeptonStatus != 3) {
        IncrementTimer("EventInit", true);
        return kTRUE; //triggered only for the other stream
      }
      if(fSkipDoubleTrigger) { //don't allow double triggers
        const int other_pdgid = (fIsData == 1) ? 13 : 11; //pdg ID for the other data stream
        //only skip if the selected lepton actually fired the trigger
        if((std::abs(leptonOne.flavor) == other_pdgid && leptonOne.fired) ||(std::abs(leptonTwo.flavor) == other_pdgid && leptonTwo.fired)) {
          IncrementTimer("EventInit", true);
          return kTRUE;
        }
      }
    }
  }

  fCutFlow->Fill(icutflow); ++icutflow; //2

  //Print debug info
  if(fVerbose > 0 ) {
    std::cout << " lep_1: pdg_id = " << leptonOne.flavor << " p4 = "; leptonOne.p4->Print();
    std::cout << " lep_2: pdg_id = " << leptonTwo.flavor << " p4 = "; leptonTwo.p4->Print();
  }

  fCutFlow->Fill(icutflow); ++icutflow; //3

  if((mutau + etau + emu + mumu + ee) > 1)
    std::cout << "WARNING! Entry " << entry << " passes multiple selections!\n";


  //////////////////////////
  // Initialize variables //
  //////////////////////////

  fTimes[GetTimerNumber("InitVars")] = std::chrono::steady_clock::now(); //timer for initializing object info
  InitializeTreeVariables();
  IncrementTimer("InitVars", true);

  /////////////////////////
  // Apply event weights //
  /////////////////////////

  fTimes[GetTimerNumber("InitWeights")] = std::chrono::steady_clock::now(); //timer for initializing object info
  InitializeEventWeights();
  IncrementTimer("InitWeights", true);

  if(eventWeight < 0. || !std::isfinite(eventWeight*(*genWeight))) {
    std::cout << "WARNING! Skipping event " << fentry << ", as it has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << *genWeight << "; puWeight = " << *puWeight
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOne.trig_wt*leptonTwo.trig_wt
              << "; jetPUIDWeight = " << jetPUIDWeight << std::endl;
    IncrementTimer("EventInit", true);
    return kTRUE;
  }

  //Initialize systematic variation weights
  InitializeSystematics();

  //configure bjet counting based on selection
  fBJetCounting = 2; //use pT > 20 GeV/c
  if(emu)       fBJetTightness = 2; //loose b-jets
  else if(etau) fBJetTightness = 0; //tight b-jets
  else if(mutau)fBJetTightness = 0; //tight b-jets
  else if(mumu) fBJetTightness = 2; //loose b-jets
  else if(ee)   fBJetTightness = 2; //loose b-jets

  //define how we're counting bjets, ID and pT threshold
  if(fBJetCounting == 0) {
    if     (fBJetTightness ==  0) nBJetsUse = nBJets;
    else if(fBJetTightness ==  1) nBJetsUse = nBJetsM;
    else if(fBJetTightness ==  2) nBJetsUse = nBJetsL;
    else if(fBJetTightness == -1) nBJetsUse = 0;
  } else if(fBJetCounting == 2) {
    if     (fBJetTightness ==  0) nBJetsUse = nBJets20;
    else if(fBJetTightness ==  1) nBJetsUse = nBJets20M;
    else if(fBJetTightness ==  2) nBJetsUse = nBJets20L;
    else if(fBJetTightness == -1) nBJetsUse = 0;
  } else {
    if(entry % fNotifyCount == 0) printf("Bad bJetUse definition, Count = %i Tight = %i!\n",
                                         fBJetCounting, fBJetTightness);
  }

  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl;

  if(!std::isfinite(eventWeight) || !std::isfinite(*genWeight)) {
    std::cout << __func__ << ": Warning!!! " << fentry << " point 1: Event weight not defined (" << eventWeight*(*genWeight) << "), setting to 0. Event weights:\n"
              << " btag = " << btagWeight << "; embedding = " << embeddingWeight << "; embed_unfold = " << embeddingUnfoldingWeight
              << "; genWeight = " << *genWeight << "; zPtWeight = " << zPtWeight << "; jetPUIDWt = " << jetPUIDWeight
              << "; trig_wt = " << leptonOne.trig_wt*leptonTwo.trig_wt << "; lep1_wt = " << leptonOne.wt1[0]*leptonOne.wt2[0]
              << "; lep2_wt = " << leptonTwo.wt1[0]*leptonTwo.wt2[0]
              << std::endl;
    eventWeight = 0.f;
    *genWeight = 1.f;
  }

  ////////////////////////////////////////////////////////////
  // Define the selection set for typical histogramming
  ////////////////////////////////////////////////////////////
  set_offset = 0;
  if(mutau)     set_offset = kMuTau;
  else if(etau) set_offset = kETau;
  else if(emu ) set_offset = kEMu;
  else if(mumu) set_offset = kMuMu;
  else if(ee  ) set_offset = kEE;

  if(!chargeTest) set_offset += fQcdOffset;
  if(looseQCDSelection) set_offset += fMisIDOffset;

  IncrementTimer("EventInit", true);

  return kFALSE;
}

//--------------------------------------------------------------------------------------------------------------
void HistSelector::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s\n",fOut->GetName());
  fTopDir->cd();
  fCutFlow->Write(); //add the cut-flow histogram to the output
  // if(fWriteTrees) {
  //   for(int i = 0; i < fn; ++i) {
  //     if(fTrees[i]) {
  //       fDirectories[3*fn + i]->cd();
  //       fTrees[i]->Write();
  //     }
  //   }
  //   fTopDir->cd();
  // }
  fOut->Write();
  fOut->Close();
  delete fOut;

  if(fPrintFilling) {
    printf("Histogram set filling information:\n");
    for(int i = 0; i < fn; ++i) {
      if(fSetFills[i] > 0) printf(" set %4i: %10i fills\n", i, fSetFills[i]);
    }
  }

  // timer->Stop();
  const Double_t cpuTime = timer->CpuTime();
  const Double_t realTime = timer->RealTime();
  if(fPrintTime) {
    printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
    if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  }
  if(fPrintTime > 1) PrintTimerInfo();
  printf("HistSelector::%s: Finished processing!\n", __func__);
}

//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t HistSelector::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: base selection
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  FillAllHistograms(set_offset + 1);

  fCutFlow->Fill(icutflow); ++icutflow; //5
  return kTRUE;
}