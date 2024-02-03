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
    sys_vals.push_back(sys_info.GetNum("JetToTauBiasRate0"));
    sys_vals.push_back(sys_info.GetNum("JetToTauComp"));
  }

  //common to channels with leptonic taus
  if(selection_.Contains("_")) {
    for(int isys = 0; isys < 6; ++isys) sys_vals.push_back(sys_info.GetNum(Form("QCDAltJ%iA%i", isys/(2)/*3 jet bins*/, (isys/2)%2/*2 params*/)));
    sys_vals.push_back(sys_info.GetNum("QCDNC"));
    sys_vals.push_back(sys_info.GetNum("QCDBias"));
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
// individual lepton et
Int_t print_eta(int set, bool add_sys = false) {
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

  //create a list of systematics to consider in the MVA histogram
  std::vector<fpair> sys;
  std::vector<scale_pair> scale_sys;
  if(add_sys) {
    get_mva_systematics(sys, scale_sys);
  }
  PlottingCard_t card(hist.Data(), "event", set+offset, 0, 0., 1., (selection_.Contains("tau")) ? 0.5 : 2., 1.);
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
// collinear mass estimate effect for the signal
Int_t print_signal_colm(int set) {
  if(!dataplotter_) return -1;
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
