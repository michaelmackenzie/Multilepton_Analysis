#ifndef __SLIMMUON__HH
#define __SLIMMUON__HH
#include "SlimObject_t.hh"

class SlimMuon_t : public SlimObject_t {
public:
  SlimMuon_t() : SlimObject_t(),
		 pfIsoId(0), looseId(false), mediumId(false), tightId(false) {}
  UChar_t pfIsoId;
  Bool_t looseId;
  Bool_t mediumId;
  Bool_t tightId;
  ClassDef(SlimMuon_t, 1)
};
class SlimMuons_t : public TObject {
public:
  SlimMuons_t() : size_(0) {}
  SlimMuons_t(UInt_t size) {size_=size;}
  SlimMuon_t& operator[](UInt_t index) {return slimMuons[index];}
  UInt_t size_;
  const static UInt_t MAXSIZE = 20;
  SlimMuon_t slimMuons[MAXSIZE];
  ClassDef(SlimMuons_t, 1)
};
#endif
