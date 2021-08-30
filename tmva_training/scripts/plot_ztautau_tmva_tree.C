
#include <map>
#include <string>



namespace {
  enum{kMLP, kMLP_MM, kBDT, kBDTRT};
  int MVA_ = kBDT;
  TString var_ = "lepm"; //which variable to plot
  double xMin_ = 50.; //plotting domain
  double xMax_ = 170.;
  int    bins_ = 100; //number of histogram bins
  double yMin_ = 0.; //plotting domain for 2D
  double yMax_ = 100.;
  int    binsy_ = 100; //number of histogram bins
  int    print_ = 0; //print canvases
  int    setSilent_ = 0; //sets to batch mode for canvas printing
  bool   doGIF_ = false; //change name printing to have counter
  int    gifCount_ = 0; //gif image number
  double lum_ = 1.; //luminosity to scale by, 1 by default since now in weights
  bool   correctForScale_ = true; //scale test sample to match expectations for total dataset (1./(1.-fracTrain))
  int    verbose_ = 1; //verbosity level
  double scale_signal_ = 250.; //scale signal to be visible
  int    max_categories_ = 70; //number of categories to check
}

void set_config(TString var, double xmin, double xmax, int xbins) {
  var_ = var; xMin_ = xmin; xMax_ = xmax; bins_ = xbins;
}

TString get_label_from_category(int category) {
  switch(category) {
  case 1: case 2: case 24: case 25: case 26: case 27: case 49: case 50: case 51:
    return "t#bar{t}";
  case 3: case 23: case 28: case 48: case 52:
    return "Z+Jets";
  case 4: case 5: case 29: case 30: case 53: case 54:
    return "Single top";
  case 6: case 22: case 31: case 32: case 55:
    return "Wl#nu";
  case 7: case 8: case 17: case 18: case 33: case 34:
  case 43: case 44: case 56: case 57: case 66: case 67:
    return "Diboson";
  case 15: case 16: case 41: case 42: case 64: case 65: //data
    return "QCD";
  case 9: case 35: case 58:
    return "Z->e#tau";
  case 10: case 36: case 59:
    return "Z->#mu#tau";
  case 11: case 37: case 60:
    return "Z->e#mu";
  case 12: case 38: case 61:
    return "H->e#tau";
  case 13: case 39: case 62:
    return "H->#mu#tau";
  case 14: case 40: case 63:
    return "H->e#mu";
  default:
    return "UNKNOWN";
  }
}

//get file name from configuration
TString get_file_name(TString selection = "zmutau",
                      vector<int> years = {2016, 2017, 2018}, int set = 8) {
  bool isHiggs = selection.Contains("h");
  selection.ReplaceAll("h", "");
  selection.ReplaceAll("z", "");
  TString name = "training_background_ztautau_";
  name += (isHiggs) ? "higgs_nano_" : "Z0_nano_";
  name += selection + "_";
  for(int year : years) {
    name += year;
    name += "_";
  }
  if(selection == "mutau")        set += ZTauTauHistMaker::kMuTau;
  else if(selection == "etau")    set += ZTauTauHistMaker::kETau;
  else if(selection == "emu")     set += ZTauTauHistMaker::kEMu;
  else if(selection == "mutau_e") set += ZTauTauHistMaker::kEMu;
  else if(selection == "etau_mu") set += ZTauTauHistMaker::kEMu;
  name += set;
  if(verbose_ > 0) cout << "Using file base name " << name.Data() << endl;
  return name;
}

//assumes CLs
double get_limit(double nbackground, double nsignal, double confidence = 0.95) {
  double val = 0.;
  if(nbackground > 0. && nsignal > 0.) {
    double p = 1.-confidence; //confidence limit goal
    double tolerance = 0.001; //precision to achieve for goal confidence level
    double scale = sqrt(nbackground)*1.93/nsignal; //scale signal until achieve tolerance (start with best guess)
    if(nbackground < 0.1) //~0 events
      scale = 3./nsignal; //need ~3 expected for 0 seen
    else if(nbackground < 1.)
      scale = 4./nsignal;
    else if(nbackground < 5.)
      scale = (3.+sqrt(nbackground)*1.93)/nsignal;

    double denom = ROOT::Math::poisson_cdf((int) (nbackground), nbackground); //normalizing for CLs
    int counts = 0;
    do { //guess scale factors until close to limit goal
      ++counts;
      //confidence limit at this value
      val = ROOT::Math::poisson_cdf((int) (nbackground), nbackground + nsignal*scale) / denom;
      if(verbose_ > 5) printf("Loop for limit: val = %.3e scale = %.3e\n", val, scale);
      if(abs(val-p) > tolerance) //only update if still not succeeding
        scale = (val/p < 4.) ? (1.-p)/(1.-val)*scale : scale*(0.9); //if far slowly approach
    } while(abs(val - p) > tolerance && counts < 100);
    if(counts == 100)
      cout << "Error! Maximum attempts reached in limit calculation!\n" << endl;
    val = 1./scale;
  }
  return val;
}

pair<double,double> get_train_scales(TTree* test_tree, TTree* train_tree) {
  double scaleBkgTrain = 1.;
  double scaleSigTrain = 1.;
  if(verbose_ > 1)
    cout << "get_train_scales: getting background class" << endl;
  test_tree-> Draw(Form("%s>>htest_-1" , var_.Data()),"(classID==0)");//background class ID
  TH1F* htesttmp  = (TH1F*) gDirectory->Get("htest_-1");
  train_tree->Draw(Form("%s>>htrain_-1", var_.Data()),"(classID==0)");
  TH1F* htraintmp = (TH1F*) gDirectory->Get("htrain_-1");
  if(!htesttmp || !htraintmp) {
    cout << "ERROR! Histograms for getting scale factors not found!\n";
    return pair<double,double>(1.,1.);
  }
  scaleBkgTrain = htesttmp->Integral()/htraintmp->Integral();
  delete htraintmp;
  delete htesttmp;
  if(verbose_ > 1)
    cout << "get_train_scales: getting signal class" << endl;
  test_tree-> Draw(Form("%s>>htest_-1" , var_.Data()),"(classID==1)");//signal class ID
  train_tree->Draw(Form("%s>>htrain_-1", var_.Data()),"(classID==1)");
  htesttmp  = (TH1F*) gDirectory->Get("htest_-1");
  htraintmp = (TH1F*) gDirectory->Get("htrain_-1");
  if(!htesttmp || !htraintmp) {
    cout << "ERROR! Histograms for getting scale factors not found!\n";
    return pair<double,double>(1.,1.);
  }
  scaleSigTrain = htesttmp->Integral()/htraintmp->Integral();
  delete htraintmp;
  delete htesttmp;
  return pair<double,double>(scaleBkgTrain,scaleSigTrain);
}

int plot_tmva_tree(const char* file = "training_background_ztautau_Z0_nano_mutau_2016_8",
                   double mva_cut = -1., int category = -1) {

  if(setSilent_) gROOT->SetBatch(kTRUE);
  else gROOT->SetBatch(kFALSE);

  //assume file path follows standard format
  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";

  TFile* f = TFile::Open(fname.Data(),"READ");
  if(!f)
    return 1;

  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }

  TH1F* htest  = new TH1F("htest" ,"Test" ,bins_,xMin_,xMax_);
  TH1F* htrain = new TH1F("htrain","Train",bins_,xMin_,xMax_);

  TString mva_var;
  if(MVA_ == kMLP_MM)
    mva_var = "MLP_MM";
  else if(MVA_ == kMLP)
    mva_var = "MLP";
  else if(MVA_ == kBDT)
    mva_var = "BDT";
  else if(MVA_ == kBDTRT)
    mva_var = "BDTRT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 3;
  }

  TString cut;
  if(fname.Contains("mock"))
    cut = Form("genweight*((%s>=%.5f)",mva_var.Data(),mva_cut); //don't weight data/mock data
  else
    cut = Form("fulleventweightlum*((%s>=%.5f)",mva_var.Data(),mva_cut);

  if(category > -1) cut += Form("&&(eventcategory == %i)",category);
  cut += ")";
  test_tree-> Draw(Form("%s>>htest" , var_.Data()), cut.Data());
  train_tree->Draw(Form("%s>>htrain", var_.Data()), cut.Data());
  if(htest->Integral() > 0. && htrain->Integral() > 0.) {
    double scale_train = htest->Integral()/htrain->Integral();
    printf("Scaling training sample by %.4f\n", scale_train);
    htrain->Scale(scale_train);
  }

  TCanvas* c = new TCanvas(("c_"+var_).Data(),(var_+" Canvas").Data(), 900, 500);
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
  htrain->SetTitle(Form("%s for %s >= %.5f",var_.Data(),mva_var.Data(),mva_cut));
  htrain->SetXTitle(Form("%s",var_.Data()));
  htrain->SetYTitle("Entries / 1 GeV");

  if(print_) {
    TString fnm = fname;
    fnm.ReplaceAll("training_background_ztautau_", "");
    c->Print(Form("figures/%s_hist_%s_%s_%.4f.png", fnm.Data(), var_.Data(), mva_var.Data(), mva_cut));
  }
  TObject* o = (gDirectory->Get("c1"));
  if(o) delete o;
  o = (gDirectory->Get("c1"));
  if(o) delete o;
  return 0;
}



int stack_tmva_tree(const char* file = "training_background_ztautau_Z0_nano_mutau_2016_8",
                    double mva_cut = -1.,
                    int plot_train = 0)  {

  if(setSilent_) gROOT->SetBatch(kTRUE);

  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";

  TFile* f = TFile::Open(fname.Data(),"READ");
  if(!f) {
    printf("File not found\n");
    return 1;
  } else if(verbose_ > 0)
    printf("Opened the file\n");

  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  } else if(verbose_ > 0)
    printf("Retrieved both trees\n");

  std::map<TString, int> colors;
  colors["Z+Jets"] = kRed-7;
  colors["Wl#nu"] = kGreen-7;
  colors["t#bar{t}"] = kYellow-7;
  colors["Single top"] = kCyan-7;
  colors["Diboson"]= kViolet+6;
  colors["Z->e#tau"] = kBlue;
  colors["Z->#mu#tau"] = kBlue;
  colors["Z->e#mu"] = kBlue;
  colors["H->e#tau"] = kBlue;
  colors["H->#mu#tau"] = kBlue;
  colors["H->e#mu"] = kBlue;
  colors["Data"]   = kBlack;
  colors["QCD"]   = kOrange;
  colors["UNKNOWN"]   = kRed;
  std::map<TString, int> isSignal;

  //initialize a map of histogram titles to index in histogram array to use
  std::map<TString, int> indexes;
  unsigned ndatasets = max_categories_;
  for(unsigned index = 0; index < ndatasets ; ++index)
    indexes[get_label_from_category(index+1)] = -1;

  //arrays of histograms for plotting
  TH1F* htest[ndatasets];
  TH1F* htrain[ndatasets];
  for(unsigned index = 0; index < ndatasets; ++index) {
    htest[index] = 0;
    htrain[index] = 0;
  }

  TString mva_var;
  if(MVA_ == kMLP_MM)
    mva_var = "MLP_MM";
  else if(MVA_ == kMLP)
    mva_var = "MLP";
  else if(MVA_ == kBDT)
    mva_var = "BDT";
  else if(MVA_ == kBDTRT)
    mva_var = "BDTRT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 3;
  }

  //assume all backgrounds use the same training fraction, and the same for all signals
  pair<double,double> train_scales = get_train_scales(test_tree, train_tree);
  double scaleBkgTrain = train_scales.first;
  double scaleSigTrain = train_scales.second;
  if(verbose_ > 1)
    printf("Retrieved the training/testing fractions (bkg=%.3f, sig=%.3f)\n", scaleBkgTrain, scaleSigTrain);

  if(plot_train > 0)
    printf("Scaling background by %.2f and signal by %.2f for training trees\n", scaleBkgTrain, scaleSigTrain);

  TH1F* hSigTest;
  for(int index = 0; index <  ndatasets; ++index) {
    if(verbose_ > 1)
      cout << "Filling histogram " << index << ": name = " << get_label_from_category(index+1).Data()<< endl;
    int category = index + 1; //current definition of category is offset by 1

    htest[index] = new TH1F(Form("htest_%i",index),   Form("Test %s" , get_label_from_category(index+1).Data()), bins_, xMin_, xMax_);
    if(plot_train > 0) htrain[index] = new TH1F(Form("htrain_%i",index), Form("Train %s",  get_label_from_category(index+1).Data()), bins_, xMin_, xMax_);
    TString cut;
    if(fname.Contains("mock"))
      cut = Form("genweight*((%s>=%.5f)",mva_var.Data(),mva_cut);
    else
      cut = Form("fulleventweightlum*((%s>=%.5f)",mva_var.Data(),mva_cut);

    cut += Form("&&(eventcategory == %i))",category);

    test_tree->Draw(Form("%s>>htest_%i" , var_.Data(), index), cut.Data());
    if(verbose_ > 1)
      cout << "Test histogram has " << htest[index]->GetEntries() << " entries\n";
    //check if this is signal by asking if class ID is 0 or 1
    hSigTest = new TH1F("hSigTest", "hSigTest", 2, 0., 2.);
    if(verbose_ > 1)
      cout << "Performing signal test: ";
    test_tree->Draw("classID>>hSigTest",Form("(eventcategory==%i)",category));
    bool isSig = hSigTest->GetMean() > 0.5;
    isSignal[get_label_from_category(index+1)] = isSig;//save signal label
    if(verbose_ > 1)
      cout  << isSig << endl;
    delete hSigTest;

    if(plot_train > 0) train_tree->Draw(Form("%s>>htrain_%i", var_.Data(), index), cut.Data());
    if(plot_train > 0 && verbose_ > 1)
      cout << "Test histogram has " << htest[index]->GetEntries() << " entries\n";
    TString name = get_label_from_category(index+1);
    if(plot_train > 0) htrain[index]->Scale(((isSig) ? scaleSigTrain : scaleBkgTrain));

    if(correctForScale_) {//re-scale test/train sample to each look like entire background expectation
      // x = a/b; a+b=1=b*(x+1)-->b=1/(x+1), s = 1 - 1/(x+1) = x/(x+1); -->1/s = (x+1)/x
      double sig_correction = (1.+scaleSigTrain)/scaleSigTrain;
      double bkg_correction = (1.+scaleBkgTrain)/scaleBkgTrain;
      if(verbose_>1)
        printf("Scaling by training compensation (bkg=%.4f,sig=%.4f)\n", bkg_correction, sig_correction);

      htest[index]->Scale((isSig) ? sig_correction : bkg_correction);
      if(plot_train > 0) htrain[index]->Scale((isSig) ? sig_correction : bkg_correction);
    }

    if(lum_ != 1.) {
      htest[index]->Scale(lum_);
      if(plot_train > 0) htrain[index]->Scale(lum_);
    }
    int index_map = indexes[name];
    if(index_map > -1) { //already mapped
      htest[index_map]->Add(htest[index]);
      if(plot_train > 0) htrain[index_map]->Add(htrain[index]);
    } else //map this index
      indexes[name] = index;

    htest[index]->SetLineColor(colors[get_label_from_category(index+1)]);
    if(!isSignal[name]) htest[index]->SetFillColor(colors[get_label_from_category(index+1)]);
    else htest[index]->SetLineWidth(3);

    // htest[index]->SetFillStyle(3001);
    if(plot_train > 0) htrain[index]->SetMarkerColor(colors[get_label_from_category(index+1)]-1);
    if(plot_train > 0) htrain[index]->SetMarkerStyle(20);
    if(plot_train > 0) htrain[index]->SetLineColor(colors[ get_label_from_category(index+1)]-1);
  }
  TObject* o = (gDirectory->Get("c1"));
  if(o) delete o;

  THStack* hstacktest  = new THStack("teststack" , "Testing Stack");
  THStack* hstacktrain = new THStack("trainstack", "Training Stack");
  vector<TH1F*> hsignals;
  for(unsigned index = 0; index <  ndatasets; ++index) {
    TString name = get_label_from_category(index+1);
    if(verbose_ > 1)
      cout << "Looking at histogram " << index << ", name = " << name.Data() << endl
           << "index map = " << indexes[name] << ", issignal = " << isSignal[name] << endl;
    if(indexes[name] != index)
      continue;
    htest[index] ->SetTitle(Form("%s #scale[0.5]{#int} = %.2e", htest[index] ->GetTitle() , htest[index]->Integral()));
    if(isSignal[name]) htest[index]->Scale(scale_signal_);
    if(plot_train > 0) {
      htrain[index]->SetTitle(Form("%s #scale[0.5]{#int} = %.2e", htrain[index]->GetTitle(), htrain[index]->Integral()));
      if(isSignal[name]) htrain[index]->Scale(scale_signal_);
    }
    if(isSignal[name] && htest[index]->GetEntries() > 0)
      hsignals.push_back(htest[index]);
    else if(htest[index]->GetEntries() > 0) hstacktest->Add(htest[index]);

    if(plot_train > 0 && htrain[index]->GetEntries() > 0) hstacktrain->Add(htrain[index]);
    if(verbose_ > 1)
      cout << "-> Added the histogram to the stack!\n";
  }

  TCanvas* c = new TCanvas("c_stack","Stack Canvas", 1200, 800);
  if(plot_train > 0 && hstacktrain->GetNhists() > 0) {
    hstacktrain->Draw("E noclear");
    if(hstacktest->GetNhists() > 0)
      hstacktest->Draw("sames hist noclear");
  } else if(hstacktest->GetNhists() > 0)
    hstacktest->Draw("hist noclear");
  else
    return 10;
  double m = hstacktest->GetMaximum();
  if(plot_train > 0 && hstacktrain->GetNhists() > 0) m = max(m, hstacktrain->GetMaximum());

  for(unsigned index = 0; index < hsignals.size(); ++index) {
    hsignals[index]->Draw("hist sames");
    m = max(m, hsignals[index]->GetMaximum());
  }

  c->SetGridx();
  c->SetGridy();
  c->BuildLegend();
  c->Update();

  if(plot_train > 0 && hstacktrain->GetNhists() > 0) {
    hstacktrain->SetMaximum(1.2*m);
    hstacktrain->SetTitle(Form("%s for %s >= %.5f",var_.Data(),mva_var.Data(),mva_cut));
    hstacktrain->GetXaxis()->SetTitle(Form("%s",var_.Data()));
    // hstacktrain->GetYaxis()->SetTitle(Form("Entries / %.1f GeV",hstacktrain->GetXaxis()->GetBinWidth(1)));
  } else  if(hstacktest->GetNhists() > 0) {
    hstacktest->SetTitle(Form("%s for %s >= %.5f",var_.Data(),mva_var.Data(),mva_cut));
    hstacktest->SetMaximum(1.2*m);
    hstacktest->GetXaxis()->SetTitle(Form("%s",var_.Data()));
    // hstacktest->GetYaxis()->SetTitle(Form("Entries / %.1f GeV",hstacktest->GetXaxis()->GetBinWidth(1)));
  }
  if(print_) {
    TString fnm = file;
    fnm.ReplaceAll("training_background_ztautau_", "");
    fnm.ReplaceAll(".", "d");
    fnm += Form("_stack_%s_%s", var_.Data(), mva_var.Data());
    gSystem->Exec(Form("[ ! -d figures/%s ] && mkdir -p figures/%s", fnm.Data(), fnm.Data()));
    if(!doGIF_)
      c->Print(Form("figures/%s/%s_%.4f.png", fnm.Data(), fnm.Data(), mva_cut));
    else {
      c->Print(Form("figures/%s/%s-%03i.png", fnm.Data(), fnm.Data(), gifCount_));
      ++gifCount_;
    }
  }
  if(((TH1F*) hstacktest->GetStack()->Last())->Integral() <= 0.) return 10;
  gStyle->SetOptStat(0);
  return 0;
}

Int_t plot_limit_gain(const char* file = "training_background_ztautau_higgs_mutau_8",
                      TString cut = "", //apply some cut
                      int plot_train = 0, double xmin = -1, double xmax = 1., int max_steps = 100) {

  // gROOT->SetBatch(kTRUE);

  //assume file path follows standard format
  TString fname = file;
  fname = "../" + fname;
  fname += "/";
  fname += file;
  fname += ".root";

  TFile* f = TFile::Open(fname.Data(),"READ");
  if(!f)
    return 1;

  TTree *test_tree  = (TTree*) f->Get("TestTree");
  TTree *train_tree = (TTree*) f->Get("TrainTree");
  if(!test_tree || !train_tree) {
    printf("Trees not found\n");
    return 2;
  }

  TH1F* htest  = new TH1F("htest" ,"Test" ,bins_,xMin_,xMax_);
  TH1F* htrain = new TH1F("htrain","Train",bins_,xMin_,xMax_);

  TString mva_var;
  if(MVA_ == kMLP_MM)
    mva_var = "MLP_MM";
  else if(MVA_ == kMLP)
    mva_var = "MLP";
  else if(MVA_ == kBDT)
    mva_var = "BDT";
  else if(MVA_ == kBDTRT)
    mva_var = "BDTRT";
  else {
    printf("MVA_ value doesn't correspond to known MVA\n");
    return 3;
  }

  TString cut_start;
  if(fname.Contains("mock"))
    cut_start = "genweight*("; //don't weight data/mock data
  else
    cut_start = "fulleventweightlum*(";
  if(cut != "") cut_start += cut + "&&";

  cout << "Initializing mva histograms" << endl;
  //Initialize histograms with binning to match the MVA cut steps
  TString cut_bkg = cut_start + "classID==0)";
  TString cut_sig = cut_start + "classID==1)";
  TH1F* hsig_test = new TH1F("hsig_test", "hsig_test", max_steps, xmin, xmax);
  test_tree->Draw((mva_var+">>hsig_test").Data(), cut_sig.Data());
  TH1F* hsig_train = new TH1F("hsig_train", "hsig_train", max_steps, xmin, xmax);
  train_tree->Draw((mva_var+">>hsig_train").Data(), cut_sig.Data());
  TH1F* hbkg_test = new TH1F("hbkg_test", "hbkg_test", max_steps, xmin, xmax);
  test_tree->Draw((mva_var+">>hbkg_test").Data(), cut_bkg.Data());
  TH1F* hbkg_train = new TH1F("hbkg_train", "hbkg_train", max_steps, xmin, xmax);
  train_tree->Draw((mva_var+">>hbkg_train").Data(), cut_bkg.Data());


  pair<double,double> trainScales = get_train_scales(test_tree, train_tree);
  //returns test/train = x --> test + train = 1 = test + test/x = test(1+1/x) --> 1/test = 1 + 1/x
  double sig_scale = 1.+ 1./trainScales.second;
  double bkg_scale = 1.+ 1./trainScales.first;
  sig_scale *= lum_;
  bkg_scale *= lum_;
  hsig_test->Scale(sig_scale);
  hbkg_test->Scale(bkg_scale);

  double confidence = 0.95;
  //for TGraph
  double xs        [max_steps+1];
  double sigs_test [max_steps+1];
  double bkgs_test [max_steps+1];
  double sigs_train[max_steps+1];
  double bkgs_train[max_steps+1];
  double lims_test [max_steps+1];
  double lims_train[max_steps+1];

  //initialize the values
  for(int step = 0; step <= max_steps; ++step) {
    double mva_val = xmin + step*(xmax-xmin)/max_steps;
    xs[step] = mva_val;
    lims_test[step] = 0.;
    lims_train[step] = 0.;
  }
  //find the limits
  double p(0.05), tolerance(0.001);
  Significances significances(p, tolerance, true, 10); //for calculating limit gain
  for(int step = 0; step <= max_steps; ++step) {
    int bin = step + 1;
    double mva_val = xs[step];
    double n_sig_test = hsig_test->Integral(bin, max_steps) + hsig_test->GetBinContent(max_steps+1);
    double n_bkg_test = hbkg_test->Integral(bin, max_steps) + hbkg_test->GetBinContent(max_steps+1);
    if(n_sig_test <= 0. || n_bkg_test <= 0.) break;
    if(verbose_>1)
      cout << "Limit step " << step << " nbkg = " << n_bkg_test << " nsig = " << n_sig_test << endl;
    double val(-1);
    double limit = significances.LimitGain(n_sig_test, n_bkg_test, val);
    if(limit < 0.) limit = 0.;
    lims_test[step] = limit;
    if(verbose_>1)
      cout << "--limit = " << limit << endl;
  }
  TCanvas* c = new TCanvas("c_limit", "c_limit", 800, 500);
  TGraph* g = new TGraph(max_steps+1, xs, lims_test);
  g->SetLineWidth(3);
  g->SetLineColor(kBlue);
  if(!setSilent_) gROOT->SetBatch(kFALSE);
  g->Draw();
  g->GetYaxis()->SetRangeUser(0., 4.);
  g->GetXaxis()->SetRangeUser(-1., 1.);
  g->SetTitle("Limit gain vs MVA score; MVA score; Limit gain");
  c->SetGrid();
  TLine* line = new TLine(-1., 1., 1., 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw();
  if(print_) {
    TString fnm = file;
    fnm.ReplaceAll("training_background_ztautau_", "");
    c->Print(Form("figures/%s_limit_%s.png", fnm.Data(), mva_var.Data()));
  }
  return 0;
}

//print the distributions in steps of the MVA cut, to make a gif of the cut effect
int print_gif_figures(const char* file = "training_background_ztautau_Z0_nano_mutau_2016_8",
                      double mva_start = -1., double mva_end = 1., int mva_steps = 100,
                      int plot_train = 0) {
  int status(0);
  //configure to print images, don't show plots
  setSilent_ = true;
  print_ = true;

  //write out an image of the BDT score distribution for reference
  TString var_prev = var_;
  double xmin_prev = xMin_;
  double xmax_prev = xMax_;
  var_ = "BDT";
  xMin_ = -1.;
  xMax_ = 1.5;
  status += stack_tmva_tree(file, -1., plot_train);
  var_ = var_prev;
  xMin_ = xmin_prev;
  xMax_ = xmax_prev;
  doGIF_ = true;
  gifCount_ = 0;

  //loop through MVA cut values, printing the distribution for each
  for(int step = 0; step <= mva_steps; ++step) {
    double mva_cut = mva_start + step*(mva_end - mva_start)/mva_steps;
    status += stack_tmva_tree(file, mva_cut, plot_train);
    if(status) break;
  }

  return status;
}
