//Script to create background only trees
#include "../rootScripts/make_ztautau_background.C"

Int_t create_background_trees(int set = 8, vector<int> years = {2016, 2017, 2018},
			      TString base = "../histograms/nanoaods_dev/",
			      TString selection = "emu") {
  //flags in make_ztautau_background
  backgroundOnly_ = true;
  years_ = years; 

  //remove boson from selection
  selection.ReplaceAll("z", ""); selection.ReplaceAll("h", "");
  //get set offset
  int set_offset = ZTauTauHistMaker::kEMu;
  if     (selection == "mutau") set_offset = ZTauTauHistMaker::kMuTau;
  else if(selection == "etau" ) set_offset = ZTauTauHistMaker::kETau;
  //leptonic tau categories use emu data
  if(selection.Contains("_")) selection = "emu";
  
  make_background(set+set_offset, selection.Data(), base);
  gSystem->Exec("[ ! -d \"background_trees\" ] && mkdir background_trees; mv *.tree background_trees;");
  return 0;
}
