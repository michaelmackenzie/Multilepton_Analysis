#include "interface/ZPtWeight.hh"

using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------------------------------
// Mode options:
// 0: Use bin errors as the systematic uncertainty
// 1: Use the ee region measurement as the systematic uncertainty
ZPtWeight::ZPtWeight(TString Name, int Mode) {
  Name_ = Name;
  Mode_ = Mode;
  TFile* f = 0;
  TFile* fsys = 0;
  const std::vector<int> years = {2016, 2017, 2018};
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    hZPtScales_       [year] = nullptr;
    hZPtRecoScales_   [year] = nullptr;
    hZPtSysScales_    [year] = nullptr;
    hZPtSysRecoScales_[year] = nullptr;
    f = TFile::Open(Form("%s/z_pt_vs_m_scales_mumu_%i.root", path.Data(), year), "READ");
    fsys = TFile::Open(Form("%s/z_pt_vs_m_scales_ee_%i.root", path.Data(), year), "READ");
    if(!f) continue;
    hZPtScales_[year] = (TH2*) f->Get("hGenRatio");
    hZPtScales_[year]->SetName(Form("%s_%s_mumu_%i", Name.Data(), hZPtScales_[year]->GetName(), year));
    hZPtScales_[year]->SetDirectory(0);
    hZPtRecoScales_[year] = (TH2*) f->Get("hRatioNorm");
    hZPtRecoScales_[year]->SetName(Form("%s_%s_mumu_%i", Name.Data(), hZPtRecoScales_[year]->GetName(), year));
    hZPtRecoScales_[year]->SetDirectory(0);
    f->Close();
    delete f;
    if(fsys) {
      hZPtSysScales_[year] = (TH2*) fsys->Get("hGenRatio");
      hZPtSysScales_[year]->SetName(Form("%s_%s_ee_%i", Name.Data(), hZPtSysScales_[year]->GetName(), year));
      hZPtSysScales_[year]->SetDirectory(0);
      hZPtSysRecoScales_[year] = (TH2*) fsys->Get("hRatioNorm");
      hZPtSysRecoScales_[year]->SetName(Form("%s_%s_ee_%i", Name.Data(), hZPtSysRecoScales_[year]->GetName(), year));
      hZPtSysRecoScales_[year]->SetDirectory(0);
      fsys->Close();
      delete fsys;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------
ZPtWeight::~ZPtWeight() {
  for(std::pair<int, TH2*> val : hZPtScales_       ) {if(val.second) {delete val.second;}}
  for(std::pair<int, TH2*> val : hZPtRecoScales_   ) {if(val.second) {delete val.second;}}
  for(std::pair<int, TH2*> val : hZPtSysScales_    ) {if(val.second) {delete val.second;}}
  for(std::pair<int, TH2*> val : hZPtSysRecoScales_) {if(val.second) {delete val.second;}}
}

//--------------------------------------------------------------------------------------------------------------------------------------
float ZPtWeight::GetWeight(int year, float pt, float mass, bool doReco, float& up, float& down, float& sys) {
  float weight = 1.f; up = 1.f; down = 1.f; sys = 1.f;
  auto scales = (doReco) ? hZPtRecoScales_ : hZPtScales_;
  if(scales.find(year) == scales.end()) {
    std::cout << Name_.Data() << " ZPtWeight::" << __func__ << " WARNING! Z pT weights not defined for year = " << year
              << " and doReco = " << doReco << std::endl;
    return weight;
  }
  TH2* h    = scales[year];
  if(!h) {
    std::cout << Name_.Data() << " ZPtWeight::" << __func__ << " WARNING! Z pT weights not defined for year = " << year
              << " and doReco = " << doReco << std::endl;
    return weight;
  }
  TH2* hsys = nullptr;
  auto sys_scales = (doReco) ? hZPtSysRecoScales_ : hZPtSysScales_;
  if(sys_scales.find(year) != sys_scales.end()) {
    hsys = sys_scales[year];
  }
  //ensure the values are within the bounds
  const int binx = std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(mass)));
  const int biny = std::max(1, std::min(h->GetNbinsY(), h->GetYaxis()->FindBin(pt  )));
  weight = h->GetBinContent(binx, biny);
  float sys_weight = (hsys) ? hsys->GetBinContent(binx, biny) : weight;

  const float min_weight = 1.e-6; //minimum weight allowed
  const float max_weight = 10.; //maximum weight allowed
  weight     = std::min(max_weight, std::max(min_weight, weight));
  sys_weight = std::min(max_weight, std::max(min_weight, sys_weight));

  //if using systematic weight set, set up to that weight, down to the same difference but in the opposite direction
  up     = (Mode_ > 0) ? sys_weight             : weight + h->GetBinError(binx, biny);
  down   = (Mode_ > 0) ? 2.*weight - sys_weight : weight - h->GetBinError(binx, biny);
  up     = std::min(max_weight, std::max(min_weight, up  ));
  down   = std::min(max_weight, std::max(min_weight, down));
  sys    = (Mode_ > 0) ? sys_weight : up; //just set to up if using bin errors

  if(!std::isfinite(weight) || weight <= 0.) {
    std::cout << Name_.Data() << " ZPtWeight::" << __func__ << " WARNING! Z pT weight undefined or <= 0 = " << weight << " (pt, mass) = ("
              << pt << ", " << mass << ") using doReco = " << doReco << " and year = "<< year << "! Returning 1...\n";
    return 1.;
  }
  return weight;
}
