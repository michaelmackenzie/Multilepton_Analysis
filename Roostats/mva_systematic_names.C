//Get the name of the systematic from its index
#ifndef __SYSNAMES__
#define __SYSNAMES__

bool useDefaultBr_ = false; //use given branching fraction instead of fixed example
int verbose_ = 1;
CLFV::Systematics systematics_; //names of the systematics

std::pair<TString,TString> systematic_name(int sys, TString selection, int year) {
  year -= 2016; //reduce needed characters
  TString name(systematics.GetName(sys)), type("shape");
  if(name == "") return std::pair<TString, TString>(name, type);

  // //define which are uncorrelated between years
  // if     (name == "JetToTau") name = Form("%s%i", name.Data(), year);
  // else if(name == "Lumi"    ) name = Form("%s%i", name.Data(), year);
  // else if(name == "Lumi"    ) name = Form("%s%i", name.Data(), year);

  if     (sys ==   1 || sys ==   2) name = "EleID";
  else if(sys ==   4 || sys ==   5) name = "MuonID";
  else if(sys ==  10 || sys ==  11) name = Form("JetToTau%i", year); //statistical uncertainty, uncorrelated between years
  else if(sys ==  13 || sys ==  14) name = "ZpT";
  else if(sys ==  16 || sys ==  17) name = "EleRecoID";
  else if(sys ==  19 || sys ==  20) name = "MuonIsoID";
  else if(sys ==  22 || sys ==  23) name = "TauES";
  else if(sys ==  25 || sys ==  26) name = "QCDScale";
  else if(sys ==  28 || sys ==  29) name = "JetToTauNC";
  else if(sys ==  43 || sys ==  44) name = Form("Lumi%i", year);
  // else if(sys ==  43 || sys ==  44) {name = Form("Lumi%i", year); type = "lnN";}
  else if(sys ==  45 || sys ==  46) name = "BTag";
  else if(sys ==  47 || sys ==  48) name = "JetToTauBias";
  else if(sys ==  51 || sys ==  52) name = "MCEleTrig";
  else if(sys ==  53 || sys ==  54) name = "MCMuonTrig";
  else if(selection.Contains("tau") && (sys ==  49 || sys ==  50)) name = Form("EmbedUnfold%i", year); //uncorrelated between years
  else if(selection.Contains("tau") && (sys ==  55 || sys ==  56)) name = "EmbedEleTrig";
  else if(selection.Contains("tau") && (sys ==  57 || sys ==  58)) name = "EmbedMuonTrig";
  else if(selection.Contains("etau")  && (sys ==  37 || sys ==  38)) name = "TauMuID"; //combine all into 1 for non-sensitive cat
  else if(selection.Contains("mutau") && (sys ==  40 || sys ==  41)) name = "TauEleID";  //combine all into 1 for non-sensitive cat
  else if(sys >= 100 && sys < 106) name = Form("TauJetID%i", (sys-100)/2);
  else if(selection.Contains("mutau") && sys >= 110 && sys < 140) name = Form("TauMuID%i", (sys-110)/2);
  else if(selection.Contains("etau")  && sys >= 140 && sys < 158) name = Form("TauEleID%i", (sys-140)/2);
  return std::pair<TString,TString>(name,type);
}

#endif
