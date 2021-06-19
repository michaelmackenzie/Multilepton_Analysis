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
  CrossSections(int useUL = 0, int ZMode = 0) {
    //cross sections by MC dataset
    double zll = (useUL > 0) ? 6435. : /*2075.14/0.0337*/ 6077.22; //UL: AMC@NLO = 6435 MadGraph = 5321
    if(ZMode == 1 && useUL == 0) zll = 4963.0;

    double br_ll = 0.0337; //branching ratio of Z->ll
    double zxs = zll/(3.*br_ll);
    double higgs = (48.61+3.766+0.5071+1.358+0.880);
    values_["DY50"                    ][2016] = zll    ;
    values_["DY50"                    ][2017] = (ZMode == 1) ? 6435. : zll;
    values_["DY50"                    ][2018] = (ZMode == 1) ? 6435. : zll;
    values_["DY10"                    ][2016] = 18610. ;
    values_["DY10"                    ][2017] = 18610. ;
    values_["DY10"                    ][2018] = 18610. ;
    values_["Z"                       ][2016] = zxs    ;
    values_["Br_Zll"                  ][2016] = br_ll  ;
    values_["H"                       ][2016] = higgs  ;
    values_["SingleAntiToptW"         ][2016] = 35.85  ; //34.91  ;
    values_["SingleToptW"             ][2016] = 35.85  ; //34.91  ;
    values_["WWW"                     ][2016] = 0.2086 ;
    values_["WWZ"                     ][2016] = 0.1651 ;
    values_["WZ"                      ][2016] = 47.13  ; //27.6   ;
    values_["ZZ"                      ][2016] = 16.523 ; //12.14  ;
    values_["WW"                      ][2016] = 12.178 ;
    values_["Wlnu"                    ][2016] = 61526.7; //52850.0;
    values_["Wlnu-1J"                 ][2016] =  9341.3*1.219; //defined as total * fraction of unbinned with 1J * integral(unbinned 1J) / integral(1J)
    values_["Wlnu-2J"                 ][2016] =  3195.4*1.219;
    values_["Wlnu-3J"                 ][2016] =  1110.9*1.219;
    values_["Wlnu-4J"                 ][2016] =   626.2*1.219;
    values_["ttbarToSemiLeptonic"     ][2016] = 365.34 ;
    values_["ttbarlnu"                ][2016] = 87.31  ; //88.29  ;
    values_["ttbarToHadronic"         ][2016] = 687.1  ;
    values_["SingleEle"               ][2016] = 1.     ;
    values_["SingleMu"                ][2016] = 1.     ;
    values_["QCDDoubleEMEnrich30to40" ][2016] = 22180. ;
    values_["QCDDoubleEMEnrich30toInf"][2016] = 247000.;
    values_["QCDDoubleEMEnrich40toInf"][2016] = 113100.;



    //signals
    values_["ZEMu"  ][2016] = zxs*7.5e-7; //PDG: 7.5e-7 ATLAS: 7.5e-7 CMS: 7.3e-7
    values_["ZETau" ][2016] = zxs*8.1e-6; //PDG: 9.8e-6 ATLAS: 8.1e-6 (13 TeV)
    values_["ZMuTau"][2016] = zxs*6.3e-6; //PDG: 1.2e-5 ATLAS: 6.3e-6 (13 TeV) 6.1e-6 (13+8 TeV)
    values_["HEMu"  ][2016] = higgs*5.9e-5; //PDG: 3.5e-4 CMS: 5.6e-5 ATLAS: 5.9e-5
    values_["HETau" ][2016] = higgs*1.9e-3; //PDG: 6.1e-3 CMS: 1.9e-3
    values_["HMuTau"][2016] = higgs*1.6e-3; //PDG: 2.5e-3 CMS: 1.6e-3

    //data SS qcd estimates
    //give fraction of SS --> OS or j --> tau weighted (Data - MC) / Data for full Run-II
    values_["QCD_mutau"][2016] = 0.918;
    values_["QCD_etau" ][2016]  = 0.935;
    values_["QCD_emu"  ][2016]   = 0.342;

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
    lum_[2016] = 36.33e3;
    lum_[2017] = 41.48e3;
    lum_[2018] = 59.83e3;

  }

  double GetCrossSection(TString name, int year = 2016) {
    double xsec(1.);
    auto itr = values_.find(name);
    if(itr != values_.end())
      xsec = itr->second[year];
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
  std::map<TString, std::map<int, double>> values_ ; //cross sections
  std::map<int, std::map<TString, long>>   numbers_; // generated numbers
  std::map<int, double> lum_; //luminosity
};
#endif
