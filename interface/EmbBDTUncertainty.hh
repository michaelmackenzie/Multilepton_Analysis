#ifndef __EMBBDTUNCERTAINTY__
#define __EMBBDTUNCERTAINTY__
//Class to handle Embedding BDT uncertainties

//c++ includes
#include <iostream>
#include <math.h>
#include <map>
#include <vector>

//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TSystem.h"
#include "TString.h"

//local includes

namespace CLFV {
  class EmbBDTUncertainty {

  public:
    EmbBDTUncertainty();
    ~EmbBDTUncertainty();

    float Weight(float score, int year, TString selection);

    std::map<TString, std::map<int,TH1*>> scales_;
  };
}
#endif
