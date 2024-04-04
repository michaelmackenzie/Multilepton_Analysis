//Plot data from  Higgs Combine FitDiagnostics toys

int plot_combine_fit_params(const char* file_name, TString out_dir = "figures/fit_params/") {

  /////////////////////////////////////////////////////////////////
  // Retrieve the fit data

  TFile* file = TFile::Open(file_name, "READ");
  if(!file) return 1;

  TTree* tree = (TTree*) file->Get("tree_fit_sb");
  if(!tree) tree = (TTree*) file->Get("limit");
  if(!tree) {
    cout << "Tree \"tree_fit_sb\" not found in file " << file_name << endl;
    file->Close();
    return 1;
  }


  /////////////////////////////////////////////////////////////////
  // Create the output figure directory

  if(!out_dir.EndsWith("/")) out_dir += "/";
  out_dir.ReplaceAll(".", "_");
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", out_dir.Data(), out_dir.Data()));

  /////////////////////////////////////////////////////////////////
  // Loop through the available branches to plot

  TCanvas c;
  auto br_list = tree->GetListOfBranches();
  const char* cut = (TString(tree->GetName()) == "limit") ? "quantileExpected < -0.999" : "";
  for(auto br : *br_list) {
    const char* br_name = br->GetName();
    tree->Draw(br_name, cut);
    c.SaveAs(Form("%s%s.png", out_dir.Data(), br_name));
  }
  return 0;
}
