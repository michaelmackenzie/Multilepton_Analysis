#ifndef __CONSTRUCT_MULTIDIM__
#define __CONSTRUCT_MULTIDIM__
#include "bemu_defaults.C"

//Construct multi-dimensional PDF with discrete index
bool useFrameChiSq_ = false;
bool useManualChisq_ = true; //calcuate chi^2 values by hand with histograms
bool use_generic_bernstein_ = false;
bool use_fast_bernstein_ = true;

//----------------------------------------------------------------------------------------------------------------
//Get the chi-squared using a by-hand calculation
double get_manual_subrange_chisquare(RooRealVar& obs, RooAbsPdf* pdf, RooDataHist& data, const char* range = nullptr,
                                     const char* norm_range = nullptr, bool norm_skip = true, int* nbins = nullptr) {
  TH1* htmp_pdf  = pdf->createHistogram("htmp_chisq_pdf", obs);
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
    const double npdf = htmp_pdf->GetBinContent(ibin)*htmp_pdf->GetBinWidth(ibin);
    const double ndata = htmp_data->GetBinContent(ibin);
    const double val  = ndata - npdf;
    const double sigma = val*val / (npdf <= 0. ? 1.e-5 : npdf);
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
    if(useSideBands || blindData_) {
      const char* blind_region = (blindData_) ? "BlindRegion" : nullptr;
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
RooAbsPdf* create_exponential(RooRealVar& obs, int order, int set) {
  vector<RooRealVar*> vars;
  vector<RooRealVar*> coeffs;
  vector<RooExponential*> exps;
  RooArgList pdfs;
  RooArgList coefficients;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i", set, order, i), Form("exp_%i_order_%i_%i power", set, order, i), 1., -10., 10.));
    exps.push_back(new RooExponential(Form("exp_%i_pdf_order_%i_%i", set, order, i), Form("exp_%i_pdf_order_%i_%i", set, order, i), obs, *vars[i]));
    pdfs.add(*exps[i]);
    coeffs.push_back(new RooRealVar(Form("exp_%i_order_%i_n_%i", set, order, i), Form("exp_%i_order_%i_%i norm" , set, order, i), 1.e3, -1.e6, 1.e6));
    coefficients.add(*coeffs[i]);
  }
  if(order == 0) {
    pdfs.at(0)->SetTitle(Form("Exponential PDF, order %i", order));
    return ((RooAbsPdf*) pdfs.at(0));
  }
  return new RooAddPdf(Form("exp_%i_pdf_order_%i", set, order), Form("Exponential PDF, order %i", order), pdfs, coefficients, false);
}

//Create a Chebychev polynomial PDF
RooChebychev* create_chebychev(RooRealVar& obs, int order, int set) {
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("chb_%i_order_%i_%i", set, order, i), Form("chb_%i_order_%i_%i", set, order, i), 1./pow(10.,i), -5., 5.));
    list.add(*vars[i]);
  }
  return new RooChebychev(Form("chb_%i_order_%i", set, order), Form("Chebychev PDF, order %i", order), obs, list);
}

RooAbsPdf* create_generic_bernstein(RooRealVar& obs, int order, int set) {

  vector<RooRealVar*> variables;
  RooArgSet var_set;
  var_set.add(obs);
  TString formula;
  double xmin = obs.getMin();
  double xmax = obs.getMax();
  TString var_form = Form("(%s - %.3f)/%.3f", obs.GetName(), xmin, (xmax - xmin));
  double vals[] = {1.404, 2.443e-1, 5.549e-1, 3.675e-1, 0., 0., 0., 0., 0., 0., 0.};

  for(int ivar = 0; ivar <= order; ++ivar) {
    RooRealVar* v = new RooRealVar(Form("bst_%i_order_%i_c_%i", set, order, ivar), Form("bst_%i_order_%i_c_%i", set, order, ivar), 1./pow(10,ivar), -5., 5.);
    formula += Form("%.0f*(%s)^%i*(1-%s)^%i*%s", TMath::Binomial(order, ivar), var_form.Data(), ivar, var_form.Data(), order-ivar, v->GetName());
    if(ivar < order) formula += " + ";
    var_set.add(*v);
  }
  cout << "######################\n"
       << "#### Bernstein order " << order << " formula: " << formula.Data() << endl
       << "######################\n";
  RooAbsPdf* pdf = new RooGenericPdf(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), formula.Data(), var_set);
  return pdf;
}

//Create a Combine fast Bernstein polynomial PDF
RooAbsPdf* create_fast_bernstein(RooAbsReal& obs, const int order, int set) {
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_%i_order_%i_%i", set, order, i), Form("bst_%i_order_%i_%i", set, order, i), 1./pow(10.,i), -5., 5.));
    list.add(*vars[i]);
  }
  RooAbsPdf* pdf;
  //FIXME have a better way to set the template
  switch(order) {
  case 1 : pdf = new RooBernsteinFast<1 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 2 : pdf = new RooBernsteinFast<2 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 3 : pdf = new RooBernsteinFast<3 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 4 : pdf = new RooBernsteinFast<4 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 5 : pdf = new RooBernsteinFast<5 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 6 : pdf = new RooBernsteinFast<6 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 7 : pdf = new RooBernsteinFast<7 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
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
RooAbsPdf* create_bernstein(RooAbsReal& obs, const int order, int set) {
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_%i_order_%i_%i", set, order, i), Form("bst_%i_order_%i_%i", set, order, i), 1./pow(10.,i), -5., 5.));
    list.add(*vars[i]);
  }
  RooAbsPdf* pdf = new RooBernstein(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list);
  return pdf;
}

//Fit exponentials and add passing ones
std::pair<int,double> add_exponentials(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 3;
  const double max_chisq = 2.; //per DOF
  double min_chi = 1.e10;
  int min_index = -1;
  for(int order = 0; order <= max_order; ++order) {
    RooAbsPdf* basePdf = create_exponential(obs, order, set);
    //Wrap the exponential in a RooAddPdf
    RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                         data.sumEntries(), 0.5*data.sumEntries(), 1.5*data.sumEntries());
    RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    const int dof = (data.numEntries() - 2*order - 1);  //DOF = number of variables + normalization
    const double chi_sq = get_chi_squared(obs, pdf, data, useSideBands);
    if(chi_sq / dof < max_chisq) {
      list.add(*pdf);
    } else {
      delete pdf;
    }
    if(chi_sq < min_chi) {min_chi = chi_sq; min_index = list.getSize() - 1;}
    if(verbose > 1) cout << "######################\n"
                         << "### Exponential order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl
                         << "######################\n";
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit Chebychev polynomials and add passing ones
void add_chebychevs(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 5;
  const double max_chisq = 2.; //per DOF
  //for finding the best fitting function
  double chi_min = 1.e10;
  const double chi_cutoff = 3.85;
  int num_added = -1;
  int best_order = -1;
  for(int order = 1; order <= max_order; ++order) {
    RooChebychev* pdf = create_chebychev(obs, order, set);
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1 + 2*(verbose > 2)), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1 + 2*(verbose > 2)), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    const int dof = (data.numEntries() - order - 2);  //DOF = number of variables + normalization
    const double chi_sq = get_chi_squared(obs, pdf, data, useSideBands);
    if(chi_sq / dof < max_chisq) {
      list.add(*pdf);
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
    if(verbose > 1) cout << "######################\n"
                         << "### Chebychev order " << order << " has chisq = "
                         << chi_sq << " / " << dof << " = " << chi_sq/dof
                         << endl
                         << "######################\n";
  }
  if(verbose > 0) cout << "######################\n"
                       << "### Best fit Chebychev order is " << best_order << " with chisq = " << chi_min << endl
                       << "######################\n";
}

//Fit Bernstein polynomials and add passing ones
std::pair<int, double> add_bernsteins(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 5;
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
    const int dof = (data.numEntries() - order - 2);  //DOF = number of variables + normalization
    double chi_sq = 1e10;
    int ntries = 0;
    do {
      ++ntries;
      if(useSideBands) {
        pdf->fitTo(data, RooFit::Extended(true), RooFit::PrintLevel((verbose > 2) ? 1 : -1), //RooFit::Minimizer("Minuit2", "migrad"),
                   RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
        // pdf->fitTo(data, RooFit::Extended(true), RooFit::PrintLevel((verbose > 2) ? 1 : -1), //RooFit::Minimizer("Minuit2", "migrad"),
        //            RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
      } else {
        pdf->fitTo(data, RooFit::Extended(true), RooFit::PrintLevel((verbose > 2) ? 1 : -1),
                   RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
      }
      chi_sq =  get_chi_squared(obs, pdf, data, useSideBands);
    } while(chi_sq / dof > max_chisq && ntries < max_tries);

    if(chi_sq / dof < max_chisq) {
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
  result = add_bernsteins(data, obs, pdfList, useSideBands, index, set, verbose);
  if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  // add_chebychevs(data, obs, pdfList, useSideBands, index, set, verbose);
  result = add_exponentials(data, obs, pdfList, useSideBands, set, verbose);
  if(result.second < chi_min) {chi_min = result.second; index = result.first;}
  RooMultiPdf* pdfs =  new RooMultiPdf("bkg_multi", "Background function PDF choice", categories, pdfList);
  return pdfs;
}

#endif
