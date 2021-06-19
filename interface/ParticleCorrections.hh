#ifndef ParticleCorrections_hh
#define ParticleCorrections_hh

//c++ includes
#include <map>
#include <iostream>

//ROOT includes
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphAsymmErrors.h"
#include "TObject.h"
#include "TFile.h"
#include "TRandom3.h"
#include "TSystem.h"
#include "TF1.h"

class ParticleCorrections {
public :
  ParticleCorrections() : ParticleCorrections(kTightMuIso) {}
  ParticleCorrections(int muIsoLevel) {
    fRnd = new TRandom3(90);
    typedef std::pair<TString,TString> fpair;
    std::map<int, fpair> muonIDFileNames;
    //FIXME: Clone objects and close files after retrieving them

    muonIDFileNames[2*k2016]    = fpair("RunBCDEF_SF_ID_muon_2016.root","NUM_TightID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2016+1]  = fpair("RunGH_SF_ID_muon_2016.root"   ,"NUM_TightID_DEN_genTracks_eta_pt");
    muonIDFileNames[2*k2017]    = fpair("2017_Mu_RunBCDEF_SF_ID.root"  ,"NUM_TightID_DEN_genTracks_pt_abseta");
    muonIDFileNames[2*k2017+1]  = fpair("2017_Mu_RunBCDEF_SF_ID.root"  ,"NUM_TightID_DEN_genTracks_pt_abseta");
    muonIDFileNames[2*k2018]    = fpair("RunABCD_SF_ID_muon_2018.root" ,"NUM_TightID_DEN_TrackerMuons_pt_abseta");
    muonIDFileNames[2*k2018+1]  = fpair("RunABCD_SF_ID_muon_2018.root" ,"NUM_TightID_DEN_TrackerMuons_pt_abseta");
    std::map<int, fpair> muonIsoFileNames;
    if(muIsoLevel == kLooseMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_LooseID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_LooseID_eta_pt");
      //FIXME: Add 2017 muon iso scale factors
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_LooseID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_LooseID_pt_abseta");
    } else if(muIsoLevel == kMediumMuIso) {
      muonIsoFileNames[2*k2016]   = fpair("RunBCDEF_SF_ISO_muon_2016.root","NUM_LooseRelIso_DEN_MediumID_eta_pt");
      muonIsoFileNames[2*k2016+1] = fpair("RunGH_SF_ISO_muon_2016.root"   ,"NUM_LooseRelIso_DEN_MediumID_eta_pt");
      //FIXME: Add 2017 muon iso scale factors
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
      //FIXME: Add 2017 muon iso scale factors
      muonIsoFileNames[2*k2018]   = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
      muonIsoFileNames[2*k2018+1] = fpair("RunABCD_SF_ISO_muon_2018.root" ,"NUM_LooseRelIso_DEN_MediumID_pt_abseta");
    }

    //initialize ID histograms
    TString scaleFactorPath = gSystem->Getenv("CMSSW_BASE") + fScaleFactorPath;
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonIDFileNames[period].first == "") { muonIDMap[period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonIDFileNames[period].first).Data(),"READ");
      if(!f) { muonIDMap[period] = 0; continue; }
      muonIDMap[period] = (TH2F*) f->Get(muonIDFileNames[period].second.Data())->Clone();
      if(!muonIDMap[period])
        std::cout << "Error! Histogram " << muonIDFileNames[period].second.Data() << " in "
                  << muonIDFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //initialize ISO histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonIsoFileNames[period].first == "") { muonIsoMap[period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonIsoFileNames[period].first).Data(),"READ");
      if(!f) { muonIsoMap[period] = 0; continue; }
      muonIsoMap[period] = (TH2F*) f->Get(muonIsoFileNames[period].second.Data())->Clone();
      if(!muonIsoMap[period])
        std::cout << "Error! Histogram " << muonIsoFileNames[period].second.Data() << " in "
                  << muonIsoFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //Trigger map for low and high pT trigger
    std::map<int, fpair> muonTriggerLowFileNames;
    std::map<int, fpair> muonTriggerLowDataEffFileNames;
    std::map<int, fpair> muonTriggerLowMCEffFileNames;
    std::map<int, fpair> muonTriggerHighFileNames;
    std::map<int, fpair> muonTriggerHighDataEffFileNames;
    std::map<int, fpair> muonTriggerHighMCEffFileNames;

    //////////////////////
    // Low Muon Trigger //
    //////////////////////

    muonTriggerLowFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2017]    = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2017+1]  = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins/abseta_pt_ratio");
    muonTriggerLowFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"IsoMu24_PtEtaBins/abseta_pt_ratio");

    //Data efficiencies
    muonTriggerLowDataEffFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerLowDataEffFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerLowDataEffFileNames[2*k2017]    = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerLowDataEffFileNames[2*k2017+1]  = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerLowDataEffFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerLowDataEffFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"IsoMu24_PtEtaBins/efficienciesDATA/abseta_pt_DATA");

    //MC efficiencies
    muonTriggerLowMCEffFileNames[2*k2016]    = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerLowMCEffFileNames[2*k2016+1]  = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"IsoMu24_OR_IsoTkMu24_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerLowMCEffFileNames[2*k2017]    = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerLowMCEffFileNames[2*k2017+1]  = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"IsoMu27_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerLowMCEffFileNames[2*k2018]    = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","IsoMu24_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerLowMCEffFileNames[2*k2018+1]  = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"IsoMu24_PtEtaBins/efficienciesMC/abseta_pt_MC");

    ///////////////////////
    // High Muon Trigger //
    ///////////////////////

    muonTriggerHighFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2017]   = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2017+1] = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");
    muonTriggerHighFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");

    //Data efficiencies
    muonTriggerHighDataEffFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"Mu50_OR_TkMu50_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerHighDataEffFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"Mu50_OR_TkMu50_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerHighDataEffFileNames[2*k2017]   = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerHighDataEffFileNames[2*k2017+1] = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerHighDataEffFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/efficienciesDATA/abseta_pt_DATA");
    muonTriggerHighDataEffFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/efficienciesDATA/abseta_pt_DATA");

    //MC efficiencies
    muonTriggerHighMCEffFileNames[2*k2016]   = fpair("EfficienciesAndSF_RunBtoF_muon_2016.root"           ,"Mu50_OR_TkMu50_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerHighMCEffFileNames[2*k2016+1] = fpair("EfficienciesAndSF_Period4_muonTrigger_2016.root"    ,"Mu50_OR_TkMu50_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerHighMCEffFileNames[2*k2017]   = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerHighMCEffFileNames[2*k2017+1] = fpair("EfficienciesAndSF_2017_RunBtoF_Nov17Nov2017.root"   ,"Mu50_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerHighMCEffFileNames[2*k2018]   = fpair("EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root","Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/efficienciesMC/abseta_pt_MC");
    muonTriggerHighMCEffFileNames[2*k2018+1] = fpair("EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root" ,"Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/efficienciesMC/abseta_pt_MC");


    //initialize Low Trigger histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerLowFileNames[period].first == "") { muonLowTriggerMap[period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerLowFileNames[period].first).Data(),"READ");
      if(!f) { muonLowTriggerMap[period] = 0; continue; }
      muonLowTriggerMap[period] = (TH2F*) f->Get(muonTriggerLowFileNames[period].second.Data())->Clone();
      if(!muonLowTriggerMap[period])
        std::cout << "Error! Histogram " << muonTriggerLowFileNames[period].second.Data() << " in "
                  << muonTriggerLowFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //initialize Low Data Trigger Efficiency histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerLowDataEffFileNames[period].first == "") { muonLowTriggerEffMap[0][period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerLowDataEffFileNames[period].first).Data(),"READ");
      if(!f) { muonLowTriggerEffMap[0][period] = 0; continue; }
      muonLowTriggerEffMap[0][period] = (TH2F*) f->Get(muonTriggerLowDataEffFileNames[period].second.Data())->Clone();
      if(!muonLowTriggerEffMap[0][period])
        std::cout << "Error! Histogram " << muonTriggerLowDataEffFileNames[period].second.Data() << " in "
                  << muonTriggerLowDataEffFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //initialize Low MC Trigger Efficiency histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerLowMCEffFileNames[period].first == "") { muonLowTriggerEffMap[1][period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerLowMCEffFileNames[period].first).Data(),"READ");
      if(!f) { muonLowTriggerEffMap[1][period] = 0; continue; }
      muonLowTriggerEffMap[1][period] = (TH2F*) f->Get(muonTriggerLowMCEffFileNames[period].second.Data())->Clone();
      if(!muonLowTriggerEffMap[1][period])
        std::cout << "Error! Histogram " << muonTriggerLowMCEffFileNames[period].second.Data() << " in "
                  << muonTriggerLowMCEffFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //initialize High Trigger histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerHighFileNames[period].first == "") { muonHighTriggerMap[period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerHighFileNames[period].first).Data(),"READ");
      if(!f) { muonHighTriggerMap[period] = 0; continue; }
      muonHighTriggerMap[period] = (TH2F*) f->Get(muonTriggerHighFileNames[period].second.Data())->Clone();
      if(!muonHighTriggerMap[period])
        std::cout << "Error! Histogram " << muonTriggerHighFileNames[period].second.Data() << " in "
                  << muonTriggerHighFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //initialize High Data Trigger Efficiency histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerHighDataEffFileNames[period].first == "") { muonHighTriggerEffMap[0][period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerHighDataEffFileNames[period].first).Data(),"READ");
      if(!f) { muonHighTriggerEffMap[0][period] = 0; continue; }
      muonHighTriggerEffMap[0][period] = (TH2F*) f->Get(muonTriggerHighDataEffFileNames[period].second.Data())->Clone();
      if(!muonHighTriggerEffMap[0][period])
        std::cout << "Error! Histogram " << muonTriggerHighDataEffFileNames[period].second.Data() << " in "
                  << muonTriggerHighDataEffFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
    }

    //initialize High MC Trigger Efficiency histograms
    for(int period = k2016; period < 2*(k2018+1); ++period) {
      if(muonTriggerHighMCEffFileNames[period].first == "") { muonHighTriggerEffMap[1][period] = 0; continue; }
      TFile* f = TFile::Open((scaleFactorPath + muonTriggerHighMCEffFileNames[period].first).Data(),"READ");
      if(!f) { muonHighTriggerEffMap[1][period] = 0; continue; }
      muonHighTriggerEffMap[1][period] = (TH2F*) f->Get(muonTriggerHighMCEffFileNames[period].second.Data())->Clone();
      if(!muonHighTriggerEffMap[1][period])
        std::cout << "Error! Histogram " << muonTriggerHighMCEffFileNames[period].second.Data() << " in "
                  << muonTriggerHighMCEffFileNames[period].first.Data() << " not found!\n";
      fileList.push_back(f);
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
    std::map<int, fpair> electronTrigFileNames;
    electronTrigFileNames[k2016]  = fpair("egammaTriggerEfficiency_2016.root"                   ,"EGamma_SF2D");
    electronTrigFileNames[k2017]  = fpair("egammaTriggerEfficiency_2017.root"                   ,"EGamma_SF2D");
    electronTrigFileNames[k2018]  = fpair("egammaTriggerEfficiency_2018.root"                   ,"EGamma_SF2D");
    std::map<int, fpair> electronTrigDataEffFileNames;
    electronTrigDataEffFileNames[k2016]  = fpair("egammaTriggerEfficiency_2016.root"                   ,"EGamma_EffData2D");
    electronTrigDataEffFileNames[k2017]  = fpair("egammaTriggerEfficiency_2017.root"                   ,"EGamma_EffData2D");
    electronTrigDataEffFileNames[k2018]  = fpair("egammaTriggerEfficiency_2018.root"                   ,"EGamma_EffData2D");
    std::map<int, fpair> electronTrigMCEffFileNames;
    electronTrigMCEffFileNames[k2016]  = fpair("egammaTriggerEfficiency_2016.root"                   ,"EGamma_EffMC2D");
    electronTrigMCEffFileNames[k2017]  = fpair("egammaTriggerEfficiency_2017.root"                   ,"EGamma_EffMC2D");
    electronTrigMCEffFileNames[k2018]  = fpair("egammaTriggerEfficiency_2018.root"                   ,"EGamma_EffMC2D");
    //FIXME: add electron trigger, pre-fire (2016, 2017) scale
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/L1ECALPrefiringWeightRecipe

    for(int period = k2016; period <= k2018; ++period) {
      if(electronIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronIDMap[period] = (TH2F*) f->Get(electronIDFileNames[period].second.Data())->Clone();
      fileList.push_back(f);
    }
    for(int period = k2016; period <= k2018; ++period) {
      if(electronRecoFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronRecoFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronRecoMap[period] = (TH2F*) f->Get(electronRecoFileNames[period].second.Data())->Clone();
      fileList.push_back(f);
    }
    for(int period = k2016; period <= k2018; ++period) {
      if(electronTrigFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronTrigFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronTriggerMap[period] = (TH2F*) f->Get(electronTrigFileNames[period].second.Data())->Clone();
      fileList.push_back(f);
    }
    for(int period = k2016; period <= k2018; ++period) {
      if(electronTrigDataEffFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronTrigDataEffFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronTriggerEffMap[0][period] = (TH2F*) f->Get(electronTrigDataEffFileNames[period].second.Data())->Clone();
      fileList.push_back(f);
    }
    for(int period = k2016; period <= k2018; ++period) {
      if(electronTrigMCEffFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + electronTrigMCEffFileNames[period].first).Data(),"READ");
      if(!f) continue;
      electronTriggerEffMap[1][period] = (TH2F*) f->Get(electronTrigMCEffFileNames[period].second.Data())->Clone();
      fileList.push_back(f);
    }

    //vertex corrections
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaRunIIRecommendations#HLT_Zvtx_Scale_Factor
    electronVertexMap[k2016]   = 1.000; //+-0.000
    electronVertexMap[k2017]   = 0.991; //+-0.001
    electronVertexMap[k2018]   = 1.000; //+-0.000

    //initialize Tau scales
    TString tauScaleFactorPath = gSystem->Getenv("CMSSW_BASE") + fTauScaleFactorPath;
    std::map<int, fpair> tauJetIDFileNames;
    tauJetIDFileNames[k2016]    = fpair("TauID_SF_pt_DeepTau2017v2p1VSjet_2016Legacy.root","Medium");
    tauJetIDFileNames[k2017]    = fpair("TauID_SF_pt_DeepTau2017v2p1VSjet_2017ReReco.root","Medium");
    tauJetIDFileNames[k2018]    = fpair("TauID_SF_pt_DeepTau2017v2p1VSjet_2018ReReco.root","Medium");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauJetIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauJetIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauJetIDMap[period] = (TF1*) f->Get((tauJetIDFileNames[period].second+"_cent").Data())->Clone();
      tauJetUpIDMap[period] = (TF1*) f->Get((tauJetIDFileNames[period].second+"_up").Data())->Clone();
      tauJetDownIDMap[period] = (TF1*) f->Get((tauJetIDFileNames[period].second+"_down").Data())->Clone();
      if(!tauJetIDMap[period]) std::cout << "Error! tauJetID " << tauJetIDFileNames[period].second.Data() << " TF1 not found!\n";
      if(!tauJetUpIDMap[period]) std::cout << "Error! tauJetUpID " << tauJetIDFileNames[period].second.Data() << " TF1 not found!\n";
      if(!tauJetDownIDMap[period]) std::cout << "Error! tauJetDownID " << tauJetIDFileNames[period].second.Data() << " TF1 not found!\n";
      fileList.push_back(f);
    }
    std::map<int, fpair> tauEleIDFileNames;
    tauEleIDFileNames[k2016]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2016Legacy.root","Medium");
    tauEleIDFileNames[k2017]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2017ReReco.root","Medium");
    tauEleIDFileNames[k2018]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2018ReReco.root","Medium");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauEleIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauEleIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauEleIDMap[period] = (TH1F*) f->Get((tauEleIDFileNames[period].second).Data())->Clone();
      fileList.push_back(f);
    }
    std::map<int, fpair> tauMuIDFileNames;
    tauMuIDFileNames[k2016]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2016Legacy.root","Tight");
    tauMuIDFileNames[k2017]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2017ReReco.root","Tight");
    tauMuIDFileNames[k2018]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2018ReReco.root","Tight");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauMuIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauMuIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauMuIDMap[period] = (TH1F*) f->Get((tauMuIDFileNames[period].second).Data())->Clone();
      fileList.push_back(f);
    }

    //tau energy scale corrections
    std::map<int, fpair> tauESLowFileNames;
    tauESLowFileNames[k2016]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2016Legacy.root", "tes");
    tauESLowFileNames[k2017]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2017ReReco.root", "tes");
    tauESLowFileNames[k2018]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco.root", "tes");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauESLowFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauESLowFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauESLowMap[period] = (TH1F*) f->Get((tauESLowFileNames[period].second).Data())->Clone();
      fileList.push_back(f);
    }
    std::map<int, fpair> tauESHighFileNames;
    tauESHighFileNames[k2016]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2016Legacy_ptgt100.root", "tes");
    tauESHighFileNames[k2017]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2017ReReco_ptgt100.root", "tes");
    tauESHighFileNames[k2018]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco_ptgt100.root", "tes");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauESHighFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauESHighFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauESHighMap[period] = (TH1F*) f->Get((tauESHighFileNames[period].second).Data())->Clone();
      fileList.push_back(f);
    }

    std::map<int, fpair> tauFakeESFileNames;
    tauFakeESFileNames[k2016]    = fpair("TauFES_eta-dm_DeepTau2017v2p1VSe_2016Legacy.root", "fes");
    tauFakeESFileNames[k2017]    = fpair("TauFES_eta-dm_DeepTau2017v2p1VSe_2017ReReco.root", "fes");
    tauFakeESFileNames[k2018]    = fpair("TauFES_eta-dm_DeepTau2017v2p1VSe_2018ReReco.root", "fes");
    for(int period = k2016; period <= k2018; ++period) {
      if(tauFakeESFileNames[period].first == "") continue;
      TFile* f = TFile::Open((tauScaleFactorPath + tauFakeESFileNames[period].first).Data(),"READ");
      if(!f) continue;
      tauFakeESMap[period] = (TGraphAsymmErrors*) f->Get((tauFakeESFileNames[period].second).Data())->Clone();
      fileList.push_back(f);
    }

    //photon corrections
    std::map<int, fpair> photonIDFileNames;
    photonIDFileNames[k2016]    = fpair("Fall17V2_2016_MVAwp80_photons.root","EGamma_SF2D");
    photonIDFileNames[k2017]    = fpair("2017_PhotonsMVAwp80.root","EGamma_SF2D");
    photonIDFileNames[k2018]    = fpair("2018_PhotonsMVAwp80.root","EGamma_SF2D");
    for(int period = k2016; period <= k2018; ++period) {
      if(photonIDFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + photonIDFileNames[period].first).Data(),"READ");
      if(!f) continue;
      photonIDMap[period] = (TH2F*) f->Get(photonIDFileNames[period].second.Data())->Clone()->Clone();
      fileList.push_back(f);
    }


    //z correction weights
    scaleFactorPath = gSystem->Getenv("CMSSW_BASE") + fZScaleFactorPath;
    std::map<int, fpair> zFileNames;
    zFileNames[k2016] = fpair("z_pt_vs_m_scales_mumu_2016.root", "hRatioNorm");
    zFileNames[k2017] = fpair("z_pt_vs_m_scales_mumu_2017.root", "hRatioNorm");
    zFileNames[k2018] = fpair("z_pt_vs_m_scales_mumu_2018.root", "hRatioNorm");
    for(int period = k2016; period <= k2018; ++period) {
      if(zFileNames[period].first == "") continue;
      TFile* f = TFile::Open((scaleFactorPath + zFileNames[period].first).Data(),"READ");
      if(!f) continue;
      zWeightMap[period] = (TH2F*) f->Get((zFileNames[period].second).Data())->Clone();
      fileList.push_back(f);
    }

    //b-tag efficiency histograms
    for(int period = k2016; period <= k2018; ++period) { //file per year
      for(int WP = kLooseBTag; WP <= kTightBTag; ++WP) { //file per working point
        TString btagname = Form("btag_eff_wp_%i_mumu_%i.root", WP, period+2016);
        TFile* f = TFile::Open((scaleFactorPath + btagname).Data(),"READ");
        if(!f) continue;
        bJetGenBEffMap[period][WP] = (TH2F*) f->Get("hBRatio")->Clone(); //gen-level b-jets
        bJetGenCEffMap[period][WP] = (TH2F*) f->Get("hCRatio")->Clone(); //gen-level c-jets
        bJetGenLEffMap[period][WP] = (TH2F*) f->Get("hLRatio")->Clone(); //gen-level light-jets
        fileList.push_back(f);
      }
    }
  }

  virtual ~ParticleCorrections() {
    for(TFile* f : fileList) {
      if(f) delete f;
    }
  }

  virtual void MuonWeight(double pt, double eta, int trigger, int era, float& trig_scale,
                          float& weight_id, float& weight_up_id , float& weight_down_id , int& ibin_id,
                          float& weight_iso, float& weight_up_iso, float& weight_down_iso, int& ibin_iso
                          );
  double MuonWeight(double pt, double eta, int trigger, int era, float& trig_scale) {
    float wt1, wt2;
    float up, down, up2, down2;
    int ibin, ibin2;
    MuonWeight(pt, eta, trigger, era, trig_scale, wt1, up, down, ibin, wt2, up2, down2, ibin2);
    return wt1*wt2;
  }
  virtual double MuonTriggerEff(double pt, double eta, int trigger, int era, float& data_eff, float& mc_eff/*, int& ibin*/);

  virtual void ElectronWeight(double pt, double eta, int era, float& trigger_scale,
                              float& weight_id , float& weight_up_id , float& weight_down_id , int& ibin_id,
                              float& weight_rec, float& weight_up_rec, float& weight_down_rec, int& ibin_rec
                              );
  double ElectronWeight(double pt, double eta, int era) {
    float wt1, wt2;
    float trig, up, down, up2, down2;
    int ibin, ibin2;
    ElectronWeight(pt, eta, era, trig, wt1, up, down, ibin, wt2, up2, down2, ibin2);
    return wt1*wt2;
  }
  virtual double ElectronTriggerEff(double pt, double eta, int era, float& data_eff, float& mc_eff);

  virtual double TauWeight(double pt, double eta, int genID, UChar_t antiJet, int era, float& up, float& down, int& ibin);
  double TauWeight(double pt, double eta, int genID, UChar_t antiJet, int era) {
    float up, down;
    int ibin;
    return TauWeight(pt, eta, genID, antiJet, era, up, down, ibin);
  }
  virtual double TauEnergyScale(double pt, double eta, int dm, int genID, UChar_t antiJet, int era, float& up, float& down, int& ibin);

  virtual double PhotonWeight(double pt, double eta, int year);
  virtual double BTagMCProb(double pt, double eta, int jetFlavor, int year, int WP);
  virtual double BTagDataProb(double pt, double eta, int jetFlavor, int year, int WP);
  virtual double ZWeight(double pt, double mass, int year);

  virtual double BTagCut(int wp, int year);
  virtual float CombineEfficiencies(float data_eff_1, float mc_eff_1, float data_eff_2, float mc_eff_2);

  enum{k2016, k2017, k2018}; //defined years
  enum{kLowTrigger, kHighTrigger}; //defined triggers
  enum{kLooseBTag, kMediumBTag, kTightBTag}; //define btag WPs

  //muon isolation levels
  enum{kVLooseMuIso, kLooseMuIso, kMediumMuIso, kTightMuIso, kVTightMuIso, kVVTightMuIso}; //define iso scale factor sets
  const double muonIsoValues[kVVTightMuIso+1] = {0.4, 0.25, 0.20, 0.15, 0.10, 0.05}; //corresponding values

  TString fScaleFactorPath    = "/src/StandardModel/ZEMuAnalysis/test/scale_factors/"; //path from cmssw_base
  TString fTauScaleFactorPath = "/src/TauPOG/TauIDSFs/data/"; //path from cmssw_base
  TString fZScaleFactorPath   = "/src/CLFVAnalysis/scale_factors/"; //path from cmssw_base

  Int_t   fVerbose = 0; //control the verbosity
public:
  //muon corrections
  std::map<int, TH2F*> muonIDMap;
  std::map<int, TH2F*> muonIsoMap;
  std::map<int, TH2F*> muonLowTriggerMap;
  std::map<int, TH2F*> muonLowTriggerEffMap[2]; //Data and MC efficiencies
  std::map<int, TH2F*> muonHighTriggerMap;
  std::map<int, TH2F*> muonHighTriggerEffMap[2]; //Data and MC efficiencies
  //electron corrections
  std::map<int, TH2F*> electronIDMap     ;
  std::map<int, TH2F*> electronRecoMap   ;
  std::map<int, float> electronVertexMap ;
  std::map<int, TH2F*> electronTriggerMap;
  std::map<int, TH2F*> electronTriggerEffMap[2]; //Data and MC efficiencies
  std::map<int, TH2F*> electronPreFireMap;
  //tau ID corrections
  std::map<int, TF1*> tauJetIDMap    ;
  std::map<int, TF1*> tauJetUpIDMap  ;
  std::map<int, TF1*> tauJetDownIDMap;
  std::map<int, TH1F*> tauEleIDMap   ;
  std::map<int, TH1F*> tauMuIDMap    ;
  //tau ES corrections
  std::map<int, TH1F*> tauESLowMap; // 34 < pT < 170 GeV/c
  std::map<int, TH1F*> tauESHighMap; // pT > 170 GeV/c
  std::map<int, TGraphAsymmErrors*> tauFakeESMap;
  //photon ID corrections
  std::map<int, TH2F*> photonIDMap;
  //b-jet corrections
  std::map<int, std::map<int, TH2F*>> bJetGenBEffMap; //gen-level b-jets
  std::map<int, std::map<int, TH2F*>> bJetGenCEffMap; //gen-level c-jets
  std::map<int, std::map<int, TH2F*>> bJetGenLEffMap; //gen-level light-jets
  //Z pT/mass corrections
  std::map<int, TH2F*> zWeightMap;
  //Open files, to close when exiting
  std::vector<TFile*> fileList;
  TRandom* fRnd; //for getting random period in year
};
#endif // #ifdef ParticleCorrections_cxx
