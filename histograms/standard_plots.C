//Describes standard plots to be printed

#ifndef __CLFV_STANDARD_PLOTS__
#define __CLFV_STANDARD_PLOTS__

#include "datacards.C"
#include "dataplotter_clfv.C"

//------------------------------------------------------------------------------------------------------------------------------
// Get standard systematic uncertainties for MVA plots
void get_mva_systematics(std::vector<fpair>& sys, std::vector<scale_pair>& scale_sys) {
  TString hist = "mva";
  if    (selection_ == "mutau"   ) hist += "1";
  else if(selection_ == "etau"   ) hist += "3";
  else if(selection_ == "emu"    ) hist += "5";
  else if(selection_ == "mutau_e") hist += "7";
  else if(selection_ == "etau_mu") hist += "9";
  else if(selection_ == "mumu"   ) hist += "5"; //use the Z->e+mu search MVA for Z->ll
  else if(selection_ == "ee"     ) hist += "5";

  CLFV::Systematics sys_info;
  vector<int> sys_vals;
  const bool emu_search = selection_ == "emu";

  //common to all searches
  sys_vals.push_back(sys_info.GetNum("ZPt"));
  sys_vals.push_back(sys_info.GetNum("BTag"));
  sys_vals.push_back(sys_info.GetNum("BTagLight"));
  sys_vals.push_back(sys_info.GetNum("JER"));
  sys_vals.push_back(sys_info.GetNum("JES"));
  sys_vals.push_back(sys_info.GetNum("SignalMixing"));
  sys_vals.push_back(sys_info.GetNum("XS_Z"));
  sys_vals.push_back(sys_info.GetNum("XS_ttbar"));
  // sys_vals.push_back(sys_info.GetNum("XS_toptw"));
  // sys_vals.push_back(sys_info.GetNum("XS_toptCh"));
  sys_vals.push_back(sys_info.GetNum("XS_Embed"));
  sys_vals.push_back(sys_info.GetNum("XS_WJets"));
  sys_vals.push_back(sys_info.GetNum("XS_WW"));
  // sys_vals.push_back(sys_info.GetNum("XS_ZZ"));
  // sys_vals.push_back(sys_info.GetNum("XS_WZ"));
  sys_vals.push_back(sys_info.GetNum("XS_LumiUC0"));
  sys_vals.push_back(sys_info.GetNum("XS_LumiUC1"));
  sys_vals.push_back(sys_info.GetNum("XS_LumiUC2"));
  // sys_vals.push_back(sys_info.GetNum("XS_LumiCA0"));
  // sys_vals.push_back(sys_info.GetNum("XS_LumiCA1"));
  // sys_vals.push_back(sys_info.GetNum("XS_LumiCA2"));
  // sys_vals.push_back(sys_info.GetNum("XS_LumiCB1"));
  // sys_vals.push_back(sys_info.GetNum("XS_LumiCB2"));
  if(!emu_search) {
    sys_vals.push_back(sys_info.GetNum("EmbMET"));
  }

  //common to channels with muons
  if(selection_.Contains("mu")) {
    sys_vals.push_back(sys_info.GetNum("MuonID"));
    sys_vals.push_back(sys_info.GetNum("MuonIsoID"));
    sys_vals.push_back(sys_info.GetNum("MuonES"));
    // sys_vals.push_back(sys_info.GetNum("MuonTrig"));
    if(!emu_search) {
      sys_vals.push_back(sys_info.GetNum("EmbMuonID"));
      sys_vals.push_back(sys_info.GetNum("EmbMuonIsoID"));
      sys_vals.push_back(sys_info.GetNum("EmbMuonES"));
      // sys_vals.push_back(sys_info.GetNum("EmbMuonES0"));
      // sys_vals.push_back(sys_info.GetNum("EmbMuonES1"));
      // sys_vals.push_back(sys_info.GetNum("EmbMuonES2"));
      // sys_vals.push_back(sys_info.GetNum("EmbMuonTrig"));
    }
  }

  //common to channels with electrons
  if(selection_.Contains("e")) {
    sys_vals.push_back(sys_info.GetNum("EleES"));
    // sys_vals.push_back(sys_info.GetNum("EleTrig"));
    if(!emu_search) {
      sys_vals.push_back(sys_info.GetNum("EmbEleES"));
      sys_vals.push_back(sys_info.GetNum("EmbEleES1"));
      // sys_vals.push_back(sys_info.GetNum("EmbEleTrig"));
    }
  }

  //common to channels with taus
  if(selection_ == "mutau" || selection_ == "etau") {
    sys_vals.push_back(sys_info.GetNum("TauES"));
    sys_vals.push_back(sys_info.GetNum("EmbTauES"));
    sys_vals.push_back(sys_info.GetNum("TauMuID"));
    sys_vals.push_back(sys_info.GetNum("TauEleID"));
    // for(int isys = 0; isys < 5; ++isys) sys_vals.push_back(sys_info.GetNum(Form("TauJetID%i", isys)));
    // for(int isys = 0; isys < 5; ++isys) sys_vals.push_back(sys_info.GetNum(Form("EmbTauJetID%i", isys)));
    // for(int isys = 0; isys < 36; ++isys) sys_vals.push_back(sys_info.GetNum(Form("JetToTauAltP%iD%iA%i", isys/(12)/*3 procs*/, (isys/3)%4/*4 decay modes*/, isys%3/*3 params*/)));
    for(int isys = 0; isys < 3; ++isys) sys_vals.push_back(sys_info.GetNum(Form("JetToTauNC%i"  ,isys)));
    for(int isys = 0; isys < 3; ++isys) sys_vals.push_back(sys_info.GetNum(Form("JetToTauBias%i",isys)));
    sys_vals.push_back(sys_info.GetNum("JetToTauBiasRate0")); //FIXME: only include if using W+jets shape-only bias correction
    sys_vals.push_back(sys_info.GetNum("JetToTauComp"));
  }

  //common to channels with leptonic taus
  if(selection_.Contains("_")) {
    for(int isys = 0; isys < 6; ++isys) sys_vals.push_back(sys_info.GetNum(Form("QCDAltJ%iA%i", isys/(2)/*3 jet bins*/, (isys/2)%2/*2 params*/)));
    sys_vals.push_back(sys_info.GetNum("QCDNC"));
    sys_vals.push_back(sys_info.GetNum("QCDBias"));
    sys_vals.push_back(sys_info.GetNum("QCDMassBDTBias"));
  }

  for(int isys : sys_vals) {
    if(isys < CLFV::kMaxSystematics) {
      sys.push_back(fpair(hist + Form("_%i", isys), hist + Form("_%i", isys+1)));
    } else {
      std::pair<ScaleUncertainty_t,ScaleUncertainty_t> sys_pair(sys_info.GetScale(isys), sys_info.GetScale(isys+1));
      if(sys_pair.first.name_ != "" && sys_pair.second.name_ != "") {
        scale_sys.push_back(sys_pair);
        // cout << "Adding scale systematic " << sys_pair.first.name_.Data() << endl;
      }
    }
  }

}

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
  if(!dataplotter_) return 1;
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
    //add uncertainty approximation from histogramming
    systematics.push_back(std::pair<TString,TString>("lepmup", "lepmdown"));
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
  if(!dataplotter_) return 1;
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
// leading jet pT
Int_t print_jet(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  Int_t status(0);
  {
    PlottingCard_t card("jetpt", "event", set+offset, 2, 20., 100.);
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
  }
  {
    PlottingCard_t card("njets20", "event", set+offset, 0, 0, 5);
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
// delta lepton pT
Int_t print_ptdiff(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
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
// lepton pT ratio
Int_t print_ptratio(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  PlottingCard_t card("ptratio", "lep", set+offset, (same_flavor) ? 1 : 2, 0., 2.5);
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
// pT resolution info
Int_t print_pt_res(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  std::vector<TString> leps = {"one", "two"};
  Int_t status(0);
  for(TString lep : leps) {
    PlottingCard_t card((lep + "ptrelerr").Data(), "lep", set+offset, 1, 1., -1.);
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
    dataplotter_->logY_ = 0;
  }
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// di-lepton pt
Int_t print_leppt(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  PlottingCard_t card("leppt", "event", set+offset, (same_flavor) ? 1 : 2, 0., 100);
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
// di-lepton eta
Int_t print_lepeta(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  PlottingCard_t card("lepeta", "event", set+offset, (same_flavor) ? 1 : 2, -5., 5);
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
// di-lepton delta phi
Int_t print_deltaphi(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  PlottingCard_t card("lepdeltaphi", "event", set+offset, (same_flavor) ? 1 : 2, 0., 5);
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
// lepton delta phi from the MET
Int_t print_lep_metdeltaphi(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  vector<TString> hists = {"onemetdeltaphi", "twometdeltaphi"};
  Int_t status(0);
  for(TString hist : hists) {
    PlottingCard_t card(hist, "lep", set+offset, (same_flavor) ? 1 : 2, 0., 3.2);
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
    dataplotter_->logY_ = 0;
  }
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// lepton trk pt / pt
Int_t print_trkpt(int set) {
  if(!dataplotter_) return 1;
  if(selection_ != "mutau" && selection_ != "etau") return 0;
  const Int_t offset = get_offset();
  Int_t status(0);
  PlottingCard_t card("twotrkptoverpt", "lep", set+offset, 1, 0., 1.);
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
// lepton beta/alpha variables
Int_t print_lep_beta(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  const bool lep_tau = selection_.Contains("_");
  vector<TString> hists = {"beta0", "beta1"};
  const double blind_min = 0.8;
  const double blind_max = 1.2;
  Int_t status(0);
  for(TString hist : hists) {
    bool blind = blind_ && hist == "beta0" && selection_.EndsWith("_e");
    blind |= blind_ && hist == "beta1" && !same_flavor && !selection_.EndsWith("_e");
    const double xmax = ((selection_ == "etau_mu" && hist == "beta0") || (selection_ == "mutau_e" && hist == "beta1")) ? 5. : 3.;
    PlottingCard_t card(hist, "event", set+offset, (same_flavor) ? 1 : 2, 0., xmax, (blind) ? blind_min : 1., (blind) ? blind_max : -1.);
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
    dataplotter_->logY_ = 0;
  }
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// individual lepton pT
Int_t print_pt(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
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
// individual lepton eta
Int_t print_eta(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> leps = {"one","two"};
  for(TString lep : leps) {
    PlottingCard_t card(lep+"eta", "lep", set+offset, 2, 1., -1.);
    std::vector<std::pair<TString,TString>> systematics;
    std::vector<std::pair<ScaleUncertainty_t,ScaleUncertainty_t>> scale_sys;
    if(add_sys) {
      CLFV::Systematics sys_info;
      //add MET uncertainty approximation
      systematics.push_back(std::pair<TString,TString>(lep+"etaup", lep+"etadown"));
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
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> leps = {"one","two"};
  std::vector<TString> hists = {"dxy", "dz", "dxysig", "dzsig"};
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
// individual lepton IDs
Int_t print_ids(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  Int_t status(0);
  std::vector<TString> hists = {};
  if(selection_.EndsWith("tau")) hists.push_back("twoid2");
  for(TString hist : hists) {
    PlottingCard_t card(hist, "lep", set+offset, 1, 0., 5.);
    for(int logY = 1; logY < 2; ++logY) { //only make a log plot
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
// lepton gen flavor
Int_t print_gen_flavor(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  Int_t status(0);
  std::vector<TString> hists = {"onegenflavor", "twogenflavor"};
  for(TString hist : hists) {
    PlottingCard_t card(hist, "lep", set+offset, 0, 1, -1);
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
// data run info
Int_t print_run(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  std::vector<TString> hists = {"runera", "mcera", "datarun"};
  Int_t status(0);
  for(TString hist : hists) {
    PlottingCard_t card(hist, "event", set+offset, 0, 1., -1.);
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
    dataplotter_->logY_ = 0;
  }
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// individual tau decay mode
Int_t print_dm(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  Int_t status(0);
  PlottingCard_t card("taudecaymode", "event", set+offset, 1, 1, -1);
  for(int logY = 0; logY < 2; ++logY) { //only make a log plot
    dataplotter_->logY_ = logY;
    auto c = dataplotter_->print_stack(card);
    if(c) DataPlotter::Empty_Canvas(c);
    else ++status;
  }
  dataplotter_->logY_ = 0;
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// individual lepton m_T with the MET
Int_t print_mt(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> leps = {"one","two","lep"};
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
// MT((one,two,lep),met) / mass
Int_t print_mt_over_m(int set, bool add_sys = false) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> hists = {"mtoneoverm", "mttwooverm", "mtlepoverm"};
  for(TString hist : hists) {
    PlottingCard_t card(hist, "event", set+offset, 1, 0., 1.5);
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
// collinear mass estimates
Int_t print_collinear_mass(int set, bool add_sys = false, double xmax = 170.) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  std::vector<TString> masses = {"lepmestimate", "lepmestimatetwo"};
  for(TString mass : masses) {
    PlottingCard_t card(mass, "event", set+offset, 2, 40., xmax, (blind_) ? 80. : 1.e3, 100.);
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
// boosted system
Int_t print_prime_sys(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  const bool same_flavor = selection_ == "ee" || selection_ == "mumu";
  Int_t status(0);
  TString mode = (selection_ == "emu") ? "2" : (selection_ == "mutau_e") ? "1" : "0";
  std::vector<TString> hists = {"px", "py", "pz", "e"};
  std::vector<TString> objs  = {"lepone", "leptwo", "met"};
  for(TString obj : objs) {
    for(TString hist : hists) {
      if((mode == "0" || mode == "1") && obj == "met" && (hist == "py" || hist == "pz")) continue; //MET along x-axis in these modes
      TString name = obj+"prime"+hist+mode;
      PlottingCard_t card(name, "event", set+offset, 1, 1., -1.);
      for(int logY = 0; logY < 1; ++logY) {
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
// MVA distributions
Int_t print_mva(int set, bool add_sys = false, bool all_versions = false) {
  if(!dataplotter_) return 1;
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

  //create a list of systematics to consider in the MVA histogram
  std::vector<fpair> sys;
  std::vector<scale_pair> scale_sys;
  if(add_sys) {
    get_mva_systematics(sys, scale_sys);
  }
  const bool blind = blind_ && (set < 1000);
  PlottingCard_t card(hist.Data(), "event", set+offset, 0, 0., 1., (selection_.Contains("tau")) ? 0.5 : 2., (blind) ? 1. : -1.);
  for(int logY = 0; logY < 2; ++logY) {
    dataplotter_->logY_ = logY;
    auto c = dataplotter_->print_stack(card);
    if(c) DataPlotter::Empty_Canvas(c);
    else ++status;
  }
  if(add_sys) {
    PlottingCard_t card_sys((hist+"_0").Data(), "systematic", set+offset, 0, 0., 1., (selection_.Contains("tau")) ? 0.5 : 2., 1.);
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      card_sys.sys_list_ = sys;
      card_sys.scale_sys_list_ = scale_sys;
      auto c = dataplotter_->print_stack(card_sys);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
  }
  dataplotter_->logY_ = 0;
  return status;
}


//------------------------------------------------------------------------------------------------------------------------------
// Electron ID study information
Int_t print_ele_id_study(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  std::vector<PlottingCard_t> cards = {PlottingCard_t("eleconvveto"     , "lep"  , set+offset),
                                       PlottingCard_t("elepfcand"       , "lep"  , set+offset),
                                       PlottingCard_t("elelosthits"     , "lep"  , set+offset),
                                       PlottingCard_t("eleetaeta"       , "lep"  , set+offset),
                                       PlottingCard_t("eler9"           , "lep"  , set+offset),
                                       PlottingCard_t("elecutid"        , "lep"  , set+offset),
                                       PlottingCard_t("ele3dpverr"      , "lep"  , set+offset),
                                       PlottingCard_t("elescetoverpt"   , "lep"  , set+offset),
                                       PlottingCard_t("elehoe"          , "lep"  , set+offset, 1, 0., 0.15),
                                       PlottingCard_t("eleeinvminuspinv", "lep"  , set+offset, 1, -0.1, 0.05)
  };
  Int_t status(0);
  for(PlottingCard_t& card : cards) {
    for(int logY = 0; logY < 2; ++logY) {
      dataplotter_->logY_ = logY;
      auto c = dataplotter_->print_stack(card);
      if(c) DataPlotter::Empty_Canvas(c);
      else ++status;
    }
    dataplotter_->logY_ = 0;
  }
  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// Lepton energy scale effects
Int_t print_lep_es_study(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = get_offset();
  Int_t status(0);
  const float prev_rmin(dataplotter_->ratio_plot_min_), prev_rmax(dataplotter_->ratio_plot_max_);
  dataplotter_->ratio_plot_min_ = 0.9;
  dataplotter_->ratio_plot_max_ = 1.1;
  const int prev_comb(dataplotter_->combine_uncertainties_);
  dataplotter_->combine_uncertainties_ = 0; //show the sys uncertainties separately
  const int prev_color(dataplotter_->stat_unc_color_);
  dataplotter_->stat_unc_color_ = 0; //make stat uncertainty color invisible
  const int prev_stack_unc(dataplotter_->stack_uncertainty_);
  dataplotter_->stack_uncertainty_ = 0; //don't add the stat uncertainty to the top plot or legend

  const bool tau_one = selection_ == "mutau_e";
  const bool emu_data = selection_ == "emu" || selection_.Contains("_");
  for(int lep = 0; lep < 2; ++lep) {
    const char* lep_t = (lep == 0) ? "one" : "two";
    std::vector<PlottingCard_t> cards = {PlottingCard_t("lepm"           , "event"  , set+offset, 1, 40., 170., (emu_data) ? 86. : 1., (emu_data) ? 96. : -1.),
                                         PlottingCard_t("lepmestimate"   , "event"  , set+offset, 1, 40., 170., 85., 95.),
                                         PlottingCard_t("lepmestimatetwo", "event"  , set+offset, 1, 40., 170., 85., 95.),
                                         PlottingCard_t("leppt"          , "event"  , set+offset),
                                         PlottingCard_t("onept"          , "event"  , set+offset),
                                         PlottingCard_t("twopt"          , "event"  , set+offset),
                                         PlottingCard_t("lepptratio"     , "event"  , set+offset),
                                         PlottingCard_t("met"            , "event"  , set+offset),
                                         PlottingCard_t("mtone"          , "event"  , set+offset),
                                         PlottingCard_t("mttwo"          , "event"  , set+offset),
                                         PlottingCard_t("mtlep"          , "event"  , set+offset),
                                         PlottingCard_t("onemetdeltaphi" , "event"  , set+offset),
                                         PlottingCard_t("twometdeltaphi" , "event"  , set+offset),
                                         PlottingCard_t("beta0"          , "event"  , set+offset, 1, 0., 3., ( tau_one) ? 0.9 : 1., ( tau_one) ? 1.1 : -1.),
                                         PlottingCard_t("beta1"          , "event"  , set+offset, 1, 0., 3., (!tau_one) ? 0.9 : 1., (!tau_one) ? 1.1 : -1.),
                                         PlottingCard_t("mva"            , "event"  , set+offset, 1, 0., 1., 0.5, 1.)
    };
    for(PlottingCard_t& card : cards) {
      card.hist_ = Form("lepes_%s%s", lep_t, card.hist_.Data());
      card.sys_list_ = {std::pair<TString,TString>(Form("%s_up", card.hist_.Data()), Form("%s_down", card.hist_.Data()))};
      for(int logY = 0; logY < 2; ++logY) {
        dataplotter_->logY_ = logY;
        auto c = dataplotter_->print_stack(card);
        if(c) DataPlotter::Empty_Canvas(c);
        else ++status;
      }
      dataplotter_->logY_ = 0;
    }
  }

  //restore the plotting configuration
  dataplotter_->ratio_plot_min_        = prev_rmin;
  dataplotter_->ratio_plot_max_        = prev_rmax;
  dataplotter_->combine_uncertainties_ = prev_comb;
  dataplotter_->stat_unc_color_        = prev_color;
  dataplotter_->stack_uncertainty_     = prev_stack_unc;

  return status;
}

//------------------------------------------------------------------------------------------------------------------------------
// collinear mass estimate effect for the signal
Int_t print_signal_colm(int set) {
  if(!dataplotter_) return 1;
  const Int_t offset = ((set % 1000) > 100) ? 0 : get_offset();
  TString hist = (selection_ == "mutau_e") ? "lepmestimatetwo" : "lepmestimate";
  auto hists_mass = dataplotter_->get_signal("lepm", "event", set+offset);
  if(hists_mass.size() == 0) {
    cout << __func__ << ": No nominal mass distributions found!\n";
    return 1;
  }
  auto hists_colm = dataplotter_->get_signal(hist, "event", set+offset);
  if(hists_colm.size() == 0) {
    cout << __func__ << ": No collinear mass distributions found!\n";
    return 1;
  }
  TString hist_cut = (selection_ == "mutau_e") ? "lepmestimatecut1" : "lepmestimatecut0";
  auto hists_cutm = dataplotter_->get_signal(hist_cut, "event", set+offset);
  if(hists_cutm.size() == 0) {
    cout << __func__ << ": No cut collinear mass distributions found!\n";
    return 1;
  }
  TH1* h_mass = hists_mass[0];
  TH1* h_colm = hists_colm[0];
  TH1* h_cutm = hists_cutm[0];

  const int nbins_mass = h_mass->GetNbinsX();
  const int nbins_colm = h_colm->GetNbinsX();
  const double width_mass = h_mass->GetBinWidth(1);
  const double width_colm = h_colm->GetBinWidth(1);
  const int ratio = (width_mass < width_colm) ? width_colm / width_mass : width_mass / width_colm;
  cout << nbins_mass << ", " << nbins_colm << "; " << width_mass << ", " << width_colm << " --> ratio = " << ratio << endl;
  if(width_mass < width_colm && nbins_mass % ratio == 0) h_mass->Rebin(ratio);
  if(width_colm < width_mass && nbins_colm % ratio == 0) h_colm->Rebin(ratio);
  h_mass->SetLineColor(kBlue);
  h_mass->SetLineWidth(2);
  h_mass->SetFillColor(0);
  h_colm->SetLineColor(kRed);
  h_colm->SetLineWidth(2);
  h_colm->SetFillColor(0);
  h_cutm->SetLineColor(kRed);
  h_cutm->SetLineWidth(2);
  h_cutm->SetLineStyle(kDashed);
  h_cutm->SetFillColor(0);

  if(h_mass->Integral() > 0.) h_mass->Scale(1./h_mass->Integral());
  if(h_colm->Integral() > 0.) h_colm->Scale(1./h_colm->Integral()*h_mass->GetBinWidth(1)/h_colm->GetBinWidth(1));
  if(h_cutm->Integral() > 0.) h_cutm->Scale(1./h_cutm->Integral()*h_mass->GetBinWidth(1)/h_cutm->GetBinWidth(1));
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas();
  h_colm->Draw("hist");
  h_mass->Draw("hist same");
  h_cutm->Draw("hist same");
  h_colm->Draw("hist same");

  h_colm->GetXaxis()->SetRangeUser(40., 140.);
  h_colm->GetYaxis()->SetRangeUser(0., 1.2*max(h_colm->GetMaximum(), h_mass->GetMaximum()));
  h_colm->SetTitle("Collinear mass estimate effect");
  h_colm->SetXTitle("Mass estimate");
  TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
  leg->AddEntry(h_mass, "Visible mass", "L");
  leg->AddEntry(h_colm, "Collinear mass", "L");
  leg->AddEntry(h_cutm, "Cut Collinear mass", "L");
  leg->Draw();

  TString fig_name = dataplotter_->GetFigureName("lepm", "comparison", set+offset, "signal");
  c->SaveAs(fig_name.Data());
  return 0;
}


#endif
