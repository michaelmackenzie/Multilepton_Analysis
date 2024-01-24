//Script to process each individual channel and some combined channels
#include "make_combine_limit_plot_general.C"

bool do_low_bdt_data_ = true; //assume the limits are for the low BDT score region of data --> for validating backgrounds

int make_combine_limit_plot_all_channels(vector<int> sets_had = {8}, vector<int> sets_lep = {8},
                                     TString selection = "zmutau",
                                     vector<int> years = {2016, 2017, 2018},
                                     bool processCards = true,
                                     bool doNoSys = false,
                                     bool doObs = false) {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;

  TString year_s = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);
  TString set_had_s = Form("%i", sets_had[0]);
  for(int i = 1; i < sets_had.size(); ++i) set_had_s += Form("_%i", sets_had[i]);
  TString set_lep_s = Form("%i", sets_lep[0]);
  for(int i = 1; i < sets_lep.size(); ++i) set_lep_s += Form("_%i", sets_lep[i]);

  TString lep = (selection.Contains("mutau")) ? "e" : "mu";

  vector<config_t> configs;
  //add each individual hadronic channel, all years
  for(int set : sets_had) {
    configs.push_back(config_t(Form("mva_%s_%i_%s", selection.Data(), set, year_s.Data()), Form("Had %i", set), {set}, years, scale));
    if((set != 8 && set != 25) || do_low_bdt_data_) {
      configs.back().rmin_ = -5000.;
      configs.back().rmax_ =  5000.;
    }
  }
  //add each individual leptonic channel, all years
  for(int set : sets_lep) {
    configs.push_back(config_t(Form("mva_%s_%s_%i_%s", selection.Data(), lep.Data(), set, year_s.Data()), Form("Lep %i", set), {set}, years, scale));
    if((set != 8 && set != 25) || do_low_bdt_data_) {
      configs.back().rmin_ = -500.;
      configs.back().rmax_ =  500.;
    }
  }
  if(!do_low_bdt_data_) {
    //total hadronic/leptonic categories
    configs.push_back(config_t(Form("mva_%s_%s_%s"                 , selection.Data(),             set_had_s.Data()                  , year_s.Data()), "Hadronic", sets_had, years, scale));
    if(do_low_bdt_data_) {
      configs.back().rmin_ = -500.;
      configs.back().rmax_ =  500.;
    }
    configs.push_back(config_t(Form("mva_%s_%s_%s_%s"              , selection.Data(), lep.Data(), set_lep_s.Data()                  , year_s.Data()), "Leptonic", sets_lep, years, scale));
    if(do_low_bdt_data_) {
      configs.back().rmin_ = -500.;
      configs.back().rmax_ =  500.;
    }
    //total fit
    configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%s", selection.Data()            , set_had_s.Data(), set_lep_s.Data(), year_s.Data()), "Combined", sets_had, years, scale));
    if(do_low_bdt_data_) {
      configs.back().rmin_ = -500.;
      configs.back().rmax_ =  500.;
    }
  }

  //tag for the output figure
  TString tag = (doNoSys) ? "all_cat_nosys" : "all_cat";
  if(do_low_bdt_data_) tag += "_low_bdt";
  tag += Form("_had_%s_lep_%s", set_had_s.Data(), set_lep_s.Data());

  //turn off the expected limit text
  add_values_ = false;

  //call the general plotting function
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
