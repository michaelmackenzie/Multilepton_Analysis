// Script to process NANO AODs to match format of bltTrees
using namespace CLFV;

bool copyTreesLocally_ = false;
bool copyWhenDone_ = true;
bool debug_ = false;
TString debugFile_ = "DY50-amc";
UInt_t debugStart_ = 508389;
UInt_t debugNEvents_ = 1;
NanoAODConversion* debugSelec_ = 0;
bool ignoreNoTrigger_ = true;
bool ignoreNoSelection_ = true;

//information about the data file/data
struct datacard_t {
  bool process_;
  double negfrac_;
  TString fname_;
  TString filepath_;
  int isData_;
  datacard_t(bool process, TString fname,
             double negfrac = 0., int isData = 0,
             TString filepath = "") :
    process_(process), negfrac_(negfrac), fname_(fname), filepath_(filepath),
    isData_(isData) {}
};

Int_t process_nanoaods() {

  // TString cmssw = gSystem->Getenv("CMSSW_BASE");
  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/";
  // TString path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/previous_process/";
  // TString crab_path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/crab_projects/";
  TStopwatch* timer = new TStopwatch();
  timer->Start();

  //for getting relevant dataset values (gen number, xsec, etc.)
  CrossSections xsecs;

  //Data cards
  vector<datacard_t> cards;

  ////////////
  //  2016  //
  ////////////

  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY10to50_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50-amc_2016.root"                , 0));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptW_2016.root"             , 0.003758));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptW_2016.root"         , 0.0034));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptChannel_2016.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptChannel_2016.root"   , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WW_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WZ_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ZZ_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKWminus_2016.root"               , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKWplus_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKZ-M50_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WGamma_2016.root"                  , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu_2016.root"                    , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-ext_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-1J_2016.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-2J_2016.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-3J_2016.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-4J_2016.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2016.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToHadronic_2016.root"         , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarlnu_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WWW_2016.root"                     , 0.06054));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich20to30_2016.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich30to50_2016.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich50to80_2016.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich80to120_2016.root"      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich120to170_2016.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich170to300_2016.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich300toInf_2016.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30to40_2016.root" , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30toInf_2016.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich40toInf_2016.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-B_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-C_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-D_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-E_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-F_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-G_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-H_2016.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-B_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-C_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-D_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-E_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-F_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-G_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-H_2016.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-B_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-C_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-D_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-E_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-F_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-G_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-H_2016.root"             , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleEle_2016.root"                  , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMu_2016.root"                   , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZETau_2016.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZMuTau_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZEMu_2016.root"                        , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HETau_2016.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HMuTau_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HEMu_2016.root"                        , 0.));

  // cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50_2016.root"                    , 0));
  // cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50-ext_2016.root"                , 0));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016B_2016.root"         , 0.));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016C_2016.root"         , 0.));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016D_2016.root"         , 0.));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016E_2016.root"         , 0.));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016F_2016.root"         , 0.));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016G_2016.root"         , 0.));
  // cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMuonRun2016H_2016.root"         , 0.));

  ////////////
  //  2017  //
  ////////////

  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY10to50_2017.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50_2017.root"                    , 0.1624));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50-ext_2017.root"                , 0.1624));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptW_2017.root"             , 0.003758));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptW_2017.root"         , 0.0034));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptChannel_2017.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptChannel_2017.root"   , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WW_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WZ_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ZZ_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WWW_2017.root"                     , 0.06054));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKWminus_2017.root"               , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKWplus_2017.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKZ-M50_2017.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WGamma_2017.root"                  , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu_2017.root"                    , 0.0004079));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-ext_2017.root"                , 0.0004079));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-1J_2017.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-2J_2017.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-3J_2017.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-4J_2017.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2017.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToHadronic_2017.root"         , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarlnu_2017.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30to40_2017.root" , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30toInf_2017.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich40toInf_2017.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-B_2017.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-C_2017.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-D_2017.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-E_2017.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-F_2017.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-B_2017.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-C_2017.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-D_2017.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-E_2017.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-F_2017.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-B_2017.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-C_2017.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-D_2017.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-E_2017.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-F_2017.root"             , 0.));
  cards.push_back(datacard_t(true , "dataprocess/LFVAnalysis_SingleEle_2017.root"                  , 0.));
  cards.push_back(datacard_t(true , "dataprocess/LFVAnalysis_SingleMu_2017.root"                   , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZETau_2017.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZMuTau_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZEMu_2017.root"                        , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HETau_2017.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HMuTau_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HEMu_2017.root"                        , 0.));

  ////////////
  //  2018  //
  ////////////

  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY10to50_2018.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50-amc_2018.root"                , 0.0004962));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptW_2018.root"             , 0.003758));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptW_2018.root"         , 0.0034));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptChannel_2018.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptChannel_2018.root"   , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WW_2018.root"                      , 0.001916));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WZ_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ZZ_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WWW_2018.root"                     , 0.06197));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKWminus_2018.root"               , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKWplus_2018.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_EWKZ-M50_2018.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WGamma_2018.root"                  , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu_2018.root"                    , 0.0003866));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-1J_2018.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-2J_2018.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-3J_2018.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-4J_2018.root"                 , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2018.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToHadronic_2018.root"         , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarlnu_2018.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich15to20_2018.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich20to30_2018.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich30to50_2018.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich50to80_2018.root"       , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich80to120_2018.root"      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich120to170_2018.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich170to300_2018.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDEMEnrich300toInf_2018.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30toInf_2018.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich40toInf_2018.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-A_2018.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-B_2018.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-C_2018.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-MuTau-D_2018.root"           , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-A_2018.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-B_2018.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-C_2018.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-ETau-D_2018.root"            , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-A_2018.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-B_2018.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-C_2018.root"             , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Embed-EMu-D_2018.root"             , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleEle_2018.root"                  , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMu_2018.root"                   , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZETau_2018.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZMuTau_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZEMu_2018.root"                        , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HETau_2018.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HMuTau_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HEMu_2018.root"                        , 0.));
  // cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30to40_2018.root" , 0.));
  // cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50_2018.root"                    , 0.0004962));

  for(unsigned index = 0; index < cards.size(); ++index) {
    //if debugging, only do requested file
    if(debug_ && !cards[index].fname_.Contains(debugFile_.Data())) continue; //don't print other info in debug
    if(!cards[index].process_) continue;
    cout << "**Processing file " << cards[index].fname_.Data() << endl;

    const bool isDY = cards[index].fname_.Contains("DY");
    TFile* f = 0;
    if(copyTreesLocally_) {
      gSystem->Exec(Form("xrdcp -f %s tmp.tree", (path+cards[index].fname_).Data()));
      f = TFile::Open("tmp.tree", "READ");
    } else {
      f = TFile::Open((path+cards[index].fname_).Data(), "READ");
    }
    if(!f) {
      cout << " File not found! Continuing..." << endl;
      continue;
    }
    TTree* t = (TTree*) f->Get("Events");
    if(!t) {
      cout << " Tree not found! Continuing..." << endl;
      continue;
    }
    TString name = cards[index].fname_;
    int year = ParticleCorrections::k2016;
    if(name.Contains("2017")) year = ParticleCorrections::k2017;
    else if(name.Contains("2018")) year = ParticleCorrections::k2018;
    Int_t isData = 0;
    if(name.Contains("SingleEle")) isData = 1;
    else if(name.Contains("SingleMu")) isData = 2;

    name.ReplaceAll("MC/", "");
    name.ReplaceAll("backgrounds/", "");
    name.ReplaceAll("signals/", "");
    name.ReplaceAll("dataprocess/", "");
    name.ReplaceAll("LFVAnalysis", "");
    name.ReplaceAll("2016", "");
    name.ReplaceAll("2017", "");
    name.ReplaceAll("2018", "");
    name.ReplaceAll(".root", "");
    name.ReplaceAll("_", "");
    float events = -1.;
    double frac_neg = cards[index].negfrac_;
    if(isData == 0) {
      // if(!f->Get("events")) {//Try getting normalization parameters via map and crab if no event count histogram
      //   TString crab_report;
      //   TString crab_command = "crab report -d " + crab_path;
      //   crab_command += "samples_MC_";
      //   if(year == ParticleCorrections::k2016)      crab_command += "2016/crab_2016_LFVDONE_";
      //   else if(year == ParticleCorrections::k2017) crab_command += "2017/crab_2017_LFVDONE_";
      //   else if(year == ParticleCorrections::k2018) crab_command += "2018/crab_2018_LFVDONE_";
      //   crab_command += name;
      //   crab_command += " | grep read | awk '{print $NF}'";
      //   cout << "Getting normalization, using command:\n" << crab_command.Data() << endl;
      //   FILE* shell_res = gSystem->OpenPipe(crab_command.Data(), "r");
      //   crab_report.Gets(shell_res);
      //   gSystem->ClosePipe(shell_res);
      //   try {
      //     events = stof(crab_report.Data());
      //   } catch(exception e) {
      //     events = 0.;
      //   }
      //   cout << "Found " << events << " events for the file with "
      //        << frac_neg << " fraction negative events" << endl;
      events = 0;
      } else { //event count histogram is found
        TH1D* hevents = (TH1D*) f->Get("events");
        double nevents = hevents->GetBinContent(1);
        if(nevents > 0.)
          frac_neg = hevents->GetBinContent(10) / nevents;
        events = (float) nevents;
        cout << "Found events histogram, using nevents = " << nevents << " and frac_neg = "
             << frac_neg << " (vs " << cards[index].negfrac_ << ")\n";
      }
      if(events <= 0.) {
        cout << "No events found via crab dir, trying instead from CrossSection util...\n";
        events = xsecs.GetGenNumber(name, year + (2016-ParticleCorrections::k2016)); //uses absolute value year, not enum
      }
      if(events <= 0.) {
        cout << "No events found! Can't do normalization --> continue to next file!\n";
        continue;
      }
    }
    bool isSignal = name.Contains("ZEMu") || name.Contains("ZETau") || name.Contains("ZMuTau");
    isSignal     |= name.Contains("HEMu") || name.Contains("HETau") || name.Contains("HMuTau");
    bool isEmbed = name.Contains("Embed");
    NanoAODConversion* selec = new NanoAODConversion();
    if(events > 0.) selec->fEventCount = events*(1.-2.*frac_neg);
    selec->fFolderName = name;
    selec->fYear = year;
    selec->fIsData = isData;
    selec->fIsEmbed = isEmbed;
    selec->fReplacePUWeights = isSignal && !isData; //replace signal PU weights due to an issue in the skimmer
    selec->fSkipDoubleTrigger = false; //pre-skipped
    selec->fSkipMuMuEE   = 0; //skip same flavor data
    selec->fSkipMuMuEESS = 0; //skip same-sign same flavor data
    selec->fIsDY = isDY; //for Z pT weights
    selec->fVerbose = (debug_ && debugNEvents_ < 100) ? 3 : 1; //print warnings as they come normally, print extra in debug
    selec->fIgnoreNoTriggerMatch = ignoreNoTrigger_;
    selec->fIgnoreNoSelection = ignoreNoSelection_;
    if(debug_ && debugNEvents_ <= 2) selec->fVerbose = 10;
    if(debug_) debugSelec_ = selec;
    if(!debug_)
      t->Process(selec);
    else if(debugNEvents_ == 0)
      t->Process(selec,"",-1, debugStart_);
    else
      t->Process(selec,"",debugNEvents_, debugStart_);
    if(!debug_) delete selec;
    delete f;
    if(!debug_ && copyWhenDone_) {
      if(copyTreesLocally_) gSystem->Exec("rm tmp.tree");
      TString tname = Form("clfv_%i_%s", year+2016, name.Data());
      gSystem->Exec(Form("./move_trees.sh \"\" \"\" d %s; rm %s.tree;", tname.Data(), tname.Data()));
    }
  }

  if(copyTreesLocally_ && !copyWhenDone_) gSystem->Exec("rm tmp.tree");

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Total processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Total processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
