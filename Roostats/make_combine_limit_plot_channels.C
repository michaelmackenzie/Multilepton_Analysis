//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_channels(vector<int> sets_had = {8}, vector<int> sets_lep = {8},
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
  configs.push_back(config_t(Form("mva_%s_%s_%s"                 , selection.Data(),             set_had_s.Data()                  , year_s.Data()), "Hadronic", sets_had, years, scale));
  configs.push_back(config_t(Form("mva_%s_%s_%s_%s"              , selection.Data(), lep.Data(), set_lep_s.Data()                  , year_s.Data()), "Leptonic", sets_lep, years, scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%s", selection.Data()            , set_had_s.Data(), set_lep_s.Data(), year_s.Data()), "Combined", sets_had, years, scale));

  TString tag = (doNoSys) ? "cat_nosys" : "cat";
  tag += Form("_had_%s_lep_%s", set_had_s.Data(), set_lep_s.Data());
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
