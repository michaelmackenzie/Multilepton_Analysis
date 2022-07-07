//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot.C"

int make_combine_limit_plot_channels(vector<int> sets = {8}, TString selection = "zmutau",
                                     vector<int> years = {2016, 2017, 2018},
                                     bool processCards = true,
                                     bool doNoSys = false,
                                     bool doObs = false) {

  TString set_string = Form("%i", sets[0]);
  for(int i = 1; i < sets.size(); ++i) set_string += Form("_%i", sets[i]);
  TString lep = (selection.Contains("mutau")) ? "e" : "mu";
  vector<TString> cards = {Form("mva_%s_%s%s"      , selection.Data(),             set_string.Data(), (doNoSys) ? "_nosys" : ""),
                           Form("mva_%s_%s_%s%s"   , selection.Data(), lep.Data(), set_string.Data(), (doNoSys) ? "_nosys" : ""),
                           Form("mva_total_%s_%s%s", selection.Data(),             set_string.Data(), (doNoSys) ? "_nosys" : "")};
  vector<TString> labels = {"Hadronic", "Leptonic", "Combined"};
  return make_combine_limit_plot(cards, labels, (doNoSys) ? "nosys_cat" : "cat", sets, selection, years, processCards, doObs);
}
