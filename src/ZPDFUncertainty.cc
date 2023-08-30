#include "interface/ZPDFUncertainty.hh"

using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------------------------------
ZPDFUncertainty::ZPDFUncertainty() {

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";

  TFile* f = nullptr;
  const std::vector<int> years = {2016, 2017, 2018};

  for(int year : years) {
    hScaleR_[year] = nullptr;
    hScaleF_[year] = nullptr;
    hPDF_   [year] = nullptr;
    if(year == 2017) continue; //No 2017 MC dataset to derive LO corrections using

    /////////////////////////////
    //Get the PDF weights

    //define which PDF index set to use
    const int pdf_set = -1;
    f = TFile::Open(Form("%s/z_pdf_shift_p%s%i_%i.root", path.Data(), (pdf_set) < 0 ? "n" : "", std::abs(pdf_set), year), "READ");
    if(!f) continue;
    hPDF_[year] = (TH2*) f->Get("hratio");
    if(!hPDF_[year]) {
      std::cout << "ZPDFUncertainty: No PDF uncertainty histogram found for year " << year << std::endl;
    } else {
      hPDF_[year]->SetName(Form("%s_pdf_%i", hPDF_[year]->GetName(), year));
      hPDF_[year]->SetDirectory(0);
    }
    f->Close();
    delete f;

    /////////////////////////////
    //Get the Scale weights

    //define which renormalization scale index set to use
    const int renorm_scale_set = -1002;
    f = TFile::Open(Form("%s/z_pdf_shift_p%s%i_%i.root", path.Data(), (renorm_scale_set) < 0 ? "n" : "", std::abs(renorm_scale_set), year), "READ");
    if(!f) continue;
    hScaleR_[year] = (TH2*) f->Get("hratio");
    if(!hScaleR_[year]) {
      std::cout << "ZPDFUncertainty: No renormalization scale uncertainty histogram found for year " << year << std::endl;
    } else {
      hScaleR_[year]->SetName(Form("%s_renorm_scale_%i", hScaleR_[year]->GetName(), year));
      hScaleR_[year]->SetDirectory(0);
    }
    f->Close();
    delete f;

    //define which factorization scale index set to use
    const int factor_scale_set = -1003;
    f = TFile::Open(Form("%s/z_pdf_shift_p%s%i_%i.root", path.Data(), (factor_scale_set) < 0 ? "n" : "", std::abs(factor_scale_set), year), "READ");
    if(!f) continue;
    hScaleF_[year] = (TH2*) f->Get("hratio");
    if(!hScaleF_[year]) {
      std::cout << "ZPDFUncertainty: No factorization scale uncertainty histogram found for year " << year << std::endl;
    } else {
      hScaleF_[year]->SetName(Form("%s_factor_scale_%i", hScaleF_[year]->GetName(), year));
      hScaleF_[year]->SetDirectory(0);
    }
    f->Close();
    delete f;
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------
ZPDFUncertainty::~ZPDFUncertainty() {
  for(std::pair<int, TH2*> val : hPDF_   ) {if(val.second) {delete val.second;}}
  for(std::pair<int, TH2*> val : hScaleR_) {if(val.second) {delete val.second;}}
  for(std::pair<int, TH2*> val : hScaleF_) {if(val.second) {delete val.second;}}
}

//--------------------------------------------------------------------------------------------------------------------------------------
float ZPDFUncertainty::GetPDFWeight(int year, float z_pt, float z_eta, float z_mass) {
  float weight = 1.f;
  if(year == 2017) year = 2018; //Use 2018 weights in 2017 due to no LO MC sample being available
  if(hPDF_.find(year) == hPDF_.end()) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z PDF weights not defined for year = " << year << std::endl;
    return weight;
  }
  TH2* h = hPDF_[year];
  if(!h) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z PDF weights not defined for year = " << year << std::endl;
    return weight;
  }

  //use |eta|
  z_eta = std::fabs(z_eta);
  const float y_var = (year == 2016) ? z_pt : z_eta;

  //allow the overflow bins to be included in the correction
  const int binx = h->GetXaxis()->FindBin(z_mass);
  const int biny = h->GetYaxis()->FindBin(y_var);
  weight = h->GetBinContent(binx, biny);

  const float min_weight = 1.e-3; //minimum weight allowed
  const float max_weight = 1.e1 ; //maximum weight allowed
  weight = std::min(max_weight, std::max(min_weight, weight));

  if(!std::isfinite(weight) || weight <= 0.) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z PDF weight undefined or <= 0 = " << weight << " (mass, eta, pt) = ("
              << z_mass << ", " << z_eta << ", " << z_pt <<  ") using year = "<< year << "! Returning 1...\n";
    return 1.;
  }
  return weight;
}

//--------------------------------------------------------------------------------------------------------------------------------------
float ZPDFUncertainty::GetRenormScaleWeight(int year, float z_pt, float z_mass) {
  float weight = 1.f;
  if(year == 2017) year = 2018; //Use 2018 weights in 2017 due to no LO MC sample being available
  if(hScaleR_.find(year) == hScaleR_.end()) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z renormalization scale weights not defined for year = " << year << std::endl;
    return weight;
  }
  TH2* h = hScaleR_[year];
  if(!h) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z renormalization scale weights not defined for year = " << year << std::endl;
    return weight;
  }

  //allow the overflow bins to be included in the correction
  const int binx = h->GetXaxis()->FindBin(z_mass);
  const int biny = h->GetYaxis()->FindBin(z_pt);
  weight = h->GetBinContent(binx, biny);

  const float min_weight = 1.e-3; //minimum weight allowed
  const float max_weight = 1.e1 ; //maximum weight allowed
  weight = std::min(max_weight, std::max(min_weight, weight));

  if(!std::isfinite(weight) || weight <= 0.) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z renormalization scale weight undefined or <= 0 = " << weight << " (mass, pT) = ("
              << z_mass << ", " << z_pt << ") using year = "<< year << "! Returning 1...\n";
    return 1.;
  }
  return weight;
}

//--------------------------------------------------------------------------------------------------------------------------------------
float ZPDFUncertainty::GetFactorScaleWeight(int year, float z_pt, float z_mass) {
  float weight = 1.f;
  if(year == 2017) year = 2018; //Use 2018 weights in 2017 due to no LO MC sample being available
  if(hScaleF_.find(year) == hScaleF_.end()) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z factorization scale weights not defined for year = " << year << std::endl;
    return weight;
  }
  TH2* h = hScaleF_[year];
  if(!h) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z factorization scale weights not defined for year = " << year << std::endl;
    return weight;
  }

  //allow the overflow bins to be included in the correction
  const int binx = h->GetXaxis()->FindBin(z_mass);
  const int biny = h->GetYaxis()->FindBin(z_pt);
  weight = h->GetBinContent(binx, biny);

  const float min_weight = 1.e-3; //minimum weight allowed
  const float max_weight = 1.e1 ; //maximum weight allowed
  weight = std::min(max_weight, std::max(min_weight, weight));

  if(!std::isfinite(weight) || weight <= 0.) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z factorization scale weight undefined or <= 0 = " << weight << " (mass, pT) = ("
              << z_mass << ", " << z_pt << ") using year = "<< year << "! Returning 1...\n";
    return 1.;
  }
  return weight;
}
