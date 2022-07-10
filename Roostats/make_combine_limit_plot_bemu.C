//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot.C"

int make_combine_limit_plot_bemu(vector<int> sets = {8}, TString selection = "zemu",
                                 vector<int> years = {2016, 2017, 2018},
                                 bool processCards = true,
                                 bool doBkgSys = true,
                                 bool doObs = false) {
  int status = 0;
  vector<TString> cards, labels;
  int index = 0;
  for(int set : sets) {
    cards.push_back(Form("bemu_%s_%i%s", selection.Data(), set, (doBkgSys) ? "" : "_nosys"));
    labels.push_back(Form("Cat %i", index + 1));
    ++index;
  }

  //Add combined category
  if(sets.size() > 1) {
    TString set_string = Form("%i", sets[0]);
    for(int i = 1; i < sets.size(); ++i) set_string += Form("_%i", sets[i]);
    cards.push_back(Form("bemu_%s_%s%s", selection.Data(), set_string.Data(), (doBkgSys) ? "" : "_nosys"));
    labels.push_back("Combined");
  }

  status += make_combine_limit_plot(cards, labels, (doBkgSys) ? "cat" : "nosys", sets, selection, years, processCards, doObs);
  return status;
}
