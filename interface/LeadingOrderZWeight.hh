#ifndef __LEADINGORDERZWEIGHT__
#define __LEADINGORDERZWEIGHT__
//Class to handle scale factors to correct the LO DY MC Z pT vs M distribution

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {

  class LeadingOrderZWeight {
  public:
    LeadingOrderZWeight();

    ~LeadingOrderZWeight();

    float GetWeight(int year, float pt, float mass);

  private:
    std::map<int, TH2*> hScales_; //scales using generator level quantities
  };
}
#endif
