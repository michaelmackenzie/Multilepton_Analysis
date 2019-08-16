int     MVA_ = 2; //0 for NN, 1 for BDT, 2 for BDTRT
TString var_ = "lepM"; //which variable to plot
double  xMin_ = 10.; //plotting domain
double  xMax_ = 70.;
int     bins_ = 30; //number of histogram bins
double  fitMin_ = 12.; //fitting domain
double  fitMax_ = 70.;
Fitter* fitter_ = 0; //object to make TF1s to fit with
int     fitset_ = -1; //set of fit parameters to use in fitter, -1 to on the fly determine it
double  sigmaRange_ = 100.; //sigmas to allow on parameter fit range
int     plot_train_ = 0; //whether or not to show training data in fit
int     print_ = 0; //print canvases
int     setSilent_ = 0; //sets to batch mode for canvas printing
int     ignoreSignal_ = 0; //whether or not to ignore signals
vector<int> signals_ = {15,16,17,18,19}; //signal IDs
int     scan_steps_ = 41; //number of points to take when scanning mva score

THStack* get_tmva_stack(const char* file, double mva_cut = -1., int doTrain = 0)  {

  TFile* f = new TFile(file,"READ");
  if(!f) {
    printf("File not found\n");
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
  for(int i = 0; i < 100; ++i) {
    h[i] = 0;
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
  
  double scaleTrain = 1.;
  if(doTrain > 0) {
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
  double integral = 0.;
  if(doTrain > 0 && scaleTrain > 0.) printf("Scaling training histograms by %.3f\n", scaleTrain);
  for(int i = 0; i <  sizeof(names)/sizeof(*names); ++i) {
    bool skip = false;
    if(ignoreSignal_ > 0) {
      for(int j = 0; j < signals_.size(); ++j) {
	if(i == signals_[j]) skip = true;
      }
    }
    if(skip) continue;
    if(doTrain == 0) h[i] = new TH1F(Form("htest_%i",i),   Form("Test %s" , names[i]), bins_, xMin_, xMax_);
    else h[i] = new TH1F(Form("htrain_%i",i), Form("Train %s", names[i]), bins_, xMin_, xMax_);
    TString cut;
    if(fname.Contains("mock"))
      cut = Form("genWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    else
      cut = Form("fullEventWeight*((%s>=%.5f)",mva_var.Data(),mva_cut);

    cut += Form("&&(eventCategory == %i))",i);
    
    if(doTrain == 0) test_tree-> Draw(Form("%s>>htest_%i" , var_.Data(), i), cut.Data());
    else             train_tree->Draw(Form("%s>>htrain_%i", var_.Data(), i), cut.Data());
    TString name = names[i];
    if(doTrain > 0 && scaleTrain > 0.) h[i]->Scale(scaleTrain);
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
    if(doTrain == 0) {
      h[i]->SetLineColor(colors[i]);
      h[i]->SetFillColor(colors[i]);
      h[i]->SetFillStyle(3001);
    } else {
      h[i]->SetMarkerColor(colors[i]-5);
      h[i]->SetMarkerStyle(20);
      h[i]->SetLineColor(colors[i]-5);
    }
    integral += h[i]->Integral();
  }
  THStack* hstack;

  if(integral <= 0) return NULL;
  
  if(doTrain == 0) hstack  = new THStack("teststack" , "Testing Stack");
  else             hstack = new THStack("trainstack", "Training Stack");
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
    
    if(h[i]->GetEntries() > 0) hstack->Add(h[i]);
  }
  
  TObject* o = (gDirectory->Get("c1"));
  if(o) delete o;
  delete f;
  return hstack;
}




int fit_tmva_tree(const char* file = "../CWoLa_training_background_3.root", double mva_cut = 0.,
		  int category = -1) {

  if(setSilent_) gROOT->SetBatch(kTRUE); //doesn't draw canvases, so faster to make plots
  else gROOT->SetBatch(kFALSE);
  gStyle->SetTitleW(0.8f);

  if(fitter_ == 0) {
    fitter_ = new Fitter();
  }
  THStack* hstack = get_tmva_stack(file, mva_cut, 0);

  if(!hstack) {
    printf("NULL stack returned, exiting\n");
    return 1;
  }
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
    return 2;
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
  Int_t bins = h->GetNbinsX();
  double hmin = h->GetXaxis()->GetXmin();
  double hmax = h->GetXaxis()->GetXmax();
  hmax = min(hmax,fitter_->fitMax_);
  hmin = max(hmin,fitter_->fitMin_);
  f->SetRange(hmin,hmax);
  fSB->SetRange(hmin,hmax);

  TString fitStr = "R S";
  if(setSilent_) fitStr += " Q";
  TFitResultPtr res = h->Fit(f,fitStr.Data());
  double c1 = res->Chi2();
  int dof1 = res->Ndf();
  double prob1 = ROOT::Math::chisquared_cdf_c(c1,dof1);
  res = h2->Fit(fSB,fitStr.Data());
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
  printf("Chi^2 / DoF for each are %.4f / %i p = %.4e (Background Only) and %.4f / %i p = %.4e (Signal + Bkgd), p2/p1 = %.4e\n",
	 c1, dof1, prob1, c2, dof2, prob2, prob2/prob1);
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
    c->Print(Form("figures/%s_fit_%s_%s_%s%i.png", fnm.Data(), var_.Data(), mva_var.Data(), leadZeros.Data(),gifnum));
  }
  TObject* o = (gDirectory->Get("c1"));
  delete o;
  return 0;
}

int scan_fits(const char* file = "../CWoLa_training_background_3.root", int category = -1) {

  double mva_min = -1.;
  double mva_max =  1.;
  if(MVA_ == 0)
    mva_min = 0.;

  if(!setSilent_)
    printf("Warning! Non-silent fits may take longer\n");
  if(!print_)
    printf("Warning! Not printing resulting canvases\n");

  double step_size = (mva_max-mva_min)/(scan_steps_-1); // -1 so end value is taken

  for(int i = 0; i <= scan_steps_; ++i)
    fit_tmva_tree(file, i*step_size + mva_min, category);
  return 0;
}
