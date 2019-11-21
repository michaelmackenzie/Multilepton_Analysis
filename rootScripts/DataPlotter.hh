
#ifndef DataPlotter_hh
#define DataPlotter_hh

#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TLine.h"
#include "TObject.h"
#include "TLegend.h"

class DataPlotter : public TObject {
public :

  TString selection_ = "mutau"; //selection category
  vector<Double_t> scale_; //scales for datasets
  vector<Int_t>    process_; //indicates which backgrounds to use
  vector<TString> names_; //to get event histograms
  vector<TString> labels_; //to label tlegend and group datasets
  vector<Double_t> xsec_;
  vector<TString> fileNames_;
  vector<TFile*> data_;  //background data files
  vector<bool> isData_; //flag to check if is data
  vector<bool> isSignal_; //flag to check if is signal file

  Double_t lum_; //luminosity
  Double_t rootS_ = 13.; //sqrt(S)
  Int_t seed_ = 90; //random number generator seed
  TRandom* rnd_;

  Double_t xMin_ =  1e6;
  Double_t xMax_ = -1e6;
  Double_t yMin_ =  1e6;
  Double_t yMax_ = -1e6;
  Int_t logZ_ = 1;
  Int_t logY_ = 0;
  Int_t plot_data_ = 1;
  Int_t rebinH_ = 1;
  Int_t data_over_mc_ = 1;
  Int_t debug_ = 0;
  Int_t include_qcd_ = 1;
  Int_t qcd_offset_ = 100; //set number offset to get same sign selection
  Int_t plot_title_ = 0; //Plot the title on the canvas
  Double_t fill_alpha_ = 0.3; //alpha to use for hist plotting
  Int_t normalize_2ds_ = 1; //normalzie 2D histograms when plotting
  Double_t signal_scale_ = 1.; //increase the size of the signal if needed
  Int_t stack_signal_ = 0; //put signal into the stack
  Int_t plot_y_title_ = 0; //plot y title on 1D histograms
  Double_t qcd_scale_ = 1.; //scale factor for SS --> OS selection
  TString folder_ = "ztautau"; //figures folder for printing

  ~DataPlotter() {
    for(auto d : data_) {
      if(d) delete d;
    }
  }
  
  void draw_cms_label() {
    TText *cmslabel = new TText();
    cmslabel-> SetNDC();
    cmslabel -> SetTextFont(1);
    cmslabel -> SetTextColor(1);
    cmslabel -> SetTextSize(.08);
    cmslabel -> SetTextAlign(22);
    cmslabel -> SetTextAngle(0);
    cmslabel -> DrawText(0.27, 0.9, "CMS Preliminary");
  }

  void draw_luminosity() {
    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    // label.SetTextColor(1);
    label.SetTextSize(.04);
    label.SetTextAlign(13);
    label.SetTextAngle(0);
    label.DrawLatex(0.7, 0.98, Form("L=%.1f/fb #sqrt{#it{s}} = %.0f TeV",lum_/1e3,rootS_));
  }

  void reset_axes() {xMin_=1.e6; xMax_=-1.e6; yMax_=-1.e6; yMin_=1.e6;}
  
  virtual void get_titles(TString hist, TString setType, TString* xtitle, TString* ytitle, TString* title);

  virtual vector<TH1F*> get_signal(TString hist, TString setType, Int_t set);
  virtual TH2F* get_signal_2D(TString hist, TString setType, Int_t set);

  virtual TH1F* get_data(TString hist, TString setType, Int_t set);
  virtual TH2F* get_data_2D(TString hist, TString setType, Int_t set);

  virtual TH1F* get_qcd(TString hist, TString setType, Int_t set);

  virtual THStack* get_stack(TString hist, TString setType, Int_t set);

  virtual TCanvas* plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label);
  TCanvas* plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label, 
			      Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c =  plot_single_2Dhist(hist, setType, set, label);
    reset_axes(); return c;
  }
  
  virtual TCanvas* plot_2Dhist(TString hist, TString setType, Int_t set);
  TCanvas* plot_2Dhist(TString hist, TString setType, Int_t set,
		       Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = plot_2Dhist(hist, setType, set);
    reset_axes(); return c;
  }


  virtual TCanvas* plot_hist(TString hist, TString setType, Int_t set);
  TCanvas* plot_hist(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_hist(hist, setType, set); reset_axes(); return c;
  }

  virtual TCanvas* plot_stack(TString hist, TString setType, Int_t set);
  TCanvas* plot_stack(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_stack(hist, setType, set); reset_axes(); return c;
  }

  virtual TCanvas* print_stack(TString hist, TString setType, Int_t set);
  TCanvas* print_stack(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = print_stack(hist, setType, set); reset_axes(); return c;
  }

  virtual TCanvas* print_hist(TString hist, TString setType, Int_t set);
  TCanvas* print_hist(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = print_hist(hist, setType, set); reset_axes(); return c;
  }

  virtual TCanvas* print_2Dhist(TString hist, TString setType, Int_t set);
  TCanvas* print_2Dhist(TString hist, TString setType, Int_t set,
		       Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = print_2Dhist(hist, setType, set); reset_axes(); return c;
  }

  virtual TCanvas* print_single_2Dhist(TString hist, TString setType, Int_t set, TString label);
  TCanvas* print_single_2Dhist(TString hist, TString setType, Int_t set, TString label, 
			      Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = print_single_2Dhist(hist, setType, set, label); reset_axes(); return c;
  }
  
  virtual Int_t print_stacks(vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		     vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins);

  virtual Int_t print_hists(vector<TString> hists, vector<TString> setTypes, Int_t sets[],
		    vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins);

  //Load files
  virtual Int_t init_files();

  //Add file to be read and plotted
  virtual Int_t add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal);

  void set_luminosity(double lum) { lum_ = lum;}

  void set_selection(TString selec) { selection_ = selec;}
  
  ClassDef(DataPlotter,0);
};
#endif 
