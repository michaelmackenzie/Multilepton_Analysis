//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_channels(int set = 8, TString selection = "zmutau",
                                     vector<int> years = {2016, 2017, 2018},
                                     bool processCards = true,
                                     bool doNoSys = false,
                                     bool doObs = false) {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;

  TString year_s = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);

  TString lep = (selection.Contains("mutau")) ? "e" : "mu";

  vector<config_t> configs;
  configs.push_back(config_t(Form("mva_%s_%i_%s"      , selection.Data(),             set, year_s.Data()), "Hadronic", {set}, years, scale));
  configs.push_back(config_t(Form("mva_%s_%s_%i_%s"   , selection.Data(), lep.Data(), set, year_s.Data()), "Leptonic", {set}, years, scale));
  configs.push_back(config_t(Form("mva_total_%s_%i_%s", selection.Data(),             set, year_s.Data()), "Combined", {set}, years, scale));
  return make_combine_limit_plot_general(configs, (doNoSys) ? "cat_nosys" : "cat", selection, processCards, doNoSys, doObs);
}
