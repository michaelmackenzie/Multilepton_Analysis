#ifndef __PREFIREWEIGHT__
#define __PREFIREWEIGHT__
//Class to handle L1 pre-fire weight of MC processes

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {

  class PrefireWeight {
  public:
    PrefireWeight();
    ~PrefireWeight();

    float GetProbability(const int year, float jetpt, float jeteta, int base);
    float GetJetWeight(int year, int njets, float* jetspt, float* jetseta);
    float GetPhotonWeight(int year, int njets, float* jetspt, float* jetseta);

    enum {kJet = 0, kPhoton = 100};
    std::map<int, TH2*> hists_; //MC efficiencies
  };
}
#endif
