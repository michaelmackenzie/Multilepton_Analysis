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
              "etau_mu_BDT_68.higgs","etau_mu_BDT_68.Z0",
              "mutau_TMlpANN_8.higgs","mutau_TMlpANN_8.Z0", //10 - 19: alternate mvas
              "etau_TMlpANN_8.higgs","etau_TMlpANN_8.Z0",
              "emu_BDT_68.higgs","emu_BDT_68.Z0",
              "mutau_e_TMlpANN_8.higgs","mutau_e_TMlpANN_8.Z0",
              "etau_mu_TMlpANN_8.higgs","etau_mu_TMlpANN_8.Z0"
    };

    //initialize MVA output categories
    categories_["hmutau"  ] = {-0.05680,  0.02900,  0.13370,  0.26540};
    categories_["zmutau"  ] = {-0.28180, -0.15400, -0.04330,  0.12920};
    categories_["hetau"   ] = {-0.06700,  0.04130,  0.15380,  0.33350};
    categories_["zetau"   ] = {-0.25540, -0.11290, -0.03850,  0.05990};
    categories_["hemu"    ] = {-0.05770, -0.03100,  0.01220,  0.03920}; //defined with cut-and-count in M in [M_B - 5, M_B + 5]
    categories_["zemu"    ] = {-0.29020, -0.16780, -0.08200,  0.02870}; //defined with cut-and-count in M in [M_B - 5, M_B + 5]
    categories_["hmutau_e"] = {-0.10660,  0.00650,  0.08600,  0.15860};
    categories_["zmutau_e"] = {-0.12310, -0.09790, -0.05830,  0.07190};
    categories_["hetau_mu"] = {-0.12820, -0.00400,  0.08060,  0.16160};
    categories_["zetau_mu"] = {-0.32500, -0.15460, -0.02680,  0.09410};
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

  Double_t* Bins(Int_t index, Int_t& nbins) {
    TString selection = GetSelectionByIndex(index);
    std::vector<Double_t> bins;
    if       (selection == "hmutau") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "zmutau") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "hetau") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "zetau") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
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
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "zmutau_e") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "hetau_mu") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    } else if(selection == "zetau_mu") {
      for(double bin = -1.; bin <= 2.; bin += 0.05) {
        bins.push_back(bin);
      }
    }
    nbins = bins.size() - 1;
    return bins.data();
  }

  Double_t* Bins(Int_t index) {
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
  std::map<TString, std::vector<double>> categories_; //MVA categories by selection
};
#endif
