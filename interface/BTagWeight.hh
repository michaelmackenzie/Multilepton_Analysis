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
                    float& up_bc, float& down_bc, float& up_l, float& down_l);

    float BTagCut(int wp, int year) {
      double val = -1.;
      if(year == 2016) {
        if(wp == kLooseBTag)       val = 0.2217;
        else if(wp == kMediumBTag) val = 0.6321;
        else if(wp == kTightBTag)  val = 0.8953;
        else std::cout << "WARNING! Unrecognized B-Tag WP " << wp << " for "
                       << year << " configurations! Returning -1..." << std::endl;
      }
      if(year == 2017) {
        if(wp == kLooseBTag)       val = 0.1522;
        else if(wp == kMediumBTag) val = 0.4941;
        else if(wp == kTightBTag)  val = 0.8001;
        else std::cout << "WARNING! Unrecognized B-Tag WP " << wp << " for "
                       << year << " configurations! Returning -1..." << std::endl;
      }
      if(year == 2018) {
        if(wp == kLooseBTag)       val = 0.1241;
        else if(wp == kMediumBTag) val = 0.4184;
        else if(wp == kTightBTag)  val = 0.7527;
        else std::cout << "WARNING! Unrecognized B-Tag WP " << wp << " for "
                       << year << " configurations! Returning -1..." << std::endl;
      }
      return val;

    }

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
