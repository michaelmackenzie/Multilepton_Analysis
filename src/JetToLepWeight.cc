#include "interface/JetToLepWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
JetToLepWeight::JetToLepWeight(TString selection) {
  TFile* f = 0;
  std::vector<int> years = {2016, 2017, 2018};
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    f = TFile::Open(Form("%s/jet_to_lep_%s_%i.root", path.Data(), selection.Data(), year), "READ");
    if(f) {
      //Get Data histogram
      histsData_[year] = (TH2D*) f->Get(Form("hdata_eff_2d"));
      if(!histsData_[year]) {
        std::cout << "JetToLepWeight::JetToLepWeight: Warning! No Data histogram found for year = " << year << std::endl;
      } else {
        histsData_[year]->SetName(Form("%s_%s_%i", histsData_[year]->GetName(), selection.Data(), year));
        histsData_[year]->SetDirectory(0);
      }
      f->Close();
      delete f;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
JetToLepWeight::~JetToLepWeight() {
  for(unsigned i = 2016; i <= 2018; ++i) {if(histsData_[i]) delete histsData_[i];}
}

//-------------------------------------------------------------------------------------------------------------------------
//Get scale factor for Data
float JetToLepWeight::GetDataFactor(int year, float pt, float eta) {
  TH2D* h = histsData_[year];

  //check if histogram is found
  if(!h) {
    std::cout << "JetToLepWeight::" << __func__ << ": Undefined histogram for year = " << year << std::endl;
    return 1.;
  }

  return GetFactor(h, pt, eta, year);
}

//-------------------------------------------------------------------------------------------------------------------------
float JetToLepWeight::GetFactor(TH2D* h, float pt, float eta, int year) {
  //ensure within kinematic regions
  eta = std::fabs(eta);
  if(pt > 999.) pt = 999.;
  else if(pt < 20.) pt = 20.;
  if(eta > 2.5) eta = 2.49;

  //get bin value
  const int binx      = h->GetXaxis()->FindBin(pt);
  const int biny      = h->GetYaxis()->FindBin(eta);
  const float eff_bin = h->GetBinContent(binx, biny);
  const double pt_bin = h->GetXaxis()->GetBinCenter(binx);
  bool leftofcenter = h->GetXaxis()->GetBinCenter(binx) > pt; //check which side of bin center we are
  //if at boundary, use interpolation from bin inside hist
  if(binx == 1)  leftofcenter = false;
  if(binx == h->GetNbinsX())  leftofcenter = true;
  const int binx_off = binx-(2*leftofcenter-1);
  const float eff_off = h->GetBinContent(binx_off, biny);
  const double pt_off = h->GetXaxis()->GetBinCenter(binx_off);
  //linear interpolation between the bin centers
  float eff = eff_bin + (eff_off - eff_bin)*(pt - pt_bin)/(pt_off - pt_bin);

  //check if allowed value
  if(eff < 0.) {
    std::cout << "JetToLepWeight::" << __func__ << ": Warning! MC Eff < 0 = " << eff
              << " pt = " << pt << " eta = " << eta
              << " year = " << year << std::endl;
    eff = 0.000001;
  } else if(eff >= 1.) {
    std::cout << "JetToLepWeight::" << __func__ << ": Warning! MC Eff >= 1 = " << eff
              << " pt = " << pt << " eta = " << eta
              << " year = " << year << std::endl;
    eff = 1.;
  } else {
    //write as scale factor instead of efficiency
    //eff_0 = a / (a+b) = 1 / (1 + 1/eff_p) = eff_p / (eff_p + 1)
    // --> eff_p = eff_0 / (1 - eff_0)
    eff = eff / (1. - eff);
  }

  return eff;
}
