/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Config.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
// #include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#endif

// #include "../utils/TrkQualInit.cc"

TString selection_ = "zmutau"; 
Int_t split_trees_ = 0; //split training/testing using tree defined samples
Int_t trkqual_version_ = 3; //version of variables used in trkqual training
Int_t use_njets_ = 1; //whether or not to train using njets, don't use if jet binned categories
Int_t use_ht_ = 1; //whether or not to train using hT, don't use if 0 jet bin category
Int_t use_nbjets_ = 0; //whether or not to train using nbjets, = id+3*ptcut with id = 1,2,3 for tight, medium, loose and pt = 0,1,2 for >30, >25, >20

bool dryrun_ = false; //don't actually train
bool inTrainDir_ = false; //track if we've gone into a training directory
bool multiTrainings_ = false; //if doing multiple trainings, leave directory at the end of the training

//Train MVA to separate the given signal from the given background
int TrainTrkQual(TTree* signal, TTree* background, const char* tname = "TrkQual",
		 vector<int> signals = {0},
		 vector<int> ignore = {}) {

  // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
  // if you use your private .rootrc, or run from a different directory, please copy the
  // corresponding lines from .rootrc

  // methods to be processed can be given as an argument; use format:
  //
  // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
  //
  // if you like to use a method via the plugin mechanism, we recommend using
  //
  // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
  // (an example is given for using the BDT as plugin (see below),
  // but of course the real application is when you write your own
  // method based)


  //---------------------------------------------------------------
  // This loads the library
  TMVA::Tools::Instance();

  // Default MVA methods to be trained + tested
  std::map<std::string,int> Use;

  // --- Cut optimisation
  Use["Cuts"]            = 0;
  Use["CutsD"]           = 0;
  Use["CutsPCA"]         = 0;
  Use["CutsGA"]          = 0;
  Use["CutsSA"]          = 0;
  // 
  // --- 1-dimensional likelihood ("naive Bayes estimator")
  Use["Likelihood"]      = 0;
  Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
  Use["LikelihoodPCA"]   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
  Use["LikelihoodKDE"]   = 0;
  Use["LikelihoodMIX"]   = 0;
  //
  // --- Mutidimensional likelihood and Nearest-Neighbour methods
  Use["PDERS"]           = 0;
  Use["PDERSD"]          = 0;
  Use["PDERSPCA"]        = 0;
  Use["PDEFoam"]         = 0;
  Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
  Use["KNN"]             = 0; // k-nearest neighbour method
  //
  // --- Linear Discriminant Analysis
  Use["LD"]              = 0; // Linear Discriminant identical to Fisher
  Use["Fisher"]          = 0;
  Use["FisherG"]         = 0;
  Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
  Use["HMatrix"]         = 0;
  //
  // --- Function Discriminant analysis
  Use["FDA_GA"]          = 0; // minimisation of user-defined function using Genetics Algorithm
  Use["FDA_SA"]          = 0;
  Use["FDA_MC"]          = 0;
  Use["FDA_MT"]          = 0;
  Use["FDA_GAMT"]        = 0;
  Use["FDA_MCMT"]        = 0;
  //
  // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
  Use["MLP"]             = 0; // Recommended ANN
  Use["MLP_MM"]          = 0; // ANN with altered settings
  Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
  Use["MLPBNN"]          = 0; // Recommended ANN with BFGS training method and bayesian regulator
  Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
  Use["TMlpANN"]         = 0; // ROOT's own ANN

  //
  // --- Support Vector Machine 
  Use["SVM"]             = 0;
  // 
  // --- Boosted Decision Trees
  Use["BDT"]             = 1; // uses Adaptive Boost
  Use["BDTG"]            = 0; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTRT"]           = 0; // uses randomized trees (Random Forest Technique) with Bagging
  Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
  Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
  // 
  // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
  Use["RuleFit"]         = 0;
  // ---------------------------------------------------------------

  std::cout << std::endl;
  std::cout << "==> Start TMVAClassification" << std::endl;

  // --------------------------------------------------------------------------------------------------

  // --- Here the preparation phase begins
  //if in a past training directory, leave it
  if(inTrainDir_) {
    gSystem->cd("..");
    inTrainDir_ = false;
  }
  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  TString outfilename(tname);
  gSystem->Exec(Form("mkdir %s", outfilename.Data()));
  gSystem->cd(outfilename.Data());
  inTrainDir_ = true;
  
  outfilename += ".root";
  TFile* outputFile = TFile::Open( outfilename, "RECREATE" );

  // Create the factory object. Later you can choose the methods
  // whose performance you'd like to investigate. The factory is 
  // the only TMVA object you have to interact with
  //
  // The first argument is the base of the name of all the
  // weightfiles in the directory weight/
  //
  // The second argument is the output file for the training results
  // All TMVA output can be suppressed by removing the "!" (not) in
  // front of the "Silent" argument in the option string
  // TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
  //     "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
      "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );


  //data loader to handle the variables, spectators, signal trees, and background trees
  TString outFolder;
  outFolder = "tmva_";
  outFolder += tname;
  //  TMVA::DataLoader* dataloader = new TMVA::DataLoader(outFolder);


  factory->SetBackgroundWeightExpression("fulleventweight");
  factory->SetSignalWeightExpression("fulleventweight");

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;

  TString dirname(tname);
  dirname += "_Weights";
  (TMVA::gConfig().GetIONames()).fWeightFileDir = dirname.Data();
  cout << "--- Set weight directory " << dirname.Data() << endl;

  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
  
  TrkQualInit trkqualinit(trkqual_version_, use_njets_); //version number input
  trkqualinit.InitializeVariables(*factory,selection_);
  
  // You can add so-called "Spectator variables", which are not used in the MVA training,
  // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
  // input variables, the response values of all trained MVAs, and the spectator variables

  Double_t signalWeight     = 1.0;
  Double_t backgroundWeight = 1.0;

  if(split_trees_) {
    TTree* signal_train = signal->CopyTree("train>0.");
    signal_train->SetName("signal_train");
    TTree* signal_test  = signal->CopyTree("train<0.");
    signal_test->SetName("signal_test");
    TTree* bkgd_train = background->CopyTree("train>0.");
    bkgd_train->SetName("bkgd_train");
    TTree* bkgd_test  = background->CopyTree("train<0.");
    bkgd_test->SetName("bkgd_test");
    factory->AddSignalTree    ( signal_train, signalWeight,     "Training" );
    factory->AddSignalTree    ( signal_test,  signalWeight,     "Test" );
    factory->AddBackgroundTree( bkgd_train,   backgroundWeight, "Training" );
    factory->AddBackgroundTree( bkgd_test,    backgroundWeight, "Test" );
  } else {
    factory->AddSignalTree    ( signal, signalWeight);
    factory->AddBackgroundTree( background, backgroundWeight );
  }
  // global event weights per tree (see below for setting event-wise weights)

  // You can add an arbitrary number of signal or background trees


  // To give different trees for training and testing, do as follows:
  //    factory->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
  //    factory->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );




  // Apply additional cuts on the signal and background samples (can be different)


  // Tell the factory how to use the training and testing events
  //
  // If no numbers of events are given, half of the events in the tree are used 
  // for training, and the other half for testing:
  //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
  // To also specify the number of testing events, use:
  //    factory->PrepareTrainingAndTestTree( mycut,
  //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );

// signal is defined as the momentum resolution core,
  TString sig_cut, bkg_cut;
  //  nm = tname;
  for(int i = 0; i < ignore.size(); ++i) {
    if(i > 0) {
      sig_cut += Form("&&(eventcategory != %i.)", ignore[i]);
      bkg_cut += Form("&&(eventcategory != %i.)", ignore[i]);
    } else {
      sig_cut = Form("(eventcategory != %i.)", ignore[i]);
      bkg_cut = Form("(eventcategory != %i.)", ignore[i]);

    }
  }
  if(ignore.size() > 0 && signals.size() > 0) {
    sig_cut += "&&(";
    bkg_cut += "&&(";
  } else if(signals.size() > 0) {
    sig_cut = "(";
    bkg_cut = "(";
  }
  for(int i = 0; i < signals.size(); ++i) {
    if(i > 0 || ignore.size() > 0) {
      sig_cut += Form("||(eventcategory == %i.)", signals[i]);
      bkg_cut += Form("&&(eventcategory != %i.)", signals[i]);
    } else {
      sig_cut += Form("(eventcategory == %i.)", signals[i]);
      bkg_cut += Form("(eventcategory != %i.)", signals[i]);
    }
  }
  if(signals.size() > 0) {
    sig_cut += ")";
    bkg_cut += ")";
  }
  
  // sig_cut += "lepM>25&&lepM<32";
  // bkg_cut += "lepM<70&&(lepM<25||lepM>32)&&lepM>10";

  
  printf("\033[32m--- Using signal identification cut %s\033[0m\n", sig_cut.Data());
  printf("\033[32m--- Using background identification cut %s\033[0m\n", bkg_cut.Data());
  TCut signal_cuts(sig_cut);
  TCut bkg_cuts(bkg_cut);
  
  
  Double_t nSigFrac =  0.7;//0.2; //only use if trees aren't pre-split
  Double_t nBkgFrac =  0.3;//0.2;
  Long64_t nSig = signal->CopyTree(signal_cuts)->GetEntriesFast();
  Long64_t nBkg = background->CopyTree(bkg_cuts)->GetEntriesFast();
  TString options = "";
  if(!split_trees_) {
    options +=  "nTrain_Signal=";
    options += ((Int_t) nSig*nSigFrac);
    options += ":nTrain_Background=";
    options += ((Int_t) nBkg*nBkgFrac);
  }
  
  if(split_trees_)
    options += "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0";
  else
    options += ":nTest_Signal=0:nTest_Background=0:SplitMode=Random:!V:SplitSeed=89281";
  factory->PrepareTrainingAndTestTree(signal_cuts, bkg_cuts, options.Data() );
  printf("\033[32m--- Using signal fraction %.2f\033[0m\n", nSigFrac);
  printf("\033[32m--- Using background fraction %.2f\033[0m\n", nBkgFrac);



  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

  // Cut optimisation
  if (Use["Cuts"])
    factory->BookMethod( TMVA::Types::kCuts, "Cuts",
    	"" );
    // factory->BookMethod( TMVA::Types::kCuts, "Cuts",
    // 	"!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

  if (Use["CutsD"])
    factory->BookMethod( TMVA::Types::kCuts, "CutsD",
	"!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

  if (Use["CutsPCA"])
    factory->BookMethod( TMVA::Types::kCuts, "CutsPCA",
	"!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

  if (Use["CutsGA"])
    factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
	"H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

  if (Use["CutsSA"])
    factory->BookMethod( TMVA::Types::kCuts, "CutsSA",
	"!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

  // Likelihood ("naive Bayes estimator")
  if (Use["Likelihood"])
    factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
	"H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );

  // Decorrelated likelihood
  if (Use["LikelihoodD"])
    factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
	"!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );

  // PCA-transformed likelihood
  if (Use["LikelihoodPCA"])
    factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
	"!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 

  // Use a kernel density estimator to approximate the PDFs
  if (Use["LikelihoodKDE"])
    factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
	"!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" ); 

  // Use a variable-dependent mix of splines and kernel density estimator
  if (Use["LikelihoodMIX"])
    factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX",
	"!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

  // Test the multi-dimensional probability density estimator
  // here are the options strings for the MinMax and RMS methods, respectively:
  //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
  //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
  if (Use["PDERS"])
    factory->BookMethod( TMVA::Types::kPDERS, "PDERS",
	"!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

  if (Use["PDERSD"])
    factory->BookMethod( TMVA::Types::kPDERS, "PDERSD",
	"!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

  if (Use["PDERSPCA"])
    factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA",
	"!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

  // Multi-dimensional likelihood estimator using self-adapting phase-space binning
  if (Use["PDEFoam"])
    factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
	"!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

  if (Use["PDEFoamBoost"])
    factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
	"!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

  // K-Nearest Neighbour classifier (KNN)
  if (Use["KNN"])
    factory->BookMethod( TMVA::Types::kKNN, "KNN",
	"!H:!V:nkNN=70:ScaleFrac=.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

  // H-Matrix (chi2-squared) method
  if (Use["HMatrix"])
    factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

  // Linear discriminant (same as Fisher discriminant)
  if (Use["LD"])
    factory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

  // Fisher discriminant (same as LD)
  if (Use["Fisher"])
    factory->BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

  // Fisher with Gauss-transformed input variables
  if (Use["FisherG"])
    factory->BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

  // Composite classifier: ensemble (tree) of boosted Fisher classifiers
  if (Use["BoostedFisher"])
    factory->BookMethod( TMVA::Types::kFisher, "BoostedFisher", 
	"H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

  // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
  if (Use["FDA_MC"])
    factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

  if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
    factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

  if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
    factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

  if (Use["FDA_MT"])
    factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

  if (Use["FDA_GAMT"])
    factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

  if (Use["FDA_MCMT"])
    factory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

  // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
  if (Use["MLP"])
    factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=N" );

  if (Use["MLP_MM"]) {
    TString network;
    network = "10,5,5";//:LearningRate=1e-2";
    factory->BookMethod( TMVA::Types::kMLP, "MLP_MM",
			 Form("!H:!V:VarTransform=N:HiddenLayers=%s:CreateMVAPdfs=False",//:BPMode=batch:BatchSize=3e4:SamplingEpoch=1:TestRate=5:TrainingMethod=BFGS:!UseRegulator
			      network.Data()));
			 //:LearningRate=1e-2:EstimatorType=MSE:NeuronType=sigmoid" );
  }
  if (Use["MLPBFGS"])
    factory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

  if (Use["MLPBNN"])
    factory->BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

  // CF(Clermont-Ferrand)ANN
  if (Use["CFMlpANN"])
    factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  

  // Tmlp(Root)ANN
  if (Use["TMlpANN"])
    factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

  // Support Vector Machine
  if (Use["SVM"])
    factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=.5:Tol=0.001:C=1:VarTransform=Norm" );

  // Boosted Decision Trees
  if (Use["BDTG"]) // Gradient Boost
    factory->BookMethod( TMVA::Types::kBDT, "BDTG",
	"!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:NNodesMax=5" );

  if (Use["BDT"])  // Adaptive Boost
    factory->BookMethod( TMVA::Types::kBDT,"BDT",
	"!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:CreateMVAPdfs=False" );

  if (Use["BDTRT"]) {  // Bagging Boost Random Trees
    TString bdtSetup = "!H:!V";
    bdtSetup += ":NTrees=1000:MinNodeSize=1%:MaxDepth=5:nCuts=100:UseNVars=5:UseRandomisedTrees=True";
    bdtSetup += ":BoostType=Bagging:BaggedSampleFraction=0.6:CreateMVAPdfs=False";
    // bdtSetup += ":PruneMethod=ExpectedError:PruneStrength=0.2";
    factory->BookMethod( TMVA::Types::kBDT, "BDTRT",bdtSetup.Data());
  }


  if (Use["BDTB"]) // Bagging
    factory->BookMethod( TMVA::Types::kBDT, "BDTB",
	"!H:!V:NTrees=1000:MinNodeSize=1%:BoostType=Bagging:MaxDepth=4:SeparationType=GiniIndex:nCuts=50:PruneMethod=NoPruning" );

  if (Use["BDTD"]) // Decorrelation + Adaptive Boost 
   factory->BookMethod( TMVA::Types::kBDT, "BDTD",
	"!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate" );

  if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
    factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
	"!H:!V:NTrees=50:nEventsMin=150:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

  // RuleFit -- TMVA implementation of Friedman's method
  if (Use["RuleFit"])
    factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
	"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

  // For an example of the category classifier usage, see: TMVAClassificationCategory

  // --------------------------------------------------------------------------------------------------

  // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

  // factory->OptimizeAllMethods("SigEffAt001","Scan");
  // factory->OptimizeAllMethods("ROCIntegral","GA");

  // --------------------------------------------------------------------------------------------------

  // ---- Now you can tell the factory to train, test, and evaluate the MVAs

//  factory->PrintHelpMessage();
//  factory->EvaluateAllVariables();
  factory->SetVerbose();

  if(!dryrun_) {
    
    // Train MVAs using the set of training events
    factory->TrainAllMethods();

    // ---- Evaluate all MVAs using the set of test events
    factory->TestAllMethods();

    // ----- Evaluate and compare performance of all configured MVAs
    factory->EvaluateAllMethods();

    // --------------------------------------------------------------

    // Save the output
    outputFile->Write();
    outputFile->Close();

    std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
    std::cout << "==> TMVAClassification is done!" << std::endl;
  } else
    cout << "Completed dry run!\n";
  
  delete factory;
  //  delete dataloader;

   // Launch the GUI for the root macros
  if (!dryrun_&&!gROOT->IsBatch()) TMVA::TMVAGui( outfilename );
  if(multiTrainings_) {gSystem->cd(".."); inTrainDir_ = false;}
  return 0;
}
