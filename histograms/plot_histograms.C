#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "THStack.h"
#include "TCanvas.h"

Double_t scale[20]; //scales for backgrounds
Double_t scalePt[20]; //scales for backgrounds for Pt study
Int_t    bkgds_[20]; //indicates which backgrounds to use
Int_t    sgnls_[10]; //indicates which signal to use
Double_t sScale[10]; //scales for signal
Double_t sScalePt[10]; //scales for signal for Pt study
Double_t sXsec[10];
TFile* data[20]; //background data files
TFile* dataPt[20]; //background data files for Pt study
TFile* dData[10]; //data data files
TFile* sData[10]; //signal data files
TFile* sDataPt[10]; //signal data files for Pt study

Double_t lum_;
Double_t rootS_ = 13.;
Int_t seed_ = 90;
TRandom* rnd_;
Int_t signal_mock_data_ = 0;
Int_t doPtStudy_ = 0; //switch to use pt study data sets

Double_t xMin = 1e6;
Double_t xMax = -1e6;
Double_t yMin = 1e6; //for 2D plots
Double_t yMax = -1e6; //for 2D plots
Int_t logZ_ = 0;
Int_t plotSignal = 1;
Int_t plotData_ = 1;
Int_t plotBkgd_ = 1;
Int_t rebinH = 1;
Int_t set12BkgdOrder_ = 2;
Int_t set5BkgdOrder_  = 3;
Int_t scaleErrors_ = 1;
Int_t data_over_mc_ = 1;
Int_t dataMC_vs_signalBkgd_ = 1; // 1 = mini plot is data/MC
Int_t debug_ = 0;

Fitter* fitter_ = new Fitter();

void get_titles(TString hist, TString setType, TString* xtitle, TString* ytitle, TString* title) {
  if(hist == "jetslepdelr") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta R Between Jet System and Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetslepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH);
    *title  = Form("#DeltaR vs #Delta#phi Between Jet System and Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetsdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH);
    *title  = Form("#DeltaR vs #Delta#phi Between Jets %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH);
    *title  = Form("#DeltaR vs #Delta#phi Between Leptons %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetlepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH);
    *title  = Form("#DeltaR vs #Delta#phi Between Jet and Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "bjetlepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH);
    *title  = Form("#DeltaR vs #Delta#phi Between b-Jet and Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "bjetlepmvslepm") {
    *xtitle = Form("M_{#mu#mub} / %.0f GeV",5.*rebinH);
    *ytitle = Form("M_{#mu#mu} / %.0f GeV",2.*rebinH);
    *title  = Form("M_{#mu#mu} vs M_{#mu#mub} %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "sysm" && setType == "event") {
    *xtitle = "Mass (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",5.*rebinH);
    *title  = Form("Mass of the Jet System + Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "met") {
    *xtitle = "Missing Transverse Energy (GeV)";
    *ytitle = Form("Events / %.0f GeV",2.*rebinH);
    *title  = Form("Missing Transverse Energy %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepptoverm") {
    *xtitle = "Pt / M_{#mu#mu}";
    *ytitle = Form("Events / %.1f GeV/c^{2}",0.2*rebinH);
    *title  = Form("Pt Over Mass of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepm") {
    *xtitle = "M_{#mu#mu} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",5.*rebinH);
    *title  = Form("Mass of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "leppt") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",5.*rebinH);
    *title  = Form("Pt of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onept") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH);
    *title  = Form("Pt of the Leading Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twopt") {
    *xtitle = "Pt (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH);
    *title  = Form("Pt of the Trailing Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "oneiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH);
    *title  = Form("Isolation of the Leading Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twoiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH);
    *title  = Form("Isolation of the Trailing Lepton %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdeltar") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta R of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepeta") {
    *xtitle = "#eta";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#eta of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "bjetlepm") {
    *xtitle = "M_{#mu#mub} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",5.*rebinH);
    *title  = Form("Mass of the Lepton + b Jet System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepptoverm") {
    *xtitle = "Pt / Mass";
    *ytitle = Form("Events / %.2f",0.25*rebinH);
    *title  = Form("Pt / Mass of the Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetslepdelphi") {
    *xtitle = "#Delta#phi";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta#phi of the Jets vs Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetlepdelphi") {
    *xtitle = "#Delta#phi";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta#phi of the Untagged Jet vs Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "bjetlepdelphi") {
    *xtitle = "#Delta#phi";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta#phi of the b-Jet vs Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetslepdelr") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta R of the Jets vs Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "jetlepdelr") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta R of the Untagged Jet vs Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "bjetlepdelr") {
    *xtitle = "#Delta R";
    *ytitle = Form("Events / %.1f",0.1*rebinH);
    *title  = Form("#Delta R of the b-Jet vs Lepton System %.1f fb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
}

THStack* get_stack(TString hist, TString setType, Int_t set) {

  TH1F* h[20];
  for(int i = 0; i < 20; ++i) h[i] = 0;
  Int_t color[] = { kYellow+1,kYellow+3,kBlue+1, kRed+1, kRed+3};
  Int_t fill[]  = {1001,3005,1001,1001,3005};
  TString names[] = {"t","tbar","ttbar","zjets","zjets_m10to50"};
  for(int i = 0; i < 20; ++i) {
    if(bkgds_[i] == 0) continue;
    TFile* f = (doPtStudy_ == 0) ? (TFile*) data[i]->Get(Form("%s_%i",setType.Data(),set)) :
      (TFile*) dataPt[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    h[i] = (TH1F*) f->Get(hist.Data())->Clone("tmp");
    if(!h[i]) return NULL;
    h[i]->Scale((doPtStudy_ == 0) ? scale[i] : scalePt[i]);
    if(rebinH > 0) h[i]->Rebin(rebinH);
    int index = i;
    if(i > 3) { //zjets
      index = 3;
      if(h[3] == 0) {
	h[3] = h[i];
	h[3]->SetName(Form("%s_%s",names[3].Data(),hist.Data()));
      }
      else h[3]->Add(h[i]);
    }
    
    //    h[index]->SetFillStyle(fill[index]); 
    h[index]->SetFillColorAlpha(color[index],0.2);
    h[index]->SetLineColorAlpha(color[index],0.2);
    h[index]->SetLineWidth(2);
    h[index]->SetMarkerStyle(20);
    if(index < 3) h[index]->SetName(Form("%s_%s",names[index].Data(),hist.Data()));
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", names[index].Data(), h[index]->Integral()));
  }
  THStack* hstack = new THStack(Form("%s",hist.Data()),Form("%s",hist.Data()));

  h[0]->Add(h[1]); // t + tbar
  hstack->Add(h[0]);
  hstack->Add(h[2]);
  hstack->Add(h[3]); //all zjets added here

  return hstack;
}

TH1F* get_data(TString hist, TString setType, Int_t set) {
  TH1F* d = 0;
  for(int i = 0; i < sizeof(dData)/sizeof(*dData); ++i) {
    if(!dData[i]) break;
    TFile* f = (TFile*) dData[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    TH1F* tmp = (TH1F*) f->Get(hist.Data());
    if(!tmp) return NULL;
    if(!d) d = tmp;
    else d->Add(tmp);
  }
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  d->SetTitle("Data");
  d->SetName("Data");
  if(rebinH > 0) d->Rebin(rebinH);

  return d;
}

TH1F* get_mock_data(TString hist, TString setType, Int_t set) {
  TH1F* d = (TH1F*) get_data(hist,setType,set)->Clone("hData");
  Int_t order = fitter_->Get_order(set);
  if(order < 0) return NULL;
  TF1* fBkgd;
  TF1* fSignal;
  TF1* f;
  d->Clear();
  
  fBkgd = fitter_->Get_fit_background_function(set);
  fBkgd->SetName(Form("fBkgd%i",set));
  fBkgd->SetTitle("fBkgd");
  fSignal = fitter_->Get_signal_function();
  fSignal->SetName("fSignal");
  int base = 0;
  if(signal_mock_data_ == 0)
    f = new TF1("fMock",Form("fBkgd%i",set));
  else { 
    f = new TF1("fMock",Form("fBkgd%i+fSignal",set));
    f->SetParameters(fSignal->GetParameter(0),
		     fSignal->GetParameter(1),
		     fSignal->GetParameter(2));
    base = 2;
  }
  for(int i = base; i <= order; ++i) {
    f->SetParameter(i,fBkgd->GetParameter(i-base));
  }
  f->SetRange(fitter_->fitMin_,fitter_->fitMax_);
  Double_t numPerLum = fitter_->Get_set_norm(set,signal_mock_data_);
  Double_t num = numPerLum*lum_;
  for(int i = 0; i < num; ++i) {
    Double_t val = f->GetRandom();
    d->Fill(val);
  }
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  d->SetTitle("Data");
  d->SetName("Data");
  if(rebinH > 0) d->Rebin(rebinH);

  return d;
}

vector<TH1F*> get_signal(TString hist, TString setType, Int_t set) {
  vector<TH1F*> sH = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  Int_t sColor[] = { kGreen,kMagenta,kSpring-3};
  Int_t sFill[]  = {3001,3006,3005};
  TString sNames[] = {"BbX"           ,
		      "BBbb"          ,
		      "fcnc_mx10"     ,
		      "fcnc_mx30"     ,
		      "fcnc_mx60"     };
  TString sTitles[] = {"B #rightarrow bX"           ,
		      "BB #rightarrow bb"          ,
		      "fcnc_mx10"                  ,
		      "fcnc_mx30"                  ,
		      "fcnc_mx60"                  };
  for(int i = 0; i < 10; ++i) {
    if(sgnls_[i] == 0) continue;
    sH[i] = NULL;
    if(plotSignal == 0) break;
    TFile* f = (doPtStudy_ == 0) ? (TFile*) sData[i]->Get(Form("%s_%i",setType.Data(),set)) :
      (TFile*) sDataPt[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) continue;
    sH[i] = (TH1F*) f->Get(hist.Data());
    if(!sH[i]) continue;
    sH[i]->Scale((doPtStudy_ == 0) ? sScale[i] : sScalePt[i]);
    sH[i]->SetFillStyle(sFill[i]); 
    sH[i]->SetFillColor(sColor[i]);
    sH[i]->SetLineColor(sColor[i]);
    sH[i]->SetMarkerColor(sColor[i]);
    sH[i]->SetLineWidth(2);
    sH[i]->SetMarkerStyle(20);
    sH[i]->SetName(Form("%s_%s",sNames[i].Data(),hist.Data()));
    sH[i]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e, #varepsilon = %.2e",
			 sTitles[i].Data(), sH[i]->Integral(),sH[i]->Integral()/sXsec[i]/lum_));
    if(rebinH > 0) sH[i]->Rebin(rebinH);
  }

  return sH;
}

vector<TH2F*> get_signal_2D(TString hist, TString setType, Int_t set) {
  vector<TH2F*> sH = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  Int_t sColor[] = { kGreen,kMagenta,kSpring-3};
  Int_t sFill[]  = {3001,3006,3005};
  TString sNames[] = {"BbX"           ,
		      "BBbb"          ,
		      "fcnc_mx10"     ,
		      "fcnc_mx30"     ,
		      "fcnc_mx60"     };
  TString sTitles[] = {"B #rightarrow bX"           ,
		      "BB #rightarrow bb"          ,
		      "fcnc_mx10"                  ,
		      "fcnc_mx30"                  ,
		      "fcnc_mx60"                  };
  for(int i = 0; i < 10; ++i) {
    if(sgnls_[i] == 0) continue;
    sH[i] = NULL;
    if(plotSignal == 0) break;
    TFile* f = (doPtStudy_ == 0) ? (TFile*) sData[i]->Get(Form("%s_%i",setType.Data(),set)) :
      (TFile*) sDataPt[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) continue;
    sH[i] = (TH2F*) f->Get(hist.Data());
    if(!sH[i]) continue;
    sH[i]->Scale((doPtStudy_ == 0) ? sScale[i] : sScalePt[i]);
    sH[i]->SetFillStyle(sFill[i]); 
    sH[i]->SetFillColor(sColor[i]);
    sH[i]->SetLineColor(sColor[i]);
    sH[i]->SetMarkerColor(sColor[i]);
    sH[i]->SetLineWidth(2);
    sH[i]->SetMarkerStyle(20);
    sH[i]->SetName(Form("%s_%s",sNames[i].Data(),hist.Data()));
    sH[i]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e, #varepsilon = %.2e",
			 sTitles[i].Data(), sH[i]->Integral(),sH[i]->Integral()/sXsec[i]/lum_));
    //    if(rebinH > 0) sH[i]->Rebin(rebinH);
  }

  return sH;
}

TCanvas* plot_signal_hist(TString hist, TString setType, Int_t set) {


  TH1F* h[10];
  Int_t color[] = { kRed, kBlue, kYellow,kYellow+3, kRed+3};
  Int_t fill[]  = {3004,3005,3002,3004,3005};
  TString names[] = {"BB2bb", "B2bX_t", "FCNC_MX10", "FCNC_MX30", "FCNC_MX60"};
  TString titles[] = {"BB #rightarrow bb", "B #rightarrow bX (t)", "FCNC_MX10", "FCNC_MX30", "FCNC_MX60"};
  TCanvas* c = new TCanvas(Form("h_signal_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), 1000, 700);
  double m = 0.;
  for(int i = 0; i < 2; ++i) {
    TFile* f = (TFile*) sData[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) return NULL;
    h[i] = (TH1F*) f->Get(hist.Data());
    if(!h[i]) return NULL;
    h[i]->Scale(sScale[i]);
    if(rebinH > 0) h[i]->Rebin(rebinH);
    h[i]->Draw((i>0) ? "hist same" : "hist");
    h[i]->SetFillStyle(fill[i]);
    h[i]->SetFillColor(color[i]);
    h[i]->SetLineColor(color[i]);
    h[i]->SetLineWidth(2);
    h[i]->SetTitle(Form("%s #varepsilon = %.2e",titles[i].Data(),h[i]->Integral()/sXsec[i]));
    h[i]->SetName(names[i]);
    m = max(m,h[i]->GetMaximum());
  }
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  h[0]->SetAxisRange(1e-5,m*1.5,"Y");    
  if(xMin < xMax) h[0]->SetAxisRange(xMin,xMax,"X");    
  c->BuildLegend();
  h[0]->SetXTitle(xtitle.Data());
  h[0]->SetYTitle(ytitle.Data());
  h[0]->SetTitle (title.Data());
  c->SetGrid();
  return c;
}

TCanvas* plot_2D_hist(TString hist, TString setType, Int_t set) {

  TH2F* h[20];
  Int_t color[] = { kYellow,kYellow+3,kBlue, kRed, kRed+3};
  TString names[] = {"t","tbar","ttbar","zjets","zjets_m10to50","muon_2016B"};
  TCanvas* c = new TCanvas(Form("h_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), 1000, 700);
  double m = 0.;
  Int_t first = 1;
  Int_t ind = 0; // index of first histogram to set titles and axis ranges
  for(int i = 0; i < 20; ++i) h[i] = 0;
  for(int i = 0; i < 20; ++i) {
    if(plotBkgd_ == 0) break;
    if(bkgds_[i] == 0) continue;
    TFile* f = (TFile*) data[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) {printf("Can't find the file!\n"); return NULL;}
    h[i] = (TH2F*) f->Get(hist.Data());
    if(!h[i]) {printf("Can't find the histogram!\n"); return NULL;}
    h[i]->Scale(scale[i]);
    //    h[i]->Scale(1./h[i]->Integral());
    int index = i;
    if(i > 3) { //zjets
      index = 3;
      if(h[3] == 0) h[3] = h[i];
      else h[3]->Add(h[i]);
    }
    if(i == 1) { //tbar
      if(h[0] == 0) h[0] = h[i];
      else h[0]->Add(h[i]);
    }
    h[index]->SetMarkerColorAlpha(color[index],0.2);
    h[index]->SetLineColor(color[index]);
    h[index]->SetMarkerStyle(6);
    h[index]->SetName(Form("%s_%s",names[index].Data(),hist.Data()));
    h[index]->SetTitle(Form("%s",names[index].Data()));
    if(index < 3 && i != 1) h[index]->Draw((index>0) ? "same" : "");
    if(first == 1) ind = index;
    first = 0;
    m = max(m,h[index]->GetMaximum());
  }
  if(h[3] != 0) {
    //    h[3]->Scale(1./h[3]->Integral());
    h[3]->Draw("same");
    m = max(m,h[3]->GetMaximum());    
  }

  vector<TH2F*> sH = get_signal_2D(hist,setType,set);
  for(int i = 0; i < 10; ++i) {
    if(plotSignal == 0) break;
    if(sgnls_[i] == 0) continue;
    if(sH[i] == NULL) break;
    sH[i]->SetMarkerStyle(6);
    //      sH[i]->SetMarkerColorAlpha(kGreen,0.5);
    sH[i]->Draw((first == 1) ? "" : "same");
    if(first == 1) ind = i + 100;
    first = 0;
    m = max(sH[i]->GetMaximum(), m);
  }
  for(int i = 0; i < 10; ++i) {
    if(plotSignal == 0) break;
    if(sgnls_[i] == 0) continue;
    if(sH[i] == NULL) break;
    sH[i]->SetMaximum(m*1.3);
    sH[i]->SetMinimum(m*1.e-3);
  }
  for(int i = 0; i < 20; ++i) {
    if(plotBkgd_ == 0) break;
    if(bkgds_[i] == 0) continue;
    h[i]->SetMaximum(m*1.3);
    h[i]->SetMinimum(m*1.e-3);
  }
  
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  TH2F* hAxis = (ind >= 100) ? sH[ind%100] : h[ind];
  if(xMin < xMax) hAxis->SetAxisRange(xMin,xMax,"X");    
  if(yMin < yMax) hAxis->SetAxisRange(yMin,yMax,"Y");    
  c->BuildLegend();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  hAxis->SetTitle (title.Data());
  c->SetGrid();
  if(logZ_) c->SetLogz();
  return c;

}

TCanvas* plot_hist(TString hist, TString setType, Int_t set) {

  TH1F* h[20];
  for(int i = 0; i < 20; ++i) h[i] = 0;
  Int_t color[] = { kYellow,kYellow+3,kBlue, kRed, kRed+3};
  Int_t fill[]  = {3004,3005,3002,3004,3005};
  TString names[] = {"t","tbar","ttbar","zjets","zjets_m10to50","muon_2016B"};
  TCanvas* c = new TCanvas(Form("h_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), 1000, 700);
  double m = 0.;
  Int_t first = 1;
  Int_t ind = 0; // index of first histogram to set titles and axis ranges
  for(int i = 0; i < 20; ++i) {
    if(plotBkgd_ == 0) break;
    if(bkgds_[i] == 0) continue;
    TFile* f = (TFile*) data[i]->Get(Form("%s_%i",setType.Data(),set));
    if(!f) {printf("No folder %s, returning NULL\n",Form("%s_%i",setType.Data(),set));return NULL;}
    h[i] = (TH1F*) f->Get(hist.Data());
    if(!h[i]) {printf("No hist %s, returning NULL\n",hist.Data());return NULL;}
    h[i]->Scale(scale[i]);
    int index = i;
    if(rebinH > 0) h[i]->Rebin(rebinH);
    if(i > 3) { //zjets
      index = 3;
      if(!h[3]) h[3] = h[i];
      else h[3]->Add(h[i]);
    }
    
    h[index]->SetFillStyle(fill[index]); 
    h[index]->SetFillColor(color[index]);
    h[index]->SetLineColor(color[index]);
    h[index]->SetLineWidth(2);
    h[index]->SetMarkerStyle(20);
    h[index]->SetName(Form("%s_%s",names[index].Data(),hist.Data()));
    h[index]->SetTitle(Form("#scale[0.5]{#int} %s = %.2e", names[index].Data(), h[index]->Integral()));
    if(index < 3) { h[index]->Draw((first == 0) ? "hist same" : "hist"); ind = (first == 1) ? i:ind; first = 0;}
    m = max(m,h[index]->GetMaximum());
  }
  if(plotBkgd_ && h[3]){ h[3]->Draw((first == 0) ? "hist same" : "hist:"); ind = (first) ? 3:ind; first = 0;}

  vector<TH1F*> sH = get_signal(hist,setType,set);
  for(int i = 0; i < 10; ++i) {
    if(plotSignal == 0) break;
    if(sgnls_[i] == 0) continue;
    if(sH[i] == NULL) break;
    sH[i]->Draw((first == 0) ? "hist same" : "hist");
    ind = (first == 1) ? i+100:ind; 
    first = 0;
    m = max(sH[i]->GetMaximum(), m);
  }
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  TH1F* hAxis = (ind >= 100) ? sH[ind%100] : h[ind];
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
  pad2->SetBottomMargin(0.22);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  double m = 0.;

  THStack* hstack = get_stack(hist,setType,set);
  TH1F* d = get_data(hist, setType, set);
  vector<TH1F*> sH = get_signal(hist,setType,set);

  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  
  hstack->Draw("hist noclear");
  if(plotData_) d->Draw("E same");
  
  m = max(hstack->GetMaximum(), d->GetMaximum());
  for(int i = 0; i < 10; ++i) {
    if(plotSignal == 0) break;
    if(sH[i] == NULL) break;
    sH[i]->Draw("hist same");
    m = max(sH[i]->GetMaximum(), m);
  }
  TH1F* hDataMC = (TH1F*) d->Clone("hDataMC");
  hDataMC->Clear();
  for(int i = 1; i <= hDataMC->GetNbinsX(); ++i) {
    double dataVal = 0;
    double dataErr = 0;
    if(dataMC_vs_signalBkgd_ == 1) {
      dataVal = d->GetBinContent(i);
      dataErr= d->GetBinError(i);
    } else {
      for(int j = 0; j < 10; ++j) {
	if(sgnls_[j] == 0) continue;
	if(sH[j] == NULL) break;
	dataVal += sH[j]->GetBinContent(i);
	dataErr += sH[j]->GetBinContent(i);
      }
      dataErr = 1.;//sqrt(dataErr);
    }
    double mcVal = ((TH1F*) hstack->GetStack()->Last())->GetBinContent(i);
    double mcErr = ((TH1F*) hstack->GetHistogram())->GetBinError(i);
    if(dataVal == 0 || mcVal == 0 || dataErr == 0) {hDataMC->SetBinContent(i,-1); continue;}
    double err = sqrt(mcErr*mcErr + dataErr*dataErr);
    double errRatio = (dataVal/mcVal)*(dataVal/mcVal)*(dataErr*dataErr/(dataVal*dataVal) + mcErr*mcErr/mcVal/mcVal);
    errRatio = sqrt(errRatio);
    double z = (dataVal-mcVal)/err;
    double ratio = dataVal/mcVal;
    if(dataMC_vs_signalBkgd_ == 0) errRatio = 1.;
    if(data_over_mc_ == 0) {
      hDataMC->SetBinContent(i,z);
      hDataMC->SetBinError(i,1.);
    } else {
      hDataMC->SetBinContent(i,ratio);
      hDataMC->SetBinError(i,errRatio);
    }
  }
  
  pad1->BuildLegend();
  hDataMC->GetXaxis()->SetTitle(xtitle.Data());
  hstack->GetYaxis()->SetTitle(ytitle.Data());
  if(xMin < xMax) hDataMC->GetXaxis()->SetRangeUser(xMin,xMax);    
  if(xMin < xMax) hstack->GetXaxis()->SetRangeUser(xMin,xMax);    
  hstack->GetYaxis()->SetRangeUser(1.e-1,m*1.5);    
  hstack->SetTitle (title.Data());
  // gStyle->SetTitleSize (0.04);
  // gStyle->SetTitleX (0.1);
  // gStyle->SetTitleY (0.9);
  hstack->SetMinimum(1.e-1);
  hstack->SetMaximum(1.5*m);
  hstack->GetYaxis()->SetTitleSize(0.045);
  hstack->GetYaxis()->SetTitleOffset(0.5);
  c->SetGrid();
  pad2->cd();
  pad2->SetGrid();
  c->SetGrid();
  hDataMC->Draw("P");
  if(dataMC_vs_signalBkgd_ == 1)
    hDataMC->GetYaxis()->SetTitle((data_over_mc_ == 0) ? "(Data - MC)/#sigma" : "Data/MC");
  else { 
    hDataMC->GetYaxis()->SetTitle("Signal/Background");
    pad2->SetLogy();
  }
  hDataMC->GetXaxis()->SetTitleSize(0.11);
  hDataMC->GetXaxis()->SetTitleOffset(0.8);
  hDataMC->GetXaxis()->SetLabelSize(0.08);
  hDataMC->GetYaxis()->SetTitleSize(0.1);
  hDataMC->GetYaxis()->SetTitleOffset(0.3);
  hDataMC->GetYaxis()->SetLabelSize(0.08);
  m = hDataMC->GetMaximum();
  double mn = hDataMC->GetMinimum();
  if(dataMC_vs_signalBkgd_ == 1) {min(m*1.5,5.25); mn = -0.25;}
  else {mn = max(0.2*mn,1e-2); m = 5*m;}
  hDataMC->GetYaxis()->SetRangeUser(mn,m);    
  //  hDataMC->GetXaxis()->SetLabelOffset(0.5);
  
  hDataMC->SetMarkerStyle(20);
  //  hDataMC->SetName("hDataMC");
  return c;

}

TCanvas* fit_stack(TString hist, TString setType, Int_t set, Int_t signal = 0) {
  //signal = 1 = include signal
  TCanvas* c = new TCanvas(Form("fit_%ssignal_%s_%i",(signal == 0) ? "no_" : "", hist.Data(),set),
			   Form("fit_%ssignal_%s_%i",(signal == 0) ? "no_" : "", hist.Data(),set), 1000, 700);

  THStack* hstack = get_stack(hist,setType,set);
  vector<TH1F*> sH = get_signal(hist,setType,set);

  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  TF1* f;
  TF1* f2;
  int order = fitter_->Get_order(set);
  f = fitter_->Get_background_function(order);
  f2 = fitter_->Get_signal_function();
  TF1* fBkgd_s = (TF1*) f->Clone(Form("fBkgd_s%i",set));
  fBkgd_s->SetName(Form("fBkgd_s%i",set));
  f2->SetName("fSignal");
  TF1* fSB = new TF1("fSignalBkgd",Form("fSignal + fBkgd_s%i",set));
  Double_t low;
  Double_t high;
  for(int j = 0; j <= 2; ++j) {
    f2->GetParLimits(j,low,high);
    fSB->SetParLimits(j,low,high);
    fSB->SetParameter(j,f2->GetParameter(j));
  }
  for(int j = 1; j <= order; ++j) {
    f->GetParLimits(j,low,high);
    fSB->SetParLimits(j+3,low,high);
  }

  TH1F* h = (TH1F*) hstack->GetStack()->Last()->Clone("h");
  TH1F* h2 = (TH1F*) hstack->GetStack()->Last()->Clone("h2");
  Int_t bins = h->GetNbinsX();
  // if(scaleErrors_ == 1) {
  //   for(int i = 1; i <= bins; ++i) {
  //     double binc = h->GetBinContent(i);
  //     if(binc <= 0) continue;
  //     double bine = sqrt(binc);
  //     h->SetBinError(i, bine);
  //     h2->SetBinError(i, bine);
  //   }
  // }
  if(signal == 1) {
    for(int i = 0; i < 10; ++i) {
      if(sH[i] == NULL) break;
      h->Add(sH[i]);
      h2->Add(sH[i]);
    }
  }
  double hmin = h->GetXaxis()->GetXmin();
  double hmax = h->GetXaxis()->GetXmax();
  hmax = min(hmax,fitter_->fitMax_);
  hmin = max(hmin,fitter_->fitMin_);
  f->SetRange(hmin,hmax);
  fSB->SetRange(hmin,hmax);
  TFitResultPtr res = h->Fit(f,"R S");
  double c1 = res->Chi2();
  int dof1 = res->Ndf();
  double prob1 = ROOT::Math::chisquared_cdf_c(c1,dof1);
  res = h2->Fit(fSB,"R S");
  double c2 = res->Chi2();
  int dof2 = res->Ndf();
  double prob2 = ROOT::Math::chisquared_cdf_c(c2,dof2);
  
  if(signal == 1) {
    for(int i = 0; i < 10; ++i) {
      if(sH[i] == NULL) break;
      hstack->Add(sH[i]);
    }

  }
  //  hstack->Draw("");
  h->Draw();
  double m = hstack->GetMaximum();

  f->Draw("same");
  fSB->Draw("same");
  f->SetTitle("Background Only");
  fSB->SetTitle("Signal + Background");
  fSB->SetLineColor(kMagenta);
  h->SetTitle (Form("#scale[0.5]{#int} Mock Data = %.2e",h->Integral()));
  c->BuildLegend();
  h->GetXaxis()->SetTitle(xtitle.Data());
  h->GetYaxis()->SetTitle(ytitle.Data());
  if(xMin < xMax) {
    if(hmin < xMin && xMin < hmax) hmin = xMin;
    if(hmax > xMax && hmin < xMax) hmax = xMax;
  }
  h->GetXaxis()->SetRangeUser(hmin,hmax);    
  h->GetYaxis()->SetRangeUser(1.e-1,m*1.5);    
  h->SetTitle (title.Data());
  h->SetMinimum(1.e-1);
  h->SetMaximum(1.5*m);
  printf("Chi^2 / DoF for each are %.4f / %i p = %.4e (Background Only) and %.4f / %i p = %.4e (Signal + Bkgd), p2/p1 = %.4e\n",
	 c1, dof1, prob1, c2, dof2, prob2, prob2/prob1);
  c->SetGrid();
  return c;

}

TCanvas* fit_data(TString hist, TString setType, Int_t set, Int_t mockData = 1) {
  TCanvas* c = new TCanvas(Form("fit_data_%s_%i", hist.Data(),set),
			   Form("fit_data_%s_%i", hist.Data(),set), 1000, 700);

  TH1F* hData = (mockData == 1) ? get_mock_data(hist,setType,set) : get_data(hist,setType,set);
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  TF1* f;
  TF1* f2;
  f = fitter_->Get_fit_background_function(set);
  f2 = fitter_->Get_signal_function();
  TF1* fBkgd_s = (TF1*) f->Clone(Form("fBkgd_s%i",set));
  fBkgd_s->SetName(Form("fBkgd_s%i",set));
  f2->SetName("fSignal");
  TF1* fSB = new TF1(Form("fSignalBkgd%i",set),Form("fSignal+fBkgd_s%i",set));
  Double_t low;
  Double_t high;
  int order = fitter_->Get_order(set);
  for(int j = 0; j <= 2; ++j) {
    f2->GetParLimits(j,low,high);
    fSB->SetParLimits(j,low,high);
    // printf("fit limits %i bewteen %.2e and %.2e\n",j+3,low,high); 
  }
  for(int j = 1; j <= order; ++j) {
    f->GetParLimits(j,low,high);
    fSB->SetParLimits(j+3,low,high);
    // printf("fit limits %i bewteen %.2e and %.2e\n",j+3,low,high); 
  }
  TH1F* h  = (TH1F*) hData->Clone("h");
  TH1F* h2 = (TH1F*) hData->Clone("h2");
  Int_t bins = h->GetNbinsX();

  double hmin = h->GetXaxis()->GetXmin();
  double hmax = h->GetXaxis()->GetXmax();
  hmax = min(hmax,fitter_->fitMax_);
  hmin = max(hmin,fitter_->fitMin_);
  f->SetRange(hmin,hmax);
  fSB->SetRange(hmin,hmax);
  TFitResultPtr res = h->Fit(f,"R S B");
  double c1 = res->Chi2();
  int dof1 = res->Ndf();
  double prob1 = ROOT::Math::chisquared_cdf_c(c1,dof1);
  res = h2->Fit(fSB,"R S B");
  double c2 = res->Chi2();
  int dof2 = res->Ndf();
  double prob2 = ROOT::Math::chisquared_cdf_c(c2,dof2);
  
  hData->Draw("E");
  double mx = hData->GetMaximum();

  f->Draw("same");
  fSB->Draw("same");
  f2->Draw("same");
  fBkgd_s->Draw("same");
  f2->SetParameter(0,fSB->GetParameter(0));
  f2->SetParameter(1,fSB->GetParameter(1));
  f2->SetParameter(2,fSB->GetParameter(2));
  for(int i = 0; i <= order; ++i) {
    fBkgd_s->SetParameter(i,fSB->GetParameter(i+3));
  }
  f2->SetRange(fitter_->fitMin_,fitter_->fitMax_);
  fBkgd_s->SetRange(fitter_->fitMin_,fitter_->fitMax_);

  fBkgd_s->SetTitle("Background");
  f->SetTitle("Background Only Fit");
  fSB->SetTitle("Signal + Background");
  f2->SetTitle("Signal");
  fSB->SetLineColor(kMagenta);
  f2->SetLineColor(kMagenta);
  f2->SetLineStyle(2);
  fBkgd_s->SetLineColor(kMagenta);
  fBkgd_s->SetLineStyle(9);

  c->BuildLegend();

  hData->GetXaxis()->SetTitle(xtitle.Data());
  hData->GetYaxis()->SetTitle(ytitle.Data());

  if(xMin < xMax) {
    if(hmin < xMin && xMin < hmax) hmin = xMin;
    if(hmax > xMax && hmin < xMax) hmax = xMax;
  }

  hData->GetXaxis()->SetRangeUser(hmin,hmax);    
  hData->GetYaxis()->SetRangeUser(1.e-1,mx*1.5);    
  hData->SetTitle (title.Data());
  hData->SetMinimum(1.e-1);
  hData->SetMaximum(1.5*mx);

  printf("Chi^2 / DoF for each are %.4f / %i p = %.4e (Background Only) and %.4f / %i p = %.4e (Signal + Bkgd), p2/p1 = %.4e\n",
	 c1, dof1, prob1, c2, dof2, prob2, prob2/prob1);
  c->SetGrid();
  return c;

}

Int_t print_stack(TString folder, TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_stack(hist,setType,set);
  if(!c) return -1;
  c->Print(Form("figures/%s/%s%s_%s_set_%i%s.png",folder.Data(),hist.Data(),
		((dData[0] != NULL) ? "_data":""),(dataMC_vs_signalBkgd_==1)?"dataOverMC":"signalOverBkgd",set,
		(doPtStudy_ > 0) ? "_ptstudy":""));
  delete c;
  return 0;
}

Int_t print_hist(TString folder, TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_hist(hist,setType,set);
  cout << "plotted hist" << endl;
  if(!c) return -1;
  c->Print(Form("figures/%s/%s%s_set_%i%s.png",folder.Data(),hist.Data(),
		(plotBkgd_ == 0) ? "_signalOnly":"",set,(doPtStudy_ > 0) ? "_ptstudy":""));
  delete c;
  return 0;
}

Int_t print_2D_hist(TString folder, TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_2D_hist(hist,setType,set);
  if(!c) return -1;
  c->Print(Form("figures/%s/%s%s_set_%i%s.png",folder.Data(),hist.Data(),
		(plotBkgd_==0)?"_signalOnly":"",set,
		(doPtStudy_ > 0) ? "_ptstudy":""));
  delete c;
  return 0;
}

Int_t print_stacks(vector<TString> folders, vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		   vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  Int_t tmp = dataMC_vs_signalBkgd_;
  for(Int_t i = 0; i < folders.size(); ++i) {
    Int_t set = sets[i];
    //    if( i >= sizeof(folders)/sizeof(*folders)) return -2;
    TString folder(folders[i]);
    for(Int_t j = 0; j < hists.size(); ++j) {
      TString hist(hists[j]);
      TString setType(setTypes[j]);
      xMax = xMaxs[j];
      xMin = xMins[j];
      rebinH = rebins[j];
      dataMC_vs_signalBkgd_ = 1;
      Int_t status = print_stack(folder,hist,setType,set);
      std::printf("Printing Data/MC stack %s %s set %i into %s has status %i\n",setType.Data(),hist.Data(),set,folder.Data(),status);
      dataMC_vs_signalBkgd_ = 0;
      status = print_stack(folder,hist,setType,set);
      std::printf("Printing Signal/Bkgd stack %s %s set %i into %s has status %i\n",setType.Data(),hist.Data(),set,folder.Data(),status);
    }
  }
  dataMC_vs_signalBkgd_ = tmp;
  return 0;
}

Int_t print_hists(vector<TString> folders, vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		   vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  for(Int_t i = 0; i < folders.size(); ++i) {
    Int_t set = sets[i];
    TString folder(folders[i]);
    for(Int_t j = 0; j < hists.size(); ++j) {
      TString hist(hists[j]);
      TString setType(setTypes[j]);
      xMax = xMaxs[j];
      xMin = xMins[j];
      rebinH = rebins[j];
      Int_t status = print_hist(folder,hist,setType,set);
      std::printf("Printing hist %s %s set %i into %s has status %i\n",setType.Data(),hist.Data(),set,folder.Data(),status);
    }
  }
  return 0;
}

Int_t print_2D_hists(vector<TString> folders, vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		     vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins) {
  for(Int_t i = 0; i < folders.size(); ++i) {
    Int_t set = sets[i];
    TString folder(folders[i]);
    for(Int_t j = 0; j < hists.size(); ++j) {
      TString hist(hists[j]);
      TString setType(setTypes[j]);
      xMax = xMaxs[j];
      xMin = xMins[j];
      rebinH = rebins[j];
      Int_t status = print_2D_hist(folder,hist,setType,set);
      std::printf("Printing 2D hist %s %s set %i into %s has status %i\n",setType.Data(),hist.Data(),set,folder.Data(),status);
    }
  }
  return 0;
}

Int_t plot_histograms() {

  const char* names[] = {
    			 "t_tw"                   ,
                         "tbar_tw"                ,
			 "ttbar_inclusive"        ,
			 "zjets_m-50_alt"         ,
			 "zjets_m-10to50_alt"     ,
			 "zjets_m-50"             , //inclusive?
			 "zjets_m-10to50"         , //inclusive?
			 "z1jets_m-50"            ,
			 "z1jets_m-10to50"        ,
			 "z2jets_m-50"            ,
			 "z2jets_m-10to50"        ,
			 "z3jets_m-50"            ,
			 "z3jets_m-10to50"        ,
			 "z4jets_m-50"            ,
			 "z4jets_m-10to50"        };
  
  const char* files[] = {Form("tree_%s.hist",names[0]),
			 Form("tree_%s.hist",names[1]),
			 Form("tree_%s.hist",names[2]),
			 Form("tree_%s.hist",names[3]),
			 Form("tree_%s.hist",names[4]),
			 Form("tree_%s.hist",names[5]),
			 Form("tree_%s.hist",names[6]),
			 Form("tree_%s.hist",names[7]),
			 Form("tree_%s.hist",names[8]),
			 Form("tree_%s.hist",names[9]),
			 Form("tree_%s.hist",names[10]),
			 Form("tree_%s.hist",names[11]),
			 Form("tree_%s.hist",names[12]),
			 Form("tree_%s.hist",names[13]),
			 Form("tree_%s.hist",names[14])};

  const char* filesPtStudy[] = {Form("tree_%s.ptstudy_hist",names[0]),
				Form("tree_%s.ptstudy_hist",names[1]),
				Form("tree_%s.ptstudy_hist",names[2]),
				Form("tree_%s.ptstudy_hist",names[3]),
				Form("tree_%s.ptstudy_hist",names[4]),
				Form("tree_%s.ptstudy_hist",names[5]),
				Form("tree_%s.ptstudy_hist",names[6]),
				Form("tree_%s.ptstudy_hist",names[7]),
				Form("tree_%s.ptstudy_hist",names[8]),
				Form("tree_%s.ptstudy_hist",names[9]),
				Form("tree_%s.ptstudy_hist",names[10]),
				Form("tree_%s.ptstudy_hist",names[11]),
				Form("tree_%s.ptstudy_hist",names[12]),
				Form("tree_%s.ptstudy_hist",names[13]),
				Form("tree_%s.ptstudy_hist",names[14])};

  const Double_t xsec[] = {
			   35.6,
			   35.6,
			   831.8,
			   6803.2,
			   21959.8,
			   6803.2,
			   21959.8,
			   1198.9,
			   855.5,
			   390.6,
			   466.1,
			   113.3,
			   114.5,
			   60.2,
			   36.4,}; //taken from CMS AN-16-458
  for(int i = 0; i < sizeof(bkgds_)/sizeof(*bkgds_); ++i) bkgds_[i]=0;
  bkgds_[0]  = 1; //t
  bkgds_[1]  = 1; //tbar
  bkgds_[2]  = 1; //ttbar
  bkgds_[3]  = 0; //amc z
  bkgds_[4]  = 0; //amc z
  bkgds_[5]  = 0; //madgraph z
  bkgds_[6]  = 0; //madgraph z
  bkgds_[7]  = 1; //madgraph z + 1 jet
  bkgds_[8]  = 1; //madgraph z + 1 jet
  bkgds_[9]  = 1; //madgraph z + 2 jets
  bkgds_[10] = 1; //madgraph z + 2 jets
  bkgds_[11] = 1; //madgraph z + 3 jets
  bkgds_[12] = 1; //madgraph z + 3 jets
  bkgds_[13] = 1; //madgraph z + 4 jets
  bkgds_[14] = 1; //madgraph z + 4 jets

  const char* dNames[] = {
			 "muon_2016B"                 };
  
  const char* dFiles[] = {Form("tree_%s.hist",dNames[0])};

  const Double_t dLum[] = {5.3e3
                          };//, (36.4e3-5.3e3)}; //taken from CMS AN-16-458

  lum_    = accumulate(begin(dLum), end(dLum), 0, plus<double>());; //pb^-1

  

  const char* sNames[] = { //Signal
      			 "b2bx_t"                     ,
    			 "bb2bbdimu"                  ,
    			 "fcnc_mx10"                  ,
    			 "fcnc_mx30"                  ,
    			 "fcnc_mx60"                  };

  const char* sFiles[] = {Form("tree_%s.hist",sNames[0]),
			  Form("tree_%s.hist",sNames[1]),
			  Form("tree_%s.hist",sNames[2]),
			  Form("tree_%s.hist",sNames[3]),
			  Form("tree_%s.hist",sNames[4])};

  const char* sFilesPt[] = {Form("tree_%s.ptstudy_hist",sNames[0]),
			    Form("tree_%s.ptstudy_hist",sNames[1]),
			    Form("tree_%s.ptstudy_hist",sNames[2]),
			    Form("tree_%s.ptstudy_hist",sNames[3]),
			    Form("tree_%s.ptstudy_hist",sNames[4])};

  sXsec[0] = 0.017*1.75*10; //Using a mentioned cross section from AN2016_090_v8
  sXsec[1] = 0.017*1.75*10; //Multiplied by 1.75 as suggested by AN2016_458_v3
  sXsec[2] = 0.017*1.75*10; //Added a factor of 10 to see it
  sXsec[3] = 0.017*1.75*10;
  sXsec[4] = 0.017*1.75*10;

  for(int i = 0; i < sizeof(sgnls_)/sizeof(*sgnls_); ++i) sgnls_[i]=0;

  sgnls_[0] = 1; //b2bx_t
  sgnls_[1] = 1; //bb2bbdimu
  
  TFile* f[] = {new TFile(files[0]),new TFile(files[1]),new TFile(files[2]),
		new TFile(files[3]),new TFile(files[4]),new TFile(files[5])
		,new TFile(files[6]),new TFile(files[7]),new TFile(files[8])
		,new TFile(files[9]),new TFile(files[10]),new TFile(files[11])
		,new TFile(files[12]),new TFile(files[13]),new TFile(files[14])};

  TFile* fPt[] = {new TFile(filesPtStudy[0]), new TFile(filesPtStudy[1]), new TFile(filesPtStudy[2]),
		  new TFile(filesPtStudy[3]), new TFile(filesPtStudy[4]), new TFile(filesPtStudy[5]),
		  new TFile(filesPtStudy[6]), new TFile(filesPtStudy[7]), new TFile(filesPtStudy[8]),
		  new TFile(filesPtStudy[9]), new TFile(filesPtStudy[10]),new TFile(filesPtStudy[11]),
		  new TFile(filesPtStudy[12]),new TFile(filesPtStudy[13]),new TFile(filesPtStudy[14])};
  
  TFile* dF[] = {new TFile(dFiles[0])};

  TFile* sF[] = {new TFile(sFiles[0]),new TFile(sFiles[1]),new TFile(sFiles[2]),
		 new TFile(sFiles[3]),new TFile(sFiles[4])};

  TFile* sFPt[] = {new TFile(sFilesPt[0]),new TFile(sFilesPt[1]),new TFile(sFilesPt[2]),
		   new TFile(sFilesPt[3]),new TFile(sFilesPt[4])};

  Int_t nF  = sizeof(f) /sizeof(*f);
  Int_t ndF = sizeof(dF)/sizeof(*dF);
  Int_t nsF = sizeof(sF)/sizeof(*sF);
  for(int i = 0; i < nF; ++i) {
    data[i]  = (TFile*) f[i]->Get("Data");
  }
  for(int i = 0; i < nF; ++i) {
    dataPt[i]  = (bkgds_[i] > 0) ? (TFile*) fPt[i]->Get("Data") : 0;
  }
  for(int i = 0; i < ndF; ++i) {
    dData[i]  = (TFile*) dF[i]->Get("Data");
  }
  for(int i = 0; i < nsF; ++i) {
    sData[i]  = (TFile*) sF[i]->Get("Data");
  }
  for(int i = 0; i < nsF; ++i) {
    sDataPt[i]  = (TFile*) sFPt[i]->Get("Data");
  }

  TH1F* evts[nF];
  for(int i = 0; i < nF; ++i) {
    evts[i] = (TH1F*)  f[i]->Get(Form("TotalEvents_%s",names[i]));
    scale[i]  = 1./(evts[i]->GetBinContent(1)-2*evts[i]->GetBinContent(11))*xsec[i]*lum_;
  }

  TH1F* evtsPt[nF];
  for(int i = 0; i < nF; ++i) {
    evtsPt[i] = 0;
    if(bkgds_[i] == 0) continue;
    evtsPt[i] = (TH1F*)  fPt[i]->Get(Form("TotalEvents_%s",names[i]));
    scalePt[i]  = 1./(evtsPt[i]->GetBinContent(1)-2*evtsPt[i]->GetBinContent(11))*xsec[i]*lum_;
  }

  // TH1F* dEvts[ndF];
  // for(int i = 0; i < ndF; ++i) {
  //   dEvts[i] = (TH1F*)  dF[i]->Get(Form("TotalEvents_%s",dNames[i]));
  // }

  TH1F* sEvts[nsF];
  for(int i = 0; i < nsF; ++i) {
    sEvts[i] = (TH1F*)  sF[i]->Get(Form("TotalEvents_%s",sNames[i]));
    sScale[i]  = 1./(sEvts[i]->GetBinContent(1)-2*sEvts[i]->GetBinContent(11))*sXsec[i]*lum_;
  }
  
  TH1F* sEvtsPt[nsF];
  for(int i = 0; i < nsF; ++i) {
    sEvtsPt[i]   = (TH1F*)  sFPt[i]->Get(Form("TotalEvents_%s",sNames[i]));
    sScalePt[i]  = 1./(sEvtsPt[i]->GetBinContent(1)-2*sEvtsPt[i]->GetBinContent(11))*sXsec[i]*lum_;
  }

  gStyle->SetTitleW(0.8f);
  rnd_ = new TRandom(seed_);
  vector<TString> folders = {//"mumu_1b1c","mumu_1b1f","mumu_1b1f_1b1c_combined",
			     //"mumu_1b1c","mumu_1b1f","mumu_1b1f_1b1c_combined",
			     "mumu_1b1c","mumu_1b1f","mumu_1b1f_1b1c_combined",
                             "mumu_1b1c","mumu_1b1f"};//,"mumu_1b1f_1b1c_combined"};
  Int_t   sets[]        = {//          2,          3,                       4, //Note without pt/M
                                     //10,         11,                      12, //no pt/M
				     // 6,          7,                       5, //note + pt/M defined pt/M in weird order in sets
				     //14,         15,                      16, //pt/M
                                       20,         21,                      22, //new
                            8, 9 //just basic event selections for central and forward
			    
                            };
  vector<TString> hists{"bjetlepm","lepm",  "met","lepptoverm","jetslepdelr","jetslepdelphi","bjetlepdelr","bjetlepdelphi"};
  vector<TString> setTypes{"event","event","event","event",     "event",      "event",        "event",      "event"};
  vector<Int_t>   rebins{    2,      1,        1,       1,           1,            1,               1,          1};
  vector<Double_t> xMaxs{ 500.,    70.,       -1,      -1,          -1,           -1,              -1,         -1};
  vector<Double_t> xMins{  50.,    12.,        1,       1,           1,            1,               1,          1};
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  //  print_stacks(folders,hists,setTypes,sets, xMaxs, xMins,
  //  print_hists(folders,hists,setTypes,sets, xMaxs, xMins,rebins);
  plotBkgd_ = 0;
  //  print_hists(folders,hists,setTypes,sets, xMaxs, xMins,rebins);
  plotBkgd_ = 1;
  
  doPtStudy_ = 1;
  plotData_  = 0;
  //  print_stacks(folders,hists,setTypes,sets, xMaxs, xMins,rebins);
  doPtStudy_ = 0;
  plotData_  = 1;
  vector<TString> hists2D{"jetsdelrvsphi", "lepdelrvsphi", "jetslepdelrvsphi", "jetlepdelrvsphi", "bjetlepdelrvsphi"};
  vector<TString> setTypes2D{"event",        "event",          "event",           "event",             "event"};
  vector<Int_t>   rebins2D{   1,               1,                 1,                1,                   1};
  vector<Double_t> xMaxs2D{  -1,              -1,                -1,               -1,                  -1};
  vector<Double_t> xMins2D{   1,               1,                 1,                1,                   1};
  //  print_2D_hists(folders,hists2D,setTypes2D,sets, xMaxs2D, xMins2D,rebins2D);
  // plot_signal_hist("bjetlepm","event",4);

  dataMC_vs_signalBkgd_ = 0;
  xMin = 12.;
  xMax = 70.;
  plot_stack("lepm","event",5);
  return 0;
}
