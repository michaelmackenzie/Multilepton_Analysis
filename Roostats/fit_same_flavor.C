//Script to fit the di-lepton mass histogram for same flavor selections

Int_t do_fit(TH1F* hvar, int year, bool isMuon, TString hist, int set) {
  TString description, unit;
  double min_val, max_val, val;
  if(hist == "lepm") {
    description = "di-lepton invariant mass";
    min_val = 75.; max_val = 110.; val = 110.;
    unit = "GeV/c^{2}";
  } else {
    std::cout << "Unknown variable " << hist.Data() <<"!\n";
    return 1;
  }
  
  RooRealVar var(hist.Data(), description.Data(), val, min_val, max_val, unit.Data());
  RooDataHist data("data", "data", RooArgList(var), hvar);
  std::cout << "Number of events to fit: " << data.numEntries() << std::endl;

  //FIXME: change fit based on variable input
  RooRealVar mean("mean", "mean", 91., 85., 95.);
  RooRealVar width("width", "width", 2.5, 0.1, 5.);
  RooRealVar sigma("sigma", "sigma", 2., 0.1, 5.);
  RooRealVar alpha("alpha", "alpha", 1., 0.1, 10.);
  RooRealVar enne("enne", "enne", 5., 0.1, 20.);

  RooRealVar mean2("mean2", "mean2", 91., 70., 95.);
  RooRealVar sigma2("sigma2", "sigma2", 5., 0.1, 10.);

  RooCBShape sigpdf1("sigpdf1", "sigpdf1", var, mean, sigma, alpha, enne);
  RooGaussian sigpdf2("sigpdf2", "sigpdf2", var, mean2, sigma2);

  RooRealVar fracsig("fracsig", "fracsig", 0.7, 0., 1.);
  RooAddPdf sigpdf("sigpdf", "sigpdf", sigpdf1, sigpdf2, fracsig);

  RooRealVar a_bkg("a_bkg", "a_bkg", 0.1, -10., 10.);
  RooRealVar b_bkg("b_bkg", "b_bkg", 0.1, -10., 10.);
  RooRealVar c_bkg("c_bkg", "c_bkg", 0. , -10., 10.);

  RooBernstein bkgpdf("bkgpdf", "Background PDF", var, RooArgList(a_bkg, b_bkg, c_bkg));

  RooRealVar N_sig("N_sig", "N_sig", 2e7, 1e7, 3e7);
  RooRealVar N_bkg("N_bkg", "N_bkg", 2e6, 1e5, 1e7);

  RooAddPdf totpdf("totpdf", "Total PDF", RooArgList(sigpdf,bkgpdf), RooArgList(N_sig,N_bkg));

  totpdf.fitTo(data);

  auto xframe = var.frame();
  data.plotOn(xframe);
  totpdf.plotOn(xframe);
  totpdf.plotOn(xframe, RooFit::Components("bkgpdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));

  auto c1 = new TCanvas();
  xframe->Draw();
  TString name = (isMuon) ? "Muon" : "Electron";
  c1->SaveAs(Form("plots/latest_production/%i/fit_%s_SameSign_%s_%i.pdf", year, hist.Data(), name.Data(), set));
  TFile* fOut = new TFile(Form("workspaces/fit_%s_SameSign_%s_%i_%i.root", hist.Data(), name.Data(), year, set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(sigpdf);
  ws.Write();
  fOut->Close();
  delete fOut;
  return 0;
}


Int_t fit_same_flavor(int set = 8, int year = 2016, bool doMuon = true, TString hist = "lepm") {
  int status(0);
  TString name = (doMuon) ? "histograms/mumu_" : "histograms/ee_";
  name +=  hist + "_";
  name += (doMuon) ? set+ZTauTauHistMaker::kMuMu : set+ZTauTauHistMaker::kEE;
  name += "_";
  name += year;
  name += ".hist";
  TFile* f = TFile::Open(name.Data(), "READ");
  if(!f) return 1;
  TH1F* h = (TH1F*) f->Get(hist.Data());
  if(!h) return 2;
  std::cout << "---Performing fit!\n";
  status = do_fit(h, year, doMuon, hist, set);
  return status;
}
