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

    double MuonIDWeight(double pt, double eta, int year,
                        float& id_wt, float& id_up, float& id_down,
                        float& iso_wt, float& iso_up, float& iso_down,
                        bool qcd = false, int period = 0);
    double MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff, bool qcd = false, int period = 0);
    double ElectronIDWeight(double pt, double eta, int year,
                            float& id_wt, float& id_up, float& id_down,
                            float& iso_wt, float& iso_up, float& iso_down,
                            bool qcd = false, int period = 0
                            );
    double ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff, bool qcd = false, int period = 0);

  private:
    double GetScale(const TH2* data, const TH2* mc, const double pt, const double eta, float& data_eff, float& mc_eff, float* data_var = nullptr, float* mc_var = nullptr);

  public:
    enum {k2016, k2017, k2018, k2016BF, k2016GH, k2018ABC, k2018D, kLast};
    std::map<int, TH2*> muonTrigDataEff_;
    std::map<int, TH2*> muonTrigMCEff_;
    std::map<int, TH2*> muonQCDTrigDataEff_;
    std::map<int, TH2*> muonQCDTrigMCEff_;
    std::map<int, TH2*> muonIDDataEff_;
    std::map<int, TH2*> muonIDMCEff_;
    std::map<int, TH2*> muonIsoIDDataEff_;
    std::map<int, TH2*> muonIsoIDMCEff_;
    std::map<int, TH2*> muonQCDIsoIDDataEff_;
    std::map<int, TH2*> muonQCDIsoIDMCEff_;
    std::map<int, TH2*> electronTrigDataEff_;
    std::map<int, TH2*> electronTrigMCEff_;
    std::map<int, TH2*> electronIDDataEff_;
    std::map<int, TH2*> electronIDMCEff_;
    std::map<int, TH2*> electronIsoIDDataEff_;
    std::map<int, TH2*> electronIsoIDMCEff_;
    std::map<int, TH2*> electronQCDTrigDataEff_;
    std::map<int, TH2*> electronQCDTrigMCEff_;
    std::map<int, TH2*> electronQCDIsoIDDataEff_;
    std::map<int, TH2*> electronQCDIsoIDMCEff_;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    bool interpolate_;
    int  useRunPeriods_;
  };
}
#endif
