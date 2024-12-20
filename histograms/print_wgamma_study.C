//Print plots comparing using vs. not using W+gamma MC
#include "dataplotter_clfv.C"

int print_wgamma_study(TString selection = "mutau_e", vector<int> years = {2016,2017,2018}) {
  //setup the datacards
  years_         = years;
  hist_tag_      = "clfv";
  useEmbed_      = (selection.Contains("tau")) ? 1 : 0;
  drawStats_     = 1; //data and MC total integrals
  doStatsLegend_ = 1; //process yields in the legend
  higgsBkg_      = 1; //add Higgs backgrounds

  useEWK_    = false; //Add EWK samples
  combineVB_ =    1; //separate W+jets/W+gamma from WW/WZ/WWW etc.


  //Process with then without W+gamma MC
  int status(0);
  for(int iloop = 0; iloop < 2; ++iloop) {
    useWG_ = iloop; //turn on/off W+gamma MC inclusion

    //initialize the plotter
    if(nanoaod_init(selection.Data(),
                    (iloop == 0) ? "nanoaods_unsplit_wgamma" : "nanoaods_split_wgamma",
                    (iloop == 0) ? "unsplit_wgamma"          : "split_wgamma")) return -1;


    //setup the histogram sets of interest
    vector<int> sets = {8, 25, 1008, 1025};

    for(int set : sets) {
      status += print_mass         (set);
      status += print_mt           (set);
      status += print_mt_over_m    (set);
      status += print_pt           (set);
      status += print_met          (set);
      status += print_mva          (set);
      dataplotter_->print_stack(PlottingCard_t("lepdeltar"      , "event", set+get_offset(), 1, 1,-1));
      dataplotter_->logY_ = 1;
      dataplotter_->print_stack(PlottingCard_t("ngenphotons"    , "event", set+get_offset(), 1, 0, 5));
      dataplotter_->print_stack(PlottingCard_t("ngenhardphotons", "event", set+get_offset(), 1, 0, 5));
      dataplotter_->print_stack(PlottingCard_t("eleconvveto"    , "lep"  , set+get_offset(), 1, 0, 2));
      dataplotter_->print_stack(PlottingCard_t("elelosthits"    , "lep"  , set+get_offset(), 1, 0, 5));
    }
  }

  return status;
}
