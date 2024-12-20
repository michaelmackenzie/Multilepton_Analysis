//Print plots in BDT score regions for the tau channels
#include "dataplotter_clfv.C"

int print_tau_bdt_cats(TString out_dir = "tau_bdt_cats", TString hist_dir = "nanoaods_tau_bdt_cats",
                       TString selection = "mutau", vector<int> years = {2016,2017,2018},
                       bool mc_dy = false) {

  if(mc_dy) out_dir += "_mc_dy";

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (mc_dy) ? 0 : (selection.Contains("tau")) ? 1 : 0;
  drawStats_     = 1; //data and MC total integrals
  doStatsLegend_ = 1; //process yields in the legend

  //initialize the plotter
  // skipData_ = 1; //don't need data --> Needed for QCD/j--tau
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  //turn off data, set to plot signal/background
  dataplotter_->plot_data_ = false;
  dataplotter_->data_over_mc_ = -1;

  dataplotter_->signal_scale_     = 0.1; //make signal more comparable to background
  dataplotter_->signal_ratio_log_ =   0; //no need for log(S/B)

  //setup the histogram sets of interest
  vector<int> sets = {13};

  //print the relevant plots for each set
  for(int set : sets) {
    if(set > 8 && set < 12) blind_ = false;
    else                    blind_ = true ;
    status += print_mass           (set);
    status += print_collinear_mass (set, false, 300.);
    status += print_leppt          (set);
    status += print_pt             (set);
    status += print_ptratio        (set);
    status += print_trkpt          (set);
    status += print_lep_beta       (set);
    status += print_deltaphi       (set);
    status += print_lepeta         (set);
    status += print_eta            (set);
    status += print_deltaphi       (set);
    status += print_met            (set);
    status += print_mt             (set);
    status += print_mt_over_m      (set);
    status += print_lep_metdeltaphi(set);
    status += print_jet            (set);
    status += print_mva            (set);
    status += print_ids            (set);
    status += print_gen_flavor     (set);
    status += print_dm             (set);
    status += print_prime_sys      (set);
  }
  blind_ = true;

  return status;
}
