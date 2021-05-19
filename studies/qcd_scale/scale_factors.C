//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int drawFit_ = 0; //whether to draw the linear fits
int rebin_ = 1;

//Get the delta R histogram
TH1D* get_histogram(int setAbs, int isdata) {
  TH1D* h = 0;
  TString name = "lepdeltar1";

  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(dataplotter_->isData_[d] != (isdata > 0)) continue;
    TString hpath = Form("event_%i/%s", setAbs, name.Data());
    if(verbose_ > 0) cout << "Retrieving histogram " << hpath.Data() << " for " << dataplotter_->names_[d].Data() << endl;
    TH1D* hTmp = (TH1D*) dataplotter_->data_[d]->Get(hpath.Data());
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
  h->SetTitle("");
  h->SetXTitle("#DeltaR");
  h->SetYTitle("");
  if(rebin_ > 1) h->Rebin(rebin_);

  return h;
}

//make 1D ratio plots
TCanvas* make_ratio_canvas(TH1D* hnum, TH1D* hdnm, TF1 *&f1) {
  TCanvas* c = new TCanvas("c_ratio", "c_ratio", 800, 600);
  TVirtualPad* pad;
  TH1D* hRatio = (TH1D*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  hRatio->Divide(hdnm);

  //set bin error to 0 for <= bins to be ignored in the fit
  for(int jbin = 1; jbin <= hRatio->GetNbinsX(); ++jbin) {if(hRatio->GetBinContent(jbin) <= 0.) hRatio->SetBinError(jbin,0);}
  TString fit_option = (verbose_ > 1) ? "" : "q";
  if(!drawFit_) {
    fit_option += " 0";
    hRatio->Draw("E1");
  }
  hRatio->Fit("pol1", fit_option.Data());
  hRatio->SetLineWidth(2);
  hRatio->SetLineColor(kBlue);
  hRatio->SetMarkerStyle(20);
  hRatio->GetYaxis()->SetRangeUser(0.01, 5.);
  hRatio->SetYTitle("OS/SS");
  f1 = hRatio->GetFunction("pol1");
  if(drawFit_) {
    hRatio->Draw("E1");
    TH1D* herr_2s = (TH1D*) hRatio->Clone(Form("%s_err_2s", hRatio->GetName()));
    herr_2s->Reset();
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_2s, 0.95);
    herr_2s->SetFillColor(kYellow);
    // herr_2s->SetFillStyle(3001);
    herr_2s->Draw("E3 same");
    TH1D* herr_1s = (TH1D*) hRatio->Clone(Form("%s_err_1s", hRatio->GetName()));
    herr_1s->Reset();
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_1s, 0.68);
    herr_1s->SetFillColor(kGreen);
    // herr_1s->SetFillStyle(3001);
    herr_1s->Draw("E3 same");
    hRatio->Draw("E1 same"); //add again on top of confidence intervals
    f1->Draw("same");
  } else {
    gStyle->SetOptFit(0);
  }
  return c;
}

//make 1D plots
TCanvas* make_canvas(TH1D* hData, TH1D* hMC, TH1D* hQCD, TString name) {
  TCanvas* c = new TCanvas(("c_"+name).Data(), ("c_"+name).Data(), 800, 600);
  TVirtualPad* pad;
  hMC->Draw("hist");
  hMC->SetLineColor(kBlue);
  hMC->SetLineWidth(2);
  hMC->SetFillStyle(3003);
  hMC->SetFillColor(kBlue);
  hData->Draw("E1 same");
  hData->SetMarkerStyle(20);
  hData->SetLineWidth(2);
  hQCD->Draw("hist same");
  hQCD->SetLineColor(kRed);
  hQCD->SetLineWidth(2);
  hQCD->SetFillStyle(3003);
  hQCD->SetFillColor(kRed);
  hMC->GetYaxis()->SetRangeUser(0.1, 1.1*max(hData->GetMaximum(), hMC->GetMaximum()));

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hData, "Data");
  leg->AddEntry(hMC  , "MC");
  leg->AddEntry(hQCD , "QCD");
  leg->Draw();
  return c;
}

//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, TString path, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->verbose_ = verbose_;

  typedef DataCard_t dcard;
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
  cards.push_back(dcard(path+base+"WWW.hist"                , "WWW"                , "ZZ,WZ,WWW"         , false, xs.GetCrossSection("WWW"                ), false, year));
  cards.push_back(dcard(path+base+"WZ.hist"                 , "WZ"                 , "ZZ,WZ,WWW"         , false, xs.GetCrossSection("WZ"                 ), false, year));
  cards.push_back(dcard(path+base+"ZZ.hist"                 , "ZZ"                 , "ZZ,WZ,WWW"         , false, xs.GetCrossSection("ZZ"                 ), false, year));
  cards.push_back(dcard(path+base+"WW.hist"                 , "WW"                 , "WW"                , false, xs.GetCrossSection("WW"                 ), false, year));
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
Int_t scale_factors(TString selection = "emu", int set = 8, int year = 2016,
                    TString path = "nanoaods_dev") {

  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path + "/";

  //get the absolute value of the set, offsetting by the selection and using a loose selection
  int setAbs = set + ZTauTauHistMaker::fMisIDOffset;
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

  TH1D* hData[2]; //OS and SS
  TH1D* hMC[2]; //OS and SS
  hData[0] = get_histogram(setAbs+ZTauTauHistMaker::fQcdOffset, 1); //SS
  hMC[0]   = get_histogram(setAbs+ZTauTauHistMaker::fQcdOffset, 0); //SS
  hData[1] = get_histogram(setAbs                             , 1); //OS
  hMC[1]   = get_histogram(setAbs                             , 0); //OS

  if(!hData[0] || !hMC[0]) {
    cout << "Same sign histograms not found!\n";
    return 1;
  }
  if(!hData[1] || !hMC[1]) {
    cout << "Opposite sign histograms not found!\n";
    return 2;
  }

  ///////////////////////////
  // Get QCD contributions //
  ///////////////////////////

  TH1D* hQCD[2];
  for(int iqcd = 0; iqcd < 2; ++iqcd) {
    hQCD[iqcd] = (TH1D*) hData[iqcd]->Clone("hQCD_0");
    hQCD[iqcd]->Add(hMC[iqcd], -1);
    double norm = hQCD[iqcd]->Integral();
    for(int ibin = 1; ibin <= hQCD[iqcd]->GetNbinsX(); ++ibin) {
      if(hQCD[iqcd]->GetBinContent(ibin) < 0.) hQCD[iqcd]->SetBinContent(ibin, 0.);
    }
    hQCD[iqcd]->Scale(norm/hQCD[iqcd]->Integral());
  }


  //////////////////////
  // Draw fake rates  //
  //////////////////////
  gStyle->SetOptStat(0);

  TF1* f;
  TCanvas* c1 = make_canvas(hData[0], hMC[0], hQCD[0], "SS");
  TCanvas* c2 = make_canvas(hData[1], hMC[1], hQCD[1], "OS");
  TCanvas* c3 = make_ratio_canvas(hQCD[1], hQCD[0], f);

  //////////////////////
  //  Print results   //
  //////////////////////

  //ensure directories exist
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //construct general figure name
  TString name = "figures/qcd_";
  name += selection + "_";
  name += year;

  //print canvases
  c1->Print((name+"_SS.png").Data());
  c1->SetLogy();
  c1->Print((name+"_SS_log.png").Data());
  c2->Print((name+"_OS.png").Data());
  c2->SetLogy();
  c2->Print((name+"_OS_log.png").Data());
  c3->Print((name+"_scale.png").Data());
  c3->SetLogy();
  c3->Print((name+"_scale_log.png").Data());

  const char* fname = Form("rootfiles/qcd_scale_%s_%i.root", selection.Data(), year);
  TFile* fOut = new TFile(fname, "RECREATE");
  TH1D* hQCDScale = (TH1D*) hQCD[1]->Clone("hRatio");
  hQCDScale->Divide(hQCD[0]);
  for(int ibin = 1; ibin < hQCDScale->GetNbinsX(); ++ibin) {
    if(hQCDScale->GetBinContent(ibin) < 0.) hQCDScale->SetBinContent(ibin, 0.);
    if(hQCDScale->GetBinContent(ibin) > 5.) hQCDScale->SetBinContent(ibin, 5.);
  }
  hQCDScale->Write();
  f->SetName("fRatio");
  f->Write();
  fOut->Close();
  delete fOut;
  return 0;
}
