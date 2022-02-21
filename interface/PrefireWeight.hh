#ifndef __PREFIREWEIGHT__
#define __PREFIREWEIGHT__
//Class to handle L1 pre-fire weight of MC processes
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TRandom3.h"

namespace CLFV {

  class PrefireWeight {
  public:
    PrefireWeight(int seed = 90) {
      rnd_ = new TRandom3(seed);
      const TString cmssw = gSystem->Getenv("CMSSW_BASE");
      const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
      TFile* f = TFile::Open(Form("%s/L1prefiring_jetpt_2016BtoH.root", path.Data()), "READ"); //measured efficiencies
      if(f) {
        hists_[2016] = (TH2F*) f->Get("L1prefiring_jetpt_2016BtoH");
        if(!hists_[2016]) {
          printf("!!! %s: Pileup weight MC Efficiencies for 2016 not found!\n", __func__);
        } else {
          hists_[2016]->SetDirectory(0);
        }
        f->Close();
        delete f;
      }
      f = TFile::Open(Form("%s/L1prefiring_jetpt_2017BtoF.root", path.Data()), "READ"); //measured efficiencies
      if(f) {
        hists_[2017] = (TH2F*) f->Get("L1prefiring_jetpt_2017BtoF");
        if(!hists_[2017]) {
          printf("!!! %s: Pileup weight MC Efficiencies for 2017 not found!\n", __func__);
        } else {
          hists_[2017]->SetDirectory(0);
        }
        f->Close();
        delete f;
      }
    }

    ~PrefireWeight() {
      if(rnd_) delete rnd_;
      for(std::pair<int, TH2F*> val : hists_) {if(val.second) delete val.second;}
    }

    float GetProbability(const int year, float jetpt, float jeteta) {
      float prob(0.);
      if(jetpt > 499.99) jetpt = 499.99; //maximum pT
      else if(jetpt < 30.) jetpt = 30.;
      TH2F* h = hists_[year];
      const int binx = h->GetYaxis()->FindBin(jetpt);
      const int biny = h->GetXaxis()->FindBin(jeteta);
      prob = h->GetBinContent(binx, biny);
      if(prob < 0. || prob > 1.) {
        std::cout << "!!! PrefireWeight::" << __func__ << ": Warning! Probability not between 0 and 1 = " << prob
                  << " jetpt = " << jetpt << " jeteta = " << jeteta << std::endl;
        prob = 0.; //probability of prefire = 0 --> weight = 1 in default case
      }
      return prob;
    }

    float GetWeight(int year, int njets, float* jetspt, float* jetseta) {
      float weight(1.);
      if(year == 2018) return 1.; //no correction in 2018
      //accepted jets
      for(int jet = 0; jet < njets; ++jet) {
        if(jetspt[jet] < 20.) continue; //below pT threshold
        const float prob = GetProbability(year, jetspt[jet], jetseta[jet]);
        weight *= (1. - prob); //re-weight by product of probabilities to not pre-fire
      }
      return weight;
    }

    std::map<int, TH2F*> hists_; //MC efficiencies
    TRandom3* rnd_; //for generating systematic shifted parameters
  };
}
#endif
