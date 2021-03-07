//Script to fit the di-lepton mass histogram for same flavor selections
#include "DataInfo.C"

bool doConstraints_ = false; //adding in systematics
bool includeSignalInFit_ = false; //fit background specturm with signal shape in PDF
bool useSameFlavorCount_ = false; //use N(sig) ~ br_emu*eff*sqrt(N_ee*N_mumu)/br_ll
bool useMorphedPDF_ = false; //use signal PDF from morphing mumu and ee data fits
bool useExp_ = false; //use exp background func as alt bkg PDF

Int_t do_fit(TTree* tree, int set, vector<int> years, bool doHiggs, int seed) {
  //set the random seed for the fitting + generation
  RooRandom::randomGenerator()->SetSeed(seed);
  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  RooRealVar lepm("lepm", "di-lepton invariant mass",
                  (doHiggs) ? 160. : 110.,
                  (doHiggs) ? 100. : 75. ,
                  (doHiggs) ? 160. : 110.,
                  "GeV/c^{2}");
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
  TString nWSSignal;
  if(useMorphedPDF_ && !doHiggs) nWSSignal = Form("workspaces/morphed_signal_%s_%i.root", year_string.Data(), set);
  else                           nWSSignal = Form("workspaces/fit_signal_%s_lepm_%s_%i.root", ((doHiggs) ? "hemu" : "zemu"),
                                                  year_string.Data(), set);
  TFile* fWSSignal = TFile::Open(nWSSignal.Data(), "READ");
  if(!fWSSignal) return 1;
  RooWorkspace* ws_signal = (RooWorkspace*) fWSSignal->Get("ws");
  if(!ws_signal) return 2;
  auto sigPDF = ws_signal->pdf((useMorphedPDF_) ? "morph_pdf_binned" : "sigpdf");

  //Total PDF
  RooRealVar br_emu("br_emu", "br_emu", 0., (doHiggs) ? -0.01 : -1e-4, (doHiggs) ? 0.01 : 1e-4);
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
  DataInfo signalInfo(set, (doHiggs) ? "hemu" : "zemu");
  signalInfo.ReadData();
  CrossSections xs;
  double lum = 0.;
  double eff_signal = 0.;
  for(int year : years) {
    double lum_year = xs.GetLuminosity(year);
    lum += lum_year;
    //get the efficiency of signal for the year, weight by luminosity
    int gen_year = xs.GetGenNumber((doHiggs) ? "HEMu" : "ZEMu", year);
    double rec_year = signalInfo.datamap_[year];
    eff_signal += lum_year * (rec_year / gen_year);
  }
  eff_signal /= lum; //divide by total luminosity
  if(!useSameFlavorCount_ || doHiggs)
    eff_nominal.setVal(eff_signal);

  double bxs = xs.GetCrossSection((doHiggs) ? "H" : "Z");
  RooRealVar lum_var("lum_var", "lum_var", lum);
  RooRealVar bxs_var("bxs_var", "bxs_var", bxs);

  RooFormulaVar n_sig("n_sig", ((useSameFlavorCount_&&!doHiggs) ? "@0*@4*sqrt((@1*@2)/(@3*@3))" : "@0*@1*@2*@3")
                      , ((useSameFlavorCount_&&!doHiggs) ? RooArgList(br_emu, n_electron_var, n_muon_var,br_ll,eff) :
                         RooArgList(br_emu, lum_var, bxs_var, eff))
                      );
  RooRealVar n_bkg("n_bkg", "n_bkg", 500., 0., 1.e6);

  if(!includeSignalInFit_) {br_emu.setVal(0.); br_emu.setConstant(1);}


  ////////////////////////////////////
  // Alternate Background Functions //
  ////////////////////////////////////

  //exp bkg
  RooRealVar tau_exp1("tau_exp1", "tau_exp1", -0.0386, -10., 0.);
  RooExponential bkgPDF_exp("bkgPDF_exp", "bkgPDF_exp", lepm, tau_exp1);

  //power bkg
  RooRealVar a_pow("a_pow", "a_pow",  5.445e-3,   -5.,   5.);
  RooRealVar b_pow("b_pow", "b_pow", -1.229   ,  -10.,  10.);
  RooRealVar c_pow("c_pow", "c_pow", 8.914e1  , -200., 200.);
  RooRealVar d_pow("d_pow", "d_pow", 5.428e1  , -100., 100.);
  RooGenericPdf bkgPDF_pow("bkgPDF_pow","(@1 + (@2 + (@3 + @4/@0)/@0)/@0)/@0", RooArgList(lepm,a_pow, b_pow, c_pow, d_pow));

  //chebychev bkg
  RooRealVar a_cheb("a_cheb", "a_cheb", -2.104e1, -100., 100.);
  RooRealVar b_cheb("b_cheb", "b_cheb",-0.290163, -100., 100.);
  RooRealVar c_cheb("c_cheb", "c_cheb", 0.558070, -100., 100.);
  RooRealVar d_cheb("d_cheb", "d_cheb", 0.      , -100., 100.);
  RooChebychev bkgPDF_cheb("bkgPDF_cheb","bkgPDF_cheb", lepm, RooArgList(a_cheb, b_cheb, c_cheb, d_cheb));

  //landau bkg
  RooRealVar a_land("a_land", "a_land", 3.975e1 ,   0.,  90.);
  RooRealVar b_land("b_land", "b_land", 7.470e-7,  -5.,   5.);
  RooLandau bkgPDF_land("bkgPDF_land","bkgPDF_land", lepm, a_land, b_land);

  //1st order Bernstein
  RooRealVar a_bst2("a_bst2", "a_bst2", 1.491   , -5., 5.);
  RooRealVar b_bst2("b_bst2", "b_bst2", 2.078e-1, -5., 5.);
  RooBernstein bkgPDF_bst2("bkgPDF_bst2", "bkgPDF_bst2", lepm, RooArgList(a_bst2, b_bst2));

  //2nd order Bernstein
  RooRealVar a_bst3("a_bst3", "a_bst3", 1.491   , -5., 5.);
  RooRealVar b_bst3("b_bst3", "b_bst3", 2.078e-1, -5., 5.);
  RooRealVar c_bst3("c_bst3", "c_bst3", 5.016e-1, -5., 5.);
  RooBernstein bkgPDF_bst3("bkgPDF_bst3", "bkgPDF_bst3", lepm, RooArgList(a_bst3, b_bst3, c_bst3));

  //3rd order Bernstein
  RooRealVar a_bst4("a_bst4", "a_bst4", 1.404   , -5., 5.);
  RooRealVar b_bst4("b_bst4", "b_bst4", 2.443e-1, -5., 5.);
  RooRealVar c_bst4("c_bst4", "c_bst4", 5.549e-1, -5., 5.);
  RooRealVar d_bst4("d_bst4", "d_bst4", 3.675e-1, -5., 5.);
  RooBernstein bkgPDF_bst4("bkgPDF_bst4", "Background PDF", lepm, RooArgList(a_bst4, b_bst4, c_bst4, d_bst4));

  //4th order Bernstein
  RooRealVar a_bst5("a_bst5", "a_bst5", 1.471   , -5., 5.);
  RooRealVar b_bst5("b_bst5", "b_bst5", 1.452e-1, -5., 5.);
  RooRealVar c_bst5("c_bst5", "c_bst5", 7.524e-1, -5., 5.);
  RooRealVar d_bst5("d_bst5", "d_bst5", 2.696e-1, -5., 5.);
  RooRealVar e_bst5("e_bst5", "e_bst5", 3.687e-1, -5., 5.);
  RooBernstein bkgPDF_bst5("bkgPDF_bst5", "bkgPDF_bst5", lepm, RooArgList(a_bst5, b_bst5, c_bst5, d_bst5, e_bst5));

  //Signal + main background PDF
  RooAddPdf totPDF("totPDF", "totPDF", RooArgList(*sigPDF, (doHiggs) ? bkgPDF_bst3 : bkgPDF_bst4), RooArgList(n_sig, n_bkg));
  //PDF with constraints
  RooProdPdf totPDF_constr("totPDF_constr", "totPDF_constr", RooArgList(totPDF, constrain_eff, constr_n_muon, constr_n_electron));

  //Signal + Alternate background PDFs
  RooAddPdf totPDF_exp ("totPDF_alt" , "totPDF_alt" , RooArgList(*sigPDF, bkgPDF_exp) , RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_cheb("totPDF_cheb", "totPDF_cheb", RooArgList(*sigPDF, bkgPDF_cheb), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_pow ("totPDF_pow" , "totPDF_pow" , RooArgList(*sigPDF, bkgPDF_pow) , RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_land("totPDF_land", "totPDF_land", RooArgList(*sigPDF, bkgPDF_land), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst2("totPDF_bst2", "totPDF_bst2", RooArgList(*sigPDF, bkgPDF_bst2), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst3("totPDF_bst3", "totPDF_bst3", RooArgList(*sigPDF, bkgPDF_bst3), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst4("totPDF_bst4", "totPDF_bst4", RooArgList(*sigPDF, bkgPDF_bst4), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst5("totPDF_bst5", "totPDF_bst5", RooArgList(*sigPDF, bkgPDF_bst5), RooArgList(n_sig, n_bkg));

  //PDF with constraint
  RooProdPdf totPDF_constr_exp("totPDF_constr_alt", "totPDF_constr_alt", RooArgList(totPDF_exp, constrain_eff, constr_n_muon, constr_n_electron));
  if(doConstraints_)
    totPDF_constr_exp.fitTo(dataset);
  else
    totPDF_exp.fitTo(dataset);
  // totPDF_cheb.fitTo(dataset);
  // totPDF_pow.fitTo(dataset);
  // totPDF_land.fitTo(dataset);
  if(doHiggs)  totPDF_bst2.fitTo(dataset);
  if(doHiggs)  totPDF_bst4.fitTo(dataset);
  if(!doHiggs) totPDF_bst3.fitTo(dataset);
  if(!doHiggs) totPDF_bst5.fitTo(dataset);

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
    totPDF.plotOn(xframe, RooFit::LineColor(kBlue));
  if(!doHiggs) {
    bkgPDF_bst3.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
    bkgPDF_bst5.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
  } else {
    bkgPDF_bst2.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
    bkgPDF_bst4.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
  }

  bkgPDF_exp.plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  // bkgPDF_cheb.plotOn(xframe, RooFit::LineColor(kAzure), RooFit::LineStyle(kDashed));
  // bkgPDF_pow.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
  // bkgPDF_land.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
  auto c1 = new TCanvas();
  xframe->Draw();
  TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
  //ones not drawn won't appear in the legend
  leg->AddEntry(xframe->findObject("totPDF_Norm[lepm]")     , (doHiggs) ? "Bernstein (2nd)" : "Bernstein (3rd)"  , "L");

  if(!doHiggs) {
    leg->AddEntry(xframe->findObject("bkgPDF_bst3_Norm[lepm]"), "Bernstein (2nd)"  , "L");
    leg->AddEntry(xframe->findObject("bkgPDF_bst5_Norm[lepm]"), "Bernstein (4th)"  , "L");
  }
  if(doHiggs) {
    leg->AddEntry(xframe->findObject("bkgPDF_bst2_Norm[lepm]"), "Bernstein (1st)"  , "L");
    leg->AddEntry(xframe->findObject("bkgPDF_bst4_Norm[lepm]"), "Bernstein (3rd)"  , "L");
  }
  leg->AddEntry(xframe->findObject("bkgPDF_exp_Norm[lepm]") , "Exponential", "L");
  // leg->AddEntry(xframe->findObject("bkgPDF_pow_Norm[lepm]") , "Power"      , "L");
  // leg->AddEntry(xframe->findObject("bkgPDF_land_Norm[lepm]"), "Landau"     , "L");
  leg->Draw();

  cout << "***Frame chi squared values:\n";
  vector<pair<TString, int>> bkg_funcs = {pair<TString, int>("totPDF_Norm[lepm]", (doHiggs) ? 4 : 5),
                                          pair<TString, int>("bkgPDF_exp_Norm[lepm]", 2),
                                          pair<TString, int>("bkgPDF_bst2_Norm[lepm]", 3),
                                          pair<TString, int>("bkgPDF_bst3_Norm[lepm]", 4),
                                          pair<TString, int>("bkgPDF_bst4_Norm[lepm]", 5),
                                          pair<TString, int>("bkgPDF_bst5_Norm[lepm]", 6),
                                          pair<TString, int>("bkgPDF_land_Norm[lepm]", 3)};
  for(int index = 0; index < bkg_funcs.size(); ++index) {
    TString bkg_name = bkg_funcs[index].first;
    // if(doHiggs  && bkg_name.Contains("bst5")) continue;
    // if(!doHiggs && bkg_name.Contains("bst2")) continue;
    int     bkg_dof  = bkg_funcs[index].second;
    //print chi squared / (n(bins) - n(params)), as can't just ask for chi squared and n bins separately easily...
    cout << " " << bkg_name.Data() << ": " << xframe->chiSquare(bkg_name.Data(), "h_dataset", bkg_dof) << endl;
  }
  cout << "Frame contents:\n";
  xframe->Print();

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  if(doConstraints_)
    c1->SaveAs(Form("plots/latest_production/%s/fit_%s_lepm_background_constr_%i.png", year_string.Data(), (doHiggs) ? "hemu" : "zemu", set));
  else
    c1->SaveAs(Form("plots/latest_production/%s/fit_%s_lepm_background_%i.png", year_string.Data(), (doHiggs) ? "hemu" : "zemu", set));

  //save the workspace
  TFile* fOut = 0;
  if(doConstraints_)
    fOut = new TFile(Form("workspaces/fit_%s_lepm_background_constr_%s_%i.root", (doHiggs) ? "hemu" : "zemu",
                          year_string.Data(), set), "RECREATE");
  else
    fOut = new TFile(Form("workspaces/fit_%s_lepm_background_%s_%i.root", (doHiggs) ? "hemu" : "zemu",
                          year_string.Data(), set), "RECREATE");
  fOut->cd();
  auto bkg_data = (doHiggs) ? bkgPDF_bst3.generate(RooArgSet(lepm), n_bkg.getVal()) : bkgPDF_bst4.generate(RooArgSet(lepm), n_bkg.getVal());

  RooWorkspace ws("ws");
  if(doConstraints_) {
    ws.import(totPDF_constr);
    ws.import(totPDF_constr_exp, RooFit::RecycleConflictNodes());
  } else {
    ws.import(totPDF);
    if(!doHiggs) {
      ws.import(totPDF_bst5, RooFit::RecycleConflictNodes());
      ws.import(totPDF_bst3, RooFit::RecycleConflictNodes());
    } else {
      ws.import(totPDF_bst2, RooFit::RecycleConflictNodes());
      ws.import(totPDF_bst4, RooFit::RecycleConflictNodes());
    }
      // ws.import(totPDF_exp, RooFit::RecycleConflictNodes());
  }
  if(doHiggs)
    ws.import(bkgPDF_bst3);
  else
    ws.import(bkgPDF_bst4);
  ws.import(*bkg_data);
  ws.Print();
  ws.Write();
  fOut->Close();
  delete fOut;
  return 0;
}

Int_t fit_background(int set = 8, vector<int> years = {2016, 2017, 2018},
                     bool doHiggs = false, int seed = 90) {
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
  TTree* t_bkg = (TTree*) f_bkg->Get("background_tree");
  if(!t_bkg) return 2;
  std::cout << "---Performing background fit!\n";
  status = do_fit(t_bkg, set, years, doHiggs, seed);
  if(status) std::cout << "Fit returned status " << status << std::endl;
  return status;
}
