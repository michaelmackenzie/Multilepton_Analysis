
#ifndef DataPlotter_hh
#define DataPlotter_hh

#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TLine.h"
#include "TObject.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "Math/ProbFuncMathCore.h"

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
  Int_t stack_uncertainty_ = 1; //whether or not to add gray shading for uncertainty
  Int_t debug_ = 0;
  Int_t include_qcd_ = 1;
  Int_t qcd_offset_ = 100; //set number offset to get same sign selection
  Int_t plot_title_ = 0; //Plot the title on the canvas
  Double_t fill_alpha_ = 0.9; //alpha to use for hist plotting
  Int_t normalize_1ds_ = 0; //normalzie 1D histograms when plotting
  Int_t normalize_2ds_ = 1; //normalzie 2D histograms when plotting
  Double_t signal_scale_ = 1.; //increase the size of the signal if needed
  Int_t stack_signal_ = 0; //put signal into the stack
  Int_t plot_y_title_ = 0; //plot y title on 1D histograms
  Double_t qcd_scale_ = 1.; //scale factor for SS --> OS selection
  TString folder_ = "ztautau"; //figures folder for printing
  Int_t useOpenGL_ = 1; //Use open GL with plotting
  bool doStatsLegend_ = true; //Give each backgrounds contribution in the legend

  //Various canvas drawing numbers
  Int_t canvas_x_ = 900; //canvas dimensions
  Int_t canvas_y_ = 800;
  Double_t axis_font_size_ = 0.2; //axis title values
  Double_t y_title_offset_ = 0.18;
  Double_t x_title_offset_ = 0.6;
  Double_t x_label_size_ = 0.1;
  Double_t y_label_size_ = 0.1;
  Double_t upper_pad_x1_ = 0.0; //upper pad fractional dimensions
  Double_t upper_pad_y1_ = 0.3;
  Double_t upper_pad_x2_ = 1.0;
  Double_t upper_pad_y2_ = 1.0;
  Double_t lower_pad_x1_ = 0.0; //lower pad fractional dimensions
  Double_t lower_pad_y1_ = 0.02;
  Double_t lower_pad_x2_ = 1.0;
  Double_t lower_pad_y2_ = 0.3;
  Double_t upper_pad_topmargin_ = 0.06; //pad margins
  Double_t upper_pad_botmargin_ = 0.05;
  Double_t lower_pad_topmargin_ = 0.03;
  Double_t lower_pad_botmargin_ = 0.27;
  Double_t legend_txt_ = 0.06; //Legend parameters
  Double_t legend_x1_stats_ = 0.6; //if stats in legend
  Double_t legend_x1_ = 0.62; //if no stats in legend
  Double_t legend_x2_ = 0.9; 
  Double_t legend_y1_ = 0.93;
  Double_t legend_y2_ = 0.43; 
  // Double_t legend_y2_split_ = 0.45; 
  Double_t legend_sep_ = 2.;
  //luminosity drawing
  Double_t lum_txt_x_ = 0.67;
  Double_t lum_txt_y_ = 0.98;
  //CMS prelim drawing
  Double_t cms_txt_x_ = 0.28;
  Double_t cms_txt_y_ = 0.9;
  
  //data yield drawing
  Double_t data_txt_x_ = 0.42;
  Double_t data_txt_y_ = 0.63;
  
  //significance drawing
  Double_t sig_plot_range_ = 3.;
  
  ~DataPlotter() {
    for(auto d : data_) {
      if(d->TestBit(TObject::kNotDeleted)) {d->Close();}
    }
  }
  
  void draw_cms_label(bool single = false) {
    TText *cmslabel = new TText();
    cmslabel -> SetNDC();
    cmslabel -> SetTextFont(72);
    cmslabel -> SetTextColor(1);
    cmslabel -> SetTextSize(.08);
    cmslabel -> SetTextAlign(22);
    cmslabel -> SetTextAngle(0);
    cmslabel -> DrawText((single) ? 0.37 : cms_txt_x_, (single) ? 0.92 : cms_txt_y_, "CMS Preliminary");
  }

  void draw_luminosity() {
    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    // label.SetTextColor(1);
    label.SetTextSize(.04);
    label.SetTextAlign(13);
    label.SetTextAngle(0);
    label.DrawLatex(lum_txt_x_, lum_txt_y_, Form("L=%.1f/fb #sqrt{#it{s}} = %.0f TeV",lum_/1e3,rootS_));
  }

  void draw_data(int ndata, double nmc, map<TString, double> nsig) {
    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    // label.SetTextColor(1);
    label.SetTextSize(legend_txt_);
    label.SetTextAlign(13);
    label.SetTextAngle(0);
    label.DrawLatex(data_txt_y_, data_txt_x_, Form("%10s = %10i", "n_{Data}", ndata));
    double x = data_txt_x_;
    if(nmc > 0.) {
      x -= 0.045;
      label.DrawLatex(data_txt_y_, x, Form("%9s = %10.1f", "n_{MC}", nmc));
    }
    auto it = nsig.begin();
    while(it != nsig.end()) {
      x -= 0.045;
      label.DrawLatex(data_txt_y_, x, Form("%10s = %10.1f", Form("n_{%s}", it->first.Data()), it->second));
      it++;
    }
    
  }

  void reset_axes() {xMin_=1.e6; xMax_=-1.e6; yMax_=-1.e6; yMin_=1.e6;}
  
  virtual void get_titles(TString hist, TString setType, TString* xtitle, TString* ytitle, TString* title);

  virtual vector<TH1F*> get_signal(TString hist, TString setType, Int_t set);
  virtual TH2F* get_signal_2D(TString hist, TString setType, Int_t set);

  virtual TH1F* get_data(TString hist, TString setType, Int_t set);
  virtual TH2F* get_data_2D(TString hist, TString setType, Int_t set);

  virtual TH1F* get_qcd(TString hist, TString setType, Int_t set);

  virtual TH1F* get_stack_uncertainty(THStack* hstack, TString hname);
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

  virtual TCanvas* plot_cdf(TString hist, TString setType, Int_t set, TString label);
  TCanvas* plot_cdf(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_cdf(hist, setType, set, label); reset_axes(); return c;
  }


  virtual TCanvas* plot_significance(TString hist, TString setType, Int_t set, TString label, bool dir, Double_t line_val, bool doVsEff);
  TCanvas* plot_significance(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax,
			     bool dir=true, Double_t line_val=-1., bool doVsEff = false) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_significance(hist, setType, set, label, dir, line_val, doVsEff); reset_axes(); return c;
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
  
  virtual TCanvas* print_cdf(TString hist, TString setType, Int_t set, TString label);
  TCanvas* print_cdf(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = print_cdf(hist, setType, set, label); reset_axes(); return c;
  }

  virtual TCanvas* print_significance(TString hist, TString setType, Int_t set, TString label, bool dir, Double_t line_val, bool doVsEff);
  TCanvas* print_significance(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax,
			      bool dir=true, Double_t line_val=-1., bool doVsEff = false) {
    xMin_ = xmin; xMax_=xmax; auto c = print_significance(hist, setType, set, label, dir, line_val, doVsEff); reset_axes(); return c;
  }

  virtual Int_t print_stacks(vector<TString> hists, vector<TString> setTypes, vector<Int_t>sets,
			     vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins
			     , vector<Double_t> signal_scales, vector<Int_t> base_rebins);

  virtual Int_t print_hists(vector<TString> hists, vector<TString> setTypes, vector<Int_t> sets,
		    vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins
			    , vector<Double_t> signal_scales, vector<Int_t> base_rebins);

  //Load files
  virtual Int_t init_files();

  //Add file to be read and plotted
  virtual Int_t add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal);

  void set_luminosity(double lum) { lum_ = lum;}

  void set_selection(TString selec) { selection_ = selec;}
  
  ClassDef(DataPlotter,0);
};
#endif 
