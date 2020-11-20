//Script to calculate the 95% UL for e+mu resonance
bool doConstraints_ = false;

Int_t calculate_UL(int set = 8, vector<int> years = {2016}, bool useToyData = false) {
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }
  TFile* fInput = (doConstraints_) ? TFile::Open(Form("workspaces/fit_lepm_background_constr_%s_%i.root", year_string.Data(), set), "READ") :
    TFile::Open(Form("workspaces/fit_lepm_background_%s_%i.root", year_string.Data(), set), "READ");
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
  double max_scan = 5.e-6;
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
  canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%i.pdf", year_string.Data(), set));
  canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%i.png", year_string.Data(), set));


  //Plot background spectrum (with toy data) and signal with upper limit value
  auto totPDF = (RooAddPdf*) ws->pdf("totPDF");
  auto bkgPDF = ws->pdf("bkgPDF");
  auto sigPDF = ws->pdf("morph_pdf_binned");
  auto n_sig = ws->function("n_sig");
  auto n_electron = ws->function("n_electron_var");
  auto n_muon = ws->function("n_muon_var");
  auto eff = ws->function("eff");
  auto lum = ws->var("lum_var");
  auto zxs = ws->var("zxs_var");
  auto n_bkg = ws->var("n_bkg");
  auto lepm = ws->var("lepm");
  auto xframe = lepm->frame(RooFit::Title("Background + Signal distributions at 95\% CLs limit"));

  //set to expected upper limit
  br_emu->setVal(expectedUL);
  br_emu->setConstant(1);
  totPDF->fitTo(*data);
  data->plotOn(xframe);
  double sig_scale = n_sig->getVal()/(n_sig->getVal()+n_bkg->getVal());
  double bkg_scale = n_bkg->getVal()/(n_sig->getVal()+n_bkg->getVal());  

  totPDF->plotOn(xframe);
  bkgPDF->plotOn(xframe, RooFit::LineColor(kGreen-3), RooFit::LineStyle(kDashed),
		 RooFit::Normalization(bkg_scale));
  sigPDF->plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed),
		 RooFit::Normalization(sig_scale));
  TCanvas* cmass = new TCanvas();
  xframe->Draw();

  cmass->SaveAs(Form("plots/latest_production/%s/upperlimit_pdfs_%i.png", year_string.Data(), set));
  
  totPDF->Print();
  bkgPDF->Print();
  sigPDF->Print();
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
  
  // std::cout << "PDF list: " << totPDF->pdfList() << std::endl;
  return 0;
}
