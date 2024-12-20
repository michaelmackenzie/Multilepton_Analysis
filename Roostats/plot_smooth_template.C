// Plot the smoothed template result vs. input template

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
int plot_smooth_template(int set = 11) {

  //----------------------------------------------
  // Retrieve the input templates

  TFile* f = TFile::Open(Form("histograms/zemu_lepm_%i_2016_2017_2018.hist", set), "READ");
  if(!f) return 1;
  THStack* stack_in = (THStack*) f->Get("hstack");
  if(!stack_in) {
    cout << "Background stack for set " << set << " not found\n";
    return 2;
  }
  TString fig_dir = Form("plots/latest_production/2016_2017_2018/smooth_templates_%i", set);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fig_dir.Data(), fig_dir.Data()));

  //----------------------------------------------
  // Retrieve the histograms from  the stack

  const float xmin(70.), xmax(110.);
  TH1* hdy = nullptr;
  TH1* hww = nullptr;
  const int nhists(stack_in->GetNhists());
  for(int ihist = 0; ihist < nhists; ++ihist) {
    TH1* h = (TH1*) stack_in->GetHists()->At(ihist);
    h = make_safe(h, xmin, xmax);
    bool isflat(false);
    // isflat |= TString(h->GetName()).Contains("QCD"  );
    isflat |= TString(h->GetName()).Contains("Top"  );
    isflat |= TString(h->GetName()).Contains("Other");
    // isflat |= TString(h->GetName()).Contains("Z->ee");
    const bool isdy = TString(h->GetName()).Contains("#tau#tau");
    const bool isembed = TString(h->GetName()).Contains("Embed");
    const bool iszmumu = TString(h->GetName()).Contains("Z->ee");
    if(iszmumu) continue;
    if(isflat) {
      if(hww) hww->Add(h);
      else    hww = (TH1*) h->Clone("hww");
    } else if(isdy || isembed) {
      if(hdy) hdy->Add(h);
      else    hdy = (TH1*) h->Clone("hdy");
    }
  }
  if(!hdy || !hww) {
    cout << "Histograms not found from the stack!\n";
    return 3;
  }

  //----------------------------------------------
  // Create the PDFs and datasets

  RooRealVar obs("obs", "M_{e#mu}", 90., 70., 110., "GeV/c^{2}"); obs.setBins(80);

  // Gaussian PDF
  RooRealVar g_mean ("g_mean" , "g_mean" , 65., 60., 70.);
  RooRealVar g_width("g_width", "g_width",  8.,  5., 15.);
  RooGaussian gaus("gaus", "Gaussian PDF", obs, g_mean, g_width);

  // Polynomial PDF
  RooRealVar p_0("p_0", "p_0", 0.06, -1., 1.);
  RooRealVar p_1("p_1", "p_1", 0.  , -1., 1.);
  RooChebychev poly("poly", "Chebychev PDF", obs, RooArgList(p_0, p_1));

  // Total PDF
  RooRealVar n_g("n_g", "n_g", 0.95*hdy->Integral(), 0., 2.*hdy->Integral());
  RooRealVar n_p("n_p", "n_p", hww->Integral(), 0., 2.*hdy->Integral());
  RooAddPdf pdf("pdf", "Total PDF", RooArgList(gaus, poly), RooArgList(n_g, n_p));

  // Input data
  TH1* htot = (TH1*) hdy->Clone("htot"); htot->Add(hww);
  RooDataHist data("data", "Simulated data", obs, htot);

  //----------------------------------------------
  // Perform the fits

  pdf.fitTo(data);

  //----------------------------------------------
  // Plot the fits

  auto frame = obs.frame();
  data.plotOn(frame);
  pdf.plotOn(frame);
  pdf.plotOn(frame, RooFit::Components("poly"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  pdf.plotOn(frame, RooFit::Components("gaus"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

  TCanvas c;
  frame->Draw();
  frame->SetTitle("Simulated background distribution fits");
  c.SaveAs(Form("%s/fit.png", fig_dir.Data()));

  return 0;
}
