//Script to calculate the 95% UL for e+mu resonance using binned fits
bool doConstraints_ = true;
bool use_bernstein_ = true;
bool blind_data_ = true;
bool use_data_in_calc_ = true;

Int_t calculate_UL_bemu_binned(vector<int> sets = {8}, TString selection = "zemu",
                               vector<int> years = {2016, 2017, 2018},
                               bool useToyData = false) {
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }
  TString set_str = "";
  for(int set : sets) {
    if(set_str == "") set_str += set;
    else {set_str += "_"; set_str += set;}
  }
  bool doHiggs = selection.Contains("h");
  TFile* fInput = TFile::Open(Form("workspaces/fit_%s_lepm_background_binned%s_%s_%s.root", selection.Data(),
                                   (doConstraints_) ? "_constr" : "",
                                   year_string.Data(), set_str.Data()), "READ");
  if(!fInput) return 1;
  fInput->cd();
  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  if(!ws) {cout << "Workspace not found!\n"; return -1;}
  ws->Print();
  RooRealVar* br_sig = ws->var("br_sig");
  RooArgList poi_list(*br_sig);
  RooArgList obs_list(*(ws->var("lepm")));
  obs_list.add(*((RooCategory*) ws->obj(selection.Data())));
  RooDataHist* data = (RooDataHist*) ws->data((use_data_in_calc_) ? "combined_data" : "combined_toy_data");
  if(!data) {cout << "Data not found!\n"; return 2;}
  double bmass = (doHiggs) ? 125. : 91.;
  RooDataHist* data_sig = (RooDataHist*) data->reduce(Form("lepm > %.3f && lepm < %.3f", bmass - 5., bmass + 5.));
  double ndata_sig = data_sig->sumEntries();
  RooAbsPdf* totPDF = ws->pdf("totPDF");
  // auto data_binned = data->binnedClone("data_binned", "data_binned");
  //loop through each category, getting nuisance parameters
  RooArgList nuisance_params;
  if(doConstraints_)
    nuisance_params.add(*(ws->var("br_sig_beta")));
  int index = 0;
  for(int set : sets) {
    auto setPDF = ws->pdf(Form("bkgPDF_%i", set));
    setPDF->Print();
    TString var_name = (use_bernstein_) ? "bst" : "cheb";
    TString title = setPDF->getTitle();
    cout << "PDF has title " << title.Data() << endl;
    title.ReplaceAll("bkgPDF_", ""); title.ReplaceAll(var_name.Data(), "");
    int nvar = title.Atoi();
    var_name += nvar;
    char variable[] = "a";
    for(int ivar = 0; ivar < nvar; ++ivar) {
      TString x_bkg_name = Form("%s_%s_%i", TString(variable).Data(), var_name.Data(), set);
      auto var = ws->var(x_bkg_name.Data());
      if(var) nuisance_params.add(*var);
      else {cout << "PDF variable " << x_bkg_name.Data() << " not found for set " << set << "!\n"; return 3;}
      variable[0] += 1;
    }
    ++index;
  }

  RooArgList glb_list;
  if(doConstraints_) {
    glb_list.add(*(ws->var("br_sig_kappa")));
  }

  //Set the model and let it know about the workspace contents
  RooStats::ModelConfig model;
  model.SetWorkspace(*ws);
  model.SetPdf("totPDF");
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  if(doConstraints_)
    model.SetGlobalObservables(glb_list);
  model.SetName("S+B Model");
  model.SetProtoData(*data);

  br_sig->setVal((doHiggs) ? 1.e-4 : 3.e-7);
  cout << endl << endl << "##################################################################-" << endl
       << "--- N(sig) vs N(bkg) at " << br_sig->getVal() << ":\n";
  for(int set : sets) {
    auto n_sig = ws->function(Form("n_sig_%i", set));
    double nsig = n_sig->getVal();
    auto n_bkg = ws->var(Form("n_bkg_%i", set));
    double nbkg = n_bkg->getVal();
    std::cout << " set " << set << ": N(sig) = " << nsig << " N(bkg) = " << nbkg << " N(data region) = " << ndata_sig << std::endl;
  }
  cout << "##################################################################" << endl << endl << endl;

  auto bModel = model.Clone();
  bModel->SetName("B Model");
  double oldval = br_sig->getVal();
  br_sig->setVal(0); //BEWARE that the range of the POI has to contain zero!
  bModel->SetSnapshot(poi_list);
  br_sig->setVal(oldval);

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
  RooStats::ProfileLikelihoodTestStat profl(*(model.GetPdf()));
  //for CLs (bounded intervals) use one-sided profile likelihood
  profl.SetOneSided(1);
  //est the test statistic to use
  toymc->SetTestStatistic(&profl);

  int npoints = 200; //number of points to scan
  //min and max for the scan (better to choose smaller intervals)
  // double poimin = ((RooRealVar*) poi_list.find("br_sig"))->getMin();
  // double poimax = ((RooRealVar*) poi_list.find("br_sig"))->getMax();

  double min_scan = (doHiggs) ? 1.e-6 : 1.e-8;
  double max_scan = (doHiggs) ? 5.e-4 : 1.e-6;
  std::cout << "Doing a fixed scan in the interval: " << min_scan << ", "
            << max_scan << " with " << npoints << " points\n";
  calc.SetFixedScan(npoints, min_scan, max_scan);

  calc.SetVerbose(-1);
  auto result = calc.GetInterval();
  double upperLimit  = result->UpperLimit();
  double expectedUL  = result->GetExpectedUpperLimit( 0);
  double expectedULM = result->GetExpectedUpperLimit(-1);
  double expectedULP = result->GetExpectedUpperLimit( 1);
  std::cout << "##################################################\n";
  if(!blind_data_ || !use_data_in_calc_)
  cout << "The observed upper limit is " << upperLimit << std::endl;

  //compute the expected limit
  std::cout << "Expected upper limits, using the alternate model: " << std::endl
            << " expected limit (median): " << expectedUL  << std::endl
            << " expected limit (-1 sig): " << expectedULM << std::endl
            << " expected limit (+1 sig): " << expectedULP << std::endl
            << "##################################################\n";


  br_sig->setVal(upperLimit);
  if(!use_data_in_calc_ || !blind_data_) {
    cout << endl << endl << "##################################################################-" << endl
         << "--- N(sig) vs N(bkg) at " << br_sig->getVal() << ":\n";
    for(int set : sets) {
      auto n_sig = ws->function(Form("n_sig_%i", set));
      double nsig = n_sig->getVal();
      auto n_bkg = ws->var(Form("n_bkg_%i", set));
      double nbkg = n_bkg->getVal();
      cout << "--- Set " << set << ": N(sig) = " << nsig << " N(bkg) = " << nbkg << " N(data region) = " << ndata_sig << endl;
    }
    cout << "##################################################################" << endl << endl << endl;
  }

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
  canvas->SaveAs(Form("plots/latest_production/%s/pval_vs_br_%s_binned%s_%s.png", year_string.Data(), selection.Data(),
                      (doConstraints_) ? "_constr" : "",
                      set_str.Data()));

  cout << "Finished UL calculation, plotting dataset with expected UL...\n";

  //Plot background spectrum (with toy data) and signal with upper limit value
  // auto totPDF = (RooAddPdf*) ws->pdf("totPDF");
  // auto bkgPDF = (doHiggs) ? ws->pdf("bkgPDF_bst3") : ws->pdf("bkgPDF_bst4");
  // auto sigPDF = ws->pdf("morph_pdf_binned");
  // if(!sigPDF) sigPDF = ws->pdf("sigPDF");
  // auto n_sig = ws->function("n_sig");
  // auto eff = ws->function("eff_nominal");
  // auto lum = ws->var("lum_var");
  // auto zxs = ws->var("bxs_var");
  // auto lepm = ws->var("lepm");
  // auto br_sig_beta = ws->var("br_sig_beta");
  // auto br_sig_kappa = ws->var("br_sig_kappa");
  // auto xframe = lepm->frame(RooFit::Title("Background + Signal distributions at 95\% CLs limit"));

  // //set to expected upper limit
  // br_sig->setVal(expectedUL);
  // br_sig->setConstant(1);
  // cout << "Fitting dataset with branching ratio set to upper limit...\n";
  // totPDF->fitTo(*data);
  // data->plotOn(xframe);

  // cout << "Plotting the PDFs...\n";
  // totPDF->plotOn(xframe);
  // totPDF->plotOn(xframe, RooFit::Components((doHiggs) ? "bkgPDF_bst3" : "bkgPDF_bst4"), RooFit::LineColor(kGreen-3), RooFit::LineStyle(kDashed));
  // totPDF->plotOn(xframe, RooFit::Components("sigPDF"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  // TCanvas* cmass = new TCanvas();
  // xframe->Draw();

  // cmass->SaveAs(Form("plots/latest_production/%s/upperlimit_pdfs_%s_binned_%i.png", year_string.Data(), (doHiggs) ? "hemu" : "zemu", set));
  // cout << "Printing variable information...\n";

  // totPDF->Print();
  // bkgPDF->Print();
  // sigPDF->Print();
  // if(ws->pdf("sigpdf1")) ws->pdf("sigpdf1")->Print();
  // if(ws->pdf("sigpdf2")) ws->pdf("sigpdf2")->Print();
  // if(ws->var("fracsig")) ws->var("fracsig")->Print();
  // n_sig->Print();
  // br_sig->Print();
  // eff->Print();
  // if(lum)
  //   lum->Print();
  // if(zxs)
  //   zxs->Print();
  // n_bkg->Print();

  return 0;
}
