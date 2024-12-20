//Print background rates
#include "dataplotter_clfv.C"

int print_bkg_rates(TString selection = "mutau",
                    vector<int> years = {2016,2017,2018},
                    TString out_dir = "test",
                    TString hist_dir = "nanoaods_postfit") {

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (selection == "emu") ? 0 : 1;
  drawStats_     = 0; //data and MC total integrals
  doStatsLegend_ = 0; //process yields in the legend

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  auto data = dataplotter_->get_data("lepm", "event", 8 + get_offset());
  if(!data) return 1;
  printf("%-20s: %.1f\n", "Data", data->Integral(0, data->GetNbinsX()+1));
  delete data;
  auto stack = dataplotter_->get_stack("lepm", "event", 8 + get_offset());
  if(!stack || stack->GetNhists() == 0) return 2;
  float nbkg = 0.;
  for(auto o : *(stack->GetHists())) {
    auto h = (TH1*) o;
    float n = h->Integral(0, h->GetNbinsX()+1);
    printf("%-20s: %.1f\n", h->GetTitle(), n);
    nbkg += n;
    delete o;
  }
  printf("%-20s: %.1f\n", "Total bkg", nbkg);
  return status;
}
