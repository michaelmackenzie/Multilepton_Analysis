#ifndef __JETTOLEPWEIGHT__
#define __JETTOLEPWEIGHT__
//Class to handle scale factor of anti-iso --> iso for jets faking leps

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {
  class JetToLepWeight {
  public:
    JetToLepWeight(TString selection);
    ~JetToLepWeight();

    //Get scale factor for Data
    float GetDataFactor(int year, float pt, float eta);

  private:
    float GetFactor(TH2D* h, float pt, float eta, int year);

  public:

    std::map<int, TH2D*> histsData_;
  };
}
#endif
