#ifndef __JETTOTAUCOMPOSITION__
#define __JETTOTAUCOMPOSITION__
//Class to handle background process composition for jets faking taus

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TSystem.h"

namespace CLFV {
  class JetToTauComposition {
  public:
    JetToTauComposition(TString selection, int set = 35, int Mode = 0, int verbose = 0);
    ~JetToTauComposition();

    void GetComposition(float pt, int DM, float pt_lead, float lead_dphi, int year, float*& compositions, float*& comp_up, float*& comp_down);


  public:
    enum {kWJets, kZJets, kTop, kQCD, kLast};
    //      year           DM         process
    std::map<int, std::map<int, std::map<int, TH1D*>>> histsData_;
    int Mode_;
    int verbose_;
    bool useOnePt_;
    bool useOneDPhi_;
    bool useFits_;
    bool useDM_;
  };
}
#endif
