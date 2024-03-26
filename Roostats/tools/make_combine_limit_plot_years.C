//make the combine limit plot by year
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_years(int set = 8, TString selection = "zmutau",
                                  bool processCards = true,
                                  bool doNoSys = false,
                                  bool doObs = false) {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;
  TString year_s = "2016_2017_2018";
  vector<config_t> configs;
  configs.push_back(config_t(Form("mva_total_%s_%i_%i", selection.Data(), set, 2016         ), "2016"  , {set}, {2016}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_%i_%i", selection.Data(), set, 2017         ), "2017"  , {set},      {2017}     , scale));
  configs.push_back(config_t(Form("mva_total_%s_%i_%i", selection.Data(), set, 2018         ), "2018"  , {set},           {2018}, scale));
  configs.push_back(config_t(Form("mva_total_%s_%i_%s", selection.Data(), set, year_s.Data()), "Run-II", {set}, {2016,2017,2018}, scale));
  TString tag = (doNoSys) ? "years_nosys" : "years";
  tag += Form("_%i", set);
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
