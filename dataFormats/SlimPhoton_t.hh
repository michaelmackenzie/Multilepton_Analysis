#ifndef __SLIMPHOTON__HH
#define __SLIMPHOTON__HH
#include "SlimObject_t.hh"

class SlimPhoton_t : public SlimObject_t {
public:
  SlimPhoton_t() : SlimObject_t(),
		   deltaEta(0.),
		   MVA(0.), MVA17(0.) {}
  Float_t deltaEta;
  Float_t MVA;
  Float_t MVA17;
  Bool_t  WP80;
  ClassDef(SlimPhoton_t,2)
};
class SlimPhotons_t : public TObject {
public:
  SlimPhotons_t() : size_(0) {}
  SlimPhotons_t(UInt_t size) {size_=size;}
  SlimPhoton_t& operator[](UInt_t index) {return slimPhotons[index];}
  UInt_t size_;
  const static UInt_t MAXSIZE = 50;
  SlimPhoton_t slimPhotons[MAXSIZE];
  ClassDef(SlimPhotons_t, 2)
};
#endif
