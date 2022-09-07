//Produce scale factors to correct the signal spectrum to the MC spectrum

Long64_t max_entries_ = 1e7;

const static int nhist = 4;
int year_;
int sig_mode_;

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
  h[0]->SetAxisRange((logy) ? max_val/1.e6 : 0., max_val*((logy) ? 5. : 1.1), "Y");
  if(logy) pad1->SetLogy();

  TLegend* leg = new TLegend(0.6, 0.75, 0.89, 0.89);
  leg->AddEntry(h[0], "DY MC");
  leg->AddEntry(h[1], "Signal MC");
  leg->AddEntry(h[2], "Corrected signal");
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
  hRatio->GetYaxis()->SetTitle("Signal/DY");
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
  TFile* f = TFile::Open(Form("rootfiles/signal_z_correction_s%i_%i.root", sig_mode_, year_));
  TH2* hcorr = (f) ? (TH2*) f->Get("hratio") : nullptr;
  cout << " Input chain has " << nentries << " entries\n";
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(max_entries_ > 0 && entry >= max_entries_) break;
    if(entry % 500000 == 0) printf(" Processing entry %12lld (%6.2f%%)\n", entry, entry*100./((max_entries_ > 0) ? max_entries_ : nentries));
    tree->GetEntry(entry);
    lv1.SetPtEtaPhiM(pt1, eta1, phi1, mass1);
    lv2.SetPtEtaPhiM(pt2, eta2, phi2, mass2);
    const float mass = (lv1+lv2).M();
    const float pt   = (lv1+lv2).Pt();
    const float eta  = (lv1+lv2).Eta();
    genwt = (genwt < 0.f) ? -1.f : 1.f;
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
                                                      max(1, min(hcorr->GetYaxis()->FindBin(pt)  , hcorr->GetNbinsY()))) / nentries*genwt : 1.f/nentries*genwt;
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
  h_    [index]->Scale(1./h_    [index]->Integral());
  hpt_  [index]->Scale(1./hpt_  [index]->Integral());
  hmass_[index]->Scale(1./hmass_[index]->Integral());
  heta_ [index]->Scale(1./heta_ [index]->Integral());
  hlpt_ [index]->Scale(1./hlpt_ [index]->Integral());
  hlet_ [index]->Scale(1./hlet_ [index]->Integral());
  hleta_[index]->Scale(1./hleta_[index]->Integral());
  hwt_  [index]->Scale(1./hwt_  [index]->Integral());
  hmass_mode_[3*index + 1]->Scale(1./hmass_mode_[3*index]->Integral());
  hmass_mode_[3*index + 2]->Scale(1./hmass_mode_[3*index]->Integral());
  hmass_mode_[3*index + 0]->Scale(1./hmass_mode_[3*index]->Integral());
  hpt_mode_  [3*index + 1]->Scale(1./hpt_mode_  [3*index]->Integral());
  hpt_mode_  [3*index + 2]->Scale(1./hpt_mode_  [3*index]->Integral());
  hpt_mode_  [3*index + 0]->Scale(1./hpt_mode_  [3*index]->Integral());
  heta_mode_ [3*index + 1]->Scale(1./heta_mode_ [3*index]->Integral());
  heta_mode_ [3*index + 2]->Scale(1./heta_mode_ [3*index]->Integral());
  heta_mode_ [3*index + 0]->Scale(1./heta_mode_ [3*index]->Integral());

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
    h_    [2]->SetLineColor(kGreen-7); h_    [2]->SetLineWidth(2); h_    [2]->SetMarkerStyle(20); h_    [2]->SetMarkerSize(0.8);
    hpt_  [2]->SetLineColor(kGreen-7); hpt_  [2]->SetLineWidth(2); hpt_  [2]->SetMarkerStyle(20); hpt_  [2]->SetMarkerSize(0.8);
    hmass_[2]->SetLineColor(kGreen-7); hmass_[2]->SetLineWidth(2); hmass_[2]->SetMarkerStyle(20); hmass_[2]->SetMarkerSize(0.8);
    heta_ [2]->SetLineColor(kGreen-7); heta_ [2]->SetLineWidth(2); heta_ [2]->SetMarkerStyle(20); heta_ [2]->SetMarkerSize(0.8);
    hlpt_ [2]->SetLineColor(kGreen-7); hlpt_ [2]->SetLineWidth(2); hlpt_ [2]->SetMarkerStyle(20); hlpt_ [2]->SetMarkerSize(0.8);
    hlet_ [2]->SetLineColor(kGreen-7); hlet_ [2]->SetLineWidth(2); hlet_ [2]->SetMarkerStyle(20); hlet_ [2]->SetMarkerSize(0.8);
    hleta_[2]->SetLineColor(kGreen-7); hleta_[2]->SetLineWidth(2); hleta_[2]->SetMarkerStyle(20); hleta_[2]->SetMarkerSize(0.8);
    hwt_  [2]->SetLineColor(kGreen-7); hwt_  [2]->SetLineWidth(2); hwt_  [2]->SetMarkerStyle(20); hwt_  [2]->SetMarkerSize(0.8);
  }
  return 0;
}

/**
   Main function
   sig_mode:
   0: Original LFV Z samples
   1: V2 LFV Z samples
   2: EXO centrally produced LFV Z samples
   3: V3 LFV Z samples, processing EXO sample with Legacy
 **/
int signal_scale(int year, const int sig_mode = 0) {
  year_ = year;
  sig_mode_ = sig_mode;
  TChain* bkg_chain = new TChain("Events", "DY TChain"    );
  TChain* sig_chain = new TChain("Events", "Signal TChain");

  bkg_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_DY50*_%i.root", year));
  if(sig_mode == 0) {
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_ZEMu_%i.root"  , year));
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_ZETau_%i.root" , year));
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_ZMuTau_%i.root", year));
  } else if(sig_mode == 1) {
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_ZEMu-v2_%i.root"  , year));
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_ZETau-v2_%i.root" , year));
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_ZMuTau-v2_%i.root", year));
  } else if(sig_mode == 2) {
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_UL-LFVZ_%i.root", year));
  } else if(sig_mode == 3) {
    sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_LFVZ-v2_%i.root", year));
  } else {
    cout << "Unknown signal mode " << sig_mode << endl;
    return -1;
  }

  // TFile* fbkg = TFile::Open(Form("rootfiles/z_bkg_%i.root", year), "READ");
  // TFile* fsig = TFile::Open(Form("rootfiles/z_sig_%i.root", year), "READ");
  // if(!fbkg || !fsig) {
  //   return 1;
  // }

  // TTree* tbkg = (TTree*) fbkg->Get("Events");
  // TTree* tsig = (TTree*) fsig->Get("Events");
  // if(!tbkg || !tsig) {
  //   cout << "Input trees not found!\n";
  //   return 2;
  // }

  const double pt[] = {0., 2., 4., 6., 8., 10., 15., 20., 25., 30., 40., 50., 100., 500.};
  const double mass[] = {50., 500.}; //{50., 70., 80., 100., 500.};
  const int npt = sizeof(pt)/sizeof(*pt) - 1;
  const int nmass = sizeof(mass)/sizeof(*mass) - 1;
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
  hratio->Divide(h_[1]);

  gSystem->Exec(Form("[ ! -d figures/signal_z_s%i_%i ] && mkdir -p figures/signal_z_s%i_%i", sig_mode, year, sig_mode, year));
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
  c->SaveAs(Form("figures/signal_z_ratio_s%i_%i.png", sig_mode, year));
  delete c;

  TFile* fout = new TFile(Form("rootfiles/signal_z_correction_s%i_%i.root", sig_mode, year), "RECREATE");
  hratio->Write();
  fout->Close();

  print_figure(hpt_  , Form("figures/signal_z_s%i_%i/z_pt.png"   , sig_mode, year));
  print_figure(hmass_, Form("figures/signal_z_s%i_%i/z_mass.png" , sig_mode, year), true);
  print_figure(heta_ , Form("figures/signal_z_s%i_%i/z_eta.png"  , sig_mode, year));
  print_figure(hlpt_ , Form("figures/signal_z_s%i_%i/lep_pt.png" , sig_mode, year));
  print_figure(hlet_ , Form("figures/signal_z_s%i_%i/lep_et.png" , sig_mode, year));
  print_figure(hleta_, Form("figures/signal_z_s%i_%i/lep_eta.png", sig_mode, year));
  print_figure(hwt_  , Form("figures/signal_z_s%i_%i/weight.png" , sig_mode, year));
  print_mass_mode_figure(hmass_mode_, 0, Form("figures/signal_z_s%i_%i/z_mass_mode_dy.png" , sig_mode, year));
  print_mass_mode_figure(hmass_mode_, 1, Form("figures/signal_z_s%i_%i/z_mass_mode_sig.png", sig_mode, year));
  print_mass_mode_figure(hpt_mode_  , 0, Form("figures/signal_z_s%i_%i/z_pt_mode_dy.png"   , sig_mode, year));
  print_mass_mode_figure(hpt_mode_  , 1, Form("figures/signal_z_s%i_%i/z_pt_mode_sig.png"  , sig_mode, year));
  print_mass_mode_figure(heta_mode_ , 0, Form("figures/signal_z_s%i_%i/z_eta_mode_dy.png"  , sig_mode, year));
  print_mass_mode_figure(heta_mode_ , 1, Form("figures/signal_z_s%i_%i/z_eta_mode_sig.png" , sig_mode, year));

  return 0;
}
