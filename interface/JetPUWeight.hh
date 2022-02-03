#ifndef __JETPUWEIGHT__
#define __JETPUWEIGHT__
//Class to handle jet pileup ID weight of MC processes
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TRandom3.h"

namespace CLFV {
  class JetPUWeight {
  public:
    JetPUWeight(int seed = 90) {
      std::vector<int> years = {2016, 2017, 2018};
      rnd_ = new TRandom3(seed);
      TFile* feff = TFile::Open("../scale_factors/scalefactorsPUID_81Xtraining.root", "READ"); //given scales
      if(!feff)
        feff = TFile::Open("scale_factors/scalefactorsPUID_81Xtraining.root", "READ");
      if(feff) {
        for(int year : years) {
          efficiencies_[year] = (TH2F*) feff->Get(Form("h2_eff_sf%i_M", year));
          if(!efficiencies_[year]) printf("!!! %s: Jet PU scale factors for year %i not found!\n", __func__, year);
          TFile* f = TFile::Open(Form("../scale_factors/jet_puid_mumu_%i.root", year), "READ"); //measured efficiencies
          if(!f)
            f = TFile::Open(Form("scale_factors/jet_puid_mumu_%i.root", year), "READ");
          if(f) {
            hists_[year] = (TH2D*) f->Get("hRatio");
            if(!hists_[year]) printf("!!! %s: Jet PU MC Efficiencies for year %i not found!\n", __func__, year);
            else {
              hists_[year]->SetName(Form("hRatio_%i", year));
            }
            files_.push_back(f);
          }
        }
      }
      files_.push_back(feff);
    }

    ~JetPUWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }

    float GetMCEff(int year, float jetpt, float jeteta) {
      if(jetpt > 49.99) jetpt = 49.99;
      else if(jetpt < 15.) jetpt = 15.;
      if(fabs(jeteta) > 4.99) jeteta = 4.99*jeteta/fabs(jeteta);
      TH2D* h = hists_[year];
      if(!h) {
        std::cout << "!!! JetPUWeight::" << __func__ << " Undefined histogram for  year = " << year << std::endl;
        return 0.999999;
      }
      int binx = h->GetXaxis()->FindBin(jetpt);
      int biny = h->GetYaxis()->FindBin(jeteta);
      float eff = h->GetBinContent(binx, biny);
      if(eff <= 0.) {
        std::cout << "!!! JetPUWeight::" << __func__ << ": Warning! MC Eff <= 0 = " << eff
                  << " jetpt = " << jetpt << " jeteta = " << jeteta << std::endl;
        eff = 0.000001;
      }
      return eff;
    }

    float GetScaleFactor(int year, float jetpt, float jeteta) {
      float scale_factor(1.);
      if(jetpt > 49.99) jetpt = 49.99; //maximum pT
      else if(jetpt < 15.) jetpt = 15.;
      TH2F* h = efficiencies_[year];
      int binx = h->GetXaxis()->FindBin(jetpt);
      int biny = h->GetYaxis()->FindBin(jeteta);
      scale_factor = h->GetBinContent(binx, biny);
      if(scale_factor <= 0.) {
        std::cout << "!!! JetPUWeight::" << __func__ << ": Warning! scale factor <= 0 = " << scale_factor
                  << " jetpt = " << jetpt << " jeteta = " << jeteta << std::endl;
        scale_factor = 0.000001;
      }
      return scale_factor;
    }

    float GetWeight(int year, int njets, float* jetspt, float* jetseta,
                    int nrejjets, float* jetsrejpt, float* jetsrejeta) {
      float weight(1.), prob_data(1.), prob_mc(1.);
      //accepted jets
      for(int jet = 0; jet < njets; ++jet) {
        if(jetspt[jet] < 20.) continue; //below pT threshold
        if(jetspt[jet] >= 50.) continue; //above PU ID pT threshold
        float p_mc = GetMCEff(year, jetspt[jet], jetseta[jet]);
        float scale_factor = GetScaleFactor(year, jetspt[jet], jetseta[jet]);
        float p_data = std::max(0., std::min(1., (double) p_mc*scale_factor)); //0 <= probability <= 1
        prob_data *= p_data;
        prob_mc   *= p_mc;
      }
      //rejected jets
      for(int jet = 0; jet < nrejjets; ++jet) {
        if(jetsrejpt[jet] < 20.) continue; //below pT threshold
        if(jetsrejpt[jet] >= 50.) continue; //above PU ID pT threshold
        float p_mc = GetMCEff(year, jetsrejpt[jet], jetsrejeta[jet]);
        float scale_factor = GetScaleFactor(year, jetsrejpt[jet], jetsrejeta[jet]);
        float p_data = std::max(0., std::min(1., (double) p_mc*scale_factor)); //0 <= probability <= 1
        prob_data *= 1. - p_data;
        prob_mc   *= 1. - p_mc;
      }
      //get overall scale factor as ratio of probabilities
      if(prob_mc > 0.)
        weight = prob_data / prob_mc;
      else {
        std::cout << "!!! JetPUWeight::" << __func__ << ": weight undefined! Returning 1...\n";
      }
      if(weight <= 0.) {
        std::cout << "!!! JetPUWeight::" << __func__ << ": weight is <= 0 = " << weight << std::endl;
        // weight = 1.;
      }
      return weight;
    }

    std::map<int, TH2F*> efficiencies_;
    std::map<int, TH2D*> hists_; //MC efficiencies
    std::vector<TFile*> files_;
    TRandom3* rnd_; //for generating systematic shifted parameters
  };
}
#endif
