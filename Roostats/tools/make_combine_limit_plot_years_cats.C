//make the combine limit plot by year and category
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_years_cats(vector<int> sets = {8}, TString selection = "zmutau",
                                       bool processCards = true,
                                       bool doNoSys = false,
                                       bool doObs = false) {
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;
  TString year_s = "2016_2017_2018";
  TString set_s = Form("%i", sets[0]);
  for(int i = 1; i < sets.size(); ++i) set_s += Form("_%i", sets[i]);
  TString lep = (selection.EndsWith("mutau")) ? "_e" : "_mu";
  vector<config_t> configs;
  configs.push_back(config_t(Form("mva_%s_%s_%i"      , selection.Data(),             set_s.Data(), 2016         ), "2016 Had"  , sets, {2016}          , scale));
  configs.push_back(config_t(Form("mva_%s%s_%s_%i"    , selection.Data(), lep.Data(), set_s.Data(), 2016         ), "2016 Lep"  , sets, {2016}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%i", selection.Data(), set_s.Data(), set_s.Data(), 2016), "2016 Tot"  , sets, {2016}          , scale));
  configs.push_back(config_t(Form("mva_%s_%s_%i"      , selection.Data(),             set_s.Data(), 2017         ), "2017 Had"  , sets, {2017}          , scale));
  configs.push_back(config_t(Form("mva_%s%s_%s_%i"    , selection.Data(), lep.Data(), set_s.Data(), 2017         ), "2017 Lep"  , sets, {2017}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%i", selection.Data(), set_s.Data(), set_s.Data(), 2017), "2017 Tot"  , sets, {2017}          , scale));
  configs.push_back(config_t(Form("mva_%s_%s_%i"      , selection.Data(),             set_s.Data(), 2018         ), "2018 Had"  , sets, {2018}          , scale));
  configs.push_back(config_t(Form("mva_%s%s_%s_%i"    , selection.Data(), lep.Data(), set_s.Data(), 2018         ), "2018 Lep"  , sets, {2018}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%i", selection.Data(), set_s.Data(), set_s.Data(), 2018), "2018 Tot"  , sets, {2018}          , scale));
  configs.push_back(config_t(Form("mva_%s_%s_%s"      , selection.Data(),             set_s.Data(), year_s.Data()), "Run-II Had", sets, {2016,2017,2018}, scale));
  configs.push_back(config_t(Form("mva_%s%s_%s_%s"    , selection.Data(), lep.Data(), set_s.Data(), year_s.Data()), "Run-II Lep", sets, {2016,2017,2018}, scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s", selection.Data(), set_s.Data(), set_s.Data(), year_s.Data()), "Run-II Tot", sets, {2016,2017,2018}, scale));

  TString tag = (doNoSys) ? "years_cats_nosys" : "years_cats";
  tag += Form("_%s", set_s.Data());
  return make_combine_limit_plot_general(configs, tag, selection, processCards, doNoSys, doObs);
}
