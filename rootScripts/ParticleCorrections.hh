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
#include "TF1.h"

class ParticleCorrections : public TObject {
public :
  ParticleCorrections() : ParticleCorrections(kTightMuIso) {}
  ParticleCorrections(int muIsoLevel) {
    fRnd = new TRandom3(90);
    typedef pair<TString,TString> fpair;
    std::map<int, fpair> muonIDFileNames;
    muonIDFileNames[2*k2016]    = fpair("RunBCDEF_SF_ID_muon_2016.root","NUM_TightID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2016+1]  = fpair("RunGH_SF_ID_muon_2016.root"   ,"NUM_TightID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2017]    = fpair("2017_Mu_RunBCDEF_SF_ID.root"  ,"NUM_TightID_DEN_genTracks_pt_abseta");
    muonIDFileNames[2*k2017+1]  = fpair("2017_Mu_RunBCDEF_SF_ID.root"  ,"NUM_TightID_DEN_genTracks_pt_abseta");
    muonIDFileNames[2*k2018]    = fpair("RunABCD_SF_ID_muon_2018.root" ,"NUM_TightID_DEN_TrackerMuons_pt_abseta");
    muonIDFileNames[2*k2018+1]  = fpair("RunABCD_SF_ID_muon_2018.root" ,"NUM_TightID_DEN_TrackerMuons_pt_abseta");
    std::map<int, fpair> muonIsoFileNames;
    //FIXME: Figure out correct scale factors
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
    } else if(muIsoLevel == kTightMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt");
      muonIsoFileNames[2*k2017]   = fpair("2017_Mu_RunBCDEF_SF_ISO.root"  ,"NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
      muonIsoFileNames[2*k2017+1] = fpair("2017_Mu_RunBCDEF_SF_ISO.root"  ,"NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta");
    } else if(muIsoLevel == kVTightMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
    }

    //Trigger map for low and high pT trigger
    std::map<int, fpair> muonTriggerLowFileNames;
    std::map<int, fpair> muonTriggerHighFileNames;
    muonTriggerLowFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2017]    = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2017+1]  = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"IsoMu24_PtEtaBins/abseta_pt_ratio");

    muonTriggerHighFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2017]   = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2017+1] = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");

    //initialize ID histograms
    TString scaleFactorPath = gSystem->Getenv("CMSSW_BASE") + fScaleFactorPath;
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonIDMap[period] = (TH2F*) f->Get(muonIDFileNames[period].second.Data());
      if(!muonIDMap[period])
	std::cout << "Error! Histogram " << muonIDFileNames[period].second.Data() << " in "
		  << muonIDFileNames[period].first.Data() << " not found!\n";
    }

    //initialize ISO histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonIsoFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonIsoFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonIsoMap[period] = (TH2F*) f->Get(muonIsoFileNames[period].second.Data());
      if(!muonIsoMap[period])
	std::cout << "Error! Histogram " << muonIsoFileNames[period].second.Data() << " in "
		  << muonIsoFileNames[period].first.Data() << " not found!\n";
    }

    //initialize Low Trigger histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerLowFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerLowFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonLowTriggerMap[period] = (TH2F*) f->Get(muonTriggerLowFileNames[period].second.Data());
      if(!muonLowTriggerMap[period])
	std::cout << "Error! Histogram " << muonTriggerLowFileNames[period].second.Data() << " in "
		  << muonTriggerLowFileNames[period].first.Data() << " not found!\n";
    }

    //initialize High Trigger histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerHighFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerHighFileNames[period].first).Data(),"READ");
      if(!f) continue;
      muonHighTriggerMap[period] = (TH2F*) f->Get(muonTriggerHighFileNames[period].second.Data());
      if(!muonHighTriggerMap[period])
	std::cout << "Error! Histogram " << muonTriggerHighFileNames[period].second.Data() << " in "
		  << muonTriggerHighFileNames[period].first.Data() << " not found!\n";
    }

    //initialize Electron scales
    std::map<int, fpair> electronIDFileNames;
    electronIDFileNames[k2016]    = fpair("2016LegacyReReco_ElectronMVAwp80.root","EGamma_SF2D");
    electronIDFileNames[k2017]    = fpair("2017_ElectronMVA80.root"              ,"EGamma_SF2D");
    electronIDFileNames[k2018]    = fpair("2018_ElectronMVA80.root"              ,"EGamma_SF2D");
    std::map<int, fpair> electronRecoFileNames;
    electronRecoFileNames[k2016]  = fpair("EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root"           ,"EGamma_SF2D");
    electronRecoFileNames[k2017]  = fpair("egammaEffi.txt_EGM2D_runBCDEF_passingRECO_2017.root" ,"EGamma_SF2D");
    electronRecoFileNames[k2018]  = fpair("egammaEffi.txt_EGM2D_updatedAll_2018.root"           ,"EGamma_SF2D");

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
    
    //initialize Tau scales
    TString tauScaleFactorPath = gSystem->Getenv("CMSSW_BASE") + fTauScaleFactorPath;
    std::map<int, fpair> tauJetIDFileNames;
    tauJetIDFileNames[k2016]    = fpair("TauID_SF_pt_DeepTau2017v2p1VSjet_2016Legacy.root","Medium");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauJetIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauJetIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauJetIDMap[period] = (TF1*) f->Get((tauJetIDFileNames[period].second+"_cent").Data());
      tauJetUpIDMap[period] = (TF1*) f->Get((tauJetIDFileNames[period].second+"_up").Data());
      tauJetDownIDMap[period] = (TF1*) f->Get((tauJetIDFileNames[period].second+"_down").Data());
      if(!tauJetIDMap[period]) cout << "Error! tauJetID " << tauJetIDFileNames[period].second.Data() << " TF1 not found!\n";
      if(!tauJetUpIDMap[period]) cout << "Error! tauJetUpID " << tauJetIDFileNames[period].second.Data() << " TF1 not found!\n";
      if(!tauJetDownIDMap[period]) cout << "Error! tauJetDownID " << tauJetIDFileNames[period].second.Data() << " TF1 not found!\n";
    }
    std::map<int, fpair> tauEleIDFileNames;
    tauEleIDFileNames[k2016]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2016Legacy.root","Medium");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauEleIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauEleIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauEleIDMap[period] = (TH1F*) f->Get((tauEleIDFileNames[period].second).Data());
    }
    std::map<int, fpair> tauMuIDFileNames;
    tauMuIDFileNames[k2016]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2016Legacy.root","Tight");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauMuIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauMuIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauMuIDMap[period] = (TH1F*) f->Get((tauMuIDFileNames[period].second).Data());
    }
    
  }
  virtual ~ParticleCorrections() {}
  virtual double MuonWeight    (double pt, double eta, int trigger, int era);
  virtual double ElectronWeight(double pt, double eta, int era);
  virtual double TauWeight     (double pt, double eta, int genID, int era, double& up, double& down);
  double TauWeight(double pt, double eta, int genID, int era) {
    double up(1.), down(1.);
    return TauWeight(pt, eta, genID, era, up, down);
  }

  enum{k2016, k2017, k2018}; //defined years
  enum{kLowTrigger, kHighTrigger}; //defined triggers
  enum{kLooseMuIso, kMediumMuIso, kTightMuIso, kVTightMuIso}; //define iso scale factor sets

  TString fScaleFactorPath    = "/src/StandardModel/ZEMuAnalysis/test/scale_factors/"; //path from cmssw_base
  TString fTauScaleFactorPath = "/src/TauPOG/TauIDSFs/data/"; //path from cmssw_base
  //muon corrections
  std::map<int, TH2F*> muonIDMap;
  std::map<int, TH2F*> muonIsoMap;
  std::map<int, TH2F*> muonLowTriggerMap;
  std::map<int, TH2F*> muonHighTriggerMap;
  //electron corrections
  std::map<int, TH2F*> electronIDMap;
  std::map<int, TH2F*> electronRecoMap;
  //tau corrections
  std::map<int, TF1*> tauJetIDMap;
  std::map<int, TF1*> tauJetUpIDMap;
  std::map<int, TF1*> tauJetDownIDMap;
  std::map<int, TH1F*> tauEleIDMap;
  std::map<int, TH1F*> tauMuIDMap;
  //b-jet corrections
  std::map<int, TH2F*> bJetIDMap;
  
  TRandom* fRnd; //for getting random period in year
};
#endif // #ifdef ParticleCorrections_cxx
