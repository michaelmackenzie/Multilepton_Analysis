// Script to call the TMVA training script, TrainTrkQual

#include "TrainTrkQual.C"


int train_tmva(const char* tree_name = "background_ztautau_mutau_7.tree", vector<int> signals = {36,37,38, 39, 40, 41, 42}, vector<int> ignore = {}) {

  TFile *f;

  TString tmvaName;
  TString x = tree_name;
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

Int_t train_all_tmvas(Int_t split_trees) {
  split_trees_ = split_trees;
  gROOT->SetBatch(kTRUE);
  TStopwatch* timer = new TStopwatch();  
  Int_t status = 0;
  status += train_tmva("background_ztautau_Z0_mutau_8.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_Z0_etau_28.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_Z0_emu_48.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_Z0_mutau_e_48.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_Z0_etau_mu_48.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_higgs_mutau_8.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_higgs_etau_28.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_higgs_emu_48.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_higgs_mutau_e_48.tree");
  gSystem->cd("..");
  status += train_tmva("background_ztautau_higgs_etau_mu_48.tree");
  gSystem->cd("..");

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}
