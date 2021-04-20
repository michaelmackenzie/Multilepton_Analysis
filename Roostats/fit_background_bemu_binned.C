//Script to fit the di-lepton mass histogram for the B->e+mu searches
#include "../interface/SystematicHist_t.hh"

bool doConstraints_ = false; //adding in systematics
bool includeSignalInFit_ = false; //fit background specturm with signal shape in PDF

RooRealVar* br_sig_; //fields common to systematics and nominal PDFs
RooFormulaVar* br_sig_eff_;
RooFormulaVar* n_sig_;
RooRealVar* lepm_;
RooRealVar* bxs_var_;
RooRealVar* lum_var_;
RooRealVar* eff_nominal_;
double      xs_sig_;
RooGaussian* br_sig_constr_;

Int_t fit_systematics(TFile* fInput, TFile* fOut, TString selection, RooWorkspace& ws) {
  bool doHiggs = selection.Contains("h");
  // fInput->cd();
  for(int isys = 0; isys < kMaxSystematics; ++isys) {
    cout << "--- Getting info for systematic " << isys << endl;

    ////////////////////////////////////////////////////////////
    // Create RooFit products for the shifted MC
    ////////////////////////////////////////////////////////////

    TH1D* hbkg = (TH1D*) fInput->Get(Form("hbkg_sys_%i", isys));
    TH1D* hsig = (TH1D*) fInput->Get(Form("%s_sys_%i", selection.Data(), isys));
    if(!hbkg || !hsig) {cout << "Skipping systematic " << isys << endl; continue;}

    RooDataHist bkgData (Form("bkgData_sys_%i", isys) , "Background Data", RooArgList(*lepm_), hbkg );
    RooDataHist sigData (Form("sigData_sys_%i", isys) , "Signal Data"    , RooArgList(*lepm_), hsig );

    ////////////////////////////////////////////////////////////
    // Fit shifted signal PDF
    ////////////////////////////////////////////////////////////

    RooRealVar mean    (Form("mean_sys_%i"     , isys), "mean", (doHiggs) ? 125. : 91., (doHiggs) ? 120. : 85., (doHiggs) ? 130. : 95.);
    RooRealVar sigma   (Form("sigma_sys_%i"    , isys), "sigma", 2., 0.1, 5.);
    RooRealVar alpha   (Form("alpha_sys_%i"    , isys), "alpha", 1., 0.1, 10.);
    RooRealVar enne    (Form("enne_sys_%i"     , isys), "enne",  5., 0.1, 30.);
    RooRealVar mean2   (Form("mean2_sys_%i"    , isys), "mean2", lepm_->getVal(), lepm_->getMin(), lepm_->getMax());
    RooRealVar sigma2  (Form("sigma2_sys_%i"   , isys), "sigma2", 5., 0.1, 10.);
    RooCBShape sigpdf1 (Form("sigpdf1_sys_%i"  , isys), "sigpdf1", *lepm_, mean, sigma, alpha, enne);
    RooGaussian sigpdf2(Form("sigpdf2_sys_%i"  , isys), "sigpdf2", *lepm_, mean2, sigma2);
    RooRealVar fracsig (Form("fracsig_sys_%i"  , isys), "fracsig", 0.7, 0., 1.);
    RooAddPdf sigPDF   (Form("sigPDF_sys_%i"   , isys), "signal PDF", sigpdf1, sigpdf2, fracsig);
    RooRealVar N_sig   (Form("N_sig_sys_%i"    , isys), "N_sig", 2e5, 1e2, 3e6);
    RooAddPdf totsigpdf(Form("totsigpdf_sys_%i", isys), "Signal PDF", RooArgList(sigPDF), RooArgList(N_sig));

    cout << "--- Fitting Systematic signal " << isys << endl;
    totsigpdf.fitTo(sigData, RooFit::PrintLevel(-1));
    fracsig.setConstant(1); mean.setConstant(1); sigma.setConstant(1);
    enne.setConstant(1); alpha.setConstant(1); mean2.setConstant(1); sigma2.setConstant(1);

    ////////////////////////////////////////////////////////////
    // Fit shifted background PDF
    ////////////////////////////////////////////////////////////
    RooRealVar eff_nominal("eff_nominal", "Nominal signal efficiency", hsig->Integral()/(lum_var_->getVal()*xs_sig_), 0., 1.);
    eff_nominal.setConstant(1);
    RooFormulaVar n_sig(Form("n_sig_sys_%i", isys),  "@0*@1*@2*@3", RooArgList(*br_sig_, *lum_var_, *bxs_var_, eff_nominal));
    RooRealVar    n_bkg(Form("n_bkg_sys_%i", isys), "n_bkg", 500., 0., 1.e6);
    if(!includeSignalInFit_) {br_sig_->setVal(0.); br_sig_->setConstant(1);}

    //2nd order Bernstein
    RooRealVar a_bst3(Form("a_bst3_sys_%i", isys), "a_bst3", 1.491   , -5., 5.);
    RooRealVar b_bst3(Form("b_bst3_sys_%i", isys), "b_bst3", 2.078e-1, -5., 5.);
    RooRealVar c_bst3(Form("c_bst3_sys_%i", isys), "c_bst3", 5.016e-1, -5., 5.);
    RooBernstein bkgPDF_bst3(Form("bkgPDF_bst3_sys_%i", isys), "bkgPDF_bst3", *lepm_, RooArgList(a_bst3, b_bst3, c_bst3));

    //3rd order Bernstein
    RooRealVar a_bst4(Form("a_bst4_sys_%i", isys), "a_bst4", 1.404   , -5., 5.);
    RooRealVar b_bst4(Form("b_bst4_sys_%i", isys), "b_bst4", 2.443e-1, -5., 5.);
    RooRealVar c_bst4(Form("c_bst4_sys_%i", isys), "c_bst4", 5.549e-1, -5., 5.);
    RooRealVar d_bst4(Form("d_bst4_sys_%i", isys), "d_bst4", 3.675e-1, -5., 5.);
    RooBernstein bkgPDF_bst4(Form("bkgPDF_bst4_sys_%i", isys), "Background PDF", *lepm_, RooArgList(a_bst4, b_bst4, c_bst4, d_bst4));

    RooAddPdf totPDF(Form("totPDF_sys_%i", isys), "totPDF", RooArgList(sigPDF, (doHiggs) ? bkgPDF_bst3 : bkgPDF_bst4), RooArgList(n_sig, n_bkg));

    cout << "--- Fitting Systematic background " << isys << endl;
    fOut->cd();
    totPDF.fitTo(bkgData, RooFit::SumW2Error(1), RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(0));
    ws.import(totPDF);
  }
  ws.Write();
  fOut->Close();

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
  RooRealVar lepm("lepm", "di-lepton invariant mass",
                  (doHiggs) ? 125. : 91., //nominal
                  (doHiggs) ? 100. : 70. , //lower limit
                  (doHiggs) ? 160. : 110., //upper limit
                  "GeV/c^{2}"); //unit

  //Luminosity based parameters
  CrossSections xs;
  double lum = 0.;
  for(int year : years) {
    double lum_year = xs.GetLuminosity(year);
    lum += lum_year;
  }

  //systematic uncertainty
  double sys_unc = (doHiggs) ? 0. : 0.;

  //Get total boson cross section and set total luminosity
  double bxs = xs.GetCrossSection((selection.Contains("z")) ? "Z" : "H");

  //Parameters common to all categories
  RooRealVar br_sig("br_sig", "br_sig", 0., -0.01, 0.01); br_sig_ = &br_sig;
  RooRealVar lum_var("lum_var", "lum_var", lum); lum_var_ = &lum_var;
  RooRealVar bxs_var("bxs_var", "bxs_var", bxs); bxs_var_ = &bxs_var;
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
  RooCategory categories(selection.Data(), selection.Data());
  map<string, RooDataHist*> dataCategoryMap; //for creating a toy data sample
  vector<RooFormulaVar*> n_sigs;
  vector<RooRealVar*> n_bkgs;
  vector<RooAbsPdf*> totPDFs;
  vector<RooAbsPdf*> bkgPDFs;
  ////////////////////////////////////////////////////////////
  // Read in the data
  ////////////////////////////////////////////////////////////

  //save the workspace
  TFile* fOut = new TFile(Form("workspaces/fit_%s_lepm_background_binned_%s_%s.root", selection.Data(), year_string.Data(), set_str.Data()), "RECREATE");
  RooWorkspace ws("ws");
  int index = 0;
  for(int set : sets) {
    cout << "Reading in the data information for set " << set << endl;
    TFile* f = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "READ");
    if(!f) return 1 + 100*set;


    TH1D* hbkg  = (TH1D*) f->Get("hbackground");
    TH1D* hsig  = (TH1D*) f->Get(selection.Data());
    TH1D* hdata = (TH1D*) f->Get("hdata");
    if(!hbkg || !hsig || !hdata) {
      cout << "Not all base histograms found!\n";
      return 2 + 100*set;
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


    RooRealVar* eff_nominal = new RooRealVar(Form("eff_nominal_%i", set), "Nominal signal efficiency", hsig->Integral()/(lum*xs_sig_), 0., 1.);
    eff_nominal->setConstant(1);

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
    RooGaussian* sigpdf2 = new RooGaussian(Form("sigpdf2_%i"  , set), "sigpdf2", lepm, *mean2, *sigma2);
    RooRealVar* fracsig  = new RooRealVar(Form("fracsig_%i"  , set), "fracsig", 0.7, 0., 1.);
    RooAddPdf* sigPDF    = new RooAddPdf(Form("sigPDF_%i"   , set), "signal PDF", *sigpdf1, *sigpdf2, *fracsig);
    RooRealVar* N_sig    = new RooRealVar(Form("N_sig_%i"    , set), "N_sig", 2e5, 1e2, 3e6);
    RooAddPdf* totsigpdf = new RooAddPdf(Form("totSigPDF_%i", set), "Signal PDF", RooArgList(*sigPDF), RooArgList(*N_sig));

    totsigpdf->fitTo(sigData, RooFit::SumW2Error(1));
    fracsig->setConstant(1); mean->setConstant(1); sigma->setConstant(1);
    enne->setConstant(1); alpha->setConstant(1); mean2->setConstant(1); sigma2->setConstant(1);

    ////////////////////////////////////////////////////////////
    // Fit background distributions
    ////////////////////////////////////////////////////////////

    cout << "---Performing background fit for set " << set << endl
         << "---Number of background events to fit: " << bkgData.numEntries() << endl;

    //Total PDF parameters
    RooFormulaVar* n_sig = new RooFormulaVar(Form("n_sig_%i", set),  "@0*@1*@2*@3", RooArgList(br_sig_eff, lum_var, bxs_var, *eff_nominal));
    RooRealVar*    n_bkg = new RooRealVar   (Form("n_bkg_%i", set), "n_bkg", 500., 0., 1.e6);
    n_sigs.push_back(n_sig);
    n_bkgs.push_back(n_bkg);
    if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(1);}


    ////////////////////////////////////
    // Alternate Background Functions //
    ////////////////////////////////////

    //exp bkg
    RooRealVar     tau_exp1  (Form("tau_exp1_%i"  , set), "tau_exp1", -0.0386, -10., 0.);
    RooExponential bkgPDF_exp(Form("bkgPDF_exp_%i", set), "bkgPDF_exp", lepm, tau_exp1);

    //power bkg
    RooRealVar a_pow(Form("a_pow_%i", set), "a_pow",  5.445e-3,   -5.,   5.);
    RooRealVar b_pow(Form("b_pow_%i", set), "b_pow", -1.229   ,  -10.,  10.);
    RooRealVar c_pow(Form("c_pow_%i", set), "c_pow", 8.914e1  , -200., 200.);
    RooRealVar d_pow(Form("d_pow_%i", set), "d_pow", 5.428e1  , -100., 100.);
    RooGenericPdf bkgPDF_pow(Form("bkgPDF_pow_%i", set), "(@1 + (@2 + (@3 + @4/@0)/@0)/@0)/@0", RooArgList(lepm,a_pow, b_pow, c_pow, d_pow));

    //chebychev bkg
    RooRealVar a_cheb(Form("a_cheb_%i", set), "a_cheb", -2.104e1, -100., 100.);
    RooRealVar b_cheb(Form("b_cheb_%i", set), "b_cheb",-0.290163, -100., 100.);
    RooRealVar c_cheb(Form("c_cheb_%i", set), "c_cheb", 0.558070, -100., 100.);
    RooRealVar d_cheb(Form("d_cheb_%i", set), "d_cheb", 0.      , -100., 100.);
    RooChebychev bkgPDF_cheb(Form("bkgPDF_cheb_%i", set) ,"bkgPDF_cheb", lepm, RooArgList(a_cheb, b_cheb, c_cheb, d_cheb));

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

    //Signal + main background PDF
    RooAddPdf* totPDF = new RooAddPdf(Form("totPDF_%i", set), "totPDF", RooArgList(*sigPDF, (doHiggs) ? *bkgPDF_bst3 : *bkgPDF_bst4), RooArgList(*n_sig, *n_bkg));

    //Signal + Alternate background PDFs
    // RooAddPdf totPDF_exp (Form("totPDF_alt_%i" , set), "totPDF_alt" , RooArgList(*sigPDF, *bkgPDF_exp) , RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_cheb(Form("totPDF_cheb_%i", set), "totPDF_cheb", RooArgList(*sigPDF, *bkgPDF_cheb), RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_pow (Form("totPDF_pow_%i" , set), "totPDF_pow" , RooArgList(*sigPDF, *bkgPDF_pow) , RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_land(Form("totPDF_land_%i", set), "totPDF_land", RooArgList(*sigPDF, *bkgPDF_land), RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_bst2(Form("totPDF_bst2_%i", set), "totPDF_bst2", RooArgList(*sigPDF, *bkgPDF_bst2), RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_bst3(Form("totPDF_bst3_%i", set), "totPDF_bst3", RooArgList(*sigPDF, *bkgPDF_bst3), RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_bst4(Form("totPDF_bst4_%i", set), "totPDF_bst4", RooArgList(*sigPDF, *bkgPDF_bst4), RooArgList(*n_sig, *n_bkg));
    // RooAddPdf totPDF_bst5(Form("totPDF_bst5_%i", set), "totPDF_bst5", RooArgList(*sigPDF, *bkgPDF_bst5), RooArgList(*n_sig, *n_bkg));

    //PDFs with constraints
    RooProdPdf* totPDF_constr      = new RooProdPdf(Form("totPDF_constr_%i"     , set), "totPDF_constr"     , RooArgList(*totPDF     , br_sig_constr));
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
    if(doConstraints_) {
      // totPDF_constr_exp .fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_cheb.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_pow .fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_land.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_bst2.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_bst3.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_bst4.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_constr_bst5.fitTo(bkgData, RooFit::SumW2Error(1));
      totPDF_constr     ->fitTo(bkgData, RooFit::SumW2Error(1), RooFit::Extended(1));
      totPDFs.push_back(totPDF_constr);
    } else {
      // totPDF_exp .fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_cheb.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_pow .fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_land.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_bst2.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_bst3.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_bst4.fitTo(bkgData, RooFit::SumW2Error(1));
      // totPDF_bst5.fitTo(bkgData, RooFit::SumW2Error(1));
      totPDF     ->fitTo(bkgData, RooFit::SumW2Error(1), RooFit::Extended(1));
      totPDFs.push_back(totPDF);
    }
    RooDataHist* gen_data = totPDF->generateBinned(RooArgSet(lepm), hbkg->Integral());
    dataCategoryMap[category] = gen_data;

    ////////////////////////////////////////////////////////////
    // Plot the results
    ////////////////////////////////////////////////////////////

    if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(0);}
    auto xframe = lepm.frame();
    bkgData.plotOn(xframe);
    if(doConstraints_)
      totPDF_constr->plotOn(xframe, RooFit::LineColor(kBlue));
    else
      totPDF->plotOn(xframe, RooFit::LineColor(kBlue));
    // if(!doHiggs) {
    //   bkgPDF_bst3.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
    //   bkgPDF_bst5.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
    // } else {
    //   bkgPDF_bst2.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
    //   bkgPDF_bst4.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
    // }

    // bkgPDF_exp.plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    // bkgPDF_cheb.plotOn(xframe, RooFit::LineColor(kAzure), RooFit::LineStyle(kDashed));
    // bkgPDF_pow.plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
    // bkgPDF_land.plotOn(xframe, RooFit::LineColor(kViolet-2), RooFit::LineStyle(kDashed));
    auto c1 = new TCanvas(Form("c_%i", set), Form("c_%i", set));
    xframe->Draw();
    TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
    //ones not drawn won't appear in the legend
    leg->AddEntry(xframe->findObject("totPDF_Norm[lepm]")     , (doHiggs) ? "Bernstein (2nd)" : "Bernstein (3rd)"  , "L");

    // if(!doHiggs) {
    //   leg->AddEntry(xframe->findObject("bkgPDF_bst3_Norm[lepm]"), "Bernstein (2nd)"  , "L");
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

    c1->SaveAs(Form("plots/latest_production/%s/fit_%s_lepm_binned_background_%s_cat_%i.png", year_string.Data(), selection.Data(), set_str.Data(), index));

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

  // //import systematics
  // fit_systematics(f, fOut, selection, ws);
    ++index;
  }
  cout << "--- Finished getting data for each category! Creating simultaneous PDF\n";
  RooSimultaneous totPDF_cat("totPDF", "The Total PDF", categories);
  for(unsigned i = 0; i < totPDFs.size(); ++i) {
    totPDF_cat.addPdf(*totPDFs[i], Form("%s_%i", selection.Data(), i));
  }

  cout << "Total combined PDF defined!\n";

  RooDataHist combined_data("combined_data", "combined_data", lepm, categories, dataCategoryMap);

  cout << "Combined data defined!\n";

  fOut->cd();
  ws.import(totPDF_cat);
  ws.import(combined_data);
  ws.Print();
  ws.Write();

  delete fOut;
  // delete f;

  return status;
}
