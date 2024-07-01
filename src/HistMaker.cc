#define HISTMAKER_CXX

#include "interface/HistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
HistMaker::HistMaker(int seed, TTree * /*tree*/) : fSystematicSeed(seed),
                                                   fMuonJetToTauComp("mutau", 2035, 6, 0), fMuonJetToTauSSComp("mutau", 3035, 6, 0),
                                                   fElectronJetToTauComp("etau", 2035, 6, 0), fElectronJetToTauSSComp("etau", 3035, 6, 0),
                                                   fMuonIDWeight(1 /*use medium muon ID*/),
                                                   fElectronIDWeight(110 /*use WP90 electron ID and embed TnP trigger*/),
                                                   fZPDFSys(true),
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
  fEmbeddingResolution = nullptr;
  fRnd = nullptr;
  fMVAConfig = nullptr;
  fQCDWeight = nullptr;
  fBiasQCDWeight[0] = nullptr;
  fBiasQCDWeight[1] = nullptr;

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
  fDataset       = "dataset"; //default name
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

  if(fMVAConfig) {
    for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
      if(mva        [i]) {delete mva        [i]; mva        [i] = nullptr;}
      if(wrappedBDTs[i]) {delete wrappedBDTs[i]; wrappedBDTs[i] = nullptr;}
    }
  }
  if(fCutFlow          ) {delete fCutFlow         ; fCutFlow          = nullptr;}
  if(fBTagWeight       ) {delete fBTagWeight      ; fBTagWeight       = nullptr;}
  if(fPUWeight         ) {delete fPUWeight        ; fPUWeight         = nullptr;}
  if(fJetPUWeight      ) {delete fJetPUWeight     ; fJetPUWeight      = nullptr;}
  if(fPrefireWeight    ) {delete fPrefireWeight   ; fPrefireWeight    = nullptr;}
  if(fTauIDWeight      ) {delete fTauIDWeight     ; fTauIDWeight      = nullptr;}
  if(fRnd              ) {delete fRnd             ; fRnd              = nullptr;}
  if(fMVAConfig        ) {delete fMVAConfig       ; fMVAConfig        = nullptr;}
  if(fQCDWeight        ) {delete fQCDWeight       ; fQCDWeight        = nullptr;}
  if(fBiasQCDWeight[0] ) {delete fBiasQCDWeight[0]; fBiasQCDWeight[0] = nullptr;}
  if(fBiasQCDWeight[1] ) {delete fBiasQCDWeight[1]; fBiasQCDWeight[1] = nullptr;}
  if(fTrkQualInit      ) {delete fTrkQualInit     ; fTrkQualInit      = nullptr;}
  if(fRoccoR           ) {delete fRoccoR          ; fRoccoR           = nullptr;}
  if(leptonOne.p4      ) {delete leptonOne.p4     ; leptonOne.p4      = nullptr;}
  if(leptonTwo.p4      ) {delete leptonTwo.p4     ; leptonTwo.p4      = nullptr;}
  if(jetOne.p4         ) {delete jetOne.p4        ; jetOne.p4         = nullptr;}
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

  //0: normal unfolding; 1: use z eta unfolding; 2: use z (eta, pt) unfolding; 3: mode 2 with tight cuts; 4: 2016 unfolding; 10: IC measured unfolding
  fEmbeddingWeight = new EmbeddingWeight(10, fVerbose);

  if(fIsEmbed)
    fEmbeddingResolution = new EmbeddingResolution(fYear, fVerbose);

  fRoccoR = new RoccoR(Form("%sscale_factors/RoccoR%i.txt", dir.Data(), fYear));
  fElectronIDWeight.verbose_ = fVerbose;
  fMuonIDWeight.SetVerbose(fVerbose);
  fBTagWeight = new BTagWeight();
  fBTagWeight->verbose_ = fVerbose;
  fPUWeight = new PUWeight();
  fJetPUWeight = new JetPUWeight();
  fPrefireWeight = new PrefireWeight();
  fZPtWeight = new ZPtWeight("MuMu", 1);
  fCutFlow = new TH1D("hcutflow", "Cut-flow", 100, 0, 100);
  const int qcd_weight_mode = (fSelection == "mutau_e") ? 211100201 : (fSelection == "etau_mu") ? 221100201 : 21100201; //use Run 2 closure/bias, (mass, bdt score) bias, anti-iso, jet binned, 2D pt closure, use fits
  //Ensure the qcd_weight_mode matches the setup in QCDHistMaker
  fQCDWeight = new QCDWeight("emu", qcd_weight_mode, fYear, fVerbose);

  //initialize bias region scales
  fBiasQCDWeight[0] = new QCDWeight("emu_Bias_1", qcd_weight_mode, fYear, fVerbose); //loose electron, loose muon region
  // fBiasQCDWeight[1] = new QCDWeight("emu_Bias_2", qcd_weight_mode, fYear, fVerbose); //loose electron, tight muon region, not currently used


  //FIXME: Added back W+Jets MC bias (Mode = 10100300, 60100300 for only MC lepm bias shape)
  //FIXME: Decide on 2D (lepm, bdt score) bias (bias mode 7 (shape+rate) and 8 (shape-only))
  const int wJetsMode = fWJetsMCBiasMode*10000000 + 1300300; //1300300 = j-->tau fits + errors; NC: onept, dphi(one, met), and tau |eta|
  const int wJetsMCMode = ((fWJetsMCBiasMode == 8) ? 7 : fWJetsMCBiasMode)*10000000 + 1300300; //include the rate correction in the MC sets to see the final corrected histograms
  const int jetToTauYear = fJetToTauYearMode*1000 + fYear; //Year Mode: 0: By-year factors; 1: Run 2 scale factors; 2: By-year base, Run 2 NC/bias
  if(fSelection == "" || fSelection == "mutau" || fSelection == "mumu") {
    fMuonJetToTauWeights      [JetToTauComposition::kWJets] = new JetToTauWeight("MuonWJets"      , "mutau", "WJets",   31,wJetsMode, jetToTauYear, fVerbose);
    fMuonJetToTauWeights      [JetToTauComposition::kZJets] = new JetToTauWeight("MuonZJets"      , "mutau", "WJets",   31,wJetsMode, jetToTauYear, fVerbose);
    fMuonJetToTauWeights      [JetToTauComposition::kTop  ] = new JetToTauWeight("MuonTop"        , "mutau", "Top"  ,   82, 70100301, jetToTauYear, fVerbose);
    fMuonJetToTauWeights      [JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonQCD"        , "mutau", "QCD"  , 1030, 91300300, jetToTauYear, fVerbose);

    fMuonJetToTauMCWeights    [JetToTauComposition::kWJets] = new JetToTauWeight("MuonMCWJets"    , "mutau", "WJets",   88,wJetsMCMode+1, jetToTauYear, fVerbose); //MC weights, non-closure, bias
    fMuonJetToTauMCWeights    [JetToTauComposition::kZJets] = new JetToTauWeight("MuonMCZJets"    , "mutau", "WJets",   88,wJetsMCMode+1, jetToTauYear, fVerbose);
    fMuonJetToTauMCWeights    [JetToTauComposition::kTop  ] = new JetToTauWeight("MuonMCTop"      , "mutau", "Top"  ,   82, 70100301  , jetToTauYear, fVerbose); //Normal weights
    fMuonJetToTauMCWeights    [JetToTauComposition::kQCD  ] = new JetToTauWeight("MuonMCQCD"      , "mutau", "QCD"  , 1095, 71300300  , jetToTauYear, fVerbose); //high iso weights for SS bias
  }

  //FIXME: Added back W+Jets MC bias (Mode = 10300300, 60300300 for only MC bias shape)
  if(fSelection == "" || fSelection == "etau" || fSelection == "ee") {
    fElectronJetToTauWeights  [JetToTauComposition::kWJets] = new JetToTauWeight("ElectronWJets"  , "etau" , "WJets",   31,wJetsMode, jetToTauYear, fVerbose);
    fElectronJetToTauWeights  [JetToTauComposition::kZJets] = new JetToTauWeight("ElectronZJets"  , "etau" , "WJets",   31,wJetsMode, jetToTauYear, fVerbose);
    fElectronJetToTauWeights  [JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronTop"    , "etau" , "Top"  ,   82, 70300301, jetToTauYear, fVerbose);
    fElectronJetToTauWeights  [JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronQCD"    , "etau" , "QCD"  , 1030, 91300300, jetToTauYear, fVerbose);

    fElectronJetToTauMCWeights[JetToTauComposition::kWJets] = new JetToTauWeight("ElectronMCWJets", "etau" , "WJets",   88,wJetsMCMode+1, jetToTauYear, fVerbose); //MC weights, non-closure, bias
    fElectronJetToTauMCWeights[JetToTauComposition::kZJets] = new JetToTauWeight("ElectronMCZJets", "etau" , "WJets",   88,wJetsMCMode+1, jetToTauYear, fVerbose);
    fElectronJetToTauMCWeights[JetToTauComposition::kTop  ] = new JetToTauWeight("ElectronMCTop"  , "etau" , "Top"  ,   82, 70300301  , jetToTauYear, fVerbose); //Normal weights
    fElectronJetToTauMCWeights[JetToTauComposition::kQCD  ] = new JetToTauWeight("ElectronMCQCD"  , "etau" , "QCD"  , 1095, 71300300  , jetToTauYear, fVerbose); //high iso weights for SS bias
  }

  fTauIDWeight = new TauIDWeight((fETauAntiEleCut <= 63) ? 0 : (fETauAntiEleCut <= 127) ? 1 : 2, //which etau anti-ele ID WP is being used, Tight, VTight, or VVTight
                                 (fIsEmbed) ? fEmbedUseMCTauID+1 : 0, //whether or not to use the Embedding corrections
                                 (fSelection == "etau" || fSelection == "mutau") ? fSelection : "etau", fVerbose); //default to etau IDs


  for(int itrig = 0; itrig < 3; ++itrig) triggerWeights[itrig] = 1.f;


  fRnd = new TRandom3(fRndSeed);
  fMVAConfig = new MVAConfig(fUseCDFBDTs, fUseCDFTailFits, fUseXGBoostBDT);
  fTreeVars.type = -1;

  fMigrationBuffer = (fAllowMigration) ? 1.f : 0.f; //FIXME: Settle on an event migration buffer allowance

  //Initialize MVA tools
  InitializeMVAs();

  //Create the output file
  fOut = new TFile(GetOutputName(), "RECREATE","HistMaker output histogram file");
  fTopDir = fOut->mkdir("Data");
  fTopDir->cd();
  const TString outname = GetOutputName();
  std::cout << "Using output filename " << outname.Data() << std::endl;

  for(int ihist = 0; ihist < fn; ++ihist) {
    fEventHist     [ihist] = nullptr;
    fLepHist       [ihist] = nullptr;
    fPhotonHist    [ihist] = nullptr;
    fSystematicHist[ihist] = nullptr;
  }

  lep_tau = fSelection.EndsWith("tau_e") + 2*fSelection.EndsWith("tau_mu");

  //Add dataset type flags
  fIsHiggsBkg = 0;
  if     (outname.Contains("ggFH-"   )) fIsHiggsBkg = 1;
  else if(outname.Contains("VBFH-"   )) fIsHiggsBkg = 2;
  else if(outname.Contains("WplusH-" )) fIsHiggsBkg = 3;
  else if(outname.Contains("WminusH-")) fIsHiggsBkg = 4;
  else if(outname.Contains("ZH-"     )) fIsHiggsBkg = 5;
  fIsWJets = outname.Contains("_Wlnu") || outname.Contains("_WGamma"); //combine W+j and W+gamma
  fIsWWW   = outname.Contains("_WW");
  fIsWW    = outname.Contains("_WW") && !fIsWWW;
  fIsTTbar = outname.Contains("_ttbar");

  //Check that flags agree with the available branches in the tree
  if(fChain) {
    if(fUseBTagWeights == 2 && !fChain->GetBranch("Jet_btagSF_deepcsv_L_wt")) {
      printf("HistMaker::%s: Warning! B-tag SF weights not available in tree, will re-calculate on the fly\n", __func__);
      fUseBTagWeights = 1;
    }
    if(fUseRoccoCorr == 2 && !fChain->GetBranch("Muon_corrected_pt")) {
      printf("HistMaker::%s: Warning! Rochester corrections not available in tree, will re-calculate on the fly\n", __func__);
      fUseRoccoCorr = 1;
    }
    if(fUseJetPUIDWeights == 2 && !fChain->GetBranch("JetPUIDWeight")) {
      printf("HistMaker::%s: Warning! Jet PU ID weight not available in tree, will re-calculate on the fly\n", __func__);
      fUseJetPUIDWeights = 1;
    }
    if(fUseSignalZWeights == 2 && !fChain->GetBranch("SignalpTWeight")) {
      printf("HistMaker::%s: Warning! Signal Z pT weight not available in tree, will re-calculate on the fly\n", __func__);
      fUseSignalZWeights = 1;
    }
    if(fUseZPtWeight == 2 && !fChain->GetBranch("ZpTWeight")) {
      printf("HistMaker::%s: Warning! Z pT weight not available in tree, will re-calculate on the fly\n", __func__);
      fUseZPtWeight = 1;
    }
    if(!fIsData && !fIsEmbed && fApplyJERCorrections && (!fChain->GetBranch("Jet_pt_nom") || !fChain->GetBranch("Jet_mass_nom"))) {
      printf("HistMaker::%s: Warning! JER corrections not available in tree, will ignore\n", __func__);
      fApplyJERCorrections = 0;
    }
    if(fUseRandomField == 1 && !fChain->GetBranch("RandomField")) {
      printf("HistMaker::%s: Warning! Random Field not in the tree, will evaluate random numbers locally\n", __func__);
      fUseRandomField = 0;
    }
  } else {
    std::cout << "HistMaker::" << __func__ << ": TChain isn't defined!\n";
  }


  //Turn off weights if not requested
  if(fRemoveEventWeights > 0) {
    fUseSignalZWeights = 0;
    fUseSignalZMixWeights = 0;
    fUseZPtWeight = 0;
    fRemovePUWeights = 1;
    fUsePrefireWeights = 0;
    fUseJetPUIDWeights = 0;
    fUseBTagWeights = 0;
    fUseLepDisplacementWeights = 0;
  }

  //Initialize some weights to 1
  for(int i = 0; i < kMaxTheory; ++i) {
    PSWeight      [i] = 1.f;
    LHEPdfWeight  [i] = 1.f;
    LHEScaleWeight[i] = 1.f;
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
void HistMaker::InitializeMVAs() {
  TMVA::Tools::Instance(); //load the TMVA library
  for(int i = 0; i < kMaxMVAs; ++i) mva        [i] = nullptr; //initially 0s
  for(int i = 0; i < kMaxMVAs; ++i) wrappedBDTs[i] = nullptr; //initially 0s

  if(fReprocessMVAs) {
    if(fVerbose) printf("HistMaker::%s: Initializing MVAs, test tag = %s\n", __func__, fTestMVA.Data());
    for(unsigned mva_i = 0; mva_i < fMVAConfig->names_.size(); ++mva_i) {
      //only initialize necessary MVAs
      bool init = false;
      const TString mva_selection = fMVAConfig->GetSelectionByIndex(mva_i);
      //Check if testing this MVA
      init |= fTestMVA != "" && fTestMVA == mva_selection;
      if(fTestMVA == "" && (fSelection == "ee" || fSelection == "mumu")) { //use the emu selection for same flavor if not testing an MVA
        init |= fDoHiggs && mva_selection == "hemu";
        init |= mva_selection == "zemu";
      }
      if(fTestMVA == "qcd_histmaker") { //initialize BDTs for QCDHistMaker closure checks
        init |= mva_selection == "zetau_mu" || mva_selection == "zmutau_e";
      } else { //add BDTs that match the given selection
        init |= fDoHiggs && mva_selection == ("h" + fSelection);
        init |= mva_selection == ("z" + fSelection);
      }

      //skip this MVA if not selected
      if(!init) continue;

      if((fUseXGBoostBDT && mva_selection == "zemu") || fUseXGBoostBDT > 9) { //1-9: assume TMVA in tau channels; >= 10: XGBoost in all channels
        if(!fTrkQualInit) fTrkQualInit = new TrkQualInit(fTrkQualVersion, -1, fUseXGBoostBDT);
        mva        [mva_i] = nullptr;
        const char* model = Form("weights/%s.2016_2017_2018.json", fMVAConfig->names_[mva_i].Data());
        wrappedBDTs[mva_i] = new BDTWrapper(model, 1, fVerbose);
        if(wrappedBDTs[mva_i]->GetStatus()) throw std::runtime_error(Form("HistMaker::%s: Failed to initialize XGBoost model %s\n", __func__, model));
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
void HistMaker::FillAllHistograms(Int_t index) {

  if(eventWeight < 0. || !std::isfinite(eventWeight*genWeight)) {
    std::cout << "WARNING! Event " << fentry << ": Set " << index << " has negative bare event weight or undefined total weight:\n"
              << " eventWeight = " << eventWeight << "; genWeight = " << genWeight << "; puWeight = " << puWeight
              << "; btagWeight = " << btagWeight << "; triggerWeight = " << leptonOne.trig_wt*leptonTwo.trig_wt
              << "; jetPUIDWeight = " << jetPUIDWeight << "; zPtWeight = " << zPtWeight
              << std::endl;
  }
  if(!std::isfinite(eventWeight*genWeight)) {
    if(fVerbose > 0) printf("Warning! Event %lld: Event weight non-finite. wt = %f, gen = %f\n", fentry, eventWeight, genWeight);
    eventWeight = 0.;
    genWeight = 1.;
  }
  //check the event passes the correct cuts
  const bool pass = PassesCuts();
  if(fEventSets [index]) {
    if(fVerbose > 0) std::cout << "Filling histograms for set " << index
                               << " with event weight = " << eventWeight
                               << " and gen weight = " << genWeight << " !\n";
    fTimes[GetTimerNumber("Filling")] = std::chrono::steady_clock::now(); //timer for reading from the tree

    if(pass) {
      if(fFollowHistSet == index) {
        printf(" Entry %10lld (event %10lld): Filling histogram set %i\n", fentry, eventNumber, index);
        if(true) { //FIXME: Set this to be a flag
          printf("  lep_1  : pt = %5.1f, eta = %5.2f, mt = %5.1f, flavor = %3i, trig_eff: data = %.3f, mc = %.3f\n",
                 leptonOne.pt, leptonOne.eta, fTreeVars.mtone, leptonOne.flavor, leptonOne.trig_data_eff, leptonOne.trig_mc_eff);
          printf("  lep_2  : pt = %5.1f, eta = %5.2f, mt = %5.1f, flavor = %3i, trig_eff: data = %.3f, mc = %.3f\n",
                 leptonTwo.pt, leptonTwo.eta, fTreeVars.mttwo, leptonTwo.flavor, leptonTwo.trig_data_eff, leptonTwo.trig_mc_eff);
          printf("  dilep  : pt = %5.1f, eta = %5.2f, mt = %5.1f\n", fTreeVars.leppt, fTreeVars.lepeta, fTreeVars.mtlep);
          printf("  event  : met = %5.1f, met_sig = %5.2f, njet = %2i, nbjet = %2i\n", met, metSignificance, nJets20, nBJetsUse);
          if(fIsDY || fIsEmbed) { //print gen-level Z info
            printf("  gen-Z  : mass = %5.1f, pt = %5.2f, eta = %5.2f\n", zMass, zPt, zEta);
            printf("   -> l1 : ID = %3.0f, pt = %5.2f, eta = %5.2f\n", zLepOneID, zLepOnePt, zLepOneEta);
            printf("   -> l2 : ID = %3.0f, pt = %5.2f, eta = %5.2f\n", zLepTwoID, zLepTwoPt, zLepTwoEta);
          }
          printf("  weights: tot = %.3f, pu = %.3f, btag = %.3f, trig = %.3f, jetPUID = %.3f, zPt = %.3f, sig = %.3f, sigMix = %.3f, dxyz = %.3f\n",
                 eventWeight*genWeight, puWeight, btagWeight, leptonOne.trig_wt*leptonTwo.trig_wt, jetPUIDWeight,
                 zPtWeight, signalZWeight, signalZMixingWeight, lepDisplacementWeight);
        }
      }
      ++fSetFills[index];
      FillEventHistogram( fEventHist [index]);
      FillLepHistogram  ( fLepHist   [index]);
    }
    //allow events outside the window to the systematics filling, to account for shifted events migrating in/out
    if(fDoSystematics && fSysSets[index]) {
      if((fDoSSSystematics || chargeTest) && //skip same-sign if not relevant
         (fDoLooseSystematics || !looseQCDSelection) && //skip loose ID if not relevant
         (eventWeight*genWeight != 0.) //no way to re-scale 0, contributes nothing to histograms so can just skip filling
         ) {
        fTimes[GetTimerNumber("SystFill")] = std::chrono::steady_clock::now(); //timer for reading from the tree
        FillSystematicHistogram(fSystematicHist[index]);
        IncrementTimer("SystFill", true);
      }
    }
    // if(fFillPhotonHists) FillPhotonHistogram(fPhotonHist[index]);
    IncrementTimer("Filling", true);
  } else {
    printf("ERROR! Entry %lld: Attempted to fill un-initialized histogram set %i!\n", fentry, index);
    throw 2;
  }
  if(pass && fDoSystematics >= 0 && fWriteTrees && fTreeSets[index]) {
    if(fVerbose > 0) {
      printf("HistMaker::%s: Filling tree %i\n", __func__, index);
    }
    fDirectories[3*fn + index]->cd();
    if(fSmearTreeValues) SmearTreeValues(); //set the kinematics to smeared values
    fTrees[index]->Fill();
    if(fSmearTreeValues) SetKinematics(); //restore the kinematics after smearing
  }
}

//--------------------------------------------------------------------------------------------------------------
void HistMaker::InitHistogramFlags() {
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
    Utilities::BookH1F(fEventHist[i]->hNPU[0]                  , "npu"                     , Form("%s: NPU"                         ,dirname),  50,    0, 100, folder);
    Utilities::BookH1D(fEventHist[i]->hLHENJets                , "lhenjets"                , Form("%s: LHE N(jets)"                 ,dirname),  10,    0,  10, folder);
  }
  Utilities::BookH1F(fEventHist[i]->hNPV[0]                  , "npv"                     , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
  if(fUseEmbBDTUncertainty) {
    Utilities::BookH1F(fEventHist[i]->hEmbBDTWeight            , "embbdtweight"            , Form("%s: EmbBDTWeight"                ,dirname),  20, 0.75,1.25, folder);
  }
  Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);
  Utilities::BookH1D(fEventHist[i]->hRunEra                  , "runera"                  , Form("%s: Run era"                     ,dirname),  20,    0,  20, folder);
  Utilities::BookH1F(fEventHist[i]->hDataRun                 , "datarun"                 , Form("%s: DataRun"                     ,dirname), 100, 2.6e5, 3.3e5, folder);
  // Utilities::BookH1D(fEventHist[i]->hNPhotons                , "nphotons"                , Form("%s: NPhotons"                    ,dirname),  10,    0,  10, folder);
  Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
  Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
  Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);
  Utilities::BookH1D(fEventHist[i]->hNGenPhotons             , "ngenphotons"             , Form("%s: NGenPhotons"                 ,dirname),  10,    0,  10, folder);
  Utilities::BookH1D(fEventHist[i]->hNGenHardPhotons         , "ngenhardphotons"         , Form("%s: NGenHardPhotons"             ,dirname),  10,    0,  10, folder);
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
    Utilities::BookH1F(fEventHist[i]->hBTagWeight         , "btagweight"          , Form("%s: BTagWeight"          ,dirname), 100,    0,   5, folder);
    Utilities::BookH1F(fEventHist[i]->hZPtWeight          , "zptweight"           , Form("%s: ZPtWeight"           ,dirname),  50,    0,   2, folder);
    Utilities::BookH1F(fEventHist[i]->hSignalZWeight      , "signalzweight"       , Form("%s: SignalZWeight"       ,dirname),  50,    0,   2, folder);
    Utilities::BookH1F(fEventHist[i]->hSignalZMixWeight   , "signalzmixweight"    , Form("%s: SignalZMixWeight"    ,dirname),  50,    0,   2, folder);
    Utilities::BookH1F(fEventHist[i]->hLepDisplacementWeight, "lepdisplacementweight", Form("%s: LepDisplacementWeight",dirname),  50,    0,   2, folder);
    Utilities::BookH1F(fEventHist[i]->hPSSys              , "pssys"               , Form("%s: PSSys"               ,dirname),  50,    0,   4, folder);
    Utilities::BookH1F(fEventHist[i]->hPDFSys             , "pdfsys"              , Form("%s: PDFSys"              ,dirname),  50,    0,   4, folder);
    Utilities::BookH1F(fEventHist[i]->hScaleRSys          , "scalersys"           , Form("%s: ScaleRSys"           ,dirname),  50,    0,   4, folder);
    Utilities::BookH1F(fEventHist[i]->hScaleFSys          , "scalefsys"           , Form("%s: ScaleFSys"           ,dirname),  50,    0,   4, folder);

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
  Utilities::BookH1F(fEventHist[i]->hMetSignificance     , "metsignificance"     , Form("%s: Met Significance"        ,dirname)  ,  40,  0,  10, folder);
  Utilities::BookH1F(fEventHist[i]->hPuppiMetSig         , "puppimetsig"         , Form("%s: Met Significance"        ,dirname)  ,  50,  0,  15, folder);
  Utilities::BookH1F(fEventHist[i]->hMetPhi              , "metphi"              , Form("%s: MetPhi"                  ,dirname)  ,  40, -4,   4, folder);
  Utilities::BookH1F(fEventHist[i]->hMetCorr             , "metcorr"             , Form("%s: Met Correction"          ,dirname)  ,  40,  0,   5, folder);
  Utilities::BookH1F(fEventHist[i]->hMetUp               , "metup"               , Form("%s: Met up"                  ,dirname)  , 100,  0, 200, folder);
  Utilities::BookH1F(fEventHist[i]->hMetDown             , "metdown"             , Form("%s: Met down"                ,dirname)  , 100,  0, 200, folder);
  Utilities::BookH1F(fEventHist[i]->hMetNoCorr           , "metnocorr"           , Form("%s: Met No Correction"       ,dirname)  , 100,  0, 200, folder);
  Utilities::BookH1F(fEventHist[i]->hMetOverLepPt        , "metoverleppt"        , Form("%s: Met Over LepPt"          ,dirname)  ,  40,  0,  20, folder);
  Utilities::BookH1F(fEventHist[i]->hMetOverM            , "metoverm"            , Form("%s: Met Over M"              ,dirname)  ,  30,  0,   2, folder);
  Utilities::BookH1F(fEventHist[i]->hRawMet              , "rawmet"              , Form("%s: Raw Met"                 ,dirname)  ,  20,  0,  80, folder);
  Utilities::BookH1F(fEventHist[i]->hRawMetPhi           , "rawmetphi"           , Form("%s: Raw Met phi"             ,dirname)  ,  20,-3.2,3.2, folder);
  Utilities::BookH1F(fEventHist[i]->hRawMetDiff          , "rawmetdiff"          , Form("%s: Raw Met - Met"           ,dirname)  ,  20,-10., 10, folder);

  Utilities::BookH1F(fEventHist[i]->hNuPt                , "nupt"                , Form("%s: Nu pT"                   ,dirname)  ,  50,  0, 100, folder);
  Utilities::BookH1F(fEventHist[i]->hDetectorMet         , "detectormet"         , Form("%s: Detector Met"            ,dirname)  , 100,  0, 200, folder);

  Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTOneUp             , "mtoneup"             , Form("%s: MTOne up"                ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTOneDown           , "mtonedown"           , Form("%s: MTOne down"              ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTTwoUp             , "mttwoup"             , Form("%s: MTTwo up"                ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTTwoDown           , "mttwodown"           , Form("%s: MTTwo down"              ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTDiff              , "mtdiff"              , Form("%s: MTDiff"                  ,dirname)  ,  40, -80.,  80., folder);
  Utilities::BookH1F(fEventHist[i]->hMTRatio             , "mtratio"             , Form("%s: MTRatio"                 ,dirname)  ,  40, 0.,     5., folder);
  Utilities::BookH1F(fEventHist[i]->hMTLead              , "mtlead"              , Form("%s: MTLead"                  ,dirname)  ,  20, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTTrail             , "mttrail"             , Form("%s: MTTrail"                 ,dirname)  ,  20, 0.,   150., folder);
  Utilities::BookH1F(fEventHist[i]->hMTOneOverM          , "mtoneoverm"          , Form("%s: MTOneOverM"              ,dirname)  , 100, 0.,   10. , folder);
  Utilities::BookH1F(fEventHist[i]->hMTTwoOverM          , "mttwooverm"          , Form("%s: MTTwoOverM"              ,dirname)  , 100, 0.,   10. , folder);
  Utilities::BookH1F(fEventHist[i]->hMTLepOverM          , "mtlepoverm"          , Form("%s: MTLepOverM"              ,dirname)  , 100, 0.,   10. , folder);
  Utilities::BookH1F(fEventHist[i]->hMETDotOne           , "metdotone"           , Form("%s: METDotOne"               ,dirname)  ,  50,-70.,   70. , folder);
  Utilities::BookH1F(fEventHist[i]->hMETDotTwo           , "metdottwo"           , Form("%s: METDotTwo"               ,dirname)  ,  50,-70.,   70. , folder);
  Utilities::BookH1F(fEventHist[i]->hMETDotZOne          , "metdotzone"          , Form("%s: METDotZOne"              ,dirname)  ,  50,-70.,   70. , folder);
  Utilities::BookH1F(fEventHist[i]->hMETDotZTwo          , "metdotztwo"          , Form("%s: METDotZTwo"              ,dirname)  ,  50,-70.,   70. , folder);

  Utilities::BookH1F(fEventHist[i]->hMetDeltaPhi         , "metdeltaphi"         , Form("%s: Met Lep Delta Phi"       ,dirname)  ,  50,  0,   5, folder);

  if(!fSparseHists) {
    Utilities::BookH1F(fEventHist[i]->hLepOneDeltaPhi      , "leponedeltaphi"      , Form("%s: Lep One vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);
    Utilities::BookH1F(fEventHist[i]->hLepTwoDeltaPhi      , "leptwodeltaphi"      , Form("%s: Lep Two vs Sys Delta Phi",dirname)  ,  50,  0,   5, folder);
  }


  Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
  Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
  Utilities::BookH1F(fEventHist[i]->hLepMUp  , "lepmup"        , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
  Utilities::BookH1F(fEventHist[i]->hLepMDown, "lepmdown"      , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
  Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
  Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
  Utilities::BookH1F(fEventHist[i]->hLepPhi  , "lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  40, -3.2, 3.2, folder);
  Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,150,0.,  300, folder);
  Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,150,0.,  300, folder);
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
  Utilities::BookH1F(fEventHist[i]->hZLepOneELoss, "zleponeeloss" , Form("%s: ZLepOneELoss"   ,dirname)  , 100,    0, 1.0, folder);
  Utilities::BookH1F(fEventHist[i]->hZLepTwoELoss, "zleptwoeloss" , Form("%s: ZLepTwoELoss"   ,dirname)  , 100,    0, 1.0, folder);
  Utilities::BookH1D(fEventHist[i]->hZDecayMode , "zdecaymode"    , Form("%s: ZDecayMode"     ,dirname)  ,  20,    0,  20, folder);

  if(!fSparseHists || fSelection == "emu") {
    if(fUseCDFBDTs || fUseXGBoostBDT) {
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[0], "lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  ,  40, 0., 1., 20, 70, 110, folder);
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[1], "lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  ,  40, 0., 1., 20, 70, 110, folder);
    } else {
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[0], "lepmvsmva0"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
      Utilities::BookH2F(fEventHist[i]->hLepMVsMVA[1], "lepmvsmva1"    , Form("%s: Lepton M vs MVA",dirname)  ,  50, -1., 1., 50,   50, 150, folder);
    }
  }

  Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
  Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
  Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
  Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);
  Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[0]    , "deltaalpham0"     , Form("%s: Delta Alpha M 0"     ,dirname),  80,  40, 180, folder);
  Utilities::BookH1F(fEventHist[i]->hDeltaAlphaM[1]    , "deltaalpham1"     , Form("%s: Delta Alpha M 1"     ,dirname),  80,  40, 180, folder);
  Utilities::BookH1F(fEventHist[i]->hBeta[0]           , "beta0"            , Form("%s: Beta (Z) 0"          ,dirname), 100,   0,  5., folder);
  Utilities::BookH1F(fEventHist[i]->hBeta[1]           , "beta1"            , Form("%s: Beta (Z) 1"          ,dirname), 100,   0,  5., folder);

  Utilities::BookH1F(fEventHist[i]->hPTauVisFrac    , "ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  , 50,0.,  1.5, folder);

  for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
    Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig->names_[j].Data()) ,
                       fMVAConfig->NBins(j, i), fMVAConfig->Bins(j, i).data(), folder);
    //high mva score binning to improve cdf making
    Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig->names_[j].Data()), 2000, -1.,  1., folder);
    //CDF transformed score
    Utilities::BookH1D(fEventHist[i]->hMVA[j][2]  , Form("mva%i_2",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig->names_[j].Data()), fNCDFBins+1,  0.-1./fNCDFBins,  1., folder);
    //log10(CDF p-value)
    Utilities::BookH1D(fEventHist[i]->hMVA[j][3]  , Form("mva%i_3",j)   , Form("%s: %s log10(MVA)"  ,dirname, fMVAConfig->names_[j].Data()), 20, -3., 0., folder);
    //F(p-value) transformation
    Utilities::BookH1D(fEventHist[i]->hMVA[j][4]  , Form("mva%i_4",j)   , Form("%s: %s f(p)"  ,dirname, fMVAConfig->names_[j].Data()), 20, 0., 1., folder);
    //no BDT score correction
    Utilities::BookH1D(fEventHist[i]->hMVA[j][5], Form("mva%i_5",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig->names_[j].Data()) ,
                       fMVAConfig->NBins(j, i), fMVAConfig->Bins(j, i).data(), folder);
    //histograms of the test/train components
    Utilities::BookH1F(fEventHist[i]->hMVATrain[j], Form("mvatrain%i",j), Form("%s: %s MVA (train)" ,dirname, fMVAConfig->names_[j].Data()),  50, -1.,  1., folder);
    Utilities::BookH1F(fEventHist[i]->hMVATest[j] , Form("mvatest%i",j) , Form("%s: %s MVA (test)"  ,dirname, fMVAConfig->names_[j].Data()),  50, -1.,  1., folder);
  }
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

      // const double momerr_bins[] = {0., 5.e-4f, 1.e-3f, 2.e-3f, 5.e-3f, 0.01f, 0.02f, 0.05f};
      // const int    n_momerr_bins = sizeof(momerr_bins) / sizeof(*momerr_bins) - 1;

      // Lepton one info //
      Utilities::BookH1F(fLepHist[i]->hOnePt[0]       , "onept"            , Form("%s: Pt"           ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePtUp        , "oneptup"          , Form("%s: Pt up"        ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePtDown      , "oneptdown"        , Form("%s: Pt down"      ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePz          , "onepz"            , Form("%s: Pz"           ,dirname), 100, -100,  100, folder);
      // if(fSelection == "mutau" || fSelection == "mumu")
      //   Utilities::BookH1F(fLepHist[i]->hOneMomErr      , "onemomerr"        , Form("%s: Mom. Error"   ,dirname),  n_momerr_bins, momerr_bins, folder);
      // else
      //   Utilities::BookH1F(fLepHist[i]->hOneMomErr      , "onemomerr"        , Form("%s: Mom. Error"   ,dirname),  20, 0., 1., folder);
      if(fSelection == "mutau" || fSelection == "mumu")
        Utilities::BookH1F(fLepHist[i]->hOneMomErr      , "onemomerr"        , Form("%s: Mom. scale error"   ,dirname),  20, 0., 0.015, folder);
      else
        Utilities::BookH1F(fLepHist[i]->hOneMomErr      , "onemomerr"        , Form("%s: Mom. scale error"   ,dirname),  20, 0., 0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePtRelErr    , "oneptrelerr"      , Form("%s: Mom. Error"   ,dirname),  40, -0.2, 0.2, folder);
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
      Utilities::BookH1D(fLepHist[i]->hOnePileup      , "onepileup"         , Form("%s: Pileup"      ,dirname),   2,    0,    2, folder);
      Utilities::BookH1D(fLepHist[i]->hOneJetOrigin   , "onejetorigin"      , Form("%s: JetOrigin"   ,dirname),   2,    0,    2, folder);
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

      //Interesting electron IDs
      Utilities::BookH1F(fLepHist[i]->hEleConvVeto      , "eleconvveto     "   , Form("%s: hEleConvVeto     ",dirname),   2,    0,    2, folder);
      Utilities::BookH1F(fLepHist[i]->hEleLostHits      , "elelosthits     "   , Form("%s: hEleLostHits     ",dirname),  10,    0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hElePFCand        , "elepfcand       "   , Form("%s: hElePFCand       ",dirname),   2,    0,    2, folder);

      //Electron study info
      if(fDoEleIDStudy) {
        Utilities::BookH1F(fLepHist[i]->hEleEtaEta        , "eleetaeta       "   , Form("%s: hEleEtaEta       ",dirname),  50,    0, 0.035, folder);
        Utilities::BookH1F(fLepHist[i]->hEleR9            , "eler9           "   , Form("%s: hEleR9           ",dirname),  50,    0,  1.2, folder);
        // Utilities::BookH1F(fLepHist[i]->hElePhotonIdx     , "elephotonidx    "   , Form("%s: hElePhotonIdx    ",dirname),  50,    0,    5, folder);
        Utilities::BookH1F(fLepHist[i]->hEleCutID         , "elecutid        "   , Form("%s: hEleCutID        ",dirname),   5,    0,    5, folder);
        Utilities::BookH1F(fLepHist[i]->hEleEnergyErr     , "eleenergyerr    "   , Form("%s: hEleEnergyErr    ",dirname),  50,    0,  0.5, folder);
        Utilities::BookH1F(fLepHist[i]->hEle3DPVErr       , "ele3dpverr      "   , Form("%s: hEle3DPVErr      ",dirname),  50,    0,    5, folder);
        Utilities::BookH1F(fLepHist[i]->hEleSCEtOverPt    , "elescetoverpt   "   , Form("%s: hEleSCEtOverPt   ",dirname),  50, -0.5,  0.5, folder);
        Utilities::BookH1F(fLepHist[i]->hEleHOE           , "elehoe          "   , Form("%s: hEleHOE          ",dirname),  50,    0,  0.5, folder);
        Utilities::BookH1F(fLepHist[i]->hEleEInvMinusPInv , "eleeinvminuspinv"   , Form("%s: hEleEInvMinusPInv",dirname),  50, -0.2,  0.2, folder);
      }

      // Lepton two info //
      Utilities::BookH1F(fLepHist[i]->hTwoPt[0]       , "twopt"            , Form("%s: Pt"          ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPtUp        , "twoptup"          , Form("%s: Pt up"       ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPtDown      , "twoptdown"        , Form("%s: Pt down"     ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPz          , "twopz"            , Form("%s: Pz"          ,dirname), 100, -100,  100, folder);
      // if(fSelection == "emu" || fSelection.Contains("_"))
      //   Utilities::BookH1F(fLepHist[i]->hTwoMomErr      , "twomomerr"        , Form("%s: Mom. Error"  ,dirname),  n_momerr_bins, momerr_bins, folder);
      // else
      //   Utilities::BookH1F(fLepHist[i]->hTwoMomErr      , "twomomerr"        , Form("%s: Mom. Error"  ,dirname),  20, 0., 1., folder);
      if(fSelection == "emu" || fSelection.Contains("_"))
        Utilities::BookH1F(fLepHist[i]->hTwoMomErr      , "twomomerr"        , Form("%s: Mom. scale error"  ,dirname),  20, 0., 0.015, folder);
      else
        Utilities::BookH1F(fLepHist[i]->hTwoMomErr      , "twomomerr"        , Form("%s: Mom. scale error"  ,dirname),  20, 0., 0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPtRelErr    , "twoptrelerr"      , Form("%s: Mom. Error"  ,dirname),  40, -0.2, 0.2, folder);
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
      Utilities::BookH1D(fLepHist[i]->hTwoPileup      , "twopileup"        , Form("%s: Pileup"      ,dirname),   2,   0,    2, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoJetOrigin   , "twojetorigin"     , Form("%s: JetOrigin"   ,dirname),   2,   0,    2, folder);
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
      Utilities::BookH1F(fLepHist[i]->hPtRatio        , "ptratio"          , Form("%s: 1 pT / 2 pT"  ,dirname),  50,    0,    3, folder);
      Utilities::BookH1F(fLepHist[i]->hPtTrailOverLead, "pttrailoverlead"  , Form("%s: trail pT / lead pT",dirname),  20,    0,    1, folder);
      Utilities::BookH1F(fLepHist[i]->hD0Diff         , "d0diff"           , Form("%s: 2 D0 - 1 D0"  ,dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDXYDiff        , "dxydiff"          , Form("%s: 2 DXY - 1 DXY",dirname), 100,-0.08, 0.08, folder);
      Utilities::BookH1F(fLepHist[i]->hDZDiff         , "dzdiff"           , Form("%s: 2 DZ - 1 DZ"  ,dirname), 100,-0.08, 0.08, folder);

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
        for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
          if(fUseCDFBDTs) {
            Utilities::BookH1D(fSystematicHist[i]->hMVA[j][sys], Form("mva%i_%i",j, sys), Form("%s: %s MVA %i" ,dirname, fMVAConfig->names_[j].Data(), sys),
                               fNCDFBins, 0., 1., folder);
          } else {
            Utilities::BookH1D(fSystematicHist[i]->hMVA[j][sys], Form("mva%i_%i",j, sys), Form("%s: %s MVA %i" ,dirname, fMVAConfig->names_[j].Data(), sys),
                               fMVAConfig->NBins(j, i), fMVAConfig->Bins(j, i).data(), folder);
          }
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
  fTrees[index]->Branch("lepmestimateoverm"   , &fTreeVars.mestimateoverm    );
  fTrees[index]->Branch("lepmestimatetwooverm", &fTreeVars.mestimatetwooverm );
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
  fTrees[index]->Branch("trainfraction"     , &fTreeVars.trainfraction     );
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
  for(unsigned imva = 0; imva < fMVAConfig->names_.size(); ++imva) {
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
    } else if(fDYType == 2) { //only process Z->ee/mumu --> No longer removing Z->tau tau from processing
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
  Utilities::SetBranchAddress(tree, "Muon_corrected_pt"        , &Muon_corrected_pt         );
  Utilities::SetBranchAddress(tree, "Muon_correctedUp_pt"      , &Muon_correctedUp_pt       );
  Utilities::SetBranchAddress(tree, "Muon_correctedDown_pt"    , &Muon_correctedDown_pt     );
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
    if(fApplyLeptonIDWt == 2) { //load and use ntuple-level ID corrections
      Utilities::SetBranchAddress(tree, "Muon_ID_wt"           , &Muon_ID_wt                );
      Utilities::SetBranchAddress(tree, "Muon_ID_up"           , &Muon_ID_up                );
      Utilities::SetBranchAddress(tree, "Muon_ID_down"         , &Muon_ID_down              );
      Utilities::SetBranchAddress(tree, "Muon_IsoID_wt"        , &Muon_IsoID_wt             );
      Utilities::SetBranchAddress(tree, "Muon_IsoID_up"        , &Muon_IsoID_up             );
      Utilities::SetBranchAddress(tree, "Muon_IsoID_down"      , &Muon_IsoID_down           );
    }
    Utilities::SetBranchAddress(tree, "Muon_genPartFlav"       , &Muon_genPartFlav          );
    Utilities::SetBranchAddress(tree, "Muon_genPartIdx"        , &Muon_genPartIdx           );
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
  Utilities::SetBranchAddress(tree, "Electron_energyErr"            , &Electron_energyErr            );
  Utilities::SetBranchAddress(tree, "Electron_sip3d"                , &Electron_sip3d                );
  Utilities::SetBranchAddress(tree, "Electron_sieie"                , &Electron_sieie                );
  Utilities::SetBranchAddress(tree, "Electron_r9"                   , &Electron_r9                   );
  Utilities::SetBranchAddress(tree, "Electron_hoe"                  , &Electron_hoe                  );
  Utilities::SetBranchAddress(tree, "Electron_scEtOverPt"           , &Electron_scEtOverPt           );
  Utilities::SetBranchAddress(tree, "Electron_eInvMinusPInv"        , &Electron_eInvMinusPInv        );
  Utilities::SetBranchAddress(tree, "Electron_convVeto"             , &Electron_convVeto             );
  Utilities::SetBranchAddress(tree, "Electron_cutBased"             , &Electron_cutBased             );
  Utilities::SetBranchAddress(tree, "Electron_isPFcand"             , &Electron_isPFcand             );
  Utilities::SetBranchAddress(tree, "Electron_lostHits"             , &Electron_lostHits             );
  Utilities::SetBranchAddress(tree, "Electron_photonIdx"            , &Electron_photonIdx            );
  if(!fIsData) {
    if(fApplyLeptonIDWt == 2) { //load and use ntuple-level ID corrections
      Utilities::SetBranchAddress(tree, "Electron_ID_wt"            , &Electron_ID_wt                );
      Utilities::SetBranchAddress(tree, "Electron_ID_up"            , &Electron_ID_up                );
      Utilities::SetBranchAddress(tree, "Electron_ID_down"          , &Electron_ID_down              );
      Utilities::SetBranchAddress(tree, "Electron_IsoID_wt"         , &Electron_IsoID_wt             );
      Utilities::SetBranchAddress(tree, "Electron_IsoID_up"         , &Electron_IsoID_up             );
      Utilities::SetBranchAddress(tree, "Electron_IsoID_down"       , &Electron_IsoID_down           );
      Utilities::SetBranchAddress(tree, "Electron_RecoID_wt"        , &Electron_RecoID_wt            );
      Utilities::SetBranchAddress(tree, "Electron_RecoID_up"        , &Electron_RecoID_up            );
      Utilities::SetBranchAddress(tree, "Electron_RecoID_down"      , &Electron_RecoID_down          );
    }
    Utilities::SetBranchAddress(tree, "Electron_genPartFlav"        , &Electron_genPartFlav          );
    Utilities::SetBranchAddress(tree, "Electron_genPartIdx"         , &Electron_genPartIdx           );
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
  if(!fIsData && !fIsEmbed) { //uncertainties/corrections only relevant for simulated jets
    Utilities::SetBranchAddress(tree, "Jet_pt_nom"                    , &Jet_pt_nom                    );
    Utilities::SetBranchAddress(tree, "Jet_mass_nom"                  , &Jet_mass_nom                  );
    Utilities::SetBranchAddress(tree, "Jet_pt_jerUp"                  , &Jet_pt_jerUp                  );
    Utilities::SetBranchAddress(tree, "Jet_pt_jerDown"                , &Jet_pt_jerDown                );
    Utilities::SetBranchAddress(tree, "Jet_pt_jesTotalUp"             , &Jet_pt_jesTotalUp             );
    Utilities::SetBranchAddress(tree, "Jet_pt_jesTotalDown"           , &Jet_pt_jesTotalDown           );
  }
  if(fUseBTagWeights == 2 && !fIsData && !fIsEmbed) {
    Utilities::SetBranchAddress(tree, "Jet_btagSF_deepcsv_L_wt"     , &Jet_btagSF_L                  );
    Utilities::SetBranchAddress(tree, "Jet_btagSF_deepcsv_L_wt_up"  , &Jet_btagSF_L_up               );
    Utilities::SetBranchAddress(tree, "Jet_btagSF_deepcsv_L_wt_down", &Jet_btagSF_L_down             );
    Utilities::SetBranchAddress(tree, "Jet_btagSF_deepcsv_T_wt"     , &Jet_btagSF_T                  );
    Utilities::SetBranchAddress(tree, "Jet_btagSF_deepcsv_T_wt_up"  , &Jet_btagSF_T_up               );
    Utilities::SetBranchAddress(tree, "Jet_btagSF_deepcsv_T_wt_down", &Jet_btagSF_T_down             );
  }
  // Utilities::SetBranchAddress(tree, "Jet_charge"                    , &Jet_charge                    );
  Utilities::SetBranchAddress(tree, "Jet_jetId"                     , &Jet_jetId                     );
  Utilities::SetBranchAddress(tree, "Jet_puId"                      , &Jet_puId                      );
  // Utilities::SetBranchAddress(tree, "Jet_pfRelIso03_all"            , &Jet_pfRelIso03_al           l );
  Utilities::SetBranchAddress(tree, "Jet_btagDeepB"                 , &Jet_btagDeepB                 );
  // Utilities::SetBranchAddress(tree, "Jet_btagCMVA"                  , &Jet_btagCMVA                  );
  // Utilities::SetBranchAddress(tree, "Jet_eCorr"                     , &Jet_eCorr                     );
  // Utilities::SetBranchAddress(tree, "Jet_dxy"                       , &Jet_dxy                       );
  // Utilities::SetBranchAddress(tree, "Jet_dxyErr"                    , &Jet_dxyErr                    );
  // Utilities::SetBranchAddress(tree, "Jet_dz"                        , &Jet_dz                        );
  // Utilities::SetBranchAddress(tree, "Jet_dzErr"                     , &Jet_dzErr                     );
  if(!fIsData && !fIsEmbed) {
    Utilities::SetBranchAddress(tree, "Jet_partonFlavour"             , &Jet_partonFlavour           );
    Utilities::SetBranchAddress(tree, "JetPUIDWeight"                 , &jetPUIDWeight               );
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
  Utilities::SetBranchAddress(tree, "HLT_IsoMu24"                   , &HLT_IsoMu24                   );
  Utilities::SetBranchAddress(tree, "HLT_IsoMu27"                   , &HLT_IsoMu27                   );
  Utilities::SetBranchAddress(tree, "HLT_Ele27_WPTight_Gsf"         , &HLT_Ele27_WPTight_GsF         );
  Utilities::SetBranchAddress(tree, "HLT_Ele32_WPTight_Gsf"         , &HLT_Ele32_WPTight_GsF         );
  Utilities::SetBranchAddress(tree, "HLT_Ele32_WPTight_Gsf_L1DoubleEG", &HLT_Ele32_WPTight_GsF_L1DoubleEG);
  //determine the run info if data to get the correct e-mu trigger
  TString name = GetOutputName();
  if(fYear == 2016 && ((!fIsData && !fIsEmbed) || !(name.Contains("-G") || name.Contains("-H")))) {
    Utilities::SetBranchAddress(tree, "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL", &HLT_Mu23_Ele12);
    Utilities::SetBranchAddress(tree, "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL" , &HLT_Mu8_Ele23 );
  } else {
    Utilities::SetBranchAddress(tree, "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", &HLT_Mu23_Ele12);
    Utilities::SetBranchAddress(tree, "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ" , &HLT_Mu8_Ele23 );
  }

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
    Utilities::SetBranchAddress(tree, "GenPart_pt"                    , &GenPart_pt                    );
    Utilities::SetBranchAddress(tree, "GenPart_eta"                   , &GenPart_eta                   );
    Utilities::SetBranchAddress(tree, "GenPart_phi"                   , &GenPart_phi                   );
    Utilities::SetBranchAddress(tree, "GenPart_mass"                  , &GenPart_mass                  );
    Utilities::SetBranchAddress(tree, "GenPart_genPartIdxMother"      , &GenPart_genPartIdxMother      );

  }
  if(fIsSignal && fUseSignalZWeights == 2)
    Utilities::SetBranchAddress(tree, "SignalpTWeight"                , &signalZWeight                 );
  if((fIsSignal || fIsDY) && fUseZPtWeight == 2) {
    Utilities::SetBranchAddress(tree, "ZpTWeight"                     , &zPtWeight                     );
    Utilities::SetBranchAddress(tree, "ZpTWeight_sys"                 , &zPtWeightUp                   );
  }

  //Random field for event splitting
  Utilities::SetBranchAddress(tree, "RandomField"                     , &randomField                   );

  //Event information
  if(!fIsData) {
    Utilities::SetBranchAddress(tree, "genWeight"                     , &genWeight                     );
    if(!fIsEmbed) {
      Utilities::SetBranchAddress(tree, "LHE_Njets"                   , &LHE_Njets                     );
      Utilities::SetBranchAddress(tree, "Pileup_nTrueInt"             , &nPU                           );
      Utilities::SetBranchAddress(tree, "puWeight"                    , &puWeight                      );
      Utilities::SetBranchAddress(tree, "puWeightUp"                  , &puWeight_up                   );
      Utilities::SetBranchAddress(tree, "puWeightDown"                , &puWeight_down                 );
      Utilities::SetBranchAddress(tree, "PrefireWeight"               , &prefireWeight                 );
      Utilities::SetBranchAddress(tree, "PrefireWeight_Up"            , &prefireWeight_up              );
      Utilities::SetBranchAddress(tree, "PrefireWeight_Down"          , &prefireWeight_down            );
    }
    Utilities::SetBranchAddress(tree, "nPSWeight"                     , &nPSWeight                     );
    Utilities::SetBranchAddress(tree, "PSWeight"                      , PSWeight                       );
    Utilities::SetBranchAddress(tree, "nLHEPdfWeight"                 , &nLHEPdfWeight                 );
    Utilities::SetBranchAddress(tree, "LHEPdfWeight"                  , &LHEPdfWeight                  );
    Utilities::SetBranchAddress(tree, "nLHEScaleWeight"               , &nLHEScaleWeight               );
    Utilities::SetBranchAddress(tree, "LHEScaleWeight"                , &LHEScaleWeight                );
  }
  Utilities::SetBranchAddress(tree, "SelectionFilter_LepM"          , &SelectionFilter_LepM          );
  Utilities::SetBranchAddress(tree, "PV_npvsGood"                   , &nPV                           ) ;
  // Utilities::SetBranchAddress(tree, "Pileup_nPU"                    , &nPUAdded                      );
  Utilities::SetBranchAddress(tree, "HT"                            , &ht                            );
  Utilities::SetBranchAddress(tree, "MET_pt"                        , &PFMET                         );
  Utilities::SetBranchAddress(tree, "MET_phi"                       , &PFMETphi                      );
  Utilities::SetBranchAddress(tree, "RawMET_pt"                     , &rawPFMET                      );
  Utilities::SetBranchAddress(tree, "RawMET_phi"                    , &rawPFMETphi                   );
  Utilities::SetBranchAddress(tree, "PuppiMET_pt"                   , &puppMET                       );
  Utilities::SetBranchAddress(tree, "PuppiMET_phi"                  , &puppMETphi                    );
  Utilities::SetBranchAddress(tree, "PuppiMET_sumEt"                , &puppMETSumEt                  );
  Utilities::SetBranchAddress(tree, "PuppiMET_ptJERUp"              , &puppMETJERUp                  );
  Utilities::SetBranchAddress(tree, "PuppiMET_ptJESUp"              , &puppMETJESUp                  );
  Utilities::SetBranchAddress(tree, "PuppiMET_phiJERUp"             , &puppMETphiJERUp               );
  Utilities::SetBranchAddress(tree, "PuppiMET_phiJESUp"             , &puppMETphiJESUp               );
  Utilities::SetBranchAddress(tree, "RawPuppiMET_pt"                , &rawPuppMET                    );
  Utilities::SetBranchAddress(tree, "RawPuppiMET_phi"               , &rawPuppMETphi                 );

  //FIXME: Decide on the right MET significance
  Utilities::SetBranchAddress(tree, "MET_significance"              , &metSignificance               );

  //MVA information
  if(!fReprocessMVAs) {
    for(unsigned mva_i = 0; mva_i < 10; ++mva_i) {
      Utilities::SetBranchAddress(tree, Form("mva%i", (int) mva_i), &fMvaOutputs[mva_i]);
    }
  }

  //Event flags
  fFlags.InitBranches(tree);
}

//-----------------------------------------------------------------------------------------------------------------
//apply/replace electron energy scale corrections
void HistMaker::ApplyElectronCorrections() {
  float delta_x(metCorr*std::cos(metCorrPhi)), delta_y(metCorr*std::sin(metCorrPhi));
  for(UInt_t index = 0; index < nElectron; ++index) {

    //Store the starting pT to update the MET after corrections
    const float pt_orig = Electron_pt[index];

    //-------------------------------------------------------------
    //Remove energy corrections where needed

    float sf = 1.f;
    if(fIsEmbed) sf /= Electron_eCorr[index]; //remove the NANOAOD defined correction applied to embedding electrons
    Electron_pt[index] *= sf;
    // Electron_mass[index] *= sf;

    //-------------------------------------------------------------
    //Correct the energy scale and set an uncertainty on it

    Electron_energyScale    [index] = 1.f;
    Electron_energyScaleUp  [index] = 1.f;
    Electron_energyScaleDown[index] = 1.f;
    sf = 1.f;
    if(fIsEmbed) {
      const float gen_pt = (Electron_genPartIdx[index] >= 0) ? GenPart_pt[Electron_genPartIdx[index]] : Electron_pt[index];
      sf = fEmbeddingResolution->ElectronScale(Electron_eta[index], gen_pt, fYear,
                                               Electron_energyScaleUp[index], Electron_energyScaleDown[index]);
      // sf = fElectronIDWeight.EmbedEnergyScale(Electron_pt[index], Electron_eta[index],
      //                                         fYear, Electron_energyScaleUp[index], Electron_energyScaleDown[index]);
    } else if(!fIsData) { //FIXME: Get electron energy scale uncertainty in MC
      const float ele_scale_sys = 0.003f; //FIXME: Estimating the scale uncertainty from fits of MC resolution function (see emb_resolution study)
      Electron_energyScaleUp  [index] = 1.f + ele_scale_sys;
      Electron_energyScaleDown[index] = 1.f - ele_scale_sys;
    }
    Electron_energyScale[index] = sf;
    Electron_pt[index] *= sf;
    // Electron_mass[index] *= sf;

    //-------------------------------------------------------------
    //Correct the energy resolution and set an uncertainty on it

    Electron_resolutionScale    [index] = 1.f;
    Electron_resolutionScaleUp  [index] = 1.f;
    Electron_resolutionScaleDown[index] = 1.f;
    sf = 1.f;
    if(fIsEmbed && fApplyElectronResolution) {
      const float gen_pt = (Electron_genPartIdx[index] >= 0) ? GenPart_pt[Electron_genPartIdx[index]] : Electron_pt[index];
      sf = fEmbeddingResolution->ElectronResolution(Electron_pt[index], Electron_eta[index], gen_pt, fYear,
                                                    Electron_resolutionScaleUp[index], Electron_resolutionScaleDown[index]);
      Electron_resolutionScale[index] = sf;
      Electron_pt[index] *= sf;
      if(fVerbose > 9) {
        printf("  HistMaker::%s: Index %2i: Res scales: %.4f (%.4f/%.4f)\n", __func__, index, sf, Electron_resolutionScaleUp[index], Electron_resolutionScaleDown[index]);
      }
      // Electron_mass[index] *= sf;
    } else if(!fIsData && !fIsEmbed) {
      const float gen_pt = (Electron_genPartIdx[index] >= 0) ? GenPart_pt[Electron_genPartIdx[index]] : Electron_pt[index];
      const float ele_scale_sys = 0.05f;
      Electron_resolutionScale    [index] = 1.f; //no correction by default
      Electron_resolutionScaleUp  [index] = 1.f + ele_scale_sys*(Electron_pt[index] - gen_pt) / gen_pt;
      Electron_resolutionScaleDown[index] = 1.f - ele_scale_sys*(Electron_pt[index] - gen_pt) / gen_pt;
    }

    //-------------------------------------------------------------
    //Store the event pT change after corrections for MET updates

    //Evaluate the change in the pT = finish - start pT
    const float pt_diff = pt_orig - Electron_pt[index];

    //subtract the change from the MET
    delta_x -= pt_diff*std::cos(Electron_phi[index]);
    delta_y -= pt_diff*std::sin(Electron_phi[index]);
  }

  //-------------------------------------------------------------
  //Update the MET with these corrections applied

  if(fUpdateMET) {
    metCorr = std::sqrt(delta_x*delta_x + delta_y*delta_y);
    metCorrPhi = (metCorr > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, delta_x/metCorr)))*(delta_y < 0.f ? -1 : 1) : 0.f;
    if(!std::isfinite(metCorr) || !std::isfinite(metCorrPhi)) {
      printf("HistMaker::%s: Entry %12lld: MET correction undefined! corr = %.2f, phi = %.3f, dx = %.2f, dy = %.2f\n",
             __func__, fentry, metCorr, metCorrPhi, delta_x, delta_y);
      metCorr = 0.f;
      metCorrPhi = 0.f;
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply the Rochester corrections to the muon pt for all muons
void HistMaker::ApplyMuonCorrections() {
  //FIXME: Update to using the Rochester correction uncertainty instead of size
  const static bool use_size(fUseRoccoSize != 0 && fUseRoccoCorr != 0); //use Rochester size or evaluated uncertainty
  float delta_x(metCorr*std::cos(metCorrPhi)), delta_y(metCorr*std::sin(metCorrPhi));
  const static int s(0), m(0); //error set and member for corrections
  const static int sys_s(2), sys_m(0); //FIXME: Decide on a systematic correction set, currently using Zpt
  for(UInt_t index = 0; index < nMuon; ++index) {
    //First apply corrections to match Embedding --> MC
    if(fIsEmbed && fUseEmbedMuonES) {
      const float gen_pt = (Muon_genPartIdx[index] >= 0) ? GenPart_pt[Muon_genPartIdx[index]] : Muon_pt[index];
      float up, down;
      const float sf = fEmbeddingResolution->MuonScale(Muon_eta[index], gen_pt, fYear, up, down);

      //Record the pT change to propagate to the MET
      const float pt_diff = Muon_pt[index]*(sf - 1.f);
      Muon_pt[index] *= sf;
      delta_x -= pt_diff*std::cos(Muon_phi[index]);
      delta_y -= pt_diff*std::sin(Muon_phi[index]);
      if(fVerbose > 1) {
        printf(" HistMaker::%s: Applying pT scale of %.4f to embedding muon %u with pT %.2f and gen_pt %.2f\n", __func__, sf, index, Muon_pt[index], gen_pt);
      }
    }

    //Embedding resolution correction FIXME: Remove this
    if(fIsEmbed && fUseEmbedMuonRes) {
      const float gen_pt = (Muon_genPartIdx[index] >= 0) ? GenPart_pt[Muon_genPartIdx[index]] : Muon_pt[index];
      const float sf = fEmbeddingResolution->MuonResolutionUnc(Muon_pt[index], Muon_eta[index], gen_pt, fYear);
      //Record the pT change to propagate to the MET
      const float pt_diff = Muon_pt[index]*(sf - 1.f);
      Muon_pt[index] *= sf;
      delta_x -= pt_diff*std::cos(Muon_phi[index]);
      delta_y -= pt_diff*std::sin(Muon_phi[index]);
      if(fVerbose > 1) {
        printf(" HistMaker::%s: Applying resolution pT scale of %.4f to embedding muon %u with pT %.2f and gen_pt %.2f\n", __func__, sf, index, Muon_pt[index], gen_pt);
      }
    }

    //Apply the Rochester corrections to data and simulated muons
    if((fIsEmbed && !fUseEmbedRocco) || fUseRoccoCorr == 0) {  //don't correct embedding (or any, if selected) to muons
      Muon_RoccoSF[index] = 1.f;
      const float abs_eta = std::fabs(Muon_eta[index]);
      Muon_ESErr[index] = (fIsEmbed) ? (abs_eta < 1.2) ? 0.004 : (abs_eta < 2.1) ? 0.009 : 0.027 : 0.002; //values from the LFV Higgs AN
      continue;
    } else if(fUseRoccoCorr == 1) { //apply locally-evaluated corrections
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
          err = std::fabs(sf - err); //set error as the deviation from the nominal scale factor
        } else { //not matched
          sf   = fRoccoR->kSmearMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], Muon_nTrackerLayers[index], u, s, m);
          err  = fRoccoR->kSmearMC(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], Muon_nTrackerLayers[index], u, sys_s, sys_m);
          err *= fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], s, m);
          err /= fRoccoR->kScaleDT(Muon_charge[index], Muon_pt[index], Muon_eta[index], Muon_phi[index], sys_s, sys_m);
          err = std::fabs(sf - err); //set error as the deviation from the nominal scale factor
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
      if(fVerbose > 1) {
        printf(" HistMaker::%s: Applying Rochester pT scale of %.4f to muon %u\n", __func__, sf, index);
      }
    } else if(fUseRoccoCorr == 2) { //apply ntuple-level corrections
      Muon_RoccoSF[index]  = Muon_corrected_pt[index] / Muon_pt[index];
      const double pt_diff = Muon_corrected_pt[index] - Muon_pt[index];
      Muon_pt[index] = Muon_corrected_pt[index];
      delta_x -= pt_diff*std::cos(Muon_phi[index]);
      delta_y -= pt_diff*std::sin(Muon_phi[index]);
      if(use_size) Muon_ESErr[index] = std::fabs(1.f - Muon_RoccoSF[index]); //size of the correction
      else         Muon_ESErr[index] = std::max(std::fabs(1.f - Muon_correctedUp_pt[index]/Muon_pt[index]),
                                                std::fabs(1.f - Muon_correctedDown_pt[index]/Muon_pt[index])); //take the larger fractional uncertainty of the two
    }
    if(fIsEmbed && fUseEmbedRocco == 2) { //apply Rochester corrections, but take uncertainty from Embed vs. MC scale comparisons
      const float abs_eta = std::fabs(Muon_eta[index]);
      Muon_ESErr[index] = (abs_eta < 1.2) ? 0.001 : (abs_eta < 2.1) ? 0.0015 : 0.003; //disagreement between MC and Embedding from resolution study
    }
  }
  //Update the MET with these corrections applied
  if(fUpdateMET) {
    metCorr = std::sqrt(delta_x*delta_x + delta_y*delta_y);
    metCorrPhi = (metCorr > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, delta_x/metCorr)))*(delta_y < 0.f ? -1 : 1) : 0.f;
    if(!std::isfinite(metCorr) || !std::isfinite(metCorrPhi)) {
      printf("HistMaker::%s: Entry %12lld: MET correction undefined! corr = %.2f, phi = %.3f, dx = %.2f, dy = %.2f\n",
             __func__, fentry, metCorr, metCorrPhi, delta_x, delta_y);
      metCorr = 0.f;
      metCorrPhi = 0.f;
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------
//apply tau energy scale corrections
void HistMaker::ApplyTauCorrections() {
  tauES = 1.f; tauES_up = 1.f; tauES_down = 1.f;
  float delta_x(metCorr*std::cos(metCorrPhi)), delta_y(metCorr*std::sin(metCorrPhi));
  for(UInt_t index = 0; index < nTau; ++index) {
    float sf = (fIsData) ? 1. : fTauIDWeight->EnergyScale(Tau_pt[index], Tau_eta[index], Tau_decayMode[index],
                                                          Tau_genPartFlav[index], Tau_idDeepTau2017v2p1VSjet[index],
                                                          fYear, Tau_energyScaleUp[index], Tau_energyScaleDown[index]);
    //FIXME: remove this correction or formalize it
    if(fIsEmbed && Tau_genPartFlav[index] == 5) {
      sf *= 0.994; //Embedding tau over estimates the MC resolution mean after applying energy scale corrections
    }
    double pt_diff = Tau_pt[index];
    Tau_pt  [index] *= sf;
    Tau_mass[index] *= sf;
    Tau_energyScale[index] = sf;
    pt_diff = Tau_pt[index] - pt_diff;
    //subtract the change from the MET
    delta_x -= pt_diff*std::cos(Tau_phi[index]);
    delta_y -= pt_diff*std::sin(Tau_phi[index]);
  }
  //Update the MET with these corrections applied
  if(fUpdateMET) {
    metCorr = std::sqrt(delta_x*delta_x + delta_y*delta_y);
    metCorrPhi = (metCorr > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, delta_x/metCorr)))*(delta_y < 0.f ? -1 : 1) : 0.f;
    if(!std::isfinite(metCorr) || !std::isfinite(metCorrPhi)) {
      printf("HistMaker::%s: Entry %12lld: MET correction undefined! corr = %.2f, phi = %.3f, dx = %.2f, dy = %.2f\n",
             __func__, fentry, metCorr, metCorrPhi, delta_x, delta_y);
      metCorr = 0.f;
      metCorrPhi = 0.f;
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------
//Apply a lepton energy scale change, propagating the effect to the MET
void HistMaker::EnergyScale(const float scale, Lepton_t& lep, float* MET, float* METPhi) {
  if(!lep.p4) return;
  TLorentzVector& lv = *(lep.p4);
  EnergyScale(scale, lv, MET, METPhi);
  lep.pt   = lep.p4->Pt();
  lep.eta  = lep.p4->Eta();
  lep.phi  = lep.p4->Phi();
  lep.mass = lep.p4->M();
}

//-----------------------------------------------------------------------------------------------------------------
//Apply an energy scale change, propagating the effect to the MET
void HistMaker::EnergyScale(const float scale, TLorentzVector& lv, float* MET, float* METPhi) {
  if(scale <= 0.f) {
    printf("!!! HistMaker::%s: Non-positive energy scale %f\n", __func__, scale);
    return;
  }
  if(fVerbose > 5) printf("HistMaker::%s: Scale = %.4f, input (pT, eta, phi, M) = (%.2f, %.2f, %.2f, %.2e) ",
                          __func__, scale, lv.Pt(), lv.Eta(), lv.Phi(), lv.M());
  const float pt_shift((scale-1.f)*lv.Pt()), phi(lv.Phi());
  lv *= scale;
  //ensure the vector has a non-imaginary mass
  if(lv.M2() < 0.) lv.SetE(lv.P());
  if(fVerbose > 5) printf("output (pT, eta, phi, M) = (%.2f, %.2f, %.2f, %.2e) ",
                          lv.Pt(), lv.Eta(), lv.Phi(), lv.M());

  //adjust the MET following the shift in the lepton pT, if non-null
  if(fUpdateMET && MET && METPhi) {
    if(fVerbose > 5) printf(" MET input (pT, phi) = (%.2f, %.2f) ", *MET, *METPhi);
    const float dx(pt_shift*std::cos(phi)), dy(pt_shift*std::sin(phi));
    const float x((*MET)*std::cos(*METPhi)+dx), y((*MET)*std::sin(*METPhi)+dy);
    *MET    = std::sqrt(x*x+y*y);
    *METPhi = (*MET > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, x/(*MET))))*(y < 0.f ? -1.f : 1.f) : 0.f;
    if(fVerbose > 5) printf(" MET outnput (pT, phi) = (%.2f, %.2f) ", *MET, *METPhi);
  }
  if(fVerbose > 5) printf("\n");
}

//-----------------------------------------------------------------------------------------------------------------
//Randomly smear values in the output tree to approximate uncertainty smearing
void HistMaker::SmearTreeValues() {
  //Store the original event variables
  const float o_one_pt(leptonOne.pt), o_one_eta(leptonOne.eta), o_one_phi(leptonOne.phi), o_one_mass(leptonOne.mass);
  const float o_two_pt(leptonTwo.pt), o_two_eta(leptonTwo.eta), o_two_phi(leptonTwo.phi), o_two_mass(leptonTwo.mass);
  const float o_met(met), o_metphi(metPhi);
  const float o_ewt(eventWeight);

  //Ensure the random number generator is initialized
  if(!fRnd) fRnd = new TRandom3(fRndSeed);

  //Approximate lepton energy scale uncertainties
  if(!fIsData || looseQCDSelection) { //shift for data-driven backgrounds to keep consistent in training
    leptonOne.setPtEtaPhiM(fRnd->Gaus(1., 0.005)*o_one_pt, o_one_eta, o_one_phi, o_one_mass); //0.5% scale uncertainty
    leptonTwo.setPtEtaPhiM(fRnd->Gaus(1., 0.005)*o_two_pt, o_two_eta, o_two_phi, o_two_mass); //0.5% scale uncertainty
  }

  //Approximate met and metPhi uncertainties
  if(!fIsData || looseQCDSelection) { //shift for data-driven backgrounds to keep consistent in training
    met = fRnd->Gaus(1., 0.02)*met; //2% uncertainty
    metPhi = Utilities::DeltaPhi(metPhi, fRnd->Gaus(0., 0.03)); //0.03 radian uncertainty
  }

  //Approximate j-->tau uncertainties
  if(fIsData && looseQCDSelection && (mutau || etau)) {
    eventWeight *= 1.f + std::max(-0.5, std::min(0.5, fRnd->Gaus(0., std::fabs(1.f - jetToTauWeightBias/jetToTauWeightCorr))));
  }

  //Approximate QCD SS --> OS uncertainties
  if(fIsData && looseQCDSelection && (emu || mutau_e || etau_mu)) {
    eventWeight *= 1.f + std::max(-0.5, std::min(0.5, fRnd->Gaus(0., std::fabs(1.f - qcdIsoScale*qcdMassBDTScale))));
  }

  //Propagate the shifts to the tree variables
  SetKinematics();

  //Restore kinematic fields for future restoration
  leptonOne.setPtEtaPhiM(o_one_pt, o_one_eta, o_one_phi, o_one_mass);
  leptonTwo.setPtEtaPhiM(o_two_pt, o_two_eta, o_two_phi, o_two_mass);
  met = o_met;
  metPhi = o_metphi;
  eventWeight = o_ewt;
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
    std::swap(Muon_corrected_pt             [0], Muon_corrected_pt             [1]);
    std::swap(Muon_correctedUp_pt           [0], Muon_correctedUp_pt           [1]);
    std::swap(Muon_correctedDown_pt         [0], Muon_correctedDown_pt         [1]);
    std::swap(Muon_ESErr                    [0], Muon_ESErr                    [1]);
    std::swap(Muon_ID_wt                    [0], Muon_ID_wt                    [1]);
    std::swap(Muon_ID_up                    [0], Muon_ID_up                    [1]);
    std::swap(Muon_ID_down                  [0], Muon_ID_down                  [1]);
    std::swap(Muon_IsoID_wt                 [0], Muon_IsoID_wt                 [1]);
    std::swap(Muon_IsoID_up                 [0], Muon_IsoID_up                 [1]);
    std::swap(Muon_IsoID_down               [0], Muon_IsoID_down               [1]);
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
    std::swap(Electron_convVeto             [0], Electron_convVeto             [1]);
    std::swap(Electron_cutBased             [0], Electron_cutBased             [1]);
    std::swap(Electron_isPFcand             [0], Electron_isPFcand             [1]);
    std::swap(Electron_lostHits             [0], Electron_lostHits             [1]);
    std::swap(Electron_sieie                [0], Electron_sieie                [1]);
    std::swap(Electron_r9                   [0], Electron_r9                   [1]);
    std::swap(Electron_sip3d                [0], Electron_sip3d                [1]);
    std::swap(Electron_scEtOverPt           [0], Electron_scEtOverPt           [1]);
    std::swap(Electron_hoe                  [0], Electron_hoe                  [1]);
    std::swap(Electron_eInvMinusPInv        [0], Electron_eInvMinusPInv        [1]);
    std::swap(Electron_TaggedAsRemovedByJet [0], Electron_TaggedAsRemovedByJet [1]);
    std::swap(Electron_genPartFlav          [0], Electron_genPartFlav          [1]);
    std::swap(Electron_genPartIdx           [0], Electron_genPartIdx           [1]);
    std::swap(Electron_energyErr            [0], Electron_energyErr            [1]);
    std::swap(Electron_energyScale          [0], Electron_energyScale          [1]);
    std::swap(Electron_energyScaleUp        [0], Electron_energyScaleUp        [1]);
    std::swap(Electron_energyScaleDown      [0], Electron_energyScaleDown      [1]);
    std::swap(Electron_resolutionScale      [0], Electron_resolutionScale      [1]);
    std::swap(Electron_resolutionScaleUp    [0], Electron_resolutionScaleUp    [1]);
    std::swap(Electron_resolutionScaleDown  [0], Electron_resolutionScaleDown  [1]);
    std::swap(Electron_ID_wt                [0], Electron_ID_wt                [1]);
    std::swap(Electron_ID_up                [0], Electron_ID_up                [1]);
    std::swap(Electron_ID_down              [0], Electron_ID_down              [1]);
    std::swap(Electron_IsoID_wt             [0], Electron_IsoID_wt             [1]);
    std::swap(Electron_IsoID_up             [0], Electron_IsoID_up             [1]);
    std::swap(Electron_IsoID_down           [0], Electron_IsoID_down           [1]);
    std::swap(Electron_RecoID_wt            [0], Electron_RecoID_wt            [1]);
    std::swap(Electron_RecoID_up            [0], Electron_RecoID_up            [1]);
    std::swap(Electron_RecoID_down          [0], Electron_RecoID_down          [1]);
  }
}

//--------------------------------------------------------------------------------------------------------------
// Determine and apply the global event weights (i.e. not selection specific)
void HistMaker::InitializeEventWeights() {
  //reset to-be-defined weights
  eventWeight = 1.f;

  //muon/electron ID weight
  leptonOne.wt1[0]  = 1.f; leptonTwo.wt1[0]  = 1.f;
  leptonOne.wt1[1]  = 1.f; leptonTwo.wt1[1]  = 1.f;
  leptonOne.wt1[2]  = 1.f; leptonTwo.wt1[2]  = 1.f;
  leptonOne.wt1[3]  = 1.f; leptonTwo.wt1[3]  = 1.f;
  leptonOne.wt1_bin = 0  ; leptonTwo.wt1_bin = 0  ;

  //muon Iso ID / electron reco ID weight
  leptonOne.wt2[0]  = 1.f; leptonTwo.wt2[0]  = 1.f;
  leptonOne.wt2[1]  = 1.f; leptonTwo.wt2[1]  = 1.f;
  leptonOne.wt2[2]  = 1.f; leptonTwo.wt2[2]  = 1.f;
  leptonOne.wt2[3]  = 1.f; leptonTwo.wt2[3]  = 1.f;
  leptonOne.wt2_bin = 0  ; leptonTwo.wt2_bin = 0  ;

  //electron Iso ID weight
  leptonOne.wt3[0]  = 1.f; leptonTwo.wt3[0]  = 1.f;
  leptonOne.wt3[1]  = 1.f; leptonTwo.wt3[1]  = 1.f;
  leptonOne.wt3[2]  = 1.f; leptonTwo.wt3[2]  = 1.f;
  leptonOne.wt3[3]  = 1.f; leptonTwo.wt3[3]  = 1.f;
  leptonOne.wt3_bin = 0  ; leptonTwo.wt3_bin = 0  ;

  leptonOne.trig_wt = 1.f; leptonTwo.trig_wt = 1.f;
  const int ntrig = sizeof(triggerWeights)/sizeof(*triggerWeights);
  for(int itrig = 0; itrig < ntrig; ++itrig) triggerWeights[itrig] = 1.f;
  const int ntrig_sys = sizeof(triggerWeightsSys)/sizeof(*triggerWeightsSys);
  for(int itrig = 0; itrig < ntrig_sys; ++itrig) triggerWeightsSys[itrig] = 1.f;

  embeddingWeight = 1.f; embeddingUnfoldingWeight = 1.f;
  if(!fIsSignal) {
    signalZWeight = 1.f; signalZMixingWeight = 1.f;
    if(!fIsDY) {
      zPtWeight = 1.f; zPtWeightUp = 1.f; zPtWeightDown = 1.f; zPtWeightSys = 1.f;
    }
  }



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
    fJetToTauAltNum   [proc] = 1  ;
    for(int ialt = 0; ialt < kMaxAltFunc; ++ialt) {
      fJetToTauAltUp  [proc*kMaxAltFunc + ialt] = 1.f;
      fJetToTauAltDown[proc*kMaxAltFunc + ialt] = 1.f;
      fJetToTauAltDown[proc*kMaxAltFunc + ialt] = 1.f;
    }
  }

  qcdWeight = 1.f; qcdWeightUp = 1.f; qcdWeightDown = 1.f; qcdClosure = 1.f; qcdIsoScale = 1.f; qcdMassBDTScale = 1.f;
  for(int ialt = 0; ialt < kMaxAltFunc; ++ialt) {
    qcdWeightAltUp  [ialt] = 1.f;
    qcdWeightAltDown[ialt] = 1.f;
  }

  ////////////////////////////////////////////////////////////////////
  //   Generator/Embedding Weight
  ////////////////////////////////////////////////////////////////////

  embeddingWeight = 1.f; embeddingUnfoldingWeight = 1.f;
  MCGenWeight = genWeight; //store the original generator weight for theory weight variations
  if(fIsEmbed) {
    genWeight = std::fabs(genWeight);
    if(fIsLLEmbed) {
      embeddingWeight = 1.f;
      genWeight = 1.f;
    } else if(genWeight > 1.) { //undefined generation weight (must be < 1) --> remove the event
      genWeight = 0.f;
    } else if(genWeight == 1.f) {
      std::cout << "!!! HistMaker::" << __func__ << ": Entry = " << fentry << ": Unit input embedding weight = " << genWeight
                << " in event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                << std::endl;
      genWeight = 0.f;
    } else { //move the weight value to the embeddingWeight variable
      embeddingWeight = genWeight; //move the embedding weight from genWeight to embeddingWeight
      genWeight = 1.f;
    }
    //Retrieve the gen-level di-muon pair unfolding weight
    embeddingUnfoldingWeight = fEmbeddingWeight->UnfoldingWeight(zLepOnePt, zLepOneEta, zLepTwoPt, zLepTwoEta, zEta, zPt, fYear);

    //Apply an additional normalization matching to the gen-level Z->tau_x tau_y sample
    if(fUseEmbedZMatching && std::fabs(zLepOneID) > 14 && std::fabs(zLepTwoID) > 14) { //embedded tau tau pair
      const int decay_1 = (zLepOneDecayIdx >= 0 ) ? std::abs(GenPart_pdgId[zLepOneDecayIdx]) : 15; //identify the decay mode of the gen-level tau
      const int decay_2 = (zLepTwoDecayIdx >= 0 ) ? std::abs(GenPart_pdgId[zLepTwoDecayIdx]) : 15;
      float matching = 1.f;
      if(fUseEmbedZMatching == 1) { //use gen-level matching
        if       ((decay_1 == 11 && decay_2 == 13) || (decay_1 == 13 || decay_2 == 11)) { //tau_e tau_mu
          matching = (fYear == 2016) ? 1.0179f : (fYear == 2017) ? 1.0711f : 1.0859f;
        } else if((decay_1 == 11 && decay_2 == 15) || (decay_1 == 15 || decay_2 == 11)) { //tau_e tau_h (only if using MC tau ID scales)
          if(!fEmbedUseMCTauID) matching = 1.f; //tau ID scales compensate for unfolding issues
          else {
            if     (fYear == 2016) matching = 1.0133f;
            // if     (fYear == 2016) matching = (mcEra == 0) ? 1.0848f : 0.9361f; //significant run dependence to the unfolding, average is: 1.0133
            else if(fYear == 2017) matching = 1.0621f;
            else if(fYear == 2018) matching = 1.0732f;
          }
        } else if((decay_1 == 13 && decay_2 == 15) || (decay_1 == 15 || decay_2 == 13)) { //tau_mu tau_h (only if using MC tau ID scales)
          if(!fEmbedUseMCTauID) matching = 1.f; //tau ID scales compensate for unfolding issues
          else {
            if     (fYear == 2016) matching = 1.0089f;
            // if     (fYear == 2016) matching = (mcEra == 0) ? 1.0837f : 0.9286f; //significant run dependence to the unfolding, average is: 1.0089
            else if(fYear == 2017) matching = 1.0686f;
            else if(fYear == 2018) matching = 1.0735f;
          }
        }
      } else if(fUseEmbedZMatching) { //use reco-level matching (Only if going to float Embedding in the end)
        if       ((decay_1 == 11 && decay_2 == 13) || (decay_1 == 13 || decay_2 == 11)) { //tau_e tau_mu
          matching = (fYear == 2016) ? 1.05f : (fYear == 2017) ? 1.075f : 1.159f;
        } else if((decay_1 == 11 && decay_2 == 15) || (decay_1 == 15 || decay_2 == 11)) { //tau_e tau_h
          matching = (fYear == 2016) ? 1.f : (fYear == 2017) ? 1.f : 1.f;
        } else if((decay_1 == 13 && decay_2 == 15) || (decay_1 == 15 || decay_2 == 13)) { //tau_mu tau_h
          matching = (fYear == 2016) ? 1.f : (fYear == 2017) ? 1.f : 1.f;
        }
      }
      if(fVerbose > 9) printf(" Applying an additional unfolding weight of %.3f\n", matching);
      embeddingUnfoldingWeight *= matching;
    }
    //FIXME: Remove this or formalize it
    if(false) { //re-weight the embedding to have a more narrow Z peak
      const float dmass = zMass - ZMASS;
      float mass_pdf_weight = 1.f;
      if(std::fabs(dmass) < 10.f) {
	const double p_embed = Utilities::DSCB(zMass, 91.102, 1.568, 1.024, 2.390, 1.155, 2.738);
	const double p_dy    = Utilities::DSCB(zMass, 91.339, 1.071, 1.095, 2.399, 0.972, 2.598);
	mass_pdf_weight = 1.35 * p_dy / p_embed; //Leading factor accounts for the PDF normalizations
      }
      embeddingUnfoldingWeight *= mass_pdf_weight;
      if(fVerbose > 9) printf(" Applying an additional mass unfolding weight of %.3f\n", mass_pdf_weight);
    }
    eventWeight *= embeddingUnfoldingWeight*embeddingWeight;
  } else if(!fIsData) { //standard MC --> store only the sign of the generator weight
    genWeight = (genWeight < 0.) ? -1.f : 1.f;
  } else { //Data --> no weight
    genWeight = 1.f;
  }

  if(fRemoveEventWeights > 1) {
    genWeight = 1.f;
    embeddingWeight = 1.f;
    embeddingUnfoldingWeight = 1.f;
    eventWeight = 1.f;
  }

  ////////////////////////////////////////////////////////////////////
  //   MC Theory weights
  ////////////////////////////////////////////////////////////////////

  PSWeightMax = 1.f; LHEPdfWeightMax = 1.f;
  LHEScaleRWeightMax = 1.f; LHEScaleRWeightUp = 1.f; LHEScaleRWeightDown = 1.f;
  LHEScaleFWeightMax = 1.f; LHEScaleFWeightUp = 1.f; LHEScaleFWeightDown = 1.f;
  if(!fIsData && !fIsEmbed && MCGenWeight != 0.) {
    //Get the maximum event-by-event deviations

    //PS weight
    float deviation = 0.f;
    for(UInt_t index = 0; index < nPSWeight; ++index) {
      if(PSWeight[index] == 0.f) continue;
      const float dev = std::max(0.3f, std::min(3.f, PSWeight[index])) - 1.f;
      if(std::fabs(deviation) < std::fabs(dev)) {
        deviation = dev;
      }
    }
    PSWeightMax = 1.f + deviation;

    if(fIsDY) { //use the weights derived in Z (pT, mass) that don't change normalization
      //Store Z PDF/Scale uncertainties
      LHEPdfWeightMax    = fZPDFSys.GetPDFWeight        (fYear, zPt, zEta, zMass);
      LHEScaleRWeightMax = fZPDFSys.GetRenormScaleWeight(fYear, zPt,       zMass);
      LHEScaleFWeightMax = fZPDFSys.GetFactorScaleWeight(fYear, zPt,       zMass);
      fZPDFSys.GetPDFWeight(fYear, zPt, zEta, zMass, LHEPdfWeight, nLHEPdfWeight);
      LHEScaleRWeightUp    = LHEScaleRWeightMax;
      LHEScaleRWeightDown  = std::max(0.f, 2.f - LHEScaleRWeightMax);
      LHEScaleFWeightUp    = LHEScaleFWeightMax;
      LHEScaleFWeightDown  = std::max(0.f, 2.f - LHEScaleFWeightMax);
    } else { //evaluate the effect here
      //PDF weight
      deviation = 0.f;
      for(UInt_t index = 0; index < nLHEPdfWeight; ++index) {
        //If zero weights, set them to 1
        // if(LHEPdfWeight[0] != 0.f) LHEPdfWeight[0] = 1.f;
        if(LHEPdfWeight[index] == 0.f) LHEPdfWeight[index] = 1.f;
        //index 0 should be the nominal weight, so divide by it
        LHEPdfWeight[index] /= LHEPdfWeight[0];

        //apply a scale factor to preserve the cross section
        if(nLHEPdfWeight == 33 && index > 0) //assume the PDF set used in 2018
          LHEPdfWeight[index] *= fZPDFSys.GetPDFScale(2018, index);
        else if(nLHEPdfWeight == 101 && index > 0) //assume the PDF set used in 2016
          LHEPdfWeight[index] *= fZPDFSys.GetPDFScale(2016, index);
        else //ignore the systematic as it's not a know PDF set FIXME: Decide how to handle these cases
          LHEPdfWeight[index] = 1.f;

        //Ensure a reasonable weight ratio
        LHEPdfWeight[index] = std::max(0.5f, std::min(2.f, LHEPdfWeight[index]));
        const float dev = LHEPdfWeight[index] - 1.f;
        if(std::fabs(deviation) < std::fabs(dev)) {
          deviation = dev;
        }
      }
      LHEPdfWeightMax = 1.f + deviation;

      //Renormalization scale weight
      deviation = 0.f;
      if(nLHEScaleWeight >= 9) {
        //indices 1, 4, and 7 correspond to (muR, muF) = (0.5, 1), (1, 1), and (2, 1), respectively
        const float ref_wt = LHEScaleWeight[4];
        LHEScaleRWeightUp   = LHEScaleWeight[7]/ref_wt;
        LHEScaleRWeightDown = LHEScaleWeight[1]/ref_wt;
      }
      //Remove normalization effects for relevant MC
      if     (fIsHiggsBkg == 1) { LHEScaleRWeightUp /= 0.842; LHEScaleRWeightDown /= 1.209; } //ggF-H
      else if(fIsHiggsBkg == 2) { LHEScaleRWeightUp /= 1.002; LHEScaleRWeightDown /= 0.997; } //VBF-H
      else if(fIsHiggsBkg == 3) { LHEScaleRWeightUp /= 0.935; LHEScaleRWeightDown /= 1.145; } //W+-H
      else if(fIsHiggsBkg == 4) { LHEScaleRWeightUp /= 0.935; LHEScaleRWeightDown /= 1.145; } //W--H
      else if(fIsHiggsBkg == 5) { LHEScaleRWeightUp /= 0.935; LHEScaleRWeightDown /= 1.145; } //Z-H
      else if(fIsWJets)         { LHEScaleRWeightUp /= 0.996; LHEScaleRWeightDown /= 1.005; } //W+jets/W+gamma
      else if(fIsWW)            { LHEScaleRWeightUp /= 0.979; LHEScaleRWeightDown /= 1.025; } //WW
      else if(fIsWWW)           { LHEScaleRWeightUp /= 0.959; LHEScaleRWeightDown /= 1.050; } //WWW
      else if(fIsTTbar)         { LHEScaleRWeightUp /= 0.896; LHEScaleRWeightDown /= 1.114; } //ttbar
      else if(fIsDY)            { LHEScaleRWeightUp /= 0.920; LHEScaleRWeightDown /= 1.113; } //Z+jets
      LHEScaleRWeightMax = (std::fabs(LHEScaleRWeightUp-1.f) > std::fabs(LHEScaleRWeightDown-1.f)) ? LHEScaleRWeightUp : LHEScaleRWeightDown;

      //Factorization scale weight
      if(nLHEScaleWeight >= 9) {
        //indices 3, 4, and 5 correspond to (muR, muF) = (1, 0.5), (1, 1), and (1, 2), respectively
        const float ref_wt = LHEScaleWeight[4];
        LHEScaleFWeightUp   = LHEScaleWeight[5]/ref_wt;
        LHEScaleFWeightDown = LHEScaleWeight[3]/ref_wt;
      }
      //Remove normalization effects for relevant MC
      if     (fIsHiggsBkg == 1) { LHEScaleFWeightUp /= 1.022; LHEScaleFWeightDown /= 0.972; } //ggF-H
      else if(fIsHiggsBkg == 2) { LHEScaleFWeightUp /= 1.006; LHEScaleFWeightDown /= 0.999; } //VBF-H
      else if(fIsHiggsBkg == 3) { LHEScaleFWeightUp /= 1.030; LHEScaleFWeightDown /= 0.929; } //W+-H
      else if(fIsHiggsBkg == 4) { LHEScaleFWeightUp /= 1.030; LHEScaleFWeightDown /= 0.929; } //W--H
      else if(fIsHiggsBkg == 5) { LHEScaleFWeightUp /= 1.030; LHEScaleFWeightDown /= 0.929; } //Z-H
      else if(fIsWJets)         { LHEScaleFWeightUp /= 1.111; LHEScaleFWeightDown /= 0.876; } //W+jets
      else if(fIsWW)            { LHEScaleFWeightUp /= 1.012; LHEScaleFWeightDown /= 0.986; } //WW
      else if(fIsWWW)           { LHEScaleFWeightUp /= 0.989; LHEScaleFWeightDown /= 1.013; } //WWW
      else if(fIsTTbar)         { LHEScaleFWeightUp /= 0.980; LHEScaleFWeightDown /= 1.027; } //ttbar
      else if(fIsDY)            { LHEScaleFWeightUp /= 1.062; LHEScaleFWeightDown /= 0.928; } //Z+jets
      LHEScaleFWeightMax = (std::fabs(LHEScaleFWeightUp-1.f) > std::fabs(LHEScaleFWeightDown-1.f)) ? LHEScaleFWeightUp : LHEScaleFWeightDown;
    }
  }

  ////////////////////////////////////////////////////////////////////
  //   Signal Z info + re-weight
  ////////////////////////////////////////////////////////////////////

  if(fIsSignal && fDataset.Contains("Z")) {
    //re-weight to MC spectrum
    if(fUseSignalZWeights != 2) //2 = use ntuple input
      signalZWeight = (fUseSignalZWeights) ? fSignalZWeight.GetWeight(fYear, zPt, zMass) : 1.f;
    //re-weight to remove z/gamma* effect
    signalZMixingWeightUp = 1.f; signalZMixingWeightDown = 1.f;
    signalZMixingWeight = (fUseSignalZMixWeights) ? fSignalZMixWeight.GetWeight(fYear, zPt, zMass, signalZMixingWeightUp, signalZMixingWeightDown) : 1.f;
    //re-weight MC spectrum to data spectrum
    if(fUseZPtWeight != 2) //2 = use ntuple input
      zPtWeight = (fUseZPtWeight) ? fZPtWeight->GetWeight(fYear, zPt, zMass, false /*Use Gen level weights*/, zPtWeightUp, zPtWeightDown, zPtWeightSys) : 1.f;
    else if(fUseZPtWeight == 2) { //initialize down using up value
      zPtWeightDown = std::min(10.f, std::max(1.e-6f, 2.f*zPtWeight - zPtWeightUp)); //down = weight - (up - weight)
      zPtWeightSys = zPtWeightUp;
    }
    eventWeight *= zPtWeight*signalZWeight*signalZMixingWeight;
    if(fVerbose > 0) std::cout << " For Z pT = " << zPt << " and Mass = " << zMass << " using Data/MC weight " << zPtWeight
                               << ", signal Z weight " << signalZWeight
                               << ", and signal Z mixing weight " << signalZMixingWeight
                               << " (" << signalZMixingWeightUp << "/" << signalZMixingWeightDown
                               << ") --> event weight = " << eventWeight << std::endl;

    //Store signal Z PDF/Scale uncertainties
    LHEPdfWeightMax    = fZPDFSys.GetPDFWeight        ( 2018, zPt, zEta, zMass); //use 2018 PDF uncertainties for all years
    LHEScaleRWeightMax = fZPDFSys.GetRenormScaleWeight(fYear, zPt,       zMass);
    LHEScaleFWeightMax = fZPDFSys.GetFactorScaleWeight(fYear, zPt,       zMass);
    fZPDFSys.GetPDFWeight(2018, zPt, zEta, zMass, LHEPdfWeight, nLHEPdfWeight);  //use 2018 PDF uncertainties for all years
    LHEScaleRWeightUp    = LHEScaleRWeightMax;
    LHEScaleRWeightDown  = std::max(0.f, 2.f - LHEScaleRWeightMax);
    LHEScaleFWeightUp    = LHEScaleFWeightMax;
    LHEScaleFWeightDown  = std::max(0.f, 2.f - LHEScaleFWeightMax);
  }

  ////////////////////////////////////////////////////////////////////
  //   Z info + re-weight
  ////////////////////////////////////////////////////////////////////

  //Z pT/mass info (DY and Z signals only)
  if(fIsDY && !fIsEmbed) {
    //re-weight the Z pt vs mass spectrum
    if(fUseZPtWeight != 2) //2 = use ntuple input
      zPtWeight = (fUseZPtWeight) ? fZPtWeight->GetWeight(fYear, zPt, zMass, false /*Use Gen level weights*/, zPtWeightUp, zPtWeightDown, zPtWeightSys) : 1.f;
    eventWeight *= zPtWeight;
    if(fVerbose > 0) std::cout << " For Z pT = " << zPt << " and Mass = " << zMass << " using Data/MC weight " << zPtWeight
                               << "--> event weight = " << eventWeight << std::endl;
  }

  ////////////////////////////////////////////////////////////////////
  //   Lepton displacement weights
  ////////////////////////////////////////////////////////////////////

  if((emu || ((ee || mumu) && fSameFlavorEMuSelec)) && !lep_tau && fUseLepDisplacementWeights && !fIsData && !fIsEmbed) {
    lepDisplacementWeight  = fLeptonDisplacement.Weight(leptonOne.pt, leptonOne.eta, leptonOne.isMuon(), fYear) *
      fLeptonDisplacement.Weight(leptonTwo.pt, leptonTwo.eta, leptonTwo.isMuon(), fYear);
  } else {
    lepDisplacementWeight = 1.f;
  }
  eventWeight *= lepDisplacementWeight;


  ////////////////////////////////////////////////////////////////////
  //   BDT scale factor weights
  ////////////////////////////////////////////////////////////////////

  //only apply to the emu-like selections
  if((emu || ((ee || mumu) && fSameFlavorEMuSelec)) && !lep_tau && fUseBDTScale && !fIsData && !fIsEmbed) {
    bdtWeight = fBDTScale.Weight(fMvaOutputs[fMVAConfig->GetIndexBySelection("zemu")], fYear);
  } else {
    bdtWeight = 1.f;
  }
  eventWeight *= bdtWeight;

  //only evaluate embedding BDT uncertainty on tau channel embedding samples
  embBDTWeight = 1.f;
  if(fIsEmbed && fUseEmbBDTUncertainty && fSelection.Contains("tau")) {
    embBDTWeight = EvalEmbBDTUncertainty("z"+fSelection);
    //apply as a correction if requested
    if(fUseEmbBDTUncertainty == 2) eventWeight *= embBDTWeight;
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
        puWeight = fPUWeight->GetWeight(nPU, fYear, puWeight_up, puWeight_down);
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
    if(fUseJetPUIDWeights == 1) { //calculate the PU ID weight locally
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
      btagWeight = fBTagWeight->GetWeight(wp, fYear, nJets20, jetsPt, jetsEta, jetsFlavor, jetsBTag, btagWeightUpBC, btagWeightDownBC, btagWeightUpL, btagWeightDownL);
      eventWeight *= btagWeight;
      if((btagWeight - btagWeightUpBC) * (btagWeight - btagWeightDownBC) > 1.e-3 && fVerbose > 0) {
        std::cout << "!!! Warning: Entry " << fentry << " B-tag weight up/down are on the same side of the weight: Wt = "
                  << btagWeight << " Up = " << btagWeightUpBC << " Down = " << btagWeightDownBC << std::endl;
      }
    } else if(fUseBTagWeights == 2) { //loop over the jet collection with the ntuple-level weights
      int ncount = 0; //track the number of jets used in the weight
      for(unsigned ijet = 0; ijet < nJets20; ++ijet) {
        if(std::fabs(Jet_eta[ijet]) >= 2.4f) continue; //b-tagging region
        if(std::fabs(Jet_pt [ijet]) <= 20.f) continue; //b-tag jet pt cut
        ++ncount;
        const float wt      = (wp == BTagWeight::kLooseBTag) ? Jet_btagSF_L     [ijet] : Jet_btagSF_T     [ijet];
        const float wt_up   = (wp == BTagWeight::kLooseBTag) ? Jet_btagSF_L_up  [ijet] : Jet_btagSF_T_up  [ijet];
        const float wt_down = (wp == BTagWeight::kLooseBTag) ? Jet_btagSF_L_down[ijet] : Jet_btagSF_T_down[ijet];
        //split uncertainties by gen-level parton flavor
        const bool  isbc    = std::fabs(Jet_partonFlavour[ijet]) == 4 || std::fabs(Jet_partonFlavour[ijet]) == 5;
        btagWeight       *= wt;
        btagWeightUpBC   *= ( isbc) ? wt_up   : wt;
        btagWeightDownBC *= ( isbc) ? wt_down : wt;
        btagWeightUpL    *= (!isbc) ? wt_up   : wt;
        btagWeightDownL  *= (!isbc) ? wt_down : wt;
      }
      eventWeight *= btagWeight;
      if((btagWeightUpBC - btagWeight)*(btagWeightDownBC - btagWeight) > 1.e-6) { //weights are in the same direction
        printf("HistMaker::%s: Warning! Entry %lld: B-tag gen b/c weight has up/down in same direction: wt = %.4f, up = %.4f, down = %.4f, nJets = %i\n",
               __func__, fentry, btagWeight, btagWeightUpBC, btagWeightDownBC, ncount);
      }
      if((btagWeightUpL - btagWeight)*(btagWeightDownL - btagWeight) > 1.e-6) { //weights are in the same direction
        printf("HistMaker::%s: Warning! Entry %lld: B-tag gen light weight has up/down in same direction: wt = %.4f, up = %.4f, down = %.4f, nJets = %i\n",
               __func__, fentry, btagWeight, btagWeightUpL, btagWeightDownL, ncount);
      }
    }
  }

  ////////////////////////////////////////////////////////////////////
  //   Lepton ID weights
  ////////////////////////////////////////////////////////////////////

  if(fApplyLeptonIDWt) {
    if(fApplyLeptonIDWt == 1) { //apply locally evaluated weights
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
        //Lepton 2
        if     (leptonTwo.isElectron()) fEmbeddingTnPWeight.ElectronIDWeight(leptonTwo.p4->Pt(), leptonTwo.scEta, fYear,
                                                                             leptonTwo.wt1[0], leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                                             leptonTwo.wt3[0], leptonTwo.wt3[1], leptonTwo.wt3[2],
                                                                             leptonTwo.isLoose, mcEra);
        else if(leptonTwo.isMuon    ()) fEmbeddingTnPWeight.MuonIDWeight(leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear,
                                                                         leptonTwo.wt1[0], leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                                         leptonTwo.wt2[0], leptonTwo.wt2[1], leptonTwo.wt2[2],
                                                                         leptonTwo.isLoose, mcEra);
        //Apply the MC electron reco ID/uncertainty to the embedding electrons
        Lepton_t tmp_lep;
        if     (leptonOne.isElectron()) {
          tmp_lep.pt    = leptonOne.pt;
          tmp_lep.scEta = leptonOne.scEta;
          fElectronIDWeight.IDWeight(tmp_lep, fYear);
          leptonOne.wt2[0] = tmp_lep.wt2[0];
          leptonOne.wt2[1] = tmp_lep.wt2[1];
          leptonOne.wt2[2] = tmp_lep.wt2[2];
        }
        if     (leptonTwo.isElectron()) {
          tmp_lep.pt    = leptonTwo.pt;
          tmp_lep.scEta = leptonTwo.scEta;
          fElectronIDWeight.IDWeight(tmp_lep, fYear);
          leptonTwo.wt2[0] = tmp_lep.wt2[0];
          leptonTwo.wt2[1] = tmp_lep.wt2[1];
          leptonTwo.wt2[2] = tmp_lep.wt2[2];
        }

      } else if(!fIsData) { //MC simulations
        //Lepton 1
        if     (leptonOne.isElectron()) fElectronIDWeight.IDWeight(leptonOne, fYear);
        else if(leptonOne.isMuon    ()) fMuonIDWeight.IDWeight    (leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear, mcEra,
                                                                   leptonOne.wt1[0] , leptonOne.wt1[1], leptonOne.wt1[2], //muon ID
                                                                   leptonOne.wt2[0] , leptonOne.wt2[1], leptonOne.wt2[2]); //muon Iso ID
        //Lepton 2
        if     (leptonTwo.isElectron()) fElectronIDWeight.IDWeight(leptonTwo, fYear);
        else if(leptonTwo.isMuon    ()) fMuonIDWeight.IDWeight    (leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear, mcEra,
                                                                   leptonTwo.wt1[0] , leptonTwo.wt1[1], leptonTwo.wt1[2],
                                                                   leptonTwo.wt2[0] , leptonTwo.wt2[1], leptonTwo.wt2[2]);
      }
    } //end fApplyLeptonIDWt == 1
    else if(fApplyLeptonIDWt == 2 && !fIsData) { //ntuple-level ID corrections (only electrons/muons)
      if(leptonOne.isElectron()) {
        leptonOne.wt1[0] = Electron_ID_wt         [leptonOne.index];
        leptonOne.wt1[1] = Electron_ID_up         [leptonOne.index];
        leptonOne.wt1[2] = Electron_ID_down       [leptonOne.index];
        leptonOne.wt2[0] = Electron_RecoID_wt     [leptonOne.index];
        leptonOne.wt2[1] = Electron_RecoID_up     [leptonOne.index];
        leptonOne.wt2[2] = Electron_RecoID_down   [leptonOne.index];
        leptonOne.wt3[0] = Electron_IsoID_wt      [leptonOne.index];
        leptonOne.wt3[1] = Electron_IsoID_up      [leptonOne.index];
        leptonOne.wt3[2] = Electron_IsoID_down    [leptonOne.index];
      } else if(leptonOne.isMuon()) {
        leptonOne.wt1[0] = Muon_ID_wt             [leptonOne.index];
        leptonOne.wt1[1] = Muon_ID_up             [leptonOne.index];
        leptonOne.wt1[2] = Muon_ID_down           [leptonOne.index];
        leptonOne.wt2[0] = Muon_IsoID_wt          [leptonOne.index];
        leptonOne.wt2[1] = Muon_IsoID_up          [leptonOne.index];
        leptonOne.wt2[2] = Muon_IsoID_down        [leptonOne.index];
      }

      if(leptonTwo.isElectron()) {
        leptonTwo.wt1[0] = Electron_ID_wt         [leptonTwo.index];
        leptonTwo.wt1[1] = Electron_ID_up         [leptonTwo.index];
        leptonTwo.wt1[2] = Electron_ID_down       [leptonTwo.index];
        leptonTwo.wt2[0] = Electron_RecoID_wt     [leptonTwo.index];
        leptonTwo.wt2[1] = Electron_RecoID_up     [leptonTwo.index];
        leptonTwo.wt2[2] = Electron_RecoID_down   [leptonTwo.index];
        leptonTwo.wt3[0] = Electron_IsoID_wt      [leptonTwo.index];
        leptonTwo.wt3[1] = Electron_IsoID_up      [leptonTwo.index];
        leptonTwo.wt3[2] = Electron_IsoID_down    [leptonTwo.index];
      } else if(leptonTwo.isMuon()) {
        leptonTwo.wt1[0] = Muon_ID_wt             [leptonTwo.index];
        leptonTwo.wt1[1] = Muon_ID_up             [leptonTwo.index];
        leptonTwo.wt1[2] = Muon_ID_down           [leptonTwo.index];
        leptonTwo.wt2[0] = Muon_IsoID_wt          [leptonTwo.index];
        leptonTwo.wt2[1] = Muon_IsoID_up          [leptonTwo.index];
        leptonTwo.wt2[2] = Muon_IsoID_down        [leptonTwo.index];
      }
    } //end fApplyLeptonIDWt == 2 && !fIsData

    //Taus use the same correction object whether embedding or MC (it's initialized with a flag for embedding vs MC)
    //Also always uses locally evaluated weights
    if(leptonOne.isTau     ()) leptonOne.wt1[0] = fTauIDWeight->IDWeight(leptonOne.p4->Pt(), leptonOne.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                         fYear, leptonOne.wt1[1], leptonOne.wt1[2], leptonOne.wt1_bin);
    if(leptonTwo.isTau     ()) leptonTwo.wt1[0] = fTauIDWeight->IDWeight(leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), tauGenID, tauDeepAntiJet,
                                                                         fYear, leptonTwo.wt1[1], leptonTwo.wt1[2], leptonTwo.wt1_bin);

    //if using MC Tau ID weights for embedding taus, apply MC / DY reco efficiency correction
    if(fIsEmbed && fEmbedUseMCTauID) {
      const float mc_over_embed_eff = (fYear == 2016) ? 0.975f : (fYear == 2017) ? 0.975f : 0.95f;
      if(leptonOne.isTau()) {
        leptonOne.wt1[0] *= mc_over_embed_eff;
        leptonOne.wt1[1] *= mc_over_embed_eff;
        leptonOne.wt1[2] *= mc_over_embed_eff;
      }
      if(leptonTwo.isTau()) {
        leptonTwo.wt1[0] *= mc_over_embed_eff;
        leptonTwo.wt1[1] *= mc_over_embed_eff;
        leptonTwo.wt1[2] *= mc_over_embed_eff;
      }
    }

  } // end fApplyLeptonIDWt

  if(fRemoveEventWeights) {
    for(int i = 0; i < 3; ++i) {
      leptonOne.wt1[i] = 1.f;
      leptonOne.wt2[i] = 1.f;
      leptonOne.wt3[i] = 1.f;
      leptonTwo.wt1[i] = 1.f;
      leptonTwo.wt2[i] = 1.f;
      leptonTwo.wt3[i] = 1.f;
    }
  }

  eventWeight *= leptonOne.wt1[0]*leptonOne.wt2[0]*leptonOne.wt3[0];
  eventWeight *= leptonTwo.wt1[0]*leptonTwo.wt2[0]*leptonTwo.wt3[0];

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
                                           leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone, fTreeVars.lepm,
                                           fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        } else {
          fMuonJetToTauSSComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                             leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone, fTreeVars.lepm,
                                             fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        }
      } else {
        if(chargeTest) {
          fElectronJetToTauComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                               leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone, fTreeVars.lepm,
                                               fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        } else {
          fElectronJetToTauSSComp.GetComposition(leptonTwo.p4->Pt(), fTreeVars.twometdeltaphi, fTreeVars.mttwo, tauDecayMode,
                                                 leptonOne.p4->Pt(), fTreeVars.onemetdeltaphi, fTreeVars.mtone, fTreeVars.lepm,
                                                 fYear, fJetToTauComps, fJetToTauCompsUp, fJetToTauCompsDown);
        }
      }
      for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
        if(mutau) {
          fJetToTauWts[proc]   = (fMuonJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                            leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                            fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.leppt, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(), fMvaUse[1],
                                                                            &fJetToTauAltUp[proc*kMaxAltFunc], &fJetToTauAltDown[proc*kMaxAltFunc], fJetToTauAltNum[proc],
                                                                            fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                            fJetToTauBiases[proc]));
          fJetToTauMCWts[proc] = (fMuonJetToTauMCWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                              leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                              fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.leppt, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(), fMvaUse[1],
                                                                              fJetToTauMCCorrs[proc], fJetToTauMCBiases[proc]));
        } else {
          fJetToTauWts[proc]   = (fElectronJetToTauWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                                leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                                fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.leppt, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(), fMvaUse[3],
                                                                                &fJetToTauAltUp[proc*kMaxAltFunc], &fJetToTauAltDown[proc*kMaxAltFunc], fJetToTauAltNum[proc],
                                                                                fJetToTauCorrs[proc], jetToTauWeightCorrUp, jetToTauWeightCorrDown,
                                                                                fJetToTauBiases[proc]));
          fJetToTauMCWts[proc] = (fElectronJetToTauMCWeights[proc]->GetDataFactor(tauDecayMode, fYear, leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), leptonOne.p4->Pt(),
                                                                                  leptonOne.p4->DeltaR(*leptonTwo.p4),
                                                                                  fTreeVars.onemetdeltaphi, fTreeVars.lepm, fTreeVars.leppt, fTreeVars.mtlep, leptonOne.iso/leptonOne.p4->Pt(), fMvaUse[3],
                                                                                  fJetToTauMCCorrs[proc], fJetToTauMCBiases[proc]));
        }
        //Turn off SS --> OS bias if in the SS region
        if(!chargeTest && proc == JetToTauComposition::kQCD) {
          fJetToTauBiases[proc] = 1.f;
        }

        // if(proc == JetToTauComposition::kTop && fJetToTauBiases[proc] != 1.f) {
        //   printf("%s: Bias found in j-->tau top estimation! Bias = %.4f\n", __func__, fJetToTauBiases[proc]);
        //   throw 20;
        // }
        //approximate the statistical uncertainty components into a total up/down uncertainty
        fJetToTauWtsUp  [proc] = 0.f;
        fJetToTauWtsDown[proc] = 0.f;
        for(int ialt = 0; ialt < kMaxAltFunc; ++ialt) {
          float diff = fJetToTauWts[proc] - fJetToTauAltUp[proc*kMaxAltFunc + ialt];
          fJetToTauWtsUp[proc] += std::pow(diff, 2);
          diff = fJetToTauWts[proc] - fJetToTauAltDown[proc*kMaxAltFunc + ialt];
          fJetToTauWtsDown[proc] += std::pow(diff, 2);
        }
        fJetToTauWtsUp  [proc] = std::max(1.e-5f, std::min(3.f, fJetToTauWts[proc] + std::sqrt(fJetToTauWtsUp  [proc])));
        fJetToTauWtsDown[proc] = std::max(1.e-5f, std::min(3.f, fJetToTauWts[proc] - std::sqrt(fJetToTauWtsDown[proc])));
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
        if(fApplyJetToTauMCBias || (proc != JetToTauComposition::kWJets && proc != JetToTauComposition::kZJets)) {
          //weight with the correction and bias for this process
          jetToTauWeightBias      += fJetToTauComps    [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
          jetToTauWeight_compUp   += fJetToTauCompsUp  [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
          jetToTauWeight_compDown += fJetToTauCompsDown[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
        } else {
          //weight with the correction ignoring bias for this process
          jetToTauWeightBias      += fJetToTauComps    [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
          jetToTauWeight_compUp   += fJetToTauCompsUp  [proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
          jetToTauWeight_compDown += fJetToTauCompsDown[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
        }
        //store systematic effects, added linearly across all processes
        jttUp   += fJetToTauComps[proc] * fJetToTauWtsUp[proc];
        jttDown += fJetToTauComps[proc] * fJetToTauWtsDown[proc];
      }
      jetToTauWeightUp   = jttUp  ; //all statistical uncertainties fluctuated together
      jetToTauWeightDown = jttDown;
      jetToTauWeightCorrUp   = jetToTauWeight; //set correction up to be ignoring the correction
      jetToTauWeightCorrDown = std::max(0.f, 2.f*jetToTauWeightCorr - jetToTauWeight); //size of the weight in the other direction from 1
      jetToTauWeightBiasUp   = jetToTauWeightCorr; //set correction up to be ignoring the correction
      jetToTauWeightBiasDown = jetToTauWeightBias*jetToTauWeightBias/jetToTauWeightCorr; //set to applying it twice
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

  //If using MC fake taus, only use QCD j-->tau weights (mode 1) or remove j-->tau weights (mode 2)
  if(fUseMCFakeTau) {
    //QCD j-->tau weights
    if(fUseMCFakeTau == 1) {
      const int proc = JetToTauComposition::kQCD;
      jetToTauWeight           = fJetToTauWts[proc];
      jetToTauWeightUp         = jetToTauWeight; //ignore uncertainties
      jetToTauWeightDown       = jetToTauWeight;
      jetToTauWeightCorr       = fJetToTauWts[proc] * fJetToTauCorrs[proc];
      jetToTauWeightCorrUp     = jetToTauWeightCorr; //ignore uncertainties
      jetToTauWeightCorrDown   = jetToTauWeightCorr;
      jetToTauWeightBias       = fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
      jetToTauWeightBiasUp     = jetToTauWeightBias; //ignore uncertainties
      jetToTauWeightBiasDown   = jetToTauWeightBias;
      jetToTauWeight_compUp    = jetToTauWeightBias; //ignore uncertainties
      jetToTauWeight_compDown  = jetToTauWeightBias;
      for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) { //set composition fraction to 0 for non-QCD
        fJetToTauComps    [iproc] = (iproc == proc) ? 1.f : 0.f;
        fJetToTauCompsUp  [iproc] = (iproc == proc) ? 1.f : 0.f;
        fJetToTauCompsDown[iproc] = (iproc == proc) ? 1.f : 0.f;
      }
    }
    //no j-->tau weights
    if(fUseMCFakeTau == 2) {
      jetToTauWeight           = 1.f;
      jetToTauWeightUp         = 1.f;
      jetToTauWeightDown       = 1.f;
      jetToTauWeightCorr       = 1.f;
      jetToTauWeightCorrUp     = 1.f;
      jetToTauWeightCorrDown   = 1.f;
      jetToTauWeightBias       = 1.f;
      jetToTauWeightBiasUp     = 1.f;
      jetToTauWeightBiasDown   = 1.f;
      jetToTauWeight_compUp    = 1.f;
      jetToTauWeight_compDown  = 1.f;
    }
  }

  //only use W+jets j-->tau weights
  if(fUseWJetsJetToTau == 1) {
    const int proc = JetToTauComposition::kWJets;
    jetToTauWeight           = fJetToTauWts[proc];
    jetToTauWeightUp         = jetToTauWeight; //ignore uncertainties
    jetToTauWeightDown       = jetToTauWeight;
    jetToTauWeightCorr       = fJetToTauWts[proc] * fJetToTauCorrs[proc];
    jetToTauWeightCorrUp     = jetToTauWeightCorr; //ignore uncertainties
    jetToTauWeightCorrDown   = jetToTauWeightCorr;
    jetToTauWeightBias       = fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
    jetToTauWeightBiasUp     = jetToTauWeightBias; //ignore uncertainties
    jetToTauWeightBiasDown   = jetToTauWeightBias;
    jetToTauWeight_compUp    = jetToTauWeightBias; //ignore uncertainties
    jetToTauWeight_compDown  = jetToTauWeightBias;
    for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) { //set composition fraction to 0 for non-W+Jets
      fJetToTauComps    [iproc] = (iproc == proc) ? 1.f : 0.f;
      fJetToTauCompsUp  [iproc] = (iproc == proc) ? 1.f : 0.f;
      fJetToTauCompsDown[iproc] = (iproc == proc) ? 1.f : 0.f;
    }
  }

  if(fRemoveEventWeights > 1) {
    jetToTauWeight           = 1.f;
    jetToTauWeightUp         = 1.f;
    jetToTauWeightDown       = 1.f;
    jetToTauWeightCorr       = 1.f;
    jetToTauWeightCorrUp     = 1.f;
    jetToTauWeightCorrDown   = 1.f;
    jetToTauWeightBias       = 1.f;
    jetToTauWeightBiasUp     = 1.f;
    jetToTauWeightBiasDown   = 1.f;
    jetToTauWeight_compUp    = 1.f;
    jetToTauWeight_compDown  = 1.f;
  }

  eventWeight *= jetToTauWeightBias;

  //Test that the j-->tau weights are sensible
  if((mutau || etau) && isLooseTau) {
    {
      const float test_jtt_wt = EvalJetToTauStatSys(JetToTauComposition::kLast, 1, 0, true);
      if(std::fabs(eventWeight*genWeight - test_jtt_wt) > 1.e-5f) {
        printf("HistMaker::%s: Entry %lld: j-->tau stat sys implementation is wrong: wt = %.4f, eval = %.4f\n",
               __func__, fentry, eventWeight*genWeight, test_jtt_wt);
        throw std::runtime_error("Bad j-->tau Stat Sys test");
      }
    }
    {
      const float test_jtt_wt = EvalJetToTauNCSys(JetToTauComposition::kLast, true);
      if(std::fabs(eventWeight*genWeight - test_jtt_wt) > 1.e-5f) {
        printf("HistMaker::%s: Entry %lld: j-->tau NC sys implementation is wrong: wt = %.4f, eval = %.4f\n",
               __func__, fentry, eventWeight*genWeight, test_jtt_wt);
        throw std::runtime_error("Bad j-->tau Stat Sys test");
      }
    }
    {
      const float test_jtt_wt = EvalJetToTauBiasSys(JetToTauComposition::kLast, true);
      if(std::fabs(eventWeight*genWeight - test_jtt_wt) > 1.e-5f) {
        printf("HistMaker::%s: Entry %lld: j-->tau Bias sys implementation is wrong: wt = %.4f, eval = %.4f\n",
               __func__, fentry, eventWeight*genWeight, test_jtt_wt);
        throw std::runtime_error("Bad j-->tau Stat Sys test");
      }
    }
  }

  ///////////////////////
  // SS --> OS weights //
  ///////////////////////

  //get scale factor for same sign --> opposite sign, apply to MC and Data same-sign events
  if(emu && !chargeTest && fUseQCDWeights) {
    qcdWeight = fQCDWeight->GetWeight(fTreeVars.lepdeltar, fTreeVars.lepdeltaphi, fTreeVars.leponeeta, fTreeVars.leponept, fTreeVars.leptwopt,
                                      fTreeVars.lepm, (mutau_e) ? fMvaUse[7] : (etau_mu) ? fMvaUse[9] : -2.f,
                                      fYear, nJets20, isLooseMuon, qcdClosure, qcdIsoScale, qcdMassBDTScale, qcdWeightAltUp, qcdWeightAltDown, qcdWeightAltNum);
    float up_diff(0.f), down_diff(0.f);
    for(int ialt = 0; ialt < std::min(qcdWeightAltNum, (int) kMaxAltFunc); ++ialt) {
      up_diff   += std::pow(qcdWeight - qcdWeightAltUp  [ialt], 2);
      down_diff += std::pow(qcdWeight - qcdWeightAltDown[ialt], 2);
    }
    qcdWeightUp   = qcdWeight + std::sqrt(up_diff);
    qcdWeightDown = std::max(1.e-5f, qcdWeight - std::sqrt(down_diff));
  }

  if(fRemoveEventWeights > 1) {
    qcdWeight     = 1.f;
    qcdWeightUp   = 1.f;
    qcdWeightDown = 1.f;
  }

  eventWeight *= qcdWeight;

  ///////////////////////////////
  // MVA training re-weighting //
  ///////////////////////////////

  //account for events split off for MVA training
  if(fRemoveTraining && fTreeVars.train < 0.5f) {
    eventWeight *= (fTreeVars.trainfraction > 0.f && fTreeVars.trainfraction < 1.f) ? 1.f/(1.f - fTreeVars.trainfraction) : 1.f;
  }

  //initialize weights for the tree variables
  fTreeVars.eventweight = genWeight*eventWeight;
  fTreeVars.eventweightMVA = genWeight*eventWeight;
  fTreeVars.fulleventweight = genWeight*eventWeight*fXsec;
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
      //FIXME: Only apply process bias when applicable (see fApplyJetToTauMCBias use)
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

  //if splitting testing/training samples, remove training and re-scale the event weight to account for lost events to training
  //fTreeVars.trainfraction is 0 for events not eligible for training (e.g. signal region data)
  if(!fRemoveTraining)
    fTreeVars.eventweightMVA *= (fTreeVars.train > 0.f) ? 0.f : 1.f/(1.f-fTreeVars.trainfraction);
}

//--------------------------------------------------------------------------------------------------------------
// Set kinematics relevant to MVAs, to re-evaluate scores after shifted values etc.
void HistMaker::SetKinematics() {
  // //FIXME: Remove this line turning off the MET
  // met = 0.f;
  // metPhi = 0.f;

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
  fTreeVars.lepphi        = lep.Phi();
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
  fTreeVars.pttrailoverlead = (fTreeVars.leponept > fTreeVars.leptwopt) ? fTreeVars.leptwopt / fTreeVars.leponept : fTreeVars.leponept / fTreeVars.leptwopt;

  //Tau trk variables
  TLorentzVector trk;
  if(leptonTwo.isTau()) trk.SetPtEtaPhiM(leptonTwo.trkpt, leptonTwo.trketa, leptonTwo.trkphi, TAUMASS);
  else                  trk.SetPtEtaPhiM(leptonTwo.pt, leptonTwo.eta, leptonTwo.phi, leptonTwo.mass);
  // ROOT::Math::PtEtaPhiM trk;
  // if(leptonTwo.isTau()) trk.SetCoordinates(leptonTwo.trkpt, leptonTwo.trketa, leptonTwo.trkphi, TAUMASS);
  // else                  trk.SetCoordinates(leptonTwo.pt, leptonTwo.eta, leptonTwo.phi, leptonTwo.mass);
  // if(trk.M2() < 0.)     trk.SetE(trk.P());
  fTreeVars.trkpt   = trk.Pt();
  fTreeVars.trketa  = trk.Eta();
  fTreeVars.trkphi  = trk.Phi();
  fTreeVars.leptrkm = (*leptonOne.p4 + trk).M();
  fTreeVars.leptrkdeltam = fTreeVars.lepm - fTreeVars.leptrkm;
  fTreeVars.trkptoverpt  = fTreeVars.trkpt / leptonTwo.p4->Pt();

  //phi differences
  fTreeVars.htdeltaphi     = std::fabs(Utilities::DeltaPhi(lep.Phi(), htPhi));
  fTreeVars.metdeltaphi    = Utilities::DeltaPhi(lep.Phi(), metPhi); //FIXME: Using signed dphi in XGBoost BDT case
  if(!fUseXGBoostBDT) fTreeVars.metdeltaphi = std::fabs(fTreeVars.metdeltaphi);
  fTreeVars.leponedeltaphi = std::fabs(Utilities::DeltaPhi(leptonOne.p4->Phi(), lep.Phi()));
  fTreeVars.leptwodeltaphi = std::fabs(Utilities::DeltaPhi(leptonTwo.p4->Phi(), lep.Phi()));
  fTreeVars.onemetdeltaphi = std::fabs(Utilities::DeltaPhi(leptonOne.p4->Phi(), metPhi));
  fTreeVars.twometdeltaphi = std::fabs(Utilities::DeltaPhi(leptonTwo.p4->Phi(), metPhi));

  //MET variables
  fTreeVars.met        = met;
  //FIXME: Decide on met significance definition
  fTreeVars.metsignificance = metSignificance;
  //FIXME: How is met significance defined on data?
  // const float met_err   = std::sqrt(std::pow(puppMETJERUp - puppMET, 2) + std::pow(puppMETJESUp - puppMET, 2))/puppMET;
  // puppMETSig = (met_err > 0.f) ? puppMET/met_err : -1.f;
  puppMETSig = (puppMETSumEt > 0.f) ? puppMET/std::sqrt(puppMETSumEt) : -1.f;

  fTreeVars.mtone      = std::sqrt(2.*met*leptonOne.p4->Pt()*(1.-cos(leptonOne.p4->Phi() - metPhi)));
  fTreeVars.mttwo      = std::sqrt(2.*met*leptonTwo.p4->Pt()*(1.-cos(leptonTwo.p4->Phi() - metPhi)));
  fTreeVars.mtlep      = std::sqrt(2.*met*lep.Pt()*(1.-cos(lep.Phi() - metPhi)));
  fTreeVars.mtlead     = (fTreeVars.leponept > fTreeVars.leptwopt) ? fTreeVars.mtone : fTreeVars.mttwo;
  fTreeVars.mttrail    = (fTreeVars.leponept > fTreeVars.leptwopt) ? fTreeVars.mttwo : fTreeVars.mtone;
  fTreeVars.mtdiff     = fTreeVars.mtone - fTreeVars.mttwo;
  fTreeVars.mtoneoverm = fTreeVars.mtone / fTreeVars.lepm;
  fTreeVars.mttwooverm = fTreeVars.mttwo / fTreeVars.lepm;
  fTreeVars.mtlepoverm = fTreeVars.mtlep / fTreeVars.lepm;

  //momentum projections onto bisector
  TVector3 lp1 = leptonOne.p4->Vect();
  TVector3 lp2 = leptonTwo.p4->Vect();
  TVector3 missing(met*std::cos(metPhi), met*std::sin(metPhi), 0.);
  if(!std::isfinite(missing.Pt()) || !std::isfinite(met) || !std::isfinite(metPhi)) {
    printf("!!! HistMaker::%s: Entry: %12lld: MET vector is undefined! met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f\n",
           __func__, fentry, met, metPhi, metCorr, metCorrPhi);
  }
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector.Mag() > 0.) bisector.SetMag(1.);

  // Reflect across the Z-beam plane
  // Beam = +-z-axis
  // Z = 3-momentum vector of Z candidate
  // phi(Z) = phi0
  // dphi(mu) = phi(mu) - phi0
  // transform phi(mu) -> phi(mu) - 2*dphi(mu)
  {
    TLorentzVector zlepone, zleptwo;
    zlepone.SetPtEtaPhiM(leptonOne.pt, leptonOne.eta, leptonOne.phi, leptonOne.mass);
    zleptwo.SetPtEtaPhiM(leptonTwo.pt, leptonTwo.eta, leptonTwo.phi, leptonTwo.mass);
    TLorentzVector system = *leptonOne.p4 + *leptonTwo.p4;
    const double z_phi(system.Phi());
    const double lep1_dphi_z = Utilities::DeltaPhi(leptonOne.phi, z_phi);
    const double lep2_dphi_z = Utilities::DeltaPhi(leptonTwo.phi, z_phi);
    zlepone.SetPhi(Utilities::DeltaPhi(zlepone.Phi(), 2*lep1_dphi_z));
    zleptwo.SetPhi(Utilities::DeltaPhi(zleptwo.Phi(), 2*lep2_dphi_z));
    TVector3 zlp1 = zlepone.Vect();
    TVector3 zlp2 = zleptwo.Vect();
    zlp1.SetZ(0.);
    zlp2.SetZ(0.);

    fTreeVars.metdotzl1 = missing*zlp1/zlp1.Mag(); //MET along rotated lep 1
    fTreeVars.metdotzl2 = missing*zlp2/zlp2.Mag(); //MET along rotated lep 2
  }
  
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

  ////////////////////////////////////////
  // Additional Event Variables

  fTreeVars.ht       = ht;
  fTreeVars.htsum    = htSum;
  fTreeVars.jetpt    = jetOne.p4->Pt();
}

//--------------------------------------------------------------------------------------------------------------
// Estimate the neutrino contributions with different methods/hypotheses
void HistMaker::EstimateNeutrinos() {
  //Get pT vectors for leptons and MET
  TLorentzVector lep = *leptonOne.p4 + *leptonTwo.p4;
  TVector3 lp1 = leptonOne.p4->Vect();
  TVector3 lp2 = leptonTwo.p4->Vect();
  TVector3 missing(met*std::cos(metPhi), met*std::sin(metPhi), 0.);
  if(!std::isfinite(missing.Pt()) || !std::isfinite(met) || !std::isfinite(metPhi)) {
    printf("!!! HistMaker::%s: Entry: %12lld: MET vector is undefined! met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f\n",
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
  fTreeVars.mestimateoverm    = fTreeVars.mestimate    / fTreeVars.lepm;
  fTreeVars.mestimatetwooverm = fTreeVars.mestimatetwo / fTreeVars.lepm;

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
// Determine if a gen-particle is likely pileup or of jet origin
void HistMaker::CheckIfPileup(Lepton_t& lep) {
  lep.isPileup = false; lep.jetOrigin = false;

  //check if it's matched to a gen particle
  const int gen_index = lep.genIndex;
  if(gen_index < 0 || gen_index >= (int) nGenPart) {
    lep.isPileup = true;
    return;
  }

  //trace through the particle list until the final parent is found
  int parent = gen_index;
  int previous_parent = gen_index;
  while(parent >= 0 && parent < (int) nGenPart && GenPart_genPartIdxMother[parent] >= 0) {
    previous_parent = parent;
    parent = GenPart_genPartIdxMother[parent];
  }

  //check if final parent is consistent with pileup or not
  bool is_primary = false;
  is_primary |= parent == 0 || parent == 1; //from the parton-parton collision or embedded event
  lep.isPileup = !is_primary;

  //next check if it likely comes from a jet
  //if it comes from a q/g ISR, likely jet-origin
  const int pdg_prev = GenPart_pdgId[previous_parent];
  lep.jetOrigin = pdg_prev <= 6 || pdg_prev == 21;
  lep.jetOrigin |= lep.genFlavor == 26; //already matched to a jet
}

//--------------------------------------------------------------------------------------------------------------
// Determine the type of gen-level event matched to a hadronic tau
int HistMaker::FindTauGenType(Lepton_t& lep) {

  //check if the reco object is matched to a gen-level hadronic tau
  if(!lep.isTau()) return 0;
  if(fIsData) return 15; //for data, assume a real tau

  const int abs_flav = std::abs(lep.genFlavor);
  if(abs_flav == 15 || abs_flav == 13 || abs_flav == 11) return abs_flav; //genuine taus or light lep --> tau

  //already matched
  if(lep.genIndex >= 0) return std::abs(GenPart_pdgId[lep.genIndex]);

  //trace through the particle list until an object near the tau is found
  int type = -1;
  for(unsigned index = 0; index < nGenPart; ++index) {
    const int flavor(std::abs(GenPart_pdgId[index]));
    if(flavor == 12 || flavor == 14 || flavor == 16) continue; //skip neutrinos
    const double eta(GenPart_eta[index]), phi(GenPart_phi[index]), pt(GenPart_pt[index]);
    const double delta_r(std::sqrt(std::pow(lep.eta - eta, 2) + std::pow(Utilities::DeltaPhi(lep.phi, phi), 2)));
    if(delta_r < 0.5 && pt/lep.pt > 0.75f) {
      type = flavor;
      break;
    }
  }
  return std::max(type, 0);
}

//--------------------------------------------------------------------------------------------------------------
// Determine the gen-level hadronic tau info
void HistMaker::SetGenHadronicTau(Lepton_t& lep) {
  //check if the reco object is matched to a gen-level hadronic tau
  if(!lep.isTau()) return;
  if(std::abs(lep.genFlavor) != 15) return;

  //if gen-matched, find the corresponding tau decay near the reco tau
  if(fVerbose > 0) {
    printf(" HistMaker::%s: Checking hadronic tau gen info:\n", __func__);
    if(lep.genIndex >= 0 && lep.genIndex < (int) nGenPart)
      printf("  Gen index = %i: pdg = %i, pt = %.2f, eta = %.1f, phi = %.1f\n",
             lep.genIndex, GenPart_pdgId[lep.genIndex],  GenPart_pt[lep.genIndex],
             GenPart_eta[lep.genIndex], GenPart_phi[lep.genIndex]);
    else
      printf("  No gen index mapped to reco tau\n");
  }

  //identify gen-level taus in the event
  std::vector<int> tau_indices, veto_indices;
  for(int ipart = 0; ipart < (int) nGenPart; ++ipart) {
    if(std::abs(GenPart_pdgId[ipart]) == 15) { //taus
      if(fVerbose > 3) printf("   idx %i: pdg = %i, pt = %.1f, eta = %.2f, phi = %.2f\n",
                              ipart, GenPart_pdgId[ipart], GenPart_pt[ipart], GenPart_eta[ipart], GenPart_phi[ipart]);
      //check this tau is already identified as a tau -> tau+gamma candidate
      if(std::find(veto_indices.begin(), veto_indices.end(), ipart) != veto_indices.end()) {
        if(fVerbose > 3) printf("   idx %i vetoed\n", ipart);
        continue;
      }
      //add it to the tau list
      tau_indices.push_back(ipart);
      //check if it has a parent that is a tau and is in the list
      const int mother = GenPart_genPartIdxMother[ipart];
      if(mother >= 0 && std::abs(GenPart_pdgId[mother]) == 15) {
        //add the likely tau -> tau+gamma candidate to the veto list, remove if in the nominal list
        if(std::find(veto_indices.begin(), veto_indices.end(), mother) != veto_indices.end()) {
          veto_indices.push_back(mother);
          auto list_index = std::find(tau_indices.begin(), tau_indices.end(), mother);
          if(list_index != tau_indices.end()) tau_indices.erase(list_index);
        }
      }
    }
  }
  //if no taus are found, exit
  if(tau_indices.size() == 0) {
    if(fVerbose > 0) printf("  No gen-level taus found to match to the reco tau!\n");
    return;
  }

  //identify the tau decay daughters
  std::map<int, std::vector<int>> tau_daughters;
  std::vector<int> tau_had_decays;
  for(int ipart : tau_indices) {
    tau_daughters[ipart] = {};
    if(fVerbose > 0) printf("  Gen tau info: idx = %i, pdg = %3i, pt = %.2f, eta = %.1f, phi = %.1f\n",
                            ipart, GenPart_pdgId[ipart], GenPart_pt[ipart],
                            GenPart_eta[ipart], GenPart_phi[ipart]);
    for(int jpart = 0; jpart < (int) nGenPart; ++jpart) {
      const int mother = GenPart_genPartIdxMother[jpart];
      if(mother != ipart) continue;
      tau_daughters[ipart].push_back(jpart);
      const int pdg = std::abs(GenPart_pdgId[jpart]);
      if((pdg < 11 || pdg > 16) && pdg != 22) tau_had_decays.push_back(ipart);
      if(fVerbose > 0) printf("   idx = %i, pdg = %3i, pt = %.2f, eta = %.1f, phi = %.1f\n",
                          jpart, GenPart_pdgId[jpart], GenPart_pt[jpart],
                          GenPart_eta[jpart], GenPart_phi[jpart]);
    }
  }
  if(tau_had_decays.size() == 0) {
    if(fVerbose > 0) printf("  No gen-level hadronic taus found to match to the reco tau!\n");
    return;
  }

  //determine the tau that is closest to the reco tau
  double min_delta_r(1.e3);
  int best_match(-1);
  for(int itau : tau_had_decays) {
    const double delta_r = std::sqrt(std::pow(GenPart_eta[itau] - lep.eta, 2) + std::pow(GenPart_phi[itau] - lep.phi, 2));
    if(delta_r < min_delta_r) {
      best_match = itau;
      min_delta_r = delta_r;
    }
  }

  //check a match is found
  if(best_match < 0) {
    if(fVerbose > 0) printf("  Failed to find a best matching tau!\n");
    return;
  }

  //get the pT, eta, and phi for the hadronic tau
  TLorentzVector gen_had_tau;
  for(int ipart : tau_daughters[best_match]) {
    const int pdg = std::abs(GenPart_pdgId[ipart]);
    if(pdg == 12 || pdg == 14 || pdg == 16) continue; //can't reconstruct neutrinos
    TLorentzVector lpart;
    lpart.SetPtEtaPhiM(GenPart_pt[ipart], GenPart_eta[ipart], GenPart_phi[ipart], GenPart_mass[ipart]);
    gen_had_tau += lpart;
  }
  lep.genPt   = gen_had_tau.Pt();
  lep.genEta  = gen_had_tau.Eta();
  lep.genPhi  = gen_had_tau.Phi();
  lep.genMass = gen_had_tau.M();
}


//--------------------------------------------------------------------------------------------------------------
// Evaluate each MVA that is relevant to the current selection
void HistMaker::EvalMVAs(TString TimerName) {
  if(!fReprocessMVAs) return;
  const bool eval_timer = TimerName != "";
  if(eval_timer) fTimes[GetTimerNumber(TimerName.Data())] = std::chrono::steady_clock::now(); //timer for evaluating the MVAs
  if(fVerbose > 0) printf(" Processing event MVA scores\n");
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    //assume only relevant MVAs are configured
    fMvaOutputs[i] = -2.f;
    fMvaCDFs[i] =  0.f;
    fMvaLogP[i] = -3.f;
    fMvaFofP[i] = -2.f;
    fMvaUse [i] = -2.f;
    if(mva[i]) {
      fMvaOutputs[i] = mva[i]->EvaluateMVA(fMVAConfig->names_[i].Data());
      if(fVerbose > 0) printf("  MVA %i (%s) has score %.5f\n", i, fMVAConfig->names_[i].Data(), fMvaOutputs[i]);
    } else if(wrappedBDTs[i]) {
      fMvaOutputs[i] = wrappedBDTs[i]->EvaluateScore();
      if(fVerbose > 0) printf("  BDT %i (%s) has score %.5f\n", i, fMVAConfig->names_[i].Data(), fMvaOutputs[i]);
    } else { //not defined
      if(fVerbose > 9) printf("  Skipping undefined MVA %i (%s)\n", i, fMVAConfig->names_[i].Data());
      continue;
    }

    if(fMvaOutputs[i] < -2.1f) {
      std::cout << "Error value returned for MVA " << fMVAConfig->names_[i].Data()
                << " evaluation, Entry = " << fentry << std::endl;
    } else {
      const TString selection = fMVAConfig->GetSelectionByIndex(i);
      fMvaCDFs[i] = fMVAConfig->CDFTransform(fMvaOutputs[i], selection);

      const static float min_p(1.e-4f), min_log_p(std::log10(min_p));
      fMvaLogP[i] = (fMvaCDFs[i] > min_p) ? std::log10(fMvaCDFs[i]) : min_log_p;
      //Bound F(p) between 0 and 1 for convenience

      // //F(p) = p + log10(p)
      // fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? fMvaLogP[i]+fMvaCDFs[i] : min_log_p + min_p;
      // fMvaFofP[i] = (fMvaFofP[i] - (min_log_p + min_p)) / (1.f - (min_log_p + min_p));

      // //F(p) = p + 1/4*log10(p)
      // fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? 1.f/4.f*fMvaLogP[i]+fMvaCDFs[i] : 1.f/4.f*min_log_p + min_p;
      // fMvaFofP[i] = (fMvaFofP[i] - (1.f/4.f*min_log_p + min_p)) / (1.f - (1.f/4.f*min_log_p + min_p));

      // //F(p) = p*p + log10(p)
      // fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? fMvaLogP[i]+std::pow(fMvaCDFs[i],2) : min_log_p + min_p*min_p;
      // fMvaFofP[i] = (fMvaFofP[i] - (min_log_p + min_p*min_p)) / (1.f - (min_log_p + min_p*min_p));

      // //F(p) = p*p + 1/2*log10(p)
      // fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? 0.5f*fMvaLogP[i]+std::pow(fMvaCDFs[i],2) : 0.5f*min_log_p + min_p*min_p;
      // fMvaFofP[i] = (fMvaFofP[i] - (0.5f*min_log_p + min_p*min_p)) / (1.f - (0.5f*min_log_p + min_p*min_p));

      // //F(p) = p*p + 1/4*log10(p)
      // fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? 1.f/4.f*fMvaLogP[i]+std::pow(fMvaCDFs[i],2) : 1.f/4.f*min_log_p + min_p*min_p;
      // fMvaFofP[i] = (fMvaFofP[i] - (1.f/4.f*min_log_p + min_p*min_p)) / (1.f - (1.f/4.f*min_log_p + min_p*min_p));

      //F(p) = p*p + 1/6*log10(p)
      fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? 1.f/6.f*fMvaLogP[i]+std::pow(fMvaCDFs[i],2) : 1.f/6.f*min_log_p + min_p*min_p;
      fMvaFofP[i] = (fMvaFofP[i] - (1.f/6.f*min_log_p + min_p*min_p)) / (1.f - (1.f/6.f*min_log_p + min_p*min_p));

      // //F(p) = p*p + 1/8*log10(p)
      // fMvaFofP[i] = (fMvaCDFs[i] > min_p) ? 1.f/8.f*fMvaLogP[i]+std::pow(fMvaCDFs[i],2) : 1.f/8.f*min_log_p + min_p*min_p;
      // fMvaFofP[i] = (fMvaFofP[i] - (1.f/8.f*min_log_p + min_p*min_p)) / (1.f - (1.f/8.f*min_log_p + min_p*min_p));
    }
    //function of the MVA score to use in fits/plotting
    fMvaUse[i] = (fUseCDFBDTs == 1) ? fMvaCDFs[i] : (fUseCDFBDTs == 2) ? fMvaFofP[i] : fMvaOutputs[i];
  }
  if(eval_timer) IncrementTimer(TimerName.Data(), true);
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
  if(!fIsData && nPSWeight > kMaxTheory) {
    printf("HistMaker::%s: Error! Too many PS weight versions (%i), max allowed are %i\n", __func__, nPSWeight, kMaxTheory);
    throw 20;
  }
  if(!fIsData && nLHEPdfWeight > kMaxTheory) {
    printf("HistMaker::%s: Error! Too many PDF weight versions (%i), max allowed are %i\n", __func__, nLHEPdfWeight, kMaxTheory);
    throw 20;
  }
  if(!fIsData && nLHEScaleWeight > kMaxTheory) {
    printf("HistMaker::%s: Error! Too many Scale weight versions (%i), max allowed are %i\n", __func__, nLHEScaleWeight, kMaxTheory);
    throw 20;
  }


  if(fVerbose > 2) {
    printf(" Electron collection:\n");
    for(int i = 0; i < (int) nElectron; ++i) {
      printf("  %2i: pt = %6.1f, eta = %5.2f, eta_sc = %5.2f, phi = %5.2f", i, Electron_pt[i], Electron_eta[i], Electron_eta[i]+Electron_deltaEtaSC[i], Electron_phi[i]);
      if(!fIsData) printf(", gen_flav_ID = %2i, gen index = %2i", Electron_genPartFlav[i], Electron_genPartIdx[i]);
      printf("\n");
    }
    printf(" Muon collection:\n");
    for(int i = 0; i < (int) nMuon; ++i) {
      printf("  %2i: pt = %6.1f, eta = %5.2f, phi = %5.2f", i, Muon_pt[i], Muon_eta[i], Muon_phi[i]);
      if(!fIsData) printf(", gen_flav_ID = %2i, gen index = %2i", Muon_genPartFlav[i], Muon_genPartIdx[i]);
      printf("\n");
    }
    printf(" Tau collection:\n");
    for(int i = 0; i < (int) nTau; ++i) {
      printf("  %2i: pt = %6.1f, eta = %5.2f, phi = %5.2f", i, Tau_pt[i], Tau_eta[i], Tau_phi[i]);
      if(!fIsData) printf(", gen_flav_ID = %2i, gen index = %2i", Tau_genPartFlav[i], Tau_genPartIdx[i]);
      printf("\n");
    }
  }

  if(fVerbose > 3 && !fIsData) {
    printf(" Gen-particle collection:\n");
    for(int i = 0; i < (int) nGenPart; ++i) {
      printf("  %2i: pdg = %5i, pt = %5.1f, eta = %9.2f, phi = %5.2f, mass = %.2e, mother = %2i\n", i,
             GenPart_pdgId[i], GenPart_pt[i], GenPart_eta[i], GenPart_phi[i],
             GenPart_mass[i], GenPart_genPartIdxMother[i]);
    }
  }

  /////////////////////////////////////
  // Base selections
  mutau   =                   nMuon == 1 && nTau == 1 && (fSelection == "" || fSelection == "mutau");
  etau    = nElectron == 1               && nTau == 1 && (fSelection == "" || fSelection == "etau" );
  emu     = nElectron == 1 && nMuon == 1              && (fSelection == "" || fSelection == "emu"  || fSelection.Contains("tau_"));
  mumu    = nElectron <  2 && nMuon == 2              && (fSelection == "" || fSelection == "mumu" );
  ee      = nElectron == 2 && nMuon <  2              && (fSelection == "" || fSelection == "ee"   );
  etau_mu = emu && fSelection == "etau_mu";
  mutau_e = emu && fSelection == "mutau_e";

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
  runEra  = 0;
  if(!fIsData && !fIsEmbed) { // use random numbers for MC not split by era
    if(fYear == 2016) {
      const double rand = fRnd->Uniform();
      const double frac_first = 19.72 / 35.86;
      mcEra = rand > frac_first; //0 if first, 1 if second
      const double lums[] = {5.892,2.646,4.353,4.117,3.174,7.540,8.606};
      const int nlums = sizeof(lums)/sizeof(*lums);
      runEra = 1; //starts at B
      double tot = 0.f; //total luminosity
      for(int index = 0; index < nlums; ++index) {
        tot += lums[index];
      }
      double running = 0.f; //running luminosity
      for(int index = 0; index < nlums; ++index) {
        running += lums[index];
        if(rand > running/tot) ++runEra;
        else break;
      }
    } else if(fYear == 2018) {
      const double rand = fRnd->Uniform();
      const double frac_first = 1. -  31.93/59.59;
      mcEra = rand > frac_first; //0 if first, 1 if second
      const double lums[] = {14.027,7.061,6.916,31.258};
      const int nlums = sizeof(lums)/sizeof(*lums);
      runEra = 14; //starts at A, add 2016+2017
      double tot = 0.f; //total luminosity
      for(int index = 0; index < nlums; ++index) {
        tot += lums[index];
      }
      double running = 0.f; //running luminosity
      for(int index = 0; index < nlums; ++index) {
        running += lums[index];
        if(rand > running/tot) ++runEra;
        else break;
      }
    } else if(fYear == 2017) { //approximate run eras in 2017
      const double rand = fRnd->Uniform();
      const double lums[] = {4.7294,9.6312,4.2477,9.3136,13.5387};
      const int nlums = sizeof(lums)/sizeof(*lums);
      runEra = 9; //starts at B, add 2016 B-H runs
      double tot = 0.f; //total luminosity
      for(int index = 0; index < nlums; ++index) {
        tot += lums[index];
      }
      double running = 0.f; //running luminosity
      for(int index = 0; index < nlums; ++index) {
        running += lums[index];
        if(rand > running/tot) ++runEra;
        else break;
      }
    }
  } else if(fIsEmbed || fIsData) { //decide by run period in the file name
    TString name = GetOutputName();
    name.ReplaceAll(".hist", "");
    const char Run = name[name.Length()-1];
    runEra = Run - 'A';
    runEra += (fYear > 2016)*8 + (fYear > 2017)*6;
    if(fYear == 2016) {
      if(name.Contains("-G") || name.Contains("-H")) mcEra = 1;
    } else if(fYear == 2018) { //split 2018 into ABC and D
      if(name.Contains("-D")) mcEra = 1;
    }
  }

  ///////////////////////////////////////////////////////
  // Apply object scale corrections
  ///////////////////////////////////////////////////////

  //Evaluate the neutrino momentum in the event
  if(!fIsData && nGenPart > 0) { //check if there are gen particles
    float px(0.f), py(0.f);
    for(UInt_t ipart = 0; ipart < nGenPart; ++ipart) {
      const int pdg = std::abs(GenPart_pdgId[ipart]);
      if(pdg == 12 || pdg == 14 || pdg == 16) {
        const int mother = GenPart_genPartIdxMother[ipart]; //ensure this isn't some nu --> nu entry in the generator
        if(mother >= 0 && GenPart_pdgId[ipart] == GenPart_pdgId[mother]) continue;
        px += GenPart_pt[ipart]*std::cos(GenPart_phi[ipart]);
        py += GenPart_pt[ipart]*std::sin(GenPart_phi[ipart]);
      }
    }
    eventNuPt = std::sqrt(px*px+py*py);
    eventNuPhi = Utilities::PhiFromXY(px,py);
  } else {
    eventNuPt           = 0.f;
    eventNuPhi          = 0.f;
  }

  //Add MET information
  const bool use_puppi_met = true; //use the PF MET or Puppi MET
  if(use_puppi_met) {
    met        = puppMET; //use PUPPI MET
    metPhi     = puppMETphi;
    rawMet     = rawPuppMET;
    rawMetPhi  = rawPuppMETphi;
  } else {
    met        = PFMET; //use PF MET
    metPhi     = PFMETphi;
    rawMet     = rawPFMET;
    rawMetPhi  = rawPFMETphi;
  }
  metCorr    = 0.f; //record the changes to the MET due to changes in object energy/momentum scales
  metCorrPhi = 0.f;

  //Check if the MET uncertainties are defined, and replace them with the nominal if they are
  if(!std::isfinite(puppMETJERUp) || !std::isfinite(puppMETphiJERUp)) {puppMETJERUp = puppMET; puppMETphiJERUp = puppMETphi;}
  if(!std::isfinite(puppMETJESUp) || !std::isfinite(puppMETphiJESUp)) {puppMETJESUp = puppMET; puppMETphiJESUp = puppMETphi;}


  //if raw met is not defined, replace is with the corrected met before lepton scale corrections
  if(rawMet < 0.f || std::fabs(rawMetPhi) > 4.f) {rawMet = met; rawMetPhi = metPhi;}

  //Replace MET with Raw MET if requested
  if(fReplaceMETWithRaw) {
    // const float met_diff     = met    - rawMet   ;
    // const float met_phi_diff = metPhi - rawMetPhi;
    //set the uncertainty to be the change size
    puppMETJERUp    = met;
    puppMETphiJERUp = metPhi;
    //replace the MET
    met    = rawMet;
    metPhi = rawMetPhi;
  }

  //Correct the detector MET in Embedding events
  if(fIsEmbed && fApplyEmbedMETDPhiCorr) {
    if(zLepOnePt >= 0.f || zLepTwoPt >= 0.f) {
      //Get the gen-level lepton and MET info
      TLorentzVector zlepone, zleptwo;
      if(zLepOnePt >= zLepTwoPt) { //sort by leading lepton
        zlepone.SetPtEtaPhiM(zLepOnePt, zLepOneEta, zLepOnePhi, zLepOneMass);
        zleptwo.SetPtEtaPhiM(zLepTwoPt, zLepTwoEta, zLepTwoPhi, zLepTwoMass);
      } else {
        zlepone.SetPtEtaPhiM(zLepTwoPt, zLepTwoEta, zLepTwoPhi, zLepTwoMass);
        zleptwo.SetPtEtaPhiM(zLepOnePt, zLepOneEta, zLepOnePhi, zLepOneMass);
      }
      TLorentzVector z_lv = zlepone + zleptwo;
      //remove nu pT from the MET
      float px = met*std::cos(metPhi) - eventNuPt*std::cos(eventNuPhi);
      float py = met*std::sin(metPhi) - eventNuPt*std::sin(eventNuPhi);
      eventDetectorMet = std::sqrt(px*px + py*py);
      eventDetectorMetPhi = Utilities::PhiFromXY(px,py);

      //apply a correction to the MET
      const int mode = 5; //how to implement the correction

      if(mode == 0) { //rotate the detector met closer to the leading embedded tau
        //delta phi is defined as phi_1 - phi_2 --> if dphi > 0 then phi_2 < phi_1
        const double phi = Utilities::DeltaPhi(((zLepOnePt > zLepTwoPt) ? zlepone : zleptwo).Phi(), eventDetectorMetPhi);
        const double phi_rot = (std::fabs(phi) < 0.1) ? 0. : (phi > 0.) ? 0.04 : -0.04;
        eventDetectorMetPhi = Utilities::DeltaPhi(eventDetectorMetPhi, -1.*phi_rot);

      } else if(mode == 1) { //add MET along the Z pT
        const float scale = 0.025;
        px += scale*z_lv.Px();
        py += scale*z_lv.Py();
        eventDetectorMet = std::sqrt(px*px + py*py);
        eventDetectorMetPhi = Utilities::PhiFromXY(px,py);

      } else if(mode == 2) { //add MET along the leading gen-level lepton
        const float scale = 0.007;
        px += scale*zlepone.Px();
        py += scale*zlepone.Py();
        eventDetectorMet = std::sqrt(px*px + py*py);
        eventDetectorMetPhi = Utilities::PhiFromXY(px,py);

      } else if(mode == 3) { //add MET along each reco muon
        const float scale = 0.005;
        for(UInt_t index = 0; index < nMuon; ++index) {
          px += scale*Muon_pt[index]*std::cos(Muon_phi[index]);
          py += scale*Muon_pt[index]*std::sin(Muon_phi[index]);
        }
        eventDetectorMet = std::sqrt(px*px + py*py);
        eventDetectorMetPhi = Utilities::PhiFromXY(px,py);

      } else if(mode == 4) { //add MET along the reco leptons
        const float scale = 0.025;
        if(nElectron != 2) { //non-ee selections
          for(UInt_t index = 0; index < nMuon; ++index) {
            px += scale*Muon_pt[index]*std::cos(Muon_phi[index]);
            py += scale*Muon_pt[index]*std::sin(Muon_phi[index]);
          }
        }
        if(nMuon != 2) { //non-mumu selections
          for(UInt_t index = 0; index < nElectron; ++index) {
            px += scale*Electron_pt[index]*std::cos(Electron_phi[index]);
            py += scale*Electron_pt[index]*std::sin(Electron_phi[index]);
          }
        }
        if(nMuon+nElectron == 1) { //etau/mutau selections
          for(UInt_t index = 0; index < nTau; ++index) {
            px += scale*Tau_pt[index]*std::cos(Tau_phi[index]);
            py += scale*Tau_pt[index]*std::sin(Tau_phi[index]);
          }
        }
        eventDetectorMet = std::sqrt(px*px + py*py);
        eventDetectorMetPhi = Utilities::PhiFromXY(px,py);

      } else if(mode == 5) { //add MET along the reco leptons with different scales
        if(nElectron != 2) { //non-ee selections
          const float scale = 0.025;
          for(UInt_t index = 0; index < nMuon; ++index) {
            px += scale*Muon_pt[index]*std::cos(Muon_phi[index]);
            py += scale*Muon_pt[index]*std::sin(Muon_phi[index]);
          }
        }
        if(nMuon != 2) { //non-mumu selections
          const float scale = 0.025;
          for(UInt_t index = 0; index < nElectron; ++index) {
            px += scale*Electron_pt[index]*std::cos(Electron_phi[index]);
            py += scale*Electron_pt[index]*std::sin(Electron_phi[index]);
          }
        }
        if(nMuon+nElectron == 1) { //etau/mutau selections
          const float scale = 0.05;
          for(UInt_t index = 0; index < nTau; ++index) {
            px += scale*Tau_pt[index]*std::cos(Tau_phi[index]);
            py += scale*Tau_pt[index]*std::sin(Tau_phi[index]);
          }
        }
        eventDetectorMet = std::sqrt(px*px + py*py);
        eventDetectorMetPhi = Utilities::PhiFromXY(px,py);
      } else if(mode == 6) { //add MET along high |phi| for phi modulation
        px -= 0.025*std::fabs(px); //increase MET along -x axis (phi = pi/-pi)
        eventDetectorMet = std::sqrt(px*px + py*py);
        eventDetectorMetPhi = Utilities::PhiFromXY(px,py);
      }
      //reset the MET to be the new detector met + nu pT
      px = eventDetectorMet*std::cos(eventDetectorMetPhi) + eventNuPt*std::cos(eventNuPhi);
      py = eventDetectorMet*std::sin(eventDetectorMetPhi) + eventNuPt*std::sin(eventNuPhi);
      met    = std::sqrt(px*px+py*py);
      metPhi = Utilities::PhiFromXY(px,py);
    }
  }

  ApplyElectronCorrections();
  ApplyMuonCorrections();
  if(nElectron + nMuon < 2) ApplyTauCorrections(); //only consider the taus if doing a tau selection

  //Re-sort muon/electron collections in the case where the corrections change the pT order
  SwapSameFlavor();

  //Apply the MET changes
  const float met_x(met*std::cos(metPhi) + metCorr*std::cos(metCorrPhi)), met_y(met*std::sin(metPhi) + metCorr*std::sin(metCorrPhi));
  met = std::sqrt(met_x*met_x + met_y*met_y);
  metPhi = (met > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, met_x/met)))*(met_y < 0.f ? -1 : 1) : 0.f;
  if(!std::isfinite(met) || !std::isfinite(metPhi)) {
    printf("!!! HistMaker::%s: Entry: %12lld: MET elements are undefined! met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f, met_x = %.2f, met_y = %.2f, pupp = %.2f, puppPhi = %.3f\n",
           __func__, fentry, met, metPhi, metCorr, metCorrPhi, met_x, met_y, puppMET, puppMETphi);
    throw std::runtime_error("Undefined MET");
  }

  //Update the MET uncertainty inputs with the MET changes as well
  {
    const float met_x(puppMETJERUp*std::cos(puppMETphiJERUp) + metCorr*std::cos(metCorrPhi)), met_y(puppMETJERUp*std::sin(puppMETphiJERUp) + metCorr*std::sin(metCorrPhi));
    puppMETJERUp = std::sqrt(met_x*met_x + met_y*met_y);
    puppMETphiJERUp = (puppMETJERUp > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, met_x/puppMETJERUp)))*(met_y < 0.f ? -1 : 1) : 0.f;
    if(!std::isfinite(puppMETJERUp) || !std::isfinite(puppMETphiJERUp))
      throw std::runtime_error(Form("HistMaker::%s: Entry: %12lld: PUPPI JER up error: met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f, met_x = %.2f, met_y = %.2f\n",
                                    __func__, fentry, puppMETJERUp, puppMETphiJERUp, metCorr, metCorrPhi, met_x, met_y));
  }
  {
    const float met_x(puppMETJESUp*std::cos(puppMETphiJESUp) + metCorr*std::cos(metCorrPhi)), met_y(puppMETJESUp*std::sin(puppMETphiJESUp) + metCorr*std::sin(metCorrPhi));
    puppMETJESUp = std::sqrt(met_x*met_x + met_y*met_y);
    puppMETphiJESUp = (puppMETJESUp > 0.f) ? std::acos(std::max(-1.f, std::min(1.f, met_x/puppMETJESUp)))*(met_y < 0.f ? -1 : 1) : 0.f;
    if(!std::isfinite(puppMETJESUp) || !std::isfinite(puppMETphiJESUp))
      throw std::runtime_error(Form("HistMaker::%s: Entry: %12lld: PUPPI JES up error: met = %.2f, phi = %.3f, metCorr = %.2f, phiCorr = %.3f, met_x = %.2f, met_y = %.2f\n",
                                    __func__, fentry, puppMETJESUp, puppMETphiJESUp, metCorr, metCorrPhi, met_x, met_y));
  }

  //store MET uncertainty effects (approx down as met - (up - met) = 2*met - up)
  const float met_jer_x(2.*met*std::cos(metPhi) - puppMETJERUp*std::cos(puppMETphiJERUp)), met_jer_y(2.*met*std::sin(metPhi) - puppMETJERUp*std::sin(puppMETphiJERUp));
  const float met_jes_x(2.*met*std::cos(metPhi) - puppMETJESUp*std::cos(puppMETphiJESUp)), met_jes_y(2.*met*std::sin(metPhi) - puppMETJESUp*std::sin(puppMETphiJESUp));
  puppMETJERDown    = std::sqrt(std::pow(met_jer_x, 2) + std::pow(met_jer_y, 2));
  puppMETphiJERDown = (puppMETJERDown <= 0.f) ? 0.f : std::acos(std::max(-1.f, std::min(1.f, met_jer_x/puppMETJERDown))) * (met_jer_y < 0.f ? -1.f : 1.f);
  puppMETJESDown    = std::sqrt(std::pow(met_jes_x, 2) + std::pow(met_jes_y, 2));
  puppMETphiJESDown = (puppMETJESDown <= 0.f) ? 0.f : std::acos(std::max(-1.f, std::min(1.f, met_jes_x/puppMETJESDown))) * (met_jes_y < 0.f ? -1.f : 1.f);


  //Evaluate the missing energy from the detector/acceptance
  if(!fIsData && nGenPart > 0) { //check if there are gen particles
    //remove nu pT from the MET
    float px = met*std::cos(metPhi) - eventNuPt*std::cos(eventNuPhi);
    float py = met*std::sin(metPhi) - eventNuPt*std::sin(eventNuPhi);
    eventDetectorMet = std::sqrt(px*px + py*py);
    eventDetectorMetPhi = Utilities::PhiFromXY(px,py);
  } else {
    eventDetectorMet    = met;
    eventDetectorMetPhi = metPhi;
  }

  //For Embedding, use the puppMET uncertainty fields to set a MET uncertainty
  if(fIsEmbed && fEmbedUseMETUnc == 2) {
    //fractional error on the detector MET
    // const float met_err = (0.01f + 0.07f*(eventDetectorMet/100.f)); //fractional error on the detector MET
    const float met_err = (0.01f + 0.20f*(eventDetectorMet/100.f)); //fractional error on the detector MET

    //scale down the detector MET by this value (overestimates in data)
    const float dx(eventDetectorMet*met_err*std::cos(eventDetectorMetPhi)), dy(eventDetectorMet*met_err*std::cos(eventDetectorMetPhi));
    const float met_x(met*std::cos(metPhi) - dx), met_y(met*std::sin(metPhi) - dy);
    puppMETJESUp = std::sqrt(met_x*met_x+met_y*met_y);
    puppMETphiJESUp = Utilities::PhiFromXY(met_x,met_y);
    // puppMETJESUp    = met + met_err*eventDetectorMet;
    // puppMETphiJESUp = metPhi; //ignore phi uncertainty
    puppMETJERUp    = met; //no resolution uncertainty included
    puppMETphiJERUp = metPhi;
  }

  ///////////////////////////////////////////////////////
  // Initialize lepton selection info
  ///////////////////////////////////////////////////////

  nMuons = nMuon; nElectrons = nElectron; nTaus = nTau; nPhotons = 0;

  //associated track variables not relevant for all leptons, so reset if not used
  leptonOne.trkpt = 0.f; leptonOne.trketa = 0.f; leptonOne.trkphi = 0.f;
  leptonTwo.trkpt = 0.f; leptonTwo.trketa = 0.f; leptonTwo.trkphi = 0.f;

  //gen info not relevant for all leptons
  leptonOne.isPileup = false; leptonTwo.isPileup = false;
  leptonOne.jetOrigin = false; leptonTwo.jetOrigin = false;

  if(emu) {
    leptonOne.index  = 0;
    leptonTwo.index  = 0;
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
    leptonOne.genIndex  = Electron_genPartIdx[0];
    leptonTwo.genIndex  = Muon_genPartIdx[0];
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
    leptonOne.index  = 0;
    leptonTwo.index  = 0;
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
    leptonOne.genIndex  = Electron_genPartIdx[0];
    leptonTwo.genIndex  = Tau_genPartIdx[0];
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
    leptonOne.index  = 0;
    leptonTwo.index  = 0;
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
    leptonOne.genIndex  = Muon_genPartIdx[0];
    leptonTwo.genIndex  = Tau_genPartIdx[0];
    leptonOne.id1       = Muon_looseId[0] + Muon_mediumId[0] + Muon_tightId[0];
    leptonOne.id2       = MuonRelIsoID(Muon_pfRelIso04_all[0]);
    leptonTwo.id1       = Tau_idAntiEle[0];
    leptonTwo.id2       = Tau_idAntiMu[0];
    leptonTwo.id3       = Tau_idAntiJet[0];
    leptonOne.ES[0]     = Muon_RoccoSF[0];
    leptonOne.ES[1]     = Muon_RoccoSF[0]*(1.f + Muon_ESErr[0]);
    leptonOne.ES[2]     = Muon_RoccoSF[0]*(1.f - Muon_ESErr[0]);
    leptonTwo.ES[0]     = Tau_energyScale[0];
    leptonTwo.ES[1]     = Tau_energyScaleUp[0];
    leptonTwo.ES[2]     = Tau_energyScaleDown[0];
    leptonTwo.trkpt     = Tau_leadTrkPtOverTauPt[0]*leptonTwo.p4->Pt();
    leptonTwo.trketa    = Tau_leadTrkDeltaEta[0] + leptonTwo.p4->Eta();
    leptonTwo.trkphi    = Tau_leadTrkDeltaPhi[0] + leptonTwo.p4->Phi();
  } else if(mumu) {
    leptonOne.index  = 0;
    leptonTwo.index  = 1;
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
    leptonOne.genIndex  = Muon_genPartIdx[0];
    leptonTwo.genIndex  = Muon_genPartIdx[1];
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
    leptonOne.index  = 0;
    leptonTwo.index  = 1;
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
    leptonOne.genIndex  = Electron_genPartIdx[0];
    leptonTwo.genIndex  = Electron_genPartIdx[1];
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

  //Initialize gen-level info if available
  if(!fIsData) {
    if(leptonOne.genIndex >= 0 && leptonOne.genIndex < (int) nGenPart) {
      if(leptonOne.isTau()) SetGenHadronicTau(leptonOne);
      else {
        leptonOne.genPt    = GenPart_pt  [leptonOne.genIndex];
        leptonOne.genEta   = GenPart_eta [leptonOne.genIndex];
        leptonOne.genPhi   = GenPart_phi [leptonOne.genIndex];
        leptonOne.genMass  = GenPart_mass[leptonOne.genIndex];
      }
    } else {
      leptonOne.genPt    = -1.f;
      leptonOne.genEta   =  0.f;
      leptonOne.genPhi   =  0.f;
      leptonOne.genMass  =  0.f;
    }
    if(leptonTwo.genIndex >= 0 && leptonTwo.genIndex < (int) nGenPart) {
      if(leptonTwo.isTau()) SetGenHadronicTau(leptonTwo);
      else {
        leptonTwo.genPt    = GenPart_pt  [leptonTwo.genIndex];
        leptonTwo.genEta   = GenPart_eta [leptonTwo.genIndex];
        leptonTwo.genPhi   = GenPart_phi [leptonTwo.genIndex];
        leptonTwo.genMass  = GenPart_mass[leptonTwo.genIndex];
      }
    } else {
      leptonTwo.genPt    = -1.f;
      leptonTwo.genEta   =  0.f;
      leptonTwo.genPhi   =  0.f;
      leptonTwo.genMass  =  0.f;
    }
    CheckIfPileup(leptonOne); //check if origin is pileup/jet
    CheckIfPileup(leptonTwo);
    if(fVerbose > 1) {
      printf(" Lepton one gen info: pt = %.1f, eta = %.2f, phi = %.2f, mass = %.1e, ispileup = %o, jetorigin = %o\n",
             leptonOne.genPt, leptonOne.genEta, leptonOne.genPhi, leptonOne.genMass, leptonOne.isPileup, leptonOne.jetOrigin);
      printf(" Lepton two gen info: pt = %.1f, eta = %.2f, phi = %.2f, mass = %.1e, ispileup = %o, jetOrigin = %o",
             leptonTwo.genPt, leptonTwo.genEta, leptonTwo.genPhi, leptonTwo.genMass, leptonTwo.isPileup, leptonTwo.jetOrigin);
      if(!fIsData && leptonTwo.isTau()) printf(", gen-type = %i", FindTauGenType(leptonTwo));
      printf("\n");
    }
  } else { //data --> set to reco values
    leptonOne.genPt   = leptonOne.pt  ;
    leptonOne.genEta  = leptonOne.eta ;
    leptonOne.genPhi  = leptonOne.phi ;
    leptonOne.genMass = leptonOne.mass;
    leptonTwo.genPt   = leptonTwo.pt  ;
    leptonTwo.genEta  = leptonTwo.eta ;
    leptonTwo.genPhi  = leptonTwo.phi ;
    leptonTwo.genMass = leptonTwo.mass;
  }

  //Initialize resolution uncertainties with gen-info set
  if(leptonOne.isElectron()) {
    leptonOne.Res[0] = Electron_resolutionScale    [leptonOne.index];
    leptonOne.Res[1] = Electron_resolutionScaleUp  [leptonOne.index];
    leptonOne.Res[2] = Electron_resolutionScaleDown[leptonOne.index];
  }
  if(leptonTwo.isElectron()) {
    leptonTwo.Res[0] = Electron_resolutionScale    [leptonTwo.index];
    leptonTwo.Res[1] = Electron_resolutionScaleUp  [leptonTwo.index];
    leptonTwo.Res[2] = Electron_resolutionScaleDown[leptonTwo.index];
  }
  if(leptonOne.isMuon()) {
    leptonOne.Res[0] = 1.f;
    leptonOne.Res[1] = (fIsEmbed) ? fEmbeddingResolution->MuonResolutionUnc(leptonOne.pt, leptonOne.eta, leptonOne.genPt, fYear) : 1.f;
    leptonOne.Res[2] = 1.f;
  }
  if(leptonTwo.isMuon()) {
    leptonTwo.Res[0] = 1.f;
    leptonTwo.Res[1] = (fIsEmbed) ? fEmbeddingResolution->MuonResolutionUnc(leptonTwo.pt, leptonTwo.eta, leptonTwo.genPt, fYear) : 1.f;
    leptonTwo.Res[2] = 1.f;
  }
  if(leptonOne.isTau()) {
    leptonOne.Res[0] = 1.f;
    leptonOne.Res[1] = 1.f;
    leptonOne.Res[2] = 1.f;
  }
  if(leptonTwo.isTau()) {
    leptonTwo.Res[0] = 1.f;
    leptonTwo.Res[1] = 1.f;
    leptonTwo.Res[2] = 1.f;
  }

  if(fVerbose > 6) {
    printf(" HistMaker::%s: Resolution scales:\n", __func__);
    printf("  LeptonOne: %.5f (%.5f/%.5f)\n", leptonOne.Res[0], leptonOne.Res[1], leptonOne.Res[2]);
    printf("  LeptonTwo: %.5f (%.5f/%.5f)\n", leptonTwo.Res[0], leptonTwo.Res[1], leptonTwo.Res[2]);
  }

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
  if     (std::round(std::fabs(zLepOneID)) == 11) ++nGenHardElectrons;
  else if(std::round(std::fabs(zLepOneID)) == 13) ++nGenHardMuons;
  else if(std::round(std::fabs(zLepOneID)) == 15) ++nGenHardTaus;
  //lepton two
  if     (std::round(std::fabs(zLepTwoID)) == 11) ++nGenHardElectrons;
  else if(std::round(std::fabs(zLepTwoID)) == 13) ++nGenHardMuons;
  else if(std::round(std::fabs(zLepTwoID)) == 15) ++nGenHardTaus;

  if(fIsDY || fIsSignal || fIsEmbed) { //ensure a reasonable number of generated leptons are found in (Z)->ll(') simulated events
    if(nGenHardElectrons + nGenHardMuons + nGenHardTaus != 2) {
      printf("HistMaker::%s: Entry %lld: Error! Not all Z->ll gen-leptons were found\n", __func__, fentry);
      throw 20;
    }
  }

  //Store the N(primary photons) generated
  nGenPhotons = 0; nGenHardPhotons = 0;
  if(!fIsData) {
    for(UInt_t igen = 0; igen < nGenPart; ++igen) {
      if(std::abs(GenPart_pdgId[igen]) == 22) { //photon
        const int mother = GenPart_genPartIdxMother[igen];
        if(mother >= 0) {
          const int mother_id = std::abs(GenPart_pdgId[mother]);
          if(mother == 0 || mother == 1) { //photon from the primary interaction
            ++nGenHardPhotons;
          } else if(mother_id == 24) { //include W -> gamma events in the primary interaction count
            ++nGenHardPhotons;
          }
          if(mother_id != 22) ++nGenPhotons; //non-pileup photons or gamma -> X + gamma
        }
      }
    }
  }

  if(!fIsData && fVerbose > 3) {
    printf("Gen-particle (hard process) counts: %i (%i) electrons, %i (%i) muons, %i (%i) taus, and %i (%i) photons\n",
           nGenElectrons, nGenHardElectrons, nGenMuons, nGenHardMuons, nGenTaus, nGenHardTaus, nGenPhotons, nGenHardPhotons);
  }

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
  if(fYear == 2017) {muon_trig_pt_ = 28.f; electron_trig_pt_ = 35.f;} //2017 electron trigger efficiencies are ~0 below 35 GeV for Embedding at high |eta|
  if(fYear == 2016) electron_trig_pt_ = 29.f;
  emu_trig_ele_pt_ = 24.f; //following the LFV H AN selections
  emu_trig_mu_pt_  = 10.f;
  mue_trig_ele_pt_ = 13.f; //following the LFV H AN selections
  mue_trig_mu_pt_  = 24.f;

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
  const int min_jet_id    = 6; //7 = tight lep veto in 2016, 6 in 2017/2018, so >=6 for either
  const int min_jet_pu_id = 6;
  const float min_jet_pt = 20.f; //minimum jet pT considered in the counting

  TLorentzVector htLV;
  TLorentzVector jetLoop; //for checking delta R
  float jetptmax = -1; //track highest pt jet
  jetOne.setPtEtaPhiM(0.,0.,0.,0.); //reset to no jet found
  if(fVerbose > 5) {
    printf(" Printing input jet collection:\n");
    for(int ijet = 0; ijet < (int) nJet; ++ijet) {
      printf("  %2i: pt = %.1f, eta = %5.2f, btag = %.5f, tagged (e, mu, tau) = (%o, %o, %o), ID = %i, pu ID = %i, flavor = %i\n", ijet,
             Jet_pt[ijet], Jet_eta[ijet], Jet_btagDeepB[ijet],
             Jet_TaggedAsRemovedByElectron[ijet], Jet_TaggedAsRemovedByMuon[ijet], Jet_TaggedAsRemovedByTau[ijet],
             Jet_jetId[ijet], Jet_puId[ijet], (fIsData || fIsEmbed) ? 0 : Jet_partonFlavour[ijet]);
    }
  }
  for(UInt_t ijet = 0; ijet < nJet; ++ijet) {
    //apply jet pT and mass JER corrections
    if(!fIsData && !fIsEmbed && fApplyJERCorrections) {
      Jet_pt  [ijet] = Jet_pt_nom  [ijet];
      Jet_mass[ijet] = Jet_mass_nom[ijet];
    }

    //veto jets that fail IDs or overlap identified leptons
    if(Jet_jetId[ijet] < min_jet_id) continue; //bad jet
    if(Jet_TaggedAsRemovedByMuon[ijet]) continue; //overlapping a muon
    if(Jet_TaggedAsRemovedByElectron[ijet]) continue; //overlapping an electron
    if(Jet_TaggedAsRemovedByTau[ijet]) continue; //overlapping a hadronic tau

    const float jetpt   = Jet_pt  [ijet];
    const float jeteta  = Jet_eta [ijet];
    const float jetphi  = Jet_phi [ijet];
    const float jetmass = Jet_mass[ijet];
    if(jetpt < min_jet_pt) continue; //too low of jet pt

    //Restrict to less-forward jets
    if(std::fabs(jeteta) < 3.f) {
      //check if jet fails PU ID or not (only for low pT jets)
      if(jetpt < 50.f && Jet_puId[ijet] < min_jet_pu_id) {
        jetsRejPt    [nJets20Rej] = jetpt;
        jetsRejEta   [nJets20Rej] = jeteta;
        ++nJets20Rej;
        continue;
      }

      //accept the jet
      ++nJets20;
      if(jetpt > 25.f) ++nJets; //higher pT threshold counting
      jetLoop.SetPtEtaPhiM(jetpt, jeteta, jetphi, jetmass);

      //add HT information
      htLV  += jetLoop;
      htSum += jetpt;

      //store the highest pT jet
      if(jetpt > jetptmax) {
        jetOne.setPtEtaPhiM(jetpt, jeteta, jetphi, jetmass); jetptmax = jetpt;
        //jet systematic uncertainties
        if(!fIsData && !fIsEmbed) {
          jetOne.jer_pt_up   = Jet_pt_jerUp       [ijet];
          jetOne.jer_pt_down = Jet_pt_jerDown     [ijet];
          jetOne.jes_pt_up   = Jet_pt_jesTotalUp  [ijet];
          jetOne.jes_pt_down = Jet_pt_jesTotalDown[ijet];
        } else { //data jets don't have these uncertainties
          jetOne.jer_pt_up   = Jet_pt[ijet];
          jetOne.jer_pt_down = Jet_pt[ijet];
          jetOne.jes_pt_up   = Jet_pt[ijet];
          jetOne.jes_pt_down = Jet_pt[ijet];
        }
      }

      //store the b-tag information for the jet
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
      jetsPhi   [nJets20-1] = jetphi;
      jetsBTag  [nJets20-1] = jetBTag;
      jetsFlavor[nJets20-1] = (fIsData || fIsEmbed) ? 0 : Jet_partonFlavour[ijet];
    }
  } //end jet collection loop

  //Record HT result
  ht    = htLV.Pt();
  htPhi = htLV.Phi();

  if(fVerbose > 5) {
    printf(" Printing final jet collection:\n");
    for(int ijet = 0; ijet < (int) nJets20; ++ijet) {
      printf("  %2i: pt = %.1f, eta = %5.2f, btag = %i, flavor = %i\n", ijet, jetsPt[ijet], jetsEta[ijet], jetsBTag[ijet], jetsFlavor[ijet]);
    }
  }
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
  const bool train_eligible = (!fIsData && !looseQCDSelection) || (fIsData && looseQCDSelection); //use tight MC events or loose data events
  if(train_eligible) {
    float split_val = (fUseRandomField) ? randomField : fRnd->Uniform();
    if(fUseRandomField == 2) { //evaluate with event variables
      double event_val = std::fabs(leptonOne.phi * leptonTwo.eta);
      //use decimals to get random value
      split_val = ((event_val * 10.) - ((int) event_val * 10.)) / 10.; //take decimals 2 on as the random variable (e.g. X.abcde --> 0.bcde)
    } else if(fUseRandomField == 3) { //evaluate with event number
      split_val = (eventNumber % 1000) / 1000.f;
    }
    if(fFractionMVA > 0.f) fTreeVars.train = (split_val < fFractionMVA) ? 1.f : -1.f; //whether or not it is in the training sample
    else                   fTreeVars.train = -1.f;
    fTreeVars.trainfraction = std::min(1.f, std::max(0.f, fFractionMVA));
  } else {
    fTreeVars.train = -1.f;
    fTreeVars.trainfraction = 0.f;
  }

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
        printf("HistMaker::%s: Entry %lld: Unknown Z decay mode! lep_1 ID = %i, lep_2 ID = %i, lep_1_decay = %i, lep_2_decay = %i, Idx(decay 1) = %i, Idx(decay 2) = %i, nGenPart = %i\n",
               __func__, fentry, pdg_1, pdg_2, decay_1, decay_2, zLepOneDecayIdx, zLepTwoDecayIdx, nGenPart);
        if(fVerbose > 0) {
          printf("Gen-collection:\nIdx pdgId mother mother-ID\n");
          for(UInt_t ipart = 0; ipart < nGenPart; ++ipart) {
            const int mother = GenPart_genPartIdxMother[ipart];
            printf(" %3i: %6i %3i %6i\n", ipart, GenPart_pdgId[ipart], mother,
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
void HistMaker::FillBaseEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("HistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }

  //get rough uncertainty weights
  auto sys_weights         = OverallSystematicWeights();
  const float tot_sys_up   = sys_weights.first;
  const float tot_sys_down = sys_weights.second;


  Hist->hEventWeight              ->Fill(eventWeight);
  Hist->hLogEventWeight           ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hFullEventWeightLum       ->Fill(std::fabs(fTreeVars.fulleventweightlum));
  Hist->hLogFullEventWeightLum    ->Fill((std::fabs(fTreeVars.fulleventweightlum) > 1.e-10) ? std::log10(std::fabs(fTreeVars.fulleventweightlum)) : -999.);
  if(!fSparseHists) {
    Hist->hEventWeightMVA           ->Fill(fTreeVars.eventweightMVA);
    Hist->hGenWeight                ->Fill(genWeight);
    Hist->hEmbeddingWeight          ->Fill(embeddingWeight);
    Hist->hLogEmbeddingWeight       ->Fill((embeddingWeight > 1.e-10) ? std::log10(embeddingWeight) : -999.);
    Hist->hEmbeddingUnfoldingWeight ->Fill(embeddingUnfoldingWeight);
    Hist->hQCDWeight                ->Fill(qcdWeight     );
    Hist->hJetToTauWeight           ->Fill(jetToTauWeight);
    Hist->hJetToTauWeightCorr       ->Fill((jetToTauWeight     > 0.) ? jetToTauWeightCorr/jetToTauWeight     : -1., genWeight*eventWeight);
    Hist->hJetToTauWeightBias       ->Fill((jetToTauWeightCorr > 0.) ? jetToTauWeightBias/jetToTauWeightCorr : -1., genWeight*eventWeight);

    Hist->hIsSignal            ->Fill(fTreeVars.issignal      );
    Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
    Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
    Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
    Hist->hNPU[0]              ->Fill(nPU                , genWeight*eventWeight)      ;
    Hist->hLHENJets            ->Fill(LHE_Njets          , genWeight*eventWeight)      ;
  }
  Hist->hNPV[0]              ->Fill(nPV                , genWeight*eventWeight)      ;
  if(fUseEmbBDTUncertainty) {
    Hist->hEmbBDTWeight->Fill(embBDTWeight, genWeight*eventWeight);
  }
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight) ;
  Hist->hRunEra              ->Fill(runEra             , genWeight*eventWeight) ;
  Hist->hDataRun             ->Fill(runNumber          , genWeight*eventWeight)      ;
  // Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  Hist->hNGenTaus            ->Fill(nGenTaus           , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons      , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons          , genWeight*eventWeight)      ;
  Hist->hNGenPhotons         ->Fill(nGenPhotons        , genWeight*eventWeight)      ;
  Hist->hNGenHardPhotons     ->Fill(nGenHardPhotons    , genWeight*eventWeight)      ;
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;

  if(!fSparseHists) {
    Hist->hNTriggered          ->Fill(leptonOne.fired+leptonTwo.fired, genWeight*eventWeight)   ;
    Hist->hPuWeight            ->Fill(puWeight)     ;
    Hist->hJetPUIDWeight       ->Fill(jetPUIDWeight);
    Hist->hPrefireWeight       ->Fill(prefireWeight);
    Hist->hBTagWeight          ->Fill(btagWeight)   ;
    Hist->hZPtWeight           ->Fill(zPtWeight)    ;
    Hist->hSignalZWeight       ->Fill(signalZWeight);
    Hist->hSignalZMixWeight    ->Fill(signalZMixingWeight);
    Hist->hLepDisplacementWeight->Fill(lepDisplacementWeight);
    Hist->hPSSys               ->Fill(PSWeightMax);
    Hist->hPDFSys              ->Fill(LHEPdfWeightMax);
    Hist->hScaleRSys           ->Fill(LHEScaleRWeightMax);
    Hist->hScaleFSys           ->Fill(LHEScaleFWeightMax);

    if(tauP4 && tauP4->Pt() > 0.) { //if 0 then no additional tau stored
      Hist->hTauPt           ->Fill(tauP4->Pt()        , genWeight*eventWeight)   ;
      Hist->hTauEta          ->Fill(tauP4->Eta()       , genWeight*eventWeight)   ;
    }
  }
  Hist->hTauGenFlavor      ->Fill(tauGenFlavor               , genWeight*eventWeight)   ;
  Hist->hTauDecayMode[0]   ->Fill(tauDecayMode               , genWeight*eventWeight)   ;
  Hist->hTauDeepAntiEle    ->Fill(std::log2(tauDeepAntiEle+1), genWeight*eventWeight)   ;
  Hist->hTauDeepAntiMu     ->Fill(tauDeepAntiMu              , genWeight*eventWeight)   ;
  Hist->hTauDeepAntiJet    ->Fill(std::log2(tauDeepAntiJet+1), genWeight*eventWeight)   ;

  if(jetOne.p4 && jetOne.p4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOne.p4->Pt()            , genWeight*eventWeight)   ;
    Hist->hJetEta          ->Fill(jetOne.p4->Eta()           , genWeight*eventWeight)   ;
  }
  Hist->hHtSum             ->Fill(htSum              , genWeight*eventWeight)   ;
  Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMetSignificance   ->Fill(metSignificance    , genWeight*eventWeight)      ;
  Hist->hPuppiMetSig       ->Fill(puppMETSig         , genWeight*eventWeight)      ;
  Hist->hMetPhi            ->Fill(metPhi             , genWeight*eventWeight)      ;
  Hist->hMetCorr           ->Fill(metCorr            , genWeight*eventWeight)      ;
  Hist->hNuPt              ->Fill((fIsData) ? met : eventNuPt, genWeight*eventWeight)      ;
  Hist->hDetectorMet       ->Fill(eventDetectorMet   , genWeight*eventWeight)      ;
  Hist->hRawMet            ->Fill(rawMet             , genWeight*eventWeight)      ;
  Hist->hRawMetPhi         ->Fill(rawMetPhi          , genWeight*eventWeight)      ;
  Hist->hRawMetDiff        ->Fill(rawMet-met         , genWeight*eventWeight)      ;
  //approximate met uncertainty
  auto met_sys = ApproxMETSys();
  Hist->hMetUp  ->Fill(met_sys.first , genWeight*eventWeight);
  Hist->hMetDown->Fill(met_sys.second, genWeight*eventWeight);

  //met if no corrections were applied
  const float met_x_orig(met*std::cos(metPhi) - metCorr*std::cos(metCorrPhi)), met_y_orig(met*std::sin(metPhi) - metCorr*std::sin(metCorrPhi));
  const float met_no_corr(std::sqrt(met_x_orig*met_x_orig + met_y_orig*met_y_orig));
  Hist->hMetNoCorr         ->Fill(met_no_corr        , eventWeight*genWeight);

  Hist->hMetOverLepPt->Fill(met/fTreeVars.leppt, eventWeight*genWeight);
  Hist->hMetOverM    ->Fill(met/fTreeVars.lepm , eventWeight*genWeight);

  //mt(x, y) ~ sqrt(x*y) --> mt(x+-sigma_x,y) ~ sqrt(x+-sigma_x)/sqrt(x)*mt, sigma(mt) ~ mt*sqrt((x+-sigma_x)/x + (y+-sigma_y)/y)
  float mtone_up   = fTreeVars.mtone*std::sqrt((leptonOne.pt_up  ()/leptonOne.pt) + (met_sys.first )/met);
  float mtone_down = fTreeVars.mtone*std::sqrt((leptonOne.pt_down()/leptonOne.pt) + (met_sys.second)/met);
  float mttwo_up   = fTreeVars.mttwo*std::sqrt((leptonTwo.pt_up  ()/leptonTwo.pt) + (met_sys.first )/met);
  float mttwo_down = fTreeVars.mttwo*std::sqrt((leptonTwo.pt_down()/leptonTwo.pt) + (met_sys.second)/met);
  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTOneUp           ->Fill(mtone_up           , eventWeight*genWeight);
  Hist->hMTOneDown         ->Fill(mtone_down         , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTTwoUp           ->Fill(mttwo_up           , eventWeight*genWeight);
  Hist->hMTTwoDown         ->Fill(mttwo_down         , eventWeight*genWeight);
  Hist->hMTLead            ->Fill(fTreeVars.mtlead   , eventWeight*genWeight);
  Hist->hMTTrail           ->Fill(fTreeVars.mttrail  , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
  Hist->hMTDiff            ->Fill(fTreeVars.mtdiff   , eventWeight*genWeight);
  Hist->hMTRatio           ->Fill(fTreeVars.mtone / fTreeVars.mttwo, eventWeight*genWeight);
  Hist->hMTOneOverM        ->Fill(fTreeVars.mtoneoverm  ,eventWeight*genWeight);
  Hist->hMTTwoOverM        ->Fill(fTreeVars.mttwooverm  ,eventWeight*genWeight);
  Hist->hMTLepOverM        ->Fill(fTreeVars.mtlepoverm  ,eventWeight*genWeight);
  Hist->hMETDotOne         ->Fill(fTreeVars.metdotl1    , eventWeight*genWeight);
  Hist->hMETDotTwo         ->Fill(fTreeVars.metdotl2    , eventWeight*genWeight);
  Hist->hMETDotZOne        ->Fill(fTreeVars.metdotzl1    , eventWeight*genWeight);
  Hist->hMETDotZTwo        ->Fill(fTreeVars.metdotzl2    , eventWeight*genWeight);

  TLorentzVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
  const double lepDelR   = std::fabs(leptonOne.p4->DeltaR(*leptonTwo.p4));
  const double lepDelPhi = std::fabs(leptonOne.p4->DeltaPhi(*leptonTwo.p4));
  const double lepDelEta = std::fabs(leptonOne.p4->Eta() - leptonTwo.p4->Eta());
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  Hist->hMetDeltaPhi  ->Fill(fTreeVars.metdeltaphi  ,eventWeight*genWeight);
  if(!fSparseHists) {
    Hist->hLepOneDeltaPhi->Fill(fTreeVars.leponedeltaphi  ,eventWeight*genWeight);
    Hist->hLepTwoDeltaPhi->Fill(fTreeVars.leptwodeltaphi  ,eventWeight*genWeight);
  }

  Hist->hLepPt[0]     ->Fill(fTreeVars.leppt        ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(fTreeVars.lepm         ,eventWeight*genWeight);
  Hist->hLepMUp       ->Fill(fTreeVars.lepm         ,tot_sys_up  );
  Hist->hLepMDown     ->Fill(fTreeVars.lepm         ,tot_sys_down);
  // if(!fSparseHists) {
  //   Hist->hLepMLepESUp   ->Fill(fTreeVars.lepm         ,tot_sys_up  );
  //   Hist->hLepMLepESDown ->Fill(fTreeVars.lepm         ,tot_sys_down);
  // }
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
  // Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);
  Hist->hLepMEstimateThree->Fill(fTreeVars.mestimatethree, eventWeight*genWeight);
  Hist->hLepMEstimateFour->Fill(fTreeVars.mestimatefour, eventWeight*genWeight);
  Hist->hLepMEstimateCut[0]->Fill(fTreeVars.mestimate_cut_1, eventWeight*genWeight);
  Hist->hLepMEstimateCut[1]->Fill(fTreeVars.mestimate_cut_2, eventWeight*genWeight);
  Hist->hLepMEstimateAvg[0]->Fill(fTreeVars.mestimate_avg_1, eventWeight*genWeight);
  Hist->hLepMEstimateAvg[1]->Fill(fTreeVars.mestimate_avg_2, eventWeight*genWeight);
  Hist->hLepMBalance    ->Fill(fTreeVars.mbalance    , eventWeight*genWeight);
  Hist->hLepMBalanceTwo ->Fill(fTreeVars.mbalancetwo , eventWeight*genWeight);
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
  if(leptonOne.genPt > 0.f && leptonTwo.genPt > 0.f) {
    //associate the leptons by charge
    if(zLepOneID*leptonOne.flavor > 0) {
      Hist->hZLepOneELoss->Fill((zLepOnePt - leptonOne.genPt)/zLepOnePt, eventWeight*genWeight);
      Hist->hZLepTwoELoss->Fill((zLepTwoPt - leptonTwo.genPt)/zLepTwoPt, eventWeight*genWeight);
    } else {
      Hist->hZLepOneELoss->Fill((zLepTwoPt - leptonOne.genPt)/zLepTwoPt, eventWeight*genWeight);
      Hist->hZLepTwoELoss->Fill((zLepOnePt - leptonTwo.genPt)/zLepOnePt, eventWeight*genWeight);
    }
  } else {
      Hist->hZLepOneELoss->Fill(0.f, eventWeight*genWeight);
      Hist->hZLepTwoELoss->Fill(0.f, eventWeight*genWeight);
  }

  Hist->hPTauVisFrac  ->Fill(fTreeVars.ptauvisfrac , eventWeight*genWeight);
  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);
  Hist->hDeltaAlphaM[0]->Fill(fTreeVars.deltaalpham1, eventWeight*genWeight);
  Hist->hDeltaAlphaM[1]->Fill(fTreeVars.deltaalpham2, eventWeight*genWeight);
  Hist->hBeta[0]->Fill(fTreeVars.beta1, eventWeight*genWeight);
  Hist->hBeta[1]->Fill(fTreeVars.beta2, eventWeight*genWeight);
  // Hist->hDeltaAlphaMColM[0]->Fill(fTreeVars.mestimate    - fTreeVars.deltaalpham2, eventWeight*genWeight);
  // Hist->hDeltaAlphaMColM[1]->Fill(fTreeVars.mestimatetwo - fTreeVars.deltaalpham1, eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    // const float score = (fUseCDFBDTs == 1) ? fMvaCDFs[i] : (fUseCDFBDTs == 2) ? fMvaFofP[i] : fMvaOutputs[i];
    const float score = fMvaUse[i];
    if(score > -1.f && min_bdt_ > -1.f && score <= min_bdt_) {
      printf(" HistMaker::%s: Entry %lld: Filling BDT score %.3f but cut applied at %.3f\n", __func__, fentry, score, min_bdt_);
    }
    const float score_check = (fUseCDFBDTs == 1) ? fMvaCDFs[i] : (fUseCDFBDTs == 2) ? fMvaFofP[i] : fMvaOutputs[i];
    if(score > -1.f && score != score_check) {
      printf(" HistMaker::%s: Entry %lld: BDT score %.3f disagrees with check of %.3f\n", __func__, fentry, score, score_check);
    }
    Hist->hMVA[i][0]->Fill(score, fTreeVars.eventweightMVA); //remove training samples
    //MVA score vs mass for MVAs not correlated with mass
    if(!fSparseHists || fSelection == "emu") {
      if(fMVAConfig->names_[i].Contains("emu")) {
        if(fMVAConfig->names_[i].Contains("Z0")) Hist->hLepMVsMVA[0]->Fill(score, fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
        else                                     Hist->hLepMVsMVA[1]->Fill(score, fTreeVars.lepm, fTreeVars.eventweightMVA); //remove training samples
      }
    }
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][2]->Fill(fMvaCDFs   [i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][3]->Fill(fMvaLogP   [i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][4]->Fill(fMvaFofP   [i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][5]->Fill(score, fTreeVars.eventweightMVA*((bdtWeight > 0.f) ? 1./bdtWeight : 1.f)); //remove training samples and BDT score correction
    if (fTreeVars.train > 0) Hist->hMVATrain[i]->Fill(fMvaOutputs[i], fTreeVars.eventweight*((fTreeVars.trainfraction > 0.f) ? 1.f/fTreeVars.trainfraction : 1.f));
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
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,eventWeight*genWeight);
  Hist->hOnePtUp      ->Fill(leptonOne.pt_up()             ,eventWeight*genWeight);
  //fill pt down only if it would pass the event cuts
  const Float_t tmp_one_pt = leptonOne.pt;
  leptonOne.pt = leptonOne.pt_down();
  if(PassesCuts()) {
    Hist->hOnePtDown    ->Fill(leptonOne.pt                ,eventWeight*genWeight);
  }
  leptonOne.pt = tmp_one_pt;
  Hist->hOneMomErr    ->Fill(std::fabs(1.f - leptonOne.ES[1]/leptonOne.ES[0]), eventWeight*genWeight);
  Hist->hOnePtRelErr  ->Fill((leptonOne.genPt > 0.f) ? (leptonOne.pt - leptonOne.genPt) / leptonOne.pt : 0.f, eventWeight*genWeight);
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
  Hist->hOnePileup    ->Fill(leptonOne.isPileup            ,eventWeight*genWeight);
  Hist->hOneJetOrigin ->Fill(leptonOne.jetOrigin           ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOne.flavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hOneJetOverlap->Fill(leptonOne.jetOverlap          ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOne.trigger             ,eventWeight*genWeight);
  Hist->hOneWeight    ->Fill(leptonOne.wt1[0]*leptonOne.wt2[0]);
  Hist->hOneTrigWeight->Fill(leptonOne.trig_wt);

  Hist->hOneMetDeltaPhi   ->Fill(fTreeVars.onemetdeltaphi   ,eventWeight*genWeight);

  //Interesting electron IDs
  if(nElectron == 1) {
    Hist->hEleConvVeto   ->Fill(Electron_convVeto     [0], eventWeight*genWeight);
    Hist->hEleLostHits   ->Fill(Electron_lostHits     [0], eventWeight*genWeight);
    Hist->hElePFCand     ->Fill(Electron_isPFcand     [0], eventWeight*genWeight);
  }

  //Electron ID study
  if(fDoEleIDStudy && nElectron > 0) { //take the leading electron, if found
    Hist->hEleCutID        ->Fill(Electron_cutBased     [0], eventWeight*genWeight);
    Hist->hEleEtaEta       ->Fill(Electron_sieie        [0], eventWeight*genWeight);
    Hist->hEleR9           ->Fill(Electron_r9           [0], eventWeight*genWeight);
    Hist->hEle3DPVErr      ->Fill(Electron_sip3d        [0], eventWeight*genWeight);
    Hist->hEleSCEtOverPt   ->Fill(Electron_scEtOverPt   [0], eventWeight*genWeight);
    Hist->hEleHOE          ->Fill(Electron_hoe          [0], eventWeight*genWeight);
    Hist->hEleEInvMinusPInv->Fill(Electron_eInvMinusPInv[0], eventWeight*genWeight);
    Hist->hEleEnergyErr    ->Fill(Electron_energyErr[0]/Electron_pt[0], eventWeight*genWeight);
    // Hist->hElePhotonIdx    ->Fill(Electron_photonIdx[0], eventWeight*genWeight);
  }

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPz        ->Fill(leptonTwo.p4->Pz()            ,eventWeight*genWeight);
  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  Hist->hTwoPtUp      ->Fill(leptonTwo.pt_up()             ,eventWeight*genWeight);
  //fill pt down only if it would pass the event cuts
  const Float_t tmp_two_pt = leptonTwo.pt;
  leptonTwo.pt = leptonTwo.pt_down();
  if(PassesCuts()) {
    Hist->hTwoPtDown    ->Fill(leptonTwo.pt                ,eventWeight*genWeight);
  }
  leptonTwo.pt = tmp_two_pt;
  Hist->hTwoMomErr    ->Fill(std::fabs(1.f - leptonTwo.ES[1]/leptonTwo.ES[0]), eventWeight*genWeight);
  Hist->hTwoPtRelErr  ->Fill((leptonTwo.genPt > 0.f) ? (leptonTwo.pt - leptonTwo.genPt) / leptonTwo.pt : 0.f, eventWeight*genWeight);
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
  Hist->hTwoMetDeltaPhi->Fill(fTreeVars.twometdeltaphi  ,eventWeight*genWeight);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(std::abs(leptonTwo.flavor)        ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwo.flavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  Hist->hTwoJetOverlap->Fill(leptonTwo.jetOverlap          ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwo.trigger               ,eventWeight*genWeight);
  Hist->hTwoWeight    ->Fill(leptonTwo.wt1[0]*leptonTwo.wt2[0]);
  Hist->hTwoTrigWeight->Fill(leptonTwo.trig_wt);

  if(leptonTwo.isTau()) { //try to extract more info for gen-matching to taus
    int gen_flavor = leptonTwo.genFlavor;
    if(!fIsData && gen_flavor == 26) {
      gen_flavor = FindTauGenType(leptonTwo);
    }
    Hist->hTwoGenFlavor ->Fill(gen_flavor ,eventWeight*genWeight);
  } else {
    Hist->hTwoGenFlavor ->Fill(leptonTwo.genFlavor           ,eventWeight*genWeight);
  }
  Hist->hTwoPileup    ->Fill(leptonTwo.isPileup            ,eventWeight*genWeight);
  Hist->hTwoJetOrigin ->Fill(leptonTwo.jetOrigin           ,eventWeight*genWeight);



  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.ptdiff                      ,eventWeight*genWeight);
  Hist->hPtRatio     ->Fill(fTreeVars.ptratio                     ,eventWeight*genWeight);
  Hist->hPtTrailOverLead->Fill(fTreeVars.pttrailoverlead          ,eventWeight*genWeight);
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
  if(!fDoSSSystematics && !chargeTest) return;
  if(!fDoLooseSystematics && looseQCDSelection) return;
  if(eventWeight*genWeight == 0.) return; //no way to re-scale 0, contributes nothing to histograms so can just skip filling

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
    } else if(sys ==  4) {                                                 //muon ID scale factors
      if(leptonOne.isMuon    ()) weight *= leptonOne.wt1[1] / leptonOne.wt1[0];
      if(leptonTwo.isMuon    ()) weight *= leptonTwo.wt1[1] / leptonTwo.wt1[0];
    } else if(sys ==  5) {
      if(leptonOne.isMuon    ()) weight *= leptonOne.wt1[2] / leptonOne.wt1[0];
      if(leptonTwo.isMuon    ()) weight *= leptonTwo.wt1[2] / leptonTwo.wt1[0];
    } else if(sys ==  7) {                                                 //tau ID scale factors
      if(leptonOne.isTau     ()) weight *= leptonOne.wt1[1] / leptonOne.wt1[0];
      if(leptonTwo.isTau     ()) weight *= leptonTwo.wt1[1] / leptonTwo.wt1[0];
    } else if(sys ==  8) {
      if(leptonOne.isTau     ()) weight *= leptonOne.wt1[2] / leptonOne.wt1[0];
      if(leptonTwo.isTau     ()) weight *= leptonTwo.wt1[2] / leptonTwo.wt1[0];
    else if  (sys == 13) weight *= zPtWeightUp          / zPtWeight      ; //Drell-Yan pT weights
    else if  (sys == 14) weight *= zPtWeightDown        / zPtWeight      ;
    else if  (sys == 15) weight *= zPtWeightSys         / zPtWeight      ;
    } else if  (sys == 45) { weight *= (btagWeight > 0.) ? btagWeightUpBC   / btagWeight : 1.; //btag uncertainty
    } else if  (sys == 46) { weight *= (btagWeight > 0.) ? btagWeightDownBC / btagWeight : 1.;
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
    for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
      const int category = fMVAConfig->data_cat_[i];
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
  fChain->GetTree()->GetEntry(entry);
  IncrementTimer("Reading", true);

  fTimes[GetTimerNumber("EventInit")] = std::chrono::steady_clock::now(); //timer for initializing event info
  if(fVerbose > 0 || (fTimeCounts[GetTimerNumber("Total")]-1)%fNotifyCount == 0) {
    const double dur = fDurations[GetTimerNumber("Total")]/1.e6;
    //Number of entries in the tree (or partial tree if not processing the entire dataset)
    const Long64_t entries = (fMaxEntries < 0) ? fChain->GetEntriesFast() : fMaxEntries;
    if(fMaxEntries < 0) fMaxEntries = entries;
    const int seen = fTimeCounts[GetTimerNumber("Total")] - 1;
    const double rate = (dur > 0.) ? seen/dur : 0.;
    const double remain = (entries - seen)/rate;
    printf("%s: Processing event: %12i (%5.1f%%), entry %12lld, overall rate = %9.1f Hz (%6.1f %s remaining)\n", __func__,
           seen,
           seen*100./entries,
           entry,
           rate,
           (rate > 0.) ? (remain > 180.) ? remain/60. : remain : 0.,
           (rate > 0.) ? (remain > 180.) ?    "min"   :  "sec" : "min"
           );
  }

  icutflow = 0;
  fCutFlow->Fill(icutflow); ++icutflow; //0

  //Filter ntuple-level details
  if(fMaxLepM > 0.f && SelectionFilter_LepM > 0.f && SelectionFilter_LepM > fMaxLepM) {
    IncrementTimer("EventInit", true);
    if(fVerbose > 0) printf(" Event fails max mass cut (%.1f) with ntuple-level mass %.1f \n", fMaxLepM, SelectionFilter_LepM);
    return kTRUE;
  }
  if(fMinLepM > 0.f && SelectionFilter_LepM > 0.f && SelectionFilter_LepM < fMinLepM) {
    IncrementTimer("EventInit", true);
    if(fVerbose > 0) printf(" Event fails min mass cut (%.1f) with ntuple-level mass %.1f \n", fMinLepM, SelectionFilter_LepM);
    return kTRUE;
  }

  //Filter by event flags
  if(fUseFlags) {
    bool passed = true;
    passed &= fFlags.METFilters && fFlags.BadChargedCandidateFilter && fFlags.goodVertices;
    passed &= fFlags.HBHENoiseFilter && fFlags.HBHENoiseIsoFilter && fFlags.eeBadSCFilter;
    passed &= fFlags.muonBadTrackFilter && fFlags.EcalDeadCellTriggerPrimitiveFilter;
    passed &= fFlags.globalTightHalo2016Filter && fFlags.BadPFMuonFilter;
    if(!passed) {
      IncrementTimer("EventInit", true);
      if(fVerbose > 0) printf(" Event fails event flag filtering\n");
      return kTRUE;
    }
  }

  //reset the event cuts
  one_pt_min_ = 0.f; two_pt_min_ = 0.f;
  ptdiff_min_ = -1.e10f; ptdiff_max_ = +1.e10f;
  min_mass_ = -1.f; max_mass_ = -1.f;
  met_max_ = -1.f; mtone_max_ = -1.f; mttwo_max_ = -1.f; mtlep_max_ = -1.f;
  mtlep_over_m_max_ = -1.f; mtone_over_m_max_ = -1.f; mttwo_over_m_max_ = -1.f;
  min_bdt_ = -10.f;

  //Initialize base object information
  CountObjects(); // > 100 kHz
  if(!(mutau or etau or emu or mumu or ee)) {
    IncrementTimer("EventInit", true);
    if(fVerbose > 0) printf(" Event passes no selections \n");
    return kTRUE;
  }

  //MC sample splitting
  if(SplitSampleEvent()) {
    IncrementTimer("EventInit", true);
    if(fVerbose > 0) printf(" Event is filtered out due to sample splitting \n");
    return kTRUE;
  }

  fCutFlow->Fill(icutflow); ++icutflow; //1

  ////////////////////////
  // Data overlap check //
  ////////////////////////

  //skip if it's data and lepton status doesn't match data set (1 = electron, 2 = muon, 3 = MuonEG) unless allowing overlap and it passes both
  if(fIsData > 0) {
    //Use trigger matches, without reco pT cut, for data overlap rejection
    //--> Beyond here, with overlap rejected, simply require a lepton fires with reco pT cuts

    if(fIsData == 1 || fIsData == 2) { //single electron or muon data
      const int pdgid = (fIsData == 1) ? 11 : 13; //pdg ID for the data stream
      //if no selected lepton fired this trigger, continue
      if(!((std::abs(leptonOne.flavor) == pdgid && leptonOne.matched) || (std::abs(leptonTwo.flavor) == pdgid && leptonTwo.matched))) {
        IncrementTimer("EventInit", true);
        if(fVerbose > 0) printf(" Event fails trigger matching\n");
        return kTRUE;
      }
    } else if(fIsData == 3) { //MuonEG data
      //check that one of the e-mu triggers was matched to
      if(!(leptonOne.emu_matched && leptonTwo.emu_matched) && !(leptonOne.mue_matched && leptonTwo.mue_matched)) {
        IncrementTimer("EventInit", true);
        if(fVerbose > 0) printf(" Event fails e-mu/mu-e trigger matching\n");
        return kTRUE;
      }
    }

    if(fIsData == 1 || fIsData == 2) { //single muon/electron data
      if(triggerLeptonStatus != ((UInt_t) fIsData)) { //trigger doesn't match data stream
        if(triggerLeptonStatus != 3) {
          IncrementTimer("EventInit", true);
          if(fVerbose > 0) printf(" Event fails data trigger stream splitting\n");
          return kTRUE; //triggered only for the other stream
        }
        if(fSkipDoubleTrigger) { //don't allow electron and muon triggered event
          const int other_pdgid = (fIsData == 1) ? 13 : 11; //pdg ID for the other data stream
          //only skip if the selected lepton actually fired the trigger
          if((std::abs(leptonOne.flavor) == other_pdgid && leptonOne.matched) ||(std::abs(leptonTwo.flavor) == other_pdgid && leptonTwo.matched)) {
            IncrementTimer("EventInit", true);
            if(fVerbose > 0) printf(" Event fails data trigger stream overlap splitting\n");
            return kTRUE;
          }
        }
      }
    } //end single muon/electron data overlap checking
    else if(fIsData == 3) { //MuonEG data overlap checking
      if(dataTriggerFlags & (1 << 0) || dataTriggerFlags & (1 << 1)) { //passed a single lepton dataset trigger flag
        IncrementTimer("EventInit", true);
        if(fVerbose > 0) printf(" Event fails MuonEG data trigger stream splitting\n");
        return kTRUE;
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

  //Remove MVA training events if requested
  if(fRemoveTraining && fTreeVars.train > 0.5f) {
    IncrementTimer("EventInit", true);
    if(fVerbose > 0) printf(" MVA training event removed\n");
    return kTRUE;
  }


  /////////////////////////
  // Apply event weights //
  /////////////////////////

  fTimes[GetTimerNumber("InitWeights")] = std::chrono::steady_clock::now(); //timer for initializing object info
  InitializeEventWeights();
  IncrementTimer("InitWeights", true);

  if(fVerbose > 1) {
    printf(" Event weights:\n  event = %.5f, gen = %.4f, pu = %.3f, btag = %.3f, trig = %.3f, jetPUID = %.3f, zPt = %.3f, sig = %.3f, sigMix = %.3f, dxyz = %.3f\n",
           eventWeight, genWeight, puWeight, btagWeight, leptonOne.trig_wt*leptonTwo.trig_wt, jetPUIDWeight, zPtWeight, signalZWeight, signalZMixingWeight, lepDisplacementWeight);
    printf("  embedGen = %.3f, embedUnfold = %.3f, j-->tau = %.3f, qcd = %.3f\n", embeddingWeight, embeddingUnfoldingWeight, jetToTauWeightBias, qcdWeight);
    if(leptonOne.isElectron()) {
      printf("  one(  e): ID = %.3f, Iso ID = %.3f, reco = %.3f\n", leptonOne.wt1[0], leptonOne.wt3[0], leptonOne.wt2[0]);
    } else if(leptonOne.isMuon()) {
      printf("  one( mu): ID = %.3f, Iso ID = %.3f\n", leptonOne.wt1[0], leptonOne.wt2[0]);
    } else if(leptonOne.isTau()) {
      printf("  one(tau): \n");
    }
    if(leptonTwo.isElectron()) {
      printf("  two(  e): ID = %.3f, Iso ID = %.3f, reco = %.3f\n", leptonTwo.wt1[0], leptonTwo.wt2[0], leptonTwo.wt3[0]);
    } else if(leptonTwo.isMuon()) {
      printf("  two( mu): ID = %.3f, Iso ID = %.3f\n", leptonTwo.wt1[0], leptonTwo.wt2[0]);
    } else if(leptonTwo.isTau()) {
      printf("  two(tau): \n");
    }
  }

  if(eventWeight < 0. || !std::isfinite(eventWeight*genWeight)) {
    printf("WARNING! Skipping event %lld as it has negative bare event weight or undefined total weight:\n", fentry);
    printf("Event weights: event = %.5f, gen = %.4f, pu = %.3f, btag = %.3f, trig = %.3f jetPUID = %.3f\n",
           eventWeight, genWeight, puWeight, btagWeight, leptonOne.trig_wt*leptonTwo.trig_wt, jetPUIDWeight);
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

  //Print event summary after initialization if requested
  if(fVerbose) {
    printf(" HistMaker::%s summary:\n", __func__);
    printf("  di-lepton: mass = %.1f, pt = %.1f, eta = %5.1f, phi = %.5f, mt(ll,met) = %.1f\n", fTreeVars.lepm, fTreeVars.leppt, fTreeVars.lepeta,
           (*leptonOne.p4+*leptonTwo.p4).Phi(),
           fTreeVars.mtlep);
    printf("  lep_1: pdg = %3i, pt = %5.1f, eta(SC) = %5.2f(%5.2f), mt(l,met) = %5.1f, matched = %o\n", leptonOne.flavor, leptonOne.pt, leptonOne.eta, leptonOne.scEta, fTreeVars.mtone, leptonOne.matched);
    printf("  lep_2: pdg = %3i, pt = %5.1f, eta(SC) = %5.2f(%5.2f), mt(l,met) = %5.1f, matched = %o\n", leptonTwo.flavor, leptonTwo.pt, leptonTwo.eta, leptonTwo.scEta, fTreeVars.mttwo, leptonTwo.matched);
    printf("  event: met = %.1f, met phi = %.5f, N(jets) = %i, N(tight b) = %i, N(loose b) = %i, jet pt = %.1f, weight = %.4f\n", fTreeVars.met, metPhi, nJets20, nBJets20, nBJets20L, fTreeVars.jetpt, eventWeight*genWeight);
    // if(fIsData || fIsEmbed)
    printf("  event = %llu, lumi = %u, run = %u\n", eventNumber, lumiSection, runNumber);
  }

  IncrementTimer("EventInit", true);

  return kFALSE;
}

//-----------------------------------------------------------------------------------------------------------------
//Check if lepton matches to a trigger
// int HistMaker::GetTriggerMatch(TLorentzVector* lv, bool isMuon, Int_t& trigIndex) {
int HistMaker::GetTriggerMatch(Lepton_t& lep, bool isMuon) {
  Int_t& trigIndex = lep.trigger;
  TLorentzVector* lv = lep.p4;
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

  if     (passedBit1) lep.trigger_id = 1; //default to the first trigger bit
  else if(passedBit2) lep.trigger_id = 2;
  else                lep.trigger_id = 0; //no match

  return (passedBit1+2*passedBit2);//no matching trigger object found = 0
}

//--------------------------------------------------------------------------------------------------------------
// Match trigger objects to selected leptons
void HistMaker::MatchTriggers() {
  leptonOne.fired = false; leptonTwo.fired = false;

  const bool mu_trig  = ((fYear == 2016 && HLT_IsoMu24) ||
                        ( fYear == 2017 && HLT_IsoMu27) ||
                        ( fYear == 2018 && HLT_IsoMu24));
  const bool ele_trig = ((fYear == 2016 && HLT_Ele27_WPTight_GsF) ||
                        ( fYear == 2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) ||
                        ( fYear == 2018 && HLT_Ele32_WPTight_GsF));
  const bool emu_trig = HLT_Mu8_Ele23;
  const bool mue_trig = HLT_Mu23_Ele12;

  //Set event-level flag info for dataset filtering
  dataTriggerFlags = ele_trig + 2*mu_trig + 4*emu_trig + 8*mue_trig;

  //FIXME: Perhaps remove reco trig pT cut from matching to better eval energy scale uncertainties
  if(leptonOne.isTau     ()) {
    leptonOne.matched = false; leptonOne.fired = false;
  } else if(leptonOne.isMuon    ()) {
    leptonOne.matched = mu_trig  && (!fDoTriggerMatching || GetTriggerMatch(leptonOne, true));
    leptonOne.fired = leptonOne.matched && leptonOne.p4->Pt() > muon_trig_pt_;
  } else if(leptonOne.isElectron()) {
    leptonOne.matched = ele_trig && (!fDoTriggerMatching || GetTriggerMatch(leptonOne, false));
    leptonOne.fired = leptonOne.matched && leptonOne.p4->Pt() > electron_trig_pt_;
  }

  if(leptonTwo.isTau     ()) {
    leptonTwo.matched = false; leptonTwo.fired = false; leptonTwo.trigger = -1; leptonTwo.trigger_id = 0;
  } else if(leptonTwo.isMuon    ()) {
    leptonTwo.matched = mu_trig  && (!fDoTriggerMatching || GetTriggerMatch(leptonTwo, true));
    leptonTwo.fired   = leptonTwo.matched && leptonTwo.p4->Pt() > muon_trig_pt_;
  } else if(leptonTwo.isElectron()) {
    leptonTwo.matched = ele_trig && (!fDoTriggerMatching || GetTriggerMatch(leptonTwo, false));
    leptonTwo.fired   = leptonTwo.matched && leptonTwo.p4->Pt() > electron_trig_pt_;
  }

  //check specifically for the e-mu trigger
  if(leptonOne.isElectron() && leptonTwo.isMuon()) {
    //FIXME: Add trigger object matching for e-mu triggers
    if(emu_trig) {
      leptonOne.emu_matched = true;
      leptonTwo.emu_matched = true;
      leptonOne.emu_fired = leptonOne.pt > emu_trig_ele_pt_;
      leptonTwo.emu_fired = leptonTwo.pt > emu_trig_mu_pt_ ;
    }
    if(mue_trig) {
      leptonOne.mue_matched = true;
      leptonTwo.mue_matched = true;
      leptonOne.mue_fired = leptonOne.pt > mue_trig_ele_pt_;
      leptonTwo.mue_fired = leptonTwo.pt > mue_trig_mu_pt_ ;
    }
  } else {
    leptonOne.emu_matched = false;
    leptonTwo.emu_matched = false;
    leptonOne.emu_fired   = false;
    leptonTwo.emu_fired   = false;
    leptonOne.mue_matched = false;
    leptonTwo.mue_matched = false;
    leptonOne.mue_fired   = false;
    leptonTwo.mue_fired   = false;
  }

  //Event trigger matching without reco pT cuts, as only used for data stream overlap removal between muon and electron data
  triggerLeptonStatus = 0;
  if((leptonOne.isElectron() && leptonOne.matched) || //electron matched to a trigger
     (leptonTwo.isElectron() && leptonTwo.matched)) {
    triggerLeptonStatus += 1;
  }
  if((leptonOne.isMuon    () && leptonOne.matched) || //muon matched to a trigger
     (leptonTwo.isMuon    () && leptonTwo.matched)) {
    triggerLeptonStatus += 2;
  }

  //check if the Mu50 (2) but not IsoMu (1) trigger is fired in the event
  muonTriggerStatus = (leptonOne.isMuon() && leptonOne.fired) ? leptonOne.trigger_id : 0;
  if(muonTriggerStatus != 1) muonTriggerStatus = (leptonTwo.isMuon() && leptonTwo.fired) ? leptonTwo.trigger_id : muonTriggerStatus;


  //matches are within 0.1 delta R, so both to be matched must be within 0.2 and therefore will be rejected
  if(fVerbose > 0 && leptonOne.trigger == leptonTwo.trigger && leptonOne.matched && leptonTwo.matched) {
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

  if(fRemoveEventWeights > 0) {
    triggerWeightsSys[0] = 1.f;
    triggerWeightsSys[1] = 1.f;
    triggerWeightsSys[2] = 1.f;
    triggerWeightsSys[3] = 1.f;
    triggerWeightsSys[4] = 1.f;
    triggerWeightsSys[5] = 1.f;
    leptonOne.trig_wt = 1.f;
    leptonTwo.trig_wt = 1.f;
    return;
  }

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
      fMuonIDWeight.TriggerEff               (leptonOne.p4->Pt(), leptonOne.p4->Eta(), fYear, !leptonOne.fired || leptonOne.trigger_id != 2, mcEra,
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
      fMuonIDWeight.TriggerEff               (leptonTwo.p4->Pt(), leptonTwo.p4->Eta(), fYear, !leptonTwo.fired || leptonTwo.trigger_id != 2, mcEra,
                                              data_eff[1], mc_eff[1], data_up[1], mc_up[1], data_down[1], mc_down[1]);
    }
  }

  // Taus
  if(leptonTwo.isTau     ()) {
    data_eff[1] = 0.f; mc_eff[1] = 0.f; //tau can't trigger
    data_up[1] = 0.f; mc_up[1] = 0.f;
    data_down[1] = 0.f; mc_down[1] = 0.f;

  }

  leptonOne.trig_data_eff = data_eff[0];
  leptonOne.trig_mc_eff   = mc_eff  [0];
  leptonTwo.trig_data_eff = data_eff[1];
  leptonTwo.trig_mc_eff   = mc_eff  [1];

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
void HistMaker::EvalJetToTauWeights(float& wt, float& wtcorr, float& wtbias) {
  wt = 0.f; wtcorr = 0.f; wtbias = 0.f;
  //j-->tau weight, by process
  for(int proc = 0; proc < JetToTauComposition::kLast; ++proc) {
    wt     += fJetToTauComps[proc] * fJetToTauWts[proc];
    wtcorr += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
    if(fApplyJetToTauMCBias || (proc != JetToTauComposition::kWJets && proc != JetToTauComposition::kZJets)) {
      wtbias += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc] * fJetToTauBiases[proc];
    } else {
      wtbias += fJetToTauComps[proc] * fJetToTauWts[proc] * fJetToTauCorrs[proc];
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
// Evaluate the embedding BDT uncertainty
float HistMaker::EvalEmbBDTUncertainty(TString selection) {
  if(!fReprocessMVAs) return 1.f;
  if(!fIsEmbed) return 1.f;
  if(!fUseEmbBDTUncertainty) return 1.f;

  //Store the original event information
  const TLorentzVector o_lv1(*leptonOne.p4), o_lv2(*leptonTwo.p4), o_jet(*jetOne.p4);
  const float o_met(met), o_metPhi(metPhi);
  float o_mvas[fMVAConfig->names_.size()], o_cdfs[fMVAConfig->names_.size()], o_fofp[fMVAConfig->names_.size()];
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    o_mvas[i] = fMvaOutputs[i];
    o_cdfs[i] = fMvaCDFs[i];
    o_fofp[i] = fMvaFofP[i];
  }

  //Override with the embedded event kinematics
  if(zLepOnePt > zLepTwoPt) { //order by leading pT
    leptonOne.setPtEtaPhiM(zLepOnePt, zLepOneEta, zLepOnePhi, MUONMASS);
    leptonTwo.setPtEtaPhiM(zLepTwoPt, zLepTwoEta, zLepTwoPhi, MUONMASS);
  } else {
    leptonTwo.setPtEtaPhiM(zLepOnePt, zLepOneEta, zLepOnePhi, MUONMASS);
    leptonOne.setPtEtaPhiM(zLepTwoPt, zLepTwoEta, zLepTwoPhi, MUONMASS);
  }
  met    = eventDetectorMet; //use the MET without the neutrinos
  metPhi = eventDetectorMetPhi;
  SetKinematics();

  //Evaluate the BDT score
  EvalMVAs("EmbBDTMVAs");

  //Evaluate the uncertainty for the selection (uses F(p) in the uncertainty evaluation)
  const float score = fMvaFofP[fMVAConfig->GetIndexBySelection(selection)];
  const float weight = fEmbBDTUncertainty.Weight(score, fYear, selection);

  //Restore the event information
  leptonOne.setP(o_lv1);
  leptonTwo.setP(o_lv2);
  met = o_met;
  metPhi = o_metPhi;
  SetKinematics();
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    fMvaOutputs[i] = o_mvas[i];
    fMvaCDFs[i] = o_cdfs[i];
    fMvaFofP[i] = o_fofp[i];
  }

  return weight;
}

//--------------------------------------------------------------------------------------------------------------
// Get the approximate overall MET variations
std::pair<float,float> HistMaker::ApproxMETSys() {
  if(fIsData) return std::pair<float,float>(met,met); //no uncertainty on the data MET

  //Add fractional sources of uncertainty in quadrature as METErr/MET
  float met_err_up(0.f), met_err_down(0.f);

  //Add the JER/JES uncertainties
  if(!fIsData && (!fIsEmbed || fEmbedUseMETUnc == 1)) {
    met_err_up   = std::sqrt(std::pow(puppMETJERUp   - met, 2) + std::pow(puppMETJESUp   - met, 2))/met;
    met_err_down = std::sqrt(std::pow(puppMETJERDown - met, 2) + std::pow(puppMETJESDown - met, 2))/met;
  }

  //Add detector MET uncertainty in Embedding if requested
  if(fIsEmbed && fEmbedUseMETUnc == 2) {
    const float embed_met_err = (puppMETJESUp-met)/met; //JES uncertainty is re-used for this uncertainty
    met_err_up   = embed_met_err;
    met_err_down = 0.f; //embed_met_err;
  }

  //Add the lepton energy scale variations to the MET
  if(std::abs(leptonOne.flavor) == std::abs(leptonTwo.flavor)) { //correlate the two legs
    float met_diff_up   = METUpdate(leptonOne.pt_up()   - leptonOne.pt, leptonOne.phi) - met;
    float met_diff_down = METUpdate(leptonOne.pt_down() - leptonOne.pt, leptonOne.phi) - met;
    met_diff_up        += METUpdate(leptonTwo.pt_up()   - leptonTwo.pt, leptonTwo.phi) - met;
    met_diff_down      += METUpdate(leptonTwo.pt_down() - leptonTwo.pt, leptonTwo.phi) - met;
    met_err_up   = std::sqrt(std::pow(met_err_up  , 2) + std::pow(met_diff_up  /met, 2));
    met_err_down = std::sqrt(std::pow(met_err_down, 2) + std::pow(met_diff_down/met, 2));
  } else { //add lepton ES in quadrature
    met_err_up   = std::sqrt(std::pow(met_err_up  , 2) + std::pow(METUpdate(leptonOne.pt_up()   - leptonOne.pt, leptonOne.phi)/met - 1.f, 2));
    met_err_down = std::sqrt(std::pow(met_err_down, 2) + std::pow(METUpdate(leptonOne.pt_down() - leptonOne.pt, leptonOne.phi)/met - 1.f, 2));
    met_err_up   = std::sqrt(std::pow(met_err_up  , 2) + std::pow(METUpdate(leptonTwo.pt_up()   - leptonTwo.pt, leptonTwo.phi)/met - 1.f, 2));
    met_err_down = std::sqrt(std::pow(met_err_down, 2) + std::pow(METUpdate(leptonTwo.pt_down() - leptonTwo.pt, leptonTwo.phi)/met - 1.f, 2));
  }

  //Add the lepton energy resolution variations to the MET (Embedding only)
  if(fIsEmbed) {
    if(std::abs(leptonOne.flavor) == std::abs(leptonTwo.flavor)) { //correlate the two legs
      float met_diff_up   = METUpdate(leptonOne.pt_res_up()   - leptonOne.pt, leptonOne.phi) - met;
      float met_diff_down = METUpdate(leptonOne.pt_res_down() - leptonOne.pt, leptonOne.phi) - met;
      met_diff_up        += METUpdate(leptonTwo.pt_res_up()   - leptonTwo.pt, leptonTwo.phi) - met;
      met_diff_down      += METUpdate(leptonTwo.pt_res_down() - leptonTwo.pt, leptonTwo.phi) - met;
      met_err_up   = std::sqrt(std::pow(met_err_up  , 2) + std::pow(met_diff_up  /met, 2));
      met_err_down = std::sqrt(std::pow(met_err_down, 2) + std::pow(met_diff_down/met, 2));
    } else { //add lepton ES in quadrature
      met_err_up   = std::sqrt(std::pow(met_err_up  , 2) + std::pow(METUpdate(leptonOne.pt_res_up()   - leptonOne.pt, leptonOne.phi)/met - 1.f, 2));
      met_err_down = std::sqrt(std::pow(met_err_down, 2) + std::pow(METUpdate(leptonOne.pt_res_down() - leptonOne.pt, leptonOne.phi)/met - 1.f, 2));
      met_err_up   = std::sqrt(std::pow(met_err_up  , 2) + std::pow(METUpdate(leptonTwo.pt_res_up()   - leptonTwo.pt, leptonTwo.phi)/met - 1.f, 2));
      met_err_down = std::sqrt(std::pow(met_err_down, 2) + std::pow(METUpdate(leptonTwo.pt_res_down() - leptonTwo.pt, leptonTwo.phi)/met - 1.f, 2));
    }
  }

  float met_up   = std::max(0.f, met*(1.f + met_err_up  ));
  float met_down = std::max(0.f, met*(1.f - met_err_down));
  if(met_up < met_down) std::swap(met_up, met_down);
  return std::pair<float,float>(met_up, met_down);
}

//--------------------------------------------------------------------------------------------------------------
// Get the non-closure systematic variation on a j-->tau weight
float HistMaker::EvalJetToTauStatSys(int process, int dm_bin, int param_bin, bool up) {
  const float wt(eventWeight*genWeight); //nominal event weight
  float wt_alt = 0.f;

  //get the alternate j-->tau weight
  for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
    //Z+jets uses the W+jets scales
    const bool proc_test = iproc == process || (process == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets);
    const bool dm_test   = (tauDecayMode % 10 <= 1 || tauDecayMode % 10 == 1) && (tauDecayMode%10 + 2*(tauDecayMode/10)) == dm_bin;
    const bool alt_test  = param_bin < fJetToTauAltNum[process];
    const bool test = proc_test && dm_test && alt_test;;
    float corr(1.f);
    if(fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets)) { //apply composition and the non-closure and bias corrections
      corr = fJetToTauComps[iproc] * fJetToTauCorrs[iproc] * fJetToTauBiases[iproc];
    } else { //only apply the non-closure and composition
      corr = fJetToTauComps[iproc] * fJetToTauCorrs[iproc];
    }
    //use the alternate base j-->tau weight if it passes the checks
    if(test) {
      wt_alt += corr * ((up) ? fJetToTauAltUp[iproc*kMaxAltFunc + param_bin] : fJetToTauAltDown[iproc*kMaxAltFunc + param_bin]);
    } else { //nominal weight
      wt_alt += corr * fJetToTauWts[iproc];
    }
  }
  if(wt_alt <= 0.f || (fVerbose && std::abs(wt_alt-jetToTauWeightBias)/wt_alt > 2.f)) {
        printf("HistMaker::%s: Entry %lld: Unexpected j-->tau weight variation! wt = %.3f, nominal = %.3f, proc = %i, dm_bin = %i, dm = %i\n",
               __func__, fentry, wt_alt, jetToTauWeightBias, process, dm_bin, tauDecayMode);
  }

  //return the systematically shifted event weight
  const float sys_wt = wt * (wt_alt / jetToTauWeightBias);
  return sys_wt;
}

//--------------------------------------------------------------------------------------------------------------
// Get the non-closure systematic variation on a j-->tau weight
float HistMaker::EvalJetToTauNCSys(int process, bool up) {
  const float wt(eventWeight*genWeight); //nominal event weight

  //get the alternate j-->tau weight
  float wt_alt = 0.f;
  for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
    //Z+jets uses the W+jets scales
    const bool test = iproc == process || (process == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets);
    float base = fJetToTauComps[iproc] * fJetToTauWts[iproc];
    if(fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets)) { //bias is applied to nominal weight
      base *= fJetToTauBiases[iproc];
    }
    //For W+jets (or all for non-top mode = 2), can compare the measure non-closure in data to the predicted non-closure in MC
    if(test && ((process == JetToTauComposition::kWJets && fWJetsNCSysMode) || (fWJetsNCSysMode == 2 && process == JetToTauComposition::kQCD))) {
      const float data_corr = fJetToTauCorrs  [process];
      const float mc_corr   = fJetToTauMCCorrs[process];
      float ratio = (mc_corr > 0.) ? data_corr / mc_corr : data_corr; //default to correction size if MC isn't defined
      if(fVerbose > 5) {
        printf("  HistMaker::%s: Entry %lld has NC sys: NC(data) = %.3f, NC(MC) = %.3f, ratio = %.3f\n",
               __func__, fentry, data_corr, mc_corr, ratio);
      }
      if(!std::isfinite(ratio) || ratio <= 0.) {
        printf("  HistMaker::%s: Entry %lld has bad NC sys: NC(data) = %.3f, NC(MC) = %.3f, ratio = %.3f\n",
               __func__, fentry, data_corr, mc_corr, ratio);
        ratio = data_corr;
      }
      wt_alt += base * data_corr * ((up) ? ratio : (ratio > 0.) ? 1./ratio : 0.);
    } else { //take up as applying the correction twice, down as no correction
      if(test) wt_alt += base * ((up) ? fJetToTauCorrs[iproc]*fJetToTauCorrs[iproc] : 1.f);
      else     wt_alt += base * fJetToTauCorrs[iproc]; //apply the correction as normal for non-selected processes
    }
  }
  const float sys_wt = wt * (wt_alt / jetToTauWeightBias);
  return sys_wt;
}

//--------------------------------------------------------------------------------------------------------------
// Get the bias systematic variation on a j-->tau weight
float HistMaker::EvalJetToTauBiasSys(int process, bool up) {
  const float wt(eventWeight*genWeight); //nominal event weight

  //get the alternate j-->tau weight
  float wt_alt = 0.f;
  for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
    //Z+jets uses the W+jets scales
    bool test = iproc == process || (process == JetToTauComposition::kWJets && iproc == JetToTauComposition::kZJets);
    const float base = fJetToTauComps[iproc] * fJetToTauWts[iproc] * fJetToTauCorrs[iproc];
    if(fApplyJetToTauMCBias || (iproc != JetToTauComposition::kWJets && iproc != JetToTauComposition::kZJets)) { //bias is applied to nominal weight
      //take up as applying the bias correction twice, down as no correction
      wt_alt += base * ((test) ? (up) ? fJetToTauBiases[iproc]*fJetToTauBiases[iproc] : 1.f : fJetToTauBiases[iproc]);
    } else { //bias is not applied to the nominal weight
      //take up as applying the bias correction, down dividing by the correction
      wt_alt += base * ((test) ? (up) ? fJetToTauBiases[iproc] : ((fJetToTauBiases[iproc] > 0.f) ? 1.f/fJetToTauBiases[iproc] : 0.f) : 1.f);
    }
  }
  const float sys_wt = wt * (wt_alt / jetToTauWeightBias);
  return sys_wt;
}

//--------------------------------------------------------------------------------------------------------------
// Get the sum in quadrature of the event weight uncertainties
std::pair<float,float> HistMaker::OverallSystematicWeights() {
  const float wt(eventWeight*genWeight); //nominal event weight
  std::vector<float> up_wts, down_wts; //list of systematic weights
  if(!std::isfinite(wt)) return std::pair<float,float>(wt,wt);

  if(fSelection.EndsWith("tau")) {
    if(isLooseTau) { //add j-->tau weight uncertainties
      //j-->tau bias uncertainties
      for(int iproc = 0; iproc < JetToTauComposition::kLast; ++iproc) {
        if(iproc == JetToTauComposition::kZJets) continue; //combined with WJets
        up_wts  .push_back(EvalJetToTauNCSys  (iproc, true ));
        down_wts.push_back(EvalJetToTauNCSys  (iproc, false));
        up_wts  .push_back(EvalJetToTauBiasSys(iproc, true ));
        down_wts.push_back(EvalJetToTauBiasSys(iproc, false));
        //add each j-->tau fit stat uncertainty (3 per DM, 4 DMs considered
        for(int idm = 0; idm < 3; ++idm) {
          for(int ialt = 0; ialt < 2; ++ialt) {
            up_wts  .push_back(EvalJetToTauStatSys(iproc, idm, ialt, true ));
            down_wts.push_back(EvalJetToTauStatSys(iproc, idm, ialt, false));
          }
        }
      }
    }
  }

  if(fVerbose > 9) {
    printf("HistMaker::%s: Systematic variation lists:\n up   = {", __func__);
    for(float up_wt : up_wts) printf(" %.4f,", up_wt);
    printf("}\n down = {");
    for(float down_wt : down_wts) printf(" %.4f,", down_wt);
    printf("}\n");
  }
  auto sys_wts = Utilities::CombinedUncertainty(genWeight*eventWeight, up_wts, down_wts);
  if(fVerbose > 9) printf(" --> totals: nominal = %.4f, up = %.4f, down = %.4f\n", wt, sys_wts.first, sys_wts.second);
  return sys_wts;
}

//--------------------------------------------------------------------------------------------------------------
//determine MVA category
int HistMaker::Category(TString selection) {
  int category = 0;
  //get MVA output index by selection
  int mva_i = fMVAConfig->GetIndexBySelection(selection);
  if(mva_i < 0) return category;
  std::vector<double> mvaCuts = fMVAConfig->categories_[selection];
  const float score = fMvaUse[mva_i];
  for(unsigned index = 0; index < mvaCuts.size(); ++index) {
    if(score >= mvaCuts[index]) ++category; //passes category
    else break; //fails
  }
  return category;
}

//--------------------------------------------------------------------------------------------------------------
//initialize the randomly assigned systematic shifts
void HistMaker::InitializeSystematics() {
  leptonOne.wt1_group = 0; leptonOne.wt2_group = 0;
  leptonTwo.wt1_group = 0; leptonTwo.wt2_group = 0;
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
