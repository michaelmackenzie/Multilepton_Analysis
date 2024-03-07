//Read and plot from George's ntuple

struct Hist_t {
  TH1* hmva;
  TH1* hlepm;
  TH1* hweight;
  TH1* helec_wt;
  TH1* hmuon_wt;
  TH1* htrig_wt;
  TH1* hpu_wt;
  TH1* hzpt_wt;
  TH1* hsig_wt;
  TH1* hbtag_wt;
  TH1* hdxydz_wt;
};

struct Var_t {
  double mva;
  float lepm;
  float leppt;
  float lepeta;
  float lep1_ismuon;
  float lep2_ismuon;
  float lep1_pt;
  float lep1_eta;
  float lep2_pt;
  float lep2_eta;
  float ptratio;
  float met;
  float metsig;
  float metdphi;
  float mtone;
  float mttwo;
  float event;

  float Muon_RecoID_wt;
  float Electron_RecoID_wt;
  float Muon_ID_wt;
  float Electron_ID_wt;
  float Muon_IsoID_wt;
  float Electron_IsoID_wt;
  float PU_wt;
  float Gen_wt;
  float Prefire_wt;
  float PtZ_wt;
  float PtSignal_wt;
  float Trg_wt;
  float Muon_dxydz_wt;
  float Electron_dxydz_wt;
  float SFbtag_wt;

  float weight;
  Long64_t entry;
};

//------------------------------------------------------------------------------------------
void initialize_histograms(Hist_t& hist) {
  hist.hmva     = new TH1F("mva"   , "BDT score"         , 120, 0. , 1.2 );
  hist.hlepm    = new TH1F("lepm"  , "Di-lepton mass"    , 280, 40., 180.);
  hist.hweight  = new TH1F("weight", "Event weight"      , 100, 0. , 5.  );
  hist.helec_wt = new TH1F("elec_wt", "Electron weight"  ,  50, 0. , 2.  );
  hist.hmuon_wt = new TH1F("muon_wt", "Muon weight"      ,  50, 0. , 2.  );
  hist.htrig_wt = new TH1F("trig_wt", "Trigger weight"   ,  50, 0. , 2.  );
  hist.hpu_wt   = new TH1F("pu_wt"  , "Trigger weight"   ,  50, 0. , 2.  );
}

//------------------------------------------------------------------------------------------
void initialize_branches(TTree* tree, Var_t& vars) {
  tree->SetBranchAddress("xgb"               , &vars.mva               );
  tree->SetBranchAddress("mass_ll"           , &vars.lepm              );
  tree->SetBranchAddress("pt_ll"             , &vars.leppt             );
  tree->SetBranchAddress("eta_ll"            , &vars.lepeta            );
  tree->SetBranchAddress("evtnum"            , &vars.event             );
  tree->SetBranchAddress("isMuon_l1"         , &vars.lep1_ismuon       );
  tree->SetBranchAddress("isMuon_l2"         , &vars.lep2_ismuon       );
  tree->SetBranchAddress("pt_l1"             , &vars.lep1_pt           );
  tree->SetBranchAddress("pt_l2"             , &vars.lep2_pt           );
  tree->SetBranchAddress("eta_l1"            , &vars.lep1_eta          );
  tree->SetBranchAddress("eta_l2"            , &vars.lep2_eta          );
  tree->SetBranchAddress("ratio_ptl2_ptl1"   , &vars.ptratio           );
  tree->SetBranchAddress("met"               , &vars.met               );
  tree->SetBranchAddress("met_significance"  , &vars.metsig            );
  tree->SetBranchAddress("dphi_met_ll"       , &vars.metdphi           );
  tree->SetBranchAddress("mt_l1"             , &vars.mtone             );
  tree->SetBranchAddress("mt_l2"             , &vars.mttwo             );
  tree->SetBranchAddress("Electron_RecoID_wt", &vars.Electron_RecoID_wt);
  tree->SetBranchAddress("Electron_ID_wt"    , &vars.Electron_ID_wt    );
  tree->SetBranchAddress("Electron_IsoID_wt" , &vars.Electron_IsoID_wt );
  tree->SetBranchAddress("Muon_RecoID_wt"    , &vars.Muon_RecoID_wt    );
  tree->SetBranchAddress("Muon_ID_wt"        , &vars.Muon_ID_wt        );
  tree->SetBranchAddress("Muon_IsoID_wt"     , &vars.Muon_IsoID_wt     );
  tree->SetBranchAddress("PU_wt"             , &vars.PU_wt             );
  tree->SetBranchAddress("Gen_wt"            , &vars.Gen_wt            );
  tree->SetBranchAddress("Prefire_wt"        , &vars.Prefire_wt        );
  tree->SetBranchAddress("PtZ_wt"            , &vars.PtZ_wt            );
  tree->SetBranchAddress("PtSignal_wt"       , &vars.PtSignal_wt       );
  tree->SetBranchAddress("Trg_wt"            , &vars.Trg_wt            );
  tree->SetBranchAddress("SFbtag_wt"         , &vars.SFbtag_wt         );
  tree->SetBranchAddress("Muon_dxydz_wt"     , &vars.Muon_dxydz_wt     );
  tree->SetBranchAddress("Electron_dxydz_wt" , &vars.Electron_dxydz_wt );
}

//------------------------------------------------------------------------------------------
float get_weight(Var_t& vars) {
  float weight = 1.f;
  weight *= vars.Electron_RecoID_wt * vars.Electron_ID_wt * vars.Electron_IsoID_wt;
  weight *=                           vars.Muon_ID_wt     * vars.Muon_IsoID_wt    ;
  weight *= vars.PU_wt;
  weight *= vars.Prefire_wt;
  weight *= vars.PtZ_wt;
  weight *= vars.PtSignal_wt;
  weight *= vars.Trg_wt;
  if(weight <= 0.) printf("Warning! Entry %lld (Event %.0f) has <= 0 weight = %.4f\n",
                          vars.entry, vars.event, weight);
  // weight *= (vars.Gen_wt > 0.f) ? 1.f : -1.f;
  // weight *= vars.SFbtag_wt;
  // weight *= vars.Muon_dxydz_wt * vars.Electron_dxydz_wt;
  return weight;
}

//------------------------------------------------------------------------------------------
void fill_hists(Hist_t& hist, Var_t& vars) {
    hist.hmva ->Fill(vars.mva , vars.weight);
    hist.hlepm->Fill(vars.lepm, vars.weight);
    hist.hweight->Fill(vars.weight);
}

//------------------------------------------------------------------------------------------
void print_event(Var_t& vars) {
  printf("Entry %lld: Event %.0f, xgb = %.5f, mass = %.1f, weight = %.4f; Muon: pt = %.1f, eta = %.2f; Electron: pt = %.1f, eta = %.2f\n",
         vars.entry, vars.event, vars.mva, vars.lepm, vars.weight,
         (vars.lep1_ismuon) ? vars.lep1_pt  : vars.lep2_pt ,
         (vars.lep1_ismuon) ? vars.lep1_eta : vars.lep2_eta,
         (vars.lep1_ismuon) ? vars.lep2_pt  : vars.lep1_pt ,
         (vars.lep1_ismuon) ? vars.lep2_eta : vars.lep1_eta);
  printf(" MET = %.4f, MT(1) = %.4f, MT(2) = %.4f, pT(ll) = %.4f, pt_1_over_2 = %.4f, met_sig = %.4f, lep_eta = %.4f, met_dphi = %.4f\n",
         vars.met, vars.mtone, vars.mttwo, vars.leppt, vars.ptratio, vars.metsig, vars.lepeta, vars.metdphi);
  printf(" Event weights:\n");
  printf("  Elec: ID = %.3f, Iso ID = %.3f, Reco ID = %.3f\n", vars.Electron_ID_wt, vars.Electron_IsoID_wt, vars.Electron_RecoID_wt);
  printf("  Muon: ID = %.3f, Iso ID = %.3f, Reco ID = %.3f\n", vars.Muon_ID_wt    , vars.Muon_IsoID_wt    , vars.Muon_RecoID_wt    );
  printf("  PU = %.3f, Prefire = %.3f, Z pT = %.3f, Signal pT = %.3f, Trig = %.3f, B-tag = %.3f\n",
         vars.PU_wt, vars.Prefire_wt, vars.PtZ_wt, vars.PtSignal_wt, vars.Trg_wt, vars.SFbtag_wt);
}

//------------------------------------------------------------------------------------------
void draw_hist(TH1* hist, double xmin = 1., double xmax = -1., bool log = false) {
  hist->Draw("hist");
  hist->SetLineWidth(2);
  hist->SetLineColor(kBlue);
  hist->SetFillColor(kBlue);
  hist->SetFillStyle(3001);
  if(xmin < xmax) hist->GetXaxis()->SetRangeUser(xmin, xmax);
}

//------------------------------------------------------------------------------------------
void print_figures(Hist_t& hist, TString tag) {
  tag.ReplaceAll(".hist", "_figures");
  gSystem->Exec(Form("[ ! -d %s ] && mkdir %s", tag.Data(), tag.Data()));
  TCanvas* c = new TCanvas();
  draw_hist(hist.hmva);
  c->SaveAs(Form("%s/mva.png", tag.Data()));
  draw_hist(hist.hlepm, 70., 110.);
  c->SaveAs(Form("%s/lepm.png", tag.Data()));
  draw_hist(hist.hweight);
  c->SaveAs(Form("%s/weight.png", tag.Data()));
  delete c;
}

//------------------------------------------------------------------------------------------
// Main function
int plot_george_ntuple(const char* file_name = "~/nobackup/ZEMu/Meas_fullAndSF_Zemu_new.root", const char* file_out = "george.hist") {

  TFile* fIn = TFile::Open(file_name, "READ");
  if(!fIn) return 1;
  TTree* tree = (TTree*) fIn->Get("mytreefit");
  if(!tree) return 2;

  TFile* fOut = new TFile(file_out, "RECREATE");
  fOut->cd();
  Hist_t hist;
  initialize_histograms(hist);
  Var_t vars;
  initialize_branches(tree, vars);

  double yields[] = {0., 0., 0., 0., 0.};
  int    counts[] = {0 , 0 , 0 , 0 , 0 };
  const Long64_t nentries = tree->GetEntries();
  const Long64_t event_debug = 83697; //print an event's info
  const Long64_t entry_debug = -1; //print an entry's info
  const int print_category = -1; //print BDT category event numbers
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    tree->GetEntry(entry);
    vars.entry = entry;
    vars.weight = get_weight(vars);
    if(entry == entry_debug || vars.event == event_debug) print_event(vars);
    fill_hists(hist, vars);
    yields[0] += vars.weight; ++counts[0];
    int index = 0;
    if     (vars.mva < 0.3) {index = 1;}
    else if(vars.mva < 0.7) {index = 2;}
    else if(vars.mva < 0.9) {index = 3;}
    else                    {index = 4;}
    yields[index] += vars.weight; ++counts[index];
    if(print_category == 0 || print_category == index)
      printf("%6lld: %8.0f\n", entry, vars.event);
  }

  print_figures(hist, file_out);

  fOut->Write();
  fOut->Close();
  // const double scale = 6077.22 / (3. * 0.0337) * 2.62e-7 * (59.83 * 1000.) / (194000./6.); //N(gen) from George's table
  const double scale = 6077.22 / (3. * 0.0337) * 2.62e-7 * (59.83 * 1000.) / (194000. * 1e4 / 61936.); //Reduce N(gen) by 10k / N(ntuple entries)
  printf("Yields (scale factor = %.5f):\n", scale);
  printf(" BDT    :  count integral  yield\n");
  printf(" 0.0-1.0: %6i %8.1f %6.2f\n", counts[0], yields[0], scale*yields[0]);
  printf(" 0.0-0.3: %6i %8.1f %6.2f\n", counts[1], yields[1], scale*yields[1]);
  printf(" 0.3-0.7: %6i %8.1f %6.2f\n", counts[2], yields[2], scale*yields[2]);
  printf(" 0.7-0.9: %6i %8.1f %6.2f\n", counts[3], yields[3], scale*yields[3]);
  printf(" 0.9-1.0: %6i %8.1f %6.2f\n", counts[4], yields[4], scale*yields[4]);
  return 0;
}
