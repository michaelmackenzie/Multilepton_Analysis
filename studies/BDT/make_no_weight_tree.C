//Script to produce a boot-strapped tree without event weights
int verbose_ = 1;

int make_no_weight_tree(const char* file =  "trees/background_clfv_Z0_nano_mutau_2016_2017_2018_8.tree",
                        int nmax = 1e4, //number of entries used in (background) training
                        const int seed = 90) {


  TStopwatch* timer = new TStopwatch();
  timer->Start();

  TString fout_name = file;
  fout_name.ReplaceAll(".tree", "_noweight.tree");

  TFile* fout = new TFile(fout_name.Data(), "RECREATE");
  printf("Initializing output file %s\n", fout->GetName());

  TFile* f = TFile::Open(file,"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  TTree* tree  = (TTree*) f->Get("background_tree");
  if(!tree) {
    printf("Input tree not found\n");
    return 2;
  }

  //Get the distribution of weighted types
  tree->Draw("type>>htype","fulleventweightlum");
  TH1* htype = (TH1*) gDirectory->Get("htype");
  if(!htype) {
    cout << "Event weight or event type formula failed!\n";
    return 3;
  }
  const int    ntypes      = 10; //htype->GetNbinsX();
  const double events_init = htype->Integral();
  const int    nentries    = tree->GetEntries();
  printf("Initial tree with %i entries represents %.2f events\n",
         nentries, events_init);

  //track how many of each type are sampled
  const double zll_scale = (TString(file).Contains("_etau_201")) ? 10. : 5.; //FIXME: Set Z->ll importance weighting based on selection
  const int zll_bin = htype->FindBin(2.);
  htype->SetBinContent(zll_bin, zll_scale*htype->GetBinContent(zll_bin));
  htype->SetBinContent(htype->FindBin(0.), 0.); //ignore signal in the sampling
  htype->Scale(1./htype->Integral()); //set each bin to be the fraction of events

  //Random number generator for sampling
  TRandom3 rnd(seed);

  //number of events passed
  int evts = 0;

  fout->cd();

  //load the tree into memory
  auto cachesize = 500000000U;
  tree->SetCacheSize(cachesize);
  tree->AddBranchToCache("*",true);
  tree->LoadBaskets();

  std::vector<TTree*> out_trees; //tree for each background type
  TTree* trees[ntypes];
  TList* list = new TList;
  for(int itype = 0; itype < ntypes; ++itype) {
    trees[itype] = nullptr;
    //get N(type) needed, given N(max)
    const float fraction = htype->GetBinContent(htype->FindBin(itype));
    if(fraction <= 0.f) continue; //no events for this type
    const int nevents = std::max(1.f, nmax*fraction); //force at least 1 event
    cout << "--- Making a tree for type " << itype << " with " << nevents << " events\n";
    TTree* tree_out = Utilities::BootStrap(tree, nevents, rnd, "fulleventweightlum", Form("train > 0 && issignal < 0.5 && type == %i", itype), verbose_);
    if(!tree_out) {
      cout << "Failed to bootstrap a tree for type " << itype << endl;
      continue;
    }
    tree_out->SetName(Form("%s_noweight_%i",tree->GetName(), itype));
    out_trees.push_back(tree_out);
    trees[itype] = tree_out;
    list->Add(tree_out);
  }

  cout << "Merging the background trees\n";
  for(int itype = 0; itype < ntypes; ++itype) {
    if(!trees[itype]) continue;
    trees[itype]->SetBranchStatus("fulleventweightlum", 0);
    if(verbose_ > 0) cout << "Tree " << itype << ": " << trees[itype]
                          << " Entries: " << trees[itype]->GetEntriesFast() << endl;
  }
  TTree* tree_out = TTree::MergeTrees(list); //merge the output trees
  tree_out->SetName("background");
  for(int itype = 0; itype < ntypes; ++itype) {
    if(!trees[itype]) continue;
    delete trees[itype];
  }

  /////////////////////////////////////////////////////////////
  // Create the signal tree

  const int nsignal = std::min(nmax, 10000); //FIXME: Decide N(signal) events to use
  cout << "--- Making the signal tree with " << nsignal << " entries\n";
  TTree* signal_tree = Utilities::BootStrap(tree, nsignal, rnd, "fulleventweightlum", "train > 0 && issignal > 0.5", verbose_);
  if(!signal_tree) return -20;
  signal_tree->SetName("signal");
  cout << "Created the signal tree with " << signal_tree->GetEntries() << " entries\n";

  printf("Writing %s to disk\n", fout->GetName());
  //write results to disk
  tree_out->Write();
  signal_tree->Write();
  fout->Close();

  const Double_t cpuTime = timer->CpuTime();
  const Double_t realTime = timer->RealTime();
  if(realTime < 100.) {
    printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  } else {
    printf("Processing time: %7.2f min CPU time %7.2f min Wall time\n",cpuTime/60.,realTime/60.);
  }

  return 0;
}
