//Script to calculate the 95% UL for e+mu resonance many times

bool doConstraints_ = false;

Int_t toyMC_limits(int set = 8, int year = 2016, int seed = 90) {
  int status(0);
  TFile* fInput = (doConstraints_) ? TFile::Open(Form("workspaces/fit_lepm_background_constr_%i_%i.root", year, set), "READ") :
    TFile::Open(Form("workspaces/fit_lepm_background_%i_%i.root", year, set), "READ");
  if(!fInput) return 1;
  fInput->cd();
  
  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");

  // ws->Print();
  //PDF to generate data
  auto gener_PDF = ws->pdf((doConstraints_) ? "totPDF_constr" : "totPDF");
  //PDF to fit
  auto fit_PDF = ws->pdf((doConstraints_) ? "totPDF_constr" : "totPDF");

  RooRealVar* br_emu = ws->var("br_emu");
  RooRealVar* lepm = ws->var("lepm");
  RooArgList poi_list(*br_emu);
  RooArgList obs_list(*lepm);
  // double orig_val = ((RooRealVar*) poi_list.find("br_emu"))->getVal();

  RooRealVar* a_bkg = ws->var("a_bkg");
  RooRealVar* b_bkg = ws->var("b_bkg");
  RooRealVar* c_bkg = ws->var("c_bkg");
  RooRealVar* d_bkg = ws->var("d_bkg");
  RooRealVar* n_bkg = ws->var("n_bkg");
  // double a_orig = a_bkg->getVal();
  // double b_orig = b_bkg->getVal();
  // double c_orig = c_bkg->getVal();
  // double d_orig = d_bkg->getVal();
  double n_orig = n_bkg->getVal();

  //reduce messaging
  RooMsgService::instance().setSilentMode(true);
  gErrorIgnoreLevel = kInfo;
  // RooMsgService::instance().getStream(1).removeTopic(RooFit::NumericIntegration);
  RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING) ;
  RooRandom::randomGenerator()->SetSeed(seed);
  TRandom3 rnd(seed+17);
  int n = rnd.Poisson(n_orig);
  cout << "Performing fit using n_bkg = " << n << endl;

  auto data = gener_PDF->generate(RooArgSet(*lepm), n);
  auto data_binned = data->binnedClone("data_binned", "data_binned");

  RooArgList nuisance_params;
  nuisance_params.add(*a_bkg);
  nuisance_params.add(*b_bkg);
  nuisance_params.add(*c_bkg);
  nuisance_params.add(*d_bkg);
  nuisance_params.add(*n_bkg);
  RooArgList glb_list;
  glb_list.add(*(ws->var("global_eff")));

  if(doConstraints_)
    nuisance_params.add(*(ws->var("beta_eff")));

  //Set the model and let it know about the workspace contents
  RooStats::ModelConfig model;
  model.SetWorkspace(*ws);
  model.SetPdf((doConstraints_) ? "totPDF_constr" : "totPDF");
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  if(doConstraints_)
    model.SetGlobalObservables(glb_list);
  model.SetName("S+B Model");
  model.SetProtoData(*data_binned);

  auto bModel = model.Clone();
  bModel->SetName("B Model");
  double oldval = ((RooRealVar*) poi_list.find("br_emu"))->getVal();
  ((RooRealVar*) poi_list.find("br_emu"))->setVal(0); //BEWARE that the range of the POI has to contain zero!
  bModel->SetSnapshot(poi_list);
  ((RooRealVar*) poi_list.find("br_emu"))->setVal(oldval);

  RooStats::AsymptoticCalculator fc(*data_binned, *bModel, model);
  fc.SetOneSided(1);
  fc.SetPrintLevel(0);
  //create a hypotest inverter passing the desired calculator
  RooStats::HypoTestInverter calc(fc);
  calc.SetConfidenceLevel(0.95);

  //use CLs
  calc.UseCLs(1);
  calc.SetVerbose(-10);
  //configure toy MC sampler
  auto toymc = fc.GetTestStatSampler();
  //set profile likelihood test statistics
  RooStats::ProfileLikelihoodTestStat profl(*(model.GetPdf()));
  //for CLs (bounded intervals) use one-sided profile likelihood
  profl.SetOneSided(1);
  //est the test statistic to use
  toymc->SetTestStatistic(&profl);

  int npoints = 100; //number of points to scan
  //min and max for the scan (better to choose smaller intervals)
  double poimin = ((RooRealVar*) poi_list.find("br_emu"))->getMin();
  double poimax = ((RooRealVar*) poi_list.find("br_emu"))->getMax();

  double min_scan = 1.e-9;
  double max_scan = 5.e-6;
  cout << "Doing a fixed scan in the interval: " << min_scan << ", "
       << max_scan << " with " << npoints << " points\n";
  calc.SetFixedScan(npoints, min_scan, max_scan);

  auto result = calc.GetInterval();
  double upperLimit = result->UpperLimit();
  double limitExpected = result->GetExpectedUpperLimit( 0);
  double limitMinus = result->GetExpectedUpperLimit(-1);
  double limitPlus = result->GetExpectedUpperLimit( 1);

  
  /* add results to histograms */

    
  cout << "##################################################\n"
       << "The observed upper limit is " << upperLimit << endl
    
    //compute the expected limit
       << "Expected upper limits, using the alternate model: " << std::endl
       << " expected limit (median): " <<  limitExpected << std::endl
       << " expected limit (-1 sig): " <<  limitMinus    << std::endl
       << " expected limit (+1 sig): " <<  limitPlus     << std::endl
       << "##################################################\n";


  /* plot results */
  //print to the file nfit, observed limit, median expected, -1 expected, +1 expected
  
  gSystem->Exec(Form("echo %i %.5e %.5e %.5e %.5e >> toyMC_limits_%i_%i.txt ",
		     n, upperLimit, limitExpected, limitMinus, limitPlus, year, set));
  // gSystem->Exec(Form("[ ! -d plots/latest_production/%i ] && mkdir -p plots/latest_production/%i", year, year));
  // canvas->SaveAs(Form("plots/latest_production/%i/pval_vs_br_%i.pdf", year, set));
  // canvas->SaveAs(Form("plots/latest_production/%i/pval_vs_br_%i.png", year, set));
  return status;
}
