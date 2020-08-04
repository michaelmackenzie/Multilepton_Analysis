#ifndef __SLIMJET__HH
#define __SLIMJET__HH
#include "SlimObject_t.hh"

class SlimJet_t : public SlimObject_t {
public:
  SlimJet_t() : SlimObject_t(),
		bJetMVA(0.), cJetMVA(0.) {}
  Float_t bJetMVA;
  Float_t cJetMVA;
  ClassDef(SlimJet_t,1)
};
class SlimJets_t : public TObject {
public:
  SlimJets_t() : size_(0) {}
  SlimJets_t(UInt_t size) {size_=size;}
  SlimJet_t& operator[](UInt_t index) {return slimJets[index];}
  UInt_t size_;
  const static UInt_t MAXSIZE = 20;
  SlimJet_t slimJets[MAXSIZE];
  ClassDef(SlimJets_t, 1)
};
#endif
