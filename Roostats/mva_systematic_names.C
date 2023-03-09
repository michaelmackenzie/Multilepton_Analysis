//Get the name of the systematic from its index
#ifndef __SYSNAMES__
#define __SYSNAMES__

#include "mva_defaults.C"

CLFV::Systematics systematics_; //names of the systematics

std::pair<TString,TString> systematic_name(int sys, TString selection, int year) {
  year -= 2016; //reduce needed characters
  TString name(systematics_.GetName(sys)), type("shape");

  // if(name == "Pileup") {cout << "FIXME: Removing Pileup\n"; name = "";}

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
  else if(name == "QCDStat"     ) name = "";
  else if(name == "EmbMuonES"   ) { //1 correlated bin
    if(embed_muon_scale_mode_ != 1) name = "";
  } else if(name.BeginsWith("EmbMuonES")) { //3 uncorrelated bins
    if(embed_muon_scale_mode_ != 0) name = "";
  }

  if(use_scale_sys_ && name == "EmbedUnfold" ) name = ""; //implemented in scale uncertainty

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

  //define which are uncorrelated between years
  if     (name.Contains("JetToTauStat")) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("QCDStat")     ) name = Form("%sY%i", name.Data(), year);
  else if(name == "Lumi"               ) name = Form("%sY%i", name.Data(), year);
  else if(name == "EmbedUnfold"        ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauJetID")    ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauMuID" )    ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauEleID")    ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("TauES")       ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("BTag")        ) name = Form("%s-%s", name.Data(), (selection.EndsWith("tau")) ? "had" : "lep"); //FIXME: Decide correlation between years, and tight/loose ID

  return std::pair<TString,TString>(name,type);
}

#endif
