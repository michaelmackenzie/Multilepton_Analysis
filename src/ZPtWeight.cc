#include "interface/ZPtWeight.hh"

using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------------------------------
ZPtWeight::ZPtWeight(TString Name, int Mode, int seed) {
  Name_ = Name;
  Mode_ = Mode;
  TFile* f = 0;
  TFile* fsys = 0;
  std::vector<int> years = {2016, 2017, 2018};
  rnd_ = new TRandom3(seed);
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    f = TFile::Open(Form("%s/z_pt_vs_m_scales_mumu_%i.root", path.Data(), year), "READ");
    fsys = TFile::Open(Form("%s/z_pt_vs_m_scales_ee_%i.root", path.Data(), year), "READ");
    if(!f) continue;
    hZPtScales_[year] = (TH2*) f->Get("hGenRatio");
    hZPtScales_[year]->SetName(Form("%s_mumu_%i", hZPtScales_[year]->GetName(), year));
    hZPtScales_[year]->SetDirectory(0);
    hZPtRecoScales_[year] = (TH2*) f->Get("hRatioNorm");
    hZPtRecoScales_[year]->SetName(Form("%s_mumu_%i", hZPtRecoScales_[year]->GetName(), year));
    hZPtRecoScales_[year]->SetDirectory(0);
    f->Close();
    delete f;
    if(fsys) {
      hZPtSysScales_[year] = (TH2*) fsys->Get("hGenRatio");
      hZPtSysScales_[year]->SetName(Form("%s_ee_%i", hZPtSysScales_[year]->GetName(), year));
      hZPtSysScales_[year]->SetDirectory(0);
      hZPtSysRecoScales_[year] = (TH2*) fsys->Get("hRatioNorm");
      hZPtSysRecoScales_[year]->SetName(Form("%s_ee_%i", hZPtSysRecoScales_[year]->GetName(), year));
      hZPtSysRecoScales_[year]->SetDirectory(0);
      fsys->Close();
      delete fsys;
    }
    //initialize random systematic uncertainty map
    if(hZPtScales_[year]) {
      isShiftedUp_[year] = {};
      for(int xbin = 1; xbin <= hZPtScales_[year]->GetNbinsX(); ++xbin) {
        isShiftedUp_[year][xbin-1] = {};
        for(int ybin = 1; ybin <= hZPtScales_[year]->GetNbinsY(); ++ybin) {
          isShiftedUp_[year][xbin-1][ybin-1] = 0.5 < rnd_->Uniform();
        }
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------
ZPtWeight::~ZPtWeight() {
  if(rnd_) delete rnd_;
  for(std::pair<int, TH2*> val : hZPtScales_       ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : hZPtRecoScales_   ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : hZPtSysScales_    ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : hZPtSysRecoScales_) {if(val.second) delete val.second;}
}

//--------------------------------------------------------------------------------------------------------------------------------------
float ZPtWeight::GetWeight(int year, float pt, float mass, bool doReco, float& up, float& down, float& sys) {
  float weight = 1.; up = 1.; down = 1.; sys = 1.;
  TH2* h = (doReco > 0) ? hZPtRecoScales_[year] : hZPtScales_[year];
  TH2* hsys = (doReco > 0) ? hZPtSysRecoScales_[year] : hZPtSysScales_[year];
  if(!h) {
    std::cout << Name_.Data() << " ZPtWeight::" << __func__ << " WARNING! Z pT weights not defined for year = " << year
              << " and doReco = " << doReco << std::endl;
    return weight;
  }
  //ensure the values are within the bounds
  const int binx = std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(mass)));
  const int biny = std::max(1, std::min(h->GetNbinsY(), h->GetYaxis()->FindBin(pt  )));
  weight = h->GetBinContent(binx, biny);
  float sys_weight = (hsys) ? hsys->GetBinContent(binx, biny) : weight;

  const float min_weight = 1.e-6; //minimum weight allowed
  weight     = std::max(min_weight, weight);
  sys_weight = std::max(min_weight, sys_weight);

  //if using systematic weight set, set up to that weight, down to the same difference but in the opposite direction
  up     = (Mode_ > 0) ? sys_weight             : weight + h->GetBinError(binx, biny);
  down   = (Mode_ > 0) ? 2.*weight - sys_weight : weight - h->GetBinError(binx, biny);
  down   = std::max(min_weight, down);
  if(Mode_ > 0) sys = sys_weight;
  else sys = (isShiftedUp_[year][binx-1][biny-1]) ? up : down;

  if(!std::isfinite(weight) || weight <= 0.) {
    std::cout << Name_.Data() << " ZPtWeight::" << __func__ << " WARNING! Z pT weight undefined or <= 0 = " << weight << " (pt, mass) = ("
              << pt << ", " << mass << ") using doReco = " << doReco << " and year = "<< year << "! Returning 1...\n";
    return 1.;
  }
  return weight;
}
