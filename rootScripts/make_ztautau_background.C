
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
vector<int> years_ = {2016, 2017, 2018};

Int_t combine_trees(vector<TString> in_files, TString selection, int file_set, TString out_name) {
  TFile* fDList[in_files.size()];
  TFile* fList[in_files.size()];
  TTree* tList[in_files.size()]; //to keep trees in memory
  TList* list = new TList;
  Int_t filecount = 0;
  int nfiles = in_files.size();
  for(int index = 0; index < nfiles; ++index) {
    fDList[filecount] = 0;
    fList[filecount] = 0;
    tList[filecount] = 0;
    const char* file_path = in_files[index];
    if(gSystem->AccessPathName(file_path)) {
      printf("File %s not found, continuing\n", file_path);
      continue;
    }
    fList[filecount] = TFile::Open(file_path, "READ");
    if(verbose_)
      printf("Getting tree %i for merging, using %s\n", filecount, file_path);
    //if QCD, offset set to same sign selection
    int set_use = file_set;
    TString fname = fList[filecount]->GetName();
    if((fname.Contains("SingleMu") || fname.Contains("SingleEle")) && !dataOnly_) //check if QCD
      set_use += ZTauTauHistMaker::fQcdOffset;
    if(verbose_ > 1) cout << "Using file set " << set_use << endl;
    tList[filecount] = (TTree*) fList[filecount]->Get(Form("Data/tree_%i/tree_%i", set_use, set_use));
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
  
  TFile* out = new TFile(out_name.Data(),"RECREATE");
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

Int_t make_background(int set = 8, TString selection = "mutau", TString base = "../histograms/nanoaods_dev/") {

  cout << "Beginning to make tree for selection " << selection.Data() << " with set " << set
       << endl;
  if(dataOnly_ && backgroundOnly_) {
    cout << "Set for both background and data only! Exiting...\n";
    return 1;
  }
  vector<TString> file_list;
  TString fileSelec = selection;
  if(fileSelec.Contains("_")) //leptonic tau
    fileSelec = "emu";
  for(int year : years_) {
    const char* nano_names[] = {"DY50"               ,
				"DY50-ext"           ,
				"SingleAntiToptW"    ,
				"SingleToptW"        ,
				"WW"	           ,
				"WZ"                 ,
				"ZZ"                 ,
				"WWW"                ,
				"Wlnu"               ,
				"Wlnu-ext"           ,
				"ttbarToSemiLeptonic",
				"ttbarlnu"           ,
				"ZMuTau"             ,
				"ZETau"              ,
				"ZEMu"               ,
				"HMuTau"             ,
				"HETau"              ,
				"HEMu"               ,
				"SingleMu"           ,
				"SingleEle"          ,
				"QCD_SingleMu"       , //use same sign data
				"QCD_SingleEle"
				
    };
  
    int doNanoProcess[] = {!dataOnly_ && doDY_ //DY50
			   , !dataOnly_ && doDY_ && year != 2018 //DY50-ext
			   , !dataOnly_ && doTop_ //tbar_tw
			   , !dataOnly_ && doTop_ //t_tw
			   , !dataOnly_ && doDiboson_ //WW
			   , !dataOnly_ && doDiboson_ //WZ
			   , !dataOnly_ && doDiboson_ //ZZ
			   , !dataOnly_ && doDiboson_ //WWW
			   , !dataOnly_ && doWJets_ //WJets
			   , !dataOnly_ && doWJets_ && year != 2018 //WJets-ext
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
			   , !dataOnly_ && !fileSelec.Contains("etau")  //SingleMu QCD
			   , !dataOnly_ && !fileSelec.Contains("mutau") //SingleEle QCD
    };
    int nfiles = sizeof(nano_names)/sizeof(*nano_names);
    for(int i = 0; i < nfiles; ++i) {
      if(!doNanoProcess[i]) continue;
      TString name = nano_names[i];
      if(name.Contains("Single") && name.Contains("QCD")) name.ReplaceAll("QCD_", "");
      file_list.push_back(Form("%sztautau_%s_clfv_%i_%s.hist",base.Data(),fileSelec.Data(),year,name.Data()));
    }
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
  if(backgroundOnly_) type += "bkg_";
  else if(dataOnly_)  type += "data_";
  else type += (doHiggsDecays_) ? "higgs_" : "Z0_";  
  type += "nano_";
  if(verbose_ > 1)
    cout << "Background training type " << type.Data() << endl;
  TString year_string = "";
  for(int i = 0; i < years_.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years_[i];
  }
  TString file_out = Form("background_ztautau_%s%s_%s_%i.tree",
			      type.Data(), selection.Data(), year_string.Data(), set);
  return combine_trees(file_list, fileSelec, set, file_out);
}


Int_t make_all_backgrounds(TString base = "../histograms/nanoaods_dev/") {
  Int_t status = 0;
  verbose_ = 0;
  cout << "Making Z decay backgrounds!\n";
  doHiggsDecays_ = false;
  //b-tag sets
  status += make_background(8,  "mutau"  , base);
  status += make_background(38, "etau"   , base);
  status += make_background(68, "emu"    , base);
  status += make_background(68, "mutau_e", base);
  status += make_background(68, "etau_mu", base);
  cout << "Making Higgs decay backgrounds!\n";
  doHiggsDecays_ = true;
  //b-tag sets
  status += make_background(8,  "mutau"  , base);
  status += make_background(38, "etau"   , base);
  status += make_background(68, "emu"    , base);
  status += make_background(68, "mutau_e", base);
  status += make_background(68, "etau_mu", base);
  return status;
}

Int_t make_all_years(TString base = "../histograms/nanoaods_dev/") {
  Int_t status = 0;
  // cout << "Making 2016 backgrounds...\n";
  // years_ = {2016};
  // status += make_all_backgrounds(base);
  // cout << "Making 2017 backgrounds...\n";
  // years_ = {2017};
  // status += make_all_backgrounds(base);
  // cout << "Making 2018 backgrounds...\n";
  // years_ = {2018};
  // status += make_all_backgrounds(base);
  //make total background file for all years combined
  cout << "Making Run-II backgrounds...\n";
  years_ = {2016, 2017, 2018};
  status += make_all_backgrounds(base);
  return status;
}
