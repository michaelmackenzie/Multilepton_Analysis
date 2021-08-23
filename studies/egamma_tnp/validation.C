//Script to generate Tag and Probe (TnP) electron trigger scale factors

TH2F* hScales_; //scale factors from TnP measurement

TH1F* hData_Pt_;
TH1F* hData_Eta_;
TH2F* hData_PtvsEta_;
void fill_histograms(TTree* t, bool isData) {
  ///////////////////////////////////////
  // Initialize tree branch addresses
  ///////////////////////////////////////

  float tag_pt, tag_eta, tag_sc_eta, tag_phi, tag_q;
  float probe_pt, probe_eta, probe_sc_eta, probe_phi, probe_q;
  float pair_pt, pair_eta, pair_mass;
  bool  probe_triggered;
  float pu_weight(1.), gen_weight(1.);
  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("tag_Ele_pt"     , 1); t->SetBranchAddress("tag_Ele_pt"     , &tag_pt         );
  t->SetBranchStatus("tag_Ele_eta"    , 1); t->SetBranchAddress("tag_Ele_eta"    , &tag_eta        );
  t->SetBranchStatus("tag_sc_eta"     , 1); t->SetBranchAddress("tag_sc_eta"     , &tag_sc_eta     );
  t->SetBranchStatus("tag_Ele_phi"    , 1); t->SetBranchAddress("tag_Ele_phi"    , &tag_phi        );
  t->SetBranchStatus("tag_Ele_q"      , 1); t->SetBranchAddress("tag_Ele_q"      , &tag_q          );
  t->SetBranchStatus("probe_Ele_pt"   , 1); t->SetBranchAddress("probe_Ele_pt"   , &probe_pt       );
  t->SetBranchStatus("probe_Ele_eta"  , 1); t->SetBranchAddress("probe_Ele_eta"  , &probe_eta      );
  t->SetBranchStatus("probe_sc_eta"   , 1); t->SetBranchAddress("probe_sc_eta"   , &probe_sc_eta   );
  t->SetBranchStatus("probe_Ele_phi"  , 1); t->SetBranchAddress("probe_Ele_phi"  , &probe_phi      );
  t->SetBranchStatus("probe_Ele_q"    , 1); t->SetBranchAddress("probe_Ele_q"    , &probe_q        );
  t->SetBranchStatus("probe_triggered", 1); t->SetBranchAddress("probe_triggered", &probe_triggered);
  t->SetBranchStatus("pair_pt"        , 1); t->SetBranchAddress("pair_pt"        , &pair_pt        );
  t->SetBranchStatus("pair_mass"      , 1); t->SetBranchAddress("pair_mass"      , &pair_mass      );
  if(isMC) {
    t->SetBranchStatus("puWeight" , 1); t->SetBranchAddress("puWeight" , &pu_weight );
    t->SetBranchStatus("genWeight", 1); t->SetBranchAddress("genWeight", &gen_weight);
  }
  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  double eta_bins[] = {-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};
  double pt_bins [] = {28,30,33,35,37,39,41,43,46,50,100,200,500};
  int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  int n_pt_bins  = sizeof( pt_bins) / sizeof( *pt_bins) - 1;
  TH2F* hPtVsEta  = new TH2F("hptvseta" , "p_{T} vs #eta", n_eta_bins, eta_bins, n_pt_bins, pt_bins);
  hPtVsEta->Sumw2();
  hPtVsEta->SetXTitle("SuperCluster #eta"); hPtVsEta->SetYTitle("p_{T} (GeV/c)");
  TH1F* hPt    = new TH1F("hpt"   , "Electron p_{T}", 100,  0  , 100);
  TH1F* hEta   = new TH1F("heta"  , "Electron #eta" ,  25, -2.5, 2.5);

  ///////////////////////////////////////
  // Process the data
  ///////////////////////////////////////

  long nentries = t->GetEntriesFast();
  cout << nentries << " to process\n";
  float tag_pt_min = 33.;
  if(year == 2016) tag_pt_min = 28.;
  float probe_pt_min = 25.;
  for(long entry = 0; entry < nentries; ++entry) {
    if(entry % 100000 == 0) cout << "Processing entry " << entry << "...\n";
    t->GetEntry(entry);
    if(pair_mass < 60. || pair_mass > 120.) continue;
    if(fabs(tag_sc_eta) > 2.17) continue;
    if(tag_pt < tag_pt_min) continue;
    if(probe_pt < probe_pt_min) continue;
    float wt = pu_weight*((gen_weight < 0) ? -1. : 1.);
    //apply efficiency correction to MC
    if(!isData && probe_triggered) wt *= hScales_->GetBinContent(hScales_->FindBin(probe_sc_eta), hScales_->FindBin(probe_pt));
    if(probe_triggered) hPass->Fill(probe_sc_eta, probe_pt, wt);
    else                hFail->Fill(probe_sc_eta, probe_pt, wt);
    hTagPt  ->Fill(tag_pt  , wt); hTagEta  ->Fill(tag_sc_eta  , wt);
    hProbePt->Fill(probe_pt, wt); hProbeEta->Fill(probe_sc_eta, wt);
  }

}

void validation(int year = 2016) {

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/egamma_tnp/";
  TFile* fData = TFile::Open(Form("%sdata/EGammaTnPAnalysis_SingleEle_%i.root", path.Data(), year), "READ");
  TFile* fMC   = TFile::Open(Form("%sMC/EGammaTnPAnalysis_DY50-amcnlo_%i.root", path.Data(), year), "READ");
  if(!fData || fMC) return;

  TTree* tData = (TTree*) fData->Get("Events");
  if(!tData) {
    cout << "Data events tree not found!\n";
    return;
  }

  
  ///////////////////////////////////////
  // Initialize tree branch addresses
  ///////////////////////////////////////

  float tag_pt, tag_eta, tag_sc_eta, tag_phi, tag_q;
  float probe_pt, probe_eta, probe_sc_eta, probe_phi, probe_q;
  float pair_pt, pair_eta, pair_mass;
  bool  probe_triggered;
  float pu_weight(1.), gen_weight(1.);
  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("tag_Ele_pt"     , 1); t->SetBranchAddress("tag_Ele_pt"     , &tag_pt         );
  t->SetBranchStatus("tag_Ele_eta"    , 1); t->SetBranchAddress("tag_Ele_eta"    , &tag_eta        );
  t->SetBranchStatus("tag_sc_eta"     , 1); t->SetBranchAddress("tag_sc_eta"     , &tag_sc_eta     );
  t->SetBranchStatus("tag_Ele_phi"    , 1); t->SetBranchAddress("tag_Ele_phi"    , &tag_phi        );
  t->SetBranchStatus("tag_Ele_q"      , 1); t->SetBranchAddress("tag_Ele_q"      , &tag_q          );
  t->SetBranchStatus("probe_Ele_pt"   , 1); t->SetBranchAddress("probe_Ele_pt"   , &probe_pt       );
  t->SetBranchStatus("probe_Ele_eta"  , 1); t->SetBranchAddress("probe_Ele_eta"  , &probe_eta      );
  t->SetBranchStatus("probe_sc_eta"   , 1); t->SetBranchAddress("probe_sc_eta"   , &probe_sc_eta   );
  t->SetBranchStatus("probe_Ele_phi"  , 1); t->SetBranchAddress("probe_Ele_phi"  , &probe_phi      );
  t->SetBranchStatus("probe_Ele_q"    , 1); t->SetBranchAddress("probe_Ele_q"    , &probe_q        );
  t->SetBranchStatus("probe_triggered", 1); t->SetBranchAddress("probe_triggered", &probe_triggered);
  t->SetBranchStatus("pair_pt"        , 1); t->SetBranchAddress("pair_pt"        , &pair_pt        );
  t->SetBranchStatus("pair_mass"      , 1); t->SetBranchAddress("pair_mass"      , &pair_mass      );
  if(isMC) {
    t->SetBranchStatus("puWeight" , 1); t->SetBranchAddress("puWeight" , &pu_weight );
    t->SetBranchStatus("genWeight", 1); t->SetBranchAddress("genWeight", &gen_weight);
  }
  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  double eta_bins[] = {-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};
  double pt_bins [] = {28,30,33,35,37,39,41,43,46,50,100,200,500};
  int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  int n_pt_bins  = sizeof( pt_bins) / sizeof( *pt_bins) - 1;
  TH2F* hPass  = new TH2F("hPass" , "Passing events", n_eta_bins, eta_bins, n_pt_bins, pt_bins);
  TH2F* hFail  = new TH2F("hFail" , "Failing events", n_eta_bins, eta_bins, n_pt_bins, pt_bins);
  hPass->Sumw2(); hFail->Sumw2();
  hPass->SetXTitle("SuperCluster #eta"); hPass->SetYTitle("p_{T} (GeV/c)");
  hFail->SetXTitle("SuperCluster #eta"); hFail->SetYTitle("p_{T} (GeV/c)");
  TH1F* hTagPt    = new TH1F("htagpt"   , "Tagged electron p_{T}", 100,  0  , 100);
  TH1F* hTagEta   = new TH1F("htageta"  , "Tagged electron #eta" ,  25, -2.5, 2.5);
  TH1F* hProbePt  = new TH1F("hprobept" , "Probe electron p_{T}" , 100,  0  , 100);
  TH1F* hProbeEta = new TH1F("hprobeeta", "Tagged electron #eta" ,  25, -2.5, 2.5);

  ///////////////////////////////////////
  // Process the data
  ///////////////////////////////////////

  long nentries = t->GetEntriesFast();
  cout << nentries << " to process\n";
  float tag_pt_min = 33.;
  if(year == 2016) tag_pt_min = 28.;
  float probe_pt_min = 25.;
  for(long entry = 0; entry < nentries; ++entry) {
    if(entry % 100000 == 0) cout << "Processing entry " << entry << "...\n";
    t->GetEntry(entry);
    if(pair_mass < 60. || pair_mass > 120.) continue;
    if(fabs(tag_sc_eta) > 2.17) continue;
    if(tag_pt < tag_pt_min) continue;
    if(probe_pt < probe_pt_min) continue;
    float wt = pu_weight*((gen_weight < 0) ? -1. : 1.);
    if(probe_triggered) hPass->Fill(probe_sc_eta, probe_pt, wt);
    else                hFail->Fill(probe_sc_eta, probe_pt, wt);
    hTagPt  ->Fill(tag_pt  , wt); hTagEta  ->Fill(tag_sc_eta  , wt);
    hProbePt->Fill(probe_pt, wt); hProbeEta->Fill(probe_sc_eta, wt);
  }

  TH2F* hRatio = (TH2F*) hPass->Clone("hRatio");
  TH2F* hTotal = (TH2F*) hPass->Clone("hTotal");
  hTotal->Add(hFail);
  hRatio->Divide(hTotal);
  hRatio->SetTitle("Efficiency");

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1500, 700);
  c->Divide(3,1);
  TVirtualPad* pad;
  pad = c->cd(1);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hPass->Draw("colz");
  pad->SetLogz();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hFail->Draw("colz");
  pad->SetLogz();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hRatio->Draw("colz");
  hRatio->GetZaxis()->SetRangeUser(0., 1.);

  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  c->SaveAs(Form("figures/eff_%i_%i.png", isMC, year));

  c = new TCanvas("c", "c", 1000, 700);
  c->Divide(2,1);
  pad = c->cd(1);
  hTagPt->SetLineWidth(2);
  hTagPt->SetLineColor(kRed);
  hTagPt->Draw("hist");
  hProbePt->SetLineWidth(2);
  hProbePt->SetLineColor(kBlue);
  hProbePt->Draw("hist same");
  pad = c->cd(2);
  hTagEta->SetLineWidth(2);
  hTagEta->SetLineColor(kRed);
  hTagEta->Draw("hist");
  hProbeEta->SetLineWidth(2);
  hProbeEta->SetLineColor(kBlue);
  hProbeEta->Draw("hist same");
  c->SaveAs(Form("figures/kin_%i_%i.png", isMC, year));

  TFile* fout = new TFile(Form("rootfiles/efficiencies_%i_%i.root", isMC, year), "RECREATE");
  hRatio->Write();
  fout->Write();
  fout->Close();
}
