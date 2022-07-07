//make the combine limit plot by year
#inclue "make_combine_limit_plot.C"

int make_combine_limit_plot_years(int set = 8, TString selection = "zmutau",
                                    bool processCards = true,
                                    bool doNoSys = false,
                                    bool doObs = false) {
  cout << __func__ << ": Not yet implemented!\n";
  // vector<TString> cards = {Form("%s_%i%s"      , selection.Data(),             set, (doNoSys) ? "_nosys" : ""),
  //                          Form("%s_%s_%i%s"   , selection.Data(), lep.Data(), set, (doNoSys) ? "_nosys" : ""),
  //                          Form("total_%s_%i%s", selection.Data(),             set, (doNoSys) ? "_nosys" : "")};
  // vector<TString> labels = {"Hadronic", "Leptonic", "Total"};
  // return make_combine_limit_plot(cards, labels, "cat", set, selection, years, processCards, doObs);
  return 0;
}
