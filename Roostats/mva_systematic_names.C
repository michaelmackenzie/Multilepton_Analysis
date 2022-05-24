//Get the name of the systematic from its index
#ifndef _SYSNAMES_
#define _SYSNAMES_

bool useDefaultBr_ = false; //use given branching fraction instead of fixed example
int verbose_ = 1;

pair<TString,TString> systematic_name(int sys, TString selection) {
  TString name(""), type("shape");
  if     (sys ==   1 || sys ==   2) name = "EleID";
  else if(sys ==   4 || sys ==   5) name = "MuonID";
  else if(sys ==  10 || sys ==  11) name = "JetToTau";
  else if(sys ==  13 || sys ==  14) name = "ZpT";
  else if(sys ==  16 || sys ==  17) name = "EleRecoID";
  else if(sys ==  19 || sys ==  20) name = "MuonIsoID";
  else if(sys ==  22 || sys ==  23) name = "TauES";
  else if(sys ==  25 || sys ==  26) name = "QCDScale";
  else if(sys ==  28 || sys ==  29) name = "JetToTauNC";
  else if(sys ==  43 || sys ==  44) {name = "Lumi"; type = "lnN";}
  else if(sys ==  45 || sys ==  46) name = "BTag";
  else if(sys ==  47 || sys ==  48) name = "JetToTauBias";
  else if(selection.Contains("etau")  && (sys ==  37 || sys ==  38)) name = "TauMuID";
  else if(selection.Contains("mutau") && (sys ==  40 || sys ==  41)) name = "TauEleID";
  else if(sys >= 100 && sys < 106) name = Form("TauJetID%i", (sys-100)/2);
  else if(selection.Contains("mutau") && sys >= 110 && sys < 140) name = Form("TauMuID%i", (sys-110)/2);
  else if(selection.Contains("etau")  && sys >= 140 && sys < 158) name = Form("TauEleID%i", (sys-140)/2);
  return pair<TString,TString>(name,type);
}

#endif
