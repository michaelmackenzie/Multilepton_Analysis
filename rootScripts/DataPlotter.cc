#include "DataPlotter.hh"

void DataPlotter::get_titles(TString hist, TString setType, TString* xtitle, TString* ytitle, TString* title) {
  if(hist == "lepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH_);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH_);
    *title  = Form("#DeltaR vs #Delta#phi Between Leptons %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "sysm" && setType == "event") {
    *xtitle = "M_{ll#gamma} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("Mass of the Photon + Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "met") {
    *xtitle = "Missing Transverse Energy (GeV)";
    *ytitle = Form("Events / %.0f GeV",2.*rebinH_);
    *title  = Form("Missing Transverse Energy %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepptoverm") {
    *xtitle = "Pt / M_{ll}";
    *ytitle = Form("Events / %.1f GeV/c^{2}",0.2*rebinH_);
    *title  = Form("Pt Over Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepm") {
    *xtitle = "M_{ll} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "leppt") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",2.*rebinH_);
    *title  = Form("Pt of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "pt" && setType == "photon") {
    *xtitle = "Pt_{#gamma} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",2.*rebinH_);
    *title  = Form("Pt of the Photon");
  }
  else if(hist == "onept") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Pt of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twopt") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Pt of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onesvpt") {
    *xtitle = "SVFit Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit Pt of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twosvpt") {
    *xtitle = "SVFit Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit Pt of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onem") {
    *xtitle = "M (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Mass of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twom") {
    *xtitle = "M (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Mass of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onesvm") {
    *xtitle = "SVFit M (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit Mass of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twosvm") {
    *xtitle = "SVFit M (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit Mass of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "oneiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH_);
    *title  = Form("Isolation of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twoiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH_);
    *title  = Form("Isolation of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdeltar") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#Delta R of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepeta") {
    *xtitle = "#eta";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#eta of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "pxivis0") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Visible P_{T} projected onto the lepton bisector");
  }
  else if(hist == "pxiinv0") {
    *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Invisible P_{T} projected onto the lepton bisector");
  }
  else if(hist == "pxidiff0") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} - #vec{P}_{T}^{inv}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Visible - invisible P_{T} projected onto the lepton bisector");
  }
  else if(hist == "pxidiff20") {
    *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta} - f(#vec{P}_{T}^{vis}#bullet#hat{#zeta})";
    *ytitle = Form("");
    *title  = Form("Invisible - f(visible) P_{T} projected onto the lepton bisector");
  }
  else if(hist == "pxivisoverinv0") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} / #vec{P}_{T}^{inv}#bullet#hat{#zeta} ";
    *ytitle = Form("");
    *title  = Form("Visible P_{T} / invisible P_{T} projected onto the lepton bisector");
  }
  else if(hist == "pxiinvvsvis0") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
    *ytitle = Form("#vec{P}_{T}^{inv}#bullet#hat{#zeta} ");
    *title  = Form("Invisible P_{T} vs visible P_{T} projected onto the lepton bisector");
  }
  else if(hist == "pxivis1") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Visible P_{T} projected onto the lepton, lepton+#gamma bisector");
  }
  else if(hist == "pxiinv1") {
    *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Invisible P_{T} projected onto the lepton, lepton+#gamma bisector");
  }
  else if(hist == "pxidiff1") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} - #vec{P}_{T}^{inv}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Visible - invisible P_{T} projected onto the lepton, lepton+#gamma bisector");
  }
  else if(hist == "pxidiff21") {
    *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta} - f(#vec{P}_{T}^{vis}#bullet#hat{#zeta})";
    *ytitle = Form("");
    *title  = Form("Invisible - f(visible) P_{T} projected onto the lepton, lepton+#gamma bisector");
  }
  else if(hist == "pxivisoverinv1") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} / #vec{P}_{T}^{inv}#bullet#hat{#zeta} ";
    *ytitle = Form("");
    *title  = Form("Visible P_{T} / invisible P_{T} projected onto the lepton, lepton+#gamma bisector");
  }
  else if(hist == "pxiinvvsvis1") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
    *ytitle = Form("#vec{P}_{T}^{inv}#bullet#hat{#zeta} ");
    *title  = Form("Invisible P_{T} vs visible P_{T} projected onto the lepton, lepton+#gamma bisector");
  }
  else if(hist == "pxivis2") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Visible P_{T} projected onto the di-lepton, #gamma bisector");
  }
  else if(hist == "pxiinv2") {
    *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Invisible P_{T} projected onto the di-lepton, #gamma bisector");
  }
  else if(hist == "pxidiff2") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} - #vec{P}_{T}^{inv}#bullet#hat{#zeta}";
    *ytitle = Form("");
    *title  = Form("Visible - invisible P_{T} projected onto the di-lepton, #gamma bisector");
  }
  else if(hist == "pxidiff22") {
    *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta} - f(#vec{P}_{T}^{vis}#bullet#hat{#zeta})";
    *ytitle = Form("");
    *title  = Form("Invisible - f(visible) P_{T} projected onto the di-lepton, #gamma bisector");
  }
  else if(hist == "pxivisoverinv2") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} / #vec{P}_{T}^{inv}#bullet#hat{#zeta} ";
    *ytitle = Form("");
    *title  = Form("Visible P_{T} / invisible P_{T} projected onto the di-lepton, #gamma bisector");
  }
  else if(hist == "pxiinvvsvis2") {
    *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
    *ytitle = Form("#vec{P}_{T}^{inv}#bullet#hat{#zeta} ");
    *title  = Form("Invisible P_{T} vs visible P_{T} projected onto the di-lepton, #gamma bisector");
  }
  else if(hist == "masssvfit") {
    *xtitle = "M_{#tau#tau} (GeV/c^{2})";
    *ytitle = "";
    *title  = Form("SVFit di-tau mass");
  }
  else if(hist == "masserrsvfit") {
    *xtitle = "M_{#tau#tau} Predicted Error (GeV/c^{2})";
    *ytitle = "";
    *title  = Form("SVFit di-tau mass error");
  }
}

vector<TH1F*> DataPlotter::get_signal(TString hist, TString setType, Int_t set) {
  vector<TH1F*> h;
  Int_t color[] = {kAzure-2, kGreen+3, kViolet-2, kOrange+10, kYellow+3,kOrange-9,kBlue+1};
  Int_t fill[]  = {3001,3002,3002,3003,3005,3006,3003,3003};
  
  //for combining histograms of the same process
  map<TString, unsigned int> indexes;
  
  map<TString, int> colors; //index for the color array
  unsigned int n_colors = 0;
  
  for(UInt_t i = 0; i < data_.size(); ++i) {
    h.push_back(NULL);
    if(isData_[i]) continue;
    if(!isSignal_[i]) continue;
    TH1F* tmp = (TH1F*) data_[i]->Get(Form("%s_%i/%s", setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    tmp->Scale(scale_[i]);

    h[i] = tmp;
    if(rebinH_ > 1) h[i]->Rebin(rebinH_);
    unsigned int index = i;
    unsigned int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
    }
    TString name = labels_[index];
    if(index !=  i)  {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
    
      h[index]->SetFillStyle(fill [i_color]);
      h[index]->SetFillColor(color[i_color]);
      h[index]->SetLineColor(color[i_color]);
      h[index]->SetLineWidth(3);
      h[index]->SetName(Form("h_%s_%s",name.Data(),hist.Data()));
      
    }
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s%s = %.2e", name.Data(),
			    (signal_scale_ == 1.) ? "" : Form(" (x%.1f)",signal_scale_), h[index]->Integral()));
  }
  //scale return only meaningful histograms
  vector<TH1F*> hsignals;
  for(unsigned int i = 0; i < h.size(); ++i) {
    if(h[i] && indexes[labels_[i]] == i && h[i]->Integral() > 0.) {
      h[i]->Scale(signal_scale_);
      hsignals.push_back(h[i]);
    }
  }
  
  return hsignals;
}

TH2F* DataPlotter::get_signal_2D(TString hist, TString setType, Int_t set) {
  TH2F* h = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(isData_[i]) continue;
    if(!isSignal_[i]) continue;
    TH2F* tmp = (TH2F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    if(!h) h = tmp;
    else h->Add(tmp);
    h->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", labels_[i].Data(), h->Integral()));
  }
  if(!h) return NULL;
  h->SetLineWidth(2);
  h->SetLineColor(kAzure-2);
  h->SetFillColor(kAzure-2);
  h->SetFillStyle(3001);
  h->SetName("hSignal");
  if(rebinH_ > 0) {
    h->RebinX(rebinH_);
    h->RebinY(rebinH_);
  }

  return h;
}

TH1F* DataPlotter::get_data(TString hist, TString setType, Int_t set) {
  TH1F* d = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(!isData_[i]) continue;
    TH1F* tmp = (TH1F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    if(!d) d = tmp;
    else d->Add(tmp);
  }
  if(!d) return NULL;
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  d->SetTitle(Form("#scale[0.5]{#int} Data = %.2e",  d->Integral()));
  d->SetName("hData");
  if(rebinH_ > 0) d->Rebin(rebinH_);

  return d;
}

TH2F* DataPlotter::get_data_2D(TString hist, TString setType, Int_t set) {
  TH2F* d = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(!isData_[i]) continue;
    TH2F* tmp = (TH2F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    if(!d) d = tmp;
    else d->Add(tmp);
  }
  if(!d) return NULL;
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  d->SetTitle(Form("#scale[0.5]{#int} Data = %.2e",  d->Integral()));
  d->SetName("hData");
  if(rebinH_ > 0) {
    d->RebinX(rebinH_);
    d->RebinY(rebinH_);
  }

  return d;
}

TH1F* DataPlotter::get_qcd(TString hist, TString setType, Int_t set) {
  Int_t set_qcd = set + qcd_offset_;

  TH1F* hData = get_data(hist, setType, set_qcd);
  if(!hData) return hData;
  hData->SetName(Form("qcd_%s",hist.Data()));      
  TH1F* hMC = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(isData_[i]) continue;
    TH1F* htmp = (TH1F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set_qcd, hist.Data()));
    if(!htmp) continue;
    htmp = (TH1F*) htmp->Clone("tmp");
    htmp->Scale(scale_[i]);
    if(rebinH_ > 0) htmp->Rebin(rebinH_);
    if(hMC) hMC->Add(htmp);
    else hMC = (TH1F*) htmp->Clone("qcd_tmp");
    delete htmp;
  }
  if(hMC) hData->Add(hMC, -1.);
  for(int i = 0; i < hData->GetNbinsX(); ++i) {
    if(hData->GetBinContent(i+1) < 0.)
      hData->SetBinContent(i+1,0.);
  }
  
  hData->Scale(qcd_scale_);
  
  hData->SetTitle(Form("#scale[0.5]{#int} QCD = %.2e",  hData->Integral()));
  hData->SetLineColor(kOrange+6);
  hData->SetFillColorAlpha(kOrange+6,fill_alpha_);
  return hData;
}

THStack* DataPlotter::get_stack(TString hist, TString setType, Int_t set) {
  //make stacks less transparent
  double fill_alpha = fill_alpha_;
  fill_alpha_ = 0.9;
  vector<TH1F*> h;
  TH1F* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;

  Int_t color[] = {kRed+1, kRed-2, kYellow+1,kSpring-1 , kViolet-2, kGreen-2, kRed+3,kOrange-9,kBlue+1};
  Int_t fill[]  = {1001,3001,3005,1001,1001,3005,1001,1001,1001};
  THStack* hstack = new THStack(Form("%s",hist.Data()),Form("%s",hist.Data()));
  
  //for combining histograms of the same process
  map<TString, int> indexes;
  
  map<TString, int> colors; //index for the color array
  int n_colors = 0;
  if(debug_) 
      printf("get_stack: entry name label scale\n");
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(debug_) 
      printf("get_stack: %i %s %s %.4f\n",
	     i, names_[i].Data(), labels_[i].Data(), scale_[i]);

    if(isData_[i]) {h.push_back(NULL);continue;}
    if(isSignal_[i]) {h.push_back(NULL);continue;}
    
    h.push_back((TH1F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data())));
    if(!h[i]) continue;
    h[i] = (TH1F*) h[i]->Clone("tmp");
    h[i]->Scale(scale_[i]);
    if(rebinH_ > 0) h[i]->Rebin(rebinH_);
    int index = i;
    int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
    }
    TString name = labels_[index];
    if(index != (int) i)  {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
    
      h[index]->SetFillColorAlpha(color[i_color],fill_alpha_);
      h[index]->SetLineColor(color[i_color]);
      h[index]->SetLineWidth(2);
      h[index]->SetMarkerStyle(20);
      h[index]->SetName(Form("s_%s_%s",name.Data(),hist.Data()));    
    }
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", name.Data(), h[index]->Integral()));
  }
  auto it = indexes.begin();
  while(it != indexes.end()) {
    hstack->Add(h[it->second]);
    it++;
  }
  if(hQCD) hstack->Add(hQCD);
  fill_alpha_ = fill_alpha;
  return hstack;
}

TCanvas* DataPlotter::plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label) {

  TH2F* h = 0; //histogram

  Int_t color = kRed+1;

  TCanvas* c = new TCanvas(Form("h2d_%s_%i",hist.Data(),set),Form("h2D_%s_%i",hist.Data(),set), 1000, 700);

  //maximum z value of histograms, for plotting
  //  double m = 0.;


  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(label != labels_[i]) continue;
    
    //get histogram
    TH2F* htmp = (TH2F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!htmp) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
    htmp = (TH2F*) htmp->Clone("tmp");
    //scale to cross section and luminosity
    htmp->Scale(scale_[i]);
    if(rebinH_ > 1) {
      htmp->RebinX(rebinH_);
      htmp->RebinY(rebinH_);
    }
    if(h) {
      h->Add(htmp);
      delete htmp;
    }
    else h = htmp;
    
  }
  if(normalize_2ds_&&h->Integral() > 0.) {
    h->Scale(1./h->Integral());
    h->GetZaxis()->SetRangeUser(1.e-5,10.);
  }
  TH2F* data;
  if(plot_data_) {
    data = get_data_2D(hist, setType, set);
    gStyle->SetPalette(kGreyScale);
    if(normalize_2ds_ && data->Integral() > 0.) {
      data->Scale(1./data->Integral());
      data->GetZaxis()->SetRangeUser(1.e-5,10.);
    }
    data->Draw("col");
  }
  
  TH2F* hAxis = (plot_data_ && data) ? data : h;
  //FIXME should just be the integral plotted
  h->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", label.Data(), h->Integral()));
  h->SetLineColor(color);
  h->SetMarkerColor(color);
  h->SetMarkerStyle(6);
  h->SetName(Form("h2D_%s_%s",label.Data(),hist.Data()));    
  if(plot_data_) h->Draw("same");
  else           h->Draw();

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  c->SetGrid();
  c->SetTopMargin(0.06);
  c->SetRightMargin(0.05);
  c->SetLeftMargin(0.087);
  if(normalize_2ds_ == 0) c->BuildLegend();
  if(yMin_ <= yMax_)hAxis->GetYaxis()->SetRangeUser(yMin_,yMax_);
  if(xMin_ <= xMax_)hAxis->GetXaxis()->SetRangeUser(xMin_,xMax_);
  //draw text on plots
  draw_luminosity();
  draw_cms_label();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  if(plot_title_) hAxis->SetTitle (title.Data());
  else            hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  if(logZ_) c->SetLogz();
  return c;

}

TCanvas* DataPlotter::plot_2Dhist(TString hist, TString setType, Int_t set) {

  vector<TH2F*> h; //list of histograms
  //check if QCD is defined for this set
  //  TH2F* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;

  //array of colors and fills for each label
  Int_t color[] = {kRed+1, kYellow+1,kYellow+3,kBlue+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
  //  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};

  TCanvas* c = new TCanvas(Form("h2d_%s_%i",hist.Data(),set),Form("h2D_%s_%i",hist.Data(),set), 1000, 700);

  //maximum z value of histograms, for plotting
  //  double m = 0.;

  //for combining histograms of the same process
  map<TString, int> indexes;
  
  map<TString, int> colors; //map label to index for the color array
  int n_colors = 0; //number of colors used so far

  for(UInt_t i = 0; i < data_.size(); ++i) {
    //push null to not mess up indexing
    if(isData_[i]) {h.push_back(NULL);continue;}

    //get histogram
    TH2F* htmp = (TH2F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!htmp) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
    h.push_back((TH2F*) htmp->Clone("tmp"));
    //scale to cross section and luminosity
    h[i]->Scale(scale_[i]);

    if(rebinH_ > 1) {
      h[i]->RebinX(rebinH_);
      h[i]->RebinY(rebinH_);
    }
    //if the first, add to map, else get first of this label
    int index = i;
    int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
    }
    //if not first, add this to first histogram of this label
    TString name = labels_[index];
    if(index != (int)i) {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
      //set plotting info
      //      h[index]->SetFillStyle(fill [i_color]); 
      //      h[index]->SetFillColorAlpha(color[i_color],fill_alpha_);
      h[index]->SetLineColor(color[i_color]);
      h[index]->SetMarkerColor(color[i_color]);
      //      h[index]->SetLineWidth(2);
      //      h[index]->SetMarkerStyle(20);
      h[index]->SetName(Form("h2D_%s_%s",name.Data(),hist.Data()));    
    }
    //FIXME should just be the integral plotted
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", name.Data(), h[index]->Integral()));
    //    m = max(m,h[index]->GetMaximum());
  }
  //plot each histogram, remember which is first for axis setting
  bool first = true;
  int ind = 0;
  auto it = indexes.begin();
  while(it != indexes.end()) {
    if(normalize_2ds_ && h[it->second]->Integral() > 0.) h[it->second]->Scale(1./h[it->second]->Integral());
    if(first) {
      h[it->second]->Draw("");
      ind = it->second;
    }
    else
      h[it->second]->Draw("same");
    first = false;
    it++;
  }
  //plot QCD
  //  if(h.size() == 0 && hQCD) hQCD->Draw("hist");
  //  else if(hQCD) hQCD->Draw("hist same");

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  c->SetGrid();
  c->SetTopMargin(0.06);
  c->SetRightMargin(0.05);
  c->SetLeftMargin(0.087);
  c->BuildLegend();
  auto o = c->GetPrimitive("TPave");
  if(o) {
    auto tl = (TLegend*) o;
    tl->SetTextSize(0.04);
    tl->SetX1NDC(0.65);
    tl->SetX2NDC(0.9);
    tl->SetY2NDC(0.45);
    tl->SetY1NDC(0.9);
    tl->SetEntrySeparation(2.);
  }

  TH2F* hAxis = (h.size() > 0) ? h[ind] : 0;
  if(!hAxis) return NULL;
  
  if(yMin_ <= yMax_)hAxis->GetYaxis()->SetRangeUser(yMin_,yMax_);
  if(xMin_ <= xMax_)hAxis->GetXaxis()->SetRangeUser(xMin_,xMax_);
  //draw text on plots
  draw_luminosity();
  draw_cms_label();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  if(plot_title_) hAxis->SetTitle (title.Data());
  else hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  if(logZ_) c->SetLogz();
  return c;

}

TCanvas* DataPlotter::plot_hist(TString hist, TString setType, Int_t set) {

  vector<TH1F*> h; //list of histograms
  //check if QCD is defined for this set
  TH1F* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;

  //array of colors and fills for each label
  Int_t color[] = {kRed+1, kYellow+1,kYellow+3,kBlue+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};

  TCanvas* c = new TCanvas(Form("h_%s_%i",hist.Data(),set),Form("h_%s_%i",hist.Data(),set), 1000, 700);

  //maximum y value of histograms, for plotting
  double m = 0.;

  //for combining histograms of the same process
  map<TString, int> indexes;
  
  map<TString, int> colors; //map label to index for the color array
  int n_colors = 0; //number of colors used so far

  for(UInt_t i = 0; i < data_.size(); ++i) {
    //push null to not mess up indexing
    if(isData_[i]) {h.push_back(NULL);continue;}

    //get histogram
    h.push_back((TH1F*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data())));
    if(!h[i]) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
    h[i] = (TH1F*) h[i]->Clone("htmp");
    //scale to cross section and luminosity
    h[i]->Scale(scale_[i]);
    if(isSignal_[i] && signal_scale_ > 1.) h[i]->Scale(signal_scale_);
    if(rebinH_ > 0) h[i]->Rebin(rebinH_);

    //if the first, add to map, else get first of this label
    int index = i;
    int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
    }
    //if not first, add this to first histogram of this label
    TString name = labels_[index];
    if(index != (int)i) {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
      //set plotting info
      h[index]->SetFillStyle(fill [i_color]); 
      h[index]->SetFillColorAlpha(color[i_color],fill_alpha_);
      h[index]->SetLineColor(color[i_color]);
      h[index]->SetLineWidth(2);
      h[index]->SetMarkerStyle(20);
      h[index]->SetName(Form("h_%s_%s",name.Data(),hist.Data()));    
    }
    //FIXME should just be the integral plotted
    if(isSignal_[index])
      h[index]->SetTitle(Form("#scale[0.5]{#int} %s%s = %.2e", name.Data(),
			      (signal_scale_ == 1.) ? "" : Form(" (x%.1f)",signal_scale_),
			      h[index]->Integral()));
    else
      h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", name.Data(), h[index]->Integral()));
    m = max(m,h[index]->GetMaximum());
  }
  //plot each histogram, remember which is first for axis setting
  bool first = true;
  int ind = 0;
  auto it = indexes.begin();
  while(it != indexes.end()) {
    if(first) {
      h[it->second]->Draw("hist");
      ind = it->second;
    }
    else
      h[it->second]->Draw("same hist");
    first = false;
    it++;
  }
  //plot QCD
  if(h.size() == 0 && hQCD) hQCD->Draw("hist");
  else if(hQCD) hQCD->Draw("hist same");

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  c->SetGrid();
  c->SetTopMargin(0.06);
  c->SetRightMargin(0.05);
  c->SetLeftMargin(0.087);
  c->BuildLegend();
  auto o = c->GetPrimitive("TPave");
  if(o) {
    auto tl = (TLegend*) o;
    tl->SetTextSize(0.04);
    tl->SetX1NDC(0.65);
    tl->SetX2NDC(0.9);
    tl->SetY2NDC(0.45);
    tl->SetY1NDC(0.9);
    tl->SetEntrySeparation(2.);
  }

  TH1F* hAxis = (h.size() > 0) ? h[ind] : hQCD;
  if(!hAxis) return NULL;
  
  if(yMin_ <= yMax_)hAxis->SetAxisRange(yMin_,yMax_,"Y");
  else            hAxis->SetAxisRange(1e-1,m*1.2,"Y");    
  if(xMin_ < xMax_) hAxis->SetAxisRange(xMin_,xMax_,"X");    
  //draw text on plots
  draw_luminosity();
  draw_cms_label();
  hAxis->SetXTitle(xtitle.Data());
  if(plot_y_title_) hAxis->SetYTitle(ytitle.Data());
  if(plot_title_) hAxis->SetTitle (title.Data());
  else hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  return c;

}

TCanvas* DataPlotter::plot_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = new TCanvas(Form("s_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), 1000, 700);
  //split the top into main stack and bottom into Data/MC if plotting data
  TPad *pad1, *pad2;

  //get stack and data histogram
  THStack* hstack = get_stack(hist,setType,set);
  if(!hstack) {
    printf("Null stack returned!\n");
    return c;
  }
  TH1F* d = get_data(hist, setType, set);
  vector<TH1F*> hsignal = get_signal(hist,setType,set);

  if(plot_data_ && d) {
    pad1 = new TPad("pad1","pad1",0.0,0.30,1,1); //xL yL xH xH, (0,0) = bottom left
    pad2 = new TPad("pad2","pad2",0.0,0.02,1,0.30);

    pad1->SetTopMargin(0.06);
    pad2->SetTopMargin(0.0);
    pad1->SetBottomMargin(0.0);
    pad2->SetBottomMargin(0.22);
    pad1->Draw();
    pad2->Draw();
  } else {
    pad1 = new TPad("pad1","pad1",0.0,0.0,1,1); //xL yL xH xH, (0,0) = bottom left
    pad1->Draw();
  }
  pad1->cd();
  //store maximum of stack/Data
  double m = 0.;

  
  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  if(stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      if(hsignal[i]) hstack->Add(hsignal[i]);
    }
  }
  //draw stack, preserving style set for each histogram
  hstack->Draw("hist noclear");
  if(!stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      if(hsignal[i] && hsignal[i]->GetEntries() > 0)
	hsignal[i]->Draw("hist same");
    }
  }

  //draw the data with error bars
  if(plot_data_ && d) d->Draw("E same");
  
  m = max(hstack->GetMaximum(), (d) ? d->GetMaximum() : 0.);

  //Make a Data/MC histogram
  if(plot_data_ && !d) {
    printf("Warning! Data histogram is Null! Skipping Data/MC plot\n");
  }
  TH1F* hDataMC = (plot_data_ && d) ? (TH1F*) d->Clone("hDataMC") : 0;
  if(hDataMC) {
    hDataMC->Clear();
    for(int i = 1; i <= hDataMC->GetNbinsX(); ++i) {
      double dataVal = 0;
      double dataErr = 0;
      dataVal = d->GetBinContent(i);
      dataErr= d->GetBinError(i);
      dataErr = sqrt(dataVal);
      double mcVal = ((TH1F*) hstack->GetStack()->Last())->GetBinContent(i);
      double mcErr = ((TH1F*) hstack->GetHistogram())->GetBinError(i);
      if(dataVal == 0 || mcVal == 0) {hDataMC->SetBinContent(i,-1); continue;}
      double err = sqrt(mcErr*mcErr + dataErr*dataErr);
      double errRatio = (dataVal/mcVal)*(dataVal/mcVal)*(dataErr*dataErr/(dataVal*dataVal) + mcErr*mcErr/mcVal/mcVal);
      errRatio = sqrt(errRatio);
      double ratio = dataVal/mcVal;
      hDataMC->SetBinContent(i,ratio);
      hDataMC->SetBinError(i,errRatio); 
    }
  }
  //if not plotting data, never split so use the original canvas
  if(plot_data_) {
    pad1->BuildLegend();
    pad1->SetGrid();
    pad1->Update();
    auto o = pad1->GetPrimitive("TPave");
    if(o) {
      auto tl = (TLegend*) o;
      tl->SetTextSize(0.04);
      tl->SetY2NDC(0.45);
      tl->SetY1NDC(0.9);
      tl->SetX1NDC(0.6);
      tl->SetX2NDC(0.9);
      tl->SetEntrySeparation(2.);
      pad1->Update();
    }

  } else {
    c->BuildLegend();
    c->SetGrid();
    c->Update();
    auto o = c->GetPrimitive("TPave");
    if(o) {
      auto tl = (TLegend*) o;
      tl->SetTextSize(0.04);
      tl->SetX1NDC(0.6);
      tl->SetX2NDC(0.9);
      tl->SetY2NDC(0.45);
      tl->SetY1NDC(0.9);
      tl->SetEntrySeparation(2.);
    }
  }
  //draw text
  draw_luminosity();
  draw_cms_label();
  
  if(plot_data_ && hDataMC) hDataMC->GetXaxis()->SetTitle(xtitle.Data());
  else hstack->GetXaxis()->SetTitle(xtitle.Data());
  if(plot_y_title_) hstack->GetYaxis()->SetTitle(ytitle.Data());

  if(yMin_ < yMax_) hstack->GetYaxis()->SetRangeUser(yMin_,yMax_);    
  else              hstack->GetYaxis()->SetRangeUser(1.e-1,m*1.2);    
  if(plot_data_ && xMin_ < xMax_ && hDataMC) hDataMC->GetXaxis()->SetRangeUser(xMin_,xMax_);    
  if(xMin_ < xMax_) hstack->GetXaxis()->SetRangeUser(xMin_,xMax_);    

  if(plot_title_) hstack->SetTitle (title.Data());
  else hstack->SetTitle("");

  if(yMin_ < yMax_) {
    hstack->SetMinimum(yMin_);
    hstack->SetMaximum(yMax_);
  } else {
    hstack->SetMinimum(1.e-1);
    hstack->SetMaximum(1.2*m);
  }
  //Set Y-axis title size and offset
  hstack->GetYaxis()->SetTitleSize(0.045);
  hstack->GetYaxis()->SetTitleOffset(0.7);
  if(logY_) {
    if(plot_data_)pad1->SetLogy();
    else          c->SetLogy();
  }
  c->SetGrid();
  if(plot_data_ && hDataMC) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("E");
    TLine* line = new TLine(hDataMC->GetBinCenter(1)-hDataMC->GetBinWidth(1)/2., 1.,
			    hDataMC->GetBinCenter(hDataMC->GetNbinsX())+hDataMC->GetBinWidth(1)/2., 1.);
    line->SetLineColor(kRed);
    line->Draw("same");
    
    hDataMC->GetYaxis()->SetTitle("Data/MC");
    hDataMC->GetXaxis()->SetTitleSize(0.11);
    hDataMC->GetXaxis()->SetTitleOffset(0.8);
    hDataMC->GetXaxis()->SetLabelSize(0.08);
    hDataMC->GetYaxis()->SetTitleSize(0.1);
    hDataMC->GetYaxis()->SetTitleOffset(0.3);
    hDataMC->GetYaxis()->SetLabelSize(0.08);
    m = hDataMC->GetMaximum();
    double mn = hDataMC->GetMinimum();
    mn = max(0.2*mn,1e-2);
    m = 1.2*m;
    m = min(m, 5.0);
    hDataMC->GetYaxis()->SetRangeUser(mn,m);    
    hDataMC->SetMinimum(mn);
    hDataMC->SetMaximum(m);
    //  hDataMC->GetXaxis()->SetLabelOffset(0.5);
  
    hDataMC->SetMarkerStyle(20);
    //  hDataMC->SetName("hDataMC");
  }
  return c;

}


TCanvas* DataPlotter::print_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_stack(hist,setType,set);
  if(!c) return c;
  c->Print(Form("figures/%s/%s/stack_%s%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(),hist.Data(),
		(logY_ ? "_log":""),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_hist(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_hist(hist,setType,set);
  cout << "plotted hist" << endl;
  if(!c) return c;
  c->Print(Form("figures/%s/%s/hist_%s%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(),hist.Data(),
		(logY_ ? "_log":""),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_2Dhist(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_2Dhist(hist,setType,set);
  cout << "plotted 2D hist" << endl;
  if(!c) return c;
  c->Print(Form("figures/%s/%s/hist2D_%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(),hist.Data(),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_single_2Dhist(TString hist, TString setType, Int_t set, TString label) {
  TCanvas* c = plot_single_2Dhist(hist,setType,set,label);
  cout << "plotted 2D hist " << label.Data() << endl;
  if(!c) return c;
  c->Print(Form("figures/%s/%s/hist2D_%s_%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(),label.Data(),hist.Data(),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

Int_t DataPlotter::print_stacks(vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		   vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  for(UInt_t i = 0; i < hists.size(); ++i) {
    Int_t set = sets[i];
    TString hist(hists[i]);
    TString setType(setTypes[i]);
    xMax_ = xMaxs[i];
    xMin_ = xMins[i];
    rebinH_ = rebins[i];
    TCanvas* c = print_stack(hist,setType,set);
    Int_t status = (c) ? 0 : 1;
    printf("Printing Data/MC stack %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
  }
  
  return 0;
}

Int_t DataPlotter::print_hists(vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		   vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  for(UInt_t i = 0; i < hists.size(); ++i) {
    Int_t set = sets[i];
    TString hist(hists[i]);
    TString setType(setTypes[i]);
    xMax_ = xMaxs[i];
    xMin_ = xMins[i];
    rebinH_ = rebins[i];
    TCanvas* c = print_hist(hist,setType,set);
    Int_t status = (c) ? 0 : 1;
    printf("Printing Data/MC hist %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
  }
  
  return 0;
}


Int_t DataPlotter::init_files() {

  Int_t nFiles = fileNames_.size();
  vector<TFile*> f;
  for(int i = 0; i < nFiles; ++i) {
    f.push_back(new TFile(fileNames_[i].Data(),"READ"));
    if(f[i]) data_.push_back((TFile*) f[i]->Get("Data"));
    else {
      printf("File %s not found! Exiting\n", fileNames_[i].Data());
      return 1;
    }
  }
  for(int i = 0; i < nFiles; ++i) {
    if(isData_[i]) scale_.push_back(1.);
    else {
      TH1F* evts = (TH1F*)  f[i]->Get(Form("TotalEvents_%s",names_[i].Data()));
      scale_.push_back(1./(evts->GetBinContent(1)-2*evts->GetBinContent(10))*xsec_[i]*lum_);
      if(debug_ > 0) printf("%s: bin 1, bin 11: %.0f, %.0f\n", names_[i].Data(), evts->GetBinContent(1), evts->GetBinContent(10));
    }
  }

  gStyle->SetTitleW(0.8f);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasPreferGL(kTRUE);
  rnd_ = new TRandom(seed_);

  return 0;
}

Int_t DataPlotter::add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal) {
  fileNames_.push_back(filepath);
  names_.push_back(name);
  labels_.push_back(label);
  isData_.push_back(isData);
  if(!isData)
    xsec_.push_back(xsec);
  else
    xsec_.push_back(1.);
  isSignal_.push_back(isSignal);
  
  return 0;
}
