int MVA_ = 2; //0 for NN, 1 for BDT, 2 for BDTRT
TString var_ = "lepM"; //which variable to plot
double xMin_ = 10.; //plotting domain
double xMax_ = 70.;
int    bins_ = 30; //number of histogram bins
double yMin_ = 0.; //plotting domain for 2D
double yMax_ = 100.;
int    binsy_ = 100; //number of histogram bins
int    print_ = 0; //print canvases
int    setSilent_ = 0; //sets to batch mode for canvas printing
int    new_training_ = 1; //new root trainings have different file structure

int plot_tmva_tree(const char* file = "CWoLa_training_background_3", double mva_cut = 0.,
		   int category = -1) {

  if(setSilent_) gROOT->SetBatch(kTRUE);
  else gROOT->SetBatch(kFALSE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";
    
  TFile* f = new TFile(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  if(new_training_) {
    TString foldername = "tmva_";
    foldername += file;
    f = (TFile*) f->Get(foldername.Data());
  }
  if(!f)  {
    printf("Folder in file not found\n");
    return 2;
  }
  
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }
  
  TH1F* htest  = new TH1F("htest" ,"Test" ,bins_,xMin_,xMax_);
  TH1F* htrain = new TH1F("htrain","Train",bins_,xMin_,xMax_);

  TString mva_var;
  if(MVA_ == 0)
    mva_var = "MLP_MM";
  else if(MVA_ == 1)
    mva_var = "BDT";
  else if(MVA_ == 2)
    mva_var = "BDTRT";
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
  test_tree-> Draw(Form("%s>>htest" , var_.Data()), cut.Data());
  train_tree->Draw(Form("%s>>htrain", var_.Data()), cut.Data());

  
  TCanvas* c = new TCanvas("c_mass","Mass Canvas", 900, 500);
  //  htest->Scale(htrain->Integral()/htest->Integral());
  htrain->Draw("hist");
  htest->Draw("sames hist");

  htest->SetLineColor(kRed);
  htest->SetMarkerColor(kRed);
  htest->SetFillColor(kRed);
  htest->SetFillStyle(3002);
  htrain->SetLineColor(kAzure-3);
  htrain->SetMarkerColor(kAzure-3);
  htrain->SetFillColor(kAzure-4);

  c->SetGridx();
  c->SetGridy();
  c->BuildLegend();
  c->Update();
  
  htrain->SetMaximum(1.2*max(htest->GetMaximum(),htrain->GetMaximum()));
  htrain->SetTitle(Form("M_{l l} for %s >= %.5f",mva_var.Data(),mva_cut));
  htrain->SetXTitle("M_{l l} (GeV)");
  htrain->SetYTitle("Entries / 1 GeV");

  if(print_) {
    TString fnm = "bkg_";
    if(fname.Contains("signal_2"))
      fnm+="2";
    else if(fname.Contains("signal_3"))
      fnm+="3";
    else if(fname.Contains("signal_11"))
      fnm+="11";
    else
      fnm +="X";
    if(fname.Contains("noCat_15") || fname.Contains("no_15"))
      fnm += "_noCat_15";
    c->Print(Form("figures/%s_hist_%s_%s_%.4f.png", fnm.Data(), var_.Data(), mva_var.Data(), mva_cut));
  }
  TObject* o = (gDirectory->Get("c1"));
  delete o;
  return 0;
}


int stack_tmva_tree(const char* file = "../CWoLa_training_background_3.root", double mva_cut = 0.,
		  int  plot_train = 1)  {

  if(setSilent_) gROOT->SetBatch(kTRUE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";
    
  TFile* f = new TFile(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  if(new_training_) {
    TString foldername = "tmva_";
    foldername += file;
    f = (TFile*) f->Get(foldername.Data());
  }
  if(!f)  {
    printf("Folder in file not found\n");
    return 2;
  }
  
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
  // else if(MVA_ == 1) //not currently using standard BDT
  //   mva_var = "BDT";
  else if(MVA_ == 2)
    mva_var = "BDTRT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 3;
  }
  double scaleTrain = 1.;
  if(plot_train > 0) {
    test_tree-> Draw(Form("%s>>htest_-1" , var_.Data()));
    train_tree->Draw(Form("%s>>htrain_-1", var_.Data()));
    TH1F* htraintmp = (TH1F*) gDirectory->Get("htrain_-1");
    TH1F* htesttmp  = (TH1F*) gDirectory->Get("htest_-1");
    scaleTrain = htesttmp->Integral()/htraintmp->Integral();
    delete htraintmp;
    delete htesttmp;
  }
  
  int zjetindex = -1;
  int tindex = -1;
  if(plot_train > 0 && scaleTrain > 0.) printf("Scaling training histograms by %.3f\n", scaleTrain);
  for(int i = 0; i <  sizeof(names)/sizeof(*names); ++i) {
    htest[i] = new TH1F(Form("htest_%i",i),   Form("Test %s" , names[i]), bins_, xMin_, xMax_);
    if(plot_train > 0) htrain[i] = new TH1F(Form("htrain_%i",i), Form("Train %s", names[i]), bins_, xMin_, xMax_);
    TString cut;
    if(fname.Contains("mock"))
      cut = Form("genWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    else
      cut = Form("fullEventWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);

    cut += Form("&&(eventCategory == %i))",i);
    
    test_tree-> Draw(Form("%s>>htest_%i" , var_.Data(), i), cut.Data());
    if(plot_train > 0) train_tree->Draw(Form("%s>>htrain_%i", var_.Data(), i), cut.Data());
    TString name = names[i];
    if(plot_train > 0 && scaleTrain > 0.) htrain[i]->Scale(scaleTrain);
    if(name.Contains("Z+Jets")) {
      if(zjetindex > -1) {
	htest[zjetindex]->Add(htest[i]);
      	if(plot_train > 0) htrain[zjetindex]->Add(htrain[i]);
      } else zjetindex = i;
    }
    if(name == "T") {
      if(tindex > -1) {
	htest[tindex]->Add(htest[i]);
      	if(plot_train > 0) htrain[tindex]->Add(htrain[i]);
      } else tindex = i;
    }
    htest[i]->SetLineColor(colors[i]);
    htest[i]->SetFillColor(colors[i]);
    htest[i]->SetFillStyle(3001);
    if(plot_train > 0) htrain[i]->SetMarkerColor(colors[i]-5);
    if(plot_train > 0) htrain[i]->SetMarkerStyle(20);
    if(plot_train > 0) htrain[i]->SetLineColor(colors[i]-5);
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
    if(plot_train > 0)
      htrain[i]->SetTitle(Form("%s #scale[0.5]{#int} = %.2e", htrain[i]->GetTitle(), htrain[i]->Integral()));
    
    if(htest[i]->GetEntries() > 0) hstacktest->Add(htest[i]);
    if(plot_train > 0 && htrain[i]->GetEntries() > 0) hstacktrain->Add(htrain[i]);
  }
  
  TCanvas* c = new TCanvas("c_stack","Stack Canvas", 1200, 800);
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
    hstacktrain->SetTitle(Form("M_{l l} for %s >= %.5f",mva_var.Data(),mva_cut));
    hstacktrain->GetXaxis()->SetTitle("M_{l l} (GeV)");
    hstacktrain->GetYaxis()->SetTitle(Form("Entries / %.1f GeV",hstacktrain->GetXaxis()->GetBinWidth(1)));
  } else  if(hstacktest->GetNhists() > 0) {
    hstacktest->SetTitle(Form("M_{l l} for %s >= %.5f",mva_var.Data(),mva_cut));
    hstacktest->GetXaxis()->SetTitle("M_{l l} (GeV)");
    hstacktest->GetYaxis()->SetTitle(Form("Entries / %.1f GeV",hstacktest->GetXaxis()->GetBinWidth(1)));
  }
  if(print_) {
    TString fnm = "bkg_";
    if(fname.Contains("signal_2"))
      fnm+="2";
    else if(fname.Contains("signal_3"))
      fnm+="3";
    else if(fname.Contains("signal_11"))
      fnm+="11";
    else
      fnm +="X";
    if(fname.Contains("noCat_15") || fname.Contains("no_15"))
      fnm += "_noCat_15";
    c->Print(Form("figures/%s_stack_%s_%s_%.4f.png", fnm.Data(), var_.Data(), mva_var.Data(), mva_cut));
  }
  return 0;
}



int plot_roc(const char* file = "../CWoLa_training_background_3.root", int category = 15, double mva_step = 1.e-2) {

  if(setSilent_) gROOT->SetBatch(kTRUE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";
    
  TFile* f = new TFile(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  if(new_training_) {
    TString foldername = "tmva_";
    foldername += file;
    f = (TFile*) f->Get(foldername.Data());
  }
  if(!f)  {
    printf("Folder in file not found\n");
    return 2;
  }
  
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
  else if(MVA_ == 2)
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

  if(print_) {
    TString fnm = "bkg_";
    if(fname.Contains("signal_2"))
      fnm+="2";
    else if(fname.Contains("signal_3"))
      fnm+="3";
    else if(fname.Contains("signal_11"))
      fnm+="11";
    else
      fnm +="X";
    if(fname.Contains("noCat_15") || fname.Contains("no_15"))
      fnm += "_noCat_15";
    c->Print(Form("figures/%s_roc_%s_%s.png", fnm.Data(), var_.Data(), mva_var.Data()));
  }

  return 0;
}

int plot_2D_tmva_tree(const char* file = "../CWoLa_training_background_3.root", double mva_cut = 0.,
		   int category = -1) {

  if(setSilent_) gROOT->SetBatch(kTRUE);
  else gROOT->SetBatch(kFALSE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";
    
  TFile* f = new TFile(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  if(new_training_) {
    TString foldername = "tmva_";
    foldername += file;
    f = (TFile*) f->Get(foldername.Data());
  }
  if(!f)  {
    printf("Folder in file not found\n");
    return 2;
  }
  
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }
  
  TH2F* htest  = new TH2F("htest" ,"Test" ,bins_,xMin_,xMax_, binsy_, yMin_, yMax_);
  TH2F* htrain = new TH2F("htrain","Train",bins_,xMin_,xMax_, binsy_, yMin_, yMax_);

  TString mva_var;
  if(MVA_ == 0)
    mva_var = "MLP_MM";
  else if(MVA_ == 1)
    mva_var = "BDT";
  else if(MVA_ == 2)
    mva_var = "BDTRT";
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
  test_tree-> Draw(Form("%s:lepM>>htest" , var_.Data()), cut.Data());
  train_tree->Draw(Form("%s:lepM>>htrain", var_.Data()), cut.Data());

  TObject* o = (gDirectory->Get("c1"));
  delete o;
  
  TCanvas* c = new TCanvas("c_mass","Mass Canvas", 900, 500);
  //  htest->Scale(htrain->Integral()/htest->Integral());
  htrain->Add(htest);
  htrain->Draw("COLZ");

  // htest->SetLineColor(kRed);
  // htest->SetFillColor(kRed);
  // htest->SetFillStyle(3002);
  // htrain->SetLineColor(kAzure-3);
  // htrain->SetFillColor(kAzure-4);

  c->SetGridx();
  c->SetGridy();
  //  c->BuildLegend();
  c->Update();
  
  //  htrain->SetMaximum(1.2*max(htest->GetMaximum(),htrain->GetMaximum()));
  // htrain->SetTitle(Form("M_{{\\ell}{\\ell}} vs %s for %s >= %.5f",var_.Data(),mva_var.Data(),mva_cut));
  htrain->SetTitle("");
  htrain->SetXTitle(Form("M_{{\\ell}{\\ell}}\t /\t %.1f\t GeV}", (xMax_-xMin_)/bins_));
  htrain->SetYTitle(Form("%s / %.1f", var_.Data(), (yMax_-yMin_)/binsy_));

  if(print_) {
    TString fnm = "bkg_";
    if(fname.Contains("signal_2"))
      fnm+="2";
    else if(fname.Contains("signal_3"))
      fnm+="3";
    else if(fname.Contains("signal_11"))
      fnm+="11";
    else
      fnm +="X";
    if(fname.Contains("noCat_15") || fname.Contains("no_15"))
      fnm += "_noCat_15";
    c->Print(Form("figures/%s_2D_hist_%s_%s_%.4f.png", fnm.Data(), var_.Data(), mva_var.Data(), mva_cut));
  }
  return 0;
}

int plot_correlations_tmva_tree(const char* file = "CWoLa_training_background_3", int category = -1) {

  // Use TH2::GetCorrelationFactor
  if(setSilent_) gROOT->SetBatch(kTRUE);
  else gROOT->SetBatch(kFALSE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";
    
  TFile* f = new TFile(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  if(new_training_) {
    TString foldername = "tmva_";
    foldername += file;
    f = (TFile*) f->Get(foldername.Data());
  }
  
  if(!f)  {
    printf("Folder in file not found\n");
    return 2;
  }
  
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }

  vector<double>  corrs;
  vector<TString> names;
  auto branches = test_tree->GetListOfBranches();
  int numB = branches->GetSize();
  for(int br = 0; br < numB; ++br) {  
  
    TH2F* htest; //  = new TH2F("htest" ,"Test" ,bins_,xMin_,xMax_, binsy_, yMin_, yMax_);
    TH2F* htrain; // = new TH2F("htrain","Train",bins_,xMin_,xMax_, binsy_, yMin_, yMax_);

    TString mva_var;
    if(MVA_ == 0)
      mva_var = "MLP_MM";
    else if(MVA_ == 1)
      mva_var = "BDT";
    else if(MVA_ == 2)
      mva_var = "BDTRT";
    else {
      printf("MVA_ value doesn't correspond to known MVA\n");
      return 3;
    }
  
    TString cut;
    if(fname.Contains("mock"))
      cut = "genWeight"; //don't weight data/mock data
    else
      cut = "fullEventWeight";
    
    if(category > -1) cut += Form("*(eventCategory == %i)",category);
    if(category < -1) cut += Form("*(eventCategory != %i)",category);

    TString var = (branches->At(br))->GetName();
    test_tree-> Draw(Form("%s:%s>>htest" , var.Data(), mva_var.Data()), cut.Data());
    htest = (TH2F*) gDirectory->Get("htest");
    htrain = (TH2F*) htest->Clone("htrain");
    htrain->Reset();
    train_tree->Draw(Form("%s:%s>>htrain", var.Data(), mva_var.Data()), cut.Data());

    TObject* o = (gDirectory->Get("c1"));
    delete o;

    htrain->Add(htest);
    corrs.push_back(htrain->GetCorrelationFactor());
    names.push_back(var);

    printf("Correlation between %s and %s = %.6f\n", mva_var.Data(), var.Data(), corrs[br]);
  }

  return 0;
}

int plot_all_correlations_tmva_tree(const char* file = "CWoLa_training_background_3", int category = 15) {

  // Use TH2::GetCorrelationFactor
  if(setSilent_) gROOT->SetBatch(kTRUE);
  else gROOT->SetBatch(kFALSE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";
    
  TFile* f = new TFile(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  }

  if(new_training_) {
    TString foldername = "tmva_";
    foldername += file;
    f = (TFile*) f->Get(foldername.Data());
  }
  
  if(!f)  {
    printf("Folder in file not found\n");
    return 2;
  }
  
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }

  vector<TString> names;
  auto branches = test_tree->GetListOfBranches();
  int numB = branches->GetSize();

  TH2F* corrMatB = new TH2F("corrMatB", "Correlation Matrix (background)", numB, 0, numB, numB, 0, numB);
  TH2F* corrMatS = new TH2F("corrMatS", "Correlation Matrix (signal)"    , numB, 0, numB, numB, 0, numB);

  gROOT->SetBatch(kTRUE);
  for(int brx = 0; brx < numB; ++brx) {  
    TString varx = (branches->At(brx))->GetName();
    corrMatB->GetXaxis()->SetBinLabel(brx+1, varx.Data());
    corrMatB->GetYaxis()->SetBinLabel(brx+1, varx.Data());
    corrMatS->GetXaxis()->SetBinLabel(brx+1, varx.Data());
    corrMatS->GetYaxis()->SetBinLabel(brx+1, varx.Data());
    corrMatS->SetBinContent(brx+1,brx+1, 100.);
    corrMatB->SetBinContent(brx+1,brx+1, 100.);
    for(int bry = brx+1; bry < numB; ++bry) {  
  
      TH2F* htest; 
      TH2F* htrain; 

      TString cut;
      if(fname.Contains("mock"))
	cut = "genWeight"; //don't weight data/mock data
      else
	cut = "fullEventWeight";
    
      TString cutS = cut + Form("*(eventCategory == %i)",category);
      TString cutB = cut + Form("*(eventCategory != %i)",category);

      TString vary = (branches->At(bry))->GetName();

      test_tree-> Draw(Form("%s:%s>>htest" , varx.Data(), vary.Data()), cutB.Data());
      htest = (TH2F*) gDirectory->Get("htest");
      htrain = (TH2F*) htest->Clone("htrain");
      htrain->Reset();
      train_tree->Draw(Form("%s:%s>>htrain", varx.Data(), vary.Data()), cutB.Data());

      htrain->Add(htest);
      double corrB = 100.*htrain->GetCorrelationFactor();
      corrMatB->SetBinContent(brx+1,bry+1,corrB);
      corrMatB->SetBinContent(bry+1,brx+1,corrB);

      test_tree-> Draw(Form("%s:%s>>htest" , varx.Data(), vary.Data()), cutS.Data());
      htest = (TH2F*) gDirectory->Get("htest");
      htrain = (TH2F*) htest->Clone("htrain");
      htrain->Reset();
      train_tree->Draw(Form("%s:%s>>htrain", varx.Data(), vary.Data()), cutS.Data());

      htrain->Add(htest);
      
      double corrS = 100.*htrain->GetCorrelationFactor();
      corrMatS->SetBinContent(brx+1,bry+1,corrS);
      corrMatS->SetBinContent(bry+1,brx+1,corrS);
      delete htest;
      delete htrain;
    }
  }
  if(!setSilent_) gROOT->SetBatch(kFALSE);

  TCanvas* c1 = new TCanvas("corrsS", "Correlations (signal)", 750,900);
  corrMatS->Draw("text colz");
  c1->Update();
  
  corrMatS->GetXaxis()->SetLabelSize(0.02);
  corrMatS->GetYaxis()->SetLabelSize(0.02);
  TPaletteAxis* paletteAxis = (TPaletteAxis*)corrMatS->GetListOfFunctions()->FindObject( "palette" );
  paletteAxis->SetLabelSize( 0.03 );
  paletteAxis->SetX1NDC( paletteAxis->GetX1NDC() + 0.02 );
  TCanvas* c2 = new TCanvas("corrsB", "Correlations (background)", 750,900);
  corrMatB->Draw("text colz");
  c2->Update();

  paletteAxis = (TPaletteAxis*)corrMatB->GetListOfFunctions()->FindObject( "palette" );
  paletteAxis->SetLabelSize( 0.03 );
  paletteAxis->SetX1NDC( paletteAxis->GetX1NDC() + 0.02 );
  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat( ".0f" );

  corrMatB->GetXaxis()->SetLabelSize(0.02);
  corrMatB->GetYaxis()->SetLabelSize(0.02);
  c1->Modified();
  c1->Update();
  c2->Modified();
  c2->Update();
  
  return 0;
}

