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
    PUWeight() {
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
          dataHists_[year] = (TH1F*) f->Get("pileup");
          if(!dataHists_[year]) {
            std::cout << "!!! Warning: " << __func__ << ": No pileup histogram found for " << year << " Data!\n";
          } else {
            dataHists_[year]->Clone(Form("hd_%i", year));
            dataHists_[year]->SetDirectory(0);
          }
          f->Close();
          delete f;
        }
        //MC
        f = TFile::Open((path+fnames_mc[year]).Data(), "READ");
        if(f) {
          mcHists_[year] = (TH1F*) f->Get("pu_mc");
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
      for(std::pair<int, TH1F*> val : dataHists_) {if(val.second) delete val.second;}
      for(std::pair<int, TH1F*> val : mcHists_  ) {if(val.second) delete val.second;}
    }

    float GetWeight(float nint, int year) {
      float weight(1.), ndata(1.), nmc(1.);
      TH1F* hdata = dataHists_[year];
      TH1F* hMC   = mcHists_  [year];
      //exit if no histograms
      if(!hdata || !hMC) {std::cout << "No histograms found in PUWeight::" << __func__ << std::endl; return weight;}
      //exit if nint > number of bins in histograms --> no number stored
      if(hdata->GetNbinsX() <= nint || hMC->GetNbinsX() <= nint) return weight;
      //assume binning is 0 - N, width = 1
      //round nint to integer, which it should be
      int n = nint;
      ndata = hdata->GetBinContent(n);
      nmc   = hMC  ->GetBinContent(n);
      if(ndata > 0. && nmc > 0.) weight = ndata/nmc;
      if(weight > 3.) std::cout << "Warning in PUWeight::" << __func__ << ": weight = "
                                << weight << " > 3 with nInt = " << nint << " and year = "
                                << year << " ndata = " << ndata << " nmc = " << nmc << std::endl;
      return weight;
    }

    std::map<int, TH1F*> dataHists_;
    std::map<int, TH1F*> mcHists_;
  };
}
#endif
