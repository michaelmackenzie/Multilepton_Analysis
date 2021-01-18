//Script to calculate the p-value for a given distribution

Int_t pval_calculator(int set = 8, int year = 2016) {

  TFile* fInputData = TFile::Open(Form("background_trees/background_ztautau_data_nano_emu_%i.tree",
				       set+ZTauTauHistMaker::kEMu), "READ");
  if(!fInputData) return 1;
  TTree* tree = (TTree*) fInputData->Get("background_tree");
  TFile* fInputPDF = TFile::Open("workspaces/");
  return 0;
}
