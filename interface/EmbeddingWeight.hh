#ifndef __EMBEDDINGWEIGHT__
#define __EMBEDDINGWEIGHT__
//Class to handle embedding scale factors
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom3.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooWorkspace.h"

namespace CLFV {
  class EmbeddingWeight {
  public:
    EmbeddingWeight(/*int Mode = 0, */int seed = 90, int verbose = 0) : verbose_(verbose) {
      TFile* f = 0;
      rnd_ = new TRandom3(seed);

      for(int year = k2016; year <= k2018; ++year) {
        //Get variables
        f = TFile::Open(Form("../scale_factors/htt_scalefactors_legacy_%i.root", year + 2016), "READ");
        if(!f)
          f = TFile::Open(Form("scale_factors/htt_scalefactors_legacy_%i.root", year+2016), "READ");
        if(!f) {
          std::cout << "Embedding corrections file for " << year + 2016 << " not found!\n";
        } else {
          RooWorkspace* w = (RooWorkspace*) f->Get("w");
          if(!w) {
            std::cout << "Embedding workspace for " << year + 2016 << " not found!\n";
          } else {
            ws             [year]    = w;
            trigUnfold     [year]    = (RooFormulaVar*) w->obj(Form("m_sel_trg_%sratio", (year == k2016) ? "kit_" : ""));
            idUnfold       [year]    = (RooFormulaVar*) w->obj("m_sel_idEmb_ratio");
            genTauPt       [year][0] = (RooRealVar*) w->var("gt1_pt");
            genTauPt       [year][1] = (RooRealVar*) w->var("gt2_pt");
            genTauPt       [year][2] = (RooRealVar*) w->var("gt_pt");
            genTauEta      [year][0] = (RooRealVar*) w->var("gt1_eta");
            genTauEta      [year][1] = (RooRealVar*) w->var("gt2_eta");
            genTauEta      [year][2] = (RooRealVar*) w->var("gt_eta");
            muonTrig       [year][0] = (RooFormulaVar*) w->obj("m_trg_data");
            muonTrig       [year][1] = (RooFormulaVar*) w->obj("m_trg_mc");
            muonIso        [year]    = (RooFormulaVar*) w->obj("m_iso_ratio");
            muonID         [year]    = (RooFormulaVar*) w->obj("m_id_ratio");
            muonPt         [year]    = (RooRealVar*) w->var("m_pt");
            muonEta        [year]    = (RooRealVar*) w->var("m_eta");
            electronTrig   [year][0] = (RooFormulaVar*) w->obj("e_trg_data");
            electronTrig   [year][1] = (RooFormulaVar*) w->obj("e_trg_mc");
            electronID     [year]    = (RooFormulaVar*) w->obj("e_id_ratio");
            electronPt     [year]    = (RooRealVar*) w->var("e_pt");
            electronEta    [year]    = (RooRealVar*) w->var("e_eta");
          }
          files_.push_back(f);
        }
      }
    }

    ~EmbeddingWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }


    double UnfoldingWeight(double pt_1, double eta_1, double pt_2, double eta_2, int year) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      if(!genTauPt[year][0] || !genTauPt[year][1] || !genTauPt[year][2] || !genTauEta[year][0] || !genTauEta[year][1] || !genTauEta[year][2]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Gen tau variables not found!\n";
        return 1.;
      }
      if(!trigUnfold[year] || !idUnfold[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Unfolding functions not found!\n";
        return 1.;
      }

      ///////////////////////////
      // Apply trigger unfolding
      ///////////////////////////

      genTauPt [year][0]->setVal(pt_1);
      genTauPt [year][1]->setVal(pt_2);
      genTauEta[year][0]->setVal(eta_1);
      genTauEta[year][1]->setVal(eta_2);
      double scale_factor(trigUnfold[year]->evaluate());

      ///////////////////////////
      // Apply each ID unfolding
      ///////////////////////////

      genTauPt [year][2]->setVal(pt_1);
      genTauEta[year][2]->setVal(eta_1);
      scale_factor *= idUnfold[year]->evaluate();
      genTauPt [year][2]->setVal(pt_2);
      genTauEta[year][2]->setVal(eta_2);
      scale_factor *= idUnfold[year]->evaluate();

      if(scale_factor <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      return scale_factor;
    }

    double MuonIDWeight(double pt, double eta, int year) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      if(!muonPt[year] || !muonEta[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Muon variables not found!\n";
        return 1.;
      }
      if(!muonIso[year] || !muonID[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Weight functions not found!\n";
        return 1.;
      }

      ///////////////////////////
      // Apply Iso weight
      ///////////////////////////

      muonPt [year]->setVal(pt);
      muonEta[year]->setVal(eta);
      double scale_factor(muonIso[year]->evaluate());

      ///////////////////////////
      // Apply ID weight
      ///////////////////////////

      scale_factor *= muonID[year]->evaluate();

      if(scale_factor <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      return scale_factor;
    }

    double MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      if(!muonPt[year] || !muonEta[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Muon variables not found!\n";
        return 1.;
      }
      if(!muonTrig[year][0] || !muonTrig[year][1]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Trigger functions not found!\n";
        return 1.;
      }

      ///////////////////////////
      // Get efficiencies
      ///////////////////////////

      muonPt [year]->setVal(pt);
      muonEta[year]->setVal(eta);
      data_eff = muonTrig[year][0]->evaluate();
      mc_eff   = muonTrig[year][1]->evaluate();
      double scale_factor((mc_eff > 0.) ? data_eff / mc_eff : 1.);

      if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__
                  << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
                  << ", returning 1" << std::endl;
        //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
        mc_eff = 0.5;
        data_eff = 0.5;
        return 1.;
      }
      return scale_factor;
    }

    double ElectronIDWeight(double pt, double eta, int year) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      if(!electronPt[year] || !electronEta[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Electron variables not found!\n";
        return 1.;
      }
      if(!electronID[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Weight functions not found!\n";
        return 1.;
      }

      ///////////////////////////
      // Apply ID weight
      ///////////////////////////

      electronPt [year]->setVal(pt);
      electronEta[year]->setVal(eta);
      double scale_factor(electronID[year]->evaluate());

      if(scale_factor <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      return scale_factor;
    }

    double ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
      year -= 2016;
      if(year != k2016 && year != k2017 && year != k2018) {
        std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
        return 1.;
      }
      if(!electronPt[year] || !electronEta[year]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Electron variables not found!\n";
        return 1.;
      }
      if(!electronTrig[year][0] || !electronTrig[year][1]) {
        std::cout << "Error! EmbeddingWeight::" << __func__ << ": Trigger functions not found!\n";
        return 1.;
      }

      ///////////////////////////
      // Get efficiencies
      ///////////////////////////

      electronPt [year]->setVal(pt);
      electronEta[year]->setVal(eta);
      data_eff = electronTrig[year][0]->evaluate();
      mc_eff   = electronTrig[year][1]->evaluate();
      double scale_factor((mc_eff > 0.) ? data_eff / mc_eff : 1.);

      if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
        std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__
                  << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
                  << ", returning 1" << std::endl;
        //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
        mc_eff = 0.5;
        data_eff = 0.5;
        return 1.;
      }
      return scale_factor;
    }

  public:
    enum { k2016, k2017, k2018};
    std::vector<TFile*> files_;
    RooWorkspace* ws[k2018+1]; //workspaces for each year, in case they're needed
    RooFormulaVar* trigUnfold[k2018+1]; //selection muon trigger unfolding weight function
    RooFormulaVar* idUnfold[k2018+1]; //selection muon ID unfolding weight function
    RooRealVar* genTauPt[k2018+1][3]; //gen tau pT variables: 0 = 1, 1 = 2, 3 = current
    RooRealVar* genTauEta[k2018+1][3]; //gen tau eta variables
    RooFormulaVar* muonTrig[k2018+1][2]; //muon trigger weight function 0: data eff 1: mc eff
    RooFormulaVar* muonIso[k2018+1]; //muon Iso weight function
    RooFormulaVar* muonID[k2018+1]; //muon ID weight function
    RooRealVar* muonPt[k2018+1]; //reconstructed muon pT
    RooRealVar* muonEta[k2018+1]; //reconstructed muon eta
    RooFormulaVar* electronTrig[k2018+1][2]; //electron trigger weight function 0: data eff 1: mc eff
    RooFormulaVar* electronID[k2018+1]; //electron ID weight function
    RooRealVar* electronPt[k2018+1]; //reconstructed electron pT
    RooRealVar* electronEta[k2018+1]; //reconstructed electron eta
    int verbose_;
    TRandom3* rnd_; //for generating systematic shifted parameters
  };
}
#endif