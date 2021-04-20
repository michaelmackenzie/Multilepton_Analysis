#ifndef __JETTOTAUWEIGHT__
#define __JETTOTAUWEIGHT__
//Class to handle scale factor of anti-iso --> iso for jets faking taus
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"

class JetToTauWeight {
public:
  JetToTauWeight(TString selection, int Mode = 0, int seed = 90, int verbose = 0) : Mode_(Mode), verbose_(verbose) {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    std::vector<TString> file_regions;
    int max_pt_bins(0), max_eta_bins(0), max_corr_bins(0);
    rnd_ = new TRandom3(seed);
    if(Mode % 10 == 3) {
      file_regions.push_back("_ptregion_4"); file_regions.push_back("_ptregion_2");
    } else if(Mode % 10 == 4) {
      file_regions.push_back("_ptregion_1"); file_regions.push_back("_ptregion_2");
      file_regions.push_back("_ptregion_3"); file_regions.push_back("_ptregion_4");
    } else {
      file_regions.push_back("");
    }
    int group   = 0; //for systematic grouping
    // int groupPt = 0;
    for(int year : years) {
      if(verbose_ > 1) printf("%s: Initializing %i scale factors\n", __func__, year);
      //get the jet --> tau scale factors measured
      for(unsigned ptregion = 0; ptregion < file_regions.size(); ++ptregion) {
        TString file_region = file_regions[ptregion];
        f = TFile::Open(Form("../scale_factors/jet_to_tau_%s%s_%i.root", selection.Data(), file_region.Data(), year), "READ");
        if(!f)
          f = TFile::Open(Form("scale_factors/jet_to_tau_%s%s_%i.root", selection.Data(), file_region.Data(), year), "READ");
        if(f) {
          for(int dm = 0; dm < 4; ++dm) {
            //Get Data histogram
            histsData_[ptregion][year][dm] = (TH2D*) f->Get(Form("hdata_eff_%idm", dm));
            if(!histsData_[ptregion][year][dm]) {
              std::cout << "JetToTauWeight::JetToTauWeight: Warning! No Data histogram found for dm = "
                        << dm << " year = " << year << " ptregion = " << ptregion
                        << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
            } else {
              histsData_[ptregion][year][dm]->SetName(Form("%s_%s_%i_%i", histsData_[ptregion][year][dm]->GetName(), selection.Data(), year, ptregion));
              int nptbins = histsData_[ptregion][year][dm]->GetNbinsX();
              int netabins = histsData_[ptregion][year][dm]->GetNbinsY();
              max_pt_bins = std::max(nptbins, max_pt_bins);
              max_eta_bins = std::max(netabins, max_eta_bins);
              //split into high and low eta regions
              for(int ietabin = 1; ietabin <= netabins; ++ietabin) {
                //combine pT regions in systematic groups
                int nptgroups = 1;
                for(int iptbin = 1; iptbin <= nptbins; ++iptbin) {
                  int ptgroup = 0;
                  //split pT into high and low pT regions
                  if(nptgroups > 1 && histsData_[ptregion][year][dm]->GetXaxis()->GetBinLowEdge(iptbin) > 39.99) ptgroup = 1;
                  group_[(year-2016)*kYear + dm*kDM + ietabin*kEta + iptbin] = ptgroup + group;
                }
                group += (nptgroups - 1); //add the ptgroup assigned above
                ++group; //each eta bin is a different group
              }
            }
            //Get Data fits
            for(int ieta = 0; ieta < 2; ++ieta) {
              const char* hname = (Mode % 10 == 2) ? Form("fit_mc_func_%idm_%ieta", dm, ieta) : Form("fit_func_%idm_%ieta", dm, ieta);
              funcsData_[ptregion][year][dm][ieta] = (TF1*) f->Get(hname);
              if(!funcsData_[ptregion][year][dm][ieta]) {
                if(Mode >= 100)
                  std::cout << "JetToTauWeight::JetToTauWeight: Warning! No Data fit found for dm = "
                            << dm << " eta region = " << ieta << " year = " << year << " ptregion = " << ptregion
                        << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
              } else
                funcsData_[ptregion][year][dm][ieta]->SetName(Form("%s_%s_%i_%i", funcsData_[ptregion][year][dm][ieta]->GetName(),
                                                                   selection.Data(), year, ptregion));
            }
          }
          files_.push_back(f); //to close later
        }
      }
      //get the corrections based on the leading lepton pT
      f = TFile::Open(Form("../scale_factors/jet_to_tau_lead_pt_correction_%s_%i.root", selection.Data(), year), "READ");
      if(!f) {
        f = TFile::Open(Form("scale_factors/jet_to_tau_lead_pt_correction_%s_%i.root", selection.Data(), year), "READ");
        if(!f) continue;
      }
      corrections_[year] = (TH1D*) f->Get("PtScale")->Clone(Form("correction_%s_%i", selection.Data(), year));
      if(!corrections_[year]) {
        std::cout << "JetToTauWeight::JetToTauWeight: Warning! No lead pt correction histogram found for year = "
                  << year << " selection = " << selection.Data() << std::endl;
      } else {
        max_corr_bins = std::max(max_corr_bins, corrections_[year]->GetNbinsX());
      }
      files_.push_back(f);
    }

    //Define shifted systematic weights as up or down
    for(int year : years) {
      isShiftedUp_[year] = {};
      isShiftedUpPt_[year] = {};
      for(int dm = 0; dm < 4; ++dm) {
        isShiftedUp_[year][dm] = {};
        for(int eta = 0; eta < max_eta_bins; ++eta) {
          isShiftedUp_[year][dm][eta] = {};
          for(int pt = 0; pt < max_pt_bins; ++pt) {
            isShiftedUp_[year][dm][eta][pt] = {};
            for(unsigned ptr = 0; ptr < file_regions.size(); ++ptr) {
              isShiftedUp_[year][dm][eta][pt][ptr] = rnd_->Uniform() > 0.5;
            }
          }
        }
      }
      for(int ibin = 0; ibin < max_corr_bins; ++ibin) {
        isShiftedUpPt_[year][ibin] = rnd_->Uniform() > 0.5;
      }
    }
  }

  ~JetToTauWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  //Get scale factor for Data
  float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float& up, float& down, float& sys, int& group,
                      float& pt_wt, float& pt_up, float& pt_down, float& pt_sys) {
    pt_wt = 1.; pt_up = 1.; pt_down = 1.; pt_sys = 1.;
    TH2D* h = 0;
    //get correct decay mode histogram
    int idm = 0;
    if(DM == 0)
      idm = 0;
    else if(DM == 1)
      idm = 1;
    else if(DM == 10)
      idm = 2;
    else if(DM == 11)
      idm = 3;

    //////////////////////////
    // Parse Mode parameter
    //////////////////////////
    // Mode = 100*use fits + 10*interpolate bins + pt region mode
    bool useFit = Mode_ / 100 > 0;
    int  ptMode = Mode_ % 10;

    // Get the correct histogram
    int ptregion = 0;
    if(ptMode == 3) ptregion = pt_lead > 60.;
    else if(ptMode == 4) ptregion = (pt_lead > 40.) + (pt_lead > 55.) + (pt_lead > 80.);

    h = histsData_[ptregion][year][idm];
    TH1D* hCorrection = corrections_[year];

    //check if histogram is found
    if(!h) {
      std::cout << "JetToTauWeight::" << __func__ << " Undefined histogram for DM = "
                << DM << " year = " << year << " ptregion = " << ptregion << std::endl;
      up = 1.; down = 1.; sys = 1.; group = -1;
      return 1.;
    }

    // Check if using the binned histogram values
    if(!useFit)
      return GetFactor(h, hCorrection, pt, eta, DM, ptregion, pt_lead, year, up, down, sys, group, pt_wt, pt_up, pt_down, pt_sys);

    //use the fit function if not interpolating

    eta = fabs(eta);
    if(eta > 2.29) eta = 2.29;
    TF1* func = funcsData_[ptregion][year][idm][h->GetYaxis()->FindBin(eta)-1];

    //check if function is found
    if(!func) {
      std::cout << "JetToTauWeight::" << __func__ << " Undefined function for DM = "
                << DM << " eta = " << eta << " year = " << year << " ptregion = "
                << ptregion << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }

    float eff = func->Eval(pt);
    //check if allowed value
    if(eff < 0.) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! Eff < 0 = " << eff
                << " pt = " << pt << " eta = " << eta
                << " dm = " << DM << " year = " << year
                << " ptregion = " << ptregion << std::endl;
      eff = 0.000001;
    } else if(eff >= 1.) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! Eff >= 1 = " << eff
                << " pt = " << pt << " eta = " << eta
                << " dm = " << DM << " year = " << year
                << " ptregion = " << ptregion << std::endl;
      eff = 1.;
    } else
      //write as scale factor instead of efficiency
      //eff_0 = a / (a+b) = 1 / (1 + 1/eff_p) = eff_p / (eff_p + 1)
      // --> eff_p = eff_0 / (1 - eff_0)
      eff = eff / (1. - eff);

    //FIXME: propagate function uncertainties to up and down
    up   = eff;
    down = eff;
    sys  = eff;
    return eff;
  }

  int GetGroup(int idm, int year, int ieta, int ipt) {
    return group_[(year - 2016)*kYear + idm*kDM + ieta*kEta + ipt]; //get systematic group
  }

  //Get weight without any systematics carried
  float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead) {
    float up, down, sys, pt_wt, pt_up, pt_down, pt_sys;
    int group;
    float weight = GetDataFactor(DM, year, pt, eta, pt_lead, up, down, sys, group, pt_wt, pt_up, pt_down, pt_sys);
    weight *= pt_wt;
    return weight;
  }

private:
  float GetFactor(TH2D* h, TH1D* hCorrection, float pt, float eta, int DM, int ptregion, float pt_lead, int year,
                  float& up, float& down, float& sys, int& group,
                  float& pt_wt, float& pt_up, float& pt_down, float& pt_sys) {
    //ensure within kinematic regions
    eta = fabs(eta);
    if(pt > 199.) pt = 199.;
    else if(pt < 20.) pt = 20.;
    if(eta > 2.3) eta = 2.29;

    //For grouping
    int idm = 0;
    if(DM == 0)
      idm = 0;
    else if(DM == 1)
      idm = 1;
    else if(DM == 10)
      idm = 2;
    else if(DM == 11)
      idm = 3;

    bool doInterpolation = (Mode_ % 100) / 10 > 0;

    //get bin value
    int binx = h->GetXaxis()->FindBin(pt);
    int biny = h->GetYaxis()->FindBin(eta);
    group = GetGroup(idm, year, biny, binx); //get systematic group

    float eff_bin = h->GetBinContent(binx, biny);
    float err_bin = h->GetBinError  (binx, biny);
    float eff = -1.;
    int corr_bin = (pt_lead > 0.) ? std::min(hCorrection->GetNbinsX(), hCorrection->FindBin(pt_lead)) : 1;
    pt_wt = (pt_lead > 0.) ? hCorrection->GetBinContent(corr_bin) : 1.;
    float corr_error = (pt_lead > 0.) ? hCorrection->GetBinError  (corr_bin) : 0.;
    if(pt_wt < 0.) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! Lead pT correction < 0! Lead pT = " << pt_lead
                << " pt correction weight = " << pt_wt
                << " year = " << year << std::endl;
      pt_wt = 1.;
      corr_error = 0.;
    }
    if(doInterpolation) {
      double pt_bin = h->GetXaxis()->GetBinCenter(binx);
      bool leftofcenter = h->GetXaxis()->GetBinCenter(binx) > pt; //check which side of bin center we are
      //if at boundary, use interpolation from bin inside hist
      if(binx == 1)  leftofcenter = false;
      if(binx == h->GetNbinsX())  leftofcenter = true;
      int binx_off = binx-(2*leftofcenter-1);
      float eff_off = h->GetBinContent(binx_off, biny);
      double pt_off = h->GetXaxis()->GetBinCenter(binx_off);
      //linear interpolation between the bin centers
      eff = eff_bin + (eff_off - eff_bin)*(pt - pt_bin)/(pt_off - pt_bin);
    } else {
      eff  = eff_bin;
      up   = eff + err_bin;
      down = eff - err_bin;
    }
    const static float min_eff = 0.000001;
    const static float max_eff = 0.999999;
    //check if allowed value
    if(eff < min_eff) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! Eff < " << min_eff << " = " << eff
                << " pt = " << pt << " eta = " << eta
                << " dm = " << DM << " year = " << year << std::endl;
      eff = min_eff;
    } else if(eff > max_eff) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! Eff > " << max_eff << " = " << eff
                << " pt = " << pt << " eta = " << eta
                << " dm = " << DM << " year = " << year << std::endl;
      eff = max_eff;
    }
    //write as scale factor instead of efficiency
    //eff_0 = a / (a+b) = 1 / (1 + 1/eff_p) = eff_p / (eff_p + 1)
    // --> eff_p = eff_0 / (1 - eff_0)
    eff = eff / (1. - eff);
    up   = std::max(min_eff, std::min(max_eff, up  )); //don't need to warn about these
    down = std::max(min_eff, std::min(max_eff, down));
    up   = up   / (1. - up  );
    down = down / (1. - down);

    //calculate pt correction uncertainties
    pt_up   = pt_wt + corr_error;
    pt_down = std::max(pt_wt - corr_error, 0.f);

    //Systematic shifted weight
    sys  = (isShiftedUp_[year][idm][biny-1][binx-1][ptregion]) ? up : down;
    pt_sys = (isShiftedUpPt_[year][corr_bin]) ? pt_up : pt_down;

    return eff;
  }

public:
  enum { kMaxPtRegions = 10, kYear = 10000, kDM = 1000, kEta = 100};
  std::map<int, std::map<int, TH2D*>> histsData_[kMaxPtRegions];
  std::map<int, TH1D*> corrections_;
  std::map<int, std::map<int, std::map<int, TF1*>>> funcsData_[kMaxPtRegions];
  std::vector<TFile*> files_;
  int Mode_; // 100*use linear fit + 10*interpolate bins + scale factor version
  int verbose_;
  //scale factor versions: 0/1 = 1 pt range, 2 = use MC estimated factors, 3 = 2 pt ranges, 4 = 4 pt ranges
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          DM            eta           pt      ptregion
  std::map<int, std::map<int, std::map<int, std::map<int, std::map<int, bool>>>>> isShiftedUp_; //whether the systematic is shifted up or down
  //       year          ptbin
  std::map<int, std::map<int, bool>> isShiftedUpPt_; //whether the pt correction systematic is shifted up or down
  std::map<int, int> group_; //correction groups for systematics
  std::map<int, int> groupPt_; //pT correction groups for systematics
};
#endif
