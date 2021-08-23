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

    //Trigger info
    std::map<int, fpair> muonTriggerLowFileNames;
    muonTriggerLowFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"IsoMu24_OR_IsoTkMu24_PtEtaBins");
    muonTriggerLowFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"IsoMu24_OR_IsoTkMu24_PtEtaBins");
    muonTriggerLowFileNames[2*k2017]    = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins");
    muonTriggerLowFileNames[2*k2017+1]  = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins");
    muonTriggerLowFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins");
    muonTriggerLowFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"IsoMu24_PtEtaBins");

    std::map<int, fpair> muonTriggerHighFileNames;
    muonTriggerHighFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"Mu50_OR_TkMu50_PtEtaBins");
    muonTriggerHighFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"Mu50_OR_TkMu50_PtEtaBins");
    muonTriggerHighFileNames[2*k2017]   = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins");
    muonTriggerHighFileNames[2*k2017+1] = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins");
    muonTriggerHighFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins");
    muonTriggerHighFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins");

    int groupID  = 0; //for systematic grouping
    int groupIso = 0;
    for(int period = k2016; period < 2*(k2018 + 1); ++period) {

      ///////////////////////////////////
      // Muon ID
      ///////////////////////////////////

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

      ///////////////////////////////////
      // Muon Iso ID
      ///////////////////////////////////

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
      f = TFile::Open(("../scale_factors/" + muonTriggerLowFileNames[period].first).Data(), "READ");
      if(f) {
        TH2F* h = (TH2F*) f->Get((muonTriggerLowFileNames[period].second + "/efficienciesDATA/abseta_pt_DATA").Data());
        if(!h) {
          printf("!!! %s: Muon low trigger data histogram not found for %i!\n", __func__, period/2+2016);
        } else {
          histTriggerLowData_[period] = h;
        }
        h = (TH2F*) f->Get((muonTriggerLowFileNames[period].second + "/efficienciesMC/abseta_pt_MC").Data());
        if(!h) {
          printf("!!! %s: Muon low trigger MC histogram not found for %i!\n", __func__, period/2+2016);
        } else {
          histTriggerLowMC_[period] = h;
        }
        files_.push_back(f);
      }
      f = TFile::Open(("../scale_factors/" + muonTriggerHighFileNames[period].first).Data(), "READ");
      if(f) {
        TH2F* h = (TH2F*) f->Get((muonTriggerHighFileNames[period].second + "/efficienciesDATA/abseta_pt_DATA").Data());
        if(!h) {
          printf("!!! %s: Muon high trigger data histogram not found for %i!\n", __func__, period/2+2016);
        } else {
          histTriggerHighData_[period] = h;
        }
        h = (TH2F*) f->Get((muonTriggerHighFileNames[period].second + "/efficienciesMC/abseta_pt_MC").Data());
        if(!h) {
          printf("!!! %s: Muon high trigger MC histogram not found for %i!\n", __func__, period/2+2016);
        } else {
          histTriggerHighMC_[period] = h;
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

  double TriggerEff(double pt, double eta, int year, bool isLow, float& data_eff, float& mc_eff) {
    data_eff = 0.5; //safer default than 0 or 1, as eff and 1-eff are well defined in ratios
    mc_eff = 0.5;
    year -= 2016;
    if(year != k2016 && year != k2017 && year != k2018) {
      std::cout << "Warning! Undefined year in MuonIDWeight::" << __func__ << ", returning -1" << std::endl;
      return 1.;
    }
    //can't fire a trigger if below the threshold
    if((year == k2016 && pt < 25.) || (year == k2017 && pt < 28.) || (year == k2018 && pt < 25.)) {
      return 1.;
    }
    if(!isLow && pt < 50.) {
      return 1.;
    }
    eta = fabs(eta);
    if(eta >= 2.39)       eta =  2.39; //maximum eta for corrections
    if(!isLow && pt < 52.) pt = 52.;
    if(pt > 119.) pt = 119.; //maximum pT for corrections

    if(year == k2016) {
      if(pt < 26.) pt = 26.;
    } else if (year == k2017) {
      if(pt < 29.) pt = 29.;
    } else if (year == k2018) {
      if(pt < 26.) pt = 26.;
    }

    double rand = rnd_->Uniform();
    bool firstSection = true; //whether this MC is for the first or second part of the data taking period
    if(year == k2016)
      firstSection = rand <= 19.72/35.86;
    else if(year == k2018)
      firstSection = rand <= 8.98/59.59;

    TH2F* hTrigData = (isLow) ? histTriggerLowData_[2*year + !firstSection] : histTriggerHighData_[2*year + !firstSection];
    TH2F* hTrigMC   = (isLow) ? histTriggerLowMC_  [2*year + !firstSection] : histTriggerHighMC_  [2*year + !firstSection];
    if(!hTrigData) {
      std::cout << "!!! " << __func__ << ": Undefined Data trigger efficiency histogram for " << year
                << " low trigger = " << isLow << " first period = " << firstSection << std::endl;
      return 1.;
    }
    if(!hTrigMC) {
      std::cout << "!!! " << __func__ << ": Undefined MC trigger efficiency histogram for " << year
                << " low trigger = " << isLow << " first period = " << firstSection << std::endl;
      return 1.;
    }
    data_eff = hTrigData->GetBinContent(hTrigData->GetXaxis()->FindBin(eta), hTrigData->GetYaxis()->FindBin(pt));
    mc_eff   = hTrigMC  ->GetBinContent(hTrigMC  ->GetXaxis()->FindBin(eta), hTrigMC  ->GetYaxis()->FindBin(pt));

    double scale_factor = (mc_eff > 0.) ? data_eff / mc_eff : 1.;
    if(std::isnan(scale_factor) || scale_factor <= 0. || verbose_ > 0) {
      if(std::isnan(scale_factor) || scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0 or nan! ";
      std::cout << "MuonIDWeight::" << __func__
                << " year = " << year + 2016
                << " period = " << firstSection
                << " pt = " << pt
                << " eta = " << eta
                << " data_eff = " << data_eff
                << " mc_eff = " << mc_eff
                << " scale = " << scale_factor
                << std::endl;
    }
    return scale_factor;
  }

public:
  enum { k2016, k2017, k2018};
  enum { kYear = 100000, kRunSection = 10000, kBinY = 100};
  std::map<int, TH2F*> histIDData_;
  std::map<int, TH2F*> histIsoData_;
  std::map<int, TH2F*> histTriggerLowData_;
  std::map<int, TH2F*> histTriggerLowMC_;
  std::map<int, TH2F*> histTriggerHighData_;
  std::map<int, TH2F*> histTriggerHighMC_;
  std::vector<TFile*> files_;
  int verbose_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  std::map<int, int> groupID_; //correction groups for systematics
  std::map<int, int> groupIso_;
};
#endif
