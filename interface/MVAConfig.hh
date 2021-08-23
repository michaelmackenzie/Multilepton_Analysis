#ifndef __MVACONFIG__HH
#define __MVACONFIG__HH
//A tool to store MVA information

//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TString.h"

class MVAConfig {
public:
  MVAConfig() {
    //initialize MVA names
    names_ = {
              "mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
              "etau_BDT_38.higgs","etau_BDT_38.Z0",
              "emu_BDT_68.higgs","emu_BDT_68.Z0",
              "mutau_e_BDT_68.higgs","mutau_e_BDT_68.Z0",
              "etau_mu_BDT_68.higgs","etau_mu_BDT_68.Z0"//,
              // "mutau_TMlpANN_8.higgs","mutau_TMlpANN_8.Z0", //10 - 19: alternate mvas
              // "etau_TMlpANN_8.higgs","etau_TMlpANN_8.Z0",
              // "emu_BDT_68.higgs","emu_BDT_68.Z0",
              // "mutau_e_TMlpANN_8.higgs","mutau_e_TMlpANN_8.Z0",
              // "etau_mu_TMlpANN_8.higgs","etau_mu_TMlpANN_8.Z0"
    };

    //initialize MVA output categories
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

    for(unsigned imva = 0; imva < names_.size(); ++imva) {
      TString name = names_[imva];
      int cat = -1;
      if(name.Contains("mutau_e") || name.Contains("etau_mu") || name.Contains("emu")) cat = 0; //emu data
      else if(name.Contains("mutau")) cat = 1; //mutau data
      else if(name.Contains("etau")) cat = 2; //etau data
      data_cat_.push_back(cat);
    }
  }

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

  //get MVA output index by selection
  Int_t GetIndexBySelection(TString selection) {
    for(Int_t index = 0; index < 10; ++index) {
      if(selection == GetSelectionByIndex(index)) return index;
    }
    return -1;
  }

  std::vector<Double_t> Bins(Int_t index, Int_t& nbins) {
    TString selection = GetSelectionByIndex(index);
    std::vector<Double_t> bins;
    if       (selection == "hmutau") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.16, -0.13,
                        -0.10, -0.07, -0.04, -0.01,  0.02,
                        +0.05,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "zmutau") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.15, -0.10,
                        -0.07, -0.04, -0.01,  0.02,  0.05,
                        +0.08,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "hetau") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.16, -0.13,
                        -0.10, -0.07, -0.04, -0.01,  0.02,
                        +0.05,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "zetau") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.16, -0.13,
                        -0.10, -0.07, -0.04, -0.01,  0.02,
                        +0.05,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "hemu") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "zemu") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "hmutau_e") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.16, -0.13,
                        -0.10, -0.07, -0.04, -0.01,  0.02,
                        +0.05,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "zmutau_e") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.15, -0.10,
                        -0.07, -0.04, -0.01,  0.02,  0.05,
                        +0.08,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "hetau_mu") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.16, -0.13,
                        -0.10, -0.07, -0.04, -0.01,  0.02,
                        +0.05,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(selection == "zetau_mu") {
      double edges[] = {-1.00, -0.80,
                        -0.30, -0.25, -0.20, -0.16, -0.13,
                        -0.10, -0.07, -0.04, -0.01,  0.02,
                        +0.05,  0.20,
                        +0.50,  2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    }
    nbins = bins.size() - 1;
    return bins;
  }

  std::vector<Double_t> Bins(Int_t index) {
    Int_t nbins;
    return Bins(index, nbins);
  }

  Int_t NBins(Int_t index) {
    int nbins;
    Bins(index, nbins);
    return nbins;
  }
public:
  std::vector<TString> names_; //MVA names
  std::vector<int> data_cat_; //relevant data category
  std::map<TString, std::vector<double>> categories_; //MVA categories by selection
};
#endif
