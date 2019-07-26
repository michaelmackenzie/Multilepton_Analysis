int MVA_ = 0; //0 for NN, 1 for BDT

int plot_tmva_tree(const char* file = "../CWoLa_training_background_3.root", double mva_cut = 0.,
		   int category = -1) {

  TString fname = file;
  TFile* f = new TFile(file,"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }
  
  TH1F* htest  = new TH1F("htest" ,"Test" ,70,0.,70.);
  TH1F* htrain = new TH1F("htrain","Train",70,0.,70.);

  TString mva_var;
  if(MVA_ == 0)
    mva_var = "MLP_MM";
  else if(MVA_ == 1)
    mva_var = "BDT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 3;
  }
  
  TString cut;
  if(fname.Contains("mock"))
    cut = Form("genWeight*((%s>=%.5f)",mva_var.Data(),mva_cut); //don't weight data/mock data
  else
    cut = Form("fullEventWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    
  if(category > -1) cut += Form("&&(eventCategory == %i)",category);
  cut += ")";
  test_tree-> Draw("lepM>>htest" , cut.Data());
  train_tree->Draw("lepM>>htrain", cut.Data());

  TObject* o = (gDirectory->Get("c1"));
  delete o;
  
  TCanvas* c = new TCanvas("c_mass","Mass Canvas", 900, 500);
  //  htest->Scale(htrain->Integral()/htest->Integral());
  htrain->Draw("hist");
  htest->Draw("sames hist");

  htest->SetLineColor(kRed);
  htest->SetFillColor(kRed);
  htest->SetFillStyle(3002);
  htrain->SetLineColor(kAzure-3);
  htrain->SetFillColor(kAzure-4);

  c->SetGridx();
  c->SetGridy();
  c->BuildLegend();
  c->Update();
  
  htrain->SetMaximum(1.2*max(htest->GetMaximum(),htrain->GetMaximum()));
  htrain->SetTitle(Form("M_{ll} for %s >= %.5f",mva_var.Data(),mva_cut));
  htrain->SetXTitle("M_{ll} (GeV)");
  htrain->SetYTitle("Entries / 1 GeV");

  return 0;
}


int stack_tmva_tree(const char* file = "../CWoLa_training_background_3.root", double mva_cut = 0.,
		  int  plot_train = 1)  {

  TFile* f = new TFile(file,"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }
  TString fname = file;
  
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }

  const char* names[] = {
    			 "T"        ,  // 0
                         "T"        ,  // 1
			 "TTbar"    ,  // 2
			 "Z+Jets"   ,  // 3
			 "Z+Jets"   ,  // 4
			 "Z+Jets"   ,  // 5
			 "Z+Jets"   ,  // 6
			 "Z+Jets"   ,  // 7
			 "Z+Jets"   ,  // 8
			 "Z+Jets"   ,  // 9
			 "Z+Jets"   ,  // 10
			 "Z+Jets"   ,  // 11
			 "Z+Jets"   ,  // 12
			 "Z+Jets"   ,  // 13
			 "Z+Jets"   ,  // 14
			 //Signal      // 
      			 "B-->bX"   ,  // 15
    			 "BB2bbXX"  ,  // 16
    			 "fcnc_mx10",  // 17
    			 "fcnc_mx30",  // 18
    			 "fcnc_mx60"}; // 19

  const int colors[] = {
    			 kYellow+1  ,
                         kYellow+1  ,
			 kYellow+3  ,
			 kBlue+1  ,
			 kRed+1  ,
			 kRed+1  , //inclusive?
			 kRed+1  , //inclusive?
			 kRed+1  ,
			 kRed+1  ,
			 kRed+1  ,
			 kRed+1  ,
			 kRed+1  ,
			 kRed+1  ,
			 kRed+1  ,
			 kRed+1  ,
			 //signal
      			 kGreen+2  ,
    			 kGreen+4  ,
    			 kBlue-1  ,
    			 kBlue-1  ,
    			 kBlue-1  };
  TH1F* htest[100];
  TH1F* htrain[100];
  for(int i = 0; i < 100; ++i) {
    htest[i] = 0;
    htrain[i] = 0;
  }

  TString mva_var;
  if(MVA_ == 0)
    mva_var = "MLP_MM";
  else if(MVA_ == 1)
    mva_var = "BDT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 3;
  }
  test_tree-> Draw("lepM>>htest_-1" );
  train_tree->Draw("lepM>>htrain_-1");
  TH1F* htraintmp = (TH1F*) gDirectory->Get("htrain_-1");
  TH1F* htesttmp  = (TH1F*) gDirectory->Get("htest_-1");
  double scaleTrain = htesttmp->Integral()/htraintmp->Integral();
  delete htraintmp;
  delete htesttmp;
  
  int bins = 35;
  double xlow = 0.;
  double xhigh = 70.;
  int zjetindex = -1;
  int tindex = -1;
  if(scaleTrain > 0.) printf("Scaling training histograms by %.3f\n", scaleTrain);
  for(int i = 0; i <  sizeof(names)/sizeof(*names); ++i) {
    htest[i] = new TH1F(Form("htest_%i",i),   Form("Test %s" , names[i]), bins, xlow, xhigh);
    htrain[i] = new TH1F(Form("htrain_%i",i), Form("Train %s", names[i]), bins, xlow, xhigh);
    TString cut;
    if(fname.Contains("mock"))
      cut = Form("genWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    else
      cut = Form("fullEventWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);

    cut += Form("&&(eventCategory == %i))",i);
    
    test_tree-> Draw(Form("lepM>>htest_%i" , i), cut.Data());
    train_tree->Draw(Form("lepM>>htrain_%i", i), cut.Data());
    TString name = names[i];
    if(scaleTrain > 0.) htrain[i]->Scale(scaleTrain);
    if(name.Contains("Z+Jets")) {
      if(zjetindex > -1) {
	htest[zjetindex]->Add(htest[i]);
      	htrain[zjetindex]->Add(htrain[i]);
      } else zjetindex = i;
    }
    if(name == "T") {
      if(tindex > -1) {
	htest[tindex]->Add(htest[i]);
      	htrain[tindex]->Add(htrain[i]);
      } else tindex = i;
    }
    htest[i]->SetLineColor(colors[i]);
    htest[i]->SetFillColor(colors[i]);
    htest[i]->SetFillStyle(3001);
    htrain[i]->SetMarkerColor(colors[i]-5);
    htrain[i]->SetMarkerStyle(20);
    htrain[i]->SetLineColor(colors[i]-5);
    //    htrain[i]->SetFillColor(colors[i]-5);
  }
  TObject* o = (gDirectory->Get("c1"));
  if(o) delete o;

  THStack* hstacktest  = new THStack("teststack" , "Testing Stack");
  THStack* hstacktrain = new THStack("trainstack", "Training Stack");
  for(int i = 0; i <  sizeof(names)/sizeof(*names); ++i) {
    TString name = names[i];
    if(name.Contains("Z+Jets") && i != zjetindex)
      continue;
    if(name == "T" && i != tindex)
      continue;
    htest[i] ->SetTitle(Form("%s #scale[0.5]{#int} = %.2e", htest[i] ->GetTitle() , htest[i]->Integral()));
    htrain[i]->SetTitle(Form("%s #scale[0.5]{#int} = %.2e", htrain[i]->GetTitle(), htrain[i]->Integral()));
    
    if(htest[i]->GetEntries() > 0) hstacktest->Add(htest[i]);
    if(htrain[i]->GetEntries() > 0) hstacktrain->Add(htrain[i]);
  }
  
  TCanvas* c = new TCanvas("c_mass_stack","Mass Canvas", 1200, 800);
  //  htest->Scale(htrain->Integral()/htest->Integral());
  if(plot_train > 0 && hstacktrain->GetNhists() > 0) {
    hstacktrain->Draw("E noclear");
    if(hstacktest->GetNhists() > 0)
      hstacktest->Draw("sames hist noclear");
  } else if(hstacktest->GetNhists() > 0)
    hstacktest->Draw("hist noclear");


  c->SetGridx();
  c->SetGridy();
  c->BuildLegend();
  c->Update();
  
  if(plot_train > 0 && hstacktrain->GetNhists() > 0) {
    if(hstacktest->GetNhists() > 0)
      hstacktrain->SetMaximum(1.2*max(hstacktest->GetMaximum(),hstacktrain->GetMaximum()));
    hstacktrain->SetTitle(Form("M_{ll} for %s >= %.5f",mva_var.Data(),mva_cut));
    hstacktrain->GetXaxis()->SetTitle("M_{ll} (GeV)");
    hstacktrain->GetYaxis()->SetTitle(Form("Entries / %.1f GeV",hstacktrain->GetXaxis()->GetBinWidth(1)));
  } else  if(hstacktest->GetNhists() > 0) {
    hstacktest->SetTitle(Form("M_{ll} for %s >= %.5f",mva_var.Data(),mva_cut));
    hstacktest->GetXaxis()->SetTitle("M_{ll} (GeV)");
    hstacktest->GetYaxis()->SetTitle(Form("Entries / %.1f GeV",hstacktest->GetXaxis()->GetBinWidth(1)));
  }
  return 0;
}



int plot_roc(const char* file = "../CWoLa_training_background_3.root", int category = 15, double mva_step = 1.e-2) {

  TFile* f = new TFile(file,"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }
  TString fname = file;

  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }

  int n = (int) (1./mva_step);
  Double_t xtest[n] ,ytest[n]; 
  Double_t xtrain[n],ytrain[n]; 
  double normTest1, normTest2;
  double normTrain1, normTrain2;
  TH1F* httest1 = new TH1F( "httest1","htest1",100.,0.,100.);
  TH1F* httest2 = new TH1F( "httest2","htest2",100.,0.,100.);
  TH1F* httrain1 = new TH1F("httrain1","htrain1",100.,0.,100.);
  TH1F* httrain2 = new TH1F("httrain2","htrain2",100.,0.,100.);
  if(fname.Contains("mock")) {
    test_tree-> Draw( "lepM>>httest1",   Form("genWeight*(eventCategory == %i)",category));
    test_tree-> Draw( "lepM>>httest2",   Form("genWeight*(eventCategory != %i)",category));
    train_tree-> Draw("lepM>>httrain1",  Form("genWeight*(eventCategory == %i)",category));
    train_tree-> Draw("lepM>>httrain2",  Form("genWeight*(eventCategory != %i)",category));
  } else {
    test_tree-> Draw( "lepM>>httest1",   Form("fullEventWeight*(eventCategory == %i)",category));
    test_tree-> Draw( "lepM>>httest2",   Form("fullEventWeight*(eventCategory != %i)",category));
    train_tree-> Draw("lepM>>httrain1",  Form("fullEventWeight*(eventCategory == %i)",category));
    train_tree-> Draw("lepM>>httrain2",  Form("fullEventWeight*(eventCategory != %i)",category));
  }
  normTest1  = httest1->Integral();
  normTest2  = httest2->Integral();
  normTrain1 = httrain1->Integral();
  normTrain2 = httrain2->Integral();
  printf("Initial integrals are:\n Bkg_Test  = %.2f\n Sig_Test  = %.2f\n Bkg_Train = %.2f\n Sig_Train = %.2f\n",
	 normTest1, normTest2, normTrain1, normTrain2);
  if(normTest1 == 0. || normTest2 == 0. ||
     normTrain1 == 0. || normTrain2 == 0.) {
    printf("Number of events contains a 0, exiting\n");
    return 3;
  }
  TString mva_var;
  if(MVA_ == 0)
    mva_var = "MLP_MM";
  else if(MVA_ == 1)
    mva_var = "BDT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 4;
  }
  
  
  for(int i = 0; i <  n; ++i) {
    xtest[n] = 0.;
    ytest[n] = 0.;
    xtrain[n] = 0.;
    ytrain[n] = 0.;
  }
  for(int i = 0; i <  n; ++i) {
    double mva_cut = i*mva_step;
    if((i+1)%10 == 0) printf("Step %i, mva_cut = %.3f\n", i+1,mva_cut);
    TH1F* htest1 = new TH1F("htest1","htest1",100.,0.,100.);
    TH1F* htest2 = new TH1F("htest2","htest2",100.,0.,100.);
    TH1F* htrain1 = new TH1F("htrain1","htrain1",100.,0.,100.);
    TH1F* htrain2 = new TH1F("htrain2","htrain2",100.,0.,100.);
    TString cut;
    if(fname.Contains("mock"))
      cut = Form("genWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    else
      cut = Form("fullEventWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);      

    TString cutSig(cut);
    cutSig += Form("&&(eventCategory == %i))",category);
    TString cutBkg(cut);
    cutBkg += Form("&&(eventCategory != %i))",category);
    
    test_tree-> Draw("lepM>>htest1", cutSig.Data());
    test_tree-> Draw("lepM>>htest2", cutBkg.Data());
    train_tree-> Draw("lepM>>htrain1", cutSig.Data());
    train_tree-> Draw("lepM>>htrain2", cutBkg.Data());

    double xtestVal = (htest1->Integral())/normTest1;
    double ytestVal = (htest2->Integral())/normTest2;
    double xtrainVal = (htrain1->Integral())/normTrain1;
    double ytrainVal = (htrain2->Integral())/normTrain2;
    xtest[i] = xtestVal;
    ytest[i] = 1.-ytestVal;
    xtrain[i] = xtrainVal;
    ytrain[i] = 1.-ytrainVal;
    delete htest1  ;
    delete htest2  ;
    delete htrain1 ;
    delete htrain2 ;
    if(xtestVal == 0. && ytestVal == 0. &&
       xtrainVal == 0. && ytrainVal == 0. )
      break;
  }
  TGraph* roc_test = new TGraph(n,xtest,ytest);
  TGraph* roc_train = new TGraph(n,xtrain,ytrain);

  TCanvas* c = new TCanvas("ROC Canvas");
  roc_test->Draw();
  roc_test->SetLineColor(kBlue);
  roc_test->SetLineWidth(2);
  roc_test->SetTitle("ROC;Signal Efficiency;Background Rejection");
  roc_train->Draw("same");
  roc_train->SetLineColor(kRed);
  roc_train->SetLineWidth(2);


  c->SetGridx();
  c->SetGridy();
  TLegend* legend = new TLegend(0.1,0.5,0.48,0.7);
  legend->AddEntry(roc_train,"Train");
  legend->AddEntry(roc_test,"Test");
  legend->Draw("same");
  c->Update();
  return 0;
}

