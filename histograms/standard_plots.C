//Describes standard plots to be printed

#ifndef __CLFV_STANDARD_PLOTS__
#define __CLFV_STANDARD_PLOTS__

#include "datacards.C"
#include "dataplotter_clfv.C"

//------------------------------------------------------------------------------------------------------------------------------
// Offset to the selection's histogram block
Int_t get_offset() {
  int offset = 0;
  if     (selection_ == "mutau"  ) offset = HistMaker::kMuTau;
  else if(selection_ == "etau"   ) offset = HistMaker::kETau;
  else if(selection_ == "emu"    ) offset = HistMaker::kEMu;
  else if(selection_ == "mumu"   ) offset = HistMaker::kMuMu;
  else if(selection_ == "ee"     ) offset = HistMaker::kEE;
  else if(selection_ == "mutau_e") offset = (useLepTauSet_) ? HistMaker::kMuTauE : HistMaker::kEMu;
  else if(selection_ == "etau_mu") offset = (useLepTauSet_) ? HistMaker::kETauMu : HistMaker::kEMu;
  else {
    cout << __func__ << ": Undefined selection " << selection_.Data() << endl;
  }
  return offset;
}

//------------------------------------------------------------------------------------------------------------------------------
// di-lepton mass distribution
Int_t print_mass(int set, bool add_sys = false, double mass_min = 1., double mass_max = -1.) {
  const Int_t offset = ((set % 1000) > 100) ? 0 : get_offset();
  if(mass_min >= mass_max) {
    mass_min = (selection_.Contains("tau")) ?  40. :  70.;
    mass_max = (selection_.Contains("tau")) ? 170. : 110.;
  }
  PlottingCard_t card("lepm", "event", set+offset, 5, mass_min, mass_max);
  if((selection_ == "emu" || selection_.Contains("_")) && set < 1000) {
    card.blindmin_ = {84.};
    card.blindmax_ = {98.};
  }
  Int_t status(0);
  std::vector<std::pair<TString,TString>> systematics;
  std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>> scale_sys;
  if(add_sys) {
    CLFV::Systematics sys_info;
    //add MET uncertainty approximation
    systematics.push_back(std::pair<TString,TString>("massup", "massdown"));
    //add relevant scale uncertainties
    vector<TString> names = {"XS_LumiUC0", "XS_LumiUC1", "XS_LumiUC2", "XS_Z", "XS_WW", "XS_ttbar"};
    for(TString name : names) {
      int isys = sys_info.GetNum(name);
      scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
    }

    //add normalization uncertainties that are uncorrelated between years
    if(useEmbed_) {
      for(int year : years_) {
        int isys = sys_info.GetNum("XS_Embed");
        scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
        auto scale = scale_sys.back();
        scale.first.years_ = {year};
        scale.second.years_ = {year};
      }
    }

    //Add to the card
    card.sys_list_ = systematics;
    card.scale_sys_list_ = scale_sys;
  }
  for(int logY = 0; logY < 2; ++logY) {
    dataplotter_->logY_ = logY;
    auto c = dataplotter_->print_stack(card);
    if(c) DataPlotter::Empty_Canvas(c);
    else ++status;
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// MET distribution
Int_t print_met(int set, bool add_sys = false) {
  const Int_t offset = ((set % 1000) > 100) ? 0 : get_offset();
  PlottingCard_t card("met", "event", set+offset, 2,  0., 100.);
  Int_t status(0);
  std::vector<std::pair<TString,TString>> systematics;
  std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>> scale_sys;
  if(add_sys) {
    CLFV::Systematics sys_info;
    //add MET uncertainty approximation
    systematics.push_back(std::pair<TString,TString>("metup", "metdown"));
    //add relevant scale uncertainties
    vector<TString> names = {"XS_LumiUC0", "XS_LumiUC1", "XS_LumiUC2", "XS_Z", "XS_WW", "XS_ttbar"};
    for(TString name : names) {
      int isys = sys_info.GetNum(name);
      scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
    }

    //add normalization uncertainties that are uncorrelated between years
    if(useEmbed_) {
      for(int year : years_) {
        int isys = sys_info.GetNum("XS_Embed");
        scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
        auto scale = scale_sys.back();
        scale.first.years_ = {year};
        scale.second.years_ = {year};
      }
    }

    //Add to the card
    card.sys_list_ = systematics;
    card.scale_sys_list_ = scale_sys;
  }
  for(int logY = 0; logY < 2; ++logY) {
    dataplotter_->logY_ = logY;
    auto c = dataplotter_->print_stack(card);
    if(c) DataPlotter::Empty_Canvas(c);
    else ++status;
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// delta lepton pT
Int_t print_ptdiff(int set, bool add_sys = false) {
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  PlottingCard_t card("ptdiff", "lep", set+offset, 2, (same_flavor) ? 0. : -100., 100.);
  Int_t status(0);
  for(int logY = 0; logY < 2; ++logY) {
    dataplotter_->logY_ = logY;
    auto c = dataplotter_->print_stack(card);
    if(c) DataPlotter::Empty_Canvas(c);
    else ++status;
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// individual lepton pT
Int_t print_pt(int set, bool add_sys = false) {
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> leps = {"one","two"};
  for(TString lep : leps) {
    double xmin = (selection_.EndsWith("tau")) ? 20. : 10.;
    PlottingCard_t card(lep+"pt", "lep", set+offset, 2, xmin, 100.);
    std::vector<std::pair<TString,TString>> systematics;
    std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>> scale_sys;
    if(add_sys) {
      CLFV::Systematics sys_info;
      //add MET uncertainty approximation
      systematics.push_back(std::pair<TString,TString>(lep+"ptup", lep+"ptdown"));
      //add relevant scale uncertainties
      vector<TString> names = {"XS_LumiUC0", "XS_LumiUC1", "XS_LumiUC2", "XS_Z", "XS_WW", "XS_ttbar"};
      for(TString name : names) {
        int isys = sys_info.GetNum(name);
        scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
      }

      //add normalization uncertainties that are uncorrelated between years
      if(useEmbed_) {
        for(int year : years_) {
          int isys = sys_info.GetNum("XS_Embed");
          scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
          auto scale = scale_sys.back();
          scale.first.years_ = {year};
          scale.second.years_ = {year};
        }
      }

      //Add to the card
      card.sys_list_ = systematics;
      card.scale_sys_list_ = scale_sys;
    }
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// individual lepton dxy/dz
Int_t print_dxyz(int set, bool add_sys = false) {
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> leps = {"one","two"};
  std::vector<TString> hists = {"dxysig", "dzsig"};
  for(TString lep : leps) {
    for(TString hist : hists) {
      PlottingCard_t card(lep+hist, "lep", set+offset, 1, 0., 5.);
      for(int logY = 1; logY < 2; ++logY) { //only make a log plot
        dataplotter_->logY_ = logY;
        auto c = dataplotter_->print_stack(card);
        if(c) DataPlotter::Empty_Canvas(c);
        else ++status;
      }
    }
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// individual lepton m_T with the MET
Int_t print_mt(int set, bool add_sys = false) {
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> leps = {"one","two"};
  for(TString lep : leps) {
    PlottingCard_t card("mt"+lep, "event", set+offset, 2, 0., 100.);
    std::vector<std::pair<TString,TString>> systematics;
    std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>> scale_sys;
    if(add_sys) {
      CLFV::Systematics sys_info;
      //add MET uncertainty approximation
      systematics.push_back(std::pair<TString,TString>("mt"+lep+"up", "mt"+lep+"down"));
      //add relevant scale uncertainties
      vector<TString> names = {"XS_LumiUC0", "XS_LumiUC1", "XS_LumiUC2", "XS_Z", "XS_WW", "XS_ttbar"};
      for(TString name : names) {
        int isys = sys_info.GetNum(name);
        scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
      }

      //add normalization uncertainties that are uncorrelated between years
      if(useEmbed_) {
        for(int year : years_) {
          int isys = sys_info.GetNum("XS_Embed");
          scale_sys.push_back(std::pair<ScaleUncertainty_t,ScaleUncertainty_t>(sys_info.GetScale(isys), sys_info.GetScale(isys+1)));
          auto scale = scale_sys.back();
          scale.first.years_ = {year};
          scale.second.years_ = {year};
        }
      }

      //Add to the card
      card.sys_list_ = systematics;
      card.scale_sys_list_ = scale_sys;
    }
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// MVA distributions
Int_t print_mva(int set, bool add_sys = false, bool all_versions = false) {
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  TString hist = "mva";
  if    (selection_ == "mutau"   ) hist += "1";
  else if(selection_ == "etau"   ) hist += "3";
  else if(selection_ == "emu"    ) hist += "5";
  else if(selection_ == "mutau_e") hist += "7";
  else if(selection_ == "etau_mu") hist += "9";
  else if(selection_ == "mumu"   ) hist += "5";
  else if(selection_ == "ee"     ) hist += "5";
  PlottingCard_t card(hist.Data(), "event", set+offset, 0, 0., 1., (selection_.Contains("tau")) ? 0.5 : 2., 1.);
  for(int logY = 0; logY < 2; ++logY) {
    dataplotter_->logY_ = logY;
    auto c = dataplotter_->print_stack(card);
    if(c) DataPlotter::Empty_Canvas(c);
    else ++status;
  }
  dataplotter_->logY_ = 0;
  return status;
}

#endif
