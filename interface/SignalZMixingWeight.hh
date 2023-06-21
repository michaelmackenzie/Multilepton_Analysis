#ifndef __SIGNALZMIXINGWEIGHT__
#define __SIGNALZMIXINGWEIGHT__
//Class to handle scale factors to correct the signal mass spectrum due to Z/gamma mixing

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {

  class SignalZMixingWeight {
  public:
    SignalZMixingWeight();

    ~SignalZMixingWeight();

    float GetWeight(int year, float pt, float mass, float& up, float& down);

    float GetWeight(int year, float pt, float mass) {
      float up, down;
      return GetWeight(year, pt, mass, up, down);
    }

  private:
    std::map<int, TH2*> hScales_; //scales using generator level quantities
  };
}
#endif
