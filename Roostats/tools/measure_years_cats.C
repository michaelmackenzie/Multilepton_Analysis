//make the combine limit plot by year and category
#include "measurement_consistency.C"

int measure_years_cats(vector<int> had_sets = {8}, vector<int> lep_sets = {8},
                       TString selection = "zmutau",
                       bool processCards = true, TString card_tag = "", TString fig_tag = "") {

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
    card = Form("mva_total_%s_had_%s_lep_%s%s_%i", selection.Data(),  had_set_s.Data(), lep_set_s.Data(), card_tag.Data(), year);
    configs.push_back(config_t(card.Data(), Form("%i Tot", year), had_sets, {year} , scale));
  }
  //add Run-II datacards
  TString card = Form("mva_%s_%s%s_2016_2017_2018", selection.Data(), had_set_s.Data(), card_tag.Data());
  configs.push_back(config_t(card.Data(), "Run-II Had", had_sets, {2016,2017,2018}, scale));
  card = Form("mva_%s%s_%s%s_2016_2017_2018", selection.Data(), lep.Data(), lep_set_s.Data(), card_tag.Data());
  configs.push_back(config_t(card.Data(), "Run-II Lep", lep_sets, {2016,2017,2018}, scale));
  card = Form("mva_total_%s_had_%s_lep_%s%s_2016_2017_2018", selection.Data(),  had_set_s.Data(), lep_set_s.Data(), card_tag.Data());
  configs.push_back(config_t(card.Data(), "Run-II Tot", had_sets, {2016,2017,2018} , scale));

  TString tag = "years_cats";
  tag += Form("_had_%s_lep_%s", had_set_s.Data(), lep_set_s.Data());
  tag += card_tag;
  tag += fig_tag;
  return measurement_consistency(configs, tag, selection, processCards);
}
