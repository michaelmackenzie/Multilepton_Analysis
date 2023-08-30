#ifndef __ZPDFUncertainty__
#define __ZPDFUncertainty__
//Class to handle theory uncertainties on Z production

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {

  class ZPDFUncertainty {
  public:
    ZPDFUncertainty();

    ~ZPDFUncertainty();

    float GetPDFWeight        (int year, float z_pt, float z_eta, float z_mass);
    float GetRenormScaleWeight(int year, float z_pt, float z_mass);
    float GetFactorScaleWeight(int year, float z_pt, float z_mass);

  private:
    std::map<int, TH2*> hPDF_   ; //scales using generator level quantities
    std::map<int, TH2*> hScaleR_;
    std::map<int, TH2*> hScaleF_;
  };
}
#endif
