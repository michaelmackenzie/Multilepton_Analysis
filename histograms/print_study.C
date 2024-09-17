//Print study plots
#include "dataplotter_clfv.C"

int print_study(TString selection = "mutau_e",
                vector<int> years = {2016,2017,2018},
                TString out_dir = "wjets_study",
                // TString hist_dir = "CLFVHistMaker_20240913_173032") {
                TString hist_dir = "nanoaods_postfit") {

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (selection == "emu") ? 0 : 1;
  drawStats_     = 0; //data and MC total integrals
  doStatsLegend_ = 0; //process yields in the legend
  combineVB_     = 0; //combine W+jets and other VB
  combineWJ_     = 1; //combine W+jets N(jets) bins
  combineWG_     = 0; //combine W+gamma with W+jets

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  dataplotter_->data_over_mc_ = 2;

  //setup the histogram sets of interest
  vector<int> sets = {8};

  //print histograms of interest for each set
  for(int set : sets) {
    for(int ioffset = 0; ioffset < 2; ++ioffset) {
      print_mass      (set + ioffset*1000);
      print_mva       (set + ioffset*1000);
      print_gen_flavor(set + ioffset*1000, true);
    }
  }

  return status;
}
