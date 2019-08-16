Double_t signal_scale_ = 10.; //scale factor to increase the signal size by
Int_t doOrig_ = 0;

Int_t pre_process(int eventFolder, const char* name, Double_t xsec, Double_t lum, int category) {

  
  const char* file = Form("tree_%s%s.tree",name,(doOrig_==0)?"":"_orig");

  if(gSystem->AccessPathName(file)) {
    printf("File %s not found, continuing\n",file);
    return 1;
  }
  TFile* f = new TFile(Form("%s",file), "UPDATE");
  printf("Using file %s\n",file);
  TFile* fD = (TFile*) f->Get("Data");
  TFile* fE = (TFile*) fD->Get(Form("event_%i",eventFolder));
  if(!fE) {
    printf("Event folder not found in %s, continuing\n", file);
    return 2;
  }
  TTree* tree = (TTree*) fE->Get(Form("event_tree_%i",eventFolder));

  if(tree == 0) {
    fE->ls();
    printf("Tree in %s not found, continuing\n",file);
    return 3;
  }
  TH1F* events = (TH1F*) f->Get(Form("TotalEvents_%s",name));
  if(events == 0) {
    printf("Events Histogram in %s not found, continuing\n",file);
    return 4;
  }
  Long64_t n = tree->GetEntriesFast();
  printf("The tree contains %lld entries\n", n);
  if(n == 0) {
    return 5;
  }
  bool writeTree = false;
  if(!(tree->GetBranch("eventCategory"))) {
    printf("Adding event category branch\n");
    writeTree = true;
    TBranch* br = tree->Branch("eventCategory", &category, "eventCategory/I");
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
    return 0;
  }
  printf("Adding Full Weight Branch to %s tree\n", name);
  Float_t fullEventWeight = 0.;
  Float_t eventWeight     = 0.;
  Double_t xsecF          = 1./(events->GetBinContent(1) - 2*events->GetBinContent(11))*xsec*lum;
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

  return 0;
}

Int_t make_background(int eventFolder = 3, int doOrig = 0, int includeSignal = 1) {

  doOrig_=doOrig;

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

  int doProcess[] = {1, //t_tw
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

  const double sXsec[] = {0.017*1.75*signal_scale_, //Using a mentioned cross section from AN2016_090_v8
			  0.017*1.75*signal_scale_, //Multiplied by 1.75 as suggested by AN2016_458_v3
			  0.017*1.75*signal_scale_, //Multiplying factor to make visible
			  0.017*1.75*signal_scale_,
			  0.017*1.75*signal_scale_};

  int sDoProcess[] = {1, //b2bx_t
		      1, //bb2bbdimu
		      0, //fcnc_mx10
		      0, //fcnc_mx30
		      0  //fcnc_mx60
  };
  Double_t lum = 5.3e3;

  printf("Checking background trees\n");
  for(int i = 0; i < sizeof(names)/sizeof(*names); ++i) {
    if(!doProcess[i]) continue;
    doProcess[i] = (pre_process(eventFolder, names[i], xsec[i], lum, i) != 0 ? 0 : 1);
  }
  printf("Now checking signal trees\n");
  for(int i = 0; i < sizeof(sNames)/sizeof(*sNames); ++i) {
    if(includeSignal == 0) break;
    if(!sDoProcess[i]) continue;
    sDoProcess[i] = (pre_process(eventFolder, sNames[i], sXsec[i], lum, i+sizeof(names)/sizeof(*names)) != 0 ? 0 : 1);
  }

  TFile* fEList[30];
  TFile* fDList[30];
  TFile* fList[30];
  TTree* tList[30]; //to keep trees in memory
  TList* list = new TList;

  for(int i = 0; i < sizeof(names)/sizeof(*names); ++i) {
    fEList[i] = 0;
    fDList[i] = 0;
    fList[i] = 0;
    tList[i] = 0;
    if(!doProcess[i]) continue;
    const char* c = Form("tree_%s%s.tree",names[i],(doOrig_==0)?"":"_orig");
    TString strName = c; //c somehow gets written over on file reads?
    if(gSystem->AccessPathName(strName.Data())) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    fList[i] = new TFile(Form("%s",c), "READ");
    printf("Getting tree %i for merging, using %s\n",i,strName.Data());
    fDList[i] = (TFile*) fList[i]->Get("Data");
    fEList[i] = (TFile*) fDList[i]->Get(Form("event_%i",eventFolder));
    if(!fEList[i]) {
      printf("Event folder not found in %s, continuing\n", c);
      continue;
    }
    tList[i] = (TTree*) fEList[i]->Get(Form("event_tree_%i",eventFolder));
    list->Add(tList[i]);
  }
  
  printf("Now getting signal trees\n");
  for(int i = 0; i < sizeof(sNames)/sizeof(*sNames); ++i) {
    if(includeSignal == 0) break;
    int offset = sizeof(names)/sizeof(*names) + i;
    fEList[offset] = 0;
    fList[offset] = 0;
    tList[offset] = 0;
    if(!sDoProcess[i]) continue;
    const char* c = Form("tree_%s%s.tree",sNames[i],(doOrig_==0)?"":"_orig");
    TString strName = c;
    if(gSystem->AccessPathName(c)) {
      printf("File %s not found, continuing\n",c);
      continue;
    }
    fList[offset] = new TFile(Form("%s",c), "READ");
    printf("Getting tree %i for merging, using %s\n",i,strName.Data());
    fDList[offset] = (TFile*) fList[offset]->Get("Data");
    fEList[offset] = (TFile*) fDList[offset]->Get(Form("event_%i",eventFolder));
    if(!fEList[offset]) {
      printf("Event folder not found in %s, continuing\n", c);
      continue;
    }
    tList[offset] = (TTree*) fEList[offset]->Get(Form("event_tree_%i",eventFolder));
    list->Add(tList[offset]);
  }
  TFile* out = new TFile(Form("background%s%s_%i.tree", (includeSignal > 0 ? "_signal":""), (doOrig_==0)?"":"_orig", eventFolder),"RECREATE");
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
