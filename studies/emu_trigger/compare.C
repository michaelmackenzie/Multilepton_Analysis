//Compare the single lepton and double lepton triggers
#include "interface/GlobalConstants.h"
enum {kMaxLeptons = 20, kMaxTriggers = 500};

int verbose_ = 0;
Long64_t max_entries_ = -1;
Long64_t start_entry_ = 0;
Long64_t nentries_;

//event variables
struct vars_t {
  //trigger info
  Bool_t HLT_Mu8_Ele23;
  Bool_t HLT_Mu23_Ele12;
  Bool_t HLT_Mu24;
  Bool_t HLT_Mu27;
  Bool_t HLT_Ele27;
  Bool_t HLT_Ele32;

  //trigger objects
  UInt_t  nTrigObj;
  Float_t TrigObj_pt  [kMaxTriggers];
  Float_t TrigObj_eta [kMaxTriggers];
  Int_t   TrigObj_id  [kMaxTriggers];
  Int_t   TrigObj_bits[kMaxTriggers];

  //lepton info
  Float_t Electron_pt            [kMaxLeptons];
  Float_t Electron_eta           [kMaxLeptons];
  Float_t Electron_phi           [kMaxLeptons];
  Int_t   Electron_charge        [kMaxLeptons];
  Float_t Electron_deltaEtaSC    [kMaxLeptons];
  Float_t Electron_pfRelIso03_all[kMaxLeptons];
  Float_t Muon_pt                [kMaxLeptons];
  Float_t Muon_eta               [kMaxLeptons];
  Float_t Muon_phi               [kMaxLeptons];
  Int_t   Muon_charge            [kMaxLeptons];
  Float_t Muon_pfRelIso04_all    [kMaxLeptons];

  //evaluated info
  Float_t lead_pt;
  Float_t trail_pt;
  Float_t mass;
  Float_t pt;

  //processing info
  int year;
  Long64_t entry;
};

//histograms
struct hists_t {
  TH1* elept;
  TH1* eleeta;
  TH1* muonpt;
  TH1* muoneta;
  TH1* leadpt;
  TH1* trailpt;
  TH1* mass;
  TH1* pt;
};

//set branch addresses
void set_branches(TTree* tree, vars_t& vars) {
  tree->SetBranchStatus("*",0);
  CLFV::Utilities::SetBranchAddress(tree, "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL" , &vars.HLT_Mu8_Ele23 );
  CLFV::Utilities::SetBranchAddress(tree, "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL", &vars.HLT_Mu23_Ele12);
  CLFV::Utilities::SetBranchAddress(tree, "HLT_IsoMu24"                     , &vars.HLT_Mu24                   );
  CLFV::Utilities::SetBranchAddress(tree, "HLT_IsoMu27"                     , &vars.HLT_Mu27                   );
  if(vars.year == 2016)
    CLFV::Utilities::SetBranchAddress(tree, "HLT_Ele27_WPTight_Gsf"           , &vars.HLT_Ele27);
  if(vars.year == 2017)
    CLFV::Utilities::SetBranchAddress(tree, "HLT_Ele32_WPTight_Gsf_L1DoubleEG", &vars.HLT_Ele32);
  if(vars.year == 2018)
    CLFV::Utilities::SetBranchAddress(tree, "HLT_Ele32_WPTight_Gsf"           , &vars.HLT_Ele32);
  CLFV::Utilities::SetBranchAddress(tree, "Electron_pt"            , vars.Electron_pt);
  CLFV::Utilities::SetBranchAddress(tree, "Electron_eta"           , vars.Electron_eta);
  CLFV::Utilities::SetBranchAddress(tree, "Electron_phi"           , vars.Electron_phi);
  CLFV::Utilities::SetBranchAddress(tree, "Electron_charge"        , vars.Electron_charge);
  CLFV::Utilities::SetBranchAddress(tree, "Electron_pfRelIso03_all", vars.Electron_pfRelIso03_all);
  CLFV::Utilities::SetBranchAddress(tree, "Muon_pt"                , vars.Muon_pt);
  CLFV::Utilities::SetBranchAddress(tree, "Muon_eta"               , vars.Muon_eta);
  CLFV::Utilities::SetBranchAddress(tree, "Muon_phi"               , vars.Muon_phi);
  CLFV::Utilities::SetBranchAddress(tree, "Muon_charge"            , vars.Muon_charge);
  CLFV::Utilities::SetBranchAddress(tree, "Muon_pfRelIso04_all"    , vars.Muon_pfRelIso04_all);
  CLFV::Utilities::SetBranchAddress(tree, "nTrigObj"               , &vars.nTrigObj);
  CLFV::Utilities::SetBranchAddress(tree, "TrigObj_pt"             , vars.TrigObj_pt);
  CLFV::Utilities::SetBranchAddress(tree, "TrigObj_eta"            , vars.TrigObj_eta);
  CLFV::Utilities::SetBranchAddress(tree, "TrigObj_id"             , vars.TrigObj_id);
  CLFV::Utilities::SetBranchAddress(tree, "TrigObj_filterBits"     , vars.TrigObj_bits);
}

//book histograms
void book_hists(hists_t& hists, const char* tag) {
  hists.elept    = new TH1F(Form("ele_pt_%s"   , tag), "Electron p_{T}", 50, 0., 100.);
  hists.eleeta   = new TH1F(Form("ele_eta_%s"  , tag), "Electron #eta", 30, -2.5, 2.5);
  hists.muonpt   = new TH1F(Form("muon_pt_%s"  , tag), "Muon p_{T}", 50, 0., 100.);
  hists.muoneta  = new TH1F(Form("muon_eta_%s" , tag), "Muon #eta", 30, -2.5, 2.5);
  hists.leadpt   = new TH1F(Form("lead_pt_%s"  , tag), "Lead p_{T}" , 50, 0., 100.);
  hists.trailpt  = new TH1F(Form("trail_pt_%s" , tag), "Trail p_{T}", 50, 0., 100.);
  hists.pt       = new TH1F(Form("pt_%s"       , tag), "Di-lepton p_{T}", 50, 0., 100.);
  hists.mass     = new TH1F(Form("mass_%s"     , tag), "Di-lepton mass", 100, 0., 200.);
}

//set event variable info
void set_vars(vars_t& vars) {
  TLorentzVector lve, lvm;
  lve.SetPtEtaPhiM(vars.Electron_pt[0], vars.Electron_eta[0], vars.Electron_phi[0], CLFV::ELECMASS);
  lvm.SetPtEtaPhiM(vars.Muon_pt    [0], vars.Muon_eta    [0], vars.Muon_phi    [0], CLFV::MUONMASS);
  vars.mass = (lve+lvm).M();
  vars.pt   = (lve+lvm).Pt();
  vars.lead_pt = std::max(lve.Pt(), lvm.Pt());
  vars.trail_pt = std::min(lve.Pt(), lvm.Pt());
}

//fill histograms
void fill_hists(hists_t& hists, vars_t& vars) {
  hists.elept  ->Fill(vars.Electron_pt[0]);
  hists.eleeta ->Fill(vars.Electron_eta[0]);
  hists.muonpt ->Fill(vars.Muon_pt[0]);
  hists.muoneta->Fill(vars.Muon_eta[0]);
  hists.leadpt ->Fill(vars.lead_pt);
  hists.trailpt->Fill(vars.trail_pt);
  hists.pt     ->Fill(vars.pt);
  hists.mass   ->Fill(vars.mass);
}

//helper function for histogram integrals
double full_integral(TH1* h) {
  const double integral = h->Integral(0, h->GetNbinsX()+1);
  return integral;
}

//print histogram figure
void print_figure(TH1* h1, TH1* h2, TH1* h3, const char* fig_name, const double xmin = 1., const double xmax = -1.) {
  gStyle->SetOptStat(0);
  h2->SetLineColor(kBlue);
  h2->SetLineWidth(2);
  h3->SetLineColor(kGreen-7);
  h3->SetLineWidth(2);
  h1->SetLineColor(kRed);
  h1->SetMarkerColor(kBlack);
  h1->SetMarkerStyle(20);
  h1->SetMarkerSize(0.8);
  h1->SetLineWidth(2);

  // Make the canvas layout
  TCanvas* c = new TCanvas("c_plot_ratio", "c_plot_ratio", 1200, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
  pad1->SetBottomMargin(0.06);
  pad2->SetTopMargin(0.03);
  pad1->Draw(); pad2->Draw();
  pad1->cd();
  h1->Draw("E1");
  h2->Draw("E1 same");
  h3->Draw("E1 same");
  const double max_val = std::max(h1->GetMaximum(), std::max(h2->GetMaximum(), h3->GetMaximum()));
  h1->GetYaxis()->SetRangeUser(0.1, 1.2*max_val);
  if(xmin < xmax) h1->GetXaxis()->SetRangeUser(xmin, xmax);

  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(h1, Form("e#mu trigger #epsilon = %.3f", full_integral(h1) / nentries_));
  leg->AddEntry(h2, Form("e OR #mu trigger #epsilon = %.3f", full_integral(h2) / nentries_));
  leg->AddEntry(h3, Form("OR of both triggers #epsilon = %.3f", full_integral(h3) / nentries_));
  leg->Draw();

  pad2->cd();
  TH1* ratio = (TH1*) h1->Clone(Form("%s_ratio", h1->GetName()));
  ratio->Divide(h2);
  ratio->SetLineColor(h1->GetLineColor());
  ratio->SetLineWidth(h1->GetLineWidth());
  TH1* ratio_2 = (TH1*) h3->Clone(Form("%s_ratio", h3->GetName()));
  ratio_2->Divide(h2);
  ratio_2->SetLineColor(h3->GetLineColor());
  ratio_2->SetLineWidth(h3->GetLineWidth());
  ratio->Draw("hist");
  ratio_2->Draw("hist same");
  ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  if(xmin < xmax) ratio->GetXaxis()->SetRangeUser(xmin, xmax);

  TLine* line = new TLine((xmin < xmax) ? xmin : ratio->GetXaxis()->GetXmin(), 1., (xmin < xmax) ? xmax : ratio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->SetLineWidth(2);
  line->Draw("same");

  ratio->SetTitle("");
  h1->SetXTitle("");
  ratio->GetXaxis()->SetLabelSize(0.09);
  ratio->GetYaxis()->SetLabelSize(0.09);
  ratio->GetYaxis()->SetTitleSize(0.1);
  ratio->GetYaxis()->SetTitleOffset(0.35);

  c->SaveAs(Form("%s.png", fig_name));
  delete c;
}

//Get a trigger match
int trigger_match(bool emu_trigger, vars_t& vars) {
  if(verbose_ > 0) cout << " Checking " << ((emu_trigger) ? "emu" : "single lepton") << " trigger matches\n";
  int matched(-1);
  //check HLT flags first
  if(emu_trigger) {
    matched = ((vars.HLT_Mu8_Ele23 && vars.Muon_pt[0] > 8.f && vars.Electron_pt[0] > 23.f)
               || (vars.HLT_Mu23_Ele12 && vars.Muon_pt[0] > 23.f && vars.Electron_pt[0] > 12.f)) ? 0 : -1;
    if(matched == 0) return 0;
  } else { //single lepton triggers
    if(vars.year == 2016) matched = ((vars.HLT_Mu24 && vars.Muon_pt[0] > 24.f)
                                     || (vars.HLT_Ele27 && vars.Electron_pt[0] > 27.f)) ? 0 : -1;
    if(vars.year == 2017) matched = ((vars.HLT_Mu27 && vars.Muon_pt[0] > 27.f)
                                     || (vars.HLT_Ele32 && vars.Electron_pt[0] > 32.f)) ? 0 : -1;
    if(vars.year == 2018) matched = ((vars.HLT_Mu24 && vars.Muon_pt[0] > 24.f)
                                     || (vars.HLT_Ele32 && vars.Electron_pt[0] > 32.f)) ? 0 : -1;
    if(matched == 0) return 0;
  }
  return -1;

  //check trigger objects
  int found_ele(-1), found_muon(-1);
  const int bit_ele = (emu_trigger) ? 5 : (vars.year == 2017) ? 10 : 1;
  const int bit_muon = (emu_trigger) ? 3 /*5*/ : 1;
  for(UInt_t trig = 0; trig < vars.nTrigObj; ++trig) {
    const int id = std::abs(vars.TrigObj_id[trig]);
    bool is_ele = id == 11;
    bool is_muon = id == 13;
    // if(is_ele && found_ele >= 0) continue;
    // if(is_muon && found_muon >= 0) continue;
    if(!is_ele && !is_muon) continue;
    const int bits = vars.TrigObj_bits[trig];
    if(verbose_ > 5) {
      printf(" TrigObj %i: ID = %i, pt = %.2f, bits = %i\n", trig, id, vars.TrigObj_pt[trig], bits);
    }
    bool triggered = bits & ((is_ele) ? (1 << bit_ele) : (1 << bit_muon));
    if(triggered && verbose_ > 0) {
      cout << "--> Matched trig obj " << ((emu_trigger) ? "e-mu" : "single lepton") << " trigger with bits " << bits
           << " id = " << id << " and pt = " << vars.TrigObj_pt[trig] << endl;
    }
    if(is_ele && triggered) {
      //if multiple match, take the highest pT
      found_ele = (found_ele < 0 || vars.TrigObj_pt[trig] > vars.TrigObj_pt[found_ele]) ? trig : found_ele;
    }
    if(is_muon && triggered) {
      //if multiple match, take the highest pT
      found_muon = (found_muon < 0 || vars.TrigObj_pt[trig] > vars.TrigObj_pt[found_muon]) ? trig : found_muon;
    }
  }
  if(verbose_ > 5) cout << " Trigger obj indices: ele = " << found_ele << ", muon = " << found_muon << endl;
  matched = -1;
  if(emu_trigger) {
    if(found_ele >= 0 && found_muon >= 0) {
      if(vars.TrigObj_pt[found_ele] > 23. && vars.TrigObj_pt[found_muon] > 8.) matched = found_ele;
      else if(vars.TrigObj_pt[found_ele] > 12. && vars.TrigObj_pt[found_muon] > 23.) matched = found_muon;
    }
  } else {
    if(found_ele >= 0) {
      float ele_pt_min = (vars.year == 2016) ? 27. : 32.;
      if(vars.TrigObj_pt[found_ele] > ele_pt_min) matched = found_ele;
    }
    //default to muon if both are found
    if(found_muon >= 0) {
      float muon_pt_min = (vars.year == 2017) ? 27. : 24.;
      if(vars.TrigObj_pt[found_muon] > muon_pt_min) matched = found_muon;
    }
    // if((found_muon || found_ele) && matched < 0)
    //   cout << "Entry " << vars.entry << ": Passed trigger matching but failed pT cuts\n";
  }
  if(verbose_ > 5) cout << " Trigger match value = " << matched << endl;

  return matched;
}

int compare(TString signal = "ZETau", const int year = 2018, const bool apply_cuts = false) {

  //setup input data
  TChain* chain = new TChain("emu");
  chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/emu_trigger/TriggerStudy_%s-v*_%i.root", signal.Data(), year));
  // chain->Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/LFVAnalysis_%s-v3*_%i.root", signal.Data(), year));

  //set the relevant branches
  vars_t vars;
  vars.year = year;
  TTree* tree = (TTree*) chain;
  set_branches(tree, vars);

  //initialize the histograms
  hists_t hists[3]; //0: emu trigger; 1: e or mu trigger; 2: or of both
  book_hists(hists[0], "double");
  book_hists(hists[1], "single");
  book_hists(hists[2], "both"  );

  //process the data
  const Long64_t nentries = chain->GetEntries();
  cout << "Chain has " << nentries << " entries\n";
  nentries_ = 0; //track entries used for normalization
  for(Long64_t entry = start_entry_; entry < nentries; ++entry) {
    if(max_entries_ > 0 && (entry-start_entry_) >= max_entries_) break;
    chain->GetEntry(entry);
    if(entry % 10000 == 0 || verbose_ > 1) cout << "Processing entry " << entry << endl;
    vars.entry = entry;
    set_vars(vars);
    if(apply_cuts) {
      const float elec_pt_min = (signal == "ZEMu") ?  20.f :  15.f;
      const float muon_pt_min = (signal == "ZEMu") ?  20.f :  10.f;
      const float min_mass    = (signal == "ZEMu") ?  70.f :  40.f;
      const float max_mass    = (signal == "ZEMu") ? 110.f : 170.f;
      if(vars.Electron_pt[0] <= elec_pt_min) continue;
      if(vars.Muon_pt    [0] <= muon_pt_min) continue;
      if(vars.mass <= min_mass || vars.mass >= max_mass) continue;
      //isolation cuts
      if(vars.Electron_pfRelIso03_all[0] >= 0.15) continue;
      if(vars.Muon_pfRelIso04_all[0] >= 0.15) continue;
      //charge selection
      if(vars.Electron_charge[0]*vars.Muon_charge[0] > 0) continue;
    }
    ++nentries_;
    const bool emu_trigger     = trigger_match(true , vars) >= 0;
    const bool e_or_mu_trigger = trigger_match(false, vars) >= 0;
    if(verbose_ > 2) cout << " emu_trigger = " << emu_trigger << ", e_or_mu_trigger = " << e_or_mu_trigger << endl;
    if(emu_trigger)     fill_hists(hists[0], vars);
    if(e_or_mu_trigger) fill_hists(hists[1], vars);
    if(e_or_mu_trigger || emu_trigger) fill_hists(hists[2], vars);
    if((e_or_mu_trigger || emu_trigger) && (vars.Electron_pt[0] < 23.f && vars.Muon_pt[0] < 23.f)) {
      cout << "Entry " << entry << " has both leptons below 23 GeV/c\n";
    }
    // if(entry > 10) break;
  }
  signal.ToLower();
  const char* dir = Form("figures/%s%s/%i", signal.Data(), (apply_cuts) ? "_cut" : "", year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir, dir));
  print_figure(hists[0].elept   , hists[1].elept   ,  hists[2].elept   ,Form("%s/elept"  , dir));
  print_figure(hists[0].muonpt  , hists[1].muonpt  ,  hists[2].muonpt  ,Form("%s/muonpt" , dir));
  print_figure(hists[0].leadpt  , hists[1].leadpt  ,  hists[2].leadpt  ,Form("%s/leadpt" , dir));
  print_figure(hists[0].trailpt , hists[1].trailpt ,  hists[2].trailpt ,Form("%s/trailpt", dir));
  print_figure(hists[0].pt      , hists[1].pt      ,  hists[2].pt      ,Form("%s/pt"     , dir));
  print_figure(hists[0].mass    , hists[1].mass    ,  hists[2].mass    ,Form("%s/mass"   , dir), (signal == "zemu") ? 70. : 1., (signal == "zemu") ? 110. : -1.);
  return 0;
}
