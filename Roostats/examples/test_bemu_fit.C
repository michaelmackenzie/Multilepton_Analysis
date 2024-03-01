//test fitting functions for bemu description

inline double evaluateCrystalBallTail(double t, double alpha, double n) {
  double a = std::pow(n / alpha, n) * std::exp(-0.5 * alpha * alpha);
  double b = n / alpha - alpha;

  double val = a / std::pow(b - t, n);
  if(std::isnan(val)) {
    val = 0.;
  }
  return val;
}

Double_t crystal_ball(const double* X, const double* P) {
  const double x = X[0];
  const double x0 = P[0];
  const double sigmaL = std::abs(P[1]);
  const double sigmaR = std::abs(P[2]);
  double alphaL = std::abs(P[3]);
  double nL = P[4];
  double alphaR = std::abs(P[5]);
  double nR = P[6];
  const double norm = P[7];
  const double t = (x - x0) / (x < x0 ? sigmaL : sigmaR);

  if (t < -alphaL) {
    return norm*evaluateCrystalBallTail(t, alphaL, nL);
  } else if (t <= alphaR) {
    return norm*std::exp(-0.5 * t * t);
  } else {
    return norm*evaluateCrystalBallTail(-t, alphaR, nR);
  }
}

Double_t modified_crystal_ball(const double* X, const double* P) {
  const double x = X[0];
  const double mean = P[8];
  const double sigma = abs(P[9]);
  const double fraction = P[10];
  const double p = P[7]*fraction*TMath::Gaus(x, mean, sigma) + (1. - fraction)*crystal_ball(X, P);
  return p;
}

inline double evaluateLandauCore(double x, double x0, double sigma, double sigma2) {
  return TMath::Gaus(x, x0, sigma2)*TMath::Landau(x0-x, 0., sigma);
}

inline double evaluateLandauTail(double x, double x0, double sigma, double sigma2, double alpha, double n) {
  const double t = abs((x - x0)/sigma2);
  const double b = abs(n / alpha - alpha);
  const double a = evaluateLandauCore(alpha*(1 - 2*(x<x0)), x0, sigma, sigma2) / std::pow(b + alpha, n);

  double val = a / std::pow(b + t, n);
  if(std::isnan(val)) {
    val = 0.;
  }
  return val;
}

Double_t modified_landau(const double* X, const double* P) {
  const double x = X[0];
  const double x0 = P[0];
  const double sigma  = std::abs(P[1]);
  const double sigma2 = std::abs(P[2]);
  double alphaL = std::abs(P[3]);
  double nL = P[4];
  double alphaR = std::abs(P[5]);
  double nR = P[6];
  const double norm = P[7];
  const double t = (x - x0) / (sigma2);

  if (t < -alphaL) {
    return norm*evaluateLandauTail(x, x0, sigma, sigma2, alphaL, nL);
  } else if (t <= alphaR) {
    return norm*evaluateLandauCore(x, x0, sigma, sigma2);
  } else {
    return norm*evaluateLandauTail(x, x0, sigma, sigma2, alphaR, nR);
  }
}

void test_bemu_fit(TString selection = "zemu", int Mode = 0,
                   int set = 8,
                   vector<int> years = {2016, 2017, 2018}) {

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));

  TFile* f = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "READ");
  if(!f) return;
  TH1D* h = (TH1D*) f->Get(selection.Data());
  if(!h) {
    cout << "Signal histogram not found!\n";
    return;
  }
  const double xmin = (selection.Contains("h")) ? 110. :  70.;
  const double xmax = (selection.Contains("h")) ? 150. : 110.;
  TF1* func;
  if(Mode == 0) {
    func = new TF1("func", crystal_ball, xmin, xmax, 8);
    func->SetParameters((xmin+xmax)/2., 1., 1., 1., 5., 1., 5., h->Integral());
    func->SetParNames("#mu", "#sigma_{L}", "#sigma_{R}", "#alpha_{L}",
                      "n_{L}", "#alpha_{R}", "n_{R}", "Norm");
  } else if(Mode == 1) {
    func = new TF1("func", modified_landau, xmin, xmax, 8);
    func->SetParameters((xmin+xmax)/2., 1., 1., 1., 1., 1., 1., h->Integral());
    func->SetParNames("#mu", "#sigma_{L}", "#sigma_{R}", "#alpha_{L}",
                      "n_{L}", "#alpha_{R}", "n_{R}", "Norm");
    func->SetParLimits(func->GetParNumber("#mu"), xmin, xmax);
    func->SetParLimits(func->GetParNumber("#sigma_{R}"), 0., (xmax-xmin));
    func->SetParLimits(func->GetParNumber("#sigma_{L}"), 0., (xmax-xmin));
    func->SetParLimits(func->GetParNumber("#alpha_{R}"), 0., 1000);
    func->SetParLimits(func->GetParNumber("#alpha_{L}"), 0., 1000);
    func->SetParLimits(func->GetParNumber("n_{L}"), 0., 10.);
    func->SetParLimits(func->GetParNumber("n_{R}"), 0., 10.);
  } else if(Mode == 2) {
    func = new TF1("func", modified_crystal_ball, xmin, xmax, 11);
    func->SetParameters((xmin+xmax)/2., 1., 1., 1., 5., 1., 5., h->Integral(), (xmin+xmax)/2., 5., 0.1);
    func->SetParNames("#mu", "#sigma_{L}", "#sigma_{R}", "#alpha_{L}",
                      "n_{L}", "#alpha_{R}", "n_{R}", "Norm", "#mu_{2}", "#sigma_{2}", "fraction");
  } else {
    cout << "Unknown fit Mode = " << Mode << endl;
    return;
  }

  TCanvas* c = new TCanvas();
  h->Fit(func, "R");
  h->SetLineWidth(2);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(0.8);
  h->SetLineColor(kBlue);
  h->SetMarkerColor(kBlack);
  h->Draw();
  h->GetXaxis()->SetRangeUser(xmin, xmax+10.);
  c->SaveAs(Form("plots/latest_production/%s/test_bemu_fit_%s_%i_%i.png", year_string.Data(), selection.Data(), set, Mode));
}
