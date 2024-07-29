//Print the systematic MVA plots
#include "dataplotter_clfv.C"

int print_mva_systematics(TString out_dir = "nanoaods_mva", TString hist_dir = "nanoaods_mva_dev",
                          TString selection = "mutau", vector<int> years = {2016,2017,2018},
                          bool skip_systematics = false, bool add_stats = false, bool mc_dy = false,
                          bool all_mva_versions = false) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (mc_dy) ? 0 : (selection.Contains("tau")) ? 1 : 0;
  drawStats_     = add_stats; //data and MC total integrals
  doStatsLegend_ = add_stats; //process yields in the legend

  //FIXME: Remove run period
  // runs_ = {"A"};

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  //setup the histogram sets of interest
  vector<int> sets;
  if(selection_ == "emu" || selection_ == "mumu" || selection_ == "ee") sets = {11,12,13};
  else if(selection_.Contains("_")) sets = {25,26,27};
  else sets = {25,26,27,28};

  //print the MVA histograms
  status += print_mva (8, false, all_mva_versions); //inclusive selection, without systematics
  status += print_mass(8, true); //for reference
  for(int set : sets) {
    status += print_mass(set, true); //for reference
    status += print_mva(set, !skip_systematics && selection.Contains("tau"), all_mva_versions); //only add systematics for tau channels
  }

  return status;
}
