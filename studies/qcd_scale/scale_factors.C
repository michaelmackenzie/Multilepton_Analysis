//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int drawFit_ = 1; //whether to draw the linear fits
int rebin_ = 1;
int splitWJ_ = 1;

//Get 1D histograms
TH1D* get_histogram(TString name, int setAbs, int isdata, TString type = "event") {
  TH1D* h = 0;

  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(dataplotter_->isData_[d] != (isdata > 0)) continue;
    TString hpath = Form("%s_%i/%s", type.Data(), setAbs, name.Data());
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
  if(name.Contains("deltar"))
    h->SetXTitle("#DeltaR");
  else if(name.Contains("deltaphi"))
    h->SetXTitle("#Delta#phi");
  h->SetYTitle("");
  if(rebin_ > 1) h->Rebin(rebin_);

  return h;
}

//Get 2D histograms
TH2D* get_2D_histogram(TString name, int setAbs, int isdata, TString type = "event") {
  TH2D* h = 0;
  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(dataplotter_->isData_[d] != (isdata > 0)) continue;
    TString hpath = Form("%s_%i/%s", type.Data(), setAbs, name.Data());
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
  h->SetTitle("");
  h->SetXTitle("#eta");
  if(name.Contains("delr"))
    h->SetYTitle("#DeltaR");
  else if(name.Contains("delphi"))
    h->SetYTitle("#Delta#phi");

  return h;
}

//Get the QCD histogram for a given histogram name and set
TH1D* get_qcd_histogram(TString name, int setAbs, TString type = "event") {
  TH1D* hQCD = get_histogram(name, setAbs, 1, type);
  if(!hQCD) {
    cout << "QCD histogram building for " << name << " failed!\n";
    return 0;
  }
  hQCD->Add(get_histogram(name, setAbs, 0), -1.);
  hQCD->SetName(Form("QCD_%s_%i", name.Data(), setAbs));
  double norm = hQCD->Integral();
  for(int ibin = 1; ibin <= hQCD->GetNbinsX(); ++ibin) {
    if(hQCD->GetBinContent(ibin) < 0.) hQCD->SetBinContent(ibin, 0.);
  }
  hQCD->Scale(norm/hQCD->Integral());
  return hQCD;
}

//Get the 2D QCD histogram for a given histogram name and set
TH2D* get_2D_qcd_histogram(TString name, int setAbs, TString type = "event") {
  TH2D* hQCD = get_2D_histogram(name, setAbs, 1, type);
  if(!hQCD) {
    cout << "QCD histogram building for " << name << " failed!\n";
    return 0;
  }
  hQCD->Add(get_2D_histogram(name, setAbs, 0), -1.);
  hQCD->SetName(Form("QCD_%s_%i", name.Data(), setAbs));
  double norm = hQCD->Integral();
  for(int xbin = 1; xbin <= hQCD->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hQCD->GetNbinsY(); ++ybin) {
      if(hQCD->GetBinContent(xbin,ybin) < 0.) hQCD->SetBinContent(xbin, ybin, 0.);
    }
  }
  hQCD->Scale(norm/hQCD->Integral());
  return hQCD;
}

//make 1D ratio plots
TCanvas* make_ratio_canvas(TH1D* hnum, TH1D* hdnm, TF1 *&f1, bool print = false, bool doFit = true, double xmin = 0., double xmax = 5., int rebin = 1) {
  TCanvas* c = new TCanvas("c_ratio", "c_ratio", 800, 600);
  TVirtualPad* pad;
  TH1D* hRatio = (TH1D*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  TH1D* hdnm_rb = (TH1D*) hdnm->Clone("htmp");
  if(rebin > 1) {
    hRatio->Rebin(rebin);
    hdnm_rb->Rebin(rebin);
  }
  hRatio->Divide(hdnm_rb);
  delete hdnm_rb;

  //set bin error to 0 for <= bins to be ignored in the fit
  for(int jbin = 1; jbin <= hRatio->GetNbinsX(); ++jbin) {if(hRatio->GetBinContent(jbin) <= 0.) hRatio->SetBinError(jbin,0);}
  TString fit_option = (verbose_ > 1) ? "" : "q";
  if(!drawFit_ || !doFit) {
    fit_option += " 0";
    hRatio->Draw("E1");
  }
  TF1* f = 0;
  if(doFit) {
    TString fit_func = "pol1(0) + gaus(2)";
    f = new TF1("fit_func", fit_func.Data());
    f->SetParameters(1.7, -0.1, 0.25, 3.2, 0.4);
    hRatio->Fit(f, fit_option.Data());
  }
  hRatio->SetLineWidth(2);
  hRatio->SetLineColor(kBlue);
  hRatio->SetMarkerStyle(20);
  hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  hRatio->GetYaxis()->SetRangeUser(0.01, 5.);
  hRatio->SetYTitle("OS/SS");
  f1 = f;
  if(f1 && drawFit_ && doFit) {
    hRatio->Draw("E1");
    TH1D* herr_2s = (TH1D*) hRatio->Clone(Form("%s_err_2s", hRatio->GetName()));
    herr_2s->Reset();
    herr_2s->SetMarkerSize(0.);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_2s, 0.95);
    herr_2s->SetFillColor(kYellow);
    herr_2s->SetFillStyle(1001);
    herr_2s->Draw("E3 same");
    TH1D* herr_1s = (TH1D*) hRatio->Clone(Form("%s_err_1s", hRatio->GetName()));
    herr_1s->Reset();
    herr_1s->SetMarkerSize(0.);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_1s, 0.68);
    herr_1s->SetFillColor(kGreen);
    herr_1s->SetFillStyle(1001);
    herr_1s->Draw("E3 same");
    hRatio->Draw("E1 same"); //add again on top of confidence intervals
    f1->Draw("same");
    if(print) {
      herr_1s->SetName("fit_1s_err");
      herr_1s->Write();
    }
  } else {
    gStyle->SetOptFit(0);
  }
  if(!f1 && doFit) cout << "Error! Fit function not found!\n";
  return c;
}

//make 2D ratio plots
TCanvas* make_2D_ratio_canvas(TH2D* hnum, TH2D* hdnm,
                              double xmin = 0., double xmax = 2.5,
                              double ymin = 0., double ymax = 5.) {
  TCanvas* c = new TCanvas("c_2D_ratio", "c_2D_ratio", 800, 600);
  TVirtualPad* pad;
  TH2D* hRatio = (TH2D*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  hRatio->Divide(hdnm);

  hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  hRatio->GetYaxis()->SetRangeUser(ymin, ymax);
  hRatio->SetTitle("OS/SS");
  hRatio->Draw("colz");
  hRatio->GetZaxis()->SetRangeUser(0.01, 3.);
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
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = verbose_;

  typedef DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal
  CrossSections xs; //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(verbose_ > 0) cout << "--- Dataplotter luminosity for " << year << " = " << dataplotter_->lum_ << endl;
  if(year == 2017) {
    cards.push_back(dcard(path+base+"DY50-ext-1.hist"       , "DY50-ext-1"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year));
    cards.push_back(dcard(path+base+"DY50-ext-2.hist"       , "DY50-ext-2"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year));
  } else {
    cards.push_back(dcard(path+base+"DY50-amc-1.hist"       , "DY50-amc-1"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year));
    cards.push_back(dcard(path+base+"DY50-amc-2.hist"       , "DY50-amc-2"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year));
  }
  cards.push_back(dcard(path+base+"SingleAntiToptW.hist"    , "SingleAntiToptW"    , "Top"         , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year));
  cards.push_back(dcard(path+base+"SingleToptW.hist"        , "SingleToptW"        , "Top"         , false, xs.GetCrossSection("SingleToptW"        ), false, year));
  cards.push_back(dcard(path+base+"WWW.hist"                , "WWW"                , "WJets"         , false, xs.GetCrossSection("WWW"                ), false, year));
  cards.push_back(dcard(path+base+"WZ.hist"                 , "WZ"                 , "WJets"         , false, xs.GetCrossSection("WZ"                 ), false, year));
  cards.push_back(dcard(path+base+"ZZ.hist"                 , "ZZ"                 , "WJets"         , false, xs.GetCrossSection("ZZ"                 ), false, year));
  cards.push_back(dcard(path+base+"WW.hist"                 , "WW"                 , "WJets"                , false, xs.GetCrossSection("WW"                 ), false, year));
  double wxs = xs.GetCrossSection("Wlnu");
  double ngen1 = (year == 2018) ? 1. : xs.GetGenNumber("Wlnu", year);
  double ngen2 = (year == 2018) ? 0. : xs.GetGenNumber("Wlnu-ext", year);
  if(splitWJ_) {
    if(year != 2018) {
      cards.push_back(dcard(path+base+"Wlnu-0.hist"         , "Wlnu"               , "WJets"          , false, wxs*(ngen1)/(ngen1+ngen2)                , false, year));
      cards.push_back(dcard(path+base+"Wlnu-ext-0.hist"     , "Wlnu"               , "WJets"          , false, wxs*(ngen2)/(ngen1+ngen2)                , false, year));
    } else {
      cards.push_back(dcard(path+base+"Wlnu-0.hist"         , "Wlnu"               , "WJets"          , false, wxs                                      , false, year));
    }
    cards.push_back(dcard(path+base+"Wlnu-1J.hist"          , "Wlnu-1J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-1J"            ), false, year));
    cards.push_back(dcard(path+base+"Wlnu-2J.hist"          , "Wlnu-2J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-2J"            ), false, year));
    cards.push_back(dcard(path+base+"Wlnu-3J.hist"          , "Wlnu-3J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-3J"            ), false, year));
    if(year != 2017) {
      cards.push_back(dcard(path+base+"Wlnu-4J.hist"        , "Wlnu-4J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-4J"            ), false, year));
    } else {
      cards.push_back(dcard(path+base+"Wlnu-4.hist"         , "Wlnu"               , "WJets"          , false, wxs*(ngen1)/(ngen1+ngen2)                , false, year));
      cards.push_back(dcard(path+base+"Wlnu-ext-4.hist"     , "Wlnu"               , "WJets"          , false, wxs*(ngen2)/(ngen1+ngen2)                , false, year));
    }
  } else {
    if(year != 2018) {
      cards.push_back(dcard(path+base+"Wlnu.hist"             , "Wlnu"               , "WJets" , false, wxs*(ngen1)/(ngen1+ngen2)                   , false, year));
      cards.push_back(dcard(path+base+"Wlnu-ext.hist"         , "Wlnu"               , "WJets" , false, wxs*(ngen2)/(ngen1+ngen2)                   , false, year));
    } else {
      cards.push_back(dcard(path+base+"Wlnu.hist"             , "Wlnu"               , "WJets" , false, wxs                                         , false, year));
    }
  }
  cards.push_back(dcard(path+base+"ttbarToSemiLeptonic.hist", "ttbarToSemiLeptonic", "Top"          , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year));
  cards.push_back(dcard(path+base+"ttbarlnu.hist"           , "ttbarlnu"           , "Top"          , false, xs.GetCrossSection("ttbarlnu"           ), false, year));
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
  int setAbs = set + CLFVHistMaker::fMisIDOffset;
  if(selection == "mutau")      setAbs += CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) setAbs += CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) setAbs += CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) setAbs += CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) setAbs += CLFVHistMaker::kEE   ;
  //construct the general name of each file, not including the sample name
  TString baseName = "clfv_" + selection + "_clfv_";
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
  hData[0] = get_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset, 1); //SS
  hMC[0]   = get_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset, 0); //SS
  hData[1] = get_histogram("lepdeltar1", setAbs                             , 1); //OS
  hMC[1]   = get_histogram("lepdeltar1", setAbs                             , 0); //OS

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
    hQCD[iqcd] = (TH1D*) hData[iqcd]->Clone(Form("hQCD_%i", iqcd));
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

  const char* fname = Form("rootfiles/qcd_scale_%s_%i.root", selection.Data(), year);
  TFile* fOut = new TFile(fname, "RECREATE");

  TF1* f;
  TCanvas* c1 = make_canvas(hData[0], hMC[0], hQCD[0], "SS");
  TCanvas* c2 = make_canvas(hData[1], hMC[1], hQCD[1], "OS");
  TCanvas* c3 = make_ratio_canvas(hQCD[1], hQCD[0], f, true);

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
  // c3->SetLogy();
  // c3->Print((name+"_scale_log.png").Data());

  TH1D* hQCDScale = (TH1D*) hQCD[1]->Clone("hRatio");
  hQCDScale->Divide(hQCD[0]);
  for(int ibin = 1; ibin < hQCDScale->GetNbinsX(); ++ibin) {
    if(hQCDScale->GetBinContent(ibin) < 0.) hQCDScale->SetBinContent(ibin, 0.);
    if(hQCDScale->GetBinContent(ibin) > 5.) hQCDScale->SetBinContent(ibin, 5.);
  }
  hQCDScale->Write();
  if(f) {
    f->SetName("fRatio");
    f->Write();
  }

  TCanvas* c;
  TH1D *hOS, *hSS, *hRatio;
  hOS = get_qcd_histogram("lepdeltaphi1", setAbs);
  hSS = get_qcd_histogram("lepdeltaphi1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, false, false, 0, 3.2);
  if(c) c->Print((name + "_lepdeltaphi1_ratio.png").Data());
  if(hOS && hSS) {
    hRatio = (TH1D*) hOS->Clone("hRatio_lepdeltaphi1");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("lepdeltaphi", setAbs);
  hSS = get_qcd_histogram("lepdeltaphi", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, false, false, 0, 3.2);
  if(c) c->Print((name + "_lepdeltaphi_ratio.png").Data());
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1D*) hOS->Clone("hClosure_lepdeltaphi");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("oneeta", setAbs, "lep");
  hSS = get_qcd_histogram("oneeta", setAbs+CLFVHistMaker::fQcdOffset, "lep");
  c = make_ratio_canvas(hOS, hSS, f, false, false, -2.5, 2.5);
  if(c) c->Print((name + "_oneeta_ratio.png").Data());
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1D*) hOS->Clone("hClosure_oneeta");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("lepdeltar2", setAbs);
  hSS = get_qcd_histogram("lepdeltar2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, false, false);
  if(c) c->Print((name + "_lepdeltar2_ratio.png").Data());
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1D*) hOS->Clone("hClosure_lepdeltar");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  TH2D *h2DOS, *h2DSS, *h2DRatio;
  h2DOS = get_2D_qcd_histogram("lepdelrvsoneeta1", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelrvsoneeta1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 5.);
  if(c) c->Print((name + "_lepdelrvsoneeta1_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2D*) h2DOS->Clone("hRatio_lepdelrvsoneeta");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("lepdelrvsoneeta", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelrvsoneeta", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 5.);
  if(c) c->Print((name + "_lepdelrvsoneeta_ratio.png").Data());

  h2DOS = get_2D_qcd_histogram("lepdelphivsoneeta1", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelphivsoneeta1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 3.2);
  if(c) c->Print((name + "_lepdelphivsoneeta1_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2D*) h2DOS->Clone("hRatio_lepdelphivsoneeta");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("lepdelphivsoneeta", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelphivsoneeta", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 3.2);
  if(c) c->Print((name + "_lepdelphivsoneeta_ratio.png").Data());

  fOut->Close();
  delete fOut;
  return 0;
}
