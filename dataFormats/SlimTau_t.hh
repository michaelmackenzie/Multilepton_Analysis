#ifndef __SLIMTAU__HH
#define __SLIMTAU__HH
#include "SlimObject_t.hh"
#include <TObject.h>

class SlimTau_t : public SlimObject_t {
public:
  SlimTau_t() : SlimObject_t(),
		decayMode(0), idDecayMode(false), antiEle(0), antiEle2018(0), antiMu(0) {}
  Int_t decayMode;
  Bool_t idDecayMode;
  UChar_t antiEle;
  UChar_t antiEle2018;
  UChar_t antiMu;
  ClassDef(SlimTau_t, 1)
};

class SlimTaus_t : public TObject {
public:
  SlimTaus_t() : size_(MAXSIZE) {}
  SlimTaus_t(UInt_t size) {size_=size;}
  SlimTau_t& operator[](UInt_t index) {return slimTaus[index];}
  UInt_t size_;
  const static UInt_t MAXSIZE = 20;
  SlimTau_t slimTaus[MAXSIZE];
  ClassDef(SlimTaus_t, 1)
};
#endif