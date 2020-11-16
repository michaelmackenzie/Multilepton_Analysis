#ifndef __CROSSSECTIONS__
#define __CROSSSECTIONS__
//Class to handle cross sections of MC processes
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TString.h"

class CrossSections {
public:
  CrossSections() {
    //cross sections by MC dataset
    double zll = /*2075.14/0.0337*/ 6077.22;
    double br_ll = 0.0337; //branching ratio of Z->ll
    values_["DY50"                    ] = zll    ;
    values_["SingleAntiToptW"         ] = 34.91  ;
    values_["SingleToptW"             ] = 34.91  ;
    values_["WWW"	              ] = 0.2086 ;
    values_["WZ"                      ] = 27.6   ;
    values_["ZZ"	              ] = 12.14  ;
    values_["WW"	              ] = 12.178 ;
    values_["Wlnu"                    ] = 52850.0;
    values_["Wlnu-ext"                ] = 52850.0;
    values_["ttbarToSemiLeptonic"     ] = 365.34 ;
    values_["ttbarlnu"                ] = 88.29  ;
    values_["SingleEle"               ] = 1.     ;
    values_["SingleMu"                ] = 1.     ;
    values_["QCDDoubleEMEnrich30to40" ] = 22180. ;
    values_["QCDDoubleEMEnrich30toInf"] = 247000.;
    values_["QCDDoubleEMEnrich40toInf"] = 113100.;



    //signals
    double higgs = (48.61+3.766+0.5071+1.358+0.880);
    values_["ZEMu"  ] = zll/(3.*br_ll)*7.3e-7;
    values_["ZETau" ] = zll/(3.*br_ll)*9.8e-6;
    values_["ZMuTau"] = zll/(3.*br_ll)*1.2e-5;
    values_["HEMu"  ] = higgs*3.5e-4;
    values_["HETau" ] = higgs*6.1e-3;
    values_["HMuTau"] = higgs*2.5e-3;

    //generated numbers
    //Retrieved from DAS, can access on command line via:
    //$> das_client -query="dataset <das name> | grep dataset.nevents "
    numbers_[2016]["Wlnu"    ] =  29514020;
    numbers_[2016]["Wlnu-ext"] =  57402435;
    numbers_[2017]["Wlnu"    ] =  33073306;
    numbers_[2017]["Wlnu-ext"] =  44627200;
    numbers_[2017]["DY50"    ] =  25757729;
    numbers_[2017]["DY50-ext"] = 186217773;
    numbers_[2018]["DY50"    ] = 100194597;
    
  }

  double GetCrossSection(TString name) {
    double xsec(1.);
    auto itr = values_.find(name);
    if(itr != values_.end())
      xsec = itr->second;
    else
      std::cout << "Cross section for " << name.Data() << " not found! Returning 1...\n";
    return xsec;
  }

  //if combining files, may want number of generated events
  long GetGenNumber(TString name, int year) {
    long gen(0);
    auto itr = numbers_[year].find(name);
    if(itr != numbers_[year].end())
      gen = itr->second;
    else
      std::cout << "Generated number for " << name.Data() << " not found! Returning 0...\n";
    return gen;
  }
private:
  std::map<TString, double> values_ ; //cross sections
  std::map<int, std::map<TString, long>>   numbers_; // generated numbers
};
#endif
