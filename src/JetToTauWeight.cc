#include "interface/JetToTauWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
// Mode = 10,000,000 * (bias mode (0: none; 1,4,5,6(shape only): lepm; 2,5: mtlep; 3,4: oneiso)) + 1,000,000 * (don't use pT corrections)
//        + 100,000 * (1*(use tau eta corrections) + 2*(use one met dphi))
//        + 10,000 * (use 2D pT vs delta R corrections)
//        + 1,000 * (use DM binned pT corrections) + 100 * (1*(use scale factor fits) + 2*(use fitter errors))
//        + 10 * (interpolate bins) + 1 * (use MC Fits)
JetToTauWeight::JetToTauWeight(const TString name, const TString selection, TString process, const int set, const int Mode, const int verbose)
  : name_(name), Mode_(Mode), verbose_(verbose) {

  TFile* f = 0;
  std::vector<int> years = {2016, 2017, 2018};
  useMCFits_             = ( Mode %        10) == 1;
  doInterpolation_       = ( Mode %       100) /       10 == 1;
  useFits_               = ((Mode %      1000) /      100) % 2 == 1;
  useFitterErrors_       = ((Mode %      1000) /      100) > 1;
  doDMCorrections_       = ((Mode %     10000) /     1000) % 2 == 1;
  doMetDMCorrections_    = ((Mode %     10000) /     1000) > 1;
  use2DCorrections_      = ( Mode %    100000) /    10000;
  useEtaCorrections_     = ((Mode %   1000000) /   100000) % 2 == 1;
  useMetDPhiCorrections_ = ( Mode %   1000000) /   100000 > 1;
  doPtCorrections_       = ( Mode %  10000000) /  1000000 - 1; //0: don't do -1: nominal selection 1: xtau selection
  const int bias_mode    = ( Mode % 100000000) / 10000000;
  /*
    Bias modes:
    1: lepm (W+Jets)
    2: mtlep (W+Jets)
    3: isolation (mutau QCD)
    4: isolation + lepm (QCD iso + SS biases)
    5: mtlep + lepm (legacy etau QCD mtlep + SS biases)
    6: lepm, shape only (W+Jets)
  */
  useLepMBias_           = bias_mode == 1 || bias_mode == 4 || bias_mode == 5 || bias_mode == 6; //bias correction in terms of di-lepton mass
  useMTLepBias_          = bias_mode == 2 || bias_mode == 5; //bias correction in terms of MT(ll, MET)
  useOneIsoBias_         = bias_mode == 3 || bias_mode == 4; //bias correction in terms of one iso / pT


  if(verbose_ > 0) {
    std::cout << __func__ << ": " << name.Data() << ", process: " << process.Data()
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
              << " useLepMBias = " << useLepMBias_
              << " useMtLepBias = " << useMTLepBias_
              << " useOneIsoBias = " << useOneIsoBias_
              << std::endl
              << " scale factor selection = " << selection.Data()
              << " set = " << set << std::endl;
  }

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  if(process != "") process += "_";

  for(int year : years) {
    if(verbose_ > 1) printf("%s: Initializing %i scale factors\n", __func__, year);
    //get the jet --> tau scale factors measured
    f = TFile::Open(Form("%s/jet_to_tau_%s_%s%i_%i.root", path.Data(), selection.Data(), process.Data(), set, year), "READ");
    if(f) {
      for(int dm = 0; dm < 4; ++dm) {
        //Get Data histogram
        histsData_[year][dm] = (TH2*) f->Get(Form("h%s_eff_%idm", (useMCFits_) ? "mc" : "data", dm));
        int netabins(0);
        if(!histsData_[year][dm]) {
          std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No Data histogram found for dm = "
                    << dm << " year = " << year
                    << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
        } else {
          histsData_[year][dm]->SetName(Form("%s-%s_%s_%i", name_.Data(), histsData_[year][dm]->GetName(), selection.Data(), year));
          histsData_[year][dm]->SetDirectory(0);
          netabins = histsData_[year][dm]->GetNbinsY();
        }
        //Get Data fits and errors
        for(int ieta = 0; ieta < netabins; ++ieta) {
          const char* fname   = (useMCFits_) ? Form("fit_mc_func_%idm_%ieta", dm, ieta) : Form("fit_func_%idm_%ieta", dm, ieta);
          const char* errname = (useMCFits_) ? Form("fit_1s_error_mc_%idm_%ieta", dm, ieta) : Form("fit_1s_error_%idm_%ieta", dm, ieta);
          funcsData_[year][dm][ieta] = (TF1*) f->Get(fname);
          if(!funcsData_[year][dm][ieta]) {
            if(useFits_)
              std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No Data fit found for dm = "
                        << dm << " eta region = " << ieta << " year = " << year
                        << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
          } else {
            funcsData_[year][dm][ieta]->SetName(Form("%s-%s_%s_%i", name_.Data(), funcsData_[year][dm][ieta]->GetName(),
                                                     selection.Data(), year));
            funcsData_[year][dm][ieta]->AddToGlobalList();
          }

          errorsData_[year][dm][ieta] = (TH1*) f->Get(errname);
          if(!errorsData_[year][dm][ieta]) {
            if(useFitterErrors_)
              std::cout << "JetToTauWeight::JetToTauWeight: " << name.Data() << " Warning! No fit errors found for dm = "
                        << dm << " eta region = " << ieta << " year = " << year
                        << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
          } else {
            errorsData_[year][dm][ieta]->SetName(Form("%s-%s_%s_%i", name_.Data(), errorsData_[year][dm][ieta]->GetName(),
                                                      selection.Data(), year));
            errorsData_[year][dm][ieta]->SetDirectory(0);
          }
        } //end fit function retrieval loop
      } //end year loop
      f->Close();
      delete f;
    }

    ///////////////////////////////////////////////////////////////////
    //get the non-closure corrections based on the leading lepton pT
    ///////////////////////////////////////////////////////////////////

    TString pt_corr_selec = selection;
    if(doPtCorrections_ > 0) { //use hadronic tau selection to correct the pT
      if     (selection == "mumu") pt_corr_selec = "mutau";
      else if(selection == "ee"  ) pt_corr_selec = "etau";
    }

    f = TFile::Open(Form("%s/jet_to_tau_correction_%s_%s%i_%i.root", path.Data(), pt_corr_selec.Data(), process.Data(), set, year), "READ");
    if(f) {
      int dmmodes = (doDMCorrections_) ? 4 : 1;
      for(int dm = 0; dm < dmmodes; ++dm) {
        TString hist = "PtScale";
        TString hist2D = (use2DCorrections_ > 1) ? "Pt2Vs1Scale" : "PtVsRScale";
        if(dmmodes > 1) hist   += Form("_DM%i", dm);
        if(dmmodes > 1) hist2D += Form("_DM%i", dm);
        if(dmmodes == 1 && process.Contains("QCD")) hist += "_QCD"; //QCD uses different binning
        if(verbose_ > 2) std::cout << name_.Data() << ": Retrieving pT correction histogram " << hist.Data() << std::endl;
        corrections_[dm][year] = (TH1*) f->Get(hist.Data());
        if(!corrections_[dm][year]) {
          std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No lead pt correction histogram " << hist.Data() << " found for year = "
                    << year << " selection = " << pt_corr_selec.Data();
          if(dmmodes > 1) std::cout << " DM = " << dm + (dm > 1)*8;
          std::cout << std::endl;
        } else {
          corrections_[dm][year] = (TH1*) corrections_[dm][year]->Clone(Form("%s-correction_%s_%i_%i", name_.Data(), pt_corr_selec.Data(), dm, year));
          corrections_[dm][year]->SetDirectory(0);
        }
        corrections2D_[dm][year] = (TH2*) f->Get(hist2D.Data());
        if(!corrections2D_[dm][year]) {
          if(use2DCorrections_) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No 2D lead pt correction histogram found for year = "
                      << year << " selection = " << pt_corr_selec.Data();
            if(dmmodes > 1) std::cout << " DM = " << dm + (dm > 1)*8;
            std::cout << std::endl;
          }
        } else {
          corrections2D_[dm][year] = (TH2*) corrections2D_[dm][year]->Clone(Form("%s-correction2d_%s_%i_%i", name_.Data(), pt_corr_selec.Data(), dm, year));
          corrections2D_[dm][year]->SetDirectory(0);
        }
      }
      f->Close();
      delete f;
    }

    ///////////////////////////////////////////////////////////////////
    //get the corrections based on the tau eta
    ///////////////////////////////////////////////////////////////////

    TString eta_corr_selec = selection;
    if(selection == "mumu") eta_corr_selec = "mutau";
    if(selection == "ee"  ) eta_corr_selec = "etau";
    const int eta_set = set;

    f = TFile::Open(Form("%s/jet_to_tau_correction_%s_%s%i_%i.root", path.Data(), eta_corr_selec.Data(), process.Data(), eta_set, year), "READ");
    if(!f && (useEtaCorrections_ || useMetDPhiCorrections_)) {
      std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No eta/metDPhi correction file found for year = "
                << year << " selection = " << eta_corr_selec.Data() << " set = " << eta_set << std::endl;
    } else if(f) {
      if(useEtaCorrections_) {
        TString hist = "EtaScale";
        if(process.Contains("QCD") || process.Contains("Top")) hist += "_QCD"; //QCD+Top use different binning
        etaCorrections_[year] = (TH1*) f->Get(hist.Data());
        if(!etaCorrections_[year]) {
          std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No eta correction hist found for year = "
                    << year << " selection = " << eta_corr_selec.Data() << std::endl;
        } else {
          etaCorrections_[year] = (TH1*) etaCorrections_[year]->Clone(Form("%s-EtaScale_%i", name_.Data(), year));
          etaCorrections_[year]->SetDirectory(0);
        }
      }
      if(useMetDPhiCorrections_) {
        TString hist = "OneMetDeltaPhi";
        if(process.Contains("QCD")) hist += "_QCD"; //QCD uses different binning
        metDPhiCorrections_[year][0] = (TH1*) f->Get(hist.Data());
        if(!metDPhiCorrections_[year][0]) {
          std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No OneMetDeltaPhi correction hist found for year = "
                    << year << " selection = " << eta_corr_selec.Data() << std::endl;
        } else {
          metDPhiCorrections_[year][0] = (TH1*) metDPhiCorrections_[year][0]->Clone(Form("%s-OneMetDeltaPhi_%i", name_.Data(), year));
          metDPhiCorrections_[year][0]->SetDirectory(0);
        }
        if(doMetDMCorrections_) {
          for(int idm = 0; idm < 4; ++idm) {
            metDPhiCorrections_[year][idm+1] = (TH1*) f->Get(Form("OneMetDeltaPhi%i", idm));
            if(!metDPhiCorrections_[year][idm+1]) {
              std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No OneMetDeltaPhi" << idm << " correction hist found for year = "
                        << year << " selection = " << eta_corr_selec.Data() << std::endl;
            } else {
              metDPhiCorrections_[year][idm+1] = (TH1*) metDPhiCorrections_[year][idm+1]->Clone(Form("%s-OneMetDeltaPhi%i_%i", name_.Data(), idm, year));
              metDPhiCorrections_[year][idm+1]->SetDirectory(0);
            }
          }
        }
      }
      f->Close();
      delete f;
    }

    ///////////////////////////////////////////////////////////////////
    //get the bias corrections
    ///////////////////////////////////////////////////////////////////

    if(useMTLepBias_ || useLepMBias_ || useOneIsoBias_) {
      int bias_set = set;
      const int remainder = set % 100; //base set before SS/loose ID offsets
      if(remainder > 35 && remainder < 40) { //using MC scales in a DR
        bias_set += 45; //offset to AR/SR MC region
      } else if(remainder == 30) { //QCD bias correction
        bias_set += 63; //uses set base 93 for isolation bias
      } else if(remainder > 30 && remainder <= 34) { //use data scales in a DR
        bias_set += 50; //offset to AR/SR MC region
      } else if(remainder == 35) { //MC scales in AR/SR
        bias_set = set; //keep the same set
      } else if(remainder == 88) { //MC W+Jets scales in DR with MC non-closure measurement
        bias_set = bias_set - remainder + 81;
      }

      //W+Jets bias uses W+Jets MC, whether or not the j-->tau measurement included background process subtraction or not
      TString bias_proc = process;
      if(bias_proc == "") {
        if(bias_set == 81) bias_proc = "WJets_";
      }

      f = TFile::Open(Form("%s/jet_to_tau_correction_%s_%s%i_%i.root", path.Data(), selection.Data(), bias_proc.Data(), bias_set, year), "READ");
      if(!f) {
        std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No bias correction file found for year = "
                  << year << " selection = " << selection.Data() << " set = " << bias_set << " process = " << bias_proc.Data() << std::endl;
      } else {
        if(useLepMBias_ && bias_mode != 4 && bias_mode != 5) { //mode 4/5 is QCD lepm bias for SS --> OS
          lepMBias_[year] = (TH1*) f->Get((bias_mode) == 6 ? "LepMBiasShape" : "LepMBias");
          if(!lepMBias_[year]) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No lepton mass bias hist found for year = "
                      << year << " selection = " << selection.Data() << std::endl;
          } else {
            lepMBias_[year] = (TH1*) lepMBias_[year]->Clone(Form("%s-LepMBias_%i", name_.Data(), year));
            lepMBias_[year]->SetDirectory(0);
          }
        }
        if(useMTLepBias_ && bias_mode != 5) { //mode 5 uses QCD MTLep bias in wider MTLep set
          mtLepBias_[year] = (TH1*) f->Get("MTLepBias");
          if(!mtLepBias_[year]) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No MT(ll, MET) bias hist found for year = "
                      << year << " selection = " << selection.Data() << std::endl;
          } else {
            mtLepBias_[year] = (TH1*) mtLepBias_[year]->Clone(Form("%s-MTLepBias_%i", name_.Data(), year));
            mtLepBias_[year]->SetDirectory(0);
          }
        }
        if(useOneIsoBias_) {
          oneIsoBias_[year] = (TH1*) f->Get("OneIsoBias");
          if(!oneIsoBias_[year]) {
            std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No One Iso/pT bias hist found for year = "
                      << year << " selection = " << selection.Data() << std::endl;
          } else {
            oneIsoBias_[year] = (TH1*) oneIsoBias_[year]->Clone(Form("%s-OneIsoBias_%i", name_.Data(), year));
            oneIsoBias_[year]->SetDirectory(0);
          }
        }
        f->Close();
        delete f;
        if(bias_mode == 4 || bias_mode == 5) { //QCD SS --> OS bias
          const int qcd_ss_bias_set = 95; //fixed set for this correction
          f = TFile::Open(Form("%s/jet_to_tau_correction_%s_%s%i_%i.root", path.Data(), selection.Data(), bias_proc.Data(), qcd_ss_bias_set, year), "READ");
          if(f) {
            if(useLepMBias_) { //QCD lepm bias for SS --> OS
              lepMBias_[year] = (TH1*) f->Get("LepMBias");
              if(!lepMBias_[year]) {
                std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No lepton mass bias hist found for year = "
                          << year << " selection = " << selection.Data() << std::endl;
              } else {
                lepMBias_[year] = (TH1*) lepMBias_[year]->Clone(Form("%s-LepMBias_%i", name_.Data(), year));
                lepMBias_[year]->SetDirectory(0);
              }
            }
            f->Close();
            delete f;
          }
        }
        if(bias_mode == 5) { //QCD MT(ll, MET) restricted range bias
          const int qcd_mtlep_bias_set = 1093; //fixed set for this correction
          f = TFile::Open(Form("%s/jet_to_tau_correction_%s_%s%i_%i.root", path.Data(), selection.Data(), bias_proc.Data(), qcd_mtlep_bias_set, year), "READ");
          if(f) {
            mtLepBias_[year] = (TH1*) f->Get("MTLepBias");
            if(!mtLepBias_[year]) {
              std::cout << "JetToTauWeight::JetToTauWeight: " << name_.Data() << " Warning! No MT(ll, MET) bias hist found for year = "
                        << year << " set = " << qcd_mtlep_bias_set << " selection = " << selection.Data() << std::endl;
            } else {
              mtLepBias_[year] = (TH1*) mtLepBias_[year]->Clone(Form("%s-MTLepBias_%i", name_.Data(), year));
              mtLepBias_[year]->SetDirectory(0);
            }
            f->Close();
            delete f;
          }
        }
      }
    } //end bias correction
  }
}

//-------------------------------------------------------------------------------------------------------------------------
JetToTauWeight::~JetToTauWeight() {
  for(std::pair<int, std::map<int, TH2*>> val_1 : histsData_) {
    for(std::pair<int, TH2*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(std::pair<int, std::map<int, TH1*>> val_1 : corrections_) {
    for(std::pair<int, TH1*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(std::pair<int, std::map<int, TH2*>> val_1 : corrections2D_) {
    for(std::pair<int, TH2*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(std::pair<int, TH1*> val : etaCorrections_) {if(val.second) delete val.second;}
  for(std::pair<int, std::map<int, TH1*>> val_1 : metDPhiCorrections_) {
    for(std::pair<int, TH1*> val_2 : val_1.second) {if(val_2.second) delete val_2.second;}
  }
  for(std::pair<int, TH1*> val : lepMBias_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH1*> val : mtLepBias_ ) {if(val.second) delete val.second;}
  for(std::pair<int, TH1*> val : oneIsoBias_) {if(val.second) delete val.second;}
  for(std::pair<int, std::map<int, std::map<int, TF1*>>> val_1 : funcsData_) {
    for(std::pair<int, std::map<int, TF1*>> val_2 : val_1.second) {
      for(std::pair<int, TF1*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
    }
  }
  for(std::pair<int, std::map<int, std::map<int, TH1*>>> val_1 : errorsData_) {
    for(std::pair<int, std::map<int, TH1*>> val_2 : val_1.second) {
      for(std::pair<int, TH1*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------
// Get factor to apply to data
float JetToTauWeight::GetDataFactor(int DM, int year, float pt, float eta,
                                    float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                                    float& up, float& down,
                                    float& pt_wt, float& pt_up, float& pt_down, float& bias) {
  pt_wt = 1.f; pt_up = 1.f; pt_down = 1.f; bias = 1.f;
  TH2* h = 0;
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
  // int  ptMode = Mode_ % 10;

  // Get the correct histogram
  h = histsData_[year][idm];
  TH1* hCorrection = corrections_[(doDMCorrections_ ? idm : 0)][year];

  //check if histogram is found
  if(!h) {
    std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined histogram for DM = "
              << DM << " year = " << year << std::endl;
    up = 1.f; down = 1.f;
    return 1.f;
  }
  if(!hCorrection && doPtCorrections_) {
    std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined correction histogram for DM = "
              << DM << " year = " << year << std::endl;
    up = 1.f; down = 1.f;
    return 1.f;
  }
  //get the fit function
  eta = std::fabs(eta);
  if(eta > 2.29) eta = 2.29;
  TF1* func = funcsData_[year][idm][h->GetYaxis()->FindBin(eta)-1];
  //check if function is found
  if(!func && useFits_) {
    std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined function for DM = "
              << DM << " eta = " << eta << " year = " << year << std::endl;
    up = 1.f; down = 1.f;
    return 1.f;
  }

  TH1* hFitterErrors = errorsData_[year][idm][h->GetYaxis()->FindBin(eta)-1];
  //check if errors are found
  if(!hFitterErrors && useFitterErrors_) {
    std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Undefined fitter errors for DM = "
              << DM << " eta = " << eta << " year = " << year << std::endl;
    up = 1.f; down = 1.f;
    return 1.f;
  }

  return GetFactor(h, func, hCorrection, hFitterErrors, pt, eta, DM, pt_lead, deltar, metdphi, lepm, mtlep, oneiso,
                   year, up, down, pt_wt, pt_up, pt_down, bias);
}

//-------------------------------------------------------------------------------------------------------------------------------
//Get weight without any systematics or corrections carried
float JetToTauWeight::GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso) {
  float up, down, pt_wt, pt_up, pt_down, bias;
  float weight = GetDataFactor(DM, year, pt, eta, pt_lead, deltar, metdphi, lepm, mtlep, oneiso,
                               up, down, pt_wt, pt_up, pt_down, bias);
  weight *= pt_wt*bias;
  return weight;
}

//-------------------------------------------------------------------------------------------------------------------------------
//Get weight without any systematics carried but pT correction also stored
float JetToTauWeight::GetDataFactor(int DM, int year, float pt, float eta, float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                                    float& pt_wt, float& bias) {
  float up, down, pt_up, pt_down;
  const float weight = GetDataFactor(DM, year, pt, eta, pt_lead, deltar, metdphi, lepm, mtlep, oneiso,
                                     up, down, pt_wt, pt_up, pt_down, bias);
  return weight;
}

//-------------------------------------------------------------------------------------------------------------------------------
// interal function to calculate the transfer factor
float JetToTauWeight::GetFactor(TH2* h, TF1* func, TH1* hCorrection, TH1* hFitterErrors,
                                float pt, float eta, int DM,
                                float pt_lead, float deltar, float metdphi, float lepm, float mtlep, float oneiso,
                                int year,
                                float& up, float& down,
                                float& pt_wt, float& pt_up, float& pt_down, float& bias) {
  //ensure within kinematic regions
  eta = std::fabs(eta);
  if(pt > 199.) pt = 199.;
  else if(pt < 20.f) pt = 20.f;
  if(eta > 2.29) eta = 2.29;

  //For DM binned factors
  int idm = 0; //default in case of undefined decay modes
  if     (DM ==  0) idm = 0;
  else if(DM ==  1) idm = 1;
  else if(DM == 10) idm = 2;
  else if(DM == 11) idm = 3;

  float eff = -1.f;
  float corr_error = 0.f;

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
    //ensure up/down encloses the fit value
    up   = std::max(up  , eff);
    down = std::min(down, eff);
  } else { //use the binned values
    //get bin value
    const int binx = h->GetXaxis()->FindBin(pt);
    const int biny = h->GetYaxis()->FindBin(eta);

    float eff_bin = h->GetBinContent(binx, biny);
    float err_bin = h->GetBinError  (binx, biny);
    if(doInterpolation_) {
      const double pt_bin = h->GetXaxis()->GetBinCenter(binx);
      bool leftofcenter = h->GetXaxis()->GetBinCenter(binx) > pt; //check which side of bin center we are
      //if at boundary, use interpolation from bin inside hist
      if(binx == 1)  leftofcenter = false;
      if(binx == h->GetNbinsX())  leftofcenter = true;
      const int binx_off = binx-(2*leftofcenter-1);
      const float eff_off = h->GetBinContent(binx_off, biny);
      const double pt_off = h->GetXaxis()->GetBinCenter(binx_off);
      //linear interpolation between the bin centers
      eff = eff_bin + (eff_off - eff_bin)*(pt - pt_bin)/(pt_off - pt_bin);
    } else {
      eff  = eff_bin;
      up   = eff + err_bin;
      down = eff - err_bin;
    }
  }

  ///////////////////////////////////////////////////////////
  // Get closure test correction
  ///////////////////////////////////////////////////////////

  if(pt_lead < 0.f || !doPtCorrections_) {
    pt_wt = 1.f;
  } else if(use2DCorrections_) {
    TH2* hcorr2D = corrections2D_[(doDMCorrections_) ? idm : 0][year];
    if(!hcorr2D) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! 2D pT correction histogram not found!"
                << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                << " dm = " << DM << " year = " << year
                << std::endl;
      pt_wt = 1.f;
      corr_error = 0.f;
    } else {
      const int binx_c = std::max(1, std::min(hcorr2D->GetXaxis()->FindBin((use2DCorrections_ > 1) ? pt_lead : pt_lead), hcorr2D->GetNbinsX()));
      const int biny_c = std::max(1, std::min(hcorr2D->GetYaxis()->FindBin((use2DCorrections_ > 1) ? pt      : deltar ), hcorr2D->GetNbinsY()));
      pt_wt = hcorr2D->GetBinContent(binx_c, biny_c);
      corr_error = hcorr2D->GetBinError(binx_c, biny_c);
    }
  } else { //1D pT corrections
    const int corr_bin = (pt_lead > 0.f) ? std::max(1, std::min(hCorrection->GetNbinsX(), hCorrection->FindBin(pt_lead))) : 1;
    pt_wt = hCorrection->GetBinContent(corr_bin);
    corr_error = hCorrection->GetBinError(corr_bin);
  }
  if(pt_wt < 0.f) {
    std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Lead pT correction < 0! Lead pT = " << pt_lead
              << " pt correction weight = " << pt_wt
              << " year = " << year << std::endl;
    pt_wt = 1.f;
    corr_error = 0.f;
  }
  //Apply tau eta corrections if asked for
  if(useEtaCorrections_) {
    TH1* heta = etaCorrections_[year];
    if(!heta) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Eta correction histogram not found!"
                << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                << " dm = " << DM << " year = " << year
                << std::endl;
    } else {
      float eta_wt = heta->GetBinContent(heta->FindBin(eta));
      if(eta_wt <= 0.f) {
        if(verbose_) std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Eta correction <= 0! Eta = " << eta
                               << " eta correction weight = " << eta_wt
                               << " year = " << year << std::endl;
        eta_wt = 1.f;
      }
      pt_wt *= eta_wt;
    }
  }
  //Apply one met delta phi corrections if asked for
  if(useMetDPhiCorrections_) {
    TH1* hmet = (doMetDMCorrections_) ? metDPhiCorrections_[year][idm+1] : metDPhiCorrections_[year][0];
    if(!hmet) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! OneMetDeltaPhi correction histogram not found!"
                << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead << " metdphi = " << metdphi
                << " dm = " << DM << " year = " << year
                << std::endl;
    } else {
      float met_wt = hmet->GetBinContent(hmet->FindBin(metdphi));
      if(met_wt <= 0.) {
        if(verbose_) std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! metdphi correction <= 0! metdphi = " << metdphi
                               << " met correction weight = " << met_wt
                               << " year = " << year << std::endl;
        met_wt = 1.f;
      }
      pt_wt *= met_wt;
    }
  }

  ///////////////////////////////////////////////////////////
  // Apply bias corrections
  ///////////////////////////////////////////////////////////

  bias = 1.f;
  if(useLepMBias_) {
    TH1* hbias = lepMBias_[year];
    if(!hbias) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! LepM bias histogram not found!"
                << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                << " dm = " << DM << " year = " << year
                << std::endl;
    } else {
      bias *= hbias->GetBinContent(std::max(1, std::min(hbias->GetNbinsX(), hbias->FindBin(lepm))));
    }
  }
  if(useMTLepBias_) {
    TH1* hbias = mtLepBias_[year];
    if(!hbias) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! MT(ll, MET) bias histogram not found!"
                << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                << " dm = " << DM << " year = " << year
                << std::endl;
    } else {
      bias *= hbias->GetBinContent(std::max(1, std::min(hbias->GetNbinsX(), hbias->FindBin(mtlep))));
    }
  }
  if(useOneIsoBias_) {
    TH1* hbias = oneIsoBias_[year];
    if(!hbias) {
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! One Iso/pT bias histogram not found!"
                << " pt = " << pt << " eta = " << eta << " pt_lead = " << pt_lead
                << " dm = " << DM << " year = " << year
                << std::endl;
    } else {
      bias *= hbias->GetBinContent(std::max(1, std::min(hbias->GetNbinsX(), hbias->FindBin(oneiso))));
    }
  }

  if(bias <= 0.f) {
    if(verbose_ > 1) { //not unusual due to DR being defined by some bias variables, so only warn if verbose
      std::cout << "JetToTauWeight::" << __func__ << ": " << name_.Data() << " Warning! Bias correction <= 0! Lep mass = " << lepm << " MT(lep, MET) = " << mtlep
                << " bias correction = " << bias
                << " year = " << year << std::endl;
    }
    bias = 1.f;
  }

  ///////////////////////////////////////////////////////////
  // Convert efficiency into scale factor
  ///////////////////////////////////////////////////////////

  const static float min_eff = 0.000001;
  const static float max_eff = 0.75    ; //force weight <= 0.75 / (1 - 0.75) = 3
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

  //calculate pt correction uncertainties
  //Set systematic to be the correction size
  //Set up to be in direction of pt weight, down in opposite direction around the pt weight
  const float pt_dev = pt_wt - 1.f; //deviation from 1
  const float sigma = (1 - 2*(pt_dev < 0.f)) * std::sqrt(pt_dev*pt_dev + corr_error*corr_error); //sign it with pt_dev sign
  pt_up   = std::max(0.f, pt_wt + sigma);
  pt_down = std::max(0.f, pt_wt - sigma);

  return eff;
}
