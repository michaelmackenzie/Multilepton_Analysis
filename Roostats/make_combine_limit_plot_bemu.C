//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"
bool do_barrel_endcap_ = true; //split fit into barrel/endcap electron categories

int make_combine_limit_plot_bemu(vector<int> sets = {8}, TString selection = "zemu",
                                 vector<int> years = {2016, 2017, 2018},
                                 bool processCards = true,
                                 bool doNoSys = false,
                                 bool doObs = false) {
  scale_ = (selection.Contains("h")) ? 1.e-4 : 1.e-7;
  int status = 0;

  add_values_ = false; //don't add expected limit values to the plot

  vector<config_t> cards;
  int index = 1;
  for(int set : sets) {
    TString name = Form("Cat %i", index);
    if     (set == 11) name = "Low score";
    else if(set == 12) name = "Med score";
    else if(set == 13) name = "High score";
    else if(set == 72) name = "B: Low score";
    else if(set == 73) name = "B: Med score";
    else if(set == 74) name = "B: High score";
    else if(set == 82) name = "E: Low score";
    else if(set == 83) name = "E: Med score";
    else if(set == 84) name = "E: High score";
    config_t card(Form("bemu_%s_%i", selection.Data(), set), name, {set}, years, scale_);
    card.rmin_ = 0.; card.rmax_ = 20.;
    cards.push_back(card);
    ++index;
    //add a total barrel category
    if(do_barrel_endcap_ && set == 74) {
      name = "Barrel comb.";
      config_t card(Form("bemu_%s_72_73_74", selection.Data()), name, {set}, years, scale_);
      card.rmin_ = 0.; card.rmax_ = 20.;
      cards.push_back(card);
    }
    //add a total endcap category
    if(do_barrel_endcap_ && set == 84) {
      name = "Endcap comb.";
      config_t card(Form("bemu_%s_82_83_84", selection.Data()), name, {set}, years, scale_);
      card.rmin_ = 0.; card.rmax_ = 20.;
      cards.push_back(card);
    }
  }

  //add a total barrel/endcap card

  //Add combined category
  std::sort(sets.begin(),sets.end());

  if(sets.size() > 1) {
    TString set_string = Form("%i", sets[0]);
    for(int i = 1; i < sets.size(); ++i) set_string += Form("_%i", sets[i]);
    cards.push_back(config_t(Form("bemu_%s_%s", selection.Data(), set_string.Data()), "Combined", sets, years, scale_));
  }

  TString tag = "cat";
  if(doNoSys) tag += "_nosys";
  if(doObs) tag += "_obs";
  if(do_barrel_endcap_) tag += "_barrel_endcap";
  if(add_values_) tag += "_vals";
  for(int year : years) tag += Form("_%i", year);
  status += make_combine_limit_plot_general(cards, tag, selection, processCards, doNoSys, doObs);
  return status;
}
