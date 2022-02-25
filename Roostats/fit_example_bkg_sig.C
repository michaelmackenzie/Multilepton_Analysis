void fit_example_bkg_sig() {

  // POI
  RooRealVar mass("mass", "mass", 70., 110., "GeV/c^{2}");
  mass.setRange("LowSideband" , 70., 86.);
  mass.setRange("HighSideband", 96., 110.);
  mass.setRange("Full", 70., 110.);
  mass.setBins(80);

  // Make a generator model
  double true_val = -0.05;
  RooRealVar tau("tau", "The exponent", true_val, -10., -0.001);
  RooExponential exp("exp", "A falling exponential function", mass, tau);

  // Generate some toy data
  RooDataHist* data = exp.generateBinned(RooArgSet(mass), 1.65e5);
  // shift tau away to re-fit later
  tau.setVal(-1.);

  // Make a fit background model
  RooRealVar a("a", "a", 2.03, -5., 5.);
  RooRealVar b("b", "b", 0.65, -5., 5.);
  RooRealVar c("c", "c", 0.40, -5., 5.);
  RooRealVar d("d", "d", 0.21, -5., 5.);
  RooBernstein bkgPDF("bkgPDF", "bkgPDF", mass, RooArgSet(a,b,c,d));
  RooRealVar n_bkg("n_bkg", "n_bkg", 500., 0., 1e6);

  // Make a signal model
  RooRealVar mean("mean", "mean", 91.2, 90., 100.); mean.setConstant(1);
  RooRealVar sigma("sigma", "sigma", 2., 0.1, 100.); sigma.setConstant(1);
  RooGaussian sigPDF("sigPDF", "sigPDF", mass, mean, sigma);
  RooRealVar n_sig("n_sig", "n_sig", 0., -1e4, 1e4);

  // Make a total PDF
  RooAddPdf totPDF("totPDF", "totPDF", RooArgList(sigPDF, bkgPDF), RooArgList(n_sig, n_bkg));

  // Fit the toy data
  n_sig.setVal(0.); n_sig.setConstant(1);
  totPDF.fitTo(*data);
  n_sig.setConstant(0);

  auto xframe = mass.frame();
  data->plotOn(xframe);

  double old_val = tau.getVal();
  tau.setVal(true_val);
  exp.plotOn(xframe, RooFit::Range("Full"));
  tau.setVal(old_val);
  n_sig.setVal(2000.);
  totPDF.plotOn(xframe, RooFit::Range("Full"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  totPDF.plotOn(xframe, RooFit::Range("Full"), RooFit::Components("sigPDF"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

  auto c1 = new TCanvas();
  xframe->Draw();

  //Create a workspace
  RooWorkspace ws("ws");
  ws.import(totPDF);
  ws.import(*data);

  //Set the model and let it know about the workspace contents
  RooArgList nuisance_params;
  nuisance_params.add(a);
  nuisance_params.add(b);
  nuisance_params.add(c);
  nuisance_params.add(d);
  nuisance_params.add(n_bkg);
  RooArgList poi_list(n_sig);

  RooStats::ModelConfig model;
  model.SetWorkspace(ws);
  model.SetPdf("totPDF");
  model.SetParametersOfInterest(poi_list);
  model.SetNuisanceParameters(nuisance_params);
  model.SetObservables(mass);
  model.SetName("S+B Model");
  model.SetProtoData(*data);

  auto bModel = model.Clone();
  bModel->SetName("B Model");
  n_sig.setVal(0.);
  bModel->SetSnapshot(poi_list);

  RooStats::AsymptoticCalculator fc(*data, *bModel, model);
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
  RooStats::ProfileLikelihoodTestStat profl(*model.GetPdf());
  //for CLs (bounded intervals) use one-sided profile likelihood
  profl.SetOneSided(1);
  //est the test statistic to use
  toymc->SetTestStatistic(&profl);

  int npoints = 100; //number of points to scan
  double min_scan = 0;
  double max_scan = 1e3;
  std::cout << "Doing a fixed scan in the interval: " << min_scan << ", "
            << max_scan << " with " << npoints << " points\n";
  calc.SetFixedScan(npoints, min_scan, max_scan);

  calc.SetVerbose(-1);
  auto result = calc.GetInterval();
  double upperLimit  = result->UpperLimit();
  double expectedUL  = result->GetExpectedUpperLimit( 0);
  double expectedULM = result->GetExpectedUpperLimit(-1);
  double expectedULP = result->GetExpectedUpperLimit( 1);
  cout << "##################################################\n"
       << "The observed upper limit is " << upperLimit << endl
       << "Expected upper limits, using the alternate model: " << endl
       << " expected limit (median): " << expectedUL  << endl
       << " expected limit (-1 sig): " << expectedULM << endl
       << " expected limit (+1 sig): " << expectedULP << endl
       << "##################################################\n";

  cout << " N(bkg) = " << n_bkg.getVal() << " N(data) = " << data->sumEntries() << endl;

  //Plot the results
  RooStats::HypoTestInverterPlot freq_plot("HTI_Result_Plot","Expected CLs",result);
  //xPlot in a new canvas with style
  TCanvas* canvas = new TCanvas();
  canvas->cd();
  freq_plot.Draw();
  canvas->SetLogx();
  //Add upper limit info to the plot
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.04);
  label.SetTextColor(kRed);
  label.SetTextAlign(13);
  label.DrawLatex(0.12, 0.34, Form("Expected 95%% CL = %.2e^{+%.2e}_{-%.2e}",
                                   expectedUL, expectedULP-expectedUL, expectedUL-expectedULM));
  label.DrawLatex(0.12, 0.26, Form("Observed 95%% CL = %.2e", upperLimit));
}
