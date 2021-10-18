#ifndef __SLIMJET__HH
#define __SLIMJET__HH
#include "SlimObject_t.hh"

namespace CLFV {

  class SlimJet_t : public SlimObject_t {
  public:
    SlimJet_t() : SlimObject_t(),
                  bTagCMVA(0.), bTagDeep(0.),
                  cJetMVA(0.), ID(0), puID(0){}
    Float_t bTagCMVA;
    Float_t bTagDeep;
    Float_t cJetMVA;
    Int_t ID;
    Int_t puID;
    ClassDef(SlimJet_t,2)
  };
  class SlimJets_t : public TObject {
  public:
    SlimJets_t() : size_(0) {}
    SlimJets_t(UInt_t size) {size_=size;}
    SlimJet_t& operator[](UInt_t index) {return slimJets[index];}
    UInt_t size_;
    const static UInt_t MAXSIZE = 50;
    SlimJet_t slimJets[MAXSIZE];
    ClassDef(SlimJets_t, 3)
  };
}
#endif
