#ifndef __ELECTRONIDWEIGHT__
#define __ELECTRONIDWEIGHT__
//Class to handle electron ID scale factors
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"

class ElectronIDWeight {
public:
  ElectronIDWeight(int seed = 90, int verbose = 0) : verbose_(verbose) {
    TFile* f = 0;
    std::vector<TString> file_regions;
    rnd_ = new TRandom3(seed);

    typedef std::pair<TString,TString> fpair;
    std::map<int, fpair> electronIDFileNames;
    electronIDFileNames[k2016]    = fpair("2016LegacyReReco_ElectronMVAwp80.root","EGamma_SF2D");
    electronIDFileNames[k2017]    = fpair("2017_ElectronMVA80.root"              ,"EGamma_SF2D");
    electronIDFileNames[k2018]    = fpair("2018_ElectronMVA80.root"              ,"EGamma_SF2D");
    std::map<int, fpair> electronRecoFileNames;
    electronRecoFileNames[k2016]  = fpair("EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root"           ,"EGamma_SF2D");
    electronRecoFileNames[k2017]  = fpair("egammaEffi.txt_EGM2D_runBCDEF_passingRECO_2017.root" ,"EGamma_SF2D");
    electronRecoFileNames[k2018]  = fpair("egammaEffi.txt_EGM2D_updatedAll_2018.root"           ,"EGamma_SF2D");
    std::map<int, fpair> electronTrigFileNames;
    electronTrigFileNames[k2016]  = fpair("egammaTriggerEfficiency_2016.root"                   ,"EGamma_SF2D");
    electronTrigFileNames[k2017]  = fpair("egammaTriggerEfficiency_2017.root"                   ,"EGamma_SF2D");
    electronTrigFileNames[k2018]  = fpair("egammaTriggerEfficiency_2018.root"                   ,"EGamma_SF2D");

    int groupID  = 0; //for systematic grouping
    int groupReco = 0;
    for(int period = k2016; period < (k2018 + 1); ++period) {
      if(verbose_ > 1) printf("--- %s: Initializing %i scale factors\n", __func__, period+2016);
      f = TFile::Open(("../scale_factors/" + electronIDFileNames[period].first).Data(), "READ");
      if(f) {
        TH2F* h = (TH2F*) f->Get(electronIDFileNames[period].second.Data());
        if(!h) {
          printf("!!! %s: Electron ID histogram not found for %i!\n", __func__, period+2016);
        } else {
          histIDData_[period] = h;
          //determine which axis is pT axis
          bool ptIsX = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()) > 10.;
          int netagroups = 2;
          int nptgroups = 4;
          //loop through the histogram, determining the bin groupings
          for(int binx = 1; binx <= h->GetNbinsX(); ++binx) {
            for(int biny = 1; biny <= h->GetNbinsY(); ++biny) {
              int ptgroup(0), etagroup(0);
              double ptmin  = ((ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                               h->GetYaxis()->GetBinLowEdge(biny));
              double etamin = ((!ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                               h->GetYaxis()->GetBinLowEdge(biny));
              // etamin = abs(etamin);
              if(etamin > 1.39 || etamin < -1.39) etagroup = 1;
              if(ptmin > 49.99) ptgroup = 3;
              else if(ptmin > 34.99) ptgroup = 2;
              else if(ptmin > 19.99) ptgroup = 1;
              int totgroup = groupID + (etagroup*nptgroups) + ptgroup;
              groupID_[kYear*(period) + kBinY*biny + binx] = totgroup;
              if(verbose_ > 1)
                printf("--- %s ID bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                       __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupID, totgroup);
            }
          }
          groupID += (netagroups)*(nptgroups);
        }
        files_.push_back(f);
      }
      f = TFile::Open(("../scale_factors/" + electronRecoFileNames[period].first).Data(), "READ");
      if(f) {
        TH2F* h = (TH2F*) f->Get(electronRecoFileNames[period].second.Data());
        if(!h) {
          printf("!!! %s: Electron Reco ID histogram not found for %i!\n", __func__, period/2+2016);
        } else {
          histRecoData_[period] = h;
          //determine which axis is pT axis
          bool ptIsX = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()) > 10.;
          int netagroups = 2;
          int nptgroups = 3;
          //loop through the histogram, determining the bin groupings
          for(int binx = 1; binx <= h->GetNbinsX(); ++binx) {
            for(int biny = 1; biny <= h->GetNbinsY(); ++biny) {
              int ptgroup(0), etagroup(0);
              double ptmin  = ((ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                               h->GetYaxis()->GetBinLowEdge(biny));
              double etamin = ((!ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                               h->GetYaxis()->GetBinLowEdge(biny));
              // etamin = abs(etamin);
              if(etamin > 1.39 || etamin < -1.39) etagroup = 1;
              // if(ptmin > 49.99) ptgroup = 3;
              if(ptmin > 44.99) ptgroup = 2;
              else if(ptmin > 19.99) ptgroup = 1;
              int totgroup = groupReco + (etagroup*nptgroups) + ptgroup;
              groupReco_[kYear*(period) + kBinY*biny + binx] = totgroup;
              if(verbose_ > 1)
                printf("--- %s Reco bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                       __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupReco, totgroup);
            }
          }
          groupReco += (netagroups)*(nptgroups);
        }
        files_.push_back(f);
      }
    }
  }

  ~ElectronIDWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  int GetIDGroup(int bin, int year) {
    return groupID_[(year-2016) * kYear + bin];
  }
  int GetRecoGroup(int bin, int year) {
    return groupReco_[(year-2016) * kYear + bin];
  }

public:
  enum { k2016, k2017, k2018};
  enum { kYear = 100000, kBinY = 100};
  std::map<int, TH2F*> histIDData_;
  std::map<int, TH2F*> histRecoData_;
  std::vector<TFile*> files_;
  int verbose_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  std::map<int, int> groupID_; //correction groups for systematics
  std::map<int, int> groupReco_;
};
#endif
