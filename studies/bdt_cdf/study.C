//Create a CDF transform histogram for a signal BDT score distribution

int study(TString selection = "zmutau", const int set = 8, TString path = "nanoaods_cdf") {
  TString signal = selection;
  signal.ReplaceAll("_e", ""); signal.ReplaceAll("_mu", "");
  signal.ReplaceAll("z", "Z"); signal.ReplaceAll("e", "E"); signal.ReplaceAll("mu", "Mu"); signal.ReplaceAll("tau", "Tau");
  TString region = selection; region.ReplaceAll("z", ""); region.ReplaceAll("h", "");
  TFile* f1 = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/histograms/%s/clfv_%s_2016_%s.hist", path.Data(), region.Data(), signal.Data()), "READ");
  TFile* f2 = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/histograms/%s/clfv_%s_2017_%s.hist", path.Data(), region.Data(), signal.Data()), "READ");
  TFile* f3 = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/histograms/%s/clfv_%s_2018_%s.hist", path.Data(), region.Data(), signal.Data()), "READ");

  if(!f1 || !f2 || !f3) return 1;
  int offset(0), mva(0);
  if(selection.EndsWith("_mu"))        {offset = HistMaker::kETauMu; mva = 8;}
  else if(selection.EndsWith("_e"))    {offset = HistMaker::kMuTauE; mva = 6;}
  else if(selection.EndsWith("emu"))   {offset = HistMaker::kEMu   ; mva = 4;}
  else if(selection.EndsWith("etau"))  {offset = HistMaker::kETau  ; mva = 2;}
  else if(selection.EndsWith("mutau")) {offset = HistMaker::kMuTau ; mva = 0;}
  if(selection.BeginsWith("z")) ++mva;

  const int set_abs = set + offset;

  const char* hpath = Form("Data/event_%i/mva%i_1", set_abs, mva);
  TH1* h = (TH1*) f1->Get(hpath);
  if(!h) {
    cout << "MVA histogram " << hpath << " not found!\n";
    return 2;
  }
  h = (TH1*) h->Clone("hMVA");
  h->SetDirectory(0);
  h->Add((TH1*) f2->Get(hpath));
  h->Add((TH1*) f3->Get(hpath));
  f1->Close();
  f2->Close();
  f3->Close();

  h->SetTitle("MVA score");
  h->Scale(1./h->Integral());
  TH1* hcdf = (TH1*) h->Clone("CDF");
  hcdf->SetTitle("MVA CDF");

  double tot = 0.;
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
    tot += h->GetBinContent(bin);
    hcdf->SetBinContent(bin, tot);
  }

  TFile * fout = new TFile(Form("rootfiles/cdf_transform_%s.root", selection.Data()), "RECREATE");
  h->Write();
  hcdf->Write();

  TCanvas* c = new TCanvas();
  gStyle->SetOptStat(0);
  h->SetLineColor(kBlue);
  h->SetLineWidth(2);
  h->Scale(1./h->GetMaximum());
  h->Draw("hist");
  hcdf->SetLineColor(kRed);
  hcdf->SetLineWidth(2);
  hcdf->Draw("hist same");
  h->GetXaxis()->SetRangeUser(-1., 1.);
  h->GetYaxis()->SetRangeUser(h->GetMaximum()/1.e5, 1.1*h->GetMaximum());
  h->SetXTitle("MVA score");
  h->SetTitle("MVA score and CDF");

  const double fit_min = hcdf->GetBinLowEdge(1);
  const double fit_max = hcdf->GetBinLowEdge(hcdf->FindFirstBinAbove(5.e-3));
  TF1* fit_func = new TF1("fit_func", "exp([0]+x*[1])", fit_min, fit_max);
  fit_func->SetParameters(-0.5, 15.);
  hcdf->Fit(fit_func, "R");
  fit_func->SetLineStyle(kDashed);
  fit_func->SetLineColor(kGreen);
  fit_func->Draw("same");
  fit_func->Write();

  TLegend* leg = new TLegend(0.1, 0.75, 0.4, 0.9);
  leg->AddEntry(h, "MVA score");
  leg->AddEntry(hcdf, "MVA CDF");
  leg->Draw();

  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  c->SaveAs(Form("figures/cdf_%s_%i.png", selection.Data(), set));
  c->SetLogy();
  c->SaveAs(Form("figures/cdf_%s_%i_log.png", selection.Data(), set));

  fout->Close();

  return 0;
}
