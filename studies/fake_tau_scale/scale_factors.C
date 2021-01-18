//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;

//Get the 2D jet pT vs eta histogram
TH2D* get_histogram(int setAbs, int ijet, int idm, bool isdata, int icat) {
  TH2D* h = 0;
  TString name = Form("faketau%ijet%idm_%i", ijet, idm, icat);

  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(dataplotter_->isData_[d] != isdata) continue;
    TString hpath = Form("event_%i/%s", setAbs, name.Data());
    if(verbose_ > 0) cout << "Retrieving histogram " << hpath.Data() << " for " << dataplotter_->names_[d].Data() << endl;
    TH2D* hTmp = (TH2D*) dataplotter_->data_[d]->Get(hpath.Data());
    if(!hTmp) {
      if(verbose_ > 0) cout << "Histogram " << name.Data() << " for " << dataplotter_->names_[d].Data() << " not found!\n";
      continue;
    }
    // ignore luminosity/normalization scaling --> 1 entry ~ weight of 1
    if(!isdata) hTmp->Scale(dataplotter_->scale_[d]);
    if(!h) h = hTmp;
    else h->Add(hTmp);
  }
  
  if(!h) return NULL;

  //setup the histogram title and axis titles
  h->SetTitle(name.Data());
  h->SetXTitle("pT (GeV/c)");
  h->SetYTitle("#eta");
  return h;
}

//make a rate canvas
TCanvas* make_jet_canvas(TH2D* h[5][4], TString name) {
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1500, 900);
  TVirtualPad* pad;
  c->Divide(3,2);
  pad = c->cd(1);
  h[0][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(2);
  h[1][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(3);
  h[2][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(4);
  h[3][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(5);
  h[4][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(6);
  h[0][1]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  return c;
}

//make 1D plots in eta regions
TCanvas* make_eta_region_canvas(TH2D* hnum, TH2D* hdnm, TString name, bool iseff = false) {
  int nbins = hnum->GetNbinsY();
  TH1D* hetas[nbins];
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 500*nbins, 450);
  TVirtualPad* pad;
  c->Divide(nbins, 1);
  for(int ibin = 0; ibin < nbins; ++ibin) {
    hetas[ibin] = hnum->ProjectionX(Form("%s_eta%i", hnum->GetName(), ibin), ibin+1, ibin+1);
    hetas[ibin]->Divide(hdnm->ProjectionX(Form("%s_tmp_eta%i", hdnm->GetName(), ibin), ibin+1, ibin+1));
    pad = c->cd(ibin+1);
    hetas[ibin]->Draw("E1");
    hetas[ibin]->SetLineWidth(2);
    hetas[ibin]->SetMarkerStyle(6);
    hetas[ibin]->SetTitle(hetas[ibin]->GetName());
    if(iseff) hetas[ibin]->GetYaxis()->SetRangeUser(0., 1.);
    pad->SetLogx();
  }
  return c;
}

//make a rate canvas by DM
TCanvas* make_canvas(TH2D* h[4], TString name, bool iseff = false) {
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1000, 900);
  TVirtualPad* pad;
  c->Divide(2,2);
  pad = c->cd(1);
  h[0]->Draw("colz");
  if(iseff)
    h[0]->GetZaxis()->SetRangeUser(0.,1.2*h[0]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(2);
  h[1]->Draw("colz");
  if(iseff)
    h[1]->GetZaxis()->SetRangeUser(0.,1.2*h[1]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(3);
  h[2]->Draw("colz");
  if(iseff)
    h[2]->GetZaxis()->SetRangeUser(0.,1.2*h[2]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(4);
  h[3]->Draw("colz");
  if(iseff)
    h[3]->GetZaxis()->SetRangeUser(0.,1.2*h[3]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  return c;
}

//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, TString path, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->verbose_ = verbose_;
  
  typedef DataPlotter::DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal
  CrossSections xs; //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(year == 2018) 
    cards.push_back(dcard(path+base+"DY50.hist"             , "DY50"               , "Drell-Yan"         , false, xs.GetCrossSection("DY50"               ), false, year));
  else
    cards.push_back(dcard(path+base+"DY50-ext.hist"         , "DY50"               , "Drell-Yan"         , false, xs.GetCrossSection("DY50"               ), false, year));
  cards.push_back(dcard(path+base+"SingleAntiToptW.hist"    , "SingleAntiToptW"    , "SingleTop"         , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year));
  cards.push_back(dcard(path+base+"SingleToptW.hist"        , "SingleToptW"        , "SingleTop"         , false, xs.GetCrossSection("SingleToptW"        ), false, year));
  cards.push_back(dcard(path+base+"WWW.hist"                , "WWW"	           , "ZZ,WZ,WWW"         , false, xs.GetCrossSection("WWW"	          ), false, year));
  cards.push_back(dcard(path+base+"WZ.hist"                 , "WZ"                 , "ZZ,WZ,WWW"         , false, xs.GetCrossSection("WZ"                 ), false, year));
  cards.push_back(dcard(path+base+"ZZ.hist"                 , "ZZ"	           , "ZZ,WZ,WWW"         , false, xs.GetCrossSection("ZZ"	          ), false, year));
  cards.push_back(dcard(path+base+"WW.hist"                 , "WW"	           , "WW"                , false, xs.GetCrossSection("WW"	          ), false, year));
  cards.push_back(dcard(path+base+"Wlnu.hist"               , "Wlnu"               , "W+Jets"            , false, xs.GetCrossSection("Wlnu"               ), false, year));
  cards.push_back(dcard(path+base+"ttbarToSemiLeptonic.hist", "ttbarToSemiLeptonic", "t#bar{t}"          , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year));
  cards.push_back(dcard(path+base+"ttbarlnu.hist"           , "ttbarlnu"           , "t#bar{t}"          , false, xs.GetCrossSection("ttbarlnu"           ), false, year));
  if(!base.Contains("etau") && !base.Contains("ee"))
    cards.push_back(dcard(path+base+"SingleMu.hist", "SingleMu", "Data", true, 1., false, year));
  if(!base.Contains("mutau") && !base.Contains("mumu"))
    cards.push_back(dcard(path+base+"SingleEle.hist", "SingleEle", "Data", true, 1., false, year));

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}


//Generate the plots and scale factors
Int_t scale_factors(TString selection = "mumu", int set = 7, int year = 2016,
		       TString path = "../../histograms/nanoaods_dev/") {

  //////////////////////
  // Initialize files //
  //////////////////////

  //get the absolute value of the set, offsetting by the selection
  int setAbs = set;
  if(selection == "mutau")      setAbs += ZTauTauHistMaker::kMuTau;
  else if(selection == "etau" ) setAbs += ZTauTauHistMaker::kETau ;
  else if(selection == "emu"  ) setAbs += ZTauTauHistMaker::kEMu  ;
  else if(selection == "mumu" ) setAbs += ZTauTauHistMaker::kMuMu ;
  else if(selection == "ee"   ) setAbs += ZTauTauHistMaker::kEE   ;
  //construct the general name of each file, not including the sample name
  TString baseName = "ztautau_" + selection + "_clfv_";
  baseName += year;
  baseName += "_";

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, path, year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  //////////////////////
  //  Get histograms  //
  //////////////////////
  
  const int njetmax = 4;
  const int ndm = 4;
  const int ncat = 3;
  TH2D* hMCFakeTauRates[ncat][njetmax+1][ndm];
  TH2D* hDataFakeTauRates[ncat][njetmax+1][ndm];
  TH2D* hMCFakeTauRates_j[ncat][ndm];
  TH2D* hDataFakeTauRates_j[ncat][ndm];
  TH2D* hMCFakeTauEffs_j[ndm];
  TH2D* hDataFakeTauEffs_j[ndm];
  for(int idm = 0; idm < ndm; ++idm) {
    for(int icat = 0; icat < ncat; ++icat) {
      hMCFakeTauRates_j[icat][idm] = 0;
      hDataFakeTauRates_j[icat][idm] = 0;
      for(int ijet = 0; ijet <= njetmax; ++ijet) {
	hMCFakeTauRates[icat][ijet][idm] = get_histogram(setAbs, ijet, idm, false, icat);
	if(!hMCFakeTauRates[icat][ijet][idm]) {
	  cout << "MC Histogram for ijet = " << ijet << " idm = " << idm
	       << " and icat = " << icat << " not found!\n";
	  return 2;
	}
	hDataFakeTauRates[icat][ijet][idm] = get_histogram(setAbs, ijet, idm, true, icat);
	if(!hDataFakeTauRates[icat][ijet][idm]) {
	  cout << "Data Histogram for ijet = " << ijet << " idm = " << idm
	       << " and icat = " << icat << " not found!\n";
	  return 3;
	}
	//ensure weight sum objects are created
	if(!hMCFakeTauRates[icat][ijet][idm]->GetSumw2N()) hMCFakeTauRates[icat][ijet][idm]->Sumw2();
	if(!hDataFakeTauRates[icat][ijet][idm]->GetSumw2N()) hDataFakeTauRates[icat][ijet][idm]->Sumw2();
	if(!(hMCFakeTauRates_j[icat][idm]))
	  hMCFakeTauRates_j[icat][idm] = (TH2D*) hMCFakeTauRates[icat][ijet][idm]->Clone(Form("hmc_%icat_%idm", icat, idm));
	else
	  hMCFakeTauRates_j[icat][idm]->Add(hMCFakeTauRates[icat][ijet][idm]);
	if(!(hDataFakeTauRates_j[icat][idm]))
	  hDataFakeTauRates_j[icat][idm] = (TH2D*) hDataFakeTauRates[icat][ijet][idm]->Clone(Form("hdata_%icat_%idm", icat, idm));
	else
	  hDataFakeTauRates_j[icat][idm]->Add(hDataFakeTauRates[icat][ijet][idm]);
	//subtract MC expectation of true taus from data
	hDataFakeTauRates[icat][ijet][idm]->Add(hMCFakeTauRates[icat][ijet][idm], -1);
      }
      hMCFakeTauRates_j[icat][idm]->SetTitle(hMCFakeTauRates_j[icat][idm]->GetName());
      hDataFakeTauRates_j[icat][idm]->SetTitle(hDataFakeTauRates_j[icat][idm]->GetName());
      hDataFakeTauRates_j[icat][idm]->Add(hMCFakeTauRates_j[icat][idm], -1.);
    }
    //calculate eff of tight / (anti-iso + tight)
    hMCFakeTauEffs_j[idm] = (TH2D*) hMCFakeTauRates_j[1][idm]->Clone(Form("hmc_eff_%idm", idm));
    hMCFakeTauEffs_j[idm]->Divide(hMCFakeTauRates_j[0][idm]);
    hDataFakeTauEffs_j[idm] = (TH2D*) hDataFakeTauRates_j[1][idm]->Clone(Form("hdata_eff_%idm", idm));
    hDataFakeTauEffs_j[idm]->Divide(hDataFakeTauRates_j[0][idm]);
    hMCFakeTauEffs_j[idm]->SetTitle(hMCFakeTauEffs_j[idm]->GetName());
    hDataFakeTauEffs_j[idm]->SetTitle(hDataFakeTauEffs_j[idm]->GetName());
  }
  
  //////////////////////
  // Draw fake rates  //
  //////////////////////
  gStyle->SetOptStat(0);

  // TCanvas* c1  = make_jet_canvas(hDataFakeTauRates[0], "c_all_jetbinned_data");
  // TCanvas* c2  = make_jet_canvas(hMCFakeTauRates[0], "c_all_jetbinned_mc");
  // TCanvas* c3  = make_jet_canvas(hDataFakeTauRates[1], "c_tight_jetbinned_data");
  // TCanvas* c4  = make_jet_canvas(hMCFakeTauRates[1], "c_tight_jetbinned_mc");
  // TCanvas* c5  = make_jet_canvas(hDataFakeTauRates[2], "c_loose_jetbinned_data");
  // TCanvas* c6  = make_jet_canvas(hMCFakeTauRates[2], "c_loose_jetbinned_mc");
  TCanvas* c7  = make_canvas(hDataFakeTauRates_j[0], "c_all_data");
  TCanvas* c8  = make_canvas(hMCFakeTauRates_j[0], "c_all_mc");
  TCanvas* c9  = make_canvas(hDataFakeTauRates_j[1], "c_tight_data");
  TCanvas* c10 = make_canvas(hMCFakeTauRates_j[1], "c_tight_mc");
  TCanvas* c11 = make_canvas(hDataFakeTauRates_j[2], "c_loose_data");
  TCanvas* c12 = make_canvas(hMCFakeTauRates_j[2], "c_loose_mc");
  TCanvas* c13 = make_canvas(hDataFakeTauEffs_j, "c_eff_data", true);
  TCanvas* c14 = make_canvas(hMCFakeTauEffs_j, "c_eff_mc", true);

  //////////////////////
  //  Print results   //
  //////////////////////

  //ensure directories exist
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //construct general figure name
  TString name = "figures/fake_tau_";
  name += selection + "_";
  name += year;
  //print canvases
  // c1->Print((name+"_jetbinned_data_tot_rate.png").Data());
  // c2->Print((name+"_jetbinned_mc_tot_rate.png").Data());
  // c3->Print((name+"_jetbinned_data_tight_rate.png").Data());
  // c4->Print((name+"_jetbinned_mc_tight_rate.png").Data());
  // c5->Print((name+"_jetbinned_data_loose_rate.png").Data());
  // c6->Print((name+"_jetbinned_mc_loose_rate.png").Data());
  c7->Print((name+"_data_tot_rate.png").Data());
  c8->Print((name+"_mc_tot_rate.png").Data());
  c9->Print((name+"_data_tight_rate.png").Data());
  c10->Print((name+"_mc_tight_rate.png").Data());
  c11->Print((name+"_data_loose_rate.png").Data());
  c12->Print((name+"_mc_loose_rate.png").Data());
  c13->Print((name+"_data_eff.png").Data());
  c14->Print((name+"_mc_eff.png").Data());
  for(int idm = 0; idm < ndm; ++idm) {
    TCanvas* c = make_eta_region_canvas(hDataFakeTauRates_j[1][idm], hDataFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_data", idm), true);
    c->Print(Form("%s_data_dm%i_eff.png", name.Data(), idm));
    c = make_eta_region_canvas(hMCFakeTauRates_j[1][idm], hMCFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_mc", idm), true);
    c->Print(Form("%s_mc_dm%i_eff.png", name.Data(), idm));
  }
  
  TFile* fOut = new TFile(Form("rootfiles/jet_to_tau_%s_%i.root", selection.Data(), year), "RECREATE");
  hDataFakeTauEffs_j[0]->Write();
  hDataFakeTauEffs_j[1]->Write();
  hDataFakeTauEffs_j[2]->Write();
  hDataFakeTauEffs_j[3]->Write();
  hMCFakeTauEffs_j[0]->Write();
  hMCFakeTauEffs_j[1]->Write();
  hMCFakeTauEffs_j[2]->Write();
  hMCFakeTauEffs_j[3]->Write();
  fOut->Close();
  delete fOut;
  return 0;
}
