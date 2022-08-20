#ifndef __LEPTON__HH
#define __LEPTON__HH

#include "TLorentzVector.h"

namespace CLFV {
  struct Lepton_t {
    float pt;
    float eta;
    float phi;
    float mass;
    TLorentzVector* p4 = nullptr;
    float scEta;
    int   flavor;
    float iso;
    float dxy;
    float dz;
    float dxySig;
    float dzSig;

    int   ID1;
    int   ID2;
    int   ID3;
    float wt1[3];
    float wt2[3];
    int   wt1_bin;
    int   wt2_bin;
    float trig_wt;

    int   trigger;
    bool  fired;

    int   genFlavor;
    float ptSF[3];
    float ES[3];

    bool  jetOverlap;
  };
}
#endif
