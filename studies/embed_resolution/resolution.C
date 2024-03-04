//Compare the resolution of embedding and data events

int  debug_       = 2; //1: print info then exit; 2: process nentries_ and perform fits; 3: 2 + print fit information
int  nentries_    = 1e6; // maximum events to use when debugging
bool applyScales_ = true; //apply ID+IsoID+trigger scale factors
int  correctES_   =    0; //0: no corrections; 1: Provided scales; 2: Measured scales
bool correctRes_  = true; //correct the lepton resolution in embedding
bool interpolatePoints_ = false; //interpolate or use bin values for resolution correction

//useful fields
int isMC_;
int year_;
bool isMuon_;
int nfit_ = 0; //to help with memory leaks in the fits (?)

enum {kIDTest = 1};
enum {kVVLooseIso = 1, kVLooseIso, kLooseIso, kMediumIso, kTightIso, kVTightIso, kVVTightIso};
enum {kWPL = 1, kWP90, kWP80};

//-------------------------------------------------------------------------------------------------------------------------------------
//get the standard figure path
const char* figure_path() {
  return Form("figures/%s_mc-%i_r%i_e%i_%i", (isMuon_) ? "mumu" : "ee", isMC_, correctRes_, correctES_, year_);
}

//-------------------------------------------------------------------------------------------------------------------------------------
//print a standard figure
void print_figure(TH1* h, const char* name, const bool logy = false) {
  TCanvas* c = new TCanvas();
  h->SetLineWidth(2);
  h->SetLineColor(kBlue);
  h->Draw("hist");
  h->GetYaxis()->SetRangeUser(0.1, ((logy) ? 2. : 1.2)*h->GetMaximum());
  if(logy) c->SetLogy();
  c->SaveAs(name);
  delete c;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//Fit the error distribution with a resolution function
int fit_resolution(TH1* h, TString figname = "", TString figtitle = "", const bool is_rel = false,
                   double* width = nullptr, double* width_err = nullptr, double* mean_val = nullptr, double* mean_err = nullptr) {
  if(h->Integral() <= 0.) return 0.;

  ++nfit_;

  const double min_err(h->GetXaxis()->GetXmin()), max_err(h->GetXaxis()->GetXmax());

  //Create the observable
  RooRealVar error(Form("error_%i", nfit_), (is_rel) ? "#Deltap_{T}/p_{T}" : "#Deltap_{T}", 0., min_err, max_err, (is_rel) ? "" : "GeV/c");
  error.setBins((max_err-min_err)/h->GetBinWidth(1));

  //DoubleCB resolution
  RooRealVar     mean  (Form("mean_%i", nfit_)  , "mean"  , 0. , -5.,  5.); //central RooCBShape
  RooRealVar     sigma (Form("sigma_%i", nfit_) , "sigma" , (is_rel) ? 0.015 : 0.5,  0.,  6.);
  RooRealVar     alpha1(Form("alpha1_%i", nfit_), "alpha1", 1. , 0.1, 10.);
  RooRealVar     alpha2(Form("alpha2_%i", nfit_), "alpha2", 1. , 0.1, 10.);
  RooRealVar     enne1 (Form("enne1_%i", nfit_) , "enne1" , 7. , 1.0, 30.);
  RooRealVar     enne2 (Form("enne2_%i", nfit_) , "enne2" , 7. , 1.0, 30.);
  RooDoubleCrystalBall pdf(Form("pdf_%i", nfit_), "pdf", error, mean, sigma, alpha1, enne1, alpha2, enne2);

  //Create a data object to fit
  RooDataHist data(Form("data_%i", nfit_),  "data", RooArgList(error), h);

  //Perform the fit
  bool refit = false;
  int count = 0;
  do {
    if(debug_ < 3) {
      auto fitres = pdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::PrintLevel(-1), RooFit::Save());
      refit = fitres != 0 && fitres->status() != 0 && count < 2;
    } else {
      cout << "###### Fit: " << figtitle.Data() << ": " << figname.Data() << endl;
      auto fitres = pdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Save());
      cout << "#######################################################" << endl << endl
           << "Fit: " << figtitle.Data() << ": " << figname.Data() << endl
           << "Fit result: status = " << fitres->status() << " covQual = " << fitres->covQual() << endl << endl
           << "#######################################################" << endl;
      pdf.Print("tree");
      refit = fitres->status() != 0 && count < 2;
    }
    ++count;
    if(refit) cout << endl << "Refitting the dataset!\n";
  } while(refit);

  pdf.Print("tree");
  //Draw the fit and write the figure to disk
  auto xframe = error.frame(RooFit::Title(" "));
  data.plotOn(xframe);
  pdf.plotOn(xframe, RooFit::LineColor(kRed));
  TCanvas* c = new TCanvas();
  xframe->Draw();

  //add the fit info
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.03);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  if(is_rel) {
    label.DrawLatex(0.14, 0.88, Form("#sigma = %.4f +- %.4f", sigma.getVal(), sigma.getError()));
    label.DrawLatex(0.14, 0.84, Form("#mu = %.4f +- %.4f", mean.getVal(), mean.getError()));
  } else {
    label.DrawLatex(0.14, 0.88, Form("#sigma = %.3f +- %.3f", sigma.getVal(), sigma.getError()));
    label.DrawLatex(0.14, 0.84, Form("#mu = %.3f +- %.3f", mean.getVal(), mean.getError()));
  }
  label.DrawLatex(0.14, 0.80, Form("#alpha_{1} = %.3f +- %.3f", alpha1.getVal(), alpha1.getError()));
  label.DrawLatex(0.14, 0.76, Form("#alpha_{2} = %.3f +- %.3f", alpha2.getVal(), alpha2.getError()));
  label.DrawLatex(0.14, 0.72, Form("n_{1} = %.3f +- %.3f", enne1.getVal(), enne1.getError()));
  label.DrawLatex(0.14, 0.68, Form("n_{2} = %.3f +- %.3f", enne2.getVal(), enne2.getError()));

  if(figname != "")
    c->SaveAs(figname.Data());

  if(width) *width = sigma.getVal();
  if(width_err) *width_err = sigma.getError();
  if(mean_val) *mean_val = mean.getVal();
  if(mean_err) *mean_err = mean.getError();
  delete c;
  delete xframe;
  return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//fit pT resolution from 2D histogram
void fit_2d_resolution(TH2* h2D) {
  //perform a fit for each of the x-binned resolution histograms
  const int n_x_bins = h2D->GetNbinsX();
  double x_centers[n_x_bins], x_errs[n_x_bins];
  double widths[n_x_bins], width_errs[n_x_bins];
  double means[n_x_bins], mean_errs[n_x_bins];
  double max_width(0.), min_width(999.);
  TString dir = figure_path();
  for(int ibin = 1; ibin <= n_x_bins; ++ibin) {
    //initialize the parameters
    widths[ibin-1] = 0.; width_errs[ibin-1] = 0.; means[ibin-1] = 0.; mean_errs[ibin-1] = 0.;
    TH1* h = h2D->ProjectionY(Form("_bin_%i", ibin), ibin, ibin);
    x_centers[ibin-1] = h2D->GetXaxis()->GetBinCenter(ibin);
    x_errs   [ibin-1] = h2D->GetXaxis()->GetBinWidth(ibin)/2.;
    fit_resolution(h, Form("%s/%s_bin_%i.png", dir.Data(), h2D->GetName(), ibin), h2D->GetName(), true,
                   &(widths[ibin-1]), &(width_errs[ibin-1]),
                   &(means [ibin-1]), &(mean_errs [ibin-1]));
    max_width = max(widths[ibin-1], max_width);
    min_width = min(widths[ibin-1], min_width);
    dir = figure_path();
  }
  //plot the means
  {
    dir = figure_path();
    TGraph* g = new TGraphErrors(n_x_bins, x_centers, means, x_errs, mean_errs);
    TCanvas* c = new TCanvas();
    g->SetMarkerSize(0.8);
    g->SetMarkerStyle(20);
    g->SetLineWidth(2);
    g->SetTitle("Lepton p_{T} vs. resolution #mu; p_{T}; #mu");
    g->SetName(Form("g_means_%s", h2D->GetName()));
    g->Draw("APE");
    c->Modified(); c->Update();
    c->SaveAs(Form("%s/%s_vs_mean.png", dir.Data(), h2D->GetName()));
    delete c;
    g->Write();
    delete g;
  }
  //plot the fit of the widths
  {
    dir = figure_path();
    TGraph* gwidths = new TGraphErrors(n_x_bins, x_centers, widths, x_errs, width_errs);
    TCanvas* c = new TCanvas();
    gwidths->SetMarkerSize(0.8);
    gwidths->SetMarkerStyle(20);
    gwidths->SetLineWidth(2);
    gwidths->SetTitle("Lepton p_{T} vs. resolution #sigma; p_{T}; #sigma");
    gwidths->SetName(Form("g_widths_%s", h2D->GetName()));
    gwidths->Draw("APE");
    gwidths->GetYaxis()->SetRangeUser(min_width - 0.1*(max_width - min_width), max_width + 0.1*(max_width - min_width));
    c->Modified(); c->Update();
    c->SaveAs(Form("%s/%s_vs_width.png", dir.Data(), h2D->GetName()));
    delete c;
    gwidths->Write();
    delete gwidths;
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------
//electron energy scale correction for embedding
float embed_energy_scale(float eta, int year) {
  const bool barrel = std::fabs(eta) < 1.479;
  float scale_factor(1.f);
  if(year == 2016) {
    if(barrel) scale_factor -= 0.0024;
    else       scale_factor -= 0.0070;
  }
  else if(year == 2017) {
    if(barrel) scale_factor -= 0.0007;
    else       scale_factor -= 0.0113;
  }
  else if(year == 2018) {
    if(barrel) scale_factor -= 0.0033;
    else       scale_factor -= 0.0056;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//electron energy scale correction for embedding
float electron_energy_scale(double pt, double eta, std::vector<TGraphAsymmErrors*>& graphs) {
  //determine the graph |eta| region by the number of graphs found

  const unsigned ngraphs = graphs.size();

  if(ngraphs == 0) return 1.f;

  auto g = graphs[0]; //default to the first graph

  if(ngraphs == 2) { //barrel vs. endcap split
    const bool barrel = std::fabs(eta) < 1.479;
    g = (barrel) ? graphs[0] : graphs[1];
  } else if(ngraphs == 3) { //split barrel into a low and high |eta| region
    const double abs_eta = std::fabs(eta);
    const int index = (abs_eta >= 0.5) + (abs_eta >= 1.5);
    g = graphs[index];
  } else if(ngraphs == 4) { //split barrel and endcap into a low and high |eta| region
    const double abs_eta = std::fabs(eta);
    const int index = (abs_eta >= 0.5) + (abs_eta >= 1.5) + (abs_eta >= 2.1);
    g = graphs[index];
  }

  double val(1.);
  if(interpolatePoints_)
    val = CLFV::Utilities::Interpolate(g, pt);
  else {
    //find the bin that contains pT
    int bin(0);
    for(int ibin = 0; ibin < g->GetN(); ++ibin) {
      bin = ibin;
      if(pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
    }
    val = g->GetY()[bin];
  }
  //value returned is difference in the resolution mean --> scale factor = 1 + difference
  val += 1.;
  if(debug_ % 2 == 1) {
    printf("%s: pT = %5.1f, eta = %5.2f --> scale offset = %.3f\n", __func__, pt, eta, val);
  }
  return val;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//electron energy resolution correction for embedding
float electron_resolution_scale(double pt, double eta, std::vector<TGraphAsymmErrors*>& graphs) {
  //determine the graph |eta| region by the number of graphs found

  const unsigned ngraphs = graphs.size();

  if(ngraphs == 0) return 1.f;

  auto g = graphs[0]; //default to the first graph

  if(ngraphs == 2) { //barrel vs. endcap split
    const bool barrel = std::fabs(eta) < 1.479;
    g = (barrel) ? graphs[0] : graphs[1];
  } else if(ngraphs == 3) { //split barrel into a low and high |eta| region
    const double abs_eta = std::fabs(eta);
    const int index = (abs_eta >= 0.5) + (abs_eta >= 1.5);
    g = graphs[index];
  } else if(ngraphs == 4) { //split barrel and endcap into a low and high |eta| region
    const double abs_eta = std::fabs(eta);
    const int index = (abs_eta >= 0.5) + (abs_eta >= 1.5) + (abs_eta >= 2.1);
    g = graphs[index];
  }

  double val(1.);
  if(interpolatePoints_)
    val = CLFV::Utilities::Interpolate(g, pt);
  else {
    //find the bin that contains pT
    int bin(0);
    for(int ibin = 0; ibin < g->GetN(); ++ibin) {
      bin = ibin;
      if(pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
    }
    val = g->GetY()[bin];
  }
  if(debug_ % 2 == 1) {
    printf("%s: pT = %5.1f, eta = %5.2f --> width ratio = %.3f\n", __func__, pt, eta, val);
  }
  return val;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//muon energy resolution correction for embedding
double muon_resolution_scale(double pt, std::vector<TGraphAsymmErrors*>& graphs) {
  if(graphs.size() != 1) return 1.f;
  auto g = graphs[0];
  double val(1.);
  if(interpolatePoints_)
    val = CLFV::Utilities::Interpolate(g, pt);
  else {
    //find the bin that contains pT
    int bin(0);
    for(int ibin = 0; ibin < g->GetN(); ++ibin) {
      bin = ibin;
      if(pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
    }
    val = g->GetY()[bin];
  }
  if(debug_ % 2 == 1) {
    printf("%s: pT = %5.1f --> width ratio = %.3f\n", __func__, pt, val);
  }
  return val;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//muon energy resolution correction for embedding
double muon_resolution_scale(double gen_pt, double reco_pt) {
  const double min_scale(0.05), max_scale(0.20);
  const double min_pt(10.), max_pt(100.);
  const double pt_err = reco_pt - gen_pt;

  //parameterize in terms of gen-pT to avoid bin migration as reco pT is changed
  double pt_err_scale = std::max(min_scale, std::min(max_scale, min_scale + (max_scale - min_scale)*(gen_pt - min_pt)/(max_pt - min_pt)));
  pt_err_scale += 0.04*std::pow(std::fabs(pt_err/gen_pt)/0.01,2) - 0.08*pt_err/gen_pt/0.01;
  return pt_err_scale;
}


//-------------------------------------------------------------------------------------------------------------------------------------
/**
   isMC:
   0 = Data
   1 = Embedding
   2 = MC
   3 = UL MC
 **/
void resolution(int isMC = 1, bool isMuon = true, int year = 2016, int period = -1, int correctRes = 0, int correctES = 0) {
  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/embed_tnp/";
  isMC_ = isMC;
  isMuon_ = isMuon;
  year_ = year;
  if(isMC_ == 1) {
    correctRes_ = correctRes;
    correctES_  = correctES ;
  } else { // no corrections applied to data or MC
    correctRes  = 0;
    correctES   = 0;
    correctRes_ = 0;
    correctES_  = 0;
  }
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

  if(isMC == 2 || isMC == 3) runs = {""}; //no run-dependent MC files
  CrossSections xs;
  const double gap_low(1.444), gap_high(1.566);

  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  TH1* hMass          = new TH1D("mass"         , "Di-lepton mass; Mass (GeV/c^{2})"    , 60, 60., 120.);
  TH1* hMassUp        = new TH1D("massup"       , "Di-lepton mass; Mass (GeV/c^{2})"  , 60, 60., 120.);
  TH1* hMassDown      = new TH1D("massdown"     , "Di-lepton mass; Mass (GeV/c^{2})", 60, 60., 120.);
  TH1* hMassESUp      = new TH1D("massesup"     , "Di-lepton mass; Mass (GeV/c^{2})"  , 60, 60., 120.);
  TH1* hMassESDown    = new TH1D("massesdown"   , "Di-lepton mass; Mass (GeV/c^{2})", 60, 60., 120.);
  TH1* hGenMass       = new TH1D("genmass"      , "Z gen-level mass; Mass (GeV/c^{2})", 60, 60., 120.);
  TH1* hPt            = new TH1D("pt"           , "Di-lepton p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hGenPt         = new TH1D("genpt"        , "Z gen-level p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hEta           = new TH1D("eta"          , "Di-lepton #eta; #eta", 50, -5., 5.);
  TH1* hGenEta        = new TH1D("geneta"       , "Z gen-level #eta; #eta", 50, -5., 5.);
  TH1* hOneEta        = new TH1D("oneeta"       , "Lead #eta; #eta", 50, -2.5, 2.5);
  TH1* hTwoEta        = new TH1D("twoeta"       , "Trail #eta; #eta", 50, -2.5, 2.5);
  TH1* hOnePt         = new TH1D("onept"        , "Lead p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoPt         = new TH1D("twopt"        , "Trail p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOnePtUp       = new TH1D("oneptup"      , "Lead p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoPtUp       = new TH1D("twoptup"      , "Trail p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOnePtDown     = new TH1D("oneptdown"    , "Lead p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoPtDown     = new TH1D("twoptdown"    , "Trail p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOneGenPt      = new TH1D("onegenpt"     , "Lead gen-level p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoGenPt      = new TH1D("twogenpt"     , "Trail gen-level p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOnePtErr      = new TH1D("onepterr"     , "Lead p_{T} error; #Deltap_{T} (GeV/c)", 50, -5., 5.);
  TH1* hTwoPtErr      = new TH1D("twopterr"     , "Lead p_{T} error; #Deltap_{T} (GeV/c)", 50, -5., 5.);
  TH1* hOneRelErr     = new TH1D("onerelerr"    , "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 200, -0.2, 0.2);
  TH1* hTwoRelErr     = new TH1D("tworelerr"    , "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 200, -0.2, 0.2);
  TH1* hOneRelErrUp   = new TH1D("onerelerrup"  , "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 200, -0.2, 0.2);
  TH1* hTwoRelErrUp   = new TH1D("tworelerrup"  , "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 200, -0.2, 0.2);
  TH1* hOneRelErrDown = new TH1D("onerelerrdown", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 200, -0.2, 0.2);
  TH1* hTwoRelErrDown = new TH1D("tworelerrdown", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 200, -0.2, 0.2);
  TH1* hWeight        = new TH1D("weight"       , "Event weight; weight", 60, 0., 3.);
  TH1* hResScale      = new TH1D("resscale"     , "Resolution scale factor; scale", 60, 0., 3.);

  //lepton resolution in bins of pT
  const double pt_bins[] = {10., 20., 25., 30., 35., 40., 45., 50., 60., 70., 150.};
  const int n_pt_bins = sizeof(pt_bins) / sizeof(*pt_bins) - 1;
  TH2* hLepPtVsRes = new TH2D("lepptvsres", "p_{T} resolution in p_{T} bins; #Deltap_{T} / p_{T}", n_pt_bins, pt_bins, 200, -0.2, 0.2);
  TH2* hLepPtVsResUp = new TH2D("lepptvsresup", "p_{T} resolution in p_{T} bins; #Deltap_{T} / p_{T}", n_pt_bins, pt_bins, 200, -0.2, 0.2);
  //lepton resolution in bins of eta and pT
  // double eta_bins[] = {0., (isMuon) ? 1.2 : 1.5, (isMuon) ? 2.1 : 2.5, (isMuon) ? 2.4 : 1.e6};
  double eta_bins[] = {0., (isMuon) ? 1.2 : 0.5, (isMuon) ? 2.1 : 1.5, (isMuon) ? 2.4 : 2.5};
  // double eta_bins[] = {0., (isMuon) ? 1.2 : 0.5, (isMuon) ? 2.1 : 1.5, (isMuon) ? 2.4 : 2.1, (isMuon) ? 3. : 2.5};
  // double eta_bins[] = {0., 1.2,  2.1, 2.5};
  const int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  vector<TH2*> lepEtaVsPtVsRes;
  cout << "Making " << n_eta_bins << " |eta| bins for the resolution measurement\n";
  for(int ibin = 0; ibin < n_eta_bins; ++ibin) {
    lepEtaVsPtVsRes.push_back(new TH2D(Form("lepptvsres_eta_%i", ibin), "p_{T} resolution in (|#eta|, p_{T}) bins; #Deltap_{T} / p_{T}", n_pt_bins, pt_bins, 200, -0.2, 0.2));
  }

  ///////////////////////////////////////
  // File processing loop
  ///////////////////////////////////////

  ULong64_t nused(0), ntotal(0);
  TFile* f;
  for(TString run : runs) {
    if     (isMC == 0) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Single%sRun%i%s_%i.root", path.Data(), (isMuon) ? "Muon" : "Electron", year, run.Data(), year), "READ");
    else if(isMC == 1) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Embed-%s-%s_%i.root", path.Data(), (isMuon) ? "MuMu" : "EE" , run.Data(), year), "READ");
    else if(isMC == 2) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_DY50_%i.root", path.Data(), year), "READ");
    else if(isMC == 3) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_ULDY50_%i.root", path.Data(), year), "READ");
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
    // Retrieve the correction histograms
    ///////////////////////////////////////

    TFile *f_ID(nullptr), *f_Iso(nullptr), *f_Trig(nullptr);
    TH2 *h_ID(nullptr), *h_Iso(nullptr), *h_Trig_MC(nullptr), *h_Trig_Data(nullptr);
    bool use_abs_eta_ID(true), use_abs_eta_Iso(true), use_abs_eta_Trig(true);
    if(isMC == 1 && applyScales_) { //embedding corrections
      const char* sf_dir = "../../scale_factors";
      f_ID   = TFile::Open(Form("%s/embedding_eff_%s_mode-1_%i.root", sf_dir, (isMuon) ? "mumu" : "ee", year), "READ");
      f_Iso  = TFile::Open(Form("%s/embedding_eff_%s_mode-2_%i.root", sf_dir, (isMuon) ? "mumu" : "ee", year), "READ");
      f_Trig = TFile::Open(Form("%s/embedding_eff_%s_mode-0_%i.root", sf_dir, (isMuon) ? "mumu" : "ee", year), "READ");
      if(f_ID  ) h_ID   = (TH2*) f_ID  ->Get("PtVsEtaSF");
      if(f_Iso ) h_Iso  = (TH2*) f_Iso ->Get("PtVsEtaSF");
      if(f_Trig) h_Trig_MC = (TH2*) f_Trig->Get("PtVsEtaMC");
      if(f_Trig) h_Trig_Data = (TH2*) f_Trig->Get("PtVsEtaData");
      if(h_ID  ) {h_ID  ->SetName("h_ID"  ); use_abs_eta_ID   = h_ID  ->GetXaxis()->GetXmin() >= 0.;}
      if(h_Iso ) {h_Iso ->SetName("h_Iso" ); use_abs_eta_Iso  = h_Iso ->GetXaxis()->GetXmin() >= 0.;}
      if(h_Trig_MC) {h_Trig_MC->SetName("h_Trig_MC"); use_abs_eta_Trig = h_Trig_MC->GetXaxis()->GetXmin() >= 0.;}
      if(h_Trig_Data) {h_Trig_Data->SetName("h_Trig_Data");}

      cout << "Use |eta| for scale factors:\n"
           <<" ID  : " << use_abs_eta_ID << endl
           <<" Iso : " << use_abs_eta_Iso << endl
           <<" Trig: " << use_abs_eta_Trig << endl;
    }
    // else if(isMC == 2) { //FIXME: Add MC corrections

    // }

    //Energy scale correction graphs
    std::vector<TGraphAsymmErrors*> scale_graphs;
    TFile* f_Scale(nullptr);
    if(isMC == 1 && correctES_ == 2) {
      f_Scale = TFile::Open(Form("rootfiles/embed_scale_correction_electron_%i.root", year), "READ");
      if(f_Scale) {
        if(isMuon) scale_graphs.push_back((TGraphAsymmErrors*) f_Scale->Get("lepptvsres_mean_diff")); //inclusive |eta| bin
        else {
          for(int igraph = 0; ; ++igraph) { //retrieve the graph for each |eta| region
            auto g = (TGraphAsymmErrors*) f_Scale->Get(Form("lepptvsres_eta_%i_mean_diff", igraph));
            if(!g) break;
            scale_graphs.push_back(g);
          }
          cout << "Found " << scale_graphs.size() << " resolution mean correction graphs\n";
        }
      }
    }
    if(correctES_ == 2 && scale_graphs.size() == 0) correctES_ = 0; //no corrections are available

    //Resolution correction graphs
    std::vector<TGraphAsymmErrors*> resolution_graphs;
    TFile* f_Res(nullptr);
    if(isMC == 1 && correctRes_) {
      // f_Res = TFile::Open(Form("rootfiles/embed_resolution_correction_electron_%i.root", year), "READ");
      f_Res = TFile::Open(Form("rootfiles/embed_scale_correction_electron_%i.root", year), "READ");
      if(f_Res) {
        if(isMuon) resolution_graphs.push_back((TGraphAsymmErrors*) f_Res->Get("lepptvsres_width_ratio")); //inclusive |eta| bin
        else {
          for(int igraph = 0; ; ++igraph) { //retrieve the graph for each |eta| region
            auto g = (TGraphAsymmErrors*) f_Res->Get(Form("lepptvsres_eta_%i_width_ratio", igraph));
            if(!g) break;
            resolution_graphs.push_back(g);
          }
          cout << "Found " << resolution_graphs.size() << " resolution width correction graphs\n";
        }
      }
    }
    if(resolution_graphs.size() == 0) correctRes_ = false; //no corrections are available

    ///////////////////////////////////////
    // Initialize tree branch addresses
    ///////////////////////////////////////

    float one_pt, one_eta, one_sc_eta, one_phi, one_q, one_ecorr(0.);
    float two_pt, two_eta, two_sc_eta, two_phi, two_q, two_ecorr(0.);
    int   one_id1, one_id2;
    int   two_id1, two_id2;
    float pair_pt, pair_eta, pair_mass;
    bool  pair_ismuon;
    bool  one_triggered, two_triggered;
    float pu_weight(1.), gen_weight(1.);
    float gen_one_pt, gen_one_eta, gen_one_phi;
    float gen_two_pt, gen_two_eta, gen_two_phi;
    float gen_pt, gen_mass, gen_eta; //Gen-level Z info
    const bool has_muon_flag = t->GetBranch("pair_ismuon");
    t->SetBranchStatus("*", 0);
    CLFV::Utilities::SetBranchAddress(t, "one_pt"         , &one_pt         );
    CLFV::Utilities::SetBranchAddress(t, "one_eta"        , &one_eta        );
    CLFV::Utilities::SetBranchAddress(t, "one_sc_eta"     , &one_sc_eta     );
    CLFV::Utilities::SetBranchAddress(t, "one_phi"        , &one_phi        );
    CLFV::Utilities::SetBranchAddress(t, "one_q"          , &one_q          );
    CLFV::Utilities::SetBranchAddress(t, "one_ecorr"      , &one_ecorr      );
    CLFV::Utilities::SetBranchAddress(t, "one_id1"        , &one_id1        );
    CLFV::Utilities::SetBranchAddress(t, "one_id2"        , &one_id2        );
    CLFV::Utilities::SetBranchAddress(t, "one_triggered"  , &one_triggered  );
    CLFV::Utilities::SetBranchAddress(t, "two_pt"         , &two_pt         );
    CLFV::Utilities::SetBranchAddress(t, "two_eta"        , &two_eta        );
    CLFV::Utilities::SetBranchAddress(t, "two_sc_eta"     , &two_sc_eta     );
    CLFV::Utilities::SetBranchAddress(t, "two_phi"        , &two_phi        );
    CLFV::Utilities::SetBranchAddress(t, "two_q"          , &two_q          );
    CLFV::Utilities::SetBranchAddress(t, "two_ecorr"      , &two_ecorr      );
    CLFV::Utilities::SetBranchAddress(t, "two_id1"        , &two_id1        );
    CLFV::Utilities::SetBranchAddress(t, "two_id2"        , &two_id2        );
    CLFV::Utilities::SetBranchAddress(t, "two_triggered"  , &two_triggered  );
    CLFV::Utilities::SetBranchAddress(t, "pair_pt"        , &pair_pt        );
    CLFV::Utilities::SetBranchAddress(t, "pair_mass"      , &pair_mass      );
    CLFV::Utilities::SetBranchAddress(t, "pair_eta"       , &pair_eta       );
    if(isMC) {
      //gen info
      CLFV::Utilities::SetBranchAddress(t, "GenZll_LepOne_pt" , &gen_one_pt );
      CLFV::Utilities::SetBranchAddress(t, "GenZll_LepOne_eta", &gen_one_eta);
      CLFV::Utilities::SetBranchAddress(t, "GenZll_LepOne_phi", &gen_one_phi);
      CLFV::Utilities::SetBranchAddress(t, "GenZll_LepTwo_pt" , &gen_two_pt );
      CLFV::Utilities::SetBranchAddress(t, "GenZll_LepTwo_eta", &gen_two_eta);
      CLFV::Utilities::SetBranchAddress(t, "GenZll_LepTwo_phi", &gen_two_phi);
      CLFV::Utilities::SetBranchAddress(t, "GenZll_pt"        , &gen_pt     );
      CLFV::Utilities::SetBranchAddress(t, "GenZll_mass"      , &gen_mass   );
      CLFV::Utilities::SetBranchAddress(t, "GenZll_eta"       , &gen_eta    );

      //weights
      if(isMC > 1) {
        CLFV::Utilities::SetBranchAddress(t, "puWeight" , &pu_weight );
      }
      CLFV::Utilities::SetBranchAddress(t, "genWeight" , &gen_weight);
    }
    if(has_muon_flag) {
      CLFV::Utilities::SetBranchAddress(t, "pair_ismuon", &pair_ismuon);
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
    //add an offset from the trigger threshold
    trig_pt_min += (isMuon) ? 1.f : 2.f;
    const float eta_max = 2.4;
    const float pt_min = (isMuon) ? 10.f : 15.f;

    //Define ID selections
    const int id1_min(kIDTest), id2_min(kTightIso); //minimum IDs for all leptons, id1 = ID, id2 = Iso ID

    //Loop through the input tree
    for(ULong64_t entry = 0; entry < nentries; ++entry) {
      if(entry % 100000 == 0) printf("Processing entry %12lld (%5.1f%%), used = %12llu...\n", entry, entry*100./nentries, nused);
      t->GetEntry(entry);
      if(isMC == 1 && gen_weight > 1.) continue;
      if(!has_muon_flag) { //approximate with eta vs eta_sc
        pair_ismuon = std::fabs(one_eta - one_sc_eta) < 1.e-5 && std::fabs(two_eta - two_sc_eta) < 1.e-5;
      }
      if(pair_ismuon != isMuon) continue;

      //opposite flavor
      if(one_q*two_q > 0) continue;

      // electron eta veto
      if(!isMuon && gap_low <= fabs(one_sc_eta) && fabs(one_sc_eta) <= gap_high) continue;
      if(!isMuon && gap_low <= fabs(two_sc_eta) && fabs(two_sc_eta) <= gap_high) continue;

      //eta region cut
      if(std::fabs(one_eta) >= eta_max || std::fabs(two_eta) >= eta_max) continue;
      if(std::fabs(one_sc_eta) >= eta_max || std::fabs(two_sc_eta) >= eta_max) continue;

      //ensure each passes base IDs
      if(one_id1 < id1_min || one_id2 < id2_min ||
         two_id1 < id1_min || two_id2 < id2_min) continue;

      //remove electron energy corrections from Embedding electrons
      if(isMC == 1 && !isMuon) {
        if(one_ecorr > 0.f) one_pt /= one_ecorr;
        if(two_ecorr > 0.f) two_pt /= two_ecorr;
      }

      TLorentzVector lv1; lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
      TLorentzVector lv2; lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
      TLorentzVector lv_sys = lv1 + lv2;

      //make sure they're separated
      if(std::fabs(lv1.DeltaR(lv2)) < 0.5) continue;

      ////////////////////////////////////////////////////////////
      //Attempt to match the reco-leptons to the gen-leptons

      if(isMC) {
        TLorentzVector gen_lv1, gen_lv2;
        gen_lv1.SetPtEtaPhiM(gen_one_pt, gen_one_eta, gen_one_phi, (isMuon) ? 0.10566 : 5.11e-3);
        gen_lv2.SetPtEtaPhiM(gen_two_pt, gen_two_eta, gen_two_phi, (isMuon) ? 0.10566 : 5.11e-3);
        //test to see if the vectors are close to the opposite vectors
        const double gen_one_to_one_dr = gen_lv1.DeltaR(lv1);
        const double gen_one_to_two_dr = gen_lv1.DeltaR(lv2);
        const double gen_two_to_one_dr = gen_lv2.DeltaR(lv1);
        const double gen_two_to_two_dr = gen_lv2.DeltaR(lv2);
        //if both are closer to the other gen, swap
        bool swap = gen_one_to_one_dr > gen_one_to_two_dr && gen_two_to_two_dr > gen_two_to_one_dr;
        //if one is closer to both gen leptons, skip due to lack of information
        if(!swap && (gen_one_to_one_dr > gen_one_to_two_dr || gen_two_to_two_dr > gen_two_to_one_dr)) {
          continue;
        }

        //Swap the gen lepton info if the opposite are matched
        if(swap) {
          gen_one_pt  = gen_lv2.Pt();
          gen_one_eta = gen_lv2.Eta();
          gen_one_phi = gen_lv2.Phi();
          gen_two_pt  = gen_lv1.Pt();
          gen_two_eta = gen_lv1.Eta();
          gen_two_phi = gen_lv1.Phi();
        }
      } else { //data
        gen_one_pt  = one_pt   ;
        gen_one_eta = one_eta  ;
        gen_one_phi = one_phi  ;
        gen_two_pt  = two_pt   ;
        gen_two_eta = two_eta  ;
        gen_two_phi = two_phi  ;
        gen_mass    = pair_mass;
        gen_pt      = pair_pt  ;
        gen_eta     = pair_eta ;
      }

      ////////////////////////////////////////////
      // Apply enery scale corrections

      if(isMC == 1 && !isMuon && correctES_) {
        const float scale_one = (correctES_ == 1) ? embed_energy_scale(one_sc_eta, year) : electron_energy_scale(gen_one_pt, one_sc_eta, scale_graphs);
        const float scale_two = (correctES_ == 1) ? embed_energy_scale(two_sc_eta, year) : electron_energy_scale(gen_two_pt, two_sc_eta, scale_graphs);
        one_pt *= scale_one;
        two_pt *= scale_two;
        lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
        lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
        lv_sys = lv1 + lv2;
        pair_mass = lv_sys.M();
        pair_pt = lv_sys.Pt();
        pair_eta = lv_sys.Eta();
      }

      ////////////////////////////////////////////
      // Apply enery resolution corrections

      if(correctRes_ && isMC == 1) {
        float scale_one  = (isMuon) ? muon_resolution_scale(gen_one_pt, resolution_graphs) : electron_resolution_scale(gen_one_pt, one_sc_eta, resolution_graphs);
        float scale_two  = (isMuon) ? muon_resolution_scale(gen_two_pt, resolution_graphs) : electron_resolution_scale(gen_two_pt, two_sc_eta, resolution_graphs);
        if(scale_one <= 0.) {
          cout << "Entry " << entry << ": res scale <= 0 = " << scale_one << ": pt = " << one_pt << "; eta = " << one_sc_eta << endl;
          scale_one = 1.;
        }
        if(scale_two <= 0.) {
          cout << "Entry " << entry << ": res scale <= 0 = " << scale_two << ": pt = " << two_pt << "; eta = " << two_sc_eta << endl;
          scale_two = 1.;
        }
        hResScale->Fill(scale_one);
        hResScale->Fill(scale_two);
        const float one_pt_err = one_pt - gen_one_pt;
        const float two_pt_err = two_pt - gen_two_pt;
        //scale the pT error to correct the resolution
        one_pt += one_pt_err*(scale_one-1.); //if scale = 1 do nothing
        two_pt += two_pt_err*(scale_two-1.);
        lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
        lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
        lv_sys = lv1 + lv2;
        pair_mass = lv_sys.M();
        pair_pt = lv_sys.Pt();
        pair_eta = lv_sys.Eta();
      }

      ////////////////////////////////////////////
      // pT dependent cuts

      //Z mass peak
      if(pair_mass < 60. || pair_mass > 120.) continue;

      //must fire the trigger
      if(!((one_pt > trig_pt_min && one_triggered) || (two_pt > trig_pt_min && two_triggered))) continue;

      //must satisfy minimum pT thresholds
      if(one_pt <= pt_min || two_pt <= pt_min) continue;

      //Accept the event
      ++nused;
      if(debug_ && nentries_ > 0 && nused > nentries_) {
        if(debug_ == 1) return; //exit at this level
        break; //continue with a sub-sample
      }

      float wt = pu_weight*((isMC > 1) ? ((gen_weight < 0) ? -1. : 1.) : gen_weight)*xs_scale;
      if(h_ID) {
        wt *= CLFV::Utilities::GetBinContent(h_ID, (use_abs_eta_ID) ? std::fabs(one_sc_eta) : one_sc_eta, one_pt, false);
        wt *= CLFV::Utilities::GetBinContent(h_ID, (use_abs_eta_ID) ? std::fabs(two_sc_eta) : two_sc_eta, two_pt, false);
      }
      if(h_Iso) {
        wt *= CLFV::Utilities::GetBinContent(h_Iso, (use_abs_eta_Iso) ? std::fabs(one_sc_eta) : one_sc_eta, one_pt, false);
        wt *= CLFV::Utilities::GetBinContent(h_Iso, (use_abs_eta_Iso) ? std::fabs(two_sc_eta) : two_sc_eta, two_pt, false);
      }
      if(h_Trig_MC && h_Trig_Data) {
        const float eff_one_mc   = (one_pt > trig_pt_min) ? CLFV::Utilities::GetBinContent(h_Trig_MC  , (use_abs_eta_Trig) ? std::fabs(one_sc_eta) : one_sc_eta, one_pt) : 0.f;
        const float eff_one_data = (one_pt > trig_pt_min) ? CLFV::Utilities::GetBinContent(h_Trig_Data, (use_abs_eta_Trig) ? std::fabs(one_sc_eta) : one_sc_eta, one_pt) : 0.f;
        const float eff_two_mc   = (two_pt > trig_pt_min) ? CLFV::Utilities::GetBinContent(h_Trig_MC  , (use_abs_eta_Trig) ? std::fabs(two_sc_eta) : two_sc_eta, two_pt) : 0.f;
        const float eff_two_data = (two_pt > trig_pt_min) ? CLFV::Utilities::GetBinContent(h_Trig_Data, (use_abs_eta_Trig) ? std::fabs(two_sc_eta) : two_sc_eta, two_pt) : 0.f;
        float p_data(1.f), p_mc(1.f); //probability of not being triggered
        p_data *= 1. - eff_one_data;
        p_data *= 1. - eff_two_data;
        p_mc   *= 1. - eff_one_mc  ;
        p_mc   *= 1. - eff_two_mc  ;
        if(p_mc < 1.f) wt *= (1.f - p_data) / (1.f - p_mc); //P(a trigger | data) / P(a trigger | MC)
      }

      if(debug_ == 1) {
        cout << "Entry " << entry << ", nused " << nused << ", gen weight = " << gen_weight << ", weight = " << wt << endl
             << " one: pt = " << one_pt << " eta = " << one_eta << " id1 = " << one_id1 << " id2 = " << one_id2 << " triggered = " << one_triggered << endl
             << " two: pt = " << two_pt << " eta = " << two_eta << " id1 = " << two_id1 << " id2 = " << two_id2 << " triggered = " << two_triggered << endl;
      }

      ////////////////////////////////////////////////////////////
      // Fill the histograms

      const float one_pt_err = one_pt - gen_one_pt;
      const float two_pt_err = two_pt - gen_two_pt;

      float one_pt_up(one_pt), one_pt_down(one_pt);
      float two_pt_up(two_pt), two_pt_down(two_pt);
      float pair_mass_up(pair_mass), pair_mass_down(pair_mass);
      float one_pt_es_up(one_pt), one_pt_es_down(one_pt);
      float two_pt_es_up(two_pt), two_pt_es_down(two_pt);
      float pair_mass_es_up(pair_mass), pair_mass_es_down(pair_mass);


      //resolution up/down variations
      if(isMC) {
        TLorentzVector lv1_tmp, lv2_tmp;
        // const float pt_err_scale = (isMuon || isMC > 1) ? 0.05 : 0.50;
        float pt_one_err_scale = (isMuon) ? muon_resolution_scale(gen_one_pt, one_pt) : 0.15; //uncertainty on the resolution width
        float pt_two_err_scale = (isMuon) ? muon_resolution_scale(gen_two_pt, two_pt) : 0.15;
        one_pt_up = one_pt + pt_one_err_scale*one_pt_err;
        two_pt_up = two_pt + pt_two_err_scale*two_pt_err;
        lv1_tmp.SetPtEtaPhiM(one_pt_up, one_eta, one_phi, lv1.M());
        lv2_tmp.SetPtEtaPhiM(two_pt_up, two_eta, two_phi, lv2.M());
        pair_mass_up = (lv1_tmp + lv2_tmp).M();
        if(isMuon) {
          one_pt_down = one_pt;
          two_pt_down = two_pt;
        } else {
          one_pt_down = one_pt - pt_one_err_scale*one_pt_err;
          two_pt_down = two_pt - pt_two_err_scale*two_pt_err;
        }
        lv1_tmp.SetPtEtaPhiM(one_pt_down, one_eta, one_phi, lv1.M());
        lv2_tmp.SetPtEtaPhiM(two_pt_down, two_eta, two_phi, lv2.M());
        pair_mass_down = (lv1_tmp + lv2_tmp).M();
      }

      //energy scale up/down variations
      if(isMC) {
        TLorentzVector lv1_tmp, lv2_tmp;
        const float pt_scale = (isMuon) ? 0.0015 : (isMC > 1) ? 0.003 : 0.005;
        one_pt_es_up = one_pt*(1.f + pt_scale);
        two_pt_es_up = two_pt*(1.f + pt_scale);
        lv1_tmp.SetPtEtaPhiM(one_pt_es_up, one_eta, one_phi, lv1.M());
        lv2_tmp.SetPtEtaPhiM(two_pt_es_up, two_eta, two_phi, lv2.M());
        pair_mass_es_up = (lv1_tmp + lv2_tmp).M();
        one_pt_es_down = one_pt*(1.f - pt_scale);
        two_pt_es_down = two_pt*(1.f - pt_scale);
        lv1_tmp.SetPtEtaPhiM(one_pt_es_down, one_eta, one_phi, lv1.M());
        lv2_tmp.SetPtEtaPhiM(two_pt_es_down, two_eta, two_phi, lv2.M());
        pair_mass_es_down = (lv1_tmp + lv2_tmp).M();
      }

      hMass    ->Fill(pair_mass, wt);
      hMassUp  ->Fill(pair_mass_up  , wt);
      hMassDown->Fill(pair_mass_down, wt);
      hMassESUp  ->Fill(pair_mass_es_up  , wt);
      hMassESDown->Fill(pair_mass_es_down, wt);
      hGenMass  ->Fill(gen_mass   , wt);
      hPt       ->Fill(pair_pt    , wt);
      hGenPt    ->Fill(gen_pt     , wt);
      hEta      ->Fill(pair_eta   , wt);
      hGenEta   ->Fill(gen_eta    , wt);
      hOneEta   ->Fill(one_eta    , wt);
      hTwoEta   ->Fill(two_eta    , wt);
      hOnePt    ->Fill(one_pt     , wt);
      hTwoPt    ->Fill(two_pt     , wt);
      hOnePtUp  ->Fill(one_pt_up  , wt);
      hTwoPtUp  ->Fill(two_pt_up  , wt);
      hOnePtDown->Fill(one_pt_down, wt);
      hTwoPtDown->Fill(two_pt_down, wt);
      hOneGenPt ->Fill(gen_one_pt   , wt);
      hTwoGenPt ->Fill(gen_two_pt   , wt);
      hOnePtErr ->Fill(one_pt - gen_one_pt, wt);
      hTwoPtErr ->Fill(two_pt - gen_two_pt, wt);
      hOneRelErr->Fill((one_pt - gen_one_pt) / one_pt, wt);
      hTwoRelErr->Fill((two_pt - gen_two_pt) / two_pt, wt);
      hOneRelErrUp->Fill((one_pt_up - gen_one_pt) / one_pt, wt);
      hTwoRelErrUp->Fill((two_pt_up - gen_two_pt) / two_pt, wt);
      hOneRelErrDown->Fill((one_pt_down - gen_one_pt) / one_pt, wt);
      hTwoRelErrDown->Fill((two_pt_down - gen_two_pt) / two_pt, wt);
      hLepPtVsRes->Fill(one_pt, (one_pt - gen_one_pt) / one_pt, wt);
      hLepPtVsRes->Fill(two_pt, (two_pt - gen_two_pt) / two_pt, wt);
      hLepPtVsResUp->Fill(gen_one_pt, (one_pt_up - gen_one_pt) / one_pt_up, wt);
      hLepPtVsResUp->Fill(gen_two_pt, (two_pt_up - gen_two_pt) / two_pt_up, wt);
      hWeight->Fill(wt);
      //fill the (|eta|, pT, res) histogram
      int eta_bin_one(0), eta_bin_two(0);
      for(int ibin = 1; ibin < n_eta_bins; ++ibin) {
        if(std::fabs(one_eta) >= eta_bins[ibin]) eta_bin_one = ibin;
        if(std::fabs(two_eta) >= eta_bins[ibin]) eta_bin_two = ibin;
      }
      //ensure not in overflow
      if(eta_bin_one < n_eta_bins) lepEtaVsPtVsRes[eta_bin_one]->Fill(gen_one_pt, (one_pt - gen_one_pt) / one_pt, wt);
      if(eta_bin_two < n_eta_bins) lepEtaVsPtVsRes[eta_bin_two]->Fill(gen_two_pt, (two_pt - gen_two_pt) / two_pt, wt);
    } //end tree processing loop

    delete t;
    f->Close();
    if(f_ID  ) f_ID  ->Close();
    if(f_Iso ) f_Iso ->Close();
    if(f_Trig) f_Trig->Close();
    cout << "Closed completed file " << f->GetName() << endl;
  }
  cout << "Used " << nused << " of the " << ntotal << " entries (" << (100.*nused)/ntotal << "%)\n";

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);
  //Base output name
  const char* outdir = figure_path();
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", outdir, outdir));
  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");

  //write the histograms to an output file
  TFile* fout = new TFile(Form("histograms/%s_mc-%i_r%i_e%i_%i.hist", (isMuon) ? "mumu" : "ee", isMC, correctRes_, correctES_, year), "RECREATE");
  hMass->Write();
  hMassUp->Write();
  hMassDown->Write();
  hMassESUp->Write();
  hMassESDown->Write();
  hGenMass->Write();
  hPt->Write();
  hGenPt->Write();
  hEta->Write();
  hGenEta->Write();
  hOneEta->Write();
  hTwoEta->Write();
  hOnePt->Write();
  hTwoPt->Write();
  hOnePtUp->Write();
  hTwoPtUp->Write();
  hOnePtDown->Write();
  hTwoPtDown->Write();
  hOneGenPt->Write();
  hTwoGenPt->Write();
  hOnePtErr->Write();
  hTwoPtErr->Write();
  hOneRelErr->Write();
  hTwoRelErr->Write();
  hOneRelErrUp->Write();
  hTwoRelErrUp->Write();
  hOneRelErrDown->Write();
  hTwoRelErrDown->Write();
  hLepPtVsRes->Write();
  hLepPtVsResUp->Write();

  //print the 1-D figures
  print_figure(hMass      , Form("%s/mass.png"       , outdir));
  print_figure(hMassUp    , Form("%s/massup.png"     , outdir));
  print_figure(hMassDown  , Form("%s/massdown.png"   , outdir));
  print_figure(hMassESUp  , Form("%s/mass_es_up.png"  , outdir));
  print_figure(hMassESDown, Form("%s/mass_es_down.png", outdir));
  print_figure(hGenMass   , Form("%s/genmass.png"    , outdir));
  print_figure(hPt        , Form("%s/pt.png"         , outdir));
  print_figure(hGenPt     , Form("%s/genpt.png"      , outdir));
  print_figure(hEta       , Form("%s/eta.png"        , outdir));
  print_figure(hGenEta    , Form("%s/geneta.png"     , outdir));
  print_figure(hOneEta    , Form("%s/one_eta.png"    , outdir));
  print_figure(hTwoEta    , Form("%s/two_eta.png"    , outdir));
  print_figure(hOnePt     , Form("%s/one_pt.png"     , outdir));
  print_figure(hTwoPt     , Form("%s/two_pt.png"     , outdir));
  print_figure(hOneGenPt  , Form("%s/one_genpt.png"  , outdir));
  print_figure(hTwoGenPt  , Form("%s/two_genpt.png"  , outdir));
  print_figure(hOnePtErr  , Form("%s/one_pterr.png"  , outdir));
  print_figure(hTwoPtErr  , Form("%s/two_pterr.png"  , outdir));
  print_figure(hOneRelErr , Form("%s/one_relerr.png" , outdir));
  print_figure(hTwoRelErr , Form("%s/two_relerr.png" , outdir));
  print_figure(hOneRelErrUp , Form("%s/one_relerr_up.png" , outdir));
  print_figure(hTwoRelErrUp , Form("%s/two_relerr_up.png" , outdir));
  print_figure(hOneRelErrDown , Form("%s/one_relerr_down.png" , outdir));
  print_figure(hTwoRelErrDown , Form("%s/two_relerr_down.png" , outdir));
  print_figure(hWeight        , Form("%s/weight.png"          , outdir));
  print_figure(hResScale      , Form("%s/resscale.png"        , outdir));

  //print resolution fits
  if(isMC) {
    TString dir;
    dir = figure_path();
    fit_resolution(hOnePtErr , Form("%s/one_pterr_fit.png" , dir.Data()), "One pT error", false);
    dir = figure_path();
    fit_resolution(hTwoPtErr , Form("%s/two_pterr_fit.png" , dir.Data()), "Two pT error", false);
    dir = figure_path();
    fit_resolution(hOneRelErr, Form("%s/one_relerr_fit.png", dir.Data()), "One rel. pT error", true);
    dir = figure_path();
    fit_resolution(hTwoRelErr, Form("%s/two_relerr_fit.png", dir.Data()), "Two rel. pT error", true);
    dir = figure_path();
    fit_resolution(hOneRelErrUp, Form("%s/one_relerr_up_fit.png", dir.Data()), "One rel. pT error", true);
    dir = figure_path();
    fit_resolution(hTwoRelErrUp, Form("%s/two_relerr_up_fit.png", dir.Data()), "Two rel. pT error", true);
    dir = figure_path();
    fit_resolution(hOneRelErrDown, Form("%s/one_relerr_down_fit.png", dir.Data()), "One rel. pT error", true);
    dir = figure_path();
    fit_resolution(hTwoRelErrDown, Form("%s/two_relerr_down_fit.png", dir.Data()), "Two rel. pT error", true);
    dir = figure_path();

    //perform a fit for each of the pT binned resolution histograms
    fit_2d_resolution(hLepPtVsRes);

    //perform a fit each of the (|eta|, pT) binned resolution histograms
    for(int ibin = 0; ibin < n_eta_bins; ++ibin) {
      if(lepEtaVsPtVsRes[ibin]->GetEntries() > 0) {
        fit_2d_resolution(lepEtaVsPtVsRes[ibin]);
      }
    }
  }

  fout->Close();
}
