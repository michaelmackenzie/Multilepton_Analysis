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
#include "TSystem.h"

namespace CLFV {
  class MuonIDWeight {
  public:
    MuonIDWeight(const int Mode = 0, const int verbose = 0);
    ~MuonIDWeight();

    void SetVerbose(int verbose) {verbose_ = verbose;}

    void IDWeight(double pt, double eta, int year, int mcEra,
                  float& weight_id , float& weight_up_id , float& weight_down_id ,
                  float& weight_iso, float& weight_up_iso, float& weight_down_iso);
    double TriggerEff(double pt, double eta, int year, bool isLow, int mcEra, float& data_eff, float& mc_eff,
                      float& data_up, float& mc_up, float& data_down, float& mc_down);

  public:
    enum { k2016, k2017, k2018};
    //muon isolation levels
    enum{kVVLooseMuIso, kVLooseMuIso, kLooseMuIso, kMediumMuIso, kTightMuIso, kVTightMuIso, kVVTightMuIso, kMuonIsos}; //define iso scale factor sets
    const double muonIsoValues[kMuonIsos] = {0.5, 0.4, 0.25, 0.20, 0.15, 0.10, 0.05}; //corresponding values

  private:
    std::map<int, TH2*> histIDData_         ;
    std::map<int, TH2*> histIsoData_        ;
    std::map<int, TH2*> histTriggerLowData_ ;
    std::map<int, TH2*> histTriggerLowMC_   ;
    std::map<int, TH2*> histTriggerHighData_;
    std::map<int, TH2*> histTriggerHighMC_  ;

    int Mode_   ;
    int verbose_;
    std::map<int, int> groupID_; //correction groups for systematics
    std::map<int, int> groupIso_;
  };
}
#endif
