//Script to calculate the 95% UL for e+mu resonance
bool doConstraints_ = false;
bool fixBkgParams_ = false;
bool useBinnedBkgFit_ = true;

Int_t calculate_UL_MVA(int set = 8, TString selection = "zmutau",
                       vector<int> years = {2016/*, 2017, 2018*/},
                       bool useToyData = false) {
  TString hist;
  if     (selection == "hmutau"  ) hist = "mva0";
  else if(selection == "zmutau"  ) hist = "mva1";
  else if(selection == "hetau"   ) hist = "mva2";
  else if(selection == "zetau"   ) hist = "mva3";
  else if(selection == "hemu"    ) hist = "mva4";
  else if(selection == "zemu"    ) hist = "mva5";
  else if(selection == "hmutau_e") hist = "mva6";
  else if(selection == "zmutau_e") hist = "mva7";
  else if(selection == "hetau_mu") hist = "mva8";
  else if(selection == "zetau_mu") hist = "mva9";
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }

  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  if(selec.Contains("_")) selec = "emu";
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("_e", ""); signame.ReplaceAll("_mu",""); signame.ReplaceAll("m", "M");
  signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }
  TFile* fInput = TFile::Open(Form("workspaces/hist_background_mva_%s_%s_%s_%i.root", selection.Data(), hist.Data(), year_string.Data(), set), "READ");
  if(!fInput) return 1;
  fInput->cd();
  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  if(!ws) return 2;
  RooRealVar* br_sig = ws->var("br_sig");
  if(!br_sig) return 2;

  RooArgList poi_list(*br_sig);
  RooArgList obs_list(*(ws->var(hist.Data())));
  RooDataSet* data = (RooDataSet*) ws->data("bkgMVAPDFData");
  if(!data) return 3;
  auto data_binned = data->binnedClone("data_binned", "data_binned");

  ws->Print();
  auto n_bkg  = ws->var("n_bkg");
  if(!n_bkg) return 4;

  RooArgList nuisance_params;
  nuisance_params.add(*n_bkg );
  if(doConstraints_)
    nuisance_params.add(*(ws->var("beta_eff")));

  RooArgList glb_list;
  glb_list.add(*(ws->var("global_eff")));

  //Set the model and let it know about the workspace contents
  RooStats::ModelConfig model;
  model.SetWorkspace(*ws);
  model.SetPdf((doConstraints_) ? "totMVAPDF_constr" : "totMVAPDF");
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  if(doConstraints_)
    model.SetGlobalObservables(glb_list);
  model.SetName("S+B Model");
  model.SetProtoData(*data_binned);

  auto bModel = model.Clone();
  bModel->SetName("B Model");
  double oldval = ((RooRealVar*) poi_list.find("br_sig"))->getVal();
  ((RooRealVar*) poi_list.find("br_sig"))->setVal(0); //BEWARE that the range of the POI has to contain zero!
  bModel->SetSnapshot(poi_list);
  ((RooRealVar*) poi_list.find("br_sig"))->setVal(oldval);

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
  double poimin = ((RooRealVar*) poi_list.find("br_sig"))->getMin();
  double poimax = ((RooRealVar*) poi_list.find("br_sig"))->getMax();

  double min_scan = (selection.Contains("z")) ? 1.e-6 : 1.e-5;
  double max_scan = (selection.Contains("z")) ? 2.e-5 : 1.e-2;
  std::cout << "Doing a fixed scan in the interval: " << min_scan << ", "
            << max_scan << " with " << npoints << " points\n";
  calc.SetFixedScan(npoints, min_scan, max_scan);

  auto result = calc.GetInterval();
  double upperLimit  = result->UpperLimit();
  double expectedUL  = result->GetExpectedUpperLimit( 0);
  double expectedULM = result->GetExpectedUpperLimit(-1);
  double expectedULP = result->GetExpectedUpperLimit( 1);
  std::cout << "##################################################\n"
            << "The observed upper limit is " << upperLimit << std::endl;

  //compute the expected limit
  std::cout << "Expected upper limits, using the alternate model: " << std::endl
            << " expected limit (median): " << expectedUL  << std::endl
            << " expected limit (-1 sig): " << expectedULM << std::endl
            << " expected limit (+1 sig): " << expectedULP << std::endl
            << "##################################################\n";

  //Plot the results
  RooStats::HypoTestInverterPlot freq_plot("HTI_Result_Plot","Expected CLs",result);
  //xPlot in a new canvas with style
  TCanvas* canvas = new TCanvas();
  canvas->cd();
  // if(useToyData)
  //   freq_plot.Draw();
  // else
  //   freq_plot.Draw("EXP");
  freq_plot.Draw();
  if(!useToyData) {
    TGraph* g = (TGraph*) canvas->GetPrimitive("CLs_observed");
    if(g) {
      g->SetMarkerSize(0);
      g->SetLineWidth(0);
      g->SetMarkerColorAlpha(0,0.);
    }
  }
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
  if(useToyData)
    label.DrawLatex(0.12, 0.26, Form("Observed 95%% CL = %.2e", upperLimit));

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%s_%s_%i.pdf", year_string.Data(), hist.Data(), selection.Data(), set));
  canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%s_%s_%i.png", year_string.Data(), hist.Data(), selection.Data(), set));

  cout << "Finished UL calculation, plotting dataset with expected UL...\n";

  //Plot background spectrum (with toy data) and signal with upper limit value
  auto totPDF = (RooAddPdf*) ws->pdf("totMVAPDF");
  auto bkgPDF = ws->pdf("bkgMVAPDF");
  auto sigPDF = ws->pdf("sigMVAPDF");
  auto n_sig = ws->function("n_sig");
  auto eff = ws->function("eff");
  auto lum = ws->var("lum_var");
  auto zxs = ws->var("bxs_var");
  auto mva = ws->var(hist.Data());
  auto xframe = mva->frame(RooFit::Title("Background + Signal distributions at 95\% CLs limit"));

  //set to expected upper limit
  br_sig->setVal(expectedUL);
  br_sig->setConstant(1);
  cout << "Fitting dataset with branching ratio set to upper limit...\n";
  totPDF->fitTo(*data);
  data->plotOn(xframe);

  cout << "Plotting the PDFs...\n";
  totPDF->plotOn(xframe);
  totPDF->plotOn(xframe, RooFit::Components("bkgMVAPDF"), RooFit::LineColor(kGreen-3), RooFit::LineStyle(kDashed));
  totPDF->plotOn(xframe, RooFit::Components("sigMVAPDF"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  TCanvas* cmass = new TCanvas();
  xframe->Draw();

  cmass->SaveAs(Form("plots/latest_production/%s/upperlimit_pdfs_%s_%s_%i.png", year_string.Data(), hist.Data(), selection.Data(), set));
  cout << "Printing variable information...\n";

  totPDF->Print();
  bkgPDF->Print();
  sigPDF->Print();
  if(br_sig) br_sig->Print();
  if(n_sig) n_sig->Print();
  if(eff) eff->Print();
  if(n_bkg) n_bkg->Print();
  if(lum) lum->Print();

  return 0;
}
