#ifndef __SLIMTAU__HH
#define __SLIMTAU__HH
#include "SlimObject_t.hh"

class SlimTau_t : public SlimObject_t {
public:
  Int_t decayMode;
  Bool_t idDecayMode;
  UChar_t anitEle;
  UChar_t anitEle2018;
  UChar_t antiMu;
};

#endif
