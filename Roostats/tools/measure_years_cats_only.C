//make the combine limit plot by year and category, without any combined categories
#include "measurement_consistency.C"

int measure_years_cats_only(TString selection = "zmutau",
                            vector<int> had_sets = {25,26,27,28}, vector<int> lep_sets = {25,26,27},
                            bool processCards = true, TString card_tag = "", TString fig_tag = "v09j_unblinded") {

  blinding_offset_ = false;
  add_asimov_      = false;
  add_values_      = true;
  force_best_fit_  = true;
  best_fit_ = (selection == "zmutau") ? 8.9e-6 : 4.0e-6;

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
  //add datacards by year
  for(int year = 2016; year <= 2018; ++year) {
    TString card = Form("mva_%s_%s%s_%i", selection.Data(), had_set_s.Data(), card_tag.Data(), year);
    configs.push_back(config_t(card.Data(), Form("%i Had", year), had_sets, {year}, scale));
    card = Form("mva_%s%s_%s%s_%i", selection.Data(), lep.Data(), lep_set_s.Data(), card_tag.Data(), year);
    configs.push_back(config_t(card.Data(), Form("%i Lep", year), lep_sets, {year}, scale));
  }

  TString tag = "years_cats_only";
  tag += Form("_had_%s_lep_%s", had_set_s.Data(), lep_set_s.Data());
  tag += card_tag;
  tag += fig_tag;
  return measurement_consistency(configs, tag, selection, processCards);
}
