
bool  doHiggsDecays_ = false; //Z or H0 CLFV decay sets
bool  doDY_ = true;
bool  doWJets_ = true;
bool  doTop_ = true;
bool  doHiggs_ = true;
bool  doDiboson_ = true;
bool  debug_ = false;
Int_t verbose_ = 0;

Int_t make_background(int set = 7, TString selection = "mutau", TString base = "../histograms/ztautau/") {

  cout << "Beginning to make tree for selection " << selection.Data() << " with set " << set << endl;
  
  const char* names[] = {"t_tw"                   
                         , "tbar_tw"                
			 , "ttbar_inclusive"        
			 , "zjets_m-50_amcatnlo"         
			 , "zjets_m-10to50_amcatnlo"     
			 , "w1jets"   
			 , "w2jets"   
			 , "w3jets"   
			 , "w4jets"   
			 , "ww"            
			 , "wz_2l2q"  
			 , "wz_3lnu"  
			 , "zz_2l2nu" 
			 , "zz_2l2q"  
			 , "zz_4l"    
			 , "htautau_gluglu"
			 , "zmutau"                 
			 , "zetau"                 
			 , "zemu"                 
			 , "hmutau"                 
			 , "hetau"                 
			 , "hemu"                 
  };
  
  int doProcess[] = {  doTop_ //t_tw
		       , doTop_ //tbar_tw
		       , doTop_ //ttbar
		       , doDY_ //DY AMC
		       , doDY_ //DY AMC
		       , doWJets_ //W1Jets
		       , doWJets_ //W2Jets
		       , doWJets_ //W3Jets
		       , doWJets_ //W4Jets
		       , doDiboson_ //WW
		       , doDiboson_ //WZ
		       , doDiboson_ //WZ
		       , doDiboson_ //ZZ
		       , doDiboson_ //ZZ
		       , doDiboson_ //ZZ
		       , doHiggs_ //htautau
		       , (!doHiggsDecays_ && (selection.Contains("mutau"))) //zmutau
		       , (!doHiggsDecays_ && (selection.Contains("etau") )) //zetau
		       , (!doHiggsDecays_ && (selection == "emu")  ) //zetau
		       , (doHiggsDecays_  && (selection.Contains("mutau"))) //hmutau
		       , (doHiggsDecays_  && (selection.Contains("etau") )) //hetau
		       , (doHiggsDecays_  && (selection == "emu")  ) //hetau
  };

  TFile* fDList[30];
  TFile* fList[30];
  TTree* tList[30]; //to keep trees in memory
  TList* list = new TList;
  Int_t dycount = 0;
  Int_t filecount = 0;
  TString fileSelec = selection; //use different selection for files since tau_e/mu decays --> emu channel
  if(selection.Contains("_")) //is a leptonic tau channel
    fileSelec = "emu";
  for(int i = 0; i < sizeof(names)/sizeof(*names); ++i) {
    fDList[filecount] = 0;
    fList[filecount] = 0;
    tList[filecount] = 0;
    if(!doProcess[i]) continue;
    TString sname = names[i];
    bool isDY = sname.Contains("zjets");
    const char* c = Form("%sztautau_%s_%sbltTree_%s.hist",base.Data(),fileSelec.Data(),
			 (isDY) ? Form("%i_",dycount+1) : "", names[i]);
    if(isDY) {
      if(dycount == 0) --i;
      dycount = (dycount+1)%2;
    }
    if(gSystem->AccessPathName(c)) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    fList[filecount] = new TFile(Form("%s",c), "READ");
    if(verbose_)
      printf("Getting tree %i for merging, using %s\n",filecount,c);
    tList[filecount] = (TTree*) fList[filecount]->Get(Form("Data/tree_%i/tree_%i", set, set));
    if(!tList[filecount]) {
      printf("tree not found, continuing\n");
      continue;
    }
    if(debug_) tList[filecount]->Print();
    if(verbose_ > 1)
      cout << "Tree " << filecount << ": " << tList[filecount]
	   << " Entries: " << tList[filecount]->GetEntriesFast() << endl;
    list->Add(tList[filecount]);
    filecount++;
  }
  if(list->GetSize() == 0) {
    printf("No trees found to merge, exiting\n");
    return 1;
  }
  TString type = ""; //which type of process is trained
  if     ( doDY_ && !doWJets_ && !doDiboson_ && !doTop_ && !doHiggs_)
    type = "DY_";
  else if(!doDY_ &&  doWJets_ && !doDiboson_ && !doTop_ && !doHiggs_)
    type = "WJets_";
  else if(!doDY_ && !doWJets_ &&  doDiboson_ && !doTop_ && !doHiggs_)
    type = "DiBoson_";
  else if(!doDY_ && !doWJets_ && !doDiboson_ &&  doTop_ && !doHiggs_)
    type = "Top_";
  else if(!doDY_ && !doWJets_ &&  doDiboson_ &&  doTop_ &&  doHiggs_)
    type = "DB_Top_H_";
  else if( doDY_ &&  doWJets_ &&  doDiboson_ &&  doTop_ &&  doHiggs_)
    type = "";
  else
    printf("Unknown process combination! No name flag added\n");
  type += (doHiggsDecays_) ? "higgs_" : "Z0_";
  TFile* out = new TFile(Form("background_ztautau_%s%s_%i.tree",
			      type.Data(),
			      selection.Data(),set),"RECREATE");
  TTree* t = 0;
  printf("Merging trees\n");
  if(verbose_ > 1) {
    for(int i = 0; i < list->GetSize(); ++i)
      cout << "Tree " << i << ": " << list->At(i)
	   << " Entries " << tList[i]->GetEntriesFast() << endl;
  }
  t = TTree::MergeTrees(list);
  t->SetName("background_tree");
  for(int i = 0; i < list->GetSize(); ++i)
    delete list->At(i);

  out->cd();
  printf("Writing output tree\n");
  t->Write();
  delete t;
  printf("Writing output file\n");
  out->Write();
  out->Close();

  return 0;
}


Int_t make_all_backgrounds(TString base = "../histograms/ztautau/") {
  Int_t status = 0;
  //no b-tag sets
  status += make_background(8,  "mutau"  , base);
  status += make_background(28, "etau"   , base);
  status += make_background(48, "emu"    , base);
  status += make_background(48, "mutau_e", base);
  status += make_background(48, "etau_mu", base);
  //no jet sets
  status += make_background(18,  "mutau" , base);
  status += make_background(38, "etau"   , base);
  status += make_background(58, "emu"    , base);
  status += make_background(58, "mutau_e", base);
  status += make_background(58, "etau_mu", base);
  //1 jet sets
  status += make_background(19,  "mutau" , base);
  status += make_background(39, "etau"   , base);
  status += make_background(59, "emu"    , base);
  status += make_background(59, "mutau_e", base);
  status += make_background(59, "etau_mu", base);
  //> 1 jet sets
  status += make_background(20,  "mutau" , base);
  status += make_background(40, "etau"   , base);
  status += make_background(60, "emu"    , base);
  status += make_background(60, "mutau_e", base);
  status += make_background(60, "etau_mu", base);
  return status;
}
