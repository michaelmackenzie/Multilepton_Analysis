//script to create plots of scale factors used in the analysis


//////////////////////////////////////////////////
// Plot the scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_1D_slices(TH2* hID, TString cname, bool xaxis = true) {
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  const int nbins = (xaxis) ? hID->GetNbinsX() : hID->GetNbinsY();
  TLegend* leg = new TLegend(0.7, 0.13, 0.9, 0.5);
  const int colors[] = {kRed, kBlue, kGreen, kViolet, kOrange, kYellow+2,
                        kAtlantic, kGreen+2, kRed+2, kBlue+2, kViolet-2};
  TH1* haxis;
  double ymin(1.e9), ymax(-1.e9);
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    TH1* h = (xaxis) ? hID->ProjectionY(Form("_%i", ibin), ibin, ibin) : hID->ProjectionX(Form("_%i", ibin), ibin, ibin);
    h->SetLineColor(colors[ibin-1]);
    h->SetLineWidth(2);
    h->SetMarkerSize(0.8);
    h->SetMarkerStyle(20);
    h->SetMarkerColor(colors[ibin-1]);
    if(ibin == 1) {h->Draw("E"); haxis = h;}
    else h->Draw("E same");
    const double xmin = (xaxis) ? hID->GetXaxis()->GetBinLowEdge(ibin) : hID->GetYaxis()->GetBinLowEdge(ibin);
    const double xmax = (xaxis) ? xmin + hID->GetXaxis()->GetBinWidth(ibin) : xmin + hID->GetYaxis()->GetBinWidth(ibin);
    leg->AddEntry(h, Form("[%.2f, %.2f]", xmin, xmax));
    for(int jbin = 1; jbin <= h->GetNbinsX(); ++jbin) {
      const double binc = h->GetBinContent(jbin);
      ymin = std::min(ymin, binc);
      ymax = std::max(ymax, binc);
    }
  }
  leg->Draw();
  haxis->GetXaxis()->SetTitleSize(0.05);
  haxis->GetYaxis()->SetTitleSize(0.05);
  haxis->GetYaxis()->SetTitleOffset(1.);
  haxis->GetYaxis()->SetRangeUser(0.9*ymin, 1.1*ymax);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.1);
  c->SetRightMargin(0.1);
  return c;
}

//////////////////////////////////////////////////
// Plot the scale factors/errors from functions
//////////////////////////////////////////////////
TCanvas* plot_scale(TF1* fID, TString cname, TF1* fID_up = 0, TF1* fID_down = 0) {
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  fID->SetLineColor(kRed);
  fID->SetLineWidth(2);
  fID->Draw();
  if(fID_up) {
    fID_up->SetLineColor(kGreen);
    fID_up->SetLineStyle(kDashed);
    fID_up->SetLineWidth(2);
    fID_up->Draw("same");
  }
  if(fID_down) {
    fID_down->SetLineColor(kBlue);
    fID_down->SetLineStyle(kDashed);
    fID_down->SetLineWidth(2);
    fID_down->Draw("same");
  }
  TLegend* leg = new TLegend();
  leg->AddEntry(fID, "Scale factor", "L");
  if(fID_up  ) leg->AddEntry(fID_up  , "+#sigma", "L");
  if(fID_down) leg->AddEntry(fID_down, "-#sigma", "L");
  leg->Draw();
  fID->GetYaxis()->SetRangeUser(0., 1.1*fID_up->GetMaximum());

  fID->GetXaxis()->SetTitleSize(0.05);
  fID->GetYaxis()->SetTitleSize(0.05);
  fID->GetYaxis()->SetTitleOffset(1.);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.13);
  c->SetRightMargin(0.13);
  return c;
}

//////////////////////////////////////////////////
// Plot the scale factors/errors from TH1
//////////////////////////////////////////////////
TCanvas* plot_scale(TH1* hID, TString cname) {
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  hID->SetLineColor(kBlue);
  hID->SetLineWidth(2);
  hID->SetMarkerStyle(20);
  hID->SetMarkerSize(0.8);
  hID->Draw("E");

  hID->GetXaxis()->SetTitleSize(0.05);
  hID->GetYaxis()->SetTitleSize(0.05);
  hID->GetYaxis()->SetTitleOffset(1.);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.13);
  c->SetRightMargin(0.13);
  return c;
}

//////////////////////////////////////////////////
// Plot the scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_scale(TH2* hID, TString cname, bool error) {
  if(error) {
    double min_err(1.e5), max_err(-1.);
    for(int xbin = 1; xbin <= hID->GetNbinsX(); ++xbin) {
      for(int ybin = 1; ybin <= hID->GetNbinsY(); ++ybin) {
        double err = hID->GetBinError(xbin, ybin);
        max_err = max(max_err, err);
        min_err = min(min_err, err);
        hID->SetBinContent(xbin, ybin, err);
      }
    }
    hID->GetZaxis()->SetRangeUser(0.95*min_err, 1.05*max_err);
  }
  if(error) cname += "_err";
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  hID->Draw("colz");
  hID->GetXaxis()->SetTitleSize(0.05);
  hID->GetYaxis()->SetTitleSize(0.05);
  if(error) hID->SetTitle(Form("%s errors", hID->GetTitle()));
  hID->GetYaxis()->SetTitleOffset(1.);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.13);
  c->SetRightMargin(0.13);
  return c;
}

//////////////////////////////////////////////////
// Plot the muon trigger scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_muon_low_trigger_scale(int year, int period = 0, bool error = false) {
  TString path = "../../scale_factors/";
  TString hname, xname, yname;
  if(year == 2016) {
    if(period == 0) {
      path += "EfficienciesAndSF_RunBtoF_muon_2016.root";
    } else {
      path += "EfficienciesAndSF_Period4_muonTrigger_2016.root";
    }
    hname = "IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio";
  } else if(year == 2017) {
    path += "EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root";
    hname = "IsoMu27_PtEtaBins/abseta_pt_ratio";
  } else if(year == 2018) {
    path += "EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root";
    hname = "IsoMu24_PtEtaBins/abseta_pt_ratio";
  } else {
    cout << "Unknown year!\n";
    return NULL;
  }
  yname = "p_{T}"; xname = "#eta";
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get(hname.Data());
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  hID->SetTitle("Muon Iso trigger scale");
  hID->SetXTitle(xname.Data());
  hID->SetYTitle(yname.Data());
  hID->SetZTitle("");
  return plot_scale(hID, Form("c_muon_low_trigger_%i", year), error);
}

//////////////////////////////////////////////////
// Plot the muon ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_muon_ID_scale(int year, int period = 0, bool error = false) {
  TString path = "../../scale_factors/";
  TString hname, xname, yname;
  if(year == 2016) {
    if(period == 0) {
      path += "RunBCDEF_SF_ID_muon_2016.root";
    } else {
      path += "RunGH_SF_ID_muon_2016.root";
    }
    hname = "NUM_TightID_DEN_genTracks_eta_pt";
    yname = "p_{T}"; xname = "#eta";
  } else if(year == 2017) {
    path += "2017_Mu_RunBCDEF_SF_ID.root";
    hname = "NUM_TightID_DEN_genTracks_pt_abseta";
    xname = "p_{T}"; yname = "#eta";
  } else if(year == 2018) {
    path += "RunABCD_SF_ID_muon_2018.root";
    hname = "NUM_TightID_DEN_TrackerMuons_pt_abseta";
    xname = "p_{T}"; yname = "#eta";
  } else {
    cout << "Unknown year!\n";
    return NULL;
  }
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get(hname.Data());
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  hID->SetTitle("Muon ID scale");
  hID->SetXTitle(xname.Data());
  hID->SetYTitle(yname.Data());
  return plot_scale(hID, Form("c_muon_id_%i", year), error);
}

//////////////////////////////////////////////////
// Plot the muon Iso scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_muon_iso_scale(int year, int period = 0, bool error = false) {
  TString path = "../../scale_factors/";
  TString hname, xname, yname;
  if(year == 2016) {
    if(period == 0) {
      path += "RunBCDEF_SF_ISO_muon_2016.root";
    } else {
      path += "RunGH_SF_ISO_muon_2016.root";
    }
    hname = "NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt";
    yname = "p_{T}"; xname = "#eta";
  } else if(year == 2017) {
    path += "2017_Mu_RunBCDEF_SF_ISO.root";
    hname = "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta";
    xname = "p_{T}"; yname = "#eta";
  } else if(year == 2018) {
    path += "RunABCD_SF_ISO_muon_2018.root";
    hname = "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta";
    xname = "p_{T}"; yname = "#eta";
  } else {
    cout << "Unknown year!\n";
    return NULL;
  }
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get(hname.Data());
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  hID->SetTitle("Muon Iso scale");
  hID->SetXTitle(xname.Data());
  hID->SetYTitle(yname.Data());
  return plot_scale(hID, Form("c_muon_iso_%i", year), error);
}

//////////////////////////////////////////////////
// Plot the electron ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_electron_scale(int year, bool error = false) {
  TString path = "../../scale_factors/";
  if(year == 2016) {
    path += "2016LegacyReReco_ElectronMVAwp80.root";
  } else if(year == 2017) {
    path += "2017_ElectronMVA80.root";
  } else if(year == 2018) {
    path += "2018_ElectronMVA80.root";
  } else {
    cout << "Unknown year!\n";
    return NULL;
  }
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get("EGamma_SF2D");
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  return plot_scale(hID, Form("c_elec_id_%i", year), error);
}

//////////////////////////////////////////////////
// Plot the electron reco ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_electron_reco_scale(int year, bool error = false) {
  TString path = "../../scale_factors/";
  if(year == 2016) {
    path += "EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root";
  } else if(year == 2017) {
    path += "egammaEffi.txt_EGM2D_runBCDEF_passingRECO_2017.root";
  } else if(year == 2018) {
    path += "egammaEffi.txt_EGM2D_updatedAll_2018.root";
  } else {
    cout << "Unknown year!\n";
    return NULL;
  }
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get("EGamma_SF2D");
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  return plot_scale(hID, Form("c_elec_reco_%i", year), error);
}

//////////////////////////////////////////////////
// Plot the electron trigger scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_electron_trigger_scale(int year, bool error = false) {
  TString path = "../../scale_factors/egamma_trigger_eff_";
  path += year; path += ".root";
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get("EGamma_SF2D");
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  if(!error) {
    TCanvas* c = plot_1D_slices(hID, Form("c_elec_trig_slice_%i", year), true);
    if(c) {
      c->Print(Form("figures/electron_trigger_slices_%i.png", year));
      c->SetLogx();
      c->Print(Form("figures/electron_trigger_slices_log_%i.png", year));    }
  }
  return plot_scale(hID, Form("c_elec_trigger_%i", year), error);
}

//////////////////////////////////////////////////
// Plot the embedding scale factors
//////////////////////////////////////////////////
TCanvas* plot_embedding_scale(int year, int Mode, bool isMuon) {
  TString path = Form("../../scale_factors/embedding_eff_%s_mode-%i_%i.root", (isMuon) ? "mumu" : "ee", Mode, year);
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2F* hID = (TH2F*) f->Get("PtVsEtaSF");
  if(!hID) {
    cout << "Embedding histogram for " << year << " Mode = " << Mode << " isMuon = " << isMuon << " not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  TCanvas* c = plot_1D_slices(hID, Form("c_embed_%s_mode-%i_slice_%i", (isMuon) ? "mumu" : "ee", Mode, year), true);
  c->SetTitle("");
  c->Print(Form("figures/embed_%s_mode-%i_slices_%i.png", (isMuon) ? "mumu" : "ee", Mode, year));
  c->SetLogx();
  c->Print(Form("figures/embed_%s_mode-%i_slices_log_%i.png", (isMuon) ? "mumu" : "ee", Mode, year));
  return c;
}

//////////////////////////////////////////////////
// Plot the embedding KIT scale factors
//////////////////////////////////////////////////
TCanvas* plot_embedding_kit_scale(int year, int Mode, bool isMuon) {
  TString path = Form("../../scale_factors/htt_scalefactors_legacy_%i.root", year);
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  RooWorkspace* w = (RooWorkspace*) f->Get("w");
  if(!w) return NULL;
  RooRealVar* pt  = (RooRealVar*) w->obj((isMuon) ? "m_pt"  : "e_pt");
  RooRealVar* eta = (RooRealVar*) w->obj((isMuon) ? "m_eta" : "e_eta");
  RooFormulaVar* scale = 0;
  if(year == 2017) {
    if(Mode == 0) scale = (RooFormulaVar*) w->obj((isMuon) ? "m_trg27_embed_kit_ratio" : "e_trg32_embed_kit_ratio");
    if(Mode == 1) scale = (RooFormulaVar*) w->obj((isMuon) ? "m_id_embed_kit_ratio" : "e_id80_kit_ratio");
    if(Mode == 2 && isMuon) return NULL; //iso binned drawing is broken
    if(Mode == 2) scale = (RooFormulaVar*) w->obj((isMuon) ? "m_iso_binned_embed_kit_ratio" : "e_iso_kit_ratio");
  }
  if(!scale || !pt || !eta) return NULL;

  vector<double> eta_bins;
  if(isMuon) eta_bins = {0., 1., 1.4442, 1.566, 2.1, 2.5}; //{-2.4,-2.0,-1.5,-0.8, 0.0, 0.8, 1.5, 2.0, 2.4};
  else       eta_bins = {0., 0.9, 1.2, 2.1, 2.4}; //{-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};

  TString cname = Form("embed_kit_scale_%s_mode-%i_%i", (isMuon) ? "mumu" : "ee", Mode, year);
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  TLegend* leg = new TLegend(0.7, 0.13, 0.9, 0.5);
  int colors[] = {kRed, kBlue, kGreen, kViolet, kOrange, kYellow+2,
                  kAtlantic, kGreen+2, kRed+2, kBlue+2, kViolet-2};
  auto frame = pt->frame(((Mode == 0) ? ((isMuon) ? 20. : 25) : 10.), 500.);
  if(!frame) return NULL;
  for(int ibin = 1; ibin < eta_bins.size(); ++ibin) {
    eta->setVal((eta_bins[ibin - 1] + eta_bins[ibin])/2.);
    scale->plotOn(frame, RooFit::LineColor(colors[ibin-1]), RooFit::Name(Form("bin_%i", ibin)));
  }
  frame->Draw();
  for(int ibin = 1; ibin < eta_bins.size(); ++ibin) {
    leg->AddEntry(Form("bin_%i", ibin), Form("#eta = %.2f", (eta_bins[ibin - 1] + eta_bins[ibin])/2.));
  }
  frame->SetMinimum(0.);
  frame->SetMaximum(2.);
  leg->Draw();
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.1);
  c->SetRightMargin(0.1);
  c->Print(Form("figures/%s.png", cname.Data()));
  c->SetLogx();
  c->Print(Form("figures/%s_log.png", cname.Data()));
  f->Close();
  return c;
}


//////////////////////////////////////////////////
// Plot the tau anti-jet ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_tau_jet_ID_scale(int year) {
  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/TauPOG/TauIDSFs/data/TauID_SF_pt_DeepTau2017v2p1VSjet_";
  path += year;
  if(year == 2016) path += "Legacy";
  else             path += "ReReco";
  path += ".root";
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TF1* fID      = (TF1*) f->Get("Medium_cent");
  TF1* fID_up   = (TF1*) f->Get("Medium_up");
  TF1* fID_down = (TF1*) f->Get("Medium_down");
  if(!fID || !fID_up || !fID_down) {
    cout << __func__ << ": ID scale functions not found!\n";
    return NULL;
  }
  // fID->SetDirectory(0);
  // fID_up->SetDirectory(0);
  // fID_down->SetDirectory(0);
  // f->Close();
  return plot_scale(fID, Form("c_tau_jet_id_%i", year), fID_up, fID_down);
}

//////////////////////////////////////////////////
// Plot the tau anti-muon ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_tau_mu_ID_scale(int year) {
  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/TauPOG/TauIDSFs/data/TauID_SF_eta_DeepTau2017v2p1VSmu_";
  path += year;
  if(year == 2016) path += "Legacy";
  else             path += "ReReco";
  path += ".root";
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH1* hID      = (TH1*) f->Get("Tight");
  if(!hID) {
    cout << __func__ << ": ID scale histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  return plot_scale(hID, Form("c_tau_mu_id_%i", year));
}

//////////////////////////////////////////////////
// Plot the tau anti-electron ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_tau_ele_ID_scale(int year) {
  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/TauPOG/TauIDSFs/data/TauID_SF_eta_DeepTau2017v2p1VSe_";
  path += year;
  if(year == 2016) path += "Legacy";
  else             path += "ReReco";
  path += ".root";
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH1* hID      = (TH1*) f->Get("Medium");
  if(!hID) {
    cout << __func__ << ": ID scale histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  return plot_scale(hID, Form("c_tau_ele_id_%i", year));
}

void plot_scales() {
  gStyle->SetOptStat(0);
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  for(int year = 2016; year < 2019; ++year) {
    TCanvas* c;
    c = plot_tau_jet_ID_scale(year);
    if(c) {
      c->Print(Form("figures/tau_jet_id_%i.png", year));
      delete c;
    }
    c = plot_tau_mu_ID_scale(year);
    if(c) {
      c->Print(Form("figures/tau_mu_id_%i.png", year));
      delete c;
    }
    c = plot_tau_ele_ID_scale(year);
    if(c) {
      c->Print(Form("figures/tau_ele_id_%i.png", year));
      delete c;
    }
    c = plot_electron_scale(year, false);
    if(c) {
      c->Print(Form("figures/electron_scale_%i.png", year));
      delete c;
    }
    c = plot_electron_scale(year, true);
    if(c) {
      c->Print(Form("figures/electron_scale_%i_err.png", year));
      delete c;
    }
    c = plot_electron_reco_scale(year, false);
    if(c) {
      c->Print(Form("figures/electron_reco_scale_%i.png", year));
      delete c;
    }
    c = plot_electron_reco_scale(year, true);
    if(c) {
      c->Print(Form("figures/electron_reco_scale_%i_err.png", year));
      delete c;
    }
    c = plot_electron_trigger_scale(year, false);
    if(c) {
      c->Print(Form("figures/electron_trigger_scale_%i.png", year));
      delete c;
    }
    c = plot_electron_trigger_scale(year, true);
    if(c) {
      c->Print(Form("figures/electron_trigger_scale_%i_err.png", year));
      delete c;
    }

    //Do muon scale factors
    c = plot_muon_low_trigger_scale(year, 0, false);
    if(c) {
      c->Print(Form("figures/muon_low_trigger_scale_%i.png", year));
      delete c;
    }
    c = plot_muon_low_trigger_scale(year, 0, true);
    if(c) {
      c->Print(Form("figures/muon_low_trigger_scale_%i_err.png", year));
      delete c;
    }
    c = plot_muon_ID_scale(year, 0, false);
    if(c) {
      c->Print(Form("figures/muon_ID_scale_%i.png", year));
      delete c;
    }
    c = plot_muon_ID_scale(year, 0, true);
    if(c) {
      c->Print(Form("figures/muon_ID_scale_%i_err.png", year));
      delete c;
    }
    c = plot_muon_iso_scale(year, 0, false);
    if(c) {
      c->Print(Form("figures/muon_Iso_scale_%i.png", year));
      delete c;
    }
    c = plot_muon_iso_scale(year, 0, true);
    if(c) {
      c->Print(Form("figures/muon_Iso_scale_%i_err.png", year));
      delete c;
    }
    if(year == 2016) {
      c = plot_muon_ID_scale(year, 1, false);
      if(c) {
        c->Print(Form("figures/muon_ID_scale_%i_period_2.png", year));
        delete c;
      }
      c = plot_muon_ID_scale(year, 1, true);
      if(c) {
        c->Print(Form("figures/muon_ID_scale_%i_period_2_err.png", year));
        delete c;
      }
      c = plot_muon_iso_scale(year, 1, false);
      if(c) {
        c->Print(Form("figures/muon_iso_scale_%i_period_2.png", year));
        delete c;
      }
      c = plot_muon_iso_scale(year, 1, true);
      if(c) {
        c->Print(Form("figures/muon_iso_scale_%i_period_2_err.png", year));
        delete c;
      }
    }
    //Embedding TnP scale factors
    c = plot_embedding_scale    (year, 0, false); if(c) delete c; //electron trigger
    c = plot_embedding_scale    (year, 1, false); if(c) delete c; //electron ID
    c = plot_embedding_scale    (year, 0, true ); if(c) delete c; //muon trigger
    c = plot_embedding_scale    (year, 1, true ); if(c) delete c; //muon ID
    c = plot_embedding_scale    (year, 2, true ); if(c) delete c; //muon iso ID
    //Embedding KIT scales
    c = plot_embedding_kit_scale(year, 0, false); if(c) delete c; //electron trigger
    c = plot_embedding_kit_scale(year, 1, false); if(c) delete c; //electron ID
    c = plot_embedding_kit_scale(year, 2, false); if(c) delete c; //electron iso ID
    c = plot_embedding_kit_scale(year, 0, true ); if(c) delete c; //muon trigger
    c = plot_embedding_kit_scale(year, 1, true ); if(c) delete c; //muon ID
    c = plot_embedding_kit_scale(year, 2, true ); if(c) delete c; //muon iso ID
  }
}
