#ifndef __JET__HH
#define __JET__HH

#include "interface/GlobalConstants.h"

namespace CLFV {
  struct Jet_t {
    float pt;
    float eta;
    float phi;
    float mass;
    PtEtaPhiMVector* p4 = nullptr;
    int   flavor;
    float iso;
    float dxy;
    float dz;
    float dxySig;
    float dzSig;

    float jer_pt_up;
    float jer_pt_down;
    float jes_pt_up;
    float jes_pt_down;

    int   ID;
    int   puID;
    int   btag;
    float bMVA;

    int   genFlavor;
    int   genID;

    bool  lepOverlap;

    int   charge    () {return (flavor != 0) ? flavor/std::abs(flavor) : 0;}
    float absEta    () {return std::fabs(eta);}

    void setPtEtaPhiM(const float vpt, const float veta, const float vphi, const float vmass) {
      if(p4) p4->SetCoordinates(vpt, veta, vphi, vmass);
      pt = vpt; eta = veta; phi = vphi; mass = vmass;
    }
    void setP(PtEtaPhiMVector  p) { setPtEtaPhiM(p.Pt(), p.Eta(), p.Phi(), p.M()); }
    void setP(TLorentzVector   p) { setPtEtaPhiM(p.Pt(), p.Eta(), p.Phi(), p.M()); }
  };
}
#endif
