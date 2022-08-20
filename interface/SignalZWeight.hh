#ifndef __SIGNALZWEIGHT__
#define __SIGNALZWEIGHT__
//Class to handle scale factors to correct the DY MC Z pT vs M distribution

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {

  class SignalZWeight {
  public:
    SignalZWeight();

    ~SignalZWeight();

    float GetWeight(int year, float pt, float mass);

  private:
    std::map<int, TH2*> hScales_; //scales using generator level quantities
  };
}
#endif
