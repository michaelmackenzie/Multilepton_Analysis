#ifndef __EMBEDDINGTNPWEIGHT__
#define __EMBEDDINGTNPWEIGHT__
//Class to handle embedding scale factors from local TnP calculations

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TSystem.h"

//local includes
#include "interface/Utilities.hh"

namespace CLFV {

  class EmbeddingTnPWeight {
  public:
    EmbeddingTnPWeight(const int Mode = 0, const int verbose = 0);

    ~EmbeddingTnPWeight();

    double MuonIDWeight(double pt, double eta, int year, bool qcd = false, int period = 0);
    double MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff, bool qcd = false, int period = 0);
    double ElectronIDWeight(double pt, double eta, int year, bool qcd = false, int period = 0);
    double ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff, bool qcd = false, int period = 0);

  private:
    double GetScale(const TH2F* data, const TH2F* mc, const double pt, const double eta, float& data_eff, float& mc_eff);

  public:
    enum {k2016, k2017, k2018, k2016BF, k2016GH, k2018ABC, k2018D, kLast};
    std::map<int, TH2F*> muonTrigDataEff_;
    std::map<int, TH2F*> muonTrigMCEff_;
    std::map<int, TH2F*> muonQCDTrigDataEff_;
    std::map<int, TH2F*> muonQCDTrigMCEff_;
    std::map<int, TH2F*> muonIDDataEff_;
    std::map<int, TH2F*> muonIDMCEff_;
    std::map<int, TH2F*> muonIsoIDDataEff_;
    std::map<int, TH2F*> muonIsoIDMCEff_;
    std::map<int, TH2F*> muonQCDIsoIDDataEff_;
    std::map<int, TH2F*> muonQCDIsoIDMCEff_;
    std::map<int, TH2F*> electronTrigDataEff_;
    std::map<int, TH2F*> electronTrigMCEff_;
    std::map<int, TH2F*> electronIDDataEff_;
    std::map<int, TH2F*> electronIDMCEff_;
    std::map<int, TH2F*> electronQCDTrigDataEff_;
    std::map<int, TH2F*> electronQCDTrigMCEff_;
    std::map<int, TH2F*> electronQCDIDDataEff_;
    std::map<int, TH2F*> electronQCDIDMCEff_;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    bool interpolate_;
    bool useRunPeriods_;
  };
}
#endif
