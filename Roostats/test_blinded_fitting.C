bool use_mc_ = true;
bool use_range_ = true;
int use_high_side_ = 0;
bool do_no_fit_ = false;
bool use_bernstein_ = false;

RooAbsPdf* build_generic_bernstein(int order, RooRealVar& obs) {

  vector<RooRealVar*> variables;
  RooArgSet var_set;
  var_set.add(obs);
  TString formula;
  double xmin = obs.getMin();
  double xmax = obs.getMax();
  TString var_form = Form("(%s - %.3f)/%.3f", obs.GetName(), xmin, (xmax - xmin));
  double vals[] = {1.404, 2.443e-1, 5.549e-1, 3.675e-1, 0., 0., 0., 0., 0., 0., 0.};

  for(int ivar = 0; ivar <= order; ++ivar) {
    RooRealVar* v = new RooRealVar(Form("bst_order_%i_c_%i", order, ivar), Form("bst_order_%i_c_%i", order, ivar), vals[ivar], -5., 5.);
    formula += Form("%.0f*(%s)^%i*(1-%s)^%i*%s", TMath::Binomial(order, ivar), var_form.Data(), ivar, var_form.Data(), order-ivar, v->GetName());
    if(ivar < order) formula += " + ";
    var_set.add(*v);
  }
  cout << "Formula: " << formula.Data() << endl;
  RooAbsPdf* pdf = new RooGenericPdf(Form("bst_order_%i", order), Form("Bernstein order %i", order), formula.Data(), var_set);
  return pdf;
}


void test_blinded_fitting(int set = 8, TString selection = "zemu",
                          vector<int> years = {2016, 2017, 2018},
                          int seed = 90) {
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  bool doHiggs = selection.Contains("h");
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  double xlow  = (doHiggs) ? 100. :  70.;
  double xhigh = (doHiggs) ? 150. : 110.;
  double bmass = (doHiggs) ? 125. :  91.; //just rough mass for blinding region definition
  RooRealVar lepm("lepm", "di-lepton invariant mass", (doHiggs) ? 125. : 91., xlow, xhigh, "GeV/c^{2}");

  //initialize ranges
  lepm.setRange("FullRange", xlow, xhigh);
  lepm.setRange("LowSideband", xlow, bmass - 5.);
  lepm.setRange("HighSideband", bmass + 5., xhigh);
  TFile* f = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "READ");
  if(!f) return;
  TH1D* hbkg  = (TH1D*) f->Get("hbackground");
  TH1D* hdata = (TH1D*) f->Get("hdata");
  if(!hbkg || !hdata) {
    cout << "Histograms not found!\n";
    return;
  }
  int bin_lo = hdata->FindBin(bmass - 5.);
  int bin_hi = hdata->FindBin(bmass + 5.);
  for(int bin = bin_lo; bin <= bin_hi; ++bin) hdata->SetBinContent(bin, 0.);
  lepm.setBins((lepm.getMax() - lepm.getMin())/hbkg->GetBinWidth(1)); //match binning of histograms

  RooDataHist bkgData (Form("bkgData_%i", set), "Background Data", RooArgList(lepm), hbkg);
  RooDataHist dataData(Form("dataData_%i", set),  "Data Data"      , RooArgList(lepm), hdata);

  RooRealVar* a_bst4 = new RooRealVar(Form("a_bst4_%i", set), "a_bst4", 1.404   , -5., 5.);
  RooRealVar* b_bst4 = new RooRealVar(Form("b_bst4_%i", set), "b_bst4", 2.443e-1, -5., 5.);
  RooRealVar* c_bst4 = new RooRealVar(Form("c_bst4_%i", set), "c_bst4", 5.549e-1, -5., 5.);
  RooRealVar* d_bst4 = new RooRealVar(Form("d_bst4_%i", set), "d_bst4", 3.675e-1, -5., 5.);
  // RooBernstein* bkgPDF = new RooBernstein(Form("bkgPDF_bst4_%i", set), "Background PDF", lepm, RooArgList(*a_bst4, *b_bst4, *c_bst4, *d_bst4));
  RooAbsPdf* bkgPDF;
  if(use_bernstein_)
    bkgPDF = new RooBernstein(Form("bkgPDF_bst4_%i", set), "Background PDF", lepm, RooArgList(*a_bst4, *b_bst4, *c_bst4, *d_bst4));
  else
    bkgPDF = build_generic_bernstein(3, lepm);

  const char* range = (use_high_side_ > 0) ? "HighSideband" : (use_high_side_ < 0) ? "LowSideband" : "LowSideband,HighSideband";
  if(!do_no_fit_) {
    if(use_range_)
      bkgPDF->fitTo((use_mc_) ? bkgData : dataData, RooFit::Range(range));
    else
      bkgPDF->fitTo((use_mc_) ? bkgData : dataData);
  }
  RooRealVar exp_tau("exp_tau", "exp_tau", -1., -10., -0.001);
  RooExponential exp("exp", "exp", lepm, exp_tau);
  if(use_range_)
    exp.fitTo((use_mc_) ? bkgData : dataData, RooFit::Range(range));
  else
    exp.fitTo((use_mc_) ? bkgData : dataData);

  auto xframe = lepm.frame();
  if(use_mc_)  bkgData.plotOn(xframe);
  else        dataData.plotOn(xframe);
  const char* plot_range = (false) ? "Full" : range;
  bkgPDF->plotOn(xframe, RooFit::Range(plot_range), RooFit::NormRange(range));
  double chi_sq = xframe->chiSquare()*bkgData.numEntries();
  cout << "Bkg PDF chi-squared: " << chi_sq << " / " << bkgData.numEntries() << " = " << chi_sq / bkgData.numEntries() << endl;
  exp.plotOn(xframe, RooFit::Range(plot_range), RooFit::NormRange(range), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  chi_sq = xframe->chiSquare()*bkgData.numEntries();
  cout << "Exp PDF chi-squared: " << chi_sq << " / " << bkgData.numEntries() << " = " << chi_sq / bkgData.numEntries() << endl;
  xframe->Draw();
  bkgPDF->Print();
}
