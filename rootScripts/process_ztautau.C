
//Data formats
#include "../dataFormats/SlimObject_t.hh+g"
#include "../dataFormats/SlimElectron_t.hh+g"
#include "../dataFormats/SlimMuon_t.hh+g"
#include "../dataFormats/SlimTau_t.hh+g"
#include "../dataFormats/SlimJet_t.hh+g"
#include "../dataFormats/SlimPhoton_t.hh+g"
#include "../dataFormats/Tree_t.hh+g"

//Utilities
#include "../utils/TrkQualInit.cc+g"

//Histogrammer
#include "ZTauTauHistMaker.cc+g"

//information about the data file/data
struct datacard_t {
  bool process_;
  double xsec_;
  TString fname_;
  TString filepath_;
  int isData_;
  int category_;
  TH1F* events_;
  datacard_t(bool process, double xsec, TString fname,
	     int isData = 0,  TString filepath = "",
	     int category = 0, TH1F* events = 0) :
    process_(process), xsec_(xsec), fname_(fname), filepath_(filepath),
    isData_(isData), category_(category), events_(events) {}
};

//information about the selector configuration
struct config_t {
  bool writeTrees_;
  TString onlyChannel_;
  vector<TString> skipChannels_;
  bool reProcessMVAs_;
  Int_t removeZPtWeights_;
  float signalTrainFraction_;
  float backgroundTrainFraction_;
  Int_t useTauFakeSF_;
  config_t() : writeTrees_(true), onlyChannel_(""), skipChannels_(),
	       reProcessMVAs_(false),
	       removeZPtWeights_(0), signalTrainFraction_(0.),
	       backgroundTrainFraction_(0.), useTauFakeSF_(1) {}
};

//run the histogrammer over a specific channel
Int_t process_channel(datacard_t& card, config_t& config, TDirectoryFile* fChannel) {
  //for splitting DY lepton decays
  bool isDY = false; //Not implemented yet
  bool isSignal = (card.fname_.Contains("MuTau") || card.fname_.Contains("ETau")
		   || card.fname_.Contains("EMu") || card.fname_.Contains("Signal"));
  //for avoiding double counting data events
  bool isElectronData = card.isData_ == 1;
  bool isMuonData = card.isData_ == 2;

  TTree* tree = 0;
  TH1F* eventsChannel = 0;
  TKey* key2 = 0;
  TIter nextkey2(fChannel->GetListOfKeys());
  //get events tree and events counting histogram
  while((key2 = (TKey*)nextkey2())) {
    TObject* obj2 = key2->ReadObj();
    if(obj2->IsA()->InheritsFrom(TH1::Class())) eventsChannel = (TH1F*)obj2;
    if(obj2->IsA()->InheritsFrom(TTree::Class())) {
      auto tmptree = (TTree*)obj2;
      //take the latest tree iteration from the file in case of copies
      if(tree) tree = (tmptree->GetEntriesFast() > tree->GetEntriesFast()) ? tmptree : tree;
      else tree = tmptree;
    }
  }
  //check that these are found
  if(tree == 0) {
    printf("Tree in %s/%s not found, continuing\n",card.filepath_.Data(),fChannel->GetName());
    return 1;
  }
  if(eventsChannel == 0) {
    printf("Events Channel Histogram in %s/%s not found, continuing processing without it\n",card.filepath_.Data(),fChannel->GetName());
  }
  // if Drell-Yan, loop through it twice, doing tautau then ee/mumu
  int nloops = (isDY) ? 2 : 1;
  for(int loop = 1; loop <= nloops; ++loop) {
    ZTauTauHistMaker* selec = new ZTauTauHistMaker(); //selector
    selec->fFolderName = fChannel->GetName();
    if(isDY) selec->fDYType = loop; //if Drell-Yan, tell the selector which loop we're on
    //skip electron data events with both triggers for e+mu channel, to not double count
    selec->fIsData = card.isData_;
    selec->fSkipDoubleTrigger = (isElectronData && (selec->fFolderName == "emu" || selec->fFolderName == "llg_study"));	
    //store a label for this dataset
    selec->fEventCategory = card.category_;
    selec->fWriteTrees = selec->fIsData == 0 && config.writeTrees_; //don't write trees for data
    selec->fUseTauFakeSF = config.useTauFakeSF_; //whether or not to use fake tau weights from analyzer/locally re-defined
    selec->fXsec = card.xsec_;
    selec->fXsec /= (card.events_->GetBinContent(1) - 2.*card.events_->GetBinContent(10));; //for writing trees with correct normalization
    selec->fRemoveZPtWeights = config.removeZPtWeights_; //whether or not to re-weight Z pT
    selec->fFractionMVA = (isSignal) ? config.signalTrainFraction_ : config.backgroundTrainFraction_; //fraction of events to use for MVA training
    if(isMuonData || isElectronData) selec->fFractionMVA = 0.; //don't split off data for training
    selec->fReprocessMVAs = config.reProcessMVAs_; //whether to evaluate the MVAs or use defined ones from the trees
    selec->fIsNano = true;

    tree->Process(selec,""); //run the selector over the tree

    //open back up the file
    TFile* out = TFile::Open(Form("ztautau_%s%s_%s.hist",fChannel->GetName(),
				  (isDY) ? Form("_%i",loop) : "", tree->GetName()),"UPDATE");
    if(out == 0) {
      printf("Unable to find output hist file ztautau_%s%s_%s.hist, continuing\n",fChannel->GetName(),
	     (isDY) ? Form("_%i",loop) : "", tree->GetName());
      continue;
    }
    //add the events histogram to the output
    card.events_->Write();
    out->Write();
    delete out;
  }

  return 0;
}

//run the histogrammer over a data file
Int_t process_card(datacard_t& card, config_t& config, TFile* file) {
  TH1F* events = 0;
  TKey* key = 0;
  TIter nextkey(file->GetListOfKeys());

  //get events histograms first
  while((key = (TKey*)nextkey())) {
    TObject* obj = key->ReadObj();
    if(obj->IsA()->InheritsFrom(TH1::Class())) events = (TH1F*)obj;
  }
  if(events == 0) {
    printf("Events Histogram in %s not found, continuing\n",card.filepath_.Data());
    return 1;
  }

  //for avoiding double counting data events
  bool isElectronData = card.isData_ == 1;
  bool isMuonData = card.isData_ == 2;
    
  card.events_ = events;
  TIter nextkeyT(file->GetListOfKeys());
  nextkeyT.Reset();
  TDirectoryFile* fChannel = 0;
  while((key = (TKey*)nextkeyT())) {
    TObject* obj = key->ReadObj();
    if(obj->IsA()->InheritsFrom(TDirectoryFile::Class())) fChannel = (TDirectoryFile*)obj;
    else {
      continue;
    }
    printf("Found file %s\n",fChannel->GetName());
    TString currentChannel = fChannel->GetName();
    //check if only suppose to do 1 channel, and if this is that channel
    if(config.onlyChannel_ != "") {
      if(config.onlyChannel_ != currentChannel) { cout << "Continuing!\n"; continue;}
      else cout << "Found correct channel --> processing!\n";
    }
    //check if this channel is part of the skip list
    if(config.skipChannels_.size() > 0) {
      bool skip = false;
      for(TString channel : config.skipChannels_) {
	if(channel == currentChannel) {cout << "Skipping channel!\n"; skip=true;}
      }
      if(skip) continue;
    }
    //skip if data on a channel without that trigger used
    if(isElectronData && (currentChannel == "mutau" || currentChannel == "mumu")) {
      cout << "Electron data on muon only channel, continuing!\n"; continue;
    }
    if(isMuonData && (currentChannel == "etau" || currentChannel == "ee")) {
      cout << "Muon data on electron only channel, continuing!\n"; continue;
    }
    process_channel(card, config, fChannel);
  }
  delete events;

  return 0;
}

Int_t process_ztautau() {

  /******************************/
  /* Define NANO AOD processing */
  /******************************/
  TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_test_trees/";

  vector<datacard_t> nanocards;
  //2016
  nanocards.push_back(datacard_t(true , 365.34                                  , "clfv_2016_ttbarToSemiLeptonic.tree"     , 0)); //1
  nanocards.push_back(datacard_t(true , 88.29                                   , "clfv_2016_ttbarlnu.tree"                , 0)); //2
  nanocards.push_back(datacard_t(true , 6225.42                                 , "clfv_2016_DY50.tree"                    , 0)); //3
  nanocards.push_back(datacard_t(true , 34.91                                   , "clfv_2016_SingleAntiToptW.tree"         , 0)); //4
  nanocards.push_back(datacard_t(true , 34.91                                   , "clfv_2016_SingleToptW.tree"             , 0)); //5
  nanocards.push_back(datacard_t(true , 52850.0                                 , "clfv_2016_Wlnu.tree"                    , 0)); //6
  nanocards.push_back(datacard_t(true , 12.178                                  , "clfv_2016_WW.tree"                      , 0)); //7
  nanocards.push_back(datacard_t(true , 27.6                                    , "clfv_2016_WZ.tree"                      , 0)); //8
  nanocards.push_back(datacard_t(true , ((6225.42)/(3.*3.3658e-2))*9.8e-6       , "clfv_2016_ZETau.tree"                   , 0)); //9
  nanocards.push_back(datacard_t(true , ((6225.42)/(3.*3.3658e-2))*1.2e-5       , "clfv_2016_ZMuTau.tree"                  , 0)); //10
  nanocards.push_back(datacard_t(true , 2075.14/0.0337*7.3e-7                   , "clfv_2016_ZEMu.tree"                    , 0)); //11
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*6.1e-3 , "clfv_2016_HETau.tree"                   , 0)); //12
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*2.5e-3 , "clfv_2016_HMuTau.tree"                  , 0)); //13
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*3.5e-4 , "clfv_2016_HEMu.tree"                    , 0)); //14
  nanocards.push_back(datacard_t(true , 1.                                      , "clfv_2016_SingleMu.tree"                , 2)); //15
  nanocards.push_back(datacard_t(true , 1.                                      , "clfv_2016_SingleEle.tree"               , 1)); //16
  nanocards.push_back(datacard_t(true , 12.14                                   , "clfv_2016_ZZ.tree"                      , 0)); //17
  nanocards.push_back(datacard_t(true , 0.2086                                  , "clfv_2016_WWW.tree"                     , 0)); //18
  nanocards.push_back(datacard_t(true , 22180.                                  , "clfv_2016_QCDDoubleEMEnrich30to40.tree" , 0)); //19
  nanocards.push_back(datacard_t(true , 247000.                                 , "clfv_2016_QCDDoubleEMEnrich30toInf.tree", 0)); //20
  nanocards.push_back(datacard_t(true , 113100.                                 , "clfv_2016_QCDDoubleEMEnrich40toInf.tree", 0)); //21
  //2017
  nanocards.push_back(datacard_t(false, 365.34                                  , "clfv_2017_ttbarToSemiLeptonic.tree", 0)); //
  nanocards.push_back(datacard_t(false, 88.29                                   , "clfv_2017_ttbarlnu.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, 6225.42                                 , "clfv_2017_DY50.tree"               , 0)); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2017_SingleAntiToptW.tree"    , 0)); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2017_SingleToptW.tree"        , 0)); //
  nanocards.push_back(datacard_t(false, 52850.0                                 , "clfv_2017_Wlnu.tree"               , 0)); //
  nanocards.push_back(datacard_t(false, 12.178                                  , "clfv_2017_WW.tree"                 , 0)); //
  nanocards.push_back(datacard_t(false, 27.6                                    , "clfv_2017_WZ.tree"                 , 0)); //
  nanocards.push_back(datacard_t(false, 2075.14/0.0337*7.3e-7                   , "clfv_2017_Signal.tree"             , 0)); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2017_SingleMu.tree"           , 2)); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2017_SingleEle.tree"          , 1)); //
  //2018
  nanocards.push_back(datacard_t(false, 365.34                                  , "clfv_2018_ttbarToSemiLeptonic.tree", 0)); //
  nanocards.push_back(datacard_t(false, 88.29                                   , "clfv_2018_ttbarlnu.tree"           , 0)); //
  nanocards.push_back(datacard_t(false, 6225.42                                 , "clfv_2018_DY50.tree"               , 0)); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2018_SingleAntiToptW.tree"    , 0)); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2018_SingleToptW.tree"        , 0)); //
  nanocards.push_back(datacard_t(false, 52850.0                                 , "clfv_2018_Wlnu.tree"               , 0)); //
  nanocards.push_back(datacard_t(false, 12.178                                  , "clfv_2018_WW.tree"                 , 0)); //
  nanocards.push_back(datacard_t(false, 27.6                                    , "clfv_2018_WZ.tree"                 , 0)); //
  nanocards.push_back(datacard_t(false, 2075.14/0.0337*7.3e-7                   , "clfv_2018_Signal.tree"             , 0)); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2018_SingleMu.tree"           , 2)); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2018_SingleEle.tree"          , 1)); //


  TStopwatch* timer = new TStopwatch();

  config_t config;
  config.useTauFakeSF_ = 1; //1 = use given scale factors, 2 = override them with local ones
  config.writeTrees_ = true;
  config.onlyChannel_ = "ee";
  config.skipChannels_ = {"all", "jets", "llg_study"};

  config.reProcessMVAs_ = false;
  config.removeZPtWeights_ = 0;
  config.signalTrainFraction_ = 0.7;
  config.backgroundTrainFraction_ = 0.3;
  cout << "--- Fake Tau SF mode: " << config.useTauFakeSF_
       << ", Write Trees mode: " << config.writeTrees_
       << ", Remove z pt weights: " << config.removeZPtWeights_
       << ", training fractions: signal = " << config.signalTrainFraction_
       << ", background = " << config.backgroundTrainFraction_
       << endl;
  if(config.skipChannels_.size() > 0) {
    cout << "--- Skipping channels: ";
    for(TString channel : config.skipChannels_)
      cout << channel.Data() << ", ";
    cout << endl;
  }
  if(config.onlyChannel_ != "")
    cout << "--- WARNING! Only processing " << config.onlyChannel_.Data() << " channel!\n";

  //loop over data formats, 0 = bltTrees from ZTauTauAnalyzer, 1 = trees from converting Nano AODS
  Int_t category = 0;
  unsigned nfiles = nanocards.size();
  for(unsigned i = 0; i < nfiles; ++i) {
    ++category; // could have just used i + 1?
    cout << "Loop " << i << ", category " << category
	 << ", file " << nanocards[i].fname_.Data()
	 << ", xsec " << nanocards[i].xsec_
	 << ", doProcess " << nanocards[i].process_ << endl;
    if(!nanocards[i].process_) continue;
    nanocards[i].category_ = category;
    TString filepath = nanoaod_path + nanocards[i].fname_;
    TFile* f = TFile::Open((filepath).Data(),"READ");
    if(f == 0) {
      printf("File %s not found, continuing\n",filepath.Data());
      continue;
    }
    printf("using %s\n",filepath.Data());
    nanocards[i].filepath_ = filepath;
    int status = process_card(nanocards[i], config, f);
    delete f;
  } //end file loop
  //report the time spent histogramming
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
