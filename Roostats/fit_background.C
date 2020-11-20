//Script to fit the di-lepton mass histogram for same flavor selections
#include "DataInfo.C"

bool doConstraints_ = false; //adding in systematics
bool includeSignalInFit_ = false; //fit background specturm with signal shape in PDF
bool useSameFlavorCount_ = true; //use N(sig) ~ br_emu*eff*sqrt(N_ee*N_mumu)/br_ll

Int_t do_fit(TTree* tree, int set, vector<int> years, int seed) {
  //set the random seed for the fitting + generation
  RooRandom::randomGenerator()->SetSeed(seed);
  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  RooRealVar lepm("lepm", "di-lepton invariant mass", 110, 75., 110., "GeV/c^{2}");
  RooRealVar weight("fulleventweightlum", "Full event weight*luminosity", 1., -100., 100.);
  RooDataSet dataset("dataset", "dataset", RooArgList(lepm,weight),
		     RooFit::Import(*tree), RooFit::WeightVar(weight));

  //get the nominal data numbers
  DataInfo muonInfo(set, "muon");
  DataInfo electronInfo(set, "electron");
  muonInfo.ReadData();
  electronInfo.ReadData();
  
  double n_muon = 0.;
  double n_electron = 0.;
  for(int year : years) {
    n_muon += muonInfo.datamap_[year];
    n_electron += electronInfo.datamap_[year];
  }

  
  std::cout << "Number of events to fit: " << dataset.numEntries() << std::endl
	    << "Number of muon events is " << n_muon << std::endl
	    << "Number of electron events is " << n_electron << std::endl;

  //Get the signal PDF
  TFile* fWSSignal = TFile::Open(Form("workspaces/morphed_signal_%s_%i.root", year_string.Data(), set), "READ");
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
  RooRealVar br_emu("br_emu", "br_emu", 0., -1e-4, 1e-4);
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

  //Luminosity based parameters
  DataInfo signalInfo(set, "zemu");
  signalInfo.ReadData();
  CrossSections xs;
  double lum = 0.;
  double eff_signal = 0.;
  for(int year : years) {
    double lum_year = xs.GetLuminosity(year);
    lum += lum_year;
    //get the efficiency of signal for the year, weight by luminosity
    int gen_year = xs.GetGenNumber("ZEMu", year);
    double rec_year = signalInfo.datamap_[year];
    eff_signal += lum_year * (rec_year / gen_year);
  }
  eff_signal /= lum; //divide by total luminosity
  if(!useSameFlavorCount_)
    eff_nominal.setVal(eff_signal);
  
  double zxs = xs.GetCrossSection("Z");
  RooRealVar lum_var("lum_var", "lum_var", lum);
  RooRealVar zxs_var("zxs_var", "zxs_var", zxs);
  
  RooFormulaVar n_sig("n_sig", ((useSameFlavorCount_) ? "@0*@4*sqrt((@1*@2)/(@3*@3))" : "@0*@1*@2*@3")
		      , ((useSameFlavorCount_) ? RooArgList(br_emu, n_electron_var, n_muon_var,br_ll,eff) :
			 RooArgList(br_emu, lum_var, zxs_var, eff))
		      );
  RooRealVar n_bkg("n_bkg", "n_bkg", 500., 0., 1.e6);

  if(!includeSignalInFit_) {br_emu.setVal(0.); br_emu.setConstant(1);}
  
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
    
  if(!includeSignalInFit_) {br_emu.setVal(0.); br_emu.setConstant(0);}
  auto xframe = lepm.frame();
  dataset.plotOn(xframe);
  if(doConstraints_)
    totPDF_constr.plotOn(xframe);
  else
    totPDF.plotOn(xframe);
  bkgPDF_exp.plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

  auto c1 = new TCanvas();
  xframe->Draw();
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  if(doConstraints_)
    c1->SaveAs(Form("plots/latest_production/%s/fit_lepm_background_constr_%i.pdf", year_string.Data(), set));
  else
    c1->SaveAs(Form("plots/latest_production/%s/fit_lepm_background_%i.pdf", year_string.Data(), set));

  //save the workspace
  TFile* fOut = (doConstraints_) ? new TFile(Form("workspaces/fit_lepm_background_constr_%s_%i.root", year_string.Data(), set), "RECREATE") :
    new TFile(Form("workspaces/fit_lepm_background_%s_%i.root", year_string.Data(), set), "RECREATE");
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
  ws.import(bkgPDF);
  ws.import(*bkg_data);
  ws.Print();
  ws.Write();
  fOut->Close();
  delete fOut;
  return 0;
}

Int_t fit_background(int set = 8, vector<int> years = {2016}, int seed = 90) {
  int status(0);
  TList* list = new TList;
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  TString bkg_name = "background_trees/background_ztautau_bkg_nano_emu_";
  bkg_name += year_string;
  bkg_name += "_";
  bkg_name += set+ZTauTauHistMaker::kEMu;
  bkg_name += ".tree";
  TFile* f_bkg = TFile::Open(bkg_name.Data(), "READ");
  if(!f_bkg) return 1;
  TTree* t_bkg = (TTree*) f_bkg->Get(Form("background_tree_%s", year_string.Data()));
  if(!t_bkg) return 2;
  std::cout << "---Performing background fit!\n";
  status = do_fit(t_bkg, set, years, seed);
  if(status) std::cout << "Fit returned status " << status << std::endl;
  return status;
}
