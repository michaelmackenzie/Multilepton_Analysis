#ifndef __MVACONFIG__HH
#define __MVACONFIG__HH
//A tool to store MVA information

//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TString.h"
#include "TH1.h"
#include "TF1.h"
#include "TSystem.h"
#include "TFile.h"

namespace CLFV {
  class MVAConfig {
  public:
    MVAConfig(int useCDF = 0, int use_cdf_fit = 0, int use_xgboost = 0);

    //-------------------------------------------------------------------------------------------------
    ~MVAConfig() {
      for(auto cdf : cdf_) {
        if(cdf.second) {delete cdf.second; cdf_[cdf.first] = nullptr;}
      }
    }

    //-------------------------------------------------------------------------------------------------
    //apply the cdf transform to the BDT score for a given selection
    float CDFTransform(const float score, const TString selection) {
      if(cdf_.find(selection) == cdf_.end()) return score; //don't transform if not defined
      TH1* hcdf = cdf_[selection];
      if(!hcdf) return score;
      const float max_p = 0.9999f; //no 100% p-value for binning overflow
      const int bin = std::max(1, std::min(hcdf->GetNbinsX(), hcdf->FindBin(score)));
      const float p = std::min(max_p, (float) hcdf->GetBinContent(bin));

      //use fit function if enabled and below the p-value threshold
      const float p_fit_switch(5.e-3f);
      if(use_cdf_fit_ && p < p_fit_switch) {
        TF1* f = cdf_fit_[selection];
        if(!f) return p;
        const float p_fit = f->Eval(score);
        return std::max(0.f, std::min(2.f*p_fit_switch, p_fit)); //return the fit value, ensuring a physical value that's reasonable for the tail
      }
      return p;
    }

    //-------------------------------------------------------------------------------------------------
    //get MVA output selection name
    TString GetSelectionByIndex(Int_t index) {
      if     (index == 0) return "hmutau";
      else if(index == 1) return "zmutau";
      else if(index == 2) return "hetau";
      else if(index == 3) return "zetau";
      else if(index == 4) return "hemu";
      else if(index == 5) return "zemu";
      else if(index == 6) return "hmutau_e";
      else if(index == 7) return "zmutau_e";
      else if(index == 8) return "hetau_mu";
      else if(index == 9) return "zetau_mu";
      return "UNKNOWN";
    }

    //-------------------------------------------------------------------------------------------------
    //get MVA output index by selection
    Int_t GetIndexBySelection(TString selection) {
      for(Int_t index = 0; index < 10; ++index) {
        if(selection == GetSelectionByIndex(index)) return index;
      }
      return -1;
    }

    //-------------------------------------------------------------------------------------------------
    // Nominal binnings of the MVA score distributions
    std::vector<Double_t> Bins(Int_t index, Int_t HistSet = 8);

    //-------------------------------------------------------------------------------------------------
    // binning assuming the use of the CDF transform
    std::vector<Double_t> CDFBins(Int_t index, Int_t HistSet = 8);

    //-------------------------------------------------------------------------------------------------
    Int_t NBins(Int_t index, Int_t HistSet = 8) {
      auto bins = Bins(index, HistSet);
      const Int_t nbins = bins.size() - 1;
      return nbins;
    }

  public:
    std::vector<TString> names_; //MVA names
    std::vector<int> data_cat_; //relevant data category
    std::map<TString, std::vector<double>> categories_; //MVA categories by selection
    std::map<TString, TH1*> cdf_;
    std::map<TString, TF1*> cdf_fit_;
    int useCDF_; //approximate CDF transform
    int use_cdf_fit_; //use fit to the CDF tail for low probability values
    int use_xgboost_; //use XGBoost BDT in Z->e+mu
  };
}
#endif
