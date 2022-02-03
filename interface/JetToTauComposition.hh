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

namespace CLFV {
  class JetToTauComposition {
  public:
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mode = 10*(use fits) + (do QCD vs W+Jets in tau pT)
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    JetToTauComposition(TString selection, int set = 35, int Mode = 0, int verbose = 0) : Mode_(Mode), verbose_(verbose) {
      TFile* f = 0;
      std::vector<int> years = {2016, 2017, 2018};
      useOnePt_   = (Mode %   10) /  1 == 1; //composition in light lepton pT
      useOneDPhi_ = (Mode %   10) /  1 == 2; //composition in delta phi (light lep, MET)
      useFits_    = (Mode %  100) / 10 == 1; //use fits of the composition (Not Implemented)
      useDM_      = (Mode % 1000) / 100 == 1; //decay mode dependent compositions
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
            for(int idm = -1; idm < 4; ++idm) {
              TString nm = (idm > -1) ? Form("hComposition_%s_%i_%s", name.Data(), idm, names[proc].c_str()) : Form("hComposition_%s_%s", name.Data(), names[proc].c_str());
              //Get Data histogram
              histsData_[year][idm+1][proc] = (TH1D*) f->Get(nm.Data());
              if(!histsData_[year][idm+1][proc]) {
                std::cout << __func__ << ": Warning! No " <<  nm.Data() << " Data histogram found for year = " << year
                          << " idm = " << idm
                          << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
              } else {
                histsData_[year][idm+1][proc]->SetName(Form("%s_%s_%i_%i", histsData_[year][idm+1][proc]->GetName(), selection.Data(), idm+1, year));
              }
            }
          }
          files_.push_back(f); //to close later
        }
      }
    }

    ~JetToTauComposition() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }

    //Get scale factor for Data
    void GetComposition(float pt, int DM, float pt_lead, float lead_dphi, int year, float*& compositions, float*& comp_up, float*& comp_down) {
      if(pt < 20. ) pt = 20.f;
      if(pt > 199.) pt = 199.f;
      if(pt_lead < 20. ) pt_lead = 20.f;
      if(pt_lead > 199.) pt_lead = 199.f;
      lead_dphi = fabs(lead_dphi);
      if(lead_dphi < 0.) lead_dphi = 0.f;
      if(lead_dphi > M_PI) lead_dphi = M_PI;
      int idm = (!useDM_) ? 0 : 1 + 2*(DM/10) + DM%10; //inclusive->0, 0->1, 1->2, 10->3, 11->4
      if(idm > 4 || idm < 0) {
        std::cout << "!!! Error! DM undefined in " << __func__ << " idm = " << idm << " DM = " << DM << std::endl;
        idm = std::min(3, std::max(0, idm));
      }
      float tot = 0.f;
      float var = pt;
      if(useOnePt_) var = pt_lead;
      else if(useOneDPhi_) var = lead_dphi;
      if(verbose_ > 1)
        std::cout << __func__ << ": Composition fractions for"
                  << " pt = " << pt << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi
                  << " year = " << year << " DM = " << DM
                  << std::endl;
      for(int proc = 0; proc < kLast; ++proc) {
        float comp(0.f), up(0.f), down(0.f);
        auto h = histsData_[year][idm][proc];
        if(!h) {
          std::cout << "!!! " << __func__ << ": No histogram found for year = " << year << " DM = " << DM << " and process = " << proc << std::endl;
        } else {
          comp = h->GetBinContent(h->FindBin(var));
          up   = comp + h->GetBinError(h->FindBin(var));
          down = comp - h->GetBinError(h->FindBin(var));
        }
        if(verbose_ > 1) std::cout << __func__ << ": Composition fraction for process " << proc << " = " << comp << std::endl;
        if(comp > 1.f || comp < 0.f) {
          std::cout << "!!! " << __func__ << ": Composition fraction out of bounds, comp = " << comp
                    << " pt = " << pt << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi
                    << " year = " << year << " DM = " << DM
                    << std::endl;
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
                  << " year = " << year << " DM = " << DM
                  << std::endl;
      }
    }


  public:
    enum {kWJets, kZJets, kTop, kQCD, kLast};
    //      year           DM         process
    std::map<int, std::map<int, std::map<int, TH1D*>>> histsData_;
    std::vector<TFile*> files_;
    int Mode_;
    int verbose_;
    bool useOnePt_;
    bool useOneDPhi_;
    bool useFits_;
    bool useDM_;

  };
}
#endif
