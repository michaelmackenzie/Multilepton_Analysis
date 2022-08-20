#ifndef __EMBEDDINGWEIGHT__
#define __EMBEDDINGWEIGHT__
//Class to handle embedding scale factors

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TSystem.h"
#include "TRandom3.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooWorkspace.h"

namespace CLFV {
  class EmbeddingWeight {
  public:
    EmbeddingWeight(int Mode = 0, int seed = 90, int verbose = 0);
    ~EmbeddingWeight();

    double UnfoldingWeight(double pt_1, double eta_1, double pt_2, double eta_2, double zeta, double zpt, int year);
    double MuonIDWeight(double pt, double eta, int year);
    double MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff);
    double ElectronIDWeight(double pt, double eta, int year);
    double ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff);

  public:
    enum { k2016, k2017, k2018};
    std::vector<TFile*> files_;
    RooWorkspace* ws[k2018+1]; //workspaces for each year, in case they're needed
    RooFormulaVar* trigUnfold[k2018+1]; //selection muon trigger unfolding weight function
    RooFormulaVar* idUnfold[k2018+1]; //selection muon ID unfolding weight function
    RooRealVar* genTauPt[k2018+1][3]; //gen tau pT variables: 0 = 1, 1 = 2, 3 = current
    RooRealVar* genTauEta[k2018+1][3]; //gen tau eta variables
    RooFormulaVar* muonTrig[k2018+1][2]; //muon trigger weight function 0: data eff 1: mc eff
    RooFormulaVar* muonIso[k2018+1]; //muon Iso weight function
    RooFormulaVar* muonID[k2018+1]; //muon ID weight function
    RooRealVar* muonPt[k2018+1]; //reconstructed muon pT
    RooRealVar* muonEta[k2018+1]; //reconstructed muon eta
    RooFormulaVar* electronTrig[k2018+1][2]; //electron trigger weight function 0: data eff 1: mc eff
    RooFormulaVar* electronID[k2018+1]; //electron ID weight function
    RooFormulaVar* electronIso[k2018+1]; //electron iso ID weight function
    RooRealVar* electronPt[k2018+1]; //reconstructed electron pT
    RooRealVar* electronEta[k2018+1]; //reconstructed electron eta
    TH1* zetaFF[k2018+1]; //Z eta fudge-factor for unfolding
    TH2* zetavptFF[k2018+1]; //Z eta vs pT fudge-factor for unfolding
    int verbose_;
    int useFF_; //apply an unfolding fudge-factor
    TRandom3* rnd_; //for generating systematic shifted parameters
  };
}
#endif
