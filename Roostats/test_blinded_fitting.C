bool use_mc_ = true;
bool use_range_ = true;

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
  RooBernstein* bkgPDF = new RooBernstein(Form("bkgPDF_bst4_%i", set), "Background PDF", lepm, RooArgList(*a_bst4, *b_bst4, *c_bst4, *d_bst4));

  if(use_range_)
    bkgPDF->fitTo((use_mc_) ? bkgData : dataData, RooFit::Range("LowSideband,HighSideband"));
  else
    bkgPDF->fitTo((use_mc_) ? bkgData : dataData);
  RooRealVar exp_tau("exp_tau", "exp_tau", -1., -10., -0.001);
  RooExponential exp("exp", "exp", lepm, exp_tau);
  if(use_range_)
    exp.fitTo((use_mc_) ? bkgData : dataData, RooFit::Range("LowSideband,HighSideband"));
  else
    exp.fitTo((use_mc_) ? bkgData : dataData);

  auto xframe = lepm.frame();
  if(use_mc_)  bkgData.plotOn(xframe);
  else        dataData.plotOn(xframe);
  bkgPDF->plotOn(xframe, RooFit::Range("Full"), RooFit::NormRange("LowSideband,HighSideband"));
  exp.plotOn(xframe, RooFit::Range("Full"), RooFit::NormRange("LowSideband,HighSideband"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  xframe->Draw();
}
