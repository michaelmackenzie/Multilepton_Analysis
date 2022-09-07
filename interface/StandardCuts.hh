#ifndef __STANDARDCUTS__
#define __STANDARDCUTS__
//Class to control standard selection cuts

//c++ includes
#include <iostream>

namespace CLFV {
  class StandardCuts {
  public:
    StandardCuts(int year, bool EmbedCuts = true) : year_(year), embedCuts_(EmbedCuts) {}

    ~StandardCuts() {}

    float GetElectronPt()      {return 15.;}
    float GetElectronEta()     {return (embedCuts_) ? 2.2 : 2.5;}
    float GetMuonPt()          {return 10.;}
    float GetTauEta()          {return (embedCuts_) ? 2.2 : 2.3;}
    float GetTauPt()           {return 20.;}
    float GetMuonEta()         {return (embedCuts_) ? 2.2 : 2.4;}

    float GetElectronTrigger() {return (year_ == 2016) ? 29. : 35.;}
    float GetMuonTrigger()     {return (year_ == 2017) ? 28. : 25.;}

    float GetMET(int /*selection*/)   {return 60.;}
    float GetMTLep(int /*selection*/) {return 70.;}

    int   GetNElectrons(int selection) {
      switch(selection) {
      case kMuTau: return 0;
      case kETau: case kEMu: case kMuTauE: case kETauMu: return 1;
      case kEE: return 2;
      case kMuMu: return -1; //no cut
      default: printf("StandardCuts::%s: Undefined selection %i\n", __func__, selection);
      }
      return -1;
    }

    enum {kMuTau = 1, kETau, kEMu, kMuTauE, kETauMu, kMuMu, kEE};
  private:
    bool embedCuts_;
    int year_;
  };
}
#endif
