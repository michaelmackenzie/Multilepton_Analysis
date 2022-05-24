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

//local includes
#include "interface/Utilities.hh"

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

    double IDWeight(double pt, double eta, int year,
                    float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                    float& weight_rec, float& weight_up_rec, float& weight_down_rec, int& ibin_rec);
    double IDWeight(double pt, double eta, int year) {
      float wt_id, wt_up_id, wt_down_id, wt_rec, wt_up_rec, wt_down_rec;
      int ibin_id, ibin_rec;
      return IDWeight(pt, eta, year, wt_id, wt_up_id, wt_down_id, ibin_id, wt_rec, wt_up_rec, wt_down_rec, ibin_rec);
    }
    double EmbedEnergyScale(double pt, double eta, int year, float& up, float& down);
    double TriggerEff(double pt, double eta, int year, int WP, float& data_eff, float& mc_eff);

  public:
    enum { k2016, k2017, k2018};
    enum { kYear = 100000, kBinY = 100};
    enum { kFailed, kWPL, kWP90, kWP80, kWPLNotWP90, kWPLNotWP80}; //electron ID working points (kWPLNotWPXX = WPL + !WPXX)
    std::map<int, TH2*> histID_;
    std::map<int, TH2*> histReco_;
    std::map<int, TH2*> histLowReco_;
    std::map<int, float> vertexMap_;
    std::map<int, std::map<int, TH2*>> histTrigMCEff_  ; //map is <year, <WP, hist>>
    std::map<int, std::map<int, TH2*>> histTrigDataEff_;
    std::vector<TFile*> files_;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    std::map<int, int> groupID_; //correction groups for systematics
    std::map<int, int> groupReco_;
    bool interpolate_;
  };
}
#endif
