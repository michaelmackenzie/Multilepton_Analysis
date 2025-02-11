//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_channels(TString selection = "zmutau",
                                     vector<int> sets_had = {25,26,27,28}, vector<int> sets_lep = {25,26,27},
                                     vector<int> years = {2016, 2017, 2018},
                                     bool processCards = false,
                                     bool doNoSys = false,
                                     bool doObs = true,
                                     TString card_tag = "",
                                     TString tag = "v09j") {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;
  add_values_ = false; //Whether to add expected limit values to the figure

  // force a specific y-axis range
  ymin_ = 0.;
  ymax_ = 45.e-6;

  if(card_tag != "") card_tag += "_";
  TString year_s = Form("%s%i", card_tag.Data(), years[0]);
  for(int i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);
  TString set_had_s = Form("%i", sets_had[0]);
  for(int i = 1; i < sets_had.size(); ++i) set_had_s += Form("_%i", sets_had[i]);
  TString set_lep_s = Form("%i", sets_lep[0]);
  for(int i = 1; i < sets_lep.size(); ++i) set_lep_s += Form("_%i", sets_lep[i]);

  TString lep = (selection.Contains("mutau")) ? "e" : "mu";

  const float r_range = (selection == "zmutau") ? 25. : 40.;
  vector<config_t> configs;
  configs.push_back(config_t(Form("mva_%s_%s_%s"                 , selection.Data(),             set_had_s.Data()                  , year_s.Data()), "Hadronic", sets_had, years, scale, -r_range, r_range));
  configs.push_back(config_t(Form("mva_%s_%s_%s_%s"              , selection.Data(), lep.Data(), set_lep_s.Data()                  , year_s.Data()), "Leptonic", sets_lep, years, scale, -r_range, r_range));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%s", selection.Data()            , set_had_s.Data(), set_lep_s.Data(), year_s.Data()), "Combined", sets_had, years, scale, -r_range, r_range));

  if(tag != "") tag += "_";
  if(card_tag != "") tag += card_tag;
  tag += (doNoSys) ? "cat_nosys" : "cat";
  tag += Form("_had_%s_lep_%s", set_had_s.Data(), set_lep_s.Data());
  if(add_values_) tag += "_vals";
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
