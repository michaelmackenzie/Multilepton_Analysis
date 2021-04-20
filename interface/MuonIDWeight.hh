#ifndef __MUONIDWEIGHT__
#define __MUONIDWEIGHT__
//Class to handle muon ID scale factors
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"

class MuonIDWeight {
public:
  MuonIDWeight(int seed = 90, int verbose = 0) : verbose_(verbose) {
    TFile* f = 0;
    std::vector<TString> file_regions;
    rnd_ = new TRandom3(seed);

    typedef std::pair<TString,TString> fpair;
    std::map<int, fpair> muonIDFileNames;
    std::map<int, fpair> muonIsoFileNames;

    muonIDFileNames[2*k2016]    = fpair("RunBCDEF_SF_ID_muon_2016.root",
                                        "NUM_TightID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2016+1]  = fpair("RunGH_SF_ID_muon_2016.root"   ,
                                        "NUM_TightID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2017]    = fpair("2017_Mu_RunBCDEF_SF_ID.root"  ,
                                        "NUM_TightID_DEN_genTracks_pt_abseta");
    muonIDFileNames[2*k2017+1]  = fpair("2017_Mu_RunBCDEF_SF_ID.root"  ,
                                        "NUM_TightID_DEN_genTracks_pt_abseta");
    muonIDFileNames[2*k2018]    = fpair("RunABCD_SF_ID_muon_2018.root" ,
                                        "NUM_TightID_DEN_TrackerMuons_pt_abseta");
    muonIDFileNames[2*k2018+1]  = fpair("RunABCD_SF_ID_muon_2018.root" ,
                                        "NUM_TightID_DEN_TrackerMuons_pt_abseta");

    muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root",
                                        "NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt");
    muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,
                                        "NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt");
    muonIsoFileNames[2*k2017]   = fpair("2017_Mu_RunBCDEF_SF_ISO.root"  ,
                                        "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
    muonIsoFileNames[2*k2017+1] = fpair("2017_Mu_RunBCDEF_SF_ISO.root"  ,
                                        "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
    muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,
                                        "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
    muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,
                                        "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");

    int groupID  = 0; //for systematic grouping
    int groupIso = 0;
    for(int period = k2016; period < 2*(k2018 + 1); ++period) {
      if(verbose_ > 1) printf("--- %s: Initializing %i scale factors\n", __func__, period/2+2016);
      f = TFile::Open(("../scale_factors/" + muonIDFileNames[period].first).Data(), "READ");
      if(f) {
        TH2F* h = (TH2F*) f->Get(muonIDFileNames[period].second.Data());
        if(!h) {
          printf("!!! %s: Muon ID histogram not found for %i!\n", __func__, period/2+2016);
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
              etamin = abs(etamin);
              if(etamin > 1.15) etagroup = 1;
              if(ptmin > 49.99) ptgroup = 3;
              else if(ptmin > 39.99) ptgroup = 2;
              else if(ptmin > 24.99) ptgroup = 1;
              int totgroup = groupID + (etagroup*nptgroups) + ptgroup;
              groupID_[kYear*(period/2) + kRunSection*(period % 2 == 0) + kBinY*biny + binx] = totgroup;
              if(verbose_ > 1)
                printf("--- %s ID bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                       __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupID, totgroup);
            }
          }
          if(period % 2 == 1)
            groupID += (netagroups)*(nptgroups);
        }
        files_.push_back(f);
      }
      f = TFile::Open(("../scale_factors/" + muonIsoFileNames[period].first).Data(), "READ");
      if(f) {
        TH2F* h = (TH2F*) f->Get(muonIsoFileNames[period].second.Data());
        if(!h) {
          printf("!!! %s: Muon Iso ID histogram not found for %i!\n", __func__, period/2+2016);
        } else {
          histIsoData_[period] = h;
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
              etamin = abs(etamin);
              if(etamin > 1.15) etagroup = 1;
              if(ptmin > 49.99) ptgroup = 3;
              else if(ptmin > 39.99) ptgroup = 2;
              else if(ptmin > 24.99) ptgroup = 1;
              int totgroup = groupIso + (etagroup*nptgroups) + ptgroup;
              groupIso_[kYear*(period/2) + kRunSection*(period % 2 == 0) + kBinY*biny + binx] = totgroup;
              if(verbose_ > 1)
                printf("--- %s Iso bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                       __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupIso, totgroup);
            }
          }
          if(period % 2 == 1)
            groupIso += (netagroups)*(nptgroups);
        }
        files_.push_back(f);
      }
    }
  }

  ~MuonIDWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  int GetIDGroup(int bin, int year) {
    return groupID_[(year-2016) * kYear + bin];
  }
  int GetIsoGroup(int bin, int year) {
    return groupIso_[(year-2016) * kYear + bin];
  }

public:
  enum { k2016, k2017, k2018};
  enum { kYear = 100000, kRunSection = 10000, kBinY = 100};
  std::map<int, TH2F*> histIDData_;
  std::map<int, TH2F*> histIsoData_;
  std::vector<TFile*> files_;
  int verbose_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  std::map<int, int> groupID_; //correction groups for systematics
  std::map<int, int> groupIso_;
};
#endif
