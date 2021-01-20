#ifndef __JETTOLEPWEIGHT__
#define __JETTOLEPWEIGHT__
//Class to handle scale factor of anti-iso --> iso for jets faking leps
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"

class JetToLepWeight {
public:
  JetToLepWeight(TString selection) {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    for(int year : years) {
      f = TFile::Open(Form("../scale_factors/jet_to_lep_%s_%i.root", selection.Data(), year), "READ");
      if(!f)
	f = TFile::Open(Form("scale_factors/jet_to_lep_%s_%i.root", selection.Data(), year), "READ");
      if(f) {
	//Get Data histogram
	histsData_[year] = (TH2D*) f->Get(Form("hdata_eff_2d"));
	if(!histsData_[year])
	  std::cout << "JetToLepWeight::JetToLepWeight: Warning! No Data histogram found for year = " << year << std::endl;
	else 
	  histsData_[year]->SetName(Form("%s_%s_%i", histsData_[year]->GetName(), selection.Data(), year));
	files_.push_back(f); //to close later
      }
    }
  }
  
  ~JetToLepWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  //Get scale factor for Data
  float GetDataFactor(int year, float pt, float eta) {
    TH2D* h = 0;
    h = histsData_[year];

    //check if histogram is found
    if(!h) {
      std::cout << "JetToLepWeight::" << __func__ << " Undefined histogram for year = " << year << std::endl;
      return 1.;
    }

    return GetFactor(h, pt, eta, year);
  }
  
private:
  float GetFactor(TH2D* h, float pt, float eta, int year) {
    //ensure within kinematic regions
    eta = fabs(eta);
    if(pt > 999.) pt = 999.;
    else if(pt < 20.) pt = 20.;
    if(eta > 2.5) eta = 2.49;

    //get bin value
    int binx = h->GetXaxis()->FindBin(pt);
    int biny = h->GetYaxis()->FindBin(eta);
    float eff_bin = h->GetBinContent(binx, biny);
    double pt_bin = h->GetXaxis()->GetBinCenter(binx);
    bool leftofcenter = h->GetXaxis()->GetBinCenter(binx) > pt; //check which side of bin center we are
    //if at boundary, use interpolation from bin inside hist
    if(binx == 1)  leftofcenter = false; 
    if(binx == h->GetNbinsX())  leftofcenter = true; 
    int binx_off = binx-(2*leftofcenter-1); 
    float eff_off = h->GetBinContent(binx_off, biny);
    double pt_off = h->GetXaxis()->GetBinCenter(binx_off);
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
    } else
      //write as scale factor instead of efficiency
      //eff_0 = a / (a+b) = 1 / (1 + 1/eff_p) = eff_p / (eff_p + 1)
      // --> eff_p = eff_0 / (1 - eff_0)
      eff = eff / (1. - eff);
    
    
    return eff;
  }
  
public:
  
  std::map<int, TH2D*> histsData_; 
  std::vector<TFile*> files_;
};
#endif
