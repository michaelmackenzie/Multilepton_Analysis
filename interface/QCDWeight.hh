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
  QCDWeight(TString selection, int Mode = 0, int seed = 90, int verbose = 0) : verbose_(verbose) {

    useFits_       = (Mode %     10) /     1 == 1;
    useDeltaPhi_   = (Mode %    100) /    10 == 1;
    useEtaClosure_ = (Mode %   1000) /   100 == 1;
    use2DScale_    = (Mode %  10000) /  1000 == 1;
    useDeltaRSys_  = (Mode % 100000) / 10000 == 1;

    if(verbose) {
      std::cout << __func__ << ": useFits = " << useFits_
                << " useDeltaPhi = " << useDeltaPhi_
                << " useEtaClosure = " << useEtaClosure_
                << " use2DScale = " << use2DScale_
                << " useDeltaRSys = " << useDeltaRSys_
                << std::endl;
    }

    TFile* f = 0;
    std::vector<int> years = {2016, 2017, 2018};
    int max_deltar_bins(0);
    rnd_ = new TRandom3(seed);
    TString hist = (useDeltaPhi_) ? "hRatio_lepdeltaphi1" : "hRatio";
    TString hist_2D = (useDeltaPhi_) ? "hRatio_lepdelphivsoneeta" : "hRatio_lepdelrvsoneeta";
    TString hist_closure = "hClosure_oneeta";
    TString hist_sys = "hClosure_lepdeltar";

    if(verbose > 1)
      std::cout << __func__ << ": Histogram name = " << hist.Data()
                << " 2D name = " << hist_2D.Data()
                << " closure name = " << hist_closure.Data()
                << " systematic name = " << hist_sys.Data()
                << std::endl;

    for(int year : years) {
      //get the SS --> OS scale factors measured
      f = TFile::Open(Form("../scale_factors/qcd_scale_%s_%i.root", selection.Data(), year), "READ");
      if(!f)
        f = TFile::Open(Form("scale_factors/qcd_scale_%s_%i.root", selection.Data(), year), "READ");
      if(f) {
        ///////////////////////////////////////////
        //Get Data histogram
        ///////////////////////////////////////////
        histsData_[year] = (TH1D*) f->Get(hist.Data());
        if(!histsData_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Data histogram " << hist.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          histsData_[year]->SetName(Form("%s_%s_%i", histsData_[year]->GetName(), selection.Data(), year));
          max_deltar_bins = std::max(histsData_[year]->GetNbinsX(), max_deltar_bins);
        }
        ///////////////////////////////////////////
        //Get 2D Data histogram
        ///////////////////////////////////////////
        hists2DData_[year] = (TH2D*) f->Get(hist_2D.Data());
        if(!hists2DData_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Data 2D histogram " << hist_2D.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          hists2DData_[year]->SetName(Form("%s_%s_%i", hists2DData_[year]->GetName(), selection.Data(), year));
        }
        ///////////////////////////////////////////
        //Get Data Fit
        ///////////////////////////////////////////
        fitsData_[year] = (TF1*) f->Get("fRatio");
        if(!fitsData_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Data fit found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          fitsData_[year]->SetName(Form("%s_%s_%i", fitsData_[year]->GetName(), selection.Data(), year));
        }
        ///////////////////////////////////////////
        //Get Data Fit Error
        ///////////////////////////////////////////
        fitErrData_[year] = (TH1D*) f->Get("fit_1s_err");
        if(!fitErrData_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Data fit error found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          fitErrData_[year]->SetName(Form("%s_%s_%i", fitErrData_[year]->GetName(), selection.Data(), year));
        }
        ///////////////////////////////////////////
        //Get Closure histogram
        ///////////////////////////////////////////
        histsClosure_[year] = (TH1D*) f->Get(hist_closure.Data());
        if(!histsClosure_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Closure histogram " << hist_closure.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          histsClosure_[year]->SetName(Form("%s_%s_%i", histsClosure_[year]->GetName(), selection.Data(), year));
          max_deltar_bins = std::max(histsClosure_[year]->GetNbinsX(), max_deltar_bins);
        }

        ///////////////////////////////////////////
        //Get Systematic histogram
        ///////////////////////////////////////////
        histsSys_[year] = (TH1D*) f->Get(hist_sys.Data());
        if(!histsSys_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No systematic histogram " << hist_sys.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          histsSys_[year]->SetName(Form("%s_%s_sys_%i", histsSys_[year]->GetName(), selection.Data(), year));
        }

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
  float GetWeight(float deltar, float deltaphi, float oneeta, int year, float& up, float& down, float& sys) {
    TH1D* h = histsData_[year];
    TF1*  f = fitsData_ [year];
    TH1D* fErr = fitErrData_[year];
    TH1D* hClosure = histsClosure_[year];
    TH1D* hSys = histsSys_[year];
    TH2D* h2D = hists2DData_[year];

    //ensure within kinematic regions
    deltar = fabs(deltar);
    if(deltar > 4.99) deltar = 4.99;
    deltaphi = fabs(deltaphi);
    if(deltaphi > M_PI) deltaphi = M_PI;
    oneeta = std::min( 2.49f, std::max(oneeta, -2.49f));

    float var = (useDeltaPhi_) ? deltaphi : deltar;
    float closure = oneeta;

    //check if histogram is found
    if(!h && !useFits_) {
      std::cout << "QCDWeight::" << __func__ << " Undefined histogram for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    } else if(!f && useFits_) {
      std::cout << "QCDWeight::" << __func__ << " Undefined fit for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    } else if(!fErr && useFits_) {
      std::cout << "QCDWeight::" << __func__ << " Undefined fit error for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    } else if(!hClosure && useEtaClosure_) {
      std::cout << "QCDWeight::" << __func__ << " Undefined closure histogram for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    } else if(!hSys && useDeltaRSys_) {
      std::cout << "QCDWeight::" << __func__ << " Undefined systematic histogram for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    } else if(!h2D && use2DScale_) {
      std::cout << "QCDWeight::" << __func__ << " Undefined 2D histogram for year = "
                << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }

    if(verbose_ > 9) {
      std::cout << "QCDWeight::" << __func__ << ":\n Getting weight for:"
                << " delta R = " << deltar << " delta phi = " << deltaphi
                << " one eta = " << oneeta
                << " year = " << year
                << std::endl;
    }

    float eff, err;
    int bin(1);
    if(useFits_) {
      //get bin value
      bin = fErr->FindBin(var);
      if(bin == 0) bin = 1;
      else if(bin > fErr->GetNbinsX()) bin = fErr->GetNbinsX();

      eff = f->Eval(deltar);
      err = fErr->GetBinError(bin); //68% confidence band from virtual fitter
    } else if(use2DScale_) {
      //get bin values
      bin = std::max(1, std::min(h2D->GetNbinsY(), h2D->GetYaxis()->FindBin(var)));
      int bin2 = std::max(1, std::min(h2D->GetNbinsX(), h2D->GetXaxis()->FindBin(fabs(oneeta))));
      eff = h2D->GetBinContent(bin2, bin);
      err = h2D->GetBinError  (bin2, bin); //use bin error for now
      if(verbose_ > 9) {
        std::cout << " x bin = " << bin2
                  << " y bin = " << bin
                  << " eff = " << eff << " +- " << err
                  << std::endl;
      }
    } else {
      //get bin value
      bin = h->FindBin(var);
      if(bin == 0) bin = 1;
      else if(bin > h->GetNbinsX()) bin = h->GetNbinsX();

      eff = h->GetBinContent(bin);
      err = h->GetBinError(bin); //use bin error for now
    }
    const float max_eff = 5.;
    const float min_eff = 1.e-5;
    if(std::isnan(eff) || eff < 0.) {
      std::cout << "QCDWeight::" << __func__ << " Weight < 0  = " << eff
                << " delta R = " << deltar << " delta phi = " << deltaphi
                << " for year = " << year << std::endl;
      up = 1.; down = 1.; sys = 1.;
      return 1.;
    }
    eff = std::max(min_eff, std::min(eff, max_eff));
    up   = eff + err;
    down = std::max(eff - err, 0.f);
    if(!useFits_)
      sys  = (isShiftedUp_[year][bin-1]) ? up : down;
    else
      sys = up;

    //Apply a closure correction
    if(useEtaClosure_) {
      int bin = std::max(1, std::min(hClosure->FindBin(closure), hClosure->GetNbinsX()));
      float correction = hClosure->GetBinContent(bin);
      if(std::isnan(correction) || correction < 0.) {
        std::cout << "QCDWeight::" << __func__ << " Closure Weight < 0  = " << closure
                  << " delta R = " << deltar << " delta phi = " << deltaphi
                  << " for year = " << year << std::endl;
        up = 1.; down = 1.; sys = 1.;
        return 1.;
      }
      eff  *= correction;
      up   *= correction;
      down *= correction;
      sys  *= correction;
    }

    //Evaluate systematic by varying a delta R closure
    if(useDeltaRSys_) {
      int bin = std::max(1, std::min(hSys->FindBin(deltar), hSys->GetNbinsX()));
      float correction = hSys->GetBinContent(bin);
      if(std::isnan(correction) || correction < 0.) {
        std::cout << "QCDWeight::" << __func__ << " Systematic < 0  = " << closure
                  << " delta R = " << deltar << " delta phi = " << deltaphi
                  << " for year = " << year << std::endl;
        up = 1.; down = 1.; sys = 1.;
        return 1.;
      }
      float eff_sys = eff*correction;
      up = eff_sys;
      down = 2.*eff - eff_sys; //eff - (sys - eff) = 2*eff - sys
      // sys = eff_sys; leave sys as the statistical variation from the fits
    }

    return eff;
  }


public:
  std::map<int, TH1D*> histsData_;
  std::map<int, TH2D*> hists2DData_;
  std::map<int, TF1*> fitsData_;
  std::map<int, TH1D*> fitErrData_;
  std::map<int, TH1D*> histsClosure_;
  std::map<int, TH1D*> histsSys_;
  std::vector<TFile*> files_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          bin
  std::map<int, std::map<int, bool>> isShiftedUp_; //whether the systematic is shifted up or down
  int verbose_;
  bool useFits_;
  bool useDeltaPhi_;
  bool useEtaClosure_;
  bool use2DScale_;
  bool useDeltaRSys_;
};
#endif
