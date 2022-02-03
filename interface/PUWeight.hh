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
    PUWeight(TString base_path = "/uscms/home/mmackenz/nobackup/ZEMu/CMSSW_10_2_18/") {
      base_path += "src/PhysicsTools/NanoAODTools/python/postprocessing/data/pileup/";
      TFile* f = 0;

      ////////////////
      // 2016 files //
      ////////////////
      //data
      f = TFile::Open((base_path+"PileupData_GoldenJSON_Full2016.root").Data(), "READ");
      if(f) {
        dataHists_[2016] = (TH1F*) f->Get("pileup")->Clone("hd_2016");
        if(!dataHists_[2016]) std::cout << "Warning! No pileup histogram found for 2016 data!\n";
        files_.push_back(f);
      }
      //MC
      f = TFile::Open((base_path+"pileup_profile_Summer16.root").Data(), "READ");
      if(f) {
        mcHists_[2016] = (TH1F*) f->Get("pu_mc")->Clone("hm_2016");
        if(!mcHists_[2016]) std::cout << "Warning! No pileup histogram found for 2016 MC!\n";
        files_.push_back(f);
      }

      ////////////////
      // 2017 files //
      ////////////////
      //data
      f = TFile::Open((base_path+"PileupHistogram-goldenJSON-13tev-2017-99bins_withVar.root").Data(), "READ");
      if(f) {
        dataHists_[2017] = (TH1F*) f->Get("pileup")->Clone("hd_2017");
        if(!dataHists_[2017]) std::cout << "Warning! No pileup histogram found for 2017 data!\n";
        files_.push_back(f);
      }
      //MC
      f = TFile::Open((base_path+"mcPileup2017.root").Data(), "READ");
      if(f) {
        mcHists_[2017] = (TH1F*) f->Get("pu_mc")->Clone("hm_2017");
        if(!mcHists_[2017]) std::cout << "Warning! No pileup histogram found for 2017 MC!\n";
        files_.push_back(f);
      }

      ////////////////
      // 2018 files //
      ////////////////
      //data
      f = TFile::Open((base_path+"PileupHistogram-goldenJSON-13tev-2018-100bins_withVar.root").Data(), "READ");
      if(f) {
        dataHists_[2018] = (TH1F*) f->Get("pileup")->Clone("hd_2018");
        if(!dataHists_[2018]) std::cout << "Warning! No pileup histogram found for 2018 data!\n";
        files_.push_back(f);
      }
      //MC
      f = TFile::Open((base_path+"mcPileup2018.root").Data(), "READ");
      if(f) {
        mcHists_[2018] = (TH1F*) f->Get("pu_mc")->Clone("hm_2018");
        if(!mcHists_[2018]) std::cout << "Warning! No pileup histogram found for 2018 MC!\n";
        files_.push_back(f);
      }

      for(int year = 2016; year < 2019; ++year) {
        if(dataHists_[year] && mcHists_[year]) mcHists_[year]->Scale(dataHists_[year]->Integral()/mcHists_[year]->Integral());
      }
    }

    ~PUWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }

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
    std::vector<TFile*> files_;
  };
}
#endif
