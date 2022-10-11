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
#include "TSystem.h"

//local includes
#include "interface/Utilities.hh"

namespace CLFV {
  class ElectronIDWeight {
  public:
    ElectronIDWeight(const int Mode = 0, const int verbose = 0);
    ~ElectronIDWeight();

    double IDWeight(double pt, double eta, int year,
                    float& weight_id , float& weight_up_id , float& weight_down_id ,
                    float& weight_rec, float& weight_up_rec, float& weight_down_rec);
    double IDWeight(double pt, double eta, int year) {
      float wt_id, wt_up_id, wt_down_id, wt_rec, wt_up_rec, wt_down_rec;
      return IDWeight(pt, eta, year, wt_id, wt_up_id, wt_down_id, wt_rec, wt_up_rec, wt_down_rec);
    }
    double EmbedEnergyScale(double pt, double eta, int year, float& up, float& down);
    double TriggerEff(double pt, double eta, int year, int WP, float& data_eff, float& mc_eff,
                      float& data_up, float& mc_up, float& data_down, float& mc_down);

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
    std::map<int, int> groupID_; //correction groups for systematics
    std::map<int, int> groupReco_;
    bool interpolate_;
    bool use_embed_tnp_;
  };
}
#endif
