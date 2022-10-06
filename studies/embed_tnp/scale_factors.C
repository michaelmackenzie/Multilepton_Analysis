//Script to generate Tag and Probe (TnP) electron trigger scale factors

int debug_ = 0; //1: print info then exit; 2: process nentries_ and perform fits; 3: 2 + print fit information
int  nentries_ = 2e6;
bool single_fit_ = false; //do just one bin of the matrix fit
int  fit_x_ = 1; //bin of the single fit
int  fit_y_ = 1; //bin of the single fit
bool applyScales_ = true; //apply conditioned IDs' scale factors
bool use_abs_eta_ = false; //use eta or |eta| in the scale factor measurement

enum {kIDTest = 1};
enum {kVVLooseIso = 1, kVLooseIso, kLooseIso, kMediumIso, kTightIso, kVTightIso, kVVTightIso};
enum {kWPL = 1, kWP90, kWP80};

struct lepdata_t {
  float pt;
  float eta;
  float sc_eta;
  float phi;
  float q;
  int   id1;
  int   id2;
  bool  triggered;
};

struct eventdata_t {
  float pair_pt;
  float pair_eta;
  float pair_mass;
  float pu_weight;
  float gen_weight;
};

//set reasonable z-range
void set_z_range(TH2* h, const double min_default = 0.8, const double max_default = 1.2) {
  double min_z(1.e20), max_z(-1.e20);
  for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= h->GetNbinsY(); ++ybin) {
      const double binc = h->GetBinContent(xbin, ybin);
      if(binc > 1.e-4) min_z = std::min(min_z, binc);
      max_z = std::max(max_z, binc);
    }
  }
  if(min_z < max_z) {
    min_z -= 0.1*(max_z - min_z);
    max_z += 0.1*(max_z - min_z);
    h->GetZaxis()->SetRangeUser(min_z, max_z);
  }
  else h->GetZaxis()->SetRangeUser(min_default, max_default);
}

// //perform tag and probe check
// bool perform_check(lepdata_t& tag, lepdata_t& probe, int Mode) {
//   if(std::fabs(tag.sc_eta) < tag_eta_max && tag.pt > tag_pt_min &&
//      std::fabs(probe.sc_eta) < probe_eta_max && probe.pt > probe_pt_min &&
//      tag.id1 >= id1_tag && tag.id2 >= id2_tag && (trig_mode || tag.triggered)
//      && (isMuon || std::fabs(tag.sc_eta) < gap_low || std::fabs(tag.sc_eta) > gap_high)
//      && (!tag_triggers || tag.triggered)/* && probe.id1 <= id1_max && probe.id2 <= id2_max*/) {
//     bool test = false;
//     if     (trig_mode) test = probe.triggered;
//     else if(Mode == 1) test = probe.id1 >= kIDTest;
//     else if(Mode == 2) test = probe.id2 >= kTightIso;
//     else if(Mode == 3) test = (probe.id2 >= kVVLooseIso && probe.id2 <= kMediumIso);
//     else {
//       cout << "Unknown testing mode!\n";
//       return;
//     }

//   return true;
// }

//Fit the mass distribution with a signal and background function
double extract_nz(TH1* hMass, double& nerror, int BkgMode, TString figname = "", TString figtitle = "") {
  nerror = 0.;
  if(hMass->Integral() <= 0.) return 0.;

  const double min_mass(60.), max_mass(120.);

  //Create the observable
  RooRealVar mass("mass", "Dilepton mass", 91., min_mass, max_mass, "GeV/c^{2}");
  mass.setBins((max_mass-min_mass)/hMass->GetBinWidth(1));

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
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 2.e-3, -2., 2.); bkgvars.push_back(a_bkg);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 7.e-3, -2., 2.); bkgvars.push_back(b_bkg);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 5.e-3, -2., 2.); bkgvars.push_back(c_bkg);
    RooRealVar* d_bkg = new RooRealVar("d_bkg", "d_bkg", 3.e-3, -2., 2.); bkgvars.push_back(d_bkg);
    bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg));
    // bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg, *d_bkg));
  } else if(BkgMode == 2) { //Chebychev polynomial
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 2.e-3, -2., 2.); bkgvars.push_back(a_bkg);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 3.e-2, -2., 2.); bkgvars.push_back(b_bkg);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 1.e-2, -2., 2.); bkgvars.push_back(c_bkg);
    RooRealVar* d_bkg = new RooRealVar("d_bkg", "d_bkg", 1.e-2, -2., 2.); bkgvars.push_back(d_bkg);
    bkgpdf = new RooChebychev("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg, *d_bkg));
  // } else if(BkgMode == 3) { //two exponential PDFs
  //   RooRealVar* tau_1 = new RooRealVar("tau_1", "tau_2", -1., -10., 1.); bkgvars.push_back(tau_1);
  //   RooRealVar* tau_2 = new RooRealVar("tau_2", "tau_2", -1., -10., 1.); bkgvars.push_back(tau_2);
  //   bkgpdf = new RooExponential("bkgpdf", "bkgpdf", mass, *tau_1);
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
      auto fitres = totpdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::PrintLevel(-1), RooFit::Save());
      refit = fitres != 0 && fitres->status() != 0 && count < 2;
    } else {
      cout << "###### Fit: " << figtitle.Data() << ": " << figname.Data() << endl;
      auto fitres = totpdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Save());
      cout << "#######################################################" << endl << endl
           << "Fit: " << figtitle.Data() << ": " << figname.Data() << endl
           << "Fit result: status = " << fitres->status() << " covQual = " << fitres->covQual() << endl << endl
           << "#######################################################" << endl;
      sigpdf.Print("tree");
      bkgpdf->Print("tree");
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
  label.DrawLatex(0.12, 0.83, Form("N(data) = %.0f", hMass->Integral()));
  label.DrawLatex(0.12, 0.78, Form("N(sig)  = %.1f +- %.1f", nsig, nerror));
  if(figname != "")
    c->SaveAs(figname.Data());
  delete c;
  delete xframe;

  delete bkgpdf;
  for(auto var : bkgvars) delete var;

  return nsig;
}

/**
   Mode:
   0 = muon/electron trigger
   1 = muon/electron ID
   2 = Tight muon/electron Iso ID
   3 = Loose + !Tight muon / electron Iso ID
   4 = Muon/electron trigger for Loose + !Tight QCD ID

   isMC:
   0 = Data
   1 = Embedding
   2 = MC (not currently implemented)
 **/

void scale_factors(int Mode = 0, int isMC = 1, bool isMuon = true, int year = 2016, int period = -1) {

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/embed_tnp/";
  TFile* f;

  vector<TString> runs;
  if     (year == 2016) {
    if     (period == -1)   runs = {"B", "C", "D", "E", "F", "G", "H"};
    else if(period ==  0)   runs = {"B", "C", "D", "E", "F"};
    else if(period ==  1)   runs = {"G", "H"};
  } else if(year == 2017) { runs = {"B", "C", "D", "E", "F"};
  } else if(year == 2018) {
    if      (period == -1)  runs = {"A", "B", "C", "D"};
    else if (period ==  0)  runs = {"A", "B", "C"};
    else if (period ==  1)  runs = {"D"};
  }
  if(isMC == 2) runs = {""}; //no run-dependent MC files
  CrossSections xs;

  ///////////////////////////////////////
  // Initialize ID correction objects
  ///////////////////////////////////////

  const bool trig_mode = (Mode == 0 || Mode == 4);

  TFile* fSF_1 = TFile::Open(Form("rootfiles/embedding_eff_%s_mode-%i_%i%s.root",
                                  (isMuon) ? "mumu" : "ee", 1, year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
  TFile* fSF_2 = TFile::Open(Form("rootfiles/embedding_eff_%s_mode-%i_%i%s.root",
                                  (isMuon) ? "mumu" : "ee", (Mode == 4) ? 3 : 2, year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
  if((!fSF_1 && Mode != 1) || (!fSF_2 && trig_mode)) {
    cout << "Scale factor files not found for prerequisite IDs! "
         << "Mode = " << Mode << " isMC = " << isMC << " isMuon = " << isMuon << " year = " << year << endl;

    if(applyScales_) return;
  }

  TH2* hSF_1 = 0;
  TH2* hSF_2 = 0;

  if(fSF_1) {
    hSF_1 = (TH2*) fSF_1->Get("PtVsEtaSF");
    if(hSF_1) hSF_1->SetName("scales_1");
    else {
      cout << "Scale factor histogram for ID 1 not found! "
           << "Mode = " << Mode << " isMC = " << isMC << " isMuon = " << isMuon << " year = " << year << endl;
      return;
    }
  }
  if(fSF_2) {
    hSF_2 = (TH2*) fSF_2->Get("PtVsEtaSF");
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

  //eta sign only matters for the electron ID correction
  use_abs_eta_ = isMuon || Mode != 1;

  vector<double> eta_bins;
  const double gap_low(1.4442), gap_high(1.566);
  if(isMuon && use_abs_eta_) eta_bins = {0., 0.9, 1.2, 1.6, 2.2}; //muon
  else if(isMuon)            eta_bins = {-2.2, -1.6, -1.2, -0.9, 0., 0.9, 1.2, 1.6, 2.2}; //muon
  else if(use_abs_eta_)      eta_bins = {0., 0.2, 0.5, 1., gap_low, gap_high, 1.9, 2.2}; //electron
  else                       eta_bins = {-2.2, -1.9, -gap_high, -gap_low, -1., -0.5, -0.2, 0., 0.2, 0.5, 1., gap_low, gap_high, 1.9, 2.2}; //electron
  vector<double> pt_bins;
  if(isMuon) {
    if(trig_mode) {
      if(year != 2017) pt_bins = {    24, 27, 30, 35, 40, 45, 50, 60, 80, 100, 500}; //IsoMu24
      else             pt_bins = {        27, 30, 35, 40, 45, 50, 60, 80, 100, 500}; //IsoMu27
    } else             pt_bins = {10, 20, 25, 30, 35, 40, 45, 50, 60, 80, 100, 500}; //Muon ID/Iso ID
  } else {
    if(trig_mode) {
      if(year == 2016) pt_bins = {        27, 30, 35, 40, 45, 50, 60, 80, 100, 500}; //Ele27
      else             pt_bins = {            32, 35, 40, 45, 50, 60, 80, 100, 500}; //Ele32
    } else             pt_bins = {10, 20, 25, 30, 35, 40, 45, 50, 60, 80, 100, 500}; //Electron ID/Iso ID
  }
  const int n_eta_bins = eta_bins.size() - 1;
  const int n_pt_bins  = pt_bins .size() - 1;

  TH2* hPass  = new TH2F("hPass" , "Passing events", n_eta_bins, &eta_bins[0], n_pt_bins, &pt_bins[0]);
  TH2* hFail  = new TH2F("hFail" , "Failing events", n_eta_bins, &eta_bins[0], n_pt_bins, &pt_bins[0]);
  hPass->Sumw2(); hFail->Sumw2();
  hPass->SetXTitle((isMuon) ? "#eta" : "SuperCluster #eta"); hPass->SetYTitle("p_{T} (GeV/c)");
  hFail->SetXTitle((isMuon) ? "#eta" : "SuperCluster #eta"); hFail->SetYTitle("p_{T} (GeV/c)");
  TH1* hTagPt    = new TH1F("htagpt"   , (isMuon) ? "Muon p_{T}" : "Electron p_{T}", 100,  0  , 100);
  TH1* hTagEta   = new TH1F("htageta"  , (isMuon) ? "Muon #eta"  : "Electron #eta" ,  50, -2.5, 2.5);
  TH1* hProbePt  = new TH1F("hprobept" , (isMuon) ? "Muon p_{T}" : "Electron p_{T}", 100,  0  , 100);
  TH1* hProbeEta = new TH1F("hprobeeta", (isMuon) ? "Muon #eta"  : "Electron #eta" ,  50, -2.5, 2.5);
  TH1* hDiffPt   = new TH1F("hdiffpt"  , (isMuon) ? "Muon #Deltap_{T}" : "Electron #Deltap_{T}", 100,  -50  , 50);
  TH1* hDiffEta  = new TH1F("hdiffeta" , (isMuon) ? "Muon #Delta#eta"  : "Electron #Delta#eta" ,  50, -5, 5);

  ///////////////////////////////////////
  // Make a di-lepton resonance histogram per bin
  ///////////////////////////////////////

  std::map<int, TH1*> massHists_pass, massHists_fail;
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
    if     (isMC == 0) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Single%sRun%i%s_%i.root", path.Data(), (isMuon) ? "Muon"   : "Electron", year, run.Data(), year), "READ");
    else if(isMC == 1) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Embed-%s-%s_%i.root", path.Data(), (isMuon) ? "MuMu" : "EE" , run.Data(), year), "READ");
    else if(isMC == 2) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_DY50_%i.root", path.Data(), year), "READ");
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
    bool  pair_ismuon;
    bool  one_triggered, two_triggered;
    float pu_weight(1.), gen_weight(1.);
    const bool has_muon_flag = t->GetBranch("pair_ismuon");
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
    if(has_muon_flag) {
      t->SetBranchStatus("pair_ismuon"  , 1); t->SetBranchAddress("pair_ismuon"    , &pair_ismuon    );
    }

    const double xs_scale = (isMC == 1) ? xs.GetCrossSection(Form("Embed-%s-%s", (isMuon) ? "MuMu" : "EE", run.Data()), year) : 1.;

    ///////////////////////////////////////
    // Process the data
    ///////////////////////////////////////

    ULong64_t nentries = t->GetEntriesFast();
    ntotal += nentries;
    cout << nentries << " events to process";
    if(isMC == 1) cout << " in run " << run.Data();
    cout << endl;

    //Define the thresholds
    float trig_pt_min = (isMuon) ? 24. : 32.;
    if((!isMuon && year == 2016) || (isMuon && year == 2017)) trig_pt_min = 27.;
    const float tag_pt_min = trig_pt_min + 3.;
    const float tag_eta_max = 2.1;
    const bool  tag_triggers = true; //whether or not to always require the tag to trigger
    const float probe_eta_max = 2.2;
    const float probe_pt_min = (trig_mode) ? trig_pt_min - 2. : 10.;

    //Define ID selections
    int id1_min(0), id2_min(0); //minimum IDs for all leptons, id1 = ID, id2 = Iso ID
    int id1_max(999), id2_max(999); //maximum IDs for probe leptons (e.g. Loose + !Tight measurements)

    const int id1_tag = kIDTest; //just a boolean check
    const int id2_tag = kTightIso; //tight muon or electron Iso ID

    //probe IDs
    if(Mode != 1) id1_min = kIDTest; //pass ID test for all but ID scale measurement
    if(Mode == 0) id2_min = kTightIso; //tight Iso ID for trigger only
    if(Mode == 4) { //Loose + !Tight Iso ID trigger scale
      id2_min = kVVLooseIso; //measuring VVLoose ID
      id2_max = kMediumIso; //fails Tight Iso ID
    }

    //Loop through the input tree
    for(ULong64_t entry = 0; entry < nentries; ++entry) {
      if(entry % 100000 == 0) printf("Processing entry %12lld (%5.1f%%)...\n", entry, entry*100./nentries);
      t->GetEntry(entry);
      if(isMC == 1 && gen_weight > 1.) continue;
      if(!has_muon_flag) { //approximate with eta vs eta_sc
        pair_ismuon = std::fabs(one_eta - one_sc_eta) < 1.e-5 && std::fabs(two_eta - two_sc_eta) < 1.e-5;
      }
      if(pair_ismuon != isMuon) continue;

      //Z mass peak
      if(pair_mass < 60. || pair_mass > 120.) continue;

      //opposite flavor
      if(one_q*two_q > 0) continue;

      //electron eta veto
      // // no need to veto since it's naturally a bin region
      // if(!isMuon && gap_low <= fabs(one_sc_eta) && fabs(one_sc_eta) <= gap_high) continue;
      // if(!isMuon && gap_low <= fabs(two_sc_eta) && fabs(two_sc_eta) <= gap_high) continue;

      //must fire the trigger
      if(!((one_pt > trig_pt_min && one_triggered) || (two_pt > trig_pt_min && two_triggered))) continue;

      //must satisfy minimum thresholds
      if(one_pt < probe_pt_min && two_pt < probe_pt_min) continue;
      if(one_pt < tag_pt_min && two_pt < tag_pt_min) continue;

      //ensure each passes base IDs
      if(one_id1 < id1_min || one_id2 < id2_min ||
         two_id1 < id1_min || two_id2 < id2_min) continue;

      //ensure they're within the accepted volume
      if(std::fabs(one_sc_eta) >= probe_eta_max || std::fabs(two_sc_eta) >= probe_eta_max) continue;

      //make sure they're separated
      TLorentzVector lv1; lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
      TLorentzVector lv2; lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
      if(std::fabs(lv1.DeltaR(lv2)) < 0.5) continue;

      ++nused;
      if(debug_ && nused > nentries_) {
        if(debug_ == 1) return; //exit at this level
        break; //continue with a sub-sample
      }

      float wt = pu_weight*((isMC > 1) ? ((gen_weight < 0) ? -1. : 1.) : gen_weight)*xs_scale;

      if(applyScales_ && Mode != 1 && isMC == 1 && hSF_1) { //Apply the ID1 scale factors for the trigger and ID2 measurements
        wt *= hSF_1->GetBinContent(hSF_1->GetXaxis()->FindBin((use_abs_eta_) ? std::fabs(one_sc_eta) : one_sc_eta), min(hSF_1->GetNbinsY(), hSF_1->GetYaxis()->FindBin(one_pt)));
        wt *= hSF_1->GetBinContent(hSF_1->GetXaxis()->FindBin((use_abs_eta_) ? std::fabs(two_sc_eta) : two_sc_eta), min(hSF_1->GetNbinsY(), hSF_1->GetYaxis()->FindBin(two_pt)));
      }
      if(applyScales_ && trig_mode && isMC == 1 && hSF_2) { //Apply the ID2 scale factors for the trigger measurements
        wt *= hSF_2->GetBinContent(hSF_2->GetXaxis()->FindBin((use_abs_eta_) ? std::fabs(one_sc_eta) : one_sc_eta), min(hSF_2->GetNbinsY(), hSF_2->GetYaxis()->FindBin(one_pt)));
        wt *= hSF_2->GetBinContent(hSF_2->GetXaxis()->FindBin((use_abs_eta_) ? std::fabs(two_sc_eta) : two_sc_eta), min(hSF_2->GetNbinsY(), hSF_2->GetYaxis()->FindBin(two_pt)));
      }
      if(isMC == 1 && wt <= 0.) {
        cout << "!!! Warning! Weight <= 0: Entry " << entry << ", nused " << nused << ", gen weight = " << gen_weight << ", weight = " << wt << endl
             << " one: pt = " << one_pt << " eta = " << one_sc_eta << " id1 = " << one_id1 << " id2 = " << one_id2 << " triggered = " << one_triggered << endl
             << " two: pt = " << two_pt << " eta = " << two_sc_eta << " id1 = " << two_id1 << " id2 = " << two_id2 << " triggered = " << two_triggered << endl;
      }
      if(debug_ == 1) {
        cout << "Entry " << entry << ", nused " << nused << ", gen weight = " << gen_weight << ", weight = " << wt << endl
             << " one: pt = " << one_pt << " eta = " << one_eta << " id1 = " << one_id1 << " id2 = " << one_id2 << " triggered = " << one_triggered << endl
             << " two: pt = " << two_pt << " eta = " << two_eta << " id1 = " << two_id1 << " id2 = " << two_id2 << " triggered = " << two_triggered << endl;
      }

      ////////////////////////////////////////////////////////////
      //First consider one the tag, two the probe

      if(std::fabs(one_sc_eta) < tag_eta_max && one_pt > tag_pt_min &&
         std::fabs(two_sc_eta) < probe_eta_max && two_pt > probe_pt_min &&
         one_id1 >= id1_tag && one_id2 >= id2_tag && (trig_mode || one_triggered)
         && (isMuon || std::fabs(one_sc_eta) < gap_low || std::fabs(one_sc_eta) > gap_high)
         && (!tag_triggers || one_triggered)/* && two_id1 <= id1_max && two_id2 <= id2_max*/) {
        bool test = false;
        if     (trig_mode) test = two_triggered;
        else if(Mode == 1) test = two_id1 >= kIDTest;
        else if(Mode == 2) test = two_id2 >= kTightIso;
        else if(Mode == 3) test = (two_id2 >= kVVLooseIso && two_id2 <= kMediumIso);
        else {
          cout << "Unknown testing mode!\n";
          return;
        }

        //Fill the (eta,pt) counting histogram for the (eta,pt) point
        if(test) hPass->Fill((use_abs_eta_) ? std::fabs(two_sc_eta) : two_sc_eta, two_pt, wt);
        else     hFail->Fill((use_abs_eta_) ? std::fabs(two_sc_eta) : two_sc_eta, two_pt, wt);

        //Fill the mass histogram for the (eta,pt) point
        const int mapBin = hPass->GetXaxis()->FindBin((use_abs_eta_) ? std::fabs(two_sc_eta) : two_sc_eta) + 100*hPass->GetYaxis()->FindBin(two_pt);
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

      if(std::fabs(two_sc_eta) < tag_eta_max && two_pt > tag_pt_min &&
         std::fabs(one_sc_eta) < probe_eta_max && one_pt > probe_pt_min &&
         two_id1 >= id1_tag && two_id2 >= id2_tag && (trig_mode || two_triggered)
         && (isMuon || std::fabs(two_sc_eta) < gap_low || std::fabs(two_sc_eta) > gap_high)
         && (!tag_triggers || two_triggered)/* && one_id1 <= id1_max && two_id2 <= id2_max*/) {
        bool test = false;
        if     (trig_mode) test = one_triggered;
        else if(Mode == 1) test = one_id1 >= kIDTest;
        else if(Mode == 2) test = one_id2 >= kTightIso;
        else if(Mode == 3) test = (one_id2 >= kVVLooseIso && one_id2 <= kMediumIso);
        else {
          cout << "Unknown testing mode!\n";
          return;
        }

        //Fill the (eta,pt) counting histogram for the (eta,pt) point
        if(test) hPass->Fill((use_abs_eta_) ? std::fabs(one_sc_eta) : one_sc_eta, one_pt, wt);
        else     hFail->Fill((use_abs_eta_) ? std::fabs(one_sc_eta) : one_sc_eta, one_pt, wt);

        //Fill the mass histogram for the (eta,pt) point
        const int mapBin = hPass->GetXaxis()->FindBin((use_abs_eta_) ? std::fabs(one_sc_eta) : one_sc_eta) + 100* hPass->GetYaxis()->FindBin(one_pt);
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
  TH2* hRatio = (TH2*) hPass->Clone("hRatio_cc");
  TH2* hTotal = (TH2*) hPass->Clone("hTotal_cc");
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
  pad->SetLogy();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hFail->Draw("colz");
  pad->SetLogz();
  pad->SetLogy();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  gStyle->SetPaintTextFormat(".2f");
  hRatio->Draw("colz text");
  set_z_range(hRatio, 0., 1.);
  pad->SetLogy();
  // if(Mode == 3)
  //   hRatio->GetZaxis()->SetRangeUser(0., 1.);
  // else
  //   hRatio->GetZaxis()->SetRangeUser((isMuon) ? 0.5 : 0.2*(Mode != 0), 1.);

  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  //Base output name
  TString outname = Form("%s_mode-%i_mc-%i_%i", (isMuon) ? "mumu" : "ee", Mode, isMC, year);
  if(period >= 0) outname += Form("_period_%i", period);

  c->SaveAs(Form("figures/eff_%s_cc.png", outname.Data()));
  delete c;

  //Plot some basic kinematics of the tag/probe
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

  c->SaveAs(Form("figures/kin_%s.png", outname.Data()));

  ///////////////////////////////////////
  // Generate resonance fit scales
  ///////////////////////////////////////

  TH2* hResPass = (TH2*) hPass->Clone("hResPass"); hResPass->Reset();
  TH2* hResFail = (TH2*) hFail->Clone("hResFail"); hResFail->Reset();

  TString fitdir = Form("figures/fit_%s", outname.Data());
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
      if(single_fit_ && (xbin != fit_x_ || ybin != fit_y_)) continue;
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

  TH2* hResRatio = (TH2*) hResPass->Clone("hRatio");
  TH2* hResTotal = (TH2*) hResPass->Clone("hTotal");
  hResTotal->Add(hResFail);
  hResRatio->Divide(hResTotal);
  hResRatio->SetTitle("Efficiency");

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  //Plot the yields and the efficiencies
  c = new TCanvas("c", "c", 1500, 700);
  c->Divide(3,1);
  pad = c->cd(1);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResPass->Draw("colz");
  pad->SetLogz();
  pad->SetLogy();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResFail->Draw("colz");
  pad->SetLogz();
  pad->SetLogy();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResRatio->Draw("colz text");
  set_z_range(hResRatio, 0., 1.);
  pad->SetLogy();
  // hResRatio->GetZaxis()->SetRangeUser((isMuon) ? 0.5 : 0.2*(Mode != 0), 1.);

  c->SaveAs(Form("figures/eff_%s.png", outname.Data()));
  delete c;

  //Write efficiency histogram
  TFile* fout = new TFile(Form("rootfiles/efficiencies_%s.root", outname.Data()), "RECREATE");
  hRatio->Write();
  hResRatio->Write();
  fout->Write();
  fout->Close();
}
