//Produce scale factors to correct the leading order Drell-Yan MC

Long64_t max_entries_ = 1e7;

const static int nhist = 4;
int year_;

float min_mass_ = -1.;  //Z mass range to perform the correction in
float max_mass_ = 1.e5;

TH2* h_    [nhist]; //scale factors
TH1* hpt_  [nhist];
TH1* hmass_[nhist];
TH1* heta_ [nhist];
TH1* hlpt_ [nhist];
TH1* hlet_ [nhist];
TH1* hleta_[nhist];
TH1* hwt_  [nhist];
TH1* hmass_mode_[nhist*3]; //separate the Z in vs not in the gen-part list events
TH1* hpt_mode_  [nhist*3]; //separate the Z in vs not in the gen-part list events
TH1* heta_mode_ [nhist*3]; //separate the Z in vs not in the gen-part list events


int print_mass_mode_figure(TH1** h, const int index, const char* outname) {
  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();

  h[3*index + 1]->SetLineStyle(kDashed); h[3*index + 1]->SetLineColor(kRed   - 7); h[3*index + 1]->SetLineWidth(h[3*index]->GetLineWidth());
  h[3*index + 2]->SetLineStyle(kDashed); h[3*index + 2]->SetLineColor(kGreen - 7); h[3*index + 2]->SetLineWidth(h[3*index]->GetLineWidth());
  //Drell-Yan
  h[3*index + 0]->Draw("hist");
  h[3*index + 1]->Draw("hist same");
  h[3*index + 2]->Draw("hist same");
  // //Signal
  // h[3]->Draw("hist same");
  // h[4]->Draw("E1 same");
  // h[5]->Draw("E1 same");
  const double max_val = max(h[3*index]->GetMaximum(), max(h[3*index + 1]->GetMaximum(), h[3*index + 2]->GetMaximum()));
  const double min_val = min(h[3*index]->GetMinimum(), min(h[3*index + 1]->GetMinimum(), h[3*index + 2]->GetMinimum()));
  h[0]->SetAxisRange(max(0.5*min_val, max_val/1.e6), max_val*5., "Y");
  pad1->SetLogy();

  TLegend* leg = new TLegend(0.6, 0.75, 0.89, 0.89);
  leg->AddEntry(h[3*index + 0], "Total");
  leg->AddEntry(h[3*index + 1], "Z found");
  leg->AddEntry(h[3*index + 2], "No Z");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  leg->Draw();

  pad2->cd();
  pad2->SetGrid();
  TH1* hRatio = (TH1*) h[3*index + 1]->Clone("hratio_tmp");
  hRatio->Divide(h[3*index + 0]);
  hRatio->Draw("hist");
  TH1* hRatio2 = (TH1*) h[3*index + 2]->Clone("hratio2_tmp");
  hRatio2->Divide(h[3*index + 0]);
  hRatio2->Draw("hist same");

  hRatio->SetTitle("");
  hRatio->GetYaxis()->SetTitle("Component/Total");
  hRatio->GetYaxis()->SetTitleSize(0.13);
  hRatio->GetYaxis()->SetTitleOffset(0.33);
  hRatio->GetYaxis()->SetLabelSize(0.08);
  hRatio->GetXaxis()->SetLabelSize(0.08);
  hRatio->GetYaxis()->SetRangeUser(1.e-5, 5.);
  pad2->SetLogy();

  c->SaveAs(outname);
  delete c;
  delete hRatio;
  delete hRatio2;

  return 0;
}

int print_figure(TH1* h[nhist], const char* outname, const bool logy = false) {
  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();
  h[0]->Draw("hist");
  h[1]->Draw("E1 same");
  h[2]->Draw("E1 same");
  const double max_val = max(h[0]->GetMaximum(), max(h[1]->GetMaximum(), h[2]->GetMaximum()));
  h[0]->SetAxisRange((logy) ? max_val/1.e6 : 0., max_val*((logy) ? 5. : 1.25), "Y");
  if(logy) pad1->SetLogy();

  TLegend* leg = new TLegend(0.6, 0.75, 0.89, 0.89);
  leg->AddEntry(h[0], "NLO MC");
  leg->AddEntry(h[1], "LO MC");
  leg->AddEntry(h[2], "Corrected LO MC");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  leg->Draw();

  pad2->cd();
  pad2->SetGrid();
  TH1* hRatio = (TH1*) h[1]->Clone("hratio_tmp");
  hRatio->Divide(h[0]);
  hRatio->Draw("E1");
  TH1* hRatio2 = (TH1*) h[2]->Clone("hratio2_tmp");
  hRatio2->Divide(h[0]);
  hRatio2->Draw("E1 same");

  hRatio->SetTitle("");
  hRatio->GetYaxis()->SetTitle("LO / NLO");
  hRatio->GetYaxis()->SetTitleSize(0.13);
  hRatio->GetYaxis()->SetTitleOffset(0.30);
  hRatio->GetYaxis()->SetLabelSize(0.08);
  hRatio->GetXaxis()->SetLabelSize(0.08);
  hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);

  TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");

  c->SaveAs(outname);
  delete c;

  return 0;
}

int print_slices(TH2* h, const bool proj_x, const char* outdir) {
  TCanvas* c = new TCanvas("c", "c", 1000, 700);

  c->SetGrid();
  const int nbins = (proj_x) ? h->GetNbinsY() : h->GetNbinsX();
  const char* proj_var = (proj_x) ? h->GetYaxis()->GetTitle() : h->GetXaxis()->GetTitle();
  const char* var      = (proj_x) ? h->GetXaxis()->GetTitle() : h->GetYaxis()->GetTitle();
  auto proj_axis = (proj_x) ? h->GetYaxis() : h->GetXaxis();
  gSystem->Exec(Form("[ ! -d %s ] && mkdir %s", outdir, outdir));
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    TH1* hproj = (proj_x) ? h->ProjectionX("_px", ibin, ibin) : h->ProjectionY("_py", ibin, ibin);
    hproj->SetLineColor(kBlue);
    hproj->SetLineWidth(2);
    hproj->SetMarkerStyle(20);
    hproj->SetMarkerSize(0.8);
    hproj->Draw("E1");
    hproj->SetTitle(Form("%.0f < %s < %.0f Projection", proj_axis->GetBinLowEdge(ibin), proj_var, proj_axis->GetBinLowEdge(ibin)+proj_axis->GetBinWidth(ibin)));
    hproj->SetXTitle(var);
    hproj->SetYTitle("NLO / LO");
    c->SaveAs(Form("%s/slice_%i.png", outdir, ibin));
    delete hproj;
  }
  delete c;

  return 0;
}

int fill_hist(TTree* tree, const int index) {
  float pt1, pt2, eta1, eta2, phi1, phi2, mass1, mass2, genwt(1.f);
  int   z_idx;
  tree->SetBranchStatus("*", 0);
  Utilities::SetBranchAddress(tree, "GenZll_LepOne_pt"  , &pt1  );
  Utilities::SetBranchAddress(tree, "GenZll_LepOne_eta" , &eta1 );
  Utilities::SetBranchAddress(tree, "GenZll_LepOne_phi" , &phi1 );
  Utilities::SetBranchAddress(tree, "GenZll_LepOne_mass", &mass1);
  Utilities::SetBranchAddress(tree, "GenZll_LepTwo_pt"  , &pt2  );
  Utilities::SetBranchAddress(tree, "GenZll_LepTwo_eta" , &eta2 );
  Utilities::SetBranchAddress(tree, "GenZll_LepTwo_phi" , &phi2 );
  Utilities::SetBranchAddress(tree, "GenZll_LepTwo_mass", &mass2);
  Utilities::SetBranchAddress(tree, "GenZll_Idx"        , &z_idx);
  Utilities::SetBranchAddress(tree, "genWeight"         , &genwt);
  Long64_t nentries = tree->GetEntries();
  TLorentzVector lv1, lv2;
  TFile* f = TFile::Open(Form("rootfiles/lo_dy_correction_%i.root", year_));
  TH2* hcorr = (f) ? (TH2*) f->Get("hratio") : nullptr;
  cout << " Input chain has " << nentries << " entries\n";
  if(max_entries_ > 0 && max_entries_ < nentries) cout << "--> Will only use " << max_entries_ << " entries\n";
  nentries = (max_entries_ > 0 && max_entries_ < nentries) ? max_entries_ : nentries;
  Long64_t nused = 0;
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 500000 == 0) printf(" Processing entry %12lld (%6.2f%%)\n", entry, entry*100./nentries);
    tree->GetEntry(entry);
    lv1.SetPtEtaPhiM(pt1, eta1, phi1, mass1);
    lv2.SetPtEtaPhiM(pt2, eta2, phi2, mass2);
    const float mass = (lv1+lv2).M();
    const float pt   = (lv1+lv2).Pt();
    const float eta  = (lv1+lv2).Eta();
    if(mass <= min_mass_ || mass >= max_mass_) continue;
    genwt = (genwt < 0.f) ? -1.f : 1.f;
    nused += (genwt < 0.f) ? -1 : 1;
    h_    [index]->Fill(mass, pt, genwt);
    hpt_  [index]->Fill(pt, genwt);
    hmass_[index]->Fill(mass, genwt);
    heta_ [index]->Fill(eta, genwt);
    hlpt_ [index]->Fill(pt1, genwt);
    hlpt_ [index]->Fill(pt2, genwt);
    hlet_ [index]->Fill(sqrt(pt1*pt1 + mass1*mass1), genwt);
    hlet_ [index]->Fill(sqrt(pt2*pt2 + mass2*mass2), genwt);
    hleta_[index]->Fill(eta1, genwt);
    hleta_[index]->Fill(eta2, genwt);
    hwt_  [index]->Fill(genwt);
    hmass_mode_[3*index + 0]->Fill(mass, genwt);
    if(z_idx >= 0) hmass_mode_[3*index + 1]->Fill(mass, genwt);
    else           hmass_mode_[3*index + 2]->Fill(mass, genwt);
    hpt_mode_  [3*index + 0]->Fill(pt, genwt);
    if(z_idx >= 0) hpt_mode_  [3*index + 1]->Fill(pt, genwt);
    else           hpt_mode_  [3*index + 2]->Fill(pt, genwt);
    heta_mode_ [3*index + 0]->Fill(eta);
    if(z_idx >= 0) heta_mode_ [3*index + 1]->Fill(eta, genwt);
    else           heta_mode_ [3*index + 2]->Fill(eta, genwt);

    if(index == 1) { //make plots of corrected signal
      const float wt = (hcorr) ? hcorr->GetBinContent(max(1, min(hcorr->GetXaxis()->FindBin(mass), hcorr->GetNbinsX())),
                                                      max(1, min(hcorr->GetYaxis()->FindBin(pt)  , hcorr->GetNbinsY()))) / genwt : 1.f/genwt;
      h_    [2]->Fill(mass, pt       , wt);
      hpt_  [2]->Fill(pt             , wt);
      hmass_[2]->Fill(mass           , wt);
      heta_ [2]->Fill((lv1+lv2).Eta(), wt);
      hlpt_ [2]->Fill(pt1            , wt/2.);
      hlpt_ [2]->Fill(pt2            , wt/2.);
      hlet_ [2]->Fill(sqrt(pt1*pt1 + mass1*mass1), wt/2.);
      hlet_ [2]->Fill(sqrt(pt2*pt2 + mass2*mass2), wt/2.);
      hleta_[2]->Fill(eta1           , wt/2.);
      hleta_[2]->Fill(eta2           , wt/2.);
      hwt_  [2]->Fill(wt*nentries    , 1./nentries);
    }
  }
  Int_t color = (index == 0) ? kBlue : kRed;
  h_    [index]->Scale(1./nused);
  hpt_  [index]->Scale(1./nused);
  hmass_[index]->Scale(1./nused);
  heta_ [index]->Scale(1./nused);
  hlpt_ [index]->Scale(1./nused);
  hlet_ [index]->Scale(1./nused);
  hleta_[index]->Scale(1./nused);
  hwt_  [index]->Scale(1./nused);
  hmass_mode_[3*index + 1]->Scale(1./nused);
  hmass_mode_[3*index + 2]->Scale(1./nused);
  hmass_mode_[3*index + 0]->Scale(1./nused);
  hpt_mode_  [3*index + 1]->Scale(1./nused);
  hpt_mode_  [3*index + 2]->Scale(1./nused);
  hpt_mode_  [3*index + 0]->Scale(1./nused);
  heta_mode_ [3*index + 1]->Scale(1./nused);
  heta_mode_ [3*index + 2]->Scale(1./nused);
  heta_mode_ [3*index + 0]->Scale(1./nused);

  h_    [index]->SetLineColor(color); h_    [index]->SetLineWidth(2); h_    [index]->SetMarkerStyle(20); h_    [index]->SetMarkerSize(0.8);
  hpt_  [index]->SetLineColor(color); hpt_  [index]->SetLineWidth(2); hpt_  [index]->SetMarkerStyle(20); hpt_  [index]->SetMarkerSize(0.8);
  hmass_[index]->SetLineColor(color); hmass_[index]->SetLineWidth(2); hmass_[index]->SetMarkerStyle(20); hmass_[index]->SetMarkerSize(0.8);
  heta_ [index]->SetLineColor(color); heta_ [index]->SetLineWidth(2); heta_ [index]->SetMarkerStyle(20); heta_ [index]->SetMarkerSize(0.8);
  hlpt_ [index]->SetLineColor(color); hlpt_ [index]->SetLineWidth(2); hlpt_ [index]->SetMarkerStyle(20); hlpt_ [index]->SetMarkerSize(0.8);
  hlet_ [index]->SetLineColor(color); hlet_ [index]->SetLineWidth(2); hlet_ [index]->SetMarkerStyle(20); hlet_ [index]->SetMarkerSize(0.8);
  hleta_[index]->SetLineColor(color); hleta_[index]->SetLineWidth(2); hleta_[index]->SetMarkerStyle(20); hleta_[index]->SetMarkerSize(0.8);
  hwt_  [index]->SetLineColor(color); hwt_  [index]->SetLineWidth(2); hwt_  [index]->SetMarkerStyle(20); hwt_  [index]->SetMarkerSize(0.8);
  hmass_mode_[3*index]->SetLineColor(kBlue); hmass_mode_[3*index]->SetLineWidth(3); hmass_mode_[3*index]->SetMarkerStyle(20); hmass_mode_[3*index]->SetMarkerSize(0.8);
  hpt_mode_  [3*index]->SetLineColor(kBlue); hpt_mode_  [3*index]->SetLineWidth(3); hpt_mode_  [3*index]->SetMarkerStyle(20); hpt_mode_  [3*index]->SetMarkerSize(0.8);
  heta_mode_ [3*index]->SetLineColor(kBlue); heta_mode_ [3*index]->SetLineWidth(3); heta_mode_ [3*index]->SetMarkerStyle(20); heta_mode_ [3*index]->SetMarkerSize(0.8);

  if(index == 1) {
    h_    [2]->Scale(1./nused);
    hpt_  [2]->Scale(1./nused);
    hmass_[2]->Scale(1./nused);
    heta_ [2]->Scale(1./nused);
    hlpt_ [2]->Scale(1./nused);
    hlet_ [2]->Scale(1./nused);
    hleta_[2]->Scale(1./nused);
    hwt_  [2]->Scale(1./nused);
    h_    [2]->SetLineColor(kGreen-7); h_    [2]->SetLineWidth(2); h_    [2]->SetMarkerStyle(20); h_    [2]->SetMarkerSize(0.8);
    hpt_  [2]->SetLineColor(kGreen-7); hpt_  [2]->SetLineWidth(2); hpt_  [2]->SetMarkerStyle(20); hpt_  [2]->SetMarkerSize(0.8);
    hmass_[2]->SetLineColor(kGreen-7); hmass_[2]->SetLineWidth(2); hmass_[2]->SetMarkerStyle(20); hmass_[2]->SetMarkerSize(0.8);
    heta_ [2]->SetLineColor(kGreen-7); heta_ [2]->SetLineWidth(2); heta_ [2]->SetMarkerStyle(20); heta_ [2]->SetMarkerSize(0.8);
    hlpt_ [2]->SetLineColor(kGreen-7); hlpt_ [2]->SetLineWidth(2); hlpt_ [2]->SetMarkerStyle(20); hlpt_ [2]->SetMarkerSize(0.8);
    hlet_ [2]->SetLineColor(kGreen-7); hlet_ [2]->SetLineWidth(2); hlet_ [2]->SetMarkerStyle(20); hlet_ [2]->SetMarkerSize(0.8);
    hleta_[2]->SetLineColor(kGreen-7); hleta_[2]->SetLineWidth(2); hleta_[2]->SetMarkerStyle(20); hleta_[2]->SetMarkerSize(0.8);
    hwt_  [2]->SetLineColor(kGreen-7); hwt_  [2]->SetLineWidth(2); hwt_  [2]->SetMarkerStyle(20); hwt_  [2]->SetMarkerSize(0.8);
  }

  printf("Sample index %i:\n No Z fraction: %.5f\n", index, hmass_mode_[3*index+2]->Integral() / hmass_mode_[3*index]->Integral());
  if(min_mass_ < 75.f) {
    printf(" Mass fraction below 75 GeV: %.5f\n",
           hmass_[index]->Integral(0, hmass_[index]->FindBin(75.))/hmass_[index]->Integral(0, hmass_[index]->GetNbinsX()+1));
    printf(" Z found below 75 GeV fraction: %.5f\n",
           hmass_mode_[3*index+1]->Integral(0, hmass_mode_[3*index+1]->FindBin(75.))/hmass_mode_[3*index+1]->Integral(0, hmass_mode_[3*index+1]->GetNbinsX()+1));
    printf(" No Z below 75 GeV fraction: %.5f\n",
           hmass_mode_[3*index+2]->Integral(0, hmass_mode_[3*index+2]->FindBin(75.))/hmass_mode_[3*index+2]->Integral(0, hmass_mode_[3*index+2]->GetNbinsX()+1));
  }
  return 0;
}

int dy_lo_scale(int year) {
  year_ = year;

  min_mass_ =  50.f;
  max_mass_ = 500.f;
  cout << "Using Z mass cut: " << min_mass_ << " - " << max_mass_ << endl;
  TChain* bkg_chain = new TChain("Events", "NLO DY TChain");
  TChain* sig_chain = new TChain("Events", "LO DY TChain" );

  bkg_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_DY50*_%i.root", year));
  sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_LO-DY50*_%i.root", year));

  const double pt[] = {0., 2., 4., 6., 8., 10., 15., 20., 23., 26., 30., 35., 40., 50., 70., 100., 500.};
  double mass[] = {50., 70., 80., 100., 120., 500.}; //{50.,500.}
  const int npt = sizeof(pt)/sizeof(*pt) - 1;
  const int nmass = sizeof(mass)/sizeof(*mass) - 1;
  if(nmass == 2 && min_mass_ > mass[    0]) mass[    0] = min_mass_;
  if(nmass == 2 && max_mass_ < mass[nmass]) mass[nmass] = max_mass_;
  for(int index = 0; index < nhist; ++index) {
    h_    [index] = new TH2F(Form("h%i"     , index), "M_{Z} vs p^{Z}_{T}", nmass, mass, npt, pt);
    hpt_  [index] = new TH1F(Form("h%i_pt"  , index), " p^{Z}_{T}"        , 50,  0, 100);
    hmass_[index] = new TH1F(Form("h%i_mass", index), " M_{Z}"            , 50, 50, 150);
    heta_ [index] = new TH1F(Form("h%i_eta" , index), " #eta_{Z}"         , 50, -8,   8);
    hlpt_ [index] = new TH1F(Form("h%i_lpt" , index), " p^{l}_{T}"        , 50,  0, 100);
    hlet_ [index] = new TH1F(Form("h%i_le"  , index), " E^{l}_{T}"        , 50,  0, 100);
    hleta_[index] = new TH1F(Form("h%i_leta", index), " #eta_{l}"         , 50, -5,   5);
    hwt_  [index] = new TH1F(Form("h%i_wt"  , index), " Event weight"     , 50, -5,   5);
    for(int i = 0; i < 3; ++i) {
      hmass_mode_[3*index + i] = new TH1F(Form("h%i_mass_mode", 3*index + i), " M_{Z}", 50, 50, 150);
      hpt_mode_  [3*index + i] = new TH1F(Form("h%i_pt_mode"  , 3*index + i), " p^{Z}_{T}", 50, 0, 100);
      heta_mode_ [3*index + i] = new TH1F(Form("h%i_eta_mode" , 3*index + i), " #eta_{Z}", 50, -8, 8);
    }
  }

  cout << "Processing background chain...\n";
  fill_hist(bkg_chain, 0);
  cout << "Processing signal chain...\n";
  fill_hist(sig_chain, 1);

  if(h_[0]->GetEntries() == 0 || h_[1]->GetEntries() == 0) {
    cout << "No events found!\n";
    return 3;
  }

  TH2* hratio = (TH2*) h_[0]->Clone("hratio");
  hratio->Scale(h_[1]->Integral() / h_[0]->Integral());
  hratio->Divide(h_[1]);

  gSystem->Exec(Form("[ ! -d figures/lo_dy_%i ] && mkdir -p figures/lo_dy_%i", year, year));
  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas();
  gStyle->SetPaintTextFormat(".2f");
  hratio->Draw("colz text");
  hratio->SetXTitle("M_{Z}");
  hratio->SetYTitle("p^{Z}_{T}");
  c->SetLogx();
  c->SetLogy();
  c->SetLogz();
  hratio->GetXaxis()->SetMoreLogLabels(kTRUE);
  hratio->GetYaxis()->SetMoreLogLabels(kTRUE);
  c->SaveAs(Form("figures/lo_dy_ratio_%i.png", year));

  print_slices(hratio, true , Form("figures/lo_dy_%i/z_mass_proj", year));
  print_slices(hratio, false, Form("figures/lo_dy_%i/z_pt_proj"  , year));

  TH1* hdy  = h_[0]->ProjectionY("_py", 1, h_[0]->GetNbinsX());
  TH1* hsig = h_[1]->ProjectionY("_py", 1, h_[1]->GetNbinsX());
  hdy->SetLineColor(kRed);
  hdy->SetMarkerColor(kRed);
  hdy->SetMarkerStyle(6);
  hsig->SetLineColor(kBlue);
  hsig->SetMarkerColor(kBlue);
  hsig->SetMarkerStyle(6);
  hdy->Draw("E1");
  hsig->Draw("E1 same");
  hdy->GetYaxis()->SetRangeUser(0., 1.1*max(hdy->GetMaximum(), hsig->GetMaximum()));
  c->SetLogx(true);
  c->SetLogy(false);
  c->SetLogz(false);
  hdy->GetXaxis()->SetMoreLogLabels(true);
  hdy->GetYaxis()->SetMoreLogLabels(true);
  c->SaveAs(Form("figures/lo_dy_z_pt_%i.png", year));

  TH1* hprojy = hratio->ProjectionY("_py", 1, hratio->GetNbinsX());
  hprojy->Scale(1./hratio->GetNbinsX());
  hprojy->SetTitle("LO Drell-Yan p_{T}^{Z} corrections");
  hprojy->SetXTitle("p_{T}^{Z}");
  hprojy->SetYTitle("NLO / LO");
  hprojy->SetLineWidth(2);
  hprojy->SetMarkerStyle(6);
  hprojy->Draw("E1");
  c->SetLogx(true);
  c->SetLogy(false);
  c->SetLogz(false);
  hprojy->GetXaxis()->SetMoreLogLabels(true);
  hprojy->GetYaxis()->SetMoreLogLabels(true);
  hprojy->GetXaxis()->SetRangeUser(0.5, hprojy->GetBinCenter(hprojy->GetNbinsX()));
  c->SaveAs(Form("figures/lo_dy_z_pt_ratio_%i.png", year));

  delete c;

  TFile* fout = new TFile(Form("rootfiles/lo_dy_correction_%i.root", year), "RECREATE");
  hratio->Write();
  fout->Close();

  print_figure(hpt_  , Form("figures/lo_dy_%i/z_pt.png"   , year));
  print_figure(hmass_, Form("figures/lo_dy_%i/z_mass.png" , year), true);
  print_figure(heta_ , Form("figures/lo_dy_%i/z_eta.png"  , year));
  print_figure(hlpt_ , Form("figures/lo_dy_%i/lep_pt.png" , year));
  print_figure(hlet_ , Form("figures/lo_dy_%i/lep_et.png" , year));
  print_figure(hleta_, Form("figures/lo_dy_%i/lep_eta.png", year));
  print_figure(hwt_  , Form("figures/lo_dy_%i/weight.png" , year));
  print_mass_mode_figure(hmass_mode_, 0, Form("figures/lo_dy_%i/z_mass_mode_dy.png" , year));
  print_mass_mode_figure(hmass_mode_, 1, Form("figures/lo_dy_%i/z_mass_mode_sig.png", year));
  print_mass_mode_figure(hpt_mode_  , 0, Form("figures/lo_dy_%i/z_pt_mode_dy.png"   , year));
  print_mass_mode_figure(hpt_mode_  , 1, Form("figures/lo_dy_%i/z_pt_mode_sig.png"  , year));
  print_mass_mode_figure(heta_mode_ , 0, Form("figures/lo_dy_%i/z_eta_mode_dy.png"  , year));
  print_mass_mode_figure(heta_mode_ , 1, Form("figures/lo_dy_%i/z_eta_mode_sig.png" , year));

  return 0;
}
