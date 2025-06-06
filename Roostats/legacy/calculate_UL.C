//Script to calculate the 95% UL for e+mu resonance
bool doConstraints_ = false;
bool fixBkgParams_ = false;

Int_t calculate_UL(int set = 8, vector<int> years = {2016, 2017, 2018},
		   bool doHiggs = false, bool useToyData = false) {
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }
  TFile* fInput = 0;
  if(doConstraints_)
    fInput = TFile::Open(Form("workspaces/fit_%s_lepm_background_constr_%s_%i.root", (doHiggs) ? "hemu" : "zemu", year_string.Data(), set), "READ");
  else
    fInput = TFile::Open(Form("workspaces/fit_%s_lepm_background_%s_%i.root", (doHiggs) ? "hemu" : "zemu", year_string.Data(), set), "READ");
  if(!fInput) return 1;
  fInput->cd();
  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  RooRealVar* br_emu = ws->var("br_emu");
  RooArgList poi_list(*br_emu);
  RooArgList obs_list(*(ws->var("lepm")));
  RooDataSet* data = (RooDataSet*) ws->data("bkgPDFData");
  auto data_binned = data->binnedClone("data_binned", "data_binned");

  ws->Print();
  auto a_bkg = ws->var((doHiggs) ? "a_bst3" : "a_bst4");
  auto b_bkg = ws->var((doHiggs) ? "b_bst3" : "b_bst4");
  auto c_bkg = ws->var((doHiggs) ? "c_bst3" : "c_bst4");
  auto d_bkg = ws->var((doHiggs) ? "d_bst3" : "d_bst4");
  auto n_bkg = ws->var("n_bkg");
  if(fixBkgParams_) {
    a_bkg->setConstant(1);
    b_bkg->setConstant(1);
    c_bkg->setConstant(1);
    if(!doHiggs)
      d_bkg->setConstant(1);
  }
  
  RooArgList nuisance_params;
  nuisance_params.add(*a_bkg);
  nuisance_params.add(*b_bkg);
  nuisance_params.add(*c_bkg);
  if(!doHiggs)
    nuisance_params.add(*d_bkg);
  nuisance_params.add(*n_bkg);
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

  double min_scan = (doHiggs) ? 5.e-7 : 1.e-9;
  double max_scan = (doHiggs) ? 1.e-3 : 5.e-6;
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
  // canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%s_%i.pdf", year_string.Data(), (doHiggs) ? "hemu" : "zemu", set));
  canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%s_%i.png", year_string.Data(), (doHiggs) ? "hemu" : "zemu", set));

  cout << "Finished UL calculation, plotting dataset with expected UL...\n";
  
  //Plot background spectrum (with toy data) and signal with upper limit value
  auto totPDF = (RooAddPdf*) ws->pdf("totPDF");
  auto bkgPDF = (doHiggs) ? ws->pdf("bkgPDF_bst3") : ws->pdf("bkgPDF_bst4");
  auto sigPDF = ws->pdf("morph_pdf_binned");
  if(!sigPDF) sigPDF = ws->pdf("sigpdf");
  auto n_sig = ws->function("n_sig");
  auto n_electron = ws->function("n_electron_var");
  auto n_muon = ws->function("n_muon_var");
  auto eff = ws->function("eff");
  auto lum = ws->var("lum_var");
  auto zxs = ws->var("bxs_var");
  auto lepm = ws->var("lepm");
  auto xframe = lepm->frame(RooFit::Title("Background + Signal distributions at 95\% CLs limit"));

  //set to expected upper limit
  br_emu->setVal(expectedUL);
  br_emu->setConstant(1);
  cout << "Fitting dataset with branching ratio set to upper limit...\n";
  totPDF->fitTo(*data);
  data->plotOn(xframe);

  cout << "Plotting the PDFs...\n";
  totPDF->plotOn(xframe);
  totPDF->plotOn(xframe, RooFit::Components((doHiggs) ? "bkgPDF_bst3" : "bkgPDF_bst4"), RooFit::LineColor(kGreen-3), RooFit::LineStyle(kDashed));
  totPDF->plotOn(xframe, RooFit::Components("sigpdf"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  TCanvas* cmass = new TCanvas();
  xframe->Draw();

  cmass->SaveAs(Form("plots/latest_production/%s/upperlimit_pdfs_%s_%i.png", year_string.Data(), (doHiggs) ? "hemu" : "zemu", set));
  cout << "Printing variable information...\n";

  totPDF->Print();
  bkgPDF->Print();
  sigPDF->Print();
  if(ws->pdf("sigpdf1")) ws->pdf("sigpdf1")->Print();
  if(ws->pdf("sigpdf2")) ws->pdf("sigpdf2")->Print();
  if(ws->var("fracsig")) ws->var("fracsig")->Print();
  n_sig->Print();
  br_emu->Print();
  if(n_electron)
    n_electron->Print();
  if(n_muon)
    n_muon->Print();
  eff->Print();
  if(lum)
    lum->Print();
  if(zxs)
    zxs->Print();
  n_bkg->Print();
  
  return 0;
}
