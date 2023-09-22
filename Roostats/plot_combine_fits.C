//Plot data from  Higgs Combine FitDiagnostics toys

int plot_combine_fits(const char* file_name, double r_true = 0., TString out_name = "") {
  TFile* file = TFile::Open(file_name, "READ");
  if(!file) return 1;

  TTree* tree = (TTree*) file->Get("tree_fit_sb");
  if(!tree) {
    cout << "Tree \"tree_fit_sb\" not found in file " << file_name << endl;
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

  TCanvas* c = new TCanvas("c","c",1000,600);
  c->Divide(2,1);
  c->cd(1);
  tree->Draw("r >> htmp"/*, "fit_status == 0"*/);
  TH1* h = (TH1*) gDirectory->Get("htmp");

  const double mean  = h->GetMean();
  const double sigma = h->GetStdDev();
  delete h;
  h = new TH1D("hr", "Fit signal strengths", 25, mean - 2.5*sigma, mean + 2.5*sigma);
  tree->Draw("r >> hr"/*, "fit_status"*/);

  h->SetLineWidth(2);
  const bool do_fit = false;
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  const bool do_quantiles = true;
  if(do_quantiles) {
    Double_t quantiles[] = {0.025, 0.16, 0.5, 0.84, 0.975};
    const int nquantiles = sizeof(quantiles) / sizeof(*quantiles);
    Double_t res[nquantiles];
    h->GetQuantiles(nquantiles, res, quantiles);
    for(int iquant = 0; iquant < nquantiles; ++iquant) {
      TLine* line = new TLine(res[iquant], 0., res[iquant], 1.1*h->GetMaximum());
      const double p = std::fabs(0.5 - quantiles[iquant]);
      line->SetLineColor((p > 0.40) ? kYellow+1 : (p > 0.10) ? kGreen : kBlack);
      line->SetLineWidth(3);
      line->SetLineStyle(kDashed);
      line->Draw();
    }
  }
  if(do_fit) {
    h->Fit("gaus");
  }
  h->SetXTitle("#mu");
  h->SetYTitle("N(toys)");
  h->GetYaxis()->SetRangeUser(0., 1.2*h->GetMaximum());
  h->SetFillColor(kBlue);
  h->SetFillStyle(3003);

  c->cd(2);
  TH1* hpull = new TH1D("hpull", "Pulls", 40, -4., 4.);
  tree->Draw(Form("(r-%.5f)/((r > %.5f) ? rHiErr : rLoErr) >> hpull", r_true, r_true));
  hpull->Fit("gaus");
  hpull->SetLineWidth(2);
  hpull->SetXTitle("(#mu-#mu_{true})/#sigma");
  hpull->SetYTitle("N(toys)");
  hpull->GetYaxis()->SetRangeUser(0., 1.2*hpull->GetMaximum());
  hpull->SetFillColor(kBlue);
  hpull->SetFillStyle(3003);

  if(out_name == "") {out_name = file_name; out_name.ReplaceAll(".root", ".png");}
  else if(!out_name.EndsWith(".png")) out_name += ".png";
  c->SaveAs(out_name.Data());
  return 0;
}
