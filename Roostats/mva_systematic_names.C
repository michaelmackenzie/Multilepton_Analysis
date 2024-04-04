//Get the name of the systematic from its index
#ifndef __SYSNAMES__
#define __SYSNAMES__

#include "mva_defaults.C"

CLFV::Systematics systematics_; //names of the systematics

std::pair<TString,TString> systematic_name(int sys, TString selection, int year, int set = -1) {
  TString name(systematics_.GetName(sys)), type("shape");
  set = set % 100; //ensure it's the base set number

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
  if(name == "2018HEM") {
    if(year != 2018) name = ""; //only defined in 2018
    else             name = "HEM";
  }

  //Embedding detector MET uncertainty
  if(name == "EmbMET") name = "";

  //j-->tau bias systematic configuration check
  const int jtt_bias_mode = 1; //mode XY: X = 1: uncorrelated j-->tau bias in mass regions; Y = 1: use separate rate bias from shape bias
  if(jtt_bias_mode % 10 == 0) { //ignore j-->tau bias rate uncertainty, only use if separating from the shape effect
    if(name.BeginsWith("JetToTauBiasRate")) name = "";
  }
  if((jtt_bias_mode / 10) % 10 == 1 && set > 0) {//make separate mass regions uncorrelated
    if(name.BeginsWith("JetToTauBias0") && !name.BeginsWith("JetToTauBiasRate")) {
      const char* region = (set == 25) ? "MidM" : (set == 26) ? "HighM" : (set == 27) ? "LowM" : (set == 28) ? "ZllM" : "";
      name = Form("%sRgn%s", name.Data(), region);
    }
  }

  // if(name.BeginsWith("JetToTauBias") && !name.Contains("Rate")) name = ""; //turn off j-->tau bias uncertainties

  //Re-name j-->tau systematics to have process names
  if(name.BeginsWith("JetToTauNC") || name.BeginsWith("JetToTauBias")) {
    if(name.EndsWith("0")) name.ReplaceAll("0", "WJets");
    if(name.EndsWith("1")) name.ReplaceAll("1", "Top");
    if(name.EndsWith("2")) name.ReplaceAll("2", "QCD");
  }
  if(name.BeginsWith("JetToTauAlt")) {
    if(name.Contains("P0")) name.ReplaceAll("P0", "WJets");
    if(name.Contains("P1")) name.ReplaceAll("P1", "Top"  );
    if(name.Contains("P2")) name.ReplaceAll("P2", "QCD"  );
  }


  //non-prompt emu estimate check
  // if(name == "QCDMassBDTBias") name = "";

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

  //decide if using theory pdf as total or split
  if(name.BeginsWith("TheoryPDF")) {
    if(name == "TheoryPDF" && split_pdf_sys_) name = "";
    else if(!split_pdf_sys_ && name != "TheoryPDF") name = "";
  }

  //for Z->e+mu, ignore background/hadronic tau uncertainties
  if(selection == "emu") {
    if(name.Contains("QCD")) name = "";
    if(name.Contains("Tau")) name = "";
    if(name.Contains("Emb")) name = "";
  } else {
    if(name == "SignalBDT" ) name = ""; //signal BDT corrections only in the Z->e+mu search
  }

  //FIXME: Remove DM inclusive Tau ES nuisances
  //remove DM inclusive tau ES
  if(name.EndsWith("TauES")) name = "";

  //re-name DM-binned tau ES nuisances
  if     (name.EndsWith("TauES0")) name.ReplaceAll("0", "DM0" );
  else if(name.EndsWith("TauES1")) name.ReplaceAll("1", "DM1" );
  else if(name.EndsWith("TauES2")) name.ReplaceAll("2", "DM10");
  else if(name.EndsWith("TauES3")) name.ReplaceAll("3", "DM11");

  //ignore EWK W/Z and WWW samples
  if(name == "XS_WWW" ) name = "";
  if(name == "XS_EWKZ") name = "";
  if(name == "XS_EWKW") name = "";
  if(name == "XS_toptCh") name = ""; //contributes nothing at 1e-4 norm level
  if(name == "XS_ZZ") name = ""; //contributes nothing at 1e-4 norm level

  //define which are implemented just as rate uncertainties
  if     (name == "SignalMixing"      ) type = "lnN";
  // else if(name == "SignalScale"       ) type = "lnN";
  // else if(name == "SignalPDF"         ) type = "lnN";
  else if(name == "XS_EWKZ"           ) type = "lnN";
  else if(name == "XS_EWKW"           ) type = "lnN";
  else if(name == "XS_WWW"            ) type = "lnN";
  else if(name == "XS_WW"             ) type = "lnN";
  else if(name == "XS_WZ"             ) type = "lnN";
  else if(name == "XS_ZZ"             ) type = "lnN";
  else if(name == "XS_Z"              ) type = "lnN";
  else if(name == "XS_WJets"          ) type = "lnN";
  else if(name == "XS_ttbar"          ) type = "lnN";
  else if(name == "XS_toptw"          ) type = "lnN";
  else if(name == "XS_toptCh"         ) type = "lnN";
  else if(name == "XS_ggFH"           ) type = "lnN";
  else if(name == "XS_VBFH"           ) type = "lnN";
  else if(name == "XS_HWW"            ) type = "lnN";
  else if(name == "XS_HTauTau"        ) type = "lnN";
  else if(name == "Prefire"           ) type = "lnN";
  else if(name == "EleID"             ) type = "lnN";
  else if(name == "EmbEleID"          ) type = "lnN"; //don't use contains to avoid TauEleID
  else if(name.Contains("EleIsoID")   ) type = "lnN";
  else if(name.Contains("EleRecoID")  ) type = "lnN";
  else if(name.Contains("MuonID")     ) type = "lnN";
  else if(name.Contains("MuonIsoID")  ) type = "lnN";
  else if(name.Contains("Lumi")       ) type = "lnN";
  else if(name.Contains("EleTrig")    ) type = "lnN";
  else if(name.Contains("MuonTrig")   ) type = "lnN";
  else if(name.Contains("EmbTauEleID")) type = "lnN"; //Embedded e-->tau negligible as could only be tau tau --> tau_e tau_(e/mu) --> (tau_e --> tau_h) tau_(e/mu)
  else if(name.Contains("EmbTauMuID" )) type = "lnN"; //Embedded mu-->tau negligible as could only be tau tau --> tau_mu tau_(e/mu) --> (tau_mu --> tau_h) tau_(e/mu)
  else if(name == "BTag"              ) type = "lnN"; //c/b b-tagging efficiencies (light b-tag eff left as shape)
  else if(name.BeginsWith("TauJetID") ) type = "lnN"; //MC tau ID (Embed tau ID left as shape)
  // else if(name.BeginsWith("TheoryPDF")) type = "lnN"; //FIXME: Implement as a shape if needed

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
  else if(name == "EmbMET"             ) name = Form("%sY%i", name.Data(), year);
  else if(name.Contains("BTag")        ) name = Form("%s-%s", name.Data(), (selection.EndsWith("tau")) ? "had" : "lep"); //FIXME: Decide correlation between years, and tight/loose ID
  else if(name == "SignalMixing"       ) name = Form("%sY%i", name.Data(), year);

  return std::pair<TString,TString>(name,type);
}

#endif
