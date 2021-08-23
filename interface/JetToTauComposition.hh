#ifndef __JETTOTAUCOMPOSITION__
#define __JETTOTAUCOMPOSITION__
//Class to handle composition for jets faking taus
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"

class JetToTauComposition {
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Mode = 10*(use fits) + (do QCD vs W+Jets in tau pT)
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  JetToTauComposition(TString selection, int set = 35, int Mode = 0, int verbose = 0) : Mode_(Mode), verbose_(verbose) {
    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    useOnePt_   = (Mode %  10) /  1 == 1;
    useOneDPhi_ = (Mode %  10) /  1 == 2;
    useFits_    = (Mode % 100) / 10 == 1;
    TString name            = "jettautwopt";
    if(useOnePt_)      name = "jettauonept";
    else if(useOneDPhi_) name = "jettauonemetdeltaphi";
    if(verbose_ > 0) {
      std::cout << __func__ << ":\n Mode = " << Mode
                << " useOnePt = " << useOnePt_
                << " useOneDPhi = " << useOneDPhi_
                << " useFits = " << useFits_
                << std::endl
                << " scale factor selection = " << selection.Data()
                << " set = " << set << std::endl;
    }
    for(int year : years) {
      if(verbose_ > 1) printf("%s: Initializing %i scale factors\n", __func__, year);
      //get the jet --> tau compositions measured
      f = TFile::Open(Form("../scale_factors/jet_to_tau_comp_%s_%i_%i.root", selection.Data(), set, year), "READ");
      if(!f)
        f = TFile::Open(Form("scale_factors/jet_to_tau_comp_%s_%i_%i.root", selection.Data(), set, year), "READ");
      if(f) {
        std::map<int, std::string> names;
        names[kWJets] = "WJets";
        names[kZJets] = "ZJets";
        names[kTop  ] = "Top"  ;
        names[kQCD  ] = "QCD"  ;
        for(int proc = 0; proc < kLast; ++proc) {
          //Get Data histogram
          histsData_[year][proc] = (TH1D*) f->Get(Form("hComposition_%s_%s", name.Data(), names[proc].c_str()));
          if(!histsData_[year][proc]) {
            std::cout << __func__ << ": Warning! No " <<  names[proc].c_str() << " Data histogram found for year = " << year
                      << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
          } else {
            histsData_[year][proc]->SetName(Form("%s_%s_%i", histsData_[year][proc]->GetName(), selection.Data(), year));
            //Get Data fits
            funcsData_[year][proc] = (TF1*) f->Get(Form("fComposition_%s_%s", name.Data(), names[proc].c_str()));
            if(!funcsData_[year][proc]) {
              if(useFits_)
                std::cout << __func__ << ": Warning! No " << names[proc].c_str() << " Data fit found for year = " << year
                          << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
            } else
              funcsData_[year][proc]->SetName(Form("%s_%s_%i", funcsData_[year][proc]->GetName(), selection.Data(), year));
          }
        }
        files_.push_back(f); //to close later
      }
    }
  }

  ~JetToTauComposition() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  //Get scale factor for Data
  void GetComposition(float pt, float pt_lead, float lead_dphi, int year, float*& compositions, float*& comp_up, float*& comp_down) {
    if(pt < 20. ) pt = 20.f;
    if(pt > 199.) pt = 199.f;
    if(pt_lead < 20. ) pt_lead = 20.f;
    if(pt_lead > 199.) pt_lead = 199.f;
    lead_dphi = fabs(lead_dphi);
    if(lead_dphi < 0.) lead_dphi = 0.f;
    if(lead_dphi > M_PI) lead_dphi = M_PI;
    float tot = 0.f;
    float var = pt;
    if(useOnePt_) var = pt_lead;
    else if(useOneDPhi_) var = lead_dphi;
    if(verbose_ > 1)
      std::cout << __func__ << ": Composition fractions for"
                << " pt = " << pt << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi
                << " year = " << year << std::endl;
    for(int proc = 0; proc < kLast; ++proc) {
      float comp(0.f), up(0.f), down(0.f);
      if(useFits_) {
        auto f = funcsData_[year][proc];
        if(!f) {
          std::cout << "!!! " << __func__ << ": No fit function found for year = " << year << " and process = " << proc << std::endl;
        } else {
          comp = f->Eval(var);
          up = comp; down = comp;
        }
      } else {
        auto h = histsData_[year][proc];
        if(!h) {
          std::cout << "!!! " << __func__ << ": No histogram found for year = " << year << " and process = " << proc << std::endl;
        } else {
          comp = h->GetBinContent(h->FindBin(var));
          up   = comp + h->GetBinError(h->FindBin(var));
          down = comp - h->GetBinError(h->FindBin(var));
        }
      }
      if(verbose_ > 1) std::cout << __func__ << ": Composition fraction for process " << proc << " = " << comp << std::endl;
      if(comp > 1.f || comp < 0.f) {
        std::cout << "!!! " << __func__ << ": Composition fraction out of bounds, comp = " << comp
                  << " pt = " << pt << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi
                  << " year = " << year << std::endl;
        comp = std::max(0.f, std::min(comp, 1.f));
      }
      up   = std::max(0.f, std::min(up  , 1.f));
      down = std::max(0.f, std::min(down, 1.f));
      compositions[proc] = comp;
      comp_up  [proc] = up;
      comp_down[proc] = down;
      tot += comp;
    }
    if(fabs(tot - 1.f) > 1.e-4) {
        std::cout << "!!! " << __func__ << ": Total composition out of tolerance, total = " << tot
                  << " pt = " << pt << " lead pt = " << pt_lead << " lead met dphi = " << lead_dphi
                  << " year = " << year << std::endl;
    }
  }


public:
  enum {kWJets, kZJets, kTop, kQCD, kLast};
  std::map<int, std::map<int, TH1D*>> histsData_;
  std::map<int, std::map<int, TF1*>>  funcsData_;
  std::vector<TFile*> files_;
  int Mode_;
  int verbose_;
  bool useOnePt_;
  bool useOneDPhi_;
  bool useFits_;

};
#endif
