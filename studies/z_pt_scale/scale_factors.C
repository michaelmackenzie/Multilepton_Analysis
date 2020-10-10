//Script to define scale factors to apply to MC
//to correct the Z pT vs Mass distribution

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
double lum_;
int verbose_ = 0;
bool rebin_ = true; //rebin fixed with histogram to variable width

//create a histogram with variable bin widths from fixed with histogram
TH2F* create_rebinned_histogram(TH2F* h, const int& nbinsx, const double* xbins,
				const int& nbinsy, const double* ybins) {
  if(!h) return NULL;
  TH2F* hBinned = new TH2F(Form("%s_binned", h->GetName()), h->GetTitle(), nbinsx, xbins, nbinsy, ybins);
  int nx(0);
  for(int ix = 0; ix <= h->GetNbinsX()+1; ++ix) {
    if(ix == 0) nx = 0; //underflow
    else if(ix == h->GetNbinsX()+1) nx = nbinsx+1; //overflow
    else if(nx >= nbinsx+1) nx = nbinsx+1;
    else {
      while(h->GetXaxis()->GetBinLowEdge(ix) >= xbins[nx]) ++nx;
    }
    if(nx >= nbinsx+1) nx = nbinsx+1;
    int ny(0);
    for(int iy = 0; iy <= h->GetNbinsY()+1; ++iy) {
      if(iy == 0) ny = 0;
      else if(iy == h->GetNbinsY()+1) ny = nbinsy+1; //overflow
      else if(ny >= nbinsy+1) ny = nbinsy+1;
      else {
	while(h->GetYaxis()->GetBinLowEdge(iy) >= ybins[ny]) ++ny;
      }
      if(ny >= nbinsy+1) ny = nbinsy+1;
      double binc  = hBinned->GetBinContent(nx,ny);
      double bine  = hBinned->GetBinError(nx,ny);
      double origc = h->GetBinContent(ix,iy);
      double orige = h->GetBinError(ix,iy);
      hBinned->SetBinContent(nx, ny, binc+origc);
      hBinned->SetBinError(nx, ny, sqrt(bine*bine+orige*orige));
      if(verbose_ > 1) cout << "Bin old: (" << ix << ", " << iy << ") = "
			    << origc << "+-" << orige
			    <<" bin new: ("
			    << nx << ", " << ny << ") = "
			    << hBinned->GetBinContent(nx,ny) << "+-" << hBinned->GetBinError(nx,ny)
			    << endl;
    }
  }
  if(verbose_ > 0) cout << "Integral before rebinning = " << h->Integral()
			<< ", integral after rebinning = " << hBinned->Integral() << endl;
  return hBinned;
}

void get_pt_binning(double* binning, int &nbins) {
  nbins = 11;
  double bins[] = { 0.  , 10. , 15. , 20. , 25. ,
		    30. , 40. , 50. , 70. , 100.,
		    150., 1000.};
  for(int bin = 0; bin <= nbins; ++bin)
    binning[bin] = bins[bin];
}

void get_mass_binning(double* binning, int &nbins) {
      nbins = 21;
      double bins[] = { 0.  , 75. , 80. , 84. , 86. ,
			87. , 88. , 89. , 90. , 91. ,
			92. , 93. , 94. , 95. , 96. ,
			98. , 102., 105., 110.,	120.,
			140., 1000.};
  for(int bin = 0; bin <= nbins; ++bin)
    binning[bin] = bins[bin];
}

TH2F* get_mc_histogram(int set) {
  TH2F* h = 0;
  unsigned nfiles = dataplotter_->data_.size();
  for(unsigned d = 0; d < nfiles; ++d) {
    if(verbose_ > 0) cout << "Retrieving histogram for " << dataplotter_->names_[d].Data() << endl;
    TH2F* hTmp = (TH2F*) dataplotter_->data_[d]->Get(Form("event_%i/lepptvsm", set));
    if(!hTmp) continue;
    hTmp->Scale(dataplotter_->scale_[d]);
    if(!h) h = hTmp;
    else h->Add(hTmp);
  }
  return h;
}

Int_t initialize_plotter(bool useMuon, TString base, TString path) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->set_luminosity(lum_);
  dataplotter_->verbose_ =verbose_;
  
  typedef DataPlotter::DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal, color
  cards.push_back(dcard(path+base+"DY50.hist"               , "DY50"               , "Drell-Yan"         , false, 6225.42, false, kRed-7));
  cards.push_back(dcard(path+base+"SingleAntiToptW.hist"    , "SingleAntiToptW"    , "SingleTop"         , false, 34.91  , false, kCyan-7));
  cards.push_back(dcard(path+base+"SingleToptW.hist"        , "SingleToptW"        , "SingleTop"         , false, 34.91  , false, kCyan-7));
  cards.push_back(dcard(path+base+"WWW.hist"                , "WWW"	           , "ZZ,WZ,WWW"         , false, 0.2086 , false, kViolet-2));
  cards.push_back(dcard(path+base+"WZ.hist"                 , "WZ"                 , "ZZ,WZ,WWW"         , false, 27.6   , false, kViolet-2));
  cards.push_back(dcard(path+base+"ZZ.hist"                 , "ZZ"	           , "ZZ,WZ,WWW"         , false, 12.14  , false, kViolet-2));
  cards.push_back(dcard(path+base+"WW.hist"                 , "WW"	           , "WW"                , false, 12.178 , false, kViolet-9));
  cards.push_back(dcard(path+base+"Wlnu.hist"               , "Wlnu"               , "W+Jets"            , false, 52850.0, false, kGreen-7));
  cards.push_back(dcard(path+base+"ttbarToSemiLeptonic.hist", "ttbarToSemiLeptonic", "t#bar{t}"          , false, 365.34 , false, kYellow-7));
  cards.push_back(dcard(path+base+"ttbarlnu.hist"           , "ttbarlnu"           , "t#bar{t}"          , false, 88.29  , false, kYellow-7));

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}


TCanvas* scale_factors(bool useMuon = true, int set = 8, int year = 2016, TString path = "../../histograms/nanoaods_dev/") {
  //Get data histogram
  TString baseName = "ztautau_";
  baseName += (useMuon) ? "mumu_clfv_" : "ee_clfv_";
  baseName += year;
  baseName += "_";
  TString dataName = (useMuon) ? baseName+"SingleMu.hist" : baseName+"SingleEle.hist";
  TFile* fData = TFile::Open((path+dataName).Data(), "READ");
  if(!fData) return NULL;
  int setAbs = (useMuon) ? set+ZTauTauHistMaker::kMuMu : set+ZTauTauHistMaker::kEE;
  TH2F* hData = (TH2F*) fData->Get(Form("Data/event_%i/lepptvsm", setAbs));
  if(!hData) {
    cout << "2D data histogram not found!\n";
    return NULL;
  }
  hData->SetName("hData");
  if(year == 2016) lum_ = 35.92e3;
  else if(year == 2017) lum_ = 42.48e3;
  else if(year == 2018) lum_ = 59.74e3;

  if(initialize_plotter(useMuon, baseName, path)) {
    cout << "Dataplotter initialization failed!\n";
    return NULL;
  }

  TH2F* hMC = get_mc_histogram(setAbs);
  if(!hMC) {
    cout << "Failed to retrieve the MC histogram!\n";
    return NULL;
  }
  hMC->SetName("hMC");

  int n_pt_bins = 100;
  double pt_bins[n_pt_bins+1];
  get_pt_binning(pt_bins, n_pt_bins);
  int n_mass_bins = 100;
  double mass_bins[n_mass_bins+1];
  get_mass_binning(mass_bins, n_mass_bins);
  if(verbose_ > 1) {
    cout << "Z pT binning:\n";
    for(int bin = 0; bin <= n_pt_bins; ++bin)
      cout << "bin " << bin << ": " << pt_bins[bin] << endl;
    cout << "Z mass binning:\n";
    for(int bin = 0; bin <= n_mass_bins; ++bin)
      cout << "bin " << bin << ": " << mass_bins[bin] << endl;
  }
  if(rebin_ && verbose_ > 1) cout << "Rebinning data histogram\n";
  TH2F* hDataRebinned = (!rebin_) ? hData : create_rebinned_histogram(hData, n_mass_bins, mass_bins, n_pt_bins, pt_bins);
  TH2F* hMCRebinned   = (!rebin_) ? hMC   : create_rebinned_histogram(hMC  , n_mass_bins, mass_bins, n_pt_bins, pt_bins);
  
  TCanvas* c = new TCanvas("c_zpt", "c_zpt", 1200, 1000);
  c->Divide(2,2);
  TVirtualPad* pad = c->cd(1);
  hDataRebinned->SetTitle("Data: Di-lepton pT vs Mass");
  hDataRebinned->Draw("colz");
  hDataRebinned->GetXaxis()->SetRangeUser(1., 1000.);
  hDataRebinned->GetYaxis()->SetRangeUser(1., 1000.);
  hDataRebinned->SetXTitle("Mass (GeV/c^{2})");
  hDataRebinned->SetYTitle("pT (GeV/c)");
  pad->SetLogx();
  pad->SetLogy();
  pad->SetLogz();
  pad = c->cd(2);
  hMCRebinned->SetTitle("MC: Di-lepton pT vs Mass");
  hMCRebinned->Draw("colz");
  hMCRebinned->GetXaxis()->SetRangeUser(1., 1000.);
  hMCRebinned->GetYaxis()->SetRangeUser(1., 1000.);
  hMCRebinned->SetXTitle("Mass (GeV/c^{2})");
  hMCRebinned->SetYTitle("pT (GeV/c)");
  pad->SetLogx();
  pad->SetLogy();
  pad->SetLogz();
  pad = c->cd(3);
  TH2F* hRatio = (TH2F*) hDataRebinned->Clone("hRatio");
  hRatio->Divide(hMCRebinned);
  hRatio->SetTitle("Data/MC: Di-lepton pT vs Mass");
  hRatio->Draw("colz");
  hRatio->GetZaxis()->SetRangeUser(0.5, 1.5);
  pad->SetLogx();
  pad->SetLogy();
  pad = c->cd(4);
  TH2F* hRatioShape = (TH2F*) hRatio->Clone("hRatioNorm");
  double data_mc_scale = hDataRebinned->Integral()/hMCRebinned->Integral();
  cout << "Data / MC = " << data_mc_scale << endl;
  hRatioShape->Scale(data_mc_scale);
  hRatioShape->SetTitle("Normalized Data/MC: Di-lepton pT vs Mass");
  hRatioShape->Draw("colz");
  hRatioShape->GetXaxis()->SetRangeUser(1, 1000);
  hRatioShape->GetYaxis()->SetRangeUser(1, 1000);
  hRatioShape->GetZaxis()->SetRangeUser(0.5, 1.5);
  pad->SetLogx();
  pad->SetLogy();
  //save the images
  c->Print(Form("figures/pt_vs_m_%i.png", year));
  c->Print(Form("figures/pt_vs_m_%i.pdf", year));
  //save the scale factor histogram
  TFile* fout = new TFile(Form("z_pt_vs_m_scales_%i.root", year), "RECREATE");
  fout->cd();
  hRatioShape->Write();
  fout->Close();
  return c;
}
