//Script to fit the MVA background distribution
#include "DataInfo.C"

bool doConstraints_ = false; //adding in systematics
bool includeSignalInFit_ = false; //fit background specturm with signal shape in PDF

Int_t fit_background_MVA_binned(int set = 8, TString selection = "zmutau",
				vector<int> years = {2016, 2017, 2018},
				int seed = 90) {
  int status(0);
  TString hist;
  if     (selection == "hmutau"  ) hist = "mva0";
  else if(selection == "zmutau"  ) hist = "mva1";
  else if(selection == "hetau"   ) hist = "mva2";
  else if(selection == "zetau"   ) hist = "mva3";
  else if(selection == "hemu"    ) hist = "mva4";
  else if(selection == "zemu"    ) hist = "mva5";
  else if(selection == "hmutau_e") hist = "mva6";
  else if(selection == "zmutau_e") hist = "mva7";
  else if(selection == "hetau_mu") hist = "mva8";
  else if(selection == "zetau_mu") hist = "mva9";
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }

  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  if(selec.Contains("_")) selec = "emu";
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("_e", ""); signame.ReplaceAll("_mu",""); signame.ReplaceAll("m", "M");
  signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  int set_offset = ZTauTauHistMaker::kEMu;
  if     (selec == "mutau") set_offset = ZTauTauHistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = ZTauTauHistMaker::kETau;
  
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  
  TString bkg_name = "background_trees/background_ztautau_bkg_nano_" + selec + "_";
  bkg_name += year_string + "_";
  bkg_name += set+set_offset;
  bkg_name += ".tree";
  
  TFile* f_bkg = TFile::Open(bkg_name.Data(), "READ");
  if(!f_bkg) return 1;
  TTree* tree = (TTree*) f_bkg->Get("background_tree");
  if(!tree) return 2;

  std::cout << "---Making MVA histogram\n";
  int nbins = 100;
  TH1F* hmva = new TH1F("hmva", "hmva", nbins, -1, 1.);
  tree->Draw(Form("%s>>hmva", hist.Data()), "fulleventweightlum");
  
  RooRealVar mva(hist.Data(), "MVA Score", 0., -1, 1., "");
  RooDataHist dataset("dataset", "dataset", RooArgList(mva), hmva);
  int ndata = dataset.numEntries();

  /////////////////////
  // Perform the fit //
  /////////////////////
  cout << "---Performing background fit!\n"
       << "Number of events to fit: " << ndata
       << endl;

  //set the random seed for the fitting + generation
  RooRandom::randomGenerator()->SetSeed(seed);


  //Get the signal PDF
  TString nWSSignal = Form("workspaces/fit_signal_mva_%s_%s_%s_%i.root", selection.Data(), hist.Data(), year_string.Data(), set);
  TFile* fWSSignal = TFile::Open(nWSSignal.Data(), "READ");
  if(!fWSSignal) return 1;
  RooWorkspace* ws_signal = (RooWorkspace*) fWSSignal->Get("ws");
  if(!ws_signal) return 2;
  auto sigPDF = ws_signal->pdf("sigpdf");

  //Total PDF
  RooRealVar br_sig("br_sig", "br_sig", 0., -0.01, 0.01);

  //Add log-normal systematics
  //direct systematics on N_sig
  RooRealVar eff_nominal("eff_nominal", "eff_nominal", 1.);
  RooRealVar eff_kappa("eff_kappa", "eff_kappa", 1.03);
  RooRealVar beta_eff("beta_eff", "beta_eff", 0., -10., 10.);
  RooFormulaVar eff("eff", "@0 * pow(@1,@2)", RooArgList(eff_nominal, eff_kappa, beta_eff));
  RooRealVar global_eff("global_eff", "global_eff", 0., -5., 5.);
  RooRealVar one("one", "one", 1.);
  RooGaussian constrain_eff("constrain_eff", "constrain_eff", global_eff, beta_eff, one);
  if(!doConstraints_) beta_eff.setConstant(1);
  global_eff.setConstant(1);

  //Luminosity based parameters
  DataInfo signalInfo(set, selection.Data());
  signalInfo.ReadData();
  CrossSections xs;
  double lum = 0.;
  double eff_signal = 0.;
  for(int year : years) {
    double lum_year = xs.GetLuminosity(year);
    lum += lum_year;
    //get the efficiency of signal for the year, weight by luminosity
    int gen_year = xs.GetGenNumber(signame.Data(), year);
    double rec_year = signalInfo.datamap_[year];
    eff_signal += lum_year * (rec_year / gen_year);
  }
  eff_signal /= lum; //divide by total luminosity
  eff_nominal.setVal(eff_signal);
  cout << "Nominal efficiency = " << eff_signal << endl;
  
  //Get total boson cross section and set total luminosity
  double bxs = xs.GetCrossSection((selection.Contains("z")) ? "Z" : "H");
  RooRealVar lum_var("lum_var", "lum_var", lum);
  RooRealVar bxs_var("bxs_var", "bxs_var", bxs);

  //
  RooFormulaVar n_sig("n_sig", "@0*@1*@2*@3", RooArgList(br_sig, lum_var, bxs_var, eff));
  RooRealVar n_bkg("n_bkg", "n_bkg", 1.e5, 0., 1.e8);

  if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(1);}

  ////////////////////////////////////
  // Alternate Background Functions //
  ////////////////////////////////////

  //exp bkg
  RooRealVar tau_exp1("tau_exp1", "tau_exp1", -0.0386, -10., 0.);
  RooExponential bkgPDF_exp("bkgPDF_exp", "bkgPDF_exp", mva, tau_exp1);

  //power bkg
  RooRealVar a_pow("a_pow", "a_pow",  5.445e-3,   -5.,   5.);
  RooRealVar b_pow("b_pow", "b_pow", -1.229   ,  -10.,  10.);
  RooRealVar c_pow("c_pow", "c_pow", 8.914e1  , -200., 200.);
  RooRealVar d_pow("d_pow", "d_pow", 5.428e1  , -100., 100.);
  RooGenericPdf bkgPDF_pow("bkgPDF_pow","(@1 + (@2 + (@3 + @4/@0)/@0)/@0)/@0", RooArgList(mva,a_pow, b_pow, c_pow, d_pow));

  //chebychev bkg
  RooRealVar a_cheb("a_cheb", "a_cheb", -2.104e1, -100., 100.);
  RooRealVar b_cheb("b_cheb", "b_cheb",-0.290163, -100., 100.);
  RooRealVar c_cheb("c_cheb", "c_cheb", 0.558070, -100., 100.);
  RooRealVar d_cheb("d_cheb", "d_cheb", 0.      , -100., 100.);
  RooChebychev bkgPDF_cheb("bkgPDF_cheb","bkgPDF_cheb", mva, RooArgList(a_cheb, b_cheb, c_cheb, d_cheb));

  //landau bkg
  RooRealVar a_land("a_land", "a_land", 3.975e1 ,   0.,  90.);
  RooRealVar b_land("b_land", "b_land", 7.470e-7,  -5.,   5.);
  RooLandau bkgPDF_land("bkgPDF_land","bkgPDF_land", mva, a_land, b_land);

  //1st order Bernstein
  RooRealVar a_bst2("a_bst2", "a_bst2", 1.491   , -5., 5.);
  RooRealVar b_bst2("b_bst2", "b_bst2", 2.078e-1, -5., 5.);
  RooBernstein bkgPDF_bst2("bkgPDF_bst2", "bkgPDF_bst2", mva, RooArgList(a_bst2, b_bst2));
  
  //2nd order Bernstein
  RooRealVar a_bst3("a_bst3", "a_bst3", 1.491   , -5., 5.);
  RooRealVar b_bst3("b_bst3", "b_bst3", 2.078e-1, -5., 5.);
  RooRealVar c_bst3("c_bst3", "c_bst3", 5.016e-1, -5., 5.);
  RooBernstein bkgPDF_bst3("bkgPDF_bst3", "bkgPDF_bst3", mva, RooArgList(a_bst3, b_bst3, c_bst3));

  //3rd order Bernstein
  RooRealVar a_bst4("a_bst4", "a_bst4", 1.404   , -5., 5.);
  RooRealVar b_bst4("b_bst4", "b_bst4", 2.443e-1, -5., 5.);
  RooRealVar c_bst4("c_bst4", "c_bst4", 5.549e-1, -5., 5.);
  RooRealVar d_bst4("d_bst4", "d_bst4", 3.675e-1, -5., 5.);
  RooBernstein bkgPDF_bst4("bkgPDF_bst4", "Background PDF", mva, RooArgList(a_bst4, b_bst4, c_bst4, d_bst4));

  //4th order Bernstein
  RooRealVar a_bst5("a_bst5", "a_bst5", 1.471   , -5., 5.);
  RooRealVar b_bst5("b_bst5", "b_bst5", 1.452e-1, -5., 5.);
  RooRealVar c_bst5("c_bst5", "c_bst5", 7.524e-1, -5., 5.);
  RooRealVar d_bst5("d_bst5", "d_bst5", 2.696e-1, -5., 5.);
  RooRealVar e_bst5("e_bst5", "e_bst5", 3.687e-1, -5., 5.);
  RooBernstein bkgPDF_bst5("bkgPDF_bst5", "bkgPDF_bst5", mva, RooArgList(a_bst5, b_bst5, c_bst5, d_bst5, e_bst5));

  //Crystall Ball
  RooRealVar m_cbll("m_cbll", "m_cbll", -0.2, -1., 1.);
  RooRealVar s_cbll("s_cbll", "s_cbll", 0.1, 0.01, 5.);
  RooRealVar a_cbll("a_cbll", "a_cbll", 1., 0.1, 10.);
  RooRealVar n_cbll("n_cbll", "n_cbll", 5., 0.1, 30.);
  RooCBShape bkgPDF_cbll("bkgPDF_cbll", "bkgPDF_cbll", mva, m_cbll, s_cbll, a_cbll, n_cbll);

  //Gaussian
  RooRealVar m_gaus("m_gaus", "m_gaus", 0., -1., 1.);
  RooRealVar s_gaus("s_gaus", "s_gaus", 0.1, 0.01, 5.);
  RooGaussian bkgPDF_gaus("bkgPDF_gaus", "bkgPDF_cbll", mva, m_gaus, s_gaus);

  //Crystall Ball + Gaussian
  RooRealVar fracgaus("fracgaus", "fracgaus", 0.2, 0., 1.);  
  RooAddPdf bkgPDF_cblg("bkgPDF_cblg", "bkgPDF_cblg", bkgPDF_cbll, bkgPDF_gaus, fracgaus);

  //Signal + main background PDF
  RooAddPdf totPDF("totPDF", "totPDF", RooArgList(*sigPDF, bkgPDF_cblg), RooArgList(n_sig, n_bkg));
  //PDF with constraints
  RooProdPdf totPDF_constr("totPDF_constr", "totPDF_constr", RooArgList(totPDF, constrain_eff));

  //Signal + Alternate background PDFs
  RooAddPdf totPDF_exp ("totPDF_alt" , "totPDF_alt" , RooArgList(*sigPDF, bkgPDF_exp) , RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_cheb("totPDF_cheb", "totPDF_cheb", RooArgList(*sigPDF, bkgPDF_cheb), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_pow ("totPDF_pow" , "totPDF_pow" , RooArgList(*sigPDF, bkgPDF_pow) , RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_land("totPDF_land", "totPDF_land", RooArgList(*sigPDF, bkgPDF_land), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst2("totPDF_bst2", "totPDF_bst2", RooArgList(*sigPDF, bkgPDF_bst2), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst3("totPDF_bst3", "totPDF_bst3", RooArgList(*sigPDF, bkgPDF_bst3), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst4("totPDF_bst4", "totPDF_bst4", RooArgList(*sigPDF, bkgPDF_bst4), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_bst5("totPDF_bst5", "totPDF_bst5", RooArgList(*sigPDF, bkgPDF_bst5), RooArgList(n_sig, n_bkg));
  RooAddPdf totPDF_cbll("totPDF_cbll", "totPDF_cbll", RooArgList(*sigPDF, bkgPDF_cbll), RooArgList(n_sig, n_bkg));

  /////////////////////
  // Fit backgrounds //
  /////////////////////

  // totPDF_exp.fitTo(dataset);
  // totPDF_bst2.fitTo(dataset);
  // totPDF_bst4.fitTo(dataset);
  // totPDF_cbll.fitTo(dataset);

  if(doConstraints_)
    totPDF_constr.fitTo(dataset, RooFit::Extended(1));
  else
    totPDF.fitTo(dataset, RooFit::Extended(1));
  
  if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(0);}

  //////////////////
  // Plot results //
  //////////////////

  auto xframe = mva.frame();
  dataset.plotOn(xframe);
  if(doConstraints_)
    totPDF_constr.plotOn(xframe);
  else
    totPDF.plotOn(xframe, RooFit::LineColor(kBlue));

  // bkgPDF_bst2.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
  // bkgPDF_bst4.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));  
  // bkgPDF_exp.plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  // bkgPDF_cbll.plotOn(xframe, RooFit::LineColor(kYellow-2), RooFit::LineStyle(kDashed));  
  // bkgPDF_cheb.plotOn(xframe, RooFit::LineColor(kAzure), RooFit::LineStyle(kDashed));
  // bkgPDF_pow.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
  // bkgPDF_land.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
  auto c1 = new TCanvas();
  xframe->Draw();

  //Create the legend
  TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
  leg->AddEntry(xframe->findObject(Form("totPDF_Norm[%s]", hist.Data())), "Crystall Ball + Gaussian", "L");
  // leg->AddEntry(xframe->findObject(Form("bkgPDF_bst2_Norm[%s]", hist.Data())), "Bernstein (1st)", "L");
  // leg->AddEntry(xframe->findObject(Form("bkgPDF_bst4_Norm[%s]", hist.Data())), "Bernstein (3rd)", "L");
  // leg->AddEntry(xframe->findObject(Form("bkgPDF_exp_Norm[%s]", hist.Data())), "Exponential (1st)", "L");
  // leg->AddEntry(xframe->findObject(Form("bkgPDF_cbll_Norm[%s]", hist.Data())), "Crystall Ball", "L");
  leg->Draw();
  
  cout << "Frame contents:\n";
  xframe->Print();

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  if(doConstraints_)
    c1->SaveAs(Form("plots/latest_production/%s/fit_%s_%s_background_binned_constr_%i.png", year_string.Data(), hist.Data(), selection.Data(), set));
  else
    c1->SaveAs(Form("plots/latest_production/%s/fit_%s_%s_background_binned_%i.png", year_string.Data(), hist.Data(), selection.Data(), set));

  //save the workspace
  TFile* fOut = 0;
  if(doConstraints_)
    fOut = new TFile(Form("workspaces/fit_%s_%s_background_binned_constr_%s_%i.root", hist.Data(), selection.Data(), year_string.Data(), set), "RECREATE");
  else
    fOut = new TFile(Form("workspaces/fit_%s_%s_background_binned_%s_%i.root", hist.Data(), selection.Data(), year_string.Data(), set), "RECREATE");
  fOut->cd();
  //Generate toy data
  auto bkg_data = bkgPDF_cblg.generate(RooArgSet(mva), n_bkg.getVal());
  
  RooWorkspace ws("ws");
  if(doConstraints_) {
    ws.import(totPDF_constr);
  } else {
    ws.import(totPDF);
    // ws.import(totPDF_bst2, RooFit::RecycleConflictNodes());
    // ws.import(totPDF_bst4, RooFit::RecycleConflictNodes());
  }
  ws.import(bkgPDF_cblg);
  ws.import(*bkg_data);
  ws.Print();
  ws.Write();
  fOut->Close();
  delete fOut;

  return status;
}
