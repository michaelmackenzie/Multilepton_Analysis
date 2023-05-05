#ifndef __MVACONFIG__HH
#define __MVACONFIG__HH
//A tool to store MVA information

//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TString.h"
#include "TH1.h"
#include "TSystem.h"

namespace CLFV {
  class MVAConfig {
  public:
    MVAConfig(bool useCDF = false) {
      useCDF_ = useCDF;
      //initialize MVA names
      names_ = {
                //0 - 9: nominal mvas, even are Higgs, odd are Z
                "mutau_BDT_MM_8.higgs","mutau_BDT_MM_8.Z0",
                "etau_BDT_MM_8.higgs","etau_BDT_MM_8.Z0",
                "emu_BDT_MM_8.higgs","emu_BDT_MM_8.Z0",
                "mutau_e_BDT_MM_8.higgs","mutau_e_BDT_MM_8.Z0" ,
                "etau_mu_BDT_MM_8.higgs","etau_mu_BDT_MM_8.Z0"//,
      };

      //initialize MVA output categories
      if(useCDF) {
        categories_["hmutau"  ] = {0.05, 0.10, 0.25, 0.50};
        categories_["zmutau"  ] = {0.05, 0.10, 0.25, 0.50};
        categories_["hetau"   ] = {0.05, 0.10, 0.25, 0.50};
        categories_["zetau"   ] = {0.05, 0.10, 0.25, 0.50};
        categories_["hemu"    ] = {0.05, 0.10, 0.25, 0.50};
        // categories_["zemu"    ] = {0.050, 0.100, 0.200, 0.250}; //defined with a cut-and-count sqrt(S)/B in a single category (top one), others added as filler
        categories_["zemu"    ] = {-1.0, -0.08, 0.00, 0.04}; //defined with a cut-and-count sqrt(S)/B quadrature sum for (top) 3 categories in M in 86 - 96 GeV/c^2
        // categories_["zemu"    ] = {-1.0, 0.075, 0.300, 0.675}; //(CDF) defined with a cut-and-count sqrt(S)/B quadrature sum for (top) 3 categories in M in 86 - 96 GeV/c^2
        categories_["hmutau_e"] = {0.05, 0.10, 0.25, 0.50};
        categories_["zmutau_e"] = {0.05, 0.10, 0.25, 0.50};
        categories_["hetau_mu"] = {0.05, 0.10, 0.25, 0.50};
        categories_["zetau_mu"] = {0.05, 0.10, 0.25, 0.50};
      } else {
        categories_["hmutau"  ] = {-0.05680,  0.02900,  0.13370,  0.26540};
        categories_["zmutau"  ] = {-0.28180, -0.15400, -0.04330,  0.12920};
        categories_["hetau"   ] = {-0.06700,  0.04130,  0.15380,  0.33350};
        categories_["zetau"   ] = {-0.25540, -0.11290, -0.03850,  0.05990};
        categories_["hemu"    ] = {-0.05770, -0.015  , -0.015  ,  0.085  }; //defined with cut-and-count in M in [M_B - 5, M_B + 5]
        categories_["zemu"    ] = {-0.21800, -0.080  , -0.005  ,  0.060  }; //defined with cut-and-count in M in [M_B - 5, M_B + 5]
        categories_["hmutau_e"] = {-0.10660,  0.00650,  0.08600,  0.15860};
        categories_["zmutau_e"] = {-0.12310, -0.09790, -0.05830,  0.07190};
        categories_["hetau_mu"] = {-0.12820, -0.00400,  0.08060,  0.16160};
        categories_["zetau_mu"] = {-0.32500, -0.15460, -0.02680,  0.09410};
      }

      for(unsigned imva = 0; imva < names_.size(); ++imva) {
        TString name = names_[imva];
        int cat = -1;
        if(name.Contains("mutau_e") || name.Contains("etau_mu") || name.Contains("emu")) cat = 0; //emu data
        else if(name.Contains("mutau")) cat = 1; //mutau data
        else if(name.Contains("etau")) cat = 2; //etau data
        data_cat_.push_back(cat);
      }

      //initialize signal CDF transform histograms
      const TString cmssw = gSystem->Getenv("CMSSW_BASE");
      const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
      std::vector<TString> selections = {"zmutau", "zetau", "zmutau_e", "zetau_mu", "zemu"};
      for(TString selection : selections) {
        TFile* fcdf = TFile::Open(Form("%s/cdf_transform_%s.root", path.Data(), selection.Data()), "READ");
        if(!fcdf) continue;
        TH1* hcdf = (TH1*) fcdf->Get("CDF");
        if(!hcdf) {
          printf("MVAConfig: Failed to find %s CDF histogram\n", selection.Data());
          fcdf->Close();
          continue;
        }
        hcdf =  (TH1*) hcdf->Clone(Form("CDF_%s", selection.Data()));
        hcdf->SetDirectory(0);
        fcdf->Close();
        cdf_[selection] = hcdf;
      }
    }

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
    std::vector<Double_t> Bins(Int_t index, Int_t HistSet = 8) {
      if(useCDF_) return CDFBins(index, HistSet);
      TString selection = GetSelectionByIndex(index);
      std::vector<Double_t> bins;
      HistSet = HistSet % 100; //get the base set number
      //-------------------------------------------------------------------------------
      if       (selection == "hmutau") {
        double edges[] = {-1.00, -0.80,
                          -0.60, -0.50, -0.40, -0.30, -0.25,
                          -0.20, -0.15, -0.10, -0.05, -0.01,
                          +0.02,  0.05,  0.08,  0.11,  0.14,
                          +0.17,  0.20,
                          +0.50,  2.00};
        for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
          bins.push_back(edges[bin]);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "zmutau") {
        if(HistSet == 26) { //high mass region, so low scores
          double edges[] = {-1.00, -0.80, -0.55, -0.50, -0.45,
                            -0.40, -0.35, -0.30, -0.25, -0.20,
                            -0.15, -0.10, +0.30,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region, so low scores
          double edges[] = {-1.00, -0.80, -0.50, -0.45, -0.40,
                            -0.35, -0.30, -0.25, -0.20, -0.15,
                            -0.10, -0.05, +0.00, +0.05, +0.10,
                            +0.30,
                            +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region, high scores
          //v12 binning, 11/18/22-01/13/23
          double edges[] = {-1.00, -0.80, -0.70,
                            -0.40, -0.35, -0.30, -0.26, -0.22,
                            -0.20, -0.18, -0.16, -0.14, -0.12,
                            -0.10, -0.08, -0.06, -0.04, -0.02,
                            +0.00, +0.02, +0.04, +0.06, +0.08,
                            +0.10, +0.12, +0.14, +0.16, +0.18,
                            +0.20,
                            +0.35, +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 28) { //Z->mumu region, high scores
          double edges[] = {-1.00, -0.80, -0.70,
                            -0.35, -0.30, -0.25,
                            -0.20, -0.18, -0.16, -0.14, -0.12,
                            -0.10, -0.08, -0.06, -0.04, -0.02,
                            +0.00, +0.02, +0.04, +0.06, +0.08,
                            +0.10, +0.12, +0.14, +0.16, +0.18,
                            +0.35, +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else { //default, nominal score distribution
          //v12 binning, 11/18/22-01/13/23
          double edges[] = {-1.00, -0.80, -0.55,
                            -0.50, -0.45, -0.40, -0.35, -0.30,
                            -0.25, -0.20, -0.18, -0.16, -0.14,
                            -0.12, -0.10, -0.08, -0.06, -0.04,
                            -0.02, +0.00, +0.02, +0.04, +0.06,
                            +0.08, +0.10, +0.12, +0.14, +0.16,
                            +0.18, +0.20, +0.25,
                            +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hetau") {
        double edges[] = {-1.00, -0.80,
                          -0.60, -0.50, -0.40, -0.30, -0.25,
                          -0.20, -0.15, -0.10, -0.05, -0.01,
                          +0.02,  0.05,  0.08,  0.11,  0.15,
                          +0.40,
                          +0.50,  2.00};
        for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
          bins.push_back(edges[bin]);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "zetau") {
        if(HistSet == 26) { //high mass region, so low scores
          double edges[] = {-1.00, -0.90,
                            -0.65, -0.57, -0.50, -0.45, -0.40,
                            -0.35, -0.30, -0.25, -0.20, -0.15,
                            -0.10, +0.20, +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region, so low scores
          double edges[] = {-1.00, -0.90, -0.55,
                            -0.45, -0.40, -0.35, -0.30, -0.26,
                            -0.22, -0.18, -0.12, -0.06, +0.00,
                            +0.05, +0.20,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region, high scores mostly
          //dev, as of 11/22
          double edges[] = {-1.00, -0.80, -0.70,
                            -0.40, -0.30, -0.25, -0.22, -0.20,
                            -0.18, -0.16, -0.14, -0.12, -0.10,
                            -0.08, -0.06, -0.04, -0.02, +0.00,
                            +0.02, +0.04, +0.06, +0.08, +0.10,
                            +0.12, +0.14, +0.16,
                            +0.35, +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 28) { //z->ee mass region, high scores mostly
          double edges[] = {-1.00, -0.80, -0.60,
                            -0.30, -0.25, -0.20,
                            -0.18, -0.16, -0.14, -0.12, -0.10,
                            -0.08, -0.06, -0.04, -0.02, +0.00,
                            +0.02, +0.04, +0.06, +0.08, +0.10,
                            +0.12, +0.14, +0.16,
                            +0.35, +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else { //default, nominal score distribution
          //dev, as of 11/22
          double edges[] = {-1.00, -0.80,
                            -0.50, -0.45, -0.40, -0.35, -0.30,
                            -0.25, -0.20, -0.18, -0.16, -0.14,
                            -0.12, -0.10, -0.08, -0.06, -0.04,
                            -0.02, +0.00, +0.02, +0.04, +0.06,
                            +0.08, +0.10, +0.12, +0.15, +0.18,
                            +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hemu") {
        for(float bin = -1.; bin <= 2.; bin += 0.05) {
          bins.push_back(bin);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "zemu") {
        for(float bin = -1.; bin <= 2.; bin += 0.05) {
          bins.push_back(bin);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hmutau_e") {
        double edges[] = {-1.00, -0.80,
                          -0.55, -0.45, -0.35, -0.30, -0.25,
                          -0.20, -0.15, -0.10, -0.07, -0.04,
                          -0.01,  0.02,  0.08,  0.30
                          +0.50,  2.00};
        for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
          bins.push_back(edges[bin]);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "zmutau_e") {
        if(HistSet == 26) { //high mass region, so low scores
          double edges[] = {-1.00, -0.80, -0.70, -0.40, -0.30,
                            -0.25, -0.20, -0.15, -0.10, -0.05,
                            +0.20,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region, so low scores
          double edges[] = {-1.00, -0.90, -0.50, -0.40,
                            -0.35, -0.30, -0.25, -0.20, -0.15,
                            -0.10, -0.05, +0.00, +0.05, +0.35,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region, high scores
          double edges[] = {-1.00, -0.80,
                            -0.50, -0.45, -0.40, -0.35,
                            -0.30, -0.25, -0.20, -0.16, -0.12,
                            -0.08, -0.04, +0.00, +0.02, +0.04,
                            +0.06, +0.08, +0.10, +0.12, +0.14,
                            +0.16, +0.18,
                            +0.35, +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else { //default, nominal score distribution
          //dev, as of 11/29/22
          double edges[] = {-1.00, -0.80,
                            -0.55, -0.50, -0.45, -0.40, -0.35,
                            -0.30, -0.26, -0.23,
                            -0.20, -0.17, -0.14,
                            -0.11, -0.08, -0.06, -0.04, -0.02,
                            +0.00, +0.02, +0.04, +0.06, +0.08,
                            +0.10, +0.12, +0.14, +0.16, +0.18,
                            +0.20,
                            +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hetau_mu") {
        double edges[] = {-1.00, -0.80,
                          -0.55, -0.45, -0.35, -0.30, -0.25,
                          -0.20, -0.15, -0.10, -0.07, -0.04,
                          -0.01,  0.02,  0.08,  0.30
                          +0.50,  2.00};
        for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
          bins.push_back(edges[bin]);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "zetau_mu") {
        if(HistSet == 26) { //high mass region
          double edges[] = {-1.00, -0.90,
                            -0.55, -0.45, -0.35, -0.25, -0.10,
                            +0.20, +0.40,
                            +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region
          double edges[] = {-1.00,
                            -0.90, -0.55, -0.45, -0.40, -0.35,
                            -0.30, -0.25, -0.20, -0.15, -0.10,
                            -0.05, +0.00, +0.05, +0.10, +0.15,
                            +0.25,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region
          double edges[] = {-1.00,
                            -0.90, -0.50, -0.40, -0.30,
                            -0.25, -0.20, -0.17, -0.14, -0.11,
                            -0.08, -0.05, -0.01, +0.02, +0.04,
                            +0.06, +0.08, +0.10, +0.12, +0.14,
                            +0.16, +0.18, +0.30,
                            +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else { //default, nominal score distribution
          double edges[] = {-1.00,
                            -0.90, -0.50, -0.40, -0.30,
                            -0.25, -0.20, -0.17, -0.14, -0.11,
                            -0.08, -0.05, -0.02, +0.01, +0.04,
                            +0.06, +0.08, +0.10, +0.12, +0.14,
                            +0.16, +0.18, +0.30,
                            +0.40,
                            +0.50, +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        }
      }
      //perform a sanity check
      for(unsigned bin = 0; bin < bins.size() - 1; ++bin) {
        if(bins[bin] >= bins[bin+1]) {
          std::cout << "MVAConfig::" << __func__ << ": Error! MVA bins for selection " << selection.Data()
               << " not in increasing order!\n";
          throw 20;
        }
      }
      return bins;
    }


    //-------------------------------------------------------------------------------------------------
    // binning assuming the use of the CDF transform
    std::vector<Double_t> CDFBins(Int_t index, Int_t HistSet = 8) {
      TString selection = GetSelectionByIndex(index);
      std::vector<Double_t> bins;
      HistSet = HistSet % 100; //get the base set number
      const float default_width = 0.05;
      const int ndefault = std::ceil(1.f/default_width) + 1;
      //-------------------------------------------------------------------------------
      if       (selection == "hmutau") {
        for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
      //-------------------------------------------------------------------------------
      } else if(selection == "zmutau") {
        if(HistSet == 26) { //high mass region, so low scores
          double edges[] = {-default_width, 0., 0.05, 0.10, 0.15,
                            0.20, 1.};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region, so low scores
          double edges[] = {-default_width, 0., 0.05, 0.10, 0.15,
                            0.20, 0.25, 0.30, 0.35, 0.40,
                            0.50, 0.60, 0.70, 1.};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region, high scores
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        } else if(HistSet == 28) { //Z->mumu region, high scores
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        } else { //default, nominal score distribution
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hetau") {
        for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
      //-------------------------------------------------------------------------------
      } else if(selection == "zetau") {
        if(HistSet == 26) { //high mass region, so low scores
          double edges[] = {-default_width, 0., 0.05, 0.10, 0.15,
                            0.20, 1.};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region, so low scores
          double edges[] = {-default_width, 0., 0.05, 0.10, 0.15,
                            0.20, 0.30, 0.35, 0.40,
                            0.50, 0.60, 0.70, 1.};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region, high scores mostly
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        } else if(HistSet == 28) { //z->ee mass region, high scores mostly
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        } else { //default, nominal score distribution
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hemu") {
        for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
      //-------------------------------------------------------------------------------
      } else if(selection == "zemu") {
        for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
      //-------------------------------------------------------------------------------
      } else if(selection == "hmutau_e") {
        for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
      //-------------------------------------------------------------------------------
      } else if(selection == "zmutau_e") {
        if(HistSet == 26) { //high mass region, so low scores
          double edges[] = {-1.00, -0.80, -0.70, -0.40, -0.30,
                            -0.25, -0.20, -0.15, -0.10, -0.05,
                            +0.20,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region, so low scores
          double edges[] = {-1.00, -0.90, -0.50, -0.40,
                            -0.35, -0.30, -0.25, -0.20, -0.15,
                            -0.10, -0.05, +0.00, +0.05, +0.35,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region, high scores
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        } else { //default, nominal score distribution
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        }
      //-------------------------------------------------------------------------------
      } else if(selection == "hetau_mu") {
        for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
      //-------------------------------------------------------------------------------
      } else if(selection == "zetau_mu") {
        if(HistSet == 26) { //high mass region
          double edges[] = {-1.00, -0.90,
                            -0.55, -0.45, -0.35, -0.25, -0.15,
                            -0.05, +0.15, +0.40,
                            +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 27) { //low mass region
          double edges[] = {-1.00,
                            -0.90, -0.55, -0.45, -0.40, -0.35,
                            -0.30, -0.25, -0.20, -0.15, -0.10,
                            -0.05, +0.00, +0.05, +0.10,
                            +0.25,
                            +0.40, +0.50,
                            +2.00};
          for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
            bins.push_back(edges[bin]);
          }
        } else if(HistSet == 25) { //central mass region
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        } else { //default, nominal score distribution
          for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
        }
      }
      //perform a sanity check
      for(unsigned bin = 0; bin < bins.size() - 1; ++bin) {
        if(bins[bin] >= bins[bin+1]) {
          std::cout << "MVAConfig::" << __func__ << ": Error! MVA bins for selection " << selection.Data()
               << " not in increasing order!\n";
          throw 20;
        }
      }
      return bins;
    }


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
    bool useCDF_;
  };
}
#endif
