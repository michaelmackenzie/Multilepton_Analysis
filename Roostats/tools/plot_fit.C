//Plot fit results from fit diagnostics
//create a fit diagnostics root file via:
//$> combine -M FitDiagnostics -d <input card/workspace> --saveShapes --saveWithUncertainties [additional options]

bool unblind_ = false;
bool debug_ = false;

//------------------------------------------------------------------------------------------
// Print an individual histogram
int print_hist(TDirectoryFile* dir, TString tag, TString outdir) {

  //Get the fit results and the data
  TH1* hbackground = (TH1*) dir->Get("total_background");
  TH1* hsignal     = (TH1*) dir->Get("total_signal");
  TH1* htotal      = (TH1*) dir->Get("total");
  TGraphAsymmErrors* gdata = (TGraphAsymmErrors*) dir->Get("data");
  if(!hsignal || !hbackground || !htotal || !gdata) {
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
  TCanvas* c = new TCanvas("c", "c", 1000, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.40, 1., 1.00);
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.20, 1., 0.40);
  TPad* pad3 = new TPad("pad3", "pad3", 0., 0.00, 1., 0.20);
  pad1->SetRightMargin(0.03); pad2->SetRightMargin(0.03); pad3->SetRightMargin(0.03);
  pad1->SetBottomMargin(0.02); pad2->SetBottomMargin(0.05); pad3->SetBottomMargin(0.25);
  pad2->SetTopMargin(0.03); pad3->SetTopMargin(0.02);
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
  htotal->SetFillColor(kRed);
  htotal->SetFillStyle(3003);
  // htotal->SetMarkerStyle(20);
  htotal->SetMarkerSize(0.);
  htotal->SetLineWidth(3);
  htotal->SetLineColor(kRed);
  htotal->SetTitle("");
  htotal->SetXTitle("");
  htotal->SetYTitle("Events / Bin");
  htotal->GetYaxis()->SetTitleSize(0.04);
  htotal->GetYaxis()->SetTitleOffset(1.3);
  htotal->GetXaxis()->SetLabelSize(0.);

  //Configure the background component style
  hbackground->SetLineColor(kRed);
  hbackground->SetMarkerColor(kRed);
  hbackground->SetLineWidth(3);
  hbackground->SetLineStyle(kDashed);

  //Configure the signal component style
  hsignal->SetLineColor(kBlue);
  hsignal->SetMarkerColor(kBlue);
  hsignal->SetLineWidth(3);

  //Draw the results
  htotal->Draw("E1");
  htotal->Draw("E2 same");
  if(unblind_) {
    hbackground->Draw("hist same");
    hsignal->Draw("hist same");
  }
  gdata->Draw("P");
  htotal->GetXaxis()->SetRangeUser(xmin, xmax);


  //Add a legend
  TLegend leg(0.6, 0.5, 0.85, 0.85);
  leg.AddEntry(gdata, "Data", "PLE");
  leg.AddEntry(htotal, "Background+signal fit", "LF");
  if(unblind_) {
    leg.AddEntry(hbackground, "Background component", "L");
    leg.AddEntry(hsignal, "Signal component", "L");
  }
  leg.SetTextSize(0.05);
  leg.SetFillStyle(0);
  leg.SetFillColor(0);
  leg.SetLineColor(0);
  leg.SetLineStyle(0);
  leg.Draw();

  //Make the ratio plots
  pad2->cd();

  //Make the data / total fit and data / background component distributions
  TGraphAsymmErrors* gRatio_s = (TGraphAsymmErrors*) gdata->Clone("gRatio_s"); //data / S+B
  TGraphAsymmErrors* gRatio_b = (TGraphAsymmErrors*) gdata->Clone("gRatio_b"); //data / B-only
  TH1* hPull_s = (TH1*) htotal->Clone("hPull_s"); hPull_s->Reset(); //(data - model)/unc (S+B-only)
  TH1* hPull_b = (TH1*) htotal->Clone("hPull_b"); hPull_b->Reset(); //(data - model)/unc (B-only)
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

    //Calculate the ratio and errors
    const double val_s      = (tot_v > 0.) ? y        / tot_v : 0.;
    const double val_s_high = (tot_v > 0.) ? err_high / tot_v : 0.;
    const double val_s_low  = (tot_v > 0.) ? err_low  / tot_v : 0.;
    const double val_b      = (bkg_v > 0.) ? y        / bkg_v : 0.;
    const double val_b_high = (bkg_v > 0.) ? err_high / bkg_v : 0.;
    const double val_b_low  = (bkg_v > 0.) ? err_low  / bkg_v : 0.;

    //Calculate the pulls, combining the data and model errors
    const double data_err_s = (y > tot_v) ? err_low : err_high;
    const double data_err_b = (y > bkg_v) ? err_low : err_high;
    const double pull_s     = (tot_e > 0.) ? (y - tot_v) / sqrt(tot_e*tot_e + data_err_s*data_err_s) : 0.;
    const double pull_b     = (bkg_e > 0.) ? (y - bkg_v) / sqrt(bkg_e*bkg_e + data_err_b*data_err_b) : 0.;

    //Set the points
    gRatio_s->SetPoint      (bin, x, val_s);
    gRatio_s->SetPointEYhigh(bin, val_s_high);
    gRatio_s->SetPointEYlow (bin, val_s_low );
    gRatio_b->SetPoint      (bin, x, val_b);
    gRatio_b->SetPointEYhigh(bin, val_b_high);
    gRatio_b->SetPointEYlow (bin, val_b_low );
    hPull_s->SetBinContent(bin+1, pull_s);
    hPull_b->SetBinContent(bin+1, pull_b);

    //Debug printout if needed
    if(debug_) {
      printf(" %2i: %8.0f %8.1f +- %8.2f  %8.1f +- %8.2f   %5.2f    %5.2f\n",
             bin+1, y, bkg_v, bkg_e, tot_v, tot_e, pull_b, pull_s);
    }
  }

  auto gRatio = (unblind_) ? gRatio_b : gRatio_s; //Ratio plot to use, data/total if blinded, data/bkg-only otherwise

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

  //Make a total / background component distribution
  TH1* hRatio_s = (TH1*) htotal->Clone("hRatio_s");
  hRatio_s->Divide(hbackground);
  for(int ibin = 1; ibin <= hsignal->GetNbinsX(); ++ibin) {
    if(hbackground->GetBinContent(ibin) <= 0.1) hRatio_s->SetBinContent(ibin, 0.);
  }
  hRatio_s->SetFillColor(0);
  hRatio_s->SetLineColor(kBlue);
  hRatio_s->SetLineStyle(kDashed);

  //Draw the results
  hBkg_unc->Draw("E2 same");
  if(unblind_) hRatio_s->Draw("hist same");
  gRatio->Draw("P");
  hBkg_unc->GetXaxis()->SetRangeUser(xmin, xmax);

  //Add a reference line for perfect agreement
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->SetLineStyle(kDashed);
  line->Draw("same");

  //Configure the titles and axes
  hBkg_unc->GetYaxis()->SetRangeUser(0.8, 1.2);
  hBkg_unc->SetTitle("");
  hBkg_unc->SetXTitle("Bins");
  hBkg_unc->GetXaxis()->SetLabelSize(0.);
  hBkg_unc->GetYaxis()->SetLabelSize(0.10);
  hBkg_unc->GetYaxis()->SetTitleSize(0.12);
  hBkg_unc->GetYaxis()->SetTitleOffset(0.40);
  if(unblind_) hBkg_unc->SetYTitle("Data/Bkg");
  else         hBkg_unc->SetYTitle("Data/Fit");


  //Make a pull plot
  pad3->cd();

  auto hPull = hPull_s; //FIXME: Decide whether pulls should use background or total model
  hPull->SetLineColor(kAtlantic);
  hPull->SetFillColor(kAtlantic);
  hPull->SetFillStyle(1000);
  hPull->Draw("hist");
  hPull->GetYaxis()->SetRangeUser(-2,2);
  hPull->SetTitle("");
  hPull->SetXTitle("Bins");
  hPull->SetYTitle("#frac{(Data-Fit)}{Uncertainty}");
  hPull->GetXaxis()->SetLabelSize(0.10);
  hPull->GetYaxis()->SetLabelSize(0.10);
  hPull->GetXaxis()->SetTitleSize(0.12);
  hPull->GetYaxis()->SetTitleSize(0.12);
  hPull->GetXaxis()->SetTitleOffset(0.70);
  hPull->GetYaxis()->SetTitleOffset(0.35);

  //Add a reference line for perfect agreement
  TLine* line_2 = new TLine(xmin, 0., xmax, 0.);
  line_2->SetLineColor(kBlack);
  line_2->SetLineWidth(2);
  line_2->SetLineStyle(kDashed);
  line_2->Draw("same");

  //Print a linear and a log version of the distribution
  double min_val = std::max(0.1, 0.8*std::min(gdata->GetMinimum(), htotal->GetMinimum()));
  double max_val = std::max(gdata->GetMaximum(), htotal->GetMaximum());
  htotal->GetYaxis()->SetRangeUser(0., 1.2*max_val);
  c->SaveAs(Form("%s%s.png", outdir.Data(), tag.Data()));
  double log_max = min_val*pow(10, 1.5*log10(max_val/min_val));
  htotal->GetYaxis()->SetRangeUser(0.8*min_val, log_max);
  pad1->SetLogy();
  c->SaveAs(Form("%s%s_logy.png", outdir.Data(), tag.Data()));

  //Clean up after printing
  delete c;
  delete gRatio_s;
  delete gRatio_b;
  delete hPull_s;
  delete hPull_b;
  delete hBkg_unc;
  delete hRatio_s;
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
      status += print_dir((TDirectoryFile*) obj, (tag + "_") + obj->GetName(), outdir);
      subdir = true;
    }
  }

  //If this directory doesn't contain a sub-directory, print the histograms within the category
  if(!subdir) status += print_hist(dir, tag, outdir); //histogram directory
  return status;
}

//------------------------------------------------------------------------------------------
// Print all fit figures
int plot_fit(TString fname, TString outdir = "figures", bool unblind = false) {
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
  status += print_dir(prefit, "prefit", outdir);
  if(fit_b) status += print_dir(fit_b , "fit_b" , outdir);
  if(fit_s) status += print_dir(fit_s , "fit_s" , outdir);

  cout << "Plotting status = " << status << endl;
  return status;
}
