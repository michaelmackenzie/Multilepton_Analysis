//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int usePtRegion_ = 0; //regions: 0 = all, 1 = pT > 45, 2 = pT > 60, 3 = pT < 45, 4 = pT < 60
int drawFit_ = 1; //whether to draw the linear fits
int splitWJ_ = 1; //whether to use jet binned w+jets samples or not
TString process_ = ""; //Process of interest, where all others should be subtracted
bool usingMCTaus_ = false; //whether or not the histogram set uses MC taus

//Get the 2D jet pT vs eta histogram
TH2D* get_histogram(int setAbs, int ijet, int idm, int isdata, int icat) {
  TH2D* h = 0;
  TString name = (isdata < 0) ? "faketaumc" : "faketau";
  name += (usePtRegion_ == 0) ? Form("%ijet%idm_%i", ijet, idm, icat) : Form("%ijet%idm_%i_%i", ijet, idm, icat, usePtRegion_);

  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(process_ == "") {
      if(dataplotter_->isData_[d] != (isdata > 0)) continue;
    } else if(isdata <= 0) { //MC taus
      //check if this is the correct process
      if(dataplotter_->labels_[d] != process_) continue;
    } else { //data, with other processes subtracted
      if(dataplotter_->labels_[d] == process_) continue; //skip correct process
    }
    TString hpath = Form("event_%i/%s", setAbs, name.Data());
    if(verbose_ > 1) cout << "Retrieving histogram " << hpath.Data() << " for " << dataplotter_->names_[d].Data()
                          << " with scale = " << dataplotter_->scale_[d] << endl;
    TH2D* hTmp = (TH2D*) dataplotter_->data_[d]->Get(hpath.Data());
    if(!hTmp) {
      if(verbose_ >= 0) cout << "Histogram " << name.Data() << " for " << dataplotter_->names_[d].Data() << " not found!\n";
      continue;
    }
    hTmp = (TH2D*) hTmp->Clone(Form("hTmp_%s", name.Data()));
    if(!dataplotter_->isData_[d]) hTmp->Scale(dataplotter_->scale_[d]);

    //subtract other MC processes from data
    if(isdata > 0 && !dataplotter_->isData_[d]) hTmp->Scale(-1.);

    if(!h) h = hTmp;
    else h->Add(hTmp);
    if(verbose_ > 1) cout << "Histogram " << hpath.Data() << " has integral " << hTmp->Integral() << endl;
  }

  if(!h) return NULL;
  if(verbose_ > 1) cout << "--- Histogram " << name.Data() << " set " << setAbs << " integral = " << h->Integral() << endl;
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
TCanvas* make_eta_region_canvas(TH2D* hnum, TH2D* hdnm, TString name, bool iseff,
                                TF1 *&f1, TF1 *&f2, bool print_unc = false, TString tag = "") {
  int nbins = hnum->GetNbinsY();
  TH1D* hetas[nbins];
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 500*nbins, 450);
  TVirtualPad* pad;
  double cl = 0.68; //confidence level
  c->Divide(nbins, 1);
  TString decay_mode = "Decay Mode ";
  TString dm_n = hdnm->GetName();
  if     (dm_n.Contains("_0dm")) decay_mode += "0";
  else if(dm_n.Contains("_1dm")) decay_mode += "1";
  else if(dm_n.Contains("_2dm")) decay_mode += "10";
  else if(dm_n.Contains("_3dm")) decay_mode += "11";
  for(int ibin = 0; ibin < nbins; ++ibin) {
    hetas[ibin] = hnum->ProjectionX(Form("%s_eta%i", hnum->GetName(), ibin), ibin+1, ibin+1);
    hetas[ibin]->Divide(hdnm->ProjectionX(Form("%s_tmp_eta%i", hdnm->GetName(), ibin), ibin+1, ibin+1));
    pad = c->cd(ibin+1);
    //set bin error to 0 for <= bins to be ignored in the fit
    for(int jbin = 1; jbin <= hetas[ibin]->GetNbinsX(); ++jbin) {
      if(hetas[ibin]->GetBinContent(jbin) <= 0.) {
        hetas[ibin]->SetBinContent(jbin,1e-6);
        hetas[ibin]->SetBinError  (jbin,0);
      } else if(hetas[ibin]->GetBinContent(jbin) >= 0.9) { //also force a maximum efficiency
        hetas[ibin]->SetBinContent(jbin,0.9);
        hetas[ibin]->SetBinError  (jbin,0.9);
      }
    }
    TString fit_option = (verbose_ > 0) ? "S" : "q";
    if(!drawFit_) {
      fit_option += " 0";
      hetas[ibin]->Draw("E1");
    }
    TF1* func;
    int mode = 4; //0: pol1 1: atan 2: pol1 + landau 3: pol1 + gaussian
    if(mode == 0) {
      func = new TF1("func", "[slope]*x + [intercept]", 0.,  999.);
      func->SetParameters(0.1, 0.1);
      func->SetParLimits(0, 0., 1.);
      func->SetParLimits(1, -10., 10.);
    } else if(mode == 1) {
      func = new TF1("func", "[scale]*(std::atan([slope]*(x + [xoffset]))/3.14159265 + 0.5) + (1 - [scale])*[yoffset]", 0.,  999.);
      func->SetParameters(0.1, 0.01, 0., 0.);
      // func->SetParLimits(func->GetParNumber("scale")  , 0., 1.); //0
      // func->SetParLimits(func->GetParNumber("slope")  , -1e5, 1e5); //1
      // func->SetParLimits(func->GetParNumber("xoffset"), -1e5, 1e5); //2
      // func->SetParLimits(func->GetParNumber("yoffset"), 0., 1.); //3
    } else if(mode == 2) {
      func = new TF1("func", "[offset] + [slope]*x + [scale]*TMath::Landau(x, [MPV], [Sigma])", 0., 999.);
      func->SetParameters(50., 10., 0.1, 1., 0.1);
      func->SetParLimits(func->GetParNumber("MPV"), -1.e4, 1.e4);    //0
      func->SetParLimits(func->GetParNumber("Sigma"), 1e-6, 1.e5);   //1
      func->SetParLimits(func->GetParNumber("offset"), -1.e3, 1.e3);   //2
      func->SetParLimits(func->GetParNumber("scale"), -1.e7, 1.e7); //3
      func->SetParLimits(func->GetParNumber("slope"), -10., 10.);    //4
    } else if(mode == 3) {
      func = new TF1("func", "[offset] + [slope]*x + [scale]*TMath::Gaus(x, [MPV], [Sigma])", 0., 999.);
      func->SetParameters(50., 10., 0.1, 1., 0.1);
      func->SetParLimits(func->GetParNumber("MPV"), -1.e4, 1.e4);    //0
      func->SetParLimits(func->GetParNumber("Sigma"), 1e-6, 1.e5);   //1
      func->SetParLimits(func->GetParNumber("offset"), -1.e3, 1.e3);   //2
      func->SetParLimits(func->GetParNumber("scale"), -1.e7, 1.e7); //3
      func->SetParLimits(func->GetParNumber("slope"), -10., 10.);    //4
    } else if(mode == 4) {
      func = new TF1("func", "[offset] + [slope] / (x + [xoffset])", 19., 999.);
      func->SetParameters(0.1, -5., 10.);
      // func->SetParLimits(func->GetParNumber("offset"), -1e3, 1e3);   //0
      // func->SetParLimits(func->GetParNumber("slope"), -1.e3, 1.e3);  //1
      func->SetParLimits(func->GetParNumber("xoffset"), -19., 1e4);  //2
    }
    auto fit_res = hetas[ibin]->Fit(func, fit_option.Data());
    if(!fit_res || fit_res == -1) {
      cout << "Fit result not returned!\n";
    } else if(fit_res == 4) {
      cout << "Fit failed! Repeating with shifted values...\n";
      if(mode == 4) func->SetParameters(0.1, 1., 2.);
      fit_res = hetas[ibin]->Fit(func, fit_option.Data());
    } else if(verbose_ > 0) {
      try {
        cout << "Fit address: " << fit_res << endl;
        fit_res->Print();
      } catch(exception e) {
        cout << "Printing the fit result failed! " << e.what() << endl;
      }
    }
    hetas[ibin]->SetLineWidth(2);
    hetas[ibin]->SetMarkerStyle(6);
    TString eta_region = Form("%.1f #leq |#eta| < %.1f, %s", hnum->GetYaxis()->GetBinLowEdge(ibin+1),
                              hnum->GetYaxis()->GetBinLowEdge(ibin+1)+hnum->GetYaxis()->GetBinWidth(ibin+1),
                              decay_mode.Data());
    hetas[ibin]->SetTitle(eta_region.Data());
    hetas[ibin]->SetYTitle("f_{#tau}");
    if(iseff) hetas[ibin]->GetYaxis()->SetRangeUser(0., 1.);
    pad->SetLogx(0);
    pad->Update();

    TF1* f = func;
    if(drawFit_ && f) {
      hetas[ibin]->Draw("E1");
      TH1D* herr_2s = (TH1D*) hetas[ibin]->Clone(Form("%s_err_2s", hetas[ibin]->GetName()));
      herr_2s->Reset();
      (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_2s, 0.95);
      herr_2s->SetFillColor(kYellow);
      // herr_2s->SetFillStyle(3001);
      herr_2s->Draw("E3 same");
      TH1D* herr_1s = (TH1D*) hetas[ibin]->Clone(Form("%s_err_1s", hetas[ibin]->GetName()));
      herr_1s->Reset();
      (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_1s, 0.68);
      herr_1s->SetFillColor(kGreen);
      // herr_1s->SetFillStyle(3001);
      herr_1s->Draw("E3 same");
      hetas[ibin]->Draw("E1 same"); //add again on top of confidence intervals
      f->Draw("same");
      if(print_unc) {
        herr_1s->SetName(Form("fit_1s_error_%s_%ieta", tag.Data(), ibin));
        herr_1s->Write();
      }
    } else {
      if(!drawFit_) gStyle->SetOptFit(0);
      if(!f) cout << "!!! " << __func__ << ": Fit function not found! Eta region " << eta_region.Data() << endl;
    }
    if(ibin == 0) f1 = f;
    else f2 = f;
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
  dataplotter_->verbose_ = max(0, verbose_ - 1);

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
Int_t scale_factors(TString selection = "mumu", TString process = "", int set1 = 8, int set2 = 8, int year = 2016,
                    TString path = "nanoaods_dev") {

  ///////////////////////
  // Initialize params //
  ///////////////////////
  if(process != "") {
    cout << "Warning! Setting process tag to null as not yet implemented!\n";
    process = "";
  }
  process_ = process;
  usingMCTaus_ = (set1 % 100) > 34 && (set1 % 100) < 40;

  TVirtualFitter::SetMaxIterations( 1e6 );
  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path + "/";

  //get the absolute value of the set, offsetting by the selection
  int set1Abs = set1;
  if(selection == "mutau")      set1Abs += ZTauTauHistMaker::kMuTau;
  else if(selection == "etau" ) set1Abs += ZTauTauHistMaker::kETau ;
  else if(selection == "emu"  ) set1Abs += ZTauTauHistMaker::kEMu  ;
  else if(selection == "mumu" ) set1Abs += ZTauTauHistMaker::kMuMu ;
  else if(selection == "ee"   ) set1Abs += ZTauTauHistMaker::kEE   ;
  int set2Abs = set2 + (set1Abs - set1);
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
  const int ncat = 3; //N(categories), 0: all, 1: loose, 2: tight
  TH2D* hMCFakeTauRates[ncat][njetmax+1][ndm]; //MC true fake taus
  TH2D* hMCTrueTauRates[ncat][njetmax+1][ndm]; //MC true genuine taus
  TH2D* hDataFakeTauRates[ncat][njetmax+1][ndm]; //Data taus
  TH2D* hMCFakeTauRates_j[ncat][ndm]; //MC true fake taus, no jet binning
  TH2D* hMCTrueTauRates_j[ncat][ndm]; //MC true genuine taus, no jet binning
  TH2D* hDataFakeTauRates_j[ncat][ndm]; //Data taus, no jet binning
  TH2D* hMCFakeTauEffs_j[ndm]; //MC true fake taus, tight / (loose + tight)
  TH2D* hDataFakeTauEffs_j[ndm]; //Data taus, tight / (loose + tight)
  for(int idm = 0; idm < ndm; ++idm) {
    for(int icat = 0; icat < ncat; ++icat) {
      hMCFakeTauRates_j[icat][idm] = 0;
      hMCTrueTauRates_j[icat][idm] = 0;
      hDataFakeTauRates_j[icat][idm] = 0;
      for(int ijet = 0; ijet <= njetmax; ++ijet) {
        hMCFakeTauRates[icat][ijet][idm] = get_histogram(set1Abs, ijet, idm, -1, icat); //get fake tau MC
        hMCTrueTauRates[icat][ijet][idm] = get_histogram(set1Abs, ijet, idm,  0, icat); //get true tau MC
        if(!hMCFakeTauRates[icat][ijet][idm] || !hMCTrueTauRates[icat][ijet][idm]) {
          cout << "MC Fake or True Histogram for ijet = " << ijet << " idm = " << idm
               << " and icat = " << icat << " not found!\n";
          return 2;
        }
        //ensure weight sum objects are created
        if(!hMCFakeTauRates[icat][ijet][idm]->GetSumw2N()) hMCFakeTauRates[icat][ijet][idm]->Sumw2();
        if(!hMCTrueTauRates[icat][ijet][idm]->GetSumw2N()) hMCFakeTauRates[icat][ijet][idm]->Sumw2();
        //get the histograms from the second set as well, for selections where this is split into loose and tight histogram sets
        if(set1Abs != set2Abs) {
          hMCFakeTauRates[icat][ijet][idm]->Add(get_histogram(set2Abs, ijet, idm, -1, icat));
          hMCTrueTauRates[icat][ijet][idm]->Add(get_histogram(set2Abs, ijet, idm,  0, icat));
        }
        //Get data histogram
        hDataFakeTauRates[icat][ijet][idm] = get_histogram(set1Abs, ijet, idm, true, icat);
        if(!hDataFakeTauRates[icat][ijet][idm]) {
          cout << "Data Histogram for ijet = " << ijet << " idm = " << idm
               << " and icat = " << icat << " not found!\n";
          return 3;
        }
        //ensure weight sum objects are created
        if(!hDataFakeTauRates[icat][ijet][idm]->GetSumw2N()) hDataFakeTauRates[icat][ijet][idm]->Sumw2();
        if(set1Abs != set2Abs) {
          hDataFakeTauRates[icat][ijet][idm]->Add(get_histogram(set2Abs, ijet, idm, true, icat));
        }
        //add to the non-jet binned hists
        if(!(hMCFakeTauRates_j[icat][idm])) hMCFakeTauRates_j[icat][idm] = (TH2D*) hMCFakeTauRates[icat][ijet][idm]->Clone(Form("hmc_fake_%icat_%idm", icat, idm));
        else hMCFakeTauRates_j[icat][idm]->Add(hMCFakeTauRates[icat][ijet][idm]);
        if(!(hMCTrueTauRates_j[icat][idm])) hMCTrueTauRates_j[icat][idm] = (TH2D*) hMCTrueTauRates[icat][ijet][idm]->Clone(Form("hmc_true_%icat_%idm", icat, idm));
        else hMCTrueTauRates_j[icat][idm]->Add(hMCTrueTauRates[icat][ijet][idm]);
        if(!(hDataFakeTauRates_j[icat][idm])) hDataFakeTauRates_j[icat][idm] = (TH2D*) hDataFakeTauRates[icat][ijet][idm]->Clone(Form("hdata_%icat_%idm", icat, idm));
        else hDataFakeTauRates_j[icat][idm]->Add(hDataFakeTauRates[icat][ijet][idm]);

        //subtract MC expectation of true taus from data
        hDataFakeTauRates[icat][ijet][idm]->Add(hMCTrueTauRates[icat][ijet][idm], -1);
      }
      hMCFakeTauRates_j[icat][idm]->SetTitle(hMCFakeTauRates_j[icat][idm]->GetName());
      hMCTrueTauRates_j[icat][idm]->SetTitle(hMCTrueTauRates_j[icat][idm]->GetName());
      hDataFakeTauRates_j[icat][idm]->SetTitle(hDataFakeTauRates_j[icat][idm]->GetName());
        //subtract MC expectation of true taus from data
      hDataFakeTauRates_j[icat][idm]->Add(hMCTrueTauRates_j[icat][idm], -1.);
    }
    //calculate eff of tight / (anti-iso + tight)
    hMCFakeTauEffs_j[idm] = (TH2D*) hMCFakeTauRates_j[1][idm]->Clone(Form("hmc_eff_%idm", idm));
    hMCFakeTauEffs_j[idm]->Divide(hMCFakeTauRates_j[0][idm]);
    hDataFakeTauEffs_j[idm] = (TH2D*) hDataFakeTauRates_j[1][idm]->Clone(Form("hdata_eff_%idm", idm));
    hDataFakeTauEffs_j[idm]->Divide(hDataFakeTauRates_j[0][idm]);
    hMCFakeTauEffs_j[idm]->SetTitle(hMCFakeTauEffs_j[idm]->GetName());
    hDataFakeTauEffs_j[idm]->SetTitle(hDataFakeTauEffs_j[idm]->GetName());
  }

  cout << "--- MC fake rates:\n";
  double nloosemc(0.), ntightmc(0.);
  for(int idm = 0; idm < ndm; ++idm) {
    cout << " DM = " << (idm + (idm > 1) * 8)
         << " N(loose) = " << hMCFakeTauRates_j[2][idm]->Integral()
         << " N(tight) = " << hMCFakeTauRates_j[1][idm]->Integral()
         << endl;
    nloosemc += hMCFakeTauRates_j[2][idm]->Integral();
    ntightmc += hMCFakeTauRates_j[1][idm]->Integral();
  }
  cout << " Total N(loose) = " << nloosemc << " N(tight) = " << ntightmc << endl;
  cout << "--- Data rates:\n";
  double nloosedata(0.), ntightdata(0.);
  for(int idm = 0; idm < ndm; ++idm) {
    cout << " DM = " << (idm + (idm > 1) * 8)
         << " N(loose) = " << hDataFakeTauRates_j[2][idm]->Integral()
         << " N(tight) = " << hDataFakeTauRates_j[1][idm]->Integral()
         << endl;
    nloosedata += hDataFakeTauRates_j[2][idm]->Integral();
    ntightdata += hDataFakeTauRates_j[1][idm]->Integral();
  }
  cout << " Total N(loose) = " << nloosedata << " N(tight) = " << ntightdata << endl;

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
  TCanvas* c8  = make_canvas(hMCFakeTauRates_j[0], "c_all_mc_fake");
  TCanvas* c8b = make_canvas(hMCTrueTauRates_j[0], "c_all_mc_true");
  TCanvas* c9  = make_canvas(hDataFakeTauRates_j[1], "c_tight_data");
  TCanvas* c10 = make_canvas(hMCFakeTauRates_j[1], "c_tight_mc_fake");
  TCanvas* c10b= make_canvas(hMCTrueTauRates_j[1], "c_tight_mc_true");
  TCanvas* c11 = make_canvas(hDataFakeTauRates_j[2], "c_loose_data");
  TCanvas* c12 = make_canvas(hMCFakeTauRates_j[2], "c_loose_mc_fake");
  TCanvas* c12b= make_canvas(hMCTrueTauRates_j[2], "c_loose_mc_true");
  TCanvas* c13 = make_canvas(hDataFakeTauEffs_j, "c_eff_data", true);
  TCanvas* c14 = make_canvas(hMCFakeTauEffs_j, "c_eff_mc", true);


  //////////////////////
  //  Print results   //
  //////////////////////

  //construct general figure name
  TString name = "figures/fake_tau_";
  name += selection + "_";
  name += year;
  name += "_";
  name += set1;
  name += "/";
  //ensure directories exist
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", name.Data(), name.Data()));
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  if(usePtRegion_ > 0) {
    name = Form("%s/ptregion_%i/fake_tau_", name.Data(), usePtRegion_);
    gSystem->Exec(Form("[ ! -d figures/ptregion_%i ] && mkdir figures/ptregion_%i", usePtRegion_, usePtRegion_));
  }

  //print canvases
  c7  ->Print((name+"data_tot_rate.png").Data());
  c8  ->Print((name+"mc_fake_tot_rate.png").Data());
  c8b ->Print((name+"mc_true_tot_rate.png").Data());
  c9  ->Print((name+"data_tight_rate.png").Data());
  c10 ->Print((name+"mc_fake_tight_rate.png").Data());
  c10b->Print((name+"mc_true_tight_rate.png").Data());
  c11 ->Print((name+"data_loose_rate.png").Data());
  c12 ->Print((name+"mc_fake_loose_rate.png").Data());
  c12b->Print((name+"mc_true_loose_rate.png").Data());
  c13 ->Print((name+"data_eff.png").Data());
  c14 ->Print((name+"mc_fake_eff.png").Data());

  const char* fname = ((usePtRegion_ == 0) ? Form("rootfiles/jet_to_tau_%s_%i_%i.root", selection.Data(), set1, year) :
                       Form("rootfiles/jet_to_tau_%s_%i_ptregion_%i_%i.root", selection.Data(), set1, usePtRegion_, year));
  TFile* fOut = new TFile(fname, "RECREATE");

  TF1 *funcs[8], *mcfuncs[8];
  for(int idm = 0; idm < ndm; ++idm) {
    //Make data factors
    TCanvas* c = make_eta_region_canvas(hDataFakeTauRates_j[1][idm], hDataFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_data", idm), true,
                                        funcs[2*idm], funcs[2*idm+1], true, Form("%idm", idm));
    if(!funcs[2*idm] || !funcs[2*idm+1]) cout << "Error gettting fit functions for DM = " << idm << endl;
    else {
      funcs[2*idm]  ->SetName(Form("fit_func_%idm_0eta", idm));
      funcs[2*idm+1]->SetName(Form("fit_func_%idm_1eta", idm));
    }
    c->Print(Form("%sdata_dm%i_eff.png", name.Data(), idm));
    c->cd(1)->SetLogx(1); c->cd(2)->SetLogx(1); c->Update();
    c->Print(Form("%sdata_dm%i_eff_log.png", name.Data(), idm));

    //Make MC fake estimated factors
    c = make_eta_region_canvas(hMCFakeTauRates_j[1][idm], hMCFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_fake_mc", idm), true,
                               mcfuncs[2*idm], mcfuncs[2*idm+1], true, Form("mc_%idm", idm));
    if(!mcfuncs[2*idm] || !mcfuncs[2*idm+1]) cout << "Error gettting fit functions for DM = " << idm << endl;
    else {
      mcfuncs[2*idm]  ->SetName(Form("fit_mc_func_%idm_0eta", idm));
      mcfuncs[2*idm+1]->SetName(Form("fit_mc_func_%idm_1eta", idm));
    }
    c->Print(Form("%sfake_mc_dm%i_eff.png", name.Data(), idm));
    c->cd(1)->SetLogx(1); c->cd(2)->SetLogx(1); c->Update();
    c->Print(Form("%sfake_mc_dm%i_eff_log.png", name.Data(), idm));
  }

  for(int i = 0; i < 4; ++i) {
    hDataFakeTauEffs_j[i]->Write();
    if(hMCFakeTauEffs_j[i]) hMCFakeTauEffs_j[i]->Write();
    funcs[2*i]->Write();
    funcs[2*i+1]->Write();
    if(mcfuncs[2*i]  ) mcfuncs[2*i]  ->Write();
    if(mcfuncs[2*i+1]) mcfuncs[2*i+1]->Write();
  }

  fOut->Close();
  delete fOut;
  return 0;
}

int do_all_ptregions(TString selection = "mumu", TString process = "", int set1 = 7, int set2 = 7, int year = 2016,
                     TString path = "nanoaods_dev/") {
  int status(0);
  for(int ipt = 0; ipt < 5; ++ipt) {
    usePtRegion_ = ipt;
    status += scale_factors(selection, process, set1, set2, year, path);
  }
  return status;
}
