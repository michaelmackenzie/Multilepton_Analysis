//Plot B->e+mu fit results from fit diagnostics
//create a fit diagnostics root file via:
//$> combine -M FitDiagnostics -d <input card/workspace> --saveShapes --saveWithUncertainties [additional options]

bool unblind_      = false;
bool debug_        = false; //print debug info
bool do_single_    = false; //test printing a single histogram


//------------------------------------------------------------------------------------------
// Helper functions
double hmax(TH1* h) {
  double max_val = h->GetBinContent(1);
  for(int ibin = 2; ibin <= h->GetNbinsX(); ++ibin) max_val = max(max_val, h->GetBinContent(ibin));
  return max_val;
}

double hmin(TH1* h, double cutoff = 0.01) {
  double min_val = h->GetMaximum();
  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    if(h->GetBinContent(ibin) < cutoff) continue;
    min_val = min(min_val, h->GetBinContent(ibin));
  }
  return max(cutoff, min_val);
}

double gmin(TGraph* g, double cutoff = 0.01) {
  const int nbins = g->GetN();
  double min_val = -1;
  for(int ibin = 0; ibin < nbins; ++ibin) {
    const double val = g->GetY()[ibin];
    if(val < cutoff) continue;
    min_val = (min_val < 0.) ? val : min(min_val, val);
  }
  return max(cutoff, min_val);
}

//------------------------------------------------------------------------------------------
// Get a distribution from the directory list
TH1* get_hist(vector<TDirectoryFile*> dirs, const char* name) {
  TH1* h = nullptr;
  for(auto dir : dirs) {
    TH1* h_tmp = (TH1*) dir->Get(name);
    if(!h_tmp) {
      // cout << __func__ << ": Histogram " << name << " not found in directory " << dir->GetName() << endl;
      return nullptr;
    }
    if(!h) {
      h = (TH1*) h_tmp->Clone(Form("%s_Run2", name));
    } else {
      h->Add(h_tmp);
    }
  }
  return h;
}

//------------------------------------------------------------------------------------------
// Get the data distribution from the directory list
TGraphAsymmErrors* get_data(vector<TDirectoryFile*> dirs) {
  TGraphAsymmErrors* g = nullptr;
  for(auto dir : dirs) {
    TGraphAsymmErrors* g_tmp = (TGraphAsymmErrors*) dir->Get("data");
    if(!g_tmp) {
      cout << __func__ << ": Data not found in directory " << dir->GetName() << endl;
      return nullptr;
    }
    if(!g) {
      g = (TGraphAsymmErrors*) g_tmp->Clone("data_Run2");
    } else {
      //Add the data and errors
      for(int ipoint = 0; ipoint < g->GetN(); ++ipoint) {
        g->SetPointY(ipoint, g->GetPointY(ipoint)+g_tmp->GetPointY(ipoint));
        //use sqrt(N) for the errors
        g->SetPointEYhigh(ipoint, sqrt(g->GetPointY(ipoint)));
        g->SetPointEYlow (ipoint, sqrt(g->GetPointY(ipoint)));
      }
    }
  }
  return g;
}

//------------------------------------------------------------------------------------------
// Print an individual distribution
int print_hist(vector<TDirectoryFile*> dirs, TString tag, TString outdir) {

  //Get the fit results and the data
  TH1* hbackground         = get_hist(dirs, "total_background");
  TH1* hbkg                = get_hist(dirs, "bkg");
  TH1* hzmumu              = get_hist(dirs, "zmumu");
  TH1* hzemu               = get_hist(dirs, "zemu");
  TH1* hsignal             = get_hist(dirs, "total_signal");
  TH1* htotal              = get_hist(dirs, "total");
  TGraphAsymmErrors* gdata = get_data(dirs);

  if(!hzmumu) hzmumu = get_hist(dirs, "zmm");
  if(!hzemu ) hzemu  = get_hist(dirs, "sgn");
  if(!hsignal || !hbackground || !htotal || ! hzmumu || !hzemu || !gdata) {
    cout << "Data not found for tag " << tag.Data() << endl;
    return 1;
  }

  //N(data) points, ensure it matches the background model
  const int nbins = gdata->GetN();
  if(nbins != hbackground->GetNbinsX()) {
    cout << "Data and background have different bin numbers!\n";
    return 2;
  }

  //Determine the x-axis range to use
  const double xmin = htotal->GetBinLowEdge(htotal->FindFirstBinAbove(0.01));
  const double xmax = htotal->GetXaxis()->GetBinUpEdge(htotal->FindLastBinAbove(0.01));

  //Create the canvas to plot on
  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  TCanvas* c = new TCanvas("c", "c", 900, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.40, 1., 1.00);
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.20, 1., 0.40);
  TPad* pad3 = new TPad("pad3", "pad3", 0., 0.00, 1., 0.20);
  pad1->SetRightMargin(0.03); pad2->SetRightMargin(0.03); pad3->SetRightMargin(0.03);
  pad1->SetBottomMargin(0.02); pad2->SetBottomMargin(0.05); pad3->SetBottomMargin(0.28);
  pad2->SetTopMargin(0.03); pad3->SetTopMargin(0.04);
  pad1->Draw(); pad2->Draw(); pad3->Draw();

  // Draw the data and fit components
  pad1->cd();

  //Configure the data style
  gdata->SetMarkerStyle(20);
  gdata->SetMarkerSize(0.8);
  gdata->SetLineWidth(3);

  //Configure the total fit (S+B) style
  htotal->SetLineColor(kRed);
  htotal->SetMarkerColor(kRed);
  htotal->SetFillColor(0);
  htotal->SetFillStyle(3003);
  // htotal->SetMarkerStyle(20);
  htotal->SetMarkerSize(0.);
  htotal->SetLineWidth(3);
  htotal->SetLineColor(kRed);
  htotal->SetTitle("");
  htotal->SetXTitle("");
  htotal->SetYTitle(Form("Events / %.1f GeV/c^{2}", htotal->GetBinWidth(1)));
  htotal->GetYaxis()->SetTitleSize(0.05);
  htotal->GetYaxis()->SetTitleOffset(0.92);
  htotal->GetXaxis()->SetLabelSize(0.);

  //Configure the background component style
  hbkg->SetLineColor(kRed);
  hbkg->SetMarkerColor(kRed);
  hbkg->SetLineWidth(3);
  hbkg->SetLineStyle(kDashed);

  //Configure the Z->mumu component style
  hzmumu->SetLineColor(kGreen);
  hzmumu->SetMarkerColor(kGreen);
  hzmumu->SetLineWidth(3);
  hzmumu->SetLineStyle(kDashed);

  //Configure the signal component style
  hsignal->SetLineColor(kBlue);
  hsignal->SetMarkerColor(kBlue);
  hsignal->SetLineWidth(3);
  hsignal->SetLineStyle(kDashed);

  //Draw the results
  htotal->Draw("L");
  if(unblind_) {
    hbkg->Draw("L same");
    hzmumu->Draw("L same");
    hsignal->Draw("L same");
  }
  gdata->Draw("P");
  htotal->GetXaxis()->SetRangeUser(xmin, xmax);


  //Add a legend
  TLegend leg(0.6, 0.5, 0.85, 0.85);
  leg.AddEntry(gdata, "Data", "PLE");
  leg.AddEntry(htotal, "Background+signal fit", "LF");
  if(unblind_) {
    leg.AddEntry(hbkg, "Parametric background", "L");
    leg.AddEntry(hzmumu, "Z#rightarrow#mu#mu", "L");
    leg.AddEntry(hsignal, "Fit Z#rightarrowe#mu", "L");
  }
  leg.SetTextSize(0.05);
  leg.SetFillStyle(0);
  leg.SetFillColor(0);
  leg.SetLineColor(0);
  leg.SetLineStyle(0);
  leg.Draw();

  //Make the background subtracted plots
  pad2->cd();

  //Make the data / total fit and data / background component distributions
  TGraphAsymmErrors* gDiff_s = (TGraphAsymmErrors*) gdata->Clone("gDiff_s"); //data / S+B
  TGraphAsymmErrors* gDiff_b = (TGraphAsymmErrors*) gdata->Clone("gDiff_b"); //data / B-only
  TH1* hPull_s = (TH1*) htotal->Clone("hPull_s"); hPull_s->Reset(); //(data - model)/unc (S+B-only)
  TH1* hPull_b = (TH1*) htotal->Clone("hPull_b"); hPull_b->Reset(); //(data - model)/unc (B-only)
  double min_diff(1.e10), max_diff(-1.e10);
  //Debug printout if needed
  if(debug_) {
    printf("Bin:     data        B +-  sigma_B       S+B +- sigma_S+B  pull_B  pull_S+B\n");

  }
  for(int bin = 0; bin < nbins; ++bin) {
    //Retrieve the data point and corresponding model value
    double x, y;
    gdata->GetPoint(bin, x, y);
    const double err_high = gdata->GetErrorYhigh(bin);
    const double err_low  = gdata->GetErrorYlow (bin);
    const double tot_v    = htotal->GetBinContent(bin+1);
    const double tot_e    = htotal->GetBinError  (bin+1);
    const double bkg_v    = hbackground->GetBinContent(bin+1);
    const double bkg_e    = hbackground->GetBinError  (bin+1);

    //Calculate the differences and errors (data-only)
    const double val_s      = (tot_v > 0.) ? y - tot_v : 0.;
    const double val_s_high = (tot_v > 0.) ? err_high  : 0.;
    const double val_s_low  = (tot_v > 0.) ? err_low   : 0.;
    const double val_b      = (bkg_v > 0.) ? y - bkg_v : 0.;
    const double val_b_high = (bkg_v > 0.) ? err_high  : 0.;
    const double val_b_low  = (bkg_v > 0.) ? err_low   : 0.;

    //Calculate the pulls, combining the data and model errors
    const double data_err_s = (y > tot_v) ? err_low : err_high;
    const double data_err_b = (y > bkg_v) ? err_low : err_high;
    const double pull_s     = (tot_v > 0.) ? (y - tot_v) / sqrt(tot_v) : 0.;
    const double pull_b     = (bkg_v > 0.) ? (y - bkg_v) / sqrt(bkg_v) : 0.;

    //Set the points
    gDiff_s->SetPoint      (bin, x, val_s);
    gDiff_s->SetPointEYhigh(bin, val_s_high);
    gDiff_s->SetPointEYlow (bin, val_s_low );
    gDiff_b->SetPoint      (bin, x, val_b);
    gDiff_b->SetPointEYhigh(bin, val_b_high);
    gDiff_b->SetPointEYlow (bin, val_b_low );
    hPull_s->SetBinContent(bin+1, pull_s);
    hPull_b->SetBinContent(bin+1, pull_b);

    max_diff = max(max_diff, val_b);
    min_diff = min(min_diff, val_b);

    //Debug printout if needed
    if(debug_) {
      printf(" %2i: %8.0f %8.1f +- %8.2f  %8.1f +- %8.2f   %5.2f    %5.2f\n",
             bin+1, y, bkg_v, bkg_e, tot_v, tot_e, pull_b, pull_s);
    }
  }

  auto gDiff = (unblind_) ? gDiff_b : gDiff_s; //Difference plot to use, data-total if blinded, data-bkg-only otherwise

  //Make a background uncertainty plot
  TH1* hBkg_unc = (TH1*) hbackground->Clone("hBkg_unc");
  for(int ibin = 1; ibin <= hbackground->GetNbinsX(); ++ibin) {
    if(hbackground->GetBinContent(ibin) <= 0.01) hBkg_unc->SetBinContent(ibin, 0.);
    else {
      hBkg_unc->SetBinContent(ibin, 1.);
      if(hbackground->GetBinError(ibin) > 0.) {
        hBkg_unc->SetBinError(ibin, hbackground->GetBinError(ibin) / hbackground->GetBinContent(ibin));
      } else {
        hBkg_unc->SetBinError(ibin, 0.1 / hbackground->GetBinContent(ibin));
      }
    }
  }
  hBkg_unc->SetFillColor(kGray+3);
  hBkg_unc->SetLineColor(kGray+3);
  hBkg_unc->SetFillStyle(3003);

  //Add the signal fit to the difference plot
  hzemu->SetFillColor(0);
  hzemu->SetLineWidth(3);
  hzemu->SetLineStyle(kSolid);

  //Draw the results
  hBkg_unc->Draw("E2");
  if(unblind_) {
    hzemu->Draw("L same");
    hzemu->GetYaxis()->SetRangeUser(min_diff - 0.05*(max_diff-min_diff), max_diff + 0.05*(max_diff-min_diff)); //necessary due to y-axis importing from clone
  }
  gDiff->Draw("P");
  hBkg_unc->GetXaxis()->SetRangeUser(xmin, xmax);

  //Add a reference line for perfect agreement
  TLine* line = new TLine(xmin, 0., xmax, 0.);
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->SetLineStyle(kDashed);
  line->Draw("same");

  //Configure the titles and axes
  hBkg_unc->GetYaxis()->SetRangeUser(min_diff - 0.05*(max_diff-min_diff), max_diff + 0.05*(max_diff-min_diff));
  hBkg_unc->SetTitle("");
  hBkg_unc->SetXTitle("");
  hBkg_unc->GetXaxis()->SetLabelSize(0.);
  hBkg_unc->GetYaxis()->SetLabelSize(0.10);
  hBkg_unc->GetYaxis()->SetTitleSize(0.15);
  hBkg_unc->GetYaxis()->SetTitleOffset(0.30);
  if(unblind_) hBkg_unc->SetYTitle("Data - Bkg");
  else         hBkg_unc->SetYTitle("Data - Fit");


  //Make a pull plot
  pad3->cd();

  auto hPull = hPull_s; //FIXME: Decide whether pulls should use background or total model
  hPull->SetLineColor(kAtlantic);
  hPull->SetFillColor(kAtlantic);
  hPull->SetFillStyle(1000);
  hPull->Draw("hist");
  hPull->GetYaxis()->SetRangeUser(-3,3);
  hPull->SetTitle("");
  hPull->SetXTitle("M_{e#mu} (GeV/c^{2})");
  hPull->SetYTitle("#frac{(Data-Fit)}{#sigma_{Fit}}");
  hPull->GetXaxis()->SetLabelSize(0.10);
  hPull->GetYaxis()->SetLabelSize(0.10);
  hPull->GetXaxis()->SetTitleSize(0.15);
  hPull->GetYaxis()->SetTitleSize(0.15);
  hPull->GetXaxis()->SetTitleOffset(0.75);
  hPull->GetYaxis()->SetTitleOffset(0.27);

  //Add a reference line for perfect agreement
  TLine* line_2 = new TLine(xmin, 0., xmax, 0.);
  line_2->SetLineColor(kBlack);
  line_2->SetLineWidth(2);
  line_2->SetLineStyle(kDashed);
  line_2->Draw("same");

  //Print a linear and a log version of the distribution
  double min_val = std::max(0.1, std::min(gmin(gdata), hmin(htotal)));
  double max_val = std::max(gdata->GetMaximum(), hmax(htotal));
  htotal->GetYaxis()->SetRangeUser(0., 1.2*max_val);
  c->SaveAs(Form("%s%s.png", outdir.Data(), tag.Data()));
  double plot_min = std::min(std::max(0.2, 0.2*hmax(hsignal)), 0.2*min_val);
  double plot_max = plot_min*pow(10, 1.7*log10(max_val/plot_min));
  htotal->GetYaxis()->SetRangeUser(plot_min, plot_max);
  pad1->SetLogy();
  c->SaveAs(Form("%s%s_logy.png", outdir.Data(), tag.Data()));

  //Clean up after printing
  delete c;
  delete gDiff_s;
  delete gDiff_b;
  delete hPull_s;
  delete hPull_b;
  delete hBkg_unc;
  delete line;
  delete line_2;

  return 0;
}

//------------------------------------------------------------------------------------------
// Print the fit results for each category in a fit configuration directory
int print_dir(TDirectoryFile* dir, TString tag, TString outdir) {
  int status(0);
  if(!dir) return 1;

  //List of categories
  TList* list = dir->GetListOfKeys();
  if(!list) return 10;
  bool subdir(false); //whether there are sub-directories or not
  for(TObject* o : *list) {
    TObject* obj = dir->Get(o->GetName());
    if(!obj) continue;

    //Check if this object is a directory with categories, if so recursively process it
    bool isdir = obj->InheritsFrom(TDirectoryFile::Class());
    if(isdir) {
      auto next_dir = (TDirectoryFile*) obj;
      status += print_dir(next_dir, (tag + "_") + obj->GetName(), outdir);
      subdir = true;
      if(do_single_) return status;
    }
  }

  //If this directory doesn't contain a sub-directory, print the histograms within the category
  if(!subdir) { //histogram directory
    status += print_hist({dir}, tag, outdir);
  }
  return status;
}

//------------------------------------------------------------------------------------------
// Print all fit figures
int plot_bemu(TString fname, TString outdir = "figures", bool unblind = false) {
  unblind_ = unblind;

  //Get the fit file
  TFile* file = TFile::Open(fname.Data(), "READ");
  if(!file) return 1;

  TDirectoryFile *prefit = (TDirectoryFile*) file->Get("shapes_prefit");
  TDirectoryFile *fit_b  = (TDirectoryFile*) file->Get("shapes_fit_b");
  TDirectoryFile *fit_s  = (TDirectoryFile*) file->Get("shapes_fit_s");

  if(!prefit || (unblind && (!fit_b || !fit_s))) {
    cout << "Fit directories not found!\n";
    return 2;
  }

  //Create the figure directory
  if(!outdir.EndsWith("/")) outdir += "/";
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", outdir.Data(), outdir.Data()));

  //Print the fit configurations: Pre-fit, background-only fit, and background+signal fit
  int status(0);
  if(!do_single_) status += print_dir(prefit, "prefit", outdir);
  if(!do_single_ && fit_b) status += print_dir(fit_b , "fit_b" , outdir);
  if(fit_s) status += print_dir(fit_s , "fit_s" , outdir);

  cout << "Plotting status = " << status << endl;
  return status;
}
