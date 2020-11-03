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
TString debugFile_ = "HMuTau";
UInt_t debugStart_ = 8947;
UInt_t debugNEvents_ = 1;
NanoAODConversion* debugSelec_ = 0;

//information about the data file/data
struct datacard_t {
  bool process_;
  double negfrac_;
  TString fname_;
  TString filepath_;
  int isData_;
  datacard_t(bool process, TString fname,
	     double negfrac = 0., int isData = 0,  
	     TString filepath = "") :
    process_(process), negfrac_(negfrac), fname_(fname), filepath_(filepath),
    isData_(isData) {}
};

Int_t process_nanoaods() {

  TString cmssw = gSystem->Getenv("CMSSW_BASE");
  // TString path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/rootfiles/latest_production/";
  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/";
  TString crab_path = cmssw + "/src/StandardModel/ZEMuAnalysis/test/crab_projects/";
  TStopwatch* timer = new TStopwatch();
  timer->Start();

  //Data cards
  vector<datacard_t> cards;
  //2016
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptW_2016.root"             , 0.003758));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptW_2016.root"         , 0.0034));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50_2016.root"                    , 0));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WW_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WZ_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ZZ_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WWW_2016.root"                     , 0.06054));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu_2016.root"                    , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-ext_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2016.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarlnu_2016.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30to40_2016.root" , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30toInf_2016.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich40toInf_2016.root", 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZETau_2016.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZMuTau_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZEMu_2016.root"                        , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HETau_2016.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HMuTau_2016.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HEMu_2016.root"                        , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleEle_2016.root"                  , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMu_2016.root"                   , 0.));                 
  //2017
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50_2017.root"                    , 0.1624));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50-ext_2017.root"                , 0.1624));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptW_2017.root"             , 0.003758));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptW_2017.root"         , 0.0034));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WW_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WZ_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ZZ_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WWW_2017.root"                     , 0.06054));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu_2017.root"                    , 0.0004079));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu-ext_2017.root"                , 0.0004079));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2017.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarlnu_2017.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZETau_2017.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZMuTau_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZEMu_2017.root"                        , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HETau_2017.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HMuTau_2017.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HEMu_2017.root"                        , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleEle_2017.root"                  , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMu_2017.root"                   , 0.));                 
  //2018
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleToptW_2018.root"             , 0.003758));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_SingleAntiToptW_2018.root"         , 0.0034));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_DY50_2018.root"                    , 0.0004962));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WW_2018.root"                      , 0.001916));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WZ_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ZZ_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_WWW_2018.root"                     , 0.06197));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_Wlnu_2018.root"                    , 0.0003866));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarToSemiLeptonic_2018.root"     , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_ttbarlnu_2018.root"                , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30to40_2018.root" , 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich30toInf_2018.root", 0.));
  cards.push_back(datacard_t(false, "MC/backgrounds/LFVAnalysis_QCDDoubleEMEnrich40toInf_2018.root", 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZETau_2018.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZMuTau_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_ZEMu_2018.root"                        , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HETau_2018.root"                       , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HMuTau_2018.root"                      , 0.));
  cards.push_back(datacard_t(false, "MC/signals/LFVAnalysis_HEMu_2018.root"                        , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleEle_2018.root"                  , 0.));
  cards.push_back(datacard_t(false, "dataprocess/LFVAnalysis_SingleMu_2018.root"                   , 0.));                 

  
  for(unsigned index = 0; index < cards.size(); ++index) {
    if(debug_ && !cards[index].fname_.Contains(debugFile_.Data())) continue; //don't print other info in debug
    cout << "**File " << cards[index].fname_.Data() << ", process = " << cards[index].process_ << endl;
    if(!cards[index].process_) continue;
    //if debugging, only do requested file

    bool isDY = cards[index].fname_.Contains("DY");
    TFile* f = TFile::Open((path+cards[index].fname_).Data(), "READ");
    if(!f) {
      cout << " File not found! Continuing..." << endl;
      continue;
    }
    TTree* t = (TTree*) f->Get("Events");
    if(!t) {
      cout << " Tree not found! Continuing..." << endl;
      continue;
    }
    TString name = cards[index].fname_;
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
    double frac_neg = cards[index].negfrac_;
    if(isData == 0) {
      TString crab_report;
      TString crab_command = "crab report -d " + crab_path;
      crab_command += "samples_MC_";
      if(year == ParticleCorrections::k2016)      crab_command += "2016/crab_2016_LFVDONE_"; 
      else if(year == ParticleCorrections::k2017) crab_command += "2017/crab_2017_LFVDONE_";
      else if(year == ParticleCorrections::k2018) crab_command += "2018/crab_2018_LFVDONE_";
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
      cout << "Found " << events << " events for the file with "
	   << frac_neg << " fraction negative events" << endl;
      if(events <= 0.) {
	cout << "No events found! Can't do normalization --> continue to next file!\n";
	continue;
      }
    }
    NanoAODConversion* selec = new NanoAODConversion();
    if(events > 0.) selec->fEventCount = events*(1.-2.*frac_neg);
    selec->fFolderName = name;
    selec->fYear = year;
    selec->fIsData = isData;
    selec->fSkipDoubleTrigger = false; //pre-skipped
    selec->fSkipMuMuEE = 0; //skip same flavor data
    selec->fIsDY = isDY; //for Z pT weights
    selec->fVerbose = (debug_) ? 3 : 1; //print warnings as they come normally, print extra in debug
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
