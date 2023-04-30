#ifndef __JETTOTAUWEIGHT__
#define __JETTOTAUWEIGHT__
//Class to handle scale factor of anti-iso --> iso for jets faking taus

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TSystem.h"


namespace CLFV {
  class JetToTauWeight {
  public:
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mode = 10,000,000 * (bias mode) + 1,000,000 * (don't use pT corrections) + 100000 * (1*(use tau eta corrections) + 2*(use one met dphi))
    //        + 10000 * (use 2D pT vs delta R corrections)
    //        + 1000 * (use DM binned pT corrections) + 100 * (1*(use scale factor fits) + 2*(use fitter errors))
    //        + 10 * (interpolate bins) + (2* use MC Fits + pT region mode)
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    JetToTauWeight(const TString name, const TString selection, TString process, const int set = 8, const int Mode = 0, const int year = 0, const int verbose = 0);
    JetToTauWeight(const TString name, const TString selection, const int set = 8, const int Mode = 0, const int verbose = 0) :
      JetToTauWeight(name, selection, "", set, Mode, verbose) {}

    ~JetToTauWeight();

    //list of up/down uncertainties
    float GetDataFactor(int DM, int year, float pt, float eta,
                        float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                        float* up, float* down, int& nsys,
                        float& pt_wt, float& pt_up, float& pt_down, float& bias);
    //single up/down uncertainties
    float GetDataFactor(int DM, int year, float pt, float eta,
                        float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                        float& up, float& down,
                        float& pt_wt, float& pt_up, float& pt_down, float& bias);
    //no uncertainty or non-closure/bias correction references
    float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso);
    //no uncertainty references
    float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso, float& pt_wt, float& bias);

  private:
    float GetFactor(TH2* h, TF1* func, TH1* hCorrection, TH1* hFitterErrors,
                    std::vector<TF1*> alt_up, std::vector<TF1*> alt_down,
                    float pt, float eta, int DM,
                    float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                    int year,
                    float* up, float* down,
                    float& pt_wt, float& pt_up, float& pt_down, float& bias);

  public:
  // private:
    std::map<int, std::map<int, TH2*>> histsData_;
    std::map<int, std::map<int, TH1*>> corrections_;
    std::map<int, std::map<int, TH2*>> corrections2D_;
    std::map<int, TH1*> etaCorrections_;
    std::map<int, std::map<int, TH1*>> metDPhiCorrections_;
    std::map<int, TH1*> lepMBias_;
    std::map<int, TH1*> mtLepBias_;
    std::map<int, TH1*> oneIsoBias_;
    std::map<int, std::map<int, std::map<int, TF1*>>> funcsData_; //fits to the transfer factors
    std::map<int, std::map<int, std::map<int, TH1*>>> errorsData_; //68% confidence interval around the fit
    std::map<int, std::map<int, std::map<int, std::vector<TF1*>>>> altFuncsUp_; //alternate fit function shapes
    std::map<int, std::map<int, std::map<int, std::vector<TF1*>>>> altFuncsDown_; //alternate fit function shapes
    TString name_;
    int Mode_;
    int verbose_;

    bool useMCFits_;
    bool doInterpolation_;
    bool useFits_;
    bool useFitterErrors_;
    int  doPtCorrections_;
    bool doDMCorrections_;
    bool doMetDMCorrections_;
    int  use2DCorrections_;
    bool useEtaCorrections_;
    bool useMetDPhiCorrections_;
    bool useLepMBias_;
    bool useMTLepBias_;
    bool useOneIsoBias_;

  };
}
#endif
