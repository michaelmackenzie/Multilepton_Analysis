#ifndef __PUWEIGHT__
#define __PUWEIGHT__
//Class to handle pileup weight of MC processes
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TString.h"

namespace CLFV {

  class PUWeight {
  public:
    PUWeight(int verbose = 0) {
      verbose_ = verbose;
      const TString cmssw = gSystem->Getenv("CMSSW_BASE");
      const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/PhysicsTools/NanoAODTools/python/postprocessing/data/pileup";
      TFile* f = 0;

      std::map<int, TString> fnames_data, fnames_mc;
      fnames_data[2016] = "/PileupData_GoldenJSON_Full2016.root";
      fnames_data[2017] = "/PileupHistogram-goldenJSON-13tev-2017-99bins_withVar.root";
      fnames_data[2018] = "/PileupHistogram-goldenJSON-13tev-2018-100bins_withVar.root";
      fnames_mc  [2016] = "/pileup_profile_Summer16.root";
      fnames_mc  [2017] = "/mcPileup2017.root";
      fnames_mc  [2018] = "/mcPileup2018.root";

      //Retrieve data for each year
      for(int year = 2016; year <= 2018; ++year) {
        //data
        f = TFile::Open((path+fnames_data[year]).Data(), "READ");
        if(f) {
          dataHists_[year] = (TH1*) f->Get("pileup");
          if(!dataHists_[year]) {
            std::cout << "!!! Warning: " << __func__ << ": No pileup histogram found for " << year << " Data!\n";
          } else {
            dataHists_[year]->Clone(Form("hd_%i", year));
            dataHists_[year]->SetDirectory(0);
          }
          dataHistsUp_[year] = (TH1*) f->Get("pileup_plus");
          if(!dataHistsUp_[year]) {
            std::cout << "!!! Warning: " << __func__ << ": No pileup_plus histogram found for " << year << " Data!\n";
          } else {
            dataHistsUp_[year]->Clone(Form("hd_up_%i", year));
            dataHistsUp_[year]->SetDirectory(0);
          }
          dataHistsDown_[year] = (TH1*) f->Get("pileup_minus");
          if(!dataHistsDown_[year]) {
            std::cout << "!!! Warning: " << __func__ << ": No pileup_down histogram found for " << year << " Data!\n";
          } else {
            dataHistsDown_[year]->Clone(Form("hd_down_%i", year));
            dataHistsDown_[year]->SetDirectory(0);
          }
          f->Close();
          delete f;
        }
        //MC
        f = TFile::Open((path+fnames_mc[year]).Data(), "READ");
        if(f) {
          mcHists_[year] = (TH1*) f->Get("pu_mc");
          if(!mcHists_[year]) {
            std::cout << "!!! Warning: " << __func__ << ": No pileup histogram found for " << year << " MC!\n";
          } else {
            mcHists_[year]->Clone(Form("hm_%i", year));
            mcHists_[year]->SetDirectory(0);
          }
          f->Close();
          delete f;
        }
        //Scale MC normalization to match the data normalization
        if(dataHists_[year] && mcHists_[year]) mcHists_[year]->Scale(dataHists_[year]->Integral()/mcHists_[year]->Integral());
      }


    }

    ~PUWeight() {
      for(std::pair<int, TH1*> val : dataHists_    ) {if(val.second) delete val.second;}
      for(std::pair<int, TH1*> val : dataHistsUp_  ) {if(val.second) delete val.second;}
      for(std::pair<int, TH1*> val : dataHistsDown_) {if(val.second) delete val.second;}
      for(std::pair<int, TH1*> val : mcHists_      ) {if(val.second) delete val.second;}
    }

    float GetWeight(float nint, int year) {
      float tmp_up, tmp_down;
      return GetWeight(nint, year, tmp_up, tmp_down);
    }

    float GetWeight(float nint, int year, float& up, float& down) {
      float weight(1.f), ndata(1.f), nmc(1.f);
      up = 1.f; down = 1.f;
      TH1* hdata     = dataHists_    [year];
      TH1* hdataUp   = dataHistsUp_  [year];
      TH1* hdataDown = dataHistsDown_[year];
      TH1* hMC       = mcHists_      [year];
      //exit if no histograms
      if(!hdata || !hMC || !hdataUp || !hdataDown) {std::cout << "No histograms found in PUWeight::" << __func__ << std::endl; return weight;}
      //exit if nint > number of bins in histograms --> no number stored
      if(hdata->GetNbinsX() <= nint || hMC->GetNbinsX() <= nint) return weight;
      //assume binning is 0 - N, width = 1
      //round nint to integer, which it should be
      const int n = nint;
      ndata = hdata->GetBinContent(n);
      nmc   = hMC  ->GetBinContent(n);
      if(ndata > 0.f && nmc > 0.f) weight = ndata/nmc;
      const float max_weight(3.f);
      if(weight > 3.) {
        if(verbose_) std::cout << "Warning in PUWeight::" << __func__ << ": weight = "
                               << weight << " > " << max_weight << " with nInt = " << nint << " and year = "
                               << year << " ndata = " << ndata << " nmc = " << nmc << std::endl;
        weight = max_weight;
      }
      //calculate up/down values
      ndata = hdataUp->GetBinContent(n);
      up    = (ndata > 0.f && nmc > 0.f) ? std::min(max_weight, ndata/nmc) : weight;
      ndata = hdataDown->GetBinContent(n);
      down  = (ndata > 0.f && nmc > 0.f) ? std::min(max_weight, ndata/nmc) : weight;

      return weight;
    }

    int verbose_;
    std::map<int, TH1*> dataHists_;
    std::map<int, TH1*> dataHistsUp_;
    std::map<int, TH1*> dataHistsDown_;
    std::map<int, TH1*> mcHists_;
  };
}
#endif
