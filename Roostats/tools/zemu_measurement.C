#include "measurement_consistency.C"

int zemu_measurement() {
  preliminary_     = true;
  add_values_      = false;
  blinding_offset_ = false;
  add_asimov_      = false;
  TString tag = "v01";
  return measurement_consistency({
      config_t("low_bin" , "0.3 < BDT < 0.7", {11}      , {2016,2017,2018}, 2.62e-7),
      config_t("mid_bin" , "0.7 < BDT < 0.9", {12}      , {2016,2017,2018}, 2.62e-7),
      config_t("high_bin", "0.9 < BDT < 1"  , {13}      , {2016,2017,2018}, 2.62e-7),
      config_t("total"   , "Total"          , {11,12,13}, {2016,2017,2018}, 2.62e-7)}, tag, "zemu", -1);
}
