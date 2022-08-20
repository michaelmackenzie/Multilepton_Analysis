//Produce scale factors to correct the signal spectrum to the MC spectrum
const static int nhist = 4;
int year_;

TH2* h_    [nhist]; //scale factors
TH1* hpt_  [nhist];
TH1* hmass_[nhist];
TH1* heta_ [nhist];
TH1* hlpt_ [nhist];
TH1* hlet_ [nhist];
TH1* hleta_[nhist];


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
  float pt1, pt2, eta1, eta2, phi1, phi2, mass1, mass2;
  tree->SetBranchAddress("GenZll_LepOne_pt"  , &pt1  );
  tree->SetBranchAddress("GenZll_LepOne_eta" , &eta1 );
  tree->SetBranchAddress("GenZll_LepOne_phi" , &phi1 );
  tree->SetBranchAddress("GenZll_LepOne_mass", &mass1);
  tree->SetBranchAddress("GenZll_LepTwo_pt"  , &pt2  );
  tree->SetBranchAddress("GenZll_LepTwo_eta" , &eta2 );
  tree->SetBranchAddress("GenZll_LepTwo_phi" , &phi2 );
  tree->SetBranchAddress("GenZll_LepTwo_mass", &mass2);
  Long64_t nentries = tree->GetEntries();
  TLorentzVector lv1, lv2;
  TFile* f = TFile::Open(Form("rootfiles/signal_z_correction_%i.root", year_));
  TH2* hcorr = (f) ? (TH2*) f->Get("hratio") : nullptr;
  cout << " Input chain has " << nentries << " entries\n";
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 1000000 == 0) printf(" Processing entry %12lld (%6.2f%%)\n", entry, entry*100./nentries);
    tree->GetEntry(entry);
    lv1.SetPtEtaPhiM(pt1, eta1, phi1, mass1);
    lv2.SetPtEtaPhiM(pt2, eta2, phi2, mass2);
    const float mass = (lv1+lv2).M();
    const float pt   = (lv1+lv2).Pt();
    h_    [index]->Fill(mass, pt);
    hpt_  [index]->Fill(pt);
    hmass_[index]->Fill(mass);
    heta_ [index]->Fill((lv1+lv2).Eta());
    hlpt_ [index]->Fill(pt1);
    hlpt_ [index]->Fill(pt2);
    hlet_ [index]->Fill(sqrt(pt1*pt1 + mass1*mass1));
    hlet_ [index]->Fill(sqrt(pt2*pt2 + mass2*mass2));
    hleta_[index]->Fill(eta1);
    hleta_[index]->Fill(eta2);
    if(index == 1) { //make plots of corrected signal
      const float wt = (hcorr) ? hcorr->GetBinContent(max(1, min(hcorr->GetXaxis()->FindBin(mass), hcorr->GetNbinsX())),
                                                      max(1, min(hcorr->GetYaxis()->FindBin(pt)  , hcorr->GetNbinsY()))) / nentries : 1.f/nentries;
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

  h_    [index]->SetLineColor(color); h_    [index]->SetLineWidth(2); h_    [index]->SetMarkerStyle(20); h_    [index]->SetMarkerSize(0.8);
  hpt_  [index]->SetLineColor(color); hpt_  [index]->SetLineWidth(2); hpt_  [index]->SetMarkerStyle(20); hpt_  [index]->SetMarkerSize(0.8);
  hmass_[index]->SetLineColor(color); hmass_[index]->SetLineWidth(2); hmass_[index]->SetMarkerStyle(20); hmass_[index]->SetMarkerSize(0.8);
  heta_ [index]->SetLineColor(color); heta_ [index]->SetLineWidth(2); heta_ [index]->SetMarkerStyle(20); heta_ [index]->SetMarkerSize(0.8);
  hlpt_ [index]->SetLineColor(color); hlpt_ [index]->SetLineWidth(2); hlpt_ [index]->SetMarkerStyle(20); hlpt_ [index]->SetMarkerSize(0.8);
  hlet_ [index]->SetLineColor(color); hlet_ [index]->SetLineWidth(2); hlet_ [index]->SetMarkerStyle(20); hlet_ [index]->SetMarkerSize(0.8);
  hleta_[index]->SetLineColor(color); hleta_[index]->SetLineWidth(2); hleta_[index]->SetMarkerStyle(20); hleta_[index]->SetMarkerSize(0.8);

  if(index == 1) {
    h_    [2]->SetLineColor(kGreen-7); h_    [2]->SetLineWidth(2); h_    [2]->SetMarkerStyle(20); h_    [2]->SetMarkerSize(0.8);
    hpt_  [2]->SetLineColor(kGreen-7); hpt_  [2]->SetLineWidth(2); hpt_  [2]->SetMarkerStyle(20); hpt_  [2]->SetMarkerSize(0.8);
    hmass_[2]->SetLineColor(kGreen-7); hmass_[2]->SetLineWidth(2); hmass_[2]->SetMarkerStyle(20); hmass_[2]->SetMarkerSize(0.8);
    heta_ [2]->SetLineColor(kGreen-7); heta_ [2]->SetLineWidth(2); heta_ [2]->SetMarkerStyle(20); heta_ [2]->SetMarkerSize(0.8);
    hlpt_ [2]->SetLineColor(kGreen-7); hlpt_ [2]->SetLineWidth(2); hlpt_ [2]->SetMarkerStyle(20); hlpt_ [2]->SetMarkerSize(0.8);
    hlet_ [2]->SetLineColor(kGreen-7); hlet_ [2]->SetLineWidth(2); hlet_ [2]->SetMarkerStyle(20); hlet_ [2]->SetMarkerSize(0.8);
    hleta_[2]->SetLineColor(kGreen-7); hleta_[2]->SetLineWidth(2); hleta_[2]->SetMarkerStyle(20); hleta_[2]->SetMarkerSize(0.8);
  }
  return 0;
}

int signal_scale(int year) {
  year_ = year;
  TChain* bkg_chain = new TChain("Events", "DY MC TChain" );
  TChain* sig_chain = new TChain("Events", "Signal TChain");

  bkg_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_DY50*_%i.root", year));
  sig_chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_Z*_%i.root"   , year));

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

  double pt[] = {0., 2., 4., 6., 8., 10., 15., 20., 25., 30., 40., 50., 100., 500.};
  double mass[] = {50., 70., 80., 100., 500.};
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
  }

  //tbkg->Draw("GenZll_mass:GenZll_pt>>hbkg");
  //tsig->Draw("GenZll_mass:GenZll_pt>>hsig");
  // fill_hist(tbkg, 0);
  // fill_hist(tsig, 1);
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

  gSystem->Exec(Form("[ ! -d figures/signal_z_%i ] && mkdir -p figures/signal_z_%i", year, year));
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
  c->SaveAs(Form("figures/signal_z_ratio_%i.png", year));
  delete c;

  TFile* fout = new TFile(Form("rootfiles/signal_z_correction_%i.root", year), "RECREATE");
  hratio->Write();
  fout->Close();

  print_figure(hpt_  , Form("figures/signal_z_%i/z_pt.png"   , year));
  print_figure(hmass_, Form("figures/signal_z_%i/z_mass.png" , year), true);
  print_figure(heta_ , Form("figures/signal_z_%i/z_eta.png"  , year));
  print_figure(hlpt_ , Form("figures/signal_z_%i/lep_pt.png" , year));
  print_figure(hlet_ , Form("figures/signal_z_%i/lep_et.png" , year));
  print_figure(hleta_, Form("figures/signal_z_%i/lep_eta.png", year));

  return 0;
}
