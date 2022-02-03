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


namespace CLFV {
  class ElectronIDWeight {
  public:
    ElectronIDWeight(int Mode = 0, int seed = 90, int verbose = 0) : verbose_(verbose) {
      TFile* f = 0;
      std::vector<TString> file_regions;
      rnd_ = new TRandom3(seed);

      interpolate_ = Mode % 10 == 1;

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
      electronTrigFileNames[k2016]  = fpair("egamma_trigger_eff_2016.root", "EGamma_SF2D");
      electronTrigFileNames[k2017]  = fpair("egamma_trigger_eff_2017.root", "EGamma_SF2D");
      electronTrigFileNames[k2018]  = fpair("egamma_trigger_eff_2018.root", "EGamma_SF2D");
      // electronTrigFileNames[k2016]  = fpair("egammaTriggerEfficiency_2016.root", "EGamma_SF2D");
      // electronTrigFileNames[k2017]  = fpair("egammaTriggerEfficiency_2017.root", "EGamma_SF2D");
      // electronTrigFileNames[k2018]  = fpair("egammaTriggerEfficiency_2018.root", "EGamma_SF2D");

      int groupID  = 0; //for systematic grouping
      int groupReco = 0;
      for(int period = k2016; period < (k2018 + 1); ++period) {
        if(verbose_ > 1) printf("--- %s: Initializing %i scale factors\n", __func__, period+2016);

        //////////////////////////////////////////
        //Electron ID files
        //////////////////////////////////////////

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

        //////////////////////////////////////////
        //Electron Reco ID files
        //////////////////////////////////////////

        f = TFile::Open(("../scale_factors/" + electronRecoFileNames[period].first).Data(), "READ");
        if(f) {
          TH2F* h = (TH2F*) f->Get(electronRecoFileNames[period].second.Data());
          if(!h) {
            printf("!!! %s: Electron Reco ID histogram not found for %i!\n", __func__, period+2016);
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

        //////////////////////////////////////////
        //Electron Trigger files
        //////////////////////////////////////////

        f = TFile::Open(("../scale_factors/" + electronTrigFileNames[period].first).Data(), "READ");
        if(f) {
          TH2F* h = (TH2F*) f->Get("EGamma_EffData2D");
          if(!h) {
            printf("!!! %s: Electron Trigger Data Eff histogram not found for %i!\n", __func__, period+2016);
          } else {
            h->SetName(Form("%s_%i", h->GetName(), period+2016));
            histTrigDataEff_[period] = h;
          }
          h = (TH2F*) f->Get("EGamma_EffMC2D");
          if(!h) {
            printf("!!! %s: Electron Trigger MC Eff histogram not found for %i!\n", __func__, period+2016);
          } else {
            h->SetName(Form("%s_%i", h->GetName(), period+2016));
            histTrigMCEff_[period] = h;
          }
          files_.push_back(f);
        }
      }
    }

    ~ElectronIDWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }

    int GetIDGroup(int bin, int year) {
      return groupID_[(year-2016) * kYear + bin];
    }
    int GetRecoGroup(int bin, int year) {
      return groupReco_[(year-2016) * kYear + bin];
    }

    //interpolate only based on the pT value
    double interpolate(TH2F* h, double eta, double pt) {
      int xbin = std::min(h->GetNbinsX(), std::max(h->GetXaxis()->FindBin(eta), 1));
      int nybins = h->GetNbinsY();
      int ybin = std::min(nybins, std::max(h->GetYaxis()->FindBin(pt), 1));
      double val = h->GetBinContent(xbin, ybin);
      double cen = h->GetYaxis()->GetBinCenter(ybin);
      double val_2, cen_2;
      //interpolate to higher bin if above center but not last bin, or it's the first bin so no lower bin
      if((pt > cen && ybin < nybins) || ybin == 1) {
        val_2 = h->GetBinContent(xbin, ybin+1);
        cen_2 = h->GetYaxis()->GetBinCenter(ybin+1);
      } else {
        val_2 = h->GetBinContent(xbin, ybin-1);
        cen_2 = h->GetYaxis()->GetBinCenter(ybin-1);
      }
      val = val + (pt - cen) * (val_2 - val) / (cen_2 - cen);
      val = std::min(0.9999, std::max(val, 1.e-4)); //ensure a reasonable efficiency
      return val;
    }

    double TriggerEff(double pt, double eta, int year, float& data_eff, float& mc_eff) {
      data_eff = 0.5; //safer default than 0 or 1, as eff and 1-eff are well defined in ratios
      mc_eff = 0.5;
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
        return 1.;
      }
      //can't fire a trigger if below the threshold
      if((year == k2016 && pt < 28.) || (year == k2017 && pt < 33.) || (year == k2018 && pt < 33.)) {
        return 1.;
      }

      if(eta >= 2.49)       eta =  2.49; //maximum eta for corrections
      else if(eta <= -2.49) eta = -2.49; //minimum eta for corrections
      if(pt > 499.)     pt = 499.; //maximum pT for corrections
      else if(pt < 28.) pt =  28.; //minimum pT for corrections

      TH2F* hTrigData = histTrigDataEff_[year];
      TH2F* hTrigMC = histTrigMCEff_[year];
      if(!hTrigData) {
        std::cout << "!!! " << __func__ << ": Undefined Data trigger efficiency histogram for " << year << std::endl;
        return 1.;
      }
      if(!hTrigMC) {
        std::cout << "!!! " << __func__ << ": Undefined MC trigger efficiency histogram for " << year << std::endl;
        return 1.;
      }
      if(interpolate_) {
        data_eff = interpolate(hTrigData, eta, pt);
        mc_eff   = interpolate(hTrigMC  , eta, pt);
      } else {
        data_eff = hTrigData->GetBinContent(hTrigData->GetXaxis()->FindBin(eta), hTrigData->GetYaxis()->FindBin(pt));
        mc_eff   = hTrigMC  ->GetBinContent(hTrigMC  ->GetXaxis()->FindBin(eta), hTrigMC  ->GetYaxis()->FindBin(pt));
      }

      double scale_factor = (mc_eff > 0.) ? data_eff / mc_eff : 1.;
      if(scale_factor <= 0. || verbose_ > 0) {
        if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
        std::cout << "ElectronIDWeight::" << __func__
                  << " year = " << year + 2016
                  << " data_eff = " << data_eff
                  << " mc_eff = " << mc_eff
                  << std::endl;
      }
      return scale_factor;
    }

  public:
    enum { k2016, k2017, k2018};
    enum { kYear = 100000, kBinY = 100};
    std::map<int, TH2F*> histIDData_;
    std::map<int, TH2F*> histRecoData_;
    std::map<int, TH2F*> histTrigMCEff_;
    std::map<int, TH2F*> histTrigDataEff_;
    std::vector<TFile*> files_;
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
    std::map<int, int> groupID_; //correction groups for systematics
    std::map<int, int> groupReco_;
    bool interpolate_;
  };
}
#endif
