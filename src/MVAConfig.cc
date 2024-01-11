#include "interface/MVAConfig.hh"

using namespace CLFV;


MVAConfig::MVAConfig(int useCDF, int use_cdf_fit, int use_xgboost) {
  useCDF_ = useCDF;
  use_cdf_fit_ = use_cdf_fit;
  use_xgboost_ = use_xgboost;
  //initialize MVA names
  names_ = {
            //0 - 9: nominal mvas, even are Higgs, odd are Z
            "mutau_BDT_MM_8.higgs",(use_xgboost > 9) ? "mutau.Z0.xgboost" : "mutau_BDT_MM_8.Z0",
            "etau_BDT_MM_8.higgs" ,(use_xgboost > 9) ? "etau.Z0.xgboost"  : "etau_BDT_MM_8.Z0",
            // "emu_BDT_MM_8.higgs","emu_BDT_MM_8.Z0",
            "emu_BDT_MM_8.higgs",(use_xgboost == 1) ? "emu.Z0.xgboost" : (use_xgboost == 2 || use_xgboost == 3) ? "emu.Z0.xgboost_atlas" : "emu_BDT_MM_8.Z0",
            "mutau_e_BDT_MM_8.higgs",(use_xgboost > 9) ? "mutau_e.Z0.xgboost" : "mutau_e_BDT_MM_8.Z0" ,
            "etau_mu_BDT_MM_8.higgs",(use_xgboost > 9) ? "etau_mu.Z0.xgboost" : "etau_mu_BDT_MM_8.Z0"//,
  };

  //initialize MVA output categories
  if(useCDF) {
    categories_["hmutau"  ] = {0.05, 0.10, 0.25, 0.50};
    categories_["zmutau"  ] = {0.05, 0.10, 0.25, 0.50};
    categories_["hetau"   ] = {0.05, 0.10, 0.25, 0.50};
    categories_["zetau"   ] = {0.05, 0.10, 0.25, 0.50};
    categories_["hemu"    ] = {0.05, 0.10, 0.25, 0.50};
    // categories_["zemu"    ] = {0.050, 0.100, 0.200, 0.250}; //defined with a cut-and-count sqrt(S)/B in a single category (top one), others added as filler
    categories_["zemu"    ] = {-1.0, -0.08, 0.00, 0.04}; //defined with a cut-and-count sqrt(S)/B quadrature sum for (top) 3 categories in M in 86 - 96 GeV/c^2
    // categories_["zemu"    ] = {-1.0, 0.075, 0.300, 0.675}; //(CDF) defined with a cut-and-count sqrt(S)/B quadrature sum for (top) 3 categories in M in 86 - 96 GeV/c^2
    categories_["hmutau_e"] = {0.05, 0.10, 0.25, 0.50};
    categories_["zmutau_e"] = {0.05, 0.10, 0.25, 0.50};
    categories_["hetau_mu"] = {0.05, 0.10, 0.25, 0.50};
    categories_["zetau_mu"] = {0.05, 0.10, 0.25, 0.50};
  } else {
    categories_["hmutau"  ] = {-0.05680,  0.02900,  0.13370,  0.26540};
    categories_["zmutau"  ] = {-0.28180, -0.15400, -0.04330,  0.12920};
    categories_["hetau"   ] = {-0.06700,  0.04130,  0.15380,  0.33350};
    categories_["zetau"   ] = {-0.25540, -0.11290, -0.03850,  0.05990};
    categories_["hemu"    ] = {-0.05770, -0.015  , -0.015  ,  0.085  }; //defined with cut-and-count in M in [M_B - 5, M_B + 5]
    if(use_xgboost == 1) {
      categories_["zemu"    ] = {-1., 0.3, 0.7, 0.9}; //Taken from https://indico.cern.ch/event/1284989/contributions/5399522/attachments/2644302/4585307/ZmuE_run2.pdf
    } else if(use_xgboost == 2) {
      categories_["zemu"    ] = {-1., -1., -1., 0.575}; //minimal variable version, 1 category optimization
    } else if(use_xgboost == 3) {
      categories_["zemu"    ] = {-1., 0.250, 0.600, 0.850}; //minimal variable version, 3 category optimization
    } else {
      categories_["zemu"    ] = {-0.21800, -0.080  , -0.005  ,  0.060  }; //defined with cut-and-count in M in [M_B - 5, M_B + 5]
    }
    categories_["hmutau_e"] = {-0.10660,  0.00650,  0.08600,  0.15860};
    categories_["zmutau_e"] = {-0.12310, -0.09790, -0.05830,  0.07190};
    categories_["hetau_mu"] = {-0.12820, -0.00400,  0.08060,  0.16160};
    categories_["zetau_mu"] = {-0.32500, -0.15460, -0.02680,  0.09410};
  }

  for(unsigned imva = 0; imva < names_.size(); ++imva) {
    TString name = names_[imva];
    int cat = -1;
    if(name.Contains("mutau_e") || name.Contains("etau_mu") || name.Contains("emu")) cat = 0; //emu data
    else if(name.Contains("mutau")) cat = 1; //mutau data
    else if(name.Contains("etau")) cat = 2; //etau data
    data_cat_.push_back(cat);
  }

  //initialize signal CDF transform histograms
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  std::vector<TString> selections = {"zmutau", "zetau", "zmutau_e", "zetau_mu", "zemu"};
  for(TString selection : selections) {
    TFile* fcdf = TFile::Open(Form("%s/cdf_transform_%s.root", path.Data(), selection.Data()), "READ");
    if(!fcdf) continue;

    //Retrieve the histogrammed CDF transform
    TH1* hcdf = (TH1*) fcdf->Get("CDF");
    if(!hcdf) {
      printf("MVAConfig: Failed to find %s CDF histogram\n", selection.Data());
      fcdf->Close();
      continue;
    }
    hcdf =  (TH1*) hcdf->Clone(Form("CDF_%s", selection.Data()));
    hcdf->SetDirectory(0);
    cdf_[selection] = hcdf;

    //Retrieve the fit to the low score tail
    TF1* cdf_fit = (TF1*) fcdf->Get("fit_func");
    if(!cdf_fit) {
      printf("MVAConfig: Failed to find %s CDF tail fit\n", selection.Data());
      fcdf->Close();
      continue;
    }
    cdf_fit->SetName(Form("CDF_%s", selection.Data()));
    cdf_fit->AddToGlobalList();
    fcdf->Close();
    cdf_fit_[selection] = cdf_fit;
  }
}

    //-------------------------------------------------------------------------------------------------
    // Nominal binnings of the MVA score distributions
std::vector<Double_t> MVAConfig::Bins(Int_t index, Int_t HistSet) {
  if(useCDF_) return CDFBins(index, HistSet);
  TString selection = GetSelectionByIndex(index);
  std::vector<Double_t> bins;
  HistSet = HistSet % 100; //get the base set number
  //-------------------------------------------------------------------------------
  if       (selection == "hmutau") {
    double edges[] = {-1.00, -0.80,
                      -0.60, -0.50, -0.40, -0.30, -0.25,
                      -0.20, -0.15, -0.10, -0.05, -0.01,
                      +0.02,  0.05,  0.08,  0.11,  0.14,
                      +0.17,  0.20,
                      +0.50,  2.00};
    for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
      bins.push_back(edges[bin]);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "zmutau") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-1.00, -0.80, -0.55, -0.50, -0.45,
                        -0.40, -0.35, -0.30, -0.25, -0.20,
                        -0.15, -0.10, -0.05, +0.00, +0.25,
                        +0.40, +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-1.00, -0.80, -0.50, -0.45, -0.40,
                        -0.35, -0.30, -0.25, -0.20, -0.15,
                        -0.10, -0.05, +0.00, +0.05, +0.10,
                        +0.15, +0.35,
                        +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 28) { //Z->mumu region, high scores
      double edges[] = {-1.00, -0.80,
                        -0.40, -0.35, -0.30, -0.25,
                        -0.20, -0.18, -0.16, -0.14, -0.12,
                        -0.10, -0.08, -0.06, -0.04, -0.02,
                        +0.00, +0.02, +0.04, +0.06, +0.08,
                        +0.10, +0.12, +0.14, +0.16, +0.19,
                        +0.35, +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 25) { //central mass region, high scores
      double edges[] = {-1.00, -0.80, -0.45,
                        -0.40, -0.35, -0.30, -0.26, -0.22,
                        -0.20, -0.18, -0.16, -0.14, -0.12,
                        -0.10, -0.08, -0.06, -0.04, -0.02,
                        +0.00, +0.02, +0.04, +0.06, +0.08,
                        +0.10, +0.12, +0.14, +0.16, +0.18,
                        +0.20, +0.23,
                        +0.35, +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else { //default, nominal score distribution
      double edges[] = {-1.00, -0.80, -0.45,
                        -0.40, -0.35, -0.30, -0.26, -0.22,
                        -0.20, -0.18, -0.16, -0.14, -0.12,
                        -0.10, -0.08, -0.06, -0.04, -0.02,
                        +0.00, +0.02, +0.04, +0.06, +0.08,
                        +0.10, +0.12, +0.14, +0.16, +0.18,
                        +0.20, +0.23,
                        +0.35, +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hetau") {
    double edges[] = {-1.00, -0.80,
                      -0.60, -0.50, -0.40, -0.30, -0.25,
                      -0.20, -0.15, -0.10, -0.05, -0.01,
                      +0.02,  0.05,  0.08,  0.11,  0.15,
                      +0.40,
                      +0.50,  2.00};
    for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
      bins.push_back(edges[bin]);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "zetau") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-1.00, -0.90,
                        -0.65, -0.57, -0.50, -0.45, -0.40,
                        -0.35, -0.30, -0.25, -0.20, -0.15,
                        -0.10, +0.20, +0.40, +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-1.00, -0.90, -0.55,
                        -0.45, -0.40, -0.35, -0.30, -0.26,
                        -0.22, -0.18, -0.12, -0.06, +0.00,
                        +0.05, +0.10, +0.35,
                        +0.40, +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 28) { //z->ee mass region, high scores mostly
      double edges[] = {-1.00, -0.80, -0.70,
                        -0.35, -0.25, -0.20,
                        -0.18, -0.16, -0.14, -0.12, -0.10,
                        -0.08, -0.06, -0.04, -0.02, +0.00,
                        +0.02, +0.04, +0.06, +0.08, +0.10,
                        +0.12, +0.14, +0.17,
                        +0.35, +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 25) { //central mass region, high scores mostly
      double edges[] = {-1.00, -0.80,
                        -0.45, -0.35, -0.30, -0.25, -0.22,
                        -0.20, -0.18, -0.16, -0.14, -0.12,
                        -0.10, -0.08, -0.06, -0.04, -0.02,
                        +0.00, +0.02, +0.04, +0.06, +0.08,
                        +0.10, +0.12, +0.14, +0.16, +0.18,
                        +0.20,
                        +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else { //default, nominal score distribution
      double edges[] = {-1.00, -0.80,
                        -0.45, -0.35, -0.30, -0.25, -0.22,
                        -0.20, -0.18, -0.16, -0.14, -0.12,
                        -0.10, -0.08, -0.06, -0.04, -0.02,
                        +0.00, +0.02, +0.04, +0.06, +0.08,
                        +0.10, +0.12, +0.14, +0.16, +0.18,
                        +0.20,
                        +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hemu") {
    for(float bin = -1.; bin <= 2.; bin += 0.05) {
      bins.push_back(bin);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "zemu") {
    if(use_xgboost_) {
      for(float bin = 0.f; bin <= 1.f; bin += 0.05f) {
        bins.push_back(bin);
      }
    } else {
      for(float bin = -1.f; bin <= 1.f; bin += 0.05f) {
        bins.push_back(bin);
      }
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hmutau_e") {
    double edges[] = {-1.00, -0.80,
                      -0.55, -0.45, -0.35, -0.30, -0.25,
                      -0.20, -0.15, -0.10, -0.07, -0.04,
                      -0.01,  0.02,  0.08,  0.30
                      +0.50,  2.00};
    for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
      bins.push_back(edges[bin]);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "zmutau_e") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-1.00, -0.80, -0.40, -0.30,
                        -0.25, -0.20, -0.15, -0.10, -0.05,
                        +0.20,
                        +0.40, +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-1.00, -0.90, -0.50, -0.40,
                        -0.35, -0.30, -0.25, -0.20, -0.15,
                        -0.10, -0.05, +0.00, +0.05, +0.10,
                        +0.35,
                        +0.40, +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 25) { //central mass region, high scores
      double edges[] = {-1.00, -0.80,
                        -0.50, -0.45, -0.40, -0.35,
                        -0.30, -0.25, -0.20, -0.16, -0.12,
                        -0.08, -0.04, +0.00, +0.02, +0.04,
                        +0.06, +0.08, +0.10, +0.12, +0.14,
                        +0.16, +0.18, +0.20, +0.22,
                        +0.35, +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else { //default, nominal score distribution
      double edges[] = {-1.00, -0.80,
                        -0.50, -0.45, -0.40, -0.35,
                        -0.30, -0.25, -0.20, -0.16, -0.12,
                        -0.08, -0.04, +0.00, +0.02, +0.04,
                        +0.06, +0.08, +0.10, +0.12, +0.14,
                        +0.16, +0.18, +0.20, +0.22,
                        +0.35, +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hetau_mu") {
    double edges[] = {-1.00, -0.80,
                      -0.55, -0.45, -0.35, -0.30, -0.25,
                      -0.20, -0.15, -0.10, -0.07, -0.04,
                      -0.01,  0.02,  0.08,  0.30
                      +0.50,  2.00};
    for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
      bins.push_back(edges[bin]);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "zetau_mu") {
    if(HistSet == 26) { //high mass region
      double edges[] = {-1.00, -0.90,
                        -0.55, -0.45, -0.35, -0.25, -0.10,
                        +0.20, +0.40,
                        +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region
      double edges[] = {-1.00,
                        -0.90, -0.55, -0.45, -0.40, -0.35,
                        -0.30, -0.25, -0.20, -0.15, -0.10,
                        -0.05, +0.00, +0.05, +0.10, +0.15,
                        +0.25,
                        +0.40, +0.50,
                        +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 25) { //central mass region
      double edges[] = {-1.00,
                        -0.90, -0.50, -0.40, -0.30,
                        -0.25, -0.20, -0.17, -0.14, -0.11,
                        -0.08, -0.05, -0.01, +0.02, +0.04,
                        +0.06, +0.08, +0.10, +0.12, +0.14,
                        +0.16, +0.18,
                        +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else { //default, nominal score distribution
      double edges[] = {-1.00,
                        -0.90, -0.50, -0.40, -0.30,
                        -0.25, -0.20, -0.17, -0.14, -0.11,
                        -0.08, -0.05, -0.02, +0.01, +0.04,
                        +0.06, +0.08, +0.10, +0.12, +0.14,
                        +0.16, +0.18, +0.30,
                        +0.40,
                        +0.50, +2.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    }
  }
  //perform a sanity check
  for(unsigned bin = 0; bin < bins.size() - 1; ++bin) {
    if(bins[bin] >= bins[bin+1]) {
      std::cout << "MVAConfig::" << __func__ << ": Error! MVA bins for selection " << selection.Data()
                << " not in increasing order!\n";
      throw 20;
    }
  }
  return bins;
}


//-------------------------------------------------------------------------------------------------
// binning assuming the use of the CDF transform
std::vector<Double_t> MVAConfig::CDFBins(Int_t index, Int_t HistSet) {
  std::vector<Double_t> bins;
  HistSet = HistSet % 100; //get the base set number
  TString selection = GetSelectionByIndex(index);

  //FIXME: Temporary hack to test F(p) fits rather than p-value fits
  if(useCDF_ == 2) { //default to evenly spaced 0-1 binning for tests of F(p)
    float default_width = 0.04;
    float xmin(0.f), xmax(1.f);
    /* // Binning for F(p) = p^2 + 1/2*log(p)
    if(selection == "zetau_mu") {
      if(HistSet == 25) {xmin = 0.08f; xmax = 1.00f;} //central mass
      if(HistSet == 26) {xmin = 0.08f; xmax = 0.56f;} //high mass
      if(HistSet == 27) {xmin = 0.08f; xmax = 0.92f;} //low mass
    }
    if(selection == "zmutau_e") {
      if(HistSet == 25) {xmin = 0.00f; xmax = 1.00f;} //central mass
      if(HistSet == 26) {xmin = 0.00f; xmax = 0.56f;} //high mass
      if(HistSet == 27) {xmin = 0.00f; xmax = 0.80f;} //low mass
    }
    if(selection == "zetau") {
      if(HistSet == 25) {xmin = 0.12f; xmax = 1.00f;} //central mass
      if(HistSet == 26) {xmin = 0.00f; xmax = 0.56f;} //high mass
      if(HistSet == 27) {xmin = 0.00f; xmax = 0.84f;} //low mass
      if(HistSet == 28) {xmin = 0.16f; xmax = 1.00f;} //zll mass
    }
    if(selection == "zmutau") {
      if(HistSet == 25) {xmin = 0.00f; xmax = 1.00f;} //central mass
      if(HistSet == 26) {xmin = 0.00f; xmax = 0.64f;} //high mass
      if(HistSet == 27) {xmin = 0.00f; xmax = 0.92f;} //low mass
      if(HistSet == 28) {xmin = 0.00f; xmax = 1.00f;} //zll mass
    }
    */
    /*
    // Binning for F(p) = p^2 + 1/4*log(p)
    if(selection == "zetau_mu") {
    if(HistSet == 25) {xmin = 0.08f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.08f; xmax = 0.44f;} //high mass
    if(HistSet == 27) {xmin = 0.08f; xmax = 1.00f;} //low mass
    }
    if(selection == "zmutau_e") {
    if(HistSet == 25) {xmin = 0.00f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.00f; xmax = 0.56f;} //high mass
    if(HistSet == 27) {xmin = 0.00f; xmax = 0.80f;} //low mass
    }
    if(selection == "zetau") {
    if(HistSet == 25) {xmin = 0.12f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.00f; xmax = 0.52f;} //high mass
    if(HistSet == 27) {xmin = 0.00f; xmax = 0.84f;} //low mass
    if(HistSet == 28) {xmin = 0.12f; xmax = 1.00f;} //zll mass
    }
    if(selection == "zmutau") {
    if(HistSet == 25) {xmin = 0.00f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.00f; xmax = 0.60f;} //high mass
    if(HistSet == 27) {xmin = 0.00f; xmax = 0.92f;} //low mass
    if(HistSet == 28) {xmin = 0.00f; xmax = 1.00f;} //zll mass
    }
    */

    // Binning for F(p) = p^2 + 1/6*log(p)
    if(selection == "zetau_mu") {
    if(HistSet == 25) {xmin = 0.08f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.08f; xmax = 0.28f;} //high mass
    if(HistSet == 27) {xmin = 0.08f; xmax = 0.92f;} //low mass
    }
    if(selection == "zmutau_e") {
    if(HistSet == 25) {xmin = 0.00f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.00f; xmax = 0.36f;} //high mass
    if(HistSet == 27) {xmin = 0.00f; xmax = 0.56f;} //low mass
    }
    if(selection == "zetau") {
    if(HistSet == 25) {xmin = 0.12f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.00f; xmax = 0.36f;} //high mass
    if(HistSet == 27) {xmin = 0.00f; xmax = 0.60f;} //low mass
    if(HistSet == 28) {xmin = 0.12f; xmax = 1.00f;} //zll mass
    }
    if(selection == "zmutau") {
    if(HistSet == 25) {xmin = 0.00f; xmax = 1.00f;} //central mass
    if(HistSet == 26) {xmin = 0.00f; xmax = 0.44f;} //high mass
    if(HistSet == 27) {xmin = 0.00f; xmax = 0.72f;} //low mass
    if(HistSet == 28) {xmin = 0.00f; xmax = 1.00f;} //zll mass
    }

    const int ndefault = std::ceil(1.f/default_width) + 1;
    const int bin_min(std::ceil(xmin/default_width)), bin_max(xmax/default_width);
    if(bin_min < 0 || bin_max >= ndefault) {
      printf("MVAConfig::%s: Bin dimensions not sensible:  [%.3f (bin %i) --> %.3f (bin %i)]\n",
             __func__, xmin, bin_min, xmax, bin_max);
      throw 30;
    }
    if(bin_min != 0) bins.push_back(0.f); //ensure 0 to xmin is a bin
    for(int ibin = bin_min; ibin <= bin_max; ++ibin) bins.push_back(ibin*default_width);
    if(bin_max*default_width < 1.f) bins.push_back(1.f); //ensure xmax to 1 is a bin
    return bins;
  }

  const float default_width = 0.05;
  const int ndefault = std::ceil(1.f/default_width) + 1;


  //-------------------------------------------------------------------------------
  if       (selection == "hmutau") {
    for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    //-------------------------------------------------------------------------------
  } else if(selection == "zmutau") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15,
                        0.20, 0.25, 1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15, 0.20,
                        0.30, 0.40, 0.50, 0.60, 0.70,
                        1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 28) { //z->ll mass region, high scores mostly
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else if(HistSet == 25) { //central mass region, high scores
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else if(HistSet == 28) { //Z->mumu region, high scores
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else { //default, nominal score distribution
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hetau") {
    for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    //-------------------------------------------------------------------------------
  } else if(selection == "zetau") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15,
                        0.20, 1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15, 0.20,
                        0.30, 0.40, 0.50, 0.60,
                        1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 28) { //z->ll mass region, high scores mostly
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else if(HistSet == 25) { //central mass region, high scores mostly
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else if(HistSet == 28) { //z->ee mass region, high scores mostly
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else { //default, nominal score distribution
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hemu") {
    for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    //-------------------------------------------------------------------------------
  } else if(selection == "zemu") {
    for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    //-------------------------------------------------------------------------------
  } else if(selection == "hmutau_e") {
    for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    //-------------------------------------------------------------------------------
  } else if(selection == "zmutau_e") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15,
                        0.20, 1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15,
                        0.20, 0.30, 0.40, 0.50,
                        0.60, 1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 25) { //central mass region, high scores
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else { //default, nominal score distribution
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    }
    //-------------------------------------------------------------------------------
  } else if(selection == "hetau_mu") {
    for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    //-------------------------------------------------------------------------------
  } else if(selection == "zetau_mu") {
    if(HistSet == 26) { //high mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15,
                        0.20, 1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 27) { //low mass region, so low scores
      double edges[] = {-default_width,
                        0.00, 0.05, 0.10, 0.15,
                        0.20, 0.30, 0.40, 0.50,
                        0.60, 0.70, 1.00};
      for(unsigned bin = 0; bin < sizeof(edges)/sizeof(*edges); ++bin) {
        bins.push_back(edges[bin]);
      }
    } else if(HistSet == 25) { //central mass region
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    } else { //default, nominal score distribution
      for(int ibin = -1; ibin < ndefault; ++ibin) bins.push_back(ibin*default_width);
    }
  }
  //perform a sanity check
  for(unsigned bin = 0; bin < bins.size() - 1; ++bin) {
    if(bins[bin] >= bins[bin+1]) {
      std::cout << "MVAConfig::" << __func__ << ": Error! MVA bins for selection " << selection.Data()
                << " not in increasing order!\n";
      throw 20;
    }
  }
  return bins;
}
