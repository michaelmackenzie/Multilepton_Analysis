#ifndef ParticleCorrections_hh
#define ParticleCorrections_hh

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TFile.h"
#include "TRandom3.h"
#include "TSystem.h"

class ParticleCorrections : public TObject {
public :
  ParticleCorrections() : ParticleCorrections(kTightMuIso) {}
  ParticleCorrections(int muIsoLevel) {
    fRnd = new TRandom3(90);
    typedef pair<TString,TString> fpair;
    std::map<int, fpair> muonIDFileNames;
    muonIDFileNames[2*k2016]    = fpair("RunBCDEF_SF_ID_muon_2016.root","NUM_MediumID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2016+1]  = fpair("RunGH_SF_ID_muon_2016.root"   ,"NUM_MediumID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2018]    = fpair("RunABCD_SF_ID_muon_2018.root" ,"NUM_MediumID_DEN_TrackerMuons_pt_abseta");
    muonIDFileNames[2*k2018+1]  = fpair("RunABCD_SF_ID_muon_2018.root" ,"NUM_MediumID_DEN_TrackerMuons_pt_abseta");
    std::map<int, fpair> muonIsoFileNames;
    if(muIsoLevel == kLooseMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_LooseID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_LooseID_eta_pt");
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_LooseID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_LooseID_pt_abseta");
    } else if(muIsoLevel == kMediumMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
    } else if(muIsoLevel == kTightMuIso) { //FIXME: Figure out correct scale factors
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
    } else if(muIsoLevel == kVTightMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
    }

    //Trigger map for low and high pT trigger
    std::map<int, fpair> muonTriggerLowFileNames;
    std::map<int, fpair> muonTriggerHighFileNames;
    muonTriggerLowFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root","IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root","IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root","IsoMu24_PtEtaBins/abseta_pt_ratio");

    muonTriggerHighFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root","Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root","Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");

    //initialize ID histograms
    TString scaleFactorPath = gSystem->Getenv("CMSSW_BASE") + fScaleFactorPath;
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonIDMap[period] = (TH2F*) f->Get(muonIDFileNames[period].second.Data());
    }

    //initialize ISO histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonIsoFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonIsoFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonIsoMap[period] = (TH2F*) f->Get(muonIsoFileNames[period].second.Data());
    }

    //initialize Low Trigger histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerLowFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerLowFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonLowTriggerMap[period] = (TH2F*) f->Get(muonTriggerLowFileNames[period].second.Data());
    }

    //initialize High Trigger histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerHighFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerHighFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonHighTriggerMap[period] = (TH2F*) f->Get(muonTriggerHighFileNames[period].second.Data());
    }

    //initialize Electron scales
    std::map<int, fpair> electronIDFileNames;
    electronIDFileNames[k2016]    = fpair("2016LegacyReReco_ElectronMVAwp80.root","EGamma_SF2D");
    electronIDFileNames[k2018]    = fpair("2016LegacyReReco_ElectronMVAwp80.root","EGamma_SF2D");
    std::map<int, fpair> electronRecoFileNames;
    electronRecoFileNames[k2016]  = fpair("2018_ElectronMVA80.root","EGamma_SF2D");
    electronRecoFileNames[k2018]  = fpair("egammaEffi.txt_EGM2D_updatedAll_2018.root","EGamma_SF2D");

    for(int period = k2016; period <= k2018; ++period) {
      if(electronIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronIDMap[period] = (TH2F*) f->Get(electronIDFileNames[period].second.Data());
    }
    for(int period = k2016; period <= k2018; ++period) {
      if(electronRecoFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronRecoFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronRecoMap[period] = (TH2F*) f->Get(electronRecoFileNames[period].second.Data());
    }
    
  }
  virtual ~ParticleCorrections() {}
  virtual double MuonWeight    (double pt, double eta, int trigger, int era);
  virtual double ElectronWeight(double pt, double eta, int era);

  enum{k2016, k2017, k2018}; //defined years
  enum{kLowTrigger, kHighTrigger}; //defined triggers
  enum{kLooseMuIso, kMediumMuIso, kTightMuIso, kVTightMuIso}; //define iso scale factor sets

  TString fScaleFactorPath = "/src/StandardModel/ZEMuAnalysis/test/scale_factors/"; //path from cmssw_base
  std::map<int, TH2F*> muonIDMap;
  std::map<int, TH2F*> muonIsoMap;
  std::map<int, TH2F*> muonLowTriggerMap;
  std::map<int, TH2F*> muonHighTriggerMap;
  std::map<int, TH2F*> electronIDMap;
  std::map<int, TH2F*> electronRecoMap;

  TRandom* fRnd; //for getting random period in year
};
#endif // #ifdef ParticleCorrections_cxx
