//Script to calculate the 95% UL for e+mu resonance
bool doConstraints_ = false;

Int_t calculate_UL(int set = 8, int year = 2016) {
  TFile* fInput = (doConstraints_) ? TFile::Open(Form("workspaces/fit_lepm_background_constr_%i_%i.root", year, set), "READ") :
    TFile::Open(Form("workspaces/fit_lepm_background_%i_%i.root", year, set), "READ");
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
  nuisance_params.add(*(ws->var("n_bkg")));
  if(doConstraints_)
    nuisance_params.add(*(ws->var("beta_eff")));

  RooArgList glb_list;
  glb_list.add(*(ws->var("global_eff")));

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
  //min and max for the scan (better to choose smaller intervals)
  double poimin = ((RooRealVar*) poi_list.find("br_emu"))->getMin();
  double poimax = ((RooRealVar*) poi_list.find("br_emu"))->getMax();

  double min_scan = 1.e-9;
  double max_scan = 1.e-5;
  std::cout << "Doing a fixed scan in the interval: " << min_scan << ", "
	    << max_scan << " with " << npoints << " points\n";
  calc.SetFixedScan(npoints, min_scan, max_scan);

  auto result = calc.GetInterval();
  double upperLimit = result->UpperLimit();

  std::cout << "##################################################\n"
	    << "The observed upper limit is " << upperLimit << std::endl;

  //compute the expected limit
  std::cout << "Expected upper limits, using the alternate model: " << std::endl
	    << " expected limit (median): " << result->GetExpectedUpperLimit( 0) << std::endl
	    << " expected limit (-1 sig): " << result->GetExpectedUpperLimit(-1) << std::endl
	    << " expected limit (+1 sig): " << result->GetExpectedUpperLimit( 1) << std::endl
	    << "##################################################\n";

  //Plot the results
  RooStats::HypoTestInverterPlot freq_plot("HTI_Result_Plot","Frequentist scan result for the W -> pigamma BR",result);
  //xPlot in a new canvas with style
  TCanvas* canvas = new TCanvas();
  canvas->cd();
  freq_plot.Draw();
  canvas->SetLogx();
  //freq_plot.Draw("EXP")
  gSystem->Exec(Form("[ ! -d plots/latest_production/%i ] && mkdir -p plots/latest_production/%i", year, year));
  canvas->SaveAs(Form("plots/latest_production/%i/pval_vs_br_%i.pdf", year, set));
  canvas->SaveAs(Form("plots/latest_production/%i/pval_vs_br_%i.png", year, set));
  return 0;
}
