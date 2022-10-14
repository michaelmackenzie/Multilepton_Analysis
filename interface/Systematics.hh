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

  private:
    struct Data {
      int     num_;
      TString name_;
      bool    up_;
      Data(int num = -1, TString name = "", bool up = true) : num_(num), name_(name), up_(up) {}
    };

    static Data getData(const int isys) {
      switch(isys) {
      case   0: return Data(isys, "Nominal"       , true );
      case   1: return Data(isys, "EleID"         , true );
      case   2: return Data(isys, "EleID"         , false);
      case   3: return Data(isys, "MuonID"        , true );
      case   4: return Data(isys, "MuonID"        , false);
      case   5: return Data(isys, "TauID"         , true );
      case   6: return Data(isys, "TauID"         , false);
      case   7: return Data(isys, "EleRecoID"     , true );
      case   8: return Data(isys, "EleRecoID"     , false);
      case   9: return Data(isys, "MuonIsoID"     , true );
      case  10: return Data(isys, "MuonIsoID"     , false);
      case  11: return Data(isys, "TauES"         , true );
      case  12: return Data(isys, "TauES"         , false);
      case  13: return Data(isys, "QCDStat"       , true );
      case  14: return Data(isys, "QCDStat"       , false);
      case  15: return Data(isys, "QCDNC"         , true );
      case  16: return Data(isys, "QCDNC"         , false);
      case  17: return Data(isys, "JetToTauStat"  , true );
      case  18: return Data(isys, "JetToTauStat"  , false);
      case  19: return Data(isys, "JetToTauNC"    , true );
      case  20: return Data(isys, "JetToTauNC"    , false);
      case  21: return Data(isys, "JetToTauComp"  , true );
      case  22: return Data(isys, "JetToTauComp"  , false);
      case  23: return Data(isys, "JetToTauBias"  , true );
      case  24: return Data(isys, "JetToTauBias"  , false);
      case  25: return Data(isys, "TauJetID"      , true );
      case  26: return Data(isys, "TauJetID"      , false);
      case  27: return Data(isys, "TauMuID"       , true );
      case  28: return Data(isys, "TauMuID"       , false);
      case  29: return Data(isys, "TauEleID"      , true );
      case  30: return Data(isys, "TauEleID"      , false);
      case  31: return Data(isys, "BTag"          , true );
      case  32: return Data(isys, "BTag"          , false);
      case  33: return Data(isys, "Lumi"          , true );
      case  34: return Data(isys, "Lumi"          , false);
      case  35: return Data(isys, "EmbedUnfold"   , true );
      case  36: return Data(isys, "EmbedUnfold"   , false);
      case  37: return Data(isys, "MCEleTrig"     , true );
      case  38: return Data(isys, "MCEleTrig"     , false);
      case  39: return Data(isys, "MCMuonTrig"    , true );
      case  40: return Data(isys, "MCMuonTrig"    , false);
      case  41: return Data(isys, "EmbEleTrig"    , true );
      case  42: return Data(isys, "EmbEleTrig"    , false);
      case  43: return Data(isys, "EmbMuonTrig"   , true );
      case  44: return Data(isys, "EmbMuonTrig"   , false);
      case  45: return Data(isys, "METJER"        , true );
      case  46: return Data(isys, "METJER"        , false);
      case  47: return Data(isys, "METJES"        , true );
      case  48: return Data(isys, "METJES"        , false);
      case  49: return Data(isys, "QCDBias"       , true );
      case  50: return Data(isys, "QCDBias"       , false);
      default: break;
      }

      int base(70), nbin(0);
      //tau anti-jet ID, uncorrelated over years and 5 pT bins (for now use 5 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 5; //70 - 79
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("TauJetID%i", (isys-base)/2), (isys-base)%2 == 0);
      //tau anti-muon ID, uncorrelated over years and 5 bins / year (15 bins, for now use 5 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 5; //80 - 89
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("TauMuID%i", (isys-base)/2), (isys-base)%2 == 0);
      //tau anti-ele ID, uncorrelated over years and 3 bins / year (9 bins, for now use 3 bins, assume year dealt with downstream)
      base += 2*nbin; nbin = 3; //90 - 95
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("TauEleID%i", (isys-base)/2), (isys-base)%2 == 0);
      //j-->tau process/DM binned fit uncertainty
      base += 2*nbin; nbin = 3*4; //96 - 119
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("JetToTauStat%i", (isys-base)/2), (isys-base)%2 == 0);
      //j-->tau process binned bias uncertainty
      base += 2*nbin; nbin = 3; //120 - 125
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("JetToTauBias%i", (isys-base)/2), (isys-base)%2 == 0);
      //jet binned QCD fit uncertainty
      base += 2*nbin; nbin = 3; //126 - 131
      if(isys >= base && isys < base+2*nbin) return Data(isys, Form("QCDStat%i", (isys-base)/2), (isys-base)%2 == 0);

      //return the default result if no systematic is defined
      return Data();
    }

  private:
    std::map<int, Data> idToData_;
    std::map<TString, Data> nameToData_;
  };
}
#endif
