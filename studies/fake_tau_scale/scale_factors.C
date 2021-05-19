//Script to determine the MC efficiency of b-tags
//by generator-level quark information

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int usePtRegion_ = 0; //regions: 0 = all, 1 = pT > 45, 2 = pT > 60, 3 = pT < 45, 4 = pT < 60
int drawFit_ = 0; //whether to draw the linear fits

//Get the 2D jet pT vs eta histogram
TH2D* get_histogram(int setAbs, int ijet, int idm, int isdata, int icat) {
  TH2D* h = 0;
  TString name = (isdata < 0) ? "faketaumc" : "faketau";
  name += (usePtRegion_ == 0) ? Form("%ijet%idm_%i", ijet, idm, icat) : Form("%ijet%idm_%i_%i", ijet, idm, icat, usePtRegion_);

  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(dataplotter_->isData_[d] != (isdata > 0)) continue;
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
TCanvas* make_eta_region_canvas(TH2D* hnum, TH2D* hdnm, TString name, bool iseff,
                                TF1 *&f1, TF1 *&f2) {
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
    for(int jbin = 1; jbin <= hetas[ibin]->GetNbinsX(); ++jbin) {if(hetas[ibin]->GetBinContent(jbin) <= 0.) hetas[ibin]->SetBinError(jbin,0);}
    TString fit_option = (verbose_ > 1) ? "" : "q";
    if(!drawFit_) {
      fit_option += " 0";
      hetas[ibin]->Draw("E1");
    }
    hetas[ibin]->Fit("pol1", fit_option.Data());
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

    TF1* f = hetas[ibin]->GetFunction("pol1");
    if(drawFit_) {
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
    } else {
      gStyle->SetOptFit(0);
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
  dataplotter_->verbose_ = verbose_;

  typedef DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal
  CrossSections xs; //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  // if(year == 2018)
    cards.push_back(dcard(path+base+"DY50.hist"             , "DY50"               , "Drell-Yan"         , false, xs.GetCrossSection("DY50"               ), false, year));
  // else
  //   cards.push_back(dcard(path+base+"DY50-ext.hist"         , "DY50"               , "Drell-Yan"         , false, xs.GetCrossSection("DY50"               ), false, year));
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
Int_t scale_factors(TString selection = "mumu", int set = 7, int year = 2016,
                    TString path = "nanoaods_dev/") {

  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path;

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
  TH2D* hMCTrueTauRates[ncat][njetmax+1][ndm];
  TH2D* hDataFakeTauRates[ncat][njetmax+1][ndm];
  TH2D* hMCFakeTauRates_j[ncat][ndm];
  TH2D* hMCTrueTauRates_j[ncat][ndm];
  TH2D* hDataFakeTauRates_j[ncat][ndm];
  TH2D* hMCFakeTauEffs_j[ndm];
  TH2D* hDataFakeTauEffs_j[ndm];
  for(int idm = 0; idm < ndm; ++idm) {
    for(int icat = 0; icat < ncat; ++icat) {
      hMCFakeTauRates_j[icat][idm] = 0;
      hMCTrueTauRates_j[icat][idm] = 0;
      hDataFakeTauRates_j[icat][idm] = 0;
      for(int ijet = 0; ijet <= njetmax; ++ijet) {
        hMCFakeTauRates[icat][ijet][idm] = get_histogram(setAbs, ijet, idm, -1, icat);
        hMCTrueTauRates[icat][ijet][idm] = get_histogram(setAbs, ijet, idm,  0, icat);
        if(!hMCFakeTauRates[icat][ijet][idm]) {
          cout << "MC Fake Histogram for ijet = " << ijet << " idm = " << idm
               << " and icat = " << icat << " not found!\n";
          return 2;
        }
        if(!hMCTrueTauRates[icat][ijet][idm]) {
          cout << "MC True Histogram for ijet = " << ijet << " idm = " << idm
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
        if(!hMCTrueTauRates[icat][ijet][idm]->GetSumw2N()) hMCFakeTauRates[icat][ijet][idm]->Sumw2();
        if(!hDataFakeTauRates[icat][ijet][idm]->GetSumw2N()) hDataFakeTauRates[icat][ijet][idm]->Sumw2();

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

  //ensure directories exist
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //construct general figure name
  TString name = "figures/fake_tau_";
  if(usePtRegion_ > 0) {
    name = Form("figures/ptregion_%i/fake_tau_", usePtRegion_);
    gSystem->Exec(Form("[ ! -d figures/ptregion_%i ] && mkdir figures/ptregion_%i", usePtRegion_, usePtRegion_));
  }
  name += selection + "_";
  name += year;

  //print canvases
  c7  ->Print((name+"_data_tot_rate.png").Data());
  c8  ->Print((name+"_mc_fake_tot_rate.png").Data());
  c8b ->Print((name+"_mc_true_tot_rate.png").Data());
  c9  ->Print((name+"_data_tight_rate.png").Data());
  c10 ->Print((name+"_mc_fake_tight_rate.png").Data());
  c10b->Print((name+"_mc_true_tight_rate.png").Data());
  c11 ->Print((name+"_data_loose_rate.png").Data());
  c12 ->Print((name+"_mc_fake_loose_rate.png").Data());
  c12b->Print((name+"_mc_true_loose_rate.png").Data());
  c13 ->Print((name+"_data_eff.png").Data());
  c14 ->Print((name+"_mc_fake_eff.png").Data());
  TF1 *funcs[8], *mcfuncs[8];
  for(int idm = 0; idm < ndm; ++idm) {
    //Make data factors
    TCanvas* c = make_eta_region_canvas(hDataFakeTauRates_j[1][idm], hDataFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_data", idm), true,
                                        funcs[2*idm], funcs[2*idm+1]);
    if(!funcs[2*idm] || !funcs[2*idm+1]) cout << "Error gettting fit functions for DM = " << idm << endl;
    else {
      funcs[2*idm]  ->SetName(Form("fit_func_%idm_0eta", idm));
      funcs[2*idm+1]->SetName(Form("fit_func_%idm_1eta", idm));
    }
    c->Print(Form("%s_data_dm%i_eff.png", name.Data(), idm));
    c->cd(1)->SetLogx(1); c->cd(2)->SetLogx(1); c->Update();
    c->Print(Form("%s_data_dm%i_eff_log.png", name.Data(), idm));

    //Make MC fake estimated factors
    c = make_eta_region_canvas(hMCFakeTauRates_j[1][idm], hMCFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_fake_mc", idm), true,
                               mcfuncs[2*idm], mcfuncs[2*idm+1]);
    if(!mcfuncs[2*idm] || !mcfuncs[2*idm+1]) cout << "Error gettting fit functions for DM = " << idm << endl;
    else {
      mcfuncs[2*idm]  ->SetName(Form("fit_mc_func_%idm_0eta", idm));
      mcfuncs[2*idm+1]->SetName(Form("fit_mc_func_%idm_1eta", idm));
    }
    c->Print(Form("%s_fake_mc_dm%i_eff.png", name.Data(), idm));
    c->cd(1)->SetLogx(1); c->cd(2)->SetLogx(1); c->Update();
    c->Print(Form("%s_fake_mc_dm%i_eff_log.png", name.Data(), idm));
  }

  const char* fname = ((usePtRegion_ == 0) ? Form("rootfiles/jet_to_tau_%s_%i.root", selection.Data(), year) :
                       Form("rootfiles/jet_to_tau_%s_ptregion_%i_%i.root", selection.Data(), usePtRegion_, year));
  TFile* fOut = new TFile(fname, "RECREATE");
  for(int i = 0; i < 4; ++i) {
    hDataFakeTauEffs_j[i]->Write();
    funcs[2*i]->Write();
    funcs[2*i+1]->Write();
    mcfuncs[2*i]->Write();
    mcfuncs[2*i+1]->Write();
  }

  fOut->Close();
  delete fOut;
  return 0;
}

int do_all_ptregions(TString selection = "mumu", int set = 7, int year = 2016,
                     TString path = "nanoaods_dev/") {
  int status(0);
  for(int ipt = 0; ipt < 5; ++ipt) {
    usePtRegion_ = ipt;
    status += scale_factors(selection, set, year, path);
  }
  return status;
}
