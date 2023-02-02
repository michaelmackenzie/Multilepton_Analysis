#include "interface/QCDWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
QCDWeight::QCDWeight(const TString selection, const int Mode, const int verbose) : verbose_(verbose) {

  useFits_        = (Mode %       10) /       1 == 1;
  useDeltaPhi_    = (Mode %      100) /      10 == 1;
  useEtaClosure_  = (Mode %     1000) /     100 == 1;
  use2DPtClosure_ = (Mode %     1000) /     100 == 2;
  use2DScale_     = (Mode %    10000) /    1000 == 1;
  useDeltaRSys_   = (Mode %   100000) /   10000 == 1;
  useJetBinned_   = (Mode %  1000000) /  100000 == 1;
  useAntiIso_     = (Mode % 10000000) / 1000000 == 1;

  if(verbose > 0) {
    std::cout << __func__ << ": useFits = " << useFits_
              << " useDeltaPhi = " << useDeltaPhi_
              << " useEtaClosure = " << useEtaClosure_
              << " use2DPtClosure = " << use2DPtClosure_
              << " use2DScale = " << use2DScale_
              << " useDeltaRSys = " << useDeltaRSys_
              << " useJetBinned = " << useJetBinned_
              << std::endl;
  }

  TFile* f = 0;
  std::vector<int> years = {2016, 2017, 2018};
  const TString hist = (useDeltaPhi_) ? "hRatio_lepdeltaphi1" : "hRatio";
  const TString hist_2D = (useDeltaPhi_) ? "hRatio_lepdelphivsoneeta" : "hRatio_lepdelrvsoneeta";
  TString hist_closure = "hClosure_oneeta";
  if(useEtaClosure_) hist_closure = "hClosure_oneeta";
  else if(use2DPtClosure_) hist_closure = "hRatio_oneptvstwopt";
  const TString hist_sys = "hClosure_lepdeltar";
  const TString hist_jb = "hRatio_lepdeltarj";

  if(verbose > 1) {
    std::cout << __func__ << ": Histogram name = " << hist.Data()
              << " 2D name = " << hist_2D.Data()
              << " closure name = " << hist_closure.Data()
              << " systematic name = " << hist_sys.Data()
              << std::endl;
  }

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    //get the SS --> OS scale factors measured
    f = TFile::Open(Form("%s/qcd_scale_%s_%i.root", path.Data(), selection.Data(), year), "READ");
    if(f) {
      ///////////////////////////////////////////
      //Get Data histogram
      ///////////////////////////////////////////
      histsData_[year] = (TH1*) f->Get(hist.Data());
      if(!histsData_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No Data histogram " << hist.Data() << " found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        histsData_[year]->SetName(Form("%s_%s_%i", histsData_[year]->GetName(), selection.Data(), year));
        histsData_[year]->SetDirectory(0);
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
        hists2DData_[year]->SetDirectory(0);
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
        fitsData_[year]->AddToGlobalList();
      }

      ///////////////////////////////////////////
      //Get Data Fit Error
      ///////////////////////////////////////////
      fitErrData_[year] = (TH1*) f->Get("fit_1s_err");
      if(!fitErrData_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No Data fit error found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        fitErrData_[year]->SetName(Form("%s_%s_%i", fitErrData_[year]->GetName(), selection.Data(), year));
        fitErrData_[year]->SetDirectory(0);
      }

      ///////////////////////////////////////////
      //Get jet-binned results
      ///////////////////////////////////////////

      //binned in: 0 jets, 1 jet, >= 2 jets
      for(int ijet = 0; ijet < 3; ++ijet) {
        const int index = 10*year + ijet;
        {
          const char* name = Form("%s%i", hist_jb.Data(), ijet);
          TH1* h = (TH1*) f->Get(name);
          jetBinnedHists_[index] = h;
          if(!h) {
            std::cout << "QCDWeight::QCDWeight: Warning! No jet-binned histogram " << name << " found for year = " << year
                      << " selection = " << selection.Data() << std::endl;
          } else {
            h->SetName(Form("%s_%s_%i", h->GetName(), selection.Data(), year));
            h->SetDirectory(0);
          }
        }
        {
          const char* name = Form("fRatioJ%i", ijet);
          TF1* fit = (TF1*) f->Get(name);
          jetBinnedFits_[index] = fit;
          if(!fit) {
            std::cout << "QCDWeight::QCDWeight: Warning! No jet-binned fit " << name << " found for year = " << year
                      << " selection = " << selection.Data() << std::endl;
          } else {
            fit->SetName(Form("%s_%s_%i", fit->GetName(), selection.Data(), year));
            fit->AddToGlobalList();
          }
        }
        {
          const char* name = Form("fit_j%i_1s_err", ijet);
          TH1* h = (TH1*) f->Get(name);
          jetBinnedFitErrs_[index] = h;
          if(!h) {
            std::cout << "QCDWeight::QCDWeight: Warning! No jet-binned fit error " << name << " found for year = " << year
                      << " selection = " << selection.Data() << std::endl;
          } else {
            h->SetName(Form("%s_%s_%i", h->GetName(), selection.Data(), year));
            h->SetDirectory(0);
          }
        }
      }

      ///////////////////////////////////////////
      //Get Closure histogram
      ///////////////////////////////////////////
      if(useEtaClosure_) {
        histsClosure_[year] = (TH1*) f->Get(hist_closure.Data());
        if(!histsClosure_[year]) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Closure histogram " << hist_closure.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          histsClosure_[year]->SetName(Form("%s_%s_%i", histsClosure_[year]->GetName(), selection.Data(), year));
          histsClosure_[year]->SetDirectory(0);
        }
      } else if(use2DPtClosure_) {
        TH2* h = (TH2*) f->Get(hist_closure.Data());
        Pt2DClosure_[year] = h;
        if(!h) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Closure histogram " << hist_closure.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          h->SetName(Form("%s_%s_%i", h->GetName(), selection.Data(), year));
          h->SetDirectory(0);
        }
      }

      //Muon Anti-iso --> iso scale factor
      if(useAntiIso_) {
        const char* name = "hRatio_oneptvstwopt_antiisoscale";
        TH2* h = (TH2*) f->Get(name);
        AntiIsoScale_[year] = h;
        if(!h) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Muon anti-iso scale histogram " << name << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          h->SetName(Form("%s_%s_%i", h->GetName(), selection.Data(), year));
          h->SetDirectory(0);
        }
      }

      ///////////////////////////////////////////
      //Get Systematic histogram
      ///////////////////////////////////////////
      histsSys_[year] = (TH1*) f->Get(hist_sys.Data());
      if(!histsSys_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No systematic histogram " << hist_sys.Data() << " found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        histsSys_[year]->SetName(Form("%s_%s_sys_%i", histsSys_[year]->GetName(), selection.Data(), year));
        histsSys_[year]->SetDirectory(0);
      }
      f->Close();
      delete f;
    }
  }

}

//-------------------------------------------------------------------------------------------------------------------------
QCDWeight::~QCDWeight() {
  for(std::pair<int, TH1*> val : histsData_     ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : hists2DData_   ) {if(val.second) delete val.second;}
  for(std::pair<int, TF1*> val : fitsData_      ) {if(val.second) delete val.second;}
  for(std::pair<int, TH1*> val : fitErrData_    ) {if(val.second) delete val.second;}
  for(std::pair<int, TH1*> val : histsClosure_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH1*> val : histsSys_      ) {if(val.second) delete val.second;}
  for(std::pair<int, TH1*> val : jetBinnedHists_) {if(val.second) delete val.second;}
}

//-------------------------------------------------------------------------------------------------------------------------
//Get scale factor for Data
float QCDWeight::GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, const int year, int njets, const bool isantiiso,
                           float& nonclosure, float& antiiso, float& up, float& down, float& sys) {
  njets = std::max(0, std::min(njets, 2)); //jet-bins: 0, 1, >=2
  TH1* h          = (useJetBinned_) ? jetBinnedHists_  [10*year + njets] : histsData_ [year];
  TF1* f          = (useJetBinned_) ? jetBinnedFits_   [10*year + njets] : fitsData_  [year];
  TH1* fErr       = (useJetBinned_) ? jetBinnedFitErrs_[10*year + njets] : fitErrData_[year];
  TH1* hClosure   = histsClosure_[year];
  TH2* h2DClosure = Pt2DClosure_[year];
  TH1* hSys       = histsSys_[year];
  TH2* h2D        = hists2DData_[year];
  TH2* hAntiIso   = AntiIsoScale_[year]; //muon anti-iso --> iso scale


  //ensure within kinematic regions
  deltar = std::fabs(deltar);
  if(deltar > 4.99f) deltar = 4.99f;
  deltaphi = std::fabs(deltaphi);
  if(deltaphi > M_PI) deltaphi = M_PI;
  oneeta = std::min(2.49f, std::max(oneeta, -2.49f));
  onept = std::min(149.f, std::max(onept, 10.f));
  twopt = std::min(149.f, std::max(twopt, 10.f));

  //initialize values
  nonclosure = 1.f; up = 1.f; down = 1.f; sys = 1.f; antiiso = 1.f;
  const float var = (useDeltaPhi_) ? deltaphi : deltar;
  const float closure = oneeta;

  //check if histogram is found
  if(!h && !useFits_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined histogram for year = "
              << year << std::endl;
    return 1.;
  } else if(!f && useFits_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined fit for year = "
              << year << std::endl;
    return 1.;
  } else if(!fErr && useFits_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined fit error for year = "
              << year << std::endl;
    return 1.;
  } else if(!hClosure && useEtaClosure_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined closure histogram for year = "
              << year << std::endl;
    return 1.;
  } else if(!h2DClosure && use2DPtClosure_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined closure histogram for year = "
              << year << std::endl;
    return 1.;
  } else if(!hSys && useDeltaRSys_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined systematic histogram for year = "
              << year << std::endl;
    return 1.;
  } else if(!h2D && use2DScale_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined 2D histogram for year = "
              << year << std::endl;
    return 1.;
  } else if(!hAntiIso && useAntiIso_) {
    std::cout << "QCDWeight::" << __func__ << " Undefined anti-iso correction histogram for year = "
              << year << std::endl;
    return 1.;
  }

  if(verbose_ > 9) {
    std::cout << "QCDWeight::" << __func__ << ":\n Getting weight for:"
              << " delta R = " << deltar << " delta phi = " << deltaphi
              << " one eta = " << oneeta << " one pt = " << onept
              << " two pt = " << twopt << " njets = " << njets
              << " anti-iso = " << isantiiso
              << " year = " << year
              << std::endl;
  }

  float eff(1.f), err(0.f);
  int bin(1);
  if(useFits_) {
    //get bin value
    bin = std::max(1, std::min(fErr->GetNbinsX(), fErr->FindBin(var)));
    //get efficiency from fit function
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
    bin = std::max(1, std::min(h->GetNbinsX(), h->FindBin(var)));

    eff = h->GetBinContent(bin);
    err = h->GetBinError(bin); //use bin error for now
  }

  if(verbose_ > 9) {
    std::cout << " bare weight = " << eff << " +- " << err << std::endl;
  }

  //ensure reasonable values on the SS --> OS scale
  const float max_eff = 5.f;
  const float min_eff = 1.e-5f;
  if(std::isnan(eff) || eff < 0.f) {
    std::cout << "QCDWeight::" << __func__ << " Weight < 0  = " << eff
              << " delta R = " << deltar << " delta phi = " << deltaphi
              << " for year = " << year << std::endl;
    nonclosure = 1.f; up = 1.f; down = 1.f; sys = 1.f; antiiso = 1.f;
    return 1.;
  }

  eff  = std::max(min_eff, std::min(max_eff, eff      ));
  up   = std::max(min_eff, std::min(max_eff, eff + err));
  down = std::max(min_eff, std::min(max_eff, eff - err));
  sys  = up;

  //Apply a closure correction
  if(useEtaClosure_) {
    const int bin = std::max(1, std::min(hClosure->FindBin(closure), hClosure->GetNbinsX()));
    nonclosure = hClosure->GetBinContent(bin);
    if(!std::isfinite(nonclosure) || nonclosure < 0.f) {
      std::cout << "QCDWeight::" << __func__ << " Closure Weight < 0  = " << nonclosure
                << " delta R = " << deltar << " delta phi = " << deltaphi
                << " for year = " << year << std::endl;
      nonclosure = 1.f; up = 1.f; down = 1.f; sys = 1.f; antiiso = 1.f;
      return 1.;
    }
  } else if(use2DPtClosure_) {
    const int xbin = std::max(1, std::min(h2DClosure->GetXaxis()->FindBin(onept), h2DClosure->GetNbinsX()));
    const int ybin = std::max(1, std::min(h2DClosure->GetYaxis()->FindBin(twopt), h2DClosure->GetNbinsY()));
    nonclosure = h2DClosure->GetBinContent(xbin, ybin);
    if(!std::isfinite(nonclosure) || nonclosure <= 0.) {
      if(verbose_) std::cout << "QCDWeight::" << __func__ << " Closure Weight <= 0  = " << nonclosure
                             << " delta R = " << deltar << " delta phi = " << deltaphi
                             << " one pt = " << onept << " two pt = " << twopt
                             << " for year = " << year << std::endl;
      nonclosure = 1.f;
    }
  } else {
    nonclosure = 1.f;
  }

  //apply non-closure correction
  eff  *= nonclosure;
  up   *= nonclosure;
  down *= nonclosure;
  sys  *= nonclosure;

  if(verbose_ > 9) {
    std::cout << " closure = " << nonclosure << ", weight = " << eff << " (" << up << "/" << down << ")" << std::endl;
  }

  //get anti-iso --> iso correction factor if needed
  if(useAntiIso_ && !isantiiso) { //only apply to isolated muons since scales measured with anti-isolated muons
    const int xbin = std::max(1, std::min(hAntiIso->GetXaxis()->FindBin(onept), hAntiIso->GetNbinsX()));
    const int ybin = std::max(1, std::min(hAntiIso->GetYaxis()->FindBin(twopt), hAntiIso->GetNbinsY()));
    antiiso = hAntiIso->GetBinContent(xbin, ybin);
    if(!std::isfinite(antiiso) || antiiso <= 0.f) {
      if(verbose_) std::cout << "QCDWeight::" << __func__ << " Closure Weight < 0  = " << nonclosure
                             << " delta R = " << deltar << " delta phi = " << deltaphi
                             << " one pt = " << onept << " two pt = " << twopt
                             << " for year = " << year << std::endl;
      antiiso = 1.f;
    }
  } else {
    antiiso = 1.f;
  }

  //apply anti-iso correction
  eff  *= antiiso;
  up   *= antiiso;
  down *= antiiso;
  sys  *= antiiso;

  if(verbose_ > 9) {
    std::cout << " iso = " << antiiso << ", weight = " << eff << " (" << up << "/" << down << ")" << std::endl;
  }

  //Evaluate systematic by varying a delta R closure
  if(useDeltaRSys_) {
    const int bin = std::max(1, std::min(hSys->FindBin(deltar), hSys->GetNbinsX()));
    const float correction = hSys->GetBinContent(bin);
    if(!std::isfinite(correction) || correction < 0.f) {
      std::cout << "QCDWeight::" << __func__ << " Systematic < 0  = " << closure
                << " delta R = " << deltar << " delta phi = " << deltaphi
                << " for year = " << year << std::endl;
      nonclosure = 1.f; up = 1.f; down = 1.f; sys = 1.f; antiiso = 1.f;
      return 1.;
    }
    const float eff_sys = eff*correction;
    up = eff_sys;
    down = std::max(min_eff, 2.f*eff - eff_sys); //eff - (sys - eff) = 2*eff - sys
    // sys = eff_sys; leave sys as the statistical variation from the fits
  }

  //Check that the results are reasonable
  if(!std::isfinite(eff) || eff <= 0.f) {
    std::cout << __func__ << ": Warning! QCD scale value error, scale = " << eff << " err = " << err << " nonclosure = " << nonclosure
              << " for year = " << year << "; deltar = " << deltar << "; deltaphi = " << deltaphi << "; oneeta = " << oneeta
              << std::endl;
    eff = min_eff; //default to a small weight
    err = 0.99f*eff; //99% uncertainty
    nonclosure = 1.f;
    antiiso = 1.f;
    up = eff + err;
    down = eff - err;
    sys = up;
  }

  if(verbose_ > 9) {
    std::cout << " final weight = " << eff << " (" << up << "/" << down << ")" << std::endl;
  }

  return eff;
}
