// Script to call the TMVA training script, TrainTrkQual

#include "TrainTrkQual.C"
vector<int> nano_signals_2016_ = { 9, 10, 11, 12, 13, 14};
vector<int> nano_signals_2017_ = {32, 33, 34, 35, 36, 37};
vector<int> nano_signals_2018_ = {54, 55, 56, 57, 58, 59};

int train_tmva(const char* tree_name = "trees/background_ztautau_Z0_nano_mutau_2016_8.tree",
	       vector<int> signals = nano_signals_2016_, vector<int> ignore = {}) {
  TFile *f;

  TString tmvaName;
  TString x = tree_name;
  x.ReplaceAll("trees/", ""); //remove directory from name
  TObjArray *tx = x.Tokenize(".");
  
  printf("Getting Trees\n");

  f = TFile::Open(tree_name, "READ");

  
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
    f->ls("");
    return 1;
  }

  //check if jet binned selection
  if(x.Contains("_18") || x.Contains("_19") || x.Contains("_20.") || //mutau
     x.Contains("_48") || x.Contains("_49") || x.Contains("_50") || //etau
     x.Contains("_78") || x.Contains("_79") || x.Contains("_80")) //emu
    use_njets_ = 0;
  else
    use_njets_ = 1;
  
  //if 0 jet bin, don't use hT
  if(x.Contains("_18")  || //mutau
     x.Contains("_48")  || //etau
     x.Contains("_78")   ) //emu
    use_ht_ = 0;
  else
    use_ht_ = 1;
  
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
  return TrainTrkQual(signal, background, tmvaName.Data(), signals, ignore);
}

//Train a standard set of selections
Int_t train_all_selections(int year = 2016, Int_t split_trees = 1, bool doJetBinned = false) {
  split_trees_ = split_trees;
  gROOT->SetBatch(kTRUE);
  TStopwatch* timer = new TStopwatch();  
  Int_t status = 0;
  multiTrainings_ = true; //so TrainTrkQual changes directory back at the end
  vector<int> signals;
  if(year == 2016)      signals = nano_signals_2016_;
  else if(year == 2017) signals = nano_signals_2017_;
  else if(year == 2018) signals = nano_signals_2018_;
  if(doJetBinned){
    // 0 jets
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_%i_18.tree"     , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_%i_48.tree"      , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_emu_%i_78.tree"       , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_e_%i_78.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_mu_%i_78.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_%i_18.tree"  , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_%i_48.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_emu_%i_78.tree"    , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_e_%i_78.tree", year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_mu_%i_78.tree", year), signals);
    // 1 jet
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_%i_19.tree"     , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_%i_49.tree"      , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_emu_%i_79.tree"       , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_e_%i_79.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_mu_%i_79.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_%i_19.tree"  , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_%i_49.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_emu_%i_79.tree"    , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_e_%i_79.tree", year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_mu_%i_79.tree", year), signals);
    // >1 jets
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_%i_20.tree"     , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_%i_50.tree"      , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_emu_%i_80.tree"       , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_e_%i_80.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_mu_%i_80.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_%i_20.tree"  , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_%i_50.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_emu_%i_80.tree"    , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_e_%i_80.tree", year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_mu_%i_80.tree", year), signals);
   } else { //Unbinned in njets
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_%i_8.tree"      , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_%i_38.tree"      , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_emu_%i_68.tree"       , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_mutau_e_%i_68.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_Z0_nano_etau_mu_%i_68.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_%i_8.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_%i_38.tree"   , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_emu_%i_68.tree"    , year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_mutau_e_%i_68.tree", year), signals);
    status += train_tmva(Form("trees/background_ztautau_higgs_nano_etau_mu_%i_68.tree", year), signals);
  }
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}

Int_t train_all_years(Int_t split_trees = 1, bool doJetBinned = false) {
  int status(0);
  TStopwatch* timer = new TStopwatch();  
  cout << "*** Training 2016 MVAs...\n";
  status += train_all_selections(2016, split_trees, doJetBinned);
  cout << "*** Training 2017 MVAs...\n";
  status += train_all_selections(2017, split_trees, doJetBinned);
  cout << "*** Training 2018 MVAs...\n";
  status += train_all_selections(2018, split_trees, doJetBinned);
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}
