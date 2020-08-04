#ifndef __SLIMELECTRON__HH
#define __SLIMELECTRON__HH
#include "SlimObject_t.hh"

class SlimElectron_t : public SlimObject_t {
public:
  Float_t scDeltaEta;
  Bool_t WPL;
  Bool_t WP80;
  Bool_t WP90;
};

#endif
