#ifndef __ELECTRONIDWEIGHT__
#define __ELECTRONIDWEIGHT__
//Class to handle electron ID scale factors

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
  class ElectronIDWeight {
  public:
    ElectronIDWeight(int Mode = 0, int seed = 90, int verbose = 0);
    ~ElectronIDWeight();

    int GetIDGroup(int bin, int year) {
      return groupID_[(year-2016) * kYear + bin];
    }
    int GetRecoGroup(int bin, int year) {
      return groupReco_[(year-2016) * kYear + bin];
    }
    double interpolate(TH2F* h, double eta, double pt);
    double TriggerEff(double pt, double eta, int year, float& data_eff, float& mc_eff);

  public:
    enum { k2016, k2017, k2018};
    enum { kYear = 100000, kBinY = 100};
    std::map<int, TH2F*> histIDData_;
    std::map<int, TH2F*> histRecoData_;
    std::map<int, TH2F*> histTrigMCEff_;
    std::map<int, TH2F*> histTrigDataEff_;
    std::vector<TFile*> files_;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    std::map<int, int> groupID_; //correction groups for systematics
    std::map<int, int> groupReco_;
    bool interpolate_;
  };
}
#endif
