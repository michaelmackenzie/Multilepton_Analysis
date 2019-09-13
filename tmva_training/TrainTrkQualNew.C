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
#include "TMVA/CrossValidation.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#endif

int useMCTruth_ = 0; //train on MC signal vs background or mass band

int kfolds_ = 3; //fold number for k-fold validation


// Training script that uses a newer ROOT build
int TrainTrkQualNew(TTree* signal, TTree* background, const char* tname = "TrkQual", int isData = 0,
		 vector<int> ignore = {16}) {

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


  // Flag to use true category ID to determine which variables are useful in seeing the signal
  int signalCategory = 15;

  //flag to apply the old 1b1c/1b1f cuts
  //  1 = old cut set
  //  2 = pt/M included
  // -1 = only add bjet requirement
  int useOldCuts = 0; 

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
  Use["BDT"]             = 0; // uses Adaptive Boost
  Use["BDTG"]            = 0; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTRT"]           = 1; // uses randomized trees (Random Forest Technique) with Bagging
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

  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  TString outfilename(tname);
  outfilename += useMCTruth_ ? "_truth" : "";
  gSystem->Exec(Form("mkdir %s", outfilename.Data()));
  gSystem->cd(outfilename.Data());
  
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

  // TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
  //     "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );



  //data loader to handle the variables, spectators, signal trees, and background trees
  TString outFolder;
  outFolder = "tmva_";
  outFolder += tname;
  TMVA::DataLoader* dataloader = new TMVA::DataLoader(outFolder);


  if(isData == 0) dataloader->SetBackgroundWeightExpression("fullEventWeight");
  else dataloader->SetBackgroundWeightExpression("genWeight"); //1 for data, +- 1 for mock data
  if(isData == 0) dataloader->SetSignalWeightExpression("fullEventWeight");
  else dataloader->SetSignalWeightExpression("genWeight"); //1 for data, +- 1 for mock data

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  TString dirname(tname);
  dirname += "_Weights";
  (TMVA::gConfig().GetIONames()).fWeightFileDir = dirname.Data();

  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
  bool is1b1c = dirname.Contains("d_2") || dirname.Contains("l_2");
  is1b1c |= dirname.Contains("d_11") || dirname.Contains("l_11"); //similar selection
  bool is1b1f = dirname.Contains("d_3") || dirname.Contains("l_3");
  bool is1b1fb= dirname.Contains("d_6") || dirname.Contains("l_6"); //similar selection
  is1b1f |= is1b1fb;
  
  dataloader->AddVariable("met","MET","GeV",'F');
  dataloader->AddVariable("jetsDelR","#DeltaR_{jj}","",'D');// more correlated for background training
  //  if(!is1b1fb) //biased set 6 background?
  dataloader->AddVariable("jetsDelPhi","#Delta#phi_{jj}","",'D'); // added back in
  // dataloader->AddVariable("jet1eta","#eta_{j1}","",'D'); // not useful
  // dataloader->AddVariable("jet2eta","#eta_{j2}","",'D'); // not useful
  // if(!is1b1fb) //biased set 6 background?
  // dataloader->AddVariable("jetsLepDelR","#DeltaR_{jj+ll}","",'D');
  // dataloader->AddVariable("jetsLepDelPhi","#Delta#phi_{jj+ll}","",'D'); // 5% correlation with mass, biases set 11 background
  // if(!is1b1fb) //biased set 6 background?
  //   dataloader->AddVariable("jetLepDelR","#DeltaR_{j+ll}","",'D');
  // if(!is1b1fb) //biased set 6 background?
  // dataloader->AddVariable("jetLepDelPhi","#Delta#phi_{j+ll}","",'D'); 
  // dataloader->AddVariable("bjetLepDelR","#DeltaR_{b+ll}","",'D'); // only correlated for background training?
  dataloader->AddVariable("bjetLepDelPhi","#Delta#phi_{b+ll}","",'D'); // more correlated for background training
  // if(!is1b1fb) //biased set 6 background?
  dataloader->AddVariable("jetbLepDelR","#DeltaR_{j+bll}","",'D'); // equally correlated for both respones in both
  // if(!is1b1fb) //biased set 6 background?
  // dataloader->AddVariable("jetbLepDelPhi","#Delta#phi_{j+bll}","",'D'); // added back, somewhat correlated for each?
  // dataloader->AddVariable("jetsPt","Pt_{jj}","GeV",'D'); // not useful
  // dataloader->AddVariable("jetsEta","#eta_{jj}","",'D'); // not useful
  // dataloader->AddVariable("bjetLepPt","Pt_{bll}","",'D'); // more correlated for background training
  // dataloader->AddVariable("jetLepPt" ,"Pt_{jll}","",'D'); // more correlated for background training
  // dataloader->AddVariable("lepPt", "Pt_{ll}", "GeV", 'D'); // Biases background 3
  // if(!is1b1f) //possibly biased background
  // dataloader->AddVariable("sysPt","Pt_{jjll}","GeV",'D'); 
  // dataloader->AddVariable("sysEta","#eta_{jjll}","",'D'); // more correlated for background training
  if(!is1b1f)
    dataloader->AddVariable("jet1tag","Tag_{j1}","",'F'); 
  // dataloader->AddVariable("jet2tag","Tag_{j2}","",'F'); 
  // if(!is1b1f)
  //   dataloader->AddVariable("jet1tagged", "Tagged_{j1}", "", 'O');  // somewhat correlated with background training, more correlated for truth
  // if((!useOldCuts&&!is1b1fb) || is1b1c)
  //   dataloader->AddVariable("jet2tagged", "Tagged_{j2}", "", 'O');  // somewhat correlated with background training, more correlated for truth
  // dataloader->AddVariable("jet1puid","ID_{j1}","",'F'); //somewhat correlated for background and truth training
  // dataloader->AddVariable("jet2puid","ID_{j2}","",'F'); //somewhat correlated for background and truth training
  dataloader->AddVariable("jet1d0","D0_{j1}","",'F');// correlates with mass
  // dataloader->AddVariable("jet2d0","D0_{j2}","",'F'); //not correlated for either training
  // dataloader->AddVariable("jet1pt","Pt_{j1}","",'D'); // more correlated for background training
  // dataloader->AddVariable("jet2pt","Pt_{j2}","",'D'); // more correlated for background training
  // dataloader->AddVariable("lep1reliso","Iso/Pt_{l1}","",'D');// correlates with mass
  // dataloader->AddVariable("lep2reliso","Iso/Pt_{l2}","",'D'); // correlates with mass
  if(!is1b1f) dataloader->AddVariable("nJets", "nJets", "", 'i');
  //  if(!dirname.Contains("_2")) dataloader->AddVariable("nFwdJets", "nFwdJets", "", 'i');
  // if(!is1b1f) dataloader->AddVariable("nBJets", "nBJets", "", 'i');
  // dataloader->AddVariable("jetsM","M_{jj}","GeV",'D'); // more correlated for background training
  // dataloader->AddVariable("sysM","M_{jjll}","GeV",'D'); // more correlated for background training

  //  dataloader->AddVariable("nElectrons", "nElectrons", "", 'i');

  dataloader->AddSpectator("lepM", "M_{ll}", "GeV", 'D'); 
  dataloader->AddSpectator("lepPtOverM","Pt/M","",'D');
  dataloader->AddSpectator("bjetLepM", "M_{b+ll}", "GeV", 'D'); 
  dataloader->AddSpectator("bjetLepPtOverM := bjetLepPt/bjetLepM","b+ll Pt/M","",'D');
  dataloader->AddSpectator("lepDelR","#DeltaR_{ll}","",'D');
  dataloader->AddSpectator("lepDelPhi","#Delta#phi_{ll}","",'D');
  dataloader->AddSpectator("fullEventWeight", "fullEventWeight", "", 'D'); 
  dataloader->AddSpectator("eventWeight", "eventWeight", "", 'F'); 
  dataloader->AddSpectator("genWeight", "eventWeight", "", 'F'); 
  dataloader->AddSpectator("eventCategory", "eventCategory", "", 'I'); 


  // You can add so-called "Spectator variables", which are not used in the MVA training,
  // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
  // input variables, the response values of all trained MVAs, and the spectator variables

  Double_t signalWeight     = 1.0;
  Double_t backgroundWeight = 1.0;

  dataloader->AddSignalTree    ( signal, signalWeight);
  dataloader->AddBackgroundTree( background, backgroundWeight );

  // global event weights per tree (see below for setting event-wise weights)

  // You can add an arbitrary number of signal or background trees


  // To give different trees for training and testing, do as follows:
  //    dataloader->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
  //    dataloader->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );




  // Apply additional cuts on the signal and background samples (can be different)


  // Tell the dataloader how to use the training and testing events
  //
  // If no numbers of events are given, half of the events in the tree are used 
  // for training, and the other half for testing:
  //    dataloader->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
  // To also specify the number of testing events, use:
  //    dataloader->PrepareTrainingAndTestTree( mycut,
  //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );

// signal is defined as the momentum resolution core,
  TString sig_cut, bkg_cut;
  //  nm = tname;
  sig_cut = "lepM>27&&lepM<32";
  bkg_cut = "lepM<70&&(lepM<27||lepM>32)&&lepM>10";
  if(useMCTruth_) {
    sig_cut = Form("lepM>10&&lepM<70&&(eventCategory == %i)", signalCategory);
    bkg_cut = Form("lepM>10&&lepM<70&&(eventCategory != %i)", signalCategory);
    printf("\033[31m--- Warning! Using MC truth to identify signal!\033[0m\n");
  }

  for(int i = 0; i < ignore.size(); ++i) {
    sig_cut += Form("&&(eventCategory != %i)", ignore[i]);
    bkg_cut += Form("&&(eventCategory != %i)", ignore[i]);
  }

  if(useOldCuts > 0) { // Add the missing cuts previously applied to the selection
    if(is1b1c) {
      sig_cut += "&&(met<40)&&(jetsLepDelPhi>2.5)&&(nBJets+nJets==2)&&(jet1tagged || jet2tagged)";
      bkg_cut += "&&(met<40)&&(jetsLepDelPhi>2.5)&&(nBJets+nJets==2)&&(jet1tagged || jet2tagged)";
    }
    else if(is1b1f) {
      sig_cut += "&&(jet1tagged || jet2tagged)";
      bkg_cut += "&&(jet1tagged || jet2tagged)";
    }

    if(useOldCuts > 1) { //use pt/M cut
      sig_cut += "&&(lepPtOverM > 2)";
      bkg_cut += "&&(lepPtOverM > 2)";
    }
  } else if(useOldCuts == -1) { // only require btag as an additional
      sig_cut += "&&(jet1tagged || jet2tagged)";
      bkg_cut += "&&(jet1tagged || jet2tagged)";
  }
  
  printf("\033[32m--- Using signal identification cut %s\033[0m\n", sig_cut.Data());
  printf("\033[32m--- Using background identification cut %s\033[0m\n", bkg_cut.Data());
  TCut signal_cuts(sig_cut);
  TCut bkg_cuts(bkg_cut);
  
  
  Double_t nFraction = (isData) ? 0.5 : 0.1;//0.2;
  Long64_t nSig = signal->CopyTree(signal_cuts)->GetEntriesFast();
  Long64_t nBkg = background->CopyTree(bkg_cuts)->GetEntriesFast();
  if(isData == 0 && nSig*nFraction < 5000) nFraction = min(1.,5000./nSig);

  //Train set to 0 with kFold validation
  TString options = "nTrain_Signal=0";
  // options += ((Int_t) nSig*nFraction);
  options += ":nTrain_Background=0";
  // if(useMCTruth_)
  //   options += ((Int_t) nBkg*nFraction/2.);
  // else
  //   options += ((Int_t) nBkg*nFraction);

  // nTest set to 1 since kfold only uses the training sets for testing
  options += ":nTest_Signal=1:nTest_Background=1:SplitMode=Random:!V:SplitSeed=89281";
  // options += numFolds;
  dataloader->PrepareTrainingAndTestTree(signal_cuts, bkg_cuts, options.Data() );

  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable


  // TMVA::CrossValidation *cv = new TMVA::CrossValidation( "TMVAClassification", dataloader,
  //      outputFile, "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );
  TMVA::CrossValidation cv{ "TMVAClassification", dataloader, outputFile,
      "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" };

  // Cut optimisation
  if (Use["Cuts"])
    cv.BookMethod( TMVA::Types::kCuts, "Cuts",
    	"" );
    // cv.BookMethod( TMVA::Types::kCuts, "Cuts",
    // 	"!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

  if (Use["CutsD"])
    cv.BookMethod( TMVA::Types::kCuts, "CutsD",
	"!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

  if (Use["CutsPCA"])
    cv.BookMethod( TMVA::Types::kCuts, "CutsPCA",
	"!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

  if (Use["CutsGA"])
    cv.BookMethod( TMVA::Types::kCuts, "CutsGA",
	"H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

  if (Use["CutsSA"])
    cv.BookMethod( TMVA::Types::kCuts, "CutsSA",
	"!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

  // Likelihood ("naive Bayes estimator")
  if (Use["Likelihood"])
    cv.BookMethod( TMVA::Types::kLikelihood, "Likelihood",
	"H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );

  // Decorrelated likelihood
  if (Use["LikelihoodD"])
    cv.BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
	"!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );

  // PCA-transformed likelihood
  if (Use["LikelihoodPCA"])
    cv.BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
	"!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 

  // Use a kernel density estimator to approximate the PDFs
  if (Use["LikelihoodKDE"])
    cv.BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
	"!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" ); 

  // Use a variable-dependent mix of splines and kernel density estimator
  if (Use["LikelihoodMIX"])
    cv.BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX",
	"!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

  // Test the multi-dimensional probability density estimator
  // here are the options strings for the MinMax and RMS methods, respectively:
  //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
  //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
  if (Use["PDERS"])
    cv.BookMethod( TMVA::Types::kPDERS, "PDERS",
	"!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

  if (Use["PDERSD"])
    cv.BookMethod( TMVA::Types::kPDERS, "PDERSD",
	"!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

  if (Use["PDERSPCA"])
    cv.BookMethod( TMVA::Types::kPDERS, "PDERSPCA",
	"!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

  // Multi-dimensional likelihood estimator using self-adapting phase-space binning
  if (Use["PDEFoam"])
    cv.BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
	"!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

  if (Use["PDEFoamBoost"])
    cv.BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
	"!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

  // K-Nearest Neighbour classifier (KNN)
  if (Use["KNN"])
    cv.BookMethod( TMVA::Types::kKNN, "KNN",
	"!H:!V:nkNN=70:ScaleFrac=.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

  // H-Matrix (chi2-squared) method
  if (Use["HMatrix"])
    cv.BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

  // Linear discriminant (same as Fisher discriminant)
  if (Use["LD"])
    cv.BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

  // Fisher discriminant (same as LD)
  if (Use["Fisher"])
    cv.BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

  // Fisher with Gauss-transformed input variables
  if (Use["FisherG"])
    cv.BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

  // Composite classifier: ensemble (tree) of boosted Fisher classifiers
  if (Use["BoostedFisher"])
    cv.BookMethod( TMVA::Types::kFisher, "BoostedFisher", 
	"H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

  // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
  if (Use["FDA_MC"])
    cv.BookMethod( TMVA::Types::kFDA, "FDA_MC",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

  if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
    cv.BookMethod( TMVA::Types::kFDA, "FDA_GA",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

  if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
    cv.BookMethod( TMVA::Types::kFDA, "FDA_SA",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

  if (Use["FDA_MT"])
    cv.BookMethod( TMVA::Types::kFDA, "FDA_MT",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

  if (Use["FDA_GAMT"])
    cv.BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

  if (Use["FDA_MCMT"])
    cv.BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
	"H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

  // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
  if (Use["MLP"])
    cv.BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=N" );

  if (Use["MLP_MM"]) {
    TString network;
    // if(outfilename.Contains("d_3") || outfilename.Contains("l_3"))
    //   network = "10,5,3";
    // else
    network = "12,5,2";//:LearningRate=1e-2";
    cv.BookMethod( TMVA::Types::kMLP, "MLP_MM",
			 Form("!H:!V:VarTransform=N:HiddenLayers=%s",
			      network.Data()));
			 //:LearningRate=1e-2:EstimatorType=MSE:NeuronType=sigmoid" );
  }
  if (Use["MLPBFGS"])
    cv.BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

  if (Use["MLPBNN"])
    cv.BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

  // CF(Clermont-Ferrand)ANN
  if (Use["CFMlpANN"])
    cv.BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  

  // Tmlp(Root)ANN
  if (Use["TMlpANN"])
    cv.BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

  // Support Vector Machine
  if (Use["SVM"])
    cv.BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=.5:Tol=0.001:C=1:VarTransform=Norm" );

  // Boosted Decision Trees
  if (Use["BDTG"]) // Gradient Boost
    cv.BookMethod( TMVA::Types::kBDT, "BDTG",
	"!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:NNodesMax=5" );

  if (Use["BDT"])  // Adaptive Boost
    cv.BookMethod( TMVA::Types::kBDT, "BDT",
	"!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

  if (Use["BDTRT"]) {  // Bagging Boost Random Trees
    TString bdtSetup = "!H:!V";
    bdtSetup += ":NTrees=200:MinNodeSize=3%:MaxDepth=2:nCuts=1600:UseNVars=6:UseRandomisedTrees=True";
    bdtSetup += ":BoostType=Bagging:BaggedSampleFraction=0.95";
    // bdtSetup += ":PruneMethod=ExpectedError:PruneStrength=0.2";
    cv.BookMethod( TMVA::Types::kBDT, "BDTRT",bdtSetup.Data());
  }


  if (Use["BDTB"]) // Bagging
    cv.BookMethod( TMVA::Types::kBDT, "BDTB",
	"!H:!V:NTrees=1000:MinNodeSize=1%:BoostType=Bagging:MaxDepth=4:SeparationType=GiniIndex:nCuts=50:PruneMethod=NoPruning" );

  if (Use["BDTD"]) // Decorrelation + Adaptive Boost 
   cv.BookMethod( TMVA::Types::kBDT, "BDTD",
	"!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate" );

  if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
    cv.BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
	"!H:!V:NTrees=50:nEventsMin=150:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

  // RuleFit -- TMVA implementation of Friedman's method
  if (Use["RuleFit"])
    cv.BookMethod( TMVA::Types::kRuleFit, "RuleFit",
	"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

  // For an example of the category classifier usage, see: TMVAClassificationCategory

  // --------------------------------------------------------------------------------------------------

  // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

  // cv.OptimizeAllMethods("SigEffAt001","Scan");
  // cv.OptimizeAllMethods("ROCIntegral","GA");

  // --------------------------------------------------------------------------------------------------

  // ---- Now you can tell the cv to train, test, and evaluate the MVAs

//  cv.PrintHelpMessage();
//  cv.EvaluateAllVariables();
  cv.SetNumFolds(kfolds_);
  cv.SetVerbose(kTRUE);
 
  // Train MVAs using the set of training events
  // factory->TrainAllMethods();
  cv.Evaluate();

  // ---- Evaluate all MVAs using the set of test events
  // factory->TestAllMethods();
  
  // ----- Evaluate and compare performance of all configured MVAs
  // factory->EvaluateAllMethods();

  // --------------------------------------------------------------

  // Save the output
  outputFile->Write();
  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
  std::cout << "==> TMVAClassification is done!" << std::endl;

  // delete cv;
  // delete dataloader;

   // Launch the GUI for the root macros
  if (!gROOT->IsBatch()) TMVA::TMVAGui( outfilename );

  return 0;
}
