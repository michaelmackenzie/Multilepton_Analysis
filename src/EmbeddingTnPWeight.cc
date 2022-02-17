#include "interface/EmbeddingTnPWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingTnPWeight::EmbeddingTnPWeight(const int Mode, const int verbose) : verbose_(verbose), rnd_(nullptr) {
  // Check Mode bits
  interpolate_ = Mode % 10 == 1; //interpolate between pT bin centers

  TFile* f = 0;
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year = k2016; year <= k2018; ++year) {
    std::vector<TString> files = {Form("%s/embedding_eff_mumu_mode-0_%i.root", path.Data(), year + 2016),
                                  Form("%s/embedding_eff_mumu_mode-1_%i.root", path.Data(), year + 2016),
                                  Form("%s/embedding_eff_mumu_mode-2_%i.root", path.Data(), year + 2016),
                                  Form("%s/embedding_eff_ee_mode-0_%i.root"  , path.Data(), year + 2016),
                                  Form("%s/embedding_eff_ee_mode-1_%i.root"  , path.Data(), year + 2016)};
    for(TString file : files) {
      //Get muon trigger scale factors
      f = TFile::Open(file.Data(), "READ");
      if(!f) {
        std::cout << "!!! Embedding corrections file " << file.Data() << " for " << year + 2016 << " not found!\n";
      } else {
        TH2F* hData = (TH2F*) f->Get("PtVsEtaData");
        TH2F* hMC   = (TH2F*) f->Get("PtVsEtaMC"  );
        TH2F* hSF   = (TH2F*) f->Get("PtVsEtaSF"  );
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
            }
          } else { //electron files
            if(file.Contains("mode-0")) { //trigger files
              electronTrigDataEff_[year] = hData;
              electronTrigMCEff_  [year] = hMC  ;
            } else if(file.Contains("mode-1")) { //ID files
              electronIDDataEff_[year] = hData;
              electronIDMCEff_  [year] = hMC  ;
            }
          }
        }
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingTnPWeight::~EmbeddingTnPWeight() {
  if(rnd_) delete rnd_;
  for(std::pair<int, TH2F*> val : muonTrigDataEff_    ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : muonTrigMCEff_      ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : muonIDDataEff_      ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : muonIDMCEff_        ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : muonIsoIDDataEff_   ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : muonIsoIDMCEff_     ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : electronTrigDataEff_) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : electronTrigMCEff_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : electronIDDataEff_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2F*> val : electronIDMCEff_    ) {if(val.second) delete val.second;}
}

//-------------------------------------------------------------------------------------------------------------------------
// For a given (pt, eta) point and Data/MC histograms, evaluate the scale factor
double EmbeddingTnPWeight::GetScale(const TH2F* data, const TH2F* mc, const double pt, const double eta, float& data_eff, float& mc_eff) {
  if(interpolate_) {
    //Perform a linear interpolation
    data_eff = Utilities::Interpolate(data, eta, pt, Utilities::kYAxis);
    mc_eff   = Utilities::Interpolate(mc  , eta, pt, Utilities::kYAxis);
  } else {
    data_eff = data->GetBinContent(data->GetXaxis()->FindBin(eta), data->GetYaxis()->FindBin(pt));
    mc_eff   = mc  ->GetBinContent(mc  ->GetXaxis()->FindBin(eta), mc  ->GetYaxis()->FindBin(pt));
  }
  const double scale_factor = data_eff / mc_eff;
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingTnPWeight::MuonIDWeight(double pt, double eta, int year) {
  year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  const TH2F* hIDMC   = muonIDMCEff_  [year];
  const TH2F* hIDData = muonIDDataEff_[year];
  if(!hIDMC || !hIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": ID histograms not found!\n";
    return 1.;
  }

  const TH2F* hIsoIDMC   = muonIsoIDMCEff_  [year];
  const TH2F* hIsoIDData = muonIsoIDDataEff_[year];
  if(!hIsoIDMC || !hIsoIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Iso ID histograms not found!\n";
    return 1.;
  }

  double scale_factor(1.);
  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.39, fabs(eta));
  float data_eff(1.), mc_eff(1.);

  ///////////////////////////
  // Apply ID weight
  ///////////////////////////
  scale_factor *= GetScale(hIDData, hIDMC, pt, eta, data_eff, mc_eff);

  ///////////////////////////
  // Apply Iso ID weight
  ///////////////////////////
  scale_factor *= GetScale(hIsoIDData, hIsoIDMC, pt, eta, data_eff, mc_eff);

  if(scale_factor <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingTnPWeight::MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
  year -= 2016;
  data_eff = 0.5; mc_eff = 0.5;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  //If it can't fire the trigger, no correction
  if(pt < 25. || (pt < 28. && year == k2017)) return 1.;

  const TH2F* hMC   = muonTrigMCEff_  [year];
  const TH2F* hData = muonTrigDataEff_[year];
  if(!hMC || !hData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Trigger histograms not found!\n";
    return 1.;
  }

  double scale_factor(1.);
  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.39, fabs(eta)); //std::max(-2.39, std::min(2.39, eta));

  ///////////////////////////
  // Apply trigger weight
  ///////////////////////////
  scale_factor *= GetScale(hData, hMC, pt, eta, data_eff, mc_eff);

  if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__
              << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
              << " pt = " << pt << " eta = " << eta << " year = " << year + 2016
              << ", returning 1" << std::endl;
    //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
    mc_eff = 0.5;
    data_eff = 0.5;
    return 1.;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingTnPWeight::ElectronIDWeight(double pt, double eta, int year) {
  year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  const TH2F* hIDMC   = electronIDMCEff_  [year];
  const TH2F* hIDData = electronIDDataEff_[year];
  if(!hIDMC || !hIDData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": ID histograms not found!\n";
    return 1.;
  }

  double scale_factor(1.);
  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.49, std::fabs(eta)); //std::max(-2.49, std::min(2.49, eta));
  float data_eff(1.), mc_eff(1.);

  ///////////////////////////
  // Apply ID weight
  ///////////////////////////
  scale_factor *= GetScale(hIDData, hIDMC, pt, eta, data_eff, mc_eff);

  if(scale_factor <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  return scale_factor;
}


//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingTnPWeight::ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
  year -= 2016;
  data_eff = 0.5; mc_eff = 0.5;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingTnPWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  //IF it can't fire the trigger, no correction
  if(pt < 28. || (pt < 33. && year != k2016)) return 1.;

  const TH2F* hMC   = electronTrigMCEff_  [year];
  const TH2F* hData = electronTrigDataEff_[year];
  if(!hMC || !hData) {
    std::cout << "Error! EmbeddingTnPWeight::" << __func__ << ": Trigger histograms not found!\n";
    return 1.;
  }

  double scale_factor(1.);
  pt = std::max(10.01, std::min(pt, 499.));
  eta = std::min(2.49, fabs(eta)); //std::max(-2.49, std::min(2.49, eta));

  ///////////////////////////
  // Apply trigger weight
  ///////////////////////////

  scale_factor *= GetScale(hData, hMC, pt, eta, data_eff, mc_eff);

  if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingTnPWeight::" << __func__
              << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
              << " pt = " << pt << " eta = " << eta << " year = " << year + 2016
              << ", returning 1" << std::endl;
    //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
    mc_eff = 0.5;
    data_eff = 0.5;
    return 1.;
  }
  return scale_factor;
}
