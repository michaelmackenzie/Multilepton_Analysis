#ifndef __CLFV_DATACARDS__
#define __CLFV_DATACARDS__

typedef DataCard_t dcard;

vector<int> years_    = {2016, 2017, 2018}; //list of years of interest
int useRunPeriodData_ = 1; //use run periods of data
int doRunPeriod_      = 0; //use predefined set of run periods of data (e.g. 2016 B-F)
TString hist_path_    = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/"; //where histogram files are
TString hist_dir_     = "nanoaods_dev"; //which histogram directory to use
TString hist_tag_     = "clfv"; //leading tag from HistMaker, e.g. "clfv", "hist", or "sparse"

int    useUL_        =  0 ; //use UL dataset definitions
int    ZMode_        =  0 ; //which Z cross-section information to use
int    useAMC_       =  1 ; //use amc@NLO samples in place of previous LO samples
int    splitDY_      =  1 ; //split Z->tautau and Z->ee/mumu
int    splitWJ_      =  1 ; //use N(LHE jets) split W+Jets samples
int    useEmbed_     =  1 ; //use Z->tautau embedding
double embedScale_   =  1.; //scale factor to add onto the embedding normalization, < 0 means use defaults
int    useQCDMC_     =  0 ; //use MC QCD background estimates
int    combineVB_    =  1 ; //combine W+Jets with other vector boson processes
int    includeHiggs_ =  1 ; //include the higgs signals in the plots
int    correctEmbed_ =  0 ; //check the event histogram vs gen numbers to correct for missing events

//get the data cards needed
void get_datacards(std::vector<dcard>& cards, TString selection, bool forStudies = false) {
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

  //Names of the different background categories
  TString top   = "Top";
  TString dy_tt = (forStudies) ? "ZJets" : "Z->#tau#tau";
  TString dy_ll = (forStudies) ? "ZJets" : "Z->ee/#mu#mu";
  TString dy    = (forStudies) ? "ZJets" : "Drell-Yan";
  TString embed = (forStudies) ? "ZJets" : "#tau#tau Embedding";
  TString wj    = (forStudies) ? "WJets" : (combineVB_) ? "Other VB" : "W+Jets";
  TString vb    = (forStudies) ? "WJets" : "Other VB";

  const int top_c = kYellow - 7;
  const int dy_ll_c = (forStudies) ? kRed - 7 : kRed - 2;
  const int dy_tt_c = (forStudies) ? kRed - 7 : kRed - 7;
  const int wj_c = kViolet - 9;
  const int vb_c = (vb == wj) ? wj_c : kMagenta - 9;

  for(int year : years_) {
    bool combineZ = !oneDY && !useUL_ && year != 2018 && (!useAMC_ || year == 2017);
    TString DYName = (useAMC_ && year != 2017) ? "DY50-amc" : "DY50";
    //card constructor:    filepath,              name,                  label,      isData,                   xsec               ,  isSignal,year,  color,   combine extension samples
    cards.push_back(dcard("SingleAntiToptW"    , "SingleAntiToptW"    , top.Data()  , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year, top_c));
    cards.push_back(dcard("SingleToptW"        , "SingleToptW"        , top.Data()  , false, xs.GetCrossSection("SingleToptW"        ), false, year, top_c));
    cards.push_back(dcard("SingleAntiToptChannel", "SingleAntiToptChannel", top.Data()  , false, xs.GetCrossSection("SingleAntiToptChannel"), false, year, top_c));
    cards.push_back(dcard("SingleToptChannel"    , "SingleToptChannel"    , top.Data()  , false, xs.GetCrossSection("SingleToptChannel"    ), false, year, top_c));
    cards.push_back(dcard("ttbarToHadronic"    , "ttbarToHadronic"    , top.Data()  , false, xs.GetCrossSection("ttbarToHadronic")    , false, year, top_c));
    cards.push_back(dcard("ttbarToSemiLeptonic", "ttbarToSemiLeptonic", top.Data()  , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year, top_c));
    cards.push_back(dcard("ttbarlnu"           , "ttbarlnu"           , top.Data()  , false, xs.GetCrossSection("ttbarlnu"           ), false, year, top_c));
    if(useUL_ == 0)
    cards.push_back(dcard("WWW"                , "WWW"                , vb.Data()   , false, xs.GetCrossSection("WWW"                ), false, year, vb_c));
    cards.push_back(dcard("WZ"                 , "WZ"                 , vb.Data()   , false, xs.GetCrossSection("WZ"                 ), false, year, vb_c));
    cards.push_back(dcard("ZZ"                 , "ZZ"                 , vb.Data()   , false, xs.GetCrossSection("ZZ"                 ), false, year, vb_c));
    cards.push_back(dcard("WW"                 , "WW"                 , vb.Data()   , false, xs.GetCrossSection("WW"                 ), false, year, vb_c));
    cards.push_back(dcard("EWKWplus"           , "EWKWplus"           , vb.Data()   , false, xs.GetCrossSection("EWKWplus"           ), false, year, vb_c));
    cards.push_back(dcard("EWKWminus"          , "EWKWminus"          , vb.Data()   , false, xs.GetCrossSection("EWKWminus"          ), false, year, vb_c));
    cards.push_back(dcard("EWKZ-M50"           , "EWKZ-M50"           , vb.Data()   , false, xs.GetCrossSection("EWKZ-M50"           ), false, year, vb_c));
    cards.push_back(dcard("WGamma"             , "WGamma"             , wj.Data()   , false, xs.GetCrossSection("WGamma"             ), false, year, wj_c));
    //if splitting W+Jets into jet-binned samples, use W+Jets inclusive 0-j for 0-j, then jet-binned samples for the rest
    if(splitWJ_) {
      cards.push_back(dcard("Wlnu-0"           , "Wlnu-0"             , wj.Data()   , false, xs.GetCrossSection("Wlnu"               ), false, year, wj_c, !useUL_&&year!=2018));
      if(year != 2018 && !useUL_){
        cards.push_back(dcard("Wlnu-ext-0"     , "Wlnu-ext-0"         , wj.Data()   , false, xs.GetCrossSection("Wlnu"               ), false, year, wj_c, true));
      }
      cards.push_back(dcard("Wlnu-1J"           , "Wlnu-1J"           , wj.Data()   , false, xs.GetCrossSection("Wlnu-1J"      , year), false, year, wj_c, false));
      cards.push_back(dcard("Wlnu-2J"           , "Wlnu-2J"           , wj.Data()   , false, xs.GetCrossSection("Wlnu-2J"      , year), false, year, wj_c, false));
      cards.push_back(dcard("Wlnu-3J"           , "Wlnu-3J"           , wj.Data()   , false, xs.GetCrossSection("Wlnu-3J"      , year), false, year, wj_c, false));
      if(year != 2017) {
        cards.push_back(dcard("Wlnu-4J"         , "Wlnu-4J"           , wj.Data()   , false, xs.GetCrossSection("Wlnu-4J"      , year), false, year, wj_c, false));
      }
    } else {
      cards.push_back(dcard("Wlnu"              , "Wlnu"              , wj.Data()   , false, xs.GetCrossSection("Wlnu"               ), false, year, wj_c, !useUL_&&year!=2018));
      if(year != 2018 && !useUL_){
        cards.push_back(dcard("Wlnu-ext"        , "Wlnu-ext"          , wj.Data()   , false, xs.GetCrossSection("Wlnu"               ), false, year, wj_c, true));
      }
    }
    if(splitDY_ > 0 || useEmbed_) {
      if(!(selection == "ee" || selection == "mumu") || !useEmbed_) { //use DY MC ee/mumu for non-ee/mumu categories or if using DY MC in general
        cards.push_back(dcard((DYName+"-2").Data(), (DYName+"-2").Data(), dy_ll.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_ll_c, combineZ));
      }
      if((selection == "ee" || selection == "mumu") || !useEmbed_) {//either ee/mumu or using DY MC, in both cases need MC Z->tau tau
        cards.push_back(dcard((DYName+"-1").Data(), (DYName+"-1").Data(), dy_tt.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c, combineZ));
      }
      if(useEmbed_) {
        for(int period = 0; period < periods[year].size(); ++period) {
          TString run = periods[year][period];
          if(!(selection == "ee" || selection == "mumu")) {
            cards.push_back(dcard(("Embed-MuTau-"+run).Data(), ("Embed-MuTau-"+run).Data(), embed.Data(), false, xs.GetCrossSection("Embed-MuTau-"+run, year), false, year, dy_tt_c));
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
      cards.push_back(dcard((DYName+"-1").Data(), (DYName+"-1").Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c, combineZ));
      cards.push_back(dcard((DYName+"-2").Data(), (DYName+"-2").Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c, combineZ));
    } else { //splitDY_ < 0, assume old dataset that was never separated into tautau and ee/mumu output files
      cards.push_back(dcard(DYName.Data(), DYName.Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c, combineZ));
    }
    if(combineZ) {
      if(splitDY_ > 0 || useEmbed_) {
        if(!(selection == "ee" || selection == "mumu") || !useEmbed_) {
          cards.push_back(dcard((DYName+"-ext-2").Data(), (DYName+"-ext-2").Data(), dy_ll.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_ll_c   , true));
        }
        if(!useEmbed_ || selection == "ee" || selection == "mumu") {
          cards.push_back(dcard((DYName+"-ext-1").Data(), (DYName+"-ext-1").Data(), dy_tt.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
        }
      } else if(splitDY_ == 0) {
        cards.push_back(dcard((DYName+"-ext-1").Data(), (DYName+"-ext-1").Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
        cards.push_back(dcard((DYName+"-ext-2").Data(), (DYName+"-ext-2").Data(), dy.Data(), false, xs.GetCrossSection("DY50", year), false, year, dy_tt_c   , true));
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
    if(!forStudies) {
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
    //Add data
    if(useRunPeriodData_ == 0) { //use merged data samples
      if(selection != "etau"  && selection !="ee"  ) cards.push_back(dcard("SingleMu" , "SingleMu" , "Data", true , 1., false, year));
      if(selection != "mutau" && selection !="mumu") cards.push_back(dcard("SingleEle", "SingleEle", "Data", true , 1., false, year));
    } else { //use run-period-specific data samples
      for(int period = 0; period < periods[year].size(); ++period) {
        if(year == 2016 && ((doRunPeriod_ == 1 && period > 4) || (doRunPeriod_ == 2 && period < 5))) continue;
        if(year == 2017 && ((doRunPeriod_ == 1 && period > 2) || (doRunPeriod_ == 2 && period < 3))) continue;
        if(year == 2018 && ((doRunPeriod_ == 1 && period > 1) || (doRunPeriod_ == 2 && period < 2))) continue;
        TString muon_name = "SingleMuon-" + periods[year][period];
        TString electron_name = "SingleElectron-" + periods[year][period];
        if(selection != "etau"  && selection!="ee"  ) cards.push_back(dcard(muon_name.Data()    , muon_name.Data()    , "Data", true , 1., false, year));
        if(selection != "mutau" && selection!="mumu") cards.push_back(dcard(electron_name.Data(), electron_name.Data(), "Data", true , 1., false, year));
      }
    }
  } //end years loop


  TString selection_dir = (leptonic_tau) ? "emu" : selection;
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
    if(correctEmbed_ && cards[index].name_.Contains("Embed")) {
      //Open the file to access the event count histogram
      TFile* f = TFile::Open(cards[index].filename_.Data(), "READ");
      if(!f) continue;
      TH1* events = (TH1*) f->Get("events");
      if(!events) {std::cout << "Event histogram not found in " << f->GetName() << std::endl; f->Close(); continue;}
      const double nevt = events->GetBinContent(1);
      const double ngen = xs.GetGenNumber(cards[index].name_, cards[index].year_);
      if(ngen < 1) {f->Close(); continue;}
      if(ngen > nevt) { //only apply a correction if ngen/nevt make sense
        //notify if more than a 5% correction
        std::cout << "Dataset " << cards[index].name_.Data() << ", " << cards[index].year_ << ", has gen = "
                  << ngen << " and event count = " << nevt << " --> applying " << ngen/nevt << " correction\n";
        cards[index].xsec_ *= ngen/nevt;
      }
      f->Close();
    }
  } //end file name loop
}

#endif
