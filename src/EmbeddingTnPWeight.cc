#include "interface/EmbeddingTnPWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingTnPWeight::EmbeddingTnPWeight(const int Mode, const int verbose) : verbose_(verbose), rnd_(nullptr) {
  // Check Mode bits
  interpolate_   = (Mode /  1) % 10 == 1; //interpolate between pT bin centers
  useRunPeriods_ = (Mode / 10) % 10; //use run-dependent scale factors: 0 = full year scales; 1 = 2016 B-F/GH; 2 = 2016 B-F/GH and 2018 A-C/D

  TFile* f = 0;
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year = k2016; year < kLast; ++year) {
    int year_v = year + 2016;
    if(year == k2016BF  || year == k2016GH) year_v = 2016;
    if(year == k2018ABC || year == k2018D ) year_v = 2018;
    std::vector<TString> files = {Form("%s/embedding_eff_mumu_mode-0_%i%s.root", path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_mumu_mode-1_%i%s.root", path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_mumu_mode-2_%i%s.root", path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_mumu_mode-3_%i%s.root", path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_mumu_mode-4_%i%s.root", path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_ee_mode-0_%i%s.root"  , path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_ee_mode-1_%i%s.root"  , path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_ee_mode-2_%i%s.root"  , path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_ee_mode-3_%i%s.root"  , path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : ""),
                                  Form("%s/embedding_eff_ee_mode-4_%i%s.root"  , path.Data(), year_v, (year > k2018) ? Form("_period_%i", (year - k2018) % 2) : "")
    };
    for(TString file : files) {
      //Get muon trigger scale factors
      f = TFile::Open(file.Data(), "READ");
      if(!f) {
        std::cout << "!!! Embedding corrections file " << file.Data() << " for " << year_v << " not found!\n";
      } else {
        TH2* hData = (TH2*) f->Get("PtVsEtaData");
        TH2* hMC   = (TH2*) f->Get("PtVsEtaMC"  );
        TH2* hSF   = (TH2*) f->Get("PtVsEtaSF"  );
        if(!hData || !hMC || !hSF) {
          std::cout << "!!! Efficiency histograms not found in " << file.Data() << std::endl;
        } else {
          hData->SetName(Form("%s_%s", hData->GetName(), hData->GetTitle()));
          hMC  ->SetName(Form("%s_%s", hMC  ->GetName(), hMC  ->GetTitle()));
          hSF  ->SetName(Form("%s_%s", hSF  ->GetName(), hSF  ->GetTitle()));
          hData->SetDirectory(0);
          hMC  ->SetDirectory(0);
          hSF  ->SetDirectory(0);
          if(file.Contains("_mumu_")) { //muon files
            if(file.Contains("mode-0")) { //trigger files
              muonTrigDataEff_[year] = hData;
              muonTrigMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-1")) { //ID files
              muonIDDataEff_[year] = hData;
              muonIDMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-2")) { //Iso ID files
              muonIsoIDDataEff_[year] = hData;
              muonIsoIDMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-3")) { //Loose + !Tight Iso ID files
              muonQCDIsoIDDataEff_[year] = hData;
              muonQCDIsoIDMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-4")) { //Loose + !Tight Iso ID trigger files
              muonQCDTrigDataEff_[year] = hData;
              muonQCDTrigMCEff_  [year] = hMC  ;
            }
          } else { //electron files
            if(file.Contains("mode-0")) { //trigger files
              electronTrigDataEff_[year] = hData;
              electronTrigMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-1")) { //ID files
              electronIDDataEff_[year] = hData;
              electronIDMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-2")) { //Iso ID files
              electronIsoIDDataEff_[year] = hData;
              electronIsoIDMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-3")) { //Loose + !Tight ID files
              electronQCDIsoIDDataEff_[year] = hData;
              electronQCDIsoIDMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-4")) { //Loose + !Tight ID trigger files
              electronQCDTrigDataEff_[year] = hData;
              electronQCDTrigMCEff_  [year] = hMC  ;
            }
          }
        }
        f->Close();
        delete f;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingTnPWeight::~EmbeddingTnPWeight() {
  if(rnd_) delete rnd_;
  for(std::pair<int, TH2*> val : muonTrigDataEff_    ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : muonTrigMCEff_      ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : muonIDDataEff_      ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : muonIDMCEff_        ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : muonIsoIDDataEff_   ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : muonIsoIDMCEff_     ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : electronTrigDataEff_) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : electronTrigMCEff_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : electronIDDataEff_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : electronIDMCEff_    ) {if(val.second) delete val.second;}
}

//-------------------------------------------------------------------------------------------------------------------------
// For a given (pt, eta) point and Data/MC histograms, evaluate the scale factor
double EmbeddingTnPWeight::GetScale(const TH2* data, const TH2* mc, const double pt, const double eta, float& data_eff, float& mc_eff,
                                    float* data_var, float* mc_var) {
  if(interpolate_) {
    //Perform a linear interpolation
    data_eff = Utilities::Interpolate(data, eta, pt, Utilities::kYAxis);
    mc_eff   = Utilities::Interpolate(mc  , eta, pt, Utilities::kYAxis);
    if(data_var && mc_var) { //FIXME: Implement uncertainties on interpolation
      data_var[0] = data_eff; data_var[1] = data_eff;
      mc_var  [0] = mc_eff  ; mc_var  [1] = mc_eff;
    }
  } else {
    const int xbin_data = std::max(1, std::min(data->GetNbinsX(), data->GetXaxis()->FindBin(eta)));
    const int ybin_data = std::max(1, std::min(data->GetNbinsY(), data->GetYaxis()->FindBin(pt )));
    data_eff = data->GetBinContent(xbin_data, ybin_data);
    if(data_var) {
      const double err = data->GetBinError(xbin_data, ybin_data);
      data_var[0] = err; data_var[1] = err;
    }
    const int xbin_mc   = std::max(1, std::min(mc  ->GetNbinsX(), mc  ->GetXaxis()->FindBin(eta)));
    const int ybin_mc   = std::max(1, std::min(mc  ->GetNbinsY(), mc  ->GetYaxis()->FindBin(pt )));
    mc_eff   = mc  ->GetBinContent(xbin_mc  , ybin_mc  );
    if(mc_var) {
      const double err = mc->GetBinError(xbin_mc, ybin_mc);
      mc_var[0] = err; mc_var[1] = err;
    }
  }
  const double scale_factor = (mc_eff > 0.) ? data_eff / mc_eff : 1.;
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
// Get Muon ID + Iso ID scale factor
double EmbeddingTnPWeight::MuonIDWeight(double pt, double eta, int year,
                                        float& id_wt, float& id_up, float& id_down,
                                        float& iso_wt, float& iso_up, float& iso_down,
                                        bool qcd, int period) {
  if(year > 2000) year -= 2016;
  id_wt = 1.f; id_up = 1.f; id_down = 1.f;
  iso_wt = 1.f; iso_up = 1.f; iso_down = 1.f;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  const int index = ((year == k2016 && useRunPeriods_) || (year == k2018 && useRunPeriods_ > 1)) ? ((year == k2016) ? k2016BF : k2018ABC) + period : year;
  const TH2* hIDMC   = muonIDMCEff_  [index];
  const TH2* hIDData = muonIDDataEff_[index];
  if(!hIDMC || !hIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": ID histograms not found! year = "
              << year + 2016 << ", period = " << period << std::endl;
    return 1.;
  }

  const TH2* hIsoIDMC   = (qcd) ? muonQCDIsoIDMCEff_  [index] : muonIsoIDMCEff_  [index];
  const TH2* hIsoIDData = (qcd) ? muonQCDIsoIDDataEff_[index] : muonIsoIDDataEff_[index];
  if(!hIsoIDMC || !hIsoIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Iso ID histograms not found! year = "
              << year + 2016 << ", period = " << period << std::endl;
    return 1.;
  }

  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.39, std::max(-2.39, eta));
  float eta_var = eta;

  double scale_factor(1.);
  float data_eff(1.f), mc_eff(1.f);
  float data_unc[2] = {1.f, 1.f}, mc_unc[2] = {1.f, 1.f}; //for retrieving uncertainties

  ///////////////////////////
  // Apply ID weight
  ///////////////////////////
  bool use_abs_eta = hIDMC->GetXaxis()->GetBinLowEdge(1) > -1.; //test if |eta| or eta on x-axis
  eta_var = (use_abs_eta) ? std::fabs(eta) : eta;
  id_wt = GetScale(hIDData, hIDMC, pt, eta_var, data_eff, mc_eff, data_unc, mc_unc);
  float err = data_eff/mc_eff * std::sqrt(std::pow(mc_unc[0]/mc_eff, 2) + std::pow(data_unc[0]/data_eff, 2));
  id_up   = std::max(0.f, id_wt + err);
  id_down = std::max(0.f, id_wt - err);
  scale_factor *= id_wt;

  if(verbose_) {
    printf(" ID weight = %.2f + %.2f - %.2f\n", id_wt, id_up, id_down);
  }

  ///////////////////////////
  // Apply Iso ID weight
  ///////////////////////////
  use_abs_eta = hIsoIDMC->GetXaxis()->GetBinLowEdge(1) > -1.; //test if |eta| or eta on x-axis
  eta_var = (use_abs_eta) ? std::fabs(eta) : eta;
  iso_wt = GetScale(hIsoIDData, hIsoIDMC, pt, eta_var, data_eff, mc_eff, data_unc, mc_unc);
  err = data_eff/mc_eff * std::sqrt(std::pow(mc_unc[0]/mc_eff, 2) + std::pow(data_unc[0]/data_eff, 2));
  iso_up   = std::max(0.f, iso_wt + err);
  iso_down = std::max(0.f, iso_wt - err);
  scale_factor *= iso_wt;

  if(verbose_) {
    printf(" Iso weight = %.2f + %.2f - %.2f\n", iso_wt, iso_up, iso_down);
  }

  if(scale_factor <= 0. || !std::isfinite(scale_factor)) {
    std::cout << "Warning! Scale factor <= 0 or undefined (" << scale_factor << ") in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    iso_wt = 1.f; id_wt = 1.f;
    return 1.;
  }

  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
// Get Muon trigger scale factor
double EmbeddingTnPWeight::MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff,
                                             float& data_up, float& mc_up, float& data_down, float& mc_down,
                                             bool qcd, int period) {
  if(year > 2000) year -= 2016;
  data_eff = 0.5f; mc_eff = 0.5f; data_up = 0.5f; mc_up = 0.5f; data_down = 0.5f; mc_down = 0.5f;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  //If it can't fire the trigger, no correction
  if(pt < 25. || (pt < 28. && year == k2017)) return 1.;

  const int index = ((year == k2016 && useRunPeriods_) || (year == k2018 && useRunPeriods_ > 1)) ? ((year == k2016) ? k2016BF : k2018ABC) + period : year;
  const TH2* hMC   = (qcd) ? muonQCDTrigMCEff_  [index] : muonTrigMCEff_  [index];
  const TH2* hData = (qcd) ? muonQCDTrigDataEff_[index] : muonTrigDataEff_[index];
  if(!hMC || !hData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Trigger histograms not found! year = "
              << year + 2016 << ", period = " << period << std::endl;
    return 1.;
  }

  const bool use_abs_eta = hMC->GetXaxis()->GetBinLowEdge(1) > -1.; //test if |eta| or eta on x-axis
  if(use_abs_eta) eta = std::fabs(eta);
  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.39, std::max(-2.39, eta));

  ///////////////////////////
  // Apply trigger weight
  ///////////////////////////
  float data_unc[2], mc_unc[2];
  double scale_factor = GetScale(hData, hMC, pt, eta, data_eff, mc_eff, data_unc, mc_unc);

  if(data_eff <= 0. || mc_eff <= 0. ||
     !std::isfinite(scale_factor) || !std::isfinite(data_eff) || !std::isfinite(mc_eff)) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__
              << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
              << " pt = " << pt << " eta = " << eta << " qcd = " << qcd
              << " year = " << year + 2016 << " period = " << period
              << ", returning 1" << std::endl;
    //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
    mc_eff = 0.5;
    data_eff = 0.5;
    return 1.;
  }

  data_up   = data_eff + data_unc[0];
  mc_up     = mc_eff   + mc_unc  [0];
  data_down = data_eff - data_unc[1];
  mc_down   = mc_eff   - mc_unc  [1];

  const float max_eff(0.999), min_eff(0.001); // maximum of 99.9% efficiency, minimum of 0.1%
  data_eff  = std::max(min_eff, std::min(max_eff, data_eff ));
  mc_eff    = std::max(min_eff, std::min(max_eff, mc_eff   ));
  data_up   = std::max(min_eff, std::min(max_eff, data_up  ));
  mc_up     = std::max(min_eff, std::min(max_eff, mc_up    ));
  data_down = std::max(min_eff, std::min(max_eff, data_down));
  mc_down   = std::max(min_eff, std::min(max_eff, mc_down  ));

  scale_factor = data_eff / mc_eff;

  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
// Get Electron ID scale factor
double EmbeddingTnPWeight::ElectronIDWeight(double pt, double eta, int year,
                                            float& id_wt, float& id_up, float& id_down,
                                            float& iso_wt, float& iso_up, float& iso_down,
                                            bool qcd, int period
                                            ) {
  id_wt = 1.f; id_up = 1.f; id_down = 1.f;
  iso_wt = 1.f; iso_up = 1.f; iso_down = 1.f;
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  // x / y
  const int index = ((year == k2016 && useRunPeriods_) || (year == k2018 && useRunPeriods_ > 1)) ? ((year == k2016) ? k2016BF : k2018ABC) + period : year;
  const TH2* hIDMC   = electronIDMCEff_  [index];
  const TH2* hIDData = electronIDDataEff_[index];
  if(!hIDMC || !hIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": ID histograms not found! year = "
              << year + 2016 << ", period = " << period << std::endl;
    return 1.;
  }

  const TH2* hIsoIDMC   = (qcd) ? electronQCDIsoIDMCEff_  [index] : electronIsoIDMCEff_  [index];
  const TH2* hIsoIDData = (qcd) ? electronQCDIsoIDDataEff_[index] : electronIsoIDDataEff_[index];
  if(!hIsoIDMC || !hIsoIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Iso ID histograms not found! year = "
              << year + 2016 << ", period = " << period << std::endl;
    return 1.;
  }

  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.49, std::max(-2.49, eta));

  float eta_var = eta;

  double scale_factor(1.);
  float data_eff(1.f), mc_eff(1.f);
  float data_unc[2] = {1.f, 1.f}, mc_unc[2] = {1.f, 1.f}; //for retrieving uncertainties

  ///////////////////////////
  // Apply ID weight
  ///////////////////////////

  bool use_abs_eta = hIDMC->GetXaxis()->GetBinLowEdge(1) > -1.; //test if |eta| or eta on x-axis
  eta_var = (use_abs_eta) ? std::fabs(eta) : eta;
  id_wt = GetScale(hIDData, hIDMC, pt, eta_var, data_eff, mc_eff, data_unc, mc_unc);
  float err = data_eff/mc_eff * std::sqrt(std::pow(mc_unc[0]/mc_eff, 2) + std::pow(data_unc[0]/data_eff, 2));
  id_up   = std::max(0.f, id_wt + err);
  id_down = std::max(0.f, id_wt - err);
  scale_factor *= id_wt;

  if(verbose_) {
    printf(" ID weight = %.2f + %.2f - %.2f\n", id_wt, id_up, id_down);
  }

  ///////////////////////////
  // Apply IsoID weight
  ///////////////////////////
  use_abs_eta = hIsoIDMC->GetXaxis()->GetBinLowEdge(1) > -1.; //test if |eta| or eta on x-axis
  eta_var = (use_abs_eta) ? std::fabs(eta) : eta;
  iso_wt = GetScale(hIsoIDData, hIsoIDMC, pt, eta_var, data_eff, mc_eff, data_unc, mc_unc);
  err = data_eff/mc_eff * std::sqrt(std::pow(mc_unc[0]/mc_eff, 2) + std::pow(data_unc[0]/data_eff, 2));
  iso_up   = std::max(0.f, iso_wt + err);
  iso_down = std::max(0.f, iso_wt - err);
  scale_factor *= iso_wt;

  if(verbose_) {
    printf(" Iso weight = %.2f + %.2f - %.2f\n", iso_wt, iso_up, iso_down);
  }

  if(scale_factor <= 0. || !std::isfinite(scale_factor)) {
    std::cout << "Warning! Scale factor <= 0 or undefined (" << scale_factor << ") in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    iso_wt = 1.f; id_wt = 1.f;
    return 1.;
  }
  return scale_factor;
}


//-------------------------------------------------------------------------------------------------------------------------
// Get Electron trigger scale factor
double EmbeddingTnPWeight::ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff,
                                                 float& data_up, float& mc_up, float& data_down, float& mc_down,
                                                 bool qcd, int period) {
  if(year > 2000) year -= 2016;
  data_eff = 0.5f; mc_eff = 0.5f; data_up = 0.5f; mc_up = 0.5f; data_down = 0.5f; mc_down = 0.5f;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  //IF it can't fire the trigger, no correction
  if(pt < 28. || (pt < 33. && year != k2016)) return 1.;

  const int index = ((year == k2016 && useRunPeriods_) || (year == k2018 && useRunPeriods_ > 1)) ? ((year == k2016) ? k2016BF : k2018ABC) + period : year;
  const TH2* hMC   = (qcd) ? electronQCDTrigMCEff_  [index] : electronTrigMCEff_  [index];
  const TH2* hData = (qcd) ? electronQCDTrigDataEff_[index] : electronTrigDataEff_[index];
  if(!hMC || !hData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Trigger histograms not found! year = "
              << year + 2016 << ", period = " << period << std::endl;
    return 1.;
  }

  const bool use_abs_eta = hMC->GetXaxis()->GetBinLowEdge(1) > -1.; //test if |eta| or eta on x-axis
  if(use_abs_eta) eta = std::fabs(eta);
  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.49, std::max(-2.49, eta));

  ///////////////////////////
  // Apply trigger weight
  ///////////////////////////
  float data_unc[2], mc_unc[2];
  double scale_factor = GetScale(hData, hMC, pt, eta, data_eff, mc_eff, data_unc, mc_unc);

  if(data_eff <= 0. || mc_eff <= 0. ||
     !std::isfinite(scale_factor) || !std::isfinite(data_eff) || !std::isfinite(mc_eff)) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__
              << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
              << " pt = " << pt << " eta = " << eta << " qcd = " << qcd
              << " year = " << year + 2016 << " period = " << period
              << ", returning 1" << std::endl;
    //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
    mc_eff = 0.5;
    data_eff = 0.5;
    return 1.;
  }

  data_up   = data_eff + data_unc[0];
  mc_up     = mc_eff   + mc_unc  [0];
  data_down = data_eff - data_unc[1];
  mc_down   = mc_eff   - mc_unc  [1];

  const float max_eff(0.999), min_eff(0.001); // maximum of 99.9% efficiency, minimum of 0.1%
  data_eff  = std::max(min_eff, std::min(max_eff, data_eff ));
  mc_eff    = std::max(min_eff, std::min(max_eff, mc_eff   ));
  data_up   = std::max(min_eff, std::min(max_eff, data_up  ));
  mc_up     = std::max(min_eff, std::min(max_eff, mc_up    ));
  data_down = std::max(min_eff, std::min(max_eff, data_down));
  mc_down   = std::max(min_eff, std::min(max_eff, mc_down  ));

  scale_factor = data_eff / mc_eff;

  return scale_factor;
}
