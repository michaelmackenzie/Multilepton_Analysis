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
    int   trigger_id;
    float trig_data_eff;
    float trig_mc_eff;

    //Mu8_Ele23 trigger
    int   emu_trigger;
    bool  emu_matched;
    bool  emu_fired;
    int   emu_trigger_id;
    float emu_trig_data_eff;
    float emu_trig_mc_eff;

    //Mu23_Ele12 trigger
    int   mue_trigger;
    bool  mue_matched;
    bool  mue_fired;
    int   mue_trigger_id;
    float mue_trig_data_eff;
    float mue_trig_mc_eff;

    bool  isLoose;

    //generator truth information
    int   genFlavor;
    int   genID;
    float genPt;
    float genEta;
    float genPhi;
    float genMass;
    int   genIndex;
    bool  isPileup;
    bool  jetOrigin;

    //energy and momentum scales and up/down systematic scales
    float ptSF[3] = {1.f, 1.f, 1.f};
    float ES[3] = {1.f, 1.f, 1.f};
    float Res[3] = {1.f, 1.f, 1.f}; //correction applied to correct the resolution

    bool  jetOverlap;

    int index; //index in the lepton collection

    bool  isElectron () {return std::abs(flavor) == 11;}
    bool  isMuon     () {return std::abs(flavor) == 13;}
    bool  isTau      () {return std::abs(flavor) == 15;}
    int   charge     () {return (flavor != 0) ? flavor/std::abs(flavor) : 0;}
    float absEta     () {return std::fabs(eta);}
    float pt_up      () {return (ES[0] > 0.f && ES[1] > 0.f) ? pt*ES[1]/ES[0] : pt;}
    float pt_down    () {return (ES[0] > 0.f && ES[2] > 0.f) ? pt*ES[2]/ES[0] : pt;}
    float pt_res_up  () {return (Res[0] > 0.f && Res[1] > 0.f) ? pt*Res[1]/Res[0] : pt;}
    float pt_res_down() {return (Res[0] > 0.f && Res[2] > 0.f) ? pt*Res[2]/Res[0] : pt;}

    void setPtEtaPhiM(const float vpt, const float veta, const float vphi, const float vmass) {
      if(p4) p4->SetPtEtaPhiM(vpt, veta, vphi, vmass);
      pt = vpt; eta = veta; phi = vphi; mass = vmass;
    }
    void setP(TLorentzVector p) { setPtEtaPhiM(p.Pt(), p.Eta(), p.Phi(), p.M()); }
  };
}
#endif
