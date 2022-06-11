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
#include "TRandom3.h"
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
    JetToTauWeight(const TString name, const TString selection, TString process, const int set = 8, const int Mode = 0, const int seed = 90, const int verbose = 0);
    JetToTauWeight(const TString name, const TString selection, const int set = 8, const int Mode = 0, const int seed = 90, const int verbose = 0) :
      JetToTauWeight(name, selection, "", set, Mode, seed, verbose) {}

    ~JetToTauWeight();

    float GetDataFactor(int DM, int year, float pt, float eta,
                        float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                        float& up, float& down, float& sys, int& group,
                        float& pt_wt, float& pt_up, float& pt_down, float& pt_sys, float& bias);
    float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso);
    float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso, float& pt_wt, float& bias);

    int GetGroup(int idm, int year, int ieta, int ipt) {
      return group_[(year - 2016)*kYear + idm*kDM + ieta*kEta + ipt]; //get systematic group
    }

  private:
    float GetFactor(TH2* h, TF1* func, TH1* hCorrection, TH1* hFitterErrors,
                    float pt, float eta, int DM,
                    float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                    int year,
                    float& up, float& down, float& sys, int& group,
                    float& pt_wt, float& pt_up, float& pt_down, float& pt_sys, float& bias);

  private:
    enum { kYear = 10000, kDM = 1000, kEta = 100};
    std::map<int, std::map<int, TH2*>> histsData_;
    std::map<int, std::map<int, TH1*>> corrections_;
    std::map<int, std::map<int, TH2*>> corrections2D_;
    std::map<int, TH1*> etaCorrections_;
    std::map<int, std::map<int, TH1*>> metDPhiCorrections_;
    std::map<int, TH1*> lepMBias_;
    std::map<int, TH1*> mtLepBias_;
    std::map<int, TH1*> oneIsoBias_;
    std::map<int, std::map<int, std::map<int, TF1*>>> funcsData_;
    std::map<int, std::map<int, std::map<int, TH1*>>> errorsData_;
    TString name_;
    int Mode_;
    int verbose_;
    //scale factor versions: 0/1 = 1 pt range, 2 = use MC estimated factors, 3 = 2 pt ranges, 4 = 4 pt ranges
    TRandom3* rnd_; //for generating systematic shifted parameters
    //       year          DM            eta           pt
    std::map<int, std::map<int, std::map<int, std::map<int, bool>>>> isShiftedUp_; //whether the systematic is shifted up or down
    //       year          ptbin
    std::map<int, std::map<int, bool>> isShiftedUpPt_; //whether the pt correction systematic is shifted up or down
    std::map<int, int> group_; //correction groups for systematics
    std::map<int, int> groupPt_; //pT correction groups for systematics
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
