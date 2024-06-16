 //Print plots for debugging embedding MET
#include "dataplotter_clfv.C"

int print_embed_met_debug(TString out_dir = "embed_met", TString hist_dir = "nanoaods_embed_met",
                          TString selection = "mumu", vector<int> years = {2018},
                          bool use_dy_mc = false, bool do_es_study = false) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (use_dy_mc) ? 0 : 2;
  drawStats_     = 1; //data and MC total integrals
  doStatsLegend_ = 1; //process yields in the legend
  higgsBkg_      = 1; //add Higgs backgrounds


  TString tag = "";
  if(runs_.size() > 0) {
    tag += "_";
    for(auto run : runs_) tag += run;
  }
  if(use_dy_mc) tag += "_dy_mc";

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir + tag);
  if(status) return status;

  dataplotter_->ratio_plot_min_ = 0.8;
  dataplotter_->ratio_plot_max_ = 1.2;

  //setup the histogram sets of interest
  vector<int> sets = {8};
  if     (selection == "mumu")  sets = {8}; //30,31,32,33,34,35,36,37,38,39,40,41,42};
  else if(selection.EndsWith("tau")) sets = {8, 25, 29, 45, 46, 47, 48, 1008, 2008}; //,30,31,32,33,34,35,36,38,39,40,41,42,43};
  else if(selection.Contains("tau")) sets = {8,25,29};

  if(selection != "mumu" && selection != "ee") { //print the BDT score in each mass category
    const int max_set = (selection.Contains("_")) ? 27 : 28;
    for(int set = 25; set <= max_set; ++set) status += print_mva(set);
  }

  //print debug histograms for each set
  for(int set : sets) {
    if(do_es_study) {status += print_lep_es_study(set); continue;}
    if(selection != "mumu" && selection != "ee") status += print_mva(set);
    else if(selection == "mumu" || selection == "ee") status += print_mva(set, false, false, 1);
    status += print_met            (set);
    status += print_mt             (set);
    status += print_mt_over_m      (set);
    status += print_lep_metdeltaphi(set);
    status += print_mass           (set);
    status += print_pt             (set);
    if(set == 8 || set == 25 || set == 29) { //more histograms for the nominal selection
      status += print_eta            (set);
      status += print_leppt          (set);
      status += print_met_dot_lep    (set);
      status += print_met_phi        (set);
      status += print_run            (set);
      status += print_jet            (set);
      status += print_lepeta         (set);
      status += print_lepphi         (set);
      status += print_collinear_mass (set);
      status += print_met_over_pt    (set);
      status += print_ptratio        (set);
      status += print_deltaphi       (set);
      status += print_lep_beta       (set);
      status += print_lep_gen_info   (set);
      status += print_figure("metoverm", "event", set, 1, 0., 2., 0);
      status += print_figure("npv", "event", set, 1, 0., 60., 0);
    }
  }

  return status;
}
