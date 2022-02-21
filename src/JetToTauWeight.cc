#include "interface/JetToTauWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
// Mode = 1000000 * (don't use pT corrections) + 100000 * (1*(use tau eta corrections) + 2*(use one met dphi))
//        + 10000 * (use 2D pT vs delta R corrections)
//        + 1000 * (use DM binned pT corrections) + 100 * (1*(use scale factor fits) + 2*(use fitter errors))
//        + 10 * (interpolate bins) + (2* use MC Fits + pT region mode)
JetToTauWeight::JetToTauWeight(const TString name, const TString selection, const int set, const int Mode, const int seed, const int verbose) : name_(name), Mode_(Mode), verbose_(verbose) {
  TFile* f = 0;
  std::vector<int> years = {2016, 2017, 2018};
  std::vector<TString> file_regions;
  int max_pt_bins(0), max_eta_bins(0), max_corr_bins(0);
  useMCFits_             = ( Mode %       10) == 2;
  doInterpolation_       = ( Mode %      100) /      10 == 1;
  useFits_               = ((Mode %     1000) /     100) % 2 == 1;
  useFitterErrors_       = ((Mode %     1000) /     100) > 1;
  doDMCorrections_       = ((Mode %    10000) /    1000) % 2 == 1;
  doMetDMCorrections_    = ((Mode %    10000) /    1000) > 1;
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
              << " doMetDMCorrections = " << doMetDMCorrections_
              << " use2DCorrections = " << use2DCorrections_
              << " useEtaCorrections = " << useEtaCorrections_
              << " useMetDPhiCorrections = " << useMetDPhiCorrections_
              << " doPtCorrections = " << doPtCorrections_
              << std::endl
              << " scale factor selection = " << selection.Data()
              << " set = " << set << std::endl;
  }

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";

  int group   = 0; //for systematic grouping
  // int groupPt = 0;
  for(int year : years) {
    if(verbose_ > 1) printf("%s: Initializing %i scale factors\n", __func__, year);
    //get the jet --> tau scale factors measured
    for(unsigned ptregion = 0; ptregion < file_regions.size(); ++ptregion) {
      TString file_region = file_regions[ptregion];
      f = TFile::Open(Form("%s/jet_to_tau_%s_%i%s_%i.root", path.Data(), selection.Data(), set, file_region.Data(), year), "READ");
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
            histsData_[ptregion][year][dm]->SetDirectory(0);
            int nptbins  = histsData_[ptregion][year][dm]->GetNbinsX();
            int netabins = histsData_[ptregion][year][dm]->GetNbinsY();
            max_pt_bins  = std::max(nptbins, max_pt_bins);
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
            } else {
              funcsData_[ptregion][year][dm][ieta]->SetName(Form("%s-%s_%s_%i_%i", name_.Data(), funcsData_[ptregion][year][dm][ieta]->GetName(),
                                                                 selection.Data(), year, ptregion));
              funcsData_[ptregion][year][dm][ieta]->AddToGlobalList();
            }
            errorsData_[ptregion][year][dm][ieta] = (TH1D*) f->Get(errname);
            if(!errorsData_[ptregion][year][dm][ieta]) {
              if(useFitterErrors_)
                std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No fit errors found for dm = "
                          << dm << " eta region = " << ieta << " year = " << year << " ptregion = " << ptregion
                          << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
            } else {
              errorsData_[ptregion][year][dm][ieta]->SetName(Form("%s-%s_%s_%i_%i", name_.Data(), errorsData_[ptregion][year][dm][ieta]->GetName(),
                                                                  selection.Data(), year, ptregion));
              errorsData_[ptregion][year][dm][ieta]->SetDirectory(0);
            }
          }
        }
        f->Close();
        delete f;
      }
    }

    ///////////////////////////////////////////////////////////////////
    //get the non-closure corrections based on the leading lepton pT
    ///////////////////////////////////////////////////////////////////

    TString pt_corr_selec = selection;
    if(doPtCorrections_ > 0) { //use hadronic tau selection to correct the pT
      if     (selection == "mumu") pt_corr_selec = "mutau";
      else if(selection == "ee"  ) pt_corr_selec = "etau";
    }
    f = TFile::Open(Form("%s/jet_to_tau_lead_pt_correction_%s_%i_%i.root", path.Data(), pt_corr_selec.Data(), set, year), "READ");
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
          corrections_[dm][year]->SetDirectory(0);
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
          corrections2D_[dm][year]->SetDirectory(0);
        }
      }
      f->Close();
      delete f;
    }

    ///////////////////////////////////////////////////////////////////
    //get the corrections based on the tau eta
    ///////////////////////////////////////////////////////////////////

    TString eta_corr_selec = "mutau";
    if(selection == "ee") eta_corr_selec = "etau";
    int eta_set = set;
    if(eta_set % 100 >= 36 && eta_set % 100 <= 38) eta_set -=6; //use data distributions for non-closure corrections if using MC taus

    f = TFile::Open(Form("%s/jet_to_tau_lead_pt_correction_%s_%i_%i.root", path.Data(), eta_corr_selec.Data(), eta_set, year), "READ");
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
          etaCorrections_[year]->SetDirectory(0);
        }
      }
      if(useMetDPhiCorrections_) {
        metDPhiCorrections_[year][0] = (TH1D*) f->Get("OneMetDeltaPhi");
        if(!metDPhiCorrections_[year][0]) {
          std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No OneMetDeltaPhi correction hist found for year = "
                    << year << " selection = " << eta_corr_selec.Data() << std::endl;
        } else {
          metDPhiCorrections_[year][0] = (TH1D*) metDPhiCorrections_[year][0]->Clone(Form("%s-OneMetDeltaPhi_%i", name_.Data(), year));
          metDPhiCorrections_[year][0]->SetDirectory(0);
        }
        if(doMetDMCorrections_) {
          for(int idm = 0; idm < 4; ++idm) {
            metDPhiCorrections_[year][idm+1] = (TH1D*) f->Get(Form("OneMetDeltaPhi%i", idm));
            if(!metDPhiCorrections_[year][idm+1]) {
              std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No OneMetDeltaPhi" << idm << " correction hist found for year = "
                        << year << " selection = " << eta_corr_selec.Data() << std::endl;
            } else {
              metDPhiCorrections_[year][idm+1] = (TH1D*) metDPhiCorrections_[year][idm+1]->Clone(Form("%s-OneMetDeltaPhi%i_%i", name_.Data(), idm, year));
              metDPhiCorrections_[year][idm+1]->SetDirectory(0);
            }
          }
        }
      }
      f->Close();
      delete f;
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

//-------------------------------------------------------------------------------------------------------------------------
JetToTauWeight::~JetToTauWeight() {
  if(rnd_) delete rnd_;
  for(int ipt = 0; ipt < kMaxPtRegions; ++ipt) {
    for(std::pair<int, std::map<int, TH2D*>> val_1 : histsData_[ipt]) {
      for(std::pair<int, TH2D*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
    }
  }
  for(std::pair<int, std::map<int, TH1D*>> val_1 : corrections_) {
    for(std::pair<int, TH1D*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(std::pair<int, std::map<int, TH2D*>> val_1 : corrections2D_) {
    for(std::pair<int, TH2D*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(std::pair<int, TH1D*> val : etaCorrections_) {if(val.second) delete val.second;}
  for(std::pair<int, std::map<int, TH1D*>> val_1 : metDPhiCorrections_) {
    for(std::pair<int, TH1D*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(int ipt = 0; ipt < kMaxPtRegions; ++ipt) {
    for(std::pair<int, std::map<int, std::map<int, TF1*>>> val_1 : funcsData_[ipt]) {
      for(std::pair<int, std::map<int, TF1*>> val_2 : val_1.second) {
        for(std::pair<int, TF1*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
      }
    }
    for(std::pair<int, std::map<int, std::map<int, TH1D*>>> val_1 : errorsData_[ipt]) {
      for(std::pair<int, std::map<int, TH1D*>> val_2 : val_1.second) {
        for(std::pair<int, TH1D*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
      }
    }
  }
}
