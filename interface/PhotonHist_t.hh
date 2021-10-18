#ifndef __PHOTONHIST__HH
#define __PHOTONHIST__HH
#include "TH1.h"
#include "TH2.h"

namespace CLFV {

  struct PhotonHist_t {
    TH1D* hPz;
    TH1D* hPt;
    TH1D* hP;
    TH1D* hEta;
    TH1D* hPhi;
    TH1D* hMVA;
    // TH1D* hIso;
    // TH1D* hRelIso;
    // TH1D* hTrigger;
  };
}
#endif
