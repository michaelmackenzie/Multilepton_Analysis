// Script to process NANO AODs to match format of bltTrees
#include "ParticleCorrections.cc+g"
#include "../dataFormats/SlimObject_t.hh+g"
#include "../dataFormats/SlimElectron_t.hh+g"
#include "../dataFormats/SlimMuon_t.hh+g"
#include "../dataFormats/SlimTau_t.hh+g"
#include "../dataFormats/SlimJet_t.hh+g"
#include "../dataFormats/SlimPhoton_t.hh+g"
#include "NanoAODConversion.cc+g"

bool debug_ = false;
TString debugFile_ = "";
UInt_t debugStart_ = 0;
UInt_t debugNEvents_ = 0;
NanoAODConversion* debugSelec_ = 0;

Int_t process_nanoaods() {

  TString cmssw = gSystem->Getenv("CMSSW_BASE");
  TString path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/rootfiles/latest_production/";
  TString crab_path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/crab_projects/";
  TStopwatch* timer = new TStopwatch();
  timer->Start();
  
  const char* samples[] = {
    "MC/backgrounds/LFVAnalysis_DY50_2016.root"
    ,"MC/backgrounds/LFVAnalysis_SingleToptW_2016.root"
    ,"MC/backgrounds/LFVAnalysis_SingleAntiToptW_2016.root"
    ,"MC/backgrounds/LFVAnalysis_WW_2016.root"
    ,"MC/backgrounds/LFVAnalysis_WZ_2016.root"
    ,"MC/backgrounds/LFVAnalysis_ZZ_2016.root"
    ,"MC/backgrounds/LFVAnalysis_WWW_2016.root"
    ,"MC/backgrounds/LFVAnalysis_Wlnu_2016.root"
    ,"MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2016.root"
    ,"MC/backgrounds/LFVAnalysis_ttbarlnu_2016.root"
    ,"MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30to40_2016.root"
    ,"MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30toInf_2016.root"
    ,"MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich40toInf_2016.root"
    ,"MC/signals/LFVAnalysis_ZETau_2016.root"
    ,"MC/signals/LFVAnalysis_ZMuTau_2016.root"
    ,"MC/signals/LFVAnalysis_ZEMu_2016.root"
    ,"MC/signals/LFVAnalysis_HETau_2016.root"
    ,"MC/signals/LFVAnalysis_HMuTau_2016.root"
    ,"MC/signals/LFVAnalysis_HEMu_2016.root"
    ,"dataprocess/LFVAnalysis_SingleEle_2016.root"
    ,"dataprocess/LFVAnalysis_SingleMu_2016.root"                 
    ,"MC/backgrounds/LFVAnalysis_DY50_2017.root"
    ,"MC/backgrounds/LFVAnalysis_SingleToptW_2017.root"
    ,"MC/backgrounds/LFVAnalysis_SingleAntiToptW_2017.root"
    ,"MC/backgrounds/LFVAnalysis_WW_2017.root"
    ,"MC/backgrounds/LFVAnalysis_WZ_2017.root"
    ,"MC/backgrounds/LFVAnalysis_Wlnu_2017.root"
    ,"MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2017.root"
    ,"MC/backgrounds/LFVAnalysis_ttbarlnu_2017.root"
    ,"MC/signals/LFVAnalysis_ZEMu_2017.root"
    ,"MC/signals/LFVAnalysis_HEMu_2017.root"
    ,"dataprocess/LFVAnalysis_SingleEle_2017.root"
    ,"dataprocess/LFVAnalysis_SingleMu_2017.root"                 
    ,"MC/backgrounds/LFVAnalysis_DY50_2018.root"
    ,"MC/backgrounds/LFVAnalysis_SingleToptW_2018.root"
    ,"MC/backgrounds/LFVAnalysis_SingleAntiToptW_2018.root"
    ,"MC/backgrounds/LFVAnalysis_WW_2018.root"
    ,"MC/backgrounds/LFVAnalysis_WZ_2018.root"
    ,"MC/backgrounds/LFVAnalysis_Wlnu_2018.root"
    ,"MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2018.root"
    ,"MC/backgrounds/LFVAnalysis_ttbarlnu_2018.root"
    ,"MC/signals/LFVAnalysis_ZEMu_2018.root"
    ,"MC/signals/LFVAnalysis_HEMu_2018.root"
    ,"dataprocess/LFVAnalysis_SingleEle_2018.root"
    ,"dataprocess/LFVAnalysis_SingleMu_2018.root"                 
  };
  const bool doProcess[] = {
    false,    //LFVAnalysis_DY50_2016.root"		      
    false,    //LFVAnalysis_SingleToptW_2016.root"	      
    false,    //LFVAnalysis_SingleAntiToptW_2016.root"	      
    false,    //LFVAnalysis_WW_2016.root"			      
    false,    //LFVAnalysis_WZ_2016.root"			      
    false,    //LFVAnalysis_ZZ_2016.root"			      
    false,    //LFVAnalysis_WWW_2016.root"			      
    false,    //LFVAnalysis_Wlnu_2016.root"		      
    false,    //LFVAnalysis_ttbarToSemiLeptonic_2016.root"      
    false,    //LFVAnalysis_ttbarlnu_2016.root"		      
    false,    //LFVAnalysis_QCDDoubleEMEnrich30to40_2016.root" 	      
    false,    //LFVAnalysis_QCDDoubleEMEnrich30toInf_2016.root"	      
    false,    //LFVAnalysis_QCDDoubleEMEnrich40toInf_2016.root"	      
    false,    //LFVAnalysis_ZETau_2016.root"		      
    false,    //LFVAnalysis_ZMuTau_2016.root"
    false,    //LFVAnalysis_ZEMu_2016.root"
    true ,    //LFVAnalysis_HETau_2016.root"		      
    true ,    //LFVAnalysis_HMuTau_2016.root"		      
    true ,    //LFVAnalysis_HEMu_2016.root"		      
    false,    //LFVAnalysis_SingleEle_2016.root"		      
    false,    //LFVAnalysis_SingleMu_2016.root"                 
    //2017
    false,    //LFVAnalysis_DY50_2017.root"		      
    false,    //LFVAnalysis_SingleToptW_2017.root"	      
    false,    //LFVAnalysis_SingleAntiToptW_2017.root"	      
    false,    //LFVAnalysis_WW_2017.root"			      
    false,    //LFVAnalysis_WZ_2017.root"			      
    false,    //LFVAnalysis_Wlnu_2017.root"		      
    false,    //LFVAnalysis_ttbarToSemiLeptonic_2017.root"      
    false,    //LFVAnalysis_ttbarlnu_2017.root"		      
    false,    //LFVAnalysis_ZEMu_2017.root"		      
    false,    //LFVAnalysis_HEMu_2017.root"		      
    false,    //LFVAnalysis_SingleEle_2017.root"		      
    false,     //LFVAnalysis_SingleMu_2017.root"                 
    false,    //LFVAnalysis_DY50_2018.root"		      
    false,    //LFVAnalysis_SingleToptW_2018.root"	      
    false,    //LFVAnalysis_SingleAntiToptW_2018.root"	      
    false,    //LFVAnalysis_WW_2018.root"			      
    false,    //LFVAnalysis_WZ_2018.root"			      
    false,    //LFVAnalysis_Wlnu_2018.root"		      
    false,    //LFVAnalysis_ttbarToSemiLeptonic_2018.root"      
    false,    //LFVAnalysis_ttbarlnu_2018.root"		      
    false,    //LFVAnalysis_ZEMu_2018.root"		      
    false,    //LFVAnalysis_HEMu_2018.root"		      
    false,    //LFVAnalysis_SingleEle_2018.root"		      
    false     //LFVAnalysis_SingleMu_2018.root"                 
  };

  std::map<TString, double> frac_table_2016;
  frac_table_2016["ttbarToSemiLeptonic"] = 0.;           
  frac_table_2016["ttbarlnu"] = 0.;
  frac_table_2016["SingleToptW"] = 0.003708;
  frac_table_2016["SingleAntiToptW"] = 0.00369;
  frac_table_2016["DY50"] = 0.;
  frac_table_2016["WW"] = 0.;
  frac_table_2016["WZ"] = 0.;
  frac_table_2016["ZZ"] = 0.;
  frac_table_2016["WWW"] = 0.06054;
  frac_table_2016["Wlnu"] = 0.;
  frac_table_2016["QCDDoubleEMEnrich30to40"] = 0.;
  frac_table_2016["QCDDoubleEMEnrich30toInf"] = 0.;
  frac_table_2016["QCDDoubleEMEnrich40toInf"] = 0.;
  frac_table_2016["ZETau"] = 0. ;                        
  frac_table_2016["ZMuTau"] = 0. ;                        
  frac_table_2016["ZEMu"] = 0. ;                        
  frac_table_2016["HETau"] = 0. ;                        
  frac_table_2016["HMuTau"] = 0. ;                        
  frac_table_2016["HEMu"] = 0. ;                        

  std::map<TString, double> frac_table_2017;
  frac_table_2017["ttbarToSemiLeptonic"] = 0.;
  frac_table_2017["ttbarlnu"] = 0.;
  frac_table_2017["SingleToptW"] = 0.003758;
  frac_table_2017["SingleAntiToptW"] = 0.0034;
  frac_table_2017["DY50"] = 0.1624;
  frac_table_2017["WW"] = 0.;
  frac_table_2017["WZ"] = 0.;
  frac_table_2017["Wlnu"] = 0.0004079;
  frac_table_2017["WWW"] = 0.06054;
  frac_table_2017["QCDDoubleEMEnrich30to40"] = 0.;
  frac_table_2017["QCDDoubleEMEnrich30toInf"] = 0.;
  frac_table_2017["QCDDoubleEMEnrich40toInf"] = 0.;
  frac_table_2017["ZZ"] = 0.;
  frac_table_2017["ZEMu"] = 0.;
  frac_table_2017["HEMu"] = 0.;

  std::map<TString, double> frac_table_2018;
  frac_table_2018["ttbarToSemiLeptonic"] = 0.;
  frac_table_2018["ttbarlnu"] = 0.;
  frac_table_2018["QCDPt30toInf"] = 0.;
  frac_table_2018["QCDPt40toInf"] = 0.;
  frac_table_2018["SingleToptW"] = 0.003758;
  frac_table_2018["SingleAntiToptW"] = 0.0034;
  frac_table_2018["DY50"] = 0.163;
  frac_table_2018["WW"] = 0.001755;
  frac_table_2018["WZ"] = 0.;
  frac_table_2018["Wlnu"] = 0.0003866;
  frac_table_2018["WWW"] = 0.06054;
  frac_table_2018["QCDDoubleEMEnrich30to40"] = 0.;
  frac_table_2018["QCDDoubleEMEnrich30toInf"] = 0.;
  frac_table_2018["QCDDoubleEMEnrich40toInf"] = 0.;
  frac_table_2018["ZZ"] = 0.;
  frac_table_2018["ZEMu"] = 0.;
  frac_table_2018["HEMu"] = 0.;
  
  for(unsigned index = 0; index < sizeof(doProcess)/sizeof(*doProcess); ++index) {
    cout << "**File " << samples[index] << ", process = " << doProcess[index] << endl;
    if(!doProcess[index]) continue;
    //if debugging, only do requested file
    if(debug_ && !TString(samples[index]).Contains(debugFile_.Data())) continue;

    bool isDY = TString(samples[index]).Contains("DY");
    TFile* f = TFile::Open((path+samples[index]).Data(), "READ");
    if(!f) {
      cout << " File not found! Continuing..." << endl;
      continue;
    }
    TTree* t = (TTree*) f->Get("Events");
    if(!t) {
      cout << " Tree not found! Continuing..." << endl;
      continue;
    }
    TString name = samples[index];
    int year = ParticleCorrections::k2016;
    if(name.Contains("2017")) year = ParticleCorrections::k2017;
    else if(name.Contains("2018")) year = ParticleCorrections::k2018;
    Int_t isData = 0;
    if(name.Contains("SingleEle")) isData = 1;
    else if(name.Contains("SingleMu")) isData = 2;
    
    name.ReplaceAll("MC/", "");
    name.ReplaceAll("backgrounds/", "");
    name.ReplaceAll("signals/", "");
    name.ReplaceAll("dataprocess/", "");
    name.ReplaceAll("LFVAnalysis", "");
    name.ReplaceAll("2016", "");
    name.ReplaceAll("2017", "");
    name.ReplaceAll("2018", "");
    name.ReplaceAll(".root", "");
    name.ReplaceAll("_", "");
    float events = -1.;
    double frac_neg = 0.;
    if(isData == 0) {
      TString crab_report;
      TString crab_command = "crab report -d " + crab_path;
      crab_command += "samples_MC_";
      if(year == ParticleCorrections::k2016)      crab_command += "2016/crab_2016_LFVAnalysis_";
      else if(year == ParticleCorrections::k2017) crab_command += "2017/crab_2017_LFVAnalysis_";
      else if(year == ParticleCorrections::k2018) crab_command += "2018/crab_2018_LFVAnalysis_";
      crab_command += name;
      crab_command += " | grep read | awk '{print $NF}'";
      cout << "Getting normalization, using command:\n" << crab_command.Data() << endl;
      FILE* shell_res = gSystem->OpenPipe(crab_command.Data(), "r");
      crab_report.Gets(shell_res);
      gSystem->ClosePipe(shell_res);
      try {
	events = stof(crab_report.Data());
      } catch(exception e) {
	events = 0.;
      }
      if(year == ParticleCorrections::k2016)      frac_neg = frac_table_2016[name];
      else if(year == ParticleCorrections::k2017) frac_neg = frac_table_2017[name];
      else if(year == ParticleCorrections::k2018) frac_neg = frac_table_2018[name];
      cout << "Found " << events << " events for the file with "
	   << frac_neg << " fraction negative events" << endl;
    }
    NanoAODConversion* selec = new NanoAODConversion();
    if(events > 0.) selec->fEventCount = events*(1.-2.*frac_neg);
    selec->fFolderName = name;
    selec->fYear = year;
    selec->fIsData = isData;
    selec->fSkipDoubleTrigger = false; //pre-skipped
    selec->fSkipMuMuEE = 0; //skip same flavor data
    selec->fIsDY = isDY; //for Z pT weights
    if(debug_) debugSelec_ = selec;
    if(!debug_)
      t->Process(selec);
    else if(debugNEvents_ == 0)
      t->Process(selec,"",-1, debugStart_);
    else
      t->Process(selec,"",debugNEvents_, debugStart_);
    if(!debug_) delete selec;
    delete f;
  }
  
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Total processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Total processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  
  return 0;
}
