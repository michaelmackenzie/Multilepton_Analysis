//make the combine limit plot by year
#include "measurement_consistency.C"

int measure_years(TString selection = "zmutau",
                  vector<int> had_sets = {25,26,27,28}, vector<int> lep_sets = {25,26,27},
                  bool processCards = true) {

  blinding_offset_ = true;
  add_asimov_      = true;
  add_values_      = true;

  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;
  TString year_s = "2016_2017_2018";
  TString had_set_s = Form("%i", had_sets[0]);
  for(int i = 1; i < had_sets.size(); ++i) had_set_s += Form("_%i", had_sets[i]);
  TString lep_set_s = Form("%i", lep_sets[0]);
  for(int i = 1; i < lep_sets.size(); ++i) lep_set_s += Form("_%i", lep_sets[i]);
  TString lep = (selection.EndsWith("mutau")) ? "_e" : "_mu";
  vector<config_t> configs;
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%i", selection.Data(),  had_set_s.Data(), lep_set_s.Data(), 2016), "2016"  , had_sets, {2016}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%i", selection.Data(),  had_set_s.Data(), lep_set_s.Data(), 2017), "2017"  , had_sets, {2017}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%i", selection.Data(),  had_set_s.Data(), lep_set_s.Data(), 2018), "2018"  , had_sets, {2018}          , scale));
  configs.push_back(config_t(Form("mva_total_%s_had_%s_lep_%s_%s", selection.Data(), had_set_s.Data(), lep_set_s.Data(), year_s.Data()), "Run-II", had_sets, {2016,2017,2018}, scale));

  TString tag = "years";
  tag += Form("_had_%s_lep_%s", had_set_s.Data(), lep_set_s.Data());
  return measurement_consistency(configs, tag, selection, processCards);
}
