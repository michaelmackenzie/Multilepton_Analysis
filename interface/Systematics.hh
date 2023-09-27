#ifndef __SYSTEMATICS__
#define __SYSTEMATICS__
//Define and describe implemented systematics

//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TString.h"
//local includes
#include "interface/GlobalConstants.h"
#include "interface/ScaleUncertainty_t.hh"

namespace CLFV {

  class Systematics {
  public:
    Systematics() {
      //initialize the defined systematics information
      for(int isys = 0; isys < kMaxSystematics; ++isys) {
        Data data = getData(isys);
        if(data.name_ == "") continue; //only store defined systematics
        idToData_[isys] = data;
        if(data.up_) nameToData_[data.name_] = data; //map to the up value
      }
      //initialize the defined normalization/scale systematics information
      for(int isys = kMaxSystematics; isys < kMaxSystematics+kMaxScaleSystematics; ++isys) {
        Data data = getScaleData(isys);
        if(data.name_ == "") continue; //only store defined systematics
        idToData_[isys] = data;
        if(data.up_) nameToData_[data.name_] = data; //map to the up value
      }
    }
    ~Systematics() {}

    TString GetName(const int isys) {
      if(idToData_.find(isys) != idToData_.end()) return idToData_[isys].name_;
      return TString("");
    }

    int GetNum(TString name) {
      if(nameToData_.find(name) != nameToData_.end()) return nameToData_[name].num_;
      std::cout << "Systematics::" << __func__ << ": Undefined systematic " << name.Data() << std::endl;
      return -1;
    }

    bool IsUp(const int isys) {
      if(idToData_.find(isys) != idToData_.end()) return idToData_[isys].up_;
      return false;
    }

    ScaleUncertainty_t GetScale(const int isys) {
      if(idToData_.find(isys) != idToData_.end()) return idToData_[isys].scale_;
      return ScaleUncertainty_t();
    }

  private:
    struct Data {
      int     num_;
      TString name_;
      bool    up_;
      //for scale/normalization uncertainties only (e.g. cross section)
      ScaleUncertainty_t scale_;
      Data(int num = -1, TString name = "", bool up = true) : num_(num), name_(name), up_(up) {}
    };

    static Data getData(const int isys) {
      switch(isys) {
      case   0: return Data(isys, "Nominal"       , true );
      case   1: return Data(isys, "EleID"         , true );
      case   2: return Data(isys, "EleID"         , false);
      case   3: return Data(isys, "MuonID"        , true );
      case   4: return Data(isys, "MuonID"        , false);
      case   5: return Data(isys, "EmbEleID"      , true );
      case   6: return Data(isys, "EmbEleID"      , false);
      case   7: return Data(isys, "EleRecoID"     , true );
      case   8: return Data(isys, "EleRecoID"     , false);
      case   9: return Data(isys, "MuonIsoID"     , true );
      case  10: return Data(isys, "MuonIsoID"     , false);
      case  11: return Data(isys, "TauES"         , true );
      case  12: return Data(isys, "TauES"         , false);
      case  13: return Data(isys, "EmbMuonID"     , true );
      case  14: return Data(isys, "EmbMuonID"     , false);
      case  15: return Data(isys, "QCDNC"         , true );
      case  16: return Data(isys, "QCDNC"         , false);
      case  17: return Data(isys, "EmbMuonIsoID"  , true );
      case  18: return Data(isys, "EmbMuonIsoID"  , false);
      case  19: return Data(isys, "EmbEleRecoID"  , true );
      case  20: return Data(isys, "EmbEleRecoID"  , false);
      case  21: return Data(isys, "JetToTauComp"  , true );
      case  22: return Data(isys, "JetToTauComp"  , false);
      // case  23: return Data(isys, "JetToTauBias"  , true );
      // case  24: return Data(isys, "JetToTauBias"  , false);
      // case  25: return Data(isys, "TauJetID"      , true );
      // case  26: return Data(isys, "TauJetID"      , false);
      case  27: return Data(isys, "TauMuID"       , true );
      case  28: return Data(isys, "TauMuID"       , false);
      case  29: return Data(isys, "TauEleID"      , true );
      case  30: return Data(isys, "TauEleID"      , false);
      case  31: return Data(isys, "BTag"          , true );
      case  32: return Data(isys, "BTag"          , false);
      // case  33: return Data(isys, "Lumi"          , true );
      // case  34: return Data(isys, "Lumi"          , false);
      // case  35: return Data(isys, "EmbedUnfold"   , true );
      // case  36: return Data(isys, "EmbedUnfold"   , false);
      case  37: return Data(isys, "EleTrig"       , true );
      case  38: return Data(isys, "EleTrig"       , false);
      case  39: return Data(isys, "MuonTrig"      , true );
      case  40: return Data(isys, "MuonTrig"      , false);
      case  41: return Data(isys, "JER"           , true );
      case  42: return Data(isys, "JER"           , false);
      case  43: return Data(isys, "JES"           , true );
      case  44: return Data(isys, "JES"           , false);
      case  45: return Data(isys, "QCDBias"       , true );
      case  46: return Data(isys, "QCDBias"       , false);
      case  49: return Data(isys, "Prefire"       , true );
      case  50: return Data(isys, "Prefire"       , false);
      case  51: return Data(isys, "Pileup"        , true );
      case  52: return Data(isys, "Pileup"        , false);
      case  53: return Data(isys, "ZPt"           , true );
      case  54: return Data(isys, "ZPt"           , false);
      case  55: return Data(isys, "EleIsoID"     , true );
      case  56: return Data(isys, "EleIsoID"     , false);
      case  57: return Data(isys, "EleES"         , true );
      case  58: return Data(isys, "EleES"         , false);
      case  59: return Data(isys, "MuonES"        , true );
      case  60: return Data(isys, "MuonES"        , false);
      case  61: return Data(isys, "BTagLight"     , true );
      case  62: return Data(isys, "BTagLight"     , false);
      // case  63: return Data(isys, "BTagHeavy"     , true ); //FIXME: Turn on and turn off just "BTag"
      // case  64: return Data(isys, "BTagHeavy"     , false);
      case  65: return Data(isys, "SignalMixing"  , true );
      case  66: return Data(isys, "SignalMixing"  , false);
      case  67: return Data(isys, "SignalBDT"     , true );
      case  68: return Data(isys, "SignalBDT"     , false);

      // case  70: return Data(isys, "EmbTauJetID"   , true );
      // case  71: return Data(isys, "EmbTauJetID"   , false);
      case  72: return Data(isys, "EmbTauMuID"    , true );
      case  73: return Data(isys, "EmbTauMuID"    , false);
      case  74: return Data(isys, "EmbTauEleID"   , true );
      case  75: return Data(isys, "EmbTauEleID"   , false);
      case  76: return Data(isys, "EmbEleTrig"    , true );
      case  77: return Data(isys, "EmbEleTrig"    , false);
      case  78: return Data(isys, "EmbMuonTrig"   , true );
      case  79: return Data(isys, "EmbMuonTrig"   , false);
      case  80: return Data(isys, "EmbEleIsoID"   , true );
      case  81: return Data(isys, "EmbEleIsoID"   , false);
      case  82: return Data(isys, "EmbEleES"      , true ); //low |eta| bin
      case  83: return Data(isys, "EmbEleES"      , false);
      case  84: return Data(isys, "EmbMuonES"     , true );
      case  85: return Data(isys, "EmbMuonES"     , false);
      case  86: return Data(isys, "EmbTauES"      , true );
      case  87: return Data(isys, "EmbTauES"      , false);
      case  88: return Data(isys, "EmbEleES1"     , true ); //high |eta| bin
      case  89: return Data(isys, "EmbEleES1"     , false);

      case  90: return Data(isys, "TheoryPDF"     , true );
      case  91: return Data(isys, "TheoryPDF"     , false);
      case  92: return Data(isys, "TheoryScaleF"  , true );
      case  93: return Data(isys, "TheoryScaleF"  , false);
      case  94: return Data(isys, "TheoryScaleR"  , true );
      case  95: return Data(isys, "TheoryScaleR"  , false);
      default: break;
      }

      int base(100), nbin(0);
      //tau anti-jet ID, uncorrelated over years and 5 pT bins (for now use 5 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 5; //100 - 109
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("TauJetID%i", (isys-base)/2), (isys-base)%2 == 0);
      //tau anti-muon ID, uncorrelated over years and 5 bins / year (15 bins, for now use 5 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 5; //110 - 119
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("TauMuID%i", (isys-base)/2), (isys-base)%2 == 0);
      //tau anti-ele ID, uncorrelated over years and 3 bins / year (9 bins, for now use 3 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 3; //120 - 125
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("TauEleID%i", (isys-base)/2), (isys-base)%2 == 0);
      //j-->tau process/DM binned fit uncertainty
      base += 2*nbin; nbin = 3*4; //126 - 149
      // if(isys >= base && isys < base+2*nbin) return Data(isys, Form("JetToTauStat%i", (isys-base)/2), (isys-base)%2 == 0); //Replaced with Alt function version
      //j-->tau process binned bias uncertainty
      base += 2*nbin; nbin = 3; //150 - 155
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("JetToTauBias%i", (isys-base)/2), (isys-base)%2 == 0);
      //jet binned QCD fit uncertainty
      base += 2*nbin; nbin = 3; //156 - 161
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("QCDStat%i", (isys-base)/2), (isys-base)%2 == 0);
      //j-->tau process binned non-closure uncertainty
      base += 2*nbin; nbin = 3; //162 - 167
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("JetToTauNC%i", (isys-base)/2), (isys-base)%2 == 0);

      //embedding uncorrelated with MC
      //tau anti-jet ID, uncorrelated over years and 5 pT bins (for now use 5 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 5; //168 - 177
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("EmbTauJetID%i", (isys-base)/2), (isys-base)%2 == 0);
      //tau anti-muon ID, uncorrelated over years and 5 bins / year (15 bins, for now use 5 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 5; //178 - 187
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("EmbTauMuID%i", (isys-base)/2), (isys-base)%2 == 0);
      //tau anti-ele ID, uncorrelated over years and 3 bins / year (9 bins, for now use 3 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 3; //188 - 193
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("EmbTauEleID%i", (isys-base)/2), (isys-base)%2 == 0);
      //embedding muon ES, 3 bins / year
      base += 2*nbin; nbin = 3; //194 - 199
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("EmbMuonES%i", (isys-base)/2), (isys-base)%2 == 0);
      //j-->tau statistical function shape variations (3 per fit)
      int nproc(3), ndm(4), nalt(3);
      base += 2*nbin; nbin = nproc*ndm*nalt; //200-271
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("JetToTauAltP%iD%iA%i",
                                                                    (isys-base)/(2*ndm*nalt), //e.g. 200-223 = 1 proc
                                                                    (((isys-base)/(2*nalt))%ndm),//200-205 = 1 DM mode
                                                                    ((isys-base)/2)%nalt), (isys-base)%2 == 0);
      //QCD SS-->OS statistical function shape variations (2 per fit)
      //3 jet bins, 2 alt function shapes
      base += 2*nbin; nbin = 3*2; //272-283
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("QCDAltJ%iA%i",
                                                                    (isys-base)/(2*2), //e.g. 272-277 = 0 jets
                                                                    ((isys-base)/2)%2), //272-273 = up/down of 1 shape param
                                                         (isys-base)%2 == 0);

      //return the default result if no systematic is defined
      return Data();
    }

    //Scale uncertainty data
    static Data getScaleData(const int isys) {
      if(isys < kMaxSystematics) return Data();
      Data data(isys);
      bool up = true;
      //constructor:                                name             scale  process          tag              veto   year  data    mc   embed
      switch(isys-kMaxSystematics) {
      case   0: data.scale_ = ScaleUncertainty_t("XS_Z"          ,   1.0201, ""       , {"DY","ZE","ZMu"},      {},   -1, false,  true, false); up = true ; break; //from cms xs twiki
      case   1: data.scale_ = ScaleUncertainty_t("XS_Z"          ,   0.9799, ""       , {"DY","ZE","ZMu"},      {},   -1, false,  true, false); up = false; break;
      case   2: data.scale_ = ScaleUncertainty_t("XS_WJets"      ,   1.0384, ""       , "Wlnu"           ,      "",   -1, false,  true, false); up = true ; break; //from cms xs twiki
      case   3: data.scale_ = ScaleUncertainty_t("XS_WJets"      ,   0.9622, ""       , "Wlnu"           ,      "",   -1, false,  true, false); up = false; break;
      case   4: data.scale_ = ScaleUncertainty_t("XS_Embed"      ,   1.0400, ""       , ""               ,      "",   -1, false, false,  true); up = true ; break; //from embed twiki
      case   5: data.scale_ = ScaleUncertainty_t("XS_Embed"      ,   0.9600, ""       , ""               ,      "",   -1, false, false,  true); up = false; break;
      case  12: data.scale_ = ScaleUncertainty_t("XS_ttbar"      , 1.+0.060, ""       , "ttbar"          ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  13: data.scale_ = ScaleUncertainty_t("XS_ttbar"      , 1.-0.060, ""       , "ttbar"          ,      "",   -1, false,  true, false); up = false; break;
      case  14: data.scale_ = ScaleUncertainty_t("XS_toptw"      , 1.+0.050, ""       , "ToptW"          ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  15: data.scale_ = ScaleUncertainty_t("XS_toptw"      , 1.-0.050, ""       , "ToptW"          ,      "",   -1, false,  true, false); up = false; break;
      case  16: data.scale_ = ScaleUncertainty_t("XS_toptCh"     , 1.+0.050, ""       , "ToptChannel"    ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  17: data.scale_ = ScaleUncertainty_t("XS_toptCh"     , 1.-0.050, ""       , "ToptChannel"    ,      "",   -1, false,  true, false); up = false; break;
      case  18: data.scale_ = ScaleUncertainty_t("XS_ZZ"         , 1.+0.050, "ZZ"     , ""               ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  19: data.scale_ = ScaleUncertainty_t("XS_ZZ"         , 1.-0.050, "ZZ"     , ""               ,      "",   -1, false,  true, false); up = false; break;
      case  20: data.scale_ = ScaleUncertainty_t("XS_WW"         , 1.+0.050, "WW"     , ""               ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  21: data.scale_ = ScaleUncertainty_t("XS_WW"         , 1.-0.050, "WW"     , ""               ,      "",   -1, false,  true, false); up = false; break;
      case  22: data.scale_ = ScaleUncertainty_t("XS_WZ"         , 1.+0.050, "WZ"     , ""               ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  23: data.scale_ = ScaleUncertainty_t("XS_WZ"         , 1.-0.050, "WZ"     , ""               ,      "",   -1, false,  true, false); up = false; break;
      case  24: data.scale_ = ScaleUncertainty_t("XS_WWW"        , 1.+0.050, "WWW"    , ""               ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  25: data.scale_ = ScaleUncertainty_t("XS_WWW"        , 1.-0.050, "WWW"    , ""               ,      "",   -1, false,  true, false); up = false; break;
      case  26: data.scale_ = ScaleUncertainty_t("XS_EWKZ"       , 1.+0.020,  ""      , "EWKZ"           ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  27: data.scale_ = ScaleUncertainty_t("XS_EWKZ"       , 1.-0.020,  ""      , "EWKZ"           ,      "",   -1, false,  true, false); up = false; break;
      case  28: data.scale_ = ScaleUncertainty_t("XS_EWKW"       , 1.+0.020,  ""      , "EWKW"           ,      "",   -1, false,  true, false); up = true ; break; //from HTauTau AN
      case  29: data.scale_ = ScaleUncertainty_t("XS_EWKW"       , 1.-0.020,  ""      , "EWKW"           ,      "",   -1, false,  true, false); up = false; break;

      case  50: data.scale_ = ScaleUncertainty_t("XS_LumiUC0"    , 1.+0.010, ""       , ""               ,      "", 2016, false,  true, false); up = true ; break; //from lumi twiki
      case  51: data.scale_ = ScaleUncertainty_t("XS_LumiUC0"    , 1.-0.010, ""       , ""               ,      "", 2016, false,  true, false); up = false; break; //UC = uncorrelated comp.
      case  52: data.scale_ = ScaleUncertainty_t("XS_LumiUC1"    , 1.+0.020, ""       , ""               ,      "", 2017, false,  true, false); up = true ; break;
      case  53: data.scale_ = ScaleUncertainty_t("XS_LumiUC1"    , 1.-0.020, ""       , ""               ,      "", 2017, false,  true, false); up = false; break;
      case  54: data.scale_ = ScaleUncertainty_t("XS_LumiUC2"    , 1.+0.015, ""       , ""               ,      "", 2018, false,  true, false); up = true ; break;
      case  55: data.scale_ = ScaleUncertainty_t("XS_LumiUC2"    , 1.-0.015, ""       , ""               ,      "", 2018, false,  true, false); up = false; break;
      case  56: data.scale_ = ScaleUncertainty_t("XS_LumiCA0"    , 1.+0.006, ""       , ""               ,      "", 2016, false,  true, false); up = true ; break; //corrleated pieces
      case  57: data.scale_ = ScaleUncertainty_t("XS_LumiCA0"    , 1.-0.006, ""       , ""               ,      "", 2016, false,  true, false); up = false; break; //CA = Run-II correlated
      case  58: data.scale_ = ScaleUncertainty_t("XS_LumiCA1"    , 1.+0.009, ""       , ""               ,      "", 2017, false,  true, false); up = true ; break;
      case  59: data.scale_ = ScaleUncertainty_t("XS_LumiCA1"    , 1.-0.009, ""       , ""               ,      "", 2017, false,  true, false); up = false; break;
      case  60: data.scale_ = ScaleUncertainty_t("XS_LumiCA2"    , 1.+0.020, ""       , ""               ,      "", 2018, false,  true, false); up = true ; break;
      case  61: data.scale_ = ScaleUncertainty_t("XS_LumiCA2"    , 1.-0.020, ""       , ""               ,      "", 2018, false,  true, false); up = false; break;
      case  62: data.scale_ = ScaleUncertainty_t("XS_LumiCB1"    , 1.+0.006, ""       , ""               ,      "", 2017, false,  true, false); up = true ; break; //CB = 2017/2018
      case  63: data.scale_ = ScaleUncertainty_t("XS_LumiCB1"    , 1.-0.006, ""       , ""               ,      "", 2017, false,  true, false); up = false; break; //     correlated
      case  64: data.scale_ = ScaleUncertainty_t("XS_LumiCB2"    , 1.+0.002, ""       , ""               ,      "", 2018, false,  true, false); up = true ; break;
      case  65: data.scale_ = ScaleUncertainty_t("XS_LumiCB2"    , 1.-0.002, ""       , ""               ,      "", 2018, false,  true, false); up = false; break;

      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC0"    , 1.+0.012, ""       , ""               ,      "", 2016, false,  true, false); up = true ; break; //individual year corr
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC0"    , 1.-0.012, ""       , ""               ,      "", 2016, false,  true, false); up = false; break;
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC1"    , 1.+0.023, ""       , ""               ,      "", 2017, false,  true, false); up = true ; break;
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC1"    , 1.-0.023, ""       , ""               ,      "", 2017, false,  true, false); up = false; break;
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC2"    , 1.+0.025, ""       , ""               ,      "", 2018, false,  true, false); up = true ; break;
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC2"    , 1.-0.025, ""       , ""               ,      "", 2018, false,  true, false); up = false; break;
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC"     , 1.+0.016, ""       , ""               ,      "",   -1, false,  true, false); up = true ; break; //Run-II total unc
      // case  : data.scale_ = ScaleUncertainty_t("XS_LumiUC"     , 1.-0.016, ""       , ""               ,      "",   -1, false,  true, false); up = false; break;
      }
      data.name_ = data.scale_.name_;
      data.up_ = up;
      return data;
    }


  private:
    std::map<int, Data> idToData_;
    std::map<TString, Data> nameToData_;
  };
}
#endif
