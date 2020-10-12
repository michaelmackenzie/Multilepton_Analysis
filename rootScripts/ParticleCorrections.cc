#include "ParticleCorrections.hh"


double ParticleCorrections::MuonWeight(double pt, double eta, int trigger, int year, float& trig_scale) {
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
  
  //axes flip between years for some reason
  int binx = (year != k2016) ? hID->GetXaxis()->FindBin(pt) : hID->GetXaxis()->FindBin(fabs(eta));	   
  int biny = (year != k2016) ? hID->GetYaxis()->FindBin(fabs(eta)) : hID->GetYaxis()->FindBin(pt);
  double id_scale = hID->GetBinContent(binx, biny);

  TH2F* hIso = muonIsoMap[2*year + !firstSection];
  binx = (year != k2016) ? hIso->GetXaxis()->FindBin(pt) : hIso->GetXaxis()->FindBin(fabs(eta));	   
  biny = (year != k2016) ? hIso->GetYaxis()->FindBin(fabs(eta)) : hIso->GetYaxis()->FindBin(pt);
  double iso_scale = hIso->GetBinContent(binx, biny);

  //Trigger weights
  trig_scale = 1.;
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
  if(scale_factor <= 0.)
    std::cout << "Warning! ParticleCorrections::" << __func__
	      << " scale factor <= 0, year = " << year
	      << " id_scale = " << id_scale
	      << " iso_scale = " << iso_scale
	      << " trig_scale = " << trig_scale
	      << " trigger = " << trigger
	      << " firstSection = " << firstSection << std::endl;

  return scale_factor;
}

double ParticleCorrections::ElectronWeight(double pt, double eta, int year, float& trigger_scale) {
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

  TH2F* hTrig = electronTriggerMap[year];
  trigger_scale = hTrig->GetBinContent(hTrig->GetXaxis()->FindBin(eta), hTrig->GetYaxis()->FindBin(pt));

  //FIXME: add pre-fire for 2017
  // double prefire_scale = electronPreFireMap[year];
  double vertex_scale = electronVertexMap[year];
  
  double scale_factor = id_scale * reco_scale * vertex_scale;
  return scale_factor;
}

//correction for the tau ID
double ParticleCorrections::TauWeight(double pt, double eta, int genID, int year, double& up, double& down) {
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return -1.;
  }

  double scale_factor = 1.;
  up = 1.; down = 1.;
  if(genID == 5) { //genuine tau
    scale_factor *= tauJetIDMap[year]->Eval(pt);
    up *= tauJetUpIDMap[year]->Eval(pt);
    down *= tauJetDownIDMap[year]->Eval(pt);
  } else if(genID == 1) { //genuine electron -> tau
    scale_factor *= tauEleIDMap[year]->GetBinContent(tauEleIDMap[year]->FindBin(fabs(eta)));
  } else if(genID == 2) { //genuine muon -> tau
    scale_factor *= tauMuIDMap[year]->GetBinContent(tauMuIDMap[year]->FindBin(fabs(eta)));
  }
  return scale_factor;
}

//correction to the tau TLorentzVector
double ParticleCorrections::TauEnergyScale(double pt, double eta, int dm, int genID, int year, double& up, double& down) {
  double scale_factor = 1.;
  up = 1.; down = 1.;
  if(dm > 12 || dm < 0) return scale_factor;
  if(genID == 5) {
    if(year == k2016) {
      if(pt < 34.)  return scale_factor;
      else if(pt < 170.) {
	scale_factor = tauESLowMap[year]->GetBinContent(tauESLowMap[year]->FindBin(dm));
      } else {
	scale_factor = tauESHighMap[year]->GetBinContent(tauESHighMap[year]->FindBin(dm));
      }
    }
  } else if(genID == 1 || genID == 3) { //genuine electron -> fake tau or tau -> e -> fake tau
    if(dm != 0 && dm != 1) return scale_factor;
    if(year == k2016) {
      eta = fabs(eta);
      int point = 0;
      if(eta > 2.5)  return scale_factor;
      else if(eta < 1.5) point += 0; //barrel region
      else point += 2; //endcap region
      if(dm == 0) point += 0;
      else        point += 1;
      double x, y;
      tauFakeESMap[year]->GetPoint(point, x, y);
      scale_factor *= y;
      up *= y + tauFakeESMap[year]->GetErrorYhigh(point);
      down *= y - tauFakeESMap[year]->GetErrorYlow(point);            
    }
  }
  return scale_factor;
}

double ParticleCorrections::BTagWeight(double pt, double eta, int jetFlavor, int year, int WP) {
  if(pt < 20.) pt = 20.;
  if(fabs(eta) > 2.4) return 1.; //can't tag high eta jets
  
  double scale_factor(1.), x(pt); //use x here
  if(jetFlavor == 5) { //true b-jets
    if(year == k2016) {
      if(WP == kLooseBTag)       scale_factor = 0.971065*((1.+(0.0100459*x))/(1.+(0.00975219*x)));
      else if(WP == kMediumBTag) scale_factor = 0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x)));
      else if(WP == kTightBTag)  scale_factor = 0.573021*((1.+(0.472221*x))/(1.+(0.27584*x)));
    }
  } else {
    if(year == k2016) {
      if(WP == kLooseBTag)       scale_factor = 0.971065*((1.+(0.0100459*x))/(1.+(0.00975219*x)));
      else if(WP == kMediumBTag) scale_factor = 0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x)));
      else if(WP == kTightBTag)  scale_factor = 0.573021*((1.+(0.472221*x))/(1.+(0.27584*x)));
    }
  }
  return scale_factor;
}

double ParticleCorrections::ZWeight(double pt, double mass, int year) {
  if(pt > 999.) pt = 999.;
  if(mass > 999.) mass = 999.;
  int binx = zWeightMap[year]->GetXaxis()->FindBin(mass);
  int biny = zWeightMap[year]->GetYaxis()->FindBin(pt);
  double scale_factor = zWeightMap[year]->GetBinContent(binx,biny);
  if(scale_factor <= 0.) scale_factor = 1.;
  return scale_factor;
}
