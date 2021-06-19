#include "interface/ParticleCorrections.hh"


void ParticleCorrections::MuonWeight(double pt, double eta, int trigger, int year, float& trig_scale,
                                     float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                                     float& weight_iso, float& weight_up_iso, float& weight_down_iso, int& ibin_iso
                                     ) {
  weight_id  = 1.; weight_up_id  = 1.; weight_down_id  = 1.; ibin_id  = 0;
  weight_iso = 1.; weight_up_iso = 1.; weight_down_iso = 1.; ibin_iso = 0;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return;
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

  //axes flip between years for some reason
  int binx = (year != k2016) ? hID->GetXaxis()->FindBin(pt) : hID->GetXaxis()->FindBin(fabs(eta));
  int biny = (year != k2016) ? hID->GetYaxis()->FindBin(fabs(eta)) : hID->GetYaxis()->FindBin(pt);
  double id_scale = hID->GetBinContent(binx, biny);
  double id_error = hID->GetBinError(binx, biny);
  ibin_id = 10000 * firstSection + 100*biny + binx; //assumes binx and biny < 100

  TH2F* hIso = muonIsoMap[2*year + !firstSection];

  binx = (year != k2016) ? hIso->GetXaxis()->FindBin(pt) : hIso->GetXaxis()->FindBin(fabs(eta));
  biny = (year != k2016) ? hIso->GetYaxis()->FindBin(fabs(eta)) : hIso->GetYaxis()->FindBin(pt);
  double iso_scale = hIso->GetBinContent(binx, biny);
  double iso_error = hIso->GetBinError(binx, biny);
  ibin_iso = 10000*firstSection + 100*biny + binx; //assumes binx and biny < 100

  //Trigger weights
  trig_scale = 1.;
  //check if can fire a trigger
  if(trigger == kLowTrigger && ((year == k2016 && pt < 25.) || (year == k2017 && pt < 28.) || (year == k2018 && pt < 25.))) trigger = -1;
  else if(trigger == kHighTrigger && pt < 51.) trigger = -1;

  if(trigger >= 0) {
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
    //doesn't flip between years
    trig_scale = hTrigger->GetBinContent(hTrigger->GetXaxis()->FindBin(fabs(eta)), hTrigger->GetYaxis()->FindBin(pt));
  }

  double scale_factor = id_scale * iso_scale;
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " id_scale = " << id_scale << " +- " << id_error
              << " iso_scale = " << iso_scale << " +- " << iso_error
              << " trig_scale = " << trig_scale
              << " trigger = " << trigger
              << " firstSection = " << firstSection << std::endl;
  }
  //calculate the +- 1 sigma weights
  // weight_up   = scale_factor * (1. + sqrt(iso_error*iso_error/(iso_scale*iso_scale) + id_error*id_error/(id_scale*id_scale)));
  // weight_down = scale_factor * (1. - sqrt(iso_error*iso_error/(iso_scale*iso_scale) + id_error*id_error/(id_scale*id_scale)));
  weight_id = id_scale; weight_iso = iso_scale;

  weight_up_id    = weight_id  +  id_error;
  weight_down_id  = weight_id  -  id_error;
  weight_up_iso   = weight_iso + iso_error;
  weight_down_iso = weight_iso - iso_error;
}

double ParticleCorrections::MuonTriggerEff(double pt, double eta, int trigger, int year, float& data_eff, float& mc_eff) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }

  if(trigger != kLowTrigger && trigger != kHighTrigger) {
    std::cout << __func__ << ": Warning! Undefined trigger " << trigger << ", defaulting to low trigger\n";
    trigger = kLowTrigger;
  }

  if((pt < 25. && year != k2017) || (pt < 28. && year == k2017)) { //can't fire the trigger
    data_eff = 1.e-10;
    mc_eff   = 1.e-10;
    return 0.;
  }

  if(pt >= 120.) pt = 119.; //maximum pT for corrections
  else if(pt < 20.) pt = 20.; //minimum pT for corrections
  if(eta >= 2.4) eta = 2.39; //maximum eta for corrections
  else if(eta <= -2.4) eta = -2.39; //minimum eta for corrections

  double rand = fRnd->Uniform();
  bool firstSection = true; //whether this MC is for the first or second part of the data taking period
  mc_eff = 1.; data_eff = 1.;
  if(year == k2016)
    firstSection = rand <= 19.72/35.86;
  else if(year == k2018)
    firstSection = rand <= 8.98/59.59;
  data_eff = 1.; mc_eff = 1.;
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

  TH2F* hTriggerData = (trigger == kLowTrigger) ? muonLowTriggerEffMap[0][2*year + !firstSection] : muonHighTriggerEffMap[0][2*year + !firstSection];
  TH2F* hTriggerMC   = (trigger == kLowTrigger) ? muonLowTriggerEffMap[1][2*year + !firstSection] : muonHighTriggerEffMap[1][2*year + !firstSection];
  //axes doesn't flip between years
  data_eff = hTriggerData->GetBinContent(hTriggerData->GetXaxis()->FindBin(fabs(eta)), hTriggerData->GetYaxis()->FindBin(pt));
  mc_eff = hTriggerMC->GetBinContent(hTriggerMC->GetXaxis()->FindBin(fabs(eta)), hTriggerMC->GetYaxis()->FindBin(pt));

  //return the scale factor applied to MC to match Data
  double scale_factor = (mc_eff > 0.) ? data_eff/mc_eff : 0.;
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " pt = " << pt
              << " eta = " << eta
              << " data_eff = " << data_eff
              << " mc_eff = " << mc_eff
              << " scale_factor = " << scale_factor
              << " trigger = " << trigger
              << " firstSection = " << firstSection << std::endl;
  }

  return scale_factor;
}

void ParticleCorrections::ElectronWeight(double pt, double eta, int year, float& trigger_scale,
                                         float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                                         float& weight_rec, float& weight_up_rec, float& weight_down_rec, int& ibin_rec
                                         ) {
  weight_id  = 1.; weight_up_id  = 1.; weight_down_id  = 1.; ibin_id  = 0;
  weight_rec = 1.; weight_up_rec = 1.; weight_down_rec = 1.; ibin_rec = 0;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return;
  }

  double trig_pt = pt;
  if(year == k2016) {
    if(pt > 499.)     pt = 499.; //maximum pT for corrections
    else if(pt < 20.) pt = 20.; //minimum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2017) {
    if(pt > 499.)     pt = 499.; //maximum pT for corrections
    else if(pt < 20.) pt = 20.; //minimum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2018) {
    if(pt > 499.)     pt = 499.; //maximum pT for corrections
    else if(pt < 20.) pt = 20.; //minimum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  }

  TH2F* hID = electronIDMap[year];
  int binx = hID->GetXaxis()->FindBin(fabs(eta));
  int biny = hID->GetYaxis()->FindBin(pt);
  double id_scale = hID->GetBinContent(binx, biny);
  double id_error = hID->GetBinError(binx, biny);
  ibin_id = 100*biny + binx;

  TH2F* hReco = electronRecoMap[year];
  binx = hReco->GetXaxis()->FindBin(fabs(eta));
  biny = hReco->GetYaxis()->FindBin(pt);
  double reco_scale = hReco->GetBinContent(binx, biny);
  double reco_error = hReco->GetBinError(binx, biny);
  ibin_rec = 100*biny + binx;

  double eta_trig = eta;
  if(eta_trig > 2.4) eta_trig = 2.39;
  else if(eta_trig <- 2.4) eta_trig = -2.39;
  TH2F* hTrig = electronTriggerMap[year];
  trigger_scale = hTrig->GetBinContent(hTrig->GetXaxis()->FindBin(eta_trig), hTrig->GetYaxis()->FindBin(pt));

  //don't scale if can't fire the trigger
  if((trig_pt < 28. && year == k2016) || (trig_pt < 33. && year == k2017) || (trig_pt < 33. && year == k2018))
    trigger_scale = 1.;


  //FIXME: add pre-fire for 2017
  // double prefire_scale = electronPreFireMap[year];
  double vertex_scale = electronVertexMap[year];
  //FIXME: add vertex scale uncertainty

  double scale_factor = id_scale * reco_scale * vertex_scale;

  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " id_scale = " << id_scale
              << " reco_scale = " << reco_scale
              << " trig_scale = " << trigger_scale
              << " vertex_scale = " << vertex_scale
              << std::endl;
  }
  //calculate the +- 1 sigma weights
  // weight_up   = scale_factor * (1. + sqrt(id_error*id_error/(id_scale*id_scale) + reco_error*id_error/(reco_scale*reco_scale)));
  // weight_down = scale_factor * (1. - sqrt(id_error*id_error/(id_scale*id_scale) + reco_error*id_error/(reco_scale*reco_scale)));
  //FIXME: Currently just combining vertex scale and reco scale
  weight_id = id_scale; weight_rec = vertex_scale * reco_scale;
  weight_up_id    = id_scale + id_error;
  weight_down_id  = id_scale - id_error;
  weight_up_rec   = vertex_scale * (reco_scale + reco_error);
  weight_down_rec = vertex_scale * (reco_scale - reco_error);
}

double ParticleCorrections::ElectronTriggerEff(double pt, double eta, int year, float& data_eff, float& mc_eff) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }

  if(year == k2016) {
    if(pt > 499.)     pt = 499.; //maximum pT for corrections
    else if(pt < 20.) pt = 20.; //minimum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2017) {
    if(pt > 499.)     pt = 499.; //maximum pT for corrections
    else if(pt < 20.) pt = 20.; //minimum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2018) {
    if(pt > 499.)     pt = 499.; //maximum pT for corrections
    else if(pt < 20.) pt = 20.; //minimum pT for corrections
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  }

  double eta_trig = eta;
  if(eta_trig > 2.4) eta_trig = 2.39;
  else if(eta_trig <- 2.4) eta_trig = -2.39;
  TH2F* hTrigData = electronTriggerEffMap[0][year];
  data_eff = hTrigData->GetBinContent(hTrigData->GetXaxis()->FindBin(eta_trig), hTrigData->GetYaxis()->FindBin(pt));
  TH2F* hTrigMC = electronTriggerEffMap[1][year];
  mc_eff = hTrigMC->GetBinContent(hTrigMC->GetXaxis()->FindBin(eta_trig), hTrigMC->GetYaxis()->FindBin(pt));

  //can't fire a trigger if below the threshold
  if((year == k2016 && pt < 28.) || (year == k2017 && pt < 33.) || (year == k2018 && pt < 33.)) {
    data_eff = 1.;
    mc_eff = 1.;
  }
  double scale_factor = (mc_eff > 0.) ? data_eff / mc_eff : 0.;
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " data_eff = " << data_eff
              << " mc_eff = " << mc_eff
              << std::endl;
  }
  return scale_factor;
}

//correction for the tau ID
double ParticleCorrections::TauWeight(double pt, double eta, int genID, UChar_t antiJet, int year,
                                      float& up, float& down, int& ibin
                                      ) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }
  if(pt < 20.001) pt = 20.001; //has to be slightly greater than 20
  double scale_factor = 1.;
  up = 1.; down = 1.; ibin = 0;
  int antiJetIndex = (antiJet >= 50) ? 1 : 0; //tight or loose tau
  if(genID == 5) { //genuine tau
    scale_factor *= tauJetIDMap[year]->Eval(pt);
    up *= tauJetUpIDMap[year]->Eval(pt);
    down *= tauJetDownIDMap[year]->Eval(pt);
    ibin = 0; //Only 1 bin since up/down for all pT
  } else if(genID == 1) { //genuine electron -> tau
    int bin = tauEleIDMap[year]->FindBin(fabs(eta));
    scale_factor *= tauEleIDMap[year]->GetBinContent(bin);
    up = scale_factor + tauEleIDMap[year]->GetBinError(bin);
    down = scale_factor - tauEleIDMap[year]->GetBinError(bin);
    ibin = bin - 1;
  } else if(genID == 2) { //genuine muon -> tau
    int bin = tauMuIDMap[year]->FindBin(fabs(eta));
    scale_factor *= tauMuIDMap[year]->GetBinContent(bin);
    up = scale_factor + tauMuIDMap[year]->GetBinError(bin);
    down = scale_factor - tauMuIDMap[year]->GetBinError(bin);
    ibin = bin - 1;
  }
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " pt = " << pt
              << " eta = " << eta
              << " genID = " << genID
              << " antiJet = " << antiJetIndex
              << " scale_factor = " << scale_factor
              << std::endl;
  }
  return scale_factor;
}

//correction to the tau TLorentzVector
double ParticleCorrections::TauEnergyScale(double pt, double eta, int dm, int genID, UChar_t antiJet, int year, float& up, float& down, int& ibin) {
  double scale_factor = 1.;
  up = 1.; down = 1.; ibin = 0;
  double pt_low = 34.;
  double pt_high = 170.;
  //only defined for certain decay modes
  if(!(dm == 0 || dm == 1 || dm == 10 || dm == 11)) return scale_factor;

  int antiJetIndex = (antiJet >= 50) ? 1 : 0; //tight or loose tau

  /////////////////////
  // Genuine taus
  /////////////////////
  if(genID == 5) {

    //Use the low energy scale factor, but uncertainty using high and low
    int bin_low  = tauESLowMap [year]->FindBin(dm);
    int bin_high = tauESHighMap[year]->FindBin(dm);
    scale_factor = tauESLowMap [year]->GetBinContent(bin_low);
    ibin = bin_low - 1;
    if(pt < pt_low) { //use only the low uncertainty
      up   = scale_factor + tauESLowMap[year]->GetBinError(bin_low);
      down = scale_factor - tauESLowMap[year]->GetBinError(bin_low);
    } else if(pt < pt_high) { //interpolate the uncertainties
      double err_low  = tauESLowMap [year]->GetBinError(bin_low );
      double err_high = tauESHighMap[year]->GetBinError(bin_high);
      double err = err_low + (err_high - err_low)/(pt_high - pt_low)*(pt-pt_low);
      up   = scale_factor + err;
      down = scale_factor - err;
    } else { //use high pT map
      up   = scale_factor + tauESHighMap[year]->GetBinError(bin_high);
      down = scale_factor - tauESHighMap[year]->GetBinError(bin_high);
    }

  /////////////////////
  // Genuine electrons
  /////////////////////
  } else if(genID == 1 || genID == 3) { //genuine electron -> fake tau or tau -> e -> fake tau
    if(dm != 0 && dm != 1) return scale_factor;

    eta = fabs(eta);
    int point = 0;
    if(eta > 2.5)  return scale_factor;
    else if(eta < 1.5) point += 0; //barrel region
    else point += 2; //endcap region
    if(dm == 0) point += 0;
    else        point += 1;
    double x, y;
    tauFakeESMap[year]->GetPoint(point, x, y);
    scale_factor = y;
    ibin = point;
    up   = scale_factor + tauFakeESMap[year]->GetErrorYhigh(point);
    down = scale_factor - tauFakeESMap[year]->GetErrorYlow (point);

  /////////////////////
  // Genuine muons
  /////////////////////
  } else if(genID == 2 || genID == 4) { //genuine muon
    //no correction, +- 1% uncertainty
    scale_factor = 1.;
    up = 1.01;
    down = 0.99;
    ibin = 0;
  }

  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " GenID = " << genID
              << " dm = " << dm
              << " pt = " << pt
              << " eta = " << eta
              << " tight jet = " << antiJetIndex
              << " scale_factor = " << scale_factor
              << std::endl;
  }
  return scale_factor;
}


double ParticleCorrections::PhotonWeight(double pt, double eta, int year) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }

  if(year == k2016) {
    if(pt >= 500.) pt = 499.9; //maximum pT for corrections
    else if (pt < 20.) pt = 20.;
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2017) {
    if(pt >= 499.) pt = 498.9; //maximum pT for corrections
    else if (pt < 20.) pt = 20.;
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  } else if(year == k2018) {
    if(pt >= 500.) pt = 499.9; //maximum pT for corrections
    else if (pt < 20.) pt = 20.;
    if(eta >= 2.5) eta = 2.49; //maximum eta for corrections
    else if(eta <= -2.5) eta = -2.49; //minimum eta for corrections
  }

  TH2F* hID = photonIDMap[year];
  double id_scale = hID->GetBinContent(hID->GetXaxis()->FindBin(fabs(eta)), hID->GetYaxis()->FindBin(pt));

  double scale_factor = id_scale;
   if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " scale_factor = " << scale_factor
              << std::endl;
  }
 return scale_factor;
}

//Probability of passing b-tag WP in the MC
double ParticleCorrections::BTagMCProb(double pt, double eta, int jetFlavor, int year, int WP) {
  if(pt < 20.) pt = 20.;
  else if (pt > 999.) pt = 999.;
  if(fabs(eta) >= 2.5) return 0.; //can't tag high eta jets
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "WARNING! Unknown year in ParticleCorrections::" << __func__ << ": "
              << year << std::endl;
    return 0.;
  }
  if(WP != kLooseBTag && WP != kMediumBTag && WP != kTightBTag) {
    std::cout << "WARNING! In ParticleCorrections::" << __func__ << ": Unknown WP "
              << WP << std::endl;
    return 0.;
  }
  TH2F* h = 0;
  if     (abs(jetFlavor) == 5) h = bJetGenBEffMap[year][WP];
  else if(abs(jetFlavor) == 4) h = bJetGenCEffMap[year][WP];
  else if(abs(jetFlavor) <  4 || abs(jetFlavor) > 20) h = bJetGenLEffMap[year][WP];
  else {
    std::cout << "WARNING! In ParticleCorrections::" << __func__ << ": Unknown jet flavor "
              << jetFlavor << std::endl;
    return 0.;
  }

  if(!h) {
    std::cout << "WARNING! In ParticleCorrections::" << __func__ << ": No Histogram found for year "
              << year + 2016 << ", jetFlavor " << jetFlavor << ", WP " << WP << std::endl;
    return 0.;
  }

  int binx = h->GetXaxis()->FindBin(eta);
  int biny = h->GetYaxis()->FindBin(pt);
  double eff = h->GetBinContent(binx, biny);
  if(eff <= 0.) {
    std::cout << "WARNING! In ParticleCorrections::" << __func__ << ": Efficiency <= 0. = "
              << eff << std::endl;
    return 0.;
  }
  if(eff > 1.) eff = 1.;
  return eff;
}

//Probability of passing b-tag WP in data
double ParticleCorrections::BTagDataProb(double pt, double eta, int jetFlavor, int year, int WP) {
  if(pt < 20.) pt = 20.;
  if(fabs(eta) >= 2.5) return 1.; //can't tag high eta jets

  double scale_factor(1.), x(pt); //use x here, seems like it should be pT?
  if(jetFlavor == 5 || jetFlavor == 4) { //true b-jets and c-jets (since corrections the same)
    if(year == k2016) {
      if(WP == kLooseBTag)       scale_factor = 0.971065*((1.+(0.0100459*x))/(1.+(0.00975219*x)));
      else if(WP == kMediumBTag) scale_factor = 0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x)));
      else if(WP == kTightBTag)  scale_factor = 0.573021*((1.+(0.472221*x))/(1.+(0.27584*x)));
    } else if(year == k2017) {
      if(WP == kLooseBTag)       scale_factor = 1.04891*((1.+(0.0145976*x))/(1.+(0.0165274*x)));
      else if(WP == kMediumBTag) scale_factor = 0.991757*((1.+(0.0209615*x))/(1.+(0.0234962*x)));
      else if(WP == kTightBTag)  scale_factor = 0.908648*((1.+(0.00516407*x))/(1.+(0.00564675*x)));
    } else if(year == k2018) {
      if(WP == kLooseBTag)       scale_factor = 0.873139+(0.00420739*(log(x+19.)*(log(x+18.)*(3-(0.380932*log(x+18.))))));
      else if(WP == kMediumBTag) scale_factor = 1.0097+(-(2.89663e-06*(log(x+19.)*(log(x+18.)*(3-(-(110.381*log(x+18.))))))));
      else if(WP == kTightBTag)  scale_factor = 0.818896+(0.00682971*(log(x+19.)*(log(x+18.)*(3-(0.440998*log(x+18.))))));
    }
  } else { //other jets
    if(year == k2016) {
      if(WP == kLooseBTag)       scale_factor = 1.03726+0.000218815*x+-6.70561e-11*x*x+2.15167/x;
      else if(WP == kMediumBTag) scale_factor = 1.09149+3.31851e-05*x+2.34826e-07*x*x+-0.888846/x;
      else if(WP == kTightBTag)  scale_factor = 1.09741+-0.000871879*x+2.20312e-06*x*x+-1.53037e-09*x*x*x;
    } else if(year == k2017) {
      if(WP == kLooseBTag)       scale_factor = 1.43763+-0.000337048*x+2.22072e-07*x*x+-4.85489/x;
      else if(WP == kMediumBTag) scale_factor = 1.40779+-0.00094558*x+8.74982e-07*x*x+-4.67814/x;
      else if(WP == kTightBTag)  scale_factor = 0.952956+0.000569069*x+-1.88872e-06*x*x+1.25729e-09*x*x*x;
    } else if(year == k2018) {
      if(WP == kLooseBTag)       scale_factor = 1.61341+-0.000566321*x+1.99464e-07*x*x+-5.09199/x;
      else if(WP == kMediumBTag) scale_factor = 1.59373+-0.00113028*x+8.66631e-07*x*x+-1.10505/x;
      else if(WP == kTightBTag)  scale_factor = 1.77088+-0.00371551*x+5.86489e-06*x*x+-3.01178e-09*x*x*x;
    }
  }
  if(scale_factor < 0.01) scale_factor = 1.; //taken from btagSFProducer.py

  //Get probability of being tagged in MC
  double mc_prob = BTagMCProb(pt, eta, jetFlavor, year, WP);
  //Probaility in data is then P(MC) * SF
  double data_prob = mc_prob*scale_factor;
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " pt = " << pt
              << " eta = " << eta
              << " flavor = " << jetFlavor
              << " scale_factor = " << scale_factor
              << " mc_prob = " << mc_prob
              << " data_prob = " << data_prob
              << std::endl;
  }
  if(data_prob > 1.) data_prob = 1.;
  else if(data_prob < 0.) data_prob = 0.;

  return data_prob;
}

double ParticleCorrections::ZWeight(double pt, double mass, int year) {
  if(pt > 999.) pt = 999.;
  if(mass > 999.) mass = 999.;
  //FIXME: Add some sort of warning, suppress after a number of times thrown if no map
  if(!zWeightMap[year]) return 1.;
  int binx = zWeightMap[year]->GetXaxis()->FindBin(mass);
  int biny = zWeightMap[year]->GetYaxis()->FindBin(pt);
  double scale_factor = zWeightMap[year]->GetBinContent(binx,biny);
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " year = " << year
              << " scale_factor = " << scale_factor
              << std::endl;
  }
  if(scale_factor <= 0.) scale_factor = 1.;
  return scale_factor;
}

//get B-Tag WP cut value
double ParticleCorrections::BTagCut(int wp, int year) {
  double val = -1.;
  if(year == ParticleCorrections::k2016) {
    if(wp == ParticleCorrections::kLooseBTag)       val = 0.2217;
    else if(wp == ParticleCorrections::kMediumBTag) val = 0.6321;
    else if(wp == ParticleCorrections::kTightBTag)  val = 0.8953;
    else std::cout << "WARNING! Unrecognized B-Tag WP " << wp << " for "
                   << year << " configurations! Returning -1..." << std::endl;
  }
  if(year == ParticleCorrections::k2017) {
    if(wp == ParticleCorrections::kLooseBTag)       val = 0.1522;
    else if(wp == ParticleCorrections::kMediumBTag) val = 0.4941;
    else if(wp == ParticleCorrections::kTightBTag)  val = 0.8001;
    else std::cout << "WARNING! Unrecognized B-Tag WP " << wp << " for "
                   << year << " configurations! Returning -1..." << std::endl;
  }
  if(year == ParticleCorrections::k2018) {
    if(wp == ParticleCorrections::kLooseBTag)       val = 0.1241;
    else if(wp == ParticleCorrections::kMediumBTag) val = 0.4184;
    else if(wp == ParticleCorrections::kTightBTag)  val = 0.7527;
    else std::cout << "WARNING! Unrecognized B-Tag WP " << wp << " for "
                   << year << " configurations! Returning -1..." << std::endl;
  }
  return val;
}

//combine efficiencies for two triggers fired
float ParticleCorrections::CombineEfficiencies(float data_eff_1, float mc_eff_1, float data_eff_2, float mc_eff_2) {
  float eff_data = 1. - (1. - data_eff_1)*(1. - data_eff_2);
  float eff_mc   = 1. - (1. -   mc_eff_1)*(1. -   mc_eff_2);
  float scale_factor = eff_data / eff_mc;
  if(scale_factor <= 0. || fVerbose > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ParticleCorrections::" << __func__
              << " data_eff_1 = " << data_eff_1
              << " mc_eff_1 = " << mc_eff_1
              << " data_eff_2 = " << data_eff_2
              << " mc_eff_2 = " << mc_eff_2
              << " scale_factor = " << scale_factor
              << std::endl;
  }
  if(scale_factor <= 0.) scale_factor = 1.;
  return scale_factor;
}
