//Plot fit results from fit diagnostics
//create a fit diagnostics root file via:
//$> combine -M FitDiagnostics -d <input card/workspace> --saveShapes --saveWithUncertainties [additional options]

bool unblind_      = false  ;
int  err_mode_     =  1     ; //errors in the pulls: 0: sqrt(data^2 + fit^2); 1: sqrt(data^2 - fit^2)
int  print_stacks_ = -1     ; //print stacked plots: 0 don't, 1 do, -1 only stacks
bool debug_        = false  ; //print debug info
bool do_single_    = false  ; //test printing a single histogram
bool do_run2_      = true   ; //print Run 2 inclusive histograms
bool only_run2_    = true   ; //skip individual year plots
TString only_mode_ = "fit_s"; //fit version to print ("" to ignore)
bool is_prelim_    = false  ;
TString file_type_ = "pdf"  ;


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

void draw_cms_label(double left_margin = 0.10) {
    //CMS prelim drawing
    TText cmslabel;
    cmslabel.SetNDC();
    cmslabel.SetTextColor(1);
    cmslabel.SetTextSize(0.11);
    cmslabel.SetTextAlign(11);
    cmslabel.SetTextAngle(0);
    cmslabel.SetTextFont(61);
    cmslabel.DrawText(left_margin, 0.915, "CMS");
    if(is_prelim_) {
      cmslabel.SetTextFont(52);
      cmslabel.SetTextSize(0.76*cmslabel.GetTextSize());
      cmslabel.DrawText(left_margin + 0.09, 0.915, "Preliminary");
    }
}

void draw_luminosity(int year = -1) {
  TLatex label;
  label.SetNDC();
  label.SetTextFont(42);
  // label.SetTextColor(1);
  label.SetTextSize(0.06);
  label.SetTextAlign(31);
  label.SetTextAngle(0);
  TString period = (year > 2000) ? Form("%i, ", year) : "";
  const double lum = (year == 2016) ? 36.33 : (year == 2017) ? 41.48 : (year == 2018) ? 59.83 : 137.64;
  label.DrawLatex(0.97, 0.915, Form("%s%.0f fb^{-1} (13 TeV)",period.Data(),lum));
}

void draw_category(TString tag, float left_margin = 0.10) {
  const bool zetau = tag.Contains("zetau");
  const bool hadronic = tag.Contains("_had_");
  TString cat = (zetau) ? "e#tau" : "#mu#tau";
  cat += (hadronic) ? "_{h}" : (zetau) ? "_{#mu}" : "_{e}";
  if(hadronic) {
    if     (tag.Contains("mva_25")) cat += ": 60 < m_{" + cat + "} < 85 GeV";
    else if(tag.Contains("mva_26")) cat += ": 100 < m_{" + cat + "} < 170 GeV";
    else if(tag.Contains("mva_28")) cat += ": 85 < m_{" + cat + "} < 100 GeV";
    else                            cat += ": 40 < m_{" + cat + "} < 60 GeV";
  } else {
    if     (tag.Contains("mva_25")) cat += ": 50 < m_{" + cat + "} < 100 GeV";
    else if(tag.Contains("mva_26")) cat += ": 100 < m_{" + cat + "} < 170 GeV";
    else                            cat += ": 40 < m_{" + cat + "} < 50 GeV";
  }
  TLatex label;
  label.SetNDC();
  label.SetTextFont(42);
  label.SetTextSize(0.060);
  label.SetTextAlign(11);
  label.SetTextAngle(0);
  label.DrawLatex(left_margin + 0.04, 0.58, cat.Data());
}

void replace_bin_labels(TH1* h, const int neff_bins) {
  if(!h) return;
  // change bin numbers to text labels
  const int nbins = h->GetNbinsX();
  for(int bin = 1; bin <= nbins; ++bin) {
    if(neff_bins < 10 || (bin-1) % 2 == 0) h->GetXaxis()->SetBinLabel(bin, Form("%i", bin-1));
    else                                   h->GetXaxis()->SetBinLabel(bin, Form(" "));
  }
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
  if(g) {
    //no x errors
    for(int ipoint = 0; ipoint < g->GetN(); ++ipoint) {
      g->SetPointEXhigh(ipoint, 0.);
      g->SetPointEXlow (ipoint, 0.);
    }
  }
  return g;
}

//------------------------------------------------------------------------------------------
// Print an individual stack plot
int print_stack(vector<TDirectoryFile*> dirs, TString tag, TString outdir) {

  //Get the fit results and the data
  TH1* hbackground         = get_hist(dirs, "total_background");
  TH1* hsignal             = get_hist(dirs, "total_signal");
  TH1* htotal              = get_hist(dirs, "total");
  TGraphAsymmErrors* gdata = get_data(dirs);
  if(!hsignal || !hbackground || !htotal || !gdata) {
    cout << "Data not found for tag " << tag.Data() << endl;
    return 1;
  }
  const int year = (tag.Contains("2016")) ? 2016 : (tag.Contains("2017")) ? 2017 : (tag.Contains("2018")) ? 2018 : -1;

  //Build the stack
  THStack* stack = new THStack("hstack", ("stack_" + tag).Data());
  vector<TString> names  = {"HiggsBkg"               , "Top"       , "OtherVB"   , "ZToeemumu"     , "Embedding", "MisID"               , "QCD"           };
  vector<TString> titles = {"H#rightarrow#tau#tau,WW", "Top quark" , "W, multi-V", "Z#rightarrowll", "#tau#tau" , "j#rightarrow#tau_{h}", "nonprompt e#mu"};
  vector<int>     colors = {kCMSColor_9              , kCMSColor_10, kCMSColor_8 , kCMSColor_6     , kCMSColor_7, kCMSColor_1           , kCMSColor_2     };
  for(unsigned i = 0; i < names.size(); ++i) {
    TString name = names[i];
    auto h = get_hist(dirs, name.Data());
    if(!h && names[i] != "QCD" && names[i] != "MisID") { //add zero rate processes
      h = (TH1*) hbackground->Clone((names[i]+"_"+tag).Data());
      h->Reset();
    }
    if(h) {
      const int color = colors[i];
      h->SetLineColor(color);
      h->SetFillColor(color);
      h->SetTitle(titles[i].Data());
      stack->Add(h);
    }
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
  TCanvas* c = new TCanvas("c_stack", "c_stack", 1000, 900);
  const float x1(0.23), x2(0.43);
  TPad* pad1 = new TPad("pad1_stack", "pad1_stack", 0., x2, 1., 1.);
  TPad* pad2 = new TPad("pad2_stack", "pad2_stack", 0., x1, 1., x2);
  TPad* pad3 = new TPad("pad3_stack", "pad3_stack", 0., 0., 1., x1);
  pad1->SetRightMargin (0.03); pad2->SetRightMargin (pad1->GetRightMargin()); pad3->SetRightMargin(pad1->GetRightMargin());
  pad1->SetLeftMargin  (0.13); pad2->SetLeftMargin  (pad1->GetLeftMargin ()); pad3->SetLeftMargin (pad1->GetLeftMargin ());
  pad1->SetBottomMargin(0.03); pad2->SetBottomMargin(0.09); pad3->SetBottomMargin(0.33);
  pad1->SetTopMargin   (0.10); pad2->SetTopMargin   (0.08); pad3->SetTopMargin   (0.05);
  pad1->Draw(); pad2->Draw(); pad3->Draw();

  // Draw the data and fit components
  pad1->cd();

  //Configure the data style
  gdata->SetMarkerStyle(20);
  gdata->SetMarkerSize(1.7);
  gdata->SetMarkerColor(kBlack);
  gdata->SetLineColor(kBlack);
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
  htotal->GetYaxis()->SetTitleSize(0.08);
  htotal->GetYaxis()->SetTitleOffset(0.70);
  htotal->GetXaxis()->SetLabelSize(0.);
  htotal->GetYaxis()->SetLabelSize(0.065);

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
  stack->Draw("hist noclear same");
  htotal->Draw("E1 same");
  htotal->Draw("E2 same");
  if(unblind_) {
    hbackground->Draw("hist same");
    hsignal->Draw("hist same");
  }
  gdata->Draw("PZ");
  htotal->GetXaxis()->SetRangeUser(xmin, xmax);
  const int neff_bins = htotal->FindBin(xmax-1.e-3) - htotal->FindBin(xmin+1.e-3) + 1;
  const int ndivisions = (neff_bins > 10) ? 200 + (neff_bins+1) / 2 : neff_bins;
  htotal->GetXaxis()->SetNdivisions(ndivisions);


  //Add a legend for the summary components and one for the background stack
  TLegend leg_sum(pad1->GetLeftMargin() + 0.03, 0.63, pad1->GetLeftMargin() + 0.29, 0.88);
  leg_sum.AddEntry(gdata, "Data", "PE");
  leg_sum.AddEntry(htotal, "Total", "LF");
  if(unblind_) {
    leg_sum.AddEntry(hbackground, "Background", "L");
    leg_sum.AddEntry(hsignal, "Signal", "L");
  }
  TLegend leg_bkg(pad1->GetLeftMargin() + 0.30, 0.63, 1. - 0.04 - pad1->GetRightMargin(), 0.88);
  leg_bkg.SetNColumns(2);
  for(auto h : *(stack->GetHists())) leg_bkg.AddEntry(h);

  leg_sum.SetTextSize(0.06);
  leg_sum.SetFillStyle(0);
  leg_sum.SetFillColor(0);
  leg_sum.SetLineColor(0);
  leg_sum.SetLineStyle(0);
  leg_sum.Draw();
  leg_bkg.SetTextSize(0.06);
  leg_bkg.SetFillStyle(0);
  leg_bkg.SetFillColor(0);
  leg_bkg.SetLineColor(0);
  leg_bkg.SetLineStyle(0);
  leg_bkg.Draw();

  pad1->Draw(); //redraw the pad
  gPad->RedrawAxis();

  //Make the ratio plots
  pad2->cd();

  //Make the data / total fit and data / background component distributions
  TGraphAsymmErrors* gRatio_s = (TGraphAsymmErrors*) gdata->Clone("gRatio_s_stack"); //data / S+B
  TGraphAsymmErrors* gRatio_b = (TGraphAsymmErrors*) gdata->Clone("gRatio_b_stack"); //data / B-only
  TH1* hPull_s = (TH1*) htotal->Clone("hPull_s_stack"); hPull_s->Reset(); //(data - model)/unc (S+B-only)
  TH1* hPull_b = (TH1*) htotal->Clone("hPull_b_stack"); hPull_b->Reset(); //(data - model)/unc (B-only)
  //Debug printout if needed
  if(debug_) {
    printf("                           Bin  :     data   sqrt(n)         B +-  sigma_B       S+B +- sigma_S+B  pull_B  pull_S+B\n");

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
    const double err_s      = (err_mode_ == 0) ? sqrt(tot_e*tot_e + data_err_s*data_err_s) : (tot_e > data_err_s) ? 0. : sqrt(data_err_s*data_err_s - tot_e*tot_e);
    const double err_b      = (err_mode_ == 0) ? sqrt(bkg_e*bkg_e + data_err_b*data_err_b) : (bkg_e > data_err_b) ? 0. : sqrt(data_err_b*data_err_b - bkg_e*bkg_e);
    if(!std::isfinite(err_s) || !std::isfinite(err_b)) printf(" %s: %s bin %i has non-finite error(s): err_s = %f; err_b = %f\n",
                                                              __func__, tag.Data(), bin, err_s, err_b);
    const double pull_s     = (err_s > 0.) ? (y - tot_v) / err_s : 0.;
    const double pull_b     = (err_b > 0.) ? (y - bkg_v) / err_b : 0.;

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
      printf("%25s bin %2i: %8.0f (%8.2f) %8.1f +- %8.2f  %8.1f +- %8.2f   %5.2f    %5.2f\n",
             tag.Data(), bin+1, y, sqrt(y), bkg_v, bkg_e, tot_v, tot_e, pull_b, pull_s);
    }
    if(std::fabs(pull_s) > 3.f) {
      printf(">>> High pull!\n");
      printf("                           Bin  :     data   sqrt(n)       S+B +- sigma_S+B  Delta +- err     pull_S+B\n");
      //          fit_s_had_Run2_mva_28 bin  2:    13225 (  115.00)  13312.7 +-   112.54   -87.7 +-  23.64   -3.71

      printf("%25s bin %2i: %8.0f (%8.2f) %8.1f +- %8.2f  %6.1f +- %6.2f   %5.2f\n",
             tag.Data(), bin+1, y, sqrt(y), tot_v, tot_e, y-tot_v, err_s, pull_s);
    }
  }

  auto gRatio = (unblind_) ? gRatio_b : gRatio_s; //Ratio plot to use, data/total if blinded, data/bkg-only otherwise

  //Make a background uncertainty plot
  TH1* hBkg_unc = (TH1*) hbackground->Clone("hBkg_unc_stack");
  for(int ibin = 1; ibin <= hbackground->GetNbinsX(); ++ibin) {
    if(hbackground->GetBinContent(ibin) <= 0.01) hBkg_unc->SetBinContent(ibin, 0.);
    else {
      hBkg_unc->SetBinContent(ibin, 1.);
      if(hbackground->GetBinError(ibin) > 0.) {
        hBkg_unc->SetBinError(ibin, hbackground->GetBinError(ibin) / hbackground->GetBinContent(ibin));
      } else { //if the error isn't defined, set it to 1%
        hBkg_unc->SetBinError(ibin, 0.01 * hbackground->GetBinContent(ibin));
      }
    }
  }
  hBkg_unc->SetFillColor(kGray+3);
  hBkg_unc->SetLineColor(kGray+3);
  hBkg_unc->SetFillStyle(3003);

  //Make a total / background component distribution
  TH1* hRatio_s = (TH1*) htotal->Clone("hRatio_s_stack");
  hRatio_s->Divide(hbackground);
  for(int ibin = 1; ibin <= hsignal->GetNbinsX(); ++ibin) {
    if(hbackground->GetBinContent(ibin) <= 0.1) hRatio_s->SetBinContent(ibin, 0.);
  }
  hRatio_s->SetFillColor(0);
  hRatio_s->SetLineColor(kBlue);
  hRatio_s->SetLineStyle(kDashed);
  hRatio_s->SetLineWidth(3);

  //Draw the results
  hBkg_unc->Draw("E2");
  if(unblind_) {
    hRatio_s->Draw("hist same");
    hRatio_s->GetYaxis()->SetRangeUser(0.9, 1.1); //necessary due to y-axis importing from clone
  }
  gRatio->SetLineColor(gdata->GetLineColor());
  gRatio->SetLineWidth(gdata->GetLineWidth());
  gRatio->SetMarkerColor(gdata->GetMarkerColor());
  gRatio->SetMarkerStyle(gdata->GetMarkerStyle());
  gRatio->SetMarkerSize(gdata->GetMarkerSize());
  gRatio->Draw("PEZ");
  hBkg_unc->GetXaxis()->SetRangeUser(xmin, xmax);


  //Add a reference line for perfect agreement
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->SetLineStyle(kDashed);
  line->Draw("same");

  //Configure the titles and axes
  float txt_scale = (1.-x2)/(x2-x1);
  hBkg_unc->GetYaxis()->SetRangeUser(0.9, 1.1);
  hBkg_unc->SetTitle("");
  hBkg_unc->SetXTitle("");
  hBkg_unc->GetXaxis()->SetLabelSize(0.);
  hBkg_unc->GetYaxis()->SetNdivisions(205);
  hBkg_unc->GetXaxis()->SetNdivisions(ndivisions);
  hBkg_unc->GetYaxis()->SetLabelSize(txt_scale*htotal->GetYaxis()->GetLabelSize());
  hBkg_unc->GetYaxis()->SetTitleSize(txt_scale*htotal->GetYaxis()->GetTitleSize());
  hBkg_unc->GetYaxis()->SetTitleOffset(0.23);
  if(unblind_) hBkg_unc->SetYTitle("Data/Bkg");
  else         hBkg_unc->SetYTitle("Data/Fit");


  //Make a pull plot
  pad3->cd();

  auto hPull = hPull_s;
  replace_bin_labels(hPull, neff_bins);
  const double max_pull = hPull->GetMaximum();
  const double min_pull = hPull->GetMinimum();
  hPull->SetLineColor(kAtlantic);
  hPull->SetFillColor(kAtlantic);
  hPull->SetFillStyle(1000);
  hPull->Draw("hist");
  hPull->GetYaxis()->SetRangeUser(min(-3., 1.1*min_pull),max(3., 1.1*max_pull));
  hPull->SetTitle("");
  hPull->SetXTitle("BDT score bin");
  // hPull->SetYTitle("#frac{Data-Fit}{#sigma}");
  // hPull->GetYaxis()->SetTitleOffset(0.22);
  hPull->SetYTitle("Pull");
  // if(err_mode_ == 1) hPull->SetYTitle("#frac{(Data-Fit)}{#sqrt{#sigma_{Data}^{2} - #sigma_{Fit}^{2}}}");

  txt_scale = (1.-x2)/(x1);
  hPull->GetXaxis()->SetLabelOffset(0.012);
  hPull->GetXaxis()->SetLabelSize(1.5*txt_scale*htotal->GetYaxis()->GetLabelSize());
  hPull->GetYaxis()->SetNdivisions(505);
  hPull->GetXaxis()->SetNdivisions(ndivisions);
  hPull->GetYaxis()->SetLabelSize(hPull->GetXaxis()->GetLabelSize());
  hPull->GetXaxis()->SetTitleSize(txt_scale*htotal->GetYaxis()->GetTitleSize());
  hPull->GetYaxis()->SetTitleSize(hPull->GetXaxis()->GetTitleSize());
  hPull->GetXaxis()->SetTitleOffset(0.70);
  hPull->GetYaxis()->SetTitleOffset(0.27);

  //Add a reference line for perfect agreement
  TLine* line_2 = new TLine(xmin, 0., xmax, 0.);
  line_2->SetLineColor(kBlack);
  line_2->SetLineWidth(2);
  line_2->SetLineStyle(kDashed);
  line_2->Draw("same");

  //Add the CMS label
  pad1->cd();
  draw_cms_label(pad1->GetLeftMargin());
  draw_luminosity(year);
  draw_category(outdir + tag, pad1->GetLeftMargin());

  //Print a linear and a log version of the distribution
  double min_val = std::max(0.1, std::min(gmin(gdata), hmin(htotal)));
  double max_val = std::max(gdata->GetMaximum(), hmax(htotal));
  htotal->GetYaxis()->SetRangeUser(0., 1.75*max_val);
  c->SaveAs(Form("%s%s_stack.%s", outdir.Data(), tag.Data(), file_type_.Data()));
  c->SaveAs(Form("%s%s_stack.root", outdir.Data(), tag.Data()));
  double plot_min = 0.01*min_val;
  double plot_max = plot_min*pow(10, 1.75*log10(max_val/plot_min));
  htotal->GetYaxis()->SetRangeUser(plot_min, plot_max);
  pad1->SetLogy();
  c->SaveAs(Form("%s%s_stack_logy.%s", outdir.Data(), tag.Data(), file_type_.Data()));
  c->SaveAs(Form("%s%s_stack_logy.root", outdir.Data(), tag.Data()));

  //Clean up after printing
  delete c;
  delete gRatio_s;
  delete gRatio_b;
  delete hPull_s;
  delete hPull_b;
  delete hBkg_unc;
  delete hRatio_s;
  delete stack;
  delete line;
  delete line_2;

  return 0;
}

//------------------------------------------------------------------------------------------
// Print an individual histogram
int print_hist(vector<TDirectoryFile*> dirs, TString tag, TString outdir) {

  //Get the fit results and the data
  TH1* hbackground         = get_hist(dirs, "total_background");
  TH1* hsignal             = get_hist(dirs, "total_signal");
  TH1* htotal              = get_hist(dirs, "total");
  TGraphAsymmErrors* gdata = get_data(dirs);
  if(!hsignal || !hbackground || !htotal || !gdata) {
    cout << "Data not found for tag " << tag.Data() << endl;
    return 1;
  }
  const int year = (tag.Contains("2016")) ? 2016 : (tag.Contains("2017")) ? 2017 : (tag.Contains("2018")) ? 2018 : -1;

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
  pad2->SetTopMargin(0.03); pad3->SetTopMargin(0.05);
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
  htotal->GetYaxis()->SetTitleSize(0.05);
  htotal->GetYaxis()->SetTitleOffset(0.92);
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
    const double err_s      = (tot_e <= 0.) ? 0. : (err_mode_ == 0) ? sqrt(tot_e*tot_e + data_err_s*data_err_s) : sqrt(data_err_s*data_err_s - tot_e*tot_e);
    const double err_b      = (bkg_e <= 0.) ? 0. : (err_mode_ == 0) ? sqrt(bkg_e*bkg_e + data_err_b*data_err_b) : sqrt(data_err_b*data_err_b - bkg_e*bkg_e);
    if(!std::isfinite(err_s) || !std::isfinite(err_b)) printf(" %s: %s bin %i has non-finite error(s): err_s = %f; err_b = %f\n",
                                                              __func__, tag.Data(), bin, err_s, err_b);
    const double pull_s     = (tot_e > 0.) ? (y - tot_v) / err_s : 0.;
    const double pull_b     = (bkg_e > 0.) ? (y - bkg_v) / err_b : 0.;

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
  hBkg_unc->Draw("E2");
  if(unblind_) {
    hRatio_s->Draw("hist same");
    hRatio_s->GetYaxis()->SetRangeUser(0.9, 1.1); //necessary due to y-axis importing from clone
  }
  gRatio->Draw("P");
  hBkg_unc->GetXaxis()->SetRangeUser(xmin, xmax);

  //Add a reference line for perfect agreement
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->SetLineStyle(kDashed);
  line->Draw("same");

  //Configure the titles and axes
  hBkg_unc->GetYaxis()->SetRangeUser(0.9, 1.1);
  hBkg_unc->SetTitle("");
  hBkg_unc->SetXTitle("");
  hBkg_unc->GetXaxis()->SetLabelSize(0.);
  hBkg_unc->GetYaxis()->SetLabelSize(0.10);
  hBkg_unc->GetYaxis()->SetTitleSize(0.15);
  hBkg_unc->GetYaxis()->SetTitleOffset(0.30);
  if(unblind_) hBkg_unc->SetYTitle("Data/Bkg");
  else         hBkg_unc->SetYTitle("Data/Fit");


  //Make a pull plot
  pad3->cd();

  auto hPull = hPull_s;
  const double max_pull = hPull->GetMaximum();
  const double min_pull = hPull->GetMinimum();
  hPull->SetLineColor(kAtlantic);
  hPull->SetFillColor(kAtlantic);
  hPull->SetFillStyle(1000);
  hPull->Draw("hist");
  hPull->GetYaxis()->SetRangeUser(min(-3., 1.1*min_pull),max(3., 1.1*max_pull));
  hPull->SetTitle("");
  hPull->SetXTitle("BDT score bin");
  hPull->SetYTitle("#frac{Data-Fit}{Uncertainty}");
  hPull->GetXaxis()->SetLabelSize(0.10);
  hPull->GetYaxis()->SetLabelSize(0.10);
  hPull->GetXaxis()->SetTitleSize(0.15);
  hPull->GetYaxis()->SetTitleSize(0.15);
  hPull->GetXaxis()->SetTitleOffset(0.75);
  hPull->GetYaxis()->SetTitleOffset(0.29);

  //Add a reference line for perfect agreement
  TLine* line_2 = new TLine(xmin, 0., xmax, 0.);
  line_2->SetLineColor(kBlack);
  line_2->SetLineWidth(2);
  line_2->SetLineStyle(kDashed);
  line_2->Draw("same");

  //Add the CMS label
  pad1->cd();
  draw_cms_label(pad1->GetLeftMargin());
  draw_luminosity(year);

  //Print a linear and a log version of the distribution
  double min_val = std::max(0.1, std::min(gmin(gdata), hmin(htotal)));
  double max_val = std::max(gdata->GetMaximum(), hmax(htotal));
  htotal->GetYaxis()->SetRangeUser(0., 1.2*max_val);
  c->SaveAs(Form("%s%s.%s", outdir.Data(), tag.Data(), file_type_.Data()));
  c->SaveAs(Form("%s%s.root", outdir.Data(), tag.Data()));
  double plot_min = std::min(std::max(0.2, 0.2*hmax(hsignal)), 0.2*min_val);
  double plot_max = plot_min*pow(10, 1.7*log10(max_val/plot_min));
  htotal->GetYaxis()->SetRangeUser(plot_min, plot_max);
  pad1->SetLogy();
  c->SaveAs(Form("%s%s_logy.%s", outdir.Data(), tag.Data(), file_type_.Data()));
  c->SaveAs(Form("%s%s_logy.root", outdir.Data(), tag.Data()));

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
      auto next_dir = (TDirectoryFile*) obj;
      if(!only_run2_) {
        status += print_dir(next_dir, (tag + "_") + obj->GetName(), outdir);
        if(do_single_) return status;
      }
      subdir = true;
      if(do_run2_ && tag.Contains(only_mode_)) {
        TString dir_name = next_dir->GetName();
        if(dir_name.Contains("2018")) { //a sub-directory to print, also print the inclusive
          vector<TDirectoryFile*> dirs = {next_dir};
          dir_name.ReplaceAll("2018", "2017");
          next_dir = (TDirectoryFile*) dir->Get(dir_name.Data());
          if(next_dir) dirs.push_back(next_dir);
          dir_name.ReplaceAll("2017", "2016");
          next_dir = (TDirectoryFile*) dir->Get(dir_name.Data());
          if(next_dir) dirs.push_back(next_dir);
          dir_name.ReplaceAll("y2016", "Run2");
          if(is_prelim_) dir_name += "_prelim";
          if(print_stacks_ >= 0) status += print_hist(dirs, tag + "_" + dir_name, outdir);
          if(print_stacks_ != 0) status += print_stack(dirs, tag + "_" + dir_name, outdir); //stacked histogram
          if(do_single_) return status;
        }
      }
    }
  }

  //If this directory doesn't contain a sub-directory, print the histograms within the category
  if(!subdir && tag.Contains(only_mode_)) { //histogram directory
    if(is_prelim_) tag += "_prelim";
    if(print_stacks_ >= 0) status += print_hist({dir}, tag, outdir);
    if(print_stacks_ != 0) status += print_stack({dir}, tag, outdir); //stacked histogram
  }
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

  // Reduce number of entries listed in linear, move exponent
  TGaxis::SetMaxDigits(3);
  TGaxis::SetExponentOffset(-0.06, 0.01, "Y");

  //Print the fit configurations: Pre-fit, background-only fit, and background+signal fit
  int status(0);
  status += print_dir(prefit, "prefit", outdir);
  if(true || !do_single_) {
    if(fit_b) status += print_dir(fit_b , "fit_b" , outdir);
    if(fit_s) status += print_dir(fit_s , "fit_s" , outdir);
  }

  cout << "Plotting status = " << status << endl;
  return status;
}
