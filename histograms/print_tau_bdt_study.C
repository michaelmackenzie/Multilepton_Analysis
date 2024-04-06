//Print plots debugging low score issues in hadronic tau channel BDTs
#include "dataplotter_clfv.C"

int print_tau_bdt_study(TString out_dir = "tau_bdt_study", TString hist_dir = "nanoaods_mttwooverm_no_cut",
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
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  //setup the histogram sets of interest
  vector<int> sets = {8, 25, 29};
  if(selection.Contains("_")) sets = {8};

  //print the relevant plots for each set
  for(int set : sets) {
    if(set == 29) blind_ = false;
    else          blind_ = true ;
    status += print_mass           (set);
    status += print_collinear_mass (set, false, 200.);
    status += print_leppt          (set);
    status += print_pt             (set);
    status += print_ptratio        (set);
    status += print_lep_beta       (set);
    status += print_lepeta         (set);
    status += print_eta            (set);
    status += print_deltaphi       (set);
    status += print_met            (set);
    status += print_mt             (set);
    status += print_mt_over_m      (set);
    status += print_lep_metdeltaphi(set);
    status += print_jet            (set);
    status += print_mva            (set);
  }
  blind_ = true;

  //print the bdts for all regions
  status += print_mva(25);
  status += print_mva(26);
  status += print_mva(27);
  if(!selection.Contains("_")) status += print_mva(28);

  return status;
}
