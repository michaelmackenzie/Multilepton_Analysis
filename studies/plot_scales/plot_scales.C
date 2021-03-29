//script to create plots of scale factors used in the analysis

//////////////////////////////////////////////////
// Plot the electron ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_muon_scale(int year, int period = 0, bool error = false) {
  TString path = "../../../ZEMuAnalysis/test/scale_factors/";
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

  TString name = Form("c_muon_id_%i%s", year, (error) ? "_err" : "");
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1000, 700);
  hID->Draw("colz");
  hID->SetTitle("Muon ID scale");
  hID->SetXTitle(xname.Data());
  hID->SetYTitle(yname.Data());
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
// Plot the electron ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_electron_scale(int year, bool error = false) {
  TString path = "../../../ZEMuAnalysis/test/scale_factors/";
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
    hID->GetZaxis()->SetRangeUser(0., 0.2); //0.95*min_err, 1.05*max_err);
  }
  TString name = Form("c_elec_id_%i%s", year, (error) ? "_err" : "");
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1000, 700);
  hID->Draw("colz");
  if(error) hID->SetTitle(Form("%s errors", hID->GetTitle()));
  hID->GetYaxis()->SetTitleOffset(1.);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.13);
  c->SetRightMargin(0.13);
  return c;
}

//////////////////////////////////////////////////
// Plot the electron reco ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_electron_reco_scale(int year, bool error = false) {
  TString path = "../../../ZEMuAnalysis/test/scale_factors/";
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
  TString name = Form("c_elec_reco_id_%i%s", year, (error) ? "_err" : "");
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1000, 700);
  hID->Draw("colz");
  if(error) hID->SetTitle(Form("%s errors", hID->GetTitle()));
  hID->GetYaxis()->SetTitleOffset(1.);
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.13);
  c->SetRightMargin(0.13);
  return c;
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
    c = plot_muon_scale(year, 0, false);
    if(c) {
      c->Print(Form("figures/muon_scale_%i.png", year));
      delete c;
    }
    c = plot_muon_scale(year, 0, true);
    if(c) {
      c->Print(Form("figures/muon_scale_%i_err.png", year));
      delete c;
    }
    if(year == 2016) {
      c = plot_muon_scale(year, 1, false);
      if(c) {
        c->Print(Form("figures/muon_scale_%i_2.png", year));
        delete c;
      }
      c = plot_muon_scale(year, 1, true);
      if(c) {
        c->Print(Form("figures/muon_scale_%i_2_err.png", year));
        delete c;
      }
    }
  }
}
