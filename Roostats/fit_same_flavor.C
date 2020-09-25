//Script to fit the di-lepton mass histogram for same flavor selections

Int_t do_fit(TH1F* hlep, int year, bool isMuon) {
  RooRealVar lepm("lepm", "di-lepton invariant mass", 110, 75., 110., "GeV/c^{2}");
  RooDataHist data("data", "data", RooArgList(lepm), hlep);
  std::cout << "Number of events to fit: " << data.numEntries() << std::endl;

  RooRealVar mean("mean", "mean", 91., 85., 95.);
  RooRealVar width("width", "width", 2.5, 0.1, 5.);
  RooRealVar sigma("sigma", "sigma", 2., 0.1, 5.);
  RooRealVar alpha("alpha", "alpha", 1., 0.1, 10.);
  RooRealVar enne("enne", "enne", 5., 0.1, 20.);

  RooRealVar mean2("mean2", "mean2", 91., 70., 95.);
  RooRealVar sigma2("sigma2", "sigma2", 5., 0.1, 10.);

  RooCBShape sigpdf1("sigpdf1", "sigpdf1", lepm, mean, sigma, alpha, enne);
  RooGaussian sigpdf2("sigpdf2", "sigpdf2", lepm, mean2, sigma2);

  RooRealVar fracsig("fracsig", "fracsig", 0.7, 0., 1.);
  RooAddPdf sigpdf("sigpdf", "sigpdf", sigpdf1, sigpdf2, fracsig);

  RooRealVar a_bkg("a_bkg", "a_bkg", 0.1, -10., 10.);
  RooRealVar b_bkg("b_bkg", "b_bkg", 0.1, -10., 10.);
  RooRealVar c_bkg("c_bkg", "c_bkg", 0. , -10., 10.);

  RooBernstein bkgpdf("bkgpdf", "Background PDF", lepm, RooArgList(a_bkg, b_bkg, c_bkg));

  RooRealVar N_sig("N_sig", "N_sig", 2e7, 1e7, 3e7);
  RooRealVar N_bkg("N_bkg", "N_bkg", 2e6, 1e5, 1e7);

  RooAddPdf totpdf("totpdf", "Total PDF", RooArgList(sigpdf,bkgpdf), RooArgList(N_sig,N_bkg));

  totpdf.fitTo(data);

  auto xframe = lepm.frame();
  data.plotOn(xframe);
  totpdf.plotOn(xframe);
  totpdf.plotOn(xframe, RooFit::Components("bkgpdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));

  auto c1 = new TCanvas();
  xframe->Draw();
  TString name = (isMuon) ? "Muon" : "Electron";
  c1->SaveAs(Form("plots/latest_production/%i/fit_lepm_SameSign_%s.pdf", year, name.Data()));
  TFile* fOut = new TFile(Form("workspaces/fit_lepm_SameSign_%s_%i.root", name.Data(), year), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(sigpdf);
  ws.Write();
  fOut->Close();
  delete fOut;
  return 0;
}


Int_t fit_same_flavor(int set = 8, int year = 2016, bool doMuon = true) {
  int status(0);
  if(doMuon) {
    TString muon_name = "mumu_lepm_";
    muon_name += set+ZTauTauHistMaker::kMuMu;
    muon_name += "_";
    muon_name += year;
    muon_name += ".hist";
    TFile* f_muon = TFile::Open(muon_name.Data(), "READ");
    if(!f_muon) return 1;
    TH1F* h_muon = (TH1F*) f_muon->Get("lepm");
    if(!h_muon) return 2;
    std::cout << "---Performing mumu fit!\n";
    status = do_fit(h_muon, year, true);
  } else {
    TString electron_name = "ee_lepm_";
    electron_name += set+ZTauTauHistMaker::kEE;
    electron_name += "_";
    electron_name += year;
    electron_name += ".hist";
    TFile* f_electron = TFile::Open(electron_name.Data(), "READ");
    if(!f_electron) return 3;
    TH1F* h_electron = (TH1F*) f_electron->Get("lepm");
    if(!h_electron) return 4;

    std::cout << "---Performing ee fit!\n";
    status = do_fit(h_electron, year, false);
  }
  return status;
}
