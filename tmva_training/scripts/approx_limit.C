//Approximate the upper limit on the branching fraction using MVA output trees

TString get_file_name(TString selection, int set, vector<int> years, int version) {
  TString file = "training_background_clfv_";
  bool ishiggs = selection.Contains("h");
  if(ishiggs) file += "higgs_";
  else        file += "Z0_";
  selection.ReplaceAll("z", ""); selection.ReplaceAll("h","");
  file += "nano_" + selection + "_";
  for(int year : years) {
    file += year;
    file += "_";
  }
  file += set;
  if(version > -1) file += Form("_v%i", version);
  return file;
}

int approx_limit(TString selection = "zmutau", int version = -1,
                 int Mode = 0,
                 TString MVA = "BDT_MM",
                 int set = 8, vector<int> years = {2016, 2017, 2018}) {

  //Retrieve the training names
  TString nHad = get_file_name(selection, set, years, version);
  TString nLep = get_file_name(selection+(selection.Contains("mutau") ? "_e" : "_mu"), set, years, version);
  //Get the training files
  TFile* fHad = TFile::Open(("../" + nHad + "/" + nHad + ".root").Data(), "READ");
  TFile* fLep = TFile::Open(("../" + nLep + "/" + nLep + ".root").Data(), "READ");
  if(!fHad || !fLep) return 1;

  //Get the TTrees
  TTree* tTrainHad = (TTree*) fHad->Get(("tmva_" + nHad + "/TrainTree").Data());
  TTree* tTestHad  = (TTree*) fHad->Get(("tmva_" + nHad + "/TestTree").Data());
  if(!tTrainHad || !tTestHad) return 2;
  tTrainHad->SetName("TrainHad");
  tTestHad->SetName("TestHad");
  TTree* tTrainLep = (TTree*) fLep->Get(("tmva_" + nLep + "/TrainTree").Data());
  TTree* tTestLep  = (TTree*) fLep->Get(("tmva_" + nLep + "/TestTree").Data());
  if(!tTrainLep || !tTestLep) return 3;
  tTrainLep->SetName("TrainLep");
  tTestLep->SetName("TestLep");

  TH1D* hBkgTrainHad = new TH1D("hBkgTrainHad", "hBkgTrainHad", 50, -1., 1.);
  TH1D* hBkgTestHad  = new TH1D("hBkgTestHad" , "hBkgTestHad" , 50, -1., 1.);
  TH1D* hBkgTrainLep = new TH1D("hBkgTrainLep", "hBkgTrainLep", 50, -1., 1.);
  TH1D* hBkgTestLep  = new TH1D("hBkgTestLep" , "hBkgTestLep" , 50, -1., 1.);
  TH1D* hSigTrainHad = new TH1D("hSigTrainHad", "hSigTrainHad", 50, -1., 1.);
  TH1D* hSigTestHad  = new TH1D("hSigTestHad" , "hSigTestHad" , 50, -1., 1.);
  TH1D* hSigTrainLep = new TH1D("hSigTrainLep", "hSigTrainLep", 50, -1., 1.);
  TH1D* hSigTestLep  = new TH1D("hSigTestLep" , "hSigTestLep" , 50, -1., 1.);

  tTrainHad->Draw(Form("%s >> hBkgTrainHad", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hBkgTestHad ", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hBkgTrainLep", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hBkgTestLep ", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTrainHad->Draw(Form("%s >> hSigTrainHad", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hSigTestHad ", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hSigTrainLep", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hSigTestLep ", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");

  double nBkgTrainHad = hBkgTrainHad->Integral();
  double nBkgTestHad  = hBkgTestHad ->Integral();
  double nBkgTrainLep = hBkgTrainLep->Integral();
  double nBkgTestLep  = hBkgTestLep ->Integral();
  double nSigTrainHad = hSigTrainHad->Integral();
  double nSigTestHad  = hSigTestHad ->Integral();
  double nSigTrainLep = hSigTrainLep->Integral();
  double nSigTestLep  = hSigTestLep ->Integral();

  //Plot the histograms
  TCanvas* c_bkg = new TCanvas("c_bkg", "c_bkg", 1400, 600);
  c_bkg->Divide(2,1);
  hBkgTrainHad->Scale(1./nBkgTrainHad);
  hBkgTestHad ->Scale(1./nBkgTestHad );
  hBkgTrainLep->Scale(1./nBkgTrainLep);
  hBkgTestLep ->Scale(1./nBkgTestLep );
  hSigTrainHad->Scale(1./nSigTrainHad);
  hSigTestHad ->Scale(1./nSigTestHad );
  hSigTrainLep->Scale(1./nSigTrainLep);
  hSigTestLep ->Scale(1./nSigTestLep );
  hBkgTrainHad->SetLineWidth(2); hBkgTrainHad->SetFillStyle(3005);
  hBkgTestHad ->SetLineWidth(2); hBkgTestHad ->SetFillStyle(3005);
  hBkgTrainLep->SetLineWidth(2); hBkgTrainLep->SetFillStyle(3005);
  hBkgTestLep ->SetLineWidth(2); hBkgTestLep ->SetFillStyle(3005);
  hSigTrainHad->SetLineWidth(2); hSigTrainHad->SetFillStyle(3004);
  hSigTestHad ->SetLineWidth(2); hSigTestHad ->SetFillStyle(3004);
  hSigTrainLep->SetLineWidth(2); hSigTrainLep->SetFillStyle(3004);
  hSigTestLep ->SetLineWidth(2); hSigTestLep ->SetFillStyle(3004);
  hBkgTrainHad->SetLineColor(kRed   ); hBkgTrainHad->SetFillColor(kRed   );
  hBkgTestHad ->SetLineColor(kRed   ); hBkgTestHad ->SetFillColor(kRed   );
  hBkgTrainLep->SetLineColor(kBlue  ); hBkgTrainLep->SetFillColor(kBlue  );
  hBkgTestLep ->SetLineColor(kBlue  ); hBkgTestLep ->SetFillColor(kBlue  );
  hSigTrainHad->SetLineColor(kViolet); hSigTrainHad->SetFillColor(kViolet);
  hSigTestHad ->SetLineColor(kViolet); hSigTestHad ->SetFillColor(kViolet);
  hSigTrainLep->SetLineColor(kGreen ); hSigTrainLep->SetFillColor(kGreen );
  hSigTestLep ->SetLineColor(kGreen ); hSigTestLep ->SetFillColor(kGreen );
  gStyle->SetOptStat(0);
  c_bkg->cd(1);
  hSigTrainHad->Draw("hist"); hSigTrainHad->SetTitle("Hadronic MVA scores");
  hSigTestHad ->Draw("E1 same");
  hBkgTrainHad->Draw("hist same");
  hBkgTestHad ->Draw("E1 same");
  c_bkg->cd(2);
  hSigTrainLep->Draw("hist"); hSigTrainLep->SetTitle("Leptonic MVA scores");
  hSigTestLep ->Draw("E1 same");
  hBkgTrainLep->Draw("hist same");
  hBkgTestLep ->Draw("E1 same");

  //////////////////////////////////////////////////
  // Perform upper limit estimate
  //////////////////////////////////////////////////

  //Make Roo variables
  RooRealVar mvaHad("mvaHad", "mvaHad", -1., 1.); mvaHad.setBins(50);
  RooRealVar mvaLep("mvaLep", "mvaLep", -1., 1.); mvaLep.setBins(50);
  RooRealVar nSigHad("nSigHad", "nSigHad", nSigTrainHad+nSigTestHad, -1e5, 1e5); nSigHad.setConstant(1);
  RooRealVar nSigLep("nSigLep", "nSigLep", nSigTrainLep+nSigTestLep, -1e5, 1e5); nSigLep.setConstant(1);
  RooRealVar nBkgHad("nBkgHad", "nBkgHad", nBkgTrainHad+nBkgTestHad,  1e4, 1e7);
  RooRealVar nBkgLep("nBkgLep", "nBkgLep", nBkgTrainLep+nBkgTestLep,  1e4, 1e7);
  RooRealVar sigScale("sigScale", "sigScale", 0., -10., 10.);
  RooFormulaVar nEffSigHad("nEffSigHad", "@0 * @1", RooArgList(sigScale, nSigHad));
  RooFormulaVar nEffSigLep("nEffSigLep", "@0 * @1", RooArgList(sigScale, nSigLep));

  //Make RooDataHists and RooHistPdfs
  TH1D *hSigPdfHad, *hSigPdfLep, *hBkgPdfHad, *hBkgPdfLep;
  if(Mode == 0) { //use testing distributions
    hSigPdfHad = (TH1D*) hSigTestHad->Clone("hSigPdfHad");
    hSigPdfLep = (TH1D*) hSigTestLep->Clone("hSigPdfLep");
    hBkgPdfHad = (TH1D*) hBkgTestHad->Clone("hBkgPdfHad");
    hBkgPdfLep = (TH1D*) hBkgTestLep->Clone("hBkgPdfLep");
  } else if(Mode == 1) { //use training distributions
    hSigPdfHad = (TH1D*) hSigTrainHad->Clone("hSigPdfHad");
    hSigPdfLep = (TH1D*) hSigTrainLep->Clone("hSigPdfLep");
    hBkgPdfHad = (TH1D*) hBkgTrainHad->Clone("hBkgPdfHad");
    hBkgPdfLep = (TH1D*) hBkgTrainLep->Clone("hBkgPdfLep");
  } else if(Mode == 2) { //use averaged distributions
    hSigPdfHad = (TH1D*) hSigTrainHad->Clone("hSigPdfHad"); hSigPdfHad->Add(hSigTestHad); hSigPdfHad->Scale(0.5);
    hSigPdfLep = (TH1D*) hSigTrainLep->Clone("hSigPdfLep"); hSigPdfLep->Add(hSigTestLep); hSigPdfLep->Scale(0.5);
    hBkgPdfHad = (TH1D*) hBkgTrainHad->Clone("hBkgPdfHad"); hBkgPdfHad->Add(hBkgTestHad); hBkgPdfHad->Scale(0.5);
    hBkgPdfLep = (TH1D*) hBkgTrainLep->Clone("hBkgPdfLep"); hBkgPdfLep->Add(hBkgTestLep); hBkgPdfLep->Scale(0.5);
  }
  RooDataHist bkgDataHad("bkgDataHad", "bkgDataHad", mvaHad, hBkgPdfHad);
  RooDataHist bkgDataLep("bkgDataLep", "bkgDataLep", mvaLep, hBkgPdfLep);
  RooDataHist sigDataHad("sigDataHad", "sigDataHad", mvaHad, hSigPdfHad);
  RooDataHist sigDataLep("sigDataLep", "sigDataLep", mvaLep, hSigPdfLep);
  RooHistPdf  bkgPdfHad ("bkgPdfHad" , "bkgPdfHad" , mvaHad, bkgDataHad);
  RooHistPdf  bkgPdfLep ("bkgPdfLep" , "bkgPdfLep" , mvaLep, bkgDataLep);
  RooHistPdf  sigPdfHad ("sigPdfHad" , "sigPdfHad" , mvaHad, sigDataHad);
  RooHistPdf  sigPdfLep ("sigPdfLep" , "sigPdfLep" , mvaLep, sigDataLep);

  //Make the total category PDFs
  RooAddPdf totHadPdf("totHadPdf", "totHadPdf", RooArgList(sigPdfHad, bkgPdfHad), RooArgList(nEffSigHad, nBkgHad));
  RooAddPdf totLepPdf("totLepPdf", "totLepPdf", RooArgList(sigPdfLep, bkgPdfLep), RooArgList(nEffSigLep, nBkgLep));

  //Define the two categories
  RooCategory categories("categories", "categories");
  categories.defineType("hadronic", 0);
  categories.defineType("leptonic", 1);

  //Generate toy background only data
  RooDataHist* gen_had = bkgPdfHad.generateBinned(mvaHad, nBkgHad.getVal());
  RooDataHist* gen_lep = bkgPdfLep.generateBinned(mvaLep, nBkgLep.getVal());
  map<string, RooDataHist*> dataMap;
  dataMap["hadronic"] = gen_had;
  dataMap["leptonic"] = gen_lep;
  RooDataHist combined_gen_data("combined_gen_data", "combined_gen_data", RooArgList(mvaHad, mvaLep), categories, dataMap);

  //Make the total overall PDF, combining the categories
  RooSimultaneous totPDF("totPDF", "The total PDF", categories);
  totPDF.addPdf(totHadPdf, "hadronic");
  totPDF.addPdf(totLepPdf, "leptonic");

  //Create a workspace
  RooWorkspace ws("ws");
  ws.import(totPDF);

  //Create calculator to do the upper limit
  RooArgList nuisance_params(nBkgHad, nBkgLep);
  RooArgList poi_list(sigScale);
  RooArgList obs_list(mvaHad, mvaLep, categories);
  RooStats::ModelConfig model;
  model.SetWorkspace(ws);
  model.SetPdf("totPDF");
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  model.SetName("S+B Model");
  model.SetProtoData(combined_gen_data);
  auto bModel = model.Clone();
  bModel->SetName("B Model");
  ((RooRealVar*) poi_list.find("sigScale"))->setVal(0.); //BEWARE that the range of the POI has to contain zero!
  bModel->SetSnapshot(poi_list);
  ((RooRealVar*) poi_list.find("sigScale"))->setVal(1.);

  RooStats::AsymptoticCalculator fc(combined_gen_data, *bModel, model, true);
  fc.SetOneSided(1);
  //create a hypotest inverter passing the desired calculator
  RooStats::HypoTestInverter calc(fc);
  calc.SetConfidenceLevel(0.95);

  //use CLs
  calc.UseCLs(1);
  calc.SetVerbose(0);
  //configure toy MC sampler
  auto toymc = fc.GetTestStatSampler();
  //set profile likelihood test statistics
  RooStats::ProfileLikelihoodTestStat profl(*(model.GetPdf()));
  //for CLs (bounded intervals) use one-sided profile likelihood
  profl.SetOneSided(1);
  //est the test statistic to use
  toymc->SetTestStatistic(&profl);

  int npoints = 200; //number of points to scan
  calc.SetFixedScan(npoints, 1.e-3, 10, true); //bool is for log scan vs linear scan

  CrossSections xs;
  double bxs = selection.Contains("h") ? xs.GetCrossSection("H") : xs.GetCrossSection("Z");
  TString signame = selection;
  signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("m", "M");
  double sigxs = xs.GetCrossSection(signame.Data());
  auto result = calc.GetInterval();
  double upperLimit  = result->UpperLimit()*sigxs/bxs;
  double expectedUL  = result->GetExpectedUpperLimit( 0)*sigxs/bxs;
  double expectedULM = result->GetExpectedUpperLimit(-1)*sigxs/bxs;
  double expectedULP = result->GetExpectedUpperLimit( 1)*sigxs/bxs;
  std::cout << "##################################################\n"
            << "The observed upper limit is " << upperLimit << std::endl;

  //compute the expected limit
  std::cout << "Expected upper limits, using the alternate model: " << std::endl
            << " expected limit (median): " << expectedUL  << std::endl
            << " expected limit (-1 sig): " << expectedULM << std::endl
            << " expected limit (+1 sig): " << expectedULP << std::endl
            << "##################################################\n";
  return 0;
}
