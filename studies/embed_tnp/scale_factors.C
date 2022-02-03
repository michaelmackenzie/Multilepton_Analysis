//Script to generate Tag and Probe (TnP) electron trigger scale factors

int debug_ = 0;
int  nentries_ = 2e6;
bool applyScales_ = false; //apply conditioned IDs' scale factors

//Fit the mass distribution with a signal and background function
double extract_nz(TH1F* hMass, double& nerror, int BkgMode, TString figname = "", TString figtitle = "") {
  nerror = 0.;
  if(hMass->Integral() <= 0.) return 0.;

  const double min_mass(60.), max_mass(120.);

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

  // //DoubleCB signal
  RooRealVar     mean  ("mean"  , "mean"  , 90., 85., 95.); //central RooCBShape
  RooRealVar     sigma ("sigma" , "sigma" , 2.2, 0.5, 10.);
  RooRealVar     alpha1("alpha1", "alpha1", 1. , 0.1, 10.);
  RooRealVar     alpha2("alpha2", "alpha2", 2. , 0.1, 10.);
  RooRealVar     enne1 ("enne1" , "enne1" , 3.7, 0.5, 10.);
  RooRealVar     enne2 ("enne2" , "enne2" , 1.9, 0.5, 10.);
  RooDoubleCrystalBall sigpdf("sigpdf", "sigpdf", mass, mean, sigma, alpha1, enne1, alpha2, enne2);

  //Build the background PDF
  RooAbsPdf* bkgpdf;
  vector<RooRealVar*> bkgvars;
  if(BkgMode == 0) { //exponential PDF
    RooRealVar* tau = new RooRealVar("tau", "tau", -1., -10., 1.); bkgvars.push_back(tau);
    bkgpdf = new RooExponential("bkgpdf", "bkgpdf", mass, *tau);
  } else if(BkgMode == 1) { //Bernstein polynomial
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 2.e-3, -2., 2.); bkgvars.push_back(a_bkg);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 3.e-2, -2., 2.); bkgvars.push_back(b_bkg);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 1.e-2, -2., 2.); bkgvars.push_back(c_bkg);

    bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg));
  }

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
    if(debug_ < 3) {
      totpdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::PrintLevel(-1));
    } else {
      cout << "###### Fit: " << figtitle.Data() << ": " << figname.Data() << endl;
      auto fitres = totpdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Save());
      cout << "#######################################################" << endl << endl
           << "Fit result: status = " << fitres->status() << " covQual = " << fitres->covQual() << endl << endl
           << "#######################################################" << endl;
      refit = fitres->status() != 0 && count < 2;
    }
    ++count;
    if(refit) cout << endl << "Refitting the dataset!\n";
  } while(refit);
  double nsig = N_sig.getVal();
  nerror = N_sig.getError();
  if(nsig < 0.) cout << "N(sig) < 0 for " << figname.Data() << ", " << figtitle.Data() << endl;
  nsig = max(nsig, 0.);

  //ensure a somewhat reasonable error
  if(nerror > 3.*sqrt(nsig) && nerror > 3.*sqrt(rough_sig_frac*hMass->Integral()))
    nerror = 1.2*sqrt(max(nsig, rough_sig_frac*hMass->Integral()));

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
  label.DrawLatex(0.12, 0.83, Form("N(sig) = %.1f +- %.1f", nsig, nerror));
  if(figname != "")
    c->SaveAs(figname.Data());
  delete c;
  delete xframe;

  delete bkgpdf;
  for(auto var : bkgvars) delete var;

  return nsig;
}

// TChain* create_chain(int isMC, bool isMuon, int year) {
//   TChain* chain = new TChain("Events");
//   vector<TString> runs;
//   if(year == 2016)      runs = {"B", "C", "D", "E", "F", "G", "H"};
//   else if(year == 2017) runs = {"B", "C", "D", "E", "F"};
//   else if(year == 2018) runs = {"A", "B", "C", "D"};
//   TString path = "root://cmseos.fnal.gov//store/user/mmackenz/embed_tnp/EmbedTnPAnalysis_";
//   if(isMC) path += Form("Embed-%s", (isMuon) ? "MuMu" : "ElEl");
//   else {
//     // path += Form("Single%s", (isMuon) ? "MuonRun" : "ElectronRun");
//     //Just return the merged data file since no scale factors are applied by run
//     chain->Add((path + Form("Single%s_%i.root", (isMuon) ? "Mu" : "Ele", year)).Data());
//     return chain;
//   }

//   for(TString run : runs) chain->Add((path + run + Form("_%i.root", year)).Data());
//   return chain;
// }

/**
   Mode:
   0 = muon/electron trigger
   1 = muon/electron ID
   2 = muon Iso ID

   isMC:
   0 = Data
   1 = Embedding
   2 = MC
 **/

void scale_factors(int Mode = 0, int isMC = 1, bool isMuon = true, int year = 2016) {

  if(Mode == 2 && !isMuon) {
    cout << "Mode 2 is not defined for electrons!\n";
    return;
  }

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/embed_tnp/";
  TFile* f;

  vector<TString> runs;
  if(year == 2016)      runs = {"B", "C", "D", "E", "F", "G", "H"};
  else if(year == 2017) runs = {"B", "C", "D", "E", "F"};
  else if(year == 2018) runs = {"A", "B", "C", "D"};
  if(!isMC) runs = {""}; //don't do run dependence for data
  CrossSections xs;

  ///////////////////////////////////////
  // Initialize ID correction objects
  ///////////////////////////////////////

  TFile* fSF_1 =            TFile::Open(Form("rootfiles/embedding_eff_%s_mode-1_%i.root", (isMuon) ? "mumu" : "ee", year));
  TFile* fSF_2 = (isMuon) ? TFile::Open(Form("rootfiles/embedding_eff_%s_mode-2_%i.root", (isMuon) ? "mumu" : "ee", year)) : 0;
  if((!fSF_1 && Mode != 1) || (!fSF_2 && Mode == 0 && isMuon)) {
    cout << "Scale factor files not found for required IDs! "
         << "Mode = " << Mode << " isMC = " << isMC << " isMuon = " << isMuon << " year = " << year << endl;
    return;
  }

  TH2F* hSF_1 = 0;
  TH2F* hSF_2 = 0;

  if(fSF_1) {
    hSF_1 = (TH2F*) fSF_1->Get("PtVsEtaSF");
    if(hSF_1) hSF_1->SetName("scales_1");
    else {
      cout << "Scale factor histogram for ID 1 not found! "
           << "Mode = " << Mode << " isMC = " << isMC << " isMuon = " << isMuon << " year = " << year << endl;
      return;
    }
  }
  if(fSF_2) {
    hSF_2 = (TH2F*) fSF_2->Get("PtVsEtaSF");
    if(hSF_2) hSF_2->SetName("scales_2");
    else {
      cout << "Scale factor histogram for ID 2 not found! "
           << "Mode = " << Mode << " isMC = " << isMC << " isMuon = " << isMuon << " year = " << year << endl;
      return;
    }
  }

  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  vector<double> eta_bins;
  const double gap_low(1.4442), gap_high(1.566);
  if(isMuon) eta_bins = {0., 0.9, 1.2, 2.1, 2.4}; //{-2.4,-2.0,-1.5,-0.8, 0.0, 0.8, 1.5, 2.0, 2.4};
  else       eta_bins = {0., 1., gap_low, gap_high, 2.1, 2.5}; //{-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};
  vector<double> pt_bins;
  if(isMuon) {
    if(Mode == 0) pt_bins = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 40, 45, 50, 60, 80, 100, 500};
    else          pt_bins = {10, 15, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 45, 50, 60, 80, 100, 500};
  } else {
    if(Mode == 0) pt_bins = {25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 44, 46, 48, 50, 100, 500};
    else          pt_bins = {10, 15, 20, 24, 26, 28, 30, 32, 34, 36, 38, 40, 45, 50, 100, 500};
  }
  // if(Mode == 0) pt_bins = {24,26,28,30,33,36,40,45,50,70,100,500};
  // else          pt_bins = {10,15,20,25,30,35,40,45,50,70,100,500};
  int n_eta_bins = eta_bins.size() - 1;
  int n_pt_bins  = pt_bins .size() - 1;

  TH2F* hPass  = new TH2F("hPass" , "Passing events", n_eta_bins, &eta_bins[0], n_pt_bins, &pt_bins[0]);
  TH2F* hFail  = new TH2F("hFail" , "Failing events", n_eta_bins, &eta_bins[0], n_pt_bins, &pt_bins[0]);
  hPass->Sumw2(); hFail->Sumw2();
  hPass->SetXTitle((isMuon) ? "#eta" : "SuperCluster #eta"); hPass->SetYTitle("p_{T} (GeV/c)");
  hFail->SetXTitle((isMuon) ? "#eta" : "SuperCluster #eta"); hFail->SetYTitle("p_{T} (GeV/c)");
  TH1F* hTagPt    = new TH1F("htagpt"   , (isMuon) ? "Muon p_{T}" : "Electron p_{T}", 100,  0  , 100);
  TH1F* hTagEta   = new TH1F("htageta"  , (isMuon) ? "Muon #eta"  : "Electron #eta" ,  50, -2.5, 2.5);
  TH1F* hProbePt  = new TH1F("hprobept" , (isMuon) ? "Muon p_{T}" : "Electron p_{T}", 100,  0  , 100);
  TH1F* hProbeEta = new TH1F("hprobeeta", (isMuon) ? "Muon #eta"  : "Electron #eta" ,  50, -2.5, 2.5);
  TH1F* hDiffPt   = new TH1F("hdiffpt"  , (isMuon) ? "Muon #Deltap_{T}" : "Electron #Deltap_{T}", 100,  -50  , 50);
  TH1F* hDiffEta  = new TH1F("hdiffeta" , (isMuon) ? "Muon #Delta#eta"  : "Electron #Delta#eta" ,  50, -5, 5);

  ///////////////////////////////////////
  // Make a di-lepton resonance histogram per bin
  ///////////////////////////////////////

  std::map<int, TH1F*> massHists_pass, massHists_fail;
  for(int xbin = 0; xbin <= hPass->GetNbinsX()+1; ++xbin) {
    for(int ybin = 0; ybin <= hPass->GetNbinsY()+1; ++ybin) {
      const int mapBin = xbin + 100*ybin;
      massHists_pass[mapBin] = new TH1F(Form("zmass_pass_x-%i_y-%i", xbin, ybin), "Dilepton resonance", 60, 60., 120.); massHists_pass[mapBin]->Sumw2();
      massHists_fail[mapBin] = new TH1F(Form("zmass_fail_x-%i_y-%i", xbin, ybin), "Dilepton resonance", 60, 60., 120.); massHists_fail[mapBin]->Sumw2();
    }
  }

  ///////////////////////////////////////
  // File processing loop
  ///////////////////////////////////////

  ULong64_t nused(0), ntotal(0);
  for(TString run : runs) {
    if     (isMC == 0) f = TFile::Open(Form("%sdata/EmbedTnPAnalysis_Single%s_%i.root" , path.Data(), (isMuon) ? "Mu"   : "Ele", year), "READ");
    else if(isMC == 1) f = TFile::Open(Form("%sMC/EmbedTnPAnalysis_Embed-%s-%s_%i.root", path.Data(), (isMuon) ? "MuMu" : "EE" , run.Data(), year), "READ");
  // else if(isMC == 2) f = TFile::Open(Form("%sMC/EmbedTnPAnalysis_DY50-amc_%i.root"  , path.Data(), year), "READ");
    else {
      cout << "Undefined isMC value " << isMC << endl;
      return;
    }
    if(!f) return;

    TTree* t = (TTree*) f->Get("Events");
    if(!t) {
      cout << "Events tree not found in file " << f->GetName() << endl;
      return;
    }

    cout << "Opened new file " << f->GetName() << endl;

    ///////////////////////////////////////
    // Initialize tree branch addresses
    ///////////////////////////////////////

    float one_pt, one_eta, one_sc_eta, one_phi, one_q;
    float two_pt, two_eta, two_sc_eta, two_phi, two_q;
    int   one_id1, one_id2;
    int   two_id1, two_id2;
    float pair_pt, pair_eta, pair_mass;
    bool  one_triggered, two_triggered;
    float pu_weight(1.), gen_weight(1.);
    t->SetBranchStatus("*", 0);
    t->SetBranchStatus("one_pt"         , 1); t->SetBranchAddress("one_pt"         , &one_pt         );
    t->SetBranchStatus("one_eta"        , 1); t->SetBranchAddress("one_eta"        , &one_eta        );
    t->SetBranchStatus("one_sc_eta"     , 1); t->SetBranchAddress("one_sc_eta"     , &one_sc_eta     );
    t->SetBranchStatus("one_phi"        , 1); t->SetBranchAddress("one_phi"        , &one_phi        );
    t->SetBranchStatus("one_q"          , 1); t->SetBranchAddress("one_q"          , &one_q          );
    t->SetBranchStatus("one_id1"        , 1); t->SetBranchAddress("one_id1"        , &one_id1        );
    t->SetBranchStatus("one_id2"        , 1); t->SetBranchAddress("one_id2"        , &one_id2        );
    t->SetBranchStatus("one_triggered"  , 1); t->SetBranchAddress("one_triggered"  , &one_triggered  );
    t->SetBranchStatus("two_pt"         , 1); t->SetBranchAddress("two_pt"         , &two_pt         );
    t->SetBranchStatus("two_eta"        , 1); t->SetBranchAddress("two_eta"        , &two_eta        );
    t->SetBranchStatus("two_sc_eta"     , 1); t->SetBranchAddress("two_sc_eta"     , &two_sc_eta     );
    t->SetBranchStatus("two_phi"        , 1); t->SetBranchAddress("two_phi"        , &two_phi        );
    t->SetBranchStatus("two_q"          , 1); t->SetBranchAddress("two_q"          , &two_q          );
    t->SetBranchStatus("two_id1"        , 1); t->SetBranchAddress("two_id1"        , &two_id1        );
    t->SetBranchStatus("two_id2"        , 1); t->SetBranchAddress("two_id2"        , &two_id2        );
    t->SetBranchStatus("two_triggered"  , 1); t->SetBranchAddress("two_triggered"  , &two_triggered  );
    t->SetBranchStatus("pair_pt"        , 1); t->SetBranchAddress("pair_pt"        , &pair_pt        );
    t->SetBranchStatus("pair_mass"      , 1); t->SetBranchAddress("pair_mass"      , &pair_mass      );
    if(isMC) {
      if(isMC > 1) {
        t->SetBranchStatus("puWeight" , 1); t->SetBranchAddress("puWeight" , &pu_weight );
      }
      t->SetBranchStatus("genWeight", 1); t->SetBranchAddress("genWeight", &gen_weight);
    }

    const double xs_scale = (isMC == 1) ? xs.GetCrossSection(Form("Embed-%s-%s", (isMuon) ? "MuMu" : "ElEl", run.Data()), year) : 1.;

    ///////////////////////////////////////
    // Process the data
    ///////////////////////////////////////

    ULong64_t nentries = t->GetEntriesFast();
    ntotal += nentries;
    cout << nentries << " events to process";
    if(isMC == 1) cout << " in run " << run.Data();
    cout << endl;
    float trig_pt_min = (isMuon) ? 25. : 33.;
    if((!isMuon && year == 2016) || (isMuon && year == 2017)) trig_pt_min = 28.;
    float tag_pt_min = trig_pt_min; //(Mode == 0) ? trig_pt_min : 15.;
    float tag_eta_max = 2.3;
    bool  tag_triggers = true; //whether or not to always require the tag to trigger
    float probe_eta_max = 2.3;
    float probe_pt_min = (Mode == 0) ? tag_pt_min - 1. : 10.;
    int id1_min(0), id2_min(0);
    if(Mode != 1) id1_min = 3; //tight muon ID / WP80 electron ID
    if(isMuon && Mode == 0) id2_min = 4; //tight muon Iso ID for trigger only
    for(ULong64_t entry = 0; entry < nentries; ++entry) {
      if(entry % 100000 == 0) printf("Processing entry %12lld (%5.1f%%)...\n", entry, entry*100./nentries);
      t->GetEntry(entry);
      if(isMC == 1 && gen_weight > 1.) continue;
      //Z mass peak
      if(pair_mass < 60. || pair_mass > 120.) continue;
      //opposite flavor
      if(one_q*two_q > 0) continue;
      //electron eta veto
      if(!isMuon && gap_low <= fabs(one_sc_eta) && fabs(one_sc_eta) <= gap_high) continue;
      if(!isMuon && gap_low <= fabs(two_sc_eta) && fabs(two_sc_eta) <= gap_high) continue;
      //must fire the trigger
      if(!((one_pt > trig_pt_min && one_triggered) || (two_pt > trig_pt_min && two_triggered))) continue;
      //must satisfy minimum thresholds
      if(one_pt < probe_pt_min && two_pt < probe_pt_min) continue;
      if(one_pt < tag_pt_min && two_pt < tag_pt_min) continue;
      //ensure each passes base IDs
      if(one_id1 < id1_min || one_id2 < id2_min ||
         two_id1 < id1_min || two_id2 < id2_min) continue;
      //make sure they're separated
      TLorentzVector lv1; lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
      TLorentzVector lv2; lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
      if(fabs(lv1.DeltaR(lv2)) < 0.5) continue;

      ++nused;
      if(debug_ && nused > ntotal) {
        if(debug_ == 1) return; //exit at this level
        break; //continue with a sub-sample
      }

      float wt = pu_weight*((isMC > 1) ? ((gen_weight < 0) ? -1. : 1.) : gen_weight)*xs_scale;

      if(applyScales_ && Mode != 1 && isMC && hSF_1) { //Apply the ID1 scale factors for the trigger and ID2 measurements
        wt *= hSF_1->GetBinContent(hSF_1->GetXaxis()->FindBin(one_sc_eta), hSF_1->GetYaxis()->FindBin(one_pt));
        wt *= hSF_1->GetBinContent(hSF_1->GetXaxis()->FindBin(two_sc_eta), hSF_1->GetYaxis()->FindBin(two_pt));
      }
      if(applyScales_ && Mode == 0 && isMC && hSF_2) { //Apply the ID2 scale factors for the trigger measurements
        wt *= hSF_2->GetBinContent(hSF_2->GetXaxis()->FindBin(one_sc_eta), hSF_2->GetYaxis()->FindBin(one_pt));
        wt *= hSF_2->GetBinContent(hSF_2->GetXaxis()->FindBin(two_sc_eta), hSF_2->GetYaxis()->FindBin(two_pt));
      }

      if(debug_ == 1) {
        cout << "Entry " << entry << ", nused " << nused << ", gen weight = " << gen_weight << ", weight = " << wt << endl
             << " one: pt = " << one_pt << " eta = " << one_eta << " id1 = " << one_id1 << " id2 = " << one_id2 << " triggered = " << one_triggered << endl
             << " two: pt = " << two_pt << " eta = " << two_eta << " id1 = " << two_id1 << " id2 = " << two_id2 << " triggered = " << two_triggered << endl;
      }
      ////////////////////////////////////////////////////////////
      //First consider one the tag, two the probe

      if(fabs(one_sc_eta) < tag_eta_max && one_pt > tag_pt_min &&
         fabs(two_sc_eta) < probe_eta_max && two_pt > probe_pt_min &&
         one_id1 >= 3 && one_id2 >= (4 * isMuon) && (Mode != 0 || one_triggered)
         && (!tag_triggers || one_triggered)) {
        bool test = false;
        if(Mode == 0) test = two_triggered;
        if(Mode == 1) test = two_id1 >= 3;
        if(Mode == 2) test = two_id2 >= 4;

        //Fill the (eta,pt) counting histogram for the (eta,pt) point
        if(test) hPass->Fill(fabs(two_sc_eta), two_pt, wt);
        else     hFail->Fill(fabs(two_sc_eta), two_pt, wt);

        //Fill the mass histogram for the (eta,pt) point
        const int mapBin = hPass->GetXaxis()->FindBin(fabs(two_sc_eta)) + 100*hPass->GetYaxis()->FindBin(two_pt);
        if(test) massHists_pass[mapBin]->Fill(pair_mass, wt);
        else     massHists_fail[mapBin]->Fill(pair_mass, wt);

        //Fill debug kinematics
        hTagPt  ->Fill(one_pt, wt); hTagEta  ->Fill(one_sc_eta, wt);
        hProbePt->Fill(two_pt, wt); hProbeEta->Fill(two_sc_eta, wt);
        hDiffPt ->Fill(two_pt - one_pt, wt); hDiffEta ->Fill(two_sc_eta - one_sc_eta, wt);
        if(debug_ == 1) cout << " two as probe has test status " << test << endl;
      } else if(debug_ == 1) cout << " one fails tag test\n";

      ////////////////////////////////////////////////////////////
      //Next consider two the tag, one the probe

      if(fabs(two_sc_eta) < tag_eta_max && two_pt > tag_pt_min &&
         fabs(one_sc_eta) < probe_eta_max && one_pt > probe_pt_min &&
         two_id1 >= 3 && two_id2 >= (4 * isMuon) && (Mode != 0 || two_triggered)
         && (!tag_triggers || two_triggered)) {
        bool test = false;
        if(Mode == 0) test = one_triggered;
        if(Mode == 1) test = two_id1 >= 3;
        if(Mode == 2) test = two_id2 >= 4;

        //Fill the (eta,pt) counting histogram for the (eta,pt) point
        if(test) hPass->Fill(fabs(one_sc_eta), one_pt, wt);
        else     hFail->Fill(fabs(one_sc_eta), one_pt, wt);

        //Fill the mass histogram for the (eta,pt) point
        int mapBin = hPass->GetXaxis()->FindBin(fabs(one_sc_eta)) + 100* hPass->GetYaxis()->FindBin(one_pt);
        if(test) massHists_pass[mapBin]->Fill(pair_mass, wt);
        else     massHists_fail[mapBin]->Fill(pair_mass, wt);

        //Fill debug kinematics
        hTagPt  ->Fill(two_pt, wt); hTagEta  ->Fill(two_sc_eta, wt);
        hProbePt->Fill(one_pt, wt); hProbeEta->Fill(one_sc_eta, wt);
        hDiffPt ->Fill(one_pt - two_pt, wt); hDiffEta ->Fill(one_sc_eta - two_sc_eta, wt);
        if(debug_ == 1) cout << " two as probe has test status " << test << endl;
      } else if(debug_ == 1) cout << " one fails tag test\n";
    }
    delete t;
    f->Close();
    cout << "Closed completed file " << f->GetName() << endl;
  }
  cout << "Used " << nused << " of the " << ntotal << " entries (" << (100.*nused)/ntotal << "%)\n";
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
  hRatio->GetZaxis()->SetRangeUser((isMuon) ? 0.5 : 0.2*(Mode != 0), 1.);

  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  c->SaveAs(Form("figures/eff_%s_mode-%i_mc-%i_%i_cc.png", (isMuon) ? "mumu" : "ee", Mode, isMC, year));
  delete c;

  c = new TCanvas("c", "c", 1000, 1200);
  c->Divide(2,2);
  TLegend* leg = new TLegend(0.7, 0.75, 0.9, 0.9);
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
  pad = c->cd(3);
  hDiffPt->SetLineWidth(2);
  hDiffPt->SetLineColor(kRed);
  hDiffPt->Draw("hist");
  pad = c->cd(4);
  hDiffEta->SetLineWidth(2);
  hDiffEta->SetLineColor(kRed);
  hDiffEta->Draw("hist");
  c->SaveAs(Form("figures/kin_%s_mode-%i_mc-%i_%i.png", (isMuon) ? "mumu" : "ee", Mode, isMC, year));

  ///////////////////////////////////////
  // Generate resonance fit scales
  ///////////////////////////////////////

  TH2F* hResPass = (TH2F*) hPass->Clone("hResPass"); hResPass->Reset();
  TH2F* hResFail = (TH2F*) hFail->Clone("hResFail"); hResFail->Reset();

  TString fitdir = Form("figures/fit_%s_%i_mode-%i_%i", (isMuon) ? "mumu" : "ee", year, Mode, isMC);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fitdir.Data(), fitdir.Data()));
  const int bkgmode = 0; //(Mode != 2) ? 1 : 0;
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
  hResRatio->GetZaxis()->SetRangeUser((isMuon) ? 0.5 : 0.2*(Mode != 0), 1.);

  c->SaveAs(Form("figures/eff_%s_mode-%i_mc-%i_%i.png", (isMuon) ? "mumu" : "ee", Mode, isMC, year));
  delete c;

  TFile* fout = new TFile(Form("rootfiles/efficiencies_%s_mode-%i_mc-%i_%i.root", (isMuon) ? "mumu" : "ee", Mode, isMC, year), "RECREATE");
  hRatio->Write();
  hResRatio->Write();
  // hResPass->Write();
  // hResFail->Write();
  // hResTotal->Write();
  fout->Write();
  fout->Close();

  // const double pt  = 15.;
  // const double eta = 0.1;
  // const int xbin = hPass->GetXaxis()->FindBin(eta);
  // const int ybin = hPass->GetYaxis()->FindBin(pt);
  // const int mapBin = xbin + 100*ybin;

  // TString title = Form("%.2f #leq #eta < %.2f, %.2f #leq p_{T} < %.2f GeV/c",
  //                      hResPass->GetXaxis()->GetBinLowEdge(xbin), hResPass->GetXaxis()->GetBinLowEdge(xbin) + hResPass->GetXaxis()->GetBinWidth(xbin),
  //                      hResPass->GetYaxis()->GetBinLowEdge(ybin), hResPass->GetYaxis()->GetBinLowEdge(ybin) + hResPass->GetYaxis()->GetBinWidth(ybin));
  // const double nsig_pass = extract_nz(massHists_pass[mapBin], bkgmode,
  //                                     Form("%s/x-%i_y-%i_pass.png", fitdir.Data(), xbin, ybin), title);
  // const double nsig_fail = extract_nz(massHists_fail[mapBin], bkgmode,
  //                                     Form("%s/x-%i_y-%i_fail.png", fitdir.Data(), xbin, ybin), title);
  // cout << "For an example pt = " << pt << " eta = " << eta << ":\n Cut-and-count gets"
  //      << " N(pass) = " << hPass->GetBinContent(xbin, ybin)
  //      << " N(fail) = " << hFail->GetBinContent(xbin, ybin)
  //      << " eff = "     << hRatio->GetBinContent(xbin, ybin) << endl
  //      << " Resonance fit gets"
  //      << " N(pass) = " << nsig_pass
  //      << " N(fail) = " << nsig_fail
  //      << " eff = "     << nsig_pass/(nsig_pass+nsig_fail) << endl;
}
