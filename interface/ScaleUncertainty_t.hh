#ifndef __SCALEUNCERTAINTY__
#define __SCALEUNCERTAINTY__

#include "TString.h"

namespace CLFV {

  //Scale uncertainty object
  struct ScaleUncertainty_t {
    TString  name_   ; //name of the uncertainty
    Double_t scale_  ; //scale to apply overall
    TString  process_; //process to consider, "" to not apply
    TString  tag_    ; //process tag to consider, "" to not apply
    TString  veto_   ; //process tag to reject, "" to not apply
    Int_t    year_   ; //year to consider, < 0 to not apply
    ScaleUncertainty_t(TString name, Double_t scale,
                       TString process = "", TString tag = "", TString veto = "",
                       Int_t year = -1): name_(name), scale_(scale), process_(process), tag_(tag), veto_(veto), year_(year) {}
  };
}
#endif
