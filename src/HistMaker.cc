#define HISTMAKER_CXX

#include "interface/HistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
HistMaker::HistMaker(int seed, TTree * /*tree*/) : fSystematicSeed(seed),
                                                   fMuonJetToTauComp("mutau", 2035, 3, 0), fMuonJetToTauSSComp("mutau", 3035, 3, 0),
                                                   fElectronJetToTauComp("etau", 2035, 3, 0), fElectronJetToTauSSComp("etau", 3035, 3, 0),
                                                   fQCDWeight("emu", 1100200/*anti-iso, jet binned, use fits*/, 0),
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
  fSystematicShifts = nullptr;
  fRnd = nullptr;

  leptonOne.p4 = new TLorentzVector();
  leptonTwo.p4 = new TLorentzVector();
  jetOneP4     = new TLorentzVector();
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
HistMaker::~HistMaker() {
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
  if(fSystematicShifts ) {delete fSystematicShifts; fSystematicShifts = nullptr;}
  if(fRnd              ) {delete fRnd             ; fRnd              = nullptr;}
  if(leptonOne.p4      ) {delete leptonOne.p4     ; leptonOne.p4      = nullptr;}
  if(leptonTwo.p4      ) {delete leptonTwo.p4     ; leptonTwo.p4      = nullptr;}
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

  TString dir = gSystem->Getenv("CMSSW_BASE");
  if(dir == "") dir = "../"; //if not in a CMSSW environment, assume in a sub-directory of the package
  else dir += "/src/CLFVAnalysis/";
  fRoccoR = new RoccoR(Form("%sscale_factors/RoccoR%i.txt", dir.Data(), fYear));
  fElectronIDWeight.verbose_ = fVerbose;
  fMuonIDWeight.SetVerbose(fVerbose);
  fBTagWeight = new BTagWeight();
  fBTagWeight->verbose_ = fVerbose;
  fPUWeight = new PUWeight();
  fJetPUWeight = new JetPUWeight();
  fPrefireWeight = new PrefireWeight();
  fZPtWeight = new ZPtWeight("MuMu", 1, fSystematicSeed);
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
      if(fMVAConfig.names_[mva_i].Contains("higgs")) {if(!fDoHiggs) continue; else selection += "h";}
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
      const char* f = Form("weights/%s.2016_2017_2018.weights.xml",fMVAConfig.names_[mva_i].Data());
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
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOne.trig_wt*leptonTwo.trig_wt
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
    fTimes[GetTimerNumber("Filling")] = std::chrono::steady_clock::now(); //timer for reading from the tree
    FillEventHistogram( fEventHist [index]);
    FillLepHistogram(   fLepHist   [index]);
    if(fDoSystematics && fSysSets[index]) {
      fTimes[GetTimerNumber("SystFill")] = std::chrono::steady_clock::now(); //timer for reading from the tree
      FillSystematicHistogram(fSystematicHist[index]);
      IncrementTimer("SystFill", true);
    }
    // if(fFillPhotonHists) FillPhotonHistogram(fPhotonHist[index]);
    IncrementTimer("Filling", true);
  } else {
    printf("WARNING! Entry %lld: Attempted to fill un-initialized histogram set %i!\n", fentry, index);
  }
  if(fDoSystematics >= 0 && fWriteTrees && fTreeSets[index]) {
    if(fVerbose > 0) {
      printf("HistMaker::%s: Filling tree %i\n", __func__, index);
    }
    fDirectories[3*fn + index]->cd();
    fTrees[index]->Fill();
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i]  = 0;
    fSysSets[i] = 0;
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
      // Utilities::BookH1D(fEventHist[i]->hNPartons                , "npartons"                , Form("%s: NPartons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hLHENJets                , "lhenjets"                , Form("%s: LHE N(jets)"                 ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);
      Utilities::BookH1F(fEventHist[i]->hDataRun                 , "datarun"                 , Form("%s: DataRun"                     ,dirname), 100, 2.6e5, 3.3e5, folder);
      // Utilities::BookH1D(fEventHist[i]->hNPhotons                , "nphotons"                , Form("%s: NPhotons"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets                   , "njets"                   , Form("%s: NJets"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      // Utilities::BookH1D(fEventHist[i]->hNJets20Rej[0]           , "njets20rej"              , Form("%s: NJets20Rej"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNFwdJets                , "nfwdjets"                , Form("%s: NFwdJets"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);


      // Utilities::BookH1D(fEventHist[i]->hTriggerLeptonStatus, "triggerleptonstatus" , Form("%s: TriggerLeptonStatus" ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTriggered         , "ntriggered"          , Form("%s: NTriggered"          ,dirname),   5,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hPuWeight           , "puweight"            , Form("%s: PUWeight"            ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPUIDWeight      , "jetpuidweight"       , Form("%s: JetPUIDWeight"       ,dirname),  50,    0,   2, folder);
      Utilities::BookH1D(fEventHist[i]->hPrefireWeight      , "prefireweight"       , Form("%s: PrefireWeight"       ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hBTagWeight         , "btagweight"          , Form("%s: BTagWeight"          ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hZPtWeight          , "zptweight"           , Form("%s: ZPtWeight"           ,dirname),  50,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hSignalZWeight      , "signalzweight"       , Form("%s: SignalZWeight"       ,dirname),  50,    0,   2, folder);

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
      // Utilities::BookH1F(fEventHist[i]->hHtPhi              , "htphi"               , Form("%s: HtPhi"               ,dirname),  50,   -4,   4, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMetPhi              , "metphi"              , Form("%s: MetPhi"                  ,dirname)  ,  40, -4,   4, folder);
      // Utilities::BookH1F(fEventHist[i]->hMetCorr             , "metcorr"             , Form("%s: Met Correction"          ,dirname)  ,  50,  0,  25, folder);
      // Utilities::BookH1F(fEventHist[i]->hMetCorrPhi          , "metcorrphi"          , Form("%s: MetPhi Correction"       ,dirname)  ,  40, -4,   4, folder);

      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTOneOverM          , "mtoneoverm"          , Form("%s: MTOneOverM"              ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwoOverM          , "mttwooverm"          , Form("%s: MTTwoOverM"              ,dirname)  , 100, 0.,   10. , folder);
      Utilities::BookH1F(fEventHist[i]->hMTLepOverM          , "mtlepoverm"          , Form("%s: MTLepOverM"              ,dirname)  , 100, 0.,   10. , folder);

      Utilities::BookH1F(fEventHist[i]->hMetDeltaPhi         , "metdeltaphi"         , Form("%s: Met Lep Delta Phi"       ,dirname)  ,  50,  0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepOneDeltaPhi      , "leponedeltaphi"      , Form("%s: Lep One vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepTwoDeltaPhi      , "leptwodeltaphi"      , Form("%s: Lep Two vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);


      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
      // Utilities::BookH1F(fEventHist[i]->hLepPhi  , "lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  40,  -4,   4, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
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

      Utilities::BookH1F(fEventHist[i]->hZEta[0]    , "zeta"          , Form("%s: ZEta"           ,dirname)  ,  50,  -10,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepOnePt  , "zleponept"     , Form("%s: ZLepOnePt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoPt  , "zleptwopt"     , Form("%s: ZLepTwoPt"      ,dirname)  ,  60,    0, 120, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepOneEta , "zleponeeta"    , Form("%s: ZLepOneEta"     ,dirname)  ,  50, -2.5, 2.5, folder);
      Utilities::BookH1F(fEventHist[i]->hZLepTwoEta , "zleptwoeta"    , Form("%s: ZLepTwoEta"     ,dirname)  ,  50, -2.5, 2.5, folder);
      Utilities::BookH1D(fEventHist[i]->hZDecayMode , "zdecaymode"    , Form("%s: ZDecayMode"     ,dirname)  ,  20,    0,  20, folder);

      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[0], "lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[1], "lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[0], "lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);
      // Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[0]    , "deltaalpham0"     , Form("%s: Delta Alpha Mass 0"  ,dirname), 100,   0, 200, folder);
      // Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[1]    , "deltaalpham1"     , Form("%s: Delta Alpha Mass 1"  ,dirname), 100,   0, 200, folder);
      // Utilities::BookH1F(fEventHist[i]->hDeltaAlphaMColM[0], "deltaalphamcolm0" , Form("%s: MCol - Alpha Mass 0" ,dirname),  50, -50,  50, folder);
      // Utilities::BookH1F(fEventHist[i]->hDeltaAlphaMColM[1], "deltaalphamcolm1" , Form("%s: MCol - Alpha Mass 1" ,dirname),  50, -50,  50, folder);

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
      Utilities::BookH1F(fLepHist[i]->hD0Diff         , "d0diff"           , Form("%s: 2 D0 - 1 D0"  ,dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDXYDiff        , "dxydiff"          , Form("%s: 2 DXY - 1 DXY",dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDZDiff         , "dzdiff"           , Form("%s: 2 DZ - 1 DZ"  ,dirname), 100,-0.08, 0.08, folder);

      //2D distributions
      // Utilities::BookH2F(fLepHist[i]->hTwoPtVsOnePt   , "twoptvsonept"     , Form("%s: Two pT vs One pT", dirname), 30, 0, 150, 30, 0, 150, folder);
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
      fTrees[i] = new TTree(Form("tree_%i",i),Form("HistMaker TTree %i",i));
      BookBaseTree(i);
      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::BookBaseTree(Int_t index) {
  fTrees[index]->Branch("leponept"           , &fTreeVars.leponept          );
  fTrees[index]->Branch("leponeptoverm"      , &fTreeVars.leponeptoverm     );
  fTrees[index]->Branch("leptwopt"           , &fTreeVars.leptwopt          );
  fTrees[index]->Branch("leptwoptoverm"      , &fTreeVars.leptwoptoverm     );
  fTrees[index]->Branch("leptwod0"           , &fTreeVars.leptwod0          ); //FIXME: Remove
  fTrees[index]->Branch("leppt"              , &fTreeVars.leppt             );
  fTrees[index]->Branch("lepm"               , &fTreeVars.lepm              );
  fTrees[index]->Branch("lepptoverm"         , &fTreeVars.lepptoverm        );
  fTrees[index]->Branch("lepeta"             , &fTreeVars.lepeta            );
  fTrees[index]->Branch("lepdeltaeta"        , &fTreeVars.lepdeltaeta       );
  fTrees[index]->Branch("lepdeltar"          , &fTreeVars.lepdeltar         );
  fTrees[index]->Branch("lepdeltaphi"        , &fTreeVars.lepdeltaphi       );
  fTrees[index]->Branch("ptdiff"             , &fTreeVars.ptdiff            );
  fTrees[index]->Branch("ptdiffoverm"        , &fTreeVars.ptdiffoverm       );
  fTrees[index]->Branch("deltaalphaz1"       , &fTreeVars.deltaalphaz1      );
  fTrees[index]->Branch("deltaalphaz2"       , &fTreeVars.deltaalphaz2      );
  fTrees[index]->Branch("deltaalphah1"       , &fTreeVars.deltaalphah1      );
  fTrees[index]->Branch("deltaalphah2"       , &fTreeVars.deltaalphah2      );
  fTrees[index]->Branch("deltaalpham1"       , &fTreeVars.deltaalpham1      );
  fTrees[index]->Branch("deltaalpham2"       , &fTreeVars.deltaalpham2      );
  fTrees[index]->Branch("metdeltaphi"        , &fTreeVars.metdeltaphi       );
  fTrees[index]->Branch("leponedeltaphi"     , &fTreeVars.leponedeltaphi    );
  fTrees[index]->Branch("leptwodeltaphi"     , &fTreeVars.leptwodeltaphi    );
  fTrees[index]->Branch("onemetdeltaphi"     , &fTreeVars.onemetdeltaphi    );
  fTrees[index]->Branch("twometdeltaphi"     , &fTreeVars.twometdeltaphi    );
  fTrees[index]->Branch("lepmestimate"       , &fTreeVars.mestimate         );
  fTrees[index]->Branch("lepmestimatetwo"    , &fTreeVars.mestimatetwo      );
  fTrees[index]->Branch("met"                , &fTreeVars.met               );
  fTrees[index]->Branch("mtone"              , &fTreeVars.mtone             );
  fTrees[index]->Branch("mttwo"              , &fTreeVars.mttwo             );
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

  fTrees[index]->Branch("eventweight"       , &fTreeVars.eventweightMVA    );
  fTrees[index]->Branch("fulleventweight"   , &fTreeVars.fulleventweight   );
  fTrees[index]->Branch("fulleventweightlum", &fTreeVars.fulleventweightlum);
  fTrees[index]->Branch("eventcategory"     , &fTreeVars.eventcategory     );
  fTrees[index]->Branch("category"          , &fTreeVars.category          );
  fTrees[index]->Branch("train"             , &fTreeVars.train             );
  fTrees[index]->Branch("issignal"          , &fTreeVars.issignal          );
  fTrees[index]->Branch("type"              , &fTreeVars.type              );

  //FIXME: Add a few more important effects
  fTrees[index]->Branch("jettotaunonclosure", &fTreeVars.jettotaunonclosure);
  fTrees[index]->Branch("zptup"             , &fTreeVars.zptup             );
  fTrees[index]->Branch("zptdown"           , &fTreeVars.zptdown           );
  fTrees[index]->Branch("jetantimu"         , &fTreeVars.jetantimu         );
  fTrees[index]->Branch("jetantiele"        , &fTreeVars.jetantiele        );
  fTrees[index]->Branch("btagsys"           , &fTreeVars.btagsys           );
  fTrees[index]->Branch("qcdsys"            , &fTreeVars.qcdsys            );

  //add MVA score branches
  for(unsigned imva = 0; imva < fMVAConfig.names_.size(); ++imva) {
    fTrees[index]->Branch(Form("mva%i", imva), &fMvaOutputs[imva]);
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
// Initialize an event list for the tree to only process a subset of events
void HistMaker::SetEventList(TTree* tree) {
  tree->SetBranchStatus("*", 1); //ensure all potentially needed branches are active
  if(tree->GetEventList()) tree->SetEventList(nullptr); //remove any previously applied event list

  TString cut = "";
  if(fIsDY && fDYType > 0) { //Split DY into tautau and ee/mumu
    if(fDYType == 1) { //only process Z->tautau
      cut = "std::abs(GenZll_LepOne_pdgId) == 15 && std::abs(GenZll_LepTwo_pdgId) == 15";
    } else if(fDYType == 2) { //only process Z->ee/mumu
      cut = ""; //"std::abs(GenZll_LepOne_pdgId) != 15 && std::abs(GenZll_LepTwo_pdgId) != 15";
    } else {
      std::cout << "HistMaker::" << __func__
                << ": Warning! Unable to identify type of DY event to process!" << std::endl
                << "fDYType = " << fDYType << std::endl;
      return;
    }
  } else if(fWNJets > -1) { //split W+Jets into N(LHE jets) generated for binned sample combination
    cut = Form("LHE_Njets == %i", fWNJets);
  }

  if(fProcessSSSF == 0 && (fSelection == "ee" || fSelection == "mumu")) { //skip same-sign, same-flavor data
    cut = Form("%sn%s == 2 && %s_charge[0] != %s_charge[1]", (cut == "") ? "" : (cut + " && ").Data(),
               (fSelection == "ee") ? "Electron" : "Muon",
               (fSelection == "ee") ? "Electron" : "Muon",
               (fSelection == "ee") ? "Electron" : "Muon");
  }

  if(cut == "") return;
  //Attempt to set the desired event list
  tree->Draw(">>elist", cut.Data());
  auto elist = (TEventList*) gDirectory->Get("elist");
  if(!elist) {
    std::cout << "HistMaker::" << __func__ << ": Failed to generate event list!\n";
    return;
  }
  printf("HistMaker::%s: Selected event list has %i entries (%.2f%%)\n",
         __func__, elist->GetN(), elist->GetN()*100./fChain->GetEntriesFast());
  tree->SetEventList(elist);
}

//--------------------------------------------------------------------------------------------------------------
// Initialize branch structure of the input tree
void HistMaker::InitializeInputTree(TTree* tree) {
  if(fDoEventList) SetEventList(tree);

  tree->SetBranchStatus("*", 0); //turn off all branches by default, ones with addresses set are turned on
  Utilities::SetBranchAddress(tree, "run"                      , &runNumber                 );
  Utilities::SetBranchAddress(tree, "luminosityBlock"          , &lumiSection               );
  Utilities::SetBranchAddress(tree, "event"                    , &eventNumber               );

  Utilities::SetBranchAddress(tree, "nMuon"                    , &nMuon                     );
  Utilities::SetBranchAddress(tree, "nGenMuon"                 , &nGenMuons                 );
  Utilities::SetBranchAddress(tree, "Muon_pt"                  , &Muon_pt                   );
  Utilities::SetBranchAddress(tree, "Muon_eta"                 , &Muon_eta                  );
  Utilities::SetBranchAddress(tree, "Muon_phi"                 , &Muon_phi                  );
  Utilities::SetBranchAddress(tree, "Muon_charge"              , &Muon_charge               );
  Utilities::SetBranchAddress(tree, "Muon_looseId"             , &Muon_looseId              );
  Utilities::SetBranchAddress(tree, "Muon_mediumId"            , &Muon_mediumId             );
  Utilities::SetBranchAddress(tree, "Muon_tightId"             , &Muon_tightId              );
  Utilities::SetBranchAddress(tree, "Muon_pfRelIso04_all"      , &Muon_pfRelIso04_all       );
  Utilities::SetBranchAddress(tree, "Muon_dxy"                 , &Muon_dxy                  );
  Utilities::SetBranchAddress(tree, "Muon_dxyErr"              , &Muon_dxyErr               );
  Utilities::SetBranchAddress(tree, "Muon_dz"                  , &Muon_dz                   );
  Utilities::SetBranchAddress(tree, "Muon_dzErr"               , &Muon_dzErr                );
  Utilities::SetBranchAddress(tree, "Muon_nTrackerLayers"      , &Muon_nTrackerLayers       );
  Utilities::SetBranchAddress(tree, "Muon_TaggedAsRemovedByJet", &Muon_TaggedAsRemovedByJet );
  if(!fIsData) {
    Utilities::SetBranchAddress(tree, "Muon_genPartFlav"    , &Muon_genPartFlav    );
    Utilities::SetBranchAddress(tree, "Muon_genPartIdx"     , &Muon_genPartIdx     );
  }

  Utilities::SetBranchAddress(tree, "nElectron"                     , &nElectron                     );
  Utilities::SetBranchAddress(tree, "nGenElectron"                  , &nGenElectrons                 );
  Utilities::SetBranchAddress(tree, "Electron_pt"                   , &Electron_pt                   );
  Utilities::SetBranchAddress(tree, "Electron_eta"                  , &Electron_eta                  );
  Utilities::SetBranchAddress(tree, "Electron_phi"                  , &Electron_phi                  );
  Utilities::SetBranchAddress(tree, "Electron_charge"               , &Electron_charge               );
  Utilities::SetBranchAddress(tree, "Electron_deltaEtaSC"           , &Electron_deltaEtaSC           );
  Utilities::SetBranchAddress(tree, "Electron_mvaFall17V2Iso_WPL"   , &Electron_mvaFall17V2Iso_WPL   );
  Utilities::SetBranchAddress(tree, "Electron_mvaFall17V2Iso_WP90"  , &Electron_mvaFall17V2Iso_WP90  );
  Utilities::SetBranchAddress(tree, "Electron_mvaFall17V2Iso_WP80"  , &Electron_mvaFall17V2Iso_WP80  );
  Utilities::SetBranchAddress(tree, "Electron_mvaFall17V2noIso_WPL" , &Electron_mvaFall17V2noIso_WPL );
  Utilities::SetBranchAddress(tree, "Electron_mvaFall17V2noIso_WP90", &Electron_mvaFall17V2noIso_WP90);
  Utilities::SetBranchAddress(tree, "Electron_mvaFall17V2noIso_WP80", &Electron_mvaFall17V2noIso_WP80);
  Utilities::SetBranchAddress(tree, "Electron_pfRelIso03_all"       , &Electron_pfRelIso03_all       );
  Utilities::SetBranchAddress(tree, "Electron_eCorr"                , &Electron_eCorr                );
  Utilities::SetBranchAddress(tree, "Electron_dxy"                  , &Electron_dxy                  );
  Utilities::SetBranchAddress(tree, "Electron_dxyErr"               , &Electron_dxyErr               );
  Utilities::SetBranchAddress(tree, "Electron_dz"                   , &Electron_dz                   );
  Utilities::SetBranchAddress(tree, "Electron_dzErr"                , &Electron_dzErr                );
  Utilities::SetBranchAddress(tree, "Electron_TaggedAsRemovedByJet" , &Electron_TaggedAsRemovedByJet );
  if(!fIsData) {
    Utilities::SetBranchAddress(tree, "Electron_genPartFlav"          , &Electron_genPartFlav          );
    Utilities::SetBranchAddress(tree, "Electron_genPartIdx"           , &Electron_genPartIdx           );
  }

  Utilities::SetBranchAddress(tree, "nTau"                          , &nTau                          );
  Utilities::SetBranchAddress(tree, "nGenTau"                       , &nGenTaus                      );
  Utilities::SetBranchAddress(tree, "Tau_pt"                        , &Tau_pt                        );
  Utilities::SetBranchAddress(tree, "Tau_eta"                       , &Tau_eta                       );
  Utilities::SetBranchAddress(tree, "Tau_phi"                       , &Tau_phi                       );
  Utilities::SetBranchAddress(tree, "Tau_mass"                      , &Tau_mass                      );
  Utilities::SetBranchAddress(tree, "Tau_charge"                    , &Tau_charge                    );
  Utilities::SetBranchAddress(tree, "Tau_idDeepTau2017v2p1VSe"      , &Tau_idDeepTau2017v2p1VSe      );
  Utilities::SetBranchAddress(tree, "Tau_idDeepTau2017v2p1VSmu"     , &Tau_idDeepTau2017v2p1VSmu     );
  Utilities::SetBranchAddress(tree, "Tau_idDeepTau2017v2p1VSjet"    , &Tau_idDeepTau2017v2p1VSjet    );
  Utilities::SetBranchAddress(tree, "Tau_idAntiEle"                 , &Tau_idAntiEle                 );
  Utilities::SetBranchAddress(tree, "Tau_idAntiEle2018"             , &Tau_idAntiEle2018             );
  Utilities::SetBranchAddress(tree, "Tau_idAntiMu"                  , &Tau_idAntiMu                  );
  Utilities::SetBranchAddress(tree, "Tau_idMVAnewDM2017v2"          , &Tau_idAntiJet                 );
  Utilities::SetBranchAddress(tree, "Tau_decayMode"                 , &Tau_decayMode                 );
  Utilities::SetBranchAddress(tree, "Tau_idDecayMode"               , &Tau_idDecayMode               );
  Utilities::SetBranchAddress(tree, "Tau_idDecayModeNewDMs"         , &Tau_idDecayModeNewDMs         );
  Utilities::SetBranchAddress(tree, "Tau_dxy"                       , &Tau_dxy                       );
  Utilities::SetBranchAddress(tree, "Tau_dz"                        , &Tau_dz                        );
  Utilities::SetBranchAddress(tree, "Tau_leadTkPtOverTauPt"         , &Tau_leadTrkPtOverTauPt        );
  Utilities::SetBranchAddress(tree, "Tau_leadTkDeltaEta"            , &Tau_leadTrkDeltaEta           );
  Utilities::SetBranchAddress(tree, "Tau_leadTkDeltaPhi"            , &Tau_leadTrkDeltaPhi           );
  Utilities::SetBranchAddress(tree, "Tau_TaggedAsRemovedByJet"      , &Tau_TaggedAsRemovedByJet      );
  if(!fIsData) {
    Utilities::SetBranchAddress(tree, "Tau_genPartFlav"               , &Tau_genPartFlav               );
    Utilities::SetBranchAddress(tree, "Tau_genPartIdx"                , &Tau_genPartIdx                );
  }

  Utilities::SetBranchAddress(tree, "nJet"                          , &nJet                          );
  Utilities::SetBranchAddress(tree, "Jet_pt"                        , &Jet_pt                        );
  Utilities::SetBranchAddress(tree, "Jet_eta"                       , &Jet_eta                       );
  Utilities::SetBranchAddress(tree, "Jet_phi"                       , &Jet_phi                       );
  Utilities::SetBranchAddress(tree, "Jet_mass"                      , &Jet_mass                      );
  // Utilities::SetBranchAddress(tree, "Jet_charge"                    , &Jet_charge                    );
  Utilities::SetBranchAddress(tree, "Jet_jetId"                     , &Jet_jetId                     );
  Utilities::SetBranchAddress(tree, "Jet_puId"                      , &Jet_puId                      );
  // Utilities::SetBranchAddress(tree, "Jet_pfRelIso03_all"            , &Jet_pfRelIso03_al           l );
  Utilities::SetBranchAddress(tree, "Jet_btagDeepB"                 , &Jet_btagDeepB                 );
  Utilities::SetBranchAddress(tree, "Jet_btagCMVA"                  , &Jet_btagCMVA                  );
  // Utilities::SetBranchAddress(tree, "Jet_eCorr"                     , &Jet_eCorr                     );
  // Utilities::SetBranchAddress(tree, "Jet_dxy"                       , &Jet_dxy                       );
  // Utilities::SetBranchAddress(tree, "Jet_dxyErr"                    , &Jet_dxyErr                    );
  // Utilities::SetBranchAddress(tree, "Jet_dz"                        , &Jet_dz                        );
  // Utilities::SetBranchAddress(tree, "Jet_dzErr"                     , &Jet_dzErr                     );
  if(!fIsData && !fIsEmbed) {
    Utilities::SetBranchAddress(tree, "Jet_partonFlavour"             , &Jet_partonFlavour             );
  }
  Utilities::SetBranchAddress(tree, "Jet_TaggedAsRemovedByMuon"     , &Jet_TaggedAsRemovedByMuon     );
  Utilities::SetBranchAddress(tree, "Jet_TaggedAsRemovedByElectron" , &Jet_TaggedAsRemovedByElectron );
  Utilities::SetBranchAddress(tree, "Jet_TaggedAsRemovedByTau"      , &Jet_TaggedAsRemovedByTau      );

  Utilities::SetBranchAddress(tree, "nTrigObj"                      , &nTrigObj                      );
  Utilities::SetBranchAddress(tree, "TrigObj_pt"                    , &TrigObj_pt                    );
  Utilities::SetBranchAddress(tree, "TrigObj_eta"                   , &TrigObj_eta                   );
  Utilities::SetBranchAddress(tree, "TrigObj_phi"                   , &TrigObj_phi                   );
  Utilities::SetBranchAddress(tree, "TrigObj_id"                    , &TrigObj_id                    );
  Utilities::SetBranchAddress(tree, "TrigObj_filterBits"            , &TrigObj_filterBits            );

  //Z information
  if(!fIsData) {
    Utilities::SetBranchAddress(tree, "GenZll_pt"                     , &zPt                           );
    Utilities::SetBranchAddress(tree, "GenZll_mass"                   , &zMass                         );
    Utilities::SetBranchAddress(tree, "GenZll_eta"                    , &zEta                          );
    Utilities::SetBranchAddress(tree, "GenZll_LepOne_pdgId"           , &zLepOneID                     );
    Utilities::SetBranchAddress(tree, "GenZll_LepTwo_pdgId"           , &zLepTwoID                     );
    Utilities::SetBranchAddress(tree, "GenZll_LepOne_pt"              , &zLepOnePt                     );
    Utilities::SetBranchAddress(tree, "GenZll_LepTwo_pt"              , &zLepTwoPt                     );
    Utilities::SetBranchAddress(tree, "GenZll_LepOne_eta"             , &zLepOneEta                    );
    Utilities::SetBranchAddress(tree, "GenZll_LepTwo_eta"             , &zLepTwoEta                    );
    Utilities::SetBranchAddress(tree, "GenZll_LepOne_phi"             , &zLepOnePhi                    );
    Utilities::SetBranchAddress(tree, "GenZll_LepTwo_phi"             , &zLepTwoPhi                    );
    Utilities::SetBranchAddress(tree, "GenZll_LepOne_mass"            , &zLepOneMass                   );
    Utilities::SetBranchAddress(tree, "GenZll_LepTwo_mass"            , &zLepTwoMass                   );
    Utilities::SetBranchAddress(tree, "GenZll_LepOne_DecayIdx"        , &zLepOneDecayIdx               );
    Utilities::SetBranchAddress(tree, "GenZll_LepTwo_DecayIdx"        , &zLepTwoDecayIdx               );

    Utilities::SetBranchAddress(tree, "nGenPart"                      , &nGenPart                      );
    Utilities::SetBranchAddress(tree, "GenPart_pdgId"                 , &GenPart_pdgId                 );
    Utilities::SetBranchAddress(tree, "GenPart_genPartIdxMother"      , &GenPart_genPartIdxMother      );
  }

  //Event information
  if(!fIsData) {
    Utilities::SetBranchAddress(tree, "genWeight"                     , &genWeight                     );
    if(!fIsEmbed) {
      Utilities::SetBranchAddress(tree, "LHE_Njets"                   , &LHE_Njets                     );
      Utilities::SetBranchAddress(tree, "Pileup_nTrueInt"             , &nPU                           );
      Utilities::SetBranchAddress(tree, "PrefireWeight"               , &prefireWeight                 );
      Utilities::SetBranchAddress(tree, "PrefireWeight_Up"            , &prefireWeight_up              );
      Utilities::SetBranchAddress(tree, "PrefireWeight_Down"          , &prefireWeight_down            );
    }
  }
  Utilities::SetBranchAddress(tree, "PV_npvsGood"                   , &nPV                           ) ;
  // Utilities::SetBranchAddress(tree, "Pileup_nPU"                    , &nPUAdded                      );
  Utilities::SetBranchAddress(tree, "HT"                            , &ht                            );
  Utilities::SetBranchAddress(tree, "PuppiMET_pt"                   , &puppMET                       );
  Utilities::SetBranchAddress(tree, "PuppiMET_phi"                  , &puppMETphi                    );
  Utilities::SetBranchAddress(tree, "PuppiMET_ptJERUp"              , &puppMETJERUp                  );
  Utilities::SetBranchAddress(tree, "PuppiMET_ptJESUp"              , &puppMETJESUp                  );
  Utilities::SetBranchAddress(tree, "PuppiMET_phiJERUp"             , &puppMETphiJERUp               );
  Utilities::SetBranchAddress(tree, "PuppiMET_phiJESUp"             , &puppMETphiJESUp               );

  //MVA information
  if(!fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < fMVAConfig.names_.size(); ++mva_i) {
      Utilities::SetBranchAddress(tree, Form("mva%i",mva_i), &fMvaOutputs[mva_i]);
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply/replace electron energy scale corrections
void HistMaker::ApplyElectronCorrections() {
  for(UInt_t index = 0; index < nElectron; ++index) {
    float sf = (fIsEmbed) ? fElectronIDWeight.EmbedEnergyScale(Electron_pt[index], Electron_eta[index],
                                                               fYear, Electron_energyScaleUp[index], Electron_energyScaleDown[index]) : 1.;
    Electron_energyScale[index] = sf;
    if(fIsEmbed) sf /= Electron_eCorr[index]; //remove the correction applied to embedding electrons
    Electron_pt  [index] *= sf;
    // Electron_mass[index] *= sf;
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply the Rochester corrections to the muon pt for all muons
void HistMaker::ApplyMuonCorrections() {
  // if(fIsEmbed) return;
  const static int s(0), m(0); //error set and member for corrections
  for(UInt_t index = 0; index < nMuon; ++index) {
    const double u = fRnd->Uniform();
    double sf = 1.;
    if(fIsData) {
      sf = fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
    } else {
      // //For muons in embedding events not identified as the main leptons, apply typical data corrections
      // if(fIsEmbed && (abs(leptonOne.flavor) != 13 || (int) index != leptonOneIndex) && (abs(leptonTwo.flavor) != 13 || (int) index != leptonTwoIndex)) {
      //   sf = fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
      // } else {
      //For simulated muons, both in embedding and standard MC, apply given MC corrections
      sf = fRoccoR->kSmearMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], Muon_nTrackerLayers[index], u, s, m);
      //FIXME: use kSpreadMC with the gen-level muon info (see RoccoR.h:L242)
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
    const float sf = (fIsData) ? 1. : fTauIDWeight->EnergyScale(Tau_pt[index], Tau_eta[index], Tau_decayMode[index],
                                                                Tau_genPartFlav[index], Tau_idDeepTau2017v2p1VSjet[index],
                                                                fYear, Tau_energyScaleUp[index], Tau_energyScaleDown[index]);
    Tau_pt  [index] *= sf;
    Tau_mass[index] *= sf;
    Tau_energyScale[index] = sf;
  }
}

//--------------------------------------------------------------------------------------------------------------
// If the same flavor leptons are not in increasing pT order, swap them
// FIXME: Add a more generic method, e.g. a Muon/Electron object with fields, loop through fields to swap
void HistMaker::SwapSameFlavor() {
  if(nMuon == 2 && Muon_pt[0] < Muon_pt[1]) {
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
  if(nElectron == 2 && Electron_pt[0] < Electron_pt[1]) {
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
    std::swap(Electron_energyScale          [0], Electron_energyScale          [1]);
  }
}

//--------------------------------------------------------------------------------------------------------------
// Determine and apply the global event weights (i.e. not selection specific)
void HistMaker::InitializeEventWeights() {
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

  jetPUIDWeight = 1.f; btagWeight = 1.f; embeddingWeight = 1.f; embeddingUnfoldingWeight = 1.f;
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
    genWeight = std::fabs(genWeight);
    if(fIsLLEmbed) {
      embeddingWeight = 1.f;
      genWeight = 1.f;
    } else if(genWeight > 1.) { //undefined generation weight (must be < 1) --> remove the event
      genWeight = 0.f;
    } else if(genWeight == 1.f) {
      std::cout << "!!! Warning! Entry = " << fentry << ": Unit input embedding weight = " << genWeight
                << " in event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                << std::endl;
      genWeight = 0.f;
    } else { //move the weight value to the embeddingWeight variable
      embeddingWeight = genWeight; //move the embedding weight from genWeight to embeddingWeight
      genWeight = 1.f;
    }
    embeddingUnfoldingWeight = fEmbeddingWeight->UnfoldingWeight(zLepOnePt, zLepOneEta, zLepTwoPt, zLepTwoEta, zEta, zPt, fYear);
    eventWeight *= embeddingUnfoldingWeight*embeddingWeight;
  } else if(!fIsData) { //standard MC --> store only the sign of the generator weight
    genWeight = (genWeight < 0.) ? -1.f : 1.f;
  } else { //Data --> no weight
    genWeight = 1.f;
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
    puWeight   = 1.f;
    zPtWeight  = 1.f;
    btagWeight = 1.f;
  } else {
    if(fRemovePUWeights > 0) {
      puWeight = 1.f;
      //replace weight
      if(fRemovePUWeights > 1) {
        puWeight = fPUWeight->GetWeight(nPU, fYear);
      }
    }
    eventWeight *= puWeight;
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
    prefireWeight = fPrefireWeight->GetPhotonWeight(fYear, nelec, pt, eta);
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

  jetPUIDWeight = 1.f;
  if(fUseJetPUIDWeights > 0 && !fIsData && !fIsEmbed) {
    jetPUIDWeight = fJetPUWeight->GetWeight(fYear, nJets20, jetsPt, jetsEta, nJets20Rej, jetsRejPt, jetsRejEta);
  }
  eventWeight *= jetPUIDWeight;

  ////////////////////////////////////////////////////////////////////
  //   B-tag ID weights
  ////////////////////////////////////////////////////////////////////

  btagWeight = 1.f; btagWeightUp = 1.f; btagWeightDown = 1.f;
  if(fUseBTagWeights && !fIsData && !fIsEmbed) {
    //FIXME: Add a category with Tight ID b-tag in ee/mumu for Z->x+tau_h normalization?
    int wp = BTagWeight::kLooseBTag; //ee/mumu/emu use loose ID b-tag
    if(leptonTwo.isTau()) wp = BTagWeight::kTightBTag;
    btagWeight = fBTagWeight->GetWeight(wp, fYear, nJets20, jetsPt, jetsEta, jetsFlavor, jetsBTag, btagWeightUp, btagWeightDown);
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
    //Lepton 1
    if     (leptonOne.isElectron()) fEmbeddingTnPWeight.ElectronIDWeight(leptonOne.p4->Pt(), leptonOne.scEta, fYear,
                                                                         leptonOne.wt1[0], leptonOne.wt1[1], leptonOne.wt1[2],
                                                                         leptonOne.wt2[0], leptonOne.wt2[1], leptonOne.wt2[2],
                                                                         leptonOne.isLoose, mcEra);
    else if(leptonOne.isMuon    ()) fEmbeddingTnPWeight.MuonIDWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear,
                                                                     leptonOne.wt1[0], leptonOne.wt1[1], leptonOne.wt1[2],
                                                                     leptonOne.wt2[0], leptonOne.wt2[1], leptonOne.wt2[2],
                                                                     leptonOne.isLoose, mcEra);
    else if(leptonOne.isTau     ()) leptonOne.wt1[0] = fTauIDWeight->IDWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                              fYear, leptonOne.wt1[1], leptonOne.wt1[2], leptonOne.wt1_bin);
    //Lepton 2
    if     (leptonTwo.isElectron()) fEmbeddingTnPWeight.ElectronIDWeight(leptonTwo.p4->Pt(), leptonTwo.scEta, fYear,
                                                                         leptonTwo.wt1[0], leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                                         leptonTwo.wt2[0], leptonTwo.wt2[1], leptonTwo.wt2[2],
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
                                                               leptonOne.wt1[0] , leptonOne.wt1[1], leptonOne.wt1[2],
                                                               leptonOne.wt2[0] , leptonOne.wt2[1], leptonOne.wt2[2]);
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
          if(fVerbose > -1) printf("HistMaker::%s: j-->tau weight up for proc %i below nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
                                   __func__, proc, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauDecayMode, leptonOne.p4->Pt(), leptonOne.p4->Eta());
          fJetToTauWtsUp[proc] = fJetToTauWts[proc];
        }
        if(fJetToTauWtsDown[proc] > fJetToTauWts[proc]) {
          if(fVerbose > -1) printf("HistMaker::%s: j-->tau weight down for proc %i above nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
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
        if(fVerbose > -1) printf("HistMaker::%s: j-->tau weight up below nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
                                 __func__, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauDecayMode, leptonOne.p4->Pt(), leptonOne.p4->Eta());
        jetToTauWeightUp = jetToTauWeight;
      }
      if(jetToTauWeightDown > jetToTauWeight) {
        if(fVerbose > -1) printf("HistMaker::%s: j-->tau weight down above nominal, setting to nominal! tau: pt = %.2f, eta = %.2f, DM = %i; lead: pt = %.2f, eta = %.2f\n",
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
  fTreeVars.eventweight = genWeight*eventWeight;
  fTreeVars.eventweightMVA = genWeight*eventWeight;
  fTreeVars.fulleventweight = genWeight*eventWeight*fXsec;
  fTreeVars.fulleventweightlum = fTreeVars.fulleventweight*fLum;

  //initialize systematic weights for TMVA systematic testing, remove non-closure and bis corrections as an estimate
  fTreeVars.jettotaunonclosure = jetToTauWeight / jetToTauWeightBias; //weight without the non-closure or bias corrections
  fTreeVars.zptup              = zPtWeightUp / zPtWeight;
  fTreeVars.zptdown            = zPtWeightDown / zPtWeight;
  fTreeVars.jetantimu          = (leptonTwo.isTau() && std::abs(tauGenFlavor) == 13) ? leptonTwo.wt1[1] / leptonTwo.wt1[0] : 1.f;
  fTreeVars.jetantiele         = (leptonTwo.isTau() && std::abs(tauGenFlavor) == 11) ? leptonTwo.wt1[1] / leptonTwo.wt1[0] : 1.f;
  fTreeVars.qcdsys             = (qcdWeight > 0.) ? qcdWeightUp / qcdWeight : 0.f;
  fTreeVars.btagsys            = btagWeightUp / btagWeight;

  //if splitting testing/training samples
  fTreeVars.eventweightMVA *= (fTreeVars.train > 0.f) ? 0.f : 1.f/(1.f-fFractionMVA); //if training, ignore, else rescale to account for training sample removed
}

//--------------------------------------------------------------------------------------------------------------
// Set kinematics relevant to MVAs, to re-evaluate scores after shifted values etc.
void HistMaker::SetKinematics() {
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
  fTreeVars.ptdiff        = leptonOne.p4->Pt() - leptonTwo.p4->Pt();
  fTreeVars.ptdiffoverm   = fTreeVars.ptdiff / fTreeVars.lepm;

  //Tau trk variables
  TLorentzVector trk;
  if(leptonTwo.isTau()) trk.SetPtEtaPhiM(leptonTwo.trkpt, leptonTwo.trketa, leptonTwo.trkphi, TAUMASS);
  else                  trk = *leptonTwo.p4;
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
  fTreeVars.met = met;
  fTreeVars.mtone = sqrt(2.*met*leptonOne.p4->Pt()*(1.-cos(leptonOne.p4->Phi() - metPhi)));
  fTreeVars.mttwo = sqrt(2.*met*leptonTwo.p4->Pt()*(1.-cos(leptonTwo.p4->Phi() - metPhi)));
  fTreeVars.mtlep = sqrt(2.*met*lep.Pt()*(1.-cos(lep.Phi() - metPhi)));
  fTreeVars.mtoneoverm = fTreeVars.mtone / fTreeVars.lepm;
  fTreeVars.mttwooverm = fTreeVars.mttwo / fTreeVars.lepm;
  fTreeVars.mtlepoverm = fTreeVars.mtlep / fTreeVars.lepm;

  //momentum projections onto bisector
  TVector3 lp1 = leptonOne.p4->Vect();
  TVector3 lp2 = leptonTwo.p4->Vect();
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector.Mag() > 0.) bisector.SetMag(1.);

  //project onto the bisectors
  fTreeVars.pzetavis     = (lp1+lp2)*bisector;
  fTreeVars.pzetainv     = missing*bisector;
  fTreeVars.pzetaall     = (lp1 + lp2 + missing)*bisector;
  fTreeVars.dzeta        = fTreeVars.pzetaall - 0.85*fTreeVars.pzetavis;
  const double pnuest    = std::max(1.e-8,lp2*missing/lp2.Mag()); //inv pT along tau = lep 2 pt unit vector dot missing
  const double pnuesttwo = std::max(1.e-8,lp1*missing/lp1.Mag()); //inv pT along tau = lep 1 (for emu case with tau decay)
  fTreeVars.ptauvisfrac  = lp2.Mag() / (lp2.Mag() + pnuest);
  fTreeVars.mestimate    = fTreeVars.lepm/sqrt(fTreeVars.ptauvisfrac);
  fTreeVars.mestimatetwo = fTreeVars.lepm/sqrt(lp1.Mag() / (lp1.Mag() + pnuesttwo));


  //definition from (14) and (16) of arxiv:1207.4894
  const double hmass(HIGGSMASS), zmass(ZMASS), tmass(TAUMASS), lepdot(2.*((*leptonOne.p4)*(*leptonTwo.p4)));
  //delta alpha 1 = (m_boson^2 - m_tau^2) / (p(l1)\cdot p(l2))
  //delta alpha 2 = pT(l1) / pT(l2) (l1 = tau)
  //delta alpha 3 = pT(l2) / pT(l1) (l2 = tau)
  fTreeVars.alphaz1 = (zmass*zmass-tmass*tmass)/(lepdot);
  fTreeVars.alphah1 = (hmass*hmass-tmass*tmass)/(lepdot);
  fTreeVars.alpha2 = leptonTwo.p4->Pt()/leptonOne.p4->Pt(); //for lep 1 = tau, lep 2 = non-tau
  fTreeVars.alpha3 = leptonOne.p4->Pt()/leptonTwo.p4->Pt(); //for lep 2 = non-tau, lep 1 = tau
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

  //Perform a transform to the Z/H boson frame
  //Requires knowledge of which lepton has associated neutrinos (or if none do)
  //modes:
  // 0: MET defines x-axis, lepton two has positive Pz (mutau_h, etau_h,etau_mu)
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
      printf("!!! HistMaker::%s: Entry %lld : Non-finite boosted lepton p4 components!\n", __func__, fentry);
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
}

//--------------------------------------------------------------------------------------------------------------
// Count objects in the event (leptons, jets, etc.) and initialize base variables
void HistMaker::CountObjects() {

  /////////////////////////////////////
  // Check that no arrays are too small

  if(!fIsData && nGenPart > kMaxGenPart) {
    printf("HistMaker::%s: Error! Too many gen particles (%i), max allowed are %i\n", __func__, nGenPart, kMaxGenPart);
    throw 20;
  }
  if(nElectron > kMaxLeptons) {
    printf("HistMaker::%s: Error! Too many electrons (%i), max allowed are %i\n", __func__, nElectron, kMaxLeptons);
    throw 20;
  }
  if(nMuon > kMaxLeptons) {
    printf("HistMaker::%s: Error! Too many muons (%i), max allowed are %i\n", __func__, nMuon, kMaxLeptons);
    throw 20;
  }
  if(nTau > kMaxLeptons) {
    printf("HistMaker::%s: Error! Too many taus (%i), max allowed are %i\n", __func__, nTau, kMaxLeptons);
    throw 20;
  }
  if(nJet > kMaxParticles) {
    printf("HistMaker::%s: Error! Too many jets (%i), max allowed are %i\n", __func__, nJet, kMaxParticles);
    throw 20;
  }
  if(nTrigObj > kMaxTriggers) {
    printf("HistMaker::%s: Error! Too many triggers (%i), max allowed are %i\n", __func__, nTrigObj, kMaxTriggers);
    throw 20;
  }

  /////////////////////////////////////
  // Base selections
  mutau =                   nMuon == 1 && nTau == 1 && (fSelection == "" || fSelection == "mutau");
  etau  = nElectron == 1 &&               nTau == 1 && (fSelection == "" || fSelection == "etau" );
  emu   = nElectron == 1 && nMuon == 1              && (fSelection == "" || fSelection == "emu"  );
  mumu  = nElectron <  2 && nMuon == 2              && (fSelection == "" || fSelection == "mumu" );
  ee    = nElectron == 2 && nMuon <  2              && (fSelection == "" || fSelection == "ee"   );

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

  ApplyElectronCorrections();
  ApplyMuonCorrections();
  ApplyTauCorrections();
  //Re-sort muon/electron collections in the case where the corrections change the pT order
  SwapSameFlavor();

  ///////////////////////////////////////////////////////
  // Initialize lepton selection info
  ///////////////////////////////////////////////////////

  nMuons = nMuon; nElectrons = nElectron; nTaus = nTau; nPhotons = 0;

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
    leptonTwo.ES[1]     = 1.f;
    leptonTwo.ES[2]     = 1.f + 2.f*(1.f - Muon_RoccoSF[0]);
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
    leptonTwo.id3       = Tau_idAntiJet[0];
    leptonOne.ES[0]     = Electron_energyScale[0];
    leptonOne.ES[1]     = Electron_energyScaleUp[0];
    leptonOne.ES[2]     = Electron_energyScaleDown[0];
    leptonTwo.ES[0]     = Tau_energyScale[0];
    leptonTwo.ES[1]     = Tau_energyScaleUp[0];
    leptonTwo.ES[2]     = Tau_energyScaleDown[0];
    leptonTwo.trkpt     = Tau_leadTrkPtOverTauPt[0]*leptonTwo.p4->Pt();
    leptonTwo.trketa    = Tau_leadTrkDeltaEta[0] + leptonTwo.p4->Eta();
    leptonTwo.trkphi    = Tau_leadTrkDeltaPhi[0] + leptonTwo.p4->Phi();
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
    leptonTwo.id3       = Tau_idAntiJet[0];
    leptonOne.ES[0]     = Muon_RoccoSF[0];
    leptonOne.ES[1]     = 1.f;
    leptonOne.ES[2]     = 1.f + 2.f*(1.f - Muon_RoccoSF[0]);
    leptonTwo.ES[0]     = Tau_energyScale[0];
    leptonTwo.ES[1]     = Tau_energyScaleUp[0];
    leptonTwo.ES[2]     = Tau_energyScaleDown[0];
    leptonTwo.trkpt     = Tau_leadTrkPtOverTauPt[0]*leptonTwo.p4->Pt();
    leptonTwo.trketa    = Tau_leadTrkDeltaEta[0] + leptonTwo.p4->Eta();
    leptonTwo.trkphi    = Tau_leadTrkDeltaPhi[0] + leptonTwo.p4->Phi();
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
    leptonOne.ES[1]     = 1.f;
    leptonOne.ES[2]     = 1.f + 2.f*(1.f - Muon_RoccoSF[0]);
    leptonTwo.ES[0]     = Muon_RoccoSF[1];
    leptonTwo.ES[1]     = 1.f;
    leptonTwo.ES[2]     = 1.f + 2.f*(1.f - Muon_RoccoSF[1]);
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
    std::cout << "HistMaker::" << __func__ << ": Entry " << fentry
              << ": Warning! No event selection identified: N(e) = " << nElectron
              << " N(mu) = " << nMuon << " N(tau) = " << nTau << std::endl;
      return;
  }
  if(nTau > 0) { //store information of the leading pT tau
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

  //Add MET information
  met        = puppMET; //use PUPPI MET
  metPhi     = puppMETphi;
  metCorr    = 0.;
  metCorrPhi = 0.;

  if(!std::isfinite(puppMETJERUp) || !std::isfinite(puppMETphiJERUp)) {
    if(fVerbose) printf("HistMaker::%s: Entry %lld: MET JER Up not defined\n", __func__, fentry);
    puppMETJERUp    = puppMET;
    puppMETphiJERUp = puppMETphi;
  }
  if(!std::isfinite(puppMETJESUp) || !std::isfinite(puppMETphiJESUp)) {
    if(fVerbose) printf("HistMaker::%s: Entry %lld: MET JES Up not defined\n", __func__, fentry);
    puppMETJESUp    = puppMET;
    puppMETphiJESUp = puppMETphi;
  }
  //If no Z information is found, approximate it with the lepton info
  if(fIsData || zPt < 0.f || zMass < 0.f) {
    if(fIsData || zLepOnePt < 0.f || zLepTwoPt < 0.f) { //no generator-level leptons
      if((fIsEmbed || fIsDY)) {
        printf("HistMaker::%s: Entry %lld: Z information not properly stored! M(Z) = %.2f, pT(Z) = %.2f, pT(tau_1) = %.2f, pT(tau_2) = %.2f\n",
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
  //Use 1 GeV/c above muon threshold and 4 GeV/c above electron threshold (except for 2016)
  muon_trig_pt_ = 25.; electron_trig_pt_ = 34.;
  if(fYear == 2017) muon_trig_pt_ = 28.;
  if(fYear == 2016) electron_trig_pt_ = 29.; //2 GeV/c above threshold, since better behaved near threshold

  MatchTriggers();

  ///////////////////////////////////////////////////////
  // Add Jet information
  ///////////////////////////////////////////////////////

  CountJets();

  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl
                             << " eventWeight*genWeight = " << eventWeight*genWeight << std::endl;

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
    if(Jet_TaggedAsRemovedByTau[ijet]) continue; //overlapping a hadronic tau

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

//--------------------------------------------------------------------------------------------------------------
void HistMaker::InitializeTreeVariables() {
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
  } else { //no other ids for now
    fTreeVars.leptwoidone   = 0.;
    fTreeVars.leptwoidtwo   = 0.;
    fTreeVars.leptwoidthree = 0.;
  }

  //Project the MET onto the di-lepton pT system direction for recoil definitions
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  TVector3 sys_pt_dir = (*leptonOne.p4 + *leptonTwo.p4).Vect();
  sys_pt_dir.SetZ(0.);
  sys_pt_dir.SetMag(1.); //unit vector
  fTreeVars.met_u1 = missing*sys_pt_dir; //MET projected onto di-lepton system
  fTreeVars.met_u2 = (met*met - fTreeVars.met_u1*fTreeVars.met_u1)*((sys_pt_dir.DeltaPhi(missing) > 0.) ? 1. : -1.); //MET perpendicular to di-lepton system

  //event variables
  fTreeVars.njets    = nJets;
  fTreeVars.nbjets   = nBJets;
  fTreeVars.nbjetsm  = nBJetsM;
  fTreeVars.nbjetsl  = nBJetsL;
  fTreeVars.nbjets20   = nBJets20;
  fTreeVars.nbjets20m  = nBJets20M;
  fTreeVars.nbjets20l  = nBJets20L;
  fTreeVars.nphotons = nPhotons;

  //dataset/event information
  fTreeVars.eventcategory = fEventCategory;
  if(fFractionMVA > 0.) fTreeVars.train = (fRnd->Uniform() < fFractionMVA) ? 1. : -1.; //whether or not it is in the training sample
  else                  fTreeVars.train = 0.;
  fTreeVars.issignal = (fIsData == 0) ? (2.*(fIsSignal) - 1.) : 0.; //signal = 1, background = -1, data = 0
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
        printf("HistMaker::%s: Entry %lld: Unknown Z decay mode! lep_1 ID = %i, lep_2 ID = %i, lep_1_decay = %i, lep_2_decay = %i, Idx(decay 1) = %i, Idx(decay 2) = %i, nGenPart = %i\n",
               __func__, fentry, pdg_1, pdg_2, decay_1, decay_2, zLepOneDecayIdx, zLepTwoDecayIdx, nGenPart);
        if(fVerbose > 0) {
          printf("Gen-collection:\nIdx pdgId mother mother-ID\n");
          for(UInt_t npart = 0; npart < nGenPart; ++npart) {
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
      printf("HistMaker::%s: Unknown Z decay mode! lep_1 ID = %i, lep_2 ID = %i\n",
             __func__, pdg_1, pdg_2);
    }
  }
  fTreeVars.zdecaymode = zdecaymode;

  //information about the event selection
  TString selecName = "";
  if     (leptonOne.isMuon    () && leptonTwo.isTau     ()) { fTreeVars.category = 1; selecName = "mutau"; }
  else if(leptonOne.isElectron() && leptonTwo.isTau     ()) { fTreeVars.category = 2; selecName = "etau" ; }
  else if(leptonOne.isElectron() && leptonTwo.isMuon    ()) { fTreeVars.category = 3; selecName = "emu"  ; }
  else if(leptonOne.isMuon    () && leptonTwo.isMuon    ()) { fTreeVars.category = 4; selecName = "mumu" ; }
  else if(leptonOne.isElectron() && leptonTwo.isElectron()) { fTreeVars.category = 5; selecName = "ee"   ; }
  else                    {
    std::cout << "---Warning! Entry " << fentry
              << " has undentified selection in " << __func__ << "!\n";
    selecName = "unknown";
    fTreeVars.category = -1;
  }

  if(fReprocessMVAs) {
    fTimes[GetTimerNumber("MVAs")] = std::chrono::steady_clock::now(); //timer for evaluating the MVAs
    for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
      if((fMVAConfig.names_[i].Contains(selecName.Data()) || //is this selection
          (selecName == "emu" && (fMVAConfig.names_[i].Contains("_e") || fMVAConfig.names_[i].Contains("_mu"))) || //or leptonic tau category
          ((selecName == "mumu" || selecName == "ee") && fMVAConfig.names_[i].Contains("emu")) //or is emu MVA in ee/mumu region
          )
         ) {
        if(!fDoHiggs && fMVAConfig.names_[i].Contains("higgs")) {fMvaOutputs[i] = -2.f; continue;}
        fMvaOutputs[i] = mva[i]->EvaluateMVA(fMVAConfig.names_[i].Data());
      } else {
        fMvaOutputs[i] = -2.;
      }

      if(fMvaOutputs[i] < -2.1) {
        std::cout << "Error value returned for MVA " << fMVAConfig.names_[i].Data()
             << " evaluation, Entry = " << fentry << std::endl;
      }
    }
    IncrementTimer("MVAs", true);
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
  // Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
  Hist->hLHENJets            ->Fill(LHE_Njets          , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight) ;
  Hist->hDataRun             ->Fill(runNumber          , genWeight*eventWeight)      ;
  // Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  Hist->hNGenTaus            ->Fill(nGenTaus           , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons      , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons          , genWeight*eventWeight)      ;
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  // Hist->hNJets20Rej[0]       ->Fill(nJets20Rej         , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;

  // Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hNTriggered          ->Fill(leptonOne.fired+leptonTwo.fired, genWeight*eventWeight)   ;
  Hist->hPuWeight            ->Fill(puWeight)     ;
  Hist->hJetPUIDWeight       ->Fill(jetPUIDWeight);
  Hist->hPrefireWeight       ->Fill(prefireWeight);
  Hist->hBTagWeight          ->Fill(btagWeight)   ;
  Hist->hZPtWeight           ->Fill(zPtWeight)    ;
  Hist->hSignalZWeight       ->Fill(signalZWeight);

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
  // Hist->hHtPhi             ->Fill(htPhi              , genWeight*eventWeight)   ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMetPhi            ->Fill(metPhi             , genWeight*eventWeight)      ;
  // Hist->hMetCorr           ->Fill(metCorr            , genWeight*eventWeight)      ;
  // Hist->hMetCorrPhi        ->Fill(metCorrPhi         , genWeight*eventWeight)      ;

  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
  Hist->hMTOneOverM        ->Fill(fTreeVars.mtoneoverm  ,eventWeight*genWeight);
  Hist->hMTTwoOverM        ->Fill(fTreeVars.mttwooverm  ,eventWeight*genWeight);
  Hist->hMTLepOverM        ->Fill(fTreeVars.mtlepoverm  ,eventWeight*genWeight);

  TLorentzVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
  const double lepDelR   = std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4));
  const double lepDelPhi = std::fabs(leptonOne.p4->DeltaPhi(*leptonTwo.p4));
  const double lepDelEta = std::fabs(leptonOne.p4->Eta() - leptonTwo.p4->Eta());
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  Hist->hMetDeltaPhi  ->Fill(fTreeVars.metdeltaphi  ,eventWeight*genWeight);
  Hist->hLepOneDeltaPhi->Fill(fTreeVars.leponedeltaphi  ,eventWeight*genWeight);
  Hist->hLepTwoDeltaPhi->Fill(fTreeVars.leptwodeltaphi  ,eventWeight*genWeight);

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  // Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);
  Hist->hLepTrkM        ->Fill(fTreeVars.leptrkm     , eventWeight*genWeight);
  Hist->hLepTrkDeltaM   ->Fill(fTreeVars.lepm - fTreeVars.leptrkm, eventWeight*genWeight);
  Hist->hPZetaVis       ->Fill(fTreeVars.pzetavis    , eventWeight*genWeight);
  Hist->hPZetaInv       ->Fill(fTreeVars.pzetainv    , eventWeight*genWeight);
  Hist->hPZetaAll       ->Fill(fTreeVars.pzetaall    , eventWeight*genWeight);
  Hist->hPZetaDiff      ->Fill(fTreeVars.pzetainv - fTreeVars.pzetavis     , eventWeight*genWeight);
  Hist->hDZeta          ->Fill(fTreeVars.dzeta       , eventWeight*genWeight);

  Hist->hMETU1[0]       ->Fill(fTreeVars.met_u1      , eventWeight*genWeight);
  Hist->hMETU2[0]       ->Fill(fTreeVars.met_u2      , eventWeight*genWeight);

  Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR[0] ->Fill(lepDelR                ,eventWeight*genWeight);
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);

  Hist->hZEta[0]      ->Fill(zEta      , eventWeight*genWeight);
  Hist->hZLepOnePt    ->Fill(zLepOnePt , eventWeight*genWeight);
  Hist->hZLepTwoPt    ->Fill(zLepTwoPt , eventWeight*genWeight);
  Hist->hZLepOneEta   ->Fill(zLepOneEta, eventWeight*genWeight);
  Hist->hZLepTwoEta   ->Fill(zLepTwoEta, eventWeight*genWeight);
  Hist->hZDecayMode   ->Fill(fTreeVars.zdecaymode, eventWeight*genWeight);

  Hist->hPTauVisFrac  ->Fill(fTreeVars.ptauvisfrac , eventWeight*genWeight);
  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);
  // Hist->hDeltaAlphaM[0]->Fill(fTreeVars.deltaalpham1, eventWeight*genWeight);
  // Hist->hDeltaAlphaM[1]->Fill(fTreeVars.deltaalpham2, eventWeight*genWeight);
  // Hist->hDeltaAlphaMColM[0]->Fill(fTreeVars.mestimate    - fTreeVars.deltaalpham2, eventWeight*genWeight);
  // Hist->hDeltaAlphaMColM[1]->Fill(fTreeVars.mestimatetwo - fTreeVars.deltaalpham1, eventWeight*genWeight);

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
  Hist->hOnePz        ->Fill(leptonOne.p4->Pz()            ,eventWeight*genWeight);
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept           ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOne.p4->Eta()           ,eventWeight*genWeight);
  Hist->hOneSCEta     ->Fill(leptonOne.scEta               ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOne.p4->Phi()           ,eventWeight*genWeight);
  Hist->hOneD0        ->Fill(leptonOne.d0                  ,eventWeight*genWeight);
  Hist->hOneDXY       ->Fill(leptonOne.dxy                 ,eventWeight*genWeight);
  Hist->hOneDZ        ->Fill(leptonOne.dz                  ,eventWeight*genWeight);
  Hist->hOneDXYSig    ->Fill(leptonOne.dxySig              ,eventWeight*genWeight);
  Hist->hOneDZSig     ->Fill(leptonOne.dzSig               ,eventWeight*genWeight);
  Hist->hOneIso       ->Fill(leptonOne.iso                 ,eventWeight*genWeight);
  Hist->hOneID1       ->Fill(leptonOne.id1                 ,eventWeight*genWeight);
  Hist->hOneID2       ->Fill(leptonOne.id2                 ,eventWeight*genWeight);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
  Hist->hOneTrkPtOverPt->Fill(leptonOne.trkpt / leptonOne.p4->Pt() ,eventWeight*genWeight);
  Hist->hOneTrkDeltaEta->Fill(std::fabs(leptonOne.trketa - leptonOne.p4->Eta()) ,eventWeight*genWeight);
  Hist->hOneTrkDeltaPhi->Fill(std::fabs(Utilities::DeltaPhi(leptonOne.trkphi, leptonOne.p4->Phi())) ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(std::abs(leptonOne.flavor)    ,eventWeight*genWeight);
  Hist->hOneGenFlavor ->Fill(leptonOne.genFlavor           ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOne.flavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hOneJetOverlap->Fill(leptonOne.jetOverlap          ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOne.trigger             ,eventWeight*genWeight);
  Hist->hOneWeight    ->Fill(leptonOne.wt1[0]*leptonOne.wt2[0],eventWeight*genWeight);
  Hist->hOneTrigWeight->Fill(leptonOne.trig_wt          ,eventWeight*genWeight);

  Hist->hOneMetDeltaPhi   ->Fill(fTreeVars.onemetdeltaphi   ,eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwo.p4->Pz()            ,eventWeight*genWeight);
  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwo.p4->Eta()           ,eventWeight*genWeight);
  Hist->hTwoSCEta     ->Fill(leptonTwo.scEta               ,eventWeight*genWeight);
  Hist->hTwoPhi       ->Fill(leptonTwo.p4->Phi()           ,eventWeight*genWeight);
  Hist->hTwoD0        ->Fill(leptonTwo.d0                  ,eventWeight*genWeight);
  Hist->hTwoDXY       ->Fill(leptonTwo.dxy                 ,eventWeight*genWeight);
  Hist->hTwoDZ        ->Fill(leptonTwo.dz                  ,eventWeight*genWeight);
  Hist->hTwoDXYSig    ->Fill(leptonTwo.dxySig              ,eventWeight*genWeight);
  Hist->hTwoDZSig     ->Fill(leptonTwo.dzSig               ,eventWeight*genWeight);
  Hist->hTwoIso       ->Fill(leptonTwo.iso                 ,eventWeight*genWeight);
  Hist->hTwoID1       ->Fill(leptonTwo.id1                 ,eventWeight*genWeight);
  Hist->hTwoID2       ->Fill(leptonTwo.id2                 ,eventWeight*genWeight);
  Hist->hTwoID3       ->Fill(leptonTwo.id3                 ,eventWeight*genWeight);
  Hist->hTwoTrkPtOverPt->Fill(leptonTwo.trkpt / leptonTwo.p4->Pt() ,eventWeight*genWeight);
  Hist->hTwoTrkDeltaEta->Fill(std::fabs(leptonTwo.trketa - leptonTwo.p4->Eta()) ,eventWeight*genWeight);
  Hist->hTwoTrkDeltaPhi->Fill(std::fabs(Utilities::DeltaPhi(leptonTwo.trkphi, leptonTwo.p4->Phi())) ,eventWeight*genWeight);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(std::abs(leptonTwo.flavor)        ,eventWeight*genWeight);
  Hist->hTwoGenFlavor ->Fill(leptonTwo.genFlavor           ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwo.flavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hTwoJetOverlap->Fill(leptonTwo.jetOverlap          ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwo.trigger               ,eventWeight*genWeight);
  Hist->hTwoWeight    ->Fill(leptonTwo.wt1[0]*leptonTwo.wt2[0],eventWeight*genWeight);
  Hist->hTwoTrigWeight->Fill(leptonTwo.trig_wt          ,eventWeight*genWeight);

  Hist->hTwoMetDeltaPhi   ->Fill(fTreeVars.twometdeltaphi  ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);
  Hist->hD0Diff      ->Fill(leptonTwo.d0-leptonOne.d0             ,eventWeight*genWeight);
  Hist->hDXYDiff     ->Fill(leptonTwo.dxy-leptonOne.dxy           ,eventWeight*genWeight);
  Hist->hDZDiff      ->Fill(leptonTwo.dz-leptonOne.dz             ,eventWeight*genWeight);
  // Hist->hTwoPtVsOnePt->Fill(fTreeVars.leponept, fTreeVars.leptwopt,eventWeight*genWeight);
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
  bool isSameFlavor = std::abs(leptonOne.flavor) == std::abs(leptonTwo.flavor);
  bool isMuTau = leptonOne.isMuon    () && leptonTwo.isTau     ();
  bool isETau  = leptonOne.isMuon    () && leptonTwo.isTau     ();
  for(int sys = 0; sys < kMaxSystematics; ++sys) {
    float weight = eventWeight*genWeight;
    if(weight == 0.) continue; //no way to re-scale 0, contributes nothing to histograms so can just skip
    TLorentzVector lv1 = *leptonOne.p4;
    TLorentzVector lv2 = *leptonTwo.p4;
    if(sys == 0) weight = weight;                                          //do nothing
    else if  (sys ==  1) {                                                 //electron ID scale factors
      if(leptonOne.isElectron()) weight *= leptonOne.wt1[1] / leptonOne.wt1[0];
      if(leptonTwo.isElectron()) weight *= leptonTwo.wt1[1] / leptonTwo.wt1[0];
    } else if(sys ==  2) {
      if(leptonOne.isElectron()) weight *= leptonOne.wt1[2] / leptonOne.wt1[0];
      if(leptonTwo.isElectron()) weight *= leptonTwo.wt1[2] / leptonTwo.wt1[0];
    } else if(sys ==  3) {
      if(leptonOne.isElectron()) weight *= leptonOne.wt1[3] / leptonOne.wt1[0];
      if(leptonTwo.isElectron()) weight *= leptonTwo.wt1[3] / leptonTwo.wt1[0];
    } else if(sys ==  4) {                                                 //muon ID scale factors
      if(leptonOne.isMuon    ()) weight *= leptonOne.wt1[1] / leptonOne.wt1[0];
      if(leptonTwo.isMuon    ()) weight *= leptonTwo.wt1[1] / leptonTwo.wt1[0];
    } else if(sys ==  5) {
      if(leptonOne.isMuon    ()) weight *= leptonOne.wt1[2] / leptonOne.wt1[0];
      if(leptonTwo.isMuon    ()) weight *= leptonTwo.wt1[2] / leptonTwo.wt1[0];
    } else if(sys ==  6) {
      if(leptonOne.isMuon    ()) weight *= leptonOne.wt1[3]  / leptonOne.wt1[0];
      if(leptonTwo.isMuon    ()) weight *= leptonTwo.wt1[3]  / leptonTwo.wt1[0];
    } else if(sys ==  7) {                                                 //tau ID scale factors
      if(leptonOne.isTau     ()) weight *= leptonOne.wt1[1] / leptonOne.wt1[0];
      if(leptonTwo.isTau     ()) weight *= leptonTwo.wt1[1] / leptonTwo.wt1[0];
    } else if(sys ==  8) {
      if(leptonOne.isTau     ()) weight *= leptonOne.wt1[2] / leptonOne.wt1[0];
      if(leptonTwo.isTau     ()) weight *= leptonTwo.wt1[2] / leptonTwo.wt1[0];
    } else if(sys ==  9) {
      if(leptonOne.isTau     ()) weight *= leptonOne.wt1[3]  / leptonOne.wt1[0];
      if(leptonTwo.isTau     ()) weight *= leptonTwo.wt1[3]  / leptonTwo.wt1[0];
    else if  (sys == 13) weight *= zPtWeightUp          / zPtWeight      ; //Drell-Yan pT weights
    else if  (sys == 14) weight *= zPtWeightDown        / zPtWeight      ;
    else if  (sys == 15) weight *= zPtWeightSys         / zPtWeight      ;
    } else if  (sys == 45) { weight *= (btagWeight > 0.) ? btagWeightUp   / btagWeight : 1.; //btag uncertainty
    } else if  (sys == 46) { weight *= (btagWeight > 0.) ? btagWeightDown / btagWeight : 1.;
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
// Setup the event for processes
Bool_t HistMaker::InitializeEvent(Long64_t entry)
{
  IncrementTimer("Total", false); //timer for Process method
  if(!fChain) {
    printf("HistMaker::%s: Error! TChain not found\n", __func__);
    throw 1;
  }
  fentry = entry;
  fTimes[GetTimerNumber("Reading")] = std::chrono::steady_clock::now(); //timer for reading from the tree
  fChain->GetEntry(entry);
  IncrementTimer("Reading", true);

  fTimes[GetTimerNumber("EventInit")] = std::chrono::steady_clock::now(); //timer for initializing event info
  if(fVerbose > 0 || (fTimeCounts[GetTimerNumber("Total")]-1)%fNotifyCount == 0) {
    printf("%s: Processing event: %12lld (%5.1f%%) overall rate = %9.1f Hz\n", __func__, entry,
           entry*100./fChain->GetEntriesFast(),
           (fDurations[GetTimerNumber("Total")] > 0.) ? fTimeCounts[GetTimerNumber("Total")]*1.e6/fDurations[GetTimerNumber("Total")] : 0.);
  }

  icutflow = 0;
  fCutFlow->Fill(icutflow); ++icutflow; //0

  //Initialize base object information
  CountObjects(); //> 100 kHz processing speed
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
        int other_pdgid = (fIsData == 1) ? 13 : 11; //pdg ID for the other data stream
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

  if(eventWeight < 0. || !std::isfinite(eventWeight*genWeight)) {
    std::cout << "WARNING! Skipping event " << fentry << ", as it has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << genWeight << "; puWeight = " << puWeight
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
    if(entry % fNotifyCount == 0) printf("Bad bJetUse definition, Count = %i Tight = %i!\n", fBJetCounting, fBJetTightness);
  }

  if(fVerbose > 0) std::cout << " Event has selection statuses: mutau = " << mutau
                             << " etau = " << etau << " emu = " << emu
                             << " mumu = " << mumu << " and ee = " << ee << std::endl;

  if(!std::isfinite(eventWeight) || !std::isfinite(genWeight)) {
    std::cout << __func__ << ": Warning!!! " << fentry << " point 1: Event weight not defined (" << eventWeight*genWeight << "), setting to 0. Event weights:\n"
              << " btag = " << btagWeight << "; embedding = " << embeddingWeight << "; embed_unfold = " << embeddingUnfoldingWeight
              << "; genWeight = " << genWeight << "; zPtWeight = " << zPtWeight << "; jetPUIDWt = " << jetPUIDWeight
              << "; trig_wt = " << leptonOne.trig_wt*leptonTwo.trig_wt << "; lep1_wt = " << leptonOne.wt1[0]*leptonOne.wt2[0]
              << "; lep2_wt = " << leptonTwo.wt1[0]*leptonTwo.wt2[0]
              << std::endl;
    eventWeight = 0.;
    genWeight = 1.;
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

//-----------------------------------------------------------------------------------------------------------------
//Check if lepton matches to a trigger
int HistMaker::GetTriggerMatch(TLorentzVector* lv, bool isMuon, Int_t& trigIndex) {
  float pt(lv->Pt()), eta(lv->Eta()), phi(lv->Phi()), min_pt_1, min_pt_2;
  int bit_1, bit_2, id; //trigger bits to use and pdgID
  bool flag_1, flag_2;
  if(isMuon) {
    bit_1 = 1; //IsoMu24/IsoMu27
    min_pt_1 = (fYear == 2017) ? 27. : 24.; //muon_trig_pt_;
    bit_2 = bit_1; //10; //Mu50
    min_pt_2 = min_pt_1; //50.;
    id = 13;
    flag_1 = (fYear == 2017) ? HLT_IsoMu27 : HLT_IsoMu24;
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
    flag_1 = ((fYear == 2016 && HLT_Ele27_WPTight_GsF) ||
              (fYear == 2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) ||
              (fYear == 2018 && HLT_Ele32_WPTight_GsF));
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
  for(unsigned trig_i = 0; trig_i < nTrigObj; ++trig_i) {
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
void HistMaker::MatchTriggers() {
  leptonOne.fired = false; leptonTwo.fired = false;

  //FIXME: Do actual trigger matching
  const bool mu_trig  = ((fYear == 2016 && HLT_IsoMu24) ||
                        ( fYear == 2017 && HLT_IsoMu27) ||
                        ( fYear == 2018 && HLT_IsoMu24));
  const bool ele_trig = ((fYear == 2016 && HLT_Ele27_WPTight_GsF) ||
                        ( fYear == 2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) ||
                        ( fYear == 2018 && HLT_Ele32_WPTight_GsF));

  if(leptonOne.isTau     ()) {
    leptonOne.fired = false;
  } else if(leptonOne.isMuon    ()) {
    leptonOne.fired = mu_trig  && leptonOne.p4->Pt() > muon_trig_pt_     && (!fDoTriggerMatching || GetTriggerMatch(leptonOne.p4, true , leptonOne.trigger));
  } else if(leptonOne.isElectron()) {
    leptonOne.fired = ele_trig && leptonOne.p4->Pt() > electron_trig_pt_ && (!fDoTriggerMatching || GetTriggerMatch(leptonOne.p4, false, leptonOne.trigger));
  }

  if(leptonTwo.isTau     ()) {
    leptonTwo.fired = false;
  } else if(leptonTwo.isMuon    ()) {
    leptonTwo.fired = mu_trig  && leptonTwo.p4->Pt() > muon_trig_pt_     && (!fDoTriggerMatching || GetTriggerMatch(leptonTwo.p4, true , leptonTwo.trigger));
  } else if(leptonTwo.isElectron()) {
    leptonTwo.fired = ele_trig && leptonTwo.p4->Pt() > electron_trig_pt_ && (!fDoTriggerMatching || GetTriggerMatch(leptonTwo.p4, false, leptonTwo.trigger));
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
    printf("HistMaker::%s: Entry %lld: Warning! Both leptons were matched to the same trigger index!\n", __func__, fentry);
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::ApplyTriggerWeights() {
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
  leptonOne.wt1_group = 0; leptonOne.wt2_group = 0;
  leptonTwo.wt1_group = 0; leptonTwo.wt2_group = 0;
  if(leptonOne.isTau     ()) {
    int tauGenID = 0;
    //bin by flavor, not separating tau_e and e for example
    if     (std::abs(leptonOne.genFlavor) == 15) tauGenID = 5;
    else if(std::abs(leptonOne.genFlavor) == 13) tauGenID = 2;
    else if(std::abs(leptonOne.genFlavor) == 11) tauGenID = 1;
    leptonOne.wt1[3] = fSystematicShifts->TauID(fYear, tauGenID, leptonOne.wt1_bin) ? leptonOne.wt1[1] : leptonOne.wt1[2];
    leptonOne.wt2[3] = leptonOne.wt2[0]; //not defined
  }
  if(leptonTwo.isTau     ()) {
    int tauGenID = 0;
    if     (std::abs(leptonTwo.genFlavor) == 15) tauGenID = 5;
    else if(std::abs(leptonTwo.genFlavor) == 13) tauGenID = 2;
    else if(std::abs(leptonTwo.genFlavor) == 11) tauGenID = 1;
    leptonTwo.wt1[3] = fSystematicShifts->TauID(fYear, tauGenID, leptonTwo.wt1_bin) ? leptonTwo.wt1[1] : leptonTwo.wt1[2];
    leptonTwo.wt2[3] = leptonTwo.wt2[0]; //not defined
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


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t HistMaker::Process(Long64_t entry)
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
