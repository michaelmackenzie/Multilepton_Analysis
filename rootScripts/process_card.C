//information for debugging
using namespace CLFV;
#include "histogramming_config.C"

//run the histogrammer over a specific channel
Int_t process_channel(datacard_t& card, config_t& config, TString selection, TTree* tree) {
  //for splitting DY lepton decays and Z pT vs mass weights
  const bool isDY = card.fname_.Contains("DY");
  const bool isWJ = card.fname_.Contains("Wlnu_") || card.fname_.Contains("Wlnu-ext_");

  const bool isSignal = (card.fname_.Contains("MuTau") || card.fname_.Contains("ETau")
                   || card.fname_.Contains("EMu") || card.fname_.Contains("Signal")) && !card.fname_.Contains("Embed");

  //for avoiding double counting data events
  const bool isElectronData = card.isData_ == 1;
  const bool isMuonData     = card.isData_ == 2;

  //check that the tree is defined
  if(tree == 0) {
    printf("Tree in %s/%s not found, continuing\n",card.filepath_.Data(), selection.Data());
    return 1;
  }

  // if Drell-Yan, loop through it twice, doing tautau then ee/mumu decays
  const int ndyloops = (isDY && splitDY_) ? 2 : 1;

  // if W+Jets and splitting, do N(jets) = 0 - 4 loops, else just do loop -1 = no splitting
  const int nwloops = (isWJ && splitWJets_) ? 5 : 0;

  cout << "Processing " << card.fname_.Data() << " with eventCategory = " << card.category_ << " isSignal = " << isSignal << " isDY = " << isDY
       << " isWJets = " << isWJ << " isData = " << card.isData_ << " useSystematics = " << doSystematics_ << " doMVASets = " << DoMVASets_
       << " removeTrigWeights = " << removeTrigWeights_ << endl;

  for(int wjloop = -1; wjloop < nwloops; ++wjloop) { //start from -1 to also do unsplit histogram
    for(int dyloop = 1; dyloop <= ndyloops; ++dyloop) {
      CLFVHistMaker* selec = new CLFVHistMaker(systematicSeed_); //selector
      selec->fSelection = selection;

      selec->fDYTesting         = DYTesting_;
      selec->fDYFakeTauTesting  = DYFakeTau_;
      selec->fWJFakeTauTesting  = WJFakeTau_;
      selec->fTTFakeTauTesting  = TTFakeTau_;
      selec->fQCDFakeTauTesting = QCDFakeTau_;
      selec->fJetTauTesting     = JetTauTesting_;
      selec->fCutFlowTesting    = CutFlowTesting_;
      selec->fTriggerTesting    = TriggerTesting_;

      selec->fDoMVASets = DoMVASets_ > 0 && (DoMVASets_ > 2 || (DoMVASets_ == 2 && !(selection.Contains("tau"))) || (selection == "emu"));

      selec->fRemoveTriggerWeights = removeTrigWeights_;
      selec->fUpdateMCEra          = updateMCEra_;
      selec->fUseBTagWeights       = useBTagWeights_;
      selec->fRemovePUWeights      = removePUWeights_ ; //remove for signal, due to unknown issues in skimming NanoAODs
      selec->fUseJetPUIDWeights    = useJetPUIDWeights_;
      selec->fUsePrefireWeights    = usePrefireWeights_;
      selec->fRemoveZPtWeights     = removeZPtWeights_; //whether or not to re-weight Z pT
      selec->fUseQCDWeights        = useQCDWeights_;

      selec->fUseEmbedCuts         = useEmbedCuts_;
      selec->fEmbeddedTesting      = embeddedTesting_;

      selec->fUseMCEstimatedFakeLep  = useMCFakeLep_;
      selec->fUseJetToTauComposition = useJetToTauComp_;

      if(isDY && splitDY_)     selec->fDYType = dyloop; //if Drell-Yan, tell the selector which loop we're on
      if(isWJ && splitWJets_)  selec->fWNJets = wjloop; //if W+Jets, tell the selector which loop we're on
      selec->fIsDY = isDY;
      selec->fIsData = card.isData_;
      selec->fIsEmbed = card.fname_.Contains("Embed-");
      selec->fYear = card.year_;
      selec->fDataset = card.dataset_;
      selec->fIsSignal = isSignal;

      selec->fDoSystematics = config.doSystematics_;
      //skip electron data events with both triggers for e+mu channel, to not double count
      selec->fSkipDoubleTrigger = isElectronData && (selection == "emu");
      //store a label for this dataset
      selec->fEventCategory = card.category_;
      selec->fWriteTrees = selection != "mumu" && selection != "ee" && config.writeTrees_; //don't write trees for data
      selec->fUseTauFakeSF = config.useTauFakeSF_; //whether or not to use fake tau weights from analyzer/locally re-defined
      if(card.isData_ == 0) {
        selec->fXsec = card.xsec_;
        selec->fXsec /= (selec->fIsEmbed) ? 1. : (card.events_->GetBinContent(1) - 2.*card.events_->GetBinContent(10));; //for writing trees with correct normalization
        selec->fLum = (selec->fIsEmbed) ? 1. : card.lum_; //for adding lum to tree weights
      } else {
        CrossSections xs(useUL_);
        selec->fXsec = xs.GetCrossSection(Form("QCD_%s", selection.Data()));
        selec->fLum = 1.; //no luminosity needed for data
      }
      selec->fFractionMVA = (isSignal) ? config.signalTrainFraction_ : config.backgroundTrainFraction_; //fraction of events to use for MVA training
      if(selection == "mumu" || selection == "ee") selec->fFractionMVA = 0.; //don't split off same flavor data
      if((isMuonData || isElectronData) && selection != "emu") selec->fFractionMVA = 0.; //don't split off data for training
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
  CrossSections xs(useUL_); //cross check gen number and negative fraction
  TString name = card.fname_;
  name.ReplaceAll("LFVAnalysis_", "");
  name.ReplaceAll("_2016.root", "");
  name.ReplaceAll("_2017.root", "");
  name.ReplaceAll("_2018.root", "");

  //Get the normalization event count
  TH1* events = (TH1*) file->Get("events");
  if(!events) {
    printf("Events Histogram in %s not found, continuing\n",card.filepath_.Data());
    return 1;
  }
  TTree* runs = (TTree*) file->Get("Runs");
  if(runs) {
    Long64_t runs_count = 0;
    Long64_t runs_count_loop = 0;
    bool replace = true;
    if(runs->GetBranch("genEventCount")) {
      runs->SetBranchAddress("genEventCount", &runs_count_loop);
    } else if(runs->GetBranch("genEventCount_")) {
      runs->SetBranchAddress("genEventCount_", &runs_count_loop);
    } else {
      cout << "No gen-event count found in the Runs tree!\n";
      replace = false;
    }
    if(replace) {
      for(int ientry = 0; ientry < runs->GetEntriesFast(); ++ientry) {
        runs->GetEntry(ientry);
        runs_count += runs_count_loop;
      }
      const int events_count = events->GetBinContent(1);
      double neg_frac = xs.GetNegativeFraction(name, card.year_);
      if(neg_frac < 1.e-6) neg_frac = events->GetBinContent(10)/events_count;
      cout << "Runs tree N(events): " << runs_count << ", event hist: " << events_count
           << " (neg frac = " << neg_frac << ") --> Difference = " << runs_count - events_count << endl;
      events->SetBinContent(1 , runs_count);
      events->SetBinContent(10, runs_count*neg_frac);
    }
  } else {
    cout << "Runs tree not found!\n";
  }

  //for avoiding double counting data events in muon+electron data files
  const bool isElectronData = card.isData_ == 1;
  const bool isMuonData = card.isData_ == 2;

  card.events_ = events;
  if(xs.GetGenNumber(name, card.year_) > 0) {
    const int ngen = xs.GetGenNumber(name, card.year_);
    const int nevt = events->GetBinContent(1);
    cout << "*** Data set generated with " << ngen << " events and skimming shows " << nevt << " events ***\n";
  }

  //Loop throught the file looking for all selection channels
  name.ReplaceAll("Run2016", "-"); //shorten data names
  name.ReplaceAll("Run2017", "-");
  name.ReplaceAll("Run2018", "-");
  card.dataset_ = name;
  vector<TString> selections = {"mutau", "etau", "emu", "mumu", "ee"};
  for(TString selection : selections) {
    //check if only suppose to do 1 channel, and if this is that channel
    if(config.onlyChannel_ != "") {
      if(config.onlyChannel_ != selection) { cout << "Continuing!\n"; continue;}
      else cout << "Found correct channel --> processing!\n";
    }
    //check if this channel is part of the skip list
    if(config.skipChannels_.size() > 0) {
      bool skip = false;
      for(TString channel : config.skipChannels_) {
        if(channel == selection) {cout << "Skipping channel!\n"; skip=true; break;}
      }
      if(skip) continue;
    }
    //skip if data on a channel without that trigger used
    if(isElectronData && (selection == "mutau" || selection == "mumu")) {
      cout << "Electron data on muon only channel, continuing!\n"; continue;
    }
    if(isMuonData && (selection == "etau" || selection == "ee")) {
      cout << "Muon data on electron only channel, continuing!\n"; continue;
    }

    //Process this selection
    //Retrieve the data
    TTree* tree = (TTree*) file->Get(selection.Data());
    if(!tree) {
      cout << "Selection tree " << selection.Data() << " not found!\n";
      continue;
    }
    // //skip if a signal in an irrelevant channel
    // if(card.fname_.Contains("EMu.tree"  ) && (currentChannel != "emu")) continue;
    // if(card.fname_.Contains("ETau.tree" ) && (currentChannel == "mutau" || currentChannel == "mumu")) continue;
    // if(card.fname_.Contains("MuTau.tree") && (currentChannel == "etau"  || currentChannel == "ee"  )) continue;

    process_channel(card, config, selection, tree);
  }

  return 0;
}

Int_t process_card(TString treepath, TString filename, double xsec, int isdata, bool combine, int category) {

  datacard_t card(true, xsec, filename, isdata, combine);
  config_t config(get_config());
  CrossSections xs(useUL_);

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
  if(card.fname_.Contains("_2017"))      year = 2017;
  else if(card.fname_.Contains("_2018")) year = 2018;
  card.year_ = year;
  //if combining samples, weight each by the number of generated over total generated
  if(card.combine_) {
    TString name = card.fname_;
    name.ReplaceAll("LFVAnalysis_", "");
    name.ReplaceAll("_2016.root", "");
    name.ReplaceAll("_2017.root", "");
    name.ReplaceAll("_2018.root", "");
    const bool isext = name.Contains("-ext");
    name.ReplaceAll("-ext", "");
    long num1 = xs.GetGenNumber(name       , year); //get number of events per sample
    long num2 = xs.GetGenNumber(name+"-ext", year);
    if(num1 > 0 && num2 > 0) {
      const double frac = (isext) ? (num2*1./(num1+num2)) : (num1*1./(num1+num2));
      cout << "Multiplying the " << (name + ((isext) ? "-ext" : "")).Data() << "cross section by " << frac
           << " to combine the extension samples!\n";
      card.xsec_ *= frac;
    } else {
      cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << endl;
    }
  } //end combine extension samples
  card.category_ = category;
  card.lum_ = xs.GetLuminosity(year);
  TString filepath = treepath + card.fname_;
  TFile* f = TFile::Open(filepath.Data(),"READ");
  if(!f) {
    printf("File %s not found, returning\n",filepath.Data());
    return 1;
  }
  printf("Using %s\n",filepath.Data());
  card.filepath_ = filepath;
  int status = process_single_card(card, config, f);
  f->Close();

  return 0;
}
