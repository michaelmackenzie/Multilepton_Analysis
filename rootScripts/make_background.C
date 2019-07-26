
Int_t make_background(int eventFolder = 3, int ptStudy = 0, int includeSignal = 1) {

  int doPtStudyFiles = ptStudy;

  const char* names[] = {
    			 "t_tw"                   ,
                         "tbar_tw"                ,
			 "ttbar_inclusive"        ,
			 "zjets_m-50_alt"         ,
			 "zjets_m-10to50_alt"     ,
			 "zjets_m-50"             , //inclusive?
			 "zjets_m-10to50"         , //inclusive?
			 "z1jets_m-50"            ,
			 "z1jets_m-10to50"        ,
			 "z2jets_m-50"            ,
			 "z2jets_m-10to50"        ,
			 "z3jets_m-50"            ,
			 "z3jets_m-10to50"        ,
			 "z4jets_m-50"            ,
			 "z4jets_m-10to50"        };
  
  const char* files[] = {Form("tree_%s.tree",names[0]),
			 Form("tree_%s.tree",names[1]),
			 Form("tree_%s.tree",names[2]),
			 Form("tree_%s.tree",names[3]),
			 Form("tree_%s.tree",names[4]),
			 Form("tree_%s.tree",names[5]),
			 Form("tree_%s.tree",names[6]),
			 Form("tree_%s.tree",names[7]),
			 Form("tree_%s.tree",names[8]),
			 Form("tree_%s.tree",names[9]),
			 Form("tree_%s.tree",names[10]),
			 Form("tree_%s.tree",names[11]),
			 Form("tree_%s.tree",names[12]),
			 Form("tree_%s.tree",names[13]),
			 Form("tree_%s.tree",names[14])};

  const char* filesPtStudy[] = {Form("tree_%s.ptstudy_tree",names[0]),
				Form("tree_%s.ptstudy_tree",names[1]),
				Form("tree_%s.ptstudy_tree",names[2]),
				Form("tree_%s.ptstudy_tree",names[3]),
				Form("tree_%s.ptstudy_tree",names[4]),
				Form("tree_%s.ptstudy_tree",names[5]),
				Form("tree_%s.ptstudy_tree",names[6]),
				Form("tree_%s.ptstudy_tree",names[7]),
				Form("tree_%s.ptstudy_tree",names[8]),
				Form("tree_%s.ptstudy_tree",names[9]),
				Form("tree_%s.ptstudy_tree",names[10]),
				Form("tree_%s.ptstudy_tree",names[11]),
				Form("tree_%s.ptstudy_tree",names[12]),
				Form("tree_%s.ptstudy_tree",names[13]),
				Form("tree_%s.ptstudy_tree",names[14])};

  const Double_t xsec[] = {
			   35.6,
			   35.6,
			   831.8,
			   6803.2,
			   21959.8,
			   6803.2,
			   21959.8,
			   1198.9,
			   855.5,
			   390.6,
			   466.1,
			   113.3,
			   114.5,
			   60.2,
			   36.4}; //taken from CMS AN-16-458

  const int doProcess[] = {1, //t_tw
			   1, //tbar_tw
			   1, //ttbar
			   0, //DY AMC
			   0, //DY AMC
			   0, //DY MadGraph
			   0, //DY MadGraph
			   1, //DY 1 Jet MadGraph
			   1, //DY 1 Jet MadGraph
			   1, //DY 2 Jet MadGraph
			   1, //DY 2 Jet MadGraph
			   1, //DY 3 Jet MadGraph
			   1, //DY 3 Jet MadGraph
			   1, //DY 4 Jet MadGraph
			   1  //DY 4 Jet MadGraph
  };

  const char* sNames[] = { //Signal
      			 "b2bx_t"                     ,
    			 "bb2bbdimu"                  ,
    			 "fcnc_mx10"                  ,
    			 "fcnc_mx30"                  ,
    			 "fcnc_mx60"                  };

  const char* sFiles[] = {Form("tree_%s.tree",sNames[0]),
			  Form("tree_%s.tree",sNames[1]),
			  Form("tree_%s.tree",sNames[2]),
			  Form("tree_%s.tree",sNames[3]),
			  Form("tree_%s.tree",sNames[4])};

  const char* sFilesPtStudy[] = {Form("tree_%s.ptstudy_tree",sNames[0]),
				 Form("tree_%s.ptstudy_tree",sNames[1]),
				 Form("tree_%s.ptstudy_tree",sNames[2]),
				 Form("tree_%s.ptstudy_tree",sNames[3]),
				 Form("tree_%s.ptstudy_tree",sNames[4])};

  const double sXsec[] = {0.017*1.75*10, //Using a mentioned cross section from AN2016_090_v8
			  0.017*1.75*10, //Multiplied by 1.75 as suggested by AN2016_458_v3
			  0.017*1.75*10, //Added a factor of 10 to see it
			  0.017*1.75*10,
			  0.017*1.75*10};

  const int sDoProcess[] = {1, //b2bx_t
			   1, //bb2bbdimu
			   0, //fcnc_mx10
			   0, //fcnc_mx30
			   0  //fcnc_mx60
  };
  Double_t lum = 5.3e3;
  TFile* fEList[100];
  TFile* fDList[100];
  TFile* fList[100];
  TTree* tList[100]; //to keep trees in memory

  for(int i = 0; i < sizeof(files)/sizeof(*files); ++i) {
    if(!doProcess[i]) continue;
    if(i >= sizeof(filesPtStudy)/sizeof(*filesPtStudy)
       && doPtStudyFiles > 0) break;
    const char* c = (doPtStudyFiles == 0) ? files[i] : filesPtStudy[i];
    TFile* f = new TFile(Form("%s",c), "UPDATE");
    if(f == 0) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    printf("using %s\n",c);
    TFile* fD = (TFile*) f->Get("Data");
    TFile* fE = (TFile*) fD->Get(Form("event_%i",eventFolder));
    if(!fE) {
      printf("Event folder not found in %s, continuing\n", c);
      continue;
    }
    TTree* tree = (TTree*) fE->Get(Form("event_tree_%i",eventFolder));

    TH1F* events = (TH1F*) f->Get(Form("TotalEvents_%s",names[i]));
    if(events == 0) {
      printf("Events Histogram in %s not found, continuing\n",c);
      continue;
    }
    if(tree == 0) {
      printf("Tree in %s not found, continuing\n",c);
      continue;
    }
    //    tree->SetName(Form("%s_%i",names[i],eventFolder));
    bool writeTree = false;
    if(!(tree->GetBranch("eventCategory"))) {
      printf("Adding event category branch\n");
      writeTree = true;
      Int_t category = i; //label each file by the number in the list
      TBranch* br = tree->Branch("eventCategory", &category, "eventCategory/I");
      Long64_t n = tree->GetEntriesFast();
      for(Long64_t j = 0; j < n; ++j) {
	tree->GetEntry(j);
	br->Fill();
      }
    }

    //if already has the branch, no need to fill it
    if(tree->GetBranch("fullEventWeight")) {
      if(writeTree) {
	printf("Writing back updated tree\n");
	tree->Write();
	f->Write();
	f->Close();
      }
      continue;
    }
    printf("Adding Full Weight Branch to %s tree\n", names[i]);
    Long64_t n = tree->GetEntriesFast();
    Float_t fullEventWeight = 0.;
    Float_t eventWeight     = 0.;
    Double_t xsecF          = 1./(events->GetBinContent(1) - 2*events->GetBinContent(11))*xsec[i]*lum;
    TBranch* br = tree->Branch("fullEventWeight", &fullEventWeight, "fullEventWeight/F");
    tree->SetBranchAddress("eventWeight",&eventWeight);

    for(Long64_t j = 0; j < n; ++j) {
      if(j%25000 == 0) printf("Processing event %lld\n", j);
      tree->GetEntry(j);
      fullEventWeight = eventWeight*xsecF;
      br->Fill();
    }
    delete events;
    printf("Writing back updated tree\n");
    tree->Write();
    f->Write();
    f->Close();
  }

  for(int i = 0; i < sizeof(sFiles)/sizeof(*sFiles); ++i) {
    if(includeSignal == 0) break;
    if(!sDoProcess[i]) continue;
    if(i >= sizeof(sFilesPtStudy)/sizeof(*sFilesPtStudy)
       && doPtStudyFiles > 0) break;
    const char* c = (doPtStudyFiles == 0) ? sFiles[i] : sFilesPtStudy[i];
    TFile* f = new TFile(Form("%s",c), "UPDATE");
    if(f == 0) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    printf("using %s\n",c);
    TFile* fD = (TFile*) f->Get("Data");
    TFile* fE = (TFile*) fD->Get(Form("event_%i",eventFolder));
    if(!fE) {
      printf("Event folder not found in %s, continuing\n", c);
      continue;
    }
    TTree* tree = (TTree*) fE->Get(Form("event_tree_%i",eventFolder));

    TH1F* events = (TH1F*) f->Get(Form("TotalEvents_%s",sNames[i]));
    if(events == 0) {
      printf("Events Histogram in %s not found, continuing\n",c);
      continue;
    }
    if(tree == 0) {
      printf("Tree in %s not found, continuing\n",c);
      continue;
    }

    //adding event category
    bool writeTree = false;
    if(!(tree->GetBranch("eventCategory"))) {
      printf("Adding event category branch\n");
      writeTree = true;
      Int_t category = sizeof(files)/sizeof(*files) + i; //label each file by the number in the list
      TBranch* br = tree->Branch("eventCategory", &category, "eventCategory/I");
      Long64_t n = tree->GetEntriesFast();
      for(Long64_t j = 0; j < n; ++j) {
	tree->GetEntry(j);
	br->Fill();
      }
    }

    //if already has the branch, no need to fill it
    if(tree->GetBranch("fullEventWeight")) {
      if(writeTree) {
	printf("Writing back updated tree\n");
	tree->Write();
	f->Write();
	f->Close();
      }
      continue;
    }
    
    printf("Adding Full Weight Branch to %s tree\n", sNames[i]);
    Long64_t n = tree->GetEntriesFast();
    Float_t fullEventWeight = 0.;
    Float_t eventWeight     = 0.;
    Double_t xsecF          = 1./(events->GetBinContent(1) - 2*events->GetBinContent(11))*sXsec[i]*lum;
    TBranch* br = tree->Branch("fullEventWeight", &fullEventWeight, "fullEventWeight/F");
    tree->SetBranchAddress("eventWeight",&eventWeight);

    for(Long64_t j = 0; j < n; ++j) {
      if(j%25000 == 0) printf("Processing event %lld\n", j);
      tree->GetEntry(j);
      fullEventWeight = eventWeight*xsecF;
      br->Fill();
    }
    delete events;
    printf("Writing back updated tree\n");
    tree->Write();
    f->Write();
    f->Close();
  }


  TList* list = new TList;
  for(int i = 0; i < sizeof(files)/sizeof(*files); ++i) {
    fEList[i] = 0;
    fList[i] = 0;
    tList[i] = 0;
    if(!doProcess[i]) continue;
    if(i >= sizeof(filesPtStudy)/sizeof(*filesPtStudy)
       && doPtStudyFiles > 0) break;
    const char* c = (doPtStudyFiles == 0) ? files[i] : filesPtStudy[i];
    fList[i] = new TFile(Form("%s",c), "READ");
    if(fList[i] == 0) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    printf("Getting trees for merging, using %s\n",c);
    fDList[i] = (TFile*) fList[i]->Get("Data");
    fEList[i] = (TFile*) fDList[i]->Get(Form("event_%i",eventFolder));
    if(!fEList[i]) {
      printf("Event folder not found in %s, continuing\n", c);
      continue;
    }
    tList[i] = (TTree*) fEList[i]->Get(Form("event_tree_%i",eventFolder));
    list->Add(tList[i]);
  }
  for(int i = 0; i < sizeof(sFiles)/sizeof(*sFiles); ++i) {
    if(includeSignal == 0) break;
    int offset = sizeof(files)/sizeof(*files) + i;
    fEList[offset] = 0;
    fList[offset] = 0;
    tList[offset] = 0;
    if(!sDoProcess[i]) continue;
    if(i >= sizeof(sFilesPtStudy)/sizeof(*sFilesPtStudy)
       && doPtStudyFiles > 0) break;
    const char* c = (doPtStudyFiles == 0) ? sFiles[i] : sFilesPtStudy[i];
    fList[offset] = new TFile(Form("%s",c), "READ");
    if(fList[offset] == 0) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    printf("Getting trees for merging, using %s\n",c);
    fDList[offset] = (TFile*) fList[offset]->Get("Data");
    fEList[offset] = (TFile*) fDList[offset]->Get(Form("event_%i",eventFolder));
    if(!fEList[offset]) {
      printf("Event folder not found in %s, continuing\n", c);
      continue;
    }
    tList[offset] = (TTree*) fEList[offset]->Get(Form("event_tree_%i",eventFolder));
    list->Add(tList[offset]);
  }
  TFile* out = new TFile(Form("background%s_%i.tree", (includeSignal > 0 ? "_signal":""), eventFolder),"RECREATE");
  TTree* t = 0;
  printf("Merging trees\n");
  t = TTree::MergeTrees(list);
  t->SetName("background_tree");
  out->cd();
  printf("Writing output tree\n");
  t->Write();
  delete t;
  printf("Writing output file\n");
  out->Write();
  out->Close();

  return 0;
}
