//Print plots for debugging mutau_h channel changes
#include "dataplotter_clfv.C"

int print_mutau_debug(TString out_dir = "nanoaods_mva", TString hist_dir = "nanoaods_mva_dev",
                      TString selection = "mutau", vector<int> years = {2016,2017,2018}, bool use_dy_mc = false) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (selection.Contains("tau") && !use_dy_mc) ? 1 : 0;
  drawStats_     = 1; //data and MC total integrals
  doStatsLegend_ = 1; //process yields in the legend
  higgsBkg_      = 1; //add Higgs backgrounds

  //Add EWK/WGamma samples
  const bool use_ewk = true;
  useEWK_ = use_ewk;
  // useWG_  = use_ewk; // Likely already modeled in W+jets
  combineVB_     = 0; //separate W+jets from WW/WZ/WWW etc.

  //scale Z->mumu
  // zll_scale_     = 0.70f;

  // //FIXME: Remove run period
  // runs_ = {"A","B","C"};

  //turn on/off j-->tau estimate
  const bool use_misid = true;

  TString tag = "";
  if(!use_misid) tag += "_no_jtt";
  if(use_ewk) tag += "_ewk";
  if(runs_.size() > 0) {
    tag += "_";
    for(auto run : runs_) tag += run;
  }
  if(use_dy_mc) tag += "_dy_mc";
  if(zll_scale_ != 1.f) tag += Form("_zll_%.0fpc", zll_scale_*100.f);

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir + tag);
  if(status) return status;

  dataplotter_->include_misid_ = use_misid;

  //setup the histogram sets of interest
  vector<int> sets = {8, 28, 2008, 2028};

  dataplotter_->y_title_offset_ = 0.3;
  dataplotter_->data_over_mc_ = 2;
  status += print_mass(8);
  status += print_mass(1008);
  status += print_mva (1008);
  status += print_mass(2008);
  status += print_mva (2008);
  status += print_mass(3008);
  if(use_misid) dataplotter_->data_over_mc_ = 1;
  status += print_mva (25);
  status += print_mva (26);
  status += print_mva (27);
  for(int set : sets) {
    if(use_misid) {
      if(set > 2000) dataplotter_->data_over_mc_ = 2;
      else           dataplotter_->data_over_mc_ = 1;
    } else {
      dataplotter_->data_over_mc_ = 2;
    }
    status += print_mass           (set, use_misid); //add rough uncertainties to mass plots
    status += print_pt             (set);
    status += print_eta            (set);
    status += print_dm             (set);
    status += print_jet            (set);
    status += print_lep_metdeltaphi(set);
    if(set == 8 || set == 28) { //more distributions
      status += print_leppt          (set);
      status += print_collinear_mass (set);
      status += print_met            (set);
      status += print_ptratio        (set);
      status += print_deltaphi       (set);
      status += print_lep_beta       (set);
      status += print_mt             (set);
      status += print_ids            (set);
      status += print_dxyz           (set);
      status += print_mva            (set);
      status += print_run            (set);
    }
  }

  return status;
}
