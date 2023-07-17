#ifndef __LEPTONDISPLACEMENT__
#define __LEPTONDISPLACEMENT__
//Class to handle lepton dxy/dz significance scale factors

//c++ includes
#include <iostream>
#include <math.h>

//ROOT includes

//local includes

namespace CLFV {
  class LeptonDisplacement {

  public:
    LeptonDisplacement() {}
    ~LeptonDisplacement() {}

    float Weight(float pt, float eta, bool is_muon, int year);

  private:
    float muon_2016_weight(float pt, float eta);
    float muon_2017_weight(float pt, float eta);
    float muon_2018_weight(float pt, float eta);
    float ele_2016_weight (float pt, float eta);
    float ele_2017_weight (float pt, float eta);
    float ele_2018_weight (float pt, float eta);
  };
}
#endif
