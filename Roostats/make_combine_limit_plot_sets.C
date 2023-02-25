//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_sets(vector<int> sets = {8}, TString selection = "zmutau",
                                     vector<int> years = {2016, 2017, 2018},
                                     bool processCards = true,
                                     bool doNoSys = false,
                                     bool doObs = false) {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;

  TString year_s = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);
  TString set_s = Form("%i", sets[0]);
  for(int i = 1; i < sets.size(); ++i) set_s += Form("_%i", sets[i]);

  TString lep = (selection.Contains("mutau")) ? "e" : "mu";

  vector<config_t> configs;
  for(int set : sets) {
    configs.push_back(config_t(Form("mva_%s_%i_%s"                 , selection.Data(),             set     , year_s.Data()), Form("%i: Had" , set), {set}, years, scale));
    configs.push_back(config_t(Form("mva_%s_%s_%i_%s"              , selection.Data(), lep.Data(), set     , year_s.Data()), Form("%i: Lep" , set), {set}, years, scale));
    // configs.push_back(config_t(Form("mva_total_%s_had_%i_lep_%i_%s", selection.Data()            , set, set, year_s.Data()), Form("%i: Comb", set), {set}, years, scale));
  }
  configs.push_back(config_t(Form("mva_%s_%s_%s"                 , selection.Data(),             set_s.Data()              , year_s.Data()), "Total Had" , sets, years, scale));
  configs.push_back(config_t(Form("mva_%s_%s_%s_%s"              , selection.Data(), lep.Data(), set_s.Data()              , year_s.Data()), "Total Lep" , sets, years, scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%s", selection.Data()            , set_s.Data(), set_s.Data(), year_s.Data()), "Total Comb", sets, years, scale));

  TString tag = (doNoSys) ? "sets_nosys" : "sets";
  tag += Form("_%s", set_s.Data());
  tag += Form("_%s", year_s.Data());
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
