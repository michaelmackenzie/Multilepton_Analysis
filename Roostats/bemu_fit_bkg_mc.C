//Take in a stack of the Z->e+mu backgrounds and return an updated stack with fit/smoothed background contributions

#ifndef __BEMU_FIT_MC__
#define __BEMU_FIT_MC__

#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"

int replace_high_bdt_ww_ = true; //replace the high BDT score WW fit with a lower score region fit normalized using the WW sidebands (account for WW training)

//---------------------------------------------------------------------------------------------------------------------------------------
// Ensure no negative bins and clip unused range
TH1* make_safe(TH1* h, double xmin, double xmax) {
  const int bin_low  = (xmin < xmax) ? h->FindBin(xmin+1.e-4) : 1;
  const int bin_high = (xmin < xmax) ? h->FindBin(xmax-1.e-4) : h->GetNbinsX();
  TH1* h_new = new TH1D(Form("%s_safe", h->GetName()), h->GetTitle(), (bin_high - bin_low) + 1,
                        h->GetBinLowEdge(bin_low), h->GetBinLowEdge(bin_high)+h->GetBinWidth(bin_high));
  for(int bin = bin_low; bin <= bin_high; ++bin) {
    h_new->SetBinContent(bin - bin_low + 1, max(0., h->GetBinContent(bin)));
    h_new->SetBinError  (bin - bin_low + 1, max(0., h->GetBinError  (bin)));
  }
  h_new->SetFillColor(h->GetFillColor());
  h_new->SetFillStyle(h->GetFillStyle());
  h_new->SetLineColor(h->GetLineColor());
  h_new->SetLineStyle(h->GetLineStyle());
  return h_new;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Double-sided Crystal Ball function
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

//---------------------------------------------------------------------------------------------------------------------------------------
// Double-sided Crystal Ball call for TF1
double dscb_func(double* X, double* P) {
  return P[0]*dscb(X[0], P[1], P[2], P[3], P[4], P[5], P[6]);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Fit a function to the histogram and replace the bin contents with the fit values
void fit_and_replace(TH1* h, double xmin, double xmax, const char* fig_dir = nullptr, int set = -1, int rebin = 0) {
  //histogram to fit, including rebinning if requested
  TH1* hfit = h;
  if(rebin > 1) { //rebin only for fitting
    hfit = (TH1*) hfit->Clone(Form("%s_rebinned", hfit->GetName()));
    hfit->Rebin(rebin);
  }

  //setup the function to fit with
  TF1* func;
  if(TString(h->GetName()).Contains("#tau#tau")){ //Z->tautau or tautau Embedding
    const int mode = 5; //Z->tautau background parameterization option
    if(mode == 0) { //exp fit
      func = new TF1("func", "exp([0] + [1]*(x-70))", xmin, xmax);
      func->SetParameters(std::log(hfit->Integral()/(xmax - xmin)*hfit->GetBinWidth(1)), -1.);
    } else if(mode == 1) { //2 exp fit
      func = new TF1("func", "exp([0] + [1]*(x-70)) + exp([2] + [3]*(x-70))", xmin, xmax);
      func->SetParameters(std::log(hfit->Integral()/(xmax - xmin)*hfit->GetBinWidth(1)), -1., 1., -1.);
    } else if(mode == 5) { //exp fit + poly
      func = new TF1("func", "exp([0] + [1]*(x-70)) + [2] + [3]*x + [4]*x^2", xmin, xmax);
      func->SetParameters(std::log(hfit->Integral()/(xmax - xmin)*hfit->GetBinWidth(1)), -1., -1e3, 200., -1.);
    } else if(mode == 2) { //power law fit
      func = new TF1("func", "[0]*(x)^[1]", xmin, xmax);
      func->SetParameters(hfit->Integral()/(xmax - xmin)*hfit->GetBinWidth(1), -1.);
    } else if(mode == 3) { //exp + power law fit
      func = new TF1("func", "[0]*(x)^[1] + exp([2] + [3]*(x-70))", xmin, xmax);
      func->SetParameters(1., -1., 1., -1.);
      func->SetParLimits(0, 0., hfit->Integral()*1.2);
      func->SetParLimits(1, -20., 0.);
    } else if(mode == 4) { //double-sided Crystal Ball fit
      func = new TF1("func", dscb_func, xmin, xmax, 7, 1);
      func->SetParameters(hfit->Integral(), 60., 5., 1.056, 7.490, 1.000, 7.597);
      func->SetParNames("Norm", "#mu", "#sigma", "#alpha_{1}", "n_{1}", "#alpha_{2}", "n_{2}");
      func->SetParLimits(func->GetParNumber("#mu"), 10., 80.);
      func->SetParLimits(func->GetParNumber("#sigma"), 1., 30.);
      func->FixParameter(func->GetParNumber("n_{1}"), 5.);
      func->SetParLimits(func->GetParNumber("n_{2}"), 0.2, 30.);
      func->FixParameter(func->GetParNumber("#alpha_{1}"), 1.);
      func->FixParameter(func->GetParNumber("#alpha_{2}"), 1.);
    } else {
      cout << __func__ << ": Unknown fitting mode " << mode << endl;
      return;
    }
  } else if(TString(h->GetName()).Contains("Z->ee")){ //Z->ee/mumu
    func = new TF1("func", dscb_func, xmin, 100., 7, 1);
    func->SetParameters(hfit->Integral(), 83., 5., 1.056, 7.490, 1.000, 7.597);
    // func->SetParameters(hfit->Integral(), 82.8, 5.1, 1., 0.33, 1.9, 1.1);
    func->SetParNames("Norm", "#mu", "#sigma", "#alpha_{1}", "n_{1}", "#alpha_{2}", "n_{2}");
    func->SetParLimits(func->GetParNumber("#mu"), 75., 90.);
    func->SetParLimits(func->GetParNumber("#sigma"), 3., 30.);
    func->SetParLimits(func->GetParNumber("n_{1}"), 0.2, 10.);
    func->SetParLimits(func->GetParNumber("n_{2}"), 0.2, 10.);
    func->SetParLimits(func->GetParNumber("#alpha_{1}"), 0.8, 5.);
    func->SetParLimits(func->GetParNumber("#alpha_{2}"), 0.5, 5.);
    func->Print();
  } else { //flat-ish contributions, such as WW, ttbar, and QCD
    func = new TF1("func", "[0] + [1]*x + [2]*x^2", xmin, xmax);
    func->SetParameters(hfit->Integral()/(xmax - xmin)*hfit->GetBinWidth(1), 1., 1.);
  }

  //Fit the input histogram
  if(replace_high_bdt_ww_ && (set % 100) == 13 && TString(h->GetName()).Contains("Other VB")) {
    //set the functions parameters to a fit using the medium BDT score region WW histogram
    func->SetParameters(2.53534e+01, 9.03065e-01, -7.41810e-03);
    //scale the parameters such that the integral matches the histogram sidebands
    const double nhist = hfit->Integral(h->FindBin(70.1), hfit->FindBin(84.9)) + hfit->Integral(hfit->FindBin(95.5), hfit->FindBin(109.9));
    const double nfunc = func->Integral(70., 85.) + func->Integral(95., 110.);
    const double scale = (nhist * hfit->GetBinWidth(1)) / nfunc;
    func->SetParameter(0, scale*func->GetParameter(0));
    func->SetParameter(1, scale*func->GetParameter(1));
    func->SetParameter(2, scale*func->GetParameter(2));
  } else {
    TFitResultPtr fit_res;
    bool refit = false;
    int fit_count(0);
    cout << __func__ << ": Fitting " << h->GetName() << " MC histogram\n";
    do {
      if(TString(h->GetName()).Contains("Z->ee")) {
        fit_res = hfit->Fit(func, "R S 0");
      } else {
        fit_res = hfit->Fit(func, "R S 0");
      }
      refit = fit_res && fit_res != 0;
      refit |= func->GetChisquare() / hfit->GetNbinsX() > 1.5;
      ++fit_count;
    } while(refit && fit_count <= 0);
  }

  if(fig_dir) {
    //Save a figure with the input histogram and the resulting fit
    TCanvas* c = new TCanvas();
    hfit->Draw("E1");
    func->Draw("same");
    hfit->GetXaxis()->SetRangeUser(xmin, xmax);
    hfit->GetYaxis()->SetRangeUser(0., 1.2*hfit->GetMaximum());
    hfit->SetLineWidth(2);
    if(hfit->GetLineColor() == kYellow - 7) { //make the Top histogram more visible
      hfit->SetLineColor(kOrange);
      hfit->SetFillColor(kOrange);
      hfit->SetMarkerColor(kOrange);
      h->SetLineColor(kOrange);
      h->SetFillColor(kOrange);
      h->SetMarkerColor(kOrange);
    }
    TString fig_name = Form("%s_fit", h->GetName());
    fig_name.ReplaceAll("#", "");
    fig_name.ReplaceAll("->", "");
    fig_name.ReplaceAll("/", "");
    c->SaveAs(Form("%s/%s.png", fig_dir, fig_name.Data()));

    //clean up the memory
    delete c;
  }
  if(rebin > 1) delete hfit;

  //Replace the bin values with the fit yield
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
    h->SetBinContent(bin, max(0., ((rebin > 1) ? 1./rebin : 1.) * func->Eval(h->GetBinCenter(bin))));
  }
  delete func;
}

#endif
