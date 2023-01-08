//Script to test Tag and Probe (TnP) scale factors

enum {kIDTest = 1};
enum {kVVLooseIso = 1, kVLooseIso, kLooseIso, kMediumIso, kTightIso, kVTightIso, kVVTightIso};
enum {kWPL = 1, kWP90, kWP80};

TCanvas* plot_2d_hist(TH2* h[2]) {
  TCanvas* c = new TCanvas("c", "c", 1400, 700);
  c->Divide(3,1);
  c->cd(1);
  h[0]->Draw("colz");
  h[0]->SetTitle(Form("Data %s", h[0]->GetTitle()));
  c->cd(2);
  h[1]->Draw("colz");
  h[1]->SetTitle(Form("Embedding %s", h[1]->GetTitle()));

  c->cd(3);
  TH2* hRatio = (TH2*) h[0]->Clone(Form("%s_ratio", h[0]->GetName()));
  hRatio->Divide(h[1]);
  hRatio->Draw("colz");

  hRatio->SetTitle("");
  hRatio->SetTitle("Data/MC");
  hRatio->GetZaxis()->SetRangeUser(0.5, 1.5);

  return c;
}

TCanvas* plot_hists(vector<TH1*> hists) {
  if(hists.size() < 2) {
    cout << __func__ << ": Not enough histograms given!\n";
    return nullptr;
  }

  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  const int colors[] = {kBlue, kRed, kGreen, kViolet, kOrange, kYellow+2,
                        kAtlantic, kGreen+2, kRed+2, kBlue+2, kViolet-2,
                        kOrange+2,kViolet+2, kBlue-2};
  const int ncolors = sizeof(colors) / sizeof(*colors);

  pad1->cd();
  pad1->SetGrid();
  TH1* haxis = nullptr;
  TLegend* leg = new TLegend(0.7, 0.75, 0.9, 0.9);
  double max_val(-1.e9), min_val(1.e9);
  for(int ihist = 0; ihist < hists.size(); ++ihist) {
    const int color = colors[ihist%ncolors];
    if(ihist == 0) { //Data
      hists[ihist]->SetMarkerStyle(20);
      hists[ihist]->SetMarkerSize(0.8);
      hists[ihist]->Draw("E1");
      haxis = hists[ihist];
      leg->AddEntry(hists[ihist], "Data", "PL");
    } else {
      hists[ihist]->SetLineColor(color);
      // hists[ihist]->SetFillColor(color);
      // hists[ihist]->SetFillStyle(3003);
      hists[ihist]->SetLineWidth(2);
      hists[ihist]->Draw("hist same");
      leg->AddEntry(hists[ihist], hists[ihist]->GetTitle(), "L");
    }
    max_val = std::max(max_val, hists[ihist]->GetMaximum());
    min_val = std::min(min_val, hists[ihist]->GetMinimum());
  }
  haxis->GetYaxis()->SetRangeUser(0.9*min_val, 1.1*max_val);
  leg->Draw();

  // //Add N(data) and N(MC)
  // TLatex label;
  // label.SetNDC();
  // label.SetTextFont(72);
  // // label.SetTextColor(1);
  // label.SetTextSize(0.04);
  // label.SetTextAlign(13);
  // label.SetTextAngle(0);
  // label.DrawLatex(0.62, 0.7 , Form("N(Data) = %10.0f", h[0]->Integral() + h[0]->GetBinContent(0) + h[0]->GetBinContent(h[0]->GetNbinsX()+1)));
  // label.DrawLatex(0.62, 0.65, Form("N(MC)   = %10.0f", h[1]->Integral() + h[1]->GetBinContent(0) + h[1]->GetBinContent(h[1]->GetNbinsX()+1)));

  pad2->cd();
  pad2->SetGrid();
  TLine* line = nullptr;
  for(int ihist = 1; ihist < hists.size(); ++ihist) {
    TH1* hRatio = (TH1*) haxis->Clone(Form("%s_%i_ratio", haxis->GetName(), ihist));
    hRatio->Divide(hists[ihist]);
    hRatio->SetLineColor(hists[ihist]->GetLineColor());
    hRatio->SetLineWidth(hists[ihist]->GetLineWidth());
    hRatio->Draw((ihist == 1) ? "hist" : "hist same");

    hRatio->SetTitle("");
    hRatio->GetYaxis()->SetTitle("Data/MC");
    hRatio->GetYaxis()->SetTitleSize(0.13);
    hRatio->GetYaxis()->SetTitleOffset(0.30);
    hRatio->GetYaxis()->SetLabelSize(0.08);
    hRatio->GetXaxis()->SetLabelSize(0.08);
    hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);
    if(ihist == 1) {
      line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
      line->SetLineColor(kRed);
      line->SetLineWidth(2);
    }
  }
  if(line) line->Draw("same");
  return c;
}

TCanvas* plot_hist(TH1* h[2]) {
  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();
  h[0]->SetMarkerStyle(20);
  h[0]->SetMarkerSize(0.8);
  h[1]->SetLineColor(kRed);
  h[1]->SetFillColor(kRed);
  h[1]->SetFillStyle(3003);
  h[1]->SetLineWidth(2);
  h[1]->Draw("hist");
  h[0]->Draw("E1 same");
  h[1]->GetYaxis()->SetRangeUser(0, 1.1*max(h[0]->GetMaximum(), h[1]->GetMaximum()));
  TLegend* leg = new TLegend(0.7, 0.75, 0.9, 0.9);
  leg->AddEntry(h[0], "Data", "PL");
  leg->AddEntry(h[1], "MC"  , "L" );
  leg->Draw();

  //Add N(data) and N(MC)
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  // label.SetTextColor(1);
  label.SetTextSize(0.04);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.DrawLatex(0.62, 0.7 , Form("N(Data) = %10.0f", h[0]->Integral() + h[0]->GetBinContent(0) + h[0]->GetBinContent(h[0]->GetNbinsX()+1)));
  label.DrawLatex(0.62, 0.65, Form("N(MC)   = %10.0f", h[1]->Integral() + h[1]->GetBinContent(0) + h[1]->GetBinContent(h[1]->GetNbinsX()+1)));

  pad2->cd();
  pad2->SetGrid();
  TH1* hRatio = (TH1*) h[0]->Clone(Form("%s_ratio", h[0]->GetName()));
  hRatio->Divide(h[1]);
  hRatio->Draw();

  hRatio->SetTitle("");
  hRatio->GetYaxis()->SetTitle("Data/MC");
  hRatio->GetYaxis()->SetTitleSize(0.13);
  hRatio->GetYaxis()->SetTitleOffset(0.30);
  hRatio->GetYaxis()->SetLabelSize(0.08);
  hRatio->GetXaxis()->SetLabelSize(0.08);

  hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);
  TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");
  return c;
}

float embed_energy_scale(float eta, int year) {
  const bool barrel = std::fabs(eta) < 1.479;
  float scale_factor(1.f);
  if(year == 2016) {
    if(barrel) scale_factor -= 0.0024;
    else       scale_factor -= 0.0070;
  }
  else if(year == 2017) {
    if(barrel) scale_factor -= 0.0007;
    else       scale_factor -= 0.0113;
  }
  else if(year == 2018) {
    if(barrel) scale_factor -= 0.0033;
    else       scale_factor -= 0.0056;
  }
  return scale_factor;
}

int validation(bool isMuon = true, int year = 2016, int period = -1, int max_entries = -1) {

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/embed_tnp/";

  vector<TString> runs;
  if     (year == 2016) {
    if     (period == -1)   runs = {"B", "C", "D", "E", "F", "G", "H"};
    else if(period ==  0)   runs = {"B", "C", "D", "E", "F"};
    else if(period ==  1)   runs = {"G", "H"};
  } else if(year == 2017) { runs = {"B", "C", "D", "E", "F"};
  } else if(year == 2018) {
    if      (period == -1)  runs = {"A", "B", "C", "D"};
    else if (period ==  0)  runs = {"A", "B", "C"};
    else if (period ==  1)  runs = {"D"};
  }
  CrossSections xs;
  EmbeddingWeight embed_wt(10); //to apply the embedding unfolding corrections

  ///////////////////////////////////////
  // Initialize ID correction objects
  ///////////////////////////////////////

  TFile* fSF_ID   = TFile::Open(Form("rootfiles/embedding_eff_%s_mode-1_%i%s.root",
                                  (isMuon) ? "mumu" : "ee", year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
  TFile* fSF_Iso  = TFile::Open(Form("rootfiles/embedding_eff_%s_mode-2_%i%s.root",
                                  (isMuon) ? "mumu" : "ee", year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
  TFile* fSF_Trig = TFile::Open(Form("rootfiles/embedding_eff_%s_mode-0_%i%s.root",
                                  (isMuon) ? "mumu" : "ee", year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
  if(!fSF_ID || !fSF_Iso || !fSF_Trig) return 1;

  TH2* hSF_ID   = (TH2*) fSF_ID  ->Get("PtVsEtaSF");
  TH2* hSF_Iso  = (TH2*) fSF_Iso ->Get("PtVsEtaSF");
  TH2* hSF_Trig[2] = {(TH2*) fSF_Trig->Get("PtVsEtaData"), (TH2*) fSF_Trig->Get("PtVsEtaMC")};

  if(!hSF_ID  ) {cout << "ID Scales not found!\n"  ; return 2;}
  if(!hSF_Iso ) {cout << "Iso Scales not found!\n" ; return 2;}
  if(!hSF_Trig[0] || !hSF_Trig[1]) {cout << "Trig Scales not found!\n"; return 2;}

  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  TH1* hOnePt   [2];
  TH1* hOneEta  [2];
  TH1* hTwoPt   [2];
  TH1* hTwoEta  [2];
  TH1* hDiffPt  [2];
  TH1* hDiffEta [2];
  TH1* hPairPt  [2];
  TH1* hPairEta [2];
  TH1* hPairMass[2];
  TH1* hPairMassUp[2];
  TH1* hPairMassDown[2];
  TH2* hOnePtVsTwoPt[2];


  hOnePt     [0] = new TH1F("honept"   , (isMuon) ? "Muon p_{T}"       : "Electron p_{T}"      , 100,  0  , 100);
  hOneEta    [0] = new TH1F("honeeta"  , (isMuon) ? "Muon #eta"        : "Electron #eta"       ,  50, -2.5, 2.5);
  hTwoPt     [0] = new TH1F("htwopt"   , (isMuon) ? "Muon p_{T}"       : "Electron p_{T}"      , 100,  0  , 100);
  hTwoEta    [0] = new TH1F("htwoeta"  , (isMuon) ? "Muon #eta"        : "Electron #eta"       ,  50, -2.5, 2.5);
  hDiffPt    [0] = new TH1F("hdiffpt"  , (isMuon) ? "Muon #Deltap_{T}" : "Electron #Deltap_{T}", 100,  -50,  50);
  hDiffEta   [0] = new TH1F("hdiffeta" , (isMuon) ? "Muon #Delta#eta"  : "Electron #Delta#eta" ,  50, -  5,   5);
  hPairPt    [0] = new TH1F("hpairpt"  , "p_{T}^{ll}"                                          , 100,    0, 100);
  hPairEta   [0] = new TH1F("hpaireta" , "#eta^{ll}"                                           ,  50, -  5,   5);
  hPairMass  [0] = new TH1F("hpairmass", "M_{ll}"                                              ,  60,   60, 120);
  hPairMassUp  [0] = new TH1F("hpairmass_up"  , "M_{ll} ES up"                                 ,  60,   60, 120);
  hPairMassDown[0] = new TH1F("hpairmass_down", "M_{ll} ES down"                               ,  60,   60, 120);
  hOnePtVsTwoPt[0] = new TH2F("honeptvstwopt" , "p_{T}^{1} vs p_{T}^{2}"                       ,  25, 10, 100, 25, 10, 100);

  hOnePt    [1] = (TH1*) hOnePt    [0]->Clone(Form("%s_mc", hOnePt    [0]->GetName()));
  hOneEta   [1] = (TH1*) hOneEta   [0]->Clone(Form("%s_mc", hOneEta   [0]->GetName()));
  hTwoPt    [1] = (TH1*) hTwoPt    [0]->Clone(Form("%s_mc", hTwoPt    [0]->GetName()));
  hTwoEta   [1] = (TH1*) hTwoEta   [0]->Clone(Form("%s_mc", hTwoEta   [0]->GetName()));
  hDiffPt   [1] = (TH1*) hDiffPt   [0]->Clone(Form("%s_mc", hDiffPt   [0]->GetName()));
  hDiffEta  [1] = (TH1*) hDiffEta  [0]->Clone(Form("%s_mc", hDiffEta  [0]->GetName()));
  hPairPt   [1] = (TH1*) hPairPt   [0]->Clone(Form("%s_mc", hPairPt   [0]->GetName()));
  hPairEta  [1] = (TH1*) hPairEta  [0]->Clone(Form("%s_mc", hPairEta  [0]->GetName()));
  hPairMass [1] = (TH1*) hPairMass [0]->Clone(Form("%s_mc", hPairMass [0]->GetName()));
  hPairMassUp  [1] = (TH1*) hPairMassUp  [0]->Clone(Form("%s_mc", hPairMassUp  [0]->GetName()));
  hPairMassDown[1] = (TH1*) hPairMassDown[0]->Clone(Form("%s_mc", hPairMassDown[0]->GetName()));
  hOnePtVsTwoPt[1] = (TH2*) hOnePtVsTwoPt[0]->Clone(Form("%s_mc", hOnePtVsTwoPt[0]->GetName()));


  ///////////////////////////////////////
  // Define the thresholds
  ///////////////////////////////////////

  float trig_pt_min = (isMuon) ? 24.f : 32.f;
  if((!isMuon && year == 2016) || (isMuon && year == 2017)) trig_pt_min = 27.f;
  trig_pt_min += (isMuon) ? 1.f : 2.f; //enforce the event is above the threshold
  const float min_pt = (isMuon) ? 10.f : 15.f;
  const float max_eta = 2.4f;
  const double gap_low(1.444), gap_high(1.566);


  ///////////////////////////////////////
  // File processing loop
  ///////////////////////////////////////

  TFile* f;
  for(int isMC = 0; isMC <= 1; ++isMC) {
    ULong64_t nused(0), ntotal(0);
    for(TString run : runs) {
      if(max_entries > 0 && nused >= max_entries) break;
      if     (isMC == 0) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Single%sRun%i%s_%i.root", path.Data(), (isMuon) ? "Muon"   : "Electron", year, run.Data(), year), "READ");
      else if(isMC == 1) f = TFile::Open(Form("%sfiles/EmbedTnPAnalysis_Embed-%s-%s_%i.root", path.Data(), (isMuon) ? "MuMu" : "EE" , run.Data(), year), "READ");
      else {
        cout << "Undefined isMC value " << isMC << endl;
        return 3;
      }
      if(!f) return 4;

      TTree* t = (TTree*) f->Get("Events");
      if(!t) {
        cout << "Events tree not found in file " << f->GetName() << endl;
        return 5;
      }

      cout << "Opened new file " << f->GetName() << endl;

      ///////////////////////////////////////
      // Initialize tree branch addresses
      ///////////////////////////////////////

      float one_pt, one_eta, one_sc_eta, one_phi, one_q, one_ecorr;
      float two_pt, two_eta, two_sc_eta, two_phi, two_q, two_ecorr;
      int   one_id1, one_id2;
      int   two_id1, two_id2;
      float pair_pt, pair_eta, pair_mass;
      bool  one_triggered, two_triggered;
      float pu_weight(1.), gen_weight(1.);
      float gen_one_pt(-1.f), gen_one_eta, gen_two_pt(-1.f), gen_two_eta;
      t->SetBranchStatus("*", 0);
      t->SetBranchStatus("one_pt"         , 1); t->SetBranchAddress("one_pt"         , &one_pt         );
      t->SetBranchStatus("one_eta"        , 1); t->SetBranchAddress("one_eta"        , &one_eta        );
      t->SetBranchStatus("one_sc_eta"     , 1); t->SetBranchAddress("one_sc_eta"     , &one_sc_eta     );
      t->SetBranchStatus("one_phi"        , 1); t->SetBranchAddress("one_phi"        , &one_phi        );
      t->SetBranchStatus("one_q"          , 1); t->SetBranchAddress("one_q"          , &one_q          );
      t->SetBranchStatus("one_id1"        , 1); t->SetBranchAddress("one_id1"        , &one_id1        );
      t->SetBranchStatus("one_id2"        , 1); t->SetBranchAddress("one_id2"        , &one_id2        );
      t->SetBranchStatus("one_triggered"  , 1); t->SetBranchAddress("one_triggered"  , &one_triggered  );
      t->SetBranchStatus("two_pt"         , 1); t->SetBranchAddress("two_pt"         , &two_pt         );
      t->SetBranchStatus("two_eta"        , 1); t->SetBranchAddress("two_eta"        , &two_eta        );
      t->SetBranchStatus("two_sc_eta"     , 1); t->SetBranchAddress("two_sc_eta"     , &two_sc_eta     );
      t->SetBranchStatus("two_phi"        , 1); t->SetBranchAddress("two_phi"        , &two_phi        );
      t->SetBranchStatus("two_q"          , 1); t->SetBranchAddress("two_q"          , &two_q          );
      t->SetBranchStatus("two_id1"        , 1); t->SetBranchAddress("two_id1"        , &two_id1        );
      t->SetBranchStatus("two_id2"        , 1); t->SetBranchAddress("two_id2"        , &two_id2        );
      t->SetBranchStatus("two_triggered"  , 1); t->SetBranchAddress("two_triggered"  , &two_triggered  );
      t->SetBranchStatus("pair_pt"        , 1); t->SetBranchAddress("pair_pt"        , &pair_pt        );
      t->SetBranchStatus("pair_eta"       , 1); t->SetBranchAddress("pair_eta"       , &pair_eta       );
      t->SetBranchStatus("pair_mass"      , 1); t->SetBranchAddress("pair_mass"      , &pair_mass      );
      if(isMC) {
        t->SetBranchStatus("genWeight"        , 1); t->SetBranchAddress("genWeight"        , &gen_weight );
        t->SetBranchStatus("GenZll_LepOne_pt" , 1); t->SetBranchAddress("GenZll_LepOne_pt" , &gen_one_pt );
        t->SetBranchStatus("GenZll_LepOne_eta", 1); t->SetBranchAddress("GenZll_LepOne_eta", &gen_one_eta);
        t->SetBranchStatus("GenZll_LepTwo_pt" , 1); t->SetBranchAddress("GenZll_LepTwo_pt" , &gen_two_pt );
        t->SetBranchStatus("GenZll_LepTwo_eta", 1); t->SetBranchAddress("GenZll_LepTwo_eta", &gen_two_eta);
      }
      const bool has_ecorr = isMC && t->GetBranch("one_ecorr");
      if(has_ecorr) {
        t->SetBranchStatus("one_ecorr", 1); t->SetBranchAddress("one_ecorr", &one_ecorr);
        t->SetBranchStatus("two_ecorr", 1); t->SetBranchAddress("two_ecorr", &two_ecorr);
      }

      const double xs_scale = (isMC == 1) ? xs.GetCrossSection(Form("Embed-%s-%s", (isMuon) ? "MuMu" : "EE", run.Data()), year) : 1.;

      ///////////////////////////////////////
      // Process the data
      ///////////////////////////////////////

      ULong64_t nentries = t->GetEntries();
      ntotal += nentries;
      cout << nentries << " events to process";
      if(isMC == 1) cout << " in run " << run.Data();
      cout << endl;

      //Loop through the input tree
      for(ULong64_t entry = 0; entry < nentries; ++entry) {
        if(max_entries > 0 && nused >= max_entries) break;
        if(entry % 100000 == 0) printf("Processing entry %12lld (%5.1f%%)...\n", entry, entry*100./nentries);
        t->GetEntry(entry);
        if(isMC == 1 && gen_weight > 1.) continue;
        //Z mass peak
        if(pair_mass < 60. || pair_mass > 120.) continue;

        //opposite flavor
        if(one_q*two_q > 0) continue;

        //check that the event is triggered
        if(!((one_pt > trig_pt_min && one_triggered) || (two_pt > trig_pt_min && two_triggered))) continue;

        //enforce eta range
        if(std::fabs(one_eta) > max_eta || std::fabs(two_eta) > max_eta) continue;

        //electron eta veto
        if(!isMuon && gap_low <= fabs(one_sc_eta) && fabs(one_sc_eta) <= gap_high) continue;
        if(!isMuon && gap_low <= fabs(two_sc_eta) && fabs(two_sc_eta) <= gap_high) continue;

        //enforce pt range
        if(one_pt < min_pt || two_pt < min_pt) continue;

        //enforce lepton ID
        if(one_id1 < kIDTest || two_id1 < kIDTest) continue;

        //enforce lepton Iso ID
        if(one_id2 < kTightIso || two_id2 < kTightIso) continue;

        //make sure they're separated
        TLorentzVector lv1; lv1.SetPtEtaPhiM(one_pt, one_eta, one_phi, (isMuon) ? 0.10566 : 5.11e-3);
        TLorentzVector lv2; lv2.SetPtEtaPhiM(two_pt, two_eta, two_phi, (isMuon) ? 0.10566 : 5.11e-3);
        if(std::fabs(lv1.DeltaR(lv2)) < 0.5) continue;

        //Accept the event
        ++nused;

        float wt = pu_weight*((isMC > 1) ? ((gen_weight < 0) ? -1. : 1.) : gen_weight)*xs_scale;

        //Event weights
        if(isMC) {
          float eta_var;
          //Apply the ID scale factors
          eta_var = (hSF_ID->GetXaxis()->GetBinLowEdge(1) < -1) ? one_sc_eta : std::fabs(one_sc_eta);
          wt *= hSF_ID->GetBinContent(hSF_ID->GetXaxis()->FindBin(eta_var), min(hSF_ID->GetNbinsY(), hSF_ID->GetYaxis()->FindBin(one_pt)));
          eta_var = (hSF_ID->GetXaxis()->GetBinLowEdge(1) < -1) ? two_sc_eta : std::fabs(two_sc_eta);
          wt *= hSF_ID->GetBinContent(hSF_ID->GetXaxis()->FindBin(eta_var), min(hSF_ID->GetNbinsY(), hSF_ID->GetYaxis()->FindBin(two_pt)));

          //Apply the Iso scale factors
          eta_var = (hSF_Iso->GetXaxis()->GetBinLowEdge(1) < -1) ? one_sc_eta : std::fabs(one_sc_eta);
          wt *= hSF_ID->GetBinContent(hSF_Iso->GetXaxis()->FindBin(eta_var), min(hSF_Iso->GetNbinsY(), hSF_Iso->GetYaxis()->FindBin(one_pt)));
          eta_var = (hSF_Iso->GetXaxis()->GetBinLowEdge(1) < -1) ? two_sc_eta : std::fabs(two_sc_eta);
          wt *= hSF_ID->GetBinContent(hSF_Iso->GetXaxis()->FindBin(eta_var), min(hSF_Iso->GetNbinsY(), hSF_Iso->GetYaxis()->FindBin(two_pt)));

          //Apply the Trigger scale factors
          double p_fail[2] = {1.,1.};
          if(one_pt > trig_pt_min) {
            eta_var = (hSF_Trig[0]->GetXaxis()->GetBinLowEdge(1) < -1) ? one_sc_eta : std::fabs(one_sc_eta);
            //data
            p_fail[0] *= (1.f - hSF_Trig[0]->GetBinContent(hSF_Trig[0]->GetXaxis()->FindBin(eta_var), min(hSF_Trig[0]->GetNbinsY(), hSF_Trig[0]->GetYaxis()->FindBin(one_pt))));
            //MC
            p_fail[1] *= (1.f - hSF_Trig[1]->GetBinContent(hSF_Trig[1]->GetXaxis()->FindBin(eta_var), min(hSF_Trig[1]->GetNbinsY(), hSF_Trig[1]->GetYaxis()->FindBin(one_pt))));
          }
          if(two_pt > trig_pt_min) {
            eta_var = (hSF_Trig[0]->GetXaxis()->GetBinLowEdge(1) < -1) ? two_sc_eta : std::fabs(two_sc_eta);
            //data
            p_fail[0] *= (1.f - hSF_Trig[0]->GetBinContent(hSF_Trig[0]->GetXaxis()->FindBin(eta_var), min(hSF_Trig[0]->GetNbinsY(), hSF_Trig[0]->GetYaxis()->FindBin(two_pt))));
            //MC
            p_fail[1] *= (1.f - hSF_Trig[1]->GetBinContent(hSF_Trig[1]->GetXaxis()->FindBin(eta_var), min(hSF_Trig[1]->GetNbinsY(), hSF_Trig[1]->GetYaxis()->FindBin(two_pt))));
          }
          if(p_fail[0] >= 1. || p_fail[1] >= 1.) wt *= 1.f;
          else                                   wt *= (1. - p_fail[0]) / (1. - p_fail[1]); //P(at least 1 passes | Data) / P(at least 1 passes | MC)

          //apply energy scale corrections
          if(!isMuon) {
            lv1 *= embed_energy_scale(one_sc_eta, year)*((has_ecorr) ? 1.f/one_ecorr : 1.f);
            lv2 *= embed_energy_scale(one_sc_eta, year)*((has_ecorr) ? 1.f/two_ecorr : 1.f);
            one_pt = lv1.Pt();
            two_pt = lv2.Pt();
            pair_pt = (lv1+lv2).Pt();
            pair_mass = (lv1+lv2).M();
          }

          //apply unfolding corrections
          if(gen_one_pt >= 0. && gen_two_pt >= 0.) { //only do if gen-level info is defined
            wt *= embed_wt.UnfoldingWeight(gen_one_pt, gen_one_eta, gen_two_pt, gen_two_eta, pair_eta, pair_pt, year);
          } else if(isMuon) { //approximate with the reco values if not defined
            wt *= embed_wt.UnfoldingWeight(one_pt, one_eta, two_pt, two_eta, pair_eta, pair_pt, year);
          }
        }

        //get an idea of the energy scale uncertainty
        const float escale = (isMC == 0) ? 0. : (isMuon) ?  0.005 : 0.01;
        lv1 *= 1. + escale;
        lv2 *= 1. + escale;
        const float pair_mass_up   = (lv1 + lv2).M();
        lv1 *= (1. - escale)/(1. + escale);
        lv2 *= (1. - escale)/(1. + escale);
        const float pair_mass_down = (lv1 + lv2).M();

        //Fill the histograms
        hOnePt   [isMC]->Fill(one_pt , wt); hOneEta  [isMC]->Fill(one_sc_eta, wt);
        hTwoPt   [isMC]->Fill(two_pt , wt); hTwoEta[isMC]->Fill(two_sc_eta, wt);
        hDiffPt  [isMC]->Fill(two_pt - one_pt, wt); hDiffEta[isMC]->Fill(two_sc_eta - one_sc_eta, wt);
        hPairPt  [isMC]->Fill(pair_pt, wt); hPairEta[isMC]->Fill(pair_eta, wt);
        hPairMass[isMC]->Fill(pair_mass, wt);
        hPairMassUp  [isMC]->Fill(pair_mass_up  , wt);
        hPairMassDown[isMC]->Fill(pair_mass_down, wt);
        hOnePtVsTwoPt[isMC]->Fill(one_pt, two_pt, wt);
      }
      //Close the file
      delete t;
      f->Close();
      cout << "Closed completed file " << f->GetName() << endl;
    }
    cout << "Used " << nused << " of the " << ntotal << " entries (" << (100.*nused)/ntotal << "%)\n";
  }

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);

  TString dir = Form("figures/val_%s_%i%s", (isMuon) ? "mumu" : "ee", year, (period > -1) ? Form("_period_%i", period) : "");
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s ", dir.Data(), dir.Data()));

  //Plot some basic kinematics of the one/two
  TCanvas* c;
  c = plot_hist(hOnePt);
  if(c) { c->SaveAs(Form("%s/onept.png", dir.Data())); delete c; }
  c = plot_hist(hTwoPt);
  if(c) { c->SaveAs(Form("%s/twopt.png", dir.Data())); delete c; }
  c = plot_hist(hOneEta);
  if(c) { c->SaveAs(Form("%s/oneeta.png", dir.Data())); delete c; }
  c = plot_hist(hTwoEta);
  if(c) { c->SaveAs(Form("%s/twoeta.png", dir.Data())); delete c; }
  c = plot_hist(hDiffPt);
  if(c) { c->SaveAs(Form("%s/diffpt.png", dir.Data())); delete c; }
  c = plot_hist(hDiffEta);
  if(c) { c->SaveAs(Form("%s/diffeta.png", dir.Data())); delete c; }
  c = plot_hist(hPairPt);
  if(c) { c->SaveAs(Form("%s/pairpt.png", dir.Data())); delete c; }
  c = plot_hist(hPairEta);
  if(c) { c->SaveAs(Form("%s/paireta.png", dir.Data())); delete c; }
  c = plot_hist(hPairMass);
  if(c) { c->SaveAs(Form("%s/pairmass.png", dir.Data())); delete c; }
  c = plot_hist(hPairMassUp);
  if(c) { c->SaveAs(Form("%s/pairmass_up.png", dir.Data())); delete c; }
  c = plot_hist(hPairMassDown);
  if(c) { c->SaveAs(Form("%s/pairmass_down.png", dir.Data())); delete c; }
  c = plot_2d_hist(hOnePtVsTwoPt);
  if(c) { c->SaveAs(Form("%s/onept_vs_twopt.png", dir.Data())); delete c; }

  //plot energy scale variation
  //normalize to the data for this plot
  const double embed_scale = hPairMass[0]->Integral() / hPairMass[1]->Integral();
  hPairMass    [1]->Scale(embed_scale);
  hPairMassUp  [1]->Scale(embed_scale);
  hPairMassDown[1]->Scale(embed_scale);
  std::vector<TH1*> mass_hists = {hPairMass[0], hPairMass[1], hPairMassUp[1], hPairMassDown[1]};
  c = plot_hists(mass_hists);
  if(c) { c->SaveAs(Form("%s/pairmass_variations.png", dir.Data())); delete c; }

  return 0;
}
