//Histograms to study the BDT variable distributions in regions of the BDT variables

#ifndef __BDTVARHIST__HH
#define __BDTVARHIST__HH

#include "TH1.h"

namespace CLFV {
  struct BDTVarHist_t {
    TH1* h[330]; //10 BDT variables + BDT score, 3 regions --> 10x(10+1)x3 = 330 histograms
  };
}

#endif
