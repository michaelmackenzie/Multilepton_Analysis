#ifndef __BDTSCALE__
#define __BDTSCALE__
//Class to handle BDT scale factors

//c++ includes
#include <iostream>
#include <math.h>

//ROOT includes

//local includes

namespace CLFV {
  class BDTScale {

  public:
    BDTScale() {}
    ~BDTScale() {}

    float Weight(float score, int year, float& up, float& down);
    float Weight(float score, int year) {
      float up, down;
      return Weight(score, year, up, down);
    }

  private:
    float muon_2016_weight         (float score);
    float muon_2017_weight         (float score);
    float muon_2018_weight         (float score);
    float electron_2016_weight     (float score);
    float electron_2017_weight     (float score);
    float electron_2018_weight     (float score);

    float muon_2016_weight_up      (float score);
    float muon_2017_weight_up      (float score);
    float muon_2018_weight_up      (float score);
    float electron_2016_weight_up  (float score);
    float electron_2017_weight_up  (float score);
    float electron_2018_weight_up  (float score);

    float muon_2016_weight_down    (float score);
    float muon_2017_weight_down    (float score);
    float muon_2018_weight_down    (float score);
    float electron_2016_weight_down(float score);
    float electron_2017_weight_down(float score);
    float electron_2018_weight_down(float score);
  };
}
#endif
