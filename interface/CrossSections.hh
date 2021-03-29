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
  CrossSections(int useUL = 0) {
    //cross sections by MC dataset
    double zll = (useUL > 0) ? 6435. : /*2075.14/0.0337*/ 6077.22; //UL: AMC@NLO = 6435 MadGraph = 5321
    double br_ll = 0.0337; //branching ratio of Z->ll
    double zxs = zll/(3.*br_ll);
    double higgs = (48.61+3.766+0.5071+1.358+0.880);
    values_["DY50"                    ] = zll    ;
    values_["Z"                       ] = zxs    ;
    values_["H"                       ] = higgs  ;
    values_["SingleAntiToptW"         ] = 34.91  ;
    values_["SingleToptW"             ] = 34.91  ;
    values_["WWW"                     ] = 0.2086 ;
    values_["WZ"                      ] = 27.6   ;
    values_["ZZ"                      ] = 12.14  ;
    values_["WW"                      ] = 12.178 ;
    values_["Wlnu"                    ] = 52850.0;
    values_["Wlnu-1J"                 ] = 8104.0 ;
    values_["ttbarToSemiLeptonic"     ] = 365.34 ;
    values_["ttbarlnu"                ] = 88.29  ;
    values_["ttbarToHadronic"         ] = 687.1  ;
    values_["SingleEle"               ] = 1.     ;
    values_["SingleMu"                ] = 1.     ;
    values_["QCDDoubleEMEnrich30to40" ] = 22180. ;
    values_["QCDDoubleEMEnrich30toInf"] = 247000.;
    values_["QCDDoubleEMEnrich40toInf"] = 113100.;



    //signals
    values_["ZEMu"  ] = zxs*7.5e-7; //PDG: 7.5e-7 ATLAS: 7.5e-7 CMS: 7.3e-7
    values_["ZETau" ] = zxs*8.1e-6; //PDG: 9.8e-6 ATLAS: 8.1e-6 (13 TeV)
    values_["ZMuTau"] = zxs*6.3e-6; //PDG: 1.2e-5 ATLAS: 6.3e-6 (13 TeV) 6.1e-6 (13+8 TeV)
    values_["HEMu"  ] = higgs*5.9e-5; //PDG: 3.5e-4 CMS: 5.6e-5 ATLAS: 5.9e-5
    values_["HETau" ] = higgs*1.9e-3; //PDG: 6.1e-3 CMS: 1.9e-3
    values_["HMuTau"] = higgs*1.6e-3; //PDG: 2.5e-3 CMS: 1.6e-3

    //data SS qcd estimates
    //give fraction of SS --> OS or j --> tau weighted (Data - MC) / Data for full Run-II
    values_["QCD_mutau"] = 0.918;
    values_["QCD_etau"]  = 0.935;
    values_["QCD_emu"]   = 0.342;

    ///////////////////////
    // generated numbers //
    ///////////////////////

    //Retrieved from DAS, can access on command line via:
    //$> das_client -query="dataset <das name> | grep dataset.nevents "
    numbers_[2016]["Wlnu"    ] =  29514020;
    numbers_[2016]["Wlnu-ext"] =  57402435;
    numbers_[2017]["Wlnu"    ] =  33073306;
    numbers_[2017]["Wlnu-ext"] =  44627200;
    numbers_[2016]["DY50"    ] =  49748967;
    numbers_[2016]["DY50-ext"] =  93007332;
    numbers_[2017]["DY50"    ] =  25757729;
    numbers_[2017]["DY50-ext"] = 186217773;
    numbers_[2018]["DY50"    ] = 100194597;

    //Signal gen numbers
    numbers_[2016]["ZEMu"    ] =  40000;
    numbers_[2017]["ZEMu"    ] =  80000;
    numbers_[2018]["ZEMu"    ] =  40000;
    numbers_[2016]["ZMuTau"  ] =  40000;
    numbers_[2017]["ZMuTau"  ] =  80000;
    numbers_[2018]["ZMuTau"  ] =  40000;
    numbers_[2016]["ZETau"   ] =  40000;
    numbers_[2017]["ZETau"   ] =  80000;
    numbers_[2018]["ZETau"   ] =  40000;
    numbers_[2016]["HEMu"    ] =  40000;
    numbers_[2017]["HEMu"    ] =  80000;
    numbers_[2018]["HEMu"    ] =  40000;
    numbers_[2016]["HMuTau"  ] =  40000;
    numbers_[2017]["HMuTau"  ] =  80000;
    numbers_[2018]["HMuTau"  ] =  40000;
    numbers_[2016]["HETau"   ] =  40000;
    numbers_[2017]["HETau"   ] =  80000;
    numbers_[2018]["HETau"   ] =  40000;

    //Luminosity by year
    lum_[2016] = 35.92e3;
    lum_[2017] = 41.48e3;
    lum_[2018] = 59.74e3;

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

  //Get luminosity by year
  double GetLuminosity(int year) {
    double lum(0.);
    if(year < 10) // using enums
      year += 2016;
    auto itr = lum_.find(year);
    if(itr != lum_.end())
      lum = itr->second;
    else
      std::cout << "Luminosity for " << year << " not found! Returning 0...\n";
    return lum;
  }

private:
  std::map<TString, double> values_ ; //cross sections
  std::map<int, std::map<TString, long>>   numbers_; // generated numbers
  std::map<int, double> lum_; //luminosity
};
#endif
