#ifndef __PREFIREWEIGHT__
#define __PREFIREWEIGHT__
//Class to handle L1 pre-fire weight of MC processes

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TSystem.h"

namespace CLFV {

  class PrefireWeight {
  public:
    PrefireWeight(int seed = 90);
    ~PrefireWeight();

    float GetProbability(const int year, float jetpt, float jeteta);
    float GetWeight(int year, int njets, float* jetspt, float* jetseta);

    std::map<int, TH2F*> hists_; //MC efficiencies
    TRandom3* rnd_; //for generating systematic shifted parameters
  };
}
#endif
