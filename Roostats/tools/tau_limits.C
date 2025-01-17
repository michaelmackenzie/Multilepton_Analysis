//Script to plot the total tau channel limits
#include "make_combine_limit_plot_general.C"

int tau_limits(bool processCards = false,
               bool doNoSys = false,
               bool doObs = true,
               TString card_tag = "",
               TString fig_tag = "v09j") {
  scale_ = 1.e-6;
  add_values_ = false; //don't add expected limit values to the plot
  log_plot_   = false; //plot the x-axis in linear scale

  int status = 0;
  vector<int> sets_had = {25,26,27,28};
  vector<int> sets_lep = {25,26,27};
  vector<int> years = {2016,2017,2018};
  const float r_range = 40.f;
  vector<config_t> cards;
  cards.push_back(config_t("mva_zetau_25_26_27_28_2016_2017_2018", "e#tau_{h}"  , sets_had, years, scale_, -r_range, r_range));
  cards.push_back(config_t("mva_zetau_mu_25_26_27_2016_2017_2018", "e#tau_{#mu}", sets_lep, years, scale_, -r_range, r_range));
  cards.push_back(config_t("mva_total_zetau_had_25_26_27_28_lep_25_26_27_2016_2017_2018", "e#tau", sets_had, years, scale_, -r_range, r_range));
  cards.push_back(config_t("mva_zmutau_25_26_27_28_2016_2017_2018", "#mu#tau_{h}"  , sets_had, years, scale_, -r_range, r_range));
  cards.push_back(config_t("mva_zmutau_e_25_26_27_2016_2017_2018", "#mu#tau_{e}", sets_lep, years, scale_, -r_range, r_range));
  cards.push_back(config_t("mva_total_zmutau_had_25_26_27_28_lep_25_26_27_2016_2017_2018", "#mu#tau", sets_had, years, scale_, -r_range, r_range));

  TString tag = "total";
  if(doNoSys) tag += "_nosys";
  if(add_values_) tag += "_vals";
  if(fig_tag != "") tag += "_" + fig_tag;
  status += make_combine_limit_plot_general(cards, tag, "tau", processCards, doNoSys, doObs);
  return status;
}
