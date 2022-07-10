#ifndef __CONSTRUCT_MULTIDIM__
#define __CONSTRUCT_MULTIDIM__

//Construct multi-dimensional PDF with discrete index
bool useFrameChiSq_ = true;
bool use_generic_bernstein_ = false;
bool use_fast_bernstein_ = true;

//Get the chi-squared using a RooChi2Var
double get_subrange_chisquare(RooRealVar& obs, RooAbsPdf* pdf, RooDataHist& data, const char* range) {
  RooChi2Var chi("chi", "chi", *pdf, data, RooFit::Range(range));
  return chi.getVal();
}

//Evaluate the chi-squared
double get_chi_squared(RooRealVar& obs, RooAbsPdf* pdf, RooDataHist& data, bool useSideBands) {
  if(useFrameChiSq_) {
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
  cout << "#### Bernstein order " << order << " formula: " << formula.Data() << endl;
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
  case 8 : pdf = new RooBernsteinFast<8 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 9 : pdf = new RooBernsteinFast<9 >(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  case 10: pdf = new RooBernsteinFast<10>(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list); break;
  default: return nullptr;
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
  const double max_chisq = 5.; //per DOF
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
    if(verbose > 1) cout << "### Exponential order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl;
  }
  return std::pair<int, double>(min_index, min_chi);
}

//Fit Chebychev polynomials and add passing ones
void add_chebychevs(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 6;
  const double max_chisq = 5.; //per DOF
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
    if(verbose > 1) cout << "### Chebychev order " << order << " has chisq = "
                         << chi_sq << " / " << dof << " = " << chi_sq/dof
                         << endl;
  }
  if(verbose > 0) cout << "### Best fit Chebychev order is " << best_order << " with chisq = " << chi_min << endl;
}

//Fit Bernstein polynomials and add passing ones
std::pair<int, double> add_bernsteins(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 6;
  const double max_chisq = 5.; //per DOF
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
    if(verbose > 1) cout << "### Bernstein order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl;
  }
  if(verbose > 0) cout << "### Best fit Bernstein order is " << best_order << " with chisq = " << chi_min << endl;
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
