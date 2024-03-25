#include "interface/QCDWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
QCDWeight::QCDWeight(const TString selection, const int Mode, int only_year, const int verbose) : verbose_(verbose) {

  useFits_        = (Mode %         10) /         1 == 1;
  useDeltaPhi_    = (Mode %        100) /        10 == 1;
  useEtaClosure_  = (Mode %       1000) /       100 == 1;
  use2DPtClosure_ = (Mode %       1000) /       100 == 2;
  use2DScale_     = (Mode %      10000) /      1000 == 1;
  useDeltaRSys_   = (Mode %     100000) /     10000 == 1;
  useJetBinned_   = (Mode %    1000000) /    100000 == 1;
  useAntiIso_     = (Mode %   10000000) /   1000000 == 1;
  useLepMVsBDT_   = (Mode %  100000000) /  10000000; //0: none; 1: mutau_e BDT; 2: etau_mu BDT
  useRun2_        = (Mode % 1000000000) / 100000000;

  const char* tag = (useLepMVsBDT_ == 1) ? "mutau_e" : (useLepMVsBDT_ == 2) ? "etau_mu" : "emu";

  if(verbose > 0) {
    std::cout << __func__ << ": useFits = " << useFits_
              << " useDeltaPhi = " << useDeltaPhi_
              << " useEtaClosure = " << useEtaClosure_
              << " use2DPtClosure = " << use2DPtClosure_
              << " use2DScale = " << use2DScale_
              << " useDeltaRSys = " << useDeltaRSys_
              << " useJetBinned = " << useJetBinned_
              << " useAntiIso_ = " << useAntiIso_
              << " useLepMVsBDT_ = " << useLepMVsBDT_
              << " useRun2_ = " << useRun2_
              << " tag = " << tag
              << std::endl;
  }

  std::vector<int> years = {2016, 2017, 2018};
  if(useRun2_ == 1) { //store Run 2 scales in 2016 slot
    only_year = 2016;
    years = {2016};
  }

  TFile* f = 0;
  const TString hist = (useDeltaPhi_) ? "hRatio_lepdeltaphi1" : "hRatio";
  const TString hist_2D = (useDeltaPhi_) ? "hRatio_lepdelphivsoneeta" : "hRatio_lepdelrvsoneeta";
  TString hist_closure = "hClosure_oneeta";
  if(useEtaClosure_) hist_closure = "hClosure_oneeta";
  else if(use2DPtClosure_) hist_closure = "hRatio_oneptvstwopt";
  const TString hist_sys = "hClosure_lepdeltar";
  const TString hist_jb = "hRatio_lepdeltarj";
  const TString hist_mass_v_bdt = (useLepMVsBDT_ == 1) ? "hRatio_mass_vs_mva_mutau" : "hRatio_mass_vs_mva_etau";

  if(verbose > 1) {
    std::cout << __func__ << ": Histogram name = " << hist.Data()
              << " 2D name = " << hist_2D.Data()
              << " closure name = " << hist_closure.Data()
              << " systematic name = " << hist_sys.Data()
              << " (mass, bdt) name = " << hist_mass_v_bdt.Data()
              << std::endl;
  }

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    if(only_year > 2000 && year != only_year) continue;
    //get the SS --> OS scale factors measured
    if(useRun2_ == 1) {
      f = TFile::Open(Form("%s/qcd_scale_%s_2016_2017_2018.root", path.Data(), selection.Data()), "READ");
    } else {
      f = TFile::Open(Form("%s/qcd_scale_%s_%i.root", path.Data(), selection.Data(), year), "READ");
    }
    if(f) {
      ///////////////////////////////////////////
      //Get the SS --> OS histogram
      ///////////////////////////////////////////
      histsData_[year] = (TH1*) f->Get(hist.Data());
      if(!histsData_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No Data histogram " << hist.Data() << " found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        histsData_[year]->SetName(Form("%s_%s_%i", histsData_[year]->GetName(), tag, year));
        histsData_[year]->SetDirectory(0);
      }
      ///////////////////////////////////////////
      //Get 2D SS --> OS histogram
      ///////////////////////////////////////////
      hists2DData_[year] = (TH2D*) f->Get(hist_2D.Data());
      if(!hists2DData_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No Data 2D histogram " << hist_2D.Data() << " found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        hists2DData_[year]->SetName(Form("%s_%s_%i", hists2DData_[year]->GetName(), tag, year));
        hists2DData_[year]->SetDirectory(0);
      }

      ///////////////////////////////////////////
      //Get SS --> OS Fit
      ///////////////////////////////////////////
      fitsData_[year] = (TF1*) f->Get("fRatio");
      if(!fitsData_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No Data fit found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        fitsData_[year]->SetName(Form("%s_%s_%i", fitsData_[year]->GetName(), tag, year));
        fitsData_[year]->AddToGlobalList();
      }

      ///////////////////////////////////////////
      //Get SS --> OS Fit Error
      ///////////////////////////////////////////
      fitErrData_[year] = (TH1*) f->Get("fit_1s_err");
      if(!fitErrData_[year]) {
        std::cout << "QCDWeight::QCDWeight: Warning! No Data fit error found for year = " << year
                  << " selection = " << selection.Data() << std::endl;
      } else {
        fitErrData_[year]->SetName(Form("%s_%s_%i", fitErrData_[year]->GetName(), tag, year));
        fitErrData_[year]->SetDirectory(0);
      }

      ///////////////////////////////////////////
      //Get jet-binned SS --> OS versions
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
            h->SetName(Form("%s_%s_%i", h->GetName(), tag, year));
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
            fit->SetName(Form("%s_%s_%i", fit->GetName(), tag, year));
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
            h->SetName(Form("%s_%s_%i", h->GetName(), tag, year));
            h->SetDirectory(0);
          }
        }

        ///////////////////////////
        // Get alternate fit shapes
        for(int ialt = 0; ; ++ialt) {
          const char* name = Form("fit_j%i_1s_err_%i", ijet, ialt);
          TF1* up   = (TF1*) f->Get(Form("%s_up"  , name));
          TF1* down = (TF1*) f->Get(Form("%s_down", name));
          if(!up || !down) {
            if(ialt == 0) {
              std::cout << "QCDWeight::QCDWeight: Warning! No jet-binned alternate fit shapes " << name << " found for year = " << year
                        << " selection = " << selection.Data() << std::endl;
            }
            break;
          }
          altJetFitsUp_  [index].push_back(up  );
          altJetFitsDown_[index].push_back(down);
          up->SetName(Form("%s_%s_%i", up->GetName(), tag, year));
          up->AddToGlobalList();
          down->SetName(Form("%s_%s_%i", down->GetName(), tag, year));
          down->AddToGlobalList();
        }
      }

      if(useRun2_ == 2) {
        f->Close(); //close the current file
        delete f;
        if(only_year < 2016 && year > 2016) continue; //if processing all years, only retrieve corrections for 2016 slot
        year = 2016; //for the 2016 slot
        //open the Run 2 file
        f = TFile::Open(Form("%s/qcd_scale_%s_2016_2017_2018.root", path.Data(), selection.Data()), "READ");
        if(!f) continue;
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
          histsClosure_[year]->SetName(Form("%s_%s_%i", histsClosure_[year]->GetName(), tag, year));
          histsClosure_[year]->SetDirectory(0);
        }
      } else if(use2DPtClosure_) {
        TH2* h = (TH2*) f->Get(hist_closure.Data());
        Pt2DClosure_[year] = h;
        if(!h) {
          std::cout << "QCDWeight::QCDWeight: Warning! No Closure histogram " << hist_closure.Data() << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          h->SetName(Form("%s_%s_%i", h->GetName(), tag, year));
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
          h->SetName(Form("%s_%s_%i", h->GetName(), tag, year));
          h->SetDirectory(0);
        }
      }

      //(mass, bdt score) bias histogram
      if(useLepMVsBDT_) {
        const char* name = hist_mass_v_bdt.Data();
        TH2* h = (TH2*) f->Get(name);
        LepMVsBDTScale_[year] = h;
        if(!h) {
          std::cout << "QCDWeight::QCDWeight: Warning! No (mass, bdt score) scale histogram " << name << " found for year = " << year
                    << " selection = " << selection.Data() << std::endl;
        } else {
          h->SetName(Form("%s_%s_%i", h->GetName(), tag, year));
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
        histsSys_[year]->SetName(Form("%s_%s_sys_%i", histsSys_[year]->GetName(), tag, year));
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
  for(std::pair<int, TH2*> val : AntiIsoScale_  ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : LepMVsBDTScale_) {if(val.second) delete val.second;}
}

//-------------------------------------------------------------------------------------------------------------------------
//Get scale factor for Data
float QCDWeight::GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, float mass, float bdt, int year, int njets, const bool isantiiso,
                           float& nonclosure, float& antiiso, float& massbdt, float* up, float* down, int& nsys) {
  njets = std::max(0, std::min(njets, 2)); //jet-bins: 0, 1, >=2
  if(useRun2_ == 1) year = 2016; //Run 2 scales use the 2016 slot
  TH1* h          = (useJetBinned_) ? jetBinnedHists_  [10*year + njets] : histsData_ [year];
  TF1* f          = (useJetBinned_) ? jetBinnedFits_   [10*year + njets] : fitsData_  [year];
  TH1* fErr       = (useJetBinned_) ? jetBinnedFitErrs_[10*year + njets] : fitErrData_[year];
  TH1* hClosure   = histsClosure_  [(useRun2_ == 2) ? 2016 : year];
  TH2* h2DClosure = Pt2DClosure_   [(useRun2_ == 2) ? 2016 : year];
  TH1* hSys       = histsSys_      [(useRun2_ == 2) ? 2016 : year];
  TH2* h2D        = hists2DData_   [(useRun2_ == 2) ? 2016 : year];
  TH2* hAntiIso   = AntiIsoScale_  [(useRun2_ == 2) ? 2016 : year]; //muon anti-iso --> iso scale
  TH2* hLepMVsBDT = LepMVsBDTScale_[(useRun2_ == 2) ? 2016 : year]; //(mass, bdt score) bias correction

  std::vector<TF1*> altFitsUp   = (useJetBinned_) ? altJetFitsUp_  [10*year + njets] : std::vector<TF1*>();
  std::vector<TF1*> altFitsDown = (useJetBinned_) ? altJetFitsDown_[10*year + njets] : std::vector<TF1*>();

  if(useFits_ && altFitsUp.size() != altFitsDown.size()) {
    std::cout << "QCDWeight::" << __func__ << " Disagreement in alternate fit up/down list length for year = "
              << year << " --> will skip alternate fits!"
              << std::endl;
    altFitsUp   = {};
    altFitsDown = {};
  }


  //ensure within kinematic regions
  deltar = std::fabs(deltar);
  if(deltar > 4.99f) deltar = 4.99f;
  deltaphi = std::fabs(deltaphi);
  if(deltaphi > M_PI) deltaphi = M_PI;
  oneeta = std::min(2.49f, std::max(oneeta, -2.49f));
  onept = std::min(149.f, std::max(onept, 10.f));
  twopt = std::min(149.f, std::max(twopt, 10.f));

  //initialize values
  nsys = 1;
  nonclosure = 1.f; up[0] = 1.f; down[0] = 1.f; antiiso = 1.f;
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
  bool use_alt_fits = useFits_ && altFitsUp.size() != 0;
  if(useFits_) {
    //get bin value
    bin = std::max(1, std::min(fErr->GetNbinsX(), fErr->FindBin(var)));
    //get efficiency from fit function
    eff = f->Eval(deltar);
    if(use_alt_fits) {
      nsys = altFitsUp.size();
      for(int ialt = 0; ialt < nsys; ++ialt) {
        TF1* fit_up   = altFitsUp  [ialt];
        TF1* fit_down = altFitsDown[ialt];
        up  [ialt] = fit_up  ->Eval(deltar);
        down[ialt] = fit_down->Eval(deltar);
      }
    } else {
      err = fErr->GetBinError(bin); //68% confidence band from virtual fitter
    }
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
    nonclosure = 1.f; up[0] = 1.f; down[0] = 1.f; nsys = 1; antiiso = 1.f;
    return 1.;
  }

  eff     = std::max(min_eff, std::min(max_eff, eff      ));
  if(use_alt_fits) {
    for(int ialt = 0; ialt < nsys; ++ialt) {
      up  [ialt] = std::max(min_eff, std::min(max_eff, up  [ialt]));
      down[ialt] = std::max(min_eff, std::min(max_eff, down[ialt]));
    }
  } else {
    up  [0] = std::max(min_eff, std::min(max_eff, eff + err));
    down[0] = std::max(min_eff, std::min(max_eff, eff - err));
    nsys    = 1;
  }

  //Apply a closure correction
  if(useEtaClosure_) {
    const int bin = std::max(1, std::min(hClosure->FindBin(closure), hClosure->GetNbinsX()));
    nonclosure = hClosure->GetBinContent(bin);
    if(!std::isfinite(nonclosure) || nonclosure < 0.f) {
      std::cout << "QCDWeight::" << __func__ << " Closure Weight < 0  = " << nonclosure
                << " delta R = " << deltar << " delta phi = " << deltaphi
                << " for year = " << year << std::endl;
      nonclosure = 1.f; up[0] = 1.f; down[0] = 1.f; nsys = 1; antiiso = 1.f;
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
  eff     *= nonclosure;
  for(int ialt = 0; ialt < nsys; ++ialt) {
    up  [ialt] *= nonclosure;
    down[ialt] *= nonclosure;
  }

  if(verbose_ > 9) {
    std::cout << " closure = " << nonclosure << ", weight = " << eff << " (" << up[0] << "/" << down[0] << ")" << std::endl;
  }

  //get anti-iso --> iso correction factor if needed
  if(useAntiIso_ && !isantiiso) { //only apply to isolated muons since scales measured with anti-isolated muons
    const int xbin = std::max(1, std::min(hAntiIso->GetXaxis()->FindBin(onept), hAntiIso->GetNbinsX()));
    const int ybin = std::max(1, std::min(hAntiIso->GetYaxis()->FindBin(twopt), hAntiIso->GetNbinsY()));
    antiiso = hAntiIso->GetBinContent(xbin, ybin);
    if(!std::isfinite(antiiso) || antiiso <= 0.f) {
      if(verbose_) std::cout << "QCDWeight::" << __func__ << " anti-isolated to isolated weight < 0  = " << antiiso
                             << " delta R = " << deltar << " delta phi = " << deltaphi
                             << " one pt = " << onept << " two pt = " << twopt
                             << " for year = " << year << std::endl;
      antiiso = 1.f;
    }
  } else {
    antiiso = 1.f;
  }

  //apply anti-iso correction
  eff     *= antiiso;
  for(int ialt = 0; ialt < nsys; ++ialt) {
    up  [ialt] *= antiiso;
    down[ialt] *= antiiso;
  }

  if(verbose_ > 9) {
    std::cout << " iso = " << antiiso << ", weight = " << eff << " (" << up[0] << "/" << down[0] << ")" << std::endl;
  }

  //get (mass, bdt) correction factor if needed
  if(useLepMVsBDT_ && bdt >= -1.f) { //BDT < -1 is unitialized
    const int xbin = std::max(1, std::min(hLepMVsBDT->GetXaxis()->FindBin(mass), hLepMVsBDT->GetNbinsX()));
    const int ybin = std::max(1, std::min(hLepMVsBDT->GetYaxis()->FindBin(bdt ), hLepMVsBDT->GetNbinsY()));
    massbdt = hLepMVsBDT->GetBinContent(xbin, ybin);
    if(!std::isfinite(massbdt) || massbdt <= 0.f) {
      if(verbose_) std::cout << "QCDWeight::" << __func__ << " (mass, bdt score) closure weight < 0  = " << massbdt
                             << "; mass = " << mass << " bdt = " << bdt
                             << " for year = " << year << std::endl;
      massbdt = 1.f;
    }
    massbdt = std::max(0.3f, std::max(3.f, massbdt)); //constrain to a factor of 3 correction
  } else {
    massbdt = 1.f;
  }

  //apply (mass, bdt) correction
  eff     *= massbdt;
  for(int ialt = 0; ialt < nsys; ++ialt) {
    up  [ialt] *= massbdt;
    down[ialt] *= massbdt;
  }

  if(verbose_ > 9) {
    std::cout << " mass = " << mass << ", bdt = " << bdt << ", (mass, bdt) weight = " << massbdt << " --> overall weight = " << eff << " (" << up[0] << "/" << down[0] << ")" << std::endl;
  }

  //Evaluate systematic by varying a delta R closure
  if(useDeltaRSys_) {
    const int bin = std::max(1, std::min(hSys->FindBin(deltar), hSys->GetNbinsX()));
    const float correction = hSys->GetBinContent(bin);
    if(!std::isfinite(correction) || correction < 0.f) {
      std::cout << "QCDWeight::" << __func__ << " Systematic < 0  = " << closure
                << " delta R = " << deltar << " delta phi = " << deltaphi
                << " for year = " << year << std::endl;
      nonclosure = 1.f; up[0] = 1.f; down[0] = 1.f; nsys = 1; antiiso = 1.f;
      return 1.;
    }
    const float eff_sys = eff*correction;
    up  [0] = eff_sys;
    down[0] = std::max(min_eff, 2.f*eff - eff_sys); //eff - (sys - eff) = 2*eff - sys
    nsys = 1;
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
    up  [0] = eff + err;
    down[0] = eff - err;
    nsys = 1;
  }

  if(verbose_ > 9) {
    std::cout << " final weight = " << eff << " (" << up[0] << "/" << down[0] << ")" << std::endl;
  }

  return eff;
}

//-------------------------------------------------------------------------------------------------------------------------
//Get scale factor for Data without uncertainties/separated corrections
float QCDWeight::GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, float mass, float bdt, const int year, int njets, const bool isantiiso,
                           float& nonclosure, float& antiiso, float& massbdt, float& up, float& down, float& sys) {
  float up_arr[10], down_arr[10];
  int nsys(0);
  float wt = GetWeight(deltar, deltaphi, oneeta, onept, twopt, mass, bdt, year, njets, isantiiso, nonclosure, antiiso, massbdt, up_arr, down_arr, nsys);
  up = 0.f; down = 0.f;
  //add each deviation from nominal in quadrature
  for(int ialt = 0; ialt < nsys; ++ialt) {
    up   = std::pow(wt - up_arr  [ialt], 2);
    down = std::pow(wt - down_arr[ialt], 2);
  }
  up   = wt + std::sqrt(up);
  down = std::max(1.e-5f, wt - std::sqrt(down));
  sys = up;
  return wt;
}

//-------------------------------------------------------------------------------------------------------------------------
//Get scale factor for Data without uncertainties/separated corrections
float QCDWeight::GetWeight(float deltar, float deltaphi, float oneeta, float onept, float twopt, float mass, float bdt, const int year, int njets, const bool isantiiso) {
  float nonclosure, antiiso, massbdt, up, down, sys;
  float wt = GetWeight(deltar, deltaphi, oneeta, onept, twopt, mass, bdt, year, njets, isantiiso, nonclosure, antiiso, massbdt, up, down, sys);
  return wt;
}
