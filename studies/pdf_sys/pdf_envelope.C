//Produce scale factors to produce alternate PDF templates in the Z kinematics

Long64_t max_entries_ = 1e6;
constexpr int max_pdf_ = 150; //maximum number of alternate PDFs

int year_;
int npdf_ = -1;

int hist_mode_ = 0; //0: (M, pT); 1: (M, |eta|)

TH2* h_     [max_pdf_]; //scale factors
TH1* hpt_   [max_pdf_];
TH1* hmass_ [max_pdf_];
TH1* heta_  [max_pdf_];
TH1* hlpt_  [max_pdf_];
TH1* hlet_  [max_pdf_];
TH1* hleta_ [max_pdf_];
TH1* hnpdf_ [max_pdf_];
TH1* hwt_   [max_pdf_];

TH2* hratio_[max_pdf_]; //output ratio scale factors


//--------------------------------------------------------------------------------------
// Make a ratio plot
int print_figure(TH1* h[max_pdf_], const char* outname, const bool logy = false) {
  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();
  h[0]->Draw("hist");
  for(int index = 1; index < npdf_; ++index) {h[index]->Draw("hist same");}
  const double max_val = h[0]->GetMaximum();
  h[0]->SetAxisRange((logy) ? max_val/1.e6 : 0., max_val*((logy) ? 5. : 1.1), "Y");
  if(logy) pad1->SetLogy();

  TLegend* leg = new TLegend(0.6, 0.75, 0.89, 0.89);
  leg->AddEntry(h[0], "Nominal PDF");
  leg->AddEntry(h[1], "Alternate PDFs");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  leg->Draw();

  //Create the ratio plot
  pad2->cd();
  pad2->SetGrid();
  TLine* line;
  //Envelope of the ratio changes
  TH1* hEnv_up = (TH1*) h[0]->Clone(Form("%s_env_up", h[0]->GetName()));
  TH1* hEnv_down = (TH1*) h[0]->Clone(Form("%s_env_down", h[0]->GetName()));
  hEnv_up->Divide(h[0]); //set each bin to 1 for now
  hEnv_down->Divide(h[0]);
  //Get the ratio for each alternate PDF
  for(int index = 1; index < npdf_; ++index) {
    TH1* hRatio = (TH1*) h[index]->Clone(Form("%s_ratio", h[index]->GetName()));
    hRatio->Divide(h[0]);
    if(index == 1) {
      hRatio->Draw("hist");
      hRatio->SetTitle("");
      hRatio->GetYaxis()->SetTitle("PDF / Nominal");
      hRatio->GetYaxis()->SetTitleSize(0.12);
      hRatio->GetYaxis()->SetTitleOffset(0.33);
      hRatio->GetYaxis()->SetLabelSize(0.08);
      hRatio->GetXaxis()->SetLabelSize(0.08);
      hRatio->GetYaxis()->SetRangeUser(0.85, 1.15);
      line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
    } else {
      hRatio->Draw("hist same");
    }
    //add the deviation from 1 for each bin
    for(int ibin = 1; ibin <= hRatio->GetNbinsX(); ++ibin) {
      const double dev = hRatio->GetBinContent(ibin) - 1.;
      //add deviations in quadrature
      if(dev > 0.) hEnv_up  ->SetBinContent(ibin, 1. + std::sqrt(std::pow(hEnv_up  ->GetBinContent(ibin)-1., 2) + dev*dev));
      else         hEnv_down->SetBinContent(ibin, 1. - std::sqrt(std::pow(hEnv_down->GetBinContent(ibin)-1., 2) + dev*dev));
    }
  }

  //draw the ratio envelope
  pad2->cd();
  hEnv_up  ->SetLineColor(kBlue);
  hEnv_up  ->SetLineStyle(kSolid);
  hEnv_up  ->Draw("hist same");
  hEnv_down->SetLineColor(kBlue);
  hEnv_down->SetLineStyle(kSolid);
  hEnv_down->Draw("hist same");
  hEnv_up->GetYaxis()->SetRangeUser(0.85, 1.15); //necessary to overwrite the cloned axis range
  hEnv_down->GetYaxis()->SetRangeUser(0.85, 1.15);

  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");

  cout << "Using outname " << outname << endl;
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
  Utilities::SetBranchAddress(tree, "LHEPdfWeight"    , pdf_wt);
  Utilities::SetBranchAddress(tree, "nLHEPdfWeight"   , &n_pdf);
  Utilities::SetBranchAddress(tree, "genWeight"         , &genwt);

  Long64_t nentries = tree->GetEntries();
  TLorentzVector lv1, lv2;

  //Retrieve the correction factors (if available)
  const char* base_name = Form("env_%i", year_);
  TFile* f = TFile::Open(Form("rootfiles/z_pdf_shift_%s.root", base_name));
  TH2* hcorr = (f) ? (TH2*) f->Get("hratio") : nullptr;

  //Loop over the data
  cout << " Input chain has " << nentries << " entries\n";
  if(max_entries_ > 0 && nentries > max_entries_) cout << "--> only using the first " << max_entries_ << " entries!\n";
  nentries = (max_entries_ > 0 && max_entries_ < nentries) ? max_entries_ : nentries;
  Long64_t neffective = 0; //N(positive) - N(negative) events
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 500000 == 0 || (entry*10) % nentries == 0) printf(" Processing entry %12lld (%6.2f%%)\n", entry, entry*100./nentries);
    tree->GetEntry(entry);
    lv1.SetPtEtaPhiM(pt1, eta1, phi1, mass1);
    lv2.SetPtEtaPhiM(pt2, eta2, phi2, mass2);
    const float mass = (lv1+lv2).M();
    const float pt   = (lv1+lv2).Pt();
    const float eta  = (lv1+lv2).Eta();

    if(entry == 0) {
      cout << "--- The tree has " << n_pdf << " variations to consider\n";
    }

    //store the number of PDFs considered
    if(npdf_ < 0) npdf_ = n_pdf;
    if(npdf_ > max_pdf_) throw 20;

    //normalize to PDF set 0
    const float refwt = pdf_wt[0];
    if(refwt == 0.f) continue;
    genwt = (genwt < 0.f) ? -1.f : 1.f; //only use the sign of the generator weight
    neffective += (genwt < 0.f) ? -1 : 1; //N(evt) = N(positive) - N(negative)

    //Fill histograms for each alternate PDF
    for(int ipdf = 0; ipdf < n_pdf; ++ipdf) {
      const float altwt = pdf_wt[ipdf] / refwt;
      if(altwt < 0.f || altwt > 10.f) continue;
      // const float altwt = std::max(0.f, std::min(10.f, pdf_wt[ipdf] / refwt));
      const float evt_wt = (ipdf >= 0) ? genwt*altwt : genwt;

      const int index = ipdf;
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
    }
  }

  for(int index = 0; index < npdf_; ++index) {
    const float prev_int = h_[index]->Integral(0, h_[index]->GetNbinsX()+1, 0, h_[index]->GetNbinsY()+1);
    h_    [index]->Scale(1./prev_int);
    hpt_  [index]->Scale(1./hpt_  [index]->Integral(0, hpt_  [index]->GetNbinsX()+1));
    hmass_[index]->Scale(1./hmass_[index]->Integral(0, hmass_[index]->GetNbinsX()+1));
    heta_ [index]->Scale(1./heta_ [index]->Integral(0, heta_ [index]->GetNbinsX()+1));
    hlpt_ [index]->Scale(1./hlpt_ [index]->Integral(0, hlpt_ [index]->GetNbinsX()+1));
    hlet_ [index]->Scale(1./hlet_ [index]->Integral(0, hlet_ [index]->GetNbinsX()+1));
    hleta_[index]->Scale(1./hleta_[index]->Integral(0, hleta_[index]->GetNbinsX()+1));
    hnpdf_[index]->Scale(1./hnpdf_[index]->Integral(0, hnpdf_[index]->GetNbinsX()+1));
    hwt_  [index]->Scale(1./hwt_  [index]->Integral(0, hwt_  [index]->GetNbinsX()+1));

    //compare integrals
    const float integral = h_[index]->Integral(0, h_[index]->GetNbinsX()+1, 0, h_[index]->GetNbinsY()+1);
    printf("Initial integral: %f; Re-scaled integral: %f, N(effective) = %lld\n", prev_int, integral, neffective);
    int style = (index == 0) ? kSolid : kDashed;
    Int_t color;
    if      (index == 0) color = kBlue;
    else if(index <= 10) color = kRed + index - 5;
    else if(index <= 20) color = kGreen + index - 5;
    else if(index <= 30) color = kViolet + index - 5;
    else if(index <= 40) color = kOrange + index - 5;
    else if(index <= 50) color = kYellow + index - 5;
    else if(index <= 60) color = kMagenta + index - 5;
    else if(index <= 70) color = kCyan + index - 5;
    else                 color = kGreen-7;

    h_    [index]->SetLineColor(color); h_    [index]->SetLineStyle(style); h_    [index]->SetLineWidth(2); h_    [index]->SetMarkerStyle(20); h_    [index]->SetMarkerSize(0.8);
    hmass_[index]->SetLineColor(color); hmass_[index]->SetLineStyle(style); hmass_[index]->SetLineWidth(2); hmass_[index]->SetMarkerStyle(20); hmass_[index]->SetMarkerSize(0.8);
    hpt_  [index]->SetLineColor(color); hpt_  [index]->SetLineStyle(style); hpt_  [index]->SetLineWidth(2); hpt_  [index]->SetMarkerStyle(20); hpt_  [index]->SetMarkerSize(0.8);
    heta_ [index]->SetLineColor(color); heta_ [index]->SetLineStyle(style); heta_ [index]->SetLineWidth(2); heta_ [index]->SetMarkerStyle(20); heta_ [index]->SetMarkerSize(0.8);
    hlpt_ [index]->SetLineColor(color); hlpt_ [index]->SetLineStyle(style); hlpt_ [index]->SetLineWidth(2); hlpt_ [index]->SetMarkerStyle(20); hlpt_ [index]->SetMarkerSize(0.8);
    hlet_ [index]->SetLineColor(color); hlet_ [index]->SetLineStyle(style); hlet_ [index]->SetLineWidth(2); hlet_ [index]->SetMarkerStyle(20); hlet_ [index]->SetMarkerSize(0.8);
    hleta_[index]->SetLineColor(color); hleta_[index]->SetLineStyle(style); hleta_[index]->SetLineWidth(2); hleta_[index]->SetMarkerStyle(20); hleta_[index]->SetMarkerSize(0.8);
    hnpdf_[index]->SetLineColor(color); hnpdf_[index]->SetLineStyle(style); hnpdf_[index]->SetLineWidth(2); hnpdf_[index]->SetMarkerStyle(20); hnpdf_[index]->SetMarkerSize(0.8);
    hwt_  [index]->SetLineColor(color); hwt_  [index]->SetLineStyle(style); hwt_  [index]->SetLineWidth(2); hwt_  [index]->SetMarkerStyle(20); hwt_  [index]->SetMarkerSize(0.8);
  }

  return 0;
}

/**
   Main function
   year: supported are 2016 and 2018

   PDF set array index mapping:
     2016: 101 variations, 0: nominal, 1-100: statistical replicas
     2018:  33 variations, 0: nominal, 1-30: eigent vector variations, 31: alpha(MZ)=0.1165, 32: alpha(MZ)=0.1195
 **/
int pdf_envelope(int year = 2018) {

  //Initialize fields
  year_ = year;

  //Initialize the input data from leading order Drell-Yan MC
  TChain* chain = new TChain("Events", "Events");
  chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/gen_z/files/GenZAnalysis_LO-DY50_%i.root", year));

  //Initialize the Z pT, |eta|, and mass histograms
  const double pt  [] = {0., 5., 10., 15., 20., 23., 26., 30., 35., 40., 50., 60., 70., 80., 90., 100., 500.};
  const double eta [] = {0., 1., 2., 3., 4., 5., 6., 8., 10.};
  const double mass[] = {50., 60., 70., 80., 85., 90., 95., 100., 110., 120., 150., 500.};
  const int npt   = sizeof(pt)/sizeof(*pt) - 1;
  const int neta  = sizeof(eta)/sizeof(*eta) - 1;
  const int nmass = sizeof(mass)/sizeof(*mass) - 1;

  //Decide the scale factor parameterization, (M, pT) or (M, |eta|)
  hist_mode_ = (year == 2016) ? 0 : 1;

  //Initialize the histograms
  for(int index = 0; index < max_pdf_; ++index) {
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
    hnpdf_[index] = new TH1D(Form("h%i_npdf", index), " N(PDF)"           ,110,  0, 110);
    hwt_  [index] = new TH1D(Form("h%i_wt"  , index), " Event weight"     , 70, -3.5,   3.5);
  }

  //Fill the histograms
  cout << "Processing chain...\n";
  fill_hist(chain);

  //Ensure the scale factor histograms are properly filled
  if(npdf_ < 2) {
    cout << "No alternate PDFs found!\n";
    return 3;
  }
  if(h_[0]->GetEntries() == 0 || h_[1]->GetEntries() == 0) {
    cout << "No events found!\n";
    return 3;
  }

  //Create the output figure directory
  const char* base_name = Form("env_%i", year);
  gSystem->Exec(Form("[ ! -d figures/%s ] && mkdir -p figures/%s", base_name, base_name));
  gStyle->SetOptStat(0);

  //Create the scale factors and plot them
  gSystem->Exec(Form("[ ! -d figures/%s/scales ] && mkdir -p figures/%s/scales", base_name, base_name));
  gStyle->SetPaintTextFormat(".2f");


  TFile* fout = new TFile(Form("rootfiles/z_pdf_shift_%s.root", base_name), "RECREATE");

  for(int index = 1; index < npdf_; ++index) {
    TCanvas* c = new TCanvas();
    //Create the scale factor histograms
    hratio_[index] = (TH2*) h_[index]->Clone(Form("hratio_%i", index));
    TH2* hratio = hratio_[index];
    hratio->Divide(h_[0]);

    hratio->Draw("colz text");
    hratio->SetXTitle("M_{Z}");
    if(hist_mode_ == 0) {
      hratio->SetYTitle("p^{Z}_{T}");
    } else if(hist_mode_ == 1) {
      hratio->SetYTitle("|#eta^{Z}|");
    }
    c->SetLogx();
    c->SetLogy();
    hratio->GetXaxis()->SetMoreLogLabels(kTRUE);
    hratio->GetYaxis()->SetMoreLogLabels(kTRUE);
    hratio->GetZaxis()->SetRangeUser(0.85, 1.15);

    const char* name = Form("figures/env_%i/scales/pdf_%i.png", year, index);
    c->SaveAs(name);
    hratio->Write();
    delete c;
  }
  fout->Close();

  //Plot 1-D distributions
  print_figure(hpt_  , Form("figures/env_%i/pt.png"     , year));
  print_figure(hmass_, Form("figures/env_%i/mass.png"   , year), true);
  print_figure(heta_ , Form("figures/env_%i/eta.png"    , year));
  print_figure(hlpt_ , Form("figures/env_%i/lep_pt.png" , year));
  print_figure(hlet_ , Form("figures/env_%i/lep_et.png" , year));
  print_figure(hleta_, Form("figures/env_%i/lep_eta.png", year));
  print_figure(hwt_  , Form("figures/env_%i/weight.png" , year), true);

  return 0;
}
