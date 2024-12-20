//Print the cutflow from the SparseHistMaker histogramming
#include "dataplotter_clfv.C"

int print_emu_cutflow() {
  int status(0);
  //cutflow sets
  vector<int> sets = {20,21,22,23,24,25,26,27,28};

  //print the cutflow
  for(int set : sets) {
    if(set == 20) status += print_mass(set, false, 50., 130.); //force a wide window before the mass cut
    else          status += print_mass(set);
    if(set == 27 || set == 28) status += print_dxyz(set); //only print the lepton displacement before/after the cuts
    status += print_met(set);
    status += print_mt(set);
    status += print_pt(set);
    status += print_mva(set, false, false);
  }
  return status;
}

int print_tau_cutflow() {
  int status(0);
  //cutflow sets
  vector<int> sets = {20,8,21,22,23,24,25,26,27,30};
  useLepTauSet_ = 0;
  //print the cutflow
  for(int set : sets) {
    if(set == 20) dataplotter_->print_stack("eventweight","event",set+get_offset());
    // status += print_mass(set);
    // status += print_met(set);
    // status += print_mt(set);
    status += print_mt_over_m(set);
    // status += print_pt(set);
    // status += print_ptdiff(set);
    // status += print_eta(set);
    status += print_mva(set);
  }
  return status;
}

int print_cutflow(TString selection = "emu", TString hist_dir = "nanoaods_sparse", vector<int> years = {2016,2017,2018}) {
  //setup the datacards
  years_     = years;
  hist_dir_  = hist_dir;
  hist_tag_  = "sparse";
  useEmbed_  = (selection.Contains("tau")) ? 1 : 0;
  drawStats_ = 0; //don't include integrals

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, "cutflow");
  if(status) return status;

  //print the cut flow histograms
  if(selection == "emu")        status += print_emu_cutflow();
  if(selection.Contains("tau")) status += print_tau_cutflow();

  return status;
}
