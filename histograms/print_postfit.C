//Print the MVA variable plots post fit
#include "dataplotter_clfv.C"

int print_postfit(TString selection = "mutau",
                  vector<int> years = {2016,2017,2018},
                  TString out_dir = "postfit", TString hist_dir = "nanoaods_postfit",
                  bool do_systematics = false, bool add_stats = false, bool mc_dy = false) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (mc_dy || selection == "emu") ? 0 : 2;
  drawStats_     = add_stats; //data and MC total integrals
  doStatsLegend_ = add_stats; //process yields in the legend

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  // dataplotter_->verbose_ = 1;

  //setup the histogram sets of interest
  vector<int> sets;
  if(selection_ == "emu") sets = {8,11,12,13};
  else if(selection_ == "mumu" || selection_ == "ee") sets = {8};
  else sets = {8,25,50,51,52,53};

  //print the MVA variable histograms
  status = print_bdt_variable_plots(sets, do_systematics);
  return status;
}
