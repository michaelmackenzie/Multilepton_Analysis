#ifndef __ZPTWEIGHT__
#define __ZPTWEIGHT__
//Class to handle scale factors to correct the DY MC Z pT vs M distribution

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TSystem.h"

namespace CLFV {

  class ZPtWeight {
  public:
    ZPtWeight(TString Name, int Mode = 1, int seed = 90);

    ~ZPtWeight();

    float GetWeight(int year, float pt, float mass, bool doReco, float& up, float& down, float& sys);
    float GetWeight(int year, float pt, float mass, bool doReco) {
      float up, down, sys;
      return GetWeight(year, pt, mass, doReco, up, down, sys);
    }

  private:
    std::map<int, TH2*> hZPtScales_; //scales using generator level quantities
    std::map<int, TH2*> hZPtRecoScales_; //scales using reconstructed quantities
    std::map<int, TH2*> hZPtSysScales_; //scales from a different data region
    std::map<int, TH2*> hZPtSysRecoScales_; //scales from a different data region
    // std::vector<TFile*> files_;
    TString Name_;
    int Mode_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    //       year          eta           pt
    std::map<int, std::map<int, std::map<int, bool>>> isShiftedUp_; //whether the systematic is shifted up or down
  };
}
#endif
