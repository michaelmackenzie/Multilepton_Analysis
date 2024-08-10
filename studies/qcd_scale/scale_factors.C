//Script to determine the MC efficiency of b-tags
//by generator-level quark information

using namespace CLFV;
TString selection_;

#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int  verbose_      = 0;
int  drawFit_      = 1; //whether to draw the linear fits
bool doPCA_        = true; //whether or not to generate shifted function templates
bool scalePCA_     = true; //scale the PCA transform by max(1, sqrt(chi^2/dof)) to increase the uncertainties
bool useFTest_     = true; //use an F-test to decide the fit order
int  defaultOrder_ = 3; //if not using an F-test, use this fit order
int  rebin_        = 1;

//-------------------------------------------------------------------------------------------------------------------------------
// Perform an F-test
bool perform_f_test(double chisq_1, int ndof_1, double chisq_2, int ndof_2) {
  if(chisq_1 < 0. || ndof_1 <= 0) return true; //pick the higher order function
  if(chisq_2 < 0. || ndof_2 <= 0) return false; //stick with the original function
  if(ndof_1 == ndof_2) return chisq_1 > chisq_2; //if equal degrees of freedom, pick the better chi^2 model
  if(ndof_1 < ndof_2) {
    cout << __func__ << ": Error, higher order has higher N(dof)!\n";
    return false;
  }

  double p_of_f = 1.;
  //use the difference of chi^2 p-value given N(dof) = Delta N(dof)
  const double ftest = chisq_1 - chisq_2;
  if(ftest < 0.) p_of_f = 1.; //lower order is a better fit
  else           p_of_f = ROOT::Math::chisquared_cdf_c(ftest, ndof_1 - ndof_2);
  printf("F-test: chisq_1 = %.3f / %i; chisq_2 = %.3f / %i --> p(F) = %.3e\n",
         chisq_1, ndof_1, chisq_2, ndof_2, p_of_f);
  return (p_of_f < 0.05); //select a higher order if 95% confidence it's better
}

//-------------------------------------------------------------------------------------------------------------------------------
// Smooth a 1D histogram
TH1* smooth_hist(TH1* h) {
  if(!h) return nullptr;
  TH1* hsmooth = (TH1*) h->Clone(Form("%s_smooth", h->GetName()));
  const static int nsmooth = 1; //number of times to run the smoothing algorithm
  hsmooth->Smooth(nsmooth);
  return hsmooth;
}

//Get 1D histograms
TH1* get_histogram(TString name, int setAbs, int isdata, TString type = "event") {
  TH1* h = nullptr;
  if(isdata) {
    h = dataplotter_->get_data(name, type, setAbs);
  } else {
    THStack* stack = dataplotter_->get_stack(name, type, setAbs);
    if(!stack || stack->GetNhists() <= 0) {
      cout << __func__ << ": Background stack " << name.Data() << " not found!\n";
      return nullptr;
    }
    h = (TH1*) stack->GetStack()->Last()->Clone(Form("h_%s_%s_%i", name.Data(), type.Data(), setAbs));
  }

  if(!h) return nullptr;

  //setup the histogram title and axis titles
  h->SetTitle("");
  if(name.Contains("deltar"))
    h->SetXTitle("#DeltaR");
  else if(name.Contains("deltaphi"))
    h->SetXTitle("#Delta#phi");
  h->SetYTitle("");
  if(rebin_ > 1) h->Rebin(rebin_);

  return h;
}

//Get 2D histograms
TH2* get_2D_histogram(TString name, int setAbs, int isdata, TString type = "event") {
  TH2* h = 0;
  const unsigned nfiles = dataplotter_->inputs_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    auto input = dataplotter_->inputs_[d];
    if(input.isData_ != (isdata > 0)) continue;
    TString hpath = Form("%s_%i/%s", type.Data(), setAbs, name.Data());
    if(verbose_ > 0) cout << "Retrieving histogram " << hpath.Data() << " for " << input.name_.Data() << endl;
    TH2* hTmp = (TH2*) input.data_->Get(hpath.Data());
    if(!hTmp) {
      if(verbose_ > 0) cout << "Histogram " << name.Data() << " for " << input.name_.Data() << " not found!\n";
      continue;
    }
    hTmp = (TH2*) hTmp->Clone("hTmp");
    // ignore luminosity/normalization scaling --> 1 entry ~ weight of 1
    if(!isdata) hTmp->Scale(input.scale_);
    if(!h) h = (TH2*) hTmp->Clone(Form("h_%s_%s_%i", name.Data(), type.Data(), setAbs));
    else h->Add(hTmp);
    delete hTmp;
  }

  if(!h) return NULL;

  //setup the histogram title and axis titles
  h->SetTitle("");
  h->SetXTitle("#eta");
  if(name.Contains("delr"))
    h->SetYTitle("#DeltaR");
  else if(name.Contains("delphi"))
    h->SetYTitle("#Delta#phi");

  return h;
}

//Get the QCD histogram for a given histogram name and set
TH1* get_qcd_histogram(TString name, int setAbs, TString type = "event") {
  TH1* hQCD = get_histogram(name, setAbs, 1/*is data*/, type);
  if(!hQCD) {
    cout << "QCD histogram building for " << name << " failed!\n";
    return 0;
  }
  hQCD = (TH2*) hQCD->Clone(Form("QCD_%s_%i", name.Data(), setAbs));
  dataplotter_->include_qcd_ = 0;
  hQCD->Add(get_histogram(name, setAbs, 0/*is MC*/, type), -1.);

  //Force the QCD distribution to be positive while preserving the integral
  const double norm = hQCD->Integral();
  for(int ibin = 1; ibin <= hQCD->GetNbinsX(); ++ibin) {
    if(hQCD->GetBinContent(ibin) < 0.) {
      std::cout << __func__ << ": Warning! " << name.Data() << ": QCD bin " << ibin << " < 0, setting to 0!\n";
      hQCD->SetBinContent(ibin, 0.);
    }
  }
  hQCD->Scale(norm/hQCD->Integral());
  return hQCD;
}

//Get the 2D QCD histogram for a given histogram name and set
TH2* get_2D_qcd_histogram(TString name, int setAbs, TString type = "event") {
  TH2* hQCD = get_2D_histogram(name, setAbs, 1/*is data*/, type);
  if(!hQCD) {
    cout << "QCD histogram building for " << name << " failed!\n";
    return 0;
  }
  hQCD = (TH2*) hQCD->Clone(Form("QCD_%s_%i", name.Data(), setAbs));
  dataplotter_->include_qcd_ = 0;
  hQCD->Add(get_2D_histogram(name, setAbs, 0/*is MC*/), -1.);

  //Force the QCD distribution to be positive while preserving the integral
  const double norm = hQCD->Integral();
  for(int xbin = 1; xbin <= hQCD->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hQCD->GetNbinsY(); ++ybin) {
      if(hQCD->GetBinContent(xbin,ybin) < 0.) {
        std::cout << __func__ << ": Warning! QCD bin (" << xbin << ", " << ybin << ") < 0, setting to 0!\n";
        hQCD->SetBinContent(xbin, ybin, 0.);
      }
    }
  }
  hQCD->Scale(norm/hQCD->Integral());
  return hQCD;
}

//make 1D ratio plots
TCanvas* make_ratio_canvas(TH1* hnum, TH1* hdnm, TF1 *&f1, TString print_name = "", bool doFit = true,
                           double xmin = 0., double xmax = 5., int rebin = 1) {
  TCanvas* c = new TCanvas("c_ratio", "c_ratio", 800, 600);
  TVirtualPad* pad;
  TH1* hRatio = (TH1*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  TH1* hdnm_rb = (TH1*) hdnm->Clone("htmp");
  if(rebin > 1) {
    hRatio->Rebin(rebin);
    hdnm_rb->Rebin(rebin);
  }
  hRatio->Divide(hdnm_rb);
  delete hdnm_rb;

  //set bin error to 0 for <= bins to be ignored in the fit
  for(int jbin = 1; jbin <= hRatio->GetNbinsX(); ++jbin) {if(hRatio->GetBinContent(jbin) <= 0.) hRatio->SetBinError(jbin,0);}
  TString fit_option = (verbose_ > 1) ? "S" : "S q";
  if(!drawFit_ || !doFit) {
    fit_option += " 0";
    hRatio->Draw("E1");
  }
  TF1* f = 0;
  TFitResultPtr fitres;
  double fit_min(1.75), fit_max(4.75); //perform the fit in 1.5 < R < 5, to avoid fluctuations
  int nfit_bins = hnum->FindBin(fit_max) - hnum->FindBin(fit_min) + 1.;
  if(nfit_bins <= 1) { //check if the bounds make sense
    fit_min = 1.; fit_max = -1.; nfit_bins = hnum->GetNbinsX();
  }
  if(doFit) {
    //perform an F-test to decide the order of the fit
    double chisq_prev(1.e20);
    int ndof_prev(1);
    const int min_order((useFTest_) ? 1 : defaultOrder_), max_order((useFTest_) ? 4 : defaultOrder_);
    int best_order = min_order; //store the highest order passing the F-test (if applied)
    for(int order = min_order; order <= max_order+1; ++order) {
      if(!useFTest_ && order > max_order) break; //if not using the F-test, only do the single fit
      //use an extra order to re-fit the best order
      const int current_order = (order > max_order) ? best_order : order;
      TString fit_func = Form("pol%i(0)", current_order);
      f = new TF1("fit_func", fit_func.Data(), 0., 10.);
      f->SetParameter(0, 1.5);
      // f->SetParameters(1.7, -0.1, 0.25, 3.2, 0.4);
      int count = 0;
      bool refit = false;
      do {
        if(refit) cout << __func__ << ": Refitting order " << current_order << " for " << hnum->GetName() << ", " << hdnm->GetName() << endl;
        if(fit_min < fit_max)
          fitres = hRatio->Fit(f, fit_option.Data(), "", fit_min, fit_max);
        else
          fitres = hRatio->Fit(f, fit_option.Data());
        refit = !fitres || fitres->Status() != 0;
        refit &=  count < 5;
        ++count;
      } while(refit);

      //determine if the best order has been found
      if(order <= max_order) {
        const double chisq = f->GetChisquare();
        const double ndof  = (std::max(1, nfit_bins - f->GetNpar()));
        if(!useFTest_ || chisq_prev > 1.e10 || perform_f_test(chisq_prev, ndof_prev, chisq, ndof)) { //select the higher order
          chisq_prev = chisq;
          ndof_prev = ndof;
          best_order = order;
        }
      }
    } //end F-test loop
    cout << "For histogram " << hnum->GetName() << " selected fit order " << best_order << endl;
  }
  hRatio->SetLineWidth(2);
  hRatio->SetLineColor(kBlue);
  hRatio->SetMarkerStyle(20);
  hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  hRatio->GetYaxis()->SetRangeUser(0., min(5., 1.5*hRatio->GetMaximum()));
  hRatio->SetYTitle("OS/SS");
  f1 = f;
  if(f1 && drawFit_ && doFit) {
    hRatio->Draw("E1");
    TH1* herr_2s = (TH1*) hRatio->Clone(Form("%s_err_2s", hRatio->GetName()));
    herr_2s->Reset();
    herr_2s->SetMarkerSize(0.);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_2s, 0.95);
    herr_2s->SetFillColor(kYellow);
    herr_2s->SetFillStyle(1001);
    herr_2s->Draw("E3 same");
    TH1* herr_1s = (TH1*) hRatio->Clone(Form("%s_err_1s", hRatio->GetName()));
    herr_1s->Reset();
    herr_1s->SetMarkerSize(0.);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_1s, 0.68);
    herr_1s->SetFillColor(kGreen);
    herr_1s->SetFillStyle(1001);
    herr_1s->Draw("E3 same");
    hRatio->Draw("E1 same"); //add again on top of confidence intervals
    f1->Draw("same");
    if(print_name != "") {
      herr_1s->SetName(print_name.Data());
      herr_1s->Write();
    }

    if(doPCA_) { //perform PCA to get N up/down shifted templates
      TMatrixDSym cov(fitres->GetCovarianceMatrix());
      const int nparams = cov.GetNrows();
      TVectorD params(nparams);
      for(int iparam = 0; iparam < nparams; ++iparam) params[iparam] = f1->GetParameter(iparam);
      const double nsigma = (scalePCA_) ? std::max(1., std::sqrt(f->GetChisquare()/(std::max(1, nfit_bins - f->GetNpar())))) : 1.;
      auto shifts = Utilities::PCAShifts(cov, params, nsigma);
      auto up = shifts.first;
      auto down = shifts.second;
      const int colors[] = {kViolet, kCyan+3, kOrange, kAtlantic, kYellow-3, kGreen, kBlue};
      // const int colors[] = {kViolet, kGreen, kOrange, kYellow-3, kAtlantic, kBlue};
      const int ncolors = sizeof(colors) / sizeof(*colors);
      for(int ishift = 0; ishift < nparams; ++ishift) {
        TF1* f_up = (TF1*) f1->Clone(Form("%s_%i_up", print_name.Data(), ishift));
        for(int iparam = 0; iparam < nparams; ++iparam) f_up->SetParameter(iparam, up[ishift][iparam]);
        TF1* f_down = (TF1*) f1->Clone(Form("%s_%i_down", print_name.Data(), ishift));
        for(int iparam = 0; iparam < nparams; ++iparam) f_down->SetParameter(iparam, down[ishift][iparam]);
        const int color = colors[ishift % ncolors];
        f_up->SetLineColor(color);
        f_up->SetLineStyle(kDashed);
        f_down->SetLineColor(color);
        f_down->SetLineStyle(kDashed);
        f_up->Draw("same");
        f_down->Draw("same");
        f_up->Write();
        f_down->Write();
      }
    }
    //Draw the fit parameter results
    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    label.SetTextSize(0.04);
    label.SetTextAlign(13);
    label.SetTextAngle(0);
    label.DrawLatex(0.15, 0.89, Form("#chi^{2}/NDF  %.3f / %i (p = %.3f)", f->GetChisquare(), nfit_bins - f->GetNpar(),
                                     TMath::Prob(f->GetChisquare(), hRatio->GetNbinsX() - f->GetNpar())));
    for(int ipar = 0; ipar < f->GetNpar(); ++ipar) {
      label.DrawLatex(0.15, 0.89-0.05*(ipar+1), Form("%-10s %.4g +- %.5g\n", f->GetParName(ipar), f->GetParameter(ipar), f->GetParError(ipar)));
    }
  } else {
    gStyle->SetOptFit(0);
  }
  if(!f1 && doFit) cout << "Error! Fit function not found!\n";
  return c;
}

//make 2D ratio plots
TCanvas* make_2D_ratio_canvas(TH2* hnum, TH2* hdnm,
                              double xmin = 0., double xmax = 2.5,
                              double ymin = 0., double ymax = 5.,
                              bool binc = true, TString xl = "", TString yl = "") {
  TCanvas* c = new TCanvas("c_2D_ratio", "c_2D_ratio", 800, 600);
  TVirtualPad* pad;
  TH2* hRatio = (TH2*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  hRatio->Divide(hdnm);

  for(int xbin = 1; xbin <= hRatio->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hRatio->GetNbinsY(); ++ybin) {
      if(hRatio->GetBinContent(xbin, ybin) > 3.) {
        cout << __func__ << ": " << hnum->GetName() << ": Ratio bin (" << xbin << ", " << ybin << ") is "
             << hRatio->GetBinContent(xbin, ybin) << ", setting to 3\n";
        hRatio->SetBinContent(xbin, ybin, 3.);
      }
    }
  }
  hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  hRatio->GetYaxis()->SetRangeUser(ymin, ymax);
  hRatio->SetTitle("OS/SS");
  if(binc)
    hRatio->Draw("colz text error");
  else
    hRatio->Draw("colz");

  hRatio->GetZaxis()->SetRangeUser(0.95*CLFV::Utilities::H2MinAbove(hRatio, 0.01),
                                   std::min(3., 1.05*hRatio->GetMaximum()));
  if(xl != "") hRatio->SetXTitle(xl.Data());
  if(yl != "") hRatio->SetYTitle(yl.Data());
  return c;
}

//make a closure plot
TCanvas* make_closure_plot(TString hist, TString type, const int setAbs, double xmin = 1., double xmax = -1., int rebin = 0) {
  dataplotter_->include_qcd_ = 1;
  dataplotter_->rebinH_ = rebin;
  TH1*     hData = dataplotter_->get_data (hist, type, setAbs);
  THStack* hMC   = dataplotter_->get_stack(hist, type, setAbs);
  dataplotter_->include_qcd_ = 0;
  dataplotter_->rebinH_ = 0;
  if(!hData || !hMC) return nullptr;

  TCanvas* c = new TCanvas(Form("c_%s_%s_%i", hist.Data(), type.Data(), setAbs),
                           Form("c_%s_%s_%i", hist.Data(), type.Data(), setAbs),
                           800, 1200);
  TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.3); pad2->Draw();

  pad1->cd();
  pad1->SetGrid();
  hData->Draw("E1");
  hMC->Draw("hist noclear same");
  hData->Draw("same E1");
  if(xmin < xmax) hData->GetXaxis()->SetRangeUser(xmin, xmax);
  hData->GetYaxis()->SetRangeUser(0.1, 1.1*max(hMC->GetMaximum(), hData->GetMaximum()));
  hData->SetTitle("");

  pad2->cd();
  pad2->SetGrid();
  TH1* hRatio = (TH1*) hData->Clone(Form("%s_r", hData->GetName()));
  hRatio->Divide((TH1*) hMC->GetStack()->Last());
  hRatio->Draw("E1");
  hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);
  if(xmin < xmax) hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  hRatio->SetTitle("");

  xmin = (xmin < xmax) ? xmin : hRatio->GetBinLowEdge(1);
  xmax = (xmin < xmax) ? xmax : hRatio->GetXaxis()->GetBinUpEdge(hRatio->GetNbinsX());
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineWidth(2);
  line->Draw("same");

  return c;
}

//make 1D plots
TCanvas* make_canvas(TH1* hData, TH1* hMC, TH1* hQCD, TString name, bool logy = false) {
  TCanvas* c = new TCanvas(("c_"+name).Data(), ("c_"+name).Data(), 800, 600);
  TVirtualPad* pad;
  hMC->Draw("hist");
  hMC->SetLineColor(kBlue);
  hMC->SetLineWidth(2);
  hMC->SetFillStyle(3004);
  hMC->SetFillColor(kBlue);
  hData->Draw("E1 same");
  hData->SetMarkerStyle(20);
  hData->SetLineWidth(2);
  hQCD->Draw("hist same");
  hQCD->SetLineColor(kRed);
  hQCD->SetLineWidth(2);
  hQCD->SetFillStyle(3005);
  hQCD->SetFillColor(kRed);
  hMC->GetYaxis()->SetRangeUser(0.1, ((logy) ? 5. : 1.2)*max(hData->GetMaximum(), hMC->GetMaximum()));

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hData, "Data");
  leg->AddEntry(hMC  , "MC");
  leg->AddEntry(hQCD , "QCD");
  leg->Draw();
  return c;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Plot 1D slices of a 2D distribution
void make_2d_closure_slices(PlottingCard_t card, TH2* &hTight, TH2* &hLoose, TString name, const bool normalize = false,
                            const bool add_signal = false, const bool smooth = false) {
  hTight = nullptr; hLoose = nullptr;
  TString hist = card.hist_;
  TString type = card.type_;
  const int set = card.set_;
  const bool debug = false;

  if(verbose_ > 0) {
    cout << __func__ << ": Making 2d closure slices for " << hist.Data() << "/" << type.Data() << " using set "
         << set << endl;
  }

  hTight = get_2D_qcd_histogram(hist, set);
  hLoose = get_2D_qcd_histogram(hist, set+CLFVHistMaker::fQcdOffset);

  if(hLoose->Integral() <= 0. || hTight->Integral() <= 0.) {
    cout << "Warning! " << type.Data() << "/" << hist.Data() << "/" << set
         << " has at least 1 non-positive integral! Tight = " << hTight->Integral()
         << " Loose = " << hLoose->Integral() << endl;
    return;
  }
  hTight = (TH2*) hTight->Clone(Form("%s_2d_slices", hTight->GetName()));
  hLoose = (TH2*) hLoose->Clone(Form("%s_2d_slices", hLoose->GetName()));

  if(normalize) hLoose->Scale(hTight->Integral() / hLoose->Integral());

  //Get the signal distribution if requested
  TH2* hsignal = (add_signal) ? (TH2*) dataplotter_->get_signal(hist, type, set)[0] : nullptr;


  //loop through the x-axis bins
  const int nxbins = hLoose->GetNbinsX();
  for(int ixbin = 1; ixbin <= nxbins; ++ixbin) {
    if(debug) cout << " Performing slice " << ixbin << endl;

    //create 1D projections for the loose and tight
    TH1* h_loose = hLoose->ProjectionY(Form("%s_%i_loose", hist.Data(), ixbin), ixbin, ixbin);
    TH1* h_tight = hTight->ProjectionY(Form("%s_%i_tight", hist.Data(), ixbin), ixbin, ixbin);
    if(h_loose->Integral() <= 0. || h_tight->Integral() <= 0.) {
      cout << "Bin " << ixbin << " of 2D histogram " << hist.Data() << " has 0 integral loose or tight: Loose = " << h_loose->Integral()
           << " and Tight = " << h_tight->Integral() << endl;
      delete h_loose;
      delete h_tight;
      continue;
    }
    h_loose->SetTitle(hLoose->GetTitle());
    h_loose->SetFillColor(dataplotter_->qcd_color_);
    h_loose->SetFillStyle(dataplotter_->qcd_color_);
    h_loose->SetLineColor(dataplotter_->qcd_color_);
    h_tight->SetLineColor(kBlack);
    h_tight->SetMarkerColor(kBlack);
    h_tight->SetMarkerStyle(20);
    h_tight->SetMarkerSize(0.8);
    h_tight->SetFillStyle(0);
    h_tight->SetFillColor(0);

    TCanvas* c = new TCanvas("c1","c1",1200,800);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.35, 1., 1.0 );
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0 , 1., 0.35);
    pad1->SetBottomMargin(0.03);
    pad1->SetTopMargin(0.03);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.3);
    pad1->Draw(); pad2->Draw();
    pad1->cd();
    h_tight->Draw("E1");
    h_loose->Draw("hist same");
    if(hsignal) hsignal->Draw("hist same");
    h_tight->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
    h_tight->GetYaxis()->SetRangeUser(0.1, 1.1*max(h_loose->GetMaximum(), h_tight->GetMaximum()));
    h_tight->Draw("E1 sames");
    h_tight->GetXaxis()->SetLabelSize(0.);
    h_tight->GetYaxis()->SetLabelSize(0.06);
    h_tight->GetXaxis()->SetTitleSize(0.157);
    h_tight->GetYaxis()->SetTitleSize(0.157);
    h_tight->SetTitle("");

    TLegend* leg = new TLegend(0.53, 0.65, 0.9, 0.97);
    leg->AddEntry(h_tight, "Tight non-prompt e#mu");
    leg->AddEntry(h_loose, "Non-prompt e#mu Estimate");
    leg->SetTextSize(0.06);
    leg->Draw();

    pad1->SetGrid();

    //Add labels with the event number and predicted number
    TLatex datalabel;
    datalabel.SetNDC();
    datalabel.SetTextFont(72);
    datalabel.SetTextSize(0.07);
    datalabel.SetTextAlign(13);
    datalabel.DrawLatex(0.65, 0.62,  Form("Tight      : %9.1f", h_tight->Integral() + h_tight->GetBinContent(0) + h_tight->GetBinContent(h_tight->GetNbinsX()+1)));
    datalabel.DrawLatex(0.65, 0.54 , Form("Estimate: %9.1f"   , h_loose->Integral() + h_loose->GetBinContent(0) + h_loose->GetBinContent(h_loose->GetNbinsX()+1)));
    if(hsignal)
      datalabel.DrawLatex(0.65, 0.46,Form("Signal    : %9.1f"   , hsignal->Integral() + hsignal->GetBinContent(0) + hsignal->GetBinContent(hsignal->GetNbinsX()+1)));

    //Draw the ratio plot
    pad2->cd();
    TH1* hratio = (TH1*) h_tight->Clone(Form("hRatio_%s", h_tight->GetName()));
    hratio->Divide(h_loose);
    hratio->Draw("E1");
    if(smooth) {
      TH1* hsmooth = smooth_hist(hratio);
      hsmooth->SetLineColor(kAtlantic);
      hsmooth->SetFillColor(0);
      hsmooth->Draw("hist same");
    }
    pad2->Update();

    TString xtitle(""), ytitle(""), title("");
    Titles::get_titles(hist, type, selection_, &xtitle, &ytitle, &title);
    hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
    hratio->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
    hratio->SetXTitle(xtitle.Data());
    hratio->SetYTitle("Tight / Estimate");
    hratio->SetTitle("");
    hratio->GetXaxis()->SetTitleOffset(0.75);
    hratio->GetYaxis()->SetTitleOffset(0.4);
    hratio->GetXaxis()->SetLabelSize(0.1);
    hratio->GetYaxis()->SetLabelSize(0.09);
    hratio->GetXaxis()->SetTitleSize(0.157);
    hratio->GetYaxis()->SetTitleSize(0.11);

    if(hsignal) {
      TH1* sig_ratio = (TH1*) hsignal->Clone(Form("%s_ratio", hsignal->GetName()));
      sig_ratio->Add(h_loose);
      sig_ratio->Divide(h_loose);
      sig_ratio->SetFillColor(0);
      sig_ratio->Draw("hist same");
    }

    double xmin = (card.xmin_ < card.xmax_) ? card.xmin_ : hratio->GetBinLowEdge(1);
    double xmax = (card.xmin_ < card.xmax_) ? card.xmax_ : hratio->GetBinLowEdge(hratio->GetNbinsX()) + hratio->GetBinWidth(hratio->GetNbinsX());
    TLine* line = new TLine(xmin, 1., xmax, 1.);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    line->Draw("same");
    pad2->SetGrid();

    c->SaveAs(Form("%s%s_bin_%i.png", name.Data(), hist.Data(), ixbin));
    delete h_loose;
    delete h_tight;
    delete pad1;
    delete pad2;
    delete c;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void make_2d_closure_slices(PlottingCard_t card, TString name, const bool add_signal = false, const bool smooth = false) {
  TH2 *hloose, *htight;
  make_2d_closure_slices(card, hloose, htight, name, add_signal, smooth);
}

//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = verbose_;
  dataplotter_->qcd_scale_ = 1.;
  dataplotter_->embed_scale_ = embedScale_;

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_);
  double lum = 0.;
  for(int year : years_) {
    dataplotter_->lums_[year] = xs.GetLuminosity(year);
    lum += dataplotter_->lums_[year];
  }
  dataplotter_->set_luminosity(lum);

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}


//Generate the plots and scale factors
Int_t scale_factors(TString selection = "emu", int set = 8, vector<int> years = {2016},
                    TString hist_dir = "nanoaods_qcd") {

  if(years.size() == 0) return -1;

  //////////////////////
  // Initialize files //
  //////////////////////
  // hist_path_  = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/";
  hist_dir_   = hist_dir;
  selection_  = selection;
  useEmbed_   = 0;
  embedScale_ = 1.;
  years_      = years;
  hist_tag_   = "qcd"; //tag for the QCDHistMaker

  //If doing a bias region, force linear background fits
  if(set == 70 || set == 71) {useFTest_ = false; defaultOrder_ = 1;}

  //get the absolute value of the set, offsetting by the selection and using a loose selection
  int setAbs = set + CLFVHistMaker::fMisIDOffset;
  if     (selection == "mutau") setAbs += CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) setAbs += CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) setAbs += CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) setAbs += CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) setAbs += CLFVHistMaker::kEE   ;
  const int setLsETtMu = (setAbs - set + 70); //loose electron, tight muon
  const int setLsELsMu = (setAbs - set + 71); //loose electron, loose muon

  //construct the general name of each file, not including the sample name
  TString year_s = Form("%i", years[0]);
  for(unsigned i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);
  TString baseName = Form("clfv_%s_%s_", selection.Data(), year_s.Data());

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  //////////////////////
  //  Get histograms  //
  //////////////////////

  TH1* hData[2]; //OS and SS
  TH1* hMC[2]; //OS and SS
  hData[0] = get_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset, 1); //SS
  hMC[0]   = get_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset, 0); //SS
  hData[1] = get_histogram("lepdeltar1", setAbs                          , 1); //OS
  hMC[1]   = get_histogram("lepdeltar1", setAbs                          , 0); //OS

  if(!hData[0] || !hMC[0]) {
    cout << "Same sign histograms not found!\n";
    return 1;
  }
  if(!hData[1] || !hMC[1]) {
    cout << "Opposite sign histograms not found!\n";
    return 2;
  }

  ///////////////////////////
  // Get QCD contributions //
  ///////////////////////////

  hData[0]->SetName("hData_SS");
  hData[1]->SetName("hData_OS");
  hMC  [0]->SetName("hMC_SS");
  hMC  [1]->SetName("hMC_OS");

  TH1* hQCD[2];
  hQCD[0] = get_qcd_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset);
  hQCD[1] = get_qcd_histogram("lepdeltar1", setAbs                          );


  //////////////////////
  // Draw fake rates  //
  //////////////////////
  gStyle->SetOptStat(0);

  //construct general figure name
  TString name = Form("figures/%s_%i_%s/", selection.Data(), set, year_s.Data());
  //ensure directories exist
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", name.Data(), name.Data()));
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  const char* fname = Form("rootfiles/qcd_scale_%s_%i_%s.root", selection.Data(), set, year_s.Data());
  TFile* fOut = new TFile(fname, "RECREATE");

  TF1* f;
  TCanvas* c1 = make_canvas(hData[0], hMC[0], hQCD[0], "SS");
  TCanvas* c2 = make_canvas(hData[1], hMC[1], hQCD[1], "OS");
  TCanvas* c3 = make_ratio_canvas(hQCD[1], hQCD[0], f, "fit_1s_err");

  //////////////////////
  //  Print results   //
  //////////////////////

  //print canvases
  c1->Print((name+"SS.png").Data());
  c1->SetLogy();
  c1->Print((name+"SS_log.png").Data());
  c2->Print((name+"OS.png").Data());
  c2->SetLogy();
  c2->Print((name+"OS_log.png").Data());
  c3->Print((name+"scale.png").Data());

  TH1* hQCDScale = (TH1*) hQCD[1]->Clone("hRatio");
  hQCDScale->Divide(hQCD[0]);
  for(int ibin = 1; ibin < hQCDScale->GetNbinsX(); ++ibin) {
    if(hQCDScale->GetBinContent(ibin) < 0.) hQCDScale->SetBinContent(ibin, 0.);
    if(hQCDScale->GetBinContent(ibin) > 5.) hQCDScale->SetBinContent(ibin, 5.);
  }
  hQCDScale->Write();
  if(f) {
    f->SetName("fRatio");
    f->Write();
  }

  TCanvas* c;
  TH1 *hOS, *hSS, *hRatio;
  hOS = get_qcd_histogram("lepdeltaphi1", setAbs);
  hSS = get_qcd_histogram("lepdeltaphi1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, "", false, 0, 3.2);
  if(c) {c->Print((name + "lepdeltaphi1_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltaphi1");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("lepdeltaphi", setAbs);
  hSS = get_qcd_histogram("lepdeltaphi", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, "", false, 0, 3.2);
  if(c) {c->Print((name + "lepdeltaphi_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1*) hOS->Clone("hClosure_lepdeltaphi");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("oneeta", setAbs, "lep");
  hSS = get_qcd_histogram("oneeta", setAbs+CLFVHistMaker::fQcdOffset, "lep");
  c = make_ratio_canvas(hOS, hSS, f, "", false, -2.5, 2.5);
  if(c) {c->Print((name + "oneeta_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1*) hOS->Clone("hClosure_oneeta");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("lepdeltar2", setAbs);
  hSS = get_qcd_histogram("lepdeltar2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, "", false);
  if(c) {c->Print((name + "lepdeltar2_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1*) hOS->Clone("hClosure_lepdeltar");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  //Jet-binned delta-R scale factors
  hOS = get_qcd_histogram("qcddelrj0", setAbs);
  hSS = get_qcd_histogram("qcddelrj0", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_canvas(get_histogram("qcddelrj0", setAbs+CLFVHistMaker::fQcdOffset, 1),
                  get_histogram("qcddelrj0", setAbs+CLFVHistMaker::fQcdOffset, 0), hSS, "SS", true);
  if(c) {c->SetLogy(); c->Print((name + "lepdeltarj0_SS.png").Data()); delete c;}
  c = make_canvas(get_histogram("qcddelrj0", setAbs, 1),
                  get_histogram("qcddelrj0", setAbs, 0), hOS, "OS", true);
  if(c) {c->SetLogy(); c->Print((name + "lepdeltarj0_OS.png").Data()); delete c;}
  c = make_ratio_canvas(hOS, hSS, f, "fit_j0_1s_err", true);
  if(c) {c->Print((name + "lepdeltarj0_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltarj0");
    hRatio->Divide(hSS);
    hRatio->Write();
    f->SetName("fRatioJ0");
    f->Write();
  }

  hOS = get_qcd_histogram("qcddelrj1", setAbs);
  hSS = get_qcd_histogram("qcddelrj1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_canvas(get_histogram("qcddelrj1", setAbs+CLFVHistMaker::fQcdOffset, 1),
                  get_histogram("qcddelrj1", setAbs+CLFVHistMaker::fQcdOffset, 0), hSS, "SS", true);
  if(c) {c->SetLogy(); c->Print((name + "lepdeltarj1_SS.png").Data()); delete c;}
  c = make_canvas(get_histogram("qcddelrj1", setAbs, 1),
                  get_histogram("qcddelrj1", setAbs, 0), hOS, "OS", true);
  if(c) {c->SetLogy(); c->Print((name + "lepdeltarj1_OS.png").Data()); delete c;}
  c = make_ratio_canvas(hOS, hSS, f, "fit_j1_1s_err", true);
  if(c) {c->Print((name + "lepdeltarj1_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltarj1");
    hRatio->Divide(hSS);
    hRatio->Write();
    f->SetName("fRatioJ1");
    f->Write();
  }

  hOS = get_qcd_histogram("qcddelrj2", setAbs);
  hSS = get_qcd_histogram("qcddelrj2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_canvas(get_histogram("qcddelrj2", setAbs+CLFVHistMaker::fQcdOffset, 1),
                  get_histogram("qcddelrj2", setAbs+CLFVHistMaker::fQcdOffset, 0), hSS, "SS", true);
  if(c) {c->SetLogy(); c->Print((name + "lepdeltarj2_SS.png").Data()); delete c;}
  c = make_canvas(get_histogram("qcddelrj2", setAbs, 1),
                  get_histogram("qcddelrj2", setAbs, 0), hOS, "OS", true);
  if(c) {c->SetLogy(); c->Print((name + "lepdeltarj2_OS.png").Data()); delete c;}
  c = make_ratio_canvas(hOS, hSS, f, "fit_j2_1s_err", true);
  if(c) {c->Print((name + "lepdeltarj2_ratio.png").Data()); delete c;}
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltarj2");
    hRatio->Divide(hSS);
    hRatio->Write();
    f->SetName("fRatioJ2");
    f->Write();
  }

  /////////////////////////////////////
  // 2D ratios
  gStyle->SetPaintTextFormat(".2f");

  TH2 *h2DOS, *h2DSS, *h2DRatio;
  h2DOS = get_2D_qcd_histogram("lepdelrvsoneeta1", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelrvsoneeta1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 5.);
  if(c) {c->Print((name + "lepdelrvsoneeta1_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_lepdelrvsoneeta");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("lepdelrvsoneeta", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelrvsoneeta", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 5.);
  if(c) {c->Print((name + "lepdelrvsoneeta_ratio.png").Data()); delete c;}

  h2DOS = get_2D_qcd_histogram("lepdelphivsoneeta1", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelphivsoneeta1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 3.2);
  if(c) {c->Print((name + "lepdelphivsoneeta1_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_lepdelphivsoneeta");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  //Jet-binned one pt vs two pt non-closure test
  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptj0", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptj0", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwoptj0_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwoptj0");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptj1", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptj1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwoptj1_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwoptj1");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptj2", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptj2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwoptj2_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwoptj2");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwopt", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwopt", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwopt_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwopt");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  //loose electron, tight muon region 2D scales
  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsETtMu);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsETtMu+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwopt_letm_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwopt_letm");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }
  //Save the ratio histogram to make muon anti-iso --> iso scale factors
  TH2* hIso = (TH2*) h2DRatio->Clone("hIso");

  //loose electron, loose muon region 2D scales
  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsELsMu);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsELsMu+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwopt_lelm_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwopt_lelm");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }
  //Save the ratio histogram to make muon anti-iso --> iso scale factors
  TH2* hAntiIso = (TH2*) h2DRatio->Clone("hAntiIso");

  //Make the scale factors for muon anti-iso --> iso
  c = make_2D_ratio_canvas(hIso, hAntiIso, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) {c->Print((name + "oneptvstwopt_iso_to_antiiso_scale.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) hIso->Clone("hRatio_oneptvstwopt_antiisoscale");
    h2DRatio->Divide(hAntiIso);
    h2DRatio->Write();
  }

  //mutau_e: loose electron, tight muon region 2D (mass, bdt) scales
  make_2d_closure_slices(PlottingCard_t("qcdlepmvsmvamutau2", "event", 2270), name, true);
  h2DOS = get_2D_qcd_histogram("qcdlepmvsmvamutau2", setLsETtMu);
  h2DSS = get_2D_qcd_histogram("qcdlepmvsmvamutau2", setLsETtMu+CLFVHistMaker::fQcdOffset);
  //remove normalization effects
  h2DSS->Scale(h2DOS->Integral() / h2DSS->Integral());
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 40., 170, 0., 1., true, "Mass (GeV/c^{2})", "BDT score");
  if(c) {c->Print((name + "mass_vs_mva_mutau_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_mass_vs_mva_mutau");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }
  h2DOS = get_2D_qcd_histogram("qcdlepmvsmvamutau2", setAbs); //also print in this selection
  h2DSS = get_2D_qcd_histogram("qcdlepmvsmvamutau2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 40., 170., 0., 1., true, "Mass (GeV/c^{2})", "BDT score");
  if(c) {c->Print((name + "mass_vs_mva_mutau_set_ratio.png").Data()); delete c;}

  //etau_mu: loose electron, tight muon region 2D (mass, bdt) scales
  make_2d_closure_slices(PlottingCard_t("qcdlepmvsmvaetau2" , "event", 2270), name, true);
  h2DOS = get_2D_qcd_histogram("qcdlepmvsmvaetau2", setLsETtMu);
  h2DSS = get_2D_qcd_histogram("qcdlepmvsmvaetau2", setLsETtMu+CLFVHistMaker::fQcdOffset);
  //remove normalization effects
  h2DSS->Scale(h2DOS->Integral() / h2DSS->Integral());
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 40., 170, 0., 1., true, "Mass (GeV/c^{2})", "BDT score");
  if(c) {c->Print((name + "mass_vs_mva_etau_ratio.png").Data()); delete c;}
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_mass_vs_mva_etau");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }
  h2DOS = get_2D_qcd_histogram("qcdlepmvsmvaetau2", setAbs); //also print in this selection
  h2DSS = get_2D_qcd_histogram("qcdlepmvsmvaetau2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 40., 170., 0., 1., true, "Mass (GeV/c^{2})", "BDT score");
  if(c) {c->Print((name + "mass_vs_mva_etau_set_ratio.png").Data()); delete c;}

  //delta phi vs electron eta
  h2DOS = get_2D_qcd_histogram("lepdelphivsoneeta", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelphivsoneeta", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 3.2);
  if(c) {c->Print((name + "lepdelphivsoneeta_ratio.png").Data()); delete c;}

  fOut->Close();
  delete fOut;

  //Print closure histograms
  c = make_closure_plot("lepm" , "event", setAbs, 50., 170., 5); if(c) {c->SaveAs((name + "closure_lepm.png" ).Data()); delete c;}
  c = make_closure_plot("mtone", "event", setAbs,  0., 100., 2); if(c) {c->SaveAs((name + "closure_mtone.png").Data()); delete c;}
  c = make_closure_plot("mttwo", "event", setAbs,  0., 100., 2); if(c) {c->SaveAs((name + "closure_mttwo.png").Data()); delete c;}
  c = make_closure_plot("onept", "lep"  , setAbs, 10., 100., 2); if(c) {c->SaveAs((name + "closure_onept.png").Data()); delete c;}
  c = make_closure_plot("twopt", "lep"  , setAbs, 10., 100., 2); if(c) {c->SaveAs((name + "closure_twopt.png").Data()); delete c;}
  c = make_closure_plot("mtone", "event",    208,  0., 100., 2); if(c) {c->SaveAs((name + "closure_mtone_208.png").Data()); delete c;}
  c = make_closure_plot("mttwo", "event",    208,  0., 100., 2); if(c) {c->SaveAs((name + "closure_mttwo_208.png").Data()); delete c;}
  c = make_closure_plot("onept", "lep"  ,    208, 10., 100., 2); if(c) {c->SaveAs((name + "closure_onept_208.png").Data()); delete c;}
  c = make_closure_plot("twopt", "lep"  ,    208, 10., 100., 2); if(c) {c->SaveAs((name + "closure_twopt_208.png").Data()); delete c;}
  c = make_closure_plot("mtone", "event",   1208,  0., 100., 2); if(c) {c->SaveAs((name + "closure_mtone_1208.png").Data()); delete c;}
  c = make_closure_plot("mttwo", "event",   1208,  0., 100., 2); if(c) {c->SaveAs((name + "closure_mttwo_1208.png").Data()); delete c;}
  c = make_closure_plot("onept", "lep"  ,   1208, 10., 100., 2); if(c) {c->SaveAs((name + "closure_onept_1208.png").Data()); delete c;}
  c = make_closure_plot("twopt", "lep"  ,   1208, 10., 100., 2); if(c) {c->SaveAs((name + "closure_twopt_1208.png").Data()); delete c;}

  make_2d_closure_slices(PlottingCard_t("qcdlepmvsmvamutau0", "event", 2270), name);
  make_2d_closure_slices(PlottingCard_t("qcdlepmvsmvamutau1", "event", 2270), name);
  make_2d_closure_slices(PlottingCard_t("qcdlepmvsmvaetau0" , "event", 2270), name);
  make_2d_closure_slices(PlottingCard_t("qcdlepmvsmvaetau1" , "event", 2270), name);

  return 0;
}
