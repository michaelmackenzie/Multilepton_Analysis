#ifndef __HISTOGRAMMING_CONFIG__
#define __HISTOGRAMMING_CONFIG__

bool debug_ = false;
Long64_t startEvent_ = 429;
Long64_t nEvents_ = 1; //at 20, verbosity returns to normal
ZTauTauHistMaker* selector_ = 0;

bool DYTesting_ = true; //speed up running for scale factor calculation/debugging
bool DYFakeTau_ = false; //speed up dy fake tau scale factor
bool WJFakeTau_ = true; //speed up w+jets fake tau scale factor
bool TTFakeTau_ = true; //speed up ttbar fake tau scale factor
bool QCDFakeTau_ = true; //speed up qcd fake tau scale factor
bool JetTauTesting_ = true; //perform MC closure test
bool CutFlowTesting_ = false; //test just basic cutflow sets

int removeTrigWeights_ = 4; //0: do nothing 1: remove weights 2: replace 3: replace P(event) 4: replace P(at least 1 triggered)
int removeBTagWeights_ = 2;//2;
int removeZPtWeights_ = 2;//2;
int useJetPUIDWeights_ = 1;//1;
int useMCFakeLep_ = 0; //use MC estimated light leptons in ee, mumu, and emu categories
int useJetToTauComp_ = 1; //use the composition of the anti-iso region to combine j->tau weights

int useEmbedCuts_ = 1; //use kinematic cuts based on embedded generation
int embeddedTesting_ = 0; //test embedding options: 3 = use KIT measured scales

int systematicSeed_ = 90; //seed for systematic random shifts
int doSystematics_ = 0;//1;
bool writeTrees_ = false; //true
int  DoMVASets_ = 1; //Fill sets with MVA cuts: 1 = emu, 2 = emu/ee/mumu3 = all sets

int splitWJets_ = true; //split w+jets sample based on N(LHE jets)
bool splitDY_ = true; //split z+jets sample based on gen-level lepton pair flavor


//information about the data file/data
struct datacard_t {
  bool process_;
  double xsec_;
  TString fname_;
  TString filepath_;
  int isData_;
  bool combine_;
  int category_;
  TH1F* events_;
  int year_;
  double lum_;
  datacard_t(bool process, double xsec, TString fname,
             int isData = 0,  bool combine = false,
             TString filepath = "",
             int category = 0, TH1F* events = 0,
             int year = 2016) :
    process_(process), xsec_(xsec), fname_(fname), combine_(combine),
    filepath_(filepath), isData_(isData), category_(category),
    events_(events), year_(year), lum_(0.) {}
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

config_t get_config(bool useUL) {
  CrossSections xs(useUL);
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
  // nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees/";
  // nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_test_trees/";
  nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees_nomva/";
  // nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees_debug/";
  // nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees_UL/";

  printf("\033[32m--- Using tree path: %s\033[0m\n", nanoaod_path.Data());

  //cross section handler
  bool useUL = nanoaod_path.Contains("_UL");
  CrossSections xs(useUL);
  vector<datacard_t> nanocards;

  //////////////////
  // 2016 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2016_ttbarToSemiLeptonic.tree"     , 0)); //1
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2016_ttbarlnu.tree"                , 0)); //2
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2016_DY50-amc.tree"                , 0)); //25
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2016_SingleAntiToptW.tree"         , 0)); //4
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2016_SingleToptW.tree"             , 0)); //5
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2016_Wlnu.tree"                    , 0, useUL&&true)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2016_WW.tree"                      , 0)); //7
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2016_WZ.tree"                      , 0)); //8
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2016_ZETau.tree"                   , 0)); //9
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2016_ZMuTau.tree"                  , 0)); //10
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2016_ZEMu.tree"                    , 0)); //11
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2016_HETau.tree"                   , 0)); //12
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2016_HMuTau.tree"                  , 0)); //13
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2016_HEMu.tree"                    , 0)); //14
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2016_SingleMu.tree"                , 2)); //15
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2016_SingleEle.tree"               , 1)); //16
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2016_ZZ.tree"                      , 0)); //17
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2016_WWW.tree"                     , 0)); //18
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2016_Wlnu-ext.tree"                , 0, useUL&&true)); //22
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2016_ttbarToHadronic.tree"         , 0)); //24
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"                 ), "clfv_2016_Wlnu-1J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"                 ), "clfv_2016_Wlnu-2J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"                 ), "clfv_2016_Wlnu-3J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"                 ), "clfv_2016_Wlnu-4J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-B"     , 2016), "clfv_2016_Embed-MuTau-B.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-C"     , 2016), "clfv_2016_Embed-MuTau-C.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-D"     , 2016), "clfv_2016_Embed-MuTau-D.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-E"     , 2016), "clfv_2016_Embed-MuTau-E.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-F"     , 2016), "clfv_2016_Embed-MuTau-F.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-G"     , 2016), "clfv_2016_Embed-MuTau-G.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-H"     , 2016), "clfv_2016_Embed-MuTau-H.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-B"      , 2016), "clfv_2016_Embed-ETau-B.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-C"      , 2016), "clfv_2016_Embed-ETau-C.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-D"      , 2016), "clfv_2016_Embed-ETau-D.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-E"      , 2016), "clfv_2016_Embed-ETau-E.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-F"      , 2016), "clfv_2016_Embed-ETau-F.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-G"      , 2016), "clfv_2016_Embed-ETau-G.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-H"      , 2016), "clfv_2016_Embed-ETau-H.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"       , 2016), "clfv_2016_Embed-EMu-B.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"       , 2016), "clfv_2016_Embed-EMu-C.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"       , 2016), "clfv_2016_Embed-EMu-D.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-E"       , 2016), "clfv_2016_Embed-EMu-E.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-F"       , 2016), "clfv_2016_Embed-EMu-F.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-G"       , 2016), "clfv_2016_Embed-EMu-G.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-H"       , 2016), "clfv_2016_Embed-EMu-H.tree"             , 0)); //
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2016_DY50.tree"                    , 0, useUL&&true)); //3
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2016_DY50-ext.tree"                , 0, useUL&&true)); //23
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2016_QCDDoubleEMEnrich30to40.tree" , 0)); //19
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2016_QCDDoubleEMEnrich30toInf.tree", 0)); //20
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2016_QCDDoubleEMEnrich40toInf.tree", 0)); //21
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunB.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunC.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunD.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunE.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunF.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunG.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunH.tree"          , 2)); //15

  //////////////////
  // 2017 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2017_ttbarToSemiLeptonic.tree"     , 0)); //25
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2017_ttbarToHadronic.tree"         , 0)); //26
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2017_ttbarlnu.tree"                , 0)); //27
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2017_DY50.tree"                    , 0, true)); //28
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2017_DY50-ext.tree"                , 0, true)); //48
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2017_SingleAntiToptW.tree"         , 0)); //29
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2017_SingleToptW.tree"             , 0)); //30
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2017_Wlnu.tree"                    , 0, true)); //31
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2017_Wlnu-ext.tree"                , 0, true)); //32
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"                 ), "clfv_2017_Wlnu-1J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"                 ), "clfv_2017_Wlnu-2J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"                 ), "clfv_2017_Wlnu-3J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2017_WW.tree"                      , 0)); //33
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2017_WZ.tree"                      , 0)); //34
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2017_ZETau.tree"                   , 0)); //35
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2017_ZMuTau.tree"                  , 0)); //36
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2017_ZEMu.tree"                    , 0)); //37
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2017_HETau.tree"                   , 0)); //38
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2017_HMuTau.tree"                  , 0)); //39
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2017_HEMu.tree"                    , 0)); //40
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2017_SingleMu.tree"                , 2)); //41
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2017_SingleEle.tree"               , 1)); //42
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2017_ZZ.tree"                      , 0)); //43
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2017_WWW.tree"                     , 0)); //44
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-B"     , 2017), "clfv_2017_Embed-MuTau-B.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-C"     , 2017), "clfv_2017_Embed-MuTau-C.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-D"     , 2017), "clfv_2017_Embed-MuTau-D.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-E"     , 2017), "clfv_2017_Embed-MuTau-E.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-MuTau-F"     , 2017), "clfv_2017_Embed-MuTau-F.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-B"      , 2017), "clfv_2017_Embed-ETau-B.tree"            , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-C"      , 2017), "clfv_2017_Embed-ETau-C.tree"            , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-D"      , 2017), "clfv_2017_Embed-ETau-D.tree"            , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-E"      , 2017), "clfv_2017_Embed-ETau-E.tree"            , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-ETau-F"      , 2017), "clfv_2017_Embed-ETau-F.tree"            , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-B"       , 2017), "clfv_2017_Embed-EMu-B.tree"             , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-C"       , 2017), "clfv_2017_Embed-EMu-C.tree"             , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-D"       , 2017), "clfv_2017_Embed-EMu-D.tree"             , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-E"       , 2017), "clfv_2017_Embed-EMu-E.tree"             , 0)); //
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Embed-EMu-F"       , 2017), "clfv_2017_Embed-EMu-F.tree"             , 0)); //
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2017_QCDDoubleEMEnrich30to40.tree" , 0)); //45
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2017_QCDDoubleEMEnrich30toInf.tree", 0)); //46
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2017_QCDDoubleEMEnrich40toInf.tree", 0)); //47

  //////////////////
  // 2018 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2018_ttbarToSemiLeptonic.tree"     , 0)); //49
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2018_ttbarToHadronic.tree"         , 0)); //50
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2018_ttbarlnu.tree"                , 0)); //51
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50-amc.tree"                , 0)); //53
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2018_SingleAntiToptW.tree"         , 0)); //54
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2018_SingleToptW.tree"             , 0)); //55
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2018_Wlnu.tree"                    , 0)); //56
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"                 ), "clfv_2018_Wlnu-1J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"                 ), "clfv_2018_Wlnu-2J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"                 ), "clfv_2018_Wlnu-3J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"                 ), "clfv_2018_Wlnu-4J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2018_WW.tree"                      , 0)); //57
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2018_WZ.tree"                      , 0)); //XXXX FIXME
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2018_ZETau.tree"                   , 0)); //58
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2018_ZMuTau.tree"                  , 0)); //59
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2018_ZEMu.tree"                    , 0)); //60
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2018_HETau.tree"                   , 0)); //61
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2018_HMuTau.tree"                  , 0)); //62
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2018_HEMu.tree"                    , 0)); //63
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleMu.tree"                , 2)); //64
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleEle.tree"               , 1)); //65
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2018_ZZ.tree"                      , 0)); //66
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2018_WWW.tree"                     , 0)); //67
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-A"     , 2018), "clfv_2018_Embed-MuTau-A.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-B"     , 2018), "clfv_2018_Embed-MuTau-B.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-C"     , 2018), "clfv_2018_Embed-MuTau-C.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-D"     , 2018), "clfv_2018_Embed-MuTau-D.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-A"      , 2018), "clfv_2018_Embed-ETau-A.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-B"      , 2018), "clfv_2018_Embed-ETau-B.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-C"      , 2018), "clfv_2018_Embed-ETau-C.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-D"      , 2018), "clfv_2018_Embed-ETau-D.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-A"       , 2018), "clfv_2018_Embed-EMu-A.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"       , 2018), "clfv_2018_Embed-EMu-B.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"       , 2018), "clfv_2018_Embed-EMu-C.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"       , 2018), "clfv_2018_Embed-EMu-D.tree"             , 0)); //
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2018_QCDDoubleEMEnrich30to40.tree" , 0)); //68
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2018_QCDDoubleEMEnrich30toInf.tree", 0)); //69
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2018_QCDDoubleEMEnrich40toInf.tree", 0)); //70
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50.tree"                    , 0)); //52

  return nanocards;
}


#endif
