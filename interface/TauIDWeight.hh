#ifndef __TAUIDWEIGHT__
#define __TAUIDWEIGHT__
//Class to handle tau ID scale factors

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TSystem.h"

namespace CLFV {

  class TauIDWeight {
  public:
    TauIDWeight(int Mode = 0, bool isEmbed = false, TString selection = "etau", int verbose = 0);

    ~TauIDWeight();

    //Convert the ID value to the corresponding bit
    int BitFromID(const UChar_t ID) {
      const int id = ID; //work with integer
      const int bit = std::log2(id + 1);
      if(bit < kFailed) return kFailed; //ensure the bounds
      if(bit > kVVTight) return kVVTight;
      return bit;
    }

    double IDWeight(double pt, double eta, int genID, UChar_t antiJet, int year,
                    float& up, float& down, int& bin
                    );
    double EnergyScale(double pt, double eta, int dm, int genID, UChar_t antiJet, int year, float& up, float& down);

  public:
    enum { k2016, k2017, k2018};
    enum { kYear = 100000, kRunSection = 10000, kBinY = 100};
    enum { kFailed, kVVVLoose, kVVLoose, kVLoose, kLoose, kMedium, kTight, kVTight, kVVTight}; //ID categories
    std::vector<TFile*> files_;
    bool isEmbed_;
    TString selection_;
    int verbose_;
    std::map<int, std::map<int, TF1*>> tauJetIDMap    ; //map<year, <ID, scale>>
    std::map<int, std::map<int, TF1*>> tauJetUpIDMap  ;
    std::map<int, std::map<int, TF1*>> tauJetDownIDMap;
    std::map<int, TH1*> tauEleIDMap    ;
    std::map<int, TH1*> tauMuIDMap     ;
    std::map<int, TH1*> tauESLowMap    ; // 34 < pT < 170 GeV/c
    std::map<int, TH1*> tauESHighMap   ; // pT > 170 GeV/c
    std::map<int, TGraphAsymmErrors*> tauFakeESMap;
  };
}
#endif
