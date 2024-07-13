#ifndef __CONSTRUCT_MULTIDIM__
#define __CONSTRUCT_MULTIDIM__
#include "bemu_defaults.C"
#include "bemu_bkg_functions.C"
#include "util.hh"

//Construct multi-dimensional PDF with discrete index
bool useFrameChiSq_         = false; //use a roo plot frame to evaluate chi^2
bool useManualChisq_        = true ; //calcuate chi^2 values by hand with histograms
bool useDataBinErrors_      = false; //use data bin errors when calculating chi^2
bool use_generic_bernstein_ = false; //poly(x)
bool use_fast_bernstein_    = true ; //poly(x)
bool use_exp_family_        = true ; //exp(x)
bool use_power_family_      = true ; //x^p
bool use_laurent_family_    = false; //Laurent
bool use_inv_poly_family_   = false; //1/(polynomial)
bool use_dy_ww_shape_       = true ; //model fit to the Drell-Yan MC
bool force_fit_order_       = true ; //force only the inclusion of fixed orders of each family
bool force_best_fit_        = false; //force the nominal PDF to be a specific function
bool enforce_ftest_         = false; //once a function fails the F-test, stop adding functions

bool test_single_function_  = false; //only pass 1 function into the PDF ensemble

RooAbsPdf* additional_bkg_  = nullptr; //if some background component is modeled separately
double     additional_bkg_norm_ = -1.; //fixed additional background normalization

// Structure for storing fit result information
struct fit_res_t {
  bool accept_;
  double chi_sq_;
  int ndof_;
  int nparams_;
  int nentries_;
  double chi_sq_per_dof_;
  double p_;
  fit_res_t(bool accept, double chi_sq, int ndof, int nparams, int nentries) : accept_(accept), chi_sq_(chi_sq), ndof_(ndof),
                                                                               nparams_(nparams), nentries_(nentries) {
    chi_sq_per_dof_ = (ndof > 0) ? chi_sq / ndof : 0.;
    p_ = (ndof > 0.) ? TMath::Prob(chi_sq, ndof) : 0.;
  }
};

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
  if(!pdf) return 0.;
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
    }
    return get_manual_subrange_chisquare(obs, pdf, data, "full");
  }
  if(useFrameChiSq_ || TString(pdf->GetTitle()).Contains("Exponential")) {
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

//-----------------------------------------------------------------------------------------------------------------------------------
// Helper function to combine the data-driven fit with additional backgrounds
RooAbsPdf* wrap_pdf(RooAbsPdf* pdf, double norm) {
  if(zmumu_model_) { //plot the PDF combined with Z->mumu PDF
    RooRealVar* pdf_norm = new RooRealVar(Form("%s_pdf_norm", pdf->GetName()), Form("%s_pdf_norm", pdf->GetName()), norm - additional_bkg_norm_, 0.5*norm, 1.5*norm);
    RooRealVar* bkg_norm = new RooRealVar(Form("%s_bkg_norm", pdf->GetName()), Form("%s_bkg_norm", pdf->GetName()), additional_bkg_norm_       );
    RooAbsPdf* tot_pdf = new RooAddPdf(Form("%s_total", pdf->GetName()), pdf->GetTitle(), RooArgList(*pdf, *additional_bkg_), RooArgList(*pdf_norm, *bkg_norm));
    bkg_norm->setConstant(true);
    return tot_pdf;
  } else {
    RooRealVar* pdf_norm = new RooRealVar(Form("%s_pdf_norm", pdf->GetName()), Form("%s_pdf_norm", pdf->GetName()), norm, 0.5*norm, 1.5*norm);
    RooAbsPdf* tot_pdf = new RooAddPdf(Form("%s_total", pdf->GetName()), pdf->GetTitle(), RooArgList(*pdf), RooArgList(*pdf_norm));
    return tot_pdf;
  }
  return pdf; //no wrapping
}

//-----------------------------------------------------------------------------------------------------------------------------------
// Fit a PDF to the dataset and determine whether or not to accept it
fit_res_t fit_pdf_to_data(RooAbsPdf* pdf, RooDataHist& data, RooRealVar& obs, bool useSideBands, int verbose = 0) {
  if(useSideBands)
    pdf->fitTo(data, RooFit::PrintLevel(-1 + max(0, verbose-2)), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
  else
    pdf->fitTo(data, RooFit::PrintLevel(-1 + max(0, verbose-2)), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
  int nentries = data.numEntries(); //non-constant, as this is updated in the chi^2 calculation based on region used
  const double chi_sq = get_chi_squared(obs, pdf, data, useSideBands, &nentries);
  const int nparams = count_pdf_params(pdf);
  const int dof = (nentries - nparams - 1);  //Assume normalization is not counted in the parameter list
  double p_chi_sq;
  const bool accept = perform_chisq_test(chi_sq, dof, &p_chi_sq);
  return fit_res_t(accept,chi_sq, dof, nparams, nentries);
}

///////////////////////////////////////////
// Add functions to the envelope
///////////////////////////////////////////


//Fit exponentials and add passing ones
std::pair<int,double> add_exponentials(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 3;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    RooAbsPdf* basePdf = create_recursive_exponential(obs, order, set);
    RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
    auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    const bool accept = (res.accept_ && !force_fit_order_) || (force_fit_order_ && order == 2);
    if(accept) {
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(res.chi_sq_ < min_chi) {min_chi = res.chi_sq_; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Exponential order " << order << " has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit power laws and add passing ones
std::pair<int,double> add_powerlaws(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 3;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    RooAbsPdf* basePdf = create_powerlaw(obs, order, set);
    RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
    auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    const bool accept = (res.accept_ && !force_fit_order_) || (force_fit_order_ && order == 2);
    if(accept) {
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(res.chi_sq_ < min_chi) {min_chi = res.chi_sq_; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Power law order " << order << " has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
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
    RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
    auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    const bool accept = (res.accept_ && !force_fit_order_) || (force_fit_order_ && order == 1);
    if(accept) {
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(res.chi_sq_ < min_chi) {min_chi = res.chi_sq_; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Laurent order " << order << " has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit inverse polynomial series orders and add passing ones
std::pair<int,double> add_inv_poly(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 1;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    RooAbsPdf* basePdf = create_inv_polynomial(obs, order, set);
    RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
    auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    const bool accept = (res.accept_ && !force_fit_order_) || (force_fit_order_ && order == 1);
    if(accept) {
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(res.chi_sq_ < min_chi) {min_chi = res.chi_sq_; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Inverse poly order " << order << " has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit Chebychev polynomials and add passing ones
std::pair<int,double> add_chebychevs(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 3;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 3; order <= max_order; ++order) {
    RooAbsPdf* basePdf = create_chebychev(obs, order, set);
    RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
    auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    const bool accept = (res.accept_ && !force_fit_order_) || (force_fit_order_ && order == 3);
    if(accept) {
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(res.chi_sq_ < min_chi) {min_chi = res.chi_sq_; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Chebychev order " << order << " has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit Bernstein polynomials and add passing ones
std::pair<int,double> add_bernsteins(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 4;
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 1; order <= max_order; ++order) {
    RooAbsPdf* basePdf;
    if(use_generic_bernstein_)   basePdf = create_generic_bernstein(obs, order, set);
    else if(use_fast_bernstein_) basePdf = create_fast_bernstein   (obs, order, set);
    else                         basePdf = create_bernstein        (obs, order, set);
    RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
    auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    const bool accept = (res.accept_ && !force_fit_order_) || (force_fit_order_ && order == 3);
    if(accept) {
      list.add(*basePdf);
    } else {
      delete pdf;
    }
    if(res.chi_sq_ < min_chi) {min_chi = res.chi_sq_; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Bernstein order " << order << " has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit a combination of exponential and polynomial components (DY + WW)
std::pair<int, double> add_dy_ww(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const double max_chisq = 2.; //per DOF
  const int max_tries = 2; //retry if poor fit to get a better one

  //fixed polynomial order for the WW/ttbar contribution
  const int poly_order = 3;
  RooAbsPdf* polyPdf = create_chebychev(obs, poly_order+1, set, "_comb");

  //set the polynomial initial parameters to an MC fit
  auto polyVar = RooArgList(*(polyPdf->getVariables()));
  ((RooRealVar*) polyVar.at(0))->setVal(1.66889);
  ((RooRealVar*) polyVar.at(1))->setVal(1.57434);
  ((RooRealVar*) polyVar.at(2))->setVal(-2.44089);


  //fixed exponential order for the DY contribution
  const int exp_order = 1;
  //set the Exponential initial parameters to an MC fit
  RooAbsPdf* expPdf = create_exponential(obs, exp_order, set, "_comb");
  auto expVar = RooArgList(*(expPdf->getVariables()));
  ((RooRealVar*) expVar.at(0))->setVal(-0.135877);

  //add the PDFs together
  RooRealVar* comb_coeff = new RooRealVar(Form("comb_%i_coeff", set), Form("comb_%i_coeff", set), 0.1, 0., 1.);
  RooAddPdf* basePdf     = new RooAddPdf (Form("comb_%i_pdf"  , set), Form("comb_%i_pdf"  , set),
                                          RooArgList(*polyPdf, *expPdf), RooArgList(*comb_coeff), false);
  basePdf->SetTitle("Combined DY/WW");

  RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
  auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
  //force the PDF inclusion
  list.add(*basePdf);
  if(verbose > 1) cout << "######################\n"
                       << "### DY Poly+exp PDF has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                       << " (p = " << res.p_ << ")" << endl
                       << "######################\n";
  return std::pair<int, double>(list.getSize()-1, res.chi_sq_);
}

//Embedding-fit motivated PDF
std::pair<int, double> add_dy_pdf(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {

  //fit a linear + Gaussian
  auto basePdf = create_gaus_poly_pdf(obs, 2, set);
  RooAbsPdf* pdf = wrap_pdf(basePdf, data.sumEntries());
  auto res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
  //force the PDF inclusion
  list.add(*basePdf);
  if(verbose > 1) cout << "######################\n"
                       << "### DY Gaus+poly PDF has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                       << " (p = " << res.p_ << ")" << endl
                       << "######################\n";

  //force order 1 as the best
  std::pair<int, double> poly_fit_res(list.getSize()-1, res.chi_sq_);

  // //fit a quadratic + Gaussian
  // basePdf = create_gaus_poly_pdf(obs, 2, set);
  // pdf = wrap_pdf(basePdf, data.sumEntries());
  // res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
  // //force the PDF inclusion
  // list.add(*basePdf);
  // if(verbose > 1) cout << "######################\n"
  //                      << "### DY Gaus+poly PDF has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
  //                      << " (p = " << res.p_ << ")" << endl
  //                      << "######################\n";

  if(use_exp_family_) {
    //fit an exponential + Gaussian
    basePdf = create_gaus_expo_pdf(obs, 1, set);
    pdf = wrap_pdf(basePdf, data.sumEntries());
    res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    //force the PDF inclusion
    list.add(*basePdf);
    if(verbose > 1) cout << "######################\n"
                         << "### DY Gaus+expo PDF has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }

  if(use_power_family_) {
    //fit a power law + Gaussian
    basePdf = create_gaus_power_pdf(obs, 1, set);
    pdf = wrap_pdf(basePdf, data.sumEntries());
    res = fit_pdf_to_data(pdf, data, obs, useSideBands, verbose);
    //force the PDF inclusion
    list.add(*basePdf);
    if(verbose > 1) cout << "######################\n"
                         << "### DY Gaus+power PDF has chisq = " << res.chi_sq_ << " / " << res.ndof_ << " = " << res.chi_sq_per_dof_
                         << " (p = " << res.p_ << ")" << endl
                         << "######################\n";
  }

  //default to the polynomial + Gaussian
  return poly_fit_res;
}


// Construct the envelope
RooMultiPdf* construct_multipdf(RooDataHist& data, RooRealVar& obs, RooCategory& categories, bool useSideBands, int& index, int set, int verbose = 0) {
  RooArgList pdfList;
  std::pair<int, double> result;
  double chi_min = 1e10;
  if(!use_dy_ww_shape_) {
    // result = add_bernsteins(data, obs, pdfList, useSideBands, set, verbose);
    // if(result.second < chi_min) {chi_min = result.second; index = result.first;}
    result = add_chebychevs(data, obs, pdfList, useSideBands, set, verbose);
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
    if(use_inv_poly_family_) {
      result = add_inv_poly(data, obs, pdfList, useSideBands, set, verbose);
      // if(result.second < chi_min) {chi_min = result.second; index = result.first;}
    }
  }
  if(use_dy_ww_shape_) {
    result = add_dy_pdf(data, obs, pdfList, useSideBands, set, verbose);
    chi_min = result.second; index = result.first; //force the use of the Gaussian+poly
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
