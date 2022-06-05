#ifndef __CROSSSECTIONS__
#define __CROSSSECTIONS__
//Class to handle cross sections of MC processes

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TString.h"


namespace CLFV {
  class CrossSections {
  public:

    CrossSections(int useUL = 0, int ZMode = 0);

    double GetCrossSection(TString name, int year = 2016) {
      double xsec(1.);
      auto itr = values_.find(name);
      if(itr != values_.end()) {
        auto itr_2 = itr->second.find(year);
        if(itr_2 != itr->second.end()) {
          xsec = itr->second[year];
        } else {
          std::cout << "Cross section for year " << year << " for sample " << name.Data() << " not found! Returning 1...\n";
        }
      } else {
        std::cout << "Cross section for " << name.Data() << " not found! Returning 1...\n";
      }
      return xsec;
    }

    double GetNegativeFraction(TString name, int year = 2016) {
      double frac(0.);
      auto itr = neg_frac_.find(name);
      if(itr != neg_frac_.end()) {
        auto itr_2 = itr->second.find(year);
        if(itr_2 != itr->second.end()) {
          frac = itr->second[year];
        } else {
          std::cout << "Negative fraction for year " << year << " for sample " << name.Data() << " not found! Returning 1...\n";
        }
      } else {
        std::cout << "Negative fraction for " << name.Data() << " not found! Returning 1...\n";
      }
      return frac;
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
    std::map<TString, std::map<int, double>> neg_frac_ ; //fraction of events with negative weights
    std::map<int, std::map<TString, long>>   numbers_; // generated numbers
    std::map<int, double> lum_; //luminosity
  };
}
#endif
