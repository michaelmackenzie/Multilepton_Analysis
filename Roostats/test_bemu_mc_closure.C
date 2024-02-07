//Test Z->e+mu background fit closure when fitting the MC

bool   fit_flat_bkgs_ = true;  //fit flat-ish background contributions (WW, ttbar, QCD, and (not flat) Z->mumu)
bool   fit_dy_bkg_    = false; //fit the Z->tautau background
int    smooth_hists_  =   2;   //number of times to smooth non-fit background histograms
double zmumu_scale_   = -1.;   //if >= 0 scale the Z->ee/mumu contribution
TString tag_          = "";    //tag for output figure directory

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
void fit_and_replace(TH1* h, double xmin, double xmax, const char* fig_dir) {
  TF1* func;
  if(TString(h->GetName()).Contains("#tau#tau")){ //Z->tautau or tautau Embedding
    const int mode = 5; //Z->tautau background parameterization option
    if(mode == 0) { //exp fit
      func = new TF1("func", "exp([0] + [1]*(x-70))", xmin, xmax);
      func->SetParameters(std::log(h->Integral()/(xmax - xmin)*h->GetBinWidth(1)), -1.);
    } else if(mode == 1) { //2 exp fit
      func = new TF1("func", "exp([0] + [1]*(x-70)) + exp([2] + [3]*(x-70))", xmin, xmax);
      func->SetParameters(std::log(h->Integral()/(xmax - xmin)*h->GetBinWidth(1)), -1., 1., -1.);
    } else if(mode == 5) { //exp fit + poly
      func = new TF1("func", "exp([0] + [1]*(x-70)) + [2] + [3]*x + [4]*x^2", xmin, xmax);
      func->SetParameters(std::log(h->Integral()/(xmax - xmin)*h->GetBinWidth(1)), -1., -1e3, 200., -1.);
    } else if(mode == 2) { //power law fit
      func = new TF1("func", "[0]*(x)^[1]", xmin, xmax);
      func->SetParameters(h->Integral()/(xmax - xmin)*h->GetBinWidth(1), -1.);
    } else if(mode == 3) { //exp + power law fit
      func = new TF1("func", "[0]*(x)^[1] + exp([2] + [3]*(x-70))", xmin, xmax);
      func->SetParameters(1., -1., 1., -1.);
      func->SetParLimits(0, 0., h->Integral()*1.2);
      func->SetParLimits(1, -20., 0.);
    } else if(mode == 4) { //double-sided Crystal Ball fit
      func = new TF1("func", dscb_func, xmin, xmax, 7, 1);
      func->SetParameters(h->Integral(), 60., 5., 1.056, 7.490, 1.000, 7.597);
      func->SetParNames("Norm", "#mu", "#sigma", "#alpha_{1}", "n_{1}", "#alpha_{2}", "n_{2}");
      func->SetParLimits(func->GetParNumber("#mu"), 10., 80.);
      func->SetParLimits(func->GetParNumber("#sigma"), 1., 30.);
      func->FixParameter(func->GetParNumber("n_{1}"), 5.);
      func->SetParLimits(func->GetParNumber("n_{2}"), 0.2, 30.);
      func->FixParameter(func->GetParNumber("#alpha_{1}"), 1.);
      func->FixParameter(func->GetParNumber("#alpha_{2}"), 1.);
    } else {
      return;
    }
  } else if(TString(h->GetName()).Contains("Z->ee")){ //Z->ee/mumu
    func = new TF1("func", dscb_func, xmin, xmax, 7, 1);
    func->SetParameters(h->Integral(), h->GetMean(), 5., 1.056, 7.490, 1.000, 7.597);
    func->SetParNames("Norm", "#mu", "#sigma", "#alpha_{1}", "n_{1}", "#alpha_{2}", "n_{2}");
    func->SetParLimits(func->GetParNumber("#mu"), 75., 105.);
    func->SetParLimits(func->GetParNumber("#sigma"), 3., 30.);
    func->SetParLimits(func->GetParNumber("n_{1}"), 0.2, 10.);
    func->SetParLimits(func->GetParNumber("n_{2}"), 0.2, 10.);
    func->SetParLimits(func->GetParNumber("#alpha_{1}"), 0.8, 5.);
    func->SetParLimits(func->GetParNumber("#alpha_{2}"), 0.5, 5.);
  } else { //flat-ish contributions, such as WW, ttbar, and QCD
    func = new TF1("func", "[0] + [1]*x + [2]*x^2", xmin, xmax);
    func->SetParameters(h->Integral()/(xmax - xmin)*h->GetBinWidth(1), 1., 1.);
  }

  //Fit the input histogram
  h->Fit(func, "R");

  //Save a figure with the input histogram and the resulting fit
  TCanvas* c = new TCanvas();
  h->Draw("E1");
  func->Draw("same");
  h->GetXaxis()->SetRangeUser(xmin, xmax);
  h->GetYaxis()->SetRangeUser(0., 1.2*h->GetMaximum());
  h->SetLineWidth(2);
  if(h->GetLineColor() == kYellow - 7) { //make the Top histogram more visible
    h->SetLineColor(kOrange);
    h->SetFillColor(kOrange);
    h->SetMarkerColor(kOrange);
  }
  TString fig_name = Form("%s_fit", h->GetName());
  fig_name.ReplaceAll("#", "");
  fig_name.ReplaceAll("->", "");
  fig_name.ReplaceAll("/", "");
  c->SaveAs(Form("%s/%s.png", fig_dir, fig_name.Data()));

  //Replace the bin values with the fit yield
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
    h->SetBinContent(bin, max(0., func->Eval(h->GetBinCenter(bin))));
  }

  //clean up the memory
  delete c;
  delete func;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Main function: test the background fit closure of the MC
int test_bemu_mc_closure(int set = 13, vector<int> years = {2016,2017,2018}, const int index = -1) {

  if(years.size() == 0) return -1;
  TString years_s = Form("%i", years[0]);
  for(unsigned i = 1; i < years.size(); ++i) years_s += Form("_%i", years[i]);

  //for outpur figures
  const char* fig_dir = Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data());
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fig_dir, fig_dir));
  gStyle->SetOptFit(1110);

  //Open the workspace file
  const char* fname = Form("datacards/%s/combine_bemu_zemu_%i.root", years_s.Data(), set);
  TFile* f = TFile::Open(fname, "READ");
  if(!f) return -1;

  //Retrieve the workspace
  RooWorkspace* ws = (RooWorkspace*) f->Get(Form("lepm_%i", set));
  if(!ws) return 1;

  //Retrieve the signal PDF
  RooAbsPdf* sigPDF = ws->pdf(Form("zemu"));
  if(!sigPDF) {ws->Print(); return 2;}

  //Retrieve the signal PDF
  // RooAbsPdf* bkgPDF = ws->pdf(Form("bkg"));
  RooAbsPdf* bkgPDF = ws->pdf(Form("bst_%i_order_3", set));
  if(!bkgPDF) {ws->Print(); return 3;}

  //Get the observable
  RooRealVar* obs = (RooRealVar*) ws->var(Form("lepm_%i", set));
  if(!obs) return 4;

  //Get the envelope index variable
  RooCategory* cat = (RooCategory*) ws->obj(Form("cat_%i", set));
  if(!cat) return 5;
  cat->setConstant(true);

  //turn off the energy scale uncertainties that are not currently constrained
  auto elec_es_shift = ws->var("elec_ES_shift");
  if(elec_es_shift) elec_es_shift->setConstant(true);
  auto muon_es_shift = ws->var("muon_ES_shift");
  if(muon_es_shift) muon_es_shift->setConstant(true);

  //Open the MC histogram file
  TFile* fMC = TFile::Open(Form("histograms/zemu_lepm_%i_%s.hist", set, years_s.Data()), "READ");
  if(!fMC) return -1;

  //Get the MC background stack
  THStack* stack_in = (THStack*) fMC->Get("hstack");
  if(!stack_in) return -2;

  //Replace background distributions where needed
  const int rebin = 2; //rebin the distributions to help statistical uncertainty
  THStack* stack = new THStack("bkg_stack", "Background stack");
  for(int ihist = 0; ihist < stack_in->GetNhists(); ++ihist) {
    TH1* h = (TH1*) stack_in->GetHists()->At(ihist);
    h->Rebin(rebin);
    h = make_safe(h, obs->getMin(), obs->getMax());
    cout << h->GetName() << ": " << h->GetTitle() << endl;
    bool isflat(false);
    isflat |= TString(h->GetName()).Contains("QCD"  );
    isflat |= TString(h->GetName()).Contains("Top"  );
    isflat |= TString(h->GetName()).Contains("Other");
    isflat |= TString(h->GetName()).Contains("Z->ee");
    bool isdy = TString(h->GetName()).Contains("#tau#tau");
    if(isflat) { //flat-ish distributions
      if(fit_flat_bkgs_) {
        fit_and_replace(h, obs->getMin(), obs->getMax(), Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data()));
      } else if(smooth_hists_ > 0) h->Smooth(smooth_hists_);
    }
    if(isdy) { //Z->tautau
      if(fit_dy_bkg_) {
        fit_and_replace(h, obs->getMin(), obs->getMax(), Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data()));
      } else if(smooth_hists_ > 0) h->Smooth(smooth_hists_);
    }
    if(!isflat && !isdy && smooth_hists_ > 0) h->Smooth(smooth_hists_); //any leftover histogram
    if(zmumu_scale_ >= 0. && TString(h->GetName()).Contains("Z->ee")) h->Scale(zmumu_scale_);
    stack->Add(h);
  }

  //plot the overall background stack
  {
    TCanvas* c = new TCanvas();
    // stack_in->Draw("hist noclear");
    stack->Draw("hist noclear");
    c->SaveAs(Form("%s/stack.png", Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data())));
    delete c;
  }

  //Get the MC histogram for generating data
  TH1* hMC = (TH1*) stack->GetStack()->Last()->Clone("hbkg");
  hMC = make_safe(hMC, obs->getMin(), obs->getMax());
  obs->setBins(hMC->GetNbinsX()); //ensure the binning matches
  const int n_init_mc = hMC->Integral() + 0.5;
  RooDataHist mc("mc", "MC data", RooArgList(*obs), hMC);
  RooHistPdf mcPDF("mcPDF", "MC PDF", *obs, mc);
  mcPDF.fitTo(mc, RooFit::Extended(1));

  //Add the signal and background PDFs to make a total PDF
  RooRealVar* n_sig  = new RooRealVar("n_sig", "N(signal)", 0., -n_init_mc, n_init_mc);
  RooRealVar* n_bkg  = new RooRealVar("n_bkg", "N(background)", n_init_mc, 0., 5.*n_init_mc);
  RooAddPdf* totPDF = new RooAddPdf("total_PDF", "total PDF", RooArgList(*bkgPDF, *sigPDF), RooArgList(*n_bkg, *n_sig), false);


  //Datasets to generate
  const int ngen = 1000;

  //Output info
  const double sig_range = max(1000., 5.*std::sqrt(n_init_mc));
  TH1* hNSig = new TH1D("nsig", "N(signal) fit; N(signal)", 200, -sig_range, sig_range);
  TH1* hNBkg = new TH1D("nbkg", "N(background) fit; N(background", 200,n_init_mc*0.7,n_init_mc*1.3);
  TH1* hPull = new TH1D("pull", "Signal pull; pull",100, -6., 6.);

  TRandom3 rnd(90);
  bool print_neg(true), print_pos(true);
  for(int igen = 0; igen < ngen; ++igen) {
    const int nmc = rnd.Poisson(hMC->Integral());
    RooDataHist* data = mcPDF.generateBinned(RooArgSet(*obs), nmc);
    auto fit_res = totPDF->fitTo(*data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Extended(1), RooFit::Save());
    if(fit_res->status() != 0) continue; //fit failed
    hNSig->Fill(n_sig->getVal());
    hNBkg->Fill(n_bkg->getVal());
    hPull->Fill(n_sig->getVal()/n_sig->getError());

    if(print_neg && n_sig->getVal() < 0.) { //print first negative fit
      auto frame = obs->frame();
      data->plotOn(frame);
      totPDF->plotOn(frame, RooFit::LineColor(kRed));
      totPDF->plotOn(frame, RooFit::Components(bkgPDF->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      totPDF->plotOn(frame, RooFit::Components(sigPDF->GetName()), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
      TCanvas* c = new TCanvas();
      frame->Draw();
      c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/fit_neg.png", years_s.Data(), set, tag_.Data()));
      print_neg = false;
    }
    if(print_pos && n_sig->getVal() > 0.) { //print first positive fit
      auto frame = obs->frame();
      data->plotOn(frame);
      totPDF->plotOn(frame, RooFit::LineColor(kRed));
      totPDF->plotOn(frame, RooFit::Components(bkgPDF->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      totPDF->plotOn(frame, RooFit::Components(sigPDF->GetName()), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
      TCanvas* c = new TCanvas();
      frame->Draw();
      c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/fit_pos.png", years_s.Data(), set, tag_.Data()));
      print_pos = false;
    }
  }

  TCanvas* c = new TCanvas();
  hNSig->Draw("hist");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/nsig.png", years_s.Data(), set, tag_.Data()));
  hNBkg->Draw("hist");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/nbkg.png", years_s.Data(), set, tag_.Data()));

  TF1* fgaus = new TF1("fgaus", "[norm]*TMath::Gaus(x, [mean], [sigma], true)", -6, 6);
  fgaus->SetParameters(1., 1., 1.);
  hPull->Fit(fgaus);
  hPull->Draw("hist");
  fgaus->Draw("same");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/pull.png", years_s.Data(), set, tag_.Data()));

  delete c;

  f->Close();
  fMC->Close();
  return 0;
}
