#ifndef __EMBEDDINGRESOLUTION__
#define __EMBEDDINGRESOLUTION__
//Class to handle Embedding resolution corrections and uncertainties

//c++ includes
#include <iostream>
#include <math.h>
#include <map>
#include <vector>

//ROOT includes
#include "TFile.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TSystem.h"
#include "TString.h"

//local includes

namespace CLFV {
  class EmbeddingResolution {

  public:
    EmbeddingResolution(const int in_year = -1, const int verbose = 0);
    ~EmbeddingResolution();

    float ElectronScale     (          float eta, float gen_pt, int year, float& up, float& down);
    float ElectronResolution(float pt, float eta, float gen_pt, int year, float& up, float& down);
    float MuonScale         (          float eta, float gen_pt, int year, float& up, float& down);
    float MuonResolution    (float pt, float eta, float gen_pt, int year, float& up, float& down);
    float MuonResolutionUnc (float pt, float eta, float gen_pt, int year); //if only using the one-sided uncertainty

    int verbose_;
    std::map<int,std::map<int, TGraphAsymmErrors*>> elec_scale_     ; //F(gen pT) binned by year and |eta|
    std::map<int,std::map<int, TGraphAsymmErrors*>> elec_resolution_;
    std::map<int,std::map<int, TGraphAsymmErrors*>> muon_scale_     ;
    std::map<int,std::map<int, TGraphAsymmErrors*>> muon_resolution_;
  };
}
#endif
