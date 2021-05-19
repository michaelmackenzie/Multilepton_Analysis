//script to create plots of scale factors used in the analysis

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

void plot_scales() {
  gStyle->SetOptStat(0);
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  for(int year = 2016; year < 2019; ++year) {
    TCanvas* c;
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
  }
}
