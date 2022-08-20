#include "interface/PrefireWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
PrefireWeight::PrefireWeight() {
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  TFile* f;

  //Get jet maps
  f = TFile::Open(Form("%s/L1prefiring_jetpt_2016BtoH.root", path.Data()), "READ"); //measured efficiencies
  if(f) {
    hists_[kJet + 2016] = (TH2*) f->Get("L1prefiring_jetpt_2016BtoH");
    if(!hists_[kJet + 2016]) {
      printf("!!! %s: Prefire jet probability map for 2016 not found!\n", __func__);
    } else {
      hists_[kJet + 2016]->SetDirectory(0);
    }
    f->Close();
    delete f;
  }
  f = TFile::Open(Form("%s/L1prefiring_jetpt_2017BtoF.root", path.Data()), "READ"); //measured efficiencies
  if(f) {
    hists_[kJet + 2017] = (TH2*) f->Get("L1prefiring_jetpt_2017BtoF");
    if(!hists_[kJet + 2017]) {
      printf("!!! %s: Prefire jet probability map for 2017 not found!\n", __func__);
    } else {
      hists_[kJet + 2017]->SetDirectory(0);
    }
    f->Close();
    delete f;
  }

  //Get photon maps
  f = TFile::Open(Form("%s/L1prefiring_photonpt_2016BtoH.root", path.Data()), "READ"); //measured efficiencies
  if(f) {
    hists_[kPhoton + 2016] = (TH2*) f->Get("L1prefiring_photonpt_2016BtoH");
    if(!hists_[kPhoton + 2016]) {
      printf("!!! %s: Prefire photon probability map for 2016 not found!\n", __func__);
    } else {
      hists_[kPhoton + 2016]->SetDirectory(0);
    }
    f->Close();
    delete f;
  }
  f = TFile::Open(Form("%s/L1prefiring_photonpt_2017BtoF.root", path.Data()), "READ"); //measured efficiencies
  if(f) {
    hists_[kPhoton + 2017] = (TH2*) f->Get("L1prefiring_photonpt_2017BtoF");
    if(!hists_[kPhoton + 2017]) {
      printf("!!! %s: Prefire photon probability map for 2017 not found!\n", __func__);
    } else {
      hists_[kPhoton + 2017]->SetDirectory(0);
    }
    f->Close();
    delete f;
  }
}

//-------------------------------------------------------------------------------------------------------------------------
PrefireWeight::~PrefireWeight() {
  for(std::pair<int, TH2*> val : hists_) {if(val.second) delete val.second;}
}

//-------------------------------------------------------------------------------------------------------------------------
float PrefireWeight::GetProbability(const int year, float jetpt, float jeteta, int base) {
  float prob(0.);
  jetpt = std::max(20.f, std::min(499.9f, jetpt));
  jeteta = std::max(-4.9f, std::min(4.9f, jeteta));
  TH2* h = hists_[base + year];
  const int binx = std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(jeteta)));
  const int biny = std::max(1, std::min(h->GetNbinsY(), h->GetYaxis()->FindBin(jetpt )));
  prob = h->GetBinContent(binx, biny);
  if(prob < 0. || prob > 1.) {
    std::cout << "!!! PrefireWeight::" << __func__ << ": Warning! Probability not between 0 and 1 = " << prob
              << " jetpt = " << jetpt << " jeteta = " << jeteta << std::endl;
    prob = 0.; //probability of prefire = 0 --> weight = 1 in default case
  }
  return prob;
}

//-------------------------------------------------------------------------------------------------------------------------
float PrefireWeight::GetJetWeight(int year, int njets, float* jetspt, float* jetseta) {
  float weight(1.);
  if(year == 2018) return 1.; //no correction in 2018
  //accepted jets
  for(int jet = 0; jet < njets; ++jet) {
    if(jetspt[jet] < 20.) continue; //below pT threshold
    const float prob = GetProbability(year, jetspt[jet], jetseta[jet], kJet);
    weight *= (1. - prob); //re-weight by product of probabilities to not pre-fire
  }
  if(weight <= 1.e-4) {
    std::cout << "PrefireWeight::" << __func__ << ": Warning! Very small weight = " << weight
              << ", returning 1" << std::endl;
    return 1.f;
  }
  return weight;
}

//-------------------------------------------------------------------------------------------------------------------------
float PrefireWeight::GetPhotonWeight(int year, int nphotons, float* photonspt, float* photonseta) {
  float weight(1.);
  if(year == 2018) return 1.; //no correction in 2018
  //accepted photons
  for(int photon = 0; photon < nphotons; ++photon) {
    if(photonspt[photon] < 20.) continue; //below pT threshold
    const float prob = GetProbability(year, photonspt[photon], photonseta[photon], kPhoton);
    weight *= (1. - prob); //re-weight by product of probabilities to not pre-fire
  }
  if(weight <= 1.e-4) {
    std::cout << "PrefireWeight::" << __func__ << ": Warning! Very small weight = " << weight
              << ", returning 1" << std::endl;
    return 1.f;
  }
  return weight;
}
