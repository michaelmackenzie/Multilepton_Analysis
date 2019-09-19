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
    *xtitle = "Pt / M_{#mu#mu}";
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
  else if(hist == "lepptoverm") {
    *xtitle = "Pt / Mass";
    *ytitle = Form("Events / %.2f",0.25*rebinH_);
    *title  = Form("Pt / Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
}

TH1F* DataPlotter::get_data(TString hist, TString setType, Int_t set) {
  TH1F* d = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(!isData_[i]) continue;
    TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    TH1F* tmp = (TH1F*) f->Get(hist.Data());
    if(!tmp) return NULL;
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

TH1F* DataPlotter::get_qcd(TString hist, TString setType, Int_t set) {
  Int_t set_qcd = set + qcd_offset_;

  TH1F* hData = get_data(hist, setType, set_qcd);
  if(!hData) return hData;
  hData->SetName(Form("qcd_%s",hist.Data()));      
  TH1F* hMC = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(isData_[i]) continue;
    TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set_qcd));
    if(!f) return NULL;
    TH1F* htmp = (TH1F*) f->Get(hist.Data())->Clone("tmp");
    if(!htmp) return NULL;
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
  
  hData->SetTitle(Form("#scale[0.5]{#int} QCD = %.2e",  hData->Integral()));
  hData->SetLineColor(kAzure+2);
  hData->SetFillColorAlpha(kAzure+2,fill_alpha_);
  return hData;
}

THStack* DataPlotter::get_stack(TString hist, TString setType, Int_t set) {
  //make stacks less transparent
  double fill_alpha = fill_alpha_;
  fill_alpha_ = 0.9;
  vector<TH1F*> h;
  TH1F* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;

  Int_t color[] = {kRed+1, kYellow+1,kSpring-1,kBlue+1 , kRed+3, kViolet-2, kGreen-2, kOrange-9};
  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};
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
    
    TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    h.push_back((TH1F*) f->Get(hist.Data())->Clone("tmp"));
    if(!h[i]) return NULL;
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

// TCanvas* DataPlotter::plot_2Dhist(TString hist, TString setType, Int_t set) {

//   TH2F* h[nFiles_];
//   for(int i = 0; i < nFiles_; ++i) h[i] = 0;

//   Int_t color[] = {kRed+1, kYellow+1,kGreen-5,kBlue+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
//   Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};
//   TCanvas* c = new TCanvas(Form("h2D_%s_%i",hist.Data(),set),Form("h2D_%s_%i",hist.Data(),set), 1000, 700);
//   double m = 0.;
//   //for combining histograms of the same process
//   Int_t i_zjet = -1;
//   Int_t i_wjet = -1;
//   Int_t i_diboson = -1;
//   Int_t i_t = -1;
//   Int_t i_higgs = -1;
//   Int_t i_htautau = -1;

//   Int_t i_color = -1; //index for the color array

//   Int_t first = 1;
//   Int_t ind = -1; // index of first histogram to set titles and axis ranges
//   for(int i = 0; i < nFiles_; ++i) {
//     if(process_[i] == 0) continue;

//     bool isZJet = (names_[i].Contains("z") && names_[i].Contains("jets"));
//     bool isWJet = (names_[i].Contains("w") && names_[i].Contains("jets"));
//     bool isDiboson = (names_[i].Contains("ww") || names_[i].Contains("wz") || names_[i].Contains("zz"));
//     bool isTop = (names_[i].Contains("t_tw") || names_[i].Contains("tbar_tw"));
//     isTop = isTop | names_[i].Contains("ttbar");
//     bool isHiggs = (names_[i].Contains("hzg"));
//     bool isHTauTau = (names_[i].Contains("htautau"));
//     if(isZJet+isDiboson+isTop+isHiggs+isHTauTau > 1)
//       printf("WARNING! In plot_hist: identified %s as several processes\n", names_[i].Data());

//     if(debug_ > 1) {
//       if(isZJet) printf("Found ZJets data %s\n", names_[i].Data());
//       if(isWJet) printf("Found WJets data %s\n", names_[i].Data());
//       if(isDiboson) printf("Found Diboson data %s\n", names_[i].Data());
//       if(isTop) printf("Found single Top data %s\n", names_[i].Data());
//       if(isHiggs) printf("Found Higgs data %s\n", names_[i].Data());
//       if(isHTauTau) printf("Found Higgs Tau Tau data %s\n", names_[i].Data());
//     }
    
//     TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set));
//     if(!f) {printf("No folder %s, returning NULL\n",Form("%s_%i",setType.Data(),set));return NULL;}
//     h[i] = (TH2F*) f->Get(hist.Data());
//     if(!h[i]) {printf("No hist %s, returning NULL\n",hist.Data());return NULL;}
//     h[i]->Scale(scale_[i]);
//     if(rebinH_ > 1) h[i]->RebinX(rebinH_);
//     if(rebinH_ > 1) h[i]->RebinY(rebinH_);
//     int index = i;

//     if(debug_ > 1) {
//       if((isHiggs || isZJet || isWJet || isTop || isDiboson || isHTauTau))
// 	printf("%s contributes %.1f to the integral\n", names_[i].Data(), h[i]->Integral());
//     }
    
//     if(isZJet) {
//       if(i_zjet < 0) i_zjet = i;
//       else {
// 	h[i_zjet]->Add(h[i]);
// 	index = i_zjet;
//       }
//     } else if(isWJet) {
//       if(i_wjet < 0) i_wjet = i;
//       else {
// 	h[i_wjet]->Add(h[i]);
// 	index = i_wjet;
//       }
//     } else if(isDiboson) {
//       if(i_diboson < 0) i_diboson = i;
//       else {
// 	h[i_diboson]->Add(h[i]);
// 	index = i_diboson;
//       }
//     } else if(isTop) {
//       if(i_t < 0) i_t = i;
//       else {
// 	h[i_t]->Add(h[i]);
// 	index = i_t;
//       }
//     } else if(isHiggs) {
//       if(i_higgs < 0) i_higgs = i;
//       else {
// 	h[i_higgs]->Add(h[i]);
// 	index = i_higgs;
//       }
//     } else if(isHTauTau) {
//       if(i_htautau < 0) i_htautau = i;
//       else {
// 	h[i_htautau]->Add(h[i]);
// 	index = i_htautau;
//       }
//     }
//     //new set so new color
//     if(index == i)
//       ++i_color;
    
//     // h[index]->SetFillStyle(fill [i_color]); 
//     // h[index]->SetFillColorAlpha(color[i_color],fill_alpha_);
//     h[index]->SetLineColor(color[i_color]);
//     h[index]->SetMarkerColor(color[i_color]);
//     // h[index]->SetLineWidth(2);
//     // h[index]->SetMarkerStyle(20);
//     TString name = names_[index];
//     if(isHTauTau) name = "H->#tau#tau";
//     if(isHiggs) name = "H->Z#gamma";
//     if(isZJet) name = "ZJets";
//     if(isWJet) name = "WJets";
//     if(isTop) name = "Top";
//     if(isDiboson) name = "Diboson";
//     h[index]->SetName(Form("%s_%s",name.Data(),hist.Data()));    
//     h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", name.Data(), h[index]->Integral()));
//     if(!(isHiggs || isZJet || isWJet || isTop || isDiboson || isHTauTau))
//       h[index]->Draw((first == 0) ? "same" : "");
//     ind = (first == 1) ? i:ind;
//     first = 0;
//     m = max(m,h[index]->GetMaximum());
//   }
//   if(i_zjet >= 0) {h[i_zjet]->Draw(first == 0 ? "same" : ""); ind = (first == 1) ? i_zjet:ind; first = 0;}
//   if(i_wjet >= 0) {h[i_wjet]->Draw(first == 0 ? "same" : ""); ind = (first == 1) ? i_wjet:ind; first = 0;}
//   if(i_diboson >= 0) {h[i_diboson]->Draw(first == 0 ? "same" : ""); ind = (first == 1) ? i_diboson:ind; first = 0;}
//   if(i_t >= 0) {h[i_t]->Draw(first == 0 ? "same" : ""); ind = (first == 1) ? i_t:ind; first = 0;}
//   if(i_higgs >= 0) {h[i_higgs]->Draw(first == 0 ? "same" : ""); ind = (first == 1) ? i_higgs:ind; first = 0;}
//   if(i_htautau >= 0) {h[i_htautau]->Draw(first == 0 ? "same" : ""); ind = (first == 1) ? i_htautau:ind; first = 0;}

//   TString xtitle;
//   TString ytitle;
//   TString title;
//   get_titles(hist,setType,&xtitle,&ytitle,&title);

//   c->SetGrid();
//   c->SetTopMargin(0.06);
//   c->SetRightMargin(0.05);
//   c->SetLeftMargin(0.087);
//   c->BuildLegend();
//   TH2F* hAxis = h[ind];
//   if(yMin <= yMax)hAxis->SetAxisRange(yMin,yMax,"Y");
//   // else            hAxis->SetAxisRange(1e-1,m*1.2,"Y");    
//   if(xMin < xMax) hAxis->SetAxisRange(xMin,xMax,"X");    
//   draw_luminosity();
//   draw_cms_label();
//   hAxis->SetXTitle(xtitle.Data());
//   hAxis->SetYTitle(ytitle.Data());
//   if(plot_title_) hAxis->SetTitle (title.Data());
//   else hAxis->SetTitle ("");
//   if(logY_) c->SetLogy();
//   if(logZ_) c->SetLogz();
//   return c;

// }

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

    //get histogram book
    TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) {printf("No folder %s, returning NULL\n",Form("%s_%i",setType.Data(),set));return NULL;}
    //get histogram
    h.push_back((TH1F*) ((TH1F*) f->Get(hist.Data()))->Clone("htmp"));
    if(!h[i]) {printf("No hist %s, returning NULL\n",hist.Data());return NULL;}
    //scale to cross section and luminosity
    h[i]->Scale(scale_[i]);
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
  TH1F* hAxis = (h.size() > 0) ? h[ind] : hQCD;
  if(!hAxis) return NULL;
  
  if(yMin_ <= yMax_)hAxis->SetAxisRange(yMin_,yMax_,"Y");
  else            hAxis->SetAxisRange(1e-1,m*1.2,"Y");    
  if(xMin_ < xMax_) hAxis->SetAxisRange(xMin_,xMax_,"X");    
  //draw text on plots
  draw_luminosity();
  draw_cms_label();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  if(plot_title_) hAxis->SetTitle (title.Data());
  else hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  return c;

}

TCanvas* DataPlotter::plot_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = new TCanvas(Form("s_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), 1000, 700);
  //split the top into main stack and bottom into Data/MC if plotting data
  TPad *pad1, *pad2;
  if(plot_data_) {
    pad1 = new TPad("pad1","pad1",0.0,0.30,1,1); //xL yL xH xH, (0,0) = bottom left
    pad2 = new TPad("pad2","pad2",0.0,0.02,1,0.30);

    pad1->SetTopMargin(0.06);
    pad2->SetTopMargin(0.0);
    pad1->SetBottomMargin(0.0);
    pad2->SetBottomMargin(0.22);
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
  }
  //store maximum of stack/Data
  double m = 0.;

  //get stack and data histogram
  THStack* hstack = get_stack(hist,setType,set);
  TH1F* d = get_data(hist, setType, set);

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  //draw stack, preserving style set for each histogram
  hstack->Draw("hist noclear");

  //draw the data with error bars
  if(plot_data_ && d) d->Draw("E same");
  
  m = max(hstack->GetMaximum(), (d) ? d->GetMaximum() : 0.);

  //Make a Data/MC histogram
  TH1F* hDataMC = (plot_data_) ? (TH1F*) d->Clone("hDataMC") : 0;
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
  } else {
    c->BuildLegend();
    c->SetGrid();
  }
  //draw text
  draw_luminosity();
  draw_cms_label();
  
  if(plot_data_ && hDataMC) hDataMC->GetXaxis()->SetTitle(xtitle.Data());
  else hstack->GetXaxis()->SetTitle(xtitle.Data());
  hstack->GetYaxis()->SetTitle(ytitle.Data());

  if(yMin_ < yMax_) hstack->GetYaxis()->SetRangeUser(yMin_,yMax_);    
  else              hstack->GetYaxis()->SetRangeUser(1.e-1,m*1.2);    
  if(plot_data_ && xMin_ < xMax_) hDataMC->GetXaxis()->SetRangeUser(xMin_,xMax_);    
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
  if(plot_data_) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("E");

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


Int_t DataPlotter::print_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_stack(hist,setType,set);
  if(!c) return -1;
  c->Print(Form("figures/ztautau/stack_%s%s_%s_set_%i.png",hist.Data(),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  delete c;
  return 0;
}

Int_t DataPlotter::print_hist(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_hist(hist,setType,set);
  cout << "plotted hist" << endl;
  if(!c) return -1;
  c->Print(Form("figures/ztautau/hist_%s%s_%s_set_%i.png",hist.Data(),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  delete c;
  return 0;
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
    Int_t status = print_stack(hist,setType,set);
    std::printf("Printing Data/MC stack %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
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
    Int_t status = print_hist(hist,setType,set);
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

Int_t DataPlotter::add_dataset(TString filepath, TString name, TString label, bool isData, double xsec) {
  fileNames_.push_back(filepath);
  names_.push_back(name);
  labels_.push_back(label);
  isData_.push_back(isData);
  if(!isData)
    xsec_.push_back(xsec);
  else
    xsec_.push_back(1.);
  return 0;
}
