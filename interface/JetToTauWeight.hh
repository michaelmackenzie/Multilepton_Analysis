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
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Mode = 1000000 * (don't use pT corrections) + 100000 * (1*(use tau eta corrections) + 2*(use one met dphi))
  //        + 10000 * (use 2D pT vs delta R corrections)
  //        + 1000 * (use DM binned pT corrections) + 100 * (1*(use scale factor fits) + 2*(use fitter errors))
  //        + 10 * (interpolate bins) + (2* use MC Fits + pT region mode)
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  JetToTauWeight(TString name, TString selection, int set = 8, int Mode = 0, int seed = 90, int verbose = 0) : name_(name), Mode_(Mode), verbose_(verbose) {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    std::vector<TString> file_regions;
    int max_pt_bins(0), max_eta_bins(0), max_corr_bins(0);
    useMCFits_             = ( Mode %       10) == 2;
    doInterpolation_       = ( Mode %      100) /      10 == 1;
    useFits_               = ((Mode %     1000) /     100) % 2 == 1;
    useFitterErrors_       = ((Mode %     1000) /     100) > 1;
    doDMCorrections_       = ( Mode %    10000) /    1000 == 1;
    use2DCorrections_      = ( Mode %   100000) /   10000;
    useEtaCorrections_     = ((Mode %  1000000) /  100000) % 2 == 1;
    useMetDPhiCorrections_ = ( Mode %  1000000) /  100000 > 1;
    doPtCorrections_       = ( Mode % 10000000) / 1000000 - 1; //0: don't do -1: nominal selection 1: xtau selection
    rnd_ = new TRandom3(seed);
    if(Mode % 10 == 3) {
      file_regions.push_back("_ptregion_4"); file_regions.push_back("_ptregion_2");
    } else if(Mode % 10 == 4) {
      file_regions.push_back("_ptregion_1"); file_regions.push_back("_ptregion_2");
      file_regions.push_back("_ptregion_3"); file_regions.push_back("_ptregion_4");
    } else {
      file_regions.push_back("");
    }
    if(verbose_ > 0) {
      std::cout << __func__ << ": " << name.Data()
                << "\n Mode = " << Mode
                << " --> doInterpolation = " << doInterpolation_
                << " useFits = " << useFits_
                << " useMCFits = " << useMCFits_
                << " doDMCorrections = " << doDMCorrections_
                << " use2DCorrections = " << use2DCorrections_
                << " useEtaCorrections = " << useEtaCorrections_
                << " useMetDPhiCorrections = " << useMetDPhiCorrections_
                << " doPtCorrections = " << doPtCorrections_
                << std::endl
                << " scale factor selection = " << selection.Data()
                << " set = " << set << std::endl;
    }
    int group   = 0; //for systematic grouping
    // int groupPt = 0;
    for(int year : years) {
      if(verbose_ > 1) printf("%s: Initializing %i scale factors\n", __func__, year);
      //get the jet --> tau scale factors measured
      for(unsigned ptregion = 0; ptregion < file_regions.size(); ++ptregion) {
        TString file_region = file_regions[ptregion];
        f = TFile::Open(Form("../scale_factors/jet_to_tau_%s_%i%s_%i.root", selection.Data(), set, file_region.Data(), year), "READ");
        if(!f)
          f = TFile::Open(Form("scale_factors/jet_to_tau_%s_%i%s_%i.root", selection.Data(), set, file_region.Data(), year), "READ");
        if(f) {
          for(int dm = 0; dm < 4; ++dm) {
            //Get Data histogram
            histsData_[ptregion][year][dm] = (TH2D*) f->Get(Form("h%s_eff_%idm", (useMCFits_) ? "mc" : "data", dm));
            if(!histsData_[ptregion][year][dm]) {
              std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No Data histogram found for dm = "
                        << dm << " year = " << year << " ptregion = " << ptregion
                        << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
            } else {
              histsData_[ptregion][year][dm]->SetName(Form("%s-%s_%s_%i_%i", name_.Data(), histsData_[ptregion][year][dm]->GetName(), selection.Data(), year, ptregion));
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
            //Get Data fits and errors
            for(int ieta = 0; ieta < 2; ++ieta) {
              const char* fname   = (useMCFits_) ? Form("fit_mc_func_%idm_%ieta", dm, ieta) : Form("fit_func_%idm_%ieta", dm, ieta);
              const char* errname = (useMCFits_) ? Form("fit_1s_error_mc_%idm_%ieta", dm, ieta) : Form("fit_1s_error_%idm_%ieta", dm, ieta);
              funcsData_[ptregion][year][dm][ieta] = (TF1*) f->Get(fname);
              if(!funcsData_[ptregion][year][dm][ieta]) {
                if(useFits_)
                  std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No Data fit found for dm = "
                            << dm << " eta region = " << ieta << " year = " << year << " ptregion = " << ptregion
                        << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
              } else
                funcsData_[ptregion][year][dm][ieta]->SetName(Form("%s-%s_%s_%i_%i", name_.Data(), funcsData_[ptregion][year][dm][ieta]->GetName(),
                                                                   selection.Data(), year, ptregion));
              errorsData_[ptregion][year][dm][ieta] = (TH1D*) f->Get(errname);
              if(!errorsData_[ptregion][year][dm][ieta]) {
                if(useFitterErrors_)
                  std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No fit errors found for dm = "
                            << dm << " eta region = " << ieta << " year = " << year << " ptregion = " << ptregion
                            << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
              } else
                errorsData_[ptregion][year][dm][ieta]->SetName(Form("%s-%s_%s_%i_%i", name_.Data(), errorsData_[ptregion][year][dm][ieta]->GetName(),
                                                                    selection.Data(), year, ptregion));
            }
          }
          files_.push_back(f); //to close later
        }
      }
      ///////////////////////////////////////////////////////////////////
      //get the corrections based on the leading lepton pT
      ///////////////////////////////////////////////////////////////////

      TString pt_corr_selec = selection;
      if(doPtCorrections_ > 0) { //use hadronic tau selection to correct the pT
        if     (selection == "mumu") pt_corr_selec = "mutau";
        else if(selection == "ee"  ) pt_corr_selec = "etau";
      }
      f = TFile::Open(Form("../scale_factors/jet_to_tau_lead_pt_correction_%s_%i_%i.root", pt_corr_selec.Data(), set, year), "READ");
      if(!f) {
        f = TFile::Open(Form("scale_factors/jet_to_tau_lead_pt_correction_%s_%i_%i.root", pt_corr_selec.Data(), set, year), "READ");
      }
      if(f) {
        int dmmodes = (doDMCorrections_) ? 4 : 1;
        for(int dm = 0; dm < dmmodes; ++dm) {
          TString name = "PtScale";
          TString name2D = (use2DCorrections_ > 1) ? "Pt2Vs1Scale" : "PtVsRScale";
          if(dmmodes > 1) name   += Form("_DM%i", dm);
          if(dmmodes > 1) name2D += Form("_DM%i", dm);
          corrections_[dm][year] = (TH1D*) f->Get(name.Data());
          if(!corrections_[dm][year]) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No lead pt correction histogram found for year = "
                      << year << " selection = " << pt_corr_selec.Data();
            if(dmmodes > 1) std::cout << " DM = " << dm + (dm > 1)*8;
            std::cout << std::endl;
          } else {
            corrections_[dm][year] = (TH1D*) corrections_[dm][year]->Clone(Form("%s-correction_%s_%i_%i", name_.Data(), pt_corr_selec.Data(), dm, year));
            max_corr_bins = std::max(max_corr_bins, corrections_[dm][year]->GetNbinsX());
          }
          corrections2D_[dm][year] = (TH2D*) f->Get(name2D.Data());
          if(!corrections2D_[dm][year]) {
            if(use2DCorrections_) {
              std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No 2D lead pt correction histogram found for year = "
                        << year << " selection = " << pt_corr_selec.Data();
              if(dmmodes > 1) std::cout << " DM = " << dm + (dm > 1)*8;
              std::cout << std::endl;
            }
          } else {
            corrections2D_[dm][year] = (TH2D*) corrections2D_[dm][year]->Clone(Form("%s-correction2d_%s_%i_%i", name_.Data(), pt_corr_selec.Data(), dm, year));
          }
        }
        files_.push_back(f);
      }

      ///////////////////////////////////////////////////////////////////
      //get the corrections based on the tau eta
      ///////////////////////////////////////////////////////////////////

      TString eta_corr_selec = "mutau";
      if(selection == "ee") eta_corr_selec = "etau";
      int eta_set = set;
      if(eta_set % 100 >= 36 && eta_set % 100 <= 38) eta_set -=6; //use data distributions for non-closure corrections if using MC taus

      f = TFile::Open(Form("../scale_factors/jet_to_tau_lead_pt_correction_%s_%i_%i.root", eta_corr_selec.Data(), eta_set, year), "READ");
      if(!f) {
        f = TFile::Open(Form("scale_factors/jet_to_tau_lead_pt_correction_%s_%i_%i.root", eta_corr_selec.Data(), eta_set, year), "READ");
      }
      if(!f && (useEtaCorrections_ || useMetDPhiCorrections_)) {
        std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No eta/metDPhi correction file found for year = "
                  << year << " selection = " << eta_corr_selec.Data() << " set = " << eta_set << std::endl;
      } else if(f) {
        if(useEtaCorrections_) {
          etaCorrections_[year] = (TH1D*) f->Get("EtaScale");
          if(!etaCorrections_[year]) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No eta correction hist found for year = "
                      << year << " selection = " << eta_corr_selec.Data() << std::endl;
          } else {
            etaCorrections_[year] = (TH1D*) etaCorrections_[year]->Clone(Form("%s-EtaScale_%i", name_.Data(), year));
          }
        }
        if(useMetDPhiCorrections_) {
          metDPhiCorrections_[year] = (TH1D*) f->Get("OneMetDeltaPhi");
          if(!metDPhiCorrections_[year]) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No OneMetDeltaPhi correction hist found for year = "
                      << year << " selection = " << eta_corr_selec.Data() << std::endl;
          } else {
            metDPhiCorrections_[year] = (TH1D*) metDPhiCorrections_[year]->Clone(Form("%s-OneMetDeltaPhi_%i", name_.Data(), year));
          }
        }
        files_.push_back(f);
      }
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
  float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi,
                      float& up, float& down, float& sys, int& group,
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

    if(verbose_ > 1) {
      std::cout << __func__ << ": " << name_.Data() << " Using DM = " << DM << " (" << idm << ") pt = " << pt << " eta = " << eta
                << " pt_lead = " << pt_lead << " deltar = " << deltar << std::endl;
    }
    //////////////////////////
    // Parse Mode parameter
    //////////////////////////
    int  ptMode = Mode_ % 10;

    // Get the correct histogram
    int ptregion = 0;
    if(ptMode == 3) ptregion = pt_lead > 60.;
    else if(ptMode == 4) ptregion = (pt_lead > 40.) + (pt_lead > 55.) + (pt_lead > 80.);

    h = histsData_[ptregion][year][idm];
    TH1D* hCorrection = corrections_[(doDMCorrections_ ? idm : 0)][year];

    //check if histogram is found
    if(!h) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined histogram for DM = "
                << DM << " year = " << year << " ptregion = " << ptregion << std::endl;
      up = 1.; down = 1.; sys = 1.; group = -1;
      return 1.;
    }
    if(!hCorrection && doPtCorrections_) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined correction histogram for DM = "
                << DM << " year = " << year << " ptregion = " << ptregion << std::endl;
      up = 1.; down = 1.; sys = 1.; group = -1;
      return 1.;
    }
    //get the fit function
    eta = fabs(eta);
    if(eta > 2.29) eta = 2.29;
    TF1* func = funcsData_[ptregion][year][idm][h->GetYaxis()->FindBin(eta)-1];
    //check if function is found
    if(!func && useFits_) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined function for DM = "
                << DM << " eta = " << eta << " year = " << year << " ptregion = "
                << ptregion << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }

    TH1D* hFitterErrors = errorsData_[ptregion][year][idm][h->GetYaxis()->FindBin(eta)-1];
    //check if errors are found
    if(!hFitterErrors && useFitterErrors_) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined fitter errors for DM = "
                << DM << " eta = " << eta << " year = " << year << " ptregion = "
                << ptregion << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }

    return GetFactor(h, func, hCorrection, hFitterErrors, pt, eta, DM, ptregion, pt_lead, deltar, metdphi, year, up, down, sys, group, pt_wt, pt_up, pt_down, pt_sys);
  }

  int GetGroup(int idm, int year, int ieta, int ipt) {
    return group_[(year - 2016)*kYear + idm*kDM + ieta*kEta + ipt]; //get systematic group
  }

  //Get weight without any systematics carried
  float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi) {
    float up, down, sys, pt_wt, pt_up, pt_down, pt_sys;
    int group;
    float weight = GetDataFactor(DM, year, pt, eta, pt_lead, deltar, metdphi, up, down, sys, group, pt_wt, pt_up, pt_down, pt_sys);
    weight *= pt_wt;
    return weight;
  }

  //Get weight without any systematics carried but pT correction also stored
  float GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float& pt_wt) {
    float up, down, sys, pt_up, pt_down, pt_sys;
    int group;
    float weight = GetDataFactor(DM, year, pt, eta, pt_lead, deltar, metdphi, up, down, sys, group, pt_wt, pt_up, pt_down, pt_sys);
    return weight;
  }

private:
  float GetFactor(TH2D* h, TF1* func, TH1D* hCorrection, TH1D* hFitterErrors,
                  float pt, float eta, int DM, int ptregion, float pt_lead, float deltar,
                  float metdphi, int year,
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

    float eff = -1.;
    float corr_error = 0.;

    ///////////////////////////////////////////////////////////
    // Get the tight / (loose + tight) efficiency
    ///////////////////////////////////////////////////////////

    if(useFits_) {
      eff = func->Eval(pt);
      if(useFitterErrors_) {
        int bin_fitter = std::min(hFitterErrors->FindBin(pt), hFitterErrors->GetNbinsX());
        up   = hFitterErrors->GetBinContent(bin_fitter) + hFitterErrors->GetBinError(bin_fitter);
        down = hFitterErrors->GetBinContent(bin_fitter) - hFitterErrors->GetBinError(bin_fitter);
      } else {
        double* params = new double[10];
        func->GetParameters(params);
        for(int i = 0; i < func->GetNpar(); ++i)
          func->SetParameter(i, func->GetParameter(i)+func->GetParError(i));
        up = func->Eval(pt);
        for(int i = 0; i < func->GetNpar(); ++i)
          func->SetParameter(i, func->GetParameter(i)-func->GetParError(i));
        down = func->Eval(pt);
        func->SetParameters(params);
        delete[] params;
      }
      sys = up; //FIXME: sys set to just up for fits
    } else { //use the binned values
      //get bin value
      int binx = h->GetXaxis()->FindBin(pt);
      int biny = h->GetYaxis()->FindBin(eta);
      group = GetGroup(idm, year, biny, binx); //get systematic group

      float eff_bin = h->GetBinContent(binx, biny);
      float err_bin = h->GetBinError  (binx, biny);
      if(doInterpolation_) {
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
      //Systematic shifted weight
      sys  = (isShiftedUp_[year][idm][biny-1][binx-1][ptregion]) ? up : down;
    }

    ///////////////////////////////////////////////////////////
    // Get closure test correction
    ///////////////////////////////////////////////////////////

    int corr_bin = (hCorrection && pt_lead > 0.) ? std::min(hCorrection->GetNbinsX(), hCorrection->FindBin(pt_lead)) : 1;
    if(pt_lead < 0. || !doPtCorrections_ || useMCFits_) {
      pt_wt = 1.;
    } else if(use2DCorrections_) {
      TH2D* hcorr2D = corrections2D_[(doDMCorrections_) ? idm : 0][year];
      if(!hcorr2D) {
        std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! 2D pT correction histogram not found!"
                  << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                  << " dm = " << DM << " year = " << year
                  << std::endl;
        pt_wt = 1.;
        corr_error = 0.;
      } else {
        int binx_c = std::min(hcorr2D->GetXaxis()->FindBin((use2DCorrections_ > 1) ? pt_lead : pt_lead), hcorr2D->GetNbinsX());
        int biny_c = std::min(hcorr2D->GetYaxis()->FindBin((use2DCorrections_ > 1) ? pt      : deltar ), hcorr2D->GetNbinsY());
        pt_wt = hcorr2D->GetBinContent(binx_c, biny_c);
        corr_error = hcorr2D->GetBinError(binx_c, biny_c);
      }
    } else {
      pt_wt = hCorrection->GetBinContent(corr_bin);
      corr_error = hCorrection->GetBinError(corr_bin);
    }
    if(pt_wt < 0.) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Lead pT correction < 0! Lead pT = " << pt_lead
                << " pt correction weight = " << pt_wt
                << " year = " << year << std::endl;
      pt_wt = 1.;
      corr_error = 0.;
    }
    //Apply tau eta corrections if asked for
    if(useEtaCorrections_) {
      TH1D* heta = etaCorrections_[year];
      if(!heta) {
        std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Eta correction histogram not found!"
                  << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                  << " dm = " << DM << " year = " << year
                  << std::endl;
      } else {
        float eta_wt = heta->GetBinContent(heta->FindBin(eta));
        if(eta_wt <= 0.) {
          std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Eta correction <= 0! Eta = " << eta
                    << " eta correction weight = " << eta_wt
                    << " year = " << year << std::endl;
          eta_wt = 1.;
        }
        pt_wt *= eta_wt;
      }
    }
    //Apply one met delta phi corrections if asked for
    if(useMetDPhiCorrections_) {
      TH1D* hmet = metDPhiCorrections_[year];
      if(!hmet) {
        std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! OneMetDeltaPhi correction histogram not found!"
                  << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead << " metdphi = " << metdphi
                  << " dm = " << DM << " year = " << year
                  << std::endl;
      } else {
        float met_wt = hmet->GetBinContent(hmet->FindBin(metdphi));
        if(met_wt <= 0.) {
          std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! metdphi correction <= 0! metdphi = " << metdphi
                    << " met correction weight = " << met_wt
                    << " year = " << year << std::endl;
          met_wt = 1.;
        }
        pt_wt *= met_wt;
      }
    }

    ///////////////////////////////////////////////////////////
    // Convert efficiency into scale factor
    ///////////////////////////////////////////////////////////

    const static float min_eff = 0.000001;
    const static float max_eff = 0.75     ; //force weight < 0.75 / (0.25) = 3
    //check if allowed value
    if(eff < min_eff) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Eff < " << min_eff << " = " << eff
                << " pt = " << pt << " eta = " << eta
                << " dm = " << DM << " year = " << year << std::endl;
      eff = min_eff;
    } else if(eff > max_eff) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Eff > " << max_eff << " = " << eff
                << " pt = " << pt << " eta = " << eta
                << " dm = " << DM << " year = " << year << std::endl;
      eff = max_eff;
    }
    //write as scale factor instead of efficiency
    //eff_0 = a / (a+b) = 1 / (1 + 1/eff_p) = eff_p / (eff_p + 1)
    // --> eff_p = eff_0 / (1 - eff_0)
    eff = eff / (1.f - eff);
    up   = std::max(min_eff, std::min(max_eff, up  )); //don't need to warn about these
    down = std::max(min_eff, std::min(max_eff, down));
    up   = up   / (1.f - up  );
    down = down / (1.f - down);
    sys  = sys  / (1.f - sys );

    //calculate pt correction uncertainties
    //Set systematic to be the correction size
    //Set up to be in direction of pt weight, down in opposite direction around the pt weight
    float pt_dev = pt_wt - 1.f;
    float sigma = (1 - 2*(pt_dev < 0.f)) * std::sqrt(pt_dev*pt_dev + corr_error*corr_error); //sign it with pt_dev sign
    pt_up   = std::max(0.f, pt_wt + sigma);
    pt_down = std::max(0.f, pt_wt - sigma);
    pt_sys = 1.f; //Set systematic to be without the correction

    return eff;
  }

public:
  enum { kMaxPtRegions = 10, kYear = 10000, kDM = 1000, kEta = 100};
  std::map<int, std::map<int, TH2D*>> histsData_[kMaxPtRegions];
  std::map<int, std::map<int, TH1D*>> corrections_;
  std::map<int, std::map<int, TH2D*>> corrections2D_;
  std::map<int, TH1D*> etaCorrections_;
  std::map<int, TH1D*> metDPhiCorrections_;
  std::map<int, std::map<int, std::map<int, TF1*>>> funcsData_[kMaxPtRegions];
  std::map<int, std::map<int, std::map<int, TH1D*>>> errorsData_[kMaxPtRegions];
  std::vector<TFile*> files_;
  TString name_;
  int Mode_;
  int verbose_;
  //scale factor versions: 0/1 = 1 pt range, 2 = use MC estimated factors, 3 = 2 pt ranges, 4 = 4 pt ranges
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          DM            eta           pt      ptregion
  std::map<int, std::map<int, std::map<int, std::map<int, std::map<int, bool>>>>> isShiftedUp_; //whether the systematic is shifted up or down
  //       year          ptbin
  std::map<int, std::map<int, bool>> isShiftedUpPt_; //whether the pt correction systematic is shifted up or down
  std::map<int, int> group_; //correction groups for systematics
  std::map<int, int> groupPt_; //pT correction groups for systematics
  bool useMCFits_;
  bool doInterpolation_;
  bool useFits_;
  bool useFitterErrors_;
  int  doPtCorrections_;
  bool doDMCorrections_;
  int  use2DCorrections_;
  bool useEtaCorrections_;
  bool useMetDPhiCorrections_;

};
#endif
