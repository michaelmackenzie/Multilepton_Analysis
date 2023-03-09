//information for debugging
using namespace CLFV;
#include "histogramming_config.C"

//retrieve the histogram of event counting from each file in a chain, returning the sum of the inputs
TH1* events_from_chain(vector<TString> files) {
  TH1* events = nullptr;
  int counter(0), nfiles(files.size());
  for(TString file_name : files) {
    std::cout << "Events histogram progress:  " << Form("%3i/%3i", counter, nfiles) << "\r";
    std::cout.flush();
    TFile* file = TFile::Open(file_name.Data(), "READ");
    if(!file) continue;
    TH1* h = (TH1*) file->Get("events");
    if(!h) {
      cout << "Events histogram not found in file " << file->GetName() << endl;
      continue;
    }
    if(events) events->Add(h);
    else {events = h; events->SetDirectory(0);}
    file->Close();
    ++counter;
  }
  std::cout << "Events histogram progress:  " << Form("%3i/%3i", counter, nfiles) << "\n";
  std::cout.flush();
  return events;
}

TH1* events_from_chain(vector<TFile*> files) {
  TH1* events = nullptr;
  for(auto file : files) {
    if(!file) continue;
    TH1* h = (TH1*) file->Get("events");
    if(!h) {
      cout << "Events histogram not found in file " << file->GetName() << endl;
      continue;
    }
    if(events) events->Add(h);
    else {events = h; events->SetDirectory(0);}
  }
  return events;
}

//run the histogrammer over a specific channel
Int_t process_channel(datacard_t& card, config_t& config, TString selection, TChain* chain) {
  //for splitting DY lepton decays and Z pT vs mass weights
  const bool isDY = card.fname_.Contains("DY");
  const bool isWJ = card.fname_.Contains("Wlnu_") || card.fname_.Contains("Wlnu-ext_");

  const bool isSignal = (card.fname_.Contains("MuTau") || card.fname_.Contains("ETau")
                         || card.fname_.Contains("EMu")) && !card.fname_.Contains("Embed");

  //for avoiding double counting data events
  const bool isElectronData = card.isData_ == 1;
  const bool isMuonData     = card.isData_ == 2;

  //check that the chain is defined
  TString tree_name = (selection.Contains("_")) ? "emu" : selection;
  if(!chain) {
    printf("Chain in %s/%s not found, continuing\n",card.filepath_.Data(), tree_name.Data());
    return 1;
  }

  // if Drell-Yan, loop through it twice, doing tautau then ee/mumu decays
  const int ndyloops = (isDY && splitDY_) ? 2 : 1;

  // if W+Jets and splitting, do N(jets) = 0 - 4 loops, else just do loop -1 = no splitting
  const int nwloops = (isWJ && splitWJets_) ? 5 : 0;

  cout << "Processing " << card.fname_.Data() << " with eventCategory = " << card.category_ << " isSignal = " << isSignal << " isDY = " << isDY
       << " isWJets = " << isWJ << " isData = " << card.isData_ << " useSystematics = " << doSystematics_ << " doMVASets = " << DoMVASets_
       << " removeTrigWeights = " << removeTrigWeights_ << endl;

  const Long64_t nentries = chain->GetEntries();
  float events_scale = 1.f;
  if(card.isData_ == 0 && max_sim_events_ > 0 && nentries > max_sim_events_) {
    cout << "!!! Reducing sim events from " << nentries << " to " << max_sim_events_ << "!\n";
    events_scale = (max_sim_events_*1.) / nentries;
  }

  for(int wjloop = -1; wjloop < nwloops; ++wjloop) { //start from -1 to also do unsplit histogram
    for(int dyloop = 1; dyloop <= ndyloops; ++dyloop) {
      auto selec = new HISTOGRAMMER(systematicSeed_); //selector
      selec->fSelection = selection;

      if(dynamic_cast<CLFVHistMaker*> (selec)) {
        auto clfv_selec = (CLFVHistMaker*) selec;
        clfv_selec->fDYFakeTauTesting  = DYFakeTau_;
        clfv_selec->fWJFakeTauTesting  = WJFakeTau_;
        clfv_selec->fTTFakeTauTesting  = TTFakeTau_;
        clfv_selec->fQCDFakeTauTesting = QCDFakeTau_;
        clfv_selec->fJetTauTesting     = JetTauTesting_;
        clfv_selec->fFakeLeptonTesting = FakeLeptonTesting_;
        clfv_selec->fCutFlowTesting    = CutFlowTesting_;
        clfv_selec->fTriggerTesting    = TriggerTesting_;
        clfv_selec->fDoMVASets = DoMVASets_ > 0 && (DoMVASets_ > 2 || (DoMVASets_ == 2 && !(selection.Contains("tau"))) || (selection == "emu"));
      }
      if(dynamic_cast<HistMaker*> (selec)) {
        auto hist_selec = (HistMaker*) selec;
        hist_selec->fPrintTime          = 2; //Print detailed summary of processing times
        hist_selec->fPrintFilling       = 1; //print detailed histogram set filling
        hist_selec->fDoTriggerMatching  = doTriggerMatching_;
        hist_selec->fReprocessMVAs      = ReprocessMVAs_; //reevaluate MVA scores on the fly
        hist_selec->fProcessSSSF        = doSSSF_;
        hist_selec->fDoEventList        = selection == "ee" || selection == "mumu";
        hist_selec->fDoHiggs            = doHiggs_;
        hist_selec->fSparseHists        = sparseHists_;
        hist_selec->fUseSignalZWeights  = useSignalZWeights_;
        hist_selec->fUseEmbedRocco      = useEmbedRocco_;
        hist_selec->fUseRoccoCorr       = useRoccoCorr_;
        hist_selec->fUseRoccoSize       = useRoccoSize_;
        hist_selec->fUseCDFBDTs         = useCDFBDTs_;
        hist_selec->fNotifyCount        = notify_;
        hist_selec->fLoadBaskets        = false;
        hist_selec->fMinLepM            = (selection == "emu") ?  65.f : 35.f;
        hist_selec->fMaxLepM            = (selection == "emu") ? 115.f : (selection.EndsWith("tau")) ? 175.f : 175.f;
        hist_selec->fDoSSSystematics    = selection == "emu" || selection.Contains("_");
        hist_selec->fDoLooseSystematics = selection.EndsWith("tau");
      }

      selec->fRemoveTriggerWeights = removeTrigWeights_;
      selec->fUpdateMCEra          = updateMCEra_;
      selec->fUseBTagWeights       = useBTagWeights_;
      selec->fRemovePUWeights      = (isSignal) ? removePUWeights_ : 0 ; //remove for signal, due to module issues (likely weights measured per file too low stats)
      selec->fUseJetPUIDWeights    = useJetPUIDWeights_;
      selec->fUsePrefireWeights    = usePrefireWeights_;
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
      selec->fIsLLEmbed = card.fname_.Contains("Embed-MuMu-") || card.fname_.Contains("Embed-EE-");
      selec->fYear = card.year_;
      selec->fDataset = card.dataset_;
      selec->fIsSignal = isSignal;

      selec->fDoSystematics = config.doSystematics_;
      //skip electron data events with both triggers for e+mu channel, to not double count
      selec->fSkipDoubleTrigger = isElectronData && (tree_name == "emu");
      //store a label for this dataset
      selec->fEventCategory = card.category_;
      selec->fWriteTrees = selection != "mumu" && selection != "ee" && config.writeTrees_; //don't write trees for data
      if(card.isData_ == 0) {
        selec->fXsec = card.xsec_;
        selec->fXsec /= (selec->fIsEmbed) ? 1. : (card.events_->GetBinContent(1) - 2.*card.events_->GetBinContent(10));; //for writing trees with correct normalization
        selec->fLum = (selec->fIsEmbed) ? 1. : card.lum_; //for adding lum to tree weights
      } else {
        CrossSections xs(useUL_);
        selec->fXsec = xs.GetCrossSection(Form("QCD_%s", tree_name.Data()));
        selec->fLum = 1.; //no luminosity needed for data
      }
      selec->fFractionMVA = (isSignal) ? config.signalTrainFraction_ : config.backgroundTrainFraction_; //fraction of events to use for MVA training
      if(selection == "mumu" || selection == "ee") selec->fFractionMVA = 0.; //don't split off same flavor data
      selec->fIsNano = true;
      if(debug_ && nEvents_ < 20) selec->fVerbose = 1;
      else                        selec->fVerbose = 0;
      selector_ = selec;
      //FIXME: This isn't being called by default for some reason, only for HistMaker type objects
      if(dynamic_cast<HistMaker*> (selec)) {
        selec->Init(chain);
      }
      if(!debug_) { //run the selector over the chain
        if(card.isData_ == 0 && events_scale < 1.f) chain->Process(selec,"", max_sim_events_); //only process a certain amount
        else                                        chain->Process(selec, ""); //process the whole chain
      } else                                        chain->Process(selec,"", nEvents_, startEvent_); //process specific event range for debugging

      //open back up the file
      TString outname = selec->GetOutputName();
      printf("Re-opening output file %s to append the events histogram\n", outname.Data());
      TFile* out = TFile::Open(outname.Data(),"UPDATE");
      if(!out) {
        printf("!!! Unable to find output hist file %s, continuing\n", outname.Data());
        continue;
      }
      //add the events histogram to the output
      auto events = (TH1*) card.events_->Clone(card.events_->GetName());
      if(events_scale < 1.f) {
        events->SetBinContent( 1, events_scale*events->GetBinContent( 1));
        events->SetBinContent(10, events_scale*events->GetBinContent(10));
      }
      events->Write();
      out->Write();
      out->Close();
      delete out;
      if(!debug_) delete selec;
    } //end DY loop
  } //end W+Jets loop
  return 0;
}

//run the histogrammer over a data file
Int_t process_single_card(datacard_t& card, config_t& config, vector<TString> file_names) {
  if(file_names.size() == 0) {
    cout << "Card " << card.fname_.Data() << " has no files defined!\n";
    return 10;
  }

  CrossSections xs(useUL_); //cross check gen number and negative fraction
  TString name = card.fname_;
  name.ReplaceAll("*", "");
  name.ReplaceAll("LFVAnalysis_", "");
  name.ReplaceAll(".root", "");
  name.ReplaceAll("_2016", "");
  name.ReplaceAll("_2017", "");
  name.ReplaceAll("_2018", "");
  name.ReplaceAll("_", "");

  //Get the normalization event count
  TH1* events = events_from_chain(file_names);
  if(!events) {
    printf("Events Histogram for %s not found, continuing\n",card.fname_.Data());
    return 1;
  }

  //Get the normalization information from TChains
  TChain* runs = new TChain("Runs");
  TChain* norm = new TChain("Norm");
  int counter(0), nfiles(file_names.size());
  for(auto file_name : file_names) {
    std::cout << "Normalizing chain progress:  " << Form("%3i/%3i", counter, nfiles) << "\r";
    std::cout.flush();

    runs->AddFile(file_name.Data());
    norm->AddFile(file_name.Data(), -1);
    ++counter;
  }
  std::cout << "Normalizing chain progress:  " << Form("%3i/%3i", counter, nfiles) << "\n";

  {
    TChain* chain_norm = runs;
    Long64_t runs_count = 0;
    Long64_t runs_count_loop = 0;
    Long64_t runs_neg_count = 0;
    Long64_t runs_neg_count_loop = 0;
    bool replace = true;
    cout << "Processing normalization trees...\n";
    if(norm->GetEntries() > 0) { //check for an added normalization tree
      chain_norm = norm;
      norm->SetBranchAddress("NEvents"  , &runs_count_loop);
      norm->SetBranchAddress("NNegative", &runs_neg_count_loop);
    // if(runs->GetBranch("NEvents")) {
    //   runs->SetBranchAddress("NEvents"  , &runs_count_loop);
    //   runs->SetBranchAddress("NNegative", &runs_neg_count_loop);
    } else if(runs->GetBranch("genEventCount")) {
      runs->SetBranchAddress("genEventCount", &runs_count_loop);
    } else if(runs->GetBranch("genEventCount_")) {
      runs->SetBranchAddress("genEventCount_", &runs_count_loop);
    } else {
      cout << "No gen-event count found in the Runs tree!\n";
      replace = false;
    }
    if(replace) {
      for(int ientry = 0; ientry < chain_norm->GetEntries(); ++ientry) {
        chain_norm->GetEntry(ientry);
        runs_count     += runs_count_loop;
        runs_neg_count += runs_neg_count_loop;
      }
      const int events_count = events->GetBinContent(1);
      double neg_frac = (runs_neg_count > 0) ? (runs_neg_count*1.)/(runs_count) : xs.GetNegativeFraction(name, card.year_);
      if(neg_frac < 1.e-6) neg_frac = events->GetBinContent(10)/events_count;
      cout << "Runs tree N(events): " << runs_count << ", event hist: " << events_count
           << " (neg frac = " << neg_frac << ") --> Difference = " << runs_count - events_count << endl;
      events->SetBinContent(1 , runs_count);
      events->SetBinContent(10, runs_count*neg_frac);
    }
  }

  //for avoiding double counting data events in muon+electron data files
  const bool isElectronData = card.isData_ == 1;
  const bool isMuonData     = card.isData_ == 2;

  card.events_ = events;
  if(xs.GetGenNumber(name, card.year_) > 0) {
    const int ngen = xs.GetGenNumber(name, card.year_);
    const int nevt = events->GetBinContent(1);
    cout << "*** Data set generated with " << ngen << " events and skimming shows " << nevt << " events ***\n";
  }

  //Set the dataset name
  name.ReplaceAll("Run2016", "-"); // shorten data sample names
  name.ReplaceAll("Run2017", "-");
  name.ReplaceAll("Run2018", "-");
  name.ReplaceAll("-v1", ""); //replace signal versioning in output
  name.ReplaceAll("-v2", "");
  name.ReplaceAll("-v3", "");
  card.dataset_ = name;

  //Loop throught the file looking for all selection channels
  vector<TString> selections = config.selections_;
  for(TString selection : selections) {
    //skip if data on a channel without that trigger used
    if(isElectronData && (selection == "mutau" || selection == "mumu")) {
      cout << "Electron data on muon only channel, continuing!\n"; continue;
    }
    if(isMuonData && (selection == "etau" || selection == "ee")) {
      cout << "Muon data on electron only channel, continuing!\n"; continue;
    }
    if(name.Contains("Embed-MuMu-") && selection != "mumu") {
      cout << "MuMu embedding is only relevant to the mumu channel, skipping " << selection.Data() << endl;
      continue;
    }
    if(name.Contains("Embed-EE-") && selection != "ee") {
      cout << "EE embedding is only relevant to the ee channel, skipping!\n";
      continue;
    }
    ///////////////////////////////
    //Process this selection

    TString tree_name = (selection.Contains("_")) ? "emu" : selection;
    //Retreive the correct ntuple
    TChain* chain = new TChain(tree_name.Data());
    counter = 0;
    for(TString file_name : file_names) {
      std::cout << "Selection chain progress:  " << Form("%3i/%3i", counter, nfiles) << "\r";
      std::cout.flush();
      chain->AddFile(file_name.Data(), -1);
      ++counter;
    }
    std::cout << "Selection chain progress:  " << Form("%3i/%3i", counter, nfiles) << "\n";

    process_channel(card, config, selection, chain);
    delete chain;
  }
  delete runs;
  delete norm;

  return 0;
}

Int_t process_card(TString treepath, TString filename, double xsec, int isdata, bool combine, int category) {

  datacard_t card(true, xsec, filename, isdata, combine);
  config_t config(get_config());
  CrossSections xs(useUL_);

  cout << "---  Write Trees mode: " << config.writeTrees_
       << ", training fractions: signal = " << config.signalTrainFraction_
       << ", background = " << config.backgroundTrainFraction_
       << ", doSystematics = " << config.doSystematics_
       << endl;
  cout << "--- Processing channels: ";
  for(TString channel : config.selections_)
    cout << channel.Data() << ", ";
  cout << endl;

  int year = 2016;
  if(card.fname_.Contains("_2017"))      year = 2017;
  else if(card.fname_.Contains("_2018")) year = 2018;
  card.year_ = year;
  //if combining samples, weight each by the number of generated over total generated
  if(card.combine_) {
    TString name = card.fname_;
    name.ReplaceAll("*", "");
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
  vector<TString> file_names;
  if(useTChain_ && !(card.fname_.Contains(".root") && !card.fname_.Contains("*"))) {
    TString redir = (treepath.Contains("cern.ch")) ? "root://eoscms.cern.ch/" : "root://cmseos.fnal.gov/";
    TString base_loc = treepath;
    base_loc.ReplaceAll(redir.Data(), "");
    TString txt_name = "input_" + card.fname_; txt_name.ReplaceAll("/", "_"); txt_name += ".txt";
    TString ls_command = Form("eos %s ls %s%s >| %s", redir.Data(), base_loc.Data(), card.fname_.Data(), txt_name.Data());
    cout << "ls command: " << ls_command.Data() << endl;
    gSystem->Exec(ls_command.Data());
    std::ifstream input_list(txt_name.Data());
    if(!input_list.is_open()) {
      cout << "Couldn't retrieve file list for " << card.fname_.Data() << endl;
      return 10;
    }
    std::string line;
    while(std::getline(input_list, line)) {
      file_names.push_back(Form("%s%s/%s", treepath.Data(), card.fname_.Data(), line.c_str()));
      cout << "File: " << file_names.back().Data() << endl;
    }
    input_list.close();
    if(!debug_)
      gSystem->Exec(Form("rm %s", txt_name.Data()));
  } else {
    file_names.push_back(treepath + card.fname_);
  }
  const int status = process_single_card(card, config, file_names);
  return status;
}
