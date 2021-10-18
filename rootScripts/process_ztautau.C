//information for debugging
using namespace CLFV;

bool debug_ = false;
Long64_t startEvent_ = 429;
Long64_t nEvents_ = 1; //at 20, verbosity returns to normal
ZTauTauHistMaker* selector_ = 0;

bool DYTesting_ = true; //speed up running for scale factor calculation/debugging
bool DYFakeTau_ = false; //speed up dy fake tau scale factor
bool WJFakeTau_ = true; //speed up w+jets fake tau scale factor
bool TTFakeTau_ = true; //speed up ttbar fake tau scale factor
bool QCDFakeTau_ = true; //speed up qcd fake tau scale factor
bool JetTauTesting_ = true; //perform MC closure test
bool CutFlowTesting_ = false; //test just basic cutflow sets

int removeTrigWeights_ = 4; //0: do nothing 1: remove weights 2: replace 3: replace P(event) 4: replace P(at least 1 triggered)
int removeBTagWeights_ = 2;//2;
int removeZPtWeights_ = 2;//2;
int useJetPUIDWeights_ = 1;//1;
int useMCFakeLep_ = 0; //use MC estimated light leptons in ee, mumu, and emu categories
int useJetToTauComp_ = 1; //use the composition of the anti-iso region to combine j->tau weights
int useEmbedCuts_ = 1; //use kinematic cuts based on embedded generation
int embeddedTesting_ = 0; //test embedding options

int systematicSeed_ = 90; //seed for systematic random shifts
int doSystematics_ = 0;//1;
bool writeTrees_ = false; //true
int  DoMVASets_ = 1; //Fill sets with MVA cuts: 1 = emu, 2 = emu/ee/mumu3 = all sets

int splitWJets_ = true; //split w+jets sample based on N(LHE jets)
bool splitDY_ = true; //split z+jets sample based on gen-level lepton pair flavor

//information about the data file/data
struct datacard_t {
  bool process_;
  double xsec_;
  TString fname_;
  TString filepath_;
  int isData_;
  bool combine_;
  int category_;
  TH1F* events_;
  int year_;
  double lum_;
  datacard_t(bool process, double xsec, TString fname,
             int isData = 0,  bool combine = false,
             TString filepath = "",
             int category = 0, TH1F* events = 0,
             int year = 2016) :
    process_(process), xsec_(xsec), fname_(fname), combine_(combine),
    filepath_(filepath), isData_(isData), category_(category),
    events_(events), year_(year), lum_(0.) {}
};

//information about the selector configuration
struct config_t {
  Bool_t writeTrees_;
  TString onlyChannel_;
  vector<TString> skipChannels_;
  Bool_t reProcessMVAs_;
  float signalTrainFraction_;
  float backgroundTrainFraction_;
  Int_t useTauFakeSF_;
  Int_t doSystematics_;
  config_t() : writeTrees_(true), onlyChannel_(""), skipChannels_(),
               reProcessMVAs_(false),
               signalTrainFraction_(0.),
               backgroundTrainFraction_(0.), useTauFakeSF_(1),
               doSystematics_(0){}
};

//run the histogrammer over a specific channel
Int_t process_channel(datacard_t& card, config_t& config, TDirectoryFile* fChannel) {
  //for splitting DY lepton decays and Z pT vs mass weights
  bool isDY = card.fname_.Contains("DY");
  bool isWJ = card.fname_.Contains("Wlnu.tree") || card.fname_.Contains("Wlnu-ext.tree");

  bool isSignal = (card.fname_.Contains("MuTau") || card.fname_.Contains("ETau")
                   || card.fname_.Contains("EMu") || card.fname_.Contains("Signal"));
  isSignal &= card.fname_.Contains("Embed"); //reject the embedding samples
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
    if(!obj2) continue;
    if(obj2->IsA()->InheritsFrom(TH1::Class())) eventsChannel = (TH1F*)obj2;
    if(obj2->IsA()->InheritsFrom(TTree::Class())) {
      TTree* tmptree = (TTree*)obj2;
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
  int ndyloops = (isDY && splitDY_) ? 2 : 1;
  // if W+Jets and splitting, do N(jets) = 0 - 4 loops, else just do loop -1 = no splitting
  int nwloops = (isWJ && splitWJets_) ? 5 : 0;
  cout << "Processing " << card.fname_.Data() << " with eventCategory = " << card.category_ << " isSignal = " << isSignal << " isDY = " << isDY
       << " isWJets = " << isWJ << " isData = " << card.isData_ << " useSystematics = " << doSystematics_ << " doMVASets = " << DoMVASets_
       << " removeTrigWeights = " << removeTrigWeights_ << endl;
  for(int wjloop = -1; wjloop < nwloops; ++wjloop) { //start from -1 to also do unsplit histogram
    for(int dyloop = 1; dyloop <= ndyloops; ++dyloop) {
      ZTauTauHistMaker* selec = new ZTauTauHistMaker(systematicSeed_); //selector
      selec->fFolderName = fChannel->GetName();

      selec->fDYTesting = DYTesting_;
      selec->fDYFakeTauTesting = DYFakeTau_;
      selec->fWJFakeTauTesting = WJFakeTau_;
      selec->fTTFakeTauTesting = TTFakeTau_;
      selec->fQCDFakeTauTesting = QCDFakeTau_;
      selec->fJetTauTesting = JetTauTesting_;
      selec->fCutFlowTesting = CutFlowTesting_;
      selec->fDoMVASets = DoMVASets_ > 0 && (DoMVASets_ > 2 || (DoMVASets_ == 2 && !(selec->fFolderName.Contains("tau"))) || (selec->fFolderName == "emu"));
      selec->fRemoveTriggerWeights = removeTrigWeights_;
      selec->fRemoveBTagWeights = removeBTagWeights_;
      // selec->fRemovePUWeights = 2*isSignal; //remove for signal, due to unknown issues in skimming NanoAODs
      selec->fUseJetPUIDWeights = useJetPUIDWeights_;
      selec->fUseEmbedCuts = useEmbedCuts_;
      selec->fEmbeddedTesting = embeddedTesting_;
      selec->fUseMCEstimatedFakeLep = useMCFakeLep_;
      selec->fUseJetToTauComposition = useJetToTauComp_;

      if(isDY && splitDY_)     selec->fDYType = dyloop; //if Drell-Yan, tell the selector which loop we're on
      if(isWJ && splitWJets_) selec->fWNJets = wjloop; //if W+Jets, tell the selector which loop we're on
      selec->fIsDY = isDY;
      selec->fIsData = card.isData_;
      selec->fIsEmbed = card.fname_.Contains("Embed-");
      selec->fYear = card.year_;
      selec->fDoSystematics = config.doSystematics_;
      //skip electron data events with both triggers for e+mu channel, to not double count
      selec->fSkipDoubleTrigger = (isElectronData && (selec->fFolderName == "emu" || selec->fFolderName == "llg_study"));
      //store a label for this dataset
      selec->fEventCategory = card.category_;
      selec->fWriteTrees = selec->fFolderName != "mumu" && selec->fFolderName != "ee" && config.writeTrees_; //don't write trees for data
      selec->fUseTauFakeSF = config.useTauFakeSF_; //whether or not to use fake tau weights from analyzer/locally re-defined
      if(card.isData_ == 0) {
        selec->fXsec = card.xsec_;
        selec->fXsec /= (card.events_->GetBinContent(1) - 2.*card.events_->GetBinContent(10));; //for writing trees with correct normalization
        selec->fLum = card.lum_; //for adding lum to tree weights
      } else {
        CrossSections xs;
        selec->fXsec = xs.GetCrossSection(Form("QCD_%s", selec->fFolderName.Data()));
        selec->fLum = 1.; //no luminosity needed for data
      }
      selec->fRemoveZPtWeights = removeZPtWeights_; //whether or not to re-weight Z pT
      selec->fIsSignal = isSignal;
      selec->fFractionMVA = (isSignal) ? config.signalTrainFraction_ : config.backgroundTrainFraction_; //fraction of events to use for MVA training
      if(selec->fFolderName == "mumu" || selec->fFolderName == "ee") selec->fFractionMVA = 0.; //don't split off same flavor data
      if((isMuonData || isElectronData) && selec->fFolderName != "emu") selec->fFractionMVA = 0.; //don't split off data for training
      selec->fReprocessMVAs = config.reProcessMVAs_; //whether to evaluate the MVAs or use defined ones from the trees
      selec->fIsNano = true;
      if(debug_ && nEvents_ < 20) selec->fVerbose = 1;
      else       selec->fVerbose = 0;
      selector_ = selec;
      if(!debug_)
        tree->Process(selec,""); //run the selector over the tree
      else
        tree->Process(selec,"", nEvents_, startEvent_); //process specific event range

      //open back up the file
      TString outname = selec->GetOutputName();
      TFile* out = TFile::Open(outname.Data(),"UPDATE");
      if(!out) {
        printf("!!! Unable to find output hist file %s, continuing\n", outname.Data());
        continue;
      }
      //add the events histogram to the output
      card.events_->Write();
      out->Write();
      delete out;
      if(!debug_) delete selec;
    } //end DY loop
  } //end W+Jets loop
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
  CrossSections xs; //cross check gen number
  TString name = card.fname_;
  name.ReplaceAll("clfv_", "");
  name.ReplaceAll("2016_", "");
  name.ReplaceAll("2017_", "");
  name.ReplaceAll("2018_", "");
  name.ReplaceAll(".tree", "");
  if(xs.GetGenNumber(name, card.year_) > 0) {
    int ngen = xs.GetGenNumber(name, card.year_);
    int nevt = events->GetBinContent(1);
    cout << "*** Data set generated with " << ngen << " events and skimming shows " << nevt << " events ***\n";
  }
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
  // TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees/";
  // TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_test_trees/";
  TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees_nomva/";
  // TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees_debug/";
  // TString nanoaod_path = "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_nanoaod_trees_UL/";

  printf("\033[32m--- Using tree path: %s\033[0m\n", nanoaod_path.Data());

  //cross section handler
  bool useUL = nanoaod_path.Contains("_UL");
  CrossSections xs(useUL);
  vector<datacard_t> nanocards;

  //////////////////
  // 2016 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2016_ttbarToSemiLeptonic.tree"     , 0)); //1
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2016_ttbarlnu.tree"                , 0)); //2
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2016_DY50-amc.tree"                , 0)); //25
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2016_SingleAntiToptW.tree"         , 0)); //4
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2016_SingleToptW.tree"             , 0)); //5
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2016_Wlnu.tree"                    , 0, useUL&&true)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2016_WW.tree"                      , 0)); //7
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2016_WZ.tree"                      , 0)); //8
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2016_ZETau.tree"                   , 0)); //9
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2016_ZMuTau.tree"                  , 0)); //10
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2016_ZEMu.tree"                    , 0)); //11
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2016_HETau.tree"                   , 0)); //12
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2016_HMuTau.tree"                  , 0)); //13
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2016_HEMu.tree"                    , 0)); //14
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2016_SingleMu.tree"                , 2)); //15
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2016_SingleEle.tree"               , 1)); //16
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2016_ZZ.tree"                      , 0)); //17
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2016_WWW.tree"                     , 0)); //18
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2016_Wlnu-ext.tree"                , 0, useUL&&true)); //22
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2016_ttbarToHadronic.tree"         , 0)); //24
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"                 ), "clfv_2016_Wlnu-1J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"                 ), "clfv_2016_Wlnu-2J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"                 ), "clfv_2016_Wlnu-3J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"                 ), "clfv_2016_Wlnu-4J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-B"           ), "clfv_2016_Embed-MuTau-B.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-C"           ), "clfv_2016_Embed-MuTau-C.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-D"           ), "clfv_2016_Embed-MuTau-D.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-E"           ), "clfv_2016_Embed-MuTau-E.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-F"           ), "clfv_2016_Embed-MuTau-F.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-G"           ), "clfv_2016_Embed-MuTau-G.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-MuTau-H"           ), "clfv_2016_Embed-MuTau-H.tree"           , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-B"            ), "clfv_2016_Embed-ETau-B.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-C"            ), "clfv_2016_Embed-ETau-C.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-D"            ), "clfv_2016_Embed-ETau-D.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-E"            ), "clfv_2016_Embed-ETau-E.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-F"            ), "clfv_2016_Embed-ETau-F.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-G"            ), "clfv_2016_Embed-ETau-G.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-ETau-H"            ), "clfv_2016_Embed-ETau-H.tree"            , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-B"             ), "clfv_2016_Embed-EMu-B.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-C"             ), "clfv_2016_Embed-EMu-C.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-D"             ), "clfv_2016_Embed-EMu-D.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-E"             ), "clfv_2016_Embed-EMu-E.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-F"             ), "clfv_2016_Embed-EMu-F.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-G"             ), "clfv_2016_Embed-EMu-G.tree"             , 0)); //
  nanocards.push_back(datacard_t(true , xs.GetCrossSection("Embed-EMu-H"             ), "clfv_2016_Embed-EMu-H.tree"             , 0)); //
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2016_DY50.tree"                    , 0, useUL&&true)); //3
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2016_DY50-ext.tree"                , 0, useUL&&true)); //23
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2016_QCDDoubleEMEnrich30to40.tree" , 0)); //19
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2016_QCDDoubleEMEnrich30toInf.tree", 0)); //20
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2016_QCDDoubleEMEnrich40toInf.tree", 0)); //21
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunB.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunC.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunD.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunE.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunF.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunG.tree"          , 2)); //15
  // nanocards.push_back(datacard_t(true , 1.                                            , "clfv_2016_SingleMuonRunH.tree"          , 2)); //15

  //////////////////
  // 2017 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2017_ttbarToSemiLeptonic.tree"     , 0)); //25
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2017_ttbarToHadronic.tree"         , 0)); //26
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2017_ttbarlnu.tree"                , 0)); //27
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2017_DY50.tree"                    , 0, true)); //28
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2017_DY50-ext.tree"                , 0, true)); //48
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2017_SingleAntiToptW.tree"         , 0)); //29
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2017_SingleToptW.tree"             , 0)); //30
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2017_Wlnu.tree"                    , 0, true)); //31
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2017_Wlnu-ext.tree"                , 0, true)); //32
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"                 ), "clfv_2017_Wlnu-1J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"                 ), "clfv_2017_Wlnu-2J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"                 ), "clfv_2017_Wlnu-3J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2017_WW.tree"                      , 0)); //33
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2017_WZ.tree"                      , 0)); //34
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2017_ZETau.tree"                   , 0)); //35
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2017_ZMuTau.tree"                  , 0)); //36
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2017_ZEMu.tree"                    , 0)); //37
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2017_HETau.tree"                   , 0)); //38
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2017_HMuTau.tree"                  , 0)); //39
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2017_HEMu.tree"                    , 0)); //40
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2017_SingleMu.tree"                , 2)); //41
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2017_SingleEle.tree"               , 1)); //42
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2017_ZZ.tree"                      , 0)); //43
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2017_WWW.tree"                     , 0)); //44
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2017_QCDDoubleEMEnrich30to40.tree" , 0)); //45
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2017_QCDDoubleEMEnrich30toInf.tree", 0)); //46
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2017_QCDDoubleEMEnrich40toInf.tree", 0)); //47

  //////////////////
  // 2018 samples //
  //////////////////
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToSemiLeptonic"     ), "clfv_2018_ttbarToSemiLeptonic.tree"     , 0)); //49
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarToHadronic"         ), "clfv_2018_ttbarToHadronic.tree"         , 0)); //50
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ttbarlnu"                ), "clfv_2018_ttbarlnu.tree"                , 0)); //51
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50-amc.tree"                , 0)); //53
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleAntiToptW"         ), "clfv_2018_SingleAntiToptW.tree"         , 0)); //54
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("SingleToptW"             ), "clfv_2018_SingleToptW.tree"             , 0)); //55
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu"                    ), "clfv_2018_Wlnu.tree"                    , 0)); //56
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-1J"                 ), "clfv_2018_Wlnu-1J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-2J"                 ), "clfv_2018_Wlnu-2J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-3J"                 ), "clfv_2018_Wlnu-3J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("Wlnu-4J"                 ), "clfv_2018_Wlnu-4J.tree"                 , 0)); //6
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WW"                      ), "clfv_2018_WW.tree"                      , 0)); //57
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WZ"                      ), "clfv_2018_WZ.tree"                      , 0)); //XXXX FIXME
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZETau"                   ), "clfv_2018_ZETau.tree"                   , 0)); //58
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZMuTau"                  ), "clfv_2018_ZMuTau.tree"                  , 0)); //59
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZEMu"                    ), "clfv_2018_ZEMu.tree"                    , 0)); //60
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HETau"                   ), "clfv_2018_HETau.tree"                   , 0)); //61
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HMuTau"                  ), "clfv_2018_HMuTau.tree"                  , 0)); //62
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("HEMu"                    ), "clfv_2018_HEMu.tree"                    , 0)); //63
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleMu.tree"                , 2)); //64
  nanocards.push_back(datacard_t(false, 1.                                            , "clfv_2018_SingleEle.tree"               , 1)); //65
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("ZZ"                      ), "clfv_2018_ZZ.tree"                      , 0)); //66
  nanocards.push_back(datacard_t(false, xs.GetCrossSection("WWW"                     ), "clfv_2018_WWW.tree"                     , 0)); //67
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30to40" ), "clfv_2018_QCDDoubleEMEnrich30to40.tree" , 0)); //68
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich30toInf"), "clfv_2018_QCDDoubleEMEnrich30toInf.tree", 0)); //69
  // nanocards.push_back(datacard_t(true , xs.GetCrossSection("QCDDoubleEMEnrich40toInf"), "clfv_2018_QCDDoubleEMEnrich40toInf.tree", 0)); //70
  // nanocards.push_back(datacard_t(false, xs.GetCrossSection("DY50"                    ), "clfv_2018_DY50.tree"                    , 0)); //52


  TStopwatch* timer = new TStopwatch();

  config_t config;
  config.useTauFakeSF_ = 1; //1 = use given scale factors, 2 = override them with local ones
  config.writeTrees_ = writeTrees_;
  config.onlyChannel_ = "mutau";
  config.skipChannels_ = {/*"mutau", "etau", "emu", "mumu", "ee",*/ "all", "jets", "llg_study"};
  config.reProcessMVAs_ = false;
  config.signalTrainFraction_ = 0.3;
  config.backgroundTrainFraction_ = 0.3;
  config.doSystematics_ = doSystematics_;
  cout << "--- Fake Tau SF mode: " << config.useTauFakeSF_
       << ", Write Trees mode: " << config.writeTrees_
       << ", Remove z pt weights: " << removeZPtWeights_
       << ", training fractions: signal = " << config.signalTrainFraction_
       << ", background = " << config.backgroundTrainFraction_
       << ", doSystematics = " << config.doSystematics_
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
    if(!debug_ || nanocards[i].process_)
      cout << "Loop " << i << ", category " << category
           << ", file " << nanocards[i].fname_.Data()
           << ", xsec " << nanocards[i].xsec_
           << ", doProcess " << nanocards[i].process_ << endl;
    if(!nanocards[i].process_) continue;
    //if combining Wlnu samples, weight each by the number of generated over total generated
    int year = 2016;
    if(nanocards[i].fname_.Contains("2017"))      year = 2017;
    else if(nanocards[i].fname_.Contains("2018")) year = 2018;
    nanocards[i].year_ = year;
    if(nanocards[i].combine_) { //only for 2016, 2017 Wlnu samples
      TString name = nanocards[i].fname_;
      name.ReplaceAll("clfv_", "");
      name.ReplaceAll("2016_", "");
      name.ReplaceAll("2017_", "");
      name.ReplaceAll("2018_", "");
      name.ReplaceAll(".tree", "");
      bool isext = name.Contains("-ext");
      name.ReplaceAll("-ext", "");
      long num1 = xs.GetGenNumber(name       , year); //get number of events per sample
      long num2 = xs.GetGenNumber(name+"-ext", year);
      if(isext && num1 > 0 && num2 > 0) {
        double frac = (num2*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << "-ext cross section by " << frac
             << " to combine the extension samples!\n";
        nanocards[i].xsec_ *= frac;
      } else if(num1 > 0 && num2 > 0){
        double frac = (num1*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << " cross section by " << frac
             << " to combine the extension samples!\n";
        nanocards[i].xsec_ *= frac;
      } else
        cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << endl;
    } //end combine extension samples
    nanocards[i].category_ = category;
    nanocards[i].lum_ = xs.GetLuminosity(year);
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
