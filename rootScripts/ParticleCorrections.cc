#include "ParticleCorrections.hh"


double ParticleCorrections::MuonWeight(double pt, double eta, int trigger, int year) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }
  
  if(pt >= 120.) pt = 119.; //maximum pT for corrections
  else if(pt < 20.) pt = 20.; //minimum pT for corrections
  if(eta >= 2.4) eta = 2.39; //maximum eta for corrections
  else if(eta <= -2.4) eta = -2.39; //minimum eta for corrections

  double rand = fRnd->Uniform();
  bool firstSection = true; //whether this MC is for the first or second part of the data taking period
  if(year == k2016)
    firstSection = rand <= 19.72/35.86;
  else if(year == k2018)
    firstSection = rand <= 8.98/59.59;

  TH2F* hID = muonIDMap[2*year + !firstSection];
  double id_scale = hID->GetBinContent(hID->GetXaxis()->FindBin(fabs(eta)), hID->GetYaxis()->FindBin(pt));

  TH2F* hIso = muonIsoMap[2*year + !firstSection];
  double iso_scale = hIso->GetBinContent(hIso->GetXaxis()->FindBin(fabs(eta)), hIso->GetYaxis()->FindBin(pt));

  if(year == k2016) {
    if(trigger == kLowTrigger && pt < 26.) pt = 26.;
    else if(trigger == kHighTrigger && pt < 52.) pt = 52.;
  } else if (year == k2017) {
    if(trigger == kLowTrigger && pt < 29.) pt = 29.;
    else if(trigger == kHighTrigger && pt < 52.) pt = 52.;
  } else if (year == k2018) {
    if(trigger == kLowTrigger && pt < 26.) pt = 26.;
    else if(trigger == kHighTrigger && pt < 52.) pt = 52.;
  }
  TH2F* hTrigger = (trigger == kLowTrigger) ? muonLowTriggerMap[2*year + !firstSection] : muonHighTriggerMap[2*year + !firstSection] ;
  double trig_scale = hTrigger->GetBinContent(hTrigger->GetXaxis()->FindBin(fabs(eta)), hTrigger->GetYaxis()->FindBin(pt));

  double scale_factor = id_scale * iso_scale * trig_scale;
  return scale_factor;
}

double ParticleCorrections::ElectronWeight(double pt, double eta, int year) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }

  if(year == k2016) {
    if(pt >= 150.) pt = 149.9; //maximum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2017) {
    if(pt >= 499.) pt = 498.9; //maximum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2018) {
    if(pt >= 150.) pt = 149.9; //maximum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  }
  
  TH2F* hID = electronIDMap[year];
  double id_scale = hID->GetBinContent(hID->GetXaxis()->FindBin(fabs(eta)), hID->GetYaxis()->FindBin(pt));

  TH2F* hReco = electronRecoMap[year];
  double reco_scale = hReco->GetBinContent(hReco->GetXaxis()->FindBin(fabs(eta)), hReco->GetYaxis()->FindBin(pt));

  if(year == k2016) reco_scale = 1.;
  
  double scale_factor = id_scale * reco_scale;
  return scale_factor;
}