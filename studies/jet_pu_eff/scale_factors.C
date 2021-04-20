//Script to determine the MC efficiency of jet PU ID
int verbose_ = 0;

//Run over the TTree to generate plots
pair<TH2D*,TH2D*> get_histograms(TTree* t) {
  double ptbins[]  = {15., 20., 25., 30., 40.,
                      50.};
  double etabins[] = {-5.   , -3., -2.75, -2.5, -2. ,
                      -1.479,  0., 1.479,  2. ,  2.5,
                      2.75  ,  3.,
                      5.};
  TH2D* hJetsPUID = new TH2D("hJetsPUID", "Jet #eta vs p_{T}", 5, ptbins, 12, etabins);
  TH2D* hJets     = new TH2D("hJets"    , "Jet #eta vs p_{T}", 5, ptbins, 12, etabins);
  float jetsPt[100], jetsEta[100], jetsRejPt[100], jetsRejEta[100];
  UInt_t nJets20, nJets20Rej;
  t->SetBranchAddress("nJets20", &nJets20);
  t->SetBranchAddress("nJets20Rej", &nJets20Rej);
  t->SetBranchAddress("jetsPt", &jetsPt);
  t->SetBranchAddress("jetsEta", &jetsEta);
  t->SetBranchAddress("jetsRejPt", &jetsRejPt);
  t->SetBranchAddress("jetsRejEta", &jetsRejEta);
  for(Long64_t entry = 0; entry < t->GetEntriesFast(); ++entry) {
    t->GetEntry(entry);
    for(int ijet = 0; ijet < nJets20; ++ijet) {
      hJetsPUID->Fill(jetsPt[ijet], jetsEta[ijet]);
      hJets->Fill(jetsPt[ijet], jetsEta[ijet]);
    }
    for(int ijet = 0; ijet < nJets20Rej; ++ijet) {
      hJets->Fill(jetsRejPt[ijet], jetsRejEta[ijet]);
    }
  }
  return pair<TH2D*,TH2D*>(hJets, hJetsPUID);
}

//Generate the plots and scale factors
TCanvas* scale_factors(int year = 2016, TString selection = "mumu",
                       TString path = "ztautau_nanoaod_trees_nomva", TString filename = "DY50") {

  path = "root://cmseos.fnal.gov//store/user/mmackenz/" + path + "/";

  //////////////////////
  // Initialize files //
  //////////////////////

  //add the name of the file
  path += "clfv_";
  path += year;
  path += "_" + filename;
  path += ".tree";
  cout << "Opening file " << path.Data() << endl;
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TTree* t = (TTree*) f->Get(Form("%s/clfv_%i_%s", selection.Data(), year, filename.Data()));
  if(!t) {
    cout << "Tree not found in the given file!\n";
    return NULL;
  }

  cout << "Tree retrieved!\n";
  //////////////////////
  //  Get histograms  //
  //////////////////////

  auto jetpair = get_histograms(t);
  //Histograms without PU ID requirement
  TH2D* hJets = jetpair.first;
  //Histograms with PU ID requirement
  TH2D* hIDJets = jetpair.second;

  if(!hJets || !hIDJets) {
    cout << "Not all histograms found!\n";
    return NULL;
  }

  cout << "Histograms retrieved!\n";
  //////////////////////
  //  Draw jet hists  //
  //////////////////////

  TCanvas* c = new TCanvas("c_puid", "c_puid", 1600, 500);
  c->Divide(3,1);
  TVirtualPad* pad = c->cd(1);
  hJets->Draw("colz");
  pad->SetRightMargin(0.15);
  pad = c->cd(2);
  hIDJets->Draw("colz");
  pad->SetRightMargin(0.15);
  pad = c->cd(3);
  TH2D* hRatio = (TH2D*) hIDJets->Clone("hRatio");
  hRatio->Divide(hJets);
  hRatio->Draw("colz");
  //set z axis to be interesting
  double zmin(1.), zmax(0.);
  for(int xbin = 1; xbin <= hRatio->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hRatio->GetNbinsY(); ++ybin) {
      double binc = hRatio->GetBinContent(xbin,ybin);
      if(binc <= 0.) continue;
      zmin = min(zmin, binc);
      zmax = max(zmax, binc);
    }
  }
  hRatio->GetZaxis()->SetRangeUser(zmin*0.9, zmax*1.1);
  pad->SetRightMargin(0.15);
  gStyle->SetOptStat(0);

  //////////////////////
  //  Print results   //
  //////////////////////

  //ensure directories exist
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //construct general figure name
  TString name = "figures/jet_puid_";
  name += selection + "_";
  name += year;
  //print canvases
  c->Print((name+".png").Data());
  for(int ic = 1; ic <= 3; ++ic) {
    auto pad = c->cd(ic);
    pad->SetLogx();
  }
  c->Print((name+"_log.png").Data());
  //write out histogram ratios
  TFile* fOut = new TFile(Form("rootfiles/jet_puid_%s_%i.root", selection.Data(), year), "RECREATE");
  hRatio->Write();
  fOut->Close();
  return c;
}
