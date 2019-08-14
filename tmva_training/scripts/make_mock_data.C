int make_mock_data(const char* file =  "../background_signal_2.tree",
		   double lum = 5.3e3, int category_ignore = 16,
		   double lum_init = 5.3e3, int seed = 90) {


  TStopwatch* timer = new TStopwatch();
  timer->Start();

  //build output file's name
  TString fout_name = "background_";
  TString fin_name = file;
  if(fin_name.Contains("signal"))
    fout_name += "signal_";
  if(fin_name.Contains("_2"))
    fout_name += "2_";
  else if(fin_name.Contains("_3"))
    fout_name += "3_";
  else if(fin_name.Contains("_11"))
    fout_name += "11_";
  if(category_ignore >= 0)
    fout_name += Form("noCat_%i_", category_ignore);
  
  fout_name += Form("%.0finvpb_mock_data.tree", lum);

  TFile* fout = new TFile(fout_name.Data(), "RECREATE");
  printf("Initializing output file %s\n", fout->GetName());

  TFile* f = new TFile(file,"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }
  TTree *tree  = (TTree*) f->Get("background_tree");
  if(!tree) {
    printf("Trees not found\n");
    return 2;
  }

  //to ignore some signals
  TString ignore = "";
  if(category_ignore >= 0)
    ignore += Form("*(eventCategory != %i)", category_ignore);
  
  //maximum weight in tree, for getting random weights
  double max_event_weight = max(tree->GetMaximum("fullEventWeight"),abs(tree->GetMinimum("fullEventWeight")));
  int num = tree->GetEntries();
  TRandom* rnd = new TRandom(90);
  
  tree->Draw("fullEventWeight>>hfullEventWeight",Form("fullEventWeight%s",ignore.Data()));
  TH1F* hfullEventWeight = (TH1F*) gDirectory->Get("hfullEventWeight");
  double events_init = hfullEventWeight->Integral();
  printf("Initial tree with %i entries represents %.2f events at %.1f pb^-1\n",
	 num, events_init, lum_init);

  delete hfullEventWeight;
  Float_t evt_wt;
  tree->SetBranchAddress("fullEventWeight", &evt_wt);
  int category = -1;
  if(ignore != "")
    tree->SetBranchAddress("eventCategory", &category);
    
  int evts = 0;

  fout->cd();
  TTree* tree_out = tree->CloneTree(0);
  tree_out->SetName(Form("%s_mock_data",tree->GetName()));
  int attempts = 0;
  int final_events = (int) events_init*(lum/lum_init);

  if(false) { //Poisson variation on total events
    final_events = (int) rnd->Poisson(final_events);
  }
  auto cachesize = 500000000U; //100MB
  tree->SetCacheSize(cachesize);
  tree->AddBranchToCache("*",true);
  tree->LoadBaskets();
  tree_out->SetCacheSize(cachesize);
  tree_out->AddBranchToCache("*",true);

  while(evts < final_events) { //fill number of requested events
    attempts++; //increase attempt amount
    double wt = max_event_weight*rnd->Uniform(); //random weight between 0 and max weight
    int index = (int) (num*rnd->Uniform()); //random index in the tree
    if (index >= num) 
      continue; //avoid going outside of index range, 0 - (num-1)
    tree->GetEntry(index);
    if(ignore != "" && category == category_ignore)
      continue;

    if(abs(evt_wt) > wt) { //if accepted, add to output tree
      tree_out->Fill();
      if(evt_wt > 0.)
	evts++;
      else         //dealing with negative weights
	evts--;
      if(evts%10000 == 0 || evts == 1) //print some event information
	printf("Filled %6i events so far, out of %i total events with %7i attempts (%6.2f%% done)\n",
	       evts, final_events, attempts, evts*100./final_events);
    }
  }

  printf("Filled %i events total with %i attempts\n",
	       evts,  attempts);

  printf("Writing %s to disk\n", fout->GetName());
  //write results to disk
  tree_out->Write();
  fout->Write();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  
  return 0;
}


