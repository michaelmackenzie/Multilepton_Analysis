//Plot data from Higgs Combine toys

int plot_combine_toys(const char* file_name, TString out_name = "") {
  TFile* file = TFile::Open(file_name, "READ");
  if(!file) return 1;

  TTree* tree = (TTree*) file->Get("limit");
  if(!tree) {
    cout << "Tree \"limit\" not found in file " << file_name << endl;
    file->Close();
    return 1;
  }

  // const Long64_t nentries = tree->GetEntries();
  // Double_t limit, quantile;
  // std::vector<Double_t> limits;
  // tree->SetBranchAddress("limit", &limit);
  // tree->SetBranchAddress("quantileExpected", &quantile);

  // for(Long64_t entry = 0; entry < nentries; ++entry) {
  //   tree->GetEntry(entry);
  //   if(quantile > 0.) continue;
  //   limits.push_back(limit);
  // }

  TCanvas* c = new TCanvas();
  tree->Draw("limit >> htmp", "quantileExpected < 0");
  TH1* h = (TH1*) gDirectory->Get("htmp");

  const double mean  = h->GetMean();
  const double sigma = h->GetStdDev();
  delete h;
  h = new TH1D("hlimit", "Toy limits", 25, mean - 2.5*sigma, mean + 2.5*sigma);
  tree->Draw("limit >> hlimit", "quantileExpected < 0");

  h->SetLineWidth(2);
  // h->Fit("gaus");
  gStyle->SetOptStat(1211);
  Double_t quantiles[] = {0.025, 0.16, 0.5, 0.84, 0.975};
  const int nquantiles = sizeof(quantiles) / sizeof(*quantiles);
  Double_t res[nquantiles];
  h->GetQuantiles(nquantiles, res, quantiles);
  for(int iquant = 0; iquant < nquantiles; ++iquant) {
    TLine* line = new TLine(res[iquant], 0., res[iquant], h->GetMaximum());
    const double p = std::fabs(0.5 - quantiles[iquant]);
    line->SetLineColor((p > 0.40) ? kYellow+1 : (p > 0.10) ? kGreen : kBlack);
    line->SetLineWidth(3);
    line->SetLineStyle(kDashed);
    line->Draw();
  }
  h->SetXTitle("#mu");
  h->SetYTitle("N(toys)");

  if(out_name == "") {out_name = file_name; out_name.ReplaceAll(".root", ".png");}
  c->SaveAs(out_name.Data());
  return 0;
}
