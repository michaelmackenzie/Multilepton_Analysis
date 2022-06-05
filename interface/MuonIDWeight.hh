#ifndef __MUONIDWEIGHT__
#define __MUONIDWEIGHT__
//Class to handle muon ID scale factors

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
  class MuonIDWeight {
  public:
    MuonIDWeight(const int seed = 90, const int verbose = 0);
    ~MuonIDWeight();

    int GetIDGroup(int bin, int year) {
      return groupID_[(year-2016) * kYear + bin];
    }
    int GetIsoGroup(int bin, int year) {
      return groupIso_[(year-2016) * kYear + bin];
    }
    void IDWeight(double pt, double eta, int year, int mcEra,
                  float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                  float& weight_iso, float& weight_up_iso, float& weight_down_iso, int& ibin_iso);
    double TriggerEff(double pt, double eta, int year, bool isLow, int mcEra, float& data_eff, float& mc_eff);

  public:
    enum { k2016, k2017, k2018};
    enum { kYear = 100000, kRunSection = 10000, kBinY = 100};
    std::map<int, TH2*> histIDData_         ;
    std::map<int, TH2*> histIsoData_        ;
    std::map<int, TH2*> histTriggerLowData_ ;
    std::map<int, TH2*> histTriggerLowMC_   ;
    std::map<int, TH2*> histTriggerHighData_;
    std::map<int, TH2*> histTriggerHighMC_  ;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    std::map<int, int> groupID_; //correction groups for systematics
    std::map<int, int> groupIso_;
  };
}
#endif
