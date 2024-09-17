//make the combine limit plot by mass region
#include "measurement_consistency.C"

bool include_high_mass_ = false;

int measure_masses(TString selection = "zmutau",
                   vector<int> had_sets = {25,26,27,28}, vector<int> lep_sets = {25,26,27},
                   bool processCards = true, TString card_tag = "", TString fig_tag = "v09j_unblinded") {

  blinding_offset_ = false;
  add_asimov_      = false;
  add_values_      = false;
  force_best_fit_  =  true;
  best_fit_        = 8.78e-6;

  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;
  TString year_s = "2016_2017_2018";
  TString had_set_s = Form("%i", had_sets[0]);
  for(int i = 1; i < had_sets.size(); ++i) had_set_s += Form("_%i", had_sets[i]);
  TString lep_set_s = Form("%i", lep_sets[0]);
  for(int i = 1; i < lep_sets.size(); ++i) lep_set_s += Form("_%i", lep_sets[i]);
  TString lep = (selection.EndsWith("mutau")) ? "_e" : "_mu";
  if(card_tag != "") card_tag = "_" + card_tag;
  if(fig_tag  != "") fig_tag  = "_" + fig_tag ;
  vector<config_t> configs;
  const char* had_title = Form("Z#rightarrow%s#tau_{h}" , (selection.EndsWith("mutau")) ? "#mu" : "e");
  const char* lep_title = Form("Z#rightarrow%s#tau_{%s}", (selection.EndsWith("mutau")) ? "#mu" : "e", (selection.EndsWith("mutau")) ? "e" : "#mu");
  for(int set : had_sets) {
    if(!include_high_mass_ && set == 26) continue;
    TString card = Form("mva_%s_%i%s_2016_2017_2018", selection.Data(), set, card_tag.Data());
    const char* mass = (set == 25) ? "central" : (set == 26) ? "high" : (set == 27) ? "low" : "Z->ll";
    const char* title = Form("%s %s", had_title, mass);
    float r_range = (set == 25) ? 50. : (set == 26) ? 1000. : 100.;
    configs.push_back(config_t(card.Data(), title, {set}, {2016,2017,2018}, scale, -r_range, r_range));
  }
  for(int set : lep_sets) {
    if(!include_high_mass_ && set == 26) continue;
    TString card = Form("mva_%s%s_%i%s_2016_2017_2018", selection.Data(), lep.Data(), set, card_tag.Data());
    const char* mass = (set == 25) ? "central" : (set == 26) ? "high" : "low";
    const char* title = Form("%s %s", lep_title, mass);
    float r_range = (set == 25) ? 50. : (set == 26) ? 10000. : 100.;
    configs.push_back(config_t(card.Data(), title, {set}, {2016,2017,2018}, scale, -r_range, r_range));
  }

  TString tag = "masses";
  if(!include_high_mass_) tag += "_low";
  tag += Form("_had_%s_lep_%s", had_set_s.Data(), lep_set_s.Data());
  tag += card_tag;
  tag += fig_tag;
  return measurement_consistency(configs, tag, selection, processCards);
}
