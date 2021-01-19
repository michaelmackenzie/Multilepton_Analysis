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
    names_ = {"mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
	      "etau_BDT_38.higgs","etau_BDT_38.Z0",
	      "emu_BDT_68.higgs","emu_BDT_68.Z0",
	      "mutau_e_BDT_68.higgs","mutau_e_BDT_68.Z0",
	      "etau_mu_BDT_68.higgs","etau_mu_BDT_68.Z0"};

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

  //get MVA output index by selection
  Int_t GetIndexBySelection(TString selection) {
    if(selection == "hmutau"        ) return 0;
    else if(selection == "zmutau"   ) return 1;
    else if(selection == "hetau"    ) return 2;
    else if(selection == "zetau"    ) return 3;
    else if(selection == "hemu"     ) return 4;
    else if(selection == "zemu"     ) return 5;
    else if(selection == "hmutau_e" ) return 6;
    else if(selection == "zmutau_e" ) return 7;
    else if(selection == "hetau_mu" ) return 8;
    else if(selection == "zetau_mu" ) return 9;
    return -1;
  }
  
public:
  std::vector<TString> names_; //MVA names
  std::map<TString, std::vector<double>> categories_; //MVA categories by selection
};
#endif
