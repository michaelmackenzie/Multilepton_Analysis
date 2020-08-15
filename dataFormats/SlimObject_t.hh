#ifndef __SLIMOBJECT__HH
#define __SLIMOBJECT__HH
#include <TObject.h>
class SlimObject_t : public TObject {
public:
  SlimObject_t():
    pt(0.), eta(0.), phi(0.), mass(0.), positive(false) {}
  Float_t pt;
  Float_t eta;
  Float_t phi;
  Float_t mass;
  Bool_t  positive;
  ClassDef(SlimObject_t, 2)
};
class SlimObjects_t : public TObject {
public:
  SlimObjects_t() : size_(0) {}
  SlimObjects_t(UInt_t size) {size_=size;}
  SlimObject_t& operator[](UInt_t index) {return slimObjects[index];}
  UInt_t size_;
  const static UInt_t MAXSIZE = 30;
  SlimObject_t slimObjects[MAXSIZE];
  ClassDef(SlimObjects_t, 2)
};
#endif
