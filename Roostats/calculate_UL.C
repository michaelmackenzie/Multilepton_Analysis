//Script to calculate the 95% UL for e+mu resonance


Int_t calculate_UL(int set = 8, int year = 2016) {
  TFile* fInput = TFile::Open(Form("workspaces/fit_lepm_background_%i_%i.root", year, set), "READ");
  if(!fInput) return 1;
  fInput->cd();
  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  RooRealVar* br_emu = ws->var("br_emu");
  RooArgList poi_list(*br_emu);
  RooArgList obs_list(*(ws->var("lepm")));
  RooDataSet* data = (RooDataSet*) ws->data("bkgPDFData");
  auto data_binned = data->binnedClone("data_binned", "data_binned");

  ws->Print();

  RooArgList nuisance_params;
  nuisance_params.add(*(ws->var("a_bkg")));
  nuisance_params.add(*(ws->var("b_bkg")));
  nuisance_params.add(*(ws->var("c_bkg")));
  nuisance_params.add(*(ws->var("d_bkg")));
  nuisance_params.add(*(ws->var("N_bkg")));
  nuisance_params.add(*(ws->var("beta_eff")));

  RooArgList glb_list;
  glb_list.add(*(ws->var("global_eff")));

  //Set the model and let it know about the workspace contents
  RooStats::ModelConfig model;
  model.SetWorkspace(*ws);
  model.SetPdf("totPDF_constr");
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  model.SetGlobalObservables(glb_list);
  model.SetName("S+B Model");
  model.SetProtoData(*data_binned);

  auto bModel = model.Clone();
  bModel->SetName("B Model");
  double oldval = ((RooRealVar*) poi_list.find("br_emu"))->getVal();
  bModel->SetSnapshot(poi_list);
  ((RooRealVar*) poi_list.find("br_emu"))->setVal(oldval);

  RooStats::AsymptoticCalculator fc(*data_binned, *bModel, model);
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

  int npoints = 100; //number of points to scan
  //min and max for the scan (better to choose smaller intervals)
  double poimin = ((RooRealVar*) poi_list.find("br_emu"))->getMin();
  double poimax = ((RooRealVar*) poi_list.find("br_emu"))->getMax();

  double min_scan = 1.e-9;
  double max_scan = 1.e-6;
  std::cout << "Doing a fixed scan in the interval: " << min_scan << ", "
	    << max_scan << " with " << npoints << " points\n";
  calc.SetFixedScan(npoints, min_scan, max_scan);

  auto result = calc.GetInterval();
  double upperLimit = result->UpperLimit();

  std::cout << "#####################################################\n"
	    << "The observed uppoer limit is " << upperLimit << std::endl;

  //compute the expected limit
  std::cout << "Expectected uppoer limits, using the alternate model: " << std::endl
	    << " expected limit (median): " << result->GetExpectedUpperLimit( 0) << std::endl
	    << " expected limit (-1 sig): " << result->GetExpectedUpperLimit(-1) << std::endl
	    << " expected limit (+1 sig): " << result->GetExpectedUpperLimit( 1) << std::endl
	    << "#####################################################\n";
  return 0;
}
