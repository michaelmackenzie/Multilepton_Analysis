#include "interface/ParticleCorrections.hh"
using namespace CLFV;


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

  //FIXME: run era should be fixed for the entire event, not allowed to change between muons in the same event
  const double rand = fRnd->Uniform();
  bool firstSection = true; //whether this MC is for the first or second part of the data taking period
  if(year == k2016)
    firstSection = (rand < 19.72/35.86);
  else if(year == k2018)
    firstSection = (rand < 8.98/59.59);

  TH2F* hID = muonIDMap[2*year + !firstSection];

  //axes flip between years for some reason
  const double xvar = (year != 2016) ? pt : eta;
  const double yvar = (year != 2016) ? std::fabs(eta) : pt;
  int binx = std::max(1, std::min(hID->GetNbinsX(), hID->GetXaxis()->FindBin(xvar)));
  int biny = std::max(1, std::min(hID->GetNbinsY(), hID->GetYaxis()->FindBin(yvar)));
  const double id_scale = hID->GetBinContent(binx, biny);
  const double id_error = hID->GetBinError(binx, biny);
  ibin_id = 10000 * firstSection + 100*biny + binx; //assumes binx and biny < 100

  TH2F* hIso = muonIsoMap[2*year + !firstSection];

  binx = std::max(1, std::min(hIso->GetNbinsX(), hIso->GetXaxis()->FindBin(xvar)));
  biny = std::max(1, std::min(hIso->GetNbinsY(), hIso->GetYaxis()->FindBin(yvar)));
  const double iso_scale = hIso->GetBinContent(binx, biny);
  const double iso_error = hIso->GetBinError(binx, biny);
  ibin_iso = 10000*firstSection + 100*biny + binx; //assumes binx and biny < 100

  //Trigger weights
  trig_scale = 1.;
  //check if can fire a trigger
  if(trigger == kLowTrigger && ((year == k2016 && pt < 25.) || (year == k2017 && pt < 28.) || (year == k2018 && pt < 25.))) trigger = -1;
  else if(trigger == kHighTrigger && pt < 51.) trigger = -1;

  if(trigger >= 0) {
    // if(year == k2016) {
    //   if(trigger == kLowTrigger && pt < 26.) pt = 26.;
    //   else if(trigger == kHighTrigger && pt < 52.) pt = 52.;
    // } else if (year == k2017) {
    //   if(trigger == kLowTrigger && pt < 29.) pt = 29.;
    //   else if(trigger == kHighTrigger && pt < 52.) pt = 52.;
    // } else if (year == k2018) {
    //   if(trigger == kLowTrigger && pt < 26.) pt = 26.;
    //   else if(trigger == kHighTrigger && pt < 52.) pt = 52.;
    // }
    TH2F* hTrigger = (trigger == kLowTrigger) ? muonLowTriggerMap[2*year + !firstSection] : muonHighTriggerMap[2*year + !firstSection] ;
    //doesn't flip between years
    binx = std::max(1, std::min(hTrigger->GetNbinsX(), hTrigger->GetXaxis()->FindBin(std::fabs(eta))));
    biny = std::max(1, std::min(hTrigger->GetNbinsY(), hTrigger->GetYaxis()->FindBin(pt)));
    trig_scale = hTrigger->GetBinContent(binx, biny);
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
  data_eff = hTriggerData->GetBinContent(hTriggerData->GetXaxis()->FindBin(std::fabs(eta)), hTriggerData->GetYaxis()->FindBin(pt));
  mc_eff = hTriggerMC->GetBinContent(hTriggerMC->GetXaxis()->FindBin(std::fabs(eta)), hTriggerMC->GetYaxis()->FindBin(pt));

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
  double id_scale = hID->GetBinContent(hID->GetXaxis()->FindBin(std::fabs(eta)), hID->GetYaxis()->FindBin(pt));

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
  if(std::fabs(eta) >= 2.5) return 0.; //can't tag high eta jets
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
  if(std::fabs(eta) >= 2.5) return 1.; //can't tag high eta jets

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
