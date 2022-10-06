//script to create plots of scale factors used in the analysis


//////////////////////////////////////////////////
// Plot the efficiencies and scale factors
//////////////////////////////////////////////////
int plot_1D_efficiencies(vector<TH2*> hists, TString path, TString name, bool xaxis = true, bool logx = false) {

  // For each bin in the histograms, make a figure of the efficiencies and the ratios
  const int nhists = hists.size();
  if(nhists == 0) {cout << __func__ << ": No histograms given!\n"; return 1;}

  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", path.Data(), path.Data()));
  if(!hists[0]) {cout << __func__ << ": First histogram is not defined!\n"; return 2;}
  const int nbins = (xaxis) ? hists[0]->GetNbinsX() : hists[0]->GetNbinsY();
  const int colors[] = {kRed, kBlue, kGreen, kViolet, kOrange, kYellow+2,
                        kAtlantic, kGreen+2, kRed+2, kBlue+2, kViolet-2,
                        kOrange+2,kViolet+2, kBlue-2};
  const int ncolors = sizeof(colors) / sizeof(*colors);

  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.03);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  TString bin_title = "";
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    double ymin(1.e9), ymax(-1.e9), ymin_r(0.6), ymax_r(1.4);
    TH1* haxis = nullptr;
    TH1* haxis_r = nullptr;
    double xmin, xmax;

    TCanvas* c = new TCanvas("c_1D_tmp", "c_1D_tmp", 1000, 1100);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
    pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);
    pad1->SetGrid(); pad2->SetGrid();
    pad1->cd();

    TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg->SetFillStyle(0);
    leg->SetFillColor(0);
    leg->SetLineWidth(0);
    leg->SetLineColor(0);
    vector<TH1*> hists_1D, hratios;
    for(int ihist = 0; ihist < nhists; ++ihist) {
      auto h2D = hists[ihist];
      if(!h2D) {
        cout << __func__ << ": Histogram " << ihist << " not defined for name " << name.Data() << endl;
        return 2;
      }
      TH1* h = (xaxis) ? h2D->ProjectionY(Form("_%i_%i", ihist, ibin), ibin, ibin) : h2D->ProjectionX(Form("_%i_%i", ihist, ibin), ibin, ibin);
      h->SetLineColor(colors[ihist % ncolors]);
      h->SetLineWidth(2);
      h->SetMarkerSize(0.8);
      h->SetMarkerStyle(20);
      h->SetMarkerColor(colors[ihist % ncolors]);
      pad1->cd();
      if(ihist == 0) {
        h->Draw("E");
        haxis = h;
        xmin = (xaxis) ? h2D->GetXaxis()->GetBinLowEdge(ibin) : h2D->GetYaxis()->GetBinLowEdge(ibin);
        xmax = (xaxis) ? xmin + h2D->GetXaxis()->GetBinWidth(ibin) : xmin + h2D->GetYaxis()->GetBinWidth(ibin);
        if(bin_title == "") bin_title = (xaxis) ? h2D->GetXaxis()->GetTitle() : h2D->GetYaxis()->GetTitle();
      } else {
        h->Draw("E same");
        TH1* hratio = (TH1*) haxis->Clone(Form("hratio_%i_%i", ihist, ibin));
        hratio->Divide(h);
        hratio->SetLineColor(h->GetLineColor());
        hratio->SetMarkerColor(h->GetMarkerColor());
        hratios.push_back(hratio);
        pad2->cd();
        if(ihist == 1) {
          haxis_r = hratio;
          hratio->Draw("E");
          hratio->GetXaxis()->SetMoreLogLabels(kTRUE);
          hratio->SetTitle("");
          hratio->GetYaxis()->SetTitle("Ratio");
          hratio->GetXaxis()->SetTitleSize(0.13);
          hratio->GetYaxis()->SetTitleSize(0.13);
          hratio->GetYaxis()->SetTitleOffset(0.30);
          hratio->GetYaxis()->SetLabelSize(0.08);
          hratio->GetXaxis()->SetLabelSize(0.08);
        }
        else hratio->Draw("E same");
      }
      hists_1D.push_back(h);
      leg->AddEntry(h);
      for(int jbin = 1; jbin <= h->GetNbinsX(); ++jbin) {
        const double binc = h->GetBinContent(jbin);
        ymin = std::min(ymin, binc);
        ymax = std::max(ymax, binc);
      }
    }
    pad1->cd();
    leg->Draw();
    haxis->GetXaxis()->SetTitleSize(0.05);
    haxis->GetYaxis()->SetTitleSize(0.05);
    haxis->GetYaxis()->SetTitleOffset(1.);
    haxis->GetYaxis()->SetRangeUser(0.9*ymin, 1.25*ymax);
    haxis->SetTitle("");
    haxis->SetXTitle("");
    haxis->SetYTitle("Efficiency");
    label.DrawLatex(0.12, 0.88, Form("%s [%.2f - %.2f]", bin_title.Data(), xmin, xmax));
    if(logx) {pad1->SetLogx(); pad2->SetLogx(); haxis->GetXaxis()->SetMoreLogLabels(kTRUE);}

    pad2->cd();
    haxis_r->GetYaxis()->SetRangeUser(0.5, 1.5);

    c->SaveAs(Form("%s/%s_%i.png", path.Data(), name.Data(), ibin));
    for(auto h : hists_1D) delete h;
    for(auto h : hratios ) delete h;
    delete pad1;
    delete pad2;
    delete c;
  }
  return 0;
}


//////////////////////////////////////////////////
// Plot the scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_1D_slices(TH2* hID, TString cname, bool xaxis, TH1*& haxis) {
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  const int nbins = (xaxis) ? hID->GetNbinsX() : hID->GetNbinsY();
  TLegend* leg = new TLegend(0.7, 0.13, 0.9, 0.5);
  const int colors[] = {kRed, kBlue, kGreen, kViolet, kOrange, kYellow+2,
                        kAtlantic, kGreen+2, kRed+2, kBlue+2, kViolet-2,
                        kOrange+2,kViolet+2, kBlue-2};
  double ymin(1.e9), ymax(-1.e9);
  haxis = nullptr;
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

TCanvas* plot_1D_slices(TH2* hID, TString cname, bool xaxis = true) {
  TH1* h;
  return plot_1D_slices(hID, cname, xaxis, h);
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
TCanvas* plot_scale(TH2* hID, TString cname, bool error, bool dotext = false) {
  gStyle->SetPaintTextFormat(".2f");
  double min_val(1.e10), max_val(-1.);
  for(int xbin = 1; xbin <= hID->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hID->GetNbinsY(); ++ybin) {
      if(error) {
        const double err = hID->GetBinError(xbin, ybin);
        hID->SetBinContent(xbin, ybin, err);
      }
      const double val = hID->GetBinContent(xbin, ybin);
      max_val = max(max_val, val);
      min_val = min(min_val, val);
    }
  }
  hID->GetZaxis()->SetRangeUser(0.95*min_val, 1.05*max_val);
  if(error) cname += "_err";
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);
  hID->Draw((dotext) ? "colz text" : "colz");
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
  TH2* hID = (TH2*) f->Get(hname.Data());
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  if(!error) {
    hname.ReplaceAll("abseta_pt_ratio", "efficienciesDATA/abseta_pt_DATA");
    TH2* hData = (TH2*) f->Get(hname.Data());
    if(hData) {
      TCanvas* c = plot_scale(hData, Form("c_muon_low_trigger_data_%i", year), false);
      if(c) {
        hData->GetYaxis()->SetMoreLogLabels(kTRUE);
        c->SetLogy();
        c->SaveAs(Form("figures/muon_low_trigger_data_%i%s.png", year, (period > -1) ? Form("_period_%i", period) : ""));
        delete c;
      }
    }
    hname.ReplaceAll("efficienciesDATA/abseta_pt_DATA", "efficienciesMC/abseta_pt_MC");
    TH2* hMC = (TH2*) f->Get(hname.Data());
    if(hMC) {
      TCanvas* c = plot_scale(hMC, Form("c_muon_low_trigger_mc_%i", year), false);
      c->SetLogy();
      if(c) {
        hMC->GetYaxis()->SetMoreLogLabels(kTRUE);
        c->SetLogy();
        c->SaveAs(Form("figures/muon_low_trigger_mc_%i%s.png", year, (period > -1) ? Form("_period_%i", period) : ""));
        delete c;
      }
    }
  }
  hID->SetDirectory(0);
  f->Close();
  hID->SetTitle("Muon Iso trigger scale");
  hID->SetXTitle(xname.Data());
  hID->SetYTitle(yname.Data());
  hID->SetZTitle("");
  TH1* haxis = nullptr;
  TCanvas* c = plot_1D_slices(hID, Form("c_muon_low_trig_slice_%i", year), true, haxis);
  if(c) {
    c->Print(Form("figures/muon_low_trigger_slices_%i.png", year));
    c->SetLogx();
    if(haxis) haxis->GetXaxis()->SetMoreLogLabels(kTRUE);
    c->Print(Form("figures/muon_low_trigger_slices_%i_log.png", year));
  }
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
  TH2* hID = (TH2*) f->Get(hname.Data());
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
  TH2* hID = (TH2*) f->Get(hname.Data());
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
    path += "2016LegacyReReco_ElectronMVA90noiso_Fall17V2.root";
  } else if(year == 2017) {
    path += "2017_ElectronMVA90noiso.root";
  } else if(year == 2018) {
    path += "2018_ElectronMVA90noiso.root";
  } else {
    cout << "Unknown year!\n";
    return NULL;
  }
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2* hID = (TH2*) f->Get("EGamma_SF2D");
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  return plot_scale(hID, Form("c_elec_id_%i", year), error, true);
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
  TH2* hID = (TH2*) f->Get("EGamma_SF2D");
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
TCanvas* plot_electron_trigger_scale(int year, int WP = 3, bool error = false) {
  TString path = Form("../../scale_factors/egamma_trigger_eff_wp%i_%i.root", WP, year);
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2* hID = (TH2*) f->Get("EGamma_SF2D");
  if(!hID) {
    cout << "ID histogram not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  f->Close();
  if(!error) {
    TH1* haxis = nullptr;
    TCanvas* c = plot_1D_slices(hID, Form("c_elec_trig_slice_wp%i_%i", WP, year), true, haxis);
    if(c) {
      c->Print(Form("figures/electron_trigger_slices_wp%i_%i.png", WP, year));
      c->SetLogx();
      if(haxis) haxis->GetXaxis()->SetMoreLogLabels(kTRUE);
      c->Print(Form("figures/electron_trigger_slices_wp%i_%i_log.png", WP, year));
    }
  }
  return plot_scale(hID, Form("c_elec_trigger_wp%i_%i", WP, year), error);
}

//////////////////////////////////////////////////
// Plot the embedding scale factors
//////////////////////////////////////////////////
TCanvas* plot_embedding_scale(int year, int Mode, bool isMuon, int period = -1) {
  TString path = Form("../../scale_factors/embedding_eff_%s_mode-%i_%i%s.root",
                      (isMuon) ? "mumu" : "ee", Mode, year, (period >= 0) ? Form("_period_%i", period) : "");
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  TH2* hID = (TH2*) f->Get("PtVsEtaSF");
  if(!hID) {
    cout << "Embedding histogram for " << year << " Mode = " << Mode << " isMuon = " << isMuon << " not found!\n";
    return NULL;
  }
  hID->SetDirectory(0);
  hID->SetTitle("Data Efficiency / Embedding Efficiency");
  TH1* haxis = nullptr;
  TCanvas* c = plot_1D_slices(hID, Form("c_embed_%s_mode-%i_slice_%i",
                                        (isMuon) ? "mumu" : "ee", Mode, year), true, haxis);
  c->SetTitle("");
  c->Print(Form("figures/embed_%s_mode-%i_slices_%i%s.png",
                (isMuon) ? "mumu" : "ee", Mode, year, (period >= 0) ? Form("_period_%i", period) : ""));
  c->SetLogx();
  haxis->GetXaxis()->SetMoreLogLabels(kTRUE);
  c->Print(Form("figures/embed_%s_mode-%i_slices_log_%i%s.png",
                (isMuon) ? "mumu" : "ee", Mode, year, (period >= 0) ? Form("_period_%i", period) : ""));

  vector<TH2*> hists;
  TH2* hData = (TH2*) f->Get("PtVsEtaData");
  TH2* hMC   = (TH2*) f->Get("PtVsEtaMC"  );
  TH2* hDYMC = (TH2*) f->Get("PtVsEtaDYMC");
  hData->SetTitle("Data");
  hMC->SetTitle("Embedding");
  hists.push_back(hData);
  hists.push_back(hMC  );
  if(hDYMC) {
    hDYMC->SetTitle("MC");
    hists.push_back(hDYMC);
  }
  TString dir = Form("figures/embed_%s_mode-%i_effs_%i%s", (isMuon) ? "mumu" : "ee", Mode, year,
                     (period >= 0) ? Form("_period_%i", period) : "");
  plot_1D_efficiencies(hists, dir.Data(), "eff", true, true);
  f->Close();
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
  } else {
    if(Mode == 0) scale = (RooFormulaVar*) w->obj((isMuon) ? "m_trg_ratio" : "e_trg_ratio");
    if(Mode == 1) scale = (RooFormulaVar*) w->obj((isMuon) ? "m_id_ratio" : "e_id_ratio");
    if(Mode == 2 && isMuon) return NULL; //iso binned drawing is broken
    if(Mode == 2) scale = (RooFormulaVar*) w->obj((isMuon) ? "m_iso_ratio" : "e_iso_ratio");
  }

  if(!scale || !pt || !eta) return NULL;

  vector<double> eta_bins;
  if(!isMuon) eta_bins = {0., 1., 1.4442, 1.566, 2.1, 2.5}; //{-2.4,-2.0,-1.5,-0.8, 0.0, 0.8, 1.5, 2.0, 2.4};
  else        eta_bins = {0., 0.9, 1.2, 2.1, 2.4}; //{-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};

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
  frame->GetXaxis()->SetMoreLogLabels(kTRUE);
  c->Print(Form("figures/%s_log.png", cname.Data()));
  f->Close();
  return c;
}

TCanvas* plot_embedding_unfold_scale(int year, int Mode) {
  const bool use_ic = (Mode %  100) /  10 == 1;
  const bool use_2  = (Mode % 1000) / 100 == 1;
  Mode %= 10;
  TString path = Form("../../scale_factors/htt_%sscalefactors_legacy_%i.root", (use_ic) ? "ic_" : "", year);
  TFile* f = TFile::Open(path.Data(), "READ");
  if(!f) return NULL;
  RooWorkspace* w = (RooWorkspace*) f->Get("w");
  if(!w) return NULL;
  RooRealVar* pt    = (RooRealVar*) w->obj(Form("gt%i_pt" , (use_2) ? 2 : 1));
  RooRealVar* eta   = (RooRealVar*) w->obj(Form("gt%i_eta", (use_2) ? 2 : 1));
  RooHistFunc* hist = (RooHistFunc*) w->obj(Form("m_sel_trg%s%i_%s%i_%sdata",
                                                 (use_ic) ? "_" : "",
                                                 (Mode == 0) ? 17 : 8,
                                                 (use_ic) ? "ic_" : "",
                                                 (use_2) ? 2 : 1,
                                                 (year == 2016 && !use_ic) ? "kit_" : ""));

  if(!hist || !pt || !eta) return NULL;

  TString cname = Form("embed_%s_unfold_%i_%i%s", (use_ic) ? "ic" : "kit", year, Mode, (use_2) ? "_tau-2" : "");
  TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1000, 700);

  vector<double> eta_bins = {0., 0.9, 1.2, 2.1, 2.4};

  TLegend* leg = new TLegend(0.75, 0.6, 0.9, 0.9);
  int colors[] = {kRed, kBlue, kGreen, kViolet, kOrange, kYellow+2,
                  kAtlantic, kGreen+2, kRed+2, kBlue+2, kViolet-2};
  auto frame = pt->frame(8., 500.);
  if(!frame) return NULL;
  for(int ibin = 1; ibin < eta_bins.size(); ++ibin) {
    eta->setVal((eta_bins[ibin - 1] + eta_bins[ibin])/2.);
    hist->plotOn(frame, RooFit::LineColor(colors[ibin-1]), RooFit::Name(Form("bin_%i", ibin)));
  }
  frame->Draw();
  for(int ibin = 1; ibin < eta_bins.size(); ++ibin) {
    leg->AddEntry(Form("bin_%i", ibin), Form("#eta = %.2f", (eta_bins[ibin - 1] + eta_bins[ibin])/2.));
  }
  frame->SetMinimum(0.8);
  frame->SetMaximum(1.1);
  leg->Draw();
  c->SetBottomMargin(0.13);
  c->SetLeftMargin(0.1);
  c->SetRightMargin(0.1);
  c->SetLogx();
  frame->GetXaxis()->SetMoreLogLabels(true);
  c->Print(Form("figures/%s.png", cname.Data()));

  f->Close();
  return c;
}


//////////////////////////////////////////////////
// Plot the tau anti-jet ID scale factors/errors
//////////////////////////////////////////////////
TCanvas* plot_tau_jet_ID_scale(int year, bool isembed = false) {
  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/TauPOG/TauIDSFs/data/TauID_SF_pt_DeepTau2017v2p1VSjet_";
  path += year;
  if(year == 2016) path += "Legacy";
  else             path += "ReReco";
  if(isembed) path += "_EMB";
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

void plot_scales(int only_year = -1) {
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptStat(0);
  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  const int year_min = (only_year > 0) ? only_year : 2016;
  const int year_max = (only_year > 0) ? only_year : 2018;
  for(int year = year_min; year <= year_max; ++year) {
    TCanvas* c;
    c = plot_tau_jet_ID_scale(year);
    if(c) {
      c->Print(Form("figures/tau_jet_id_%i.png", year));
      delete c;
    }
    c = plot_tau_jet_ID_scale(year, true);
    if(c) {
      c->Print(Form("figures/tau_jet_id_emb_%i.png", year));
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
    c = plot_electron_trigger_scale(year, 3, false);
    if(c) {
      c->Print(Form("figures/electron_trigger_scale_wp3_%i.png", year));
      delete c;
    }
    c = plot_electron_trigger_scale(year, 3, true);
    if(c) {
      c->Print(Form("figures/electron_trigger_scale_wp3_%i_err.png", year));
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
    if(year == 2016) {
      c = plot_muon_low_trigger_scale(year, 1, false);
      if(c) {
        c->Print(Form("figures/muon_low_trigger_scale_%i_period_1.png", year));
        delete c;
      }
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
    for(int period = -1; period < (year == 2016)*2; ++period) { //B-F and G-H specific scale factors for 2016
      c = plot_embedding_scale    (year, 0, false, period); if(c) delete c; //electron trigger
      c = plot_embedding_scale    (year, 1, false, period); if(c) delete c; //electron ID
      c = plot_embedding_scale    (year, 2, false, period); if(c) delete c; //electron Iso ID
      c = plot_embedding_scale    (year, 3, false, period); if(c) delete c; //electron Loose + !Tight ID
      c = plot_embedding_scale    (year, 4, false, period); if(c) delete c; //electron trigger, Loose + !Tight ID
      c = plot_embedding_scale    (year, 0, true , period); if(c) delete c; //muon trigger
      c = plot_embedding_scale    (year, 1, true , period); if(c) delete c; //muon ID
      c = plot_embedding_scale    (year, 2, true , period); if(c) delete c; //muon iso ID
      c = plot_embedding_scale    (year, 3, true , period); if(c) delete c; //muon Loose + !Tight iso ID
      c = plot_embedding_scale    (year, 4, true , period); if(c) delete c; //muon trigger, Loose + !Tight iso ID
    }

    //Embedding KIT scales
    c = plot_embedding_kit_scale(year, 0, false); if(c) delete c; //electron trigger
    c = plot_embedding_kit_scale(year, 1, false); if(c) delete c; //electron ID
    c = plot_embedding_kit_scale(year, 2, false); if(c) delete c; //electron iso ID
    c = plot_embedding_kit_scale(year, 0, true ); if(c) delete c; //muon trigger
    c = plot_embedding_kit_scale(year, 1, true ); if(c) delete c; //muon ID
    c = plot_embedding_kit_scale(year, 2, true ); if(c) delete c; //muon iso ID

    //Embedding unfolding corrections
    c = plot_embedding_unfold_scale(year,   0); if(c) delete c; //Mu17 leg
    c = plot_embedding_unfold_scale(year,   1); if(c) delete c; //Mu8  leg
    // c = plot_embedding_unfold_scale(year, 100); if(c) delete c; //Mu17-2 leg
    // c = plot_embedding_unfold_scale(year, 101); if(c) delete c; //Mu8-2  leg
    //Imperial college version
    c = plot_embedding_unfold_scale(year,  10); if(c) delete c; //Mu17 leg
    c = plot_embedding_unfold_scale(year,  11); if(c) delete c; //Mu8  leg
    // c = plot_embedding_unfold_scale(year, 110); if(c) delete c; //Mu17-2 leg
    // c = plot_embedding_unfold_scale(year, 111); if(c) delete c; //Mu8-2  leg
  }
}
