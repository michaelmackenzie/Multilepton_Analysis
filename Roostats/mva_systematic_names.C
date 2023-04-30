//Get the name of the systematic from its index
#ifndef __SYSNAMES__
#define __SYSNAMES__

#include "mva_defaults.C"

CLFV::Systematics systematics_; //names of the systematics

std::pair<TString,TString> systematic_name(int sys, TString selection, int year) {
  TString name(systematics_.GetName(sys)), type("shape");

  if(embed_mode_ == 0) { //not using embedding samples
    if(name.BeginsWith("Emb")) name = "";
    if(name.Contains("Embed")) name = "";
  }

  //define which to ignore
  if     (name == "TauID"       ) name = ""; //fully correlated versions, for plotting only
  else if(name == "TauJetID"    ) name = "";
  else if(name == "JetToTauStat") name = "";
  else if(name == "JetToTauNC"  ) name = "";
  else if(name == "JetToTauBias") name = "";
  else if(name.Contains("QCDStat")) name = ""; //correlated fit parameter version, replace with fit shape variations
  else if(name == "EmbMuonES"   ) { //1 correlated bin
    if(embed_muon_scale_mode_ != 1) name = "";
  } else if(name.BeginsWith("EmbMuonES")) { //3 uncorrelated bins
    if(embed_muon_scale_mode_ != 0) name = "";
  }
  // else if(name == "EmbEleES"   ) name = ""; //1 correlated bin

  //implemented in scale uncertainty
  if(use_scale_sys_ && name == "EmbedUnfold" ) name = "";
  if(use_scale_sys_ && name == "Lumi"        ) name = "";

  if(name == "XS_LumiUC0" && year != 2016) name = ""; //2016 component
  if(name == "XS_LumiUC1" && year != 2017) name = ""; //2017 component
  if(name == "XS_LumiUC2" && year != 2018) name = ""; //2018 component
  if(name == "XS_LumiCA0" && year != 2016) name = ""; //2016 component
  if(name == "XS_LumiCA1" && year != 2017) name = ""; //2017 component
  if(name == "XS_LumiCA2" && year != 2018) name = ""; //2018 component
  if(name == "XS_LumiCB1" && year != 2017) name = ""; //2017 component
  if(name == "XS_LumiCB2" && year != 2018) name = ""; //2018 component
  if(name.BeginsWith("XS_LumiUC")) name = "XS_LumiUC"; //match normal naming scheme
  if(name.BeginsWith("XS_LumiCA")) name = "XS_LumiCA"; //ensure it's correlated between years
  if(name.BeginsWith("XS_LumiCB")) name = "XS_LumiCB"; //ensure it's correlated between years


  if(name == "Prefire" && year == 2018) name = ""; //not defined for 2018
  if(name == "METCorr") name = ""; //not a real uncertainty

  if(selection.EndsWith("etau")) {
    if(name.Contains("TauMuID") && name != "TauMuID") name = ""; //don't use finely binned nuisance parameters
    else if(name == "TauEleID") name = ""; //ignore coarse binned version
    else if(name.Contains("Muon") && !name.Contains("Tau")) name = ""; //ignore muon uncertainties in etau
  } else if(selection.EndsWith("mutau")) {
    if(name.Contains("TauEleID") && name != "TauEleID") name = ""; //don't use finely binned nuisance parameters
    else if(name == "TauMuID") name = ""; //ignore coarse binned version
    else if(name.Contains("Ele") && !name.Contains("Tau")) name = ""; //ignore electron uncertainties in mutau
  }

  //define which are relevant for a given channel
  if(selection.EndsWith("tau")) { //hadronic tau channels
    if(name.Contains("QCD")) name = ""; //QCD SS --> OS only in emu
  } else { //non-hadronic tau channels
    if(name.Contains("Tau")) name = "";
  }

  //for Z->e+mu, ignore background/hadronic tau uncertainties
  if(selection == "emu") {
    if(name.Contains("QCD")) name = "";
    if(name.Contains("Tau")) name = "";
    if(name.Contains("Emb")) name = "";
  }

  //define which are uncorrelated between years
  year -= 2016; //reduce needed characters
  if     (name.Contains("JetToTauStat")) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("JetToTauAlt" )) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("QCDStat")     ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("QCDAlt")      ) name = Form("%sY%i", name.Data(), year);
  else if(name == "Lumi"               ) name = Form("%sY%i", name.Data(), year);
  else if(name.BeginsWith("XS_LumiUC") ) name = Form("%sY%i", name.Data(), year); //uncorrelated luminosity between years
  else if(name == "EmbedUnfold"        ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauJetID")    ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauMuID" )    ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauEleID")    ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauES")       ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("Trig")        ) name = Form("%sY%i", name.Data(), year); //uncorrelated trigger uncertainties
  else if(name.Contains("JER")         ) name = Form("%sY%i", name.Data(), year); //uncorrelated JER uncertainties
  else if(name.Contains("JES")         ) name = Form("%sY%i", name.Data(), year); //uncorrelated JES uncertainties
  else if(name.Contains("BTag")        ) name = Form("%s-%s", name.Data(), (selection.EndsWith("tau")) ? "had" : "lep"); //FIXME: Decide correlation between years, and tight/loose ID

  return std::pair<TString,TString>(name,type);
}

#endif
