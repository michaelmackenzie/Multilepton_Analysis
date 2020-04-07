
#ifndef DataPlotter_hh
#define DataPlotter_hh

#include "TString.h"
#include "TFile.h"
#include "TKey.h"
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

  //plotting information
  struct PlottingCard_t {
    TString  hist_;
    TString  type_;
    TString  label_;
    Int_t    set_;
    Double_t xmin_;
    Double_t xmax_;
    Double_t ymin_;
    Double_t ymax_;
    Int_t    rebin_;
    Double_t blindmin_;
    Double_t blindmax_;

    PlottingCard_t() : hist_(""), type_(""), label_(""), set_(-1),
		       xmin_(1.), xmax_(-1.), ymin_(1.), ymax_(-1.), rebin_(1),
		       blindmin_(1.), blindmax_(-1.) {}

    PlottingCard_t(TString hist, TString type) : PlottingCard_t() {
      hist_ = hist;
      type_ = type;
    }
    PlottingCard_t(TString hist, TString type, Int_t set) : PlottingCard_t(hist,type) {
      set_  = set;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, TString label) : PlottingCard_t(hist, type, set) {
      label_ = label;
    }
    PlottingCard_t(TString hist, TString type, Int_t rebin, Double_t xmin, Double_t xmax) : PlottingCard_t(hist,type) {
      rebin_ = rebin;
      xmin_  = xmin;
      xmax_  = xmax;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, Int_t rebin, Double_t xmin, Double_t xmax) : PlottingCard_t(hist,type,rebin,xmin,xmax) {
      set_ = set;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, TString label, Int_t rebin,
		   Double_t xmin, Double_t xmax) : PlottingCard_t(hist,type,set,rebin,xmin,xmax) {
      label_ = label;
    }
    PlottingCard_t(TString hist, TString type, Int_t rebin, Double_t xmin, Double_t xmax,
		   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,rebin,xmin,xmax) {
      blindmin_ = blindmin;
      blindmax_ = blindmax;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, Int_t rebin, Double_t xmin, Double_t xmax,
		   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,set,rebin,xmin,xmax) {
      blindmin_ = blindmin;
      blindmax_ = blindmax;
    }
    PlottingCard_t(TString hist, TString type, TString label, Int_t rebin, Double_t xmin, Double_t xmax,
		   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,rebin,xmin,xmax,blindmin,blindmax) {
      label_ = label;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, TString label, Int_t rebin, Double_t xmin, Double_t xmax,
		   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,set,rebin,xmin,xmax,blindmin,blindmax) {
      label_ = label;
    }
  };
  
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

  Double_t xMin_ =  1e6; //plotting ranges
  Double_t xMax_ = -1e6;
  Double_t yMin_ =  1e6;
  Double_t yMax_ = -1e6;
  Double_t blindxmin_ = 1e6; //for blinding along x axis
  Double_t blindxmax_ = -1e6;
  Double_t blindymin_ = 1e6; //for blinding along y axis
  Double_t blindymax_ = -1e6;
  Int_t logZ_ = 1; //log plot settings
  Int_t logY_ = 0;
  Int_t plot_data_ = 1; //only MC or include data
  Int_t rebinH_ = 1; //rebinning of histograms
  Int_t data_over_mc_ = 1; //do data/MC or data-MC
  Int_t stack_uncertainty_ = 1; //whether or not to add gray shading for uncertainty
  Int_t debug_ = 0; //for debugging
  Int_t include_qcd_ = 1; //use the same sign selection to get the QCD
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
  bool useCLs_ = true; //whether to use CLs or CLs+b when calculating limits/limit gains
  
  //Various canvas drawing numbers
  Int_t background_colors_[10] = {kRed-7, kRed-3, kYellow-7,kGreen-7 , kViolet+6, kCyan-7, kRed+3,kOrange-9,kBlue+1};
  Int_t signal_colors_[10] = {kBlue, kOrange+10, kGreen+4, kViolet-2, kYellow+3,kOrange-9,kBlue+1};
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
  Double_t cms_txt_x_single_ = 0.29; //text location without data/mc split pad
  Double_t cms_txt_y_single_ = 0.92;
  Double_t cms_txt_size_ = 0.08;
  Double_t cms_txt_size_single_ = 0.06; //text size without data/mc split pad
  
  //data yield drawing
  Double_t data_txt_x_ = 0.42;
  Double_t data_txt_y_ = 0.63;
  
  //significance drawing
  Double_t sig_plot_range_ = 3.5;
  
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
    cmslabel -> SetTextSize((single) ? cms_txt_size_single_ : cms_txt_size_);
    cmslabel -> SetTextAlign(22);
    cmslabel -> SetTextAngle(0);
    cmslabel -> DrawText((single) ? cms_txt_x_single_ : cms_txt_x_,
			 (single) ? cms_txt_y_single_ : cms_txt_y_, "CMS Preliminary");
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
  TCanvas* plot_single_2Dhist(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    return plot_single_2Dhist(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_, card.ymin_, card.ymax_);
  }
  
  virtual TCanvas* plot_2Dhist(TString hist, TString setType, Int_t set);
  TCanvas* plot_2Dhist(TString hist, TString setType, Int_t set,
		       Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = plot_2Dhist(hist, setType, set);
    reset_axes(); return c;
  }
  TCanvas* plot_2Dhist(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    return plot_2Dhist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.ymin_, card.ymax_);
  }


  virtual TCanvas* plot_hist(TString hist, TString setType, Int_t set);
  TCanvas* plot_hist(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_hist(hist, setType, set); reset_axes(); return c;
  }
  TCanvas* plot_hist(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    blindxmin_ = card.blindmin_;
    blindxmax_ = card.blindmax_;
    return plot_hist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_);
  }

  virtual TCanvas* plot_stack(TString hist, TString setType, Int_t set);
  TCanvas* plot_stack(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_stack(hist, setType, set); reset_axes(); return c;
  }
  TCanvas* plot_stack(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    blindxmin_ = card.blindmin_;
    blindxmax_ = card.blindmax_;
    return plot_stack(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_);
  }

  virtual TCanvas* plot_cdf(TString hist, TString setType, Int_t set, TString label);
  TCanvas* plot_cdf(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = plot_cdf(hist, setType, set, label); reset_axes(); return c;
  }
  TCanvas* plot_cdf(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    return plot_cdf(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_);
  }


  virtual TCanvas* plot_significance(TString hist, TString setType, Int_t set, TString label, bool dir,
				     Double_t line_val, bool doVsEff, TString label1, TString label2);

  TCanvas* plot_significance(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax,
			     bool dir=true, Double_t line_val=-1., bool doVsEff = false, TString label1 = "", TString label2 = "") {
    xMin_ = xmin; xMax_=xmax;
    auto c = plot_significance(hist, setType, set, label, dir, line_val, doVsEff, label1, label2);
    reset_axes(); return c;
  }

  virtual TCanvas* print_stack(TString hist, TString setType, Int_t set);
  TCanvas* print_stack(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = print_stack(hist, setType, set); reset_axes(); return c;
  }
  TCanvas* print_stack(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    blindxmin_ = card.blindmin_;
    blindxmax_ = card.blindmax_;
    return print_stack(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_);
  }

  virtual TCanvas* print_hist(TString hist, TString setType, Int_t set);
  TCanvas* print_hist(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = print_hist(hist, setType, set); reset_axes(); return c;
  }
  TCanvas* print_hist(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    blindxmin_ = card.blindmin_;
    blindxmax_ = card.blindmax_;
    return print_hist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_);
  }

  virtual TCanvas* print_2Dhist(TString hist, TString setType, Int_t set);
  TCanvas* print_2Dhist(TString hist, TString setType, Int_t set,
		       Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = print_2Dhist(hist, setType, set); reset_axes(); return c;
  }
  TCanvas* print_2Dhist(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    return print_2Dhist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.ymin_, card.ymax_);
  }

  virtual TCanvas* print_single_2Dhist(TString hist, TString setType, Int_t set, TString label);
  TCanvas* print_single_2Dhist(TString hist, TString setType, Int_t set, TString label, 
			      Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = print_single_2Dhist(hist, setType, set, label); reset_axes(); return c;
  }
  TCanvas* print_single_2Dhist(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    return print_single_2Dhist(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_, card.ymin_, card.ymax_);
  }
  
  virtual TCanvas* print_cdf(TString hist, TString setType, Int_t set, TString label);
  TCanvas* print_cdf(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax) {
    xMin_ = xmin; xMax_=xmax; auto c = print_cdf(hist, setType, set, label); reset_axes(); return c;
  }
  TCanvas* print_cdf(PlottingCard_t card) {
    rebinH_ = card.rebin_;
    return print_cdf(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_);
  }

  virtual TCanvas* print_significance(TString hist, TString setType, Int_t set, TString label, bool dir,
				      Double_t line_val, bool doVsEff, TString label1, TString label2);
  TCanvas* print_significance(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax,
			      bool dir=true, Double_t line_val=-1., bool doVsEff = false, TString label1 = "", TString label2 = "") {
    xMin_ = xmin; xMax_=xmax; auto c = print_significance(hist, setType, set, label, dir, line_val, doVsEff, label1, label2); reset_axes(); return c;
  }

  virtual Int_t print_stacks(vector<TString> hists, vector<TString> setTypes, vector<Int_t>sets,
			     vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins
			     , vector<Double_t> signal_scales, vector<Int_t> base_rebins);

  Int_t print_stacks(vector<PlottingCard_t> cards, vector<Int_t> sets, vector<Double_t> signal_scales, vector<Int_t> base_rebins) {
    Int_t status = 0;
    for(unsigned index = 0; index < sets.size(); ++index) {
      if(signal_scales.size() == sets.size()) signal_scale_ = signal_scales[index];
      for(PlottingCard_t card : cards) {
	if(base_rebins.size() == sets.size()) card.rebin_ *= base_rebins[index];
	card.set_ = sets[index];
	auto c = print_stack(card);
	status += (c) ? 0 : 1;
	delete c;
      }
    }
    return status;
  }
  
  virtual Int_t print_hists(vector<TString> hists, vector<TString> setTypes, vector<Int_t> sets,
			    vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins
			    , vector<Double_t> signal_scales, vector<Int_t> base_rebins);

  Int_t print_hists(vector<PlottingCard_t> cards, vector<Int_t> sets, vector<Double_t> signal_scales, vector<Int_t> base_rebins) {
    Int_t status = 0;
    for(unsigned index = 0; index < sets.size(); ++index) {
      if(signal_scales.size() == sets.size()) signal_scale_ = signal_scales[index];
      for(PlottingCard_t card : cards) {
	if(base_rebins.size() == sets.size()) card.rebin_ *= base_rebins[index];
	card.set_ = sets[index];
	auto c = print_hist(card);
	status += (c) ? 0 : 1;
	delete c;
      }
    }
    return status;
  }

  //Load files
  virtual Int_t init_files();

  //Add file to be read and plotted
  virtual Int_t add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal);

  void set_luminosity(double lum) { lum_ = lum;}

  void set_selection(TString selec) { selection_ = selec;}
  
  ClassDef(DataPlotter,0);
};
#endif 
