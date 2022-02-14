#ifndef __EMBEDDINGTNPWEIGHT__
#define __EMBEDDINGTNPWEIGHT__
//Class to handle embedding scale factors from local TnP calculations

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TSystem.h"

//local includes
#include "interface/Utilities.hh"

namespace CLFV {

  class EmbeddingTnPWeight {
  public:
    EmbeddingTnPWeight(int Mode = 0, int verbose = 0) : verbose_(verbose) {

      interpolate_ = Mode % 10 == 1;

      TFile* f = 0;
      TString cmssw = gSystem->Getenv("CMSSW_BASE");
      TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
      for(int year = k2016; year <= k2018; ++year) {
        std::vector<TString> files = {Form("%s/embedding_eff_mumu_mode-0_%i.root", path.Data(), year + 2016),
                                      Form("%s/embedding_eff_mumu_mode-1_%i.root", path.Data(), year + 2016),
                                      Form("%s/embedding_eff_mumu_mode-2_%i.root", path.Data(), year + 2016),
                                      Form("%s/embedding_eff_ee_mode-0_%i.root"  , path.Data(), year + 2016),
                                      Form("%s/embedding_eff_ee_mode-1_%i.root"  , path.Data(), year + 2016)};
        for(TString file : files) {
        //Get muon trigger scale factors
          f = TFile::Open(file.Data(), "READ");
          if(!f) {
            std::cout << "!!! Embedding corrections file " << file.Data() << " for " << year + 2016 << " not found!\n";
          } else {
            TH2F* hData = (TH2F*) f->Get("PtVsEtaData");
            TH2F* hMC   = (TH2F*) f->Get("PtVsEtaMC"  );
            TH2F* hSF   = (TH2F*) f->Get("PtVsEtaSF"  );
            if(!hData || !hMC || !hSF) {
              std::cout << "!!! Efficiency histograms not found in " << file.Data() << std::endl;
            } else {
              hData->SetName(Form("%s_%s", hData->GetName(), hData->GetTitle()));
              hMC  ->SetName(Form("%s_%s", hMC  ->GetName(), hMC  ->GetTitle()));
              hSF  ->SetName(Form("%s_%s", hSF  ->GetName(), hSF  ->GetTitle()));
              if(file.Contains("_mumu_")) { //muon files
                if(file.Contains("mode-0")) { //trigger files
                  muonTrigDataEff_[year] = hData;
                  muonTrigMCEff_  [year] = hMC  ;
                } else if(file.Contains("mode-1")) { //ID files
                  muonIDDataEff_[year] = hData;
                  muonIDMCEff_  [year] = hMC  ;
                } else if(file.Contains("mode-2")) { //Iso ID files
                  muonIsoIDDataEff_[year] = hData;
                  muonIsoIDMCEff_  [year] = hMC  ;
                }
              } else { //electron files
                if(file.Contains("mode-0")) { //trigger files
                  electronTrigDataEff_[year] = hData;
                  electronTrigMCEff_  [year] = hMC  ;
                } else if(file.Contains("mode-1")) { //ID files
                  electronIDDataEff_[year] = hData;
                  electronIDMCEff_  [year] = hMC  ;
                }
              }
            }
            files_.push_back(f);
          }
        }
      }
    }

    ~EmbeddingTnPWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }


    double MuonIDWeight(double pt, double eta, int year) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }

      const TH2F* hIDMC   = muonIDMCEff_  [year];
      const TH2F* hIDData = muonIDDataEff_[year];
      if(!hIDMC || !hIDData) {
        std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": ID histograms not found!\n";
        return 1.;
      }

      const TH2F* hIsoIDMC   = muonIsoIDMCEff_  [year];
      const TH2F* hIsoIDData = muonIsoIDDataEff_[year];
      if(!hIsoIDMC || !hIsoIDData) {
        std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Iso ID histograms not found!\n";
        return 1.;
      }

      double scale_factor(1.);
      pt = std::max(10.01, std::min(pt, 499.));
      eta = std::min(2.39, fabs(eta));

      ///////////////////////////
      // Apply ID weight
      ///////////////////////////
      double data_eff(1.), mc_eff(1.);
      if(interpolate_) {
        //Perform a linear interpolation
        data_eff = Utilities::Interpolate(hIDData, eta, pt, Utilities::kYAxis);
        mc_eff   = Utilities::Interpolate(hIDMC  , eta, pt, Utilities::kYAxis);
      } else {
        data_eff = hIDData->GetBinContent(hIDData->GetXaxis()->FindBin(eta), hIDData->GetYaxis()->FindBin(pt));
        mc_eff   = hIDMC  ->GetBinContent(hIDMC  ->GetXaxis()->FindBin(eta), hIDMC  ->GetYaxis()->FindBin(pt));
      }
      scale_factor *= data_eff / mc_eff;

      ///////////////////////////
      // Apply Iso ID weight
      ///////////////////////////
      if(interpolate_) {
        //Perform a linear interpolation
        data_eff = Utilities::Interpolate(hIsoIDData, eta, pt, Utilities::kYAxis);
        mc_eff   = Utilities::Interpolate(hIsoIDMC  , eta, pt, Utilities::kYAxis);
      } else {
        data_eff = hIsoIDData->GetBinContent(hIsoIDData->GetXaxis()->FindBin(eta), hIsoIDData->GetYaxis()->FindBin(pt));
        mc_eff   = hIsoIDMC  ->GetBinContent(hIsoIDMC  ->GetXaxis()->FindBin(eta), hIsoIDMC  ->GetYaxis()->FindBin(pt));
      }
      scale_factor *= data_eff / mc_eff;

      if(scale_factor <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      return scale_factor;
    }

    double MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
      year -= 2016;
      data_eff = 0.5; mc_eff = 0.5;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }

      //If it can't fire the trigger, no correction
      if(pt < 25. || (pt < 28. && year == k2017)) return 1.;

      const TH2F* hMC   = muonTrigMCEff_  [year];
      const TH2F* hData = muonTrigDataEff_[year];
      if(!hMC || !hData) {
        std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Trigger histograms not found!\n";
        return 1.;
      }

      double scale_factor(1.);
      pt = std::max(10.01, std::min(pt, 499.));
      eta = std::min(2.39, fabs(eta)); //std::max(-2.39, std::min(2.39, eta));

      ///////////////////////////
      // Apply trigger weight
      ///////////////////////////

      if(interpolate_) {
        //Perform a linear interpolation
        data_eff = Utilities::Interpolate(hData, eta, pt, Utilities::kYAxis);
        mc_eff   = Utilities::Interpolate(hMC  , eta, pt, Utilities::kYAxis);
      } else {
        data_eff = hData->GetBinContent(hData->GetXaxis()->FindBin(eta), hData->GetYaxis()->FindBin(pt));
        mc_eff   = hMC  ->GetBinContent(hMC  ->GetXaxis()->FindBin(eta), hMC  ->GetYaxis()->FindBin(pt));
      }
      scale_factor = data_eff / mc_eff;

      if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__
                  << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
                  << " pt = " << pt << " eta = " << eta << " year = " << year + 2016
                  << ", returning 1" << std::endl;
        //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
        mc_eff = 0.5;
        data_eff = 0.5;
        return 1.;
      }
      return scale_factor;
    }

    double ElectronIDWeight(double pt, double eta, int year) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }

      const TH2F* hIDMC   = electronIDMCEff_  [year];
      const TH2F* hIDData = electronIDDataEff_[year];
      if(!hIDMC || !hIDData) {
        std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": ID histograms not found!\n";
        return 1.;
      }

      double scale_factor(1.);
      pt = std::max(10.01, std::min(pt, 499.));
      eta = std::min(2.49, fabs(eta)); //std::max(-2.49, std::min(2.49, eta));

      ///////////////////////////
      // Apply ID weight
      ///////////////////////////
      double data_eff(1.), mc_eff(1.);
      if(interpolate_) {
        //Perform a linear interpolation
        data_eff = Utilities::Interpolate(hIDData, eta, pt, Utilities::kYAxis);
        mc_eff   = Utilities::Interpolate(hIDMC  , eta, pt, Utilities::kYAxis);
      } else {
        data_eff = hIDData->GetBinContent(hIDData->GetXaxis()->FindBin(eta), hIDData->GetYaxis()->FindBin(pt));
        mc_eff   = hIDMC  ->GetBinContent(hIDMC  ->GetXaxis()->FindBin(eta), hIDMC  ->GetYaxis()->FindBin(pt));
      }
      scale_factor *= data_eff / mc_eff;

      if(scale_factor <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      return scale_factor;
    }

    double ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
      year -= 2016;
      data_eff = 0.5; mc_eff = 0.5;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }

      //IF it can't fire the trigger, no correction
      if(pt < 28. || (pt < 33. && year != k2016)) return 1.;

      const TH2F* hMC   = electronTrigMCEff_  [year];
      const TH2F* hData = electronTrigDataEff_[year];
      if(!hMC || !hData) {
        std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Trigger histograms not found!\n";
        return 1.;
      }

      double scale_factor(1.);
      pt = std::max(10.01, std::min(pt, 499.));
      eta = std::min(2.49, fabs(eta)); //std::max(-2.49, std::min(2.49, eta));

      ///////////////////////////
      // Apply trigger weight
      ///////////////////////////

      if(interpolate_) {
        //Perform a linear interpolation
        data_eff = Utilities::Interpolate(hData, eta, pt, Utilities::kYAxis);
        mc_eff   = Utilities::Interpolate(hMC  , eta, pt, Utilities::kYAxis);
      } else {
        data_eff = hData->GetBinContent(hData->GetXaxis()->FindBin(eta), hData->GetYaxis()->FindBin(pt));
        mc_eff   = hMC  ->GetBinContent(hMC  ->GetXaxis()->FindBin(eta), hMC  ->GetYaxis()->FindBin(pt));
      }
      scale_factor = data_eff / mc_eff;

      if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__
                  << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
                  << " pt = " << pt << " eta = " << eta << " year = " << year + 2016
                  << ", returning 1" << std::endl;
        //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
        mc_eff = 0.5;
        data_eff = 0.5;
        return 1.;
      }
      return scale_factor;
    }


  public:
    enum { k2016, k2017, k2018};
    std::vector<TFile*> files_;
    std::map<int, TH2F*> muonTrigDataEff_;
    std::map<int, TH2F*> muonTrigMCEff_;
    std::map<int, TH2F*> muonIDDataEff_;
    std::map<int, TH2F*> muonIDMCEff_;
    std::map<int, TH2F*> muonIsoIDDataEff_;
    std::map<int, TH2F*> muonIsoIDMCEff_;
    std::map<int, TH2F*> electronTrigDataEff_;
    std::map<int, TH2F*> electronTrigMCEff_;
    std::map<int, TH2F*> electronIDDataEff_;
    std::map<int, TH2F*> electronIDMCEff_;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    bool interpolate_;
  };
}
#endif
