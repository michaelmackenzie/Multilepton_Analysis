//Print plots for the Z prime scan
#include "dataplotter_clfv.C"

int print_zprime(TString out_dir = "zprime", TString hist_dir = "nanoaods_zprime",
                 TString selection = "emu", vector<int> years = {2016,2017,2018},
                 bool do_systematics = false, bool add_stats = false, bool mc_dy = true) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (mc_dy || selection == "emu") ? 0 : 2;
  drawStats_     = add_stats; //data and MC total integrals
  doStatsLegend_ = add_stats; //process yields in the legend
  zprime_        = 1;
  blind_         = false;

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  // dataplotter_->verbose_ = 1;

  //setup the histogram sets of interest (inclusive, 0-0.3, 0.3-0.7, and 0.7-1.0)
  vector<int> sets = {8,10,11,12};

  //print the MVA variable histograms
  status = print_bdt_variable_plots(sets, do_systematics);
  return status;
}
