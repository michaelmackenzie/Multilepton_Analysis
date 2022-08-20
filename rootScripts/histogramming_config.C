#ifndef __HISTOGRAMMING_CONFIG__
#define __HISTOGRAMMING_CONFIG__

//Define which histogrammer to use
// typedef HistMaker HISTOGRAMMER;
typedef CLFVHistMaker HISTOGRAMMER;
// typedef JTTHistMaker HISTOGRAMMER;

//Process multiple cards in parallel
bool newProcess_   = true; //run card processing in a new process to avoid memory issues
int  maxProcesses_ = 4; //maximum number of new processes to run at once

bool debug_ = false;
Long64_t startEvent_ = 0;
Long64_t nEvents_ = 1; //at 20, verbosity returns to normal
HISTOGRAMMER* selector_ = 0;

bool DYTesting_      = true; //speed up running for scale factor calculation/debugging
bool DYFakeTau_      = false; //speed up dy fake tau scale factor
bool WJFakeTau_      = true; //speed up w+jets fake tau scale factor
bool TTFakeTau_      = true; //speed up ttbar fake tau scale factor
bool QCDFakeTau_     = true; //speed up qcd fake tau scale factor
bool JetTauTesting_  = true; //perform MC closure test
bool CutFlowTesting_ = false; //test just basic cutflow sets
bool TriggerTesting_ = true; //make a few extra selections for ee/mumu/emu trigger testing

int removeTrigWeights_ = 4; //0: do nothing 1: remove weights 2: replace 3: replace P(event) 4: replace P(at least 1 triggered)
int updateMCEra_       = 1; //0: do nothing 1: throw random number for MC era (data/embedding not random)
int useBTagWeights_    = 1;
int removeZPtWeights_  = 1; //0;
int useJetPUIDWeights_ = 1; //1;
int usePrefireWeights_ = 1; //0: remove weights; 1: use pre-defined weights; 2: re-define the weights
int removePUWeights_   = 2; //Signal only: 0: do nothing; 1: don't apply PU weights; 2: replace PU weights
int useMCFakeLep_      = 0; //use MC estimated light leptons in ee, mumu, and emu categories
int useJetToTauComp_   = 1; //use the composition of the anti-iso region to combine j->tau weights
int useQCDWeights_     = 1; //use QCD SS --> OS transfer weights
int doTriggerMatching_ = 1; //do trigger object matching
int doSSSF_            = 1; //do same-sign, same flavor processing
int useSignalZWeights_ = 0; //match the signal Z to the Drell-Yan Z MC

int useEmbedCuts_      = 1; //use kinematic cuts based on embedded generation
int embeddedTesting_   = 0; //test embedding options: 3 = use KIT measured scales

int systematicSeed_    = 90; //seed for systematic random shifts
int doSystematics_     = 0;//1;
bool writeTrees_       = false; //true
int  DoMVASets_        = 0; //Fill sets with MVA cuts: 1 = emu; 2 = emu/ee/mumu; 3 = all sets
int  ReprocessMVAs_    = 0; //Re-evaluate MVA scores on the fly
int  doHiggs_          = 0; //do higgs-related analysis

int splitWJets_        = true; //split w+jets sample based on N(LHE jets)
bool splitDY_          = true; //split z+jets sample based on gen-level lepton pair flavor

bool useUL_            = false; //Use UL files/cross sections

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
  datacard_t(bool process, double xsec, TString fname,
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
  TString onlyChannel_;
  vector<TString> skipChannels_;
  float signalTrainFraction_;
  float backgroundTrainFraction_;
  Int_t useTauFakeSF_;
  Int_t doSystematics_;
  config_t() : writeTrees_(true), onlyChannel_(""), skipChannels_(),
               signalTrainFraction_(0.),
               backgroundTrainFraction_(0.), useTauFakeSF_(1),
               doSystematics_(0){}
};

config_t get_config() {
  config_t config;

  config.useTauFakeSF_ = 1; //1 = use given scale factors, 2 = override them with local ones
  config.writeTrees_ = writeTrees_;
  config.onlyChannel_ = "emu";
  config.skipChannels_ = {/*"mutau", "etau", "emu", "mumu", "ee"*/};
  config.signalTrainFraction_ = 0.3;
  config.backgroundTrainFraction_ = 0.3;
  config.doSystematics_ = doSystematics_;

  return config;
}

vector<datacard_t> get_data_cards(TString& nanoaod_path) {
  nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/";
  // nanoaod_path = "/eos/uscms/store/user/mmackenz/lfvanalysis_rootfiles/";

  printf("\033[32m--- Using tree path: %s\033[0m\n", nanoaod_path.Data());

  //cross section handler
  CrossSections xs(useUL_);
  vector<datacard_t> nanocards;

  //////////////////
  // 2016 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2016.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-amc_2016.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "LFVAnalysis_ttbarToSemiLeptonic_2016.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "LFVAnalysis_ttbarlnu_2016.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "LFVAnalysis_ttbarToHadronic_2016.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "LFVAnalysis_SingleAntiToptW_2016.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "LFVAnalysis_SingleToptW_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptChannel"   ), "LFVAnalysis_SingleAntiToptChannel_2016.root"   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptChannel"       ), "LFVAnalysis_SingleToptChannel_2016.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu_2016.root"                    , 0, useUL_));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu-ext_2016.root"                , 0, useUL_));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"           , 2016), "LFVAnalysis_Wlnu-1J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"           , 2016), "LFVAnalysis_Wlnu-2J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"           , 2016), "LFVAnalysis_Wlnu-3J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"           , 2016), "LFVAnalysis_Wlnu-4J_2016.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WGamma"                  ), "LFVAnalysis_WGamma_2016.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWminus"               ), "LFVAnalysis_EWKWminus_2016.root"               , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWplus"                ), "LFVAnalysis_EWKWplus_2016.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKZ-M50"                ), "LFVAnalysis_EWKZ-M50_2016.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "LFVAnalysis_WW_2016.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWLNuQQ"                 ), "LFVAnalysis_WWLNuQQ_2016.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "LFVAnalysis_WZ_2016.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "LFVAnalysis_ZZ_2016.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "LFVAnalysis_WWW_2016.root"                     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich20to30"       ), "LFVAnalysis_QCDEMEnrich20to30_2016.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich30to50"       ), "LFVAnalysis_QCDEMEnrich30to50_2016.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich50to80"       ), "LFVAnalysis_QCDEMEnrich50to80_2016.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich80to120"      ), "LFVAnalysis_QCDEMEnrich80to120_2016.root"      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich120to170"     ), "LFVAnalysis_QCDEMEnrich120to170_2016.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich170to300"     ), "LFVAnalysis_QCDEMEnrich170to300_2016.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich300toInf"     ), "LFVAnalysis_QCDEMEnrich300toInf_2016.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "LFVAnalysis_QCDDoubleEMEnrich30to40_2016.root" , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "LFVAnalysis_QCDDoubleEMEnrich30toInf_2016.root", 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "LFVAnalysis_QCDDoubleEMEnrich40toInf_2016.root", 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-B"     , 2016), "LFVAnalysis_Embed-MuTau-B_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-C"     , 2016), "LFVAnalysis_Embed-MuTau-C_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-D"     , 2016), "LFVAnalysis_Embed-MuTau-D_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-E"     , 2016), "LFVAnalysis_Embed-MuTau-E_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-F"     , 2016), "LFVAnalysis_Embed-MuTau-F_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-G"     , 2016), "LFVAnalysis_Embed-MuTau-G_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-H"     , 2016), "LFVAnalysis_Embed-MuTau-H_2016.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-B"      , 2016), "LFVAnalysis_Embed-ETau-B_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-C"      , 2016), "LFVAnalysis_Embed-ETau-C_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-D"      , 2016), "LFVAnalysis_Embed-ETau-D_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-E"      , 2016), "LFVAnalysis_Embed-ETau-E_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-F"      , 2016), "LFVAnalysis_Embed-ETau-F_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-G"      , 2016), "LFVAnalysis_Embed-ETau-G_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-H"      , 2016), "LFVAnalysis_Embed-ETau-H_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-B"       , 2016), "LFVAnalysis_Embed-EMu-B_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-C"       , 2016), "LFVAnalysis_Embed-EMu-C_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-D"       , 2016), "LFVAnalysis_Embed-EMu-D_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-E"       , 2016), "LFVAnalysis_Embed-EMu-E_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-F"       , 2016), "LFVAnalysis_Embed-EMu-F_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-G"       , 2016), "LFVAnalysis_Embed-EMu-G_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-H"       , 2016), "LFVAnalysis_Embed-EMu-H_2016.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-B"      , 2016), "LFVAnalysis_Embed-MuMu-B_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-C"      , 2016), "LFVAnalysis_Embed-MuMu-C_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-D"      , 2016), "LFVAnalysis_Embed-MuMu-D_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-E"      , 2016), "LFVAnalysis_Embed-MuMu-E_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-F"      , 2016), "LFVAnalysis_Embed-MuMu-F_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-G"      , 2016), "LFVAnalysis_Embed-MuMu-G_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-H"      , 2016), "LFVAnalysis_Embed-MuMu-H_2016.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-B"        , 2016), "LFVAnalysis_Embed-EE-B_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-C"        , 2016), "LFVAnalysis_Embed-EE-C_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-D"        , 2016), "LFVAnalysis_Embed-EE-D_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-E"        , 2016), "LFVAnalysis_Embed-EE-E_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-F"        , 2016), "LFVAnalysis_Embed-EE-F_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-G"        , 2016), "LFVAnalysis_Embed-EE-G_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-H"        , 2016), "LFVAnalysis_Embed-EE-H_2016.root"              , 0));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016B_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016C_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016D_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016E_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016F_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016G_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2016H_2016.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016B_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016C_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016D_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016E_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016F_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016G_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2016H_2016.root"  , 1));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "LFVAnalysis_ZETau_2016.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "LFVAnalysis_ZMuTau_2016.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "LFVAnalysis_ZEMu_2016.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "LFVAnalysis_HETau_2016.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "LFVAnalysis_HMuTau_2016.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "LFVAnalysis_HEMu_2016.root"                    , 0));
  // nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleMu_2016.root"                , 2));
  // nanocards.push_back(datacard_t(true , 1.                                            , "LFVAnalysis_SingleEle_2016.root"               , 1));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50_2016.root"                    , 0, useUL_));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-ext_2016.root"                , 0, useUL_));

  //////////////////
  // 2017 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2017.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50_2017.root"                    , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-ext_2017.root"                , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "LFVAnalysis_ttbarToSemiLeptonic_2017.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "LFVAnalysis_ttbarToHadronic_2017.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "LFVAnalysis_ttbarlnu_2017.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "LFVAnalysis_SingleAntiToptW_2017.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "LFVAnalysis_SingleToptW_2017.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptChannel"       ), "LFVAnalysis_SingleToptChannel_2017.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptChannel"   ), "LFVAnalysis_SingleAntiToptChannel_2017.root"   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu_2017.root"                    , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu-ext_2017.root"                , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"           , 2017), "LFVAnalysis_Wlnu-1J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"           , 2017), "LFVAnalysis_Wlnu-2J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"           , 2017), "LFVAnalysis_Wlnu-3J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"           , 2017), "LFVAnalysis_Wlnu-4J_2017.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WGamma"                  ), "LFVAnalysis_WGamma_2017.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWminus"               ), "LFVAnalysis_EWKWminus_2017.root"               , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWplus"                ), "LFVAnalysis_EWKWplus_2017.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKZ-M50"                ), "LFVAnalysis_EWKZ-M50_2017.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "LFVAnalysis_WW_2017.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "LFVAnalysis_WZ_2017.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "LFVAnalysis_ZZ_2017.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "LFVAnalysis_WWW_2017.root"                     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-B"     , 2017), "LFVAnalysis_Embed-MuTau-B_2017.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-C"     , 2017), "LFVAnalysis_Embed-MuTau-C_2017.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-D"     , 2017), "LFVAnalysis_Embed-MuTau-D_2017.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-E"     , 2017), "LFVAnalysis_Embed-MuTau-E_2017.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-F"     , 2017), "LFVAnalysis_Embed-MuTau-F_2017.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-B"      , 2017), "LFVAnalysis_Embed-ETau-B_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-C"      , 2017), "LFVAnalysis_Embed-ETau-C_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-D"      , 2017), "LFVAnalysis_Embed-ETau-D_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-E"      , 2017), "LFVAnalysis_Embed-ETau-E_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-F"      , 2017), "LFVAnalysis_Embed-ETau-F_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-B"       , 2017), "LFVAnalysis_Embed-EMu-B_2017.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-C"       , 2017), "LFVAnalysis_Embed-EMu-C_2017.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-D"       , 2017), "LFVAnalysis_Embed-EMu-D_2017.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-E"       , 2017), "LFVAnalysis_Embed-EMu-E_2017.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-F"       , 2017), "LFVAnalysis_Embed-EMu-F_2017.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-B"        , 2017), "LFVAnalysis_Embed-EE-B_2017.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-C"        , 2017), "LFVAnalysis_Embed-EE-C_2017.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-D"        , 2017), "LFVAnalysis_Embed-EE-D_2017.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-E"        , 2017), "LFVAnalysis_Embed-EE-E_2017.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-F"        , 2017), "LFVAnalysis_Embed-EE-F_2017.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-B"      , 2017), "LFVAnalysis_Embed-MuMu-B_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-C"      , 2017), "LFVAnalysis_Embed-MuMu-C_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-D"      , 2017), "LFVAnalysis_Embed-MuMu-D_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-E"      , 2017), "LFVAnalysis_Embed-MuMu-E_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-F"      , 2017), "LFVAnalysis_Embed-MuMu-F_2017.root"            , 0));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2017B_2017.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2017C_2017.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2017D_2017.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2017E_2017.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2017F_2017.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2017B_2017.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2017C_2017.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2017D_2017.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2017E_2017.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2017F_2017.root"  , 1));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "LFVAnalysis_ZETau_2017.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "LFVAnalysis_ZMuTau_2017.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "LFVAnalysis_ZEMu_2017.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "LFVAnalysis_HETau_2017.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "LFVAnalysis_HMuTau_2017.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "LFVAnalysis_HEMu_2017.root"                    , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "LFVAnalysis_QCDDoubleEMEnrich30to40_2017.root" , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "LFVAnalysis_QCDDoubleEMEnrich30toInf_2017.root", 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "LFVAnalysis_QCDDoubleEMEnrich40toInf_2017.root", 0));

  //////////////////
  // 2018 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2018.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-amc_2018.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "LFVAnalysis_ttbarToSemiLeptonic_2018.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "LFVAnalysis_ttbarToHadronic_2018.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "LFVAnalysis_ttbarlnu_2018.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "LFVAnalysis_SingleAntiToptW_2018.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "LFVAnalysis_SingleToptW_2018.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptChannel"       ), "LFVAnalysis_SingleToptChannel_2018.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptChannel"   ), "LFVAnalysis_SingleAntiToptChannel_2018.root"   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "LFVAnalysis_Wlnu_2018.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"           , 2018), "LFVAnalysis_Wlnu-1J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"           , 2018), "LFVAnalysis_Wlnu-2J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"           , 2018), "LFVAnalysis_Wlnu-3J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"           , 2018), "LFVAnalysis_Wlnu-4J_2018.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WGamma"                  ), "LFVAnalysis_WGamma_2018.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWminus"               ), "LFVAnalysis_EWKWminus_2018.root"               , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWplus"                ), "LFVAnalysis_EWKWplus_2018.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKZ-M50"                ), "LFVAnalysis_EWKZ-M50_2018.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "LFVAnalysis_WW_2018.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWLNuQQ"                 ), "LFVAnalysis_WWLNuQQ_2018.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "LFVAnalysis_WZ_2018.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "LFVAnalysis_ZZ_2018.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "LFVAnalysis_WWW_2018.root"                     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich15to20"       ), "LFVAnalysis_QCDEMEnrich15to20_2018.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich20to30"       ), "LFVAnalysis_QCDEMEnrich20to30_2018.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich30to50"       ), "LFVAnalysis_QCDEMEnrich30to50_2018.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich50to80"       ), "LFVAnalysis_QCDEMEnrich50to80_2018.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich80to120"      ), "LFVAnalysis_QCDEMEnrich80to120_2018.root"      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich120to170"     ), "LFVAnalysis_QCDEMEnrich120to170_2018.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich170to300"     ), "LFVAnalysis_QCDEMEnrich170to300_2018.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich300toInf"     ), "LFVAnalysis_QCDEMEnrich300toInf_2018.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "LFVAnalysis_QCDDoubleEMEnrich30toInf_2018.root", 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "LFVAnalysis_QCDDoubleEMEnrich40toInf_2018.root", 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-A"     , 2018), "LFVAnalysis_Embed-MuTau-A_2018.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-B"     , 2018), "LFVAnalysis_Embed-MuTau-B_2018.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-C"     , 2018), "LFVAnalysis_Embed-MuTau-C_2018.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-D"     , 2018), "LFVAnalysis_Embed-MuTau-D_2018.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-A"      , 2018), "LFVAnalysis_Embed-ETau-A_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-B"      , 2018), "LFVAnalysis_Embed-ETau-B_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-C"      , 2018), "LFVAnalysis_Embed-ETau-C_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-D"      , 2018), "LFVAnalysis_Embed-ETau-D_2018.root"            , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-A"       , 2018), "LFVAnalysis_Embed-EMu-A_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"       , 2018), "LFVAnalysis_Embed-EMu-B_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"       , 2018), "LFVAnalysis_Embed-EMu-C_2018.root"             , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"       , 2018), "LFVAnalysis_Embed-EMu-D_2018.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-A"        , 2018), "LFVAnalysis_Embed-EE-A_2018.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-B"        , 2018), "LFVAnalysis_Embed-EE-B_2018.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-C"        , 2018), "LFVAnalysis_Embed-EE-C_2018.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EE-D"        , 2018), "LFVAnalysis_Embed-EE-D_2018.root"              , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-A"      , 2018), "LFVAnalysis_Embed-MuMu-A_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-B"      , 2018), "LFVAnalysis_Embed-MuMu-B_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-C"      , 2018), "LFVAnalysis_Embed-MuMu-C_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuMu-D"      , 2018), "LFVAnalysis_Embed-MuMu-D_2018.root"            , 0));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2018A_2018.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2018B_2018.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2018C_2018.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleMuonRun2018D_2018.root"      , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2018A_2018.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2018B_2018.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2018C_2018.root"  , 1));
  nanocards.push_back(datacard_t(false, 1.                                            , "LFVAnalysis_SingleElectronRun2018D_2018.root"  , 1));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "LFVAnalysis_ZETau_2018.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "LFVAnalysis_ZMuTau_2018.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "LFVAnalysis_ZEMu_2018.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "LFVAnalysis_HETau_2018.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "LFVAnalysis_HMuTau_2018.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "LFVAnalysis_HEMu_2018.root"                    , 0));
  // nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleMu.root"                , 2));
  // nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleEle.root"               , 1));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2018_QCDDoubleEMEnrich30to40.root" , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50.root"                    , 0));

  return nanocards;
}


#endif
