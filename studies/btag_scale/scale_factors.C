//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;

//Get the 2D jet pT vs eta histogram
TH2D* get_histogram(int set, int quark, bool useBTag, int WP) {
  TH2D* h = 0;
  //get the histogram name, depending on if requiring a b-tag
  TString name = "jetsptvseta";  
  if(useBTag) {
    if(WP == 2) //Loose
      name = "bjetslptvseta";
    else if(WP == 1) //Medium
      name = "bjetsmptvseta";
    else if(WP == 0) //Tight
      name = "bjetsptvseta";
    else {
      cout << "Unknown b-tag working point " << WP << "!\n";
      return NULL;
    }
  }
  if(quark < 0 || quark > 2) {
    cout << "Unknown jet flavor category " << quark << "!\n";
    return NULL;
  }
  //add the generator-level quark category
  name += quark;
  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    TString hpath = Form("event_%i/%s", set, name.Data());
    if(verbose_ > 0) cout << "Retrieving histogram " << hpath.Data() << " for " << dataplotter_->names_[d].Data() << endl;
    TH2D* hTmp = (TH2D*) dataplotter_->data_[d]->Get(hpath.Data());
    if(!hTmp) {
      if(verbose_ > 0) cout << "MC Histogram " << name.Data() << " for " << dataplotter_->names_[d].Data() << " not found!\n";
      continue;
    }
    // ignore luminosity/normalization scaling --> 1 entry ~ weight of 1
    // hTmp->Scale(dataplotter_->scale_[d]);
    if(!h) h = hTmp;
    else h->Add(hTmp);
  }
  
  if(!h) return NULL;

  //setup the histogram title and axis titles
  if(useBTag) {
    if(WP == 2)      name = "Loose";
    else if(WP == 1) name = "Medium";
    else if(WP == 0) name = "Tight";
    name += " B-Tagged Jet pT vs #eta for gen-level";
  } else       name = "Jet pT vs #eta for gen-level";
  if(quark == 0)      name += " light Jets";
  else if(quark == 1) name += " C-Jets";
  else if(quark == 2) name += " B-Jets";
  h->SetTitle(name.Data());
  h->SetXTitle("#eta");
  h->SetYTitle("pT (GeV/c)");
  return h;
}

//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, TString path, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->set_luminosity(1.e3); //lum per inv fb
  dataplotter_->verbose_ = verbose_;
  
  typedef DataPlotter::DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal
  CrossSections xs; //cross section handler
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

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}


//Generate the plots and scale factors
TCanvas* scale_factors(TString selection = "mumu", int set = 7, int year = 2016, int WP = 0,
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
    return NULL;
  }

  //////////////////////
  //  Get histograms  //
  //////////////////////

  //Histograms without b-tag requirement
  TH2D* hLJets = get_histogram(setAbs, 0, false, WP); //light jets
  TH2D* hCJets = get_histogram(setAbs, 1, false, WP); //c-jets
  TH2D* hBJets = get_histogram(setAbs, 2, false, WP); //b-jets
  //Histograms with b-tag requirement
  TH2D* hLBJets = get_histogram(setAbs, 0, true, WP); //light jets
  TH2D* hCBJets = get_histogram(setAbs, 1, true, WP); //c-jets
  TH2D* hBBJets = get_histogram(setAbs, 2, true, WP); //b-jets

  if(!hLJets || !hCJets || !hBJets || !hLBJets || !hCBJets || !hBBJets) {
    cout << "Not all histograms found!\n";
    return NULL;
  }
  
  //////////////////////
  //  Draw jet rates  //
  //////////////////////

  TCanvas* c = new TCanvas("c_btag", "c_btag", 1600, 900);
  c->Divide(3,2);
  TVirtualPad* pad = c->cd(1);
  hLBJets->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogy();
  pad = c->cd(2);
  hCBJets->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogy();
  pad = c->cd(3);
  hBBJets->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogy();
  pad = c->cd(4);
  hLJets->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogy();
  pad = c->cd(5);
  hCJets->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogy();
  pad = c->cd(6);
  hBJets->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogy();

  //////////////////////
  // Draw b-jet effs  //
  //////////////////////

  TCanvas* c2 = new TCanvas("c_ratio", "c_ratio", 1600, 450);
  TString wp = "Loose";
  if(WP == 1) wp = "Medium";
  else if(WP == 0) wp = "Tight";
  c2->Divide(3,1);
  pad = c2->cd(1);
  TH2D* hLRatio = (TH2D*) hLBJets->Clone("hLRatio");
  hLRatio->Divide(hLJets);
  hLRatio->Draw("colz");
  hLRatio->GetZaxis()->SetRangeUser(hLRatio->GetMinimum()*0.9, min(1.2, hLRatio->GetMaximum()*1.1));
  hLRatio->SetTitle((wp + " B-Tagged MC efficiency for gen-level light Jets").Data());
  hLRatio->SetXTitle("#eta");
  hLRatio->SetYTitle("pT (GeV/c)");
  pad->SetRightMargin(0.13);
  pad->SetLeftMargin(0.1);
  pad->SetLogy();
  pad = c2->cd(2);
  TH2D* hCRatio = (TH2D*) hCBJets->Clone("hCRatio");
  hCRatio->Divide(hCJets);
  hCRatio->Draw("colz");
  hCRatio->GetZaxis()->SetRangeUser(hCRatio->GetMinimum()*0.9, min(1.2, hCRatio->GetMaximum()*1.1));
  hCRatio->SetTitle((wp + " B-Tagged MC efficiency for gen-level C-Jets").Data());
  hCRatio->SetXTitle("#eta");
  hCRatio->SetYTitle("pT (GeV/c)");
  pad->SetRightMargin(0.13);
  pad->SetLeftMargin(0.1);
  pad->SetLogy();
  pad = c2->cd(3);
  TH2D* hBRatio = (TH2D*) hBBJets->Clone("hBRatio");
  hBRatio->Divide(hBJets);
  hBRatio->Draw("colz");
  hBRatio->GetZaxis()->SetRangeUser(hBRatio->GetMinimum()*0.9, min(1.2, hBRatio->GetMaximum()*1.1));
  hBRatio->SetTitle((wp + " B-Tagged MC efficiency for gen-level B-Jets").Data());
  hBRatio->SetXTitle("#eta");
  hBRatio->SetYTitle("pT (GeV/c)");
  pad->SetRightMargin(0.13);
  pad->SetLeftMargin(0.1);
  pad->SetLogy();
  
  gStyle->SetOptStat(0);

  //////////////////////
  //  Print results   //
  //////////////////////

  //ensure directories exist
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //construct general figure name
  TString name = "figures/jet_wp_";
  name += WP;
  name += "_" + selection + "_";
  name += year;
  //print canvases
  c->Print((name+"_rate.png").Data());
  c2->Print((name+"_eff.png").Data());
  //write out histogram ratios
  TFile* fOut = new TFile(Form("rootfiles/btag_eff_wp_%i_%s_%i.root", WP, selection.Data(), year), "RECREATE");
  hLRatio->Write();
  hCRatio->Write();
  hBRatio->Write();
  fOut->Close();
  delete fOut;
  return c2;
}
