//Print plots for e-mu trigger study
#include "dataplotter_clfv.C"

int print_emu_trig(vector<int> years = {2016}) {
  //setup the datacards
  years_     = years;
  hist_tag_  = "clfv";
  useEmbed_  = 1;
  drawStats_ = 1;
  // //FIXME: Remove run restriction
  // doRunPeriod_ = 1;
  // runs_ = {"B","C", "D", "E", "F"};

  int status = 0;

  TString tag = "";
  for(auto run : runs_) tag += run;
  if(useEmbed_) tag += "_emb";

  //etau_mu without e-mu trigger
  useMuonEGData_ = 0;
  status += nanoaod_init("etau_mu", "nanoaods_no_emu_trig", "no_emu_trig"+tag);
  if(status) return status;
  dataplotter_->draw_signal_yield_ = 1;
  print_basic_debug_plots(false, false, false, false, false, {8}); print_mva(8);

  //mutau_e without e-mu trigger
  useMuonEGData_ = 0;
  status += nanoaod_init("mutau_e", "nanoaods_no_emu_trig", "no_emu_trig"+tag);
  if(status) return status;
  dataplotter_->draw_signal_yield_ = 1;
  print_basic_debug_plots(false, false, false, false, false, {8}); print_mva(8);

  //etau_mu with e-mu trigger
  useMuonEGData_ = 1;
  status += nanoaod_init("etau_mu", "nanoaods_emu_trig", "emu_trig"+tag);
  if(status) return status;
  dataplotter_->draw_signal_yield_ = 1;
  print_basic_debug_plots(false, false, false, false, false, {8}); print_mva(8);

  //mutau_e with e-mu trigger
  useMuonEGData_ = 1;
  status += nanoaod_init("mutau_e", "nanoaods_emu_trig", "emu_trig"+tag);
  if(status) return status;
  dataplotter_->draw_signal_yield_ = 1;
  print_basic_debug_plots(false, false, false, false, false, {8}); print_mva(8);

  return status;
}
