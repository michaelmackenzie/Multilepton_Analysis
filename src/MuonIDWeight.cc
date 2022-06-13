#include "interface/MuonIDWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
MuonIDWeight::MuonIDWeight(const int Mode, const int seed, const int verbose) : Mode_(Mode), verbose_(verbose) {
  TFile* f = 0;
  std::vector<TString> file_regions;
  rnd_ = new TRandom3(seed);

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";

  typedef std::pair<TString,TString> fpair;
  std::map<int, fpair> muonIDFileNames;
  std::map<int, fpair> muonIsoFileNames;

  const bool useMediumID = Mode % 10 == 1;
  const TString idName  = (useMediumID) ? "MediumID" : "TightID";
  const TString isoName = (useMediumID) ? "MediumID" : "TightIDandIPCut";
  muonIDFileNames[2*k2016]    = fpair("RunBCDEF_SF_ID_muon_2016.root", Form("NUM_%s_DEN_genTracks_eta_pt"      , idName.Data()));
  muonIDFileNames[2*k2016+1]  = fpair("RunGH_SF_ID_muon_2016.root"   , Form("NUM_%s_DEN_genTracks_eta_pt"      , idName.Data()));
  muonIDFileNames[2*k2017]    = fpair("2017_Mu_RunBCDEF_SF_ID.root"  , Form("NUM_%s_DEN_genTracks_pt_abseta"   , idName.Data()));
  muonIDFileNames[2*k2017+1]  = fpair("2017_Mu_RunBCDEF_SF_ID.root"  , Form("NUM_%s_DEN_genTracks_pt_abseta"   , idName.Data()));
  muonIDFileNames[2*k2018]    = fpair("RunABCD_SF_ID_muon_2018.root" , Form("NUM_%s_DEN_TrackerMuons_pt_abseta", idName.Data()));
  muonIDFileNames[2*k2018+1]  = fpair("RunABCD_SF_ID_muon_2018.root" , Form("NUM_%s_DEN_TrackerMuons_pt_abseta", idName.Data()));

  muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root", Form("NUM_TightRelIso_DEN_%s_eta_pt"   , isoName.Data()));
  muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   , Form("NUM_TightRelIso_DEN_%s_eta_pt"   , isoName.Data()));
  muonIsoFileNames[2*k2017]   = fpair("2017_Mu_RunBCDEF_SF_ISO.root"  , Form("NUM_TightRelIso_DEN_%s_pt_abseta", isoName.Data()));
  muonIsoFileNames[2*k2017+1] = fpair("2017_Mu_RunBCDEF_SF_ISO.root"  , Form("NUM_TightRelIso_DEN_%s_pt_abseta", isoName.Data()));
  muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" , Form("NUM_TightRelIso_DEN_%s_pt_abseta", isoName.Data()));
  muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" , Form("NUM_TightRelIso_DEN_%s_pt_abseta", isoName.Data()));

  //Trigger info
  std::map<int, fpair> muonTriggerLowFileNames;
  muonTriggerLowFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"IsoMu24_OR_IsoTkMu24_PtEtaBins");
  muonTriggerLowFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"IsoMu24_OR_IsoTkMu24_PtEtaBins");
  muonTriggerLowFileNames[2*k2017]    = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins");
  muonTriggerLowFileNames[2*k2017+1]  = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins");
  muonTriggerLowFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins");
  muonTriggerLowFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"IsoMu24_PtEtaBins");

  std::map<int, fpair> muonTriggerHighFileNames;
  muonTriggerHighFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"Mu50_OR_TkMu50_PtEtaBins");
  muonTriggerHighFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"Mu50_OR_TkMu50_PtEtaBins");
  muonTriggerHighFileNames[2*k2017]   = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins");
  muonTriggerHighFileNames[2*k2017+1] = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins");
  muonTriggerHighFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins");
  muonTriggerHighFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins");

  int groupID  = 0; //for systematic grouping
  int groupIso = 0;
  for(int period = k2016; period < 2*(k2018 + 1); ++period) {

    ///////////////////////////////////
    // Muon ID
    ///////////////////////////////////

    if(verbose_ > 1) printf("--- %s: Initializing %i scale factors\n", __func__, period/2+2016);
    f = TFile::Open((path + "/" + muonIDFileNames[period].first).Data(), "READ");
    if(f) {
      TH2* h = (TH2*) f->Get(muonIDFileNames[period].second.Data());
      if(!h) {
        printf("!!! %s: Muon ID histogram not found for %i!\n", __func__, period/2+2016);
      } else {
        h->SetDirectory(0);
        h->SetName(Form("%s_period_%i", h->GetName(), period));
        histIDData_[period] = h;
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
            const double etamin = std::fabs(((!ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                                             h->GetYaxis()->GetBinLowEdge(biny)));
            if(etamin > 1.15) etagroup = 1;
            if(ptmin > 49.99) ptgroup = 3;
            else if(ptmin > 39.99) ptgroup = 2;
            else if(ptmin > 24.99) ptgroup = 1;
            const int totgroup = groupID + (etagroup*nptgroups) + ptgroup;
            groupID_[kYear*(period/2) + kRunSection*(period % 2 == 0) + kBinY*biny + binx] = totgroup;
            if(verbose_ > 1)
              printf("--- %s ID bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                     __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupID, totgroup);
          }
        }
        if(period % 2 == 1)
          groupID += (netagroups)*(nptgroups);
      }
      f->Close();
    }

    ///////////////////////////////////
    // Muon Iso ID
    ///////////////////////////////////

    f = TFile::Open((path + "/" + muonIsoFileNames[period].first).Data(), "READ");
    if(f) {
      TH2* h = (TH2*) f->Get(muonIsoFileNames[period].second.Data());
      if(!h) {
        printf("!!! %s: Muon Iso ID histogram not found for %i!\n", __func__, period/2+2016);
      } else {
        h->SetDirectory(0);
        h->SetName(Form("%s_period_%i", h->GetName(), period));
        histIsoData_[period] = h;
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
            const double etamin = std::fabs(((!ptIsX) ? h->GetXaxis()->GetBinLowEdge(binx) :
                                             h->GetYaxis()->GetBinLowEdge(biny)));
            if(etamin > 1.15) etagroup = 1;
            if(ptmin > 49.99) ptgroup = 3;
            else if(ptmin > 39.99) ptgroup = 2;
            else if(ptmin > 24.99) ptgroup = 1;
            const int totgroup = groupIso + (etagroup*nptgroups) + ptgroup;
            groupIso_[kYear*(period/2) + kRunSection*(period % 2 == 0) + kBinY*biny + binx] = totgroup;
            if(verbose_ > 1)
              printf("--- %s Iso bin (%i,%i) with min (%.1f,%.1f) has group (pt,eta) + offset = (%i, %i) + %i = %i\n",
                     __func__, binx, biny, ptmin, etamin, ptgroup, etagroup, groupIso, totgroup);
          }
        }
        if(period % 2 == 1)
          groupIso += (netagroups)*(nptgroups);
      }
      f->Close();
    }

    ///////////////////////////////////
    // Muon low trigger
    ///////////////////////////////////

    f = TFile::Open((path + "/" + muonTriggerLowFileNames[period].first).Data(), "READ");
    if(f) {
      TH2* h = (TH2*) f->Get((muonTriggerLowFileNames[period].second + "/efficienciesDATA/abseta_pt_DATA").Data());
      if(!h) {
        printf("!!! %s: Muon low trigger data histogram not found for %i!\n", __func__, period/2+2016);
      } else {
        h->SetDirectory(0);
        h->SetName(Form("%s_period_%i", h->GetName(), period));
        histTriggerLowData_[period] = h;
      }
      h = (TH2*) f->Get((muonTriggerLowFileNames[period].second + "/efficienciesMC/abseta_pt_MC").Data());
      if(!h) {
        printf("!!! %s: Muon low trigger MC histogram not found for %i!\n", __func__, period/2+2016);
      } else {
        h->SetDirectory(0);
        h->SetName(Form("%s_period_%i", h->GetName(), period));
        histTriggerLowMC_[period] = h;
      }
      f->Close();
    }

    ///////////////////////////////////
    // Muon high trigger
    ///////////////////////////////////

    f = TFile::Open((path + "/" + muonTriggerHighFileNames[period].first).Data(), "READ");
    if(f) {
      TH2* h = (TH2*) f->Get((muonTriggerHighFileNames[period].second + "/efficienciesDATA/abseta_pt_DATA").Data());
      if(!h) {
        printf("!!! %s: Muon high trigger data histogram not found for %i!\n", __func__, period/2+2016);
      } else {
        h->SetDirectory(0);
        h->SetName(Form("%s_period_%i", h->GetName(), period));
        histTriggerHighData_[period] = h;
      }
      h = (TH2*) f->Get((muonTriggerHighFileNames[period].second + "/efficienciesMC/abseta_pt_MC").Data());
      if(!h) {
        printf("!!! %s: Muon high trigger MC histogram not found for %i!\n", __func__, period/2+2016);
      } else {
        h->SetDirectory(0);
        h->SetName(Form("%s_period_%i", h->GetName(), period));
        histTriggerHighMC_[period] = h;
      }
      f->Close();
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
MuonIDWeight::~MuonIDWeight() {
  if(rnd_) delete rnd_;
  for(std::pair<int, TH2*> val : histIDData_         ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : histIsoData_        ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : histTriggerLowData_ ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : histTriggerLowMC_   ) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : histTriggerHighData_) {if(val.second) delete val.second;}
  for(std::pair<int, TH2*> val : histTriggerHighMC_  ) {if(val.second) delete val.second;}
}


//-------------------------------------------------------------------------------------------------------------------------
void MuonIDWeight::IDWeight(double pt, double eta, int year, int mcEra,
                            float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                            float& weight_iso, float& weight_up_iso, float& weight_down_iso, int& ibin_iso
                            ) {
  if(year > 2000) year -= 2016;
  weight_id  = 1.; weight_up_id  = 1.; weight_down_id  = 1.; ibin_id  = 0;
  weight_iso = 1.; weight_up_iso = 1.; weight_down_iso = 1.; ibin_iso = 0;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in MuonIDWeight::" << __func__ << ", returning -1" << std::endl;
    return;
  }

  mcEra = std::min(1, std::max(0, mcEra));
  TH2* hID = histIDData_[2*year + mcEra];

  //axes flip between years for some reason
  const double xvar = (year != 2016) ? pt : eta;
  const double yvar = (year != 2016) ? std::fabs(eta) : pt;
  int binx = std::max(1, std::min(hID->GetNbinsX(), hID->GetXaxis()->FindBin(xvar)));
  int biny = std::max(1, std::min(hID->GetNbinsY(), hID->GetYaxis()->FindBin(yvar)));
  const double id_scale = hID->GetBinContent(binx, biny);
  const double id_error = hID->GetBinError(binx, biny);
  ibin_id = 10000 * (1 - mcEra) + 100*biny + binx; //assumes binx and biny < 100

  TH2* hIso = histIsoData_[2*year + mcEra];

  binx = std::max(1, std::min(hIso->GetNbinsX(), hIso->GetXaxis()->FindBin(xvar)));
  biny = std::max(1, std::min(hIso->GetNbinsY(), hIso->GetYaxis()->FindBin(yvar)));
  const double iso_scale = hIso->GetBinContent(binx, biny);
  const double iso_error = hIso->GetBinError(binx, biny);
  ibin_iso = 10000*(1 - mcEra) + 100*biny + binx; //assumes binx and biny < 100


  double scale_factor = id_scale * iso_scale;
  if(scale_factor <= 0. || verbose_ > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "MuonIDWeight::" << __func__
              << " year = " << year
              << " id_scale = " << id_scale << " +- " << id_error
              << " iso_scale = " << iso_scale << " +- " << iso_error
              << " mcEra = " << mcEra << std::endl;
  }
  weight_id = id_scale; weight_iso = iso_scale;

  weight_up_id    = weight_id  +  id_error;
  weight_down_id  = weight_id  -  id_error;
  weight_up_iso   = weight_iso + iso_error;
  weight_down_iso = weight_iso - iso_error;
}

//-------------------------------------------------------------------------------------------------------------------------
double MuonIDWeight::TriggerEff(double pt, double eta, int year, bool isLow, int mcEra, float& data_eff, float& mc_eff) {
  data_eff = 0.5; //safer default than 0 or 1, as eff and 1-eff are well defined in ratios
  mc_eff = 0.5;
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in MuonIDWeight::" << __func__ << ", returning -1" << std::endl;
    return 1.;
  }
  //can't fire a trigger if below the threshold
  if((year == k2016 && pt < 24.) || (year == k2017 && pt < 27.) || (year == k2018 && pt < 24.)) {
    return 1.;
  }
  if(!isLow && pt < 50.) {
    return 1.;
  }
  eta = std::fabs(eta);
  mcEra = std::min(1, std::max(0, mcEra)); //ensure MC era is 0 or 1

  TH2* hTrigData = (isLow) ? histTriggerLowData_[2*year + mcEra] : histTriggerHighData_[2*year + mcEra];
  TH2* hTrigMC   = (isLow) ? histTriggerLowMC_  [2*year + mcEra] : histTriggerHighMC_  [2*year + mcEra];
  if(!hTrigData) {
    std::cout << "!!! " << __func__ << ": Undefined Data trigger efficiency histogram for " << year
              << " low trigger = " << isLow << " MC era = " << mcEra << std::endl;
    return 1.;
  }
  if(!hTrigMC) {
    std::cout << "!!! " << __func__ << ": Undefined MC trigger efficiency histogram for " << year
              << " low trigger = " << isLow << " MC era = " << mcEra << std::endl;
    return 1.;
  }
  const int data_binx = std::max(1, std::min(hTrigData->GetNbinsX(), hTrigData->GetXaxis()->FindBin(eta)));
  const int data_biny = std::max(1, std::min(hTrigData->GetNbinsY(), hTrigData->GetYaxis()->FindBin(pt)));
  data_eff = hTrigData->GetBinContent(data_binx, data_biny);
  const int mc_binx   = std::max(1, std::min(hTrigMC  ->GetNbinsX(), hTrigMC  ->GetXaxis()->FindBin(eta)));
  const int mc_biny   = std::max(1, std::min(hTrigMC  ->GetNbinsY(), hTrigMC  ->GetYaxis()->FindBin(pt)));
  mc_eff   = hTrigMC  ->GetBinContent(mc_binx  , mc_biny  );

  const double scale_factor = (mc_eff > 0.) ? data_eff / mc_eff : 1.;
  if(!std::isfinite(scale_factor) || scale_factor <= 0. || verbose_ > 0) {
    if(!std::isfinite(scale_factor) || scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0 or nan! ";
    std::cout << "MuonIDWeight::" << __func__
              << " year = " << year + 2016
              << " MC era = " << mcEra
              << " pt = " << pt
              << " eta = " << eta
              << " data_eff = " << data_eff
              << " mc_eff = " << mc_eff
              << " scale = " << scale_factor
              << std::endl;
    if(!std::isfinite(scale_factor) || scale_factor <= 0.) return 1.;
  }
  return scale_factor;
}
