#include "interface/ElectronIDWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
ElectronIDWeight::ElectronIDWeight(int Mode, int seed, int verbose) : verbose_(verbose) {
  TFile* f = 0;
  std::vector<TString> file_regions;
  rnd_ = new TRandom3(seed);

  interpolate_ = Mode % 10 == 1;
  const bool useMediumID = (Mode % 100) / 10 == 1;

  typedef std::pair<TString,TString> fpair;
  std::map<int, fpair> electronIDFileNames;
  if(useMediumID) {
    electronIDFileNames[k2016]      = fpair("2016LegacyReReco_ElectronMVA90noiso_Fall17V2.root" ,"EGamma_SF2D");
    electronIDFileNames[k2017]      = fpair("2017_ElectronMVA90noiso.root"                      ,"EGamma_SF2D");
    electronIDFileNames[k2018]      = fpair("2018_ElectronMVA90noiso.root"                      ,"EGamma_SF2D");
  } else {
    electronIDFileNames[k2016]      = fpair("2016LegacyReReco_ElectronMVAwp80.root"             ,"EGamma_SF2D");
    electronIDFileNames[k2017]      = fpair("2017_ElectronMVA80.root"                           ,"EGamma_SF2D");
    electronIDFileNames[k2018]      = fpair("2018_ElectronMVA80.root"                           ,"EGamma_SF2D");
  }
  std::map<int, fpair> electronRecoFileNames;
  electronRecoFileNames[k2016]    = fpair("EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root"           ,"EGamma_SF2D");
  electronRecoFileNames[k2017]    = fpair("egammaEffi.txt_EGM2D_runBCDEF_passingRECO_2017.root" ,"EGamma_SF2D");
  electronRecoFileNames[k2018]    = fpair("egammaEffi.txt_EGM2D_updatedAll_2018.root"           ,"EGamma_SF2D");
  std::map<int, fpair> electronLowRecoFileNames;
  electronLowRecoFileNames[k2016] = fpair("EGM2D_BtoH_low_RecoSF_Legacy2016.root"                ,"EGamma_SF2D");
  electronLowRecoFileNames[k2017] = fpair("egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root" ,"EGamma_SF2D");

  const std::vector<int> working_points = {kWPL, kWP80, kWPLNotWP80};

  //vertex corrections
  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaRunIIRecommendations#HLT_Zvtx_Scale_Factor
  vertexMap_[k2016]   = 1.000; //+-0.000
  vertexMap_[k2017]   = 0.991; //+-0.001
  vertexMap_[k2018]   = 1.000; //+-0.000

  int groupID  = 0; //for systematic grouping
  int groupReco = 0;
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int period = k2016; period <= k2018; ++period) {
    if(verbose_ > 1) printf("--- %s: Initializing %i scale factors\n", __func__, period+2016);

    //////////////////////////////////////////
    //Electron ID files
    //////////////////////////////////////////

    f = TFile::Open((path + "/" + electronIDFileNames[period].first).Data(), "READ");
    if(f) {
      TH2* h = (TH2*) f->Get(electronIDFileNames[period].second.Data());
      if(!h) {
        printf("!!! %s: Electron ID histogram not found for %i!\n", __func__, period+2016);
      } else {
        histID_[period] = h;
        //determine which axis is pT axis
        const bool ptIsX = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()) > 10.;
        const int netagroups = 2;
        const int nptgroups = 4;
        //loop through the histogram, determining the bin groupings
        for(int binx = 1; binx <= h->GetNbinsX(); ++binx) {
          for(int biny = 1; biny <= h->GetNbinsY(); ++biny) {
            int ptgroup(0), etagroup(0);
            const double ptmin  = ((ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                                   h->GetYaxis()->GetBinLowEdge(biny));
            const double etamin = ((!ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                                   h->GetYaxis()->GetBinLowEdge(biny));
            // etamin = abs(etamin);
            if(etamin > 1.39 || etamin < -1.39) etagroup = 1;
            if(ptmin > 49.99) ptgroup = 3;
            else if(ptmin > 34.99) ptgroup = 2;
            else if(ptmin > 19.99) ptgroup = 1;
            const int totgroup = groupID + (etagroup*nptgroups) + ptgroup;
            groupID_[kYear*(period) + kBinY*biny + binx] = totgroup;
            if(verbose_ > 1)
              printf("--- %s ID bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                     __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupID, totgroup);
          }
        }
        groupID += (netagroups)*(nptgroups);
      }
      files_.push_back(f);
    }

    //////////////////////////////////////////
    //Electron Reco ID files
    //////////////////////////////////////////

    f = TFile::Open((path + "/" + electronRecoFileNames[period].first).Data(), "READ");
    if(f) {
      TH2* h = (TH2*) f->Get(electronRecoFileNames[period].second.Data());
      if(!h) {
        printf("!!! %s: Electron Reco ID histogram not found for %i!\n", __func__, period+2016);
      } else {
        histReco_[period] = h;
        //determine which axis is pT axis
        const bool ptIsX = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()) > 10.;
        const int netagroups = 2;
        const int nptgroups = 3;
        //loop through the histogram, determining the bin groupings
        for(int binx = 1; binx <= h->GetNbinsX(); ++binx) {
          for(int biny = 1; biny <= h->GetNbinsY(); ++biny) {
            int ptgroup(0), etagroup(0);
            const double ptmin  = ((ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                                   h->GetYaxis()->GetBinLowEdge(biny));
            const double etamin = ((!ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                                   h->GetYaxis()->GetBinLowEdge(biny));
            // etamin = abs(etamin);
            if(etamin > 1.39 || etamin < -1.39) etagroup = 1;
            // if(ptmin > 49.99) ptgroup = 3;
            if(ptmin > 44.99) ptgroup = 2;
            else if(ptmin > 19.99) ptgroup = 1;
            int totgroup = groupReco + (etagroup*nptgroups) + ptgroup;
            groupReco_[kYear*(period) + kBinY*biny + binx] = totgroup;
            if(verbose_ > 1)
              printf("--- %s Reco bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                     __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupReco, totgroup);
          }
        }
        groupReco += (netagroups)*(nptgroups);
      }
      files_.push_back(f);
    }

    //Low E_T reco corrections
    if(period < k2018) { //2018 uses an inclusive correction file
      f = TFile::Open((path + "/" + electronLowRecoFileNames[period].first).Data(), "READ");
      if(f) {
        TH2* h = (TH2*) f->Get(electronLowRecoFileNames[period].second.Data());
        if(!h) {
          printf("!!! %s: Electron Low Et Reco ID histogram not found for %i!\n", __func__, period+2016);
        } else {
          histLowReco_[period] = h;
        }
        files_.push_back(f);
      }
    }

    //////////////////////////////////////////
    //Electron Trigger files
    //////////////////////////////////////////

    //loop through all working points considered
    for(int WP : working_points) {
      TString fname = Form("egamma_trigger_eff_wp%i_%i.root", WP, period+2016);
      f = TFile::Open((path + "/" + fname).Data(), "READ");
      if(f) {
        TH2* h = (TH2*) f->Get("EGamma_EffData2D");
        if(!h) {
          printf("!!! %s: Electron Trigger Data Eff histogram not found for year %i WP %i!\n", __func__, period+2016, WP);
        } else {
          h->SetName(Form("%s_%i_wp%i", h->GetName(), period+2016, WP));
          histTrigDataEff_[period][WP] = h;
        }
        h = (TH2*) f->Get("EGamma_EffMC2D");
        if(!h) {
          printf("!!! %s: Electron Trigger MC Eff histogram not found for %i WP %i!\n", __func__, period+2016, WP);
        } else {
          h->SetName(Form("%s_%i_%i", h->GetName(), period+2016, WP));
          histTrigMCEff_[period][WP] = h;
        }
        files_.push_back(f);
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
ElectronIDWeight::~ElectronIDWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }

//-------------------------------------------------------------------------------------------------------------------------
double ElectronIDWeight::IDWeight(double pt, double eta, int year,
                                  float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                                  float& weight_rec, float& weight_up_rec, float& weight_down_rec, int& ibin_rec) {
  weight_id  = 1.; weight_up_id  = 1.; weight_down_id  = 1.; ibin_id  = 0;
  weight_rec = 1.; weight_up_rec = 1.; weight_down_rec = 1.; ibin_rec = 0;
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in ElectronIDWeight::" << __func__ << ", returning -1" << std::endl;
    return 1.;
  }

  TH2* hID = histID_[year];
  TH2* hReco = (pt < 20. && year != k2018) ? histLowReco_[year] : histReco_[year];
  if(!hID || !hReco) {
    std::cout << "ElectronIDWeight:: " << __func__ << ": Error! Undefined ID/Reco histograms for year " << year+2016 << std::endl;
    return 1.;
  }

  //Get the ID weight
  int binx = std::max(1, std::min(hID->GetNbinsX(), hID->GetXaxis()->FindBin(eta)));
  int biny = std::max(1, std::min(hID->GetNbinsY(), hID->GetYaxis()->FindBin(pt)));
  const double id_scale = hID->GetBinContent(binx, biny);
  const double id_error = hID->GetBinError(binx, biny);
  ibin_id = 100*biny + binx;

  //Get the reco ID weight
  binx = std::max(1, std::min(hReco->GetNbinsX(), hReco->GetXaxis()->FindBin(eta)));
  biny = std::max(1, std::min(hReco->GetNbinsY(), hReco->GetYaxis()->FindBin(pt)));
  const double reco_scale = hReco->GetBinContent(binx, biny);
  const double reco_error = hReco->GetBinError(binx, biny);
  ibin_rec = 100*biny + binx;

  //FIXME: add pre-fire for 2017
  // double prefire_scale = electronPreFireMap[year];
  const double vertex_scale = vertexMap_[year];
  //FIXME: add vertex scale uncertainty (0.1% on 2017, so not large)

  const double scale_factor = id_scale * reco_scale * vertex_scale;

  if(scale_factor <= 0. || verbose_ > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ElectronIDWeight::" << __func__
              << " year = " << year
              << " id_scale = " << id_scale
              << " reco_scale = " << reco_scale
              << " vertex_scale = " << vertex_scale
              << std::endl;
  }
  //calculate the +- 1 sigma weights
  //FIXME: Currently just combining vertex scale and reco scale
  weight_id = id_scale; weight_rec = vertex_scale * reco_scale;
  weight_up_id    = id_scale + id_error;
  weight_down_id  = id_scale - id_error;
  weight_up_rec   = vertex_scale * (reco_scale + reco_error);
  weight_down_rec = vertex_scale * (reco_scale - reco_error);

  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double ElectronIDWeight::EmbedEnergyScale(double pt, double eta, int year, float& up, float& down) {
  up = 1.; down = 1.;
  double scale_factor = 1.;
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in ElectronIDWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }

  pt = std::max(20., pt); //not needed here

  const bool barrel = std::fabs(eta) < 1.479;
  if(year == k2016) {
    if(barrel) { scale_factor -= 0.0024; up = scale_factor + 0.0050; down = scale_factor - 0.0050; }
    else       { scale_factor -= 0.0070; up = scale_factor + 0.0125; down = scale_factor - 0.0125; }
  }
  else if(year == k2017) {
    if(barrel) { scale_factor -= 0.0007; up = scale_factor + 0.0050; down = scale_factor - 0.0050; }
    else       { scale_factor -= 0.0113; up = scale_factor + 0.0125; down = scale_factor - 0.0125; }
  }
  else if(year == k2018) {
    if(barrel) { scale_factor -= 0.0033; up = scale_factor + 0.0050; down = scale_factor - 0.0050; }
    else       { scale_factor -= 0.0056; up = scale_factor + 0.0125; down = scale_factor - 0.0125; }
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double ElectronIDWeight::TriggerEff(double pt, double eta, int year, int WP, float& data_eff, float& mc_eff) {
  data_eff = 0.5; //safer default than 0 or 1, as eff and 1-eff are well defined in ratios
  mc_eff = 0.5;
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in " << __func__ << ", returning -1" << std::endl;
    return 1.;
  }
  const double trig_pt = (year == k2016) ? 28. : 33.;
  //can't fire a trigger if below the threshold
  if(pt < trig_pt) {
    return 1.;
  }

  //ensure within scale factor region
  eta = std::min(2.49, std::max(-2.49, eta));
  pt  = std::min(499., pt);

  TH2* hTrigData = histTrigDataEff_[year][WP];
  TH2* hTrigMC = histTrigMCEff_[year][WP];
  if(!hTrigData) {
    std::cout << "!!! " << __func__ << ": Undefined Data trigger efficiency histogram for " << year << std::endl;
    return 1.;
  }
  if(!hTrigMC) {
    std::cout << "!!! " << __func__ << ": Undefined MC trigger efficiency histogram for " << year << std::endl;
    return 1.;
  }

  //check if using eta or |eta|
  if(hTrigData->GetXaxis()->GetBinLowEdge(1) > -0.1) eta = std::fabs(eta);

  if(interpolate_) {
    data_eff = Utilities::Interpolate(hTrigData, eta, pt, Utilities::kYAxis);
    mc_eff   = Utilities::Interpolate(hTrigMC  , eta, pt, Utilities::kYAxis);
  } else {
    const int data_bin_x = std::max(1, std::min(hTrigData->GetNbinsX(), hTrigData->GetXaxis()->FindBin(eta)));
    const int data_bin_y = std::max(1, std::min(hTrigData->GetNbinsY(), hTrigData->GetYaxis()->FindBin(pt )));
    data_eff = hTrigData->GetBinContent(data_bin_x, data_bin_y);
    const int mc_bin_x   = std::max(1, std::min(hTrigMC  ->GetNbinsX(), hTrigMC  ->GetXaxis()->FindBin(eta)));
    const int mc_bin_y   = std::max(1, std::min(hTrigMC  ->GetNbinsY(), hTrigMC  ->GetYaxis()->FindBin(pt )));
    mc_eff   = hTrigMC  ->GetBinContent(mc_bin_x  , mc_bin_y  );
  }

  //ensure reasonable efficiencies
  const float min_eff = 1.e-4;
  data_eff = std::min(1.f - min_eff, std::max(min_eff, data_eff));
  mc_eff   = std::min(1.f - min_eff, std::max(min_eff, mc_eff  ));

  const double scale_factor = (mc_eff > 0.) ? data_eff / mc_eff : 1.;
  if(scale_factor <= 0. || verbose_ > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "ElectronIDWeight::" << __func__
              << " year = " << year + 2016
              << " data_eff = " << data_eff
              << " mc_eff = " << mc_eff
              << std::endl;
  }
  return scale_factor;
}
