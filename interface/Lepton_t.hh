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
    float relIso;
    float dxy;
    float dz;
    float d0;
    float dxySig;
    float dzSig;
    //associated track info
    float trkpt;
    float trketa;
    float trkphi;

    UChar_t id1;
    UChar_t id2;
    UChar_t id3;
    float wt1[4] = {1.f, 1.f, 1.f, 1.f}; //weight, up, down, sys
    float wt2[4] = {1.f, 1.f, 1.f, 1.f}; //weight, up, down, sys
    float wt3[4] = {1.f, 1.f, 1.f, 1.f}; //weight, up, down, sys
    int   wt1_bin;
    int   wt2_bin;
    int   wt3_bin;
    int   wt1_group;
    int   wt2_group;
    int   wt3_group;
    float trig_wt = 1.f;

    int   trigger;
    bool  matched;
    bool  fired;

    bool  isLoose;

    int   genFlavor;
    int   genID;
    float ptSF[3] = {1.f, 1.f, 1.f};
    float ES[3] = {1.f, 1.f, 1.f};

    bool  jetOverlap;

    bool  isElectron() {return std::abs(flavor) == 11;}
    bool  isMuon    () {return std::abs(flavor) == 13;}
    bool  isTau     () {return std::abs(flavor) == 15;}
    int   charge    () {return (flavor != 0) ? flavor/std::abs(flavor) : 0;}
    float absEta    () {return std::fabs(eta);}

    void setPtEtaPhiM(const float vpt, const float veta, const float vphi, const float vmass) {
      if(p4) p4->SetPtEtaPhiM(vpt, veta, vphi, vmass);
      pt = vpt; eta = veta; phi = vphi; mass = vmass;
    }
    void setP(TLorentzVector p) { setPtEtaPhiM(p.Pt(), p.Eta(), p.Phi(), p.M()); }
  };
}
#endif
