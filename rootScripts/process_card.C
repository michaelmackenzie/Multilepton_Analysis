//information for debugging
using namespace CLFV;
#include "histogramming_config.C"

//check if running on the grid
const bool is_grid() {
  const TString user = gSystem->Getenv("USER");
  const bool grid = user == ""; //FIXME: Check grid user name on lxplus condor
  return grid;
}

//make a file list for the given dataset
bool make_file_list(TString dataset, TString path, TString txt_name) {
    TString redir = (path.Contains("cern.ch")) ? "root://eoscms.cern.ch/" : "root://cmseos.fnal.gov/";
    TString base_loc = path;
    base_loc.ReplaceAll(redir.Data(), "");
    TString ls_command;
    if(is_grid()) {
      if(!dataset.Contains("*")) {
        ls_command = Form("xrdfs %s ls %s | grep %s | awk '{print \"%s\"$0}' >| %s", redir.Data(), base_loc.Data(), dataset.Data(), redir.Data(), txt_name.Data());
      } else { //handle the wildcards
        dataset.ReplaceAll("*", "\"*\"");
        ls_command = Form("for file in `xrdfs %s ls %s`; do if [[ \"${file}\" == *\"%s\" ]]; then echo %s${file}; fi; done >| %s", redir.Data(), base_loc.Data(), dataset.Data(), redir.Data(), txt_name.Data());
      }
    } else {
      ls_command = Form("eos %s ls %s%s | awk '{print \"%s\"$0 }' >| %s", redir.Data(), base_loc.Data(), dataset.Data(), path.Data(), txt_name.Data());
    }
    cout << "ls command: " << ls_command.Data() << endl;
    gSystem->Exec(ls_command.Data());
    return true;
}

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

  int doSystematics = doSystematics_;

  // configure qcd histmaker processing
  const bool is_qcd_histmaker = TString(HISTOGRAMMER::Class()->GetName()) == "CLFV::QCDHistMaker";
  if(is_qcd_histmaker) {
    max_sim_events_ = card.fname_.Contains("ttbarlnu") ? 2e6 : -1; //use all sim events, except for ttbar MC
    doEmuDefaults_  = 0;
    useXGBoost_     = 0;
    useBDTScale_    = 0;
    test_mva_       = "qcd_histmaker"; //turn on BDTs needed for closure checks
  }

  // configure fields for emu defaults, assuming tau ones are set
  if(doEmuDefaults_ && (selection == "emu" || ((selection == "ee" || selection == "mumu") && doSameFlavorEMu_))) {
    doSystematics = (isSignal) ? doSystematics_ : (doSystematics_) ? -2 : 0; //-2 only fills the base histogram for non-signal systematics
    // doSystematics_ = 0;
    allowMigration_ = isSignal && doSystematics_;
    useEventFlags_      = 1; //filter using event flags
    useBTagWeights_     = 2; //2: ntuple-level
    removePUWeights_    = 0; //0: ntuple-level
    useSignalZWeights_  = 2; //2: ntuple-level
    useLeptonIDWeights_ = 2; //2: ntuple-level
    useRoccoCorr_       = 2; //2: ntuple-level
    useZPtWeights_      = 2; //2: ntuple-level
    // useJetPUIDWeights_ = 0;
    // removeTrigWeights_ = 1;
    // usePrefireWeights_ = 0;
    // useLepDxyzWeights_ = 0; //use dxy/dz displacement weights
    // useSignalZMixWeights_ = 0; //FIXME: Remove after done with comparisons
    // updateMET_ = 0; //FIXME: Remove after done with comparisons
    useCDFBDTs_ = 0;
    useXGBoost_ = 1;
    train_mode_ = 0;

    min_lepm_         =  65.f;
    max_lepm_         = 115.f;
    migration_buffer_ =   5.f;
    // min_lepm_         =  85.f;
    // max_lepm_         =  95.f;
    // migration_buffer_ =   0.f;
    cout << "Overridding processing defaults for emu-style selection\n";
  }

  if(doEmbedSameFlavor_ && (selection == "ee" || selection == "mumu")) {
    doSystematics       = 0;
    allowMigration_     = 0;
    useEmbedCuts_       = 3;
    useEventFlags_      = 1; //filter using event flags
    useLepDxyzWeights_  = 0;
    useRoccoCorr_       = 1;
    useRoccoSize_       = 1;
    embedUseMETUnc_     = 2; //add uncertainty on embedding MET
    useEmbedBDTUnc_     = 0;

    useBTagWeights_     = 2; //2: ntuple-level
    removePUWeights_    = 0; //0: ntuple-level
    useSignalZWeights_  = 2; //2: ntuple-level
    useLeptonIDWeights_ = 1; //2: ntuple-level
    useRoccoCorr_       = 2; //2: ntuple-level
    useZPtWeights_      = 2; //2: ntuple-level

    //use a subset of embedded events for faster processing
    max_sim_events_     = 1e6;

    //ignore BDTs
    DoMVASets_     = 0;
    ReprocessMVAs_ = 0;
    test_mva_      = ""; //(selection == "mumu") ? "zmutau" : "zetau";
    useCDFBDTs_    = 0;
    useXGBoost_    = 0;
    train_mode_    = 0;
    writeTrees_    = 0;
    sparseHists_   = false;

    min_lepm_ =  35.f;
    max_lepm_ = 175.f;
    cout << "Overridding processing defaults for same flavor embedding selection\n";
  }

  // if Drell-Yan, loop through it twice, doing tautau then ee/mumu decays
  const int ndyloops = (isDY && splitDY_) ? 2 : 1;

  // if W+Jets and splitting, do N(jets) = 0 - 4 loops, else just do loop -1 = no splitting
  const int nwloops = (isWJ && splitWJets_) ? 5 : 0;

  cout << "Processing " << card.fname_.Data() << " with eventCategory = " << card.category_ << " isSignal = " << isSignal << " isDY = " << isDY
       << " isWJets = " << isWJ << " isData = " << card.isData_ << " useSystematics = " << doSystematics << " doMVASets = " << DoMVASets_
       << " removeTrigWeights = " << removeTrigWeights_ << endl;

  const Long64_t nentries = chain->GetEntries();
  float events_scale = 1.f;
  Long64_t max_events = nentries;
  if(card.isData_ == 0 && max_sim_events_ > 0 && nentries > max_sim_events_) {
    cout << "!!! Reducing sim events from " << nentries << " to " << max_sim_events_ << "!\n";
    events_scale = (max_sim_events_*1.) / nentries;
    max_events = max_sim_events_;
  }
  if(card.isData_ && max_data_events_ > 0. && max_data_events_ < 1.) {
    cout << "!!! REDUCING DATA EVENTS BY " << 100.*(1.-max_data_events_) << " PERCENT\n";
    events_scale = max_data_events_;
    max_events = max_data_events_*nentries + 0.99;
  }

  for(int wjloop = -1; wjloop < nwloops; ++wjloop) { //start from -1 to also do unsplit histogram
    for(int dyloop = 1; dyloop <= ndyloops; ++dyloop) {
      if(isDY && dyloop == 2 && doEmbedSameFlavor_ == 1 && (selection == "ee" || selection == "mumu")) continue; //skip Z->ll if using embedding ll
      auto selec = new HISTOGRAMMER(systematicSeed_); //selector
      selec->fSelection = selection;

      //don't put limit on Z->tautau in Z->ll channel (event lists aren't used with max sim set)
      const Long64_t prev_max_sim = max_sim_events_;
      const float    prev_evt_scale = events_scale;
      if(dyloop == 1 && isDY && (selection == "ee" || selection == "mumu")) {
        max_sim_events_ = -1;
        events_scale = 1.f;
      }

      //configure fields for j-->tau measurement histogramming
      if(dynamic_cast<JTTHistMaker*> (selec)) {
        doSystematics_  =  0; //ignore systematics
        train_mode_     =  0; //ignore MVA training weights
        // ReprocessMVAs_  =  0; //ignore MVA scores
        max_sim_events_ = -1; //use all sim events
      }

      //use a subset of data events as well if producing embedding BDT scale factors
      if(dynamic_cast<EmbedBDTHistMaker*> (selec)) {
        max_data_events_    = 0.20; //use 20% of the data
      }

      //configure fields for OS-->SS measurement histogramming
      if(dynamic_cast<QCDHistMaker*> (selec)) {
        doSystematics_  =  0; //ignore systematics
        train_mode_     =  0; //ignore MVA training weights
        // ReprocessMVAs_  =  0; //ignore MVA scores
      }

      //for sparse histogramming
      if(dynamic_cast<SparseHistMaker*> (selec)) {
        auto sparse_selec = (SparseHistMaker*) selec;
        sparse_selec->fDoCutFlowSets = true; //histogram the cut flow
      }

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
        clfv_selec->fMinLepM           = min_lepm_; //(selection == "emu") ?  65.f : 35.f;
        clfv_selec->fMaxLepM           = max_lepm_; //(selection == "emu") ? 115.f : (selection.EndsWith("tau")) ? 175.f : 175.f;
        clfv_selec->fSameFlavorEMuSelec = doSameFlavorEMu_;
      }
      if(dynamic_cast<HistMaker*> (selec)) {
        auto hist_selec = (HistMaker*) selec;
        hist_selec->fPrintTime          = 2; //Print detailed summary of processing times
        hist_selec->fPrintFilling       = 1; //print detailed histogram set filling
        hist_selec->fDoTriggerMatching  = doTriggerMatching_;
        hist_selec->fUseEMuTrigger      = useEMuTrigger_;
        if(selection.Contains("e")) //only relevant for channels with leptons
          hist_selec->fDoEleIDStudy     = doEleIDStudy_;
        hist_selec->fReprocessMVAs      = ReprocessMVAs_; //reevaluate MVA scores on the fly
        hist_selec->fTestMVA            = test_mva_;
        hist_selec->fProcessSSSF        = doSSSF_;
        hist_selec->fDoEventList        = (selection == "ee" || selection == "mumu") && (isDY || max_sim_events_ < 0); //don't use lists if reducing simulation size
        hist_selec->fDoHiggs            = doHiggs_;
        hist_selec->fSparseHists        = sparseHists_;
        hist_selec->fUseFlags           = useEventFlags_;
        hist_selec->fUseZPtWeight       = useZPtWeights_;
        hist_selec->fUseSignalZWeights  = useSignalZWeights_;
        hist_selec->fUseSignalZMixWeights  = useSignalZMixWeights_;
        hist_selec->fUseLepDisplacementWeights = useLepDxyzWeights_;
        hist_selec->fRemoveEventWeights = removeEventWeights_;
        hist_selec->fUseEmbedRocco      = useEmbedRocco_;
        hist_selec->fEmbedUseMETUnc     = embedUseMETUnc_;
        hist_selec->fUseEmbBDTUncertainty = useEmbedBDTUnc_;
        hist_selec->fUseRoccoCorr       = useRoccoCorr_;
        hist_selec->fUseRoccoSize       = useRoccoSize_;
        hist_selec->fUseCDFBDTs         = useCDFBDTs_;
        hist_selec->fUseXGBoostBDT      = useXGBoost_;
        hist_selec->fUseBDTScale        = useBDTScale_;
        hist_selec->fNotifyCount        = notify_;
        hist_selec->fLoadBaskets        = false;
        hist_selec->fDoSSSystematics    = selection == "emu" || selection.Contains("_");
        hist_selec->fDoLooseSystematics = selection.EndsWith("tau");
        hist_selec->fAllowMigration     = allowMigration_ && doSystematics;
        hist_selec->fMigrationBuffer    = migration_buffer_;
        hist_selec->fMaxEntries         = max_events;
        hist_selec->fUseLooseTauBuffer  = useLooseTauBuffer_;

        if(etauAntiEleCut_ > 0) hist_selec->fETauAntiEleCut = etauAntiEleCut_;
      }

      selec->fRemoveTriggerWeights = removeTrigWeights_;
      selec->fUpdateMCEra          = updateMCEra_;
      selec->fUpdateMET            = updateMET_;
      selec->fUseBTagWeights       = useBTagWeights_;
      selec->fRemovePUWeights      = (isSignal) ? removePUWeights_ : 0 ; //remove for signal, due to module issues (likely weights measured per file too low stats)
      selec->fUseJetPUIDWeights    = useJetPUIDWeights_;
      selec->fUsePrefireWeights    = usePrefireWeights_;
      selec->fApplyLeptonIDWt      = useLeptonIDWeights_;
      selec->fUseQCDWeights        = useQCDWeights_;

      if(selection == "emu" && useEmbedCuts_ == 1)
        selec->fUseEmbedCuts         = 0;
      else
        selec->fUseEmbedCuts         = useEmbedCuts_;
      selec->fEmbeddedTesting      = embeddedTesting_;

      selec->fUseMCEstimatedFakeLep  = useMCFakeLep_;
      selec->fUseJetToTauComposition = useJetToTauComp_;
      selec->fApplyJetToTauMCBias    = applyJetToTauMCBias_;
      if(wJetsBiasMode_ >= 0)
        selec->fWJetsMCBiasMode      = wJetsBiasMode_;

      selec->fDYType = 0;
      if(isDY && splitDY_)     selec->fDYType = dyloop; //if Drell-Yan, tell the selector which loop we're on
      if(isWJ && splitWJets_)  selec->fWNJets = wjloop; //if W+Jets, tell the selector which loop we're on
      selec->fIsDY = isDY;
      selec->fIsData = card.isData_;
      selec->fIsEmbed = card.fname_.Contains("Embed-");
      selec->fIsLLEmbed = card.fname_.Contains("Embed-MuMu-") || card.fname_.Contains("Embed-EE-");
      selec->fYear = card.year_;
      selec->fDataset = card.dataset_;
      selec->fIsSignal = isSignal;

      selec->fDoSystematics = doSystematics;
      //skip electron data events with both triggers for e+mu channel, to not double count
      selec->fSkipDoubleTrigger = isElectronData && (tree_name == "emu");
      //store a label for this dataset
      selec->fEventCategory = card.category_;
      selec->fWriteTrees = selection != "mumu" && selection != "ee" && config.writeTrees_; //don't write trees for same flavor events
      if(card.isData_ == 0) {
        selec->fXsec = card.xsec_;
        selec->fXsec /= (selec->fIsEmbed) ? 1. : (card.events_->GetBinContent(1) - 2.*card.events_->GetBinContent(10));; //for writing trees with correct normalization
        selec->fLum = (selec->fIsEmbed) ? 1. : card.lum_; //for adding lum to tree weights
      } else {
        CrossSections xs(useUL_);
        selec->fXsec = xs.GetCrossSection(Form("QCD_%s", tree_name.Data())); //rough P(signal region) / P(background region) scale
        selec->fLum = 1.; //no luminosity needed for data
      }
      if(train_mode_ == 1) { //fixed training fraction for signal and background
        selec->fFractionMVA = (isSignal) ? config.signalTrainFraction_ : config.backgroundTrainFraction_; //fraction of events to use for MVA training
      } else if(train_mode_ == 2) { //variable training fractions
        if(selection == "ee" || selection == "mumu") selec->fFractionMVA = 0.f; //don't train in ee/mumu
        else if(card.isData_) { //data from data-driven backgrounds
          selec->fFractionMVA = (selection.EndsWith("tau")) ? 0.05f : 0.3f; //don't need much j-->tau data
        } else if(isSignal) selec->fFractionMVA = 0.5f; //use half the signal data for training
        else if(selec->fIsEmbed) selec->fFractionMVA = (selection.EndsWith("tau")) ? 0.1f : 0.1f; //don't need much embedding data
        else if(card.fname_.Contains("ttbarlnu")) selec->fFractionMVA = 0.1f; //reduce ttbar->ll contributions
        else selec->fFractionMVA = 0.3f; //default MC training fraction
      } else { //no training
        selec->fFractionMVA = 0.f;
      }
      if(selection == "mumu" || selection == "ee") selec->fFractionMVA = 0.; //don't split off same flavor data
      selec->fIsNano = true;
      if(follow_hist_set_ >= 0 && debug_) selec->fFollowHistSet = follow_hist_set_;
      if(debug_ && verbose_ > 1)       selec->fVerbose = verbose_;
      else if(debug_ && nEvents_ < 20) selec->fVerbose = 1;
      else                             selec->fVerbose = 0;
      selector_ = selec;
      //FIXME: This isn't being called by default for some reason, only for HistMaker type objects
      if(dynamic_cast<HistMaker*> (selec)) {
        selec->Init(chain);
      }
      if(!debug_) { //run the selector over the chain
        if(nentries > max_events)                   chain->Process(selec,"",max_events); //only process given amount
        else                                        chain->Process(selec, ""); //process the whole chain
      } else                                        chain->Process(selec,"", nEvents_, startEvent_); //process specific event range for debugging

      //get the events histogram, make a modified clone if using a subset of the dataset
      auto events = card.events_;
      if(events_scale < 1.f) {
        card.events_->SetName("tmp_events");
        events = (TH1*) card.events_->Clone("events");
        events->SetBinContent( 1, events_scale*events->GetBinContent( 1));
        events->SetBinContent(10, events_scale*events->GetBinContent(10));
      }

      //restore max sim flag, if changed
      max_sim_events_ = prev_max_sim;
      events_scale    = prev_evt_scale;

      //open back up the file
      TString outname = selec->GetOutputName();
      printf("Re-opening output file %s to append the events histogram\n", outname.Data());
      TFile* out = TFile::Open(outname.Data(),"UPDATE");
      if(!out) {
        printf("!!! Unable to find output hist file %s, continuing\n", outname.Data());
        //restore the events histogram name if used a clone
        if(TString(card.events_->GetName()) != "events") {
          delete events;
          card.events_->SetName("events");
        }
        continue;
      } else if(debug_) {
        printf("--> Successfully opened the output file\n");
      }
      //add the events histogram to the output
      events->Write();
      out->Close();
      delete out;

      //restore the events histogram name if used a clone
      if(TString(card.events_->GetName()) != "events") {
        delete events;
        card.events_->SetName("events");
      }
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
  const bool isMuonEGData   = card.isData_ == 3;

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
  name.ReplaceAll("-v3a", "");
  name.ReplaceAll("-v3b", "");
  name.ReplaceAll("-v4", "");
  name.ReplaceAll("-v*", "");
  name.ReplaceAll("-v" , "");
  // name.ReplaceAll("-old", ""); //old/new ntuple name cleaning
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
    if(isMuonEGData && (selection != "emu" && !selection.Contains("_"))) {
      cout << "MuonEG data not on e-mu data channel, continuing!\n"; continue;
    }
    if(isMuonEGData && useEMuTrigger_ == 0) {
      cout << "MuonEG data but not using e-mu trigger, continuing!\n"; continue;
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

Int_t process_card(TString treepath, TString filename, double xsec, int isdata, bool combine, int category, TString selection = "") {

  datacard_t card(true, xsec, filename, isdata, combine);
  config_t config(get_config());
  CrossSections xs(useUL_);
  //override the default selections to process if a specific one is given
  if(selection != "") config.selections_ = {selection};

  cout << "---  Write Trees mode: " << config.writeTrees_
       << ", training fractions: signal = " << config.signalTrainFraction_
       << ", background = " << config.backgroundTrainFraction_
       << ", doSystematics = " << doSystematics_
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
    if(!make_file_list(card.fname_, treepath, txt_name)) return 10; //create the list of files to process
    std::ifstream input_list(txt_name.Data());
    if(!input_list.is_open()) {
      cout << "Couldn't retrieve file list for " << card.fname_.Data() << endl;
      return 10;
    }
    std::string line;
    while(std::getline(input_list, line)) {
      TString file_path = line.c_str();
      file_names.push_back(file_path);
      cout << "File: " << file_path.Data() << endl;
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
