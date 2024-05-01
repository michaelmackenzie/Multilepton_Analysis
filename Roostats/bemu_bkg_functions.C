#ifndef __BEMU_BKG_FUNCTIONS__
#define __BEMU_BKG_FUNCTIONS__
// Define PDFs/functions to describe the B->e+mu background fits

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
    vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, i, tag.Data()), -0.1, -3., 3.));
    exps.push_back(new RooExponential(Form("exp_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), Form("exp_%i_pdf_order_%i_%i%s", set, order, i, tag.Data()), obs, *vars.back()));
    pdfs.add(*exps.back());
    if(i < order) {
      coeffs.push_back(new RooRealVar(Form("exp_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1.e3, 0., 1.e8));
      coefficients.add(*coeffs.back());
    }
  }
  if(order == 1) {
    pdfs.at(0)->SetTitle(Form("Exponential PDF, order %i", order));
    return ((RooAbsPdf*) pdfs.at(0));
  }
  return new RooAddPdf(Form("exp_%i_pdf_order_%i%s", set, order, tag.Data()), Form("Exponential PDF, order %i", order), pdfs, coefficients);
}

//Create an exponential PDF sum (recursive RooAddPdf form)
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
    vars.push_back(new RooRealVar(Form("exp_%i_order_%i_c_%i%s", set, order, i, tag.Data()), Form("exp_%i_order_%i_%i power%s", set, order, i, tag.Data()), -0.1, -3., 3.));
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
    if(i < order) {
      coeffs.push_back(new RooRealVar(Form("pwr_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("pwr_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 0.1, 0., 1.));
      coefficients.add(*coeffs.back());
    }
  }
  if(order == 1) {
    pdfs.at(0)->SetTitle(Form("Power law PDF, order %i", order));
    return ((RooAbsPdf*) pdfs.at(0));
  }
  return new RooAddPdf(Form("pwr_%i_pdf_order_%i%s", set, order, tag.Data()), Form("Power law PDF, order %i", order), pdfs, coefficients, true);
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

//Create a 1/(Polynomial) PDF
RooGenericPdf* create_inv_polynomial(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order <= 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  RooArgList var_list;
  var_list.add(obs);
  TString formula = "";
  //define the formula for fixed orders
  if     (order == 1) formula = "1/(@1*@0 + @2)";
  else if(order == 2) formula = "1/(@1*@0*@0 + @2*@0 + @3)";
  else if(order == 3) formula = "1/(@1*@0*@0*@0 + @2*@0*@0 + @3*@0 + @4)";
  else return nullptr;
  for(int i = 0; i < order+1; ++i) { //N(params) = order + 1
    vars.push_back(new RooRealVar(Form("inv_%i_order_%i_n_%i%s", set, order, i, tag.Data()), Form("inv_%i_order_%i_%i%s norm" , set, order, i, tag.Data()), 1., -1.e3, 1.e3));
    var_list.add(*vars.back());
  }
  RooGenericPdf* pdf = new RooGenericPdf(Form("inv_%i_pdf_order_%i%s", set, order, tag.Data()), formula.Data(), var_list);
  pdf->SetTitle(Form("Inverse polynomial series PDF, order %i", order));
  return pdf;
}

//Create a Gaussian + polynomial(order = order) PDF
RooGenericPdf* create_gaus_poly_pdf(RooRealVar& obs, int order, int set, TString tag = "") {
  if(order < 0) {
    cout << __func__ << ": Can't create order " << order << " PDF!\n";
    return nullptr;
  }
  vector<RooRealVar*> vars;
  RooArgList var_list;
  var_list.add(obs);
  TString formula = "";
  //define the formula for fixed orders
  if     (order == -1) formula = "TMath::Gaus(@0, @1, @2)";
  else if(order ==  0) formula = "TMath::Gaus(@0, @1, @2) + @3";
  else if(order ==  1) formula = "TMath::Gaus(@0, @1, @2) + @3 + @4*@0";
  else if(order ==  2) formula = "TMath::Gaus(@0, @1, @2) + @3 + @4*@0 + @5*@0*@0";
  else if(order ==  3) formula = "TMath::Gaus(@0, @1, @2) + @3 + @4*@0 + @5*@0*@0 + @6*@0*@0*@0";
  else return nullptr;
  //add the Gaussian parameters
  vars.push_back(new RooRealVar(Form("gaus_poly_%i_order_%i_g_0%s", set, order, tag.Data()),
                                Form("gaus_poly_%i_order_%i_g_0%s", set, order, tag.Data()),
                                60., 50., 70.)); //mean
  vars.push_back(new RooRealVar(Form("gaus_poly_%i_order_%i_g_1%s", set, order, tag.Data()),
                                Form("gaus_poly_%i_order_%i_g_1%s", set, order, tag.Data()),
                                11., 5., 20.)); //sigma

  //add the polynomial parameters
  for(int i = 0; i < order+1; ++i) { //N(params) = order + 1 = a +bx + ...
    vars.push_back(new RooRealVar(Form("gaus_poly_%i_order_%i_p_%i%s", set, order, i, tag.Data()),
                                  Form("gaus_poly_%i_order_%i_p_%i%s", set, order, i, tag.Data()),
                                  (i == 0) ? 0.5 : 0., (order == 0) ? 0. : (i == 0) ? -3. : -0.1, (i == 0) ? 3. : 0.1));
  }
  for(auto var : vars) var_list.add(*var);
  RooGenericPdf* pdf = new RooGenericPdf(Form("gaus_poly_%i_pdf_order_%i%s", set, order, tag.Data()), formula.Data(), var_list);
  pdf->SetTitle(Form("Gaussian polynomial series PDF, order %i", order));
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

//Create a Z->mumu PDF
RooAbsPdf* create_zmumu(RooRealVar& obs, int set, bool fix = true, TString tag = "") {
  RooRealVar* zmumu_mean   = new RooRealVar(Form("zmumu_mean_%i%s"  , set, tag.Data()), "mean"  , 84.4, 70., 90.);
  RooRealVar* zmumu_sigma  = new RooRealVar(Form("zmumu_sigma_%i%s" , set, tag.Data()), "sigma" , 4.48,  3., 10.);
  RooRealVar* zmumu_alpha1 = new RooRealVar(Form("zmumu_alpha1_%i%s", set, tag.Data()), "alpha1", 1.22, 0.8,  5.);
  RooRealVar* zmumu_alpha2 = new RooRealVar(Form("zmumu_alpha2_%i%s", set, tag.Data()), "alpha2", 1.78, 0.5,  5.);
  RooRealVar* zmumu_enne1  = new RooRealVar(Form("zmumu_enne1_%i%s" , set, tag.Data()), "enne1" , 0.36, 0.5, 10.);
  RooRealVar* zmumu_enne2  = new RooRealVar(Form("zmumu_enne2_%i%s" , set, tag.Data()), "enne2" , 9.14, 1.0, 10.);
  RooAbsPdf* zmumu  = new RooDoubleCrystalBall(Form("zmumu_%i%s"    , set, tag.Data()), "Z->#mu#mu PDF", obs,
                                               *zmumu_mean, *zmumu_sigma, *zmumu_alpha1, *zmumu_enne1, *zmumu_alpha2, *zmumu_enne2);

  const bool use_data_fit(true); //shape fits from same sign data vs. opposite sign MC
  if(set == 13) { //high score region
    zmumu_mean  ->setVal((use_data_fit) ? 84.5 : 84.7);
    zmumu_sigma ->setVal((use_data_fit) ? 4.86 : 4.45);
    zmumu_alpha1->setVal((use_data_fit) ? 0.80 : 1.36);
    zmumu_alpha2->setVal((use_data_fit) ? 1.70 : 1.34);
    zmumu_enne1 ->setVal((use_data_fit) ? 10.0 : 0.50);
    zmumu_enne2 ->setVal((use_data_fit) ? 1.00 : 1.00);
  } else if(set == 12) { //medium score region
    zmumu_mean  ->setVal((use_data_fit) ? 81.6 : 82.9);
    zmumu_sigma ->setVal((use_data_fit) ? 5.71 : 4.79);
    zmumu_alpha1->setVal((use_data_fit) ? 0.80 : 0.80);
    zmumu_alpha2->setVal((use_data_fit) ? 1.72 : 2.99);
    zmumu_enne1 ->setVal((use_data_fit) ? 10.0 : 0.57);
    zmumu_enne2 ->setVal((use_data_fit) ? 1.00 : 9.12);
  } else { //low score region (or undefined region)
    zmumu_mean  ->setVal(80.2); //data fits are unstable, so use MC fits
    zmumu_sigma ->setVal(6.37);
    zmumu_alpha1->setVal(1.96);
    zmumu_alpha2->setVal(1.65);
    zmumu_enne1 ->setVal(9.38);
    zmumu_enne2 ->setVal(1.65);
  }

  //freeze the parameters if not being fit
  if(fix) {
    zmumu_mean  ->setConstant(true);
    zmumu_sigma ->setConstant(true);
    zmumu_alpha1->setConstant(true);
    zmumu_alpha2->setConstant(true);
    zmumu_enne1 ->setConstant(true);
    zmumu_enne2 ->setConstant(true);
  }
  return zmumu;
}

#endif
