//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
bool doMC_ = false; //perform the study on MC

//Get the 2D jet pT vs eta histogram
TH2D* get_histogram(int setAbs, bool isdata, int icat) {
  TH2D* h = 0;
  TString name = Form("fakeleppteta_%i", icat);

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
  if(verbose_ > 0) cout << "Finished retrieving histogram!\n";
  return h;
}

//make 2D plots
TCanvas* make_canvas(TH2D* hrates[3], TString name, bool iseff = true) {
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1000, 450);
  TVirtualPad* pad;
  c->Divide(2, 1);
  pad = c->cd(1);
  hrates[1]->Draw("colz");
  hrates[1]->SetTitle(hrates[2]->GetName());
  if(iseff) hrates[0]->GetYaxis()->SetRangeUser(0., 1.2);
  pad->SetLogx();
  pad = c->cd(2);
  hrates[2]->Draw("colz");
  hrates[2]->SetTitle(hrates[2]->GetName());
  if(iseff) hrates[2]->GetYaxis()->SetRangeUser(0., 1.2);
  pad->SetLogx();
  return c;
}

//make 1D plots in eta regions
TCanvas* make_eta_region_canvas(TH2D* hnum, TH2D* hdnm, TString name, bool iseff = false, bool entire_eta = false) {
  int nbins = (entire_eta) ? 1 : hnum->GetNbinsY();
  TH1D* hetas[nbins];
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 500*nbins, 450);
  TVirtualPad* pad;
  c->Divide(nbins, 1);
  for(int ibin = 0; ibin < nbins; ++ibin) {
    int bin0 = (entire_eta) ? 1 : ibin+1;
    int bin1 = (entire_eta) ? hnum->GetNbinsY() : ibin+1;
    hetas[ibin] = hnum->ProjectionX(Form("%s_eta%i", hnum->GetName(), ibin), bin0, bin1);
    hetas[ibin]->Divide(hdnm->ProjectionX(Form("%s_tmp_eta%i", hdnm->GetName(), ibin), bin0, bin1));
    pad = c->cd(ibin+1);
    hetas[ibin]->Draw("E1");
    hetas[ibin]->SetLineWidth(2);
    hetas[ibin]->SetMarkerStyle(6);
    hetas[ibin]->SetTitle(hetas[ibin]->GetName());
    if(iseff) hetas[ibin]->GetYaxis()->SetRangeUser(0., 1.2);
    pad->SetLogx();
  }
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
  
  const int ncat = 3;
  TH2D* hMCFakeLepRates[ncat];
  TH2D* hDataFakeLepRates[ncat];
  TH2D* hMCFakeLepEffs;
  TH2D* hDataFakeLepEffs;
  for(int icat = 0; icat < ncat; ++icat) {
    if(verbose_ > 0) cout << "Beginning Histogram retrieval for  icat = " << icat << endl;
    if(doMC_) {
      hMCFakeLepRates[icat] = get_histogram(setAbs, false, icat);
      if(!hMCFakeLepRates[icat]) {
	cout << "MC Histogram for  icat = " << icat << " not found!\n";
	return 2;
      }
    }
    hDataFakeLepRates[icat] = get_histogram(setAbs, true, icat);
    if(!hDataFakeLepRates[icat]) {
      cout << "Data Histogram for icat = " << icat << " not found!\n";
      return 3;
    }
    if(verbose_ > 0) cout << "Histograms for icat = " << icat << " retrieved!\n";
    //ensure weight sum objects are created
    if(doMC_ && !hMCFakeLepRates[icat]->GetSumw2N()) hMCFakeLepRates[icat]->Sumw2();
    if(!hDataFakeLepRates[icat]->GetSumw2N()) hDataFakeLepRates[icat]->Sumw2();
  }
  if(verbose_ > 0) cout << "Finished retrieving all histograms!\n";

  //calculate eff of tight / (anti-iso + tight)
  if(doMC_) {
    hMCFakeLepEffs = (TH2D*) hMCFakeLepRates[1]->Clone("hmc_eff_2d");
    hMCFakeLepEffs->Divide(hMCFakeLepRates[0]);
    hMCFakeLepEffs->SetTitle(hMCFakeLepEffs->GetName());
  }
  hDataFakeLepEffs = (TH2D*) hDataFakeLepRates[1]->Clone("hdata_eff_2d");
  hDataFakeLepEffs->Divide(hDataFakeLepRates[0]);
  hDataFakeLepEffs->SetTitle(hDataFakeLepEffs->GetName());
  
  //////////////////////
  // Draw fake rates  //
  //////////////////////
  gStyle->SetOptStat(0);

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
  TCanvas* c;
  c = make_canvas(hDataFakeLepRates, "c_rate_data", false);
  c->Print(Form("%s_data_rate_vs_eta.png", name.Data()));
  // c = make_canvas(hDataFakeLepEffs, "c_eff_data", true);
  c = make_eta_region_canvas(hDataFakeLepRates[1], hDataFakeLepRates[0], "c_eta_eff_data", true);
  c->Print(Form("%s_data_eff_vs_eta.png", name.Data()));
  c = make_eta_region_canvas(hDataFakeLepRates[1], hDataFakeLepRates[0], "c_eff_data", true, true);
  c->Print(Form("%s_data_eff.png", name.Data()));
  if(doMC_) {
    c = make_canvas(hMCFakeLepRates, "c_rate_mc", false);
    c = make_eta_region_canvas(hMCFakeLepRates[1], hMCFakeLepRates[0], "c_eta_eff_mc", true);
    c->Print(Form("%s_mc_eff_vs_eta.png", name.Data()));
    c = make_eta_region_canvas(hMCFakeLepRates[1], hMCFakeLepRates[0], "c_eff_mc", true, true);
    c->Print(Form("%s_mc_eff.png", name.Data()));
  }
  
  if(verbose_ > 0) cout << "Finished plotting all histograms!\n";
  TFile* fOut = new TFile(Form("rootfiles/jet_to_lep_%s_%i.root", selection.Data(), year), "RECREATE");
  hDataFakeLepEffs->Write();
  TH1D* hDataEff = hDataFakeLepRates[1]->ProjectionX("hdata_eff_1d", 1, hDataFakeLepRates[0]->GetNbinsY());
  hDataEff->Divide(hDataFakeLepRates[0]->ProjectionX());
  hDataEff->Write();
  if(doMC_)
    hMCFakeLepEffs->Write();
  fOut->Close();
  delete fOut;
  if(verbose_ > 0) cout << "Finished!\n";
  return 0;
}
