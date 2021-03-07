//Script to create background only trees
#include "../rootScripts/make_ztautau_background.C"

Int_t create_background_trees(int set = 8,
			      vector<int> years = {2016, 2017, 2018},
			      TString base = "../histograms/nanoaods_dev/",
			      TString selection = "emu",
			      bool combineLepTau = false) {
  //flags in make_ztautau_background
  backgroundOnly_ = true;
  years_ = years;
  combineLepTau_ = combineLepTau;

  //remove boson from selection
  selection.ReplaceAll("z", ""); selection.ReplaceAll("h", "");
  //leptonic tau categories use emu data
  if(selection.Contains("_")) selection = "emu";
  
  make_background(set, selection.Data(), base);
  gSystem->Exec("[ ! -d \"background_trees\" ] && mkdir background_trees; mv *.tree background_trees;");
  return 0;
}
