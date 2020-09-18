
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

struct datacard_t {
  bool process_;
  double xsec_;
  TString fname_;
  datacard_t(bool process, double xsec, TString fname) :
    process_(process), xsec_(xsec), fname_(fname) {}
};
  
Int_t process_ztautau() {

  /******************************/
  /* Define NANO AOD processing */
  /******************************/
  TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_test_trees/";

  vector<datacard_t> nanocards;
  //2016
  nanocards.push_back(datacard_t(true , 365.34                                  , "clfv_2016_ttbarToSemiLeptonic.tree"     )); //1
  nanocards.push_back(datacard_t(true , 88.29                                   , "clfv_2016_ttbarlnu.tree"                )); //2
  nanocards.push_back(datacard_t(true , 6225.42                                 , "clfv_2016_DY50.tree"                    )); //3
  nanocards.push_back(datacard_t(true , 34.91                                   , "clfv_2016_SingleAntiToptW.tree"         )); //4
  nanocards.push_back(datacard_t(true , 34.91                                   , "clfv_2016_SingleToptW.tree"             )); //5
  nanocards.push_back(datacard_t(true , 52850.0                                 , "clfv_2016_Wlnu.tree"                    )); //6
  nanocards.push_back(datacard_t(true , 12.178                                  , "clfv_2016_WW.tree"                      )); //7
  nanocards.push_back(datacard_t(true , 27.6                                    , "clfv_2016_WZ.tree"                      )); //8
  nanocards.push_back(datacard_t(true , ((6225.42)/(3.*3.3658e-2))*9.8e-6       , "clfv_2016_ZETau.tree"                   )); //9
  nanocards.push_back(datacard_t(true , ((6225.42)/(3.*3.3658e-2))*1.2e-5       , "clfv_2016_ZMuTau.tree"                  )); //10
  nanocards.push_back(datacard_t(true , 2075.14/0.0337*7.3e-7                   , "clfv_2016_ZEMu.tree"                    )); //11
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*6.1e-3 , "clfv_2016_HETau.tree"                   )); //12
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*2.5e-3 , "clfv_2016_HMuTau.tree"                  )); //13
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*3.5e-4 , "clfv_2016_HEMu.tree"                    )); //14
  nanocards.push_back(datacard_t(true , 1.                                      , "clfv_2016_SingleMu.tree"                )); //15
  nanocards.push_back(datacard_t(true , 1.                                      , "clfv_2016_SingleEle.tree"               )); //16
  nanocards.push_back(datacard_t(true , 12.14                                   , "clfv_2016_ZZ.tree"                      )); //17
  nanocards.push_back(datacard_t(true , 0.2086                                  , "clfv_2016_WWW.tree"                     )); //18
  nanocards.push_back(datacard_t(true , 22180.                                  , "clfv_2016_QCDDoubleEMEnrich30to40.tree" )); //19
  nanocards.push_back(datacard_t(true , 247000.                                 , "clfv_2016_QCDDoubleEMEnrich30toInf.tree")); //20
  nanocards.push_back(datacard_t(true , 113100.                                 , "clfv_2016_QCDDoubleEMEnrich40toInf.tree")); //21
  //2017
  nanocards.push_back(datacard_t(false, 365.34                                  , "clfv_2017_ttbarToSemiLeptonic.tree")); //
  nanocards.push_back(datacard_t(false, 88.29                                   , "clfv_2017_ttbarlnu.tree"           )); //
  nanocards.push_back(datacard_t(false, 6225.42                                 , "clfv_2017_DY50.tree"               )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2017_SingleAntiToptW.tree"    )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2017_SingleToptW.tree"        )); //
  nanocards.push_back(datacard_t(false, 52850.0                                 , "clfv_2017_Wlnu.tree"               )); //
  nanocards.push_back(datacard_t(false, 12.178                                  , "clfv_2017_WW.tree"                 )); //
  nanocards.push_back(datacard_t(false, 27.6                                    , "clfv_2017_WZ.tree"                 )); //
  nanocards.push_back(datacard_t(false, 2075.14/0.0337*7.3e-7                   , "clfv_2017_Signal.tree"             )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2017_SingleMu.tree"           )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2017_SingleEle.tree"          )); //
  //2018
  nanocards.push_back(datacard_t(false, 365.34                                  , "clfv_2018_ttbarToSemiLeptonic.tree")); //
  nanocards.push_back(datacard_t(false, 88.29                                   , "clfv_2018_ttbarlnu.tree"           )); //
  nanocards.push_back(datacard_t(false, 6225.42                                 , "clfv_2018_DY50.tree"               )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2018_SingleAntiToptW.tree"    )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2018_SingleToptW.tree"        )); //
  nanocards.push_back(datacard_t(false, 52850.0                                 , "clfv_2018_Wlnu.tree"               )); //
  nanocards.push_back(datacard_t(false, 12.178                                  , "clfv_2018_WW.tree"                 )); //
  nanocards.push_back(datacard_t(false, 27.6                                    , "clfv_2018_WZ.tree"                 )); //
  nanocards.push_back(datacard_t(false, 2075.14/0.0337*7.3e-7                   , "clfv_2018_Signal.tree"             )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2018_SingleMu.tree"           )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2018_SingleEle.tree"          )); //


  TStopwatch* timer = new TStopwatch();

  Int_t useTauFakeSF = 1; //1 = use given scale factors, 2 = override them with local ones
  bool writeTrees = true;
  TString onlyChannel = "";
  vector<TString> skipChannels = {"mumu", "ee", "all", "jets", "llg_study"};

  bool reProcessMVAs = false;
  Int_t removeZPtWeights = 0;
  float signalTrainFraction = 0.7;
  float backgroundTrainFraction = 0.3;
  cout << "--- Fake Tau SF mode: " << useTauFakeSF
       << ", Write Trees mode: " << writeTrees
       << ", Remove z pt weights: " << removeZPtWeights
       << ", training fractions: signal = " << signalTrainFraction
       << ", background = " << backgroundTrainFraction
       << endl;
  if(skipChannels.size() > 0) {
    cout << "--- Skipping channels: ";
    for(TString channel : skipChannels)
      cout << channel.Data() << ", ";
    cout << endl;
  }
  if(onlyChannel != "")
    cout << "--- WARNING! Only processing " << onlyChannel.Data() << " channel!\n";

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
    TString filepath = nanoaod_path + nanocards[i].fname_;
    TFile* f = TFile::Open((filepath).Data(),"READ");
    if(f == 0) {
      printf("File %s not found, continuing\n",filepath.Data());
      continue;
    }
    printf("using %s\n",filepath.Data());
    TH1F* events = 0;
    TKey* key = 0;
    TIter nextkey(f->GetListOfKeys());

    //get events histograms first
    while((key = (TKey*)nextkey())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TH1::Class())) events = (TH1F*)obj;
    }
    if(events == 0) {
      printf("Events Histogram in %s not found, continuing\n",filepath.Data());
      continue;
    }

    //for splitting DY lepton decays
    bool isDY = false; //Not implemented yet
    bool isSignal = (nanocards[i].fname_.Contains("MuTau") || nanocards[i].fname_.Contains("ETau")
		     || nanocards[i].fname_.Contains("EMu") || nanocards[i].fname_.Contains("Signal"));
    //for avoiding double counting data events
    bool isElectronData = nanocards[i].fname_.Contains("SingleEle");
    bool isMuonData = nanocards[i].fname_.Contains("SingleMu");
    

    TIter nextkeyT(f->GetListOfKeys());
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
      if(onlyChannel != "") {
	if(onlyChannel != currentChannel) { cout << "Continuing!\n"; continue;}
	else cout << "Found correct channel --> processing!\n";
      }
      //check if this channel is part of the skip list
      if(skipChannels.size() > 0) {
	bool skip = false;
	for(TString channel : skipChannels) {
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
	printf("Tree in %s/%s not found, continuing\n",filepath.Data(),fChannel->GetName());
	continue;
      }
      if(eventsChannel == 0) {
	printf("Events Channel Histogram in %s/%s not found, continuing processing without it\n",filepath.Data(),fChannel->GetName());
      }
      // if Drell-Yan, loop through it twice, doing tautau then ee/mumu
      int nloops = (isDY) ? 2 : 1;
      for(int loop = 1; loop <= nloops; ++loop) {
	ZTauTauHistMaker* selec = new ZTauTauHistMaker(); //selector
	selec->fFolderName = fChannel->GetName();
	if(isDY) selec->fDYType = loop; //if Drell-Yan, tell the selector which loop we're on
	//skip electron data events with both triggers for e+mu channel, to not double count
	selec->fIsData = 2*isMuonData + isElectronData;
	selec->fSkipDoubleTrigger = (isElectronData && (selec->fFolderName == "emu" || selec->fFolderName == "llg_study"));	
	//store a label for this dataset
	selec->fEventCategory = category;
	selec->fWriteTrees = selec->fIsData == 0 && writeTrees; //don't write trees for data
	selec->fUseTauFakeSF = useTauFakeSF; //whether or not to use fake tau weights from analyzer/locally re-defined
	selec->fXsec = nanocards[i].xsec_;
	selec->fXsec /= (events->GetBinContent(1) - 2.*events->GetBinContent(10)); //for writing trees with correct normalization
	selec->fRemoveZPtWeights = removeZPtWeights; //whether or not to re-weight Z pT
	selec->fFractionMVA = (isSignal) ? signalTrainFraction : backgroundTrainFraction; //fraction of events to use for MVA training
	if(isMuonData || isElectronData) selec->fFractionMVA = 0.; //don't split off data for training
	selec->fReprocessMVAs = reProcessMVAs; //whether to evaluate the MVAs or use defined ones from the trees
	selec->fIsNano = true;

	tree->Process(selec,""); //run the selector over the tree

	//open back up the file
	TFile* out = new TFile(Form("ztautau_%s%s_%s.hist",fChannel->GetName(),
				    (isDY) ? Form("_%i",loop) : "", tree->GetName()),"UPDATE");
	if(out == 0) {
	  printf("Unable to find output hist file ztautau_%s%s_%s.hist, continuing\n",fChannel->GetName(),
		 (isDY) ? Form("_%i",loop) : "", tree->GetName());
	  continue;
	}
	//add the events histogram to the output
	events->Write();
	out->Write();
	delete out;
      }
    }
    delete events;
    delete f;
  } //end file loop
  //report the time spent histogramming
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
