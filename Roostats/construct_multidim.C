#ifndef __CONSTRUCT_MULTIDIM__
#define __CONSTRUCT_MULTIDIM__
//Construct multi-dimensional PDF with discrete index

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
    coeffs.push_back(new RooRealVar(Form("exp_%i_order_%i_n_%i", set, order, i), Form("exp_%i_order_%i_%i norm" , set, order, i), 1./pow(10.,i), -1.e1, 1.e1));
    coefficients.add(*coeffs[i]);
  }
  if(order == 0) return ((RooAbsPdf*) pdfs.at(0));
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

//Create a Bernstein polynomial PDF
RooBernstein* create_bernstein(RooRealVar& obs, int order, int set) {
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_%i_order_%i_%i", set, order, i), Form("bst_%i_order_%i_%i", set, order, i), 1./pow(10.,i), -5., 5.));
    list.add(*vars[i]);
  }
  return new RooBernstein(Form("bst_%i_order_%i", set, order), Form("Bernstein PDF, order %i", order), obs, list);
}

//Fit exponentials and add passing ones
void add_exponentials(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int set, int verbose) {
  const int max_order = 4;
  const double max_chisq = 5.; //per DOF
  for(int order = 0; order <= max_order; ++order) {
    RooAbsPdf* pdf = create_exponential(obs, order, set);
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    auto xframe = obs.frame();
    if(useSideBands) {
      data.plotOn(xframe, RooFit::Range("LowSideband,HighSideband"));
      pdf->plotOn(xframe, RooFit::Range("LowSideband,HighSideband"));
    } else {
      data.plotOn(xframe);
      pdf->plotOn(xframe);
    }
    const int dof = (data.numEntries() - 2*order - 2);  //DOF = number of variables + normalization
    const double chi_sq = xframe->chiSquare() * data.numEntries(); //returns chi squared / entries
    delete xframe;
    if(chi_sq / dof < max_chisq) {
      list.add(*pdf);
    } else {
      delete pdf;
    }
    if(verbose > 1) cout << "### Exponential order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl;
  }
}

//Fit Chebychev polynomials and add passing ones
void add_chebychevs(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 6;
  const double max_chisq = 5.; //per DOF
  //for finding the best fitting function
  double chi_min = 1.e10;
  const double chi_cutoff = 3.85;
  int num_added = -1;
  int best_order;
  for(int order = 1; order <= max_order; ++order) {
    RooChebychev* pdf = create_chebychev(obs, order, set);
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    auto xframe = obs.frame();
    data.plotOn(xframe);
    pdf->plotOn(xframe);

    const int dof = (data.numEntries() - order - 2);  //DOF = number of variables + normalization
    const double chi_sq = xframe->chiSquare() * data.numEntries(); //returns chi squared / entries
    delete xframe;
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
    if(verbose > 1) cout << "### Chebychev order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl;
  }
  if(verbose > 0) cout << "### Best fit Chebychev order is " << best_order << " with chisq = " << chi_min << endl;
}

//Fit Bernstein polynomials and add passing ones
void add_bernsteins(RooDataHist& data, RooRealVar& obs, RooArgList& list, bool useSideBands, int& index, int set, int verbose) {
  const int max_order = 6;
  const double max_chisq = 5.; //per DOF
  //for finding the best fitting function
  double chi_min = 1.e10;
  const double chi_cutoff = 3.85;
  int num_added = -1;
  int best_order;
  for(int order = 1; order <= max_order; ++order) {
    RooBernstein* pdf = create_bernstein(obs, order, set);
    if(useSideBands)
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      pdf->fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    auto xframe = obs.frame();
    if(useSideBands) {
      data.plotOn(xframe, RooFit::Range("LowSideband,HighSideband"));
      pdf->plotOn(xframe, RooFit::Range("LowSideband,HighSideband"));
    } else {
      data.plotOn(xframe);
      pdf->plotOn(xframe);
    }
    const int dof = (data.numEntries() - order - 2);  //DOF = number of variables + normalization
    const double chi_sq = xframe->chiSquare() * data.numEntries(); //returns chi squared / entries
    delete xframe;
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
    if(verbose > 1) cout << "### Bernstein order " << order << " has chisq = " << chi_sq << " / " << dof << " = " << chi_sq/dof << endl;
  }
  if(verbose > 0) cout << "### Best fit Bernstein order is " << best_order << " with chisq = " << chi_min << endl;
}

RooMultiPdf* construct_multidim_pdf(RooDataHist& data, RooRealVar& obs, RooCategory& categories, bool useSideBands, int& index, int set, int verbose = 0) {
  RooArgList pdfList;
  // add_bernsteins(data, obs, pdfList, useSideBands, index, set, verbose);
  add_chebychevs(data, obs, pdfList, useSideBands, index, set, verbose);
  add_exponentials(data, obs, pdfList, useSideBands, set, verbose);
  for(int ipdf = 0; ipdf < pdfList.getSize(); ++ipdf) {
    RooAbsPdf* pdf = (RooAbsPdf*) pdfList.at(ipdf);
    categories.defineType(pdf->GetName(), ipdf);
  }
  RooMultiPdf* pdf =  new RooMultiPdf("bkg_multi", "Background function PDF choice", categories, pdfList);
  return pdf;
}

#endif
