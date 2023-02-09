#ifndef __JetES__
#define __JetES__
//Class to handle jet energy scale corrections/uncertainties

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {
  class JetES {
  public:
    JetES();
    ~JetES();

    float GetJER(int year, float jetpt, float jeteta);
    float GetJES(int year, float jetpt, float jeteta);

  };
}
#endif
