#ifndef __QCDWEIGHT__
#define __QCDWEIGHT__
//Class to handle scale factor of SS --> OS for QCD events

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

  class QCDWeight {
  public:
    QCDWeight(TString selection, int Mode = 0, int seed = 90, int verbose = 0);
    ~QCDWeight();

    //Get scale factor for Data
    float GetWeight(float deltar, float deltaphi, float oneeta, const int year, float& up, float& down, float& sys);

  public:
    std::map<int, TH1D*> histsData_;
    std::map<int, TH2D*> hists2DData_;
    std::map<int, TF1*> fitsData_;
    std::map<int, TH1D*> fitErrData_;
    std::map<int, TH1D*> histsClosure_;
    std::map<int, TH1D*> histsSys_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    //       year          bin
    std::map<int, std::map<int, bool>> isShiftedUp_; //whether the systematic is shifted up or down
    int verbose_;
    bool useFits_;
    bool useDeltaPhi_;
    bool useEtaClosure_;
    bool use2DScale_;
    bool useDeltaRSys_;
  };
}
#endif
