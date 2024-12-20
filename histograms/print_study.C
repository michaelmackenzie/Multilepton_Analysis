//Print study plots
#include "dataplotter_clfv.C"

int print_study(TString selection = "mutau",
                vector<int> years = {2016,2017,2018},
                TString out_dir = "gen_met_study",
                TString hist_dir = "nanoaods_postfit") {

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (selection == "emu") ? 0 : 1;
  drawStats_     = 0; //data and MC total integrals
  doStatsLegend_ = 0; //process yields in the legend
  combineVB_     = 1; //combine W+jets and other VB
  combineWJ_     = 1; //combine W+jets N(jets) bins
  combineWG_     = 1; //combine W+gamma with W+jets

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  //setup the histogram sets of interest
  vector<int> sets = {8};

  //print histograms of interest for each set
  for(int set : sets) {
    print_gen_met(set);
    print_met    (set);
  }

  return status;
}
