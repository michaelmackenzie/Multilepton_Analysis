#ifndef __JETPUWEIGHT__
#define __JETPUWEIGHT__
//Class to handle jet pileup ID weight of MC processes

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TSystem.h"

namespace CLFV {
  class JetPUWeight {
  public:
    JetPUWeight(int seed = 90);
    ~JetPUWeight();

    float GetMCEff(int year, float jetpt, float jeteta);
    float GetScaleFactor(int year, float jetpt, float jeteta);
    float GetWeight(int year, int njets, float* jetspt, float* jetseta,
                    int nrejjets, float* jetsrejpt, float* jetsrejeta);

    std::map<int, TH2F*> efficiencies_;
    std::map<int, TH2D*> hists_; //MC efficiencies
    TRandom3* rnd_; //for generating systematic shifted parameters
  };
}
#endif
