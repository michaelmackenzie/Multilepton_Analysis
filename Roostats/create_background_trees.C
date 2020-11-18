//Script to create background only trees
#include "../rootScripts/make_ztautau_background.C"

Int_t add_lum(TTree& tree, double lum) {
  bool debug = false;
  unsigned nentries = tree.GetEntriesFast();
  std::cout << "Tree has " << nentries << " to process!\n";
  Float_t weight(1.), inweight(1.);
  tree.SetBranchAddress("fulleventweight", &inweight);
  TBranch* branch = tree.Branch("fulleventweightlum", &weight, 'F');
  double avg = 0.;
  for(unsigned entry = 0; entry < nentries; ++entry) {
    tree.GetEntry(entry);
    weight = lum*inweight;
    avg += weight/nentries;
    branch->Fill();
  }
  if(debug) {
    std::cout << "Finished processing! Avg was " << avg << std::endl;
    avg = 0.;
    for(unsigned entry = 0; entry < nentries; ++entry) {
      tree.GetEntry(entry);
      avg += weight/nentries;
    }
    std::cout << "Finished processing again! Avg was " << avg << std::endl;
  }
  return 0;
}


Int_t create_background_trees(int set = 8, vector<int> years = {2016}, TString base = "../histograms/nanoaods_dev/") {
  //create a background tree for each year
  for(int year : years) {
    backgroundOnly_ = true; //flag in make_ztautau_background
    year_ = year; //flag in make_ztautau_background
  
    make_background(set+ZTauTauHistMaker::kEMu, "emu", base);
    gSystem->Exec("[ ! -d \"background_trees\" ] && mkdir background_trees; mv *.tree background_trees;");
    //Open up file to add luminosity weights
    TString file_name = Form("background_trees/background_ztautau_bkg_nano_emu_%i_%i.tree", year, set+ZTauTauHistMaker::kEMu);
    TFile* file = TFile::Open(file_name.Data(), "UPDATE");
    TTree* tree = (TTree*) file->Get("background_tree");
    double lum = 35.92e3;
    if(year == 2017) lum = 41.48e3;
    else if (year == 2018) lum = 59.74e3;
    std::cout << "---Adding luminosity to weights!\n";
    add_lum(*tree, lum);
    tree->SetName(Form("background_tree_%i", year));
    tree->Write();
    file->Close();
  }
  
  //merge trees for each year if needed
  if(years.size() > 1) {
    TList* list = new TList;
    TFile* files[years.size()];
    TString year_string = "";
    for(unsigned i = 0; i < years.size(); ++i) {
      int year = years[i];
      if(i > 0) year_string += "_";
      year_string += year;
      TString file_name = Form("background_trees/background_ztautau_bkg_nano_emu_%i_%i.tree", year, set+ZTauTauHistMaker::kEMu);
      files[i] = TFile::Open(file_name.Data(), "READ");
      list->Add((TTree*) files[i]->Get(Form("background_tree_%i", year)));
    }
    TString file_name = Form("background_trees/background_ztautau_bkg_nano_emu_%s_%i.tree", year_string.Data(), set+ZTauTauHistMaker::kEMu);
    TFile* fout = TFile::Open(file_name.Data(), "RECREATE");
    TTree* t = TTree::MergeTrees(list);
    t->SetName(Form("background_tree_%s", year_string.Data()));
    t->Write();
    fout->Close();
  }  
  return 0;
}
