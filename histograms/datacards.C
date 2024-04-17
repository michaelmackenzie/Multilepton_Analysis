#ifndef __CLFV_DATACARDS__
#define __CLFV_DATACARDS__

typedef DataCard_t dcard;

vector<int> years_    = {2016, 2017, 2018}; //list of years of interest
int useRunPeriodData_ = 1; //use run periods of data
int doRunPeriod_      = 0; //use predefined set of run periods of data (e.g. 2016 B-F)
int useMuonEGData_    = 0; //include MuonEG data in the emu data selections
vector<TString> runs_ = {}; //runs to use
TString hist_path_    = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/"; //where histogram files are
TString hist_dir_     = "nanoaods_dev"; //which histogram directory to use
TString hist_tag_     = "clfv"; //leading tag from HistMaker, e.g. "clfv", "hist", or "sparse"

int    useUL_         =  0 ; //use UL dataset definitions
int    ZMode_         =  0 ; //which Z cross-section information to use
int    useAMC_        =  1 ; //use amc@NLO samples in place of previous LO samples
int    useWG_         =  1 ; //use the W+gamma dataset
int    useEWK_        =  0 ; //include the EWK W/Z samples
int    useWWW_        =  1 ; //include the WWW samples
int    splitDY_       =  1 ; //split Z->tautau and Z->ee/mumu
int    splitWJ_       =  1 ; //use N(LHE jets) split W+Jets samples
int    useEmbed_      =  1 ; //use Z->tautau embedding
double embedScale_    =  1.; //scale factor to add onto the embedding normalization, < 0 means use defaults
int    useQCDMC_      =  0 ; //use MC QCD background estimates
int    combineVB_     =  1 ; //combine W+Jets with other vector boson processes
int    includeHiggs_  =  0 ; //include the higgs signals in the plots
int    higgsBkg_      =  0 ; //include SM higgs samples in the background estimate
int    combineHB_     =  1 ; //combine H->tautau and H->WW backgrounds
int    correctEmbed_  =  1 ; //check the event histogram vs gen numbers to correct for missing events
int    correctData_   =  1 ; //check the event histogram vs gen numbers to correct for missing events
bool   useLepTauSet_  =  1 ; //use leptonic tau files for leptonic tau selections
bool   signalInStudy_ =  0 ; //include signal distributions in processing for studies
bool   skipData_      =  0 ; //don't initialize data cards

//for adjusting cross sections
float zll_scale_ = 1.f; //Z->ee/mumu yield scale

//Blind the analysis
bool blind_ = true;

//get the data cards needed
void get_datacards(std::vector<dcard>& cards, TString selection, int forStudies = 0 /*0: plotting; 1: studies; 2: fits*/) {
  //cross section handler
  CrossSections xs(useUL_, ZMode_);
  bool leptonic_tau = (selection.Contains("tau_")); //mutau_l, etau_l
  std::vector<bool> combines;
  bool oneDY = false; //for faster scale factor testing/debugging

  //Initialize the data run by year
  std::map<int, std::vector<TString>> runs;
  std::map<int, std::vector<TString>> periods;
  periods[2016] = {"B", "C", "D", "E", "F", "G", "H"};
  periods[2017] = {"B", "C", "D", "E", "F"};
  periods[2018] = {"A", "B", "C", "D"};

  //Update the runs used if not using the full year's data
  if(runs_.size() > 0 && years_.size() > 1) {
    cout << "WARNING! Using partial year running for multiple years!\n";
  }

  if(runs_.size() > 0) {
    for(int year : years_) {
      periods[year] = runs_;
    }
  }

  //Names of the different background categories
  TString top   = "Top";
  TString dy_tt = (forStudies == 1) ? "ZJets" : "Z->#tau#tau";
  TString dy_ll = (forStudies == 1) ? "ZJets" : "Z->ee/#mu#mu";
  TString dy    = (forStudies == 1) ? "ZJets" : "Drell-Yan";
  TString embed = (forStudies == 1) ? "ZJets" : "#tau#tau Embedding";
  TString wj    = (forStudies == 1) ? "WJets" : (combineVB_) ? "Other VB" : "W+Jets";
  TString vb    = (forStudies == 1) ? "WJets" : "Other VB";
  TString hb_tt = (forStudies == 1) ? "HBkg"  : (combineHB_) ? "H->#tau#tau/WW" : "H->#tau#tau";
  TString hb_ww = (forStudies == 1) ? "HBkg"  : (combineHB_) ? "H->#tau#tau/WW" : "H->WW";

  const int top_c = kYellow - 7;
  const int dy_ll_c = (forStudies == 1) ? kRed - 7 : kRed - 2;
  const int dy_tt_c = (forStudies == 1) ? kRed - 7 : kRed - 7;
  const int wj_c = kViolet - 9;
  const int vb_c = (vb == wj) ? wj_c : kMagenta - 9;
  const int hb_tt_c = kAtlantic;
  const int hb_ww_c = (combineHB_) ? hb_tt_c : kCyan;

  for(int year : years_) {
    bool combineZ = !oneDY && !useUL_ && year != 2018 && (!useAMC_ || year == 2017);
    TString DYName = (useAMC_ && year != 2017) ? "DY50-amc" : "DY50";
    //card constructor:    filepath,              name,                  label,      isData,                   xsec               ,  isSignal,year,  color,   combine extension samples
    if(higgsBkg_) {
      cards.push_back(dcard("ggFH-TauTau"        , "ggFH-TauTau"          , hb_tt.Data(), false, xs.GetCrossSection("ggFH-TauTau"         ), false, year, hb_tt_c));
      cards.push_back(dcard("VBFH-TauTau"        , "VBFH-TauTau"          , hb_tt.Data(), false, xs.GetCrossSection("VBFH-TauTau"         ), false, year, hb_tt_c));
      cards.push_back(dcard("WminusH-TauTau"     , "WminusH-TauTau"       , hb_tt.Data(), false, xs.GetCrossSection("WminusH-TauTau"      ), false, year, hb_tt_c));
      cards.push_back(dcard("WplusH-TauTau"      , "WplusH-TauTau"        , hb_tt.Data(), false, xs.GetCrossSection("WplusH-TauTau"       ), false, year, hb_tt_c));
      cards.push_back(dcard("ZH-TauTau"          , "ZH-TauTau"            , hb_tt.Data(), false, xs.GetCrossSection("VBFH-TauTau"         ), false, year, hb_tt_c));
      cards.push_back(dcard("ggFH-WW"            , "ggFH-WW"              , hb_ww.Data(), false, xs.GetCrossSection("ggFH-WW"             ), false, year, hb_ww_c));
      cards.push_back(dcard("VBFH-WW"            , "VBFH-WW"              , hb_ww.Data(), false, xs.GetCrossSection("VBFH-WW"             ), false, year, hb_ww_c));
    }
    cards.push_back(dcard("SingleAntiToptW"      , "SingleAntiToptW"      , top.Data()  , false, xs.GetCrossSection("SingleAntiToptW"      ), false, year, top_c));
    cards.push_back(dcard("SingleToptW"          , "SingleToptW"          , top.Data()  , false, xs.GetCrossSection("SingleToptW"          ), false, year, top_c));
    cards.push_back(dcard("SingleAntiToptChannel", "SingleAntiToptChannel", top.Data()  , false, xs.GetCrossSection("SingleAntiToptChannel"), false, year, top_c));
    cards.push_back(dcard("SingleToptChannel"    , "SingleToptChannel"    , top.Data()  , false, xs.GetCrossSection("SingleToptChannel"    ), false, year, top_c));
    cards.push_back(dcard("ttbarToHadronic"      , "ttbarToHadronic"      , top.Data()  , false, xs.GetCrossSection("ttbarToHadronic"      ), false, year, top_c));
    cards.push_back(dcard("ttbarToSemiLeptonic"  , "ttbarToSemiLeptonic"  , top.Data()  , false, xs.GetCrossSection("ttbarToSemiLeptonic"  ), false, year, top_c));
    cards.push_back(dcard("ttbarlnu"             , "ttbarlnu"             , top.Data()  , false, xs.GetCrossSection("ttbarlnu"             ), false, year, top_c));
    if(useUL_ == 0 && useWWW_)
    cards.push_back(dcard("WWW"                  , "WWW"                  , vb.Data()   , false, xs.GetCrossSection("WWW"                  ), false, year, vb_c));
    cards.push_back(dcard("WZ"                   , "WZ"                   , vb.Data()   , false, xs.GetCrossSection("WZ"                   ), false, year, vb_c));
    cards.push_back(dcard("ZZ"                   , "ZZ"                   , vb.Data()   , false, xs.GetCrossSection("ZZ"                   ), false, year, vb_c));
    cards.push_back(dcard("WW"                   , "WW"                   , vb.Data()   , false, xs.GetCrossSection("WW"                   ), false, year, vb_c));
    cards.push_back(dcard("WWLNuQQ"              , "WWLNuQQ"              , vb.Data()   , false, xs.GetCrossSection("WWLNuQQ"              ), false, year, vb_c)); //FIXME: Add back in
    if(useEWK_) {
    cards.push_back(dcard("EWKWplus"             , "EWKWplus"             , vb.Data()   , false, xs.GetCrossSection("EWKWplus"             ), false, year, vb_c));
    cards.push_back(dcard("EWKWminus"            , "EWKWminus"            , vb.Data()   , false, xs.GetCrossSection("EWKWminus"            ), false, year, vb_c));
    cards.push_back(dcard("EWKZ-M50"             , "EWKZ-M50"             , vb.Data()   , false, xs.GetCrossSection("EWKZ-M50"             ), false, year, vb_c));
    }
    if(useWG_)
    cards.push_back(dcard("WGamma"               , "WGamma"               , wj.Data()   , false, xs.GetCrossSection("WGamma"               ), false, year, wj_c));
    // cards.push_back(dcard("WGamma"               , "WGamma"               , "W#gamma"   , false, xs.GetCrossSection("WGamma"               ), false, year, kMagenta+3));
    //if splitting W+Jets into jet-binned samples, use W+Jets inclusive 0-j for 0-j, then jet-binned samples for the rest
    if(splitWJ_) {
      cards.push_back(dcard("Wlnu-0"             , "Wlnu-0"               , wj.Data()   , false, xs.GetCrossSection("Wlnu"                 ), false, year, wj_c, !useUL_&&year!=2018));
      if(year != 2018 && !useUL_){
        cards.push_back(dcard("Wlnu-ext-0"       , "Wlnu-ext-0"           , wj.Data()   , false, xs.GetCrossSection("Wlnu"                 ), false, year, wj_c, true));
      }
      // cards.push_back(dcard("Wlnu-0"             , "Wlnu-0"               , "W+0j"   , false, xs.GetCrossSection("Wlnu"                 ), false, year, kMagenta, !useUL_&&year!=2018));
      // if(year != 2018 && !useUL_){
      //   cards.push_back(dcard("Wlnu-ext-0"       , "Wlnu-ext-0"           , "W+0j"   , false, xs.GetCrossSection("Wlnu"                 ), false, year, kMagenta, true));
      // }
      cards.push_back(dcard("Wlnu-1J"            , "Wlnu-1J"              , wj.Data()   , false, xs.GetCrossSection("Wlnu-1J"        , year), false, year, wj_c, false));
      cards.push_back(dcard("Wlnu-2J"            , "Wlnu-2J"              , wj.Data()   , false, xs.GetCrossSection("Wlnu-2J"        , year), false, year, wj_c, false));
      cards.push_back(dcard("Wlnu-3J"            , "Wlnu-3J"              , wj.Data()   , false, xs.GetCrossSection("Wlnu-3J"        , year), false, year, wj_c, false));
      cards.push_back(dcard("Wlnu-4J"            , "Wlnu-4J"              , wj.Data()   , false, xs.GetCrossSection("Wlnu-4J"        , year), false, year, wj_c, false));
    } else {
      cards.push_back(dcard("Wlnu"               , "Wlnu"                 , wj.Data()   , false, xs.GetCrossSection("Wlnu"                 ), false, year, wj_c, !useUL_&&year!=2018));
      if(year != 2018 && !useUL_){
        cards.push_back(dcard("Wlnu-ext"        , "Wlnu-ext"              , wj.Data()   , false, xs.GetCrossSection("Wlnu"                 ), false, year, wj_c, true));
      }
    }
    if(splitDY_ > 0 || (useEmbed_ == 2 || (useEmbed_ && selection != "ee" && selection != "mumu"))) {
      //useEmbed_: 0 = use DY MC; 1 = use embedding in emu/etau/mutau; 2 = use embedding in all categories (including ee/mumu)
      //in ee/mumu, switch order of Z->ll/tautau
      if(selection == "ee" || selection == "mumu") {
        if(useEmbed_ < 2) {//if using DY MC, include Z->tau tau
          cards.push_back(dcard((DYName+"-1").Data(), (DYName+"-1").Data(), dy_tt.Data(), false, xs.GetCrossSection("DY50"    , year), false, year, dy_tt_c, combineZ));
          cards.push_back(dcard("DY10to50-1"        , "DY10to50-1"        , dy_tt.Data(), false, xs.GetCrossSection("DY10to50", year), false, year, dy_tt_c));
        }
        if(useEmbed_ < 2) { //use DY MC ee/mumu if using DY MC in general
          cards.push_back(dcard((DYName+"-2").Data(), (DYName+"-2").Data(), dy_ll.Data(), false, zll_scale_*xs.GetCrossSection("DY50"    , year), false, year, dy_ll_c, combineZ));
          cards.push_back(dcard("DY10to50-2"        , "DY10to50-2"        , dy_ll.Data(), false, zll_scale_*xs.GetCrossSection("DY10to50", year), false, year, dy_ll_c));
        }
      } else {
        if(true) { //use DY MC ee/mumu for non-ee/mumu categories
          cards.push_back(dcard((DYName+"-2").Data(), (DYName+"-2").Data(), dy_ll.Data(), false, zll_scale_*xs.GetCrossSection("DY50"    , year), false, year, dy_ll_c, combineZ));
          cards.push_back(dcard("DY10to50-2"        , "DY10to50-2"        , dy_ll.Data(), false, zll_scale_*xs.GetCrossSection("DY10to50", year), false, year, dy_ll_c));
        }
        if(useEmbed_ == 0) {//either ee/mumu or using DY MC, in both cases need MC Z->tau tau
          cards.push_back(dcard((DYName+"-1").Data(), (DYName+"-1").Data(), dy_tt.Data(), false, xs.GetCrossSection("DY50"    , year), false, year, dy_tt_c, combineZ));
          cards.push_back(dcard("DY10to50-1"        , "DY10to50-1"        , dy_tt.Data(), false, xs.GetCrossSection("DY10to50", year), false, year, dy_tt_c));
        }
      }
      //useEmbed_: 0 = use DY MC; 1 = use embedding in emu/etau/mutau; 2 = use embedding in all categories (including ee/mumu)
      if(useEmbed_ == 2 || (useEmbed_ && selection != "ee" && selection != "mumu")) {
        for(int period = 0; period < periods[year].size(); ++period) {
          TString run = periods[year][period];
          if(!(selection == "ee" || selection == "mumu")) {
            if(selection != "etau")
              cards.push_back(dcard(("Embed-MuTau-"+run).Data(), ("Embed-MuTau-"+run).Data(), embed.Data(), false, xs.GetCrossSection("Embed-MuTau-"+run, year), false, year, dy_tt_c));
            if(selection != "mutau")
              cards.push_back(dcard(("Embed-ETau-" +run).Data(), ("Embed-ETau-" +run).Data(), embed.Data(), false, xs.GetCrossSection("Embed-ETau-" +run, year), false, year, dy_tt_c));
            cards.push_back(dcard(("Embed-EMu-"  +run).Data(), ("Embed-EMu-"  +run).Data(), embed.Data(), false, xs.GetCrossSection("Embed-EMu-"  +run, year), false, year, dy_tt_c));
          } else if(selection == "mumu") {
            cards.push_back(dcard(("Embed-MuMu-"+run).Data(), ("Embed-MuMu-"+run).Data(), "#mu#mu Embedding", false, xs.GetCrossSection("Embed-MuMu-"+run, year), false, year, dy_ll_c));
          } else if(selection == "ee") {
            cards.push_back(dcard(("Embed-EE-"  +run).Data(), ("Embed-EE-"  +run).Data(), "ee Embedding"    , false, xs.GetCrossSection("Embed-EE-"  +run, year), false, year, dy_ll_c));
          }
        }
      }
    } else if(splitDY_ == 0) {
      cards.push_back(dcard((DYName+"-1").Data(), (DYName+"-1").Data(), dy.Data(), false, xs.GetCrossSection("DY50"    , year), false, year, dy_tt_c, combineZ));
      cards.push_back(dcard((DYName+"-2").Data(), (DYName+"-2").Data(), dy.Data(), false, zll_scale_*xs.GetCrossSection("DY50"    , year), false, year, dy_tt_c, combineZ));
      cards.push_back(dcard("DY10to50-1"        , "DY10to50-1"        , dy.Data(), false, xs.GetCrossSection("DY10to50", year), false, year, dy_tt_c));
      cards.push_back(dcard("DY10to50-2"        , "DY10to50-2"        , dy.Data(), false, zll_scale_*xs.GetCrossSection("DY10to50", year), false, year, dy_tt_c));
    } else { //splitDY_ < 0, assume old dataset that was never separated into tautau and ee/mumu output files
      cards.push_back(dcard(DYName.Data(), DYName.Data(), dy.Data(), false, xs.GetCrossSection("DY50"    , year), false, year, dy_tt_c, combineZ));
      cards.push_back(dcard("DY10to50"   , "DY10to50"   , dy.Data(), false, xs.GetCrossSection("DY10to50", year), false, year, dy_tt_c));
    }
    if(combineZ) {
      if(splitDY_ > 0 || useEmbed_) {
        if(!(selection == "ee" || selection == "mumu") || useEmbed_ < 2) {
          cards.push_back(dcard((DYName+"-ext-2").Data(), (DYName+"-ext-2").Data(), dy_ll.Data(), false, zll_scale_*xs.GetCrossSection("DY50", year), false, year, dy_ll_c   , true));
        }
        if(!useEmbed_ || selection == "ee" || selection == "mumu") {
          cards.push_back(dcard((DYName+"-ext-1").Data(), (DYName+"-ext-1").Data(), dy_tt.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
        }
      } else if(splitDY_ == 0) {
        cards.push_back(dcard((DYName+"-ext-1").Data(), (DYName+"-ext-1").Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
        cards.push_back(dcard((DYName+"-ext-2").Data(), (DYName+"-ext-2").Data(), dy.Data(), false, zll_scale_*xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
      } else {
        cards.push_back(dcard((DYName+"-ext").Data(), (DYName+"-ext").Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
      }
    }
    if(useQCDMC_) {
      if(year != 2017) { //FIXME: Add in 2017 QCD datasets
        if(year == 2018) {
          cards.push_back(dcard("QCDEMEnrich15to20"       , "QCDEMEnrich15to20"       , "QCD", false, xs.GetCrossSection("QCDEMEnrich15to20"       ), false, year, kOrange+6));
        }
        cards.push_back(dcard("QCDEMEnrich20to30"       , "QCDEMEnrich20to30"       , "QCD", false, xs.GetCrossSection("QCDEMEnrich20to30"       ), false, year, kOrange+6));
        cards.push_back(dcard("QCDEMEnrich30to50"       , "QCDEMEnrich30to50"       , "QCD", false, xs.GetCrossSection("QCDEMEnrich30to50"       ), false, year, kOrange+6));
        cards.push_back(dcard("QCDEMEnrich50to80"       , "QCDEMEnrich50to80"       , "QCD", false, xs.GetCrossSection("QCDEMEnrich50to80"       ), false, year, kOrange+6));
        cards.push_back(dcard("QCDEMEnrich80to120"      , "QCDEMEnrich80to120"      , "QCD", false, xs.GetCrossSection("QCDEMEnrich80to120"      ), false, year, kOrange+6));
        cards.push_back(dcard("QCDEMEnrich120to170"     , "QCDEMEnrich120to170"     , "QCD", false, xs.GetCrossSection("QCDEMEnrich120to170"     ), false, year, kOrange+6));
        cards.push_back(dcard("QCDEMEnrich170to300"     , "QCDEMEnrich170to300"     , "QCD", false, xs.GetCrossSection("QCDEMEnrich170to300"     ), false, year, kOrange+6));
        cards.push_back(dcard("QCDEMEnrich300toInf"     , "QCDEMEnrich300toInf"     , "QCD", false, xs.GetCrossSection("QCDEMEnrich300toInf"     ), false, year, kOrange+6));
      }
    }
    if(forStudies == 0 || (forStudies == 1 && signalInStudy_)) {
      const double zxs = xs.GetCrossSection("Z");
      const double hxs = xs.GetCrossSection("H");
      if(selection == "emu") {
        cards.push_back(                  dcard("ZEMu"             , "ZEMu"             , "Z->e#mu (10^{-5})"   , false, 1.e-5*zxs, true, year, kBlue   ));
        if(includeHiggs_) cards.push_back(dcard("HEMu"             , "HEMu"             , "H->e#mu (10^{-2})"   , false, 1.e-2*hxs, true, year, kGreen-1));
      } else if(selection.Contains("etau") || selection == "ee") {
        cards.push_back(                  dcard("ZETau"            , "ZETau"            , "Z->e#tau (10^{-3})"  , false, 1.e-3*zxs, true, year, kBlue   ));
        if(includeHiggs_) cards.push_back(dcard("HETau"            , "HETau"            , "H->e#tau (10^{-1})"  , false, 1.e-1*hxs, true, year, kGreen-1));
      } else if(selection.Contains("mutau") || selection == "mumu") {
        cards.push_back(                  dcard("ZMuTau"           , "ZMuTau"           , "Z->#mu#tau (10^{-3})", false, 1.e-3*zxs, true, year, kBlue   ));
        if(includeHiggs_) cards.push_back(dcard("HMuTau"           , "HMuTau"           , "H->#mu#tau (10^{-1})", false, 1.e-1*hxs, true, year, kGreen-1));
      }
    } else if(forStudies == 2) { //include signal in a study at the nominal cross section
      if(selection == "emu") {
        cards.push_back(                  dcard("ZEMu"             , "ZEMu"             , "Z->e#mu"   , false, xs.GetCrossSection("ZEMu"  ), true, year, kBlue   ));
        if(includeHiggs_) cards.push_back(dcard("HEMu"             , "HEMu"             , "H->e#mu"   , false, xs.GetCrossSection("HEMu"  ), true, year, kGreen-1));
      } else if(selection.Contains("etau") || selection == "ee") {
        cards.push_back(                  dcard("ZETau"            , "ZETau"            , "Z->e#tau"  , false, xs.GetCrossSection("ZETau" ), true, year, kBlue   ));
        if(includeHiggs_) cards.push_back(dcard("HETau"            , "HETau"            , "H->e#tau"  , false, xs.GetCrossSection("HETau" ), true, year, kGreen-1));
      } else if(selection.Contains("mutau") || selection == "mumu") {
        cards.push_back(                  dcard("ZMuTau"           , "ZMuTau"           , "Z->#mu#tau", false, xs.GetCrossSection("ZMuTau"), true, year, kBlue   ));
        if(includeHiggs_) cards.push_back(dcard("HMuTau"           , "HMuTau"           , "H->#mu#tau", false, xs.GetCrossSection("HMuTau"), true, year, kGreen-1));
      }
    }
    //add data
    if(!skipData_) {
      if(useRunPeriodData_ == 0) { //use merged data samples
        if(selection != "etau"  && selection !="ee"  ) cards.push_back(dcard("SingleMu" , "SingleMu" , "Data", true , 1., false, year));
        if(selection != "mutau" && selection !="mumu") cards.push_back(dcard("SingleEle", "SingleEle", "Data", true , 1., false, year));
        if(useMuonEGData_ && (selection == "emu" || selection.Contains("_"))) cards.push_back(dcard("MuonEG", "MuonEG", "Data", true , 1., false, year));
      } else { //use run-period-specific data samples
        for(int period = 0; period < periods[year].size(); ++period) {
          TString muon_name = "SingleMuon-" + periods[year][period];
          TString electron_name = "SingleElectron-" + periods[year][period];
          TString muoneg_name = "MuonEG-" + periods[year][period];
          if(selection != "etau"  && selection!="ee"  ) cards.push_back(dcard(muon_name    , muon_name    , "Data", true , 1., false, year));
          if(selection != "mutau" && selection!="mumu") cards.push_back(dcard(electron_name, electron_name, "Data", true , 1., false, year));
          if(useMuonEGData_ && (selection == "emu" || selection.Contains("_"))) cards.push_back(dcard(muoneg_name, muoneg_name, "Data", true , 1., false, year));
        }
      }
    }
  } //end years loop


  TString selection_dir = (leptonic_tau && !useLepTauSet_) ? "emu" : selection;
  //add full name to file name
  for(unsigned index = 0; index < cards.size(); ++index) {
    cards[index].isembed_ = cards[index].name_.Contains("Embed");
    //Update Wlnu cross section if combining to account for the two samples
    if(cards[index].combine_) {
      TString name = cards[index].name_;
      const bool isext = name.Contains("-ext");
      name.ReplaceAll("-ext", "");
      name.ReplaceAll("-0", ""); //remove W+Jets splitting labels if needed
      name.ReplaceAll("-1", ""); //remove DY splitting labels if needed
      name.ReplaceAll("-2", "");
      const long num1 = xs.GetGenNumber(name       , cards[index].year_); //get number of events per sample
      const long num2 = xs.GetGenNumber(name+"-ext", cards[index].year_);
      if(isext && num1 > 0 && num2 > 0) {
        const double frac = (num2*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << "-ext cross section by " << frac
             << " to combine the " << cards[index].year_ << " extension samples!\n";
        cards[index].xsec_ *= frac;
      } else if(num1 > 0 && num2 > 0){
        const double frac = (num1*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << " cross section by " << frac
             << " to combine the " << cards[index].year_ << " extension samples!\n";
        cards[index].xsec_ *= frac;
      } else {
        std::cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << std::endl;
      }
      // std::cout << "--> New cross section = " << cards[index].xsec_ << std::endl;
    }
    //update file path
    cards[index].filename_ = Form("%s/%s_%s_%i_%s.hist", (hist_path_+hist_dir_).Data(), hist_tag_.Data(), selection_dir.Data(),
                                  cards[index].year_, (cards[index].filename_).Data());

    //check if it's an embedding file and correcting for lost events
    if((correctEmbed_ && cards[index].name_.Contains("Embed")) ||
       (correctData_ && (cards[index].name_.Contains("SingleMuon") || cards[index].name_.Contains("SingleElectron")))) {
      //Open the file to access the event count histogram
      TFile* f = TFile::Open(cards[index].filename_.Data(), "READ");
      if(!f) continue;
      TH1* events = (TH1*) f->Get("events");
      if(!events) {std::cout << "Event histogram not found in " << f->GetName() << std::endl; f->Close(); continue;}
      const double nevt = events->GetBinContent(1);
      const double ngen = xs.GetGenNumber(cards[index].name_, cards[index].year_);
      if(ngen < 1) {f->Close(); continue;}
      if(ngen > nevt) { //only apply a correction if ngen/nevt make sense
        std::cout << "Dataset " << cards[index].name_.Data() << ", " << cards[index].year_ << ", has gen = "
                  << ngen << " and event count = " << nevt << " --> applying " << ngen/nevt << " correction\n";
        cards[index].xsec_ *= ngen/nevt;
      }
      f->Close();
    }
  } //end file name loop
}

#endif
