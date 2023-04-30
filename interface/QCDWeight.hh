#ifndef __QCDWEIGHT__
#define __QCDWEIGHT__
//Class to handle scale factor of SS --> OS for QCD events

//c++ includes
#include <map>
#include <vector>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TSystem.h"

namespace CLFV {

  class QCDWeight {
  public:
    QCDWeight(const TString selection, const int Mode = 0, const int year = 0, const int verbose = 0);
    ~QCDWeight();

    //Get the transfer factor
    float GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, const int year, int njets, const bool isantiiso,
                    float& nonclosure, float& antiiso, float* up, float* down, int& nsys);

    //single statistical uncertainty
    float GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, const int year, int njets, const bool isantiiso,
                    float& nonclosure, float& antiiso, float& up, float& down, float& sys);

    //no uncertainties/separated corrections
    float GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, const int year, int njets, const bool isantiiso);

  public:
    std::map<int, TH1*> histsData_;
    std::map<int, TH2*> hists2DData_;
    std::map<int, TF1*> fitsData_;
    std::map<int, TH1*> fitErrData_;
    std::map<int, TH1*> histsClosure_;
    std::map<int, TH1*> histsSys_;
    std::map<int, TH1*> jetBinnedHists_;
    std::map<int, TF1*> jetBinnedFits_;
    std::map<int, TH1*> jetBinnedFitErrs_;
    std::map<int, std::vector<TF1*>> altJetFitsUp_; //varying fit uncertainties
    std::map<int, std::vector<TF1*>> altJetFitsDown_; //varying fit uncertainties
    std::map<int, TH2*> Pt2DClosure_;
    std::map<int, TH2*> AntiIsoScale_;

    int verbose_;
    bool useFits_;
    bool useDeltaPhi_;
    bool useEtaClosure_;
    bool use2DPtClosure_;
    bool use2DScale_;
    bool useDeltaRSys_;
    bool useJetBinned_;
    bool useAntiIso_;
  };
}
#endif
