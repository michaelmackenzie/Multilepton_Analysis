//Script to process hard-coded combine cards and plot them
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_fixed(TString selection = "zmutau",
                                  bool processCards = true,
                                  bool doNoSys = false,
                                  bool doObs = false) {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;


  TString lep = (selection.Contains("mutau")) ? "e" : "mu";

  vector<config_t> configs;
  configs.push_back(config_t(Form("mva_%s_25_2016"               , selection.Data()), "2016 Sig", {25}, {2016}, scale));
  configs.push_back(config_t(Form("mva_%s_%s_%i_%s"              , selection.Data(), lep.Data(), set     , year_s.Data()), Form("%i: Lep" , set), {set}, years, scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%i_lep_%i_%s", selection.Data()            , set, set, year_s.Data()), Form("%i: Comb", set), {set}, years, scale));

  TString tag = (doNoSys) ? "fixed_nosys" : "fixed";
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
