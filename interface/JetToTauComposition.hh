#ifndef __JETTOTAUCOMPOSITION__
#define __JETTOTAUCOMPOSITION__
//Class to handle background process composition for jets faking taus

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {
  class JetToTauComposition {
  public:
    JetToTauComposition(TString selection, int set = 2035, int Mode = 0, int verbose = 0);
    ~JetToTauComposition();

    void GetComposition(float pt, float dphi, float mt, int DM,
                        float pt_lead, float lead_dphi, float lead_mt, float mass,
                        int year, float*& compositions, float*& comp_up, float*& comp_down);


  public:
    enum {kWJets, kZJets, kTop, kQCD, kLast};
    //      year           DM         process
    std::map<int, std::map<int, std::map<int, TH1*>>> histsData_;
    std::map<int, std::map<int, std::map<int, TH2*>>> hists2DData_;
    int Mode_;
    int verbose_;
    bool useOnePt_;
    bool useTwoPt_;
    bool useOneDPhi_;
    bool useTwoDPhi_;
    bool useMTOne_;
    bool useMTTwo_;
    bool use2DMvsMTOne_;
    bool useDM_;
    bool useRun2_;
  };
}
#endif
