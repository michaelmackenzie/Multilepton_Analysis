//Print the MVA variable plots in slices of the variables
#include "dataplotter_clfv.C"

int print_bdt_var_study(TString selection = "mutau",
                        vector<int> years = {2016,2017,2018},
                        TString out_dir = "bdt_var_study", TString hist_dir = "nanoaods_bdt_var_study",
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
  else sets = {25,50,51,52,53};

  for(int set : sets) {
    //print each slice for each variable
    std::vector<TString> vars = {"mass", "mcol", "alpha1", "alpha2", "ptratio", "mtlep", "dphitau", "dphi", "leppt", "jetpt"};
    for(int region = 2; region < 3; ++region) {
      for(auto var_y : vars) { //variable we're slicing
        //only do some variables
        if(!(var_y == "mass" || var_y == "mtlep")) continue;
        for(auto var_x : vars) { //variable examined within the slice
          auto c = dataplotter_->print_stack(var_x + "_vs_" + var_y + Form("_%i", region), "event", set);
          if(c) DataPlotter::Empty_Canvas(c);
          else ++status;
        }
        auto c = dataplotter_->print_stack("bdt_vs_" + var_y + Form("_%i", region), "event", set);
        if(c) DataPlotter::Empty_Canvas(c);
        else ++status;
      }
    }
  }
  return status;
}
