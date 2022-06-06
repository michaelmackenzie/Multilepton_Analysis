#ifndef __HISTOGRAMMING_CONFIG__
#define __HISTOGRAMMING_CONFIG__

bool debug_ = false;
Long64_t startEvent_ = 429;
Long64_t nEvents_ = 1; //at 20, verbosity returns to normal
CLFVHistMaker* selector_ = 0;

bool DYTesting_      = true; //speed up running for scale factor calculation/debugging
bool DYFakeTau_      = false; //speed up dy fake tau scale factor
bool WJFakeTau_      = true; //speed up w+jets fake tau scale factor
bool TTFakeTau_      = true; //speed up ttbar fake tau scale factor
bool QCDFakeTau_     = true; //speed up qcd fake tau scale factor
bool JetTauTesting_  = true; //perform MC closure test
bool CutFlowTesting_ = false; //test just basic cutflow sets
bool TriggerTesting_ = true; //make a few extra selections for ee/mumu/emu trigger testing

int removeTrigWeights_ = 3; //0: do nothing 1: remove weights 2: replace 3: replace P(event) 4: replace P(at least 1 triggered)
int updateMCEra_       = 1; //0: do nothing 1: throw random number for MC era (data/embedding not random)
int useBTagWeights_    = 1;
int removeZPtWeights_  = 2; //2;
int useJetPUIDWeights_ = 1; //1;
int usePrefireWeights_ = 1;
int removePUWeights_   = 2; //0: do nothing; 1: don't apply PU weights; 2: replace PU weights
int useMCFakeLep_      = 0; //use MC estimated light leptons in ee, mumu, and emu categories
int useJetToTauComp_   = 1; //use the composition of the anti-iso region to combine j->tau weights
int useQCDWeights_     = 1; //use QCD SS --> OS transfer weights

int useEmbedCuts_      = 1; //use kinematic cuts based on embedded generation
int embeddedTesting_   = 0; //test embedding options: 3 = use KIT measured scales

int systematicSeed_    = 90; //seed for systematic random shifts
int doSystematics_     = 0;//1;
bool writeTrees_       = false; //true
int  DoMVASets_        = 0; //Fill sets with MVA cuts: 1 = emu; 2 = emu/ee/mumu; 3 = all sets

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
  Bool_t reProcessMVAs_;
  float signalTrainFraction_;
  float backgroundTrainFraction_;
  Int_t useTauFakeSF_;
  Int_t doSystematics_;
  config_t() : writeTrees_(true), onlyChannel_(""), skipChannels_(),
               reProcessMVAs_(false),
               signalTrainFraction_(0.),
               backgroundTrainFraction_(0.), useTauFakeSF_(1),
               doSystematics_(0){}
};

config_t get_config() {
  config_t config;

  config.useTauFakeSF_ = 1; //1 = use given scale factors, 2 = override them with local ones
  config.writeTrees_ = writeTrees_;
  config.onlyChannel_ = "";
  config.skipChannels_ = {/*"mutau", "etau", "emu",*/ "mumu", "ee", "all", "jets", "llg_study"};
  config.reProcessMVAs_ = false;
  config.signalTrainFraction_ = 0.3;
  config.backgroundTrainFraction_ = 0.3;
  config.doSystematics_ = doSystematics_;

  return config;
}

vector<datacard_t> get_data_cards(TString& nanoaod_path) {
  // nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/clfv_nanoaod_test_trees/";
  nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/";

  printf("\033[32m--- Using tree path: %s\033[0m\n", nanoaod_path.Data());

  //cross section handler
  CrossSections xs(useUL_);
  vector<datacard_t> nanocards;

  //////////////////
  // 2016 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY10to50"                ), "LFVAnalysis_DY10to50_2016.root"                , 0));
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("DY50"                    ), "LFVAnalysis_DY50-amc_2016.root"                , 0));
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
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWToLNuQQ"               ), "LFVAnalysis_WWToLNuQQ_2016.root"               , 0));
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
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY10to50"                ), "clfv_2017_DY10to50.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2017_DY50.root"                    , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2017_DY50-ext.root"                , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2017_ttbarToSemiLeptonic.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2017_ttbarToHadronic.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2017_ttbarlnu.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2017_SingleAntiToptW.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2017_SingleToptW.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptChannel"       ), "clfv_2017_SingleToptChannel.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptChannel"   ), "clfv_2017_SingleAntiToptChannel.root"   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2017_Wlnu.root"                    , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2017_Wlnu-ext.root"                , 0, true));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"           , 2017), "clfv_2017_Wlnu-1J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"           , 2017), "clfv_2017_Wlnu-2J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"           , 2017), "clfv_2017_Wlnu-3J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"           , 2017), "clfv_2017_Wlnu-4J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WGamma"                  ), "clfv_2017_WGamma.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWminus"               ), "clfv_2017_EWKWminus.root"               , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWplus"                ), "clfv_2017_EWKWplus.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKZ-M50"                ), "clfv_2017_EWKZ-M50.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2017_WW.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2017_WZ.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2017_ZZ.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2017_WWW.root"                     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-B"     , 2017), "clfv_2017_Embed-MuTau-B.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-C"     , 2017), "clfv_2017_Embed-MuTau-C.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-D"     , 2017), "clfv_2017_Embed-MuTau-D.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-E"     , 2017), "clfv_2017_Embed-MuTau-E.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-F"     , 2017), "clfv_2017_Embed-MuTau-F.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-B"      , 2017), "clfv_2017_Embed-ETau-B.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-C"      , 2017), "clfv_2017_Embed-ETau-C.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-D"      , 2017), "clfv_2017_Embed-ETau-D.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-E"      , 2017), "clfv_2017_Embed-ETau-E.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-F"      , 2017), "clfv_2017_Embed-ETau-F.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-B"       , 2017), "clfv_2017_Embed-EMu-B.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-C"       , 2017), "clfv_2017_Embed-EMu-C.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-D"       , 2017), "clfv_2017_Embed-EMu-D.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-E"       , 2017), "clfv_2017_Embed-EMu-E.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-F"       , 2017), "clfv_2017_Embed-EMu-F.root"             , 0));
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2017_SingleMu.root"                , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2017_SingleEle.root"               , 1));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2017_ZETau.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2017_ZMuTau.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2017_ZEMu.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2017_HETau.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2017_HMuTau.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2017_HEMu.root"                    , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2017_QCDDoubleEMEnrich30to40.root" , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2017_QCDDoubleEMEnrich30toInf.root", 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2017_QCDDoubleEMEnrich40toInf.root", 0));

  //////////////////
  // 2018 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY10to50"                ), "clfv_2018_DY10to50.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50-amc.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2018_ttbarToSemiLeptonic.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2018_ttbarToHadronic.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2018_ttbarlnu.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2018_SingleAntiToptW.root"         , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptChannel"       ), "clfv_2018_SingleToptChannel.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptChannel"   ), "clfv_2018_SingleAntiToptChannel.root"   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2018_SingleToptW.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2018_Wlnu.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"           , 2018), "clfv_2018_Wlnu-1J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"           , 2018), "clfv_2018_Wlnu-2J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"           , 2018), "clfv_2018_Wlnu-3J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"           , 2018), "clfv_2018_Wlnu-4J.root"                 , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WGamma"                  ), "clfv_2018_WGamma.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWminus"               ), "clfv_2018_EWKWminus.root"               , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKWplus"                ), "clfv_2018_EWKWplus.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("EWKZ-M50"                ), "clfv_2018_EWKZ-M50.root"                , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2018_WW.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2018_WZ.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2018_ZZ.root"                      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2018_WWW.root"                     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich15to20"       ), "clfv_2018_QCDEMEnrich15to20.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich20to30"       ), "clfv_2018_QCDEMEnrich20to30.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich30to50"       ), "clfv_2018_QCDEMEnrich30to50.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich50to80"       ), "clfv_2018_QCDEMEnrich50to80.root"       , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich80to120"      ), "clfv_2018_QCDEMEnrich80to120.root"      , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich120to170"     ), "clfv_2018_QCDEMEnrich120to170.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich170to300"     ), "clfv_2018_QCDEMEnrich170to300.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDEMEnrich300toInf"     ), "clfv_2018_QCDEMEnrich300toInf.root"     , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2018_QCDDoubleEMEnrich30toInf.root", 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2018_QCDDoubleEMEnrich40toInf.root", 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-A"     , 2018), "clfv_2018_Embed-MuTau-A.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-B"     , 2018), "clfv_2018_Embed-MuTau-B.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-C"     , 2018), "clfv_2018_Embed-MuTau-C.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-D"     , 2018), "clfv_2018_Embed-MuTau-D.root"           , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-A"      , 2018), "clfv_2018_Embed-ETau-A.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-B"      , 2018), "clfv_2018_Embed-ETau-B.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-C"      , 2018), "clfv_2018_Embed-ETau-C.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-D"      , 2018), "clfv_2018_Embed-ETau-D.root"            , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-A"       , 2018), "clfv_2018_Embed-EMu-A.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-B"       , 2018), "clfv_2018_Embed-EMu-B.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-C"       , 2018), "clfv_2018_Embed-EMu-C.root"             , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-D"       , 2018), "clfv_2018_Embed-EMu-D.root"             , 0));
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleMu.root"                , 2));
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleEle.root"               , 1));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2018_ZETau.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2018_ZMuTau.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2018_ZEMu.root"                    , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2018_HETau.root"                   , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2018_HMuTau.root"                  , 0));
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2018_HEMu.root"                    , 0));
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2018_QCDDoubleEMEnrich30to40.root" , 0));
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50.root"                    , 0));

  return nanocards;
}


#endif
