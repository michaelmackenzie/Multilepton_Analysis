//Script to determine the MC efficiency of b-tags
//by generator-level quark information

using namespace CLFV;
TString selection_;

#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int drawFit_ = 1; //whether to draw the linear fits
int rebin_ = 1;

//Get 1D histograms
TH1* get_histogram(TString name, int setAbs, int isdata, TString type = "event") {
  TH1* h = nullptr;
  if(isdata) {
    h = dataplotter_->get_data(name, type, setAbs);
  } else {
    THStack* stack = dataplotter_->get_stack(name, type, setAbs);
    if(!stack || stack->GetNhists() <= 0) {
      cout << __func__ << ": Background stack " << name.Data() << " not found!\n";
      return nullptr;
    }
    h = (TH1*) stack->GetStack()->Last();
  }

  if(!h) return nullptr;

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
TH2* get_2D_histogram(TString name, int setAbs, int isdata, TString type = "event") {
  TH2* h = 0;
  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    if(dataplotter_->isData_[d] != (isdata > 0)) continue;
    TString hpath = Form("%s_%i/%s", type.Data(), setAbs, name.Data());
    if(verbose_ > 0) cout << "Retrieving histogram " << hpath.Data() << " for " << dataplotter_->names_[d].Data() << endl;
    TH2* hTmp = (TH2*) dataplotter_->data_[d]->Get(hpath.Data());
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
TH1* get_qcd_histogram(TString name, int setAbs, TString type = "event") {
  TH1* hQCD = get_histogram(name, setAbs, 1, type);
  if(!hQCD) {
    cout << "QCD histogram building for " << name << " failed!\n";
    return 0;
  }
  hQCD->Add(get_histogram(name, setAbs, 0, type), -1.);
  hQCD->SetName(Form("QCD_%s_%i", name.Data(), setAbs));

  //Force the QCD distribution to be positive while preserving the integral
  const double norm = hQCD->Integral();
  for(int ibin = 1; ibin <= hQCD->GetNbinsX(); ++ibin) {
    if(hQCD->GetBinContent(ibin) < 0.) {
      std::cout << __func__ << ": Warning! QCD bin " << ibin << " < 0, setting to 0!\n";
      hQCD->SetBinContent(ibin, 0.);
    }
  }
  hQCD->Scale(norm/hQCD->Integral());
  return hQCD;
}

//Get the 2D QCD histogram for a given histogram name and set
TH2* get_2D_qcd_histogram(TString name, int setAbs, TString type = "event") {
  TH2* hQCD = get_2D_histogram(name, setAbs, 1, type);
  if(!hQCD) {
    cout << "QCD histogram building for " << name << " failed!\n";
    return 0;
  }
  hQCD->Add(get_2D_histogram(name, setAbs, 0), -1.);
  hQCD->SetName(Form("QCD_%s_%i", name.Data(), setAbs));

  //Force the QCD distribution to be positive while preserving the integral
  const double norm = hQCD->Integral();
  for(int xbin = 1; xbin <= hQCD->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hQCD->GetNbinsY(); ++ybin) {
      if(hQCD->GetBinContent(xbin,ybin) < 0.) {
        std::cout << __func__ << ": Warning! QCD bin (" << xbin << ", " << ybin << ") < 0, setting to 0!\n";
        hQCD->SetBinContent(xbin, ybin, 0.);
      }
    }
  }
  hQCD->Scale(norm/hQCD->Integral());
  return hQCD;
}

//make 1D ratio plots
TCanvas* make_ratio_canvas(TH1* hnum, TH1* hdnm, TF1 *&f1, bool print = false, bool doFit = true, double xmin = 0., double xmax = 5., int rebin = 1) {
  TCanvas* c = new TCanvas("c_ratio", "c_ratio", 800, 600);
  TVirtualPad* pad;
  TH1* hRatio = (TH1*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  TH1* hdnm_rb = (TH1*) hdnm->Clone("htmp");
  if(rebin > 1) {
    hRatio->Rebin(rebin);
    hdnm_rb->Rebin(rebin);
  }
  hRatio->Divide(hdnm_rb);
  delete hdnm_rb;

  //set bin error to 0 for <= bins to be ignored in the fit
  for(int jbin = 1; jbin <= hRatio->GetNbinsX(); ++jbin) {if(hRatio->GetBinContent(jbin) <= 0.) hRatio->SetBinError(jbin,0);}
  TString fit_option = (verbose_ > 1) ? "S" : "S q";
  if(!drawFit_ || !doFit) {
    fit_option += " 0";
    hRatio->Draw("E1");
  }
  TF1* f = 0;
  if(doFit) {
    TString fit_func = "pol1(0)"; //(years_[0] == 2016) ? "pol1(0) + gaus(2)" : "pol1(0)";
    f = new TF1("fit_func", fit_func.Data());
    f->SetParameters(1.7, -0.1, 0.25, 3.2, 0.4);
    int count = 0;
    bool refit = false;
    do {
      if(refit) cout << __func__ << ": Refitting for " << hnum->GetName() << ", " << hdnm->GetName() << endl;
      auto fitres = hRatio->Fit(f, fit_option.Data());
      refit = !fitres || fitres->Status() != 0;
      refit &=  count < 5;
      ++count;
    } while(refit);
  }
  hRatio->SetLineWidth(2);
  hRatio->SetLineColor(kBlue);
  hRatio->SetMarkerStyle(20);
  hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  // hRatio->GetYaxis()->SetRangeUser(0., 5.);
  hRatio->SetYTitle("OS/SS");
  f1 = f;
  if(f1 && drawFit_ && doFit) {
    hRatio->Draw("E1");
    TH1* herr_2s = (TH1*) hRatio->Clone(Form("%s_err_2s", hRatio->GetName()));
    herr_2s->Reset();
    herr_2s->SetMarkerSize(0.);
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_2s, 0.95);
    herr_2s->SetFillColor(kYellow);
    herr_2s->SetFillStyle(1001);
    herr_2s->Draw("E3 same");
    TH1* herr_1s = (TH1*) hRatio->Clone(Form("%s_err_1s", hRatio->GetName()));
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
TCanvas* make_2D_ratio_canvas(TH2* hnum, TH2* hdnm,
                              double xmin = 0., double xmax = 2.5,
                              double ymin = 0., double ymax = 5.,
                              bool binc = false, TString xl = "", TString yl = "") {
  TCanvas* c = new TCanvas("c_2D_ratio", "c_2D_ratio", 800, 600);
  TVirtualPad* pad;
  TH2* hRatio = (TH2*) hnum->Clone(Form("hRatio_%s", hnum->GetName()));
  hRatio->Divide(hdnm);

  for(int xbin = 1; xbin <= hRatio->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hRatio->GetNbinsY(); ++ybin) {
      if(hRatio->GetBinContent(xbin, ybin) > 3.) {
        cout << __func__ << ": Ratio bin (" << xbin << ", " << ybin << ") is "
             << hRatio->GetBinContent(xbin, ybin) << ", setting to 3\n";
        hRatio->SetBinContent(xbin, ybin, 3.);
      }
    }
  }
  hRatio->GetXaxis()->SetRangeUser(xmin, xmax);
  hRatio->GetYaxis()->SetRangeUser(ymin, ymax);
  hRatio->SetTitle("OS/SS");
  if(binc)
    hRatio->Draw("colz text");
  else
    hRatio->Draw("colz");
  if(hRatio->GetMaximum() > 3.)
    hRatio->GetZaxis()->SetRangeUser(0.01, 3.);
  if(xl != "") hRatio->SetXTitle(xl.Data());
  if(yl != "") hRatio->SetYTitle(yl.Data());
  return c;
}

//make 1D plots
TCanvas* make_canvas(TH1* hData, TH1* hMC, TH1* hQCD, TString name) {
  TCanvas* c = new TCanvas(("c_"+name).Data(), ("c_"+name).Data(), 800, 600);
  TVirtualPad* pad;
  hMC->Draw("hist");
  hMC->SetLineColor(kBlue);
  hMC->SetLineWidth(2);
  hMC->SetFillStyle(3004);
  hMC->SetFillColor(kBlue);
  hData->Draw("E1 same");
  hData->SetMarkerStyle(20);
  hData->SetLineWidth(2);
  hQCD->Draw("hist same");
  hQCD->SetLineColor(kRed);
  hQCD->SetLineWidth(2);
  hQCD->SetFillStyle(3005);
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
Int_t initialize_plotter(TString base) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = verbose_;
  dataplotter_->qcd_scale_ = 1.;
  dataplotter_->embed_scale_ = embedScale_;

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_);
  double lum = 0.;
  for(int year : years_) {
    dataplotter_->lums_[year] = xs.GetLuminosity(year);
    lum += dataplotter_->lums_[year];
  }
  dataplotter_->set_luminosity(lum);

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}


//Generate the plots and scale factors
Int_t scale_factors(TString selection = "emu", int set = 8, int year = 2016,
                    TString hist_dir = "nanoaods_dev") {

  //////////////////////
  // Initialize files //
  //////////////////////
  // hist_path_  = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/";
  hist_dir_   = hist_dir;
  selection_  = selection;
  useEmbed_   = 0; //FIXME: Add back embedding once scale factors are measured
  embedScale_ = 1.;
  years_      = {year};
  useUL_      = (hist_dir_.Contains("_UL")) ? 1 : 0;

  if(!useEmbed_) cout << "WARNING! Not using embedding samples!\n";

  //get the absolute value of the set, offsetting by the selection and using a loose selection
  int setAbs = set + CLFVHistMaker::fMisIDOffset;
  if     (selection == "mutau") setAbs += CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) setAbs += CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) setAbs += CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) setAbs += CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) setAbs += CLFVHistMaker::kEE   ;
  const int setLsETtMu = (setAbs - set + 70); //loose electron, tight muon
  const int setLsELsMu = (setAbs - set + 71); //loose electron, loose muon

  //construct the general name of each file, not including the sample name
  TString baseName = Form("clfv_%s_%i_", selection.Data(), year);

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  //////////////////////
  //  Get histograms  //
  //////////////////////

  TH1* hData[2]; //OS and SS
  TH1* hMC[2]; //OS and SS
  hData[0] = get_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset, 1); //SS
  hMC[0]   = get_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset, 0); //SS
  hData[1] = get_histogram("lepdeltar1", setAbs                          , 1); //OS
  hMC[1]   = get_histogram("lepdeltar1", setAbs                          , 0); //OS

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

  hData[0]->SetName("hData_SS");
  hData[1]->SetName("hData_OS");
  hMC  [0]->SetName("hMC_SS");
  hMC  [1]->SetName("hMC_OS");

  TH1* hQCD[2];
  hQCD[0] = get_qcd_histogram("lepdeltar1", setAbs+CLFVHistMaker::fQcdOffset);
  hQCD[1] = get_qcd_histogram("lepdeltar1", setAbs                          );


  //////////////////////
  // Draw fake rates  //
  //////////////////////
  gStyle->SetOptStat(0);

  //ensure directories exist
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  const char* fname = Form("rootfiles/qcd_scale_%s_%i.root", selection.Data(), year);
  TFile* fOut = new TFile(fname, "RECREATE");

  TF1* f;
  TCanvas* c1 = make_canvas(hData[0], hMC[0], hQCD[0], "SS");
  TCanvas* c2 = make_canvas(hData[1], hMC[1], hQCD[1], "OS");
  TCanvas* c3 = make_ratio_canvas(hQCD[1], hQCD[0], f, true);

  //////////////////////
  //  Print results   //
  //////////////////////

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

  TH1* hQCDScale = (TH1*) hQCD[1]->Clone("hRatio");
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
  TH1 *hOS, *hSS, *hRatio;
  hOS = get_qcd_histogram("lepdeltaphi1", setAbs);
  hSS = get_qcd_histogram("lepdeltaphi1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, false, false, 0, 3.2);
  if(c) c->Print((name + "_lepdeltaphi1_ratio.png").Data());
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltaphi1");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("lepdeltaphi", setAbs);
  hSS = get_qcd_histogram("lepdeltaphi", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, false, false, 0, 3.2);
  if(c) c->Print((name + "_lepdeltaphi_ratio.png").Data());
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1*) hOS->Clone("hClosure_lepdeltaphi");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("oneeta", setAbs, "lep");
  hSS = get_qcd_histogram("oneeta", setAbs+CLFVHistMaker::fQcdOffset, "lep");
  c = make_ratio_canvas(hOS, hSS, f, false, false, -2.5, 2.5);
  if(c) c->Print((name + "_oneeta_ratio.png").Data());
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1*) hOS->Clone("hClosure_oneeta");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  hOS = get_qcd_histogram("lepdeltar2", setAbs);
  hSS = get_qcd_histogram("lepdeltar2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_ratio_canvas(hOS, hSS, f, false, false);
  if(c) c->Print((name + "_lepdeltar2_ratio.png").Data());
  if(hOS && hSS) {
    hSS->Scale(hOS->Integral() / hSS->Integral());
    hRatio = (TH1*) hOS->Clone("hClosure_lepdeltar");
    hRatio->Divide(hSS);
    hRatio->Write();
  }

  //Jet-binned delta-R scale factors
  hOS = get_qcd_histogram("qcddelrj0", setAbs);
  hSS = get_qcd_histogram("qcddelrj0", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_canvas(get_histogram("qcddelrj0", setAbs+CLFVHistMaker::fQcdOffset, 1),
                  get_histogram("qcddelrj0", setAbs+CLFVHistMaker::fQcdOffset, 0), hSS, "SS");
  if(c) {c->SetLogy(); c->Print((name + "_lepdeltarj0_SS.png").Data()); delete c;}
  c = make_canvas(get_histogram("qcddelrj0", setAbs, 1),
                  get_histogram("qcddelrj0", setAbs, 0), hOS, "OS");
  if(c) {c->SetLogy(); c->Print((name + "_lepdeltarj0_OS.png").Data()); delete c;}
  c = make_ratio_canvas(hOS, hSS, f, false, true);
  if(c) c->Print((name + "_lepdeltarj0_ratio.png").Data());
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltarj0");
    hRatio->Divide(hSS);
    hRatio->Write();
    f->SetName("fRatioJ0");
    f->Write();
  }

  hOS = get_qcd_histogram("qcddelrj1", setAbs);
  hSS = get_qcd_histogram("qcddelrj1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_canvas(get_histogram("qcddelrj1", setAbs+CLFVHistMaker::fQcdOffset, 1),
                  get_histogram("qcddelrj1", setAbs+CLFVHistMaker::fQcdOffset, 0), hSS, "SS");
  if(c) {c->SetLogy(); c->Print((name + "_lepdeltarj1_SS.png").Data()); delete c;}
  c = make_canvas(get_histogram("qcddelrj1", setAbs, 1),
                  get_histogram("qcddelrj1", setAbs, 0), hOS, "OS");
  if(c) {c->SetLogy(); c->Print((name + "_lepdeltarj1_OS.png").Data()); delete c;}
  c = make_ratio_canvas(hOS, hSS, f, false, true);
  if(c) c->Print((name + "_lepdeltarj1_ratio.png").Data());
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltarj1");
    hRatio->Divide(hSS);
    hRatio->Write();
    f->SetName("fRatioJ1");
    f->Write();
  }

  hOS = get_qcd_histogram("qcddelrj2", setAbs);
  hSS = get_qcd_histogram("qcddelrj2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_canvas(get_histogram("qcddelrj2", setAbs+CLFVHistMaker::fQcdOffset, 1),
                  get_histogram("qcddelrj2", setAbs+CLFVHistMaker::fQcdOffset, 0), hSS, "SS");
  if(c) {c->SetLogy(); c->Print((name + "_lepdeltarj2_SS.png").Data()); delete c;}
  c = make_canvas(get_histogram("qcddelrj2", setAbs, 1),
                  get_histogram("qcddelrj2", setAbs, 0), hOS, "OS");
  if(c) {c->SetLogy(); c->Print((name + "_lepdeltarj2_OS.png").Data()); delete c;}
  c = make_ratio_canvas(hOS, hSS, f, false, true);
  if(c) c->Print((name + "_lepdeltarj2_ratio.png").Data());
  if(hOS && hSS) {
    hRatio = (TH1*) hOS->Clone("hRatio_lepdeltarj2");
    hRatio->Divide(hSS);
    hRatio->Write();
    f->SetName("fRatioJ2");
    f->Write();
  }

  /////////////////////////////////////
  // 2D ratios

  TH2 *h2DOS, *h2DSS, *h2DRatio;
  h2DOS = get_2D_qcd_histogram("lepdelrvsoneeta1", setAbs);
  h2DSS = get_2D_qcd_histogram("lepdelrvsoneeta1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 0., 2.5, 0., 5.);
  if(c) c->Print((name + "_lepdelrvsoneeta1_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_lepdelrvsoneeta");
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
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_lepdelphivsoneeta");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  //Jet-binned one pt vs two pt non-closure test
  gStyle->SetPaintTextFormat(".2f");
  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptj0", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptj0", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwoptj0_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwoptj0");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptj1", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptj1", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwoptj1_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwoptj1");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptj2", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptj2", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwoptj2_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwoptj2");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwopt", setAbs);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwopt", setAbs+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwopt_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwopt");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }

  //loose electron region 2D scales
  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsETtMu);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsETtMu+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwopt_letm_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwopt_letm");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }
  //Save the ratio histogram to make muon anti-iso --> iso scale factors
  TH2* hIso = (TH2*) h2DRatio->Clone("hIso");

  h2DOS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsELsMu);
  h2DSS = get_2D_qcd_histogram("qcdoneptvstwoptiso", setLsELsMu+CLFVHistMaker::fQcdOffset);
  c = make_2D_ratio_canvas(h2DOS, h2DSS, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwopt_lelm_ratio.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) h2DOS->Clone("hRatio_oneptvstwopt_lelm");
    h2DRatio->Divide(h2DSS);
    h2DRatio->Write();
  }
  TH2* hAntiIso = (TH2*) h2DRatio->Clone("hAntiIso");

  //Make the scale factors for muon anti-iso --> iso
  c = make_2D_ratio_canvas(hIso, hAntiIso, 10., 150, 10., 150, true, "p_{T}^{e} (GeV/c)", "p_{T}^{#mu} (GeV/c)");
  if(c) c->Print((name + "_oneptvstwopt_iso_to_antiiso_scale.png").Data());
  if(h2DOS && h2DSS) {
    h2DRatio = (TH2*) hIso->Clone("hRatio_oneptvstwopt_antiisoscale");
    h2DRatio->Divide(hAntiIso);
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
