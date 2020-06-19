// Script to process NANO AODs to match format of bltTrees
#include "ParticleCorrections.cc+g"
#include "NanoAODConversion.cc+g"

Int_t process_nanoaods() {

  TString cmssw = gSystem->Getenv("CMSSW_BASE");
  TString path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/rootfiles/latest_production/";
  TString crab_path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/crab_projects/";
  TStopwatch* timer = new TStopwatch();
  timer->Start();
  
  const char* samples[] = {
     "MC/backgrounds/ZEMuAnalysis_DY50_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_SingleToptW_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_SingleAntiToptW_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_WW_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_WZ_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_Wlnu_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_ttbarToSemiLeptonic_2016.root"
    ,"MC/backgrounds/ZEMuAnalysis_ttbarlnu_2016.root"
    ,"MC/signals/ZEMuAnalysis_Signal_2016.root"
    ,"dataprocess/ZEMuAnalysis_SingleEle_2016.root"
    ,"dataprocess/ZEMuAnalysis_SingleMu_2016.root"                 
  };
  const bool doProcess[] = {
    true,    //ZEMuAnalysis_DY50_2016.root"		      
    true,    //ZEMuAnalysis_SingleToptW_2016.root"	      
    true,    //ZEMuAnalysis_SingleAntiToptW_2016.root"	      
    true,    //ZEMuAnalysis_WW_2016.root"			      
    true,    //ZEMuAnalysis_WZ_2016.root"			      
    true,    //ZEMuAnalysis_Wlnu_2016.root"		      
    true,    //ZEMuAnalysis_ttbarToSemiLeptonic_2016.root"      
    true,    //ZEMuAnalysis_ttbarlnu_2016.root"		      
    true,    //ZEMuAnalysis_Signal_2016.root"		      
    true,    //ZEMuAnalysis_SingleEle_2016.root"		      
    true     //ZEMuAnalysis_SingleMu_2016.root"                 
  };

  std::map<TString, double> frac_table_2016;
  frac_table_2016["ttbarToSemiLeptonic"] = 0.;           
  frac_table_2016["ttbarlnu"] = 0.;
  frac_table_2016["SingleToptW"] = 0.003708;
  frac_table_2016["SingleAntiToptW"] = 0.00369;
  frac_table_2016["DY50"] = 0.1661;
  frac_table_2016["WW"] = 0.1866;
  frac_table_2016["WZ"] = 0.;
  frac_table_2016["Wlnu"] = 0.;
  frac_table_2016["Signal"] = 0. ;                        

  std::map<TString, double> frac_table_2017;
  frac_table_2017["ttbarToSemiLeptonic"] = 0.;
  frac_table_2017["ttbarlnu"] = 0.;
  frac_table_2017["SingleToptW"] = 0.003758;
  frac_table_2017["SingleAntiToptW"] = 0.0034;
  frac_table_2017["DY50"] = 0.1624;
  frac_table_2017["WW"] = 0.;
  frac_table_2017["WZ"] = 0.;
  frac_table_2017["Signal"] = 0.;

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
  frac_table_2018["Signal"] = 0.;
  
  for(unsigned index = 0; index < sizeof(doProcess)/sizeof(*doProcess); ++index) {
    cout << "File " << samples[index] << ", process = " << doProcess[index] << endl;
    if(!doProcess[index]) continue;
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
    name.ReplaceAll("ZEMuAnalysis", "");
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
      if(year == ParticleCorrections::k2016)      crab_command += "2016/crab_2016_ZEMuAnalysis_";
      else if(year == ParticleCorrections::k2017) crab_command += "2017/crab_2017_ZEMuAnalysis_";
      else if(year == ParticleCorrections::k2018) crab_command += "2018/crab_2018_ZEMuAnalysis_";
      crab_command += name;
      crab_command += " | grep read | awk '{print $NF}'";
      FILE* shell_res = gSystem->OpenPipe(crab_command.Data(), "r");
      crab_report.Gets(shell_res);
      gSystem->ClosePipe(shell_res);
      try {
	events = stof(crab_report.Data());
      } catch(exception e) {
	events = 0.;
      }
      if(year == 0)      frac_neg = frac_table_2016[name];
      else if(year == 1) frac_neg = frac_table_2017[name];
      else if(year == 2) frac_neg = frac_table_2018[name];
      cout << "Found " << events << " events for the file with "
	   << frac_neg << " fraction negative events" << endl;
    }
    NanoAODConversion* selec = new NanoAODConversion();
    if(events > 0.) selec->fEventCount = events*(1.-2.*frac_neg);
    selec->fFolderName = name;
    selec->fYear = year;
    selec->fIsData = isData;
    selec->fSkipDoubleTrigger = false; //pre-skipped
    t->Process(selec);
    delete f;
  }
  
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Total processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Total processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
