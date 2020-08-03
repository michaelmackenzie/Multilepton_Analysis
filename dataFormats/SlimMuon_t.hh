#ifndef __SLIMMUON__HH
#define __SLIMMUON__HH
#include "SlimObject_t.hh"

class SlimMuon_t : public SlimObject_t {
public:
  UChar_t pfIsoId;
  Bool_t looseId;
  Bool_t mediumId;
  Bool_t tightId;
};

#endif
