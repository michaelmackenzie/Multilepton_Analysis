
Int_t make_background(int set = 7, TString selection = "mutau", TString base = "../histograms/ztautau/") {


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
			 , "zz_2q2q"  
			 , "zz_4l"    
			 , "htautau_gluglu"
			 , "zmutau"                 
			 , "zetau"                 
  };
  
  int doProcess[] = {1 //t_tw
		     , 1 //tbar_tw
		     , 1 //ttbar
		     , 1 //DY AMC
		     , 1 //DY AMC
		     , 1 //W1Jets
		     , 1 //W2Jets
		     , 1 //W3Jets
		     , 1 //W4Jets
		     , 1 //WW
		     , 1 //WZ
		     , 1 //WZ
		     , 1 //ZZ
		     , 1 //ZZ
		     , 1 //ZZ
		     , 1 //htautau
		     , ((selection == "mutau") ? 1 : 0) //zmutau
		     , ((selection == "etau")  ? 1 : 0) //zetau
  };

  TFile* fDList[30];
  TFile* fList[30];
  TTree* tList[30]; //to keep trees in memory
  TList* list = new TList;
  Int_t dycount = 0;
  Int_t filecount = 0;
  for(int i = 0; i < sizeof(names)/sizeof(*names); ++i) {
    fDList[filecount] = 0;
    fList[filecount] = 0;
    tList[filecount] = 0;
    if(!doProcess[i]) continue;
    TString sname = names[i];
    bool isDY = sname.Contains("zjets");
    const char* c = Form("%sztautau_%s_%sbltTree_%s.hist",base.Data(),selection.Data(),
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
    printf("Getting tree %i for merging, using %s\n",filecount,c);
    tList[filecount] = (TTree*) fList[filecount]->Get(Form("Data/tree_%i/tree_%i", set, set));
    if(!tList[filecount]) {
      printf("tree not found, continuing");
      continue;
    }
    cout << "Tree " << filecount << ": " << tList[filecount]
	 << " Entries: " << tList[filecount]->GetEntriesFast() << endl;
    list->Add(tList[filecount]);
    filecount++;
  }
  if(list->GetSize() == 0) {
    printf("No trees found to merge, exiting\n");
    return 1;
  }
  TFile* out = new TFile(Form("background_ztautau_%s_%i.tree", selection.Data(),set),"RECREATE");
  TTree* t = 0;
  printf("Merging trees\n");
  for(int i = 0; i < list->GetSize(); ++i)
    cout << "Tree " << i << ": " << list->At(i)
	 << " Entries " << tList[i]->GetEntriesFast() << endl;
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
