//Compare the resolution of embedding and data events

int  debug_       = 2; //1: print info then exit; 2: process nentries_ and perform fits; 3: 2 + print fit information
int  nentries_    = 1e6; // maximum events to use when debugging
bool single_fit_  = false; //do just one bin of the matrix fit
int  fit_x_       = 1; //bin of the single fit
int  fit_y_       = 1; //bin of the single fit
bool applyScales_ = false; //apply conditioned IDs' scale factors
bool use_abs_eta_ = false; //use eta or |eta| in the scale factor measurement
TF1* fres_funcs_[3] = {nullptr, nullptr, nullptr};
bool use_rel_pt_  = true; //use relative pT when re-weighting

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

//Fit the error distribution with a resolution function
int fit_resolution(TH1* h, TString figname = "", TString figtitle = "", const bool is_rel = false,
                   double* width = nullptr, double* width_err = nullptr, double* mean_val = nullptr, double* mean_err = nullptr) {
  if(h->Integral() <= 0.) return 0.;

  const double min_err(h->GetXaxis()->GetXmin()), max_err(h->GetXaxis()->GetXmax());

  //Create the observable
  RooRealVar error("error", (is_rel) ? "#Deltap_{T}/p_{T}" : "#Deltap_{T}", 0., min_err, max_err, (is_rel) ? "" : "GeV/c");
  error.setBins((max_err-min_err)/h->GetBinWidth(1));

  //DoubleCB resolution
  RooRealVar     mean  ("mean"  , "mean"  , 0. , -5.,  5.); //central RooCBShape
  RooRealVar     sigma ("sigma" , "sigma" , (is_rel) ? 0.015 : 0.5,  0.,  6.);
  RooRealVar     alpha1("alpha1", "alpha1", 1. , 0.1, 10.);
  RooRealVar     alpha2("alpha2", "alpha2", 1. , 0.1, 10.);
  RooRealVar     enne1 ("enne1" , "enne1" , 7. , 1.0, 30.);
  RooRealVar     enne2 ("enne2" , "enne2" , 7. , 1.0, 30.);
  RooDoubleCrystalBall pdf("pdf", "pdf", error, mean, sigma, alpha1, enne1, alpha2, enne2);

  // //Gaussian resolution
  // RooRealVar     mean  ("mean"  , "mean"  , 0., -5., 5.);
  // RooRealVar     sigma ("sigma" , "sigma" , 2.2, 0., 6.);
  // RooGaussian pdf("pdf", "pdf", error, mean, sigma);


  //Create a data object to fit
  RooDataHist data("data",  "data", RooArgList(error), h);

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

//double-sided crystall ball function
float dscb(float x, float mean, float sigma, float alpha1, float enne1, float alpha2, float enne2) {
  //function constants
  const float a1 = std::pow(std::fabs(enne1/alpha1), enne1)*std::exp(-0.5f*alpha1*alpha1);
  const float a2 = std::pow(std::fabs(enne2/alpha2), enne2)*std::exp(-0.5f*alpha2*alpha2);
  const float b1 = std::fabs(enne1/alpha1) - std::fabs(alpha1);
  const float b2 = std::fabs(enne2/alpha2) - std::fabs(alpha2);

  //function main variable
  const float var = (x - mean) /sigma;
  float val(0.f);

  if     (var < -alpha1) val = a1*std::pow(b1 - var, -enne1);
  else if(var <     0.f) val = std::exp(-0.5*var*var);
  else if(var <  alpha2) val = std::exp(-0.5*var*var); //same sigma for left/right
  else                   val = a2*std::pow(b2 + var, -enne2);

  return val;
}

double dscb_func(double* X, double* P) {
  return P[0]*dscb(X[0], P[1], P[2], P[3], P[4], P[5], P[6]);
}

// //Get the weight to a different resolution
// float res_weight(float pt_err, bool is_embed, bool is_muon, bool is_up) {
//   float weight(1.);
//   if(!fres_funcs_[0]) {
//     if(is_muon) {
//       if(is_embed) {
//         fres_funcs_[0] = new TF1("res_func_nom" , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[1] = new TF1("res_func_up"  , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[2] = new TF1("res_func_down", dscb_func, -10., 10., 7, 1);
//         fres_funcs_[0]->SetParameters(1., 0.020, 0.537, 1.056, 7.490, 1.000, 7.597);
//         fres_funcs_[1]->SetParameters(1., 0.020, 0.567, 1.056, 7.490, 1.000, 7.597);
//         fres_funcs_[2]->SetParameters(1., 0.020, 0.507, 1.056, 7.490, 1.000, 7.597);
//         fres_funcs_[0]->SetParameter(0, 1./fres_funcs_[0]->Integral(-10., 10.));
//         fres_funcs_[1]->SetParameter(0, 1./fres_funcs_[1]->Integral(-10., 10.));
//         fres_funcs_[2]->SetParameter(0, 1./fres_funcs_[2]->Integral(-10., 10.));
//       } else {
//         fres_funcs_[0] = new TF1("res_func_nom" , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[1] = new TF1("res_func_up"  , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[2] = new TF1("res_func_down", dscb_func, -10., 10., 7, 1);
//         fres_funcs_[0]->SetParameters(1., -0.004, 0.572, 0.959, 4.213, 1.014, 4.972);
//         fres_funcs_[1]->SetParameters(1., -0.004, 0.600, 0.959, 4.213, 1.014, 4.972);
//         fres_funcs_[2]->SetParameters(1., -0.004, 0.540, 0.959, 4.213, 1.014, 4.972);
//         fres_funcs_[0]->SetParameter(0, 1./fres_funcs_[0]->Integral(-10., 10.));
//         fres_funcs_[1]->SetParameter(0, 1./fres_funcs_[1]->Integral(-10., 10.));
//         fres_funcs_[2]->SetParameter(0, 1./fres_funcs_[2]->Integral(-10., 10.));
//       }
//     } else {
//       if(is_embed) {
//         fres_funcs_[0] = new TF1("res_func_nom" , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[1] = new TF1("res_func_up"  , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[2] = new TF1("res_func_down", dscb_func, -10., 10., 7, 1);
//         fres_funcs_[0]->SetParameters(1., 0.174, 0.472, 0.609, 9.298, 0.843, 5.024);
//         fres_funcs_[1]->SetParameters(1., 0.174, 0.520, 0.609, 9.298, 0.843, 5.024);
//         fres_funcs_[2]->SetParameters(1., 0.174, 0.420, 0.609, 9.298, 0.843, 5.024);
//         fres_funcs_[0]->SetParameter(0, 1./fres_funcs_[0]->Integral(-10., 10.));
//         fres_funcs_[1]->SetParameter(0, 1./fres_funcs_[1]->Integral(-10., 10.));
//         fres_funcs_[2]->SetParameter(0, 1./fres_funcs_[2]->Integral(-10., 10.));
//       } else {
//         fres_funcs_[0] = new TF1("res_func_nom" , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[1] = new TF1("res_func_up"  , dscb_func, -10., 10., 7, 1);
//         fres_funcs_[2] = new TF1("res_func_down", dscb_func, -10., 10., 7, 1);
//         fres_funcs_[0]->SetParameters(1., -0.100, 0.876, 0.733, 29.176, 1.162, 10.205);
//         fres_funcs_[1]->SetParameters(1., -0.100, 0.920, 0.733, 29.176, 1.162, 10.205);
//         fres_funcs_[2]->SetParameters(1., -0.100, 0.820, 0.733, 29.176, 1.162, 10.205);
//         fres_funcs_[0]->SetParameter(0, 1./fres_funcs_[0]->Integral(-10., 10.));
//         fres_funcs_[1]->SetParameter(0, 1./fres_funcs_[1]->Integral(-10., 10.));
//         fres_funcs_[2]->SetParameter(0, 1./fres_funcs_[2]->Integral(-10., 10.));
//       }
//     }
//   }
//   const double nom = fres_funcs_[0]->Eval(pt_err);
//   const double sys = (is_up) ? fres_funcs_[1]->Eval(pt_err) : fres_funcs_[2]->Eval(pt_err);
//   weight = sys / nom;
//   weight = std::min(3.f, std::max(0.3f, weight));
//   return weight;
// }

//Get the weight to a different resolution
float res_weight(float pt_err, bool is_embed, bool is_muon, bool is_up) {
  float weight(1.);
  if(is_muon) {
    if(is_embed) {
      const float sigma_nom = 0.537;
      const float sigma_sys = (is_up) ? sigma_nom + 0.200 : sigma_nom - 0.200;
      const float nom = dscb(pt_err, 0.020, sigma_nom, 1.056, 7.490, 1.000, 7.597);
      const float sys = dscb(pt_err, 0.020, sigma_sys, 1.056, 7.490, 1.000, 7.597);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    } else {
      const float sigma_nom = 0.572;
      const float sigma_sys = (is_up) ? sigma_nom + 0.030 : sigma_nom - 0.030;
      const float nom = dscb(pt_err, -0.004, sigma_nom, 0.959, 4.213, 1.014, 4.972);
      const float sys = dscb(pt_err, -0.004, sigma_sys, 0.959, 4.213, 1.014, 4.972);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    }
  } else { //electron resolution
    if(is_embed) {
      const float sigma_nom = 0.461;
      const float sigma_sys = (is_up) ? sigma_nom + 0.050 : sigma_nom - 0.050;
      const float nom = dscb(pt_err, 0.096, sigma_nom, 0.536, 15.086, 0.941, 4.217);
      const float sys = dscb(pt_err, 0.096, sigma_sys, 0.536, 15.086, 0.941, 4.217);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    } else {
      const float sigma_nom = 0.867;
      const float sigma_sys = (is_up) ? sigma_nom + 0.050 : sigma_nom - 0.050;
      const float nom = dscb(pt_err, -0.099, sigma_nom, 0.739, 18.693, 1.181, 8.173);
      const float sys = dscb(pt_err, -0.099, sigma_sys, 0.739, 18.693, 1.181, 8.173);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    }
  }
  weight = std::min(3.f, std::max(0.3f, weight));
  return weight;
}

//Get the weight to a different resolution
float rel_res_weight(float rel_err, bool is_embed, bool is_muon, bool is_up) {
  float weight(1.);
  if(is_muon) {
    if(is_embed) {
      const float sigma_nom = 0.0129;
      const float sigma_sys = (is_up) ? sigma_nom + 0.002 : sigma_nom - 0.002;
      const float nom = dscb(rel_err, 0.0005, sigma_nom, 1.150, 11.068, 1.103, 11.693);
      const float sys = dscb(rel_err, 0.0005, sigma_sys, 1.150, 11.068, 1.103, 11.693);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    } else {
      const float sigma_nom = 0.0134;
      const float sigma_sys = (is_up) ? sigma_nom + 0.002 : sigma_nom - 0.002;
      const float nom = dscb(rel_err, -0.0001, sigma_nom, 1.093, 3.455, 1.176, 4.203);
      const float sys = dscb(rel_err, -0.0001, sigma_sys, 1.093, 3.455, 1.176, 4.203);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    }
  } else { //electron resolution
    if(is_embed) {
      const float sigma_nom = 0.0112;
      const float sigma_sys = (is_up) ? sigma_nom + 0.003 : sigma_nom - 0.003;
      const float nom = dscb(rel_err, 0.0016, sigma_nom, 0.653, 9.203, 1.053, 4.443);
      const float sys = dscb(rel_err, 0.0016, sigma_sys, 0.653, 9.203, 1.053, 4.443);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    } else {
      const float sigma_nom = 0.0196;
      const float sigma_sys = (is_up) ? sigma_nom + 0.003 : sigma_nom - 0.003;
      const float nom = dscb(rel_err, -0.0027, sigma_nom, 0.896, 6.727, 1.405, 5.306);
      const float sys = dscb(rel_err, -0.0027, sigma_sys, 0.896, 6.727, 1.405, 5.306);
      weight = (sys/sigma_sys) / (nom/sigma_nom);
    }
  }
  weight = std::min(3.f, std::max(0.3f, weight));
  return weight;
}

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

/**
   isMC:
   0 = Data
   1 = Embedding
   2 = MC
 **/
void resolution(int isMC = 1, bool isMuon = true, int year = 2016, int period = -1) {

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/embed_tnp/";

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
  const double gap_low(1.4442), gap_high(1.566);

  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  TH1* hMass      = new TH1D("mass"     , "Di-lepton mass; Mass (GeV/c^{2})"    , 60, 60., 120.);
  TH1* hMassUp    = new TH1D("massup"   , "Di-lepton mass; Mass (GeV/c^{2})"  , 60, 60., 120.);
  TH1* hMassDown  = new TH1D("massdown" , "Di-lepton mass; Mass (GeV/c^{2})", 60, 60., 120.);
  TH1* hGenMass   = new TH1D("genmass"  , "Z gen-level mass; Mass (GeV/c^{2})", 60, 60., 120.);
  TH1* hPt        = new TH1D("pt"       , "Di-lepton p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hGenPt     = new TH1D("genpt"    , "Z gen-level p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hEta       = new TH1D("eta"      , "Di-lepton #eta; #eta", 50, -5., 5.);
  TH1* hGenEta    = new TH1D("geneta"   , "Z gen-level #eta; #eta", 50, -5., 5.);
  TH1* hOnePt     = new TH1D("onept"    , "Lead p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoPt     = new TH1D("twopt"    , "Trail p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOnePtUp   = new TH1D("oneptup"  , "Lead p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoPtUp   = new TH1D("twoptup"  , "Trail p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOnePtDown = new TH1D("oneptdown", "Lead p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoPtDown = new TH1D("twoptdown", "Trail p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOneGenPt  = new TH1D("onegenpt" , "Lead gen-level p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hTwoGenPt  = new TH1D("twogenpt" , "Trail gen-level p_{T}; p_{T} (GeV/c)", 50, 0., 100.);
  TH1* hOnePtErr  = new TH1D("onepterr" , "Lead p_{T} error; #Deltap_{T} (GeV/c)", 50, -5., 5.);
  TH1* hTwoPtErr  = new TH1D("twopterr" , "Lead p_{T} error; #Deltap_{T} (GeV/c)", 50, -5., 5.);
  TH1* hOneRelErr = new TH1D("onerelerr", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 50, -0.2, 0.2);
  TH1* hTwoRelErr = new TH1D("tworelerr", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 50, -0.2, 0.2);
  TH1* hOneRelErrUp = new TH1D("onerelerrup", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 50, -0.2, 0.2);
  TH1* hTwoRelErrUp = new TH1D("tworelerrup", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 50, -0.2, 0.2);
  TH1* hOneRelErrDown = new TH1D("onerelerrdown", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 50, -0.2, 0.2);
  TH1* hTwoRelErrDown = new TH1D("tworelerrdown", "Lead relative p_{T} error; #Deltap_{T} / p_{T}", 50, -0.2, 0.2);

  //lepton resolution in bins of pT
  const double pt_bins[] = {10., 20., 30., 40., 50., 70., 150.};
  const int n_pt_bins = sizeof(pt_bins) / sizeof(*pt_bins) - 1;
  TH2* hLepPtVsRes = new TH2D("lepptvsres", "p_{T} resolution in p_{T} bins; #Deltap_{T} / p_{T}", n_pt_bins, pt_bins, 50, -0.2, 0.2);

  ///////////////////////////////////////
  // File processing loop
  ///////////////////////////////////////

  ULong64_t nused(0), ntotal(0);
  TFile* f;
  for(TString run : runs) {
    if     (isMC == 0) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Single%sRun%i%s_%i.root", path.Data(), (isMuon) ? "Muon" : "Electron", year, run.Data(), year), "READ");
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
    CLFV::Utilities::SetBranchAddress(t, "one_id1"        , &one_id1        );
    CLFV::Utilities::SetBranchAddress(t, "one_id2"        , &one_id2        );
    CLFV::Utilities::SetBranchAddress(t, "one_triggered"  , &one_triggered  );
    CLFV::Utilities::SetBranchAddress(t, "two_pt"         , &two_pt         );
    CLFV::Utilities::SetBranchAddress(t, "two_eta"        , &two_eta        );
    CLFV::Utilities::SetBranchAddress(t, "two_sc_eta"     , &two_sc_eta     );
    CLFV::Utilities::SetBranchAddress(t, "two_phi"        , &two_phi        );
    CLFV::Utilities::SetBranchAddress(t, "two_q"          , &two_q          );
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

      TLorentzVector lv1; lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
      TLorentzVector lv2; lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
      TLorentzVector lv_sys = lv1 + lv2;

      //apply energy scale corrections to embedded electrons
      if(isMC == 1 && !isMuon) {
        const float scale_one = embed_energy_scale(one_sc_eta, year);
        const float scale_two = embed_energy_scale(one_sc_eta, year);
        one_pt *= scale_one;
        two_pt *= scale_two;
        lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
        lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
        lv_sys = lv1 + lv2;
        pair_mass = lv_sys.M();
        pair_pt = lv_sys.Pt();
        pair_eta = lv_sys.Eta();
      }

      //Z mass peak
      if(pair_mass < 60. || pair_mass > 120.) continue;

      //opposite flavor
      if(one_q*two_q > 0) continue;

      // electron eta veto
      if(!isMuon && gap_low <= fabs(one_sc_eta) && fabs(one_sc_eta) <= gap_high) continue;
      if(!isMuon && gap_low <= fabs(two_sc_eta) && fabs(two_sc_eta) <= gap_high) continue;

      //must fire the trigger
      if(!((one_pt > trig_pt_min && one_triggered) || (two_pt > trig_pt_min && two_triggered))) continue;

      //must satisfy minimum pT thresholds
      if(one_pt <= pt_min || two_pt <= pt_min) continue;

      //eta region cut
      if(fabs(one_eta) >= eta_max || fabs(two_eta) >= eta_max) continue;
      if(std::fabs(one_sc_eta) >= eta_max || std::fabs(two_sc_eta) >= eta_max) continue;

      //ensure each passes base IDs
      if(one_id1 < id1_min || one_id2 < id2_min ||
         two_id1 < id1_min || two_id2 < id2_min) continue;


      //make sure they're separated
      if(std::fabs(lv1.DeltaR(lv2)) < 0.5) continue;

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
        gen_one_pt  = one_pt ;
        gen_one_eta = one_eta;
        gen_one_phi = one_phi;
        gen_two_pt  = two_pt ;
        gen_two_eta = two_eta;
        gen_two_phi = two_phi;
        gen_mass    = pair_mass;
        gen_pt      = pair_pt;
        gen_eta     = pair_eta;
      }


      //Accept the event
      ++nused;
      if(debug_ && nused > nentries_) {
        if(debug_ == 1) return; //exit at this level
        break; //continue with a sub-sample
      }

      const float wt = pu_weight*((isMC > 1) ? ((gen_weight < 0) ? -1. : 1.) : gen_weight)*xs_scale;

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
      if(isMC) {
        TLorentzVector lv1_tmp, lv2_tmp;
        const float pt_err_scale = (isMuon || isMC == 2) ? 0.05 : 0.50;
        one_pt_up = one_pt + pt_err_scale*one_pt_err;
        two_pt_up = two_pt + pt_err_scale*two_pt_err;
        lv1_tmp.SetPtEtaPhiM(one_pt_up, one_eta, one_phi, lv1.M());
        lv2_tmp.SetPtEtaPhiM(two_pt_up, two_eta, two_phi, lv2.M());
        pair_mass_up = (lv1_tmp + lv2_tmp).M();
        one_pt_down = one_pt - pt_err_scale*one_pt_err;
        two_pt_down = two_pt - pt_err_scale*two_pt_err;
        lv1_tmp.SetPtEtaPhiM(one_pt_down, one_eta, one_phi, lv1.M());
        lv2_tmp.SetPtEtaPhiM(two_pt_down, two_eta, two_phi, lv2.M());
        pair_mass_down = (lv1_tmp + lv2_tmp).M();
      }

      // float res_wt_up(1.f), res_wt_down(1.f);
      // if(isMC) {
      //   if(use_rel_pt_) {
      //     res_wt_up   = rel_res_weight((one_pt - gen_one_pt)/one_pt, isMC == 1, isMuon, true );
      //     res_wt_down = rel_res_weight((one_pt - gen_one_pt)/one_pt, isMC == 1, isMuon, false);
      //   } else { //use absolute pT resolution on leading pT
      //     res_wt_up   = res_weight(one_pt - gen_one_pt, isMC == 1, isMuon, true );
      //     res_wt_down = res_weight(one_pt - gen_one_pt, isMC == 1, isMuon, false);
      //   }
      // }

      hMass    ->Fill(pair_mass, wt);
      hMassUp  ->Fill(pair_mass_up  , wt);
      hMassDown->Fill(pair_mass_down, wt);
      // hMassUp  ->Fill(pair_mass, wt*res_wt_up  );
      // hMassDown->Fill(pair_mass, wt*res_wt_down);
      hGenMass  ->Fill(gen_mass   , wt);
      hPt       ->Fill(pair_pt    , wt);
      hGenPt    ->Fill(gen_pt     , wt);
      hEta      ->Fill(pair_eta   , wt);
      hGenEta   ->Fill(gen_eta    , wt);
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
    } //end tree processing loop

    delete t;
    f->Close();
    cout << "Closed completed file " << f->GetName() << endl;
  }
  cout << "Used " << nused << " of the " << ntotal << " entries (" << (100.*nused)/ntotal << "%)\n";

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);
  //Base output name
  const char* outdir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", outdir, outdir));
  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");

  //write the histograms to an output file
  TFile* fout = new TFile(Form("histograms/%s_mc-%i_%i.hist", (isMuon) ? "mumu" : "ee", isMC, year), "RECREATE");
  hMass->Write();
  hMassUp->Write();
  hMassDown->Write();
  hGenMass->Write();
  hPt->Write();
  hGenPt->Write();
  hEta->Write();
  hGenEta->Write();
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

  print_figure(hMass      , Form("%s/mass.png"       , outdir));
  print_figure(hMassUp    , Form("%s/massup.png"     , outdir));
  print_figure(hMassDown  , Form("%s/massdown.png"   , outdir));
  print_figure(hGenMass   , Form("%s/genmass.png"    , outdir));
  print_figure(hPt        , Form("%s/pt.png"         , outdir));
  print_figure(hGenPt     , Form("%s/genpt.png"      , outdir));
  print_figure(hEta       , Form("%s/eta.png"        , outdir));
  print_figure(hGenEta    , Form("%s/geneta.png"     , outdir));
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

  if(isMC) {
    TString dir;
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);

    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hOnePtErr , Form("%s/one_pterr_fit.png" , dir.Data()), "One pT error", false);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hTwoPtErr , Form("%s/two_pterr_fit.png" , dir.Data()), "Two pT error", false);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hOneRelErr, Form("%s/one_relerr_fit.png", dir.Data()), "One rel. pT error", true);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hTwoRelErr, Form("%s/two_relerr_fit.png", dir.Data()), "Two rel. pT error", true);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hOneRelErrUp, Form("%s/one_relerr_up_fit.png", dir.Data()), "One rel. pT error", true);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hTwoRelErrUp, Form("%s/two_relerr_up_fit.png", dir.Data()), "Two rel. pT error", true);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hOneRelErrDown, Form("%s/one_relerr_down_fit.png", dir.Data()), "One rel. pT error", true);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    fit_resolution(hTwoRelErrDown, Form("%s/two_relerr_down_fit.png", dir.Data()), "Two rel. pT error", true);
    dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);

    //perform a fit for each of the pT binned resolution histograms
    double pt_centers[n_pt_bins], pt_errs[n_pt_bins];
    double widths[n_pt_bins], width_errs[n_pt_bins];
    double means[n_pt_bins], mean_errs[n_pt_bins];
    double max_width(0.), min_width(999.);
    for(int ibin = 1; ibin <= n_pt_bins; ++ibin) {
      TH1* h = hLepPtVsRes->ProjectionY(Form("_bin_%i", ibin), ibin, ibin);
      pt_centers[ibin-1] = (pt_bins[ibin] + pt_bins[ibin-1])/2.;
      pt_errs[ibin-1] = (pt_bins[ibin] - pt_bins[ibin-1])/2.;
      fit_resolution(h, Form("%s/lep_res_fit_bin_%i.png", dir.Data(), ibin), "Lep rel. pT error", true,
                     &(widths[ibin-1]), &(width_errs[ibin-1]),
                     &(means [ibin-1]), &(mean_errs [ibin-1]));
      max_width = max(widths[ibin-1], max_width);
      min_width = min(widths[ibin-1], min_width);
      dir = Form("figures/%s_mc-%i_%i", (isMuon) ? "mumu" : "ee", isMC, year);
    }
    //plot the means
    {
      TGraph* g = new TGraphErrors(n_pt_bins, pt_centers, means, pt_errs, mean_errs);
      TCanvas* c = new TCanvas();
      g->SetMarkerSize(0.8);
      g->SetMarkerStyle(20);
      g->SetLineWidth(2);
      g->SetTitle("Lepton p_{T} vs. resolution #mu; p_{T}; #mu");
      g->SetName("g_means");
      g->Draw("APE");
      c->Modified(); c->Update();
      c->SaveAs((dir +"/lep_res_pt_vs_mean.png").Data());
      delete c;
      g->Write();
    }
    //plot the fit of the widths
    {
      TGraph* gwidths = new TGraphErrors(n_pt_bins, pt_centers, widths, pt_errs, width_errs);
      TCanvas* c = new TCanvas();
      gwidths->SetMarkerSize(0.8);
      gwidths->SetMarkerStyle(20);
      gwidths->SetLineWidth(2);
      gwidths->SetTitle("Lepton p_{T} vs. resolution #sigma; p_{T}; #sigma");
      gwidths->SetName("g_widths");
      gwidths->Draw("APE");
      if(!isMuon) {
        TF1* fwidths = new TF1("f_widths", "[slope]*x^[power] + [flat]", 0., pt_bins[n_pt_bins]);
        fwidths->SetParameters(0.01, -4., 0.01);
        fwidths->SetParLimits(0, 0., 0.1);
        fwidths->SetParLimits(1, -20., 0.);
        fwidths->SetParLimits(2, 0., 1e4);
        gwidths->Fit(fwidths);
      } else {
        TF1* fwidths = new TF1("f_widths", "[slope]*(x - [xmin])^2 + [flat]", 0., pt_bins[n_pt_bins]);
        fwidths->SetParameters(0.01, 1.e-5, 25.);
        fwidths->SetParLimits(0, 0., 0.1);
        fwidths->SetParLimits(1, 0., 10.);
        fwidths->SetParLimits(2, -100, 150.);
        gwidths->Fit(fwidths);
      }
      gwidths->GetYaxis()->SetRangeUser(min_width - 0.1*(max_width - min_width), max_width + 0.1*(max_width - min_width));
      c->Modified(); c->Update();
      c->SaveAs((dir +"/lep_res_pt_vs_width.png").Data());
      delete c;
      gwidths->Write();
    }
  }

  printf("Resolution re-weighting check: nominal = %.3f, up = %.3f, down = %.3f, up/down = %.3f, nom/up = %.3f\n",
         hMass->Integral(), hMassUp->Integral(), hMassDown->Integral(),
         hMassUp->Integral() / hMassDown->Integral(),
         hMass  ->Integral() / hMassUp  ->Integral());
  fout->Close();
}
