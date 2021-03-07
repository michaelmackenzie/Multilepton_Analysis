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
  JetToTauWeight(TString selection, int Mode = 0, int seed = 90) : Mode_(Mode) {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    std::vector<TString> file_regions;
    int max_pt_bins(0), max_eta_bins(0);
    rnd_ = new TRandom3(seed);
    if(Mode % 10 == 3) {
      file_regions.push_back("_ptregion_4"); file_regions.push_back("_ptregion_2");
    } else if(Mode % 10 == 4) {
      file_regions.push_back("_ptregion_1"); file_regions.push_back("_ptregion_2");
      file_regions.push_back("_ptregion_3"); file_regions.push_back("_ptregion_4");
    } else {
      file_regions.push_back("");
    }
    for(int year : years) {
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
              max_pt_bins = std::max(histsData_[ptregion][year][dm]->GetNbinsX(), max_pt_bins);
              max_eta_bins = std::max(histsData_[ptregion][year][dm]->GetNbinsY(), max_eta_bins);
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
      files_.push_back(f);
    }

    //Define shifted systematic weights as up or down
    for(int year : years) {
      isShiftedUp_[year] = {};
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
    }
  }

  ~JetToTauWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  //Get scale factor for Data
  float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float& up, float& down, float& sys) {
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
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }

    // Check if using the binned histogram values
    if(!useFit)
      return GetFactor(h, hCorrection, pt, eta, DM, ptregion, pt_lead, year, up, down, sys);

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

private:
  float GetFactor(TH2D* h, TH1D* hCorrection, float pt, float eta, int DM, int ptregion, float pt_lead, int year, float& up, float& down, float& sys) {
    //ensure within kinematic regions
    eta = fabs(eta);
    if(pt > 199.) pt = 199.;
    else if(pt < 20.) pt = 20.;
    if(eta > 2.3) eta = 2.29;

    bool doInterpolation = (Mode_ % 100) / 10 > 0;

    //get bin value
    int binx = h->GetXaxis()->FindBin(pt);
    int biny = h->GetYaxis()->FindBin(eta);
    float eff_bin = h->GetBinContent(binx, biny);
    float err_bin = h->GetBinError  (binx, biny);
    float eff = -1.;
    float correction = (pt_lead > 0.) ? hCorrection->GetBinContent(std::min(hCorrection->GetNbinsX(), hCorrection->FindBin(pt_lead))) : 1.;
    if(correction < 0.) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! Lead pT correction < 0! Lead pT = " << pt_lead
                << " correction = " << correction
                << " year = " << year << std::endl;
      correction = 1.;
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
    eff = correction * eff / (1. - eff);
    up   = std::max(min_eff, std::min(max_eff, up  )); //don't need to warn about these
    down = std::max(min_eff, std::min(max_eff, down));
    up   = correction * up   / (1. - up  );
    down = correction * down / (1. - down);

    //Systematic shifted weight
    int idm = 0;
    if(DM == 0)
      idm = 0;
    else if(DM == 1)
      idm = 1;
    else if(DM == 10)
      idm = 2;
    else if(DM == 11)
      idm = 3;

    sys = (isShiftedUp_[year][idm][biny-1][binx-1][ptregion]) ? up : down;

    return eff;
  }

public:
  enum { kMaxPtRegions = 10};
  std::map<int, std::map<int, TH2D*>> histsData_[kMaxPtRegions];
  std::map<int, TH1D*> corrections_;
  std::map<int, std::map<int, std::map<int, TF1*>>> funcsData_[kMaxPtRegions];
  std::vector<TFile*> files_;
  int Mode_; // 100*use linear fit + 10*interpolate bins + scale factor version
  //scale factor versions: 0/1 = 1 pt range, 2 = use MC estimated factors, 3 = 2 pt ranges, 4 = 4 pt ranges
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          DM            eta           pt      ptregion
  std::map<int, std::map<int, std::map<int, std::map<int, std::map<int, bool>>>>> isShiftedUp_; //whether the systematic is shifted up or down
};
#endif
