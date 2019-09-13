#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "THStack.h"
#include "TCanvas.h"

Int_t nFiles_ = 50;
Int_t nDataFiles_ = 20;
TString selection_ = "mutau"; //selection category
Double_t scale_[50]; //scales for datasets
Int_t    process_[50]; //indicates which backgrounds to use
TString names_[50];
Double_t xsec_[50];
TFile* data_[50]; //background data files
TFile* dData_[20]; //data data files

Double_t lum_; //luminosity
Double_t rootS_ = 13.; //sqrt(S)
Int_t seed_ = 90; //random number generator seed
TRandom* rnd_;

Double_t xMin = 1e6;
Double_t xMax = -1e6;
Double_t yMin = 1e6; //for 2D plots
Double_t yMax = -1e6; //for 2D plots
Int_t logZ_ = 0;
Int_t plotData_ = 1;
Int_t rebinH_ = 1;
Int_t data_over_mc_ = 1;
Int_t debug_ = 0;

void get_titles(TString hist, TString setType, TString* xtitle, TString* ytitle, TString* title) {
  if(hist == "lepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH_);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH_);
    *title  = Form("#DeltaR vs #Delta#phi Between Leptons %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "sysm" && setType == "event") {
    *xtitle = "Mass (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",5.*rebinH_);
    *title  = Form("Mass of the Jet System + Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "met") {
    *xtitle = "Missing Transverse Energy (GeV)";
    *ytitle = Form("Events / %.0f GeV",2.*rebinH_);
    *title  = Form("Missing Transverse Energy %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepptoverm") {
    *xtitle = "Pt / M_{#mu#mu}";
    *ytitle = Form("Events / %.1f GeV/c^{2}",0.2*rebinH_);
    *title  = Form("Pt Over Mass of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepm") {
    *xtitle = "M_{#mu#mu} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",5.*rebinH_);
    *title  = Form("Mass of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "leppt") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",5.*rebinH_);
    *title  = Form("Pt of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onept") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Pt of the Leading Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twopt") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Pt of the Trailing Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "oneiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH_);
    *title  = Form("Isolation of the Leading Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twoiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH_);
    *title  = Form("Isolation of the Trailing Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdeltar") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#Delta R of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepeta") {
    *xtitle = "#eta";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#eta of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepptoverm") {
    *xtitle = "Pt / Mass";
    *ytitle = Form("Events / %.2f",0.25*rebinH_);
    *title  = Form("Pt / Mass of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
}

THStack* get_stack(TString hist, TString setType, Int_t set) {

  TH1F* h[nFiles_];
  for(int i = 0; i < nFiles_; ++i) h[i] = 0;

  Int_t color[] = { kYellow+1,kYellow+3,kBlue+1, kRed+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};
  THStack* hstack = new THStack(Form("%s",hist.Data()),Form("%s",hist.Data()));

  //for combining histograms of the same process
  Int_t i_zjet = -1;
  Int_t i_wjet = -1;
  Int_t i_diboson = -1;
  Int_t i_t = -1;
  Int_t i_higgs = -1;

  Int_t i_color = -1; //index for the color array
  
  for(int i = 0; i < nFiles_; ++i) {
    if(process_[i] == 0) continue;
    
    bool isZJet = (names_[i].Contains("z") && names_[i].Contains("jets"));
    bool isWJet = (names_[i].Contains("w") && names_[i].Contains("jets"));
    bool isDiboson = (names_[i].Contains("ww") || names_[i].Contains("wz") || names_[i].Contains("zz"));
    bool isT = (names_[i].Contains("t_tw") || names_[i].Contains("tbar_tw"));
    bool isHiggs = (names_[i].Contains("hzg"));
    if(isZJet+isDiboson+isT+isHiggs > 1)
      printf("WARNING! In get_stack: identified %s as several processes\n", names_[i].Data());
    
    TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    h[i] = (TH1F*) f->Get(hist.Data())->Clone("tmp");
    if(!h[i]) return NULL;
    h[i]->Scale(scale_[i]);
    if(rebinH_ > 0) h[i]->Rebin(rebinH_);
    int index = i;
    if(isZJet) {
      if(i_zjet < 0) i_zjet = i;
      else {
	h[i_zjet]->Add(h[i]);
	index = i_zjet;
      }
    } else if(isWJet) {
      if(i_wjet < 0) i_wjet = i;
      else {
	h[i_wjet]->Add(h[i]);
	index = i_wjet;
      }
    } else if(isDiboson) {
      if(i_diboson < 0) i_diboson = i;
      else {
	h[i_diboson]->Add(h[i]);
	index = i_diboson;
      }
    } else if(isT) {
      if(i_t < 0) i_t = i;
      else {
	h[i_t]->Add(h[i]);
	index = i_t;
      }
    } else if(isHiggs) {
      if(i_higgs < 0) i_higgs = i;
      else {
	h[i_higgs]->Add(h[i]);
	index = i_higgs;
      }
    }
    //new set so new color
    if(index == i)
      ++i_color;
    //    h[index]->SetFillStyle(fill[index]); 
    h[index]->SetFillColorAlpha(color[i_color],0.5);
    h[index]->SetLineColorAlpha(color[i_color],0.5);
    h[index]->SetLineWidth(2);
    h[index]->SetMarkerStyle(20);
    TString name = names_[index];
    if(isHiggs) name = "Higgs";
    if(isZJet) name = "ZJets";
    if(isWJet) name = "WJets";
    if(isT) name = "T";
    if(isDiboson) name = "Diboson";
    h[index]->SetName(Form("%s_%s",name.Data(),hist.Data()));    
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", name.Data(), h[index]->Integral()));
    if(!(isHiggs || isZJet || isWJet || isT || isDiboson))
      hstack->Add(h[index]);
  }
  if(i_zjet > 0) hstack->Add(h[i_zjet]);
  if(i_wjet > 0) hstack->Add(h[i_wjet]);
  if(i_diboson > 0) hstack->Add(h[i_diboson]);
  if(i_t > 0) hstack->Add(h[i_t]);
  if(i_higgs > 0) hstack->Add(h[i_higgs]);

  return hstack;
}

TH1F* get_data(TString hist, TString setType, Int_t set) {
  TH1F* d = 0;
  for(int i = 0; i < sizeof(dData_)/sizeof(*dData_); ++i) {
    if(!dData_[i]) break;
    TFile* f = (TFile*) dData_[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    TH1F* tmp = (TH1F*) f->Get(hist.Data());
    if(!tmp) return NULL;
    if(!d) d = tmp;
    else d->Add(tmp);
  }
  if(!d) return NULL;
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  d->SetTitle("Data");
  d->SetName("Data");
  if(rebinH_ > 0) d->Rebin(rebinH_);

  return d;
}

TCanvas* plot_hist(TString hist, TString setType, Int_t set) {

  TH1F* h[nFiles_];
  for(int i = 0; i < nFiles_; ++i) h[i] = 0;

  Int_t color[] = { kYellow+1,kYellow+3,kBlue+1, kRed+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};
  TCanvas* c = new TCanvas(Form("h_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), 1000, 700);
  double m = 0.;
  //for combining histograms of the same process
  Int_t i_zjet = -1;
  Int_t i_wjet = -1;
  Int_t i_diboson = -1;
  Int_t i_t = -1;
  Int_t i_higgs = -1;

  Int_t i_color = -1; //index for the color array


  Int_t first = 1;
  Int_t ind = -1; // index of first histogram to set titles and axis ranges
  for(int i = 0; i < nFiles_; ++i) {
    if(process_[i] == 0) continue;

    bool isZJet = (names_[i].Contains("z") && names_[i].Contains("jets"));
    bool isWJet = (names_[i].Contains("w") && names_[i].Contains("jets"));
    bool isDiboson = (names_[i].Contains("ww") || names_[i].Contains("wz") || names_[i].Contains("zz"));
    bool isT = (names_[i].Contains("t_tw") || names_[i].Contains("tbar_tw"));
    bool isHiggs = (names_[i].Contains("hzg"));
    if(isZJet+isDiboson+isT+isHiggs > 1)
      printf("WARNING! In plot_hist: identified %s as several processes\n", names_[i].Data());

    if(debug_ > 1) {
      if(isZJet) printf("Found ZJets data %s\n", names_[i].Data());
      if(isWJet) printf("Found WJets data %s\n", names_[i].Data());
      if(isDiboson) printf("Found Diboson data %s\n", names_[i].Data());
      if(isT) printf("Found single Top data %s\n", names_[i].Data());
      if(isHiggs) printf("Found Higgs data %s\n", names_[i].Data());
    }
    
    TFile* f = (TFile*) data_[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) {printf("No folder %s, returning NULL\n",Form("%s_%i",setType.Data(),set));return NULL;}
    h[i] = (TH1F*) f->Get(hist.Data());
    if(!h[i]) {printf("No hist %s, returning NULL\n",hist.Data());return NULL;}
    h[i]->Scale(scale_[i]);
    int index = i;
    if(rebinH_ > 0) h[i]->Rebin(rebinH_);

    if(debug_ > 1) {
      if((isHiggs || isZJet || isWJet || isT || isDiboson))
	printf("%s contributes %.1f to the integral\n", names_[i].Data(), h[i]->Integral());
    }
    
    if(isZJet) {
      if(i_zjet < 0) i_zjet = i;
      else {
	h[i_zjet]->Add(h[i]);
	index = i_zjet;
      }
    } else if(isWJet) {
      if(i_wjet < 0) i_wjet = i;
      else {
	h[i_wjet]->Add(h[i]);
	index = i_wjet;
      }
    } else if(isDiboson) {
      if(i_diboson < 0) i_diboson = i;
      else {
	h[i_diboson]->Add(h[i]);
	index = i_diboson;
      }
    } else if(isT) {
      if(i_t < 0) i_t = i;
      else {
	h[i_t]->Add(h[i]);
	index = i_t;
      }
    } else if(isHiggs) {
      if(i_higgs < 0) i_higgs = i;
      else {
	h[i_higgs]->Add(h[i]);
	index = i_higgs;
      }
    }
    //new set so new color
    if(index == i)
      ++i_color;
    
    h[index]->SetFillStyle(fill [i_color]); 
    h[index]->SetFillColorAlpha(color[i_color],0.3);
    h[index]->SetLineColor(color[i_color]);
    h[index]->SetLineWidth(2);
    h[index]->SetMarkerStyle(20);
    TString name = names_[index];
    if(isHiggs) name = "Higgs";
    if(isZJet) name = "ZJets";
    if(isWJet) name = "WJets";
    if(isT) name = "T";
    if(isDiboson) name = "Diboson";
    h[index]->SetName(Form("%s_%s",name.Data(),hist.Data()));    
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", name.Data(), h[index]->Integral()));
    if(!(isHiggs || isZJet || isWJet || isT || isDiboson))
      h[index]->Draw((first == 0) ? "hist same" : "hist");
    ind = (first == 1) ? i:ind;
    first = 0;
    m = max(m,h[index]->GetMaximum());
  }
  if(i_zjet > 0) {h[i_zjet]->Draw(first == 0 ? "hist same" : "hist"); ind = (first == 1) ? i_zjet:ind; first = 0;}
  if(i_wjet > 0) {h[i_wjet]->Draw(first == 0 ? "hist same" : "hist"); ind = (first == 1) ? i_wjet:ind; first = 0;}
  if(i_diboson > 0) {h[i_diboson]->Draw(first == 0 ? "hist same" : "hist"); ind = (first == 1) ? i_diboson:ind; first = 0;}
  if(i_t > 0) {h[i_t]->Draw(first == 0 ? "hist same" : "hist"); ind = (first == 1) ? i_t:ind; first = 0;}
  if(i_higgs > 0) {h[i_higgs]->Draw(first == 0 ? "hist same" : "hist"); ind = (first == 1) ? i_higgs:ind; first = 0;}

  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  TH1F* hAxis = h[ind];
  hAxis->SetAxisRange(1e-1,m*1.2,"Y");    
  if(xMin < xMax) hAxis->SetAxisRange(xMin,xMax,"X");    
  c->BuildLegend();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  hAxis->SetTitle (title.Data());
  c->SetGrid();
  return c;

}

TCanvas* plot_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = new TCanvas(Form("s_%s_%i",hist.Data(),set),Form("h_%s_%i",hist.Data(),set), 1000, 700);
  TPad* pad1 = new TPad("pad1","pad1",0.0,0.30,1,1); //xL yL xH xH, (0,0) = bottom left
  TPad* pad2 = new TPad("pad2","pad2",0.0,0.02,1,0.30);

  pad1->SetTopMargin(0.06);
  pad2->SetTopMargin(0.0);
  pad1->SetBottomMargin(0.0);
  if(plotData_ == 0) pad1->SetBottomMargin(0.04); //make x-axis visible
  pad2->SetBottomMargin(0.22);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  double m = 0.;

  THStack* hstack = get_stack(hist,setType,set);
  TH1F* d = get_data(hist, setType, set);

  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  
  hstack->Draw("hist noclear");
  if(plotData_ && d) d->Draw("E same");
  
  m = max(hstack->GetMaximum(), (d) ? d->GetMaximum() : 0);

  TH1F* hDataMC = (plotData_) ? (TH1F*) d->Clone("hDataMC") : 0;
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
      double z = (dataVal-mcVal)/err;
      double ratio = dataVal/mcVal;
      if(data_over_mc_ == 0) {
	hDataMC->SetBinContent(i,z);
	hDataMC->SetBinError(i,1.);
      } else {
	hDataMC->SetBinContent(i,ratio);
	hDataMC->SetBinError(i,errRatio);
      }
    }
    
  }
  pad1->BuildLegend();
  pad1->SetGrid();
  if(plotData_) hDataMC->GetXaxis()->SetTitle(xtitle.Data());
  hstack->GetYaxis()->SetTitle(ytitle.Data());
  if(plotData_ && xMin < xMax) hDataMC->GetXaxis()->SetRangeUser(xMin,xMax);    
  if(xMin < xMax) hstack->GetXaxis()->SetRangeUser(xMin,xMax);    
  hstack->GetYaxis()->SetRangeUser(1.e-1,m*1.5);    
  hstack->SetTitle (title.Data());
  hstack->SetMinimum(1.e-1);
  hstack->SetMaximum(1.2*m);
  hstack->GetYaxis()->SetTitleSize(0.045);
  hstack->GetYaxis()->SetTitleOffset(0.7);
  c->SetGrid();
  if(plotData_) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("P");

    hDataMC->GetYaxis()->SetTitle((data_over_mc_ == 0) ? "(Data - MC)/#sigma" : "Data/MC");
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
    hDataMC->GetYaxis()->SetRangeUser(mn,m);    
    //  hDataMC->GetXaxis()->SetLabelOffset(0.5);
  
    hDataMC->SetMarkerStyle(20);
    //  hDataMC->SetName("hDataMC");
  }
  return c;

}


Int_t print_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_stack(hist,setType,set);
  if(!c) return -1;
  c->Print(Form("figures/ztautau/stack_%s%s_%s_set_%i.png",hist.Data(),
		((dData_[0] != NULL) ? "_data":""),"dataOverMC",set));
  delete c;
  return 0;
}

Int_t print_hist(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_hist(hist,setType,set);
  cout << "plotted hist" << endl;
  if(!c) return -1;
  c->Print(Form("figures/ztautau/hist_%s%s_%s_set_%i.png",hist.Data(),
		((dData_[0] != NULL) ? "_data":""),"dataOverMC",set));
  delete c;
  return 0;
}

Int_t print_stacks(vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		   vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  for(Int_t i = 0; i < hists.size(); ++i) {
    Int_t set = sets[i];
    TString hist(hists[i]);
    TString setType(setTypes[i]);
    xMax = xMaxs[i];
    xMin = xMins[i];
    rebinH_ = rebins[i];
    Int_t status = print_stack(hist,setType,set);
    std::printf("Printing Data/MC stack %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
  }
  
  return 0;
}

Int_t print_hists(vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		   vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  for(Int_t i = 0; i < hists.size(); ++i) {
    Int_t set = sets[i];
    TString hist(hists[i]);
    TString setType(setTypes[i]);
    xMax = xMaxs[i];
    xMin = xMins[i];
    rebinH_ = rebins[i];
    Int_t status = print_hist(hist,setType,set);
    std::printf("Printing Data/MC hist %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
  }
  
  return 0;
}


Int_t init_files() {

  names_[0]  = "ttbar_inclusive"         ;
  names_[1]  = "zjets_m-50_amcatnlo"     ;
  names_[2]  = "zjets_m-10to50_amcatnlo" ;
  names_[3]  = "t_tw"                    ;
  names_[4]  = "tbar_tw"                 ;
  names_[5]  = "zjets_m-50"              ;
  names_[6]  = "zjets_m-10to50"          ;
  names_[7]  = "z1jets_m-50"             ;
  names_[8]  = "z1jets_m-10to50"         ;
  names_[9]  = "z2jets_m-50"             ;
  names_[10] = "z2jets_m-10to50"         ;
  names_[11] = "z3jets_m-50"             ;
  names_[12] = "z3jets_m-10to50"         ;
  names_[13] = "z4jets_m-50"             ;
  names_[14] = "z4jets_m-10to50"         ;
  names_[15] = "w1jets"                  ;
  names_[16] = "w2jets"                  ;
  names_[17] = "w3jets"                  ;
  names_[18] = "w4jets"                  ;
  names_[19] = "ww"                      ;
  names_[20] = "wz_2l2q"                 ;
  names_[21] = "wz_3lnu"                 ;
  names_[22] = "zz_2l2nu"                ;
  names_[23] = "zz_2l2q"                 ;
  names_[24] = "zz_4l"                   ;
  names_[25] = "hzg_gluglu"              ;
  names_[26] = "hzg_tth"                 ;
  names_[27] = "hzg_vbf"                 ;
  names_[28] = "hzg_wminus"              ;
  names_[29] = "hzg_wplus"               ;
  names_[30] = "hzg_zh"                  ;

     
  xsec_[0]  =    831.76;               //"ttbar_inclusive"         
  xsec_[1]  =    5765.4;	       //"zjets_m-50_amcatnlo"     
  xsec_[2]  =    18610.;	       //"zjets_m-10to50_amcatnlo" 
  xsec_[3]  =    35.85;	               //"t_tw"                    
  xsec_[4]  =    35.85;	               //"tbar_tw"                 
  xsec_[5]  =    6803.2;	       //"zjets_m-50"              
  xsec_[6]  =    21959.8;	       //"zjets_m-10to50"          
  xsec_[7]  =    1198.9;	       //"z1jets_m-50"             
  xsec_[8]  =    855.5;		       //"z1jets_m-10to50"         
  xsec_[9]  =    390.6;		       //"z2jets_m-50"             
  xsec_[10] =    466.1;		       //"z2jets_m-10to50"         
  xsec_[11] =    113.3;		       //"z3jets_m-50"             
  xsec_[12] =    114.5;		       //"z3jets_m-10to50"         
  xsec_[13] =    60.2;		       //"z4jets_m-50"             
  xsec_[14] =    36.4;		       //"z4jets_m-10to50"         
  xsec_[15] =    9493.;		       //"w1jets"                  
  xsec_[16] =    3120.;		       //"w2jets"                  
  xsec_[17] =    942.3;		       //"w3jets"                  
  xsec_[18] =    524.1;		       //"w4jets"                  
  xsec_[19] =    12.178;	       //"ww"                      
  xsec_[20] =    5.595;		       //"wz_2l2q"                 
  xsec_[21] =    4.42965;	       //"wz_3lnu"                 
  xsec_[22] =    0.564;		       //"zz_2l2nu"                
  xsec_[23] =    3.22;		       //"zz_2l2q"                 
  xsec_[24] =    1.212;		       //"zz_4l"
  //Higgs branching ratios: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR2014
  //Higgs production xsecs: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt1314TeV2014
  //Z decay to leptons fraction: http://pdg.lbl.gov/2012/listings/rpp2012-list-z-boson.pdf
  xsec_[25] =    1.54e-3* 3.3658/100.*43.92;		       //"hzg_gluglu"              
  xsec_[26] =    1.54e-3* 3.3658/100.*0.5085;	       //"hzg_tth"                 
  xsec_[27] =    1.54e-3* 3.3658/100.*3.748;		       //"hzg_vbf"                 
  xsec_[28] =    1.54e-3* 3.3658/100.*1.380/2.;	       //"hzg_wminus"              
  xsec_[29] =    1.54e-3* 3.3658/100.*1.380/2.;	       //"hzg_wplus"               
  xsec_[30] =    1.54e-3* 3.3658/100.*0.8696;	       //"hzg_zh"                  

  for(int i = 0; i < sizeof(process_)/sizeof(*process_); ++i)
    process_[i]=0;

  process_[0]  = 1; //"ttbar_inclusive"         
  process_[1]  = 1; //"zjets_m-50_amcatnlo"     
  process_[2]  = 1; //"zjets_m-10to50_amcatnlo" 
  process_[3]  = 1; //"t_tw"                    
  process_[4]  = 1; //"tbar_tw"                 
  process_[5]  = 0; //"zjets_m-50"              
  process_[6]  = 0; //"zjets_m-10to50"          
  process_[7]  = 0; //"z1jets_m-50"             
  process_[8]  = 0; //"z1jets_m-10to50"         
  process_[9]  = 0; //"z2jets_m-50"             
  process_[10] = 0; //"z2jets_m-10to50"         
  process_[11] = 0; //"z3jets_m-50"             
  process_[12] = 0; //"z3jets_m-10to50"         
  process_[13] = 0; //"z4jets_m-50"             
  process_[14] = 0; //"z4jets_m-10to50"         
  process_[15] = 1; //"w1jets"                  
  process_[16] = 1; //"w2jets"                  
  process_[17] = 1; //"w3jets"                  
  process_[18] = 1; //"w4jets"                  
  process_[19] = 1; //"ww"                      
  process_[20] = 1; //"wz_2l2q"                 
  process_[21] = 1; //"wz_3lnu"                 
  process_[22] = 1; //"zz_2l2nu"                
  process_[23] = 1; //"zz_2l2q"                 
  process_[24] = 1; //"zz_4l"                   
  process_[25] = 1; //"hzg_gluglu"              
  process_[26] = 1; //"hzg_tth"                 
  process_[27] = 1; //"hzg_vbf"                 
  process_[28] = 0; //"hzg_wminus"              
  process_[29] = 0; //"hzg_wplus"               
  process_[30] = 1; //"hzg_zh"                  

  vector<char*> files;
  for(int i = 0; i < nFiles_; ++i) {
    if(process_[i])
      files.push_back(Form("ztautau/ztautau_%s_bltTree_%s.hist",selection_.Data(),names_[i].Data()));
    else
      files.push_back(Form(""));
  }

  const char* dNames[] = {
  			 "muon_2016B"                 };
  
  // const char* dFiles[] = {Form("tree_%s.hist",dNames[0])};
  vector<TString> dFiles;
  for(int i = 0; i < sizeof(dNames)/sizeof(*dNames); ++i)
    dFiles.push_back(Form("ztautau/ztautau_%s_bltTree_%s.hist",selection_.Data(),dNames[i]));

  const Double_t dLum[] = {5.3e3
                          };//, (36.4e3-5.3e3)}; //taken from CMS AN-16-458

  lum_    = accumulate(begin(dLum), end(dLum), 0, plus<double>());; //pb^-1

  

  
  TFile* f[nFiles_];
  for(int i = 0; i < nFiles_; ++i) {
    if(process_[i]) {
      f[i] = new TFile(files[i],"READ");
      // printf("Opened %s\n", files[i]);
    }
  }

  for(int i = 0; i < nFiles_; ++i) {
    if(process_[i]) {
      data_[i]  = (TFile*) f[i]->Get("Data");
      // printf("Retrieved Data file from %s\n", files[i]);
    }
  }

  TFile* dF[dFiles.size()];
  for(int i = 0; i < dFiles.size(); ++i) {
    dF[i] = new TFile(dFiles[i],"READ");
  }

  for(int i = 0; i < dFiles.size(); ++i) {
    dData_[i]  = (TFile*) dF[i]->Get("Data");
  }

  TH1F* evts[nFiles_];
  for(int i = 0; i < nFiles_; ++i) {
    if(process_[i]) {
      evts[i] = (TH1F*)  f[i]->Get(Form("TotalEvents_%s",names_[i].Data()));
      scale_[i]  = 1./(evts[i]->GetBinContent(1)-2*evts[i]->GetBinContent(11))*xsec_[i]*lum_;
    }
  }

  gStyle->SetTitleW(0.8f);
  gStyle->SetOptStat(0);
  gStyle->SetCanvasPreferGL(kTRUE);
  rnd_ = new TRandom(seed_);

  return 0;
}
