#ifndef __JETTOTAUWEIGHT__
#define __JETTOTAUWEIGHT__
//Class to handle scale factor of anti-iso --> iso for jets faking taus
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"

class JetToTauWeight {
public:
  JetToTauWeight(TString selection) {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    for(int year : years) {
      f = TFile::Open(Form("../scale_factors/jet_to_tau_%s_%i.root", selection.Data(), year), "READ");
      if(!f)
	f = TFile::Open(Form("scale_factors/jet_to_tau_%s_%i.root", selection.Data(), year), "READ");
      if(f) {
	for(int dm = 0; dm < 4; ++dm) {
	  //Get MC histogram
	  histsMC_[year][dm] = (TH2D*) f->Get(Form("hmc_eff_%idm", dm));
	  if(!histsMC_[year][dm])
	    std::cout << "JetToTauWeight::JetToTauWeight: Warning! No MC histogram found for dm = "
		      << dm << " year = " << year << std::endl;
	  else 
	    histsMC_[year][dm]->SetName(Form("%s_%s_%i", histsMC_[year][dm]->GetName(), selection.Data(), year));
	  //Get Data histogram
	  histsData_[year][dm] = (TH2D*) f->Get(Form("hdata_eff_%idm", dm));
	  if(!histsData_[year][dm])
	    std::cout << "JetToTauWeight::JetToTauWeight: Warning! No Data histogram found for dm = "
		      << dm << " year = " << year << std::endl;
	  else 
	    histsData_[year][dm]->SetName(Form("%s_%s_%i", histsData_[year][dm]->GetName(), selection.Data(), year));
	}
	files_.push_back(f); //to close later
      }
    }
  }
  
  ~JetToTauWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  //Get scale factor for MC
  float GetMCFactor(int DM, int year, float pt, float eta) {
    TH2D* h = 0;
    //get correct decay mode histogram
    if(DM == 0) 
      h = histsMC_[year][0];
    else if(DM == 1) 
      h = histsMC_[year][1];
    else if(DM == 10) 
      h = histsMC_[year][2];
    else if(DM == 11) 
      h = histsMC_[year][3];

    //check if histogram is found
    if(!h) {
      std::cout << "JetToTauWeight::" << __func__ << " Undefined histogram for DM = "
		<< DM << " year = " << year << std::endl;
      return 1.;
    }

    return GetFactor(h, pt, eta, DM, year);
  }
  
  //Get scale factor for Data
  float GetDataFactor(int DM, int year, float pt, float eta) {
    TH2D* h = 0;
    //get correct decay mode histogram
    if(DM == 0) 
      h = histsData_[year][0];
    else if(DM == 1) 
      h = histsData_[year][1];
    else if(DM == 10) 
      h = histsData_[year][2];
    else if(DM == 11) 
      h = histsData_[year][3];

    //check if histogram is found
    if(!h) {
      std::cout << "JetToTauWeight::" << __func__ << " Undefined histogram for DM = "
		<< DM << " year = " << year << std::endl;
      return 1.;
    }

    return GetFactor(h, pt, eta, DM, year);
  }
  
private:
  float GetFactor(TH2D* h, float pt, float eta, int DM, int year) {
    //ensure within kinematic regions
    eta = fabs(eta);
    if(pt > 999.) pt = 999.;
    else if(pt < 20.) pt = 20.;
    if(eta > 2.3) eta = 2.29;

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
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! MC Eff < 0 = " << eff
		<< " pt = " << pt << " eta = " << eta
		<< " dm = " << DM << " year = " << year << std::endl;
      eff = 0.000001;
    } else if(eff >= 1.) {
      std::cout << "JetToTauWeight::" << __func__ << ": Warning! MC Eff >= 1 = " << eff
		<< " pt = " << pt << " eta = " << eta
		<< " dm = " << DM << " year = " << year << std::endl;
      eff = 1.;
    } else
      //write as scale factor instead of efficiency
      //eff_0 = a / (a+b) = 1 / (1 + 1/eff_p) = eff_p / (eff_p + 1)
      // --> eff_p = eff_0 / (1 - eff_0)
      eff = eff / (1. - eff);
    
    
    return eff;
  }
  
public:
  
  std::map<int, std::map<int, TH2D*>> histsMC_; 
  std::map<int, std::map<int, TH2D*>> histsData_; 
  std::vector<TFile*> files_;
};
#endif
