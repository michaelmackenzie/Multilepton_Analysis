#ifndef DataPlotter_hh
#define DataPlotter_hh

#include <vector>

#include "TString.h"
#include "TFile.h"
#include "TKey.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TLine.h"
#include "TObject.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "interface/Significances.hh"
#include "interface/Utilities.hh"
#include "interface/PlotUtil.hh"
#include "Math/ProbFuncMathCore.h"

#include "interface/DataCard_t.hh"
#include "interface/PlottingCard_t.hh"
#include "interface/ScaleUncertainty_t.hh"
#include "interface/Titles.hh"

namespace CLFV {
  class DataPlotter : public TObject {
  public :

    //Data stored per file
    struct Data_t {
      Double_t scale_     ; //scale for the dataset
      Int_t    process_   ; //indicates which backgrounds to use
      TString  name_      ; //to get event histograms
      TString  label_     ; //to label tlegend and group datasets
      Double_t xsec_      ; //cross section
      TString  fileName_  ; //input file name
      TFile*   file_      ; //histogram files opened
      TFile*   data_      ; //background data files
      bool     isData_    ; //flag to check if is data
      bool     isEmbed_   ; //flag to check if is embedded
      bool     isSignal_  ; //flag to check if is signal file
      Int_t    dataYear_  ; //list of years it's associated with (2016, 2017, or 2018)
      Int_t    ngenerated_; //generation number for datasets
      Int_t    color_     ; //color for plotting
    };

    TString selection_ = "mutau"; //selection category
    std::vector<Int_t> years_ = {2016};
    Int_t verbose_ = 0;
    std::vector<Data_t>   inputs_; //list of input data to plot

    Double_t embed_scale_ = 1.; //additional scale factor for embedding

    Double_t lum_; //luminosity
    std::map<Int_t, Double_t> lums_; //luminosity by year
    Double_t rootS_ = 13.; //sqrt(S)
    Int_t seed_ = 90; //random number generator seed
    TRandom* rnd_;
    Bool_t include_empty_ = 0; //include empty histograms in the stack
    Bool_t clip_negative_ = 0; //clip negative histogram bins for all processes

    Double_t xMin_ =  1e6; //plotting ranges
    Double_t xMax_ = -1e6;
    Double_t yMin_ =  1e6;
    Double_t yMax_ = -1e6;
    std::vector<Double_t> blindxmin_; //for blinding along x axis
    std::vector<Double_t> blindxmax_;
    std::vector<Double_t> blindymin_; //for blinding along y axis
    std::vector<Double_t> blindymax_;
    TString figure_format_ = "png"; //format plots are written out in
    Int_t logZ_ = 1; //log plot settings
    Int_t logY_ = 0;
    Int_t plot_data_ = 1; //only MC or include data
    Int_t rebinH_ = 1; //rebinning of histograms
    Int_t data_over_mc_ = 1; //do data/MC or data-MC: 0 = none, 1 = data/MC, -1 = signal/bkg, -2 = signal/sqrt(bkg)
    Float_t ratio_plot_min_ = 0.6; //Data/MC Y-axis range
    Float_t ratio_plot_max_ = 1.4;
    Int_t stack_uncertainty_ = 1; //whether or not to add gray shading for uncertainty
    Int_t combine_uncertainties_ = 1; //whether or not to combine sys and stat uncertainties
    Int_t stat_unc_color_ = kGray + 1; //stat uncertainty graph fill color
    Int_t add_bkg_hists_manually_ = 0; //whether to use Stacks given uncertainty or add them by hand
    Int_t density_plot_ = 0; //divide by bin width
    Int_t debug_ = 0; //for debugging
    Int_t include_qcd_ = 1; //use the same sign selection to get the QCD
    Int_t qcd_offset_ = 1000; //set number offset to get same sign selection
    Int_t qcd_color_ = kOrange+6; //QCD histogram color
    Int_t include_misid_ = 0; //use the anti-iso selection to get the misidentified lepton contribution
    Int_t misid_offset_ = 2000; //set number offset to get the anti-iso selection
    Int_t misid_color_ = kGreen-7; //MisID histogram color
    TString misid_label_ = "Jet->#tau";
    Double_t single_2d_right_margin_ = 0.1;
    TString single_2d_style_ = "cont3"; //Drawing option for single 2D histograms vs data
    TString single_2d_data_style_ = "colz";
    Int_t single_2d_color_ = kBlack; //color used for drawing 2D histograms
    Double_t single_2d_marker_size_ = 0.1;
    Int_t single_2d_marker_style_ = 20;
    Int_t single_2d_ncontour_ = 10;
    Int_t single_2d_linewidth_ = 2;
    Bool_t single_2d_legend_ = false;
    Int_t plot_title_ = 0; //Plot the title on the canvas
    Double_t fill_alpha_ = 1.; //alpha to use for hist plotting
    Int_t normalize_1ds_ = 0; //normalzie 1D histograms when plotting
    Int_t normalize_2ds_ = 0; //normalzie 2D histograms when plotting
    Double_t signal_scale_ = 1.; //increase the size of the signal if needed
    std::map<TString, Double_t> signal_scales_; //map from signal label to signal scaling
    Int_t stack_signal_ = 0; //put signal into the stack
    Int_t include_signal_subtraction_ = 0; //include signal when subtracting MC from data
    Int_t signal_fill_style_ = 3005; //fill for the signal histograms
    Int_t stack_as_hist_ = 0; //plot the stack as a total background histogram
    TString only_signal_ = ""; //Only plot the given signal label
    Int_t plot_y_title_ = 0; //plot y title on 1D histograms
    Double_t qcd_scale_ = 1.; //scale factor for SS --> OS selection
    TString folder_ = "clfv"; //figures folder for printing
    Int_t useOpenGL_ = 1; //Use open GL with plotting
    bool doStatsLegend_ = true; //Give each backgrounds contribution in the legend
    bool useCLs_ = true; //whether to use CLs or CLs+b when calculating limits/limit gains
    bool doExactLimit_ = true; //whether to use signal/sqrt(background) or actual calculation

    //Various canvas drawing numbers
    Int_t background_colors_[10] = {kRed-7, kRed-3    , kYellow-7 , kGreen-7, kViolet+6, kCyan-7  , kRed+3 ,kOrange-9,kBlue+1};
    Int_t signal_colors_[10] =     {kBlue , kMagenta+2, kOrange+10, kGreen+4, kViolet-2, kYellow-7, kCyan+1,kBlue+1};
    Int_t total_background_color_ = kRed-7; //for all backgrounds combined drawing
    Int_t canvas_x_ = 900; //canvas dimensions
    Int_t canvas_y_ = 800;
    Double_t axis_font_size_ = 0.157; //axis title values
    Double_t y_title_offset_ = 0.20;
    Double_t x_title_offset_ = 0.71;
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
    Double_t upper_pad_botmargin_ = 0.06;
    Double_t lower_pad_sigbkg_topmargin_ = 0.12;
    Double_t lower_pad_topmargin_ = 0.03;
    Double_t lower_pad_botmargin_ = 0.27;
    Double_t linear_buffer_ = 1.4; //scale to y-axis maximum for y-axis range (linear-mode)
    Double_t log_buffer_ = 1.5; //scale to y-axis maximum for y-axis range (log-mode)
    //Legend parameters
    Double_t legend_txt_ = 0.05;
    Double_t legend_x1_stats_ = 0.11; //if stats in legend
    Double_t legend_x1_ = 0.11; //if no stats in legend
    Double_t legend_x2_ = 0.89;
    Double_t legend_y1_ = 0.93;
    Double_t legend_y2_ = 0.75;
    Double_t legend_sys_x1_ = 0.6;
    Double_t legend_sys_x2_ = 0.89;
    Double_t legend_sys_y1_ = 0.93;
    Int_t    legend_ncol_ = 3;
    // Double_t legend_y2_split_ = 0.45;
    Double_t legend_sep_ = 2.;
    //luminosity drawing
    Double_t lum_txt_x_ = 0.65;
    Double_t lum_txt_y_ = 0.98;
    Double_t lum_txt_x_single_ = 0.63;
    Double_t lum_txt_y_single_ = 0.975;
    //CMS prelim drawing
    Double_t cms_txt_x_ = 0.28;
    Double_t cms_txt_y_ = 0.97;
    Double_t cms_txt_x_single_ = 0.28; //text location without data/mc split pad
    Double_t cms_txt_y_single_ = 0.97;
    Double_t cms_txt_size_ = 0.06;
    Double_t cms_txt_size_single_ = 0.045; //text size without data/mc split pad
    Bool_t   cms_is_prelim_ = true; //add preliminary label

    //data yield drawing
    bool     draw_statistics_ = true;
    bool     draw_signal_yield_ = false;
    Int_t    signal_digits_ = 1;
    Double_t data_txt_y_ = 0.75;
    Double_t data_txt_x_ = 0.61;

    //significance drawing
    Double_t sig_plot_range_ = 3.5;
    Int_t limit_mc_err_range_ = 1; //sigma range to show
    Double_t limit_xmax_ = -1.; // range to define limit within
    Double_t limit_xmin_ =  1.; // range to define limit within

    //systematic plotting
    Bool_t single_systematic_   = false;
    Bool_t replace_missing_sys_ = true; //if a systematic histogram has 0 entries or is null, replace with the nominal histogram

    ~DataPlotter() {
      for(Data_t& input : inputs_) {
        if(input.file_ && input.file_->TestBit(TObject::kNotDeleted)) {
          input.file_->Close();
          delete input.file_;
          input.file_ = nullptr;
        }
      }
    }

    void draw_cms_label(bool single = false) {
      TText cmslabel;
      cmslabel.SetNDC();
      cmslabel.SetTextFont(72);
      cmslabel.SetTextColor(1);
      cmslabel.SetTextSize((single) ? cms_txt_size_single_ : cms_txt_size_);
      cmslabel.SetTextAlign(22);
      cmslabel.SetTextAngle(0);
      cmslabel.DrawText((single) ? cms_txt_x_single_ : cms_txt_x_,
                        (single) ? cms_txt_y_single_ : cms_txt_y_, (cms_is_prelim_) ? "CMS Preliminary" : "CMS");
    }

    void draw_luminosity(bool single = false) {
      TLatex label;
      label.SetNDC();
      label.SetTextFont(72);
      // label.SetTextColor(1);
      label.SetTextSize((single) ? 0.03 : .04);
      label.SetTextAlign(13);
      label.SetTextAngle(0);
      label.DrawLatex((single) ? lum_txt_x_single_ : lum_txt_x_,
                      (single) ? lum_txt_y_single_ : lum_txt_y_,
                      Form("L=%.1f fb^{-1} #sqrt{#it{s}} = %.0f TeV",lum_/1.e3,rootS_));
    }

    void draw_data(int ndata, double nmc, std::map<TString, double> nsig) {
      TLatex label;
      label.SetNDC();
      label.SetTextFont(72);
      // label.SetTextColor(1);
      label.SetTextSize(legend_txt_);
      label.SetTextAlign(13);
      label.SetTextAngle(0);
      label.DrawLatex(data_txt_x_, data_txt_y_, Form("%10s = %10i", "n_{Data}", ndata));
      double y = data_txt_y_;
      if(nmc > 0.) {
        y -= 0.045;
        label.DrawLatex(data_txt_x_, y, Form("%9s = %10.0f", "n_{MC}", nmc));
      }
      auto it = nsig.begin();
      while(draw_signal_yield_ && it != nsig.end()) {
        y -= 0.045;
        TString label_string = "%10s = %10.";
        label_string += signal_digits_;
        label_string += "f";
        label.DrawLatex(data_txt_x_, y, Form(label_string.Data(), Form("n_{%s}", it->first.Data()), it->second));
        it++;
      }

    }

    void reset_axes() {xMin_=1.e6; xMax_=-1.e6; yMax_=-1.e6; yMin_=1.e6;}

    //asks if anywhere in the given range is blinded (for histogram binning)
    bool isBlind(Double_t xmin, Double_t xmax) {
      bool blind = false;
      if(xmax < xmin) return blind;
      for(unsigned index = 0; index < blindxmin_.size(); ++index) {
        if((xmin >= blindxmin_[index] && xmin < blindxmax_[index]) || //if either edge of the bin is within the blinding, blind it
           (xmax > blindxmin_[index] && xmax <= blindxmax_[index])) {
          blind = true;
          break;
        }
      }
      return blind;
    }
    //asks if a value is blinded
    bool isBlind(Double_t value) {
      return isBlind(value,value);
    }

    void density(TH1* h) {
      for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
        if(h->GetBinContent(bin) <= 0.) continue;
        h->SetBinContent(bin, h->GetBinContent(bin)/h->GetBinWidth(bin));
        h->SetBinError(bin, h->GetBinError(bin)/h->GetBinWidth(bin));
      }
    }

    std::vector<TH1*> get_histograms(TString hist, TString setType, Int_t set, Int_t Mode, TString process = "", ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");
    enum{kBackground, kSignal, kData, kAny};

    std::vector<TH1*> get_signal(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");
    // TH2* get_signal_2D(TString hist, TString setType, Int_t set);

    TH1* get_data_mc_diff(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");

    TH1* get_data(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");
    TH2* get_data_2D(TString hist, TString setType, Int_t set);

    TH1* get_qcd(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");
    TH2* get_qcd_2D(TString hist, TString setType, Int_t set);

    TH1* get_misid(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");
    TH2* get_misid_2D(TString hist, TString setType, Int_t set);

    TH1* get_stack_uncertainty(THStack* hstack, TString hname);
    TGraphAsymmErrors* get_stack_systematic(THStack* hstack, const std::vector<std::pair<TString,TString>> hnames,
                                            const std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>>& scales,
                                            const TString hist, const TString type, const int set);

    THStack* get_stack(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale = nullptr, TString tag = "");
    TH2* get_background_2D(TString hist, TString setType, Int_t set);

    TH1* get_process(TString label, TString hist, TString setType, Int_t set);

    // TCanvas* plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label);
    // TCanvas* plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label,
    //                             Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    //   xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c =  plot_single_2Dhist(hist, setType, set, label);
    //   reset_axes(); return c;
    // }
    // TCanvas* plot_single_2Dhist(PlottingCard_t card) {
    //   rebinH_ = card.rebin_;
    //   return plot_single_2Dhist(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_, card.ymin_, card.ymax_);
    // }

    // TCanvas* plot_2Dhist(TString hist, TString setType, Int_t set);
    // TCanvas* plot_2Dhist(TString hist, TString setType, Int_t set,
    //                      Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) {
    //   xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = plot_2Dhist(hist, setType, set);
    //   reset_axes(); return c;
    // }
    // TCanvas* plot_2Dhist(PlottingCard_t card) {
    //   rebinH_ = card.rebin_;
    //   return plot_2Dhist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.ymin_, card.ymax_);
    // }


    // TCanvas* plot_hist(TString hist, TString setType, Int_t set);
    // TCanvas* plot_hist(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax) {
    //   xMin_ = xmin; xMax_=xmax; auto c = plot_hist(hist, setType, set); reset_axes(); return c;
    // }
    // TCanvas* plot_hist(PlottingCard_t card) {
    //   rebinH_ = card.rebin_;
    //   blindxmin_ = card.blindmin_;
    //   blindxmax_ = card.blindmax_;
    //   if(card.plot_data_ < 100)
    //     plot_data_ = card.plot_data_;
    //   if(card.data_over_mc_ < 100)
    //     data_over_mc_ = card.data_over_mc_;
    //   return plot_hist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_);
    // }

    TCanvas* plot_stack(TString hist, TString setType, Int_t set,
                        const std::vector<std::pair<TString,TString>> sys_names = {},
                        const std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>>& scale_sys = {});
    TCanvas* plot_stack(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax,
                        const std::vector<std::pair<TString,TString>> sys_names = {},
                        const std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>>& scale_sys = {}) {
      xMin_ = xmin; xMax_=xmax; auto c = plot_stack(hist, setType, set, sys_names, scale_sys); reset_axes(); return c;
    }
    TCanvas* plot_stack(PlottingCard_t card) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      density_plot_ = card.density_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      only_signal_ = card.label_;
      return plot_stack(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.sys_list_, card.scale_sys_list_);
    }

    TGraphAsymmErrors* get_errors(TH1* h, TH1* h_p, TH1* h_m, bool ratio, double& r_min, double& r_max);
    TGraphAsymmErrors* get_errors(TH1* h, TH1* h_p, TH1* h_m, bool ratio = false) {
      double tmp1, tmp2;
      return get_errors(h, h_p, h_m, ratio, tmp1, tmp2);
    }
    void combine_errors(TH1* h, TGraphAsymmErrors* g);
    TGraphAsymmErrors* get_ratio(TH1* h, TGraphAsymmErrors* g);

    TCanvas* plot_systematic(TString hist, Int_t set, Int_t systematic, ScaleUncertainty_t* sys_scale = nullptr);
    TCanvas* plot_systematic(TString hist, Int_t set, Int_t systematic, Double_t xmin, Double_t xmax, ScaleUncertainty_t* sys_scale = nullptr) {
      xMin_ = xmin; xMax_=xmax; auto c = plot_systematic(hist, set, systematic, sys_scale); reset_axes(); return c;
    }
    TCanvas* plot_systematic(TString hist, Int_t set, Int_t systematic, Double_t xmin, Double_t xmax, Double_t blind_min, Double_t blind_max, ScaleUncertainty_t* sys_scale = nullptr) {
      blindxmin_ = {blind_min}; blindxmax_= {blind_max}; return plot_systematic(hist, set, systematic, xmin, xmax, sys_scale);
    }
    TCanvas* plot_systematic(PlottingCard_t card, ScaleUncertainty_t* sys_scale = nullptr) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      only_signal_ = card.label_;
      single_systematic_ = card.single_systematic_;
      return plot_systematic(card.hist_, card.set_, card.systematic_, card.xmin_, card.xmax_, sys_scale);
    }

    TCanvas* plot_cdf(TString hist, TString setType, Int_t set, TString label);
    TCanvas* plot_cdf(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax) {
      xMin_ = xmin; xMax_=xmax; auto c = plot_cdf(hist, setType, set, label); reset_axes(); return c;
    }
    TCanvas* plot_cdf(PlottingCard_t card) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      return plot_cdf(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_);
    }

    TCanvas* plot_roc(TString hist, TString setType, Int_t set, bool cut_low = true);
    TCanvas* plot_roc(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax, bool cut_low = true) {
      xMin_ = xmin; xMax_=xmax; auto c = plot_roc(hist, setType, set, cut_low); reset_axes(); return c;
    }
    TCanvas* plot_roc(PlottingCard_t card, bool cut_low = true) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      return plot_roc(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, cut_low);
    }


    TCanvas* plot_significance(TString hist, TString setType, Int_t set, TString label, bool dir,
                                       Double_t line_val, bool doVsEff, TString label1, TString label2);

    TCanvas* plot_significance(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax,
                               bool dir=true, Double_t line_val=-1., bool doVsEff = false, TString label1 = "", TString label2 = "") {
      xMin_ = xmin; xMax_=xmax;
      auto c = plot_significance(hist, setType, set, label, dir, line_val, doVsEff, label1, label2);
      reset_axes(); return c;
    }

    TCanvas* print_stack(TString hist, TString setType, Int_t set, TString tag = "", std::vector<std::pair<TString,TString>> sys = {},
                         const std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>>& scale_sys = {});
    TCanvas* print_stack(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax, TString tag = "", std::vector<std::pair<TString,TString>> sys = {},
                         const std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>>& scale_sys = {}) {
      xMin_ = xmin; xMax_=xmax; auto c = print_stack(hist, setType, set, tag, sys, scale_sys); reset_axes(); return c;
    }
    TCanvas* print_stack(PlottingCard_t card) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      density_plot_ = card.density_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      only_signal_ = card.label_;
      return print_stack(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.tag_, card.sys_list_, card.scale_sys_list_);
    }

    TCanvas* print_systematic(TString hist, Int_t set, Int_t systematic, TString tag = "", ScaleUncertainty_t* sys_scale = nullptr);
    TCanvas* print_systematic(TString hist, Int_t set, Int_t systematic, Double_t xmin, Double_t xmax, TString tag = "", ScaleUncertainty_t* sys_scale = nullptr) {
      xMin_ = xmin; xMax_=xmax; auto c = print_systematic(hist, set, systematic, tag, sys_scale); reset_axes(); return c;
    }
    TCanvas* print_systematic(PlottingCard_t card, ScaleUncertainty_t* sys_scale = nullptr) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      only_signal_ = card.label_;
      single_systematic_ = card.single_systematic_;
      return print_systematic(card.hist_, card.set_, card.systematic_, card.xmin_, card.xmax_, card.tag_, sys_scale);
    }

    // TCanvas* print_hist(TString hist, TString setType, Int_t set, TString tag = "");
    // TCanvas* print_hist(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax, TString tag = "") {
    //   xMin_ = xmin; xMax_=xmax; auto c = print_hist(hist, setType, set, tag); reset_axes(); return c;
    // }
    // TCanvas* print_hist(PlottingCard_t card) {
    //   rebinH_ = card.rebin_;
    //   blindxmin_ = card.blindmin_;
    //   blindxmax_ = card.blindmax_;
    //   if(card.plot_data_ < 100)
    //     plot_data_ = card.plot_data_;
    //   if(card.data_over_mc_ < 100)
    //     data_over_mc_ = card.data_over_mc_;
    //   return print_hist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.tag_);
    // }

    // TCanvas* print_2Dhist(TString hist, TString setType, Int_t set, TString tag = "");
    // TCanvas* print_2Dhist(TString hist, TString setType, Int_t set,
    //                       Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax, TString tag = "") {
    //   xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = print_2Dhist(hist, setType, set, tag); reset_axes(); return c;
    // }
    // TCanvas* print_2Dhist(PlottingCard_t card) {
    //   rebinH_ = card.rebin_;
    //   return print_2Dhist(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, card.ymin_, card.ymax_, card.tag_);
    // }

    // TCanvas* print_single_2Dhist(TString hist, TString setType, Int_t set, TString label, TString tag = "");
    // TCanvas* print_single_2Dhist(TString hist, TString setType, Int_t set, TString label,
    //                              Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax, TString tag = "") {
    //   xMin_ = xmin; xMax_=xmax; yMin_ = ymin; yMax_=ymax; auto c = print_single_2Dhist(hist, setType, set, label, tag); reset_axes(); return c;
    // }
    // TCanvas* print_single_2Dhist(PlottingCard_t card) {
    //   rebinH_ = card.rebin_;
    //   if(card.plot_data_ < 100)
    //     plot_data_ = card.plot_data_;
    //   return print_single_2Dhist(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_, card.ymin_, card.ymax_, card.tag_);
    // }

    TCanvas* print_cdf(TString hist, TString setType, Int_t set, TString label, TString tag = "");
    TCanvas* print_cdf(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax, TString tag = "") {
      xMin_ = xmin; xMax_=xmax; auto c = print_cdf(hist, setType, set, label, tag); reset_axes(); return c;
    }
    TCanvas* print_cdf(PlottingCard_t card) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      return print_cdf(card.hist_, card.type_, card.set_, card.label_, card.xmin_, card.xmax_, card.tag_);
    }

    TCanvas* print_roc(TString hist, TString setType, Int_t set, bool cut_low = false, TString tag = "");
    TCanvas* print_roc(TString hist, TString setType, Int_t set, Double_t xmin, Double_t xmax, bool cut_low = false, TString tag = "") {
      xMin_ = xmin; xMax_=xmax; auto c = print_roc(hist, setType, set, cut_low, tag); reset_axes(); return c;
    }
    TCanvas* print_roc(PlottingCard_t card, bool cut_low = false) {
      rebinH_ = card.rebin_;
      blindxmin_ = card.blindmin_;
      blindxmax_ = card.blindmax_;
      if(card.plot_data_ < 100)
        plot_data_ = card.plot_data_;
      if(card.data_over_mc_ < 100)
        data_over_mc_ = card.data_over_mc_;
      return print_roc(card.hist_, card.type_, card.set_, card.xmin_, card.xmax_, cut_low, card.tag_);
    }

    TCanvas* print_significance(TString hist, TString setType, Int_t set, TString label, bool dir = false,
                                        Double_t line_val = -1., bool doVsEff = false, TString label1 = "",
                                        TString label2 = "", TString tag = "");
    TCanvas* print_significance(TString hist, TString setType, Int_t set, TString label, Double_t xmin, Double_t xmax,
                                bool dir=true, Double_t line_val=-1., bool doVsEff = false, TString label1 = "",
                                TString label2 = "", TString tag = "") {
      xMin_ = xmin; xMax_=xmax; auto c = print_significance(hist, setType, set, label, dir, line_val, doVsEff, label1, label2, tag); reset_axes(); return c;
    }

    Int_t print_stacks(std::vector<TString> hists, std::vector<TString> setTypes, std::vector<Int_t>sets,
                               std::vector<Double_t> xMaxs, std::vector<Double_t> xMins, std::vector<Int_t> rebins
                               , std::vector<Double_t> signal_scales, std::vector<Int_t> base_rebins);

    Int_t print_stacks(std::vector<PlottingCard_t> cards, std::vector<Int_t> sets, std::vector<Double_t> signal_scales, std::vector<Int_t> base_rebins) {
      Int_t status = 0;
      for(unsigned index = 0; index < sets.size(); ++index) {
        if(signal_scales.size() == sets.size()) signal_scale_ = signal_scales[index];
        for(PlottingCard_t card : cards) {
          if(base_rebins.size() == sets.size()) card.rebin_ *= base_rebins[index];
          card.set_ = sets[index];
          auto c = print_stack(card);
          status += (c) ? 0 : 1;
          if(verbose_ > 0 || status) printf("Printing Data/MC stack %s %s set %i has status %i\n",card.type_.Data(),card.hist_.Data(),card.set_,status);
          Empty_Canvas(c);
        }
      }
      return status;
    }

    // Int_t print_hists(std::vector<TString> hists, std::vector<TString> setTypes, std::vector<Int_t> sets,
    //                           std::vector<Double_t> xMaxs, std::vector<Double_t> xMins, std::vector<Int_t> rebins
    //                           , std::vector<Double_t> signal_scales, std::vector<Int_t> base_rebins);

    // Int_t print_hists(std::vector<PlottingCard_t> cards, std::vector<Int_t> sets, std::vector<Double_t> signal_scales, std::vector<Int_t> base_rebins) {
    //   Int_t status = 0;
    //   for(unsigned index = 0; index < sets.size(); ++index) {
    //     if(signal_scales.size() == sets.size()) signal_scale_ = signal_scales[index];
    //     for(PlottingCard_t card : cards) {
    //       if(base_rebins.size() == sets.size()) card.rebin_ *= base_rebins[index];
    //       card.set_ = sets[index];
    //       auto c = print_hist(card);
    //       status += (c) ? 0 : 1;
    //       Empty_Canvas(c);
    //     }
    //   }
    //   return status;
    // }

    //get the default figure name
    TString GetFigureName(TString type, TString hist, int set, TString figureType, TString tag = "", bool noDataDefined = false) {
      TString year_dir = "";
      for(unsigned index = 0; index < years_.size(); ++index) {
        year_dir += years_[index];
        if(index != years_.size() - 1) year_dir += "_";
      }
      TString name;
      name = Form("figures/%s/%s/%s/%s_%s_%s",
                  folder_.Data(),selection_.Data(), year_dir.Data(),
                  figureType.Data(), //e.g. stack, hist, cdf, etc.
                  type.Data(), hist.Data());

      if(logY_)                                name += "_log";
      if(figureType != "signal") {
        if(!noDataDefined && plot_data_)         name += "_data";
        if(!noDataDefined && stack_as_hist_)     name += "_totbkg";
        if(!noDataDefined && data_over_mc_ < 0)  name += "_sigOverBkg";
        else if(!noDataDefined && data_over_mc_) name += "_dataOverMC";
        if(density_plot_)                        name += "_density";
        if(!include_qcd_)                        name += "_noqcd";
        if(!include_misid_)                      name += "_nomisid";
        if(tag != "")                            name += "_" + tag;
      }

      name += Form("_set_%i.%s", set, figure_format_.Data());
      return name;
    }

    //static function to delete all objects in a canvas
    static Int_t Empty_Canvas(TCanvas* c) {
      if(!c) return 0;
      TList* list = c->GetListOfPrimitives();
      if(!list) return 1;
      for(auto o : *list) {
        if(o->InheritsFrom("TPad")) {
          auto pad = (TPad*) o;
          if(!pad) continue;
          TList* pad_list = pad->GetListOfPrimitives();
          for(auto h : *pad_list) {
            if(h->InheritsFrom("THStack")) {
              THStack* hstack = (THStack*) h;
              TList* hist_list = hstack->GetHists();
              for(auto hl : *hist_list) {
                delete hl;
              }
            }
            delete h;
          }
        }
        delete o;
      }
      if(c) delete c;
      return 0;
    }

    //Load files
    Int_t init_files();

    //Add file to be read and plotted
    Int_t add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal, bool isEmbed, int color);
    Int_t add_dataset(DataCard_t card) {
      if(card.color_ > 0 && !card.isdata_) {
        //set color
        unsigned entrynumber = 0;
        std::map<TString, bool> foundLabels; //record which labels have been found, as unique is what matters
        for(unsigned index = 0; index < inputs_.size(); ++index) {
          Data_t& input = inputs_[index];
          if(input.label_ == card.label_) break; //if found the label, save this entry number and exit
          if(input.isSignal_ == card.issignal_ && !foundLabels[input.label_]) {
            foundLabels[input.label_] = true; //label has appeared (only count unique labels)
            ++entrynumber; //if is signal matches, increment
          }
        }
        if(card.issignal_) signal_colors_    [entrynumber] = card.color_;
        else               background_colors_[entrynumber] = card.color_;
      }
      return add_dataset(card.filename_, card.name_, card.label_, card.isdata_, card.xsec_, card.issignal_, card.isembed_, card.color_);
    }

    void set_luminosity(double lum) { lum_ = lum;}

    void set_selection(TString selec) { selection_ = selec;}

    ClassDef(DataPlotter,0);
  };
}
#endif
