//Plot data from  Higgs Combine FitDiagnostics toys

int plot_combine_fits(const char* file_name, double r_true = 0., TString out_name = "",
                      const int skip_bad_fits = 2, const int err_mode = 0) {

  /////////////////////////////////////////////////////////////////
  // Retrieve the fit data

  TFile* file = TFile::Open(file_name, "READ");
  if(!file) return 1;

  TTree* tree = (TTree*) file->Get("tree_fit_sb");
  if(!tree) {
    cout << "Tree \"tree_fit_sb\" not found in file " << file_name << endl;
    file->Close();
    return 1;
  }


  /////////////////////////////////////////////////////////////////
  // Setup the fit result histograms

  //Get some initial results to determine how to setup the histogram x-axis
  tree->Draw("r >> htmp"/*, "fit_status == 0"*/);
  TH1* h = (TH1*) gDirectory->Get("htmp");

  const double mean  = h->GetMean();
  const double sigma = h->GetStdDev();
  delete h;

  //Initialize fit result histograms using the initial results
  h = new TH1D("hr", "Fit signal strengths", 25, mean - 2.5*sigma, mean + 2.5*sigma);
  TH1* hpull = new TH1D("hpull", "Pulls", 40, -4., 4.);

  /////////////////////////////////////////////////////////////////
  // Loop through the fit results

  const Long64_t nentries = tree->GetEntries();
  double r, rHiErr, rLoErr;
  int fit_status;
  tree->SetBranchAddress("r"         , &r         );
  tree->SetBranchAddress("rHiErr"    , &rHiErr    );
  tree->SetBranchAddress("rLoErr"    , &rLoErr    );
  tree->SetBranchAddress("fit_status", &fit_status);
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    tree->GetEntry(entry);
    if(nentries <= 10) cout << " Entry " << entry << ": (r, r_up, r_down) = (" << r << ", " << rHiErr << ", " << rLoErr << "), status = " << fit_status << endl;

    //over-ride low error with high error
    if(err_mode == 1) rLoErr = rHiErr;

    //check for fit issues
    if(skip_bad_fits > 0) {
      if(std::fabs(rLoErr/rHiErr) > 2. || std::fabs(rHiErr/rLoErr) > 2.) {
        cout << " Entry " << entry << " has suspicious errors: (r, r_up, r_down) = (" << r << ", " << rHiErr << ", " << rLoErr << "), status = " << fit_status << endl;
        continue;
      }
    }
    if(fit_status != 0 && skip_bad_fits > 1) {
      cout << " Entry " << entry << " has non-zero fit status: (r, r_up, r_down) = (" << r << ", " << rHiErr << ", " << rLoErr << "), status = " << fit_status << endl;
      if(skip_bad_fits) continue;
    }
    //Fill the fit results histograms
    h->Fill(r);
    hpull->Fill((r > r_true) ? (r - r_true) / rHiErr : (r - r_true) / rLoErr);
  }
  // tree->Draw("r >> hr"/*, "fit_status"*/);
  // tree->Draw(Form("(r-%.5f)/((r > %.5f) ? rHiErr : rLoErr) >> hpull", r_true, r_true));

  /////////////////////////////////////////////////////////////////
  // Plot the results

  TCanvas* c = new TCanvas("c","c",1000,600);
  c->Divide(2,1);

  //Plot the fit result histogram
  c->cd(1);
  h->SetLineWidth(2);
  h->Draw("hist");
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
      if(std::fabs(quantiles[iquant] - 0.5) < 0.001) { //add central quantile value
        TLatex label;
        label.SetNDC();
        label.SetTextFont(72);
        label.SetTextAlign(13);
        label.SetTextAngle(0);
        label.SetTextSize(0.04);
        label.DrawLatex(0.45, 0.87, Form("#mu = %.2f", res[iquant]));
      }
    }
  }
  if(do_fit) {
    h->Fit("gaus","L");
  }
  h->SetXTitle("#mu");
  h->SetYTitle("N(toys)");
  h->GetYaxis()->SetRangeUser(0., 1.2*h->GetMaximum());
  h->SetFillColor(kBlue);
  h->SetFillStyle(3003);

  //Plot the pull histogram
  c->cd(2);
  hpull->Fit("gaus","L");
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
