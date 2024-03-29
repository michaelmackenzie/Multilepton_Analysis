//Script to calculate the 95% UL for e+mu resonance
bool doConstraints_ = false;
bool fixBkgParams_ = false;
bool useBinnedBkgFit_ = true;
bool doBlind_ = true; //use nominal Asimov data for limit calculator
int  single_cat_ = -1; //category to do the limit for
bool dry_run_ = false; //quit before performing calculations
bool useFixedScan_ = true;

Int_t calculate_UL_MVA_categories(vector<int> sets = {8}, TString selection = "zmutau",
                                  vector<int> years = {2016, 2017, 2018},
                                  bool useToyData = false) {

  int n_categories = (selection.Contains("emu")) ? 1 : 2;
  n_categories *= sets.size(); //emu and Xtau per histogram set

  //construct needed signal names
  // TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");

  //construct string representation of the years
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }

  //do the same for the set list
  TString set_str = "";
  for(int set : sets) {
    if(set_str == "") set_str += set;
    else {set_str += "_"; set_str += set;}
  }

  //get the workspace from the previous stage
  TFile* fInput = TFile::Open(Form("workspaces/hist_background_mva_%s%s_%s_%s.root", selection.Data(),
                                   (doConstraints_) ? "_constr" : "",
                                   year_string.Data(), set_str.Data()), "READ");
  if(!fInput) return 1;
  cout << "Using input file " << fInput->GetName() << endl;
  fInput->cd();
  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  if(!ws) return 2;

  //get the signal branching fraction
  RooRealVar* br_sig = ws->var("br_sig");
  if(!br_sig) return 2;

  RooArgList poi_list(*br_sig);
  RooArgList obs_list;
  for(int index = 0; index < 2*sets.size(); ++index) obs_list.add(*(ws->var(Form("mva%i", index))));

  RooCategory* categories = ((RooCategory*) ws->obj(selection.Data()));
  if(!categories) {
    cout << "Failed to find category variable!\n";
    return 3;
  }
  obs_list.add(*categories);
  RooDataHist* data = (RooDataHist*) ws->data((doBlind_) ? "combined_toy_data" : "combined_data");
  ws->Print();
  vector<RooRealVar*> n_bkgs;
  vector<RooFormulaVar*> n_sigs;
  RooArgList nuisance_params;
  RooArgList glb_list;
  int cat_start = (single_cat_ >= 0) ? single_cat_ : 0;
  int cat_end = (single_cat_ >= 0) ? single_cat_ + 1 : n_categories;
  cout << "---------------------------------------------------------------\n"
       << "Nominal yields by category:\n";
  for(int i_cat = cat_start; i_cat < cat_end; ++i_cat) {
    auto n_bkg  = ws->var(Form("n_bkg_%i", i_cat));
    if(!n_bkg) return 4 + i_cat;
    auto n_sig  = (RooFormulaVar*) ws->function(Form("n_sig_%i", i_cat));
    if(!n_sig) return 5 + i_cat;
    n_bkgs.push_back(n_bkg);
    n_sigs.push_back(n_sig);
    nuisance_params.add(*n_bkg);
    cout << " " << i_cat << ": N(bkg) = " << n_bkg->getVal() << " N(sig) = " << n_sig->getVal() << endl;
    glb_list.add(*(ws->var(Form("eff_nominal_%i", i_cat))));
  }
  cout << "---------------------------------------------------------------\n\n";

  if(doConstraints_) {
    nuisance_params.add(*(ws->var("br_sig_beta")));
    glb_list.add(*(ws->var("br_sig_kappa")));
    glb_list.add(*(ws->var("one")));
    glb_list.add(*(ws->var("zero")));
    glb_list.add(*(ws->var("bxs_var")));
    glb_list.add(*(ws->var("lum_var")));
  }
  else if(ws->var("br_sig_kappa")) {
    ws->var("br_sig_kappa")->setVal(1.);
    ws->var("br_sig_kappa")->setConstant(1);
    ws->var("br_sig_beta")->setVal(0.);
    ws->var("br_sig_beta")->setConstant(1);
  }
  //Set the model and let it know about the workspace contents
  RooStats::ModelConfig model;
  model.SetWorkspace(*ws);
  model.SetPdf((single_cat_ >= 0) ? Form("totMVAPDF_%i", single_cat_) : "totPDF");
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  if(doConstraints_) model.SetGlobalObservables(glb_list);
  model.SetName("S+B Model");
  model.SetProtoData(*data);

  auto mva = ws->var("mva");
  auto PDF = model.GetPdf();
  if(single_cat_ >= 0) {
    data = PDF->generateBinned(RooArgSet(*mva), n_bkgs[0]->getVal());
  }
  auto bModel = model.Clone();
  bModel->SetName("B Model");
  double oldval = ((RooRealVar*) poi_list.find("br_sig"))->getVal();
  ((RooRealVar*) poi_list.find("br_sig"))->setVal(0.); //BEWARE that the range of the POI has to contain zero!
  bModel->SetSnapshot(poi_list);
  ((RooRealVar*) poi_list.find("br_sig"))->setVal(oldval);

  // TVirtualFitter::SetMaxIterations( 1e7 );

  bool doAsimov = false && doBlind_;
  RooStats::AsymptoticCalculator fc(*data, *bModel, model, doAsimov);
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
  if(doConstraints_) npoints /= 2;

  //min and max for the scan (better to choose smaller intervals)
  double poimin = ((RooRealVar*) poi_list.find("br_sig"))->getMin();
  double poimax = ((RooRealVar*) poi_list.find("br_sig"))->getMax();

  bool doHiggs = selection.Contains("h");
  double min_scan = (!doHiggs) ? 2.e-8 : 1.e-6;
  double max_scan = (!doHiggs) ? 1.e-5 : 2.e-3;
  // if(years.size() > 1) {min_scan /= 50.; max_scan /= 2.;}
  if(doConstraints_) {
    min_scan *= 0.88;
    max_scan *= (selection.Contains("etau")) ? 10. : 3.;
  }
  std::cout << "Doing a fixed scan in the interval: " << min_scan << ", "
            << max_scan << " with " << npoints << " points\n";

  if(poimin > min_scan || poimax < max_scan) {
    cout << "POI scan issue! range = [" << min_scan << ", " << max_scan
         << "] but POI range = [" << poimin << ", " << poimax << "]\n";
    return 10;
  }
  if(useFixedScan_)
    calc.SetFixedScan(npoints, min_scan, max_scan, true); //bool is for log scan vs linear scan
  else
    calc.SetAutoScan();
  if(dry_run_) {
    cout << "Setup finished! Exiting dry run...\n";
    return 0;
  }
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
      g->GetXaxis()->SetRangeUser(min_scan, max_scan);
    }
  }
  // canvas->GetListOfPrimitives()->Print();
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
  TString cname = Form("plots/latest_production/%s/pval_vs_br_%s_mva_categories",
                       year_string.Data(), selection.Data());
  if(doConstraints_) cname += "_constr";
  cname +="_";
  cname += set_str.Data();
  if(single_cat_ >= 0) cname += Form("_cat_%i", single_cat_);
  cname += ".png";
  canvas->SaveAs(cname.Data());

  cout << "Finished UL calculation, plotting dataset with expected UL...\n";

  //Plot background spectrum (with toy data) and signal with upper limit value
  auto totPDF = (RooAddPdf*) ws->pdf("totPDF");
  // auto bkgPDF = ws->pdf("bkgMVAPDF");
  // auto sigPDF = ws->pdf("sigMVAPDF");
  // auto n_sig = ws->function("n_sig");
  // auto eff = ws->function("eff");
  // auto lum = ws->var("lum_var");
  // auto zxs = ws->var("bxs_var");
  // auto xframe = mva->frame(RooFit::Title("Background + Signal distributions at 95\% CLs limit"));

  // //set to expected upper limit
  // br_sig->setVal(expectedUL);
  // br_sig->setConstant(1);
  // cout << "Fitting dataset with branching ratio set to upper limit...\n";
  // totPDF->fitTo(*data);
  // data->plotOn(xframe);

  // cout << "Plotting the PDFs...\n";
  // totPDF->plotOn(xframe);
  // totPDF->plotOn(xframe, RooFit::Components("bkgMVAPDF"), RooFit::LineColor(kGreen-3), RooFit::LineStyle(kDashed));
  // totPDF->plotOn(xframe, RooFit::Components("sigMVAPDF"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  // TCanvas* cmass = new TCanvas();
  // xframe->Draw();

  // cmass->SaveAs(Form("plots/latest_production/%s/upperlimit_pdfs_%s_%s_%i.png", year_string.Data(), hist.Data(), selection.Data(), set));
  // cout << "Printing variable information...\n";

  // totPDF->Print();
  // bkgPDF->Print();
  // sigPDF->Print();
  // if(br_sig) br_sig->Print();
  // if(n_sig) n_sig->Print();
  // if(eff) eff->Print();
  // if(n_bkg) n_bkg->Print();
  // if(lum) lum->Print();

  return 0;
}
