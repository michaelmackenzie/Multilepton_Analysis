#ifndef __CONSTRUCT_MULTIDIM__
#define __CONSTRUCT_MULTIDIM__
#include "bemu_defaults.C"

//Construct multi-dimensional PDF with discrete index
bool useFrameChiSq_         = false; //use a roo plot frame to evaluate chi^2
bool useManualChisq_        = true ; //calcuate chi^2 values by hand with histograms
bool useDataBinErrors_      = false; //use data bin errors when calculating chi^2
bool use_generic_bernstein_ = false;
bool use_fast_bernstein_    = true ;
bool use_exp_family_        = true ;
bool use_power_family_      = true ;
bool use_laurent_family_    = false;
bool use_dy_ww_shape_       = false;
bool force_fit_order_       = true ; //force only the inclusion of fixed orders of each family
bool force_best_fit_        = false; //force the nominal PDF to be a specific function
bool enforce_ftest_         = false; //once a function fails the F-test, stop adding functions

bool test_single_function_  = false; //only pass 1 function into the PDF ensemble

//----------------------------------------------------------------------------------------------------------------
//Perform the F-test selection for a higher order function: function_1 = nominal, function_2 = higher order
bool perform_f_test(double chisq_1, int ndof_1, double chisq_2, int ndof_2) {
  if(chisq_1 < 0. || ndof_1 <= 0) return true; //pick the higher order function
  if(chisq_2 < 0. || ndof_2 <= 0) return false; //stick with the original function
  if(ndof_1 == ndof_2) return chisq_1 > chisq_2; //if equal degrees of freedom, pick the better chi^2 model
  if(ndof_1 < ndof_2) {
    cout << __func__ << ": Error, higher order has higher N(dof)!\n";
    return false;
  }

  const int Mode = 1; //how to perform the F-test: 0: use F-distribution; 1: use delta chi^2 distribution
  double p_of_f = 1.;
  if(Mode == 0) { //use CDF for the F-distribution
    const double ftest = (chisq_1 / ndof_1) / (chisq_2 / ndof_2); //(chisq_1 - chisq_2) / (ndof_1 - ndof_2) / (chisq_2 / ndof_2);
    p_of_f = 1. - ROOT::Math::fdistribution_cdf(ftest, ndof_1, ndof_2);
  } else if(Mode == 1) { //use the difference of chi^2 p-value given N(dof) = Delta N(dof)
    const double ftest = chisq_1 - chisq_2;
    if(ftest < 0.) p_of_f = 1.; //lower order is a better fit
    else           p_of_f = ROOT::Math::chisquared_cdf_c(ftest, ndof_1 - ndof_2);
  } else {
    cout << __func__ << ": Unkown F-test mode " << Mode << endl;
    return false;
  }
  printf("F-test: chisq_1 = %.3f / %i; chisq_2 = %.3f / %i --> p(F) = %.3e\n",
         chisq_1, ndof_1, chisq_2, ndof_2, p_of_f);
  return (p_of_f < 0.05); //select a higher order if 95% confidence it's better
}

//----------------------------------------------------------------------------------------------------------------
//Perform a p(chi^2, ndof) test to determine if the function has an acceptable agreement with data
bool perform_chisq_test(double chisq, int ndof, double* p_val = nullptr) {
  if(chisq < 0. || ndof <= 0) return false; //must have well-defined chi^2 and N(dof)
  const double p_chi_sq = TMath::Prob(chisq, ndof);
  if(p_val) *p_val = p_chi_sq;
  return (p_chi_sq > 0.001); //accept if p(chi^2, ndof) is at least 1%
}

//----------------------------------------------------------------------------------------------------------------
//Get the chi-squared between a model and data from the model
double get_hist_chisquare(TH1* model, TH1* data, int* nbins = nullptr) {
  if(model->GetNbinsX() != model->GetNbinsX()) {
    cout << __func__ << ": Model and data don't have the same number of bins ("
         << model->GetNbinsX() << " vs " << model->GetNbinsX() << ")!\n";
    return -1.;
  }

  double chisq = 0.;
  for(int ibin = 1; ibin <= model->GetNbinsX(); ++ibin) {
    const double x_data = data ->GetBinCenter(ibin);
    const double x_pdf  = model->GetBinCenter(ibin);
    if(x_data != x_pdf) {
      cout << __func__ << ": Warning! Data center = " << x_data << " but PDF center = " << x_pdf << endl;
    }
    const double npdf  = model->GetBinContent(ibin);
    const double ndata = data->GetBinContent(ibin);
    const double error = data->GetBinError  (ibin);
    const double val  = ndata - npdf;
    const double sigma = (error > 0.) ? std::pow(val / error, 2) : 0.;
    chisq += sigma;
    if(verbose_ > 2) {
      cout << "Bin " << ibin << " (" << data->GetBinLowEdge(ibin) << " - "
           << data->GetBinLowEdge(ibin) + data->GetBinWidth(ibin) << "): Data = " << ndata << " PDF = " << npdf
           << " --> sigma = " << sigma << endl;
    }
  }

  if(verbose_ > 1) cout << __func__ << ": Total chi^2 = " << chisq << " / " << model->GetNbinsX() << " bins\n";
  if(nbins) *nbins = model->GetNbinsX();
  return chisq;
}

//----------------------------------------------------------------------------------------------------------------
//Get the chi-squared using a by-hand calculation
double get_manual_subrange_chisquare(RooRealVar& obs, RooAbsPdf* pdf, RooDataHist& data, const char* range = nullptr,
                                     const char* norm_range = nullptr, bool norm_skip = true, int* nbins = nullptr) {
  TH1* htmp_pdf  = pdf->createHistogram("htmp_chisq_pdf" , obs);
  TH1* htmp_data = data.createHistogram("htmp_chisq_data", obs);
  if(htmp_pdf->GetNbinsX() != htmp_data->GetNbinsX()) {
    cout << __func__ << ": PDF and data don't have the same number of bins ("
         << htmp_pdf->GetNbinsX() << " vs " << htmp_data->GetNbinsX() << ")!\n";
    delete htmp_pdf;
    delete htmp_data;
    return -1.;
  }

  //Create the PDF normalization by matching it to the data, skipping the region requested
  const double xmin_norm = (norm_range) ? obs.getMin(norm_range) : (norm_skip) ?  1. : obs.getMin();
  const double xmax_norm = (norm_range) ? obs.getMax(norm_range) : (norm_skip) ? -1. : obs.getMax();
  const int bin_norm_lo = (xmin_norm < xmax_norm) ? htmp_data->GetXaxis()->FindBin(xmin_norm) : (norm_skip) ? -1 : 1;
  const int bin_norm_hi = (xmin_norm < xmax_norm) ? htmp_data->GetXaxis()->FindBin(xmax_norm) : (norm_skip) ? -1 : htmp_data->GetNbinsX();
  double pdf_norm = 0.;
  double data_norm = 0.;
  for(int ibin = 1; ibin <= htmp_data->GetNbinsX(); ++ibin) {
    bool in_norm = ibin >= bin_norm_lo && ibin <= bin_norm_hi;
    if(norm_skip && in_norm) continue; //if given a range to skip
    if(!norm_skip && !in_norm) continue; //if given a range for normalizing
    const double npdf = htmp_pdf->GetBinContent(ibin)*htmp_pdf->GetBinWidth(ibin); //expected N(events)
    const double ndata = htmp_data->GetBinContent(ibin); //observed N(events)
    pdf_norm  += npdf;
    data_norm += ndata;
    if(verbose_ > 2) {
      cout << "Norm Bin " << ibin << " (" << htmp_data->GetBinLowEdge(ibin) << " - "
           << htmp_data->GetBinLowEdge(ibin) + htmp_data->GetBinWidth(ibin) << "): Data = " << ndata << " PDF = " << npdf  << endl;
    }
  }
  if(pdf_norm <= 0.) {
    cout << __func__ << ": PDF normalization is non-positive!\n";
    delete htmp_pdf;
    delete htmp_data;
    return -1.;
  }
  htmp_pdf->Scale(data_norm / pdf_norm);
  if(verbose_ > 2) {
    cout << __func__ << ": Scaling PDF:\n"
         << "#### Data norm = " << data_norm << endl
         << "#### PDF norm = " << pdf_norm << endl
         << "#### Scaling the PDF histogram by " << data_norm / pdf_norm << endl;
  }

  const double xmin = obs.getMin(range);
  const double xmax = obs.getMax(range);

  double chisq = 0.;
  const int bin_lo = max(1, htmp_data->GetXaxis()->FindBin(xmin));
  const int bin_hi = min(htmp_data->GetNbinsX(), htmp_data->GetXaxis()->FindBin(xmax));
  for(int ibin = bin_lo; ibin <= bin_hi; ++ibin) {
    const double x_data = htmp_data->GetBinCenter(ibin);
    const double x_pdf  = htmp_pdf ->GetBinCenter(ibin);
    if(x_data != x_pdf) {
      cout << __func__ << ": Warning! Data center = " << x_data << " but PDF center = " << x_pdf << endl;
    }
    const double npdf  = htmp_pdf->GetBinContent(ibin)*htmp_pdf->GetBinWidth(ibin);
    const double ndata = htmp_data->GetBinContent(ibin);
    const double error = htmp_data->GetBinError  (ibin);
    const double val   = ndata - npdf;
    const double sigma = val*val / ((useDataBinErrors_) ? error*error : (npdf <= 0. ? 1.e-5 : npdf));
    chisq += sigma;
    if(verbose_ > 2) {
      cout << "Bin " << ibin << " (" << htmp_data->GetBinLowEdge(ibin) << " - "
           << htmp_data->GetBinLowEdge(ibin) + htmp_data->GetBinWidth(ibin) << "): Data = " << ndata << " PDF = " << npdf
           << " --> sigma = " << sigma << endl;
    }
  }

  if(verbose_ > 1) cout << __func__ << ": Total chi^2 = " << chisq << " / " << bin_hi - bin_lo+1 << " bins\n";
  if(nbins) *nbins = bin_hi - bin_lo+1;
  delete htmp_pdf;
  delete htmp_data;
  return chisq;
}

//Get the chi-squared using a RooChi2Var
double get_subrange_chisquare(RooRealVar& obs, RooAbsPdf* pdf, RooDataHist& data, const char* range) {
  RooChi2Var chi("chi", "chi", *pdf, data, RooFit::Range(range));
  return chi.getVal();
}

//Evaluate the chi-squared
double get_chi_squared(RooRealVar& obs, RooAbsPdf* pdf, RooDataHist& data, bool useSideBands, int* nbins = nullptr) {
  if(useManualChisq_) {
    if(useSideBands || blind_data_) {
      const char* blind_region = "BlindRegion"; //skip the blinding region if fitting the sidebands
      int nbin_running = 0;
      double chi_sq = get_manual_subrange_chisquare(obs, pdf, data, "LowSideband", blind_region, true, &nbin_running);
      if(nbins) *nbins = nbin_running;
      chi_sq += get_manual_subrange_chisquare(obs, pdf, data, "HighSideband", blind_region, true, &nbin_running);
      if(nbins) *nbins = *nbins + nbin_running;
      return chi_sq;
    } else {
      return get_manual_subrange_chisquare(obs, pdf, data, "full");
    }
  }
  if(useFrameChiSq_ || TString(pdf->GetTitle()).Contains("Exponential")) {
    // if(useSideBands) {
    //   double chi_sq = 0.;
    //   auto xframe = obs.frame();
    //   data.plotOn(xframe, RooFit::Range("LowSideband"));
    //   pdf->plotOn(xframe, RooFit::Range("LowSideband"), , RooFit::NormRange("LowSideband"));
    //   chi_sq += xframe->chiSquare() * data.numEntries(); //returns chi squared / entries
    //   data.plotOn(xframe, RooFit::Range("HighSideband"));
    //   pdf->plotOn(xframe, RooFit::Range("HighSideband"), , RooFit::NormRange("HighSideband"));
    //   chi_sq += xframe->chiSquare() * data.numEntries(); //returns chi squared / entries
    //   delete xframe;
    //   return chi_sq;
    // } else {
    auto xframe = obs.frame();
    data.plotOn(xframe);
    pdf->plotOn(xframe);
    const double chi_sq = xframe->chiSquare() * data.numEntries(); //returns chi squared / entries
    delete xframe;
    return chi_sq;
  }
  if(useSideBands) {
    double chi_sq = get_subrange_chisquare(obs, pdf, data, "LowSideband");
    chi_sq += get_subrange_chisquare(obs, pdf, data, "HighSideband");
    return chi_sq;
  }
  return get_subrange_chisquare(obs, pdf, data, "full");
}

//Create an exponential PDF sum
RooAbsPdf* create_exponential(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  vector<RooRealVar*> coeffs;
  vector<RooExponential*> exps;
  RooArgList pdfs;
  RooArgList coefficients;
  for(int i = 1; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, i, tag.Data()), -0.1, -10., 10.));
    exps.push_back(new RooExponential(Form("exp_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), Form("exp_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), obs, *vars.back()));
    pdfs.add(*exps.back());
    coeffs.push_back(new RooRealVar(Form("exp_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1.e3, 0., 1.e8));
    coefficients.add(*coeffs.back());
  }
  if(order == 1) {
    pdfs.at(0)->SetTitle(Form("Exponential PDF, order %i", order));
    return ((RooAbsPdf*) pdfs.at(0));
  }
  return new RooAddPdf(Form("exp_%i_pdf_order_%i%s", set, order, tag.Data()), Form("Exponential PDF, order %i", order), pdfs, coefficients, false);
}

//Create an exponential PDF sum
RooAbsPdf* create_recursive_exponential(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  vector<RooRealVar*> coeffs;
  vector<RooExponential*> exps;
  RooArgList pdfs;
  RooArgList coefficients;
  for(int i = 1; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, i, tag.Data()), -0.1, -10., 10.));
    exps.push_back(new RooExponential(Form("exp_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), Form("exp_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), obs, *vars.back()));
    pdfs.add(*exps.back());
    if(i < order) {
      coeffs.push_back(new RooRealVar(Form("exp_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 0.5, 0., 1.));
      coefficients.add(*coeffs.back());
    }
  }
  if(order == 1) {
    pdfs.at(0)->SetTitle(Form("Exponential PDF, order %i", order));
    return ((RooAbsPdf*) pdfs.at(0));
  }
  return new RooAddPdf(Form("exp_%i_pdf_order_%i%s", set, order, tag.Data()), Form("Exponential PDF, order %i", order), pdfs, coefficients, true);
}

//Create an exponential PDF sum from RooGenericPdf
RooAbsPdf* create_generic_exponential(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) return nullptr;
  vector<RooRealVar*> vars;
  RooArgList var_list;
  var_list.add(obs);
  TString formula = "";
  if(order == 1) {
    formula = "TMath::Exp(@1*@0)";
    vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, 1, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, 1, tag.Data()), -0.1, -10., 10.));
    var_list.add(*vars.back());
  } else {
    for(int i = 1; i <= order; ++i) {
      if(i < order) { //keep adding exponential terms
        formula += Form("%s@%i*TMath::Exp(@%i*@0)", (i > 1) ? " + " : "", var_list.getSize(), var_list.getSize()+1);
        vars.push_back(new RooRealVar(Form("exp_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 0.1, 0., 1));
        var_list.add(*vars.back());
        vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, i, tag.Data()), -0.01, -100., 0.));
        var_list.add(*vars.back());
      } else { //add the last term with a coefficient such that the sum is 1
        formula += " + (1";
        for(int j = 1; j < order; ++j) formula += Form(" - @%i", 2*j-1);
        formula += Form(")*TMath::Exp(@%i*@0)", 2*order-1);
        // "@%i*TMath::Exp(@%i*@0)", var_list.getSize(), var_list.getSize()+1);
        vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, i, tag.Data()), -1., -100., 0.));
        var_list.add(*vars.back());
      }
    }
  }
  cout << __func__ << ": Order " << order << " formula = " << formula.Data() << endl;
  RooGenericPdf* pdf = new RooGenericPdf(Form("exp_%i_pdf_order_%i%s", set, order, tag.Data()), formula.Data(), var_list);
  pdf->SetTitle(Form("Exponential PDF, order %i", order));
  return pdf;
}

//Create an power law PDF sum
RooAbsPdf* create_powerlaw(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  vector<RooRealVar*> coeffs;
  vector<RooPowerLaw*> pwrs;
  RooArgList pdfs;
  RooArgList coefficients;
  for(int i = 1; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("pwr_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i power%s", set, order, i, tag.Data()), 1., -100., 1.));
    pwrs.push_back(new RooPowerLaw(Form("pwr_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), Form("pwr_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), obs, *vars.back()));
    pdfs.add(*pwrs.back());
    coeffs.push_back(new RooRealVar(Form("pwr_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1.e3, 0., 1.e6));
    coefficients.add(*coeffs.back());
  }
  if(order == 1) {
    pdfs.at(0)->SetTitle(Form("Power law PDF, order %i", order));
    return ((RooAbsPdf*) pdfs.at(0));
  }
  return new RooAddPdf(Form("pwr_%i_pdf_order_%i%s", set, order, tag.Data()), Form("Power law PDF, order %i", order), pdfs, coefficients, false);
}

//Create an power law PDF sum from RooGenericPdf
RooAbsPdf* create_generic_powerlaw(RooRealVar& obs, int order, int set, TString tag = "") {
  vector<RooRealVar*> vars;
  RooArgList var_list;
  var_list.add(obs);
  TString formula = "";
  for(int i = 1; i <= order; ++i) {
    if(i == 1) {
      formula = "@1*TMath::Power(@0,@2)";
      vars.push_back(new RooRealVar(Form("pwr_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1., 0., 1.e8));
      var_list.add(*vars.back());
      vars.push_back(new RooRealVar(Form("pwr_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i power%s", set, order, i, tag.Data()), -1., -100., 0.));
      var_list.add(*vars.back());
    } else {
      formula += Form(" + @%i*TMath::Power(@0,@%i)", 2*i+1, 2*i+2);
      vars.push_back(new RooRealVar(Form("pwr_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1., 0., 1.e8));
      var_list.add(*vars.back());
      vars.push_back(new RooRealVar(Form("pwr_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i power%s", set, order, i, tag.Data()), -1., -100., 0.));
      var_list.add(*vars.back());
    }
  }
  RooGenericPdf* pdf = new RooGenericPdf(Form("pwr_%i_pdf_order_%i%s", set, order, tag.Data()), formula.Data(), var_list);
  pdf->SetTitle(Form("Power law PDF, order %i", order));
  return pdf;
}

//Create a Laurent series PDF sum
RooAbsPdf* create_laurent(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0 || order > 6) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  RooArgList var_list;
  var_list.add(obs);
  TString formula = "";
  //take the default Laurent series with x0 = 0 from H->Zgamma analysis
  if(order == 1) formula = "TMath::Power(@0, -3)"; //Add a default 1st order
  if(order == 2) formula = "TMath::Power(@0, -3) + @1*TMath::Power(@0, -4)";
  if(order == 3) formula = "TMath::Power(@0, -3) + @1*TMath::Power(@0, -4) + @2*TMath::Power(@0, -5)";
  if(order == 4) formula = "TMath::Power(@0, -3) + @1*TMath::Power(@0, -4) + @2*TMath::Power(@0, -5) + @3*TMath::Power(@0, -6)";
  if(order == 5) formula = "TMath::Power(@0, -2) + @1*TMath::Power(@0, -3) + @2*TMath::Power(@0, -4) + @3*TMath::Power(@0, -5) + @4*TMath::Power(@0, -6)";
  if(order == 6) formula = "TMath::Power(@0, -2) + @1*TMath::Power(@0, -3) + @2*TMath::Power(@0, -4) + @3*TMath::Power(@0, -5) + @4*TMath::Power(@0, -6) + @5*TMath::Power(@0, -7)";
  for(int i = 1; i < order; ++i) {
    vars.push_back(new RooRealVar(Form("lrt_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("lrt_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1., 0., 1.e8));
    var_list.add(*vars.back());
  }
  RooGenericPdf* pdf = new RooGenericPdf(Form("lrt_%i_pdf_order_%i%s", set, order, tag.Data()), formula.Data(), var_list);
  pdf->SetTitle(Form("Laurent series PDF, order %i", order));
  return pdf;
}

//Create a Chebychev polynomial PDF
RooChebychev* create_chebychev(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  RooArgList list;
  std::map<int, std::vector<double>> initial_params;
  initial_params[0] = {1.};
  initial_params[1] = {1., 0.1};
  initial_params[2] = {-1., 0.1, 0.1};
  initial_params[3] = {-0.7, 0.3, -0.07, -0.001};
  initial_params[4] = {-0.7, 0.3, -0.07, -0.001, 0.015};
  initial_params[5] = {-0.7, 0.3, -0.07, -0.001, 0.015, -0.02};
  initial_params[6] = {-0.7, 0.3, -0.07, -0.001, 0.015, -0.02, -0.01};
  for(int i = 1; i <= order; ++i) {
    const bool has_params = initial_params.count(order) && ((int) initial_params[order].size()) > i;
    vars.push_back(new RooRealVar(Form("chb_%i_order_%i_%i%s", set, order, i, tag.Data()), Form("chb_%i_order_%i_%i%s", set, order, i, tag.Data()),
                                  (has_params) ? initial_params[order][i] : 1./pow(10.,i), -25., 25.));
    list.add(*vars.back());
  }
  return new RooChebychev(Form("chb_%i_order_%i%s", set, order, tag.Data()), Form("Chebychev PDF, order %i", order), obs, list);
}

RooAbsPdf* create_generic_bernstein(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> variables;
  RooArgSet var_set;
  var_set.add(obs);
  TString formula;
  double xmin = obs.getMin();
  double xmax = obs.getMax();
  TString var_form = Form("(%s - %.3f)/%.3f", obs.GetName(), xmin, (xmax - xmin));
  double vals[] = {1.404, 2.443e-1, 5.549e-1, 3.675e-1, 0., 0., 0., 0., 0., 0., 0.};

  for(int ivar = 1; ivar <= order; ++ivar) {
    RooRealVar* v = new RooRealVar(Form("bst_%i_order_%i_c_%i%s", set, order, ivar, tag.Data()), Form("bst_%i_order_%i_c_%i%s", set, order, ivar, tag.Data()), 1./pow(10,ivar), -5., 5.);
    formula += Form("%.0f*(%s)^%i*(1-%s)^%i*%s", TMath::Binomial(order, ivar), var_form.Data(), ivar, var_form.Data(), order-ivar, v->GetName());
    if(ivar < order) formula += " + ";
    var_set.add(*v);
    variables.push_back(v);
  }
  cout << "######################\n"
       << "#### Bernstein order " << order << " formula: " << formula.Data() << endl
       << "######################\n";
  RooAbsPdf* pdf = new RooGenericPdf(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), formula.Data(), var_set);
  return pdf;
}

//Create a Combine fast Bernstein polynomial PDF
RooAbsPdf* create_fast_bernstein(RooRealVar& obs, const int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 1; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_%i_order_%i_%i%s", set, order, i, tag.Data()), Form("bst_%i_order_%i_%i%s", set, order, i, tag.Data()), 1./pow(10.,i), -25., 25.));
    list.add(*vars.back());
  }
  RooAbsPdf* pdf;
  //FIXME have a better way to set the template
  switch(order) {
  case 1 : pdf = new RooBernsteinFast<1 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 2 : pdf = new RooBernsteinFast<2 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 3 : pdf = new RooBernsteinFast<3 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 4 : pdf = new RooBernsteinFast<4 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 5 : pdf = new RooBernsteinFast<5 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 6 : pdf = new RooBernsteinFast<6 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 7 : pdf = new RooBernsteinFast<7 >(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list); break;
  // case 8 : pdf = new RooBernsteinFast<8 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  // case 9 : pdf = new RooBernsteinFast<9 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  // case 10: pdf = new RooBernsteinFast<10>(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  default: return nullptr;
  }
  if(!pdf) return nullptr;
  switch(order) {
  case 1 : ((RooBernsteinFast<1 >*) pdf)->protectSubRange(true); break;
  case 2 : ((RooBernsteinFast<2 >*) pdf)->protectSubRange(true); break;
  case 3 : ((RooBernsteinFast<3 >*) pdf)->protectSubRange(true); break;
  case 4 : ((RooBernsteinFast<4 >*) pdf)->protectSubRange(true); break;
  case 5 : ((RooBernsteinFast<5 >*) pdf)->protectSubRange(true); break;
  case 6 : ((RooBernsteinFast<6 >*) pdf)->protectSubRange(true); break;
  case 7 : ((RooBernsteinFast<7 >*) pdf)->protectSubRange(true); break;
  default: break;
  }

  return pdf;
}

//Create a Bernstein polynomial PDF
RooAbsPdf* create_bernstein(RooRealVar& obs, const int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 1; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_%i_order_%i_%i%s", set, order, i, tag.Data()), Form("bst_%i_order_%i_%i%s", set, order, i, tag.Data()), 1./pow(10.,i), -5., 5.));
    list.add(*vars.back());
  }
  RooAbsPdf* pdf = new RooBernstein(Form("bst_%i_order_%i%s", set, order, tag.Data()), Form("Bernstein PDF, order %i", order), obs, list);
  return pdf;
}

//Fit exponentials and add passing ones
std::pair<int,double> add_exponentials(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 3;
  const double max_chisq = 2.; //per DOF
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    // RooAbsPdf* basePdf = create_exponential(obs, order, set);
    RooAbsPdf* basePdf = create_recursive_exponential(obs, order, set);
    // RooAbsPdf* basePdf = create_generic_exponential(obs, order, set);
    const bool use_base_pdf = false;
    RooAbsPdf* pdf = basePdf;
    if(!use_base_pdf) { //Wrap the exponential in a RooAddPdf
      RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                         data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
      pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
    }
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    int nentries = data.numEntries();
    const double chi_sq = get_chi_squared(obs, pdf, data, useSideBands, &nentries);
    const int dof = (nentries - 2*order);  //N(params) = 2 * N(exp)
    double p_chi_sq;
    const bool accept = (perform_chisq_test(chi_sq, dof, &p_chi_sq) && !force_fit_order_) || (force_fit_order_ && order == 2);
    if(accept) {
      // bool passes_f_test = (chi_min > 1.e8) || perform_f_test(chi_min, dof + (order - best_order), chi_sq, dof);
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(chi_sq < min_chi) {min_chi = chi_sq; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Exponential order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof
                         << " (p = " << p_chi_sq << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit powerlaw orders and add passing ones
std::pair<int,double> add_powerlaws(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 3;
  const double max_chisq = 2.; //per DOF
  const double chi_cutoff = 3.85;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    // RooAbsPdf* basePdf = create_generic_powerlaw(obs, order, set);
    RooAbsPdf* basePdf = create_powerlaw(obs, order, set);
    //Wrap the exponential in a RooAddPdf
    RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                         data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
    RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    int nentries = data.numEntries();
    const double chi_sq = get_chi_squared(obs, pdf, data, useSideBands, &nentries);
    const int dof = (nentries - 2*order);  //N(params) = 2*N(power laws)
    double p_chi_sq;
    const bool accept = (perform_chisq_test(chi_sq, dof, &p_chi_sq) && !force_fit_order_) || (force_fit_order_ && order == 2);
    if(accept) {
      // list.add(*pdf);
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(chi_sq < min_chi) {min_chi = chi_sq; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Powerlaw order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof
                         << " (p = " << p_chi_sq << ")" " --> add PDF = " << accept << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit Laurent series orders and add passing ones
std::pair<int,double> add_laurents(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 6;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    RooAbsPdf* basePdf = create_laurent(obs, order, set);
    //Wrap the exponential in a RooAddPdf
    RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                         data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
    RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    int nentries = data.numEntries();
    const double chi_sq = get_chi_squared(obs, pdf, data, useSideBands, &nentries);
    const int dof = (nentries - 2*order);  //N(params) = 2 * N(exp)
    double p_chi_sq;
    const bool accept = (perform_chisq_test(chi_sq, dof, &p_chi_sq) && !force_fit_order_) || (force_fit_order_ && order == 3);
    if(accept) {
      // list.add(*pdf);
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(chi_sq < min_chi) {min_chi = chi_sq; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Laurent order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof
                         << " (p = " << p_chi_sq << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit Chebychev polynomials and add passing ones
std::pair<int, double> add_chebychevs(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 4;
  const double max_chisq = 2.; //per DOF
  //for finding the best fitting function
  double chi_min = 1.e10;
  const double chi_cutoff = 3.85; //amount of chi^2 gain to select a higher order function
  int num_added = -1;
  int best_order = -1;
  const int max_tries = 2; //retry if poor fit to get a better one
  for(int order = 1; order <= max_order; ++order) {
    RooChebychev* basePdf = create_chebychev(obs, order, set);
    //Wrap the chebychev in a RooAddPdf
    RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                         data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
    RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));

    int dof = data.numEntries();
    double chi_sq = 1.e10;
    int ntries = 0;
    do {
      ++ntries;
      if(ntries > 1) cout << "---> Re-fitting order " << order << " Chebychev polynomial (chi^2 = " << chi_sq << ")\n";
      if(useSideBands)
        pdf->fitTo(data, RooFit::PrintLevel(-1 + 2*(verbose > 2)), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
      else
        pdf->fitTo(data, RooFit::PrintLevel(-1 + 2*(verbose > 2)), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
      int nentries = data.numEntries();
      chi_sq = get_chi_squared(obs, pdf, data, useSideBands, &nentries);
      dof = (nentries - order - 1);  //DOF = number of variables + normalization
    } while(chi_sq / dof > max_chisq && ntries < max_tries); //check if it should be re-fit

    double p_chi_sq;
    const bool accept = (perform_chisq_test(chi_sq, dof, &p_chi_sq) && !force_fit_order_) || (force_fit_order_ && order == 3);
    if(accept) {
      bool is_best = (chi_min > 1.e8) || perform_f_test(chi_min, dof + (order - best_order), chi_sq, dof);
      printf("--- Best order = %i, current = %i: F(test) = %o\n", best_order, order, is_best);
      list.add(*basePdf);
      ++num_added;
      delete pdf;
      if(force_best_fit_) is_best = order == 3; //force the 3rd order to be the nominal distribution
      if(is_best) { //chi_sq < chi_min - chi_cutoff) {
        index = num_added;
        chi_min = chi_sq;
        best_order = order;
      }
    } else {
      delete pdf;
      delete basePdf;
    }
    if(verbose > 1) cout << "######################\n"
                         << "### Chebychev order " << order << " has chisq = "
                         << chi_sq << " / " << dof << " = " << chi_sq/dof
                         << endl
                         << "######################\n";
  }
  if(verbose > 0) cout << "######################\n"
                       << "### Best fit Chebychev order is " << best_order << " with chisq = " << chi_min << endl
                       << "######################\n";
  return std::pair<int, double>(index, chi_min);
}

//Fit Bernstein polynomials and add passing ones
std::pair<int, double> add_bernsteins(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 4;
  const double max_chisq = 2.; //per DOF
  //for finding the best fitting function
  double chi_min = 1.e10;
  const double chi_cutoff = 3.85; //minimum difference in chi^2 to consider a higher order function
  int num_added = -1;
  int best_order = -1;
  const int max_tries = 2; //retry if poor fit to get a better one
  for(int order = 1; order <= max_order; ++order) {
    RooAbsPdf* basePdf;
    if(use_generic_bernstein_)   basePdf = create_generic_bernstein(obs, order, set);
    else if(use_fast_bernstein_) basePdf = create_fast_bernstein   (obs, order, set);
    else                         basePdf = create_bernstein        (obs, order, set);
    //Wrap the bernstein in a RooAddPdf
    RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                         data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
    RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
    // RooAbsPdf* pdf = basePdf;
    double chi_sq = 1e10;
    int dof = data.numEntries();
    int ntries = 0;
    do {
      ++ntries;
      if(useSideBands) {
        // basePdf->fitTo(data, RooFit::PrintLevel((verbose > 2) ? 1 : -1), //RooFit::Minimizer("Minuit2", "migrad"),
        //            RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
        pdf->fitTo(data, RooFit::PrintLevel((verbose > 2) ? 1 : -1), //RooFit::Minimizer("Minuit2", "migrad"),
                   RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
      } else {
        pdf->fitTo(data, RooFit::Extended(true), RooFit::PrintLevel((verbose > 2) ? 1 : -1),
                   RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
      }
      int nentries = data.numEntries();
      chi_sq = get_chi_squared(obs, pdf, data, useSideBands, &nentries);
      dof = (nentries - order - 1);  //DOF = number of variables + normalization
      chi_sq =  get_chi_squared(obs, pdf, data, useSideBands);
    } while(chi_sq / dof > max_chisq && ntries < max_tries);

    double p_chi_sq;
    const bool accept = (perform_chisq_test(chi_sq, dof, &p_chi_sq) && !force_fit_order_) || (force_fit_order_ && order == 3);
    if(accept) {
      // list.add(*pdf);
      list.add(*basePdf);
      ++num_added;
      if(chi_sq < chi_min - chi_cutoff) {
        index = num_added;
        chi_min = chi_sq;
        best_order = order;
      }
    }
    else {
      delete pdf;
    }
    if(verbose > 1) {
      cout << "######################\n"
           << "### Bernstein order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl
           << "######################\n";
    }
  }
  if(verbose > 0) cout << "######################\n"
                       << "### Best fit Bernstein order is " << best_order << " with chisq = " << chi_min << endl
                       << "######################\n";
  return std::pair<int, double>(index, chi_min);
}

//Fit a combination of exponential and polynomial components (DY + WW)
std::pair<int, double> add_dy_ww(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const double max_chisq = 2.; //per DOF
  const int max_tries = 2; //retry if poor fit to get a better one

  //fixed polynomial order for the WW/ttbar contribution
  const int poly_order = 3;
  RooAbsPdf* polyPdf;
  const bool use_chebychev = true;
  if(use_chebychev) {
    polyPdf = create_chebychev(obs, poly_order+1, set, "_comb");
  } else {
    if(use_generic_bernstein_)   polyPdf = create_generic_bernstein(obs, poly_order, set, "_comb");
    else if(use_fast_bernstein_) polyPdf = create_fast_bernstein   (obs, poly_order, set, "_comb");
    else                         polyPdf = create_bernstein        (obs, poly_order, set, "_comb");
  }

  //set the Bernstein initial parameters to an MC fit
  auto polyVar = RooArgList(*(polyPdf->getVariables()));
  polyVar.Print();
  if(use_chebychev) {
    ((RooRealVar*) polyVar.at(0))->setVal(-0.743);
    ((RooRealVar*) polyVar.at(1))->setVal( 0.295);
    ((RooRealVar*) polyVar.at(2))->setVal(-0.082);
  } else {
    ((RooRealVar*) polyVar.at(0))->setVal(1.66889);
    ((RooRealVar*) polyVar.at(1))->setVal(1.57434);
    ((RooRealVar*) polyVar.at(2))->setVal(-2.44089);
  }


  //fixed exponential order for the DY contribution
  const int exp_order = 1;
  //set the Exponential initial parameters to an MC fit
  RooAbsPdf* expPdf = create_exponential(obs, exp_order, set, "_comb");
  auto expVar = RooArgList(*(expPdf->getVariables()));
  expVar.Print();
  ((RooRealVar*) expVar.at(0))->setVal(-0.135877);

  //add the PDFs together
  RooRealVar* comb_coeff = new RooRealVar(Form("comb_%i_coeff", set), Form("comb_%i_coeff", set), 0.1, 0., 1.);
  RooAddPdf* basePdf     = new RooAddPdf (Form("comb_%i_pdf"  , set), Form("comb_%i_pdf"  , set),
                                          RooArgList(*polyPdf, *expPdf), RooArgList(*comb_coeff), false);

  //Wrap the pdf in a RooAddPdf
  RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                       data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
  RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
  // RooAbsPdf* pdf = basePdf;
  int nentries = data.numEntries();
  double chi_sq = 1e10;
  int ntries = 0;
  int dof = nentries;
  do {
    ++ntries;
    if(useSideBands) {
      basePdf->fitTo(data, RooFit::PrintLevel((verbose > 2) ? 1 : -1), //RooFit::Minimizer("Minuit2", "migrad"),
                     RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
      // pdf->fitTo(data, RooFit::Extended(true), RooFit::PrintLevel((verbose > 2) ? 1 : -1), //RooFit::Minimizer("Minuit2", "migrad"),
      //            RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    } else {
      pdf->fitTo(data, RooFit::Extended(true), RooFit::PrintLevel((verbose > 2) ? 1 : -1),
                   RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    }
    chi_sq =  get_chi_squared(obs, pdf, data, useSideBands, &nentries);
    dof = (nentries - 2*exp_order - (poly_order+1) - 1);  //DOF = number of variables + normalization
  } while(chi_sq / dof > max_chisq && ntries < max_tries);

  if(true || chi_sq / dof < max_chisq) {
    basePdf->SetTitle("Combined DY/WW");
    // list.add(*pdf);
    list.add(*basePdf);
  }
  else {
    delete pdf;
  }
  if(verbose > 1) {
    cout << "######################\n"
         << "### Exp+Poly has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl
         << "######################\n";
  }
  return std::pair<int, double>(list.getSize()-1, chi_sq);
}

RooSimultaneous* construct_simultaneous_pdf(RooDataHist& data, RooRealVar& obs, RooCategory& categories, bool useSideBands, int& index, int set, int verbose = 0) {
  RooArgList pdfList;
  add_bernsteins(data, obs, pdfList, useSideBands, index, set, verbose);
  // add_chebychevs(data, obs, pdfList, useSideBands, index, set, verbose);
  // add_exponentials(data, obs, pdfList, useSideBands, set, verbose);
  RooSimultaneous* pdfs =  new RooSimultaneous("bkg_multi", "Background function PDF choice", categories);
  for(int ipdf = 0; ipdf < pdfList.getSize(); ++ipdf) {
    RooAbsPdf* pdf = (RooAbsPdf*) pdfList.at(ipdf);
    categories.defineType(Form("index_%i", ipdf), ipdf);
    pdfs->addPdf(*pdf, Form("index_%i", ipdf));
  }
  return pdfs;
}

RooMultiPdf* construct_multipdf(RooDataHist& data, RooRealVar& obs, RooCategory& categories, bool useSideBands, int& index, int set, int verbose = 0) {
  RooArgList pdfList;
  std::pair<int, double> result;
  double chi_min = 1e10;
  // result = add_bernsteins(data, obs, pdfList, useSideBands, index, set, verbose);
  // if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  result = add_chebychevs(data, obs, pdfList, useSideBands, index, set, verbose);
  if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  if(use_exp_family_) {
    result = add_exponentials(data, obs, pdfList, useSideBands, set, verbose);
    // if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  }
  if(use_power_family_) {
    result = add_powerlaws(data, obs, pdfList, useSideBands, set, verbose);
    // if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  }
  if(use_laurent_family_) {
    result = add_laurents(data, obs, pdfList, useSideBands, set, verbose);
    // if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  }
  if(use_dy_ww_shape_) {
    result = add_dy_ww(data, obs, pdfList, useSideBands, set, verbose);
    if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  }
  RooMultiPdf* pdfs = nullptr;
  if(test_single_function_) {
    RooAbsPdf* best_pdf = (RooAbsPdf*) pdfList.at(index);
    RooArgList new_list;
    new_list.add(*best_pdf);
    pdfs = new RooMultiPdf("bkg_multi", "Background function PDF choice", categories, new_list);
    cout << "################################################################\n"
         << "################################################################\n"
         << "## N(multi pdf) PDFs = " << categories.numTypes() << endl
         << "################################################################\n"
         << "################################################################\n";
    index = 0;
  } else  pdfs = new RooMultiPdf("bkg_multi", "Background function PDF choice", categories, pdfList);

  return pdfs;
}

#endif
