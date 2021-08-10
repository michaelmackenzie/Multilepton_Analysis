//Script to fit the di-lepton mass histogram for the B->e+mu searches
// #include "../interface/SystematicHist_t.hh"

bool doConstraints_ = false; //adding in systematics
bool includeSignalInFit_ = false; //fit background specturm with signal shape in PDF
bool fit_toy_mc_ = true; //fit the background functions to the MC background dataset
bool blind_data_ = false; //use sidebands and blind the signal region
bool require_same_flavor_ = true; //ensure same flavor histograms are available for normalization
bool require_dy_hist_ = false; //ensure DY MC estimate is with the same flavor data
bool use_bernstein_ = false; //use bernstein or chebychev for default background pdf choice
bool do_fits_ = true; //actually perform the fits, set to false for testing purposes
int test_sys_ = -1; //systematic number for testing, -1 to not test

RooRealVar* br_sig_; //fields common to systematics and nominal PDFs
TString year_string_;
RooUnblindOffset* br_sig_blind_;
RooFormulaVar* br_sig_eff_;
RooFormulaVar* n_sig_;
RooRealVar* lepm_;
RooRealVar* bxs_var_;
RooRealVar* br_zll_var_;
RooRealVar* lum_var_;
RooRealVar* eff_nominal_;
double      xs_sig_;
RooGaussian* br_sig_constr_;
RooCategory* categories_;
RooRealVar* mumu_eff_mc_;
RooRealVar* ee_eff_mc_;
TFile* fee_;
TFile* fmumu_;


Double_t chebychev_fit(RooDataHist& data, int order) {
  vector<RooRealVar*> vars;
  RooArgList list;
  map<int, vector<double>> params;
  params[0] = {1.};
  params[1] = {-1., 0.4};
  params[2] = {-1., 0.5, -0.1};
  params[3] = {-1., 0.5, -0.1, -0.05};
  params[4] = {-1., 0.5, -0.1, -0.05, 0.02};
  params[5] = {-1., 0.5, -0.1, -0.05, 0.02, 0.};
  params[6] = {-1., 0.5, -0.1, -0.05, 0.02, 0., 0.};
  if(order > 6) return 1e10;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("cheb_tmp_%i", i), Form("cheb_tmp_%i", i), params[order][i], -5., 5.));
    list.add(*vars[i]);
  }
  RooRealVar n_bkg("n_cheb_tmp", "n(bkg)", 1e5, 0., 1e6);
  RooChebychev func("cheb_tmp_pdf", "cheb_tmp_pdf", *lepm_, list);
  RooAddPdf tot("cheb_tmp_tot_pdf", "totPDF_cheb_tmp", RooArgList(func), RooArgList(n_bkg));
  if(blind_data_)
    tot.fitTo(data, RooFit::Range("LowSideband,HighSideband"));
  else
    tot.fitTo(data);
  auto xframe = lepm_->frame();
  data.plotOn(xframe);
  tot.plotOn(xframe, RooFit::Range("FullRange"));
  double chi_sq = xframe->chiSquare() * (data.numEntries() - order - 2); //DOF = number of variables + normalization
  delete xframe;
  for(auto var : vars) delete var;
  return chi_sq;
}

Double_t bernstein_fit(RooDataHist& data, int order) {
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_tmp_%i", i), Form("bst_tmp_%i", i), (i == 0) ? 1. : 0., -5., 5.));
    list.add(*vars[i]);
  }
  RooBernstein func("bst_tmp_pdf", "bst_tmp_pdf", *lepm_, list);
  if(blind_data_)
    func.fitTo(data, RooFit::Range("LowSideband,HighSideband"));
  else
    func.fitTo(data);
  auto xframe = lepm_->frame();
  data.plotOn(xframe);
  func.plotOn(xframe);
  double chi_sq = xframe->chiSquare() * (data.numEntries() - order - 2); //DOF = number of variables + normalization
  delete xframe;
  for(auto var : vars) delete var;
  return chi_sq;
}

Int_t perform_f_test(RooDataHist& data, TString function, int verbose = 0) {
  int order = -1;
  double delta_chi_end = 3.85; //Threshold to end F-test
  double last_chi_sq = 1000*data.sumEntries();
  double chi_sq = last_chi_sq;
  double chi_min = last_chi_sq;
  int order_min = order;
  do {
    ++order; //increment the order
    last_chi_sq = chi_sq; //save the previous result
    //find the next order goodness of fit
    if(function == "Bernstein")
      chi_sq = bernstein_fit(data, order); //find the next order goodness of fit
    else if(function == "Chebychev")
      chi_sq = chebychev_fit(data, order);
    else {
      cout << "!!! Warning! " << __func__ << ": Unknown fitting function " << function.Data() << endl;
      return -1;
    }
    //save the best order
    if(chi_sq < chi_min && last_chi_sq - chi_sq > delta_chi_end) {
      chi_min = chi_sq;
      order_min = order;
    }
    if(verbose > 0) cout << endl << "### " << __func__ << ": Order " << order << " has chi^2 = " << chi_sq << " (prev = " << last_chi_sq << ")\n\n";
  } while(order < 6 && (last_chi_sq - chi_sq > delta_chi_end  ||
                        chi_min / data.numEntries() > 2 || order < 2
                        )
          ); //break when improvement is small
  if(verbose > 0)
    cout << "### " << __func__ << ": Minimum order is " << order_min << " with chi^2 " << chi_min
         << " with " << data.numEntries() << " data entries" << endl << endl;
  return order_min;
}

Int_t fit_systematics(TFile* fOut, TString selection, vector<int> sets, map<int, int> orders, RooWorkspace& ws) {
  bool doHiggs = selection.Contains("h");
  map<int, TFile*> files;
  for(int set : sets) {
    TFile* f = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist", selection.Data(), set, year_string_.Data()), "READ");
    if(!f) return 1;
    files[set] = f;
  }

  TString set_str = "";
  for(int set : sets) {
    if(set_str == "") set_str += set;
    else {set_str += "_"; set_str += set;}
  }
  TString out_dir = Form("plots/latest_production/%s/fit_%s_lepm_binned_background%s_%s_sys/", year_string_.Data(), selection.Data(),
                         (doConstraints_) ? "_constr" : "", set_str.Data());
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", out_dir.Data(), out_dir.Data()));

  if(test_sys_ >= 0) cout << "--- Testing just systematic " << test_sys_ << endl;
  for(int isys = ((test_sys_ < 0) ? 0 : test_sys_); isys < ((test_sys_ < 0) ? 300 : test_sys_ + 1); ++isys) {
    if(isys >= 50 && isys < 100) continue; //j->tau sets
    vector<RooAddPdf*>      totPDFs              ;
    for(int set : sets) {
      TFile* fInput = files[set];
      if(!fInput) return 1;

      cout << "--- Getting info for systematic " << isys << " and set " << set << endl;

      ////////////////////////////////////////////////////////////
      // Create RooFit products for the shifted MC
      ////////////////////////////////////////////////////////////

      TH1D* hbkg = (TH1D*) fInput->Get(Form("hbkg_sys_%i", isys));
      TH1D* hsig = (TH1D*) fInput->Get(Form("%s_sys_%i", selection.Data(), isys));
      if(!hbkg || !hsig) {cout << "Skipping systematic " << isys << endl; continue;}

      RooDataHist* bkgData  = new RooDataHist(Form("bkgData_%i_sys_%i", set, isys) , "Background Data", RooArgList(*lepm_), hbkg );
      RooDataHist* sigData  = new RooDataHist(Form("sigData_%i_sys_%i", set, isys) , "Signal Data"    , RooArgList(*lepm_), hsig );

      ////////////////////////////////////////////////////////////
      // Get the same-flavor efficiency using shifted PDFs
      ////////////////////////////////////////////////////////////
      double n_ee_mc  , n_ee_dy  ;
      double n_mumu_mc, n_mumu_dy;
      if(!fmumu_) {
        n_mumu_mc = 1.e7;
        n_mumu_dy = 1.e7;
      } else {
        TH1D* hmumu_mc = (TH1D*) fmumu_->Get(Form("hbkg_sys_%i", isys));
        TH1D* hmumu_dy = (TH1D*) fmumu_->Get(Form("hDY_sys_%i" , isys));
        n_mumu_mc   = hmumu_mc->Integral(hmumu_mc->FindBin(70.), hmumu_mc->FindBin(110.));
        n_mumu_dy   = hmumu_dy->Integral(hmumu_dy->FindBin(70.), hmumu_dy->FindBin(110.));
        delete hmumu_mc;
        delete hmumu_dy;
      }
      if(!fee_) {
        n_ee_mc = 1.e7;
        n_ee_dy = 1.e7;
      } else {
        TH1D* hee_mc = (TH1D*) fee_->Get(Form("hbkg_sys_%i", isys));
        TH1D* hee_dy = (TH1D*) fee_->Get(Form("hDY_sys_%i" , isys));
        n_ee_mc   = hee_mc->Integral(hee_mc->FindBin(70.), hee_mc->FindBin(110.));
        n_ee_dy   = hee_dy->Integral(hee_dy->FindBin(70.), hee_dy->FindBin(110.));
        delete hee_mc;
        delete hee_dy;
      }

      //add the shifted N(Z) prediction using N(data) = shifted MC, N(MC) = non-shifted MC i.e. expected non-shifted but measured shifted amount
      RooRealVar* mumu_n_data = new RooRealVar   (Form("mumu_n_data_%i_sys_%i", set, isys), "Z->mumu count from Shifted MC", n_mumu_mc, 0., 1e9); mumu_n_data->setConstant(1);
      RooRealVar* ee_n_data   = new RooRealVar   (Form("ee_n_data_%i_sys_%i"  , set, isys), "Z->ee count from Shifted MC"  , n_ee_mc  , 0., 1e9); ee_n_data  ->setConstant(1);
      RooFormulaVar* n_z_mumu = new RooFormulaVar(Form("n_z_mumu_%i_sys_%i"   , set, isys), "@0 / @1 / @2", RooArgList(*mumu_n_data, *mumu_eff_mc_, *br_zll_var_));
      RooFormulaVar* n_z_ee   = new RooFormulaVar(Form("n_z_ee_%i_sys_%i"     , set, isys), "@0 / @1 / @2", RooArgList(*ee_n_data  , *ee_eff_mc_  , *br_zll_var_));
      ws.import(*n_z_mumu);
      ws.import(*n_z_ee);
      cout << endl << "--- Systematic " << isys << " set " << set << " has:"
           << "    N(Z) (mumu) = " << n_z_mumu->getVal() << " (" << n_mumu_mc << " / (" << mumu_eff_mc_->getVal() << " * " << br_zll_var_->getVal() << ")\n"
           << "    N(Z) ( ee ) = " << n_z_ee  ->getVal() << " (" << n_ee_mc   << " / (" << ee_eff_mc_  ->getVal() << " * " << br_zll_var_->getVal() << ")\n"
           << endl << endl;

      ////////////////////////////////////////////////////////////
      // Fit shifted signal PDF
      ////////////////////////////////////////////////////////////

      RooRealVar * mean    = new RooRealVar (Form("mean_%i_sys_%i"     , set, isys), "mean", (doHiggs) ? 125. : 91., (doHiggs) ? 120. : 85., (doHiggs) ? 130. : 95.);
      RooRealVar * sigma   = new RooRealVar (Form("sigma_%i_sys_%i"    , set, isys), "sigma", 2., 0.1, 5.);
      RooRealVar * alpha   = new RooRealVar (Form("alpha_%i_sys_%i"    , set, isys), "alpha", 1., 0.1, 10.);
      RooRealVar * enne    = new RooRealVar (Form("enne_%i_sys_%i"     , set, isys), "enne",  5., 0.1, 30.);
      RooRealVar * mean2   = new RooRealVar (Form("mean2_%i_sys_%i"    , set, isys), "mean2", lepm_->getVal(), lepm_->getMin(), lepm_->getMax());
      RooRealVar * sigma2  = new RooRealVar (Form("sigma2_%i_sys_%i"   , set, isys), "sigma2", 5., 0.1, 10.);
      RooCBShape * sigpdf1 = new RooCBShape (Form("sigpdf1_%i_sys_%i"  , set, isys), "sigpdf1", *lepm_, *mean, *sigma, *alpha, *enne);
      RooGaussian* sigpdf2 = new RooGaussian(Form("sigpdf2_%i_sys_%i"  , set, isys), "sigpdf2", *lepm_, *mean2, *sigma2);
      RooRealVar*  fracsig = new RooRealVar (Form("fracsig_%i_sys_%i"  , set, isys), "fracsig", 0.7, 0., 1.);
      RooAddPdf*  sigPDF   = new RooAddPdf(Form("sigPDF_%i_sys_%i"   , set, isys), "signal PDF", *sigpdf1, *sigpdf2, *fracsig);
      RooRealVar  N_sig    (Form("N_sig_%i_sys_%i"    , set, isys), "N_sig", 2e5, 1e2, 3e6);
      RooAddPdf   totsigpdf(Form("totsigpdf_%i_sys_%i", set, isys), "Signal PDF", RooArgList(*sigPDF), RooArgList(N_sig));

      cout << "--- Fitting Systematic signal " << isys << endl;
      totsigpdf.fitTo(*sigData, RooFit::SumW2Error(1), RooFit::PrintLevel(-1));
      fracsig->setConstant(1); mean->setConstant(1); sigma->setConstant(1);
      enne->setConstant(1); alpha->setConstant(1); mean2->setConstant(1); sigma2->setConstant(1);

      //Save signal fit results
      auto sigframe = lepm_->frame();
      sigData->plotOn(sigframe);
      sigPDF->plotOn(sigframe);
      sigPDF->plotOn(sigframe, RooFit::Components(Form("sigpdf1_%i_sys_%i", set, isys)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      sigPDF->plotOn(sigframe, RooFit::Components(Form("sigpdf2_%i_sys_%i", set, isys)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      TCanvas* c_sig = new TCanvas();
      sigframe->Draw();
      c_sig->SaveAs(Form("%ssignal_sys_%i_cat_%zu.png", out_dir.Data(), isys, totPDFs.size()));
      delete c_sig;
      delete sigframe;

      RooRealVar* eff_nominal = new RooRealVar("eff_nominal", "Nominal signal efficiency", hsig->Integral()/(lum_var_->getVal()*xs_sig_), 0., 1.);
      eff_nominal->setConstant(1);
      RooFormulaVar* n_sig = new RooFormulaVar(Form("n_sig_%i_sys_%i", set, isys),  "@0*@1*@2*@3", RooArgList(*br_sig_, *lum_var_, *bxs_var_, *eff_nominal));

      ////////////////////////////////////////////////////////////
      // Fit shifted background PDF
      ////////////////////////////////////////////////////////////

      RooRealVar* n_bkg = new RooRealVar(Form("n_bkg_%i_sys_%i", set, isys), "n_bkg", 500., 0., 1.e6);
      if(!includeSignalInFit_) {br_sig_->setVal(0.); br_sig_->setConstant(1);}

      RooAbsPdf* bkgPDF;
      vector<RooRealVar*> bkgVars;
      RooArgList bkgVarList;
      if(use_bernstein_) { //use Bernstein
        double vals[] = {1.4, 0.15, 0.75, 0.27, 0.37, 0.01, 0.01};
        for(int i = 0; i <= orders[set]; ++i) {
          bkgVars.push_back(new RooRealVar(Form("bkg_var_%i_sys_%i_%i", set, isys, i),
                                           Form("bkg_var_%i_sys_%i_%i", set, isys, i),
                                           vals[i], -5., 5.));
          bkgVarList.add(*bkgVars[i]);
        }
        bkgPDF = new RooBernstein(Form("bkgPDF_%i_sys_%i", set, isys), "Background PDF", *lepm_, bkgVarList);
      } else { //use Chebychev
        double vals[] = {-0.8, 0.3, -0.1, 0., 0., 0., 0.};
        for(int i = 0; i <= min(6, orders[set]); ++i) {
          bkgVars.push_back(new RooRealVar(Form("bkg_var_%i_sys_%i_%i", set, isys, i),
                                           Form("bkg_var_%i_sys_%i_%i", set, isys, i),
                                           vals[i], -5., 5.));
          bkgVarList.add(*bkgVars[i]);
        }
        bkgPDF = new RooChebychev(Form("bkgPDF_%i_sys_%i", set, isys), "Background PDF", *lepm_, bkgVarList);
      }

      totPDFs.push_back(new RooAddPdf(Form("totPDF_%i_sys_%i", set, isys), "totPDF", RooArgList(*sigPDF, *bkgPDF), RooArgList(*n_sig, *n_bkg)));

      cout << "--- Fitting Systematic background " << isys << " for set " << set << " with order " << orders[set] << endl;
      totPDFs[totPDFs.size() - 1]->fitTo(*bkgData, RooFit::SumW2Error(1), RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(0));
      //Save background fit results
      auto bkgframe = lepm_->frame();
      bkgData->plotOn(bkgframe);
      bkgPDF->plotOn(bkgframe);
      TCanvas* c_bkg = new TCanvas();
      bkgframe->Draw();
      c_bkg->SaveAs(Form("%sbackground_sys_%i_cat_%i.png", out_dir.Data(), isys, (int) totPDFs.size() - 1));
      delete c_bkg;
      delete bkgframe;

    }
    fOut->cd();
    RooSimultaneous* totPDF = new RooSimultaneous(Form("totPDF_sys_%i", isys), "The Total PDF", *categories_);
    for(unsigned i = 0; i < totPDFs.size(); ++i)
      totPDF->addPdf(*totPDFs[i], Form("%s_%i", selection.Data(), i));
    ws.import(*totPDF);
  }
  ws.Print();
  ws.Write();
  return 0;
}

Int_t fit_background_bemu_binned(vector<int> sets = {8}, TString selection = "zemu",
                                 vector<int> years = {2016, 2017, 2018},
                                 int seed = 90) {

  int status(0); //job status

  //set the random seed for the fitting + generation
  RooRandom::randomGenerator()->SetSeed(seed);

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  year_string_ = year_string;
  TString set_str = "";
  for(int set : sets) {
    if(set_str == "") set_str += set;
    else {set_str += "_"; set_str += set;}
  }

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));

  ////////////////////////////////////////////////////////////
  // Initialize global parameters
  ////////////////////////////////////////////////////////////

  bool doHiggs = selection.Contains("h");
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  double xlow  = (doHiggs) ? 110. :  70.;
  double xhigh = (doHiggs) ? 150. : 110.;
  double bmass = (doHiggs) ? 125. :  91.; //just rough mass for blinding region definition
  RooRealVar lepm("lepm", "di-lepton invariant mass", (doHiggs) ? 125. : 91., xlow, xhigh, "GeV/c^{2}");

  //initialize ranges
  lepm.setRange("FullRange", xlow, xhigh);
  lepm.setRange("LowSideband", xlow, bmass - 5.);
  lepm.setRange("HighSideband", bmass + 5., xhigh);

  //Luminosity based parameters
  CrossSections xs;
  double lum = 0.;
  for(int year : years) {
    double lum_year = xs.GetLuminosity(year);
    lum += lum_year;
  }

  //systematic uncertainty
  double sys_unc = (doHiggs) ? 0. : 0.6;

  //Get total boson cross section and set total luminosity
  double bxs = xs.GetCrossSection((doHiggs) ? "H" : "Z");
  double zxs = xs.GetCrossSection("Z"); //for normalizing to Z->mumu/ee
  double br_zll = xs.GetCrossSection("Br_Zll"); //for normalizing to Z->mumu/ee

  //Parameters common to all categories
  RooRealVar br_sig("br_sig", "br_sig", (doHiggs) ? 1.e-4 : 1.e-7, -0.01, 0.01); br_sig_ = &br_sig;
  RooUnblindOffset br_sig_blind("br_sig_blind", "br_sig_blind", "seedString", (doHiggs) ? 1.e-4 : 1.e-7, br_sig); br_sig_blind_ = &br_sig_blind;
  RooRealVar lum_var("lum_var", "lum_var", lum, 0.5*lum, 1.5*lum); lum_var.setConstant(1); lum_var_ = &lum_var;
  RooRealVar bxs_var("bxs_var", "bxs_var", bxs, 0.5*bxs, 1.5*bxs); bxs_var.setConstant(1); bxs_var_ = &bxs_var;
  RooRealVar zxs_var("zxs_var", "zxs_var", zxs, zxs*0.5, zxs*1.5); zxs_var.setConstant(1);
  RooRealVar br_zll_var("br_zll_var", "br_zll_var", br_zll, 0., 1.); br_zll_var.setConstant(1); br_zll_var_ = &br_zll_var;
  RooRealVar br_sig_kappa("br_sig_kappa", "br_sig_kappa", 1. + sys_unc, 0., 2. + sys_unc); br_sig_kappa.setConstant(1);
  RooRealVar br_sig_beta("br_sig_beta", "br_sig_beta", 0., -10., 10.);
  RooFormulaVar br_sig_eff("br_sig_eff", "@0 * pow(@1, @2)", RooArgList(br_sig, br_sig_kappa, br_sig_beta)); br_sig_eff_ = &br_sig_eff;
  RooRealVar zero("zero", "zero", 0., -1., 1.); zero.setConstant(1);
  RooRealVar one("one", "one", 1., 0., 2.); one.setConstant(1);
  RooGaussian   br_sig_constr("br_sig_constr", "br_sig_constr", br_sig_beta, zero, one); //constraint on br_sig systematic
  if(!doConstraints_) br_sig_beta.setConstant(1);
  br_sig_constr_ = &br_sig_constr;
  xs_sig_ = xs.GetCrossSection(signame.Data());
  cout << "Global variables defined!\n";

  //For creating the total, simultaneous PDF
  RooCategory categories(selection.Data(), selection.Data()); categories_ = &categories;
  map<string, RooDataHist*> dataCategoryMap, toyDataCategoryMap; //for creating combined data samples
  vector<RooFormulaVar*> n_sigs;
  vector<RooRealVar*> n_bkgs;
  vector<RooAbsPdf*> totPDFs;
  vector<RooAbsPdf*> totPDFs_exp1, totPDFs_cheb2, totPDFs_cheb3, totPDFs_cheb4, totPDFs_cheb5, totPDFs_bst2, totPDFs_bst3, totPDFs_bst4, totPDFs_bst5; //alternate background descriptions
  vector<RooAbsPdf*> bkgPDFs;
  map<int,int> orders;
  std::set<int> orders_used;

  ////////////////////////////////////////////////////////////
  // Read in the data
  ////////////////////////////////////////////////////////////

  //save the workspace
  TFile* fOut = new TFile(Form("workspaces/fit_%s_lepm_background_binned%s_%s_%s.root", selection.Data(),
                               (doConstraints_) ? "_constr" : "",
                               year_string.Data(), set_str.Data()), "RECREATE");
  RooWorkspace ws("ws");
  int index = 0;
  for(int set : sets) {
    cout << "Reading in the data information for set " << set << endl;
    TFile* f = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "READ");
    if(!f) return 1 + 100*set;
    fmumu_ = TFile::Open(Form("histograms/mumu_lepm_%i_%s.hist", set, year_string.Data()), "READ");
    double n_mumu_data, n_mumu_mc, n_mumu_dy;
    if(!fmumu_) {
      if(require_same_flavor_)
        return 2 + 100*set;
      n_mumu_data = 1.e7*years.size();
      n_mumu_mc = 1.e7*years.size();
      n_mumu_dy = n_mumu_mc;
    } else {
      TH1D* hmumu_data = (TH1D*) fmumu_->Get("hdata");
      TH1D* hmumu_mc   = (TH1D*) fmumu_->Get("hbackground");
      TH1D* hmumu_dy   = (TH1D*) fmumu_->Get("hDY");
      if(!hmumu_data || !hmumu_mc || (!hmumu_dy && require_dy_hist_)) {
        cout << "MuMu histograms not available for set " << set << endl;
        return 4 + 100*set;
      }
      n_mumu_data = hmumu_data->Integral(hmumu_data->FindBin(70.), hmumu_data->FindBin(110.));
      n_mumu_mc   = hmumu_mc  ->Integral(hmumu_mc  ->FindBin(70.), hmumu_mc  ->FindBin(110.));
      n_mumu_dy   = (hmumu_dy) ? hmumu_dy->Integral(hmumu_dy  ->FindBin(70.), hmumu_dy  ->FindBin(110.)) : n_mumu_mc;
      delete hmumu_data;
      delete hmumu_mc;
      if(hmumu_dy) delete hmumu_dy;
    }

    fee_ = TFile::Open(Form("histograms/ee_lepm_%i_%s.hist", set, year_string.Data()), "READ");
    double n_ee_data, n_ee_mc, n_ee_dy;
    if(!fee_) {
      if(require_same_flavor_)
        return 3 + 100*set;
      n_ee_data = 1.e7*years.size();
      n_ee_mc = 1.e7*years.size();
      n_ee_dy = n_ee_mc;
    } else {
      TH1D* hee_data = (TH1D*) fee_->Get("hdata");
      TH1D* hee_mc   = (TH1D*) fee_->Get("hbackground");
      TH1D* hee_dy   = (TH1D*) fee_->Get("hDY");
      if(!hee_data || !hee_mc || (!hee_dy && require_dy_hist_)) {
        cout << "EE histograms not available for set " << set << endl;
        return 5 + 100*set;
      }
      n_ee_data = hee_data->Integral(hee_data->FindBin(70.), hee_data->FindBin(110.));
      n_ee_mc   = hee_mc  ->Integral(hee_mc  ->FindBin(70.), hee_mc  ->FindBin(110.));
      n_ee_dy   = (hee_dy) ? hee_dy  ->Integral(hee_dy  ->FindBin(70.), hee_dy  ->FindBin(110.)) : n_ee_mc;
      delete hee_data;
      delete hee_mc;
      if(hee_dy) delete hee_dy;
    }


    TH1D* hbkg  = (TH1D*) f->Get("hbackground");
    TH1D* hsig  = (TH1D*) f->Get(selection.Data());
    TH1D* hdata = (TH1D*) f->Get("hdata");
    if(!hbkg || !hsig || !hdata) {
      cout << "Not all base histograms found!\n";
      return 4 + 100*set;
    }

    if(blind_data_) {
      int bin_lo = hdata->FindBin(bmass - 5.);
      int bin_hi = hdata->FindBin(bmass + 5.);
      for(int bin = bin_lo; bin <= bin_hi; ++bin) hdata->SetBinContent(bin, 0.);
    }

    ////////////////////////////////////////////////////////////
    // Create RooFit products for the data
    ////////////////////////////////////////////////////////////

    //configure the observable variable
    lepm.setBins((lepm.getMax() - lepm.getMin())/hbkg->GetBinWidth(1)); //match binning of histograms
    lepm_ = &lepm;

    //create RooDataHist from histograms
    RooDataHist bkgData (Form("bkgData_%i" , set),  "Background Data", RooArgList(lepm), hbkg );
    RooDataHist sigData (Form("sigData_%i" , set),  "Signal Data"    , RooArgList(lepm), hsig );
    RooDataHist dataData(Form("dataData_%i", set),  "Data Data"      , RooArgList(lepm), hdata);

    //decide which dataset to fit to
    RooDataHist *fitData;
    if(fit_toy_mc_) fitData = (blind_data_) ? (RooDataHist*) bkgData.reduce (Form("lepm < %.3f || lepm > %.3f", bmass - 5., bmass + 5.)) : &bkgData;
    else            fitData = &dataData; //(blind_data_) ? (RooDataHist*) dataData.reduce(Form("lepm < %.3f || lepm > %.3f", bmass - 5., bmass + 5.)) : &dataData;
    double nexp_sig = hsig->Integral(hsig->FindBin(lepm.getMin()), hsig->FindBin(lepm.getMax()));
    RooRealVar* eff_nominal = new RooRealVar(Form("eff_nominal_%i", set), "Nominal signal efficiency", nexp_sig/(lum*xs_sig_), 0., 1.);
    eff_nominal->setConstant(1);

    RooRealVar* mumu_n_data   = new RooRealVar(Form("mumu_n_data_%i"  , set), "Z->mumu count from Data"     , n_mumu_data          , 0., 1e9); mumu_n_data  ->setConstant(1);
    RooRealVar* ee_n_data     = new RooRealVar(Form("ee_n_data_%i"    , set), "Z->ee count from Data"       , n_ee_data            , 0., 1e9); ee_n_data    ->setConstant(1);
    //account for non-DY contamination by N(DY mumu) = N(mumu)*(N(MC DY mumu) / N(MC mumu))
    //eff(DY mumu) = N(DY mumu) / N(Z->mumu)
    // --> N(Z->mumu) = N(DY mumu) / eff(DY mumu) = N(mumu) * (N(DY mumu)/N(mumu)) / eff(DY mumu)
    // --> effective efficiency to use for N(Z->mumu) = N(mumu) / eff'(mumu):
    // eff'(mumu) = eff(DY mumu) * N(mumu)/N(DY mumu) = N(MC mumu) / N(Z->mumu), *ignoring the contamination*
    RooRealVar* mumu_eff_mc   = new RooRealVar(Form("mumu_eff_mc_%i"  , set), "Z->mumu efficiency from MC"  , n_mumu_mc  /(lum*zxs*br_zll), 0., 1.); mumu_eff_mc_ = mumu_eff_mc;
    RooRealVar* ee_eff_mc     = new RooRealVar(Form("ee_eff_mc_%i"    , set), "Z->ee efficiency from MC"    , n_ee_mc    /(lum*zxs*br_zll), 0., 1.); ee_eff_mc_ = ee_eff_mc;
    mumu_eff_mc  ->setConstant(1);
    ee_eff_mc    ->setConstant(1);

    ////////////////////////////////////////////////////////////
    // Fit the signal distribution
    ////////////////////////////////////////////////////////////

    cout << "--- Performing the signal fit for set " << set << endl;

    RooRealVar* mean     = new RooRealVar(Form("mean_%i"     , set), "mean", (doHiggs) ? 125. : 91., (doHiggs) ? 120. : 85., (doHiggs) ? 130. : 95.);
    RooRealVar* sigma    = new RooRealVar(Form("sigma_%i"    , set), "sigma", 2., 0.1, 5.);
    RooRealVar* alpha    = new RooRealVar(Form("alpha_%i"    , set), "alpha", 1., 0.1, 10.);
    RooRealVar* enne     = new RooRealVar(Form("enne_%i"     , set), "enne", (set == 9) ? 17. : 5., 0.1, 30.);
    RooRealVar* mean2    = new RooRealVar(Form("mean2_%i"    , set), "mean2", lepm.getVal(), lepm.getMin(), lepm.getMax());
    RooRealVar* sigma2   = new RooRealVar(Form("sigma2_%i"   , set), "sigma2", 5., 0.1, 10.);
    RooCBShape* sigpdf1  = new RooCBShape(Form("sigpdf1_%i"  , set), "sigpdf1", lepm, *mean, *sigma, *alpha, *enne);
    RooGaussian* sigpdf2 = new RooGaussian(Form("sigpdf2_%i" , set), "sigpdf2", lepm, *mean2, *sigma2);
    RooRealVar* fracsig  = new RooRealVar(Form("fracsig_%i"  , set), "fracsig", 0.7, 0., 1.);
    RooAddPdf* sigPDF    = new RooAddPdf(Form("sigPDF_%i"    , set), "signal PDF", *sigpdf1, *sigpdf2, *fracsig);
    RooRealVar* N_sig    = new RooRealVar(Form("N_sig_%i"    , set), "N_sig", 2e5, 1e2, 3e6);
    RooAddPdf* totsigpdf = new RooAddPdf(Form("totSigPDF_%i" , set), "Signal PDF", RooArgList(*sigPDF), RooArgList(*N_sig));

    totsigpdf->fitTo(sigData, RooFit::SumW2Error(1));
    fracsig->setConstant(1); mean->setConstant(1); sigma->setConstant(1);
    enne->setConstant(1); alpha->setConstant(1); mean2->setConstant(1); sigma2->setConstant(1);

    auto sigframe = lepm.frame();
    sigData.plotOn(sigframe);
    sigPDF->plotOn(sigframe);
    sigPDF->plotOn(sigframe, RooFit::Components(Form("sigpdf1_%i", set)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    sigPDF->plotOn(sigframe, RooFit::Components(Form("sigpdf2_%i", set)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

    TCanvas* c_sig = new TCanvas();
    sigframe->Draw();
    c_sig->SaveAs(Form("plots/latest_production/%s/fit_%s_sig_lepm_binned_background_%s_cat_%i.png", year_string.Data(), selection.Data(), set_str.Data(), index));
    delete c_sig;
    delete sigframe;

    ////////////////////////////////////////////////////////////
    // Fit background distributions
    ////////////////////////////////////////////////////////////

    cout << "---Performing background fit for set " << set << endl
         << "---Number of background events to fit: " << bkgData.numEntries() << endl;

    //Total PDF parameters
    //Use mumu and ee efficiencies to normalize our signal, N(sig) = eff(sig) * Br(sig) * (N(mumu)/eff(mumu) + N(ee)/eff(ee))/2 / Br(Z->ll) * XS(B) / XS(Z)
    // --> N(Z->ll) = N(mumu) / eff(mumu) --> N(Z) = N(mumu) / eff(mumu) / Br(Z->mumu)
    // Consider geometric mean instead of average of estimates: N(Z) = sqrt(N(Z | mumu) * N(Z | ee))
    RooFormulaVar* n_z_mumu = new RooFormulaVar(Form("n_z_mumu_%i", set), "@0 / @1 / @2", RooArgList(*mumu_n_data, *mumu_eff_mc, br_zll_var));
    RooFormulaVar* n_z_ee   = new RooFormulaVar(Form("n_z_ee_%i"  , set), "@0 / @1 / @2", RooArgList(*ee_n_data  , *ee_eff_mc  , br_zll_var));
    RooFormulaVar* n_sig = new RooFormulaVar(Form("n_sig_%i", set),  "@0*@1*sqrt(@2*@3)*(@4/@5)", //using geometric mean of N(Z) estimates
                                             RooArgList(br_sig_eff, *eff_nominal, *n_z_mumu, *n_z_ee, bxs_var, zxs_var));
    // RooFormulaVar* n_sig = new RooFormulaVar(Form("n_sig_%i", set),  "@0*@1*(@2+@3)/2", //using mean of N(Z) estimates
    //                                          RooArgList(br_sig_eff, *eff_nominal, *n_z_mumu, *n_z_ee));
    // RooFormulaVar* n_sig = new RooFormulaVar(Form("n_sig_%i", set),  "@0*@1*@2*@3", RooArgList(br_sig_eff, lum_var, bxs_var, *eff_nominal));
    RooRealVar*    n_bkg = new RooRealVar   (Form("n_bkg_%i", set), "n_bkg", bkgData.sumEntries(), 0., 1.e7);
    // RooRealVar* mumu_n_mc     = new RooRealVar(Form("mumu_n_mc_%i"    , set), "Z->mumu count from MC"       , n_mumu_mc            , 0., 1e9); mumu_n_mc    ->setConstant(1);
    // RooRealVar* mumu_n_dy     = new RooRealVar(Form("mumu_n_dy_%i"    , set), "Z->mumu count from DY MC"    , n_mumu_dy            , 0., 1e9); mumu_n_dy    ->setConstant(1);
    if(abs(n_sig->getVal() - lum*eff_nominal->getVal()*bxs*br_sig.getVal())/n_sig->getVal() > 0.01) {
      cout << "!!! Warning! Signal yield calculations significantly disagree! n_sig = " << n_sig->getVal()
           << " and lumi*eff*bxs*br_sig = " << lum*eff_nominal->getVal()*bxs*br_sig.getVal() << endl;
    }

    n_sigs.push_back(n_sig);
    n_bkgs.push_back(n_bkg);
    cout << endl << endl << "-------------------------------------------------------------" << endl
         << "--- Set " << set << ":\n"
         << "--- Eff(sig) = " << eff_nominal->getVal() << " lum = " << lum_var.getVal() << endl
         << "--- xs(sig)  = " << xs_sig_ << " N(exp sig) = " << nexp_sig << " vs lum*xs*eff*br_sig = " << lum*bxs*eff_nominal->getVal()*br_sig.getVal() << endl
         << "--- xsec(B)  = " << bxs_var.getVal() << endl
         << "--- N(sig)   = " << n_sig->getVal() << " N(bkg) = " << n_bkg->getVal() << " (br_sig = " << br_sig.getVal() << ")\n"
         << "--- N(mumu)  = " << n_mumu_data << " (" << n_mumu_mc << ") Eff(mumu) = " << mumu_eff_mc->getVal() << endl
         << "--- N(ee)    = " << n_ee_data   << " (" << n_ee_mc   << ") Eff(ee)   = " << ee_eff_mc  ->getVal() << endl
         << "--- N(Z)     = " << n_z_mumu->getVal() << " (mumu) vs " << n_z_ee->getVal() << " (ee) " << (lum*zxs) << " (calc)\n"
         << "-------------------------------------------------------------" << endl << endl << endl;

    if(!do_fits_) continue;

    if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(1);}

    ////////////////////////////////////
    // Alternate Background Functions //
    ////////////////////////////////////

    //exp bkg
    RooRealVar*     tau_exp1    = new RooRealVar(Form("tau_exp1_%i"  , set), "tau_exp1", -0.0386, -10., 0.);
    RooExponential* bkgPDF_exp1 = new RooExponential(Form("bkgPDF_exp1_%i", set), "bkgPDF_exp1", lepm, *tau_exp1);
    bkgPDFs.push_back(bkgPDF_exp1);

    //power bkg
    RooRealVar a_pow(Form("a_pow_%i", set), "a_pow",  5.445e-3,   -5.,   5.);
    RooRealVar b_pow(Form("b_pow_%i", set), "b_pow", -1.229   ,  -10.,  10.);
    RooRealVar c_pow(Form("c_pow_%i", set), "c_pow", 8.914e1  , -200., 200.);
    RooRealVar d_pow(Form("d_pow_%i", set), "d_pow", 5.428e1  , -100., 100.);
    RooGenericPdf bkgPDF_pow(Form("bkgPDF_pow_%i", set), "(@1 + (@2 + (@3 + @4/@0)/@0)/@0)/@0", RooArgList(lepm,a_pow, b_pow, c_pow, d_pow));

    //chebychev bkg (1st order)
    RooRealVar* a_cheb2 = new RooRealVar(Form("a_cheb2_%i", set), "a_cheb",-0.8     , -5., 5.);
    RooRealVar* b_cheb2 = new RooRealVar(Form("b_cheb2_%i", set), "b_cheb", 0.290163, -5., 5.);
    RooChebychev* bkgPDF_cheb2 = new RooChebychev(Form("bkgPDF_cheb2_%i", set) ,"bkgPDF_cheb2", lepm, RooArgList(*a_cheb2, *b_cheb2));
    bkgPDFs.push_back(bkgPDF_cheb2);

    //chebychev bkg (2nd order)
    RooRealVar* a_cheb3 = new RooRealVar(Form("a_cheb3_%i", set), "a_cheb",-0.8     , -5., 5.);
    RooRealVar* b_cheb3 = new RooRealVar(Form("b_cheb3_%i", set), "b_cheb", 0.290163, -5., 5.);
    RooRealVar* c_cheb3 = new RooRealVar(Form("c_cheb3_%i", set), "c_cheb",-0.1     , -5., 5.);
    RooChebychev* bkgPDF_cheb3 = new RooChebychev(Form("bkgPDF_cheb3_%i", set) ,"bkgPDF_cheb3", lepm, RooArgList(*a_cheb3, *b_cheb3, *c_cheb3));
    bkgPDFs.push_back(bkgPDF_cheb3);

    //chebychev bkg (3rd order)
    RooRealVar* a_cheb4 = new RooRealVar(Form("a_cheb4_%i", set), "a_cheb",-0.8     , -5., 5.);
    RooRealVar* b_cheb4 = new RooRealVar(Form("b_cheb4_%i", set), "b_cheb", 0.290163, -5., 5.);
    RooRealVar* c_cheb4 = new RooRealVar(Form("c_cheb4_%i", set), "c_cheb",-0.1     , -5., 5.);
    RooRealVar* d_cheb4 = new RooRealVar(Form("d_cheb4_%i", set), "d_cheb", 0.      , -5., 5.);
    RooChebychev* bkgPDF_cheb4 = new RooChebychev(Form("bkgPDF_cheb4_%i", set) ,"bkgPDF_cheb4", lepm, RooArgList(*a_cheb4, *b_cheb4, *c_cheb4, *d_cheb4));
    bkgPDFs.push_back(bkgPDF_cheb4);

    //chebychev bkg (4th order)
    RooRealVar* a_cheb5 = new RooRealVar(Form("a_cheb5_%i", set), "a_cheb",-0.8     , -5., 5.);
    RooRealVar* b_cheb5 = new RooRealVar(Form("b_cheb5_%i", set), "b_cheb", 0.290163, -5., 5.);
    RooRealVar* c_cheb5 = new RooRealVar(Form("c_cheb5_%i", set), "c_cheb",-0.1     , -5., 5.);
    RooRealVar* d_cheb5 = new RooRealVar(Form("d_cheb5_%i", set), "d_cheb", 0.      , -5., 5.);
    RooRealVar* e_cheb5 = new RooRealVar(Form("e_cheb5_%i", set), "d_cheb", 0.      , -5., 5.);
    RooChebychev* bkgPDF_cheb5 = new RooChebychev(Form("bkgPDF_cheb5_%i", set) ,"bkgPDF_cheb5", lepm, RooArgList(*a_cheb5, *b_cheb5, *c_cheb5, *d_cheb5, *e_cheb5));
    bkgPDFs.push_back(bkgPDF_cheb5);

    //landau bkg
    RooRealVar a_land(Form("a_land_%i", set), "a_land", 3.975e1 ,   0.,  90.);
    RooRealVar b_land(Form("b_land_%i", set), "b_land", 7.470e-7,  -5.,   5.);
    RooLandau bkgPDF_land(Form("bkgPDF_land_%i", set) ,"bkgPDF_land", lepm, a_land, b_land);

    //1st order Bernstein
    RooRealVar* a_bst2 = new RooRealVar(Form("a_bst2_%i", set), "a_bst2", 1.491   , -5., 5.);
    RooRealVar* b_bst2 = new RooRealVar(Form("b_bst2_%i", set), "b_bst2", 2.078e-1, -5., 5.);
    RooBernstein* bkgPDF_bst2 = new RooBernstein(Form("bkgPDF_bst2_%i", set), "bkgPDF_bst2", lepm, RooArgList(*a_bst2, *b_bst2));
    bkgPDFs.push_back(bkgPDF_bst2);

    //2nd order Bernstein
    RooRealVar* a_bst3 = new RooRealVar(Form("a_bst3_%i", set), "a_bst3", 1.491   , -5., 5.);
    RooRealVar* b_bst3 = new RooRealVar(Form("b_bst3_%i", set), "b_bst3", 2.078e-1, -5., 5.);
    RooRealVar* c_bst3 = new RooRealVar(Form("c_bst3_%i", set), "c_bst3", 5.016e-1, -5., 5.);
    RooBernstein* bkgPDF_bst3 = new RooBernstein(Form("bkgPDF_bst3_%i", set), "bkgPDF_bst3", lepm, RooArgList(*a_bst3, *b_bst3, *c_bst3));
    bkgPDFs.push_back(bkgPDF_bst3);

    //3rd order Bernstein
    RooRealVar* a_bst4 = new RooRealVar(Form("a_bst4_%i", set), "a_bst4", 1.404   , -5., 5.);
    RooRealVar* b_bst4 = new RooRealVar(Form("b_bst4_%i", set), "b_bst4", 2.443e-1, -5., 5.);
    RooRealVar* c_bst4 = new RooRealVar(Form("c_bst4_%i", set), "c_bst4", 5.549e-1, -5., 5.);
    RooRealVar* d_bst4 = new RooRealVar(Form("d_bst4_%i", set), "d_bst4", 3.675e-1, -5., 5.);
    RooBernstein* bkgPDF_bst4 = new RooBernstein(Form("bkgPDF_bst4_%i", set), "Background PDF", lepm, RooArgList(*a_bst4, *b_bst4, *c_bst4, *d_bst4));
    bkgPDFs.push_back(bkgPDF_bst4);

    //4th order Bernstein
    RooRealVar* a_bst5 = new RooRealVar(Form("a_bst5_%i", set), "a_bst5", 1.471   , -5., 5.);
    RooRealVar* b_bst5 = new RooRealVar(Form("b_bst5_%i", set), "b_bst5", 1.452e-1, -5., 5.);
    RooRealVar* c_bst5 = new RooRealVar(Form("c_bst5_%i", set), "c_bst5", 7.524e-1, -5., 5.);
    RooRealVar* d_bst5 = new RooRealVar(Form("d_bst5_%i", set), "d_bst5", 2.696e-1, -5., 5.);
    RooRealVar* e_bst5 = new RooRealVar(Form("e_bst5_%i", set), "e_bst5", 3.687e-1, -5., 5.);
    RooBernstein* bkgPDF_bst5 = new RooBernstein(Form("bkgPDF_bst5_%i", set), "bkgPDF_bst5", lepm, RooArgList(*a_bst5, *b_bst5, *c_bst5, *d_bst5, *e_bst5));
    bkgPDFs.push_back(bkgPDF_bst5);

    //Signal + Alternate background PDFs
    RooAddPdf* totPDF_exp1  = new RooAddPdf(Form("totPDF_exp1_%i" , set), "totPDF_exp1" , RooArgList(*sigPDF, *bkgPDF_exp1) , RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_cheb2 = new RooAddPdf(Form("totPDF_cheb2_%i", set), "totPDF_cheb2", RooArgList(*sigPDF, *bkgPDF_cheb2), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_cheb3 = new RooAddPdf(Form("totPDF_cheb3_%i", set), "totPDF_cheb3", RooArgList(*sigPDF, *bkgPDF_cheb3), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_cheb4 = new RooAddPdf(Form("totPDF_cheb4_%i", set), "totPDF_cheb4", RooArgList(*sigPDF, *bkgPDF_cheb4), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_cheb5 = new RooAddPdf(Form("totPDF_cheb5_%i", set), "totPDF_cheb5", RooArgList(*sigPDF, *bkgPDF_cheb5), RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_pow (Form("totPDF_pow_%i" , set), "totPDF_pow" , RooArgList(*sigPDF, *bkgPDF_pow) , RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_land(Form("totPDF_land_%i", set), "totPDF_land", RooArgList(*sigPDF, *bkgPDF_land), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_bst2 = new RooAddPdf(Form("totPDF_bst2_%i", set), "totPDF_bst2", RooArgList(*sigPDF, *bkgPDF_bst2), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_bst3 = new RooAddPdf(Form("totPDF_bst3_%i", set), "totPDF_bst3", RooArgList(*sigPDF, *bkgPDF_bst3), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_bst4 = new RooAddPdf(Form("totPDF_bst4_%i", set), "totPDF_bst4", RooArgList(*sigPDF, *bkgPDF_bst4), RooArgList(*n_sig, *n_bkg));
    RooAddPdf* totPDF_bst5 = new RooAddPdf(Form("totPDF_bst5_%i", set), "totPDF_bst5", RooArgList(*sigPDF, *bkgPDF_bst5), RooArgList(*n_sig, *n_bkg));

    //PDFs with constraints
    // RooProdPdf totPDF_constr_exp (Form("totPDF_constr_alt_%i" , set), "totPDF_constr_alt" , RooArgList(totPDF_exp , br_sig_constr));
    // RooProdPdf totPDF_constr_cheb(Form("totPDF_constr_cheb_%i", set), "totPDF_constr_cheb", RooArgList(totPDF_cheb, br_sig_constr));
    // RooProdPdf totPDF_constr_pow (Form("totPDF_constr_pow_%i" , set), "totPDF_constr_pow" , RooArgList(totPDF_pow , br_sig_constr));
    // RooProdPdf totPDF_constr_land(Form("totPDF_constr_land_%i", set), "totPDF_constr_land", RooArgList(totPDF_land, br_sig_constr));
    // RooProdPdf totPDF_constr_bst2(Form("totPDF_constr_bst2_%i", set), "totPDF_constr_bst2", RooArgList(totPDF_bst2, br_sig_constr));
    // RooProdPdf totPDF_constr_bst3(Form("totPDF_constr_bst3_%i", set), "totPDF_constr_bst3", RooArgList(totPDF_bst3, br_sig_constr));
    // RooProdPdf totPDF_constr_bst4(Form("totPDF_constr_bst4_%i", set), "totPDF_constr_bst4", RooArgList(totPDF_bst4, br_sig_constr));
    // RooProdPdf totPDF_constr_bst5(Form("totPDF_constr_bst5_%i", set), "totPDF_constr_bst5", RooArgList(totPDF_bst5, br_sig_constr));

    string category = Form("%s_%i", selection.Data(), index);
    categories.defineType(category.c_str(), index);


    //Identify the main background PDF by F-Test
    int bkg_order = perform_f_test(*fitData, (use_bernstein_) ? "Bernstein" : "Chebychev", 1);
    cout << endl
         << "############################################################################" << endl
         << "### For set " << set << " background order = " << bkg_order << endl
         << "############################################################################" << endl
         << endl;

    orders[set] = bkg_order;
    orders_used.insert(bkg_order);

    RooAbsPdf* bkgpdf_type;
    if(use_bernstein_) {
      if(bkg_order < 1 || bkg_order > 4) {
        cout << "Background order " << bkg_order << " for Bernstein too high/low! Using default for this set of ";
        bkg_order = (doHiggs || set == 13) ? 2 : 3;
        cout << bkg_order << endl;
      }
      if     (bkg_order == 1) bkgpdf_type = bkgPDF_bst2;
      else if(bkg_order == 2) bkgpdf_type = bkgPDF_bst3;
      else if(bkg_order == 3) bkgpdf_type = bkgPDF_bst4;
      else if(bkg_order == 4) bkgpdf_type = bkgPDF_bst5;
    } else {
      if(bkg_order < 1 || bkg_order > 4) {
        cout << "Background order " << bkg_order << " for Chebychev too high/low! Using default for this set of ";
        bkg_order = (doHiggs || set == 13) ? 2 : 3;
        cout << bkg_order << endl;
      }
      if     (bkg_order == 1) bkgpdf_type = bkgPDF_cheb2;
      else if(bkg_order == 2) bkgpdf_type = bkgPDF_cheb3;
      else if(bkg_order == 3) bkgpdf_type = bkgPDF_cheb4;
      else if(bkg_order == 4) bkgpdf_type = bkgPDF_cheb5;
    }
    bkgpdf_type->SetName(Form("bkgPDF_%i", set));
    RooAddPdf* totPDF         = new RooAddPdf (Form("totPDF_%i"       , set), "totPDF"       , RooArgList(*sigPDF, *bkgpdf_type), RooArgList(*n_sig, *n_bkg));
    RooProdPdf* totPDF_constr = new RooProdPdf(Form("totPDF_constr_%i", set), "totPDF_constr", RooArgList(*totPDF, br_sig_constr));

    if(doConstraints_) {
      // totPDF_constr_exp .fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_cheb.fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_pow .fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_land.fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_bst2.fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_bst3.fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_bst4.fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_constr_bst5.fitTo(*fitData, RooFit::SumW2Error(1));
      // if(blind_data_)
      //   totPDF_constr     ->fitTo(*fitData, RooFit::Range("LowSideband", "HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
      // else
      if(blind_data_)
        totPDF_constr     ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1), RooFit::Range("LowSideband,HighSideband"));
      else
        totPDF_constr     ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
      totPDFs.push_back(totPDF_constr);
    } else {
      // totPDF_pow .fitTo(*fitData, RooFit::SumW2Error(1));
      // totPDF_land.fitTo(*fitData, RooFit::SumW2Error(1));
      if(blind_data_) {
        totPDF_exp1 ->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb2->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb3->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb4->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb5->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
        if(use_bernstein_) {
          totPDF_bst2 ->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
          totPDF_bst3 ->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
          totPDF_bst4 ->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
          totPDF_bst5 ->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
        }
        totPDF      ->fitTo(*fitData, RooFit::Range("LowSideband,HighSideband"), RooFit::SumW2Error(1), RooFit::Extended(1));
      } else {
        totPDF_exp1 ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb2->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb3->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb4->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
        totPDF_cheb5->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
        if(use_bernstein_) {
          totPDF_bst2 ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
          totPDF_bst3 ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
          totPDF_bst4 ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
          totPDF_bst5 ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
        }
        totPDF      ->fitTo(*fitData, RooFit::SumW2Error(1), RooFit::Extended(1));
      }
      totPDFs_exp1 .push_back(totPDF_exp1 );
      totPDFs_cheb2.push_back(totPDF_cheb2);
      totPDFs_cheb3.push_back(totPDF_cheb3);
      totPDFs_cheb4.push_back(totPDF_cheb4);
      totPDFs_cheb5.push_back(totPDF_cheb5);
      totPDFs_bst2 .push_back(totPDF_bst2 );
      totPDFs_bst3 .push_back(totPDF_bst3 );
      totPDFs_bst4 .push_back(totPDF_bst4 );
      totPDFs_bst5 .push_back(totPDF_bst5 );
      totPDFs.push_back(totPDF);
    }
    if(!do_fits_) {
      cout << "Finished dry-run testing!\n";
      return 0;
    }

    br_sig.setVal(0.);
    double nexp = hbkg->Integral(hbkg->FindBin(lepm.getMin()), hbkg->FindBin(lepm.getMax()));
    RooDataHist* gen_data = totPDF->generateBinned(RooArgSet(lepm), nexp);
    dataCategoryMap[category] = &dataData;
    toyDataCategoryMap[category] = gen_data;

    ////////////////////////////////////////////////////////////
    // Plot the results
    ////////////////////////////////////////////////////////////

    if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(0);}
    auto xframe = lepm.frame();
    if(!xframe) {cout << "Error! POI frame not defined --> Problem with the variable!\n"; return 10 + 100*set;}
    fitData->plotOn(xframe);
    if(doConstraints_)
      totPDF_constr->plotOn(xframe, RooFit::LineColor(kBlue), RooFit::Range("FullRange"));
    else
      totPDF->plotOn(xframe, RooFit::LineColor(kBlue), RooFit::Range("FullRange"));
    cout << "### totPDF chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
    if(!doConstraints_) {
      if(use_bernstein_) {
        if(bkg_order != 1) totPDF_bst2->plotOn(xframe, RooFit::LineColor(kRed       ), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Bernstein 1st order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
        if(bkg_order != 2) totPDF_bst3->plotOn(xframe, RooFit::LineColor(kGreen     ), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Bernstein 2nd order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
        if(bkg_order != 3) totPDF_bst4->plotOn(xframe, RooFit::LineColor(kYellow + 1), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Bernstein 3rd order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
        if(bkg_order != 4) totPDF_bst5->plotOn(xframe, RooFit::LineColor(kViolet - 2), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Bernstein 4th order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
      } else {
        if(bkg_order != 1) totPDF_cheb2->plotOn(xframe, RooFit::LineColor(kRed       ), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Chebychev 1st order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
        if(bkg_order != 2) totPDF_cheb3->plotOn(xframe, RooFit::LineColor(kGreen     ), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Chebychev 2nd order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
        if(bkg_order != 3) totPDF_cheb4->plotOn(xframe, RooFit::LineColor(kYellow + 1), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Chebychev 3rd order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
        if(bkg_order != 4) totPDF_cheb5->plotOn(xframe, RooFit::LineColor(kViolet - 2), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
        cout << "### Chebychev 4th order chi square: " << xframe->chiSquare()*fitData->numEntries() << endl;
      }
      // bkgPDF_exp1->plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      // bkgPDF_cheb.plotOn(xframe, RooFit::LineColor(kAzure), RooFit::LineStyle(kDashed));
      // bkgPDF_pow.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
      // bkgPDF_land.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
    }
    auto c1 = new TCanvas(Form("c_%i", set), Form("c_%i", set));
    xframe->Draw();
    TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
    //ones not drawn won't appear in the legend
    TString func_name = (use_bernstein_) ? "Bernstein" : "Chebychev";
    if(bkg_order == 2) func_name += " (2nd)";
    else if(bkg_order == 2) func_name += " (3nd)";
    else func_name += (doHiggs || set == 13) ? " (2nd)" : " (3rd)"; //guess
    leg->AddEntry(xframe->findObject(Form("totPDF_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), func_name.Data()  , "L");
    if(!doConstraints_) {
      if(use_bernstein_) {
        if(bkg_order != 1) leg->AddEntry(xframe->findObject(Form("totPDF_bst2_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Bernstein (1st)"  , "L");
        if(bkg_order != 2) leg->AddEntry(xframe->findObject(Form("totPDF_bst3_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Bernstein (2nd)"  , "L");
        if(bkg_order != 3) leg->AddEntry(xframe->findObject(Form("totPDF_bst4_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Bernstein (3rd)"  , "L");
        if(bkg_order != 4) leg->AddEntry(xframe->findObject(Form("totPDF_bst5_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Bernstein (4th)"  , "L");
      } else {
        if(bkg_order != 1) leg->AddEntry(xframe->findObject(Form("totPDF_cheb2_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Chebychev (1st)"  , "L");
        if(bkg_order != 2) leg->AddEntry(xframe->findObject(Form("totPDF_cheb3_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Chebychev (2nd)"  , "L");
        if(bkg_order != 3) leg->AddEntry(xframe->findObject(Form("totPDF_cheb4_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Chebychev (3rd)"  , "L");
        if(bkg_order != 4) leg->AddEntry(xframe->findObject(Form("totPDF_cheb5_%i_Norm[lepm]_Range[FullRange]_NormRange[FullRange]", set)), "Chebychev (4th)"  , "L");
      }
    }
    xframe->Print();
    // if(!doHiggs) {
    //   leg->AddEntry(xframe->findObject("bkgPDF_bst5_Norm[lepm]"), "Bernstein (4th)"  , "L");
    // }
    // if(doHiggs) {
    //   leg->AddEntry(xframe->findObject("bkgPDF_bst2_Norm[lepm]"), "Bernstein (1st)"  , "L");
    //   leg->AddEntry(xframe->findObject("bkgPDF_bst4_Norm[lepm]"), "Bernstein (3rd)"  , "L");
    // }
    // leg->AddEntry(xframe->findObject("bkgPDF_exp_Norm[lepm]") , "Exponential", "L");
    // leg->AddEntry(xframe->findObject("bkgPDF_pow_Norm[lepm]") , "Power"      , "L");
    // leg->AddEntry(xframe->findObject("bkgPDF_land_Norm[lepm]"), "Landau"     , "L");
    leg->Draw();

    c1->SaveAs(Form("plots/latest_production/%s/fit_%s_lepm_binned_background%s_%s_cat_%i.png", year_string.Data(), selection.Data(),
                    (doConstraints_) ? "_constr" : "",
                    set_str.Data(), index));

    // fOut->cd();
    // auto bkg_data = (doHiggs) ? bkgPDF_bst3.generate(RooArgSet(lepm), n_bkg.getVal()) : bkgPDF_bst4.generate(RooArgSet(lepm), n_bkg.getVal());

    // if(doConstraints_) {
    //   ws.import(totPDF_constr);
    //   ws.import(totPDF_constr_exp, RooFit::RecycleConflictNodes());
    // } else {
    //   ws.import(totPDF);
    //   if(!doHiggs) {
    //     ws.import(totPDF_bst5, RooFit::RecycleConflictNodes());
    //     ws.import(totPDF_bst3, RooFit::RecycleConflictNodes());
    //   } else {
    //     ws.import(totPDF_bst2, RooFit::RecycleConflictNodes());
    //     ws.import(totPDF_bst4, RooFit::RecycleConflictNodes());
    //   }
    //   // ws.import(totPDF_exp, RooFit::RecycleConflictNodes());
    // }
    // if(doHiggs)
    //   ws.import(bkgPDF_bst3);
    // else
    //   ws.import(bkgPDF_bst4);
    // ws.import(*bkg_data);

    ++index;
  }
  cout << "--- Finished getting data for each category! Creating simultaneous PDF\n";
  RooSimultaneous totPDF_cat      ("totPDF"      , "The Total PDF", categories);
  RooSimultaneous totPDF_exp1_cat ("totPDF_exp1" , "The Total PDF", categories);
  RooSimultaneous totPDF_cheb3_cat("totPDF_cheb3", "The Total PDF", categories);
  RooSimultaneous totPDF_cheb4_cat("totPDF_cheb4", "The Total PDF", categories);
  RooSimultaneous totPDF_cheb5_cat("totPDF_cheb5", "The Total PDF", categories);
  RooSimultaneous totPDF_bst2_cat ("totPDF_bst2" , "The Total PDF", categories);
  RooSimultaneous totPDF_bst3_cat ("totPDF_bst3" , "The Total PDF", categories);
  RooSimultaneous totPDF_bst4_cat ("totPDF_bst4" , "The Total PDF", categories);
  RooSimultaneous totPDF_bst5_cat ("totPDF_bst5" , "The Total PDF", categories);
  for(unsigned i = 0; i < totPDFs.size(); ++i) {
    totPDF_cat      .addPdf(*totPDFs[i]      , Form("%s_%i", selection.Data(), i));
    if(!doConstraints_) {
      totPDF_exp1_cat .addPdf(*totPDFs_exp1 [i], Form("%s_%i", selection.Data(), i));
      totPDF_cheb3_cat.addPdf(*totPDFs_cheb3[i], Form("%s_%i", selection.Data(), i));
      totPDF_cheb4_cat.addPdf(*totPDFs_cheb4[i], Form("%s_%i", selection.Data(), i));
      totPDF_cheb5_cat.addPdf(*totPDFs_cheb5[i], Form("%s_%i", selection.Data(), i));
      totPDF_bst2_cat .addPdf(*totPDFs_bst2 [i], Form("%s_%i", selection.Data(), i));
      totPDF_bst3_cat .addPdf(*totPDFs_bst3 [i], Form("%s_%i", selection.Data(), i));
      totPDF_bst4_cat .addPdf(*totPDFs_bst4 [i], Form("%s_%i", selection.Data(), i));
      totPDF_bst5_cat .addPdf(*totPDFs_bst5 [i], Form("%s_%i", selection.Data(), i));
    }
  }

  cout << "Total combined PDF defined!\n";

  RooDataHist combined_data("combined_data", "combined_data", lepm, categories, dataCategoryMap);
  RooDataHist combined_toy_data("combined_toy_data", "combined_toy_data", lepm, categories, toyDataCategoryMap);

  cout << "Combined data defined!\n";

  fOut->cd();
  ws.import(totPDF_cat      );
  if(!doConstraints_) {
    ws.import(totPDF_exp1_cat , RooFit::RecycleConflictNodes());
    if(!use_bernstein_ && orders_used.find(2) == orders_used.end())
      ws.import(totPDF_cheb3_cat, RooFit::RecycleConflictNodes());
    if(!use_bernstein_ && orders_used.find(3) == orders_used.end())
      ws.import(totPDF_cheb4_cat, RooFit::RecycleConflictNodes());
    if(!use_bernstein_ && orders_used.find(4) == orders_used.end())
      ws.import(totPDF_cheb5_cat, RooFit::RecycleConflictNodes());
    if(use_bernstein_ && orders_used.find(1) == orders_used.end())
      ws.import(totPDF_bst2_cat , RooFit::RecycleConflictNodes());
    if(use_bernstein_ && orders_used.find(2) == orders_used.end())
      ws.import(totPDF_bst3_cat , RooFit::RecycleConflictNodes());
    if(use_bernstein_ && orders_used.find(3) == orders_used.end())
      ws.import(totPDF_bst4_cat , RooFit::RecycleConflictNodes());
    if(use_bernstein_ && orders_used.find(4) == orders_used.end())
      ws.import(totPDF_bst5_cat , RooFit::RecycleConflictNodes());
  }
  ws.import(combined_data);
  ws.import(combined_toy_data);

  //import systematics
  fit_systematics(fOut, selection, sets, orders, ws);

  delete fOut;
  // delete f;

  return status;
}
