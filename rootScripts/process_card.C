//information for debugging
using namespace CLFV;
#include "histogramming_config.C"

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
      selec->fTriggerTesting = TriggerTesting_;
      selec->fDoMVASets = DoMVASets_ > 0 && (DoMVASets_ > 2 || (DoMVASets_ == 2 && !(selec->fFolderName.Contains("tau"))) || (selec->fFolderName == "emu"));
      selec->fRemoveTriggerWeights = removeTrigWeights_;
      selec->fRemoveBTagWeights = removeBTagWeights_;
      // selec->fRemovePUWeights = 2*isSignal; //remove for signal, due to unknown issues in skimming NanoAODs
      selec->fUseJetPUIDWeights = useJetPUIDWeights_;

      selec->fUseEmbedCuts = useEmbedCuts_;
      selec->fEmbeddedTesting = embeddedTesting_;
      if(embeddedTesting_ == 3) selec->fUseEmbedTnPWeights = 0;

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
        selec->fXsec /= (selec->fIsEmbed) ? 1. : (card.events_->GetBinContent(1) - 2.*card.events_->GetBinContent(10));; //for writing trees with correct normalization
        selec->fLum = (selec->fIsEmbed) ? 1. : card.lum_; //for adding lum to tree weights
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
      else                        selec->fVerbose = 0;
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
      out->Close();
      delete out;
      if(!debug_) delete selec;
    } //end DY loop
  } //end W+Jets loop
  return 0;
}

//run the histogrammer over a data file
Int_t process_single_card(datacard_t& card, config_t& config, TFile* file) {
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
    //skip channels with irrelevant embedding final states
    if(card.fname_.Contains("Embed-")) {
      if(currentChannel == "emu" && card.fname_.Contains("Tau-")) {
        cout << "Skipping irrelavant embedding final state!\n";
        continue;
      }
      if(currentChannel == "etau" && card.fname_.Contains("MuTau-")) {
        cout << "Skipping irrelavant embedding final state!\n";
        continue;
      }
      if(currentChannel == "mutau" && card.fname_.Contains("ETau-")) {
        cout << "Skipping irrelavant embedding final state!\n";
        continue;
      }
    }
    //skip if a signal in an irrelevant channel
    if(card.fname_.Contains("EMu.tree"  ) && (currentChannel != "emu")) continue;
    if(card.fname_.Contains("ETau.tree" ) && (currentChannel == "mutau" || currentChannel == "mumu")) continue;
    if(card.fname_.Contains("MuTau.tree") && (currentChannel == "etau"  || currentChannel == "ee"  )) continue;

    process_channel(card, config, fChannel);
  }
  delete events;

  return 0;
}

Int_t process_card(TString treepath, TString filename, double xsec, int isdata, bool combine, bool useUL, int category) {

  datacard_t card(true, xsec, filename, isdata, combine);
  config_t config(get_config(useUL));
  CrossSections xs(useUL);

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

  int year = 2016;
  if(card.fname_.Contains("2017"))      year = 2017;
  else if(card.fname_.Contains("2018")) year = 2018;
  card.year_ = year;
  //if combining Wlnu samples, weight each by the number of generated over total generated
  if(card.combine_) { //only for 2016, 2017 Wlnu samples
    TString name = card.fname_;
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
      card.xsec_ *= frac;
    } else if(num1 > 0 && num2 > 0){
      double frac = (num1*1./(num1+num2));
      cout << "Multiplying the " << name.Data() << " cross section by " << frac
           << " to combine the extension samples!\n";
        card.xsec_ *= frac;
    } else
      cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << endl;
  } //end combine extension samples
  card.category_ = category;
  card.lum_ = xs.GetLuminosity(year);
  TString filepath = treepath + card.fname_;
  TFile* f = TFile::Open(filepath.Data(),"READ");
  if(f == 0) {
    printf("File %s not found, returning\n",filepath.Data());
    return 1;
  }
  printf("Using %s\n",filepath.Data());
  card.filepath_ = filepath;
  int status = process_single_card(card, config, f);
  f->Close();

  return 0;
}
