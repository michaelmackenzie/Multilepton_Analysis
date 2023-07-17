//Produce scale factors to produce alternate PDF versions of the Z spectrum

Long64_t max_entries_ = 1e7;
const static int nhist = 4;

int year_;
int pdf_set_;

int hist_mode_ = 0; //0: (M, pT); 1: (M, |eta|)

TH2* h_           [nhist]; //scale factors
TH1* hpt_         [nhist];
TH1* hmass_       [nhist];
TH1* heta_        [nhist];
TH1* hlpt_        [nhist];
TH1* hlet_        [nhist];
TH1* hleta_       [nhist];
TH1* hnpdf_       [nhist];
TH1* hwt_         [nhist];


//--------------------------------------------------------------------------------------
// Make a ratio plot
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
  leg->AddEntry(h[0], "Nominal PDF");
  leg->AddEntry(h[1], "Alternate PDF");
  leg->AddEntry(h[2], "Re-weighted Nominal");
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
  hRatio->GetYaxis()->SetTitle("PDF / Nominal");
  hRatio->GetYaxis()->SetTitleSize(0.12);
  hRatio->GetYaxis()->SetTitleOffset(0.33);
  hRatio->GetYaxis()->SetLabelSize(0.08);
  hRatio->GetXaxis()->SetLabelSize(0.08);
  hRatio->GetYaxis()->SetRangeUser(0.8, 1.2);

  TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");

  c->SaveAs(outname);
  delete c;

  return 0;
}

//--------------------------------------------------------------------------------------
// Fill histograms from the input ntuple
int fill_hist(TTree* tree) {
  //Setup the fields to read from the tree
  float pt1, pt2, eta1, eta2, phi1, phi2, mass1, mass2, genwt(1.f);
  float pdf_wt[1000];
  int   z_idx;
  UInt_t n_pdf;
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
  if(std::abs(pdf_set_) < 1000) { //PDF uncertainties
    cout << " Using PDF uncertainty branches\n";
    Utilities::SetBranchAddress(tree, "LHEPdfWeight"    , pdf_wt);
    Utilities::SetBranchAddress(tree, "nLHEPdfWeight"   , &n_pdf);
  } else { //Scale uncertainties
    cout << " Using Scale uncertainty branches\n";
    Utilities::SetBranchAddress(tree, "LHEScaleWeight"  , pdf_wt);
    Utilities::SetBranchAddress(tree, "nLHEScaleWeight" , &n_pdf);
  }
  Utilities::SetBranchAddress(tree, "genWeight"         , &genwt);

  const int pdf_set = pdf_set_ % 1000;
  Long64_t nentries = tree->GetEntries();
  TLorentzVector lv1, lv2;

  //Retrieve the correction factors (if available)
  TFile* f = TFile::Open(Form("rootfiles/z_pdf_shift_p%i_%i.root", std::abs(pdf_set_), year_));
  TH2* hcorr = (f) ? (TH2*) f->Get("hratio") : nullptr;

  //Loop over the data
  cout << " Input chain has " << nentries << " entries\n";
  if(max_entries_ > 0 && nentries > max_entries_) cout << "--> only using the first " << max_entries_ << " entries!\n";
  nentries = (max_entries_ > 0 && max_entries_ < nentries) ? max_entries_ : nentries;
  Long64_t neffective = 0; //N(positive) - N(negative) events
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 500000 == 0) printf(" Processing entry %12lld (%6.2f%%)\n", entry, entry*100./nentries);
    tree->GetEntry(entry);
    lv1.SetPtEtaPhiM(pt1, eta1, phi1, mass1);
    lv2.SetPtEtaPhiM(pt2, eta2, phi2, mass2);
    const float mass = (lv1+lv2).M();
    const float pt   = (lv1+lv2).Pt();
    const float eta  = (lv1+lv2).Eta();

    //Evaluate the weight associated with the theory uncertainty
    float shift = 1.f;
    if(pdf_set_ >= 0) { //use an index in the weight array
      shift = (pdf_set < n_pdf) ? pdf_wt[pdf_set] / genwt : 1.f;
    } else { //loop through to find the largest deviation in the event
      float max_dev = 0.f;
      int index = 0;
      for(int ipdf = 0; ipdf < n_pdf; ++ipdf) {
        const float dev = std::fabs(pdf_wt[ipdf]/genwt - 1.f);
        if(dev > max_dev) {max_dev = dev; index = ipdf;}
      }
      shift = pdf_wt[index] / genwt;
    }
    shift = std::fabs(shift); //take the sign from the nominal PDF weight
    if(shift > 3.f) shift = 1.f; //ignore large weights
    genwt = (genwt < 0.f) ? -1.f : 1.f; //only use the sign of the generator weight
    neffective += (genwt < 0.f) ? -1 : 1; //N(evt) = N(positive) - N(negative)

    //Fill histograms for nominal and shifted
    for(int index = 0; index < 2; ++index) {

      //index 0: nominal; index 1: shifted
      const float evt_wt = (index == 1) ? genwt*shift : genwt;

      if(hist_mode_ == 0) {
        h_    [index]->Fill(mass, pt, evt_wt);
      } else if(hist_mode_ == 1) {
        h_    [index]->Fill(mass, fabs(eta), evt_wt);
      }
      hpt_  [index]->Fill(pt, evt_wt);
      hmass_[index]->Fill(mass, evt_wt);
      heta_ [index]->Fill(eta, evt_wt);
      hlpt_ [index]->Fill(pt1, evt_wt);
      hlpt_ [index]->Fill(pt2, evt_wt);
      hlet_ [index]->Fill(sqrt(pt1*pt1 + mass1*mass1), evt_wt);
      hlet_ [index]->Fill(sqrt(pt2*pt2 + mass2*mass2), evt_wt);
      hleta_[index]->Fill(eta1, evt_wt);
      hleta_[index]->Fill(eta2, evt_wt);
      hnpdf_[index]->Fill(n_pdf);
      hwt_  [index]->Fill(evt_wt);

      if(index == 0) { //make plots of re-weighted input
        float wt = 1.f;
        if(hcorr) {
          if(hist_mode_ == 0) {
            const int xbin = hcorr->GetXaxis()->FindBin(mass);
            const int ybin = hcorr->GetYaxis()->FindBin(pt);
            wt = hcorr->GetBinContent(xbin, ybin);
          } else if(hist_mode_ == 1) {
            const int xbin = hcorr->GetXaxis()->FindBin(mass);
            const int ybin = hcorr->GetYaxis()->FindBin(fabs(eta));
            wt = hcorr->GetBinContent(xbin, ybin);
          }
        }
        wt *= genwt;
        if(hist_mode_ == 0) {
          h_  [2]->Fill(mass, pt       , wt);
        } else if(hist_mode_ == 1) {
          h_  [2]->Fill(mass, fabs(eta), wt);
        }
        hpt_  [2]->Fill(pt             , wt);
        hmass_[2]->Fill(mass           , wt);
        heta_ [2]->Fill((lv1+lv2).Eta(), wt);
        hlpt_ [2]->Fill(pt1            , wt/2.);
        hlpt_ [2]->Fill(pt2            , wt/2.);
        hlet_ [2]->Fill(sqrt(pt1*pt1 + mass1*mass1), wt/2.);
        hlet_ [2]->Fill(sqrt(pt2*pt2 + mass2*mass2), wt/2.);
        hleta_[2]->Fill(eta1           , wt/2.);
        hleta_[2]->Fill(eta2           , wt/2.);
        hnpdf_[2]->Fill(n_pdf, wt);
        hwt_  [2]->Fill(wt);
      }
    }
  }

  for(int index = 0; index < 3; ++index) {
    Int_t color = (index == 0) ? kBlue : (index == 1) ? kRed : kGreen - 7;
    const float prev_int = h_[index]->Integral(0, h_[index]->GetNbinsX()+1, 0, h_[index]->GetNbinsY()+1);
    if(index < 2) {
      h_    [index]->Scale(1./prev_int);
      hpt_  [index]->Scale(1./hpt_  [index]->Integral(0, hpt_  [index]->GetNbinsX()+1));
      hmass_[index]->Scale(1./hmass_[index]->Integral(0, hmass_[index]->GetNbinsX()+1));
      heta_ [index]->Scale(1./heta_ [index]->Integral(0, heta_ [index]->GetNbinsX()+1));
      hlpt_ [index]->Scale(1./hlpt_ [index]->Integral(0, hlpt_ [index]->GetNbinsX()+1));
      hlet_ [index]->Scale(1./hlet_ [index]->Integral(0, hlet_ [index]->GetNbinsX()+1));
      hleta_[index]->Scale(1./hleta_[index]->Integral(0, hleta_[index]->GetNbinsX()+1));
      hnpdf_[index]->Scale(1./hnpdf_[index]->Integral(0, hnpdf_[index]->GetNbinsX()+1));
      hwt_  [index]->Scale(1./hwt_  [index]->Integral(0, hwt_  [index]->GetNbinsX()+1));
    } else {
      h_    [index]->Scale(1./neffective);
      hpt_  [index]->Scale(1./neffective);
      hmass_[index]->Scale(1./neffective);
      heta_ [index]->Scale(1./neffective);
      hlpt_ [index]->Scale(1./neffective);
      hlet_ [index]->Scale(1./neffective);
      hleta_[index]->Scale(1./neffective);
      hnpdf_[index]->Scale(1./neffective);
      hwt_  [index]->Scale(1./neffective);
    }

    //compare integrals
    const float integral = h_[index]->Integral(0, h_[index]->GetNbinsX()+1, 0, h_[index]->GetNbinsY()+1);
    printf("Initial integral: %f; Re-scaled integral: %f, N(effective) = %lld\n", prev_int, integral, neffective);

    h_    [index]->SetLineColor(color); h_    [index]->SetLineWidth(2); h_    [index]->SetMarkerStyle(20); h_    [index]->SetMarkerSize(0.8);
    hpt_  [index]->SetLineColor(color); hpt_  [index]->SetLineWidth(2); hpt_  [index]->SetMarkerStyle(20); hpt_  [index]->SetMarkerSize(0.8);
    hmass_[index]->SetLineColor(color); hmass_[index]->SetLineWidth(2); hmass_[index]->SetMarkerStyle(20); hmass_[index]->SetMarkerSize(0.8);
    heta_ [index]->SetLineColor(color); heta_ [index]->SetLineWidth(2); heta_ [index]->SetMarkerStyle(20); heta_ [index]->SetMarkerSize(0.8);
    hlpt_ [index]->SetLineColor(color); hlpt_ [index]->SetLineWidth(2); hlpt_ [index]->SetMarkerStyle(20); hlpt_ [index]->SetMarkerSize(0.8);
    hlet_ [index]->SetLineColor(color); hlet_ [index]->SetLineWidth(2); hlet_ [index]->SetMarkerStyle(20); hlet_ [index]->SetMarkerSize(0.8);
    hleta_[index]->SetLineColor(color); hleta_[index]->SetLineWidth(2); hleta_[index]->SetMarkerStyle(20); hleta_[index]->SetMarkerSize(0.8);
    hnpdf_[index]->SetLineColor(color); hnpdf_[index]->SetLineWidth(2); hnpdf_[index]->SetMarkerStyle(20); hnpdf_[index]->SetMarkerSize(0.8);
    hwt_  [index]->SetLineColor(color); hwt_  [index]->SetLineWidth(2); hwt_  [index]->SetMarkerStyle(20); hwt_  [index]->SetMarkerSize(0.8);
  }

  return 0;
}

/**
   Main function
   pdf_set: PDF set weights to compare to nominal
 **/
int scale_factors(int year, int pdf_set = 1) {
  year_ = year;
  pdf_set_ = pdf_set;
  TChain* chain = new TChain("Events", "Events");

  chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_LO-DY50_%i.root", year));

  const double pt[] = {0., 5., 10., 15., 20., 23., 26., 30., 35., 40., 50., 60., 70., 80., 90., 100., 500.};
  const double eta[] = {0., 1., 2., 3., 4., 5., 6., 8., 10.};
  const double mass[] = {50., 60., 70., 80., 85., 90., 95., 100., 110., 120., 150., 500.}; //{50., 70., 80., 100., 500.};
  const int npt = sizeof(pt)/sizeof(*pt) - 1;
  const int neta = sizeof(eta)/sizeof(*eta) - 1;
  const int nmass = sizeof(mass)/sizeof(*mass) - 1;

  if(abs(pdf_set) < 1000) hist_mode_ = (year == 2016) ? 0 : 1; //PDF uncertainties
  else                    hist_mode_ = (year == 2016) ? 0 : 0; //Scale uncertainties

  for(int index = 0; index < nhist; ++index) {
    if(hist_mode_ == 0) {
      h_    [index] = new TH2D(Form("h%i"     , index), "M_{Z} vs p^{Z}_{T}", nmass, mass, npt, pt);
    } else if(hist_mode_ == 1) {
      h_    [index] = new TH2D(Form("h%i"     , index), "M_{Z} vs |#eta^{Z}|", nmass, mass, neta, eta);
    } else {
      cout << "Unknown scale factor histogram mode " << hist_mode_ << endl;
      return 10;
    }
    hpt_  [index] = new TH1D(Form("h%i_pt"  , index), " p^{Z}_{T}"        , 50,  0, 100);
    hmass_[index] = new TH1D(Form("h%i_mass", index), " M_{Z}"            , 50, 50, 150);
    heta_ [index] = new TH1D(Form("h%i_eta" , index), " #eta_{Z}"         , 50,-10,  10);
    hlpt_ [index] = new TH1D(Form("h%i_lpt" , index), " p^{l}_{T}"        , 50,  0, 100);
    hlet_ [index] = new TH1D(Form("h%i_le"  , index), " E^{l}_{T}"        , 50,  0, 100);
    hleta_[index] = new TH1D(Form("h%i_leta", index), " #eta_{l}"         , 50, -5,   5);
    hnpdf_[index] = new TH1D(Form("h%i_npdf", index), " N(PDF)"           , 50,  0,  50);
    hwt_  [index] = new TH1D(Form("h%i_wt"  , index), " Event weight"     , 70, -3.5,   3.5);
  }

  cout << "Processing chain...\n";
  fill_hist(chain);

  if(h_[0]->GetEntries() == 0 || h_[1]->GetEntries() == 0) {
    cout << "No events found!\n";
    return 3;
  }

  //Using negative pdf_set for looping, remove that for printout
  if(pdf_set < 0) pdf_set = std::fabs(pdf_set);
  if(pdf_set_ < 0) pdf_set_ = std::fabs(pdf_set_);

  TH2* hratio = (TH2*) h_[1]->Clone("hratio");
  hratio->Divide(h_[0]);

  gSystem->Exec(Form("[ ! -d figures/p%i_%i ] && mkdir -p figures/p%i_%i", pdf_set, year, pdf_set, year));
  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas();
  gStyle->SetPaintTextFormat(".2f");
  hratio->Draw("colz text");
  hratio->SetXTitle("M_{Z}");
  if(hist_mode_ == 0) {
    hratio->SetYTitle("p^{Z}_{T}");
  } else if(hist_mode_ == 1) {
    hratio->SetYTitle("|#eta^{Z}|");
  }
  c->SetLogx();
  if(pdf_set_ > 1000) c->SetLogy();
  // c->SetLogz();
  hratio->GetXaxis()->SetMoreLogLabels(kTRUE);
  hratio->GetYaxis()->SetMoreLogLabels(kTRUE);
  c->SaveAs(Form("figures/ratio_p%i_%i.png", pdf_set, year));

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
  hdy->GetYaxis()->SetRangeUser(0., 1.1*max(1e-10, max(hdy->GetMaximum(), hsig->GetMaximum())));
  c->SetLogx(true);
  c->SetLogy(false);
  c->SetLogz(false);
  hdy->GetXaxis()->SetMoreLogLabels(true);
  hdy->GetYaxis()->SetMoreLogLabels(true);
  if(hist_mode_ == 0)
    c->SaveAs(Form("figures/p%i_%i/pt_proj.png", pdf_set, year));
  else
    c->SaveAs(Form("figures/p%i_%i/eta_proj.png", pdf_set, year));

  TH1* hprojx = hratio->ProjectionX("_px", 1, hratio->GetNbinsY());
  hprojx->Scale(1./hratio->GetNbinsY());
  hprojx->SetTitle("Signal M^{Z} corrections");
  hprojx->SetXTitle("M^{Z}");
  hprojx->SetYTitle("No Mixing / Mixing");
  hprojx->SetLineWidth(2);
  hprojx->SetMarkerStyle(6);
  hprojx->Draw("E1");
  c->SetLogx(true);
  c->SetLogy(false);
  c->SetLogz(false);
  hprojx->GetXaxis()->SetMoreLogLabels(true);
  hprojx->GetYaxis()->SetMoreLogLabels(true);
  hprojx->GetXaxis()->SetRangeUser(0.5, hprojx->GetBinCenter(hprojx->GetNbinsX()));
  c->SaveAs(Form("figures/p%i_%i/mass_ratio.png", pdf_set, year));

  TH1* hprojy = hratio->ProjectionY("_py", 1, hratio->GetNbinsX());
  hprojy->Scale(1./hratio->GetNbinsX());
  hprojy->SetYTitle("No Mixing / Mixing");
  hprojy->SetLineWidth(2);
  hprojy->SetMarkerStyle(6);
  hprojy->Draw("E1");
  c->SetLogx(true);
  c->SetLogy(false);
  c->SetLogz(false);
  hprojy->GetXaxis()->SetMoreLogLabels(true);
  hprojy->GetYaxis()->SetMoreLogLabels(true);
  hprojy->GetXaxis()->SetRangeUser(0.5, hprojy->GetBinCenter(hprojy->GetNbinsX()));
  if(hist_mode_ == 0) {
    hprojy->SetTitle("Signal p_{T}^{Z} corrections");
    hprojy->SetXTitle("p_{T}^{Z}");
    c->SaveAs(Form("figures/p%i_%i/pt_ratio.png", pdf_set, year));
  } else if(hist_mode_ == 1) {
    hprojy->SetTitle("Signal |#eta^{Z}| corrections");
    hprojy->SetXTitle("|#eta^{Z}|");
    c->SaveAs(Form("figures/p%i_%i/eta_ratio.png", pdf_set, year));
  }

  //Test the scale factors
  TH2* closure = (TH2*) h_[1]->Clone("hclosure");
  closure->Divide(h_[2]);
  closure->Draw("colz text");
  closure->GetZaxis()->SetRangeUser(0.985, 1.015);
  c->SaveAs(Form("figures/p%i_%i/closure.png", pdf_set, year));

  delete c;

  TFile* fout = new TFile(Form("rootfiles/z_pdf_shift_p%i_%i.root", abs(pdf_set), year), "RECREATE");
  hratio->Write();
  fout->Close();

  print_figure(hpt_  , Form("figures/p%i_%i/z_pt.png"   , pdf_set, year));
  print_figure(hmass_, Form("figures/p%i_%i/z_mass.png" , pdf_set, year), true);
  print_figure(heta_ , Form("figures/p%i_%i/z_eta.png"  , pdf_set, year));
  print_figure(hlpt_ , Form("figures/p%i_%i/lep_pt.png" , pdf_set, year));
  print_figure(hlet_ , Form("figures/p%i_%i/lep_et.png" , pdf_set, year));
  print_figure(hleta_, Form("figures/p%i_%i/lep_eta.png", pdf_set, year));
  print_figure(hnpdf_, Form("figures/p%i_%i/npdf.png"   , pdf_set, year));
  print_figure(hwt_  , Form("figures/p%i_%i/weight.png" , pdf_set, year), true);

  return 0;
}
