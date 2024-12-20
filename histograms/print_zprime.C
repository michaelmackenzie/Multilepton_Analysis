//Print plots for the Z prime scan
#include "dataplotter_clfv.C"

int print_zprime(TString out_dir = "zprime", TString hist_dir = "nanoaods_zprime",
                 TString selection = "emu", vector<int> years = {2016,2017,2018},
                 bool do_systematics = false, bool add_stats = true, bool mc_dy = true) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = 0;
  drawStats_     = add_stats; //data and MC total integrals
  doStatsLegend_ = add_stats; //process yields in the legend
  zprime_        = 1;
  blind_         = false;

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  // dataplotter_->verbose_ = 1;

  //setup the histogram sets of interest (inclusive, 0-0.3, 0.3-0.7, and 0.7-1.0)
  vector<int> sets = {8,10,11,12,1008,1011,1012};

  //print the mass and BDT score distributions
  for(int set : sets) status += print_mass(set);
  for(int set : sets) status += print_mva (set);

  //print zoomed versions
  PlottingCard_t card("lepm", "event", 8+get_offset(), 1, 80., 110.);
  card.tag_ = "zoom";
  for(int set : sets) {
    card.set_ = set + get_offset();
    dataplotter_->logY_ = 0;
    {
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
    dataplotter_->logY_ = 1;
    {
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
  }

  // //print the MVA variable histograms
  // status = print_bdt_variable_plots(sets, do_systematics);
  return status;
}
