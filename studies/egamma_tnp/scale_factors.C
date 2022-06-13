//Script to generate Tag and Probe (TnP) electron trigger scale factors
bool useAbsEta_ = true; //use |eta| or signed eta for scales

//Fit the mass distribution with a signal and background function
double extract_nz(TH1* hMass, double& nerror, int BkgMode, TString figname = "", TString figtitle = "") {

  const double min_mass(70.), max_mass(115.);

  //Create the observable
  RooRealVar mass("mass", "Dilepton mass", 91., min_mass, max_mass, "GeV/c^{2}");
  mass.setBins((max_mass-min_mass)/hMass->GetBinWidth(1));

  //Build the signal PDF
  // RooRealVar mean ("mean" , "mean" , 91., 85., 95.); //central RooCBShape
  // RooRealVar width("width", "width", 2.5, 0.1, 5. );
  // RooRealVar sigma("sigma", "sigma", 2. , 0.1, 5. );
  // RooRealVar alpha("alpha", "alpha", 1. , 0.1, 10.);
  // RooRealVar enne ("enne" , "enne" , 5. , 0.01, 30.);

  // RooRealVar mean2 ("mean2" , "mean2" , 91., 70., 100.); //additional Gaussian
  // RooRealVar sigma2("sigma2", "sigma2", 5. , 0.1, 10. );

  // RooCBShape  sigpdf1("sigpdf1", "sigpdf1", mass, mean, sigma, alpha, enne);
  // RooGaussian sigpdf2("sigpdf2", "sigpdf2", mass, mean2, sigma2);
  // RooRealVar  fracsig("fracsig", "fracsig", 0.7, 0., 1.);
  // RooAddPdf   sigpdf ("sigpdf" , "sigpdf" , sigpdf1, sigpdf2, fracsig);

  //DoubleCB signal
  RooRealVar     mean  ("mean"  , "mean"  , 90., 85., 95.); //central RooCBShape
  RooRealVar     sigma ("sigma" , "sigma" , 2.2, 1.0, 6.);
  RooRealVar     alpha1("alpha1", "alpha1", 1. , 0.5, 5.);
  RooRealVar     alpha2("alpha2", "alpha2", 2. , 0.5, 5.);
  RooRealVar     enne1 ("enne1" , "enne1" , 5. , 4.0, 30.);
  RooRealVar     enne2 ("enne2" , "enne2" , 8.8, 4.0, 30.);
  RooDoubleCrystalBall sigpdf("sigpdf", "sigpdf", mass, mean, sigma, alpha1, enne1, alpha2, enne2);

  //Build the background PDF
  RooAbsPdf* bkgpdf;
  vector<RooRealVar*> bkgvars;
  if(BkgMode == 0 || hMass->Integral() < 1000) { //exponential PDF
    RooRealVar* tau = new RooRealVar("tau", "tau", -1., -10., 1.); bkgvars.push_back(tau);
    bkgpdf = new RooExponential("bkgpdf", "bkgpdf", mass, *tau);
  } else if(BkgMode == 1) { //Bernstein polynomial
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 0.1, -2., 2.);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 0.1, -2., 2.);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 0. , -2., 2.);

    bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg));
  } else if(BkgMode == 2) { //Bernstein polynomial
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 2.e-3, -2., 2.); bkgvars.push_back(a_bkg);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 7.e-3, -2., 2.); bkgvars.push_back(b_bkg);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 5.e-3, -2., 2.); bkgvars.push_back(c_bkg);
    RooRealVar* d_bkg = new RooRealVar("d_bkg", "d_bkg", 3.e-3, -2., 2.); bkgvars.push_back(d_bkg);
    // bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg));
    bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg, *d_bkg));
  } else if(BkgMode == 3) { //Chebychev polynomial
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 2.e-3, -2., 2.); bkgvars.push_back(a_bkg);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 3.e-2, -2., 2.); bkgvars.push_back(b_bkg);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 1.e-2, -2., 2.); bkgvars.push_back(c_bkg);
    RooRealVar* d_bkg = new RooRealVar("d_bkg", "d_bkg", 1.e-2, -2., 2.); bkgvars.push_back(d_bkg);
    bkgpdf = new RooChebychev("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg, *d_bkg));
  // } else if(BkgMode == 4) { //two exponential PDFs
  //   RooRealVar* tau_1 = new RooRealVar("tau_1", "tau_2", -1., -10., 1.); bkgvars.push_back(tau_1);
  //   RooRealVar* tau_2 = new RooRealVar("tau_2", "tau_2", -1., -10., 1.); bkgvars.push_back(tau_2);
  //   bkgpdf = new RooExponential("bkgpdf", "bkgpdf", mass, *tau_1);
  }
  // if(BkgMode == 0) { //exponential PDF
  //   RooRealVar* tau = new RooRealVar("tau", "tau", 1., 0., 10.);
  //   bkgpdf = new RooExponential("bkgpdf", "bkgpdf", mass, *tau);
  // }

  const double rough_sig_frac = hMass->Integral(hMass->FindBin(85.), hMass->FindBin(95.)) / hMass->Integral();
  RooRealVar N_sig("N_sig", "N_sig", hMass->Integral()*rough_sig_frac*1.1     , hMass->Integral()/200., hMass->Integral()*3.);
  RooRealVar N_bkg("N_bkg", "N_bkg", hMass->Integral()*(1.-rough_sig_frac)*1.1, 0.                    , hMass->Integral()*3.);

  RooAddPdf totpdf("totpdf", "Total PDF", RooArgList(sigpdf,*bkgpdf), RooArgList(N_sig,N_bkg));

  //Create a data object to fit
  RooDataHist data("data",  "data", RooArgList(mass), hMass);

  //Perform the fit
  bool refit = false;
  int count = 0;
  do {
    auto fitres = totpdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::PrintLevel(-1), RooFit::Save());
    refit = fitres != 0 && fitres->status() != 0 && count < 2;
    ++count;
    if(refit) cout << endl << "Refitting the dataset!\n";
  } while(refit);
  double nsig = N_sig.getVal();
  nerror = N_sig.getError();
  if(nsig < 0.) cout << "N(sig) < 0 for " << figname.Data() << ", " << figtitle.Data() << endl;
  nsig = std::max(nsig, 0.);

  //ensure a somewhat reasonable error
  if(nerror > 3.*sqrt(nsig) && nerror > 3.*sqrt(rough_sig_frac*hMass->Integral()))
    nerror = 1.2*sqrt(std::max(nsig, rough_sig_frac*hMass->Integral()));

  //Draw the fit and write the figure to disk
  auto xframe = mass.frame(RooFit::Title(" "));
  data.plotOn(xframe);
  totpdf.plotOn(xframe, RooFit::LineColor(kBlue));
  totpdf.plotOn(xframe, RooFit::Components("sigpdf"), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
  totpdf.plotOn(xframe, RooFit::Components("bkgpdf"), RooFit::LineColor(kRed ), RooFit::LineStyle(kDashed));
  TCanvas* c = new TCanvas();
  xframe->Draw();
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.03);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.DrawLatex(0.12, 0.88, figtitle.Data());
  label.DrawLatex(0.12, 0.83, Form("N(data) = %.0f", hMass->Integral()));
  label.DrawLatex(0.12, 0.78, Form("N(sig) = %.1f +- %.1f", nsig, nerror));
  if(figname != "")
    c->SaveAs(figname.Data());
  delete c;
  delete xframe;

  delete bkgpdf;
  for(auto var : bkgvars) delete var;

  return nsig;
}

/**
   year: 2016, 2017, or 2018
   isMC: 0 = Data, 1 = AMC@NLO, 2 = MadGraph
   WP  : 0 = none, 1 = WPL, 2 = WP90, 3 = WP80, 4 = WPL && !WP90, 5 = WPL && !WP80 (all for probe only, tag = WP80 is always applied)
 **/
void scale_factors(int year = 2016, int isMC = 0, int WP = 0) {

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/egamma_tnp/";
  TFile* f;
  if     (isMC == 0) f = TFile::Open(Form("%sdata/EGammaTnPAnalysis_SingleEle_%i.root", path.Data(), year), "READ");
  else if(isMC == 1) f = TFile::Open(Form("%sMC/EGammaTnPAnalysis_DY50-amcnlo_%i.root", path.Data(), year), "READ");
  else if(isMC == 2) f = TFile::Open(Form("%sMC/EGammaTnPAnalysis_DY50-madgraph_%i.root", path.Data(), year), "READ");
  else {
    cout << "Undefined isMC value " << isMC << endl;
    return;
  }
  if(!f) return;

  TTree* t = (TTree*) f->Get("Events");
  if(!t) {
    cout << "Events tree not found!\n";
    return;
  }

  ///////////////////////////////////////
  // Initialize tree branch addresses
  ///////////////////////////////////////

  float one_pt, one_eta, one_sc_eta, one_phi, one_q;
  float two_pt, two_eta, two_sc_eta, two_phi, two_q;
  float pair_pt, pair_eta, pair_mass;
  bool  one_triggered, two_triggered;
  int   one_id1, one_id2, two_id1, two_id2;
  float pu_weight(1.), gen_weight(1.);
  // t->SetBranchStatus("*", 0);
  t->SetBranchAddress("one_pt"       , &one_pt       );
  t->SetBranchAddress("one_eta"      , &one_eta      );
  t->SetBranchAddress("one_sc_eta"   , &one_sc_eta   );
  t->SetBranchAddress("one_phi"      , &one_phi      );
  t->SetBranchAddress("one_q"        , &one_q        );
  t->SetBranchAddress("one_id1"      , &one_id1      );
  t->SetBranchAddress("one_id2"      , &one_id2      );
  t->SetBranchAddress("two_pt"       , &two_pt       );
  t->SetBranchAddress("two_eta"      , &two_eta      );
  t->SetBranchAddress("two_sc_eta"   , &two_sc_eta   );
  t->SetBranchAddress("two_phi"      , &two_phi      );
  t->SetBranchAddress("two_q"        , &two_q        );
  t->SetBranchAddress("two_id1"      , &two_id1      );
  t->SetBranchAddress("two_id2"      , &two_id2      );
  t->SetBranchAddress("one_triggered", &one_triggered);
  t->SetBranchAddress("two_triggered", &two_triggered);
  t->SetBranchAddress("pair_pt"      , &pair_pt      );
  t->SetBranchAddress("pair_eta"     , &pair_eta     );
  t->SetBranchAddress("pair_mass"    , &pair_mass    );
  if(isMC) {
    t->SetBranchStatus("puWeight" , 1); t->SetBranchAddress("puWeight" , &pu_weight );
    t->SetBranchStatus("genWeight", 1); t->SetBranchAddress("genWeight", &gen_weight);
  }
  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  // double eta_bins[] = {-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};
  // double pt_bins [] = {28,30,33,36,40,45,50,70,100,500};
  // int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  // int n_pt_bins  = sizeof( pt_bins) / sizeof( *pt_bins) - 1;
  vector<double> eta_bins, pt_bins;
  const float gap_low(1.4442), gap_high(1.566); //eta region to ignore
  if(useAbsEta_) eta_bins = {0.0, 0.8, gap_low, gap_high, 2.1, 2.5};
  else           eta_bins = {-2.5, -2.1,-gap_high,-gap_low, -0.8, 0.0, 0.8, gap_low, gap_high, 2.1, 2.5};
  if(year == 2016) {
    pt_bins  = {25,27,28,29,30,31,32,34,36,40,45,50,55,60,70,85,100,500};
  } else if(year == 2017 || year == 2018) {
    pt_bins  = {25,30,32,33,34,35,36,37,39,41,45,50,55,60,70,85,100,500};
  }
  const int n_eta_bins = eta_bins.size() - 1;
  const int n_pt_bins  = pt_bins.size()  - 1;

  TH2F* hPass  = new TH2F("hPass" , "Passing events", n_eta_bins, &eta_bins[0], n_pt_bins, &pt_bins[0]);
  TH2F* hFail  = new TH2F("hFail" , "Failing events", n_eta_bins, &eta_bins[0], n_pt_bins, &pt_bins[0]);
  hPass->Sumw2(); hFail->Sumw2();
  hPass->SetXTitle("SuperCluster #eta"); hPass->SetYTitle("p_{T} (GeV/c)");
  hFail->SetXTitle("SuperCluster #eta"); hFail->SetYTitle("p_{T} (GeV/c)");
  TH1F* hTagPt    = new TH1F("htagpt"   , "Electron p_{T}", 100,  0  , 100);
  TH1F* hTagEta   = new TH1F("htageta"  , "Electron #eta" ,  50, -2.5, 2.5);
  TH1F* hProbePt  = new TH1F("hprobept" , "Electron p_{T}", 100,  0  , 100);
  TH1F* hProbeEta = new TH1F("hprobeeta", "Electron #eta" ,  50, -2.5, 2.5);

  ///////////////////////////////////////
  // Make a di-lepton resonance histogram per bin
  ///////////////////////////////////////

  std::map<int, TH1*> massHists_pass, massHists_fail;
  for(int xbin = 0; xbin <= hPass->GetNbinsX()+1; ++xbin) {
    for(int ybin = 0; ybin <= hPass->GetNbinsY()+1; ++ybin) {
      const int mapBin = xbin + 100*ybin;
      massHists_pass[mapBin] = new TH1D(Form("zmass_pass_x-%i_y-%i", xbin, ybin), "Dilepton resonance", 60, 60., 120.); massHists_pass[mapBin]->Sumw2();
      massHists_fail[mapBin] = new TH1D(Form("zmass_fail_x-%i_y-%i", xbin, ybin), "Dilepton resonance", 60, 60., 120.); massHists_fail[mapBin]->Sumw2();
    }
  }

  ///////////////////////////////////////
  // Process the data
  ///////////////////////////////////////

  const ULong64_t nentries = t->GetEntriesFast();
  cout << nentries << " events to process\n";
  const float trig_pt_min = (year == 2016) ? 27. : 32.;
  const float tag_pt_min = trig_pt_min + 2.; // 2 GeV/c above threshold
  const float tag_eta_max = 2.3;//2.17;
  const float probe_pt_min = trig_pt_min - 2.; //2 GeV/c below threshold
  const int tag_id1_min = 0; //FIXME
  const int tag_id2_min = 0; //FIXME
  const int probe_id1_min = 0; //FIXME
  const int probe_id1_max = -1; //FIXME
  const int probe_id2_min = 0; //FIXME
  const int probe_id2_max = -1; //FIXME
  for(ULong64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 100000 == 0) printf("Processing entry %12lld (%5.1f%%)...\n", entry, entry*100./nentries);
    t->GetEntry(entry);
    //filter events
    if(pair_mass <= 60. || pair_mass > 120.) continue;
    float wt = pu_weight; //*((gen_weight < 0) ? -1. : 1.);
    //first consider lepton 1 as the tag
    if(one_triggered &&
       std::fabs(one_sc_eta) < tag_eta_max &&
       (gap_low > fabs(one_sc_eta) || fabs(one_sc_eta) > gap_high) &&
       one_pt > tag_pt_min &&
       two_pt > probe_pt_min &&
       one_id1 >= tag_id1_min &&
       two_id1 >= probe_id1_min &&
       (probe_id1_max < 0 || two_id1 <= probe_id1_max) &&
       one_id2 >= tag_id2_min &&
       two_id2 >= probe_id2_min &&
       (probe_id2_max < 0 || two_id2 <= probe_id2_max)) { //Passes cuts to perform probe check
      if(two_triggered) hPass->Fill(two_sc_eta, two_pt, wt);
      else              hFail->Fill(two_sc_eta, two_pt, wt);
      hTagPt  ->Fill(one_pt, wt); hTagEta  ->Fill(one_sc_eta, wt);
      hProbePt->Fill(two_pt, wt); hProbeEta->Fill(two_sc_eta, wt);
      const int xbin = hPass->GetXaxis()->FindBin(two_sc_eta);
      const int ybin = hPass->GetYaxis()->FindBin(two_pt);
      const int mapBin = xbin + 100*ybin;
      if(two_triggered) massHists_pass[mapBin]->Fill(pair_mass, wt);
      else              massHists_fail[mapBin]->Fill(pair_mass, wt);
    } //end lepton one tag

    //next consider lepton 2 as the tag
    if(two_triggered &&
       std::fabs(two_sc_eta) < tag_eta_max &&
       (gap_low > fabs(two_sc_eta) || fabs(two_sc_eta) > gap_high) &&
       two_pt > tag_pt_min &&
       one_pt > probe_pt_min &&
       two_id1 >= tag_id1_min &&
       one_id1 >= probe_id1_min &&
       (probe_id1_max < 0 || one_id1 <= probe_id1_max) &&
       two_id2 >= tag_id2_min &&
       one_id2 >= probe_id2_min &&
       (probe_id2_max < 0 || one_id2 <= probe_id2_max)) { //Passes cuts to perform probe check
      if(one_triggered) hPass->Fill(one_sc_eta, one_pt, wt);
      else              hFail->Fill(one_sc_eta, one_pt, wt);
      hTagPt  ->Fill(two_pt, wt); hTagEta  ->Fill(two_sc_eta, wt);
      hProbePt->Fill(one_pt, wt); hProbeEta->Fill(one_sc_eta, wt);
      const int xbin = hPass->GetXaxis()->FindBin(one_sc_eta);
      const int ybin = hPass->GetYaxis()->FindBin(one_pt);
      const int mapBin = xbin + 100*ybin;
      if(one_triggered) massHists_pass[mapBin]->Fill(pair_mass, wt);
      else              massHists_fail[mapBin]->Fill(pair_mass, wt);
    } //end lepton one tag
  } //end event loop

  TH2F* hRatio = (TH2F*) hPass->Clone("hRatio_cc");
  TH2F* hTotal = (TH2F*) hPass->Clone("hTotal_cc");
  hTotal->Add(hFail);
  hRatio->Divide(hTotal);
  hRatio->SetTitle("Efficiency");

  //ensure 0 <= efficiency <= 1
  for(int xbin = 1; xbin <= hRatio->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hRatio->GetNbinsY(); ++ybin) {
      if     (hRatio->GetBinContent(xbin, ybin) > 1.) hRatio->SetBinContent(xbin, ybin, 1.);
      else if(hRatio->GetBinContent(xbin, ybin) < 0.) hRatio->SetBinContent(xbin, ybin, 0.);
    }
  }

  ///////////////////////////////////////
  // Generate resonance fit scales
  ///////////////////////////////////////

  TH2F* hResPass = (TH2F*) hPass->Clone("hResPass"); hResPass->Reset();
  TH2F* hResFail = (TH2F*) hFail->Clone("hResFail"); hResFail->Reset();

  TString fitdir = Form("figures/fit_%i_wp%i_mc%i", year, WP, isMC);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fitdir.Data(), fitdir.Data()));
  const int bkgmode = 1; //(Mode != 2) ? 1 : 0;
  for(int xbin = 1; xbin <= hResPass->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hResPass->GetNbinsY(); ++ybin) {
      const int mapBin = xbin + 100*ybin;
      if(massHists_pass[mapBin]->Integral() <= 0.) {
        hResPass->SetBinContent(xbin, ybin, 0.);
        hResFail->SetBinContent(xbin, ybin, 0.);
        continue;
      }
      TString title = Form("%.2f #leq #eta < %.2f, %.2f #leq p_{T} < %.2f GeV/c",
                           hResPass->GetXaxis()->GetBinLowEdge(xbin), hResPass->GetXaxis()->GetBinLowEdge(xbin) + hResPass->GetXaxis()->GetBinWidth(xbin),
                           hResPass->GetYaxis()->GetBinLowEdge(ybin), hResPass->GetYaxis()->GetBinLowEdge(ybin) + hResPass->GetYaxis()->GetBinWidth(ybin));
      double pass_err, fail_err;
      hResPass->SetBinContent(xbin, ybin, extract_nz(massHists_pass[mapBin], pass_err, bkgmode, Form("%s/x-%i_y-%i_pass.png", fitdir.Data(), xbin, ybin), title));
      hResFail->SetBinContent(xbin, ybin, extract_nz(massHists_fail[mapBin], fail_err, bkgmode, Form("%s/x-%i_y-%i_fail.png", fitdir.Data(), xbin, ybin), title));
      hResPass->SetBinError(xbin, ybin, pass_err);
      hResFail->SetBinError(xbin, ybin, fail_err);
    }
  }

  TH2F* hResRatio = (TH2F*) hResPass->Clone("hRatio");
  TH2F* hResTotal = (TH2F*) hResPass->Clone("hTotal");
  hResTotal->Add(hResFail);
  hResRatio->Divide(hResTotal);
  hResRatio->SetTitle("Efficiency");

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1500, 700);
  c->Divide(3,1);
  TVirtualPad* pad;
  pad = c->cd(1);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hPass->Draw("colz");
  pad->SetLogz();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hFail->Draw("colz");
  pad->SetLogz();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hRatio->Draw("colz");
  hRatio->GetZaxis()->SetRangeUser(0., 1.);

  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  c->SaveAs(Form("figures/eff_%i_wp%i_mc%i_cc.png", year, WP, isMC));
  delete c;

  c = new TCanvas("c", "c", 1500, 700);
  c->Divide(3,1);
  pad = c->cd(1);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResPass->Draw("colz");
  pad->SetLogz();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResFail->Draw("colz");
  pad->SetLogz();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResRatio->Draw("colz");
  hResRatio->GetZaxis()->SetRangeUser(0., 1.);
  c->SaveAs(Form("figures/eff_%i_wp%i_mc%i.png", year, WP, isMC));
  delete c;

  c = new TCanvas("c", "c", 1000, 700);
  c->Divide(2,1);
  TLegend* leg = new TLegend(0.6, 0.8, 0.9, 0.9);
  leg->AddEntry(hTagPt, "Tag", "L");
  leg->AddEntry(hProbePt, "Probe", "L");
  pad = c->cd(1);
  hTagPt->SetLineWidth(2);
  hTagPt->SetLineColor(kRed);
  hTagPt->Draw("hist");
  hProbePt->SetLineWidth(2);
  hProbePt->SetLineColor(kBlue);
  hProbePt->Draw("hist same");
  leg->Draw();
  pad = c->cd(2);
  hTagEta->SetLineWidth(2);
  hTagEta->SetLineColor(kRed);
  hTagEta->Draw("hist");
  hProbeEta->SetLineWidth(2);
  hProbeEta->SetLineColor(kBlue);
  hProbeEta->Draw("hist same");
  leg->Draw();
  c->SaveAs(Form("figures/kin_%i_wp%i_mc%i.png", year, WP, isMC));
  delete c;

  TFile* fout = new TFile(Form("rootfiles/efficiencies_%i_wp%i_mc%i.root", year, WP, isMC), "RECREATE");
  hRatio->Write();
  hResRatio->Write();
  fout->Write();
  fout->Close();
}
