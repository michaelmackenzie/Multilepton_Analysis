//Script to fit the di-lepton mass histogram for same flavor selections
bool doConstraints_ = false;

Int_t add_lum(TTree* tree, double lum) {
  bool debug = false;
  unsigned nentries = tree->GetEntriesFast();
  std::cout << "Tree has " << nentries << " to process!\n";
  Float_t weight(1.), inweight(1.);
  tree->SetBranchAddress("fulleventweight", &inweight);
  TBranch* branch = tree->Branch("fulleventweightlum", &weight, 'F');
  double avg = 0.;
  for(unsigned entry = 0; entry < nentries; ++entry) {
    tree->GetEntry(entry);
    weight = lum*inweight;
    avg += weight/nentries;
    branch->Fill();
  }
  if(debug) {
    std::cout << "Finished processing! Avg was " << avg << std::endl;
    avg = 0.;
    for(unsigned entry = 0; entry < nentries; ++entry) {
      tree->GetEntry(entry);
      avg += weight/nentries;
    }
    std::cout << "Finished processing again! Avg was " << avg << std::endl;
  }
  return 0;
}

Int_t do_fit(TTree* tree, int set, int year) {
  RooRealVar lepm("lepm", "di-lepton invariant mass", 110, 75., 110., "GeV/c^{2}");
  RooRealVar weight("fulleventweightlum", "Full event weight*luminosity", 1., -100., 100.);
  RooDataSet dataset("dataset", "dataset", RooArgList(lepm,weight),
		     RooFit::Import(*tree), RooFit::WeightVar(weight));

  double n_muon = 29409600.; //FIXME: Should not hard code this
  double n_electron = 12157600.; //FIXME: Should not hard code this

  std::cout << "Number of events to fit: " << dataset.numEntries() << std::endl
	    << "Number of muon events is " << n_muon << std::endl
	    << "Number of electron events is " << n_electron << std::endl;

  //Get the signal PDF
  TFile* fWSSignal = TFile::Open(Form("workspaces/morphed_signal_%i_%i.root", year, set), "READ");
  if(!fWSSignal) return 1;
  RooWorkspace* ws_signal = (RooWorkspace*) fWSSignal->Get("ws");
  if(!ws_signal) return 2;
  auto sigPDF = ws_signal->pdf("morph_pdf_binned");

  //background PDF
  //FIXME: find way of selecting these initial values based on set
  RooRealVar a_bkg("a_bkg", "a_bkg", 3.28640 , -50., 50.);
  RooRealVar b_bkg("b_bkg", "b_bkg",-0.290163, -50., 50.);
  RooRealVar c_bkg("c_bkg", "c_bkg", 0.558070, -50., 50.);
  RooRealVar d_bkg("d_bkg", "d_bkg", 0.264009, -50., 50.);
  RooBernstein bkgPDF("bkgPDF", "Background PDF", lepm, RooArgList(a_bkg, b_bkg, c_bkg, d_bkg));

  //Total PDF
  RooRealVar br_emu("br_emu", "br_emu", 0., -0.00001, 0.1);
  RooRealVar br_ll("br_ll", "br_ll", 0.033632);

  //Add log-normal systematics
  //direct systematics on N_emu
  RooRealVar eff_nominal("eff_nominal", "eff_nominal", 1.);
  RooRealVar eff_kappa("eff_kappa", "eff_kappa", 1.03);
  RooRealVar beta_eff("beta_eff", "beta_eff", 0., -10., 10.);
  RooFormulaVar eff("eff", "@0 * pow(@1,@2)", RooArgList(eff_nominal, eff_kappa, beta_eff));
  RooRealVar global_eff("global_eff", "global_eff", 0., -5., 5.);
  RooRealVar one("one", "one", 1.);
  RooGaussian constrain_eff("constrain_eff", "constrain_eff", global_eff, beta_eff, one);
  if(!doConstraints_) beta_eff.setConstant(1);
  global_eff.setConstant(1);

  //N(ee/mumu) systematics
  RooRealVar n_muon_nominal("n_muon_nominal", "n_muon_nominal", n_muon);
  RooRealVar n_electron_nominal("n_electron_nominal", "n_electron_nominal", n_electron);
  RooRealVar n_muon_kappa("n_muon_kappa", "n_muon_kappa", 1.+1./sqrt(n_muon));
  RooRealVar n_electron_kappa("n_electron_kappa", "n_electron_kappa", 1.+1./sqrt(n_electron));
  RooRealVar beta_n_muon("beta_n_muon", "beta_n_muon", 0., -10., 10.);
  RooRealVar beta_n_electron("beta_n_electron", "beta_n_electron", 0., -10., 10.);
  RooFormulaVar n_muon_var("n_muon_var", "@0 * pow(@1,@2)", RooArgList(n_muon_nominal, n_muon_kappa, beta_n_muon));
  RooFormulaVar n_electron_var("n_electron_var", "@0 * pow(@1,@2)", RooArgList(n_electron_nominal, n_electron_kappa, beta_n_electron));
  RooRealVar global_n_muon("global_n_muon", "global_n_muon", 0., -5., 5.);
  RooRealVar global_n_electron("global_n_electron", "global_n_electron", 0., -5., 5.);
  RooGaussian constr_n_muon("constr_n_muon", "constr_n_muon", global_n_muon, beta_n_muon, one);
  RooGaussian constr_n_electron("constr_n_electron", "constr_n_electron", global_n_electron, beta_n_electron, one);
  if(!doConstraints_) {
    beta_n_muon.setConstant(1);
    beta_n_electron.setConstant(1);
    n_muon_kappa.setConstant(1);
    n_electron_kappa.setConstant(1);
  }
  global_n_muon.setConstant(1);
  global_n_electron.setConstant(1);

  RooFormulaVar n_sig("n_sig", "@0*@4*sqrt((@1*@2)/(@3*@3))", RooArgList(br_emu, n_electron_var, n_muon_var,br_ll,eff));
  RooRealVar n_bkg("n_bkg", "n_bkg", 500., 0., 1.e5);

  RooAddPdf totPDF("totPDF", "totPDF", RooArgList(*sigPDF, bkgPDF), RooArgList(n_sig, n_bkg));
  //PDF with constraints
  RooProdPdf totPDF_constr("totPDF_constr", "totPDF_constr", RooArgList(totPDF, constrain_eff, constr_n_muon, constr_n_electron));

  //alternate background function
  RooRealVar tau_bkg("tau_bkg", "tau_bkg", -0.0583634, -50., 0.);
  RooExponential bkgPDF_exp("bkgPDF_exp", "bkgPDF_exp", lepm, tau_bkg);
  RooAddPdf totPDF_alt("totPDF_alt", "totPDF_alt", RooArgList(*sigPDF, bkgPDF_exp), RooArgList(n_sig, n_bkg));
  //PDF with constraint
  RooProdPdf totPDF_constr_alt("totPDF_constr_alt", "totPDF_constr_alt", RooArgList(totPDF_alt, constrain_eff, constr_n_muon, constr_n_electron));
  if(doConstraints_)
    totPDF_constr_alt.fitTo(dataset);
  else
    totPDF_alt.fitTo(dataset);

  //fit, plot, etc.
  if(doConstraints_)
    totPDF_constr.fitTo(dataset, RooFit::Extended(1));
  else
    totPDF.fitTo(dataset, RooFit::Extended(1));
    
  auto xframe = lepm.frame();
  dataset.plotOn(xframe);
  if(doConstraints_)
    totPDF_constr.plotOn(xframe);
  else
    totPDF.plotOn(xframe);
  bkgPDF_exp.plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

  auto c1 = new TCanvas();
  xframe->Draw();
  if(doConstraints_)
    c1->SaveAs(Form("plots/latest_production/%i/fit_lepm_backgroud_constr_%i.pdf", year, set));
  else
    c1->SaveAs(Form("plots/latest_production/%i/fit_lepm_backgroud_%i.pdf", year, set));
  TFile* fOut = (doConstraints_) ? new TFile(Form("workspaces/fit_lepm_background_constr_%i_%i.root", year, set), "RECREATE") :
    new TFile(Form("workspaces/fit_lepm_background_%i_%i.root", year, set), "RECREATE");
  fOut->cd();
  auto bkg_data = bkgPDF.generate(RooArgSet(lepm), n_bkg.getVal());
  
  RooWorkspace ws("ws");
  if(doConstraints_) {
    ws.import(totPDF_constr);
    ws.import(totPDF_constr_alt, RooFit::RecycleConflictNodes());
  } else {
    ws.import(totPDF);
    ws.import(totPDF_alt, RooFit::RecycleConflictNodes());
  }
  ws.import(*bkg_data);
  ws.Print();
  ws.Write();
  fOut->Close();
  delete fOut;
  return 0;
}


Int_t fit_background(int set = 8, int year = 2016, bool addLum = true) {
  int status(0);
  TString bkg_name = "background_trees/background_ztautau_bkg_nano_emu_";
  bkg_name += set+ZTauTauHistMaker::kEMu;
  bkg_name += ".tree";
  TFile* f_bkg = TFile::Open(bkg_name.Data(), "READ");
  if(!f_bkg) return 1;
  TTree* t_bkg = (TTree*) f_bkg->Get("background_tree");
  if(!t_bkg) return 2;
  if(addLum) {
    double lum = 35.9e3;
    if(year == 2017) lum = 41.48e3;
    else if (year == 2018) lum = 59.74e3;
    std::cout << "---Adding luminosity to weights!\n";
    add_lum(t_bkg, lum);
  }
  std::cout << "---Performing background fit!\n";
  status = do_fit(t_bkg, set, year);
  if(status) std::cout << "Fit returned status " << status << std::endl;
  return status;
}
