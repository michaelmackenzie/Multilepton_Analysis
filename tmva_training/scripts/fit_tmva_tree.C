TFile*  f_   = 0; //to close open files at the end
int     MVA_ = 2; //0 for NN, 1 for BDT, 2 for BDTRT
TString MVA_var_ = "BDTRT"; //MVA variable name
TString var_ = "lepM"; //which variable to plot
double  xMin_ = 10.; //plotting domain
double  xMax_ = 70.;
int     bins_ = 30; //number of histogram bins
double  fitMin_ = 12.; //fitting domain
double  fitMax_ = 70.;
Fitter* fitter_ = 0; //object to make TF1s to fit with
int     fitset_ = -1; //set of fit parameters to use in fitter, -1 to on the fly determine it
double  sigmaRange_ = 100.; //sigmas to allow on parameter fit range
int     inc_train_ = 1; //whether or not to include training data in fit
int     print_ = 1; //print canvases
int     setSilent_ = 1; //sets to batch mode for canvas printing
int     ignoreSignal_ = 0; //whether or not to ignore signals
vector<int> signals_ = {15,16,17,18,19}; //signal IDs
int     scan_steps_ = 41; //number of points to take when scanning mva score
int     make_clean_ = 1; //delete all files in the canvas directory
int     new_training_ = 1; //new root trainings have different file structure
int     doLikely_     = 0; // if > 0 do binned likelihood fit


TTree* get_tmva_tree(const char* file, int doTrain = 0)  {

  TString path = "../";
  path += file;
  path += "/";
  path += file;
  path += ".root";
  TFile* f = new TFile(path.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return NULL;
  }
  f_ = f;
  TString foldername = "tmva_";
  foldername += file;
  if(new_training_) f = (TFile*) f->Get(foldername.Data());

  if(!f)  {
    printf("Folder in file not found\n");
    return NULL;
  }
  
  TString fname = file;
  TList* list = new TList;

  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return NULL;
  }
  list->Add(test_tree);
  if(doTrain) list->Add(train_tree);
  TTree* t = 0;
  t = TTree::MergeTrees(list);
  
  return t;
}
  
THStack* get_tmva_stack(const char* file, double mva_cut = -1., int doTrain = 0)  {

  TString path = "../";
  path += file;
  path += "/";
  path += file;
  path += ".root";
  TFile* f = new TFile(path.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return NULL;
  }
  f_ = f;
  TString foldername = "tmva_";
  foldername += file;
  if(new_training_) f = (TFile*) f->Get(foldername.Data());

  if(!f)  {
    printf("Folder in file not found\n");
    return NULL;
  }
  
  TString fname = file;
  
  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return NULL;
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
  TH1F* h[100];
  TH1F* hTrain[100];
  for(int i = 0; i < 100; ++i) {
    h[i] = 0;
    hTrain[i] = 0;
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
    return NULL;
  }
  

  int zjetindex = -1;
  int tindex = -1;
  double integral = 0.;
  for(int i = 0; i <  sizeof(names)/sizeof(*names); ++i) {
    bool skip = false;
    if(ignoreSignal_ > 0) {
      for(int j = 0; j < signals_.size(); ++j) {
	if(i == signals_[j]) skip = true;
      }
    }
    if(skip) continue;
    h[i] = new TH1F(Form("htest_%i",i),   Form("Test %s" , names[i]), bins_, xMin_, xMax_);
    if(doTrain > 0) hTrain[i] = new TH1F(Form("htrain_%i",i),   Form("Train %s" , names[i]), bins_, xMin_, xMax_);
    TString cut;
    if(fname.Contains("mock"))
      cut = Form("genWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    else
      cut = Form("fullEventWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);

    cut += Form("&&(eventCategory == %i))",i);
    
    test_tree-> Draw(Form("%s>>htest_%i" , var_.Data(), i), cut.Data());
    if(doTrain > 0) {
      train_tree->Draw(Form("%s>>htrain_%i", var_.Data(), i), cut.Data());
    }
    TString name = names[i];
    if(doTrain > 0) h[i]->Add(hTrain[i]);
    if(name.Contains("Z+Jets")) {
      if(zjetindex > -1) {
	h[zjetindex]->Add(h[i]);
      } else zjetindex = i;
    }
    if(name == "T") {
      if(tindex > -1) {
	h[tindex]->Add(h[i]);
      } else tindex = i;
    }
    h[i]->SetLineColor(colors[i]);
    h[i]->SetFillColor(colors[i]);
    h[i]->SetFillStyle(3001);
    integral += h[i]->Integral();
  }
  THStack* hstack;

  if(integral <= 0) return NULL;
  
  hstack  = new THStack("hstack" , "Data Stack");
  for(int i = 0; i <  sizeof(names)/sizeof(*names); ++i) {
    bool skip = false;
    if(ignoreSignal_ > 0) {
      for(int j = 0; j < signals_.size(); ++j) {
	if(i == signals_[j]) skip = true;
      }
    }
    if(skip) continue;

    TString name = names[i];
    if(name.Contains("Z+Jets") && i != zjetindex)
      continue;
    if(name == "T" && i != tindex)
      continue;
    h[i] ->SetTitle(Form("%s #scale[0.5]{#int} = %.2e", h[i] ->GetTitle() , h[i]->Integral()));
    
    if(h[i]->GetEntries() > 0.) hstack->Add(h[i]);
    
  }
  
  TObject* o = (gDirectory->Get("c1"));
  if(o) delete o;
  //  delete f; //Deleting removes histograms from the stack!
  return hstack;
}





double fit_tmva_tree(const char* file = "CWoLa_training_background_3", double mva_cut = 0.,
		  int category = -1) {

  if(setSilent_) gROOT->SetBatch(kTRUE); //doesn't draw canvases, so faster to make plots
  else gROOT->SetBatch(kFALSE);
  gStyle->SetTitleW(0.8f);

  if(fitter_ == 0) {
    fitter_ = new Fitter();
  }
  
  THStack* hstack = get_tmva_stack(file, mva_cut, inc_train_);
  TTree* tree = 0;
  if(doLikely_ < 0) {
    tree = get_tmva_tree(file,inc_train_);
    if(!tree) {
      printf("NULL tmva tree returned, exiting\n");
      return -999;
    }
  }
  if(!hstack) {
    printf("NULL stack returned, exiting\n");
    return -999.;
  }
  if(!setSilent_) printf("Retrieved data stack\n");
  TCanvas* c = new TCanvas("c_mass","Mass Canvas", 900, 500);
  TF1* f;
  TF1* f2;
  TString fname = file;
  int fitset = fitset_;
  if(fitset < 0) {
    if(fname.Contains("_11_"))      fitset = 111; //set 2 + exactly 1 b tag      
    else if(fname.Contains("_2_"))  fitset = 102; // > 1 central jets
    else if(fname.Contains("_3_"))  fitset = 103; // 1 central jet + fwd jets
    else if(fname.Contains("_6_"))  fitset = 106; //set 3 + btag
    else if(fname.Contains("_13_")) fitset = 113; //set 3 + btag
    else {
      printf("fitset not determined well, using 111\n");
      fitset = 111;
    }
  }
  int order = fitter_->Get_order(fitset);
  f = fitter_->Get_fit_background_function(fitset, sigmaRange_);
  f2 = fitter_->Get_signal_function();
  if(!f || !f2) {
    printf("Failed to get the fit functions, exiting\n");
    return -999.;
  }
  TF1* fBkgd_s = (TF1*) f->Clone(Form("fBkgd_s%i",fitset));
  fBkgd_s->SetName(Form("fBkgd_s%i",fitset));
  f2->SetName("fSignal");
  TF1* fSB = new TF1("fSignalBkgd",Form("fSignal + fBkgd_s%i",fitset));
  Double_t low;
  Double_t high;
  int sigpars = 3;
  //set signal parameters
  for(int j = 0; j < sigpars; ++j) {
    TString nm = f2->GetParName(j);
    if(nm == "Norm") {
      f2->GetParLimits(j,low,high);
      fSB->SetParLimits(j,low,high);
      fSB->SetParameter(j,f2->GetParameter(j));
    } else
      fSB->FixParameter(j,f2->GetParameter(j));
  }
  //set background parameters
  for(int j = 1; j <= order; ++j) {
    f->GetParLimits(j,low,high);
    fSB->SetParLimits(j+sigpars,low,high);
  }


  TH1F* h = (TH1F*) hstack->GetStack()->Last()->Clone("h");
  TH1F* h2 = (TH1F*) hstack->GetStack()->Last()->Clone("h2");
  TTree* t1 = (doLikely_ < 0) ? (TTree*) tree->Clone("t1") : 0;
  TTree* t2 = (doLikely_ < 0) ? (TTree*) tree->Clone("t2") : 0;
  Int_t bins = h->GetNbinsX();
  double hmin = h->GetXaxis()->GetXmin();
  double hmax = h->GetXaxis()->GetXmax();
  hmax = min(hmax,fitter_->fitMax_);
  hmin = max(hmin,fitter_->fitMin_);
  f->SetRange(hmin,hmax);
  fSB->SetRange(hmin,hmax);

  TString fitStr = "R S";
  if(doLikely_ > 0) fitStr += " L";
  if(setSilent_) fitStr += " Q";
  TFitResultPtr res =  h->Fit(f,fitStr.Data());
  // TFitResultPtr res = (doLikely_ < 0) ?
  //   t1->Fit(f,var_.Data(),Form("genWeight*((%s>=%.5f)",MVA_var_.Data(),mva_cut), fitStr.Data()) :
  //   h->Fit(f,fitStr.Data());
  
  double c1 = res->Chi2();
  int dof1 = res->Ndf();
  double prob1 = ROOT::Math::chisquared_cdf_c(c1,dof1);
  res =  h2->Fit(fSB,fitStr.Data());
  // TFitResultPtr res = (doLikely_ < 0) ?
  //   t2->Fit(fSB,var_.Data(),Form("genWeight*((%s>=%.5f)",MVA_var_.Data(),mva_cut), fitStr.Data()) :
  //   h2->Fit(fSB,fitStr.Data());
  double c2 = res->Chi2();
  int dof2 = res->Ndf();
  double prob2 = ROOT::Math::chisquared_cdf_c(c2,dof2);


  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  hstack->Draw("hist noclear");
  // h->Draw();
  double m = hstack->GetMaximum();

  f->Draw("same");
  fSB->Draw("same");
  f->SetTitle(Form("Background Only #chi^{2} = %.2f", c1));
  fSB->SetTitle(Form("Signal + Background #chi^{2} = %.2f", c2));
  fSB->SetLineColor(kMagenta);
  hstack->SetTitle (Form("#scale[0.5]{#int} Mock Data = %.2e",h->Integral()));
  c->BuildLegend();
//   h->GetXaxis()->SetTitle(xtitle.Data());
//   h->GetYaxis()->SetTitle(ytitle.Data());
  if(xMin_ < xMax_) {
    if(hmin < xMin_ && xMin_ < hmax) hmin = xMin_;
    if(hmax > xMax_ && hmin < xMax_) hmax = xMax_;
  }
  hstack->GetXaxis()->SetRangeUser(hmin,hmax);    
  hstack->GetYaxis()->SetRangeUser(1.e-1,m*1.2);    

  TString mva_var = "MVA_X";
  if(MVA_ == 0)      mva_var = "MLP_MM";
  else if(MVA_ == 2) mva_var = "BDTRT";

  hstack->SetTitle (Form("Mass fit using %s >= %.3f #Delta#chi^{2} = %.2f", mva_var.Data(), mva_cut, c1-c2));
  hstack->SetMinimum(1.e-1);
  hstack->SetMaximum(1.2*m);
  printf("MVA_Score > %.4f: Chi^2 / DoF for each are %.4f / %i p = %.4e (Background Only) and %.4f / %i p = %.4e (Signal + Bkgd), p2/p1 = %.4e\n",
	 mva_cut, c1, dof1, prob1, c2, dof2, prob2, prob2/prob1);
  c->SetGrid();
  c->BuildLegend();
  c->Update();

  if(print_) {
    TString fnm = "bkg_";
    if(fname.Contains("signal_2"))
      fnm+="2";
    else if(fname.Contains("signal_3"))
      fnm+="3";
    else if(fname.Contains("signal_6"))
      fnm+="6";
    else if(fname.Contains("signal_11"))
      fnm+="11";
    else if(fname.Contains("signal_12"))
      fnm+="12";
    else if(fname.Contains("signal_13"))
      fnm+="13";
    else
      fnm +="X";
    if(fname.Contains("noCat_15") || fname.Contains("no_15"))
      fnm += "_noCat_15";
    if(fname.Contains("truth"))
      fnm += "_truth";
    if(fname.Contains("scale")) {
      fnm += "_scale";
      if(fname.Contains("0500"))
	fnm += "_0500";
      else if(fname.Contains("0400"))
	fnm += "_0400";
      else if(fname.Contains("0300"))
	fnm += "_0300";
      else if(fname.Contains("0200"))
	fnm += "_0200";
      else if(fname.Contains("0100"))
	fnm += "_0100";
      else
	fnm += "_XXX";
    }
    if(ignoreSignal_)
      fnm += "_nosignal";
    TString leadZeros = "0"; //for gif writing
    int gifnum = (mva_cut+1)*1000.;
    if(abs(gifnum) < 1000) leadZeros+="0";
    if(abs(gifnum) < 100)  leadZeros+="0";
    if(abs(gifnum) < 10)   leadZeros+="0";
    fnm += Form("_fit_%s_%s", var_.Data(), mva_var.Data());
    gSystem->Exec(Form("[ ! -d figures/%s ] && mkdir figures/%s", fnm.Data(), fnm.Data()));
    c->Print(Form("figures/%s/%s_%s%i.png", fnm.Data(), fnm.Data(),leadZeros.Data(),gifnum));
  }
  TObject* o = (gDirectory->Get("c1"));
  delete o;
  if(f_) {
    delete f_;
    f_ = 0;
  }
  return (c1-c2);
}

TGraph* scan_fits(const char* file = "CWoLa_training_background_3.root", int category = -1) {

  double mva_min = -1.;
  double mva_max =  1.;
  if(MVA_ == 0)
    mva_min = 0.;

  if(!setSilent_)
    printf("Warning! Non-silent fits may take longer\n");
  if(!print_)
    printf("Warning! Not printing resulting canvases\n");

  double step_size = (mva_max-mva_min)/(scan_steps_-1); // -1 so end value is taken

  //store chi squared difference vs score for TGraph
  double mva_vals[scan_steps_+1];
  double chi_diff[scan_steps_+1];
  TString fnm = "bkg_";
  
  if(make_clean_ || print_) {
    TString fname = file;
    if(fname.Contains("signal_2"))
      fnm+="2";
    else if(fname.Contains("signal_3"))
      fnm+="3";
    else if(fname.Contains("signal_6"))
      fnm+="6";
    else if(fname.Contains("signal_11"))
      fnm+="11";
    else if(fname.Contains("signal_12"))
      fnm+="12";
    else if(fname.Contains("signal_13"))
      fnm+="13";
    else
      fnm +="X";
    if(fname.Contains("noCat_15") || fname.Contains("no_15"))
      fnm += "_noCat_15";
    if(fname.Contains("truth"))
      fnm += "_truth";
    if(fname.Contains("scale")) {
      fnm += "_scale";
      if(fname.Contains("0500"))
	fnm += "_0500";
      else if(fname.Contains("0400"))
	fnm += "_0400";
      else if(fname.Contains("0300"))
	fnm += "_0300";
      else if(fname.Contains("0200"))
	fnm += "_0200";
      else if(fname.Contains("0100"))
	fnm += "_0100";
      else
	fnm += "_XXX";
    }
    if(ignoreSignal_)
      fnm += "_nosignal";
    TString mva_var;
    if(MVA_ == 0)
      mva_var = "MLP_MM";
    // else if(MVA_ == 1) //not currently using standard BDT
    //   mva_var = "BDT";
    else if(MVA_ == 2)
      mva_var = "BDTRT";

    fnm += Form("_fit_%s_%s", var_.Data(), mva_var.Data());
    if(print_) gSystem->Exec(Form("[ -d figures/%s ] && rm figures/%s/ -r", fnm.Data(), fnm.Data()));
  }

  double m = -10.;
  for(int i = 0; i <= scan_steps_; ++i) {
    double chi = fit_tmva_tree(file, i*step_size + mva_min, category);
    mva_vals[i] = i*step_size + mva_min;
    chi_diff[i] = (chi > -10.) ? chi : 0.; //-999 corresponds to failed fit
    m = max(m,chi_diff[i]);
  }
  TGraph* g_score = new TGraph(scan_steps_+1, mva_vals, chi_diff);
  g_score->SetName("gScoreVsChi");
  g_score->SetMaximum(1.2*m);
  TString title = "MVA Score vs  #Delta#chi^{2}";
  if(!fnm.Contains("noCat")) {
    if(fnm.Contains("_0500_"))
      title += " (Signal x5)";
    else if(fnm.Contains("_0400_"))
      title += " (Signal x4)";
    else if(fnm.Contains("_0300_"))
      title += " (Signal x3)";
    else if(fnm.Contains("_0200_"))
      title += " (Signal x2)";
    else if(fnm.Contains("_0100_"))
      title += " (Signal x1)";
    else if(!fnm.Contains("scale"))
      title += " (Signal x10)";
  }
  title += "; MVA Score; #Delta#chi^{2}";
  g_score->SetTitle(title.Data());
  TCanvas* c = new TCanvas("c_score", "MVA Score Canvas", 800,500);
  g_score->Draw("APL");
  g_score->SetLineColor(kBlue);
  g_score->SetMarkerColor(kBlue);
  g_score->SetMarkerStyle(20);
  g_score->SetLineWidth(2);
  c->SetGrid();
  if(print_) {
    c->Print(Form("figures/%s_score_graph.png", fnm.Data()));
  }
  return g_score;
}

// assumes existence of a set of data files using the same name pattern
void scan_fit_set(const char* fileStart = "CWoLa_training_background_2_noCat_16",
	     const char* fileEnd = "_100000invpb_mock_data_mva_no_15_16",
	     int category = -1) {

  if(setSilent_) gROOT->SetBatch(kTRUE); //doesn't draw canvases, so faster to make plots
  else gROOT->SetBatch(kFALSE);
  TString fnm = fileStart;
  TString sets[] = {"", "_scale_0500", "_scale_0400", "_scale_0300",
		    "_scale_0200", "_scale_0100"};
  TString setNames_nosignal[] = {"Set 1","Set 2","Set 3","Set 4","Set 5","Set 6"};
  TString setNames_signal[] = {"Signal x10","Signal x5","Signal x4","Signal x3","Signal x2","Signal x1"}; 

  Int_t colors[] = {kBlue, kYellow, kRed, kGreen, kMagenta, kOrange};
  Int_t numSets = 6;
  TGraph* scores[numSets];
  for(int i = 0; i < numSets; ++i) {
    printf("Scanning over %s\n", (fnm+sets[i]+fileEnd).Data());
    int tmp = setSilent_;
    setSilent_ = 1;
    TGraph* g = scan_fits((fnm+sets[i]+fileEnd).Data(), category);
    setSilent_ = tmp;
    g->SetName(Form("graph%s",sets[i].Data()));
    scores[i] = g;
    g->SetMarkerColor(colors[i]);
    g->SetLineColor(colors[i]);
  }
  if(!setSilent_) gROOT->SetBatch(kFALSE);
  TString title = "MVA Score vs  #Delta#chi^{2}";
  if((fnm+fileEnd).Contains("no_15"))
    title += " (Background only)";
  title += "; MVA Score; #Delta#chi^{2}";
  scores[0]->SetTitle(title.Data());
  
  TCanvas* c = new TCanvas("c_scores", "Score Results", 800, 500);
  TLegend* leg = new TLegend(0.1, 0.6, 0.38, 0.9);
  leg->SetFillStyle(0);
  double m = 0.1;
  for(int i = 0; i < numSets; ++i) {
    m = max(m, scores[i]->GetMaximum());
    if(i == 0)
      scores[i]->Draw("APL");
    else
      scores[i]->Draw("PL");
    if((fnm+fileEnd).Contains("no_15"))
      leg->AddEntry(scores[i], setNames_nosignal[i].Data());
    else
      leg->AddEntry(scores[i], setNames_signal[i].Data());
  }

  if(m > 150.)
    c->SetLogy();
  
  scores[0]->GetYaxis()->SetRangeUser(min(max(-m,-1.),0.), max(m, 0.));
  leg->Draw();
  c->SetGrid();
  if(print_) {
    fnm += fileEnd;
    TString out = "figures/bkg";
    if(fnm.Contains("_2_"))
      out += "_2_";
    else if(fnm.Contains("_3_"))
      out += "_3_";
    else if(fnm.Contains("_6_"))
      out += "_6_";
    else if(fnm.Contains("_11_"))
      out += "_11_";
    else if(fnm.Contains("_12_"))
      out += "_12_";
    else if(fnm.Contains("_13_"))
      out += "_13_";
    else
      out += "_X_";
    if(fnm.Contains("noCat_15") || fnm.Contains("no_15"))
      out += "noCat_15_";
    out += var_;
    out += "_";
    out += MVA_var_;
    out += "_set_fit_scores.png";
    c->Print(out.Data());
  }
}
