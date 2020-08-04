#ifndef __SLIMELECTRON__HH
#define __SLIMELECTRON__HH
#include "SlimObject_t.hh"

class SlimElectron_t : public SlimObject_t {
public:
  SlimElectron_t() : SlimObject_t(),
		     scDeltaEta(0.), WPL(false), WP80(false), WP90(false) {}
  Float_t scDeltaEta;
  Bool_t WPL;
  Bool_t WP80;
  Bool_t WP90;
  ClassDef(SlimElectron_t, 1)
};
class SlimElectrons_t : public TObject {
public:
  SlimElectrons_t() : size_(0) {}
  SlimElectrons_t(UInt_t size) {size_=size;}
  SlimElectron_t& operator[](UInt_t index) {return slimElectrons[index];}
  UInt_t size_;
  const static UInt_t MAXSIZE = 20;
  SlimElectron_t slimElectrons[MAXSIZE];
  ClassDef(SlimElectrons_t, 1)
};
#endif
