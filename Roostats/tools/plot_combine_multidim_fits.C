//Plot data from  Higgs Combine MultiDimFit toys

int plot_combine_multidim_fits(const char* file_name, double r_true = 0., TString out_name = "",
                               const int verbose = 0) {

  /////////////////////////////////////////////////////////////////
  // Retrieve the fit data

  TFile* file = TFile::Open(file_name, "READ");
  if(!file) return 1;

  TTree* tree = (TTree*) file->Get("limit");
  if(!tree) {
    cout << "Tree \"limit\" not found in file " << file_name << endl;
    file->Close();
    return 1;
  }


  /////////////////////////////////////////////////////////////////
  // Setup the fit result histograms

  //Get some initial results to determine how to setup the histogram x-axis
  tree->Draw("r >> htmp");
  TH1* h = (TH1*) gDirectory->Get("htmp");

  const double mean  = h->GetMean  (); //mean and width to initialize a histogram of results
  const double sigma = h->GetStdDev();
  delete h;

  //Initialize fit result histograms using the initial results
  h = new TH1D("hr", "Fit signal strengths", 25, mean - 2.5*sigma, mean + 2.5*sigma);
  TH1* hpull  = new TH1D("hpull", "Pulls;(#mu_{fit}-#mu_{gen})/#sigma_{#mu}", 40, -4., 4.);
  TH1* hnll   = new TH1D("hnll" , "NLL-based pull;#sqrt{2*NLL}", 40, -4., 4.);
  TH1* hindex = new TH1D("hindex", "Indices;Fit envelope index", 10, 0., 10.);
  std::vector<TH1*> pull_by_index;
  for(int i = 0; i < 10; ++i) {
    pull_by_index.push_back(new TH1D(Form("pulls_%i", i), Form("Index %i pulls", i), 40, -4., 4.));
  }

  /////////////////////////////////////////////////////////////////
  // Loop through the fit results

  const Long64_t nentries = tree->GetEntries();
  float r, quantile, deltaNLL;
  int index(0);
  tree->SetBranchAddress("r"               , &r         );
  tree->SetBranchAddress("quantileExpected", &quantile  );
  tree->SetBranchAddress("deltaNLL"        , &deltaNLL  );
  //try several indices
  if     (tree->GetBranch("cat_13")) tree->SetBranchAddress("cat_13", &index); //start with highest BDT category
  else if(tree->GetBranch("cat_12")) tree->SetBranchAddress("cat_12", &index);
  else if(tree->GetBranch("cat_11")) tree->SetBranchAddress("cat_11", &index);
  else if(tree->GetBranch("cat_10")) tree->SetBranchAddress("cat_10", &index);

  //store the fit results at each quantile
  const int max_quantiles = 10000;
  double r_vals[max_quantiles], q_vals[max_quantiles], n_vals[max_quantiles];
  int i_vals[max_quantiles]; //index at each quantile
  int quantile_index = 0; //0 = best fit
  for(Long64_t entry = 0; entry <= nentries; ++entry) {
    if(verbose > 2) cout << "Beginning entry " << entry << " / " << nentries << endl;

    if(entry < nentries) tree->GetEntry(entry);
    else  quantile = -1.f; //reset quantile at the end to store the last toy's info

    //fill the information from the previous toy if at a new toy
    if(entry > 0 && quantile <= -1.f) {
      if(verbose) cout << " Finished processing a given toy, evaluating the results\n";
      //get the best fit values
      const double r_fit = r_vals[0];
      const int    i_fit = i_vals[0];

      //get the 1-sigma error on r
      double r_err_low(-1.), r_err_high(-1.), q_low(-1.), q_high(-1.);
      double nll_diff(0.), r_close_true(1.e10);
      for(int iq = 1; iq <= quantile_index; ++iq) {
        const double rval = r_vals[iq];
        const double q    = q_vals[iq];
        const double nll  = n_vals[iq];
        //get the 1-sigma range (assuming singles)
        if(verbose > 3) printf(" r_fit, r, q, nll = %.3f, %.3f, %.3f, %.3f\n", r_fit, rval, q, nll);
        if(fabs(q_low  - 0.32) > fabs(fabs(q) - 0.32) && rval >  r_fit) {r_err_high = fabs(r_fit - rval); q_low  = fabs(q);}
        if(fabs(q_high - 0.32) > fabs(fabs(q) - 0.32) && rval <= r_fit) {r_err_low  = fabs(r_fit - rval); q_high = fabs(q);}
        //get the delta-NLL at a point closest to r_true
        if(std::fabs(r_true - rval) < std::fabs(r_true - r_close_true)) {
          nll_diff = nll;
          r_close_true = rval;
        }
      }
      if(r_err_low < 0. || r_err_high < 0.) {
        printf("Entry %lld: Failed to find fit uncertainty for prev. toy: r_lo = %.2f, r_hi = %.2f\n", entry, r_err_low, r_err_high);
      } else {
        const double r_err = (r_true < r) ? r_err_low : r_err_high;
        const double nll_pull = (nll_diff > 0.) ? ((r_fit > r_true) ? 1. : -1.)*sqrt(2.*nll_diff) : (r_fit - r_true)/r_err;
        //get the pull from the 68% CI, unless it's not accurate (e.g. grid with low points)
        const double pull = (fabs(q_low - 0.32) > 0.03 || fabs(q_high - 0.32) > 0.03) ? nll_pull : (r_fit - r_true) / r_err;
        if(verbose > 0) printf(" --> index = %i, r = %7.3f +- %.3f, pull = %.2f, for r = %7.3f: sqrt(2NLL) = %.2f\n", i_fit, r_fit, r_err, pull, r_close_true, sqrt(2.*nll_diff));
        //Fill the fit results histograms
        h->Fill(r);
        hpull->Fill(pull);
        hnll->Fill(nll_pull);
        if(i_fit >= 0 && i_fit < 10) pull_by_index[i_fit]->Fill(pull);
        hindex->Fill(i_fit);
      }
      quantile = 0.;
      quantile_index = 0;
    } else if(quantile > -1.f) {
      ++quantile_index;
    }

    if(entry >= nentries) {
      if(verbose) cout << "Finished processing the tree!\n";
      break;
    }

    if(nentries <= 10 || verbose > 1)
      printf(" Entry %7lld: (r, quantile, deltaNLL) = (%8.4f, %7.4f, %6.3f)\n", entry, r, quantile, deltaNLL);

    r_vals[quantile_index] = r;
    q_vals[quantile_index] = quantile;
    n_vals[quantile_index] = deltaNLL;
    i_vals[quantile_index] = index;
  }

  if(h->GetEntries() == 0) {
    cout << "No entries found!\n";
    return 1;
  }

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

  //print the index histogram
  c->cd(1);
  hindex->SetLineWidth(2);
  hindex->Draw("hist");
  hindex->GetXaxis()->SetRangeUser(0, hindex->GetXaxis()->GetBinUpEdge(hindex->FindLastBinAbove(0)));
  c->cd(2);
  int colors[] = {kAzure+1, kRed+1, kViolet+6, kOrange, kGreen-3};
  const int ncolors = sizeof(colors) / sizeof(*colors);
  TH1* haxis = nullptr;
  double max_val = 0.;
  TLegend* leg = new TLegend(0.55, 0.75, 0.9, 0.9);
  for(int i = 0; i < 10; ++i) {
    auto h_i = pull_by_index[i];
    if(!h_i || h_i->GetEntries() == 0) continue;
    const int color = colors[i % ncolors];
    h_i->SetLineWidth(2);
    h_i->SetLineColor(color);
    if(!haxis) {
      h_i->Draw("hist");
      haxis = h_i;
    } else {
      h_i->Draw("same hist");
    }
    max_val = max(max_val, h_i->GetMaximum());
    leg->AddEntry(h_i, Form("Index %i", i));
  }
  if(haxis) {
    haxis->SetTitle("Pulls by fit index");
    haxis->GetYaxis()->SetRangeUser(0., 1.3*max_val);
    leg->SetTextSize(0.05);
    leg->Draw();
  }
  out_name.ReplaceAll(".png", "_index.png");
  c->SaveAs(out_name.Data());
  c = new TCanvas();
  hnll->Fit("gaus","L");
  hnll->SetLineWidth(2);
  hnll->SetLineColor(kBlue);
  hnll->SetFillColor(kBlue);
  hnll->SetFillStyle(3003);

out_name.ReplaceAll("_index.png", "_nll.png");
  c->SaveAs(out_name.Data());
  return 0;
}
