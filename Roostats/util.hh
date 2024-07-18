#ifndef __FIT_UTIL__
#define __FIT_UTIL__

//----------------------------------------------------------------------------------------------------------------------------
//Replace a systematic variation with a smoothed version
void smooth_systematic(TH1* nominal, TH1* sys, bool debug = false) {
  //Ensure reasonable input
  if(!nominal || !sys || nominal->Integral() <= 0. || sys->Integral() <= 0.) return;

  //how to fit the systematic shift
  const int mode = 4;
  const double nom_norm = nominal->Integral(); //preserve integrals in some smoothing
  const double sys_norm = sys->Integral();

  //Initialize the fit function, only fitting the non-zero region of the histogram
  const double xmin(sys->GetBinLowEdge(sys->FindFirstBinAbove(0.)));
  const double xmax(sys->GetXaxis()->GetBinUpEdge(sys->FindLastBinAbove(0.)));
  if(xmin >= xmax) return;
  if(sys->FindBin(xmax-1.e-4) - sys->FindBin(xmin) < 4) { //not enough bins to smooth
    return;
  }
  // TF1* fit_func = new TF1("fit_func", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*pow(x,4) + [5]*pow(x,5)", xmin, xmax);
  // fit_func->SetParameters((mode == 1) ? 1. : 0., 0., 0., 0., 0., 0.);
  TF1* fit_func = new TF1("fit_func", "[0] + [1]*x + [2]*x*x + [3]*x*x*x", xmin, xmax);
  fit_func->SetParameters((mode == 1) ? 1. : 0., 0., 0., 0.);

  TH1* nom_sys = (debug) ? (TH1*) sys->Clone("nominal_sys_TMP") : nullptr;
  TH1* hdiff = (TH1*) sys->Clone("hdiff"); //clone of the systematic histogram to use for fitting
  TH1* nom_diff = nullptr; //for debug drawing of pre-smoothed difference
  if(mode == 0) { //Fit the shift - nominal distribution
    hdiff->Add(nominal, -1.);
    auto fit_res = hdiff->Fit(fit_func, "R S 0 w Q");
    for(int ibin = 0; ibin <= hdiff->GetNbinsX()+1; ++ibin) {
      const double binc(nominal->GetBinContent(ibin));
      const double bine(nominal->GetBinError  (ibin));
      const double diff(fit_func->Eval(nominal->GetBinCenter(ibin)));
      if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
        sys->SetBinContent(ibin, 0.);
        sys->SetBinError  (ibin, 0.);
      } else {
        sys->SetBinContent(ibin, max(0., binc + diff));
        sys->SetBinError  (ibin, bine); //original template error
      }
    }
  } else if(mode == 1){ //Fit the shift / nominal distribution
    hdiff->Divide(nominal);
    auto fit_res = hdiff->Fit(fit_func, "R S 0 w Q");
    for(int ibin = 0; ibin <= hdiff->GetNbinsX()+1; ++ibin) {
      const double binc(nominal->GetBinContent(ibin));
      const double bine(nominal->GetBinError  (ibin));
      const double ratio(fit_func->Eval(nominal->GetBinCenter(ibin)));
      if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
        sys->SetBinContent(ibin, 0.);
        sys->SetBinError  (ibin, 0.);
      } else {
        sys->SetBinContent(ibin, max(0., binc*ratio));
        sys->SetBinError  (ibin, bine); //original template error
      }
    }
  } else if(mode == 2) { //Fit the (shift - nominal)/nominal distribution
    hdiff->Add(nominal, -1.);
    hdiff->Divide(nominal);
    auto fit_res = hdiff->Fit(fit_func, "R S 0 Q");
    for(int ibin = 0; ibin <= hdiff->GetNbinsX()+1; ++ibin) {
      const double binc(nominal->GetBinContent(ibin));
      const double bine(nominal->GetBinError  (ibin));
      const double diff_ratio(fit_func->Eval(nominal->GetBinCenter(ibin)));
      if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
        sys->SetBinContent(ibin, 0.);
        sys->SetBinError  (ibin, 0.);
      } else {
        const double val = binc*(1. + diff_ratio);
        sys->SetBinContent(ibin, max(0., val));
        sys->SetBinError  (ibin, bine); //original template error
      }
    }
  } else if(mode == 4) { //Smooth the (shift - nominal)/nominal distribution
    //scale the shifts so only shape is being smoothed, not normalization
    hdiff->Scale(nom_norm / sys_norm);
    hdiff->Add(nominal, -1.);
    hdiff->Divide(nominal);
    //adjust the error bars to be more reasonable
    for(int ibin = 1; ibin <= hdiff->GetNbinsX(); ++ibin) {
      const double binc_nom(nominal->GetBinContent(ibin));
      const double bine_nom(nominal->GetBinError  (ibin));
      const double binc_sys(sys    ->GetBinContent(ibin));
      if(binc_nom <= 0.) continue;
      const double eff_weight = pow(bine_nom,2) / binc_nom; //<weight> ~ err^2/integral
      if(eff_weight < 0.) continue;
      const double diff = fabs(binc_sys - binc_nom); //sign doesn't matter for the error
      const double eff_err = diff/sqrt(max(0.1, diff)/eff_weight); //error ~ value * 1./sqrt(<N>), where <N> = difference / avg. weight
      const double min_err = eff_weight*5.; //apply a minimum error that's fairly large
      const double err = max(eff_err, min_err) / binc_nom;
      if(isfinite(err))
        hdiff->SetBinError(ibin, err);
      else {
        printf("!!! %s: Bin error issue: bin = %i, binc = %.3f, diff = %.3f, weight = %.3f, err = %.3f\n",
               __func__, ibin, binc_nom, diff, eff_weight, err);
      }
    }
    nom_diff = (TH1*) hdiff->Clone("nom_diff_TMP");
    hdiff->GetXaxis()->SetRangeUser(xmin, xmax);
    hdiff->Smooth(2, "R"); //restrict to the set x-axis range
    for(int ibin = 0; ibin <= hdiff->GetNbinsX()+1; ++ibin) {
      const double binc(nominal->GetBinContent(ibin));
      const double bine(nominal->GetBinError  (ibin));
      const double diff_ratio(hdiff->GetBinContent(ibin)); //use the smoothed ratios
      if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
        sys->SetBinContent(ibin, 0.);
        sys->SetBinError  (ibin, 0.);
      } else {
        const double val = binc*(1. + diff_ratio);
        sys->SetBinContent(ibin, max(0., val));
        sys->SetBinError  (ibin, bine); //original template error
      }
    }
    //preserve the integral prediction of the template
    sys->Scale(sys_norm / sys->Integral());
  } else {
    cout << __func__ << ": Error! Unknown smoothing mode: " << mode << endl;
  }

  if(debug) { //draw the nominal distribution, the nominal shift, and the final shift
    cout << "Printing debug figures for " << sys->GetName() << endl;
    gSystem->Exec("[ ! -d debug ] && mkdir debug");
    TCanvas c("c","c", 700, 800);
    TPad pad1("pad1", "pad1", 0., 0.3, 1., 1. );
    TPad pad2("pad2", "pad2", 0., 0. , 1., 0.3);
    pad1.SetBottomMargin(0.06);
    pad2.SetTopMargin(0.03);
    pad1.Draw(); pad2.Draw();
    pad1.cd();

    const int fill_style = nominal->GetFillStyle();
    nominal->SetFillStyle(0);
    nominal->Draw("hist");
    nom_sys->Draw("E1 same");
    sys->SetLineStyle(kDashed);
    sys->SetFillColor(0);
    sys->Draw("hist same");
    nominal->SetAxisRange(0., 1.25*max(nominal->GetMaximum(), nom_sys->GetMaximum()), "Y");

    TLegend leg(0.5, 0.75, 0.9, 0.9);
    leg.AddEntry(nominal, Form("Nominal: %.1f", nominal->Integral()), "L");
    leg.AddEntry(nom_sys, Form("Input     : %.1f", nom_sys->Integral()), "P");
    leg.AddEntry(sys    , Form("Output  : %.1f", sys    ->Integral()), "L");
    leg.Draw();

    pad2.cd();
    TH1* hr = (TH1*) nom_sys->Clone("hr");
    hr->Divide(nominal);
    TH1* hr_2 = (TH1*) sys->Clone("hr_2");
    hr_2->Divide(nominal);
    hr->Draw("E1");
    hr_2->Draw("hist same");
    hr->GetYaxis()->SetRangeUser(0.6, 1.4);
    hr->GetXaxis()->SetLabelSize(0.08);
    hr->GetYaxis()->SetLabelSize(0.08);
    hr->GetYaxis()->SetTitleSize(0.08);
    hr->GetYaxis()->SetTitleOffset(0.65);
    hr->SetTitle("");
    hr_2->SetTitle("");
    hr->SetYTitle("Sys / Nominal");
    TLine line(0., 1., 1., 1.); line.SetLineStyle(kDashed); line.SetLineColor(kBlack);
    line.SetLineWidth(2); line.Draw("same");

    c.SaveAs(Form("debug/%s_nominal.png", sys->GetName()));
    nominal->SetFillStyle(fill_style);
    delete hr;
    delete hr_2;
    delete nom_sys;
    sys->SetFillColor(sys->GetLineColor());
    sys->SetLineStyle(kSolid);
  }

  if(debug) { //draw the shift fit
    TCanvas c;
    auto val = gStyle->GetOptFit();
    gStyle->SetOptFit(0);
    TH1* haxis = hdiff;
    if(mode != 4) {
      hdiff->Draw("E1");
      fit_func->Draw("same");
    } else {
      hdiff->SetLineStyle(kDashed);
      hdiff->SetFillStyle(3003);
      nom_diff->Draw("E1");
      hdiff->Draw("hist same");
      haxis = nom_diff;
    }
    const double ymax = haxis->GetMaximum();
    const double ymin = Utilities::H1MinAbove(haxis, (mode == 1) ? 0.1 : -1.e10);
    if(mode == 1) {
      haxis->GetYaxis()->SetRangeUser(max(0., min(0.95*ymin, 1.-ymax)), max(1.05, 1.05*ymax));
    } else if(mode == 2) {
      haxis->GetYaxis()->SetRangeUser(1.1*min(-0.05, ymin), 1.1*max(0.05, ymax));
    } else if(mode == 4) {
      haxis->GetYaxis()->SetRangeUser(1.1*max(-1., min(-0.05, ymin)), 1.1*min(1., max(0.05, ymax)));
    }
    haxis->SetTitle(sys->GetName());
    haxis->SetFillStyle(0);
    const double y_nom = (mode == 1) ? 1. : 0.;
    TLine line(0., y_nom, 1., y_nom); line.SetLineStyle(kDashed); line.SetLineColor(kBlack);
    line.SetLineWidth(2); line.Draw("same");
    c.SaveAs(Form("debug/%s.png", sys->GetName()));
    gStyle->SetOptFit(val);
    if(nom_diff) delete nom_diff;
  }
  delete fit_func;
  delete hdiff;
}

//----------------------------------------------------------------------------------------------------------------------------
//Replace a systematic variation with a smoothed version
void smooth_systematic_rebinned(TH1* nom, TH1* sys, bool debug = false) {
  //Ensure reasonable input
  if(!nom || !sys || nom->Integral() <= 0. || sys->Integral() <= 0.) return;

  //how to fit the systematic shift
  const int mode = 4;

  //Initialize the fit function, only fitting the non-zero region of the histogram
  const int bin_low (sys->FindFirstBinAbove(0.));
  const int bin_high(sys->FindLastBinAbove (0.));
  const double xmin(sys->GetBinLowEdge(bin_low));
  const double xmax(sys->GetXaxis()->GetBinUpEdge(bin_high));
  if(xmin >= xmax) return;
  if(bin_high - bin_low < 3) { //not enough bins to smooth
    return;
  }

  //Save unchanged version for debugging
  TH1* sys_nom = (debug) ? (TH1*) sys->Clone("sys_unchanged_TMP") : nullptr;

  //Rebin the input histograms
  const int nbins_input = bin_high - bin_low + 1;
  const int nbins_smooth = nbins_input / 2 + nbins_input % 2; //round up
  TH1* nom_rebinned = new TH1D("nom_rebinned_TMP", "Rebinned nominal"   , nbins_smooth, 0., nbins_smooth); //ignore the input widths/axis values
  TH1* sys_rebinned = new TH1D("sys_rebinned_TMP", "Rebinned systematic", nbins_smooth, 0., nbins_smooth); //ignore the input widths/axis values
  for(int ibin = 1; ibin <= nbins_smooth; ++ibin) {
    const int base_bin = bin_low + 2*(ibin-1);
    if(base_bin > bin_high) continue;
    const double val_nom_1 = nom->GetBinContent(base_bin    );
    const double val_nom_2 = nom->GetBinContent(base_bin + 1);
    const double err_nom_1 = nom->GetBinError  (base_bin    );
    const double err_nom_2 = nom->GetBinError  (base_bin + 1);
    const double val_sys_1 = sys->GetBinContent(base_bin    );
    const double val_sys_2 = sys->GetBinContent(base_bin + 1);
    const double err_sys_1 = sys->GetBinError  (base_bin    );
    const double err_sys_2 = sys->GetBinError  (base_bin + 1);
    nom_rebinned->SetBinContent(ibin, (base_bin < bin_high) ? val_nom_1 + val_nom_2 : val_nom_1);
    sys_rebinned->SetBinContent(ibin, (base_bin < bin_high) ? val_sys_1 + val_sys_2 : val_sys_1);
    nom_rebinned->SetBinError  (ibin, (base_bin < bin_high) ? sqrt(pow(err_nom_1,2) + pow(err_nom_2,2)) : err_nom_1);
    sys_rebinned->SetBinError  (ibin, (base_bin < bin_high) ? sqrt(pow(err_sys_1,2) + pow(err_sys_2,2)) : err_sys_1);
  }

  //Evaluate the difference between the nominal and systematic
  TH1* hdiff = (TH1*) sys_rebinned->Clone("hdiff");
  hdiff->Add(nom_rebinned, -1.);
  hdiff->Divide(nom_rebinned);

  //adjust the ratio error bars to be more reasonable (double counts statistical uncertainty)
  for(int ibin = 1; ibin <= hdiff->GetNbinsX(); ++ibin) {
    const double binc_nom(nom_rebinned->GetBinContent(ibin));
    const double bine_nom(nom_rebinned->GetBinError  (ibin));
    const double binc_sys(sys_rebinned->GetBinContent(ibin));
    if(binc_nom <= 0.) continue;
    const double eff_weight = pow(bine_nom,2) / binc_nom; //<weight> ~ err^2/integral
    if(eff_weight < 0.) continue;
    const double diff = fabs(binc_sys - binc_nom); //sign doesn't matter for the error
    const double eff_err = diff/sqrt(max(0.1, diff)/eff_weight); //error ~ value * 1./sqrt(<N>), where <N> = difference / avg. weight
    const double min_err = eff_weight*5.; //apply a minimum error that's fairly large
    const double err = max(eff_err, min_err) / binc_nom;
    if(isfinite(err))
      hdiff->SetBinError(ibin, err);
    else {
      printf("!!! %s: Bin error issue: bin = %i, binc = %.3f, diff = %.3f, weight = %.3f, err = %.3f\n",
             __func__, ibin, binc_nom, diff, eff_weight, err);
    }
  }

  //Save the original ratios if debugging
  TH1* hdiff_nom = (debug) ? (TH1*) hdiff->Clone("hdiff_nom") : nullptr;

  //Smooth the ratios
  hdiff->Smooth(2);

  //Apply the results
  for(int ibin = bin_low; ibin <= bin_high; ++ibin) {
    const double binc_nom(nom->GetBinContent(ibin));
    const double bine_nom(nom->GetBinError  (ibin));

    const int bin_diff = (ibin - bin_low) / 2;
    const double diff_ratio(hdiff->GetBinContent(bin_diff)); //the smoothed ratio

    if(!std::isfinite(binc_nom) || !std::isfinite(bine_nom) || binc_nom < 0.) {
      sys->SetBinContent(ibin, 0.);
      sys->SetBinError  (ibin, 0.);
    } else {
      const double val = binc_nom*(1. + diff_ratio);
      sys->SetBinContent(ibin, max(0., val));
      sys->SetBinError  (ibin, bine_nom); //original template error
    }
  }

  if(debug) { //draw the nominal distribution, the nominal shift, and the final shift
    cout << "Printing debug figures for " << sys->GetName() << endl;
    gSystem->Exec("[ ! -d debug ] && mkdir debug");
    TCanvas c("c","c", 700, 800);
    TPad pad1("pad1", "pad1", 0., 0.3, 1., 1. );
    TPad pad2("pad2", "pad2", 0., 0. , 1., 0.3);
    pad1.SetBottomMargin(0.06);
    pad2.SetTopMargin(0.03);
    pad1.Draw(); pad2.Draw();
    pad1.cd();

    const int fill_style = nom->GetFillStyle();
    nom->SetFillStyle(0);
    nom->Draw("hist");
    sys_nom->Draw("E1 same");
    sys->SetLineStyle(kDashed);
    sys->SetFillColor(0);
    sys->Draw("hist same");

    pad2.cd();
    TH1* hr = (TH1*) sys_nom->Clone("hr");
    hr->Divide(nom);
    TH1* hr_2 = (TH1*) sys->Clone("hr_2");
    hr_2->Divide(nom);
    hr->Draw("E1");
    hr_2->Draw("hist same");
    hr->GetYaxis()->SetRangeUser(0.7, 1.3);
    hr->GetXaxis()->SetLabelSize(0.08);
    hr->GetYaxis()->SetLabelSize(0.08);
    hr->GetYaxis()->SetTitleSize(0.08);
    hr->GetYaxis()->SetTitleOffset(0.65);
    hr->SetTitle("");
    hr_2->SetTitle("");
    hr->SetYTitle("Sys / Nominal");
    TLine line(0., 1., 1., 1.); line.SetLineStyle(kDashed); line.SetLineColor(kBlack);
    line.SetLineWidth(2); line.Draw("same");
    c.SaveAs(Form("debug/%s_nominal.png", sys->GetName()));
    nom->SetFillStyle(fill_style);
    delete hr;
    delete hr_2;
    delete sys_nom;
    sys->SetFillColor(sys->GetLineColor());
    sys->SetLineStyle(kSolid);
  }

  if(debug) { //draw the shift fit
    TCanvas c;
    auto val = gStyle->GetOptFit();
    gStyle->SetOptFit(0);
    hdiff->SetLineStyle(kDashed);
    hdiff->SetFillStyle(3003);
    hdiff->SetLineWidth(3);
    hdiff->SetLineColor(kBlue);
    hdiff->SetFillColor(kBlue);
    hdiff_nom->SetLineWidth(3);
    hdiff_nom->SetLineColor(kRed);
    hdiff_nom->Draw("E1");
    hdiff->Draw("hist same");
    TH1* haxis = hdiff_nom;

    const double ymax = haxis->GetMaximum();
    const double ymin = Utilities::H1MinAbove(haxis, (mode == 1) ? 0.1 : -1.e10);
    haxis->GetYaxis()->SetRangeUser(1.1*max(-1., min(-0.05, ymin)), 1.1*min(1., max(0.05, ymax)));
    haxis->SetTitle(sys->GetName());
    haxis->SetFillStyle(0);
    const double y_nom = (mode == 1) ? 1. : 0.;
    TLine line(0., y_nom, 1., y_nom); line.SetLineStyle(kDashed); line.SetLineColor(kBlack);
    line.SetLineWidth(2); line.Draw("same");
    c.SaveAs(Form("debug/%s.png", sys->GetName()));
    gStyle->SetOptFit(val);
    if(hdiff_nom) delete hdiff_nom;
  }
  delete nom_rebinned;
  delete sys_rebinned;
  delete hdiff;
}

//---------------------------------------------------------------------------------------------------------------------
// Print a one-sided uncertainty plot
int print_mva_systematic_canvas(TH1* nominal, TH1* data, THStack* nom_stack, THStack* stack, TH1* nom_sig, TH1* sig,
                                const char* name, const char* fig_name) {
  //Check the inputs
  if(!data || !stack || !nom_sig || !sig || !nominal) return 1;

  const double xmin(0.), xmax(1.);
  const bool logy(true);

  //Make the canvas
  TCanvas* c = new TCanvas(name, name, 1200, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
  pad1->SetBottomMargin(0.06);
  pad2->SetTopMargin(0.03);
  pad1->Draw(); pad2->Draw();
  pad1->cd();

  //Draw the total stack / data on top
  data->SetLineColor(kBlack);
  data->Draw("E1");
  stack->Draw("hist noclear same");
  sig->Draw("hist same");
  data->Draw("E1 same");
  data->GetXaxis()->SetRangeUser(0., 1.);
  const double max_val = max(Utilities::H1Max(data, xmin, xmax), Utilities::H1Max((TH1*) stack->GetStack()->Last(), xmin, xmax));
  data->GetYaxis()->SetRangeUser((logy) ? 0.5 : 0.9, ((logy) ? 2 : 1.1)*max_val);
  data->SetTitle("");
  if(logy) pad1->SetLogy();

  //Draw ratio plots on the bottom
  pad2->cd();
  vector<TH1*> h_to_delete;
  TH1* bkg_ratio = (TH1*) stack->GetStack()->Last()->Clone("bkg_ratio");
  bkg_ratio->Divide(nominal);
  TH1* sig_ratio = (TH1*) sig->Clone("sig_ratio");
  sig_ratio->Divide(nom_sig);
  TH1* data_ratio = (TH1*) data->Clone("data_ratio");
  data_ratio->Divide(nominal);
  h_to_delete.push_back(bkg_ratio);
  h_to_delete.push_back(sig_ratio);
  h_to_delete.push_back(data_ratio);

  const double max_bkg_val(Utilities::H1Max(bkg_ratio, xmin, xmax)), min_bkg_val(Utilities::H1Min(bkg_ratio, 0.1));
  const double max_sig_val(Utilities::H1Max(sig_ratio, xmin, xmax)), min_sig_val(Utilities::H1MinAbove(sig_ratio, 0.1));
  double min_r = min((min_bkg_val <= 0.) ? 0.95 : min_bkg_val, (min_sig_val <= 0.) ? 0.95 : min_sig_val); //try to catch cases with val = 0
  double max_r = max(max_bkg_val, max_sig_val);
  min_r = max(0., min(0.95, min_r - 0.05*(1. - min_r))); //set the ratio bounds to be between 5-15% with a small buffer
  max_r = min(2., max(1.05, max_r + 0.05*(max_r - 1.)));

  bkg_ratio->SetLineColor(kRed-3);
  bkg_ratio->SetLineWidth(2);
  bkg_ratio->SetFillColor(0);
  sig_ratio->SetLineWidth(2);
  sig_ratio->SetFillColor(0);

  data_ratio->Draw("E1");
  sig_ratio->Draw("hist same");
  bkg_ratio->Draw("hist same");
  data_ratio->GetYaxis()->SetRangeUser(min_r, max_r);
  data_ratio->GetXaxis()->SetRangeUser(xmin, xmax);
  data_ratio->GetXaxis()->SetLabelSize(0.08);
  data_ratio->GetYaxis()->SetLabelSize(0.08);

  //Draw each stack component ratio
  const int nhists = stack->GetNhists();
  for(int ihist = 0; ihist < nhists; ++ihist) {
    TH1* h     = (TH1*) stack    ->GetHists()->At(ihist)->Clone(Form("h_stack_ratio_%i", ihist));
    TH1* h_nom = (TH1*) nom_stack->GetHists()->At(ihist);
    h->Add(h_nom, -1); //subtract off the nominal to get the shift
    //Add the nominal and then divide by it to get the relative effect on the total
    h->Add(nominal);
    h->Divide(nominal);
    int color = h_nom->GetLineColor();
    if(color == kYellow - 7) color = kOrange; //make some colors more visible
    if(color == kGreen  - 7) color = kGreen - 3;
    h->SetLineColor(color);
    h->SetLineWidth(3);
    h->SetFillColor(0);
    h->SetLineStyle(kDashed);
    h->Draw("hist same");
    h_to_delete.push_back(h);
  }

  //Print the figure
  c->SaveAs(fig_name);

  delete c;
  for(auto h : h_to_delete) delete h;

  return 0;
}

//---------------------------------------------------------------------------------------------------------------
// Get the difference between the data and a PDF: h = Data - PDF
TH1* data_pdf_diff(RooDataHist* data, RooAbsPdf* pdf, RooRealVar& obs, double norm = -1., double blind_min = 1., double blind_max = -1.) {
  if(!data || !pdf) return nullptr;
  TH1* h_pdf  = pdf ->createHistogram(Form("TMP_%s_pdf" , __func__), obs);
  TH1* h_data = data->createHistogram(Form("TMP_%s_data", __func__), obs);
  const double ndata((norm > 0.) ? norm : h_data->Integral()), npdf(h_pdf->Integral());
  if(npdf <= 0.) return nullptr;
  h_pdf->Scale(ndata / npdf);
  TH1* h_diff = (TH1*) h_data->Clone(Form("TMP_%s_diff" , __func__));
  h_diff->Add(h_pdf, -1.);
  h_diff->SetLineWidth(2);
  for(int ibin = 1; ibin <= h_diff->GetNbinsX(); ++ibin) {
    //check if blinded
    if(blind_min < blind_max) {
      const double x_low  = h_data->GetBinLowEdge(ibin);
      const double x_high = h_data->GetBinWidth(ibin) + x_low;
      if((x_low >= blind_min && x_low < blind_max) ||
         (x_high > blind_min && x_high <= blind_max)) {
        h_diff->SetBinContent(ibin, 0.);
        h_diff->SetBinError  (ibin, 0.);
        continue;
      }
    }
    const double data_val = h_data->GetBinContent(ibin);
    const double data_err = h_data->GetBinError  (ibin);
    h_diff->SetBinError(ibin, (data_err > 0.) ? data_err : (data_val > 0.) ? sqrt(data_val) : 0.);
  }
  return h_diff;
}

//---------------------------------------------------------------------------------------------------------------
// Get the difference between two PDFs for a given N(data): h = PDF_1 - PDF_2
TH1* pdf_pdf_diff(RooAbsPdf* pdf_1, RooAbsPdf* pdf_2, RooRealVar& obs, const double ndata) {
  if(!pdf_1 || !pdf_2) return nullptr;
  TH1* h_pdf_1 = pdf_1->createHistogram(Form("TMP_%s_pdf_1", __func__), obs);
  TH1* h_pdf_2 = pdf_2->createHistogram(Form("TMP_%s_pdf_2", __func__), obs);
  const double npdf_1(h_pdf_1->Integral()), npdf_2(h_pdf_2->Integral());
  if(npdf_1 <= 0. || npdf_2 <= 0.) return nullptr;
  h_pdf_1->Scale(ndata / npdf_1);
  h_pdf_2->Scale(ndata / npdf_2);
  TH1* h_diff = (TH1*) h_pdf_1->Clone(Form("TMP_%s_diff" , __func__));
  h_diff->Add(h_pdf_2, -1.);
  h_diff->SetLineWidth(2);
  return h_diff;
}

//-----------------------------------------------------------------------------------------------------------------------------------
// Count free PDF params
int count_pdf_params(RooAbsPdf* pdf) {
  int nfree = 0;
  auto vars = pdf->getVariables();
  auto itr = vars->createIterator();
  auto var = itr->Next();
  while(var) {
    if(!((RooAbsReal*) var)->isConstant()) ++nfree;
    var = itr->Next();
  }
  return max(nfree-1,0); //remove the observable from counting
}

void print_pdf_params(RooAbsPdf* pdf, const char* obs = nullptr) {
  auto vars = pdf->getVariables();
  auto itr = vars->createIterator();
  auto var = itr->Next();
  while(var) {
    if(!obs || TString(var->GetName()) != TString(obs)) {
      var->Print();
    }
    var = itr->Next();
  }
}


#endif
