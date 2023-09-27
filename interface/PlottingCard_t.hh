#ifndef __PLOTTINGCARD__
#define __PLOTTINGCARD__
#include "TString.h"
#include <vector>
#include "interface/ScaleUncertainty_t.hh"

namespace CLFV {

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
    std::vector<Double_t> blindmin_;
    std::vector<Double_t> blindmax_;
    Int_t    plot_data_;
    Int_t    data_over_mc_;
    TString  tag_;
    Int_t    systematic_;
    Bool_t   single_systematic_;
    Bool_t   density_;
    std::vector<std::pair<TString,TString>> sys_list_;
    std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>> scale_sys_list_;

    PlottingCard_t() : hist_(""), type_(""), label_(""), set_(-1),
                       xmin_(1.), xmax_(-1.), ymin_(1.), ymax_(-1.), rebin_(1),
                       plot_data_(999), data_over_mc_(999), tag_(""), systematic_(0),
                       single_systematic_(false), density_(false), sys_list_({}), scale_sys_list_({}) {}

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
    //only 1 blinding range
    PlottingCard_t(TString hist, TString type, Int_t rebin, Double_t xmin, Double_t xmax,
                   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,rebin,xmin,xmax) {
      blindmin_.push_back(blindmin);
      blindmax_.push_back(blindmax);
    }
    PlottingCard_t(TString hist, TString type, Int_t set, Int_t rebin, Double_t xmin, Double_t xmax,
                   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,set,rebin,xmin,xmax) {
      blindmin_.push_back(blindmin);
      blindmax_.push_back(blindmax);
    }
    //multiple blinding ranges
    PlottingCard_t(TString hist, TString type, Int_t rebin, Double_t xmin, Double_t xmax,
                   std::vector<Double_t> blindmin, std::vector<Double_t> blindmax) : PlottingCard_t(hist,type,rebin,xmin,xmax) {
      blindmin_ = blindmin;
      blindmax_ = blindmax;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, Int_t rebin, Double_t xmin, Double_t xmax,
                   std::vector<Double_t> blindmin, std::vector<Double_t> blindmax) : PlottingCard_t(hist,type,set,rebin,xmin,xmax) {
      blindmin_ = blindmin;
      blindmax_ = blindmax;
    }
    PlottingCard_t(TString hist, TString type, TString label, Int_t rebin, Double_t xmin, Double_t xmax,
                   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,rebin,xmin,xmax,blindmin,blindmax) {
      label_ = label;
    }
    PlottingCard_t(TString hist, TString type, TString label, Int_t rebin, Double_t xmin, Double_t xmax,
                   std::vector<Double_t> blindmin, std::vector<Double_t> blindmax) : PlottingCard_t(hist,type,rebin,xmin,xmax,blindmin,blindmax) {
      label_ = label;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, TString label, Int_t rebin, Double_t xmin, Double_t xmax,
                   Double_t blindmin, Double_t blindmax) : PlottingCard_t(hist,type,set,rebin,xmin,xmax,blindmin,blindmax) {
      label_ = label;
    }
    //for plotting density distributions if using variable width plots
    PlottingCard_t(TString hist, TString type, Int_t set, Int_t rebin, Double_t xmin, Double_t xmax,
                   Double_t blindmin, Double_t blindmax, bool density) : PlottingCard_t(hist,type,set,rebin,xmin,xmax,blindmin,blindmax) {
      density_ = density;
    }
    PlottingCard_t(TString hist, TString type, Int_t set, TString label, Int_t rebin, Double_t xmin, Double_t xmax,
                   Double_t blindmin, Double_t blindmax, bool density) : PlottingCard_t(hist,type,set,label,rebin,xmin,xmax,blindmin,blindmax) {
      density_ = density;
    }
  };
}
#endif
