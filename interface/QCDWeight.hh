#ifndef __QCDWEIGHT__
#define __QCDWEIGHT__
//Class to handle scale factor of SS --> OS for QCD events
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"

class QCDWeight {
public:
  QCDWeight(TString selection, int seed = 90)  {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    int max_deltar_bins(0);
    rnd_ = new TRandom3(seed);
    for(int year : years) {
      //get the SS --> OS scale factors measured
      f = TFile::Open(Form("../scale_factors/qcd_scale_%s_%i.root", selection.Data(), year), "READ");
      if(!f)
        f = TFile::Open(Form("scale_factors/qcd_scale_%s_%i.root", selection.Data(), year), "READ");
      if(f) {
        //Get Data histogram
        histsData_[year] = (TH1D*) f->Get("hRatio");
        if(!histsData_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Data histogram found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          histsData_[year]->SetName(Form("%s_%s_%i", histsData_[year]->GetName(), selection.Data(), year));
          max_deltar_bins = std::max(histsData_[year]->GetNbinsX(), max_deltar_bins);
        }
        //Get Data fits
        files_.push_back(f); //to close later
      }
    }

    //Define shifted systematic weights as up or down
    for(int year : years) {
      isShiftedUp_[year] = {};
      for(int ibin = 0; ibin < max_deltar_bins; ++ibin) {
        isShiftedUp_[year][ibin] = rnd_->Uniform() > 0.5;
      }
    }
  }

  ~QCDWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  //Get scale factor for Data
  float GetWeight(float deltar, int year, float& up, float& down, float& sys) {
    TH1D* h = histsData_[year];

    //check if histogram is found
    if(!h) {
      std::cout << "QCDWeight::" << __func__ << " Undefined histogram for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }
    //ensure within kinematic regions
    deltar = fabs(deltar);
    if(deltar > 5.) deltar = 5.;

    //get bin value
    int bin = h->FindBin(deltar);
    if(bin == 0) bin = 1;
    else if(bin > h->GetNbinsX()) bin = h->GetNbinsX();

    float eff = h->GetBinContent(bin);
    if(std::isnan(eff) || eff < 0.) {
      std::cout << "QCDWeight::" << __func__ << " Weight < 0  = " << eff
                << " for year = " << year << std::endl;
      up = 0.; down = 0.; sys = 0.;
      return 0.;
    }
    float err_bin = h->GetBinError  (bin);
    up   = eff + err_bin;
    down = std::max(eff - err_bin, 0.f);
    sys = (isShiftedUp_[year][bin-1]) ? up : down;

    return eff;
  }


public:
  std::map<int, TH1D*> histsData_;
  std::vector<TFile*> files_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          bin
  std::map<int, std::map<int, bool>> isShiftedUp_; //whether the systematic is shifted up or down
};
#endif
