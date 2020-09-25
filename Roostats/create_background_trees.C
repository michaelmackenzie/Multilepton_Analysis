//Script to create background only trees
#include "../rootScripts/make_ztautau_background.C"

Int_t create_background_trees(int set = 8, int year = 2016, TString base = "../histograms/nanoaods_dev/") {
  backgroundOnly_ = true; //flag in make_ztautau_background
  year_ = year; //flag in make_ztautau_background
  
  make_background(set+ZTauTauHistMaker::kEMu, "emu", base);
  gSystem->Exec("[ ! -d \"background_trees\" ] && mkdir background_trees; mv *.tree background_trees;");

  return 0;
}
