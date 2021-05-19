#ifndef __ZPTWEIGHT__
#define __ZPTWEIGHT__
//Class to handle scale factors to correct the DY MC Z pT vs M distribution
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"

class ZPtWeight {
public:
  ZPtWeight(int Mode = 1, int seed = 90) {
    Mode_ = Mode;
    TFile* f = 0;
    TFile* fsys = 0;
    std::vector<int> years = {2016, 2017, 2018};
    rnd_ = new TRandom3(seed);
    for(int year : years) {
      f = TFile::Open(Form("../scale_factors/z_pt_vs_m_scales_mumu_%i.root", year), "READ");
      fsys = TFile::Open(Form("../scale_factors/z_pt_vs_m_scales_ee_%i.root", year), "READ");
      if(!f) {
        f = TFile::Open(Form("scale_factors/z_pt_vs_m_scales_mumu_%i.root", year), "READ");
        if(!f) continue;
      }
      if(!fsys) {
        fsys = TFile::Open(Form("scale_factors/z_pt_vs_m_scales_ee_%i.root", year), "READ");
        // if(!fsys) continue;
      }
      hZPtScales_[year] = (TH2D*) f->Get("hGenRatio");
      hZPtScales_[year]->SetName(Form("%s_mumu_%i", hZPtScales_[year]->GetName(), year));
      hZPtRecoScales_[year] = (TH2D*) f->Get("hRatioNorm");
      hZPtRecoScales_[year]->SetName(Form("%s_mumu_%i", hZPtRecoScales_[year]->GetName(), year));
      files_.push_back(f);
      if(fsys) {
        hZPtSysScales_[year] = (TH2D*) fsys->Get("hGenRatio");
        hZPtSysScales_[year]->SetName(Form("%s_ee_%i", hZPtSysScales_[year]->GetName(), year));
        hZPtSysRecoScales_[year] = (TH2D*) fsys->Get("hRatioNorm");
        hZPtSysRecoScales_[year]->SetName(Form("%s_ee_%i", hZPtSysRecoScales_[year]->GetName(), year));
        files_.push_back(fsys);
      }
      //initialize random systematic uncertainty map
      if(hZPtScales_[year]) {
        isShiftedUp_[year] = {};
        for(int xbin = 1; xbin <= hZPtScales_[year]->GetNbinsX(); ++xbin) {
          isShiftedUp_[year][xbin-1] = {};
          for(int ybin = 1; ybin <= hZPtScales_[year]->GetNbinsY(); ++ybin) {
            isShiftedUp_[year][xbin-1][ybin-1] = 0.5 < rnd_->Uniform();
          }
        }
      }
    }
  }

  ~ZPtWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  float GetWeight(int year, float pt, float mass, bool doReco, float& up, float& down, float& sys) {
    float weight = 1.; up = 1.; down = 1.; sys = 1.;
    TH2D* h = (doReco > 0) ? hZPtRecoScales_[year] : hZPtScales_[year];
    TH2D* hsys = (doReco > 0) ? hZPtSysRecoScales_[year] : hZPtSysScales_[year];
    if(!h) {
      std::cout << "ZPtWeight::" << __func__ << " WARNING! Z pT weights not defined for year = " << year
                << " and doReco = " << doReco << std::endl;
      return weight;
    }
    //ensure the values are within the bounds
    if(pt >= 1000.) pt = 999.;
    if(mass >= 1000.) mass = 999.;
    int binx = h->GetXaxis()->FindBin(mass);
    int biny = h->GetYaxis()->FindBin(pt);
    weight = h->GetBinContent(binx, biny);
    double sys_weight = (hsys) ? hsys->GetBinContent(binx, biny) : weight;
    up     = (Mode_ > 0) ? weight + abs(weight - sys_weight) : weight + h->GetBinError(binx, biny);
    down   = (Mode_ > 0) ? weight - abs(weight - sys_weight) : weight - h->GetBinError(binx, biny);
    down   = std::max(0.f, down);
    sys    = (isShiftedUp_[year][binx-1][biny-1]) ? up : down;
    if(Mode_ > 0) sys = sys_weight;
    if(weight <= 0.) {
      std::cout << "ZPtWeight::" << __func__ << " WARNING! Z pT weight <= 0 = " << weight << " (pt, mass) = ("
                << pt << ", " << mass << ") using doReco = " << doReco << " and year = "<< year << "! Returning 1...\n";
      return 1.;
    }
    return weight;
  }

public:
  std::map<int, TH2D*> hZPtScales_; //scales using generator level quantities
  std::map<int, TH2D*> hZPtRecoScales_; //scales using reconstructed quantities
  std::map<int, TH2D*> hZPtSysScales_; //scales from a different data region
  std::map<int, TH2D*> hZPtSysRecoScales_; //scales from a different data region
  std::vector<TFile*> files_;
  int Mode_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          eta           pt
  std::map<int, std::map<int, std::map<int, bool>>> isShiftedUp_; //whether the systematic is shifted up or down
};
#endif
