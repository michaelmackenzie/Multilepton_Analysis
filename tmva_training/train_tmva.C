// Script to call the TMVA training script, TrainTrkQual
using namespace CLFV;

#include "TrainTrkQual.C"

int train_tmva(const char* tree_name = "trees/background_clfv_Z0_nano_mutau_2016_8.tree", vector<int> ignore = {}, int version = -1) {
  if(version > -1) trkqual_version_ = version;

  TFile *f;
  TString tmvaName;
  TString x = tree_name;
  x.ReplaceAll("trees/", ""); //remove directory from name
  TObjArray *tx = x.Tokenize(".");

  printf("Getting Trees\n");

  f = TFile::Open(tree_name, "READ");

  if(!f) {
    return 1;
  }

  TTree* signal;
  int isData = 0;
  if(x.Contains("mock")) {
    signal = (TTree*) f->Get("background_tree_mock_data");
    isData = 1;
  } else
    signal = (TTree*) f->Get("background_tree");


  TTree* background = signal;
  if(!signal || !background) {
    printf("Trees not found\n");
    f->ls();
    return 2;
  }

  //check if jet binned selection
  if(x.Contains("_18") || x.Contains("_19") || x.Contains("_20.") || //mutau
     x.Contains("_48") || x.Contains("_49") || x.Contains("_50") || //etau
     x.Contains("_78") || x.Contains("_79") || x.Contains("_80")) //emu
    use_njets_ = 0;
  else
    use_njets_ = 1;

  tmvaName = "training_";
  for(int i = 0; i < tx->GetLast(); ++i) tmvaName +=   ((TObjString *)(tx->At(i)))->String();
  int n = ignore.size();
  if(n > 0) {
    tmvaName += "_mva_no_";
    if(n > 1)
      printf("Ignoring categories: ");
    else
      printf("Ignoring category: ");
    for(int i = 0; i < n; ++i) {
      printf("%i%s",ignore[i], (i < n-1) ? ", " : "\n");
      tmvaName += ignore[i];
      if( i < n-1) tmvaName += "_";
    }
  }

  //if the version is non-default, add to the name
  if(trkqual_version_ > -1 && trkqual_version_ != TrkQualInit::Default) tmvaName += Form("_v%i", trkqual_version_);

  TString selection = "";
  if(tmvaName.Contains("Z0"))
    selection += "z";
  else if(tmvaName.Contains("higgs"))
    selection += "h";
  else {
    printf("Unknown selection! Defaulting to Z0!\n");
    selection += "z";
  }

  if(tmvaName.Contains("mutau")) {
    selection += "mutau";
    if(tmvaName.Contains("_e_"))
      selection += "_e";
  } else if(tmvaName.Contains("etau")) {
    selection += "etau";
    if(tmvaName.Contains("_mu_"))
      selection += "_mu";
  } else if(tmvaName.Contains("emu"))
    selection += "emu";
  else {
    printf("Unknown selection! Default to mutau!\n");
    selection += "mutau";
  }
  selection_ = selection;
  printf("Beginning Training %s with selection %s\n",tmvaName.Data(), selection.Data());
  return TrainTrkQual(signal, background, tmvaName.Data(), ignore);
}

//Train a standard set of selections
Int_t train_all_selections(vector<int> years = {2016}, Int_t split_trees = 1, bool doJetBinned = false, int version = -1) {
  split_trees_ = split_trees;
  gROOT->SetBatch(kTRUE);
  TStopwatch* timer = new TStopwatch();
  Int_t status = 0;
  multiTrainings_ = true; //so TrainTrkQual changes directory back at the end
  TString year_string = "";
  for(int i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  std::vector<int> ignores = {};
  if(doJetBinned){
    // 0 jets
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_%s_18.tree"     , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_%s_48.tree"      , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_emu_%s_78.tree"       , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_e_%s_78.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_mu_%s_78.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_%s_18.tree"  , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_%s_48.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_emu_%s_78.tree"    , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_e_%s_78.tree", year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_mu_%s_78.tree", year_string.Data()), ignores, version);
    // 1 jet
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_%s_19.tree"     , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_%s_49.tree"      , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_emu_%s_79.tree"       , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_e_%s_79.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_mu_%s_79.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_%s_19.tree"  , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_%s_49.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_emu_%s_79.tree"    , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_e_%s_79.tree", year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_mu_%s_79.tree", year_string.Data()), ignores, version);
    // >1 jets
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_%s_20.tree"     , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_%s_50.tree"      , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_emu_%s_80.tree"       , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_e_%s_80.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_mu_%s_80.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_%s_20.tree"  , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_%s_50.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_emu_%s_80.tree"    , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_e_%s_80.tree", year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_mu_%s_80.tree", year_string.Data()), ignores, version);
   } else { //Unbinned in njets
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_%s_8.tree"      , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_%s_8.tree"       , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_emu_%s_8.tree"        , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_mutau_e_%s_8.tree"    , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_Z0_nano_etau_mu_%s_8.tree"    , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_%s_8.tree"   , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_%s_8.tree"    , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_emu_%s_8.tree"     , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_mutau_e_%s_8.tree" , year_string.Data()), ignores, version);
    status += train_tmva(Form("trees/background_clfv_higgs_nano_etau_mu_%s_8.tree" , year_string.Data()), ignores, version);
  }
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}

Int_t train_all_years(Int_t split_trees = 1, bool doJetBinned = false, int variable_version = -1) {
  int status(0);
  TStopwatch* timer = new TStopwatch();
  if(variable_version > -1) {
    cout << "Setting the TrkQualInit version from default to " << variable_version << endl;
  }
  // cout << "*** Training 2016 MVAs...\n";
  // status += train_all_selections({2016}, split_trees, doJetBinned, variable_version);
  // cout << "*** Training 2017 MVAs...\n";
  // status += train_all_selections({2017}, split_trees, doJetBinned, variable_version);
  // cout << "*** Training 2018 MVAs...\n";
  // status += train_all_selections({2018}, split_trees, doJetBinned, variable_version);
  cout << "*** Training Full Run-II MVAs...\n";
  status += train_all_selections({2016, 2017, 2018}, split_trees, doJetBinned, variable_version);
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}
