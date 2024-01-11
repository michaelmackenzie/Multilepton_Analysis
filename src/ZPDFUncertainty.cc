#include "interface/ZPDFUncertainty.hh"

using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------------------------------
ZPDFUncertainty::ZPDFUncertainty(const bool useEnvelope) : useEnvelope_(useEnvelope) {

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

    //define which PDF index set to use for the simplified case
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

    //get all the PDFs for the full case
    if(useEnvelope_) {
      f = TFile::Open(Form("%s/z_pdf_shift_env_%i.root", path.Data(), year), "READ");
      if(!f) continue;
      for(int ipdf = 1; ipdf <= ((year == 2016) ? 100 : 32); ++ipdf) {
        const int map_index = year*kMaxNPDF + ipdf;
        hPDF_[map_index] = (TH2*) f->Get(Form("hratio_%i", ipdf));
        if(!hPDF_[map_index]) {
          std::cout << "ZPDFUncertainty: No PDF uncertainty histogram found for year " << year << " index " << ipdf << std::endl;
        } else {
          hPDF_[map_index]->SetName(Form("%s_pdf_%i", hPDF_[map_index]->GetName(), year));
          hPDF_[map_index]->SetDirectory(0);
        }

        //Retrieve the scale factor to preserve the cross section (already implemented in the Z weights, only for other MC)
        TH1* scale = (TH1*) f->Get(Form("scale_%i", ipdf));
        if(!scale) {
          std::cout << "ZPDFUncertainty: No PDF scale correction found for year " << year << " index " << ipdf << std::endl;
          pdf_scale_[map_index] = 1.f;
        } else {
          pdf_scale_[map_index] = scale->GetBinContent(1);
        }
      }
      f->Close();
      delete f;
    }

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
float ZPDFUncertainty::GetPDFWeight(int year, float z_pt, float z_eta, float z_mass, int index) {
  float weight = 1.f;
  if(year == 2017) year = 2018; //Use 2018 weights in 2017 due to no LO MC sample being available

  //Define the map index to the appropriate histogram
  int map_index = (useEnvelope_ && index >= 0 && index < kMaxNPDF) ? year*kMaxNPDF + index : year;

  //Check for the histogram
  if(hPDF_.find(map_index) == hPDF_.end()) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z PDF weights not defined for year = " << year;
    if(useEnvelope_ && index >= 0) std::cout << " and index = " << index;
    std::cout << std::endl;
    return weight;
  }
  TH2* h = hPDF_[map_index];
  if(!h) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! Z PDF weights not defined for year = " << year;
    if(useEnvelope_ && index >= 0) std::cout << " and index = " << index;
    std::cout << std::endl;
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
void ZPDFUncertainty::GetPDFWeight(int year, float z_pt, float z_eta, float z_mass, float* weights, unsigned& npdf) {
  if(!useEnvelope_) return;
  npdf = (year == 2016) ? 100 : 32;
  for(unsigned ipdf = 1; ipdf <= npdf; ++ipdf) {
    weights[ipdf] = GetPDFWeight(year, z_pt, z_eta, z_mass, ipdf);
  }
 }

//--------------------------------------------------------------------------------------------------------------------------------------
// Scale factor to preserve process cross section, only relevant for MC processes not using the Z PDF weights
float ZPDFUncertainty::GetPDFScale(int year, int index) {
  float scale = 1.f;
  if(!useEnvelope_) return scale; //only defined if using the entire PDF envelope
  if(year == 2017) year = 2018; //Use 2018 weights in 2017 due to no LO MC sample being available

  //Define the map index to the appropriate scale factor
  int map_index = year*kMaxNPDF + index;

  //Check for the histogram
  if(pdf_scale_.find(map_index) == pdf_scale_.end()) {
    std::cout << "ZPDFUncertainty::" << __func__ << " WARNING! PDF correction scale not defined for year = " << year
              << " and index = " << index << std::endl;
    return scale;
  }
  scale = std::min(1.1f, std::max(0.9f, pdf_scale_[map_index])); //should be within a few percent of 1
  return scale;
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
