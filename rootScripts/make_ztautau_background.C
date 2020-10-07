
bool backgroundOnly_ = false; //no signal tree
bool dataOnly_ = false; //only data trees
bool  doHiggsDecays_ = false; //Z or H0 CLFV decay sets
bool  doDY_ = true;
bool  doWJets_ = true;
bool  doTop_ = true;
bool  doHiggs_ = true;
bool  doDiboson_ = true;
bool  debug_ = false;
Int_t verbose_ = 1;
int   year_ = 2016;

Int_t make_background(int set = 7, TString selection = "mutau", TString base = "../histograms/ztautau/") {

  cout << "Beginning to make tree for selection " << selection.Data() << " with set " << set
       << endl;
  if(dataOnly_ && backgroundOnly_) {
    cout << "Set for both background and data only! Exiting...\n";
    return 1;
  }
  
  const char* nano_names[] = {"DY50"               ,
			      "SingleAntiToptW"    ,
			      "SingleToptW"        ,
			      "WW"	           ,
			      "WZ"                 ,
			      "ZZ"                 ,
			      "WWW"                ,
			      "Wlnu"               ,
			      "ttbarToSemiLeptonic",
			      "ttbarlnu"           ,
			      "ZMuTau"             ,
			      "ZETau"              ,
			      "ZEMu"               ,
			      "HMuTau"             ,
			      "HETau"              ,
			      "HEMu"               ,
			      "SingleMu"           ,
			      "SingleEle"          
  };
  
  int doNanoProcess[] = {!dataOnly_ && doDY_
			 , !dataOnly_ && doTop_ //tbar_tw
			 , !dataOnly_ && doTop_ //t_tw
			 , !dataOnly_ && doDiboson_ //WW
			 , !dataOnly_ && doDiboson_ //WZ
			 , !dataOnly_ && doDiboson_ //ZZ
			 , !dataOnly_ && doDiboson_ //WWW
			 , !dataOnly_ && doWJets_ //WJets
			 , !dataOnly_ && doTop_ //ttbar
			 , !dataOnly_ && doTop_ //ttbar
			 , !dataOnly_ && (!backgroundOnly_ && !doHiggsDecays_ && (selection.Contains("mutau"))) //zmutau
			 , !dataOnly_ && (!backgroundOnly_ && !doHiggsDecays_ && (selection.Contains("etau") )) //zetau
			 , !dataOnly_ && (!backgroundOnly_ && !doHiggsDecays_ && (selection == "emu")  ) //zetau
			 , !dataOnly_ && (!backgroundOnly_ && doHiggsDecays_  && (selection.Contains("mutau"))) //hmutau
			 , !dataOnly_ && (!backgroundOnly_ && doHiggsDecays_  && (selection.Contains("etau") )) //hetau
			 , !dataOnly_ && (!backgroundOnly_ && doHiggsDecays_  && (selection == "emu")  ) //hetau
			 , dataOnly_ //SingleMu
			 , dataOnly_ //SingleEle
  };

  TFile* fDList[30];
  TFile* fList[30];
  TTree* tList[30]; //to keep trees in memory
  TList* list = new TList;
  Int_t filecount = 0;
  TString fileSelec = selection; //use different selection for files since tau_e/mu decays --> emu channel
  if(selection.Contains("_")) //is a leptonic tau channel
    fileSelec = "emu";
  int nfiles = sizeof(nano_names)/sizeof(*nano_names);
  for(int i = 0; i < nfiles; ++i) {
    fDList[filecount] = 0;
    fList[filecount] = 0;
    tList[filecount] = 0;
    if(!doNanoProcess[i]) continue;
    TString sname = nano_names[i];
    const char* c = Form("%sztautau_%s_clfv_%i_%s.hist",base.Data(),fileSelec.Data(),year_,nano_names[i]);
    if(gSystem->AccessPathName(c)) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    fList[filecount] = TFile::Open(Form("%s",c), "READ");
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
  if(verbose_ > 1)
    cout << "Collected " << list->GetSize() << " trees for merging!\n";
  
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
  if(backgroundOnly_) type += "bkg_";
  else if(dataOnly_)  type += "data_";
  else type += (doHiggsDecays_) ? "higgs_" : "Z0_";  
  type += "nano_";
  if(verbose_ > 1)
    cout << "Background training type " << type.Data() << endl;
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


Int_t make_all_backgrounds(TString base = "../histograms/nanoaods/") {
  Int_t status = 0;
  verbose_ = 0;
  //b-tag sets
  status += make_background(8,  "mutau"  , base);
  status += make_background(38, "etau"   , base);
  status += make_background(68, "emu"    , base);
  status += make_background(68, "mutau_e", base);
  status += make_background(68, "etau_mu", base);
  return status;
}
