
bool doHiggsDecays = false; //Z or H0 CLFV decay sets
bool doDY_ = true;
bool doWJets_ = true;
bool doTop_ = true;
bool doHiggs_ = true;
bool doDiboson_ = true;

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
		     , (!doHiggsDecays && (selection == "mutau")) //zmutau
		     , (!doHiggsDecays && (selection == "etau") ) //zetau
		     , (!doHiggsDecays && (selection == "emu")  ) //zetau
		     , (doHiggsDecays  && (selection == "mutau")) //hmutau
		     , (doHiggsDecays  && (selection == "etau") ) //hetau
		     , (doHiggsDecays  && (selection == "emu")  ) //hetau
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
  
  TFile* out = new TFile(Form("background_ztautau_%s%s_%i.tree",
			      type.Data(),
			      selection.Data(),set),"RECREATE");
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
