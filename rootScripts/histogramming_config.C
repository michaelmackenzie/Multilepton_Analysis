#ifndef __HISTOGRAMMING_CONFIG__
#define __HISTOGRAMMING_CONFIG__

//Define which histogrammer to use
// typedef HistMaker HISTOGRAMMER;
typedef CLFVHistMaker HISTOGRAMMER;
// typedef JTTHistMaker HISTOGRAMMER;
// typedef QCDHistMaker HISTOGRAMMER;
// typedef ZPtHistMaker HISTOGRAMMER;
// typedef METHistMaker HISTOGRAMMER;
// typedef SparseHistMaker HISTOGRAMMER;
// typedef EmbedBDTHistMaker HISTOGRAMMER;
// typedef SparseTreeMaker HISTOGRAMMER;

//Process multiple cards in parallel
bool newProcess_   = true; //run card processing in a new process to avoid memory issues
int  maxProcesses_ = 6; //maximum number of new processes to run at once
bool splitSelecs_  = true; //split each selection processing into a new job
bool preFetch_     = false; //check for the sample on /tmp, and copy it there if it isn't there
bool useTChain_    = true; //use a TChain of input files rather than a tree from a single ntuple file
Long64_t notify_   = 50000; //frequency at which to printout processing info

vector<TString> tag_      = {}; //dataset tag requirement
vector<TString> veto_     = {"MuonEGRun", "Embed-EE", "Embed-MuMu"}; //dataset tags to veto
bool debug_               = false;
Long64_t startEvent_      =  0;
Long64_t nEvents_         =  1; //at 20, verbosity returns to normal
int verbose_              = -1; //override verbosity in debug mode to higher levels if > 1
int follow_hist_set_      = -1; //print info whenever this hist set is filled
HISTOGRAMMER* selector_   = nullptr;

Long64_t max_sim_events_  = -1; //5e5; //maximum number of events to skim in simulation, -1 to ignore
Double_t max_data_events_ = -1; //0.2 //maximum fraction of events to skim in data, -1 to ignore
Int_t    only_dy_loop_    = -1; //only Drell-Yan processing loop to process (-1 to process all)

bool DYFakeTau_           = false; //speed up dy fake tau scale factor
bool WJFakeTau_           = false; //speed up w+jets fake tau scale factor
bool TTFakeTau_           = false; //speed up ttbar fake tau scale factor
bool QCDFakeTau_          = false; //speed up qcd fake tau scale factor
bool JetTauTesting_       = false; //perform MC closure test
bool FakeLeptonTesting_   = false; //test MC fake leptons
bool CutFlowTesting_      = false; //test just basic cutflow sets
bool TriggerTesting_      = false; //make a few extra selections for ee/mumu/emu trigger testing
int  TopTesting_          =  0;    //>0: fill an inverted b-tag set; 2: invert the b-tag for final selections
int  doSSSF_              =  1;    //do same-sign, same flavor processingg
TString jTTProcess_       = "";    //JTTHistMaker process to measure ("" for all processes)

int  doEleIDStudy_        =  0; //add additional electron cuts in Z->emu to reduce Z->mumu*
int  doLepESHists_        =  0; //add histograms of BDT variables with lepton ES shifted up/down

int removeTrigWeights_    =  4; //0: do nothing 1: remove weights 2: replace 3: replace P(event) 4: replace P(at least 1 triggered)
int useBTagWeights_       =  1; //1: calculate locally; 2: use ntuple values for each jet
int useJetPUIDWeights_    =  2; //1: calculate locally; 2: use ntuple definition
int usePrefireWeights_    =  1; //0: remove weights; 1: use pre-defined weights; 2: re-define the weights
int removePUWeights_      =  2; //Signal only: 0: use ntuple definition; 1: don't apply PU weights; 2: replace PU weights
int useLeptonIDWeights_   =  1; //use e/mu ID weights: 0 = none, 1 = local, 2 = ntuple-level
int useQCDWeights_        =  1; //use QCD SS --> OS transfer weights
int useZPtWeights_        =  1; //MC --> Data Z pT matching: 1: calculate locally; 2: use ntuple-level weights
int useSignalZWeights_    =  1; //match the signal Z to the Drell-Yan Z MC
int useSignalZMixWeights_ =  1; //account for the Z/gamma* mixing in the signal generation
int useLepDxyzWeights_    =  1; //use dxy/dz displacement weights (emu/mumu/ee only)
int removeEventWeights_   =  0; //remove most (1) or all (2) event weights

int updateMCEra_          =  1; //0: do nothing 1: throw random number for MC era (data/embedding not random)
int updateMET_            =  1; //Update MET with lepton energy scale changes
int useEventFlags_        =  1; //apply event flags/filtering
int useMCFakeLep_         =  0; //use MC estimated fake light leptons
int useMCFakeTau_         =  0; //0: no MC j-->tau; 1: MC j-->tau, QCD j-->tau weights; 2: MC j-->tau, no j-->tau weights
int useJetToTauComp_      =  1; //use the composition of the anti-iso region to combine j->tau weights
int applyJetToTauMCBias_  =  1; //apply the W+jets/Z+jets MC bias estimate or only use as a uncertainty
int wJetsBiasMode_        =  8; //W+jets/Z+jets MC bias mode (1: lepm shape+rate; 6: lepm shape; 7: (lepm, bdt) shape+rate; 8: (lepm, bdt) shape)
int wJetsNCSysMode_       =  2; //W+jets/Z+jets non-closure uncertainty mode: 0: size of correction; 1: ratio of MC/Data correction (W+jets); 2: ratio (W+jets, QCD)
int jetToTauYearMode_     =  2; //0: use by-year scales; 1: use Run-2 scales; 2: use by-year scales + Run 2 corrections
int useWJetsJetToTau_     =  0; //Use W+jets j-->tau factors instead of composition
int doTriggerMatching_    =  1; //do trigger object matching
int useEMuTrigger_        =  0; //use the e-mu trigger in e-mu data: 1 = consider it; 2 = only consider it
int useRoccoCorr_         =  1; //Muon Rochester corrections 0: don't apply; 1: apply local corrections; 2: use ntuple-level corrections
int useRoccoSize_         =  1; //Use size of the Rochester corrections as the uncertainty
int etauAntiEleCut_       = -1; //cut on tau anti-ele ID for etau, -1 to use default
int useLooseTauBuffer_    =  0; //add a buffer between loose and tight tau events
int removeLooseSS_        =  0; //remove loose ID same-sign events

int useEmbedCuts_         =  2; //use kinematic cuts based on embedded generation: 1 = tau tau only; 2 = tau tau, mumu, ee, and emu; 3: do gen rejection in ee/mumu/emu as well
int embeddedTesting_      =  0; //test embedding options: 3 = use KIT measured scales
int useEmbedRocco_        =  1; //0: no correction and LFV Higgs AN muon sys; 1: use Rochester correction and sys; 2: use correction but separate uncertainty
int useEmbMuonES_         =  0; //apply Embed --> MC muon energy scales
int embedUseMETUnc_       =  0; //use MET uncertainties in embedding: 1: use JER/JES; 2: use approximate errors on (MET - nu pT)
int embedUseMCTauID_      =  1; //1: use MC Tau ID scales (but not energy scales) in Embedding; 2: use MC Tau ID and energy scales in Embedding
int useEmbedZMatching_    =  1; //1: match gen-Z rate MC to embed rate; 2: match using reco (only if floating the embedding)
int useEmbedBDTUnc_       =  0; //use gen-level BDT score uncertainty in embedding

int doEmuDefaults_        =  1; //set to default emu running
int doSameFlavorEMu_      =  0; //treat ee/mumu as emu
int doEmbedSameFlavor_    =  2; //setup ee/mumu for embedding testing: 1: skip MC Z->ll; 2: allow MC Z->ll
int doEmbedLLAll_         =  0; //process Embedding ee/mumu samples in non-ee/mumu selections

int systematicSeed_       = 90; //seed for systematic random shifts
int doSystematics_        =  1; //process systematic uncertainty histograms: 0: don't process; 1: process; -2: process for signal, nominal only for the rest
int allowMigration_       =  2; //event migration systematic effects
int  DoMVASets_           =  1; //Fill sets with MVA cuts: 1 = emu; 2 = emu/ee/mumu; 3 = all sets
int  ReprocessMVAs_       =  1; //Re-evaluate MVA scores on the fly
int useCDFBDTs_           =  2; //Use CDF transformed BDTs instead of the raw BDT scores in the fits
int useXGBoost_           =  1; //>0: use XGBoost BDT in Z->e+mu; >9: use XGBoost BDT in all categories
int useBDTScale_          =  1; //use BDT score corrections in Z->e+mu
bool writeTrees_          = false;
int  train_mode_          =  2; //MVA training mode, how to define training fractions
TString test_mva_         = ""; //MVA to test, independent of selection

int  doHiggs_             =  0; //do higgs-related analysis
bool sparseHists_         = true; //only plot more basic histograms

int  splitWJets_          =  1; //split W+jets sample based on N(LHE jets)
int  splitWGamma_         =  1; //remove W+gamma events from W+jets samples
bool splitDY_             = true; //split z+jets sample based on gen-level lepton pair flavor

bool useUL_               = false; //Use UL files/cross sections

float min_lepm_           = -1.f;
float max_lepm_           = 200.f;
float migration_buffer_   = 2.f; //mass, met, and pT window to use for systematic migration

//information about the data file/data
struct datacard_t {
  bool process_;
  double xsec_;
  TString fname_;
  TString filepath_;
  int isData_;
  bool combine_;
  int category_;
  TH1* events_;
  int year_;
  double lum_;
  TString dataset_;
  datacard_t(bool process = 0,  double xsec = 1., TString fname = "",
             int isData = 0,  bool combine = false,
             TString filepath = "",
             int category = 0, TH1* events = 0,
             int year = 2016) :
    process_(process), xsec_(xsec), fname_(fname), combine_(combine),
    filepath_(filepath), isData_(isData), category_(category),
    events_(events), year_(year), lum_(0.), dataset_("") {}
};


//information about the selector configuration
struct config_t {
  Bool_t writeTrees_;
  vector<TString> selections_;
  float signalTrainFraction_;
  float backgroundTrainFraction_;
  config_t() : writeTrees_(true), selections_(),
               signalTrainFraction_(0.),
               backgroundTrainFraction_(0.){}
};

config_t get_config() {
  config_t config;

  config.writeTrees_ = writeTrees_;
  config.selections_ = {"mutau"}; //{"mutau", "etau", "emu", "mutau_e", "etau_mu", "ee", "mumu"};
  config.signalTrainFraction_ = 0.5;
  config.backgroundTrainFraction_ = 0.3;

  return config;
}

vector<datacard_t> get_data_cards(TString& nanoaod_path) {
  nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/";
  // nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles_dev/";
  // nanoaod_path = "root://eoscms.cern.ch//store/group/phys_smp/ZLFV/lfvanalysis_rootfiles/";
  // nanoaod_path = "/eos/uscms/store/user/mmackenz/lfvanalysis_rootfiles/";

  printf("\033[32m--- Using tree path: %s\033[0m\n", nanoaod_path.Data());

  //cross section handler
  CrossSections xs(useUL_);
  vector<datacard_t> nanocards;

  //////////////////
  // 2016 samples //
  //////////////////

  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-amc_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarToSemiLeptonic"     ), "LFVAnalysis_ttbarToSemiLeptonic_2016.root"     , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarlnu"                ), "LFVAnalysis_ttbarlnu_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarToHadronic"         ), "LFVAnalysis_ttbarToHadronic_2016.root"         , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleAntiToptW"         ), "LFVAnalysis_SingleAntiToptW_2016.root"         , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleToptW"             ), "LFVAnalysis_SingleToptW_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleAntiToptChannel"   ), "LFVAnalysis_SingleAntiToptChannel_2016.root"   , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleToptChannel"       ), "LFVAnalysis_SingleToptChannel_2016.root"       , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu_2016.root"                    , 0, true));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu-ext_2016.root"                , 0, true));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-1J"           , 2016), "LFVAnalysis_Wlnu-1J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-2J"           , 2016), "LFVAnalysis_Wlnu-2J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-3J"           , 2016), "LFVAnalysis_Wlnu-3J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-4J"           , 2016), "LFVAnalysis_Wlnu-4J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WGamma"                  ), "LFVAnalysis_WGamma_2016.root"                  , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKWminus"               ), "LFVAnalysis_EWKWminus_2016.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKWplus"                ), "LFVAnalysis_EWKWplus_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKZ-M50"                ), "LFVAnalysis_EWKZ-M50_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WW"                      ), "LFVAnalysis_WW_2016.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WWLNuQQ"                 ), "LFVAnalysis_WWLNuQQ_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WZ"                      ), "LFVAnalysis_WZ_2016.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZZ"                      ), "LFVAnalysis_ZZ_2016.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WWW"                     ), "LFVAnalysis_WWW_2016.root"                     , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ggFH-TauTau"             ), "LFVAnalysis_ggFH-TauTau_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("VBFH-TauTau"             ), "LFVAnalysis_VBFH-TauTau_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WminusH-TauTau"          ), "LFVAnalysis_WminusH-TauTau_2016.root"          , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WplusH-TauTau"           ), "LFVAnalysis_WplusH-TauTau_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZH-TauTau"               ), "LFVAnalysis_ZH-TauTau_2016.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ggFH-WW"                 ), "LFVAnalysis_ggFH-WW_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("VBFH-WW"                 ), "LFVAnalysis_VBFH-WW_2016.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-G"     , 2016), "LFVAnalysis_Embed-MuTau-G_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-H"     , 2016), "LFVAnalysis_Embed-MuTau-H_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-B"     , 2016), "LFVAnalysis_Embed-MuTau-B_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-C"     , 2016), "LFVAnalysis_Embed-MuTau-C_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-D"     , 2016), "LFVAnalysis_Embed-MuTau-D_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-E"     , 2016), "LFVAnalysis_Embed-MuTau-E_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-F"     , 2016), "LFVAnalysis_Embed-MuTau-F_2016.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-B"      , 2016), "LFVAnalysis_Embed-ETau-B_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-C"      , 2016), "LFVAnalysis_Embed-ETau-C_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-D"      , 2016), "LFVAnalysis_Embed-ETau-D_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-E"      , 2016), "LFVAnalysis_Embed-ETau-E_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-F"      , 2016), "LFVAnalysis_Embed-ETau-F_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-G"      , 2016), "LFVAnalysis_Embed-ETau-G_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-H"      , 2016), "LFVAnalysis_Embed-ETau-H_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-G"       , 2016), "LFVAnalysis_Embed-EMu-G_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-H"       , 2016), "LFVAnalysis_Embed-EMu-H_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"       , 2016), "LFVAnalysis_Embed-EMu-B_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"       , 2016), "LFVAnalysis_Embed-EMu-C_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"       , 2016), "LFVAnalysis_Embed-EMu-D_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-E"       , 2016), "LFVAnalysis_Embed-EMu-E_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-F"       , 2016), "LFVAnalysis_Embed-EMu-F_2016.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-B"      , 2016), "LFVAnalysis_Embed-MuMu-B_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-C"      , 2016), "LFVAnalysis_Embed-MuMu-C_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-D"      , 2016), "LFVAnalysis_Embed-MuMu-D_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-E"      , 2016), "LFVAnalysis_Embed-MuMu-E_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-F"      , 2016), "LFVAnalysis_Embed-MuMu-F_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-G"      , 2016), "LFVAnalysis_Embed-MuMu-G_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-H"      , 2016), "LFVAnalysis_Embed-MuMu-H_2016.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-B"        , 2016), "LFVAnalysis_Embed-EE-B_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-C"        , 2016), "LFVAnalysis_Embed-EE-C_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-D"        , 2016), "LFVAnalysis_Embed-EE-D_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-E"        , 2016), "LFVAnalysis_Embed-EE-E_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-F"        , 2016), "LFVAnalysis_Embed-EE-F_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-G"        , 2016), "LFVAnalysis_Embed-EE-G_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-H"        , 2016), "LFVAnalysis_Embed-EE-H_2016.root"              , 0));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016B_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016C_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016D_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016E_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016F_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016G_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2016H_2016.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016B_2016.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016C_2016.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016D_2016.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016E_2016.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016F_2016.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016G_2016.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2016H_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016B_2016.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016C_2016.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016D_2016.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016E_2016.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016F_2016.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016G_2016.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2016H_2016.root"          , 3));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZETau"                   ), "LFVAnalysis_ZETau-v*_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZMuTau"                  ), "LFVAnalysis_ZMuTau-v*_2016.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZEMu"                    ), "LFVAnalysis_ZEMu-v*_2016.root"                 , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HETau"                   ), "LFVAnalysis_HETau_2016.root"                   , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HMuTau"                  ), "LFVAnalysis_HMuTau_2016.root"                  , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HEMu"                    ), "LFVAnalysis_HEMu_2016.root"                    , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich20to30"       ), "LFVAnalysis_QCDEMEnrich20to30_2016.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich30to50"       ), "LFVAnalysis_QCDEMEnrich30to50_2016.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich50to80"       ), "LFVAnalysis_QCDEMEnrich50to80_2016.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich80to120"      ), "LFVAnalysis_QCDEMEnrich80to120_2016.root"      , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich120to170"     ), "LFVAnalysis_QCDEMEnrich120to170_2016.root"     , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich170to300"     ), "LFVAnalysis_QCDEMEnrich170to300_2016.root"     , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich300toInf"     ), "LFVAnalysis_QCDEMEnrich300toInf_2016.root"     , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "LFVAnalysis_QCDDoubleEMEnrich30to40_2016.root" , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "LFVAnalysis_QCDDoubleEMEnrich30toInf_2016.root", 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "LFVAnalysis_QCDDoubleEMEnrich40toInf_2016.root", 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50_2016.root"                    , 0)); //LO Drell-Yan
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-ext_2016.root"                , 0));

  //////////////////
  // 2017 samples //
  //////////////////
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2017.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50_2017.root"                    , 0, true));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-ext_2017.root"                , 0, true));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarToSemiLeptonic"     ), "LFVAnalysis_ttbarToSemiLeptonic_2017.root"     , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarToHadronic"         ), "LFVAnalysis_ttbarToHadronic_2017.root"         , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarlnu"                ), "LFVAnalysis_ttbarlnu_2017.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleAntiToptW"         ), "LFVAnalysis_SingleAntiToptW_2017.root"         , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleToptW"             ), "LFVAnalysis_SingleToptW_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleToptChannel"       ), "LFVAnalysis_SingleToptChannel_2017.root"       , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleAntiToptChannel"   ), "LFVAnalysis_SingleAntiToptChannel_2017.root"   , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu_2017.root"                    , 0, true));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu-ext_2017.root"                , 0, true));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-1J"           , 2017), "LFVAnalysis_Wlnu-1J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-2J"           , 2017), "LFVAnalysis_Wlnu-2J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-3J"           , 2017), "LFVAnalysis_Wlnu-3J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-4J"           , 2017), "LFVAnalysis_Wlnu-4J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WGamma"                  ), "LFVAnalysis_WGamma_2017.root"                  , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKWminus"               ), "LFVAnalysis_EWKWminus_2017.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKWplus"                ), "LFVAnalysis_EWKWplus_2017.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKZ-M50"                ), "LFVAnalysis_EWKZ-M50_2017.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WW"                      ), "LFVAnalysis_WW_2017.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WWLNuQQ"                 ), "LFVAnalysis_WWLNuQQ_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WZ"                      ), "LFVAnalysis_WZ_2017.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZZ"                      ), "LFVAnalysis_ZZ_2017.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WWW"                     ), "LFVAnalysis_WWW_2017.root"                     , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ggFH-TauTau"             ), "LFVAnalysis_ggFH-TauTau_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("VBFH-TauTau"             ), "LFVAnalysis_VBFH-TauTau_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WminusH-TauTau"          ), "LFVAnalysis_WminusH-TauTau_2017.root"          , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WplusH-TauTau"           ), "LFVAnalysis_WplusH-TauTau_2017.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZH-TauTau"               ), "LFVAnalysis_ZH-TauTau_2017.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ggFH-WW"                 ), "LFVAnalysis_ggFH-WW_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("VBFH-WW"                 ), "LFVAnalysis_VBFH-WW_2017.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-B"     , 2017), "LFVAnalysis_Embed-MuTau-B_2017.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-C"     , 2017), "LFVAnalysis_Embed-MuTau-C_2017.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-D"     , 2017), "LFVAnalysis_Embed-MuTau-D_2017.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-E"     , 2017), "LFVAnalysis_Embed-MuTau-E_2017.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-F"     , 2017), "LFVAnalysis_Embed-MuTau-F_2017.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-B"      , 2017), "LFVAnalysis_Embed-ETau-B_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-C"      , 2017), "LFVAnalysis_Embed-ETau-C_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-D"      , 2017), "LFVAnalysis_Embed-ETau-D_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-E"      , 2017), "LFVAnalysis_Embed-ETau-E_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-F"      , 2017), "LFVAnalysis_Embed-ETau-F_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"       , 2017), "LFVAnalysis_Embed-EMu-B_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"       , 2017), "LFVAnalysis_Embed-EMu-C_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"       , 2017), "LFVAnalysis_Embed-EMu-D_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-E"       , 2017), "LFVAnalysis_Embed-EMu-E_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-F"       , 2017), "LFVAnalysis_Embed-EMu-F_2017.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-B"      , 2017), "LFVAnalysis_Embed-MuMu-B_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-C"      , 2017), "LFVAnalysis_Embed-MuMu-C_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-D"      , 2017), "LFVAnalysis_Embed-MuMu-D_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-E"      , 2017), "LFVAnalysis_Embed-MuMu-E_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-F"      , 2017), "LFVAnalysis_Embed-MuMu-F_2017.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-B"        , 2017), "LFVAnalysis_Embed-EE-B_2017.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-C"        , 2017), "LFVAnalysis_Embed-EE-C_2017.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-D"        , 2017), "LFVAnalysis_Embed-EE-D_2017.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-E"        , 2017), "LFVAnalysis_Embed-EE-E_2017.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-F"        , 2017), "LFVAnalysis_Embed-EE-F_2017.root"              , 0));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2017B_2017.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2017C_2017.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2017D_2017.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2017E_2017.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2017F_2017.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2017B_2017.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2017C_2017.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2017D_2017.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2017E_2017.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2017F_2017.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2017B_2017.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2017C_2017.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2017D_2017.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2017E_2017.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2017F_2017.root"          , 3));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZETau"                   ), "LFVAnalysis_ZETau-v3*_2017.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZMuTau"                  ), "LFVAnalysis_ZMuTau-v3*_2017.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZEMu"                    ), "LFVAnalysis_ZEMu-v3*_2017.root"                 , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HETau"                   ), "LFVAnalysis_HETau_2017.root"                   , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HMuTau"                  ), "LFVAnalysis_HMuTau_2017.root"                  , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HEMu"                    ), "LFVAnalysis_HEMu_2017.root"                    , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "LFVAnalysis_QCDDoubleEMEnrich30to40_2017.root" , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "LFVAnalysis_QCDDoubleEMEnrich30toInf_2017.root", 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "LFVAnalysis_QCDDoubleEMEnrich40toInf_2017.root", 0));

  //////////////////
  // 2018 samples //
  //////////////////
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2018.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-amc_2018.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarToSemiLeptonic"     ), "LFVAnalysis_ttbarToSemiLeptonic_2018.root"     , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarToHadronic"         ), "LFVAnalysis_ttbarToHadronic_2018.root"         , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ttbarlnu"                ), "LFVAnalysis_ttbarlnu_2018.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleAntiToptW"         ), "LFVAnalysis_SingleAntiToptW_2018.root"         , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleToptW"             ), "LFVAnalysis_SingleToptW_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleToptChannel"       ), "LFVAnalysis_SingleToptChannel_2018.root"       , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("SingleAntiToptChannel"   ), "LFVAnalysis_SingleAntiToptChannel_2018.root"   , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu_2018.root"                    , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-1J"           , 2018), "LFVAnalysis_Wlnu-1J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-2J"           , 2018), "LFVAnalysis_Wlnu-2J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-3J"           , 2018), "LFVAnalysis_Wlnu-3J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Wlnu-4J"           , 2018), "LFVAnalysis_Wlnu-4J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WGamma"                  ), "LFVAnalysis_WGamma_2018.root"                  , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKWminus"               ), "LFVAnalysis_EWKWminus_2018.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKWplus"                ), "LFVAnalysis_EWKWplus_2018.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("EWKZ-M50"                ), "LFVAnalysis_EWKZ-M50_2018.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WW"                      ), "LFVAnalysis_WW_2018.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WWLNuQQ"                 ), "LFVAnalysis_WWLNuQQ_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WZ"                      ), "LFVAnalysis_WZ_2018.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZZ"                      ), "LFVAnalysis_ZZ_2018.root"                      , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WWW"                     ), "LFVAnalysis_WWW_2018.root"                     , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ggFH-TauTau"             ), "LFVAnalysis_ggFH-TauTau_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("VBFH-TauTau"             ), "LFVAnalysis_VBFH-TauTau_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WminusH-TauTau"          ), "LFVAnalysis_WminusH-TauTau_2018.root"          , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("WplusH-TauTau"           ), "LFVAnalysis_WplusH-TauTau_2018.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZH-TauTau"               ), "LFVAnalysis_ZH-TauTau_2018.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ggFH-WW"                 ), "LFVAnalysis_ggFH-WW_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("VBFH-WW"                 ), "LFVAnalysis_VBFH-WW_2018.root"                 , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-D"     , 2018), "LFVAnalysis_Embed-MuTau-D_2018.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-A"     , 2018), "LFVAnalysis_Embed-MuTau-A_2018.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-B"     , 2018), "LFVAnalysis_Embed-MuTau-B_2018.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-C"     , 2018), "LFVAnalysis_Embed-MuTau-C_2018.root"           , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-D"      , 2018), "LFVAnalysis_Embed-ETau-D_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-A"      , 2018), "LFVAnalysis_Embed-ETau-A_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-B"      , 2018), "LFVAnalysis_Embed-ETau-B_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-C"      , 2018), "LFVAnalysis_Embed-ETau-C_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"       , 2018), "LFVAnalysis_Embed-EMu-D_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-A"       , 2018), "LFVAnalysis_Embed-EMu-A_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"       , 2018), "LFVAnalysis_Embed-EMu-B_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"       , 2018), "LFVAnalysis_Embed-EMu-C_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-A"        , 2018), "LFVAnalysis_Embed-EE-A_2018.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-B"        , 2018), "LFVAnalysis_Embed-EE-B_2018.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-C"        , 2018), "LFVAnalysis_Embed-EE-C_2018.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EE-D"        , 2018), "LFVAnalysis_Embed-EE-D_2018.root"              , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-A"      , 2018), "LFVAnalysis_Embed-MuMu-A_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-B"      , 2018), "LFVAnalysis_Embed-MuMu-B_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-C"      , 2018), "LFVAnalysis_Embed-MuMu-C_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuMu-D"      , 2018), "LFVAnalysis_Embed-MuMu-D_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2018D_2018.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2018A_2018.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2018B_2018.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMuonRun2018C_2018.root"      , 2));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2018D_2018.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2018A_2018.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2018B_2018.root"  , 1));
  nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleElectronRun2018C_2018.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2018D_2018.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2018A_2018.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2018B_2018.root"          , 3));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_MuonEGRun2018C_2018.root"          , 3));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZETau"                   ), "LFVAnalysis_ZETau-v*_2018.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZMuTau"                  ), "LFVAnalysis_ZMuTau-v*_2018.root"               , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("ZEMu"                    ), "LFVAnalysis_ZEMu-v*_2018.root"                 , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HETau"                   ), "LFVAnalysis_HETau_2018.root"                   , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HMuTau"                  ), "LFVAnalysis_HMuTau_2018.root"                  , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("HEMu"                    ), "LFVAnalysis_HEMu_2018.root"                    , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich15to20"       ), "LFVAnalysis_QCDEMEnrich15to20_2018.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich20to30"       ), "LFVAnalysis_QCDEMEnrich20to30_2018.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich30to50"       ), "LFVAnalysis_QCDEMEnrich30to50_2018.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich50to80"       ), "LFVAnalysis_QCDEMEnrich50to80_2018.root"       , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich80to120"      ), "LFVAnalysis_QCDEMEnrich80to120_2018.root"      , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich120to170"     ), "LFVAnalysis_QCDEMEnrich120to170_2018.root"     , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich170to300"     ), "LFVAnalysis_QCDEMEnrich170to300_2018.root"     , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich300toInf"     ), "LFVAnalysis_QCDEMEnrich300toInf_2018.root"     , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "LFVAnalysis_QCDDoubleEMEnrich30toInf_2018.root", 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "LFVAnalysis_QCDDoubleEMEnrich40toInf_2018.root", 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2018_QCDDoubleEMEnrich30to40.root" , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50.root"                    , 0)); //LO Drell-Yan

  return nanocards;
}


#endif
