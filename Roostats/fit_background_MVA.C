//Script to fit the MVA background distribution
#include "DataInfo.C"

bool doConstraints_ = false; //adding in systematics
bool includeSignalInFit_ = false; //fit background specturm with signal shape in PDF

Int_t fit_background_MVA(int set = 8, TString selection = "zmutau",
                         vector<int> years = {2016/*, 2017, 2018*/},
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
  int set_offset = CLFVHistMaker::kEMu;
  if     (selec == "mutau") set_offset = CLFVHistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = CLFVHistMaker::kETau;

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  TString bkg_name = "background_trees/background_clfv_bkg_nano_" + selec + "_";
  bkg_name += year_string + "_";
  bkg_name += set+set_offset;
  bkg_name += ".tree";

  TFile* f_bkg = TFile::Open(bkg_name.Data(), "READ");
  if(!f_bkg) return 1;
  TTree* tree = (TTree*) f_bkg->Get("background_tree");
  if(!tree) return 2;

  //Get the signal PDF
  TString nWSSignal = Form("workspaces/hist_signal_mva_%s_%s_%s_%i.root", selection.Data(), hist.Data(), year_string.Data(), set);
  TFile* fWSSignal = TFile::Open(nWSSignal.Data(), "READ");
  if(!fWSSignal) return 1;
  RooWorkspace* ws_signal = (RooWorkspace*) fWSSignal->Get("ws");
  if(!ws_signal) return 2;
  auto sigMVAPDF = ws_signal->pdf("sigMVAPDF");

  std::cout << "All file elements retrieved!\n";

  //create a histogram for the PDF
  const int nbins = 40;
  const double xmin = -1.;
  const double xmax =  1.;
  TH1D* hmva = new TH1D("hmva", "hmva", nbins, xmin, xmax);
  tree->Draw((hist+">>hmva").Data(), "fulleventweightlum");
  RooRealVar mva(hist.Data(),"MVA score", xmin, xmax);
  RooDataHist dataMVA("bkgMVA", "Background MVA Data", RooArgList(mva), hmva);
  RooHistPdf bkgMVAPDF("bkgMVAPDF", "Background MVA PDF", RooArgSet(mva), dataMVA);

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

  //Signal + main background PDF
  RooAddPdf totMVAPDF("totMVAPDF", "totMVAPDF", RooArgList(*sigMVAPDF, bkgMVAPDF), RooArgList(n_sig, n_bkg));
  //PDF with constraints
  RooProdPdf totMVAPDF_constr("totMVAPDF_constr", "totMVAPDF_constr", RooArgList(totMVAPDF, constrain_eff));

  auto bkg_data = bkgMVAPDF.generate(RooArgSet(mva), n_bkg.getVal());
  bkg_data->SetName("bkgMVAPDFData");

  //Plot PDF + toy data
  br_sig.setVal(xs.GetCrossSection(signame.Data())/xs.GetCrossSection("Z"));
  auto xframe = mva.frame();
  bkg_data->plotOn(xframe);
  totMVAPDF.plotOn(xframe);
  totMVAPDF.plotOn(xframe, RooFit::Components("bkgMVAPDF"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  totMVAPDF.plotOn(xframe, RooFit::Components("sigMVAPDF"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));


  auto c1 = new TCanvas();
  xframe->Draw();

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  c1->SaveAs(Form("plots/latest_production/%s/hist_background_mva_%s_%s_%i.png", year_string.Data(), selection.Data(), hist.Data(), set));
  TFile* fOut = new TFile(Form("workspaces/hist_background_mva_%s_%s_%s_%i.root", selection.Data(), hist.Data(), year_string.Data(), set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(totMVAPDF);
  // ws.import(totMVAPDF_constr);
  ws.import(bkgMVAPDF);
  // ws.import(*sigMVAPDF);
  ws.import(*bkg_data);
  ws.Write();
  fOut->Close();
  delete fOut;

  return status;
}
