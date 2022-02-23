#ifndef __PHOTONHIST__HH
#define __PHOTONHIST__HH
#include "TH1.h"
#include "TH2.h"

namespace CLFV {

  struct PhotonHist_t {
    TH1* hPz;
    TH1* hPt;
    TH1* hP;
    TH1* hEta;
    TH1* hPhi;
    TH1* hMVA;
    // TH1* hIso;
    // TH1* hRelIso;
    // TH1* hTrigger;
  };
}
#endif
