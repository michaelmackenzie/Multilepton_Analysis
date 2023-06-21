#include "interface/SignalZMixingWeight.hh"

using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------------------------------
SignalZMixingWeight::SignalZMixingWeight() {

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";

  TFile* f = nullptr;
  const std::vector<int> years = {2016, 2017, 2018};

  for(int year : years) {
    hScales_       [year] = nullptr;
    //s4 = 4th (current) signal iteration
    f = TFile::Open(Form("%s/signal_zgamma_mixing_correction_s4_%i.root", path.Data(), year), "READ");
    if(!f) continue;
    hScales_[year] = (TH2*) f->Get("hratio");
    if(!hScales_[year]) {
      std::cout << "SignalZMixingWeight: No correction histogram found for year " << year << std::endl;
      continue;
    }
    hScales_[year]->SetName(Form("%s_%i", hScales_[year]->GetName(), year));
    hScales_[year]->SetDirectory(0);
    f->Close();
    delete f;
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------
SignalZMixingWeight::~SignalZMixingWeight() {
  for(std::pair<int, TH2*> val : hScales_) {if(val.second) {delete val.second;}}
}

//--------------------------------------------------------------------------------------------------------------------------------------
float SignalZMixingWeight::GetWeight(int year, float pt, float mass, float& up, float& down) {
  float weight(1.f);
  up = 1.f; down = 1.f;
  if(hScales_.find(year) == hScales_.end()) {
    std::cout << "SignalZMixingWeight::" << __func__ << " WARNING! Z pT weights not defined for year = " << year << std::endl;
    return weight;
  }
  TH2* h = hScales_[year];
  if(!h) {
    std::cout << "SignalZMixingWeight::" << __func__ << " WARNING! Z pT weights not defined for year = " << year << std::endl;
    return weight;
  }

  //ensure the values are within the bounds
  const int binx = std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(mass)));
  const int biny = std::max(1, std::min(h->GetNbinsY(), h->GetYaxis()->FindBin(pt  )));
  weight = h->GetBinContent(binx, biny);
  float err = h->GetBinError(binx, biny);

  if(!std::isfinite(weight) || weight <= 0.f) {
    std::cout << "SignalZMixingWeight::" << __func__ << " WARNING! Z pT weight undefined or <= 0 = " << weight << " (pt, mass) = ("
              << pt << ", " << mass << ") using year = "<< year << "! Returning 1...\n";
    return 1.f;
  }
  const float min_weight = 1.e-3; //minimum weight allowed
  const float max_weight = 1.e1 ; //maximum weight allowed
  weight = std::min(max_weight, std::max(min_weight, weight));
  up     = std::min(max_weight, std::max(min_weight, weight+err)); //take the up/down as the stat. bin uncertainty
  down   = std::min(max_weight, std::max(min_weight, weight-err));

  return weight;
}
