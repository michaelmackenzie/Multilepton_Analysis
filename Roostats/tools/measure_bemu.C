//make the combine limit plot by BDT score category
#include "measurement_consistency.C"

int measure_bemu(TString selection = "zemu",
                 vector<int> sets = {11,12,13},
                 bool processCards = true,
                 TString card_tag = "", TString fig_tag = "v02") {

  blinding_offset_ = false;
  add_asimov_      = false;
  add_values_      = false;
  add_command_     = "";

  const double scale = (selection.Contains("h")) ? 1.e-5 : 1.e-7;
  TString year_s = "2016_2017_2018";
  TString set_s = Form("%i", sets[0]);
  for(int i = 1; i < sets.size(); ++i) set_s += Form("_%i", sets[i]);

  if(card_tag != "") card_tag = "_" + card_tag;
  if(fig_tag  != "") fig_tag  = "_" + fig_tag ;
  vector<config_t> configs;
  //add Run-II datacards by category
  for(int set : sets) {
    TString card = Form("bemu_%s_%i%s", selection.Data(), set, card_tag.Data());
    TString category = "Cat";
    if(set == 11) category = "Low score";
    if(set == 12) category = "Med. score";
    if(set == 13) category = "High score";
    configs.push_back(config_t(card.Data(), category, sets, {2016,2017,2018}, scale));
  }
  //add the total fit category
  TString card = Form("bemu_%s_%s%s", selection.Data(), set_s.Data(), card_tag.Data());
  configs.push_back(config_t(card.Data(), "Combined", sets, {2016,2017,2018}, scale));

  TString tag = set_s;
  tag += card_tag;
  tag += fig_tag;
  return measurement_consistency(configs, tag, selection, processCards);
}
