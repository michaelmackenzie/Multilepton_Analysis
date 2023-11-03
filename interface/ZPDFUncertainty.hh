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
    ZPDFUncertainty(const bool useEnvelope = false);

    ~ZPDFUncertainty();

    float GetPDFWeight        (int year, float z_pt, float z_eta, float z_mass, int index = -1);
    void  GetPDFWeight        (int year, float z_pt, float z_eta, float z_mass, float* weights, unsigned& npdf);
    float GetRenormScaleWeight(int year, float z_pt, float z_mass);
    float GetFactorScaleWeight(int year, float z_pt, float z_mass);

  private:
    enum {kMaxNPDF = 1000}; //maximum number of alternate PDFs per year
    std::map<int, TH2*> hPDF_   ; //scales using generator level quantities
    std::map<int, TH2*> hScaleR_;
    std::map<int, TH2*> hScaleF_;
    bool useEnvelope_; //use all PDFs given in a certain year
  };
}
#endif
