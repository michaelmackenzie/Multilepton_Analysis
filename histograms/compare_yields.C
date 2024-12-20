//Compare two sets/histograms yields
#include "dataplotter_clfv.C"
void print_comparison(TH1* h1, TH1* h2) {
  const double i1 = h1->Integral();
  const double i2 = h2->Integral();
  printf("%-20s: %9.2f %9.2f: %5.3f\n", h1->GetTitle(), i1, i2, i1/i2);
}

int compare_yields(TString selection = "emu",
                   vector<int> years = {2016,2017,2018},
                   TString hist_dir_1 = "nanoaods_emu_dev", int set_1 = 8,
                   TString hist_dir_2 = "nanoaods_no_displacement_cut", int set_2 = 8
                   ) {

  //setup the datacards
  years_         = years;
  hist_tag_      = "clfv";
  useEmbed_      = (selection == "emu") ? 0 : 1;
  drawStats_     = 0; //data and MC total integrals
  doStatsLegend_ = 0; //process yields in the legend
  combineVB_     = 1; //combine W+jets and other VB
  combineWJ_     = 1; //combine W+jets N(jets) bins
  combineWG_     = 1; //combine W+gamma with W+jets

  //initialize the first plotter
  cout << "Initializing the first dataset\n";
  hist_dir_      = hist_dir_1;
  int status = nanoaod_init(selection.Data(), hist_dir_, "test");
  if(status) return status;

  DataPlotter* d1 = dataplotter_;
  dataplotter_ = nullptr;

  //initialize the second plotter
  cout << "Initializing the second dataset\n";
  hist_dir_      = hist_dir_2;
  status = nanoaod_init(selection.Data(), hist_dir_, "test");
  if(status) return status;

  DataPlotter* d2 = dataplotter_;
  dataplotter_ = nullptr;

  d1->include_empty_ = 1;
  d2->include_empty_ = 1;

  set_1 += get_offset();
  set_2 += get_offset();

  // Get the distributions
  TH1*      data_1 = d1->get_data  ("lepm", "event", set_1, nullptr, "set_1");
  TH1*    signal_1 = d1->get_signal("lepm", "event", set_1, nullptr, "set_1")[0];
  THStack* stack_1 = d1->get_stack ("lepm", "event", set_1, nullptr, "set_1");
  TH1*       bkg_1 = (TH1*) stack_1->GetStack()->Last()->Clone("bkg_1");
  TH1*      data_2 = d2->get_data  ("lepm", "event", set_2, nullptr, "set_2");
  TH1*    signal_2 = d2->get_signal("lepm", "event", set_2, nullptr, "set_2")[0];
  THStack* stack_2 = d2->get_stack ("lepm", "event", set_2, nullptr, "set_2");
  TH1*       bkg_2 = (TH1*) stack_2->GetStack()->Last()->Clone("bkg_2");

  bkg_1->SetTitle("Total Bkg."); bkg_2->SetTitle("Total Bkg.");

  printf("%-20s: %9s %9s: Ratio\n", "Process", "Set 1", "Set 2");
  print_comparison(data_1, data_2);
  print_comparison(signal_1, signal_2);
  print_comparison(bkg_1, bkg_2);

  return status;
}
