//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"

int make_combine_limit_plot_bemu(vector<int> sets = {8}, TString selection = "zemu",
                                 vector<int> years = {2016, 2017, 2018},
                                 bool processCards = true,
                                 bool doNoSys = false,
                                 bool doObs = false) {
  scale_ = (selection.Contains("h")) ? 1.e-4 : 1.e-7;
  int status = 0;
  vector<config_t> cards;
  int index = 1;
  for(int set : sets) {
    config_t card(Form("bemu_%s_%i", selection.Data(), set), Form("Cat %i", index), {set}, years, scale_);
    card.rmin_ = 0.; card.rmax_ = 20.;
    cards.push_back(card);
    ++index;
  }

  //Add combined category
  if(sets.size() > 1) {
    TString set_string = Form("%i", sets[0]);
    for(int i = 1; i < sets.size(); ++i) set_string += Form("_%i", sets[i]);
    cards.push_back(config_t(Form("bemu_%s_%s", selection.Data(), set_string.Data()), "Combined", sets, years, scale_));
  }

  TString tag = "cat";
  status += make_combine_limit_plot_general(cards, tag, selection, processCards, doNoSys, doObs);
  return status;
}
