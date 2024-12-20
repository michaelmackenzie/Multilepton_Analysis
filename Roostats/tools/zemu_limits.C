//Script to process the individual and combined category limits and plot them
#include "make_combine_limit_plot_general.C"

int zemu_limits(bool processCards = true,
                bool doNoSys = false,
                bool doObs = true,
                TString card_tag = "",
                TString fig_tag = "v01") {
  scale_ = 2.62e-7;
  dir_ = "datacards/fromGeorge";
  add_values_ = false; //don't add expected limit values to the plot
  log_plot_   = false; //plot the x-axis in linear scale

  int status = 0;

  vector<config_t> cards;
  cards.push_back(config_t("datacard_prm_v5_bin1", "Low score", {11}, {2016,2017,2018}, scale_, 0., 5.));
  cards.push_back(config_t("datacard_prm_v5_bin2", "Med. score", {12}, {2016,2017,2018}, scale_, 0., 5.));
  cards.push_back(config_t("datacard_prm_v5_bin3", "High score", {13}, {2016,2017,2018}, scale_, 0., 5.));
  cards.push_back(config_t("datacard_prm_v5_total", "Combined", {11,12,13}, {2016,2017,2018}, scale_, 0., 5.));

  TString tag = "george";
  if(doNoSys) tag += "_nosys";
  if(add_values_) tag += "_vals";
  if(fig_tag != "") tag += "_" + fig_tag;
  status += make_combine_limit_plot_general(cards, tag, "zemu", processCards, doNoSys, doObs);
  return status;
}
