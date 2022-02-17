#ifndef __BTAGWEIGHT__
#define __BTAGWEIGHT__
//Class to handle b-tag weights

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TSystem.h"

namespace CLFV {
  class BTagWeight {
  public:
    BTagWeight(const int seed = 90);

    ~BTagWeight();

    float GetMCEff(const int WP, const int year, float jetpt, float jeteta, int jetflavor);
    void GetSystematic(const int WP, const int year, float jetpt, int jetFlavor,
                       float& up, float& down);
    float GetScaleFactor(const int WP, const int year, float jetpt, int jetFlavor, float& up, float& down);
    float GetWeight(const int wp, const int year, const int njets, const float* jetspt,
                    const float* jetseta, const int* jetsflavor, const int* jetsbtag,
                    float& up, float& down);

    enum {kLooseBTag, kMediumBTag, kTightBTag};
    // private:

    std::map<int, std::map<int, TH2D*>> histsL_; //light quarks
    std::map<int, std::map<int, TH2D*>> histsC_; //c quarks
    std::map<int, std::map<int, TH2D*>> histsB_; //b quarks
    TRandom3* rnd_; //for generating systematic shifted parameters
    int verbose_;
    //       year          eta           pt
    std::map<int, std::map<int, std::map<int, bool>>> isShiftedUp_; //whether the systematic is shifted up or down
  };
}
#endif
