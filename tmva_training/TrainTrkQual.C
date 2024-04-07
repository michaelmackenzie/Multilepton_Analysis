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
using namespace CLFV;

TString selection_ = "zmutau";
Int_t split_trees_ = 1; //split training/testing using tree defined samples
Int_t trkqual_version_ = -1; //version of variables used in trkqual training, < 0 is use default
Int_t use_njets_ = 1; //whether or not to train using njets, don't use if jet binned categories
int use_mass_window_ = 1; //>0: train B->e+mu MVAs in the relevant mass window; >1: train tau channels in mass window
Int_t nprongs_ = -1; //hadronic tau prong mode
Bool_t scale_zll_ = true; //increase the impact of Z->ll due to the systematic impact
TString additional_cut_ = ""; //"ptdiff > 0."; //additional cut to apply to the selection

bool dryrun_ = false; //don't actually train
bool plotOnly_ = false; //read and plot pre-existing training results
bool doDetailedPlots_  = false; //make detailed plots or not
bool inTrainDir_ = false; //track if we've gone into a training directory
bool multiTrainings_ = false; //if doing multiple trainings, leave directory at the end of the training

//print a ROC plot
int print_roc(TString var, TTree* TestTree, TTree* TrainTree, TString dir, TString cut = "") {
  TCanvas* c = new TCanvas();
  TH1 *hsig_test, *hbkg_test, *hsig_train, *hbkg_train;
  TString weight_train = (split_trees_) ? "fulleventweightlum/(trainfraction)"    : "fulleventweightlum";
  TString weight_test  = (split_trees_) ? "fulleventweightlum/(1.-trainfraction)" : "fulleventweightlum";

  //first get the initial distributions
  const int nbins(2000);
  const double xmin(-1.), xmax(1.);
  hsig_test  = new TH1D("hsig_test" , var.Data(), nbins, xmin, xmax);
  hbkg_test  = new TH1D("hbkg_test" , var.Data(), nbins, xmin, xmax);
  hsig_train = new TH1D("hsig_train", var.Data(), nbins, xmin, xmax);
  hbkg_train = new TH1D("hbkg_train", var.Data(), nbins, xmin, xmax);
  if(cut != "") cut = "(" + cut + ") &&";
  TestTree ->Draw(Form("%s >> hsig_test" , var.Data()), Form("(%s (issignal > 0.5))*%s", cut.Data(), weight_test.Data() ));
  TestTree ->Draw(Form("%s >> hbkg_test" , var.Data()), Form("(%s (issignal < 0.5))*%s", cut.Data(), weight_test.Data() ));
  TrainTree->Draw(Form("%s >> hsig_train", var.Data()), Form("(%s (issignal > 0.5))*%s", cut.Data(), weight_train.Data()));
  TrainTree->Draw(Form("%s >> hbkg_train", var.Data()), Form("(%s (issignal < 0.5))*%s", cut.Data(), weight_train.Data()));

  hsig_test ->Scale(1./hsig_test ->Integral());
  hbkg_test ->Scale(1./hbkg_test ->Integral());
  hsig_train->Scale(1./hsig_train->Integral());
  hbkg_train->Scale(1./hbkg_train->Integral());

  //create efficiency arrays
  double bkg_test[nbins], bkg_train[nbins], sig_test[nbins], sig_train[nbins];
  double nbkg_test(0.), nbkg_train(0.), nsig_test(0.), nsig_train(0.);
  double roc_test(0.), roc_train(0.);
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    nbkg_test  += hbkg_test ->GetBinContent(ibin);
    nbkg_train += hbkg_train->GetBinContent(ibin);
    nsig_test  += hsig_test ->GetBinContent(ibin);
    nsig_train += hsig_train->GetBinContent(ibin);
    bkg_test [ibin-1] = nbkg_test; //save background rejection
    bkg_train[ibin-1] = nbkg_train;
    sig_test [ibin-1] = 1. - nsig_test; //save signal efficiency
    sig_train[ibin-1] = 1. - nsig_train;
    //get the integral under the curve
    // x = sig_eff, y = bkg_rej
    // integral = sum(y(x)*dx)
    if(ibin > 1 && ibin < nbins) {
      //do trapezoidal sum
      roc_test  += (sig_test [ibin-2] - sig_test [ibin-1])*(bkg_test [ibin-2] + bkg_test [ibin-1])/2.;
      roc_train += (sig_train[ibin-2] - sig_train[ibin-1])*(bkg_train[ibin-2] + bkg_train[ibin-1])/2.;
    }
  }

  cout << "ROC scores for " << var.Data() << endl << " Test : " << roc_test << endl << " Train: " << roc_train << endl;
  TGraph* test  = new TGraph(nbins, sig_test , bkg_test );
  TGraph* train = new TGraph(nbins, sig_train, bkg_train);
  test->SetLineColor(kBlue);
  test->SetLineWidth(2);
  train->SetLineColor(kRed);
  train->SetLineWidth(2);
  train->SetLineStyle(kDashed);

  test->Draw("AL");
  train->Draw("L");
  test->SetTitle(Form("%s ROC; Signal efficiency; Background rejection", var.Data()));
  TLegend* leg = new TLegend(0.1, 0.1, 0.4, 0.3);
  leg->AddEntry(test , Form("Test : %.3f", roc_test ), "L");
  leg->AddEntry(train, Form("Train: %.3f", roc_train), "L");
  leg->Draw();
  c->SetGrid();
  if(cut == "") {
    c->SaveAs(Form("%s/plots/roc_%s.png", dir.Data(), var.Data()));
  } else {
    c->SaveAs(Form("%s/plots/roc_%s_cut.png", dir.Data(), var.Data()));
  }

  delete c;
  delete test;
  delete train;
  delete hbkg_test;
  delete hbkg_train;
  delete hsig_test;
  delete hsig_train;

  return 0;
}

//print a spectator comparison canvas
int print_spectator(TString var, TTree* TestTree, TTree* TrainTree, TString dir) {
  TCanvas* c = new TCanvas();
  TH1 *hsig_test, *hbkg_test, *hsig_train, *hbkg_train;
  TString weight_train = (split_trees_) ? "fulleventweightlum/(trainfraction)"    : "fulleventweightlum";
  TString weight_test  = (split_trees_) ? "fulleventweightlum/(1.-trainfraction)" : "fulleventweightlum";
  //first get the initial distributions
  TestTree->Draw(Form("%s >> hsig_test", var.Data()), Form("(issignal > 0.5)*%s", weight_test.Data()));
  hsig_test = (TH1*) gDirectory->Get("hsig_test");
  TestTree->Draw(Form("%s >> hbkg_test", var.Data()), Form("(issignal < 0.5)*%s", weight_test.Data()));
  hbkg_test = (TH1*) gDirectory->Get("hbkg_test");
  TrainTree->Draw(Form("%s >> hsig_train", var.Data()), Form("(issignal > 0.5)*%s", weight_train.Data()));
  hsig_train = (TH1*) gDirectory->Get("hsig_train");
  TrainTree->Draw(Form("%s >> hbkg_train", var.Data()), Form("(issignal < 0.5)*%s", weight_train.Data()));
  hbkg_train = (TH1*) gDirectory->Get("hbkg_train");

  if(!hsig_test || !hsig_train || !hbkg_test || !hbkg_train) {
    delete c;
    return 1;
  }

  //next create more reasonable histograms
  double xmin =    max(hsig_test ->GetXaxis()->GetBinLowEdge(1), hsig_test ->GetMean() - 3.5*hsig_test ->GetStdDev());
  xmin = min(xmin, max(hbkg_test ->GetXaxis()->GetBinLowEdge(1), hbkg_test ->GetMean() - 3.5*hbkg_test ->GetStdDev()));
  xmin = min(xmin, max(hsig_train->GetXaxis()->GetBinLowEdge(1), hsig_train->GetMean() - 3.5*hsig_train->GetStdDev()));
  xmin = min(xmin, max(hbkg_train->GetXaxis()->GetBinLowEdge(1), hbkg_train->GetMean() - 3.5*hbkg_train->GetStdDev()));
  double xmax =    min(hsig_test ->GetBinLowEdge(hsig_test ->GetNbinsX()) + hsig_test ->GetBinWidth(1), hsig_test ->GetMean() + 3.5*hsig_test ->GetStdDev());
  xmax = max(xmax, min(hbkg_test ->GetBinLowEdge(hbkg_test ->GetNbinsX()) + hbkg_test ->GetBinWidth(1), hbkg_test ->GetMean() + 3.5*hbkg_test ->GetStdDev()));
  xmax = max(xmax, min(hsig_train->GetBinLowEdge(hsig_train->GetNbinsX()) + hsig_train->GetBinWidth(1), hsig_train->GetMean() + 3.5*hsig_train->GetStdDev()));
  xmax = max(xmax, min(hbkg_train->GetBinLowEdge(hbkg_train->GetNbinsX()) + hbkg_train->GetBinWidth(1), hbkg_train->GetMean() + 3.5*hbkg_train->GetStdDev()));
  const int nbins = 40;
  delete hsig_test;
  delete hbkg_test;
  delete hsig_train;
  delete hbkg_train;
  hsig_test  = new TH1D("hsig_test" , var.Data(), nbins, xmin, xmax);
  hbkg_test  = new TH1D("hbkg_test" , var.Data(), nbins, xmin, xmax);
  hsig_train = new TH1D("hsig_train", var.Data(), nbins, xmin, xmax);
  hbkg_train = new TH1D("hbkg_train", var.Data(), nbins, xmin, xmax);
  TestTree ->Draw(Form("%s >> hsig_test" , var.Data()), Form("(issignal > 0.5)*%s", weight_test.Data()));
  TestTree ->Draw(Form("%s >> hbkg_test" , var.Data()), Form("(issignal < 0.5)*%s", weight_test.Data()));
  TrainTree->Draw(Form("%s >> hsig_train", var.Data()), Form("(issignal > 0.5)*%s", weight_train.Data()));
  TrainTree->Draw(Form("%s >> hbkg_train", var.Data()), Form("(issignal < 0.5)*%s", weight_train.Data()));

  const double nbkg = hbkg_test->Integral(); //for making the stacked results

  hsig_test ->Scale(1./hsig_test ->Integral());
  hbkg_test ->Scale(1./hbkg_test ->Integral());
  hsig_train->Scale(1./hsig_train->Integral());
  hbkg_train->Scale(1./hbkg_train->Integral());
  hsig_test ->SetLineColor(kBlue); hsig_test ->SetFillStyle(1001); hsig_test ->SetMarkerColor(kBlue); hsig_test ->SetFillColor(kAzure+1); hsig_test ->SetLineWidth(2);
  hbkg_test ->SetLineColor(kRed ); hbkg_test ->SetFillStyle(3004); hbkg_test ->SetMarkerColor(kRed ); hbkg_test ->SetFillColor(kRed   ); hbkg_test ->SetLineWidth(2);
  hsig_train->SetLineColor(kBlue); hsig_train->SetFillStyle(1001); hsig_train->SetMarkerColor(kBlue); hsig_train->SetFillColor(kAzure+1); hsig_train->SetLineWidth(2);
  hbkg_train->SetLineColor(kRed ); hbkg_train->SetFillStyle(3004); hbkg_train->SetMarkerColor(kRed ); hbkg_train->SetFillColor(kRed   ); hbkg_train->SetLineWidth(2);
  double max_val = max(hsig_test->GetMaximum(), hbkg_test->GetMaximum());
  max_val = max(max_val, max(hsig_test->GetMaximum(), hbkg_test->GetMaximum()));
  double min_val = max_val / 1.e5;
  //if < 3 bins within 25% of maximum, do log scale
  int nhigh = 0;
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    if(hsig_test->GetBinContent(ibin) > 0.25*hsig_test->GetMaximum()) ++nhigh;
  }
  bool dology = nhigh < 4;
  hsig_test->Draw("hist"); hsig_train->Draw("PL same"); hbkg_test->Draw("hist same"); hbkg_train->Draw("PL same");
  hsig_test->GetYaxis()->SetRangeUser(min_val, ((dology) ? 5. : 1.2)*max_val);
  if(dology) c->SetLogy();

  TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
  leg->SetNColumns(2);
  leg->AddEntry(hsig_test, "Signal (test)");
  leg->AddEntry(hsig_train, "Signal (train)", "PL");
  leg->AddEntry(hbkg_test, "Background (test)");
  leg->AddEntry(hbkg_train, "Background (train)", "PL");
  leg->Draw();
  c->SaveAs(Form("%s/plots/spectator_%s.png", dir.Data(), var.Data()));

  //Next make a stack of the test backgrounds
  const bool isemu = (selection_.EndsWith("emu") || selection_.Contains("_")); //QCD or j-->tau backgrounds
  TH1* hdy_tt = (TH1*) hbkg_test->Clone("hdy_tt");
  TH1* hdy_ll = (TH1*) hbkg_test->Clone("hdy_ll");
  TH1* hww    = (TH1*) hbkg_test->Clone("hww");
  TH1* httbar = (TH1*) hbkg_test->Clone("httbar");
  TH1* hqcd   = (TH1*) hbkg_test->Clone("hqcd");
  TH1* hwlnu  = (TH1*) hbkg_test->Clone("hwlnu");
  const int top_c   = kYellow - 7;
  const int dy_tt_c = kRed - 7;
  const int dy_ll_c = kRed - 2;
  const int wj_c    = kViolet - 9;
  const int vb_c    = kMagenta - 9;
  const int qcd_c   = (isemu) ? kOrange + 6 : kGreen - 7;
  hdy_tt->Reset(); hdy_tt->SetLineColor(dy_tt_c); hdy_tt->SetFillColor(dy_tt_c); hdy_tt->SetFillStyle(1001);
  hdy_ll->Reset(); hdy_ll->SetLineColor(dy_ll_c); hdy_ll->SetFillColor(dy_ll_c); hdy_ll->SetFillStyle(1001);
  hww   ->Reset(); hww   ->SetLineColor(   vb_c); hww   ->SetFillColor(   vb_c); hww   ->SetFillStyle(1001);
  httbar->Reset(); httbar->SetLineColor(  top_c); httbar->SetFillColor(  top_c); httbar->SetFillStyle(1001);
  hqcd  ->Reset(); hqcd  ->SetLineColor(  qcd_c); hqcd  ->SetFillColor(  qcd_c); hqcd  ->SetFillStyle(1001);
  hwlnu ->Reset(); hwlnu ->SetLineColor(   wj_c); hwlnu ->SetFillColor(   wj_c); hwlnu ->SetFillStyle(1001);
  TestTree ->Draw(Form("%s >> hdy_tt", var.Data()), Form("(type == 3)*%s", weight_test.Data()));
  TestTree ->Draw(Form("%s >> hdy_ll", var.Data()), Form("(type == 2)*%s", weight_test.Data()));
  TestTree ->Draw(Form("%s >> hww"   , var.Data()), Form("(type == 6)*%s", weight_test.Data()));
  TestTree ->Draw(Form("%s >> httbar", var.Data()), Form("(type == 4)*%s", weight_test.Data()));
  TestTree ->Draw(Form("%s >> hqcd"  , var.Data()), Form("(type == 1)*%s", weight_test.Data()));
  TestTree ->Draw(Form("%s >> hwlnu" , var.Data()), Form("(type == 5)*%s", weight_test.Data()));

  hdy_tt->Scale(1./nbkg);
  hdy_ll->Scale(1./nbkg);
  hww   ->Scale(1./nbkg);
  httbar->Scale(1./nbkg);
  hqcd  ->Scale(1./nbkg);
  hwlnu ->Scale(1./nbkg);

  THStack* hbkg = new THStack("hstack", var.Data());
  hbkg->Add(hww);
  hbkg->Add(httbar);
  hbkg->Add(hdy_ll);
  hbkg->Add(hdy_tt);
  hbkg->Add(hwlnu);
  hbkg->Add(hqcd);

  hbkg->SetMaximum(((dology) ? 5. : 1.2)*max_val);
  hbkg->SetMinimum(min_val);

  hbkg->Draw("hist noclear");
  hsig_test->SetFillStyle(3005);
  hsig_test->Draw("hist same");
  hsig_train->Draw("PL same");

  leg = new TLegend(0.15, 0.7, 0.9, 0.9);
  leg->SetNColumns(3);
  leg->AddEntry(hsig_test, "Signal (test)");
  leg->AddEntry(hsig_train, "Signal (train)", "PL");
  leg->AddEntry(hdy_tt, "Z->#tau#tau", "F");
  leg->AddEntry(hdy_ll, "Z->ll", "F");
  leg->AddEntry(httbar, "Top", "F");
  leg->AddEntry(hww, "WW/Other", "F");
  leg->AddEntry(hwlnu, "W+Jets", "F");
  leg->AddEntry(hqcd, (isemu) ? "QCD" : "j#rightarrow#tau", "F");
  leg->Draw();

  c->SaveAs(Form("%s/plots/spectator_%s_stack.png", dir.Data(), var.Data()));

  delete c;
  delete hsig_test;
  delete hbkg_test;
  delete hsig_train;
  delete hbkg_train;
  delete hdy_tt;
  delete hdy_ll;
  delete hww   ;
  delete httbar;
  delete hqcd  ;
  delete hwlnu ;
  delete hbkg  ;

  return 0;
}

//Train MVA to separate the given signal from the given background
int TrainTrkQual(TTree* signal, TTree* background, const char* tname = "TrkQual",
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

  // --- Here the preparation phase begins
  //if in a past training directory, leave it
  if(inTrainDir_) {
    gSystem->cd("..");
    inTrainDir_ = false;
  }
  // Define the ROOT output file name where TMVA will store ntuples, histograms, etc.
  TString outfilename(tname);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir %s", outfilename.Data(), outfilename.Data()));
  gSystem->cd(outfilename.Data());
  inTrainDir_ = true;
  outfilename += ".root";

  TString outFolder;
  outFolder = "tmva_";
  outFolder += tname;

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
  Use["BDT_MM"]          = 1; // uses Adaptive Boost and modifications
  Use["BDTRT"]           = 0; // uses randomized trees (Random Forest Technique) with Bagging
  Use["BDTG"]            = 0; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
  Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
  //
  // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
  Use["RuleFit"]         = 0;
  // ---------------------------------------------------------------

  std::cout << std::endl;

  TFile* outputFile = nullptr;
  TrkQualInit trkqualinit((trkqual_version_ >= 0) ? trkqual_version_ : TrkQualInit::Default, use_njets_); //version number input

  if(!plotOnly_) {
    std::cout << "==> Start TMVAClassification" << std::endl;

    // --------------------------------------------------------------------------------------------------

    // --- Here the preparation phase begins

    // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
    outputFile = TFile::Open( outfilename, "RECREATE" );

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

    TString factory_options = "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification";
    factory_options += ":Transformations=I";
    // factory_options += ":Transformations=P";
    // factory_options += ":Transformations=G,D";
    TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, factory_options.Data());


    //data loader to handle the variables, spectators, signal trees, and background trees
    TMVA::DataLoader* dataloader = new TMVA::DataLoader(outFolder);

    //Set weight expressions
    if(scale_zll_ && selection_.Contains("tau")) {
      const double zll_scale = selection_.Contains("_") ? 10. : (selection_.EndsWith("mutau")) ? 5. : 5.;
      cout << "--- Scale Z->ll background weight by " << zll_scale << endl;
      dataloader->SetBackgroundWeightExpression(Form("fulleventweightlum*(1 + %.2f*(type == 2))*((train > 0.) ? 1./trainfraction : 1./(1.-trainfraction)) ", zll_scale - 1.));
    } else {
      dataloader->SetBackgroundWeightExpression("fulleventweightlum*((train > 0.) ? 1./trainfraction : 1./(1.-trainfraction))");
     }
    dataloader->SetSignalWeightExpression("fulleventweightlum*((train > 0.) ? 1./trainfraction : 1./(1.-trainfraction))");
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

    trkqualinit.InitializeVariables(*dataloader,selection_);

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
      dataloader->AddSignalTree    (signal_train, signalWeight,     "Training" );
      dataloader->AddSignalTree    (signal_test,  signalWeight,     "Test" );
      dataloader->AddBackgroundTree(bkgd_train,   backgroundWeight, "Training" );
      dataloader->AddBackgroundTree(bkgd_test,    backgroundWeight, "Test" );
    } else {
      dataloader->AddSignalTree    (signal, signalWeight);
      dataloader->AddBackgroundTree(background, backgroundWeight );
    }


    // Tell the factory how to use the training and testing events
    //
    // If no numbers of events are given, half of the events in the tree are used
    // for training, and the other half for testing:
    //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
    // To also specify the number of testing events, use:
    //    factory->PrepareTrainingAndTestTree( mycut,
    //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );

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
    if(ignore.size() > 0) {
      sig_cut += "&&";
      bkg_cut += "&&";
    }
    sig_cut = "(issignal >  0.5)"; //&&fulleventweightlum<50&&fulleventweight>-10";
    bkg_cut = "(issignal <  0.5)"; //&&fulleventweightlum<50&&fulleventweight>-10"; //use background MC and SS data

    if(use_mass_window_ && selection_.Contains("emu")) {
      const double xmin = (selection_.Contains("h")) ? 115. : 80.;
      const double xmax = (selection_.Contains("h")) ? 135. : 100.;
      sig_cut += Form(" && (lepm > %.1f) && (lepm < %.1f)", xmin, xmax);
      bkg_cut += Form(" && (lepm > %.1f) && (lepm < %.1f)", xmin, xmax);
    } else if(use_mass_window_ > 1 && selection_.EndsWith("tau")) {
      const double xmin = (selection_.Contains("h")) ?  60. :  50.;
      const double xmax = (selection_.Contains("h")) ? 130. : 110.;
      sig_cut += Form(" && (lepm > %.1f) && (lepm < %.1f)", xmin, xmax);
      bkg_cut += Form(" && (lepm > %.1f) && (lepm < %.1f)", xmin, xmax);
    } else if(use_mass_window_ > 1) { //leptonic tau
      const double xmin = (selection_.Contains("h")) ?  60. :  40.;
      const double xmax = (selection_.Contains("h")) ? 130. : 100.;
      sig_cut += Form(" && (lepm > %.1f) && (lepm < %.1f)", xmin, xmax);
      bkg_cut += Form(" && (lepm > %.1f) && (lepm < %.1f)", xmin, xmax);
    }
    if(nprongs_ > 0 && selection_.EndsWith("tau")) {
      sig_cut += Form(" && ((2*(taudecaymode / 10) + 1) %s 2)", (nprongs_ > 1 ? ">" : "<"));
      bkg_cut += Form(" && ((2*(taudecaymode / 10) + 1) %s 2)", (nprongs_ > 1 ? ">" : "<"));
    }

    if(additional_cut_ != "") {
      printf("\033[32m!!! WARNING: Applying additional cut %s to signal and background!\033[0m\n", additional_cut_.Data());
      sig_cut += Form(" && (%s)", additional_cut_.Data());
      bkg_cut += Form(" && (%s)", additional_cut_.Data());
    }

    printf("\033[32m--- Using signal identification cut %s\033[0m\n", sig_cut.Data());
    printf("\033[32m--- Using background identification cut %s\033[0m\n", bkg_cut.Data());
    TCut signal_cuts(sig_cut);
    TCut bkg_cuts(bkg_cut);


    Double_t nSigFrac =  0.5;//0.2; //only use if trees aren't pre-split
    Double_t nBkgFrac =  0.3;//0.043;//0.2;
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
    dataloader->PrepareTrainingAndTestTree(signal_cuts, bkg_cuts, options.Data() );
    if(!split_trees_) {
      printf("\033[32m--- Using signal fraction %.2f\033[0m\n", nSigFrac);
      printf("\033[32m--- Using background fraction %.2f\033[0m\n", nBkgFrac);
    }


    // ---- Book MVA methods
    //
    // Please lookup the various method configuration options in the corresponding cxx files, eg:
    // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
    // it is possible to preset ranges in the option string in which the cut optimisation should be done:
    // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

    // TMVA::CrossValidation cv{ "TMVAClassification", dataloader, outputFile,
    //     "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" };

    // Cut optimisation
    if (Use["Cuts"])
      factory->BookMethod(dataloader, TMVA::Types::kCuts, "Cuts",
                          "" );
    // factory->BookMethod( TMVA::Types::kCuts, "Cuts",
    //  "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

    if (Use["CutsD"])
      factory->BookMethod(dataloader, TMVA::Types::kCuts, "CutsD",
                          "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

    if (Use["CutsPCA"])
      factory->BookMethod(dataloader, TMVA::Types::kCuts, "CutsPCA",
                          "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

    if (Use["CutsGA"])
      factory->BookMethod(dataloader, TMVA::Types::kCuts, "CutsGA",
                          "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

    if (Use["CutsSA"])
      factory->BookMethod(dataloader, TMVA::Types::kCuts, "CutsSA",
                          "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

    // Likelihood ("naive Bayes estimator")
    if (Use["Likelihood"])
      factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "Likelihood",
                          "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );

    // Decorrelated likelihood
    if (Use["LikelihoodD"])
      factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "LikelihoodD",
                          "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );

    // PCA-transformed likelihood
    if (Use["LikelihoodPCA"])
      factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "LikelihoodPCA",
                          "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" );

    // Use a kernel density estimator to approximate the PDFs
    if (Use["LikelihoodKDE"])
      factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "LikelihoodKDE",
                          "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" );

    // Use a variable-dependent mix of splines and kernel density estimator
    if (Use["LikelihoodMIX"])
      factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "LikelihoodMIX",
                          "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" );

    // Test the multi-dimensional probability density estimator
    // here are the options strings for the MinMax and RMS methods, respectively:
    //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
    //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
    if (Use["PDERS"])
      factory->BookMethod(dataloader, TMVA::Types::kPDERS, "PDERS",
                          "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

    if (Use["PDERSD"])
      factory->BookMethod(dataloader, TMVA::Types::kPDERS, "PDERSD",
                          "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

    if (Use["PDERSPCA"])
      factory->BookMethod(dataloader, TMVA::Types::kPDERS, "PDERSPCA",
                          "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

    // Multi-dimensional likelihood estimator using self-adapting phase-space binning
    if (Use["PDEFoam"])
      factory->BookMethod(dataloader, TMVA::Types::kPDEFoam, "PDEFoam",
                          "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

    if (Use["PDEFoamBoost"])
      factory->BookMethod(dataloader, TMVA::Types::kPDEFoam, "PDEFoamBoost",
                          "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

    // K-Nearest Neighbour classifier (KNN)
    if (Use["KNN"])
      factory->BookMethod(dataloader, TMVA::Types::kKNN, "KNN",
                          "!H:!V:nkNN=70:ScaleFrac=.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

    // H-Matrix (chi2-squared) method
    if (Use["HMatrix"])
      factory->BookMethod(dataloader, TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

    // Linear discriminant (same as Fisher discriminant)
    if (Use["LD"])
      factory->BookMethod(dataloader, TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

    // Fisher discriminant (same as LD)
    if (Use["Fisher"])
      factory->BookMethod(dataloader, TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

    // Fisher with Gauss-transformed input variables
    if (Use["FisherG"])
      factory->BookMethod(dataloader, TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

    // Composite classifier: ensemble (tree) of boosted Fisher classifiers
    if (Use["BoostedFisher"])
      factory->BookMethod(dataloader, TMVA::Types::kFisher, "BoostedFisher",
                          "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

    // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
    if (Use["FDA_MC"])
      factory->BookMethod(dataloader, TMVA::Types::kFDA, "FDA_MC",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

    if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod(dataloader, TMVA::Types::kFDA, "FDA_GA",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

    if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
      factory->BookMethod(dataloader, TMVA::Types::kFDA, "FDA_SA",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

    if (Use["FDA_MT"])
      factory->BookMethod(dataloader, TMVA::Types::kFDA, "FDA_MT",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

    if (Use["FDA_GAMT"])
      factory->BookMethod(dataloader, TMVA::Types::kFDA, "FDA_GAMT",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

    if (Use["FDA_MCMT"])
      factory->BookMethod(dataloader, TMVA::Types::kFDA, "FDA_MCMT",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

    // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
    if (Use["MLP"])
      factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP", "!H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator");
    //"!H:!V:VarTransform=N" );

    if (Use["MLP_MM"]) {
      TString network;
      network = "10,5,5"; //"20,10"; //"N,5,5"; //10,5,5
      factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP_MM",
                          Form("!H:!V:VarTransform=N:NCycles=600:HiddenLayers=%s:TestRate=5:!UseRegulator",
                               network.Data()));
      //:BPMode=batch:BatchSize=3e4:SamplingEpoch=1:TestRate=5:TrainingMethod=BFGS:!UseRegulator:CreateMVAPdfs=False
      //:LearningRate=1e-2:EstimatorType=MSE:NeuronType=sigmoid" );
    }
    if (Use["MLPBFGS"])
      factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

    if (Use["MLPBNN"])
      factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

    // CF(Clermont-Ferrand)ANN
    if (Use["CFMlpANN"])
      factory->BookMethod(dataloader, TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...

    // Tmlp(Root)ANN
    if (Use["TMlpANN"])
      factory->BookMethod( dataloader, TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

    // Support Vector Machine
    if (Use["SVM"])
      factory->BookMethod(dataloader, TMVA::Types::kSVM, "SVM", "Gamma=.5:Tol=0.001:C=1:VarTransform=Norm" );

    // Boosted Decision Trees
    if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod(dataloader, TMVA::Types::kBDT,"BDT",
                          "!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:CreateMVAPdfs=False" );

    if (Use["BDT_MM"]) { // Adaptive Boost
      TString options = "!H:!V:NTrees=800:MinNodeSize=0.1%:";
      options += Form("MaxDepth=%i",(selection_.Contains("_")) ? 4 : 5); //use more shallow tree in leptonic tau decays
      options += ":BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20";
      cout << "Using BDT options " << options.Data() << endl;
      //version until 03/23
      // "!H:!V:NTrees=800:MinNodeSize=0.1%:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20"
      //legacy version
      // "!H:!V:NTrees=400:MinNodeSize=1.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20"
      factory->BookMethod(dataloader, TMVA::Types::kBDT,"BDT_MM", options.Data());
    }

    if (Use["BDTRT"]) {  // Bagging Boost Random Trees
      TString bdtSetup = "!H:!V";
      bdtSetup += ":NTrees=1000:MinNodeSize=1%:MaxDepth=5:nCuts=100:UseNVars=5:UseRandomisedTrees=True";
      bdtSetup += ":BoostType=Bagging:BaggedSampleFraction=0.6:CreateMVAPdfs=False";
      // bdtSetup += ":PruneMethod=ExpectedError:PruneStrength=0.2";
      factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTRT",bdtSetup.Data());
    }

    if (Use["BDTG"]) // Gradient Boost
      factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
                          "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:NNodesMax=5" );

    if (Use["BDTB"]) // Bagging
      factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTB",
                          "!H:!V:NTrees=1000:MinNodeSize=1%:BoostType=Bagging:MaxDepth=4:SeparationType=GiniIndex:nCuts=50:PruneMethod=NoPruning" );

    if (Use["BDTD"]) // Decorrelation + Adaptive Boost
      factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTD",
                          "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate" );

    if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
      factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTMitFisher",
                          "!H:!V:NTrees=50:nEventsMin=150:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

    // RuleFit -- TMVA implementation of Friedman's method
    if (Use["RuleFit"])
      factory->BookMethod(dataloader, TMVA::Types::kRuleFit, "RuleFit",
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
    delete dataloader;
  } //end plotOnly_

   // Launch the GUI for the root macros
  if (!dryrun_&&!gROOT->IsBatch()) TMVA::TMVAGui( outfilename );
  else if(!dryrun_) { //if not a dry run and batch mode, make some relevant plots directly
    TMVA::variables(outFolder.Data(), outfilename.Data(), "InputVariables_Id"); //1D variable plots
    TMVA::mvas(outFolder.Data(), outfilename.Data(), TMVA::kCompareType); //Training + Testing MVA scores
    outputFile = TFile::Open( outfilename, "READ" );
    TTree* TestTree  = (outputFile) ? (TTree*) outputFile->Get(Form("tmva_%s/TestTree" , tname)) : nullptr;
    TTree* TrainTree = (outputFile) ? (TTree*) outputFile->Get(Form("tmva_%s/TrainTree", tname)) : nullptr;
    for(auto mva : Use) {
      if(Use[mva.first]) {
        print_spectator(mva.first, TestTree, TrainTree, outFolder);
        print_roc      (mva.first, TestTree, TrainTree, outFolder);
        if(selection_.Contains("tau")) {
          TString roc_cut = (selection_.EndsWith("tau")) ? "lepm > 50. && lepm < 110." : "lepm > 40. && lepm < 100.";
          print_roc    (mva.first, TestTree, TrainTree, outFolder, roc_cut);
        }
      }
    }
    if(doDetailedPlots_) { //only do detailed plots if requested
      TMVA::correlations(outFolder.Data(), outfilename.Data()); //linear correlations
      TMVA::efficiencies(outFolder.Data(), outfilename.Data()); //ROC
      //make scatter plots for all variables
      Tree_t tmp_tree_t; //not used
      auto vars = trkqualinit.GetVariables(selection_, tmp_tree_t);
      for(auto var : vars) {
        if(var.use_) TMVA::correlationscatters(outFolder.Data(), outfilename.Data(), var.var_.Data()); //2D correlations
        if(TestTree && TrainTree) { //make 1-D histograms of spectators and variables
          print_spectator(var.var_, TestTree, TrainTree, outFolder);
        } else {
          cout << "Can't plot variable " << var.var_.Data() << " due to missing trees!\n";
        }
      }
    }
    outputFile->Close();
  }
  if(multiTrainings_) {gSystem->cd(".."); inTrainDir_ = false; nprongs_ = -1;}
  return 0;
}
