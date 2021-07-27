//Simple script to test RooFit with variable width histograms

void test_variable_width_bins(int Mode = 0) {

  /////////////////////////////////////////////////
  // Generate toy data/MC
  /////////////////////////////////////////////////

  vector<double> bins_array;
  if(Mode < 10) {
    bins_array = {-6., -4., -3., -2.5, -2., -1.5, -1., -0.75, -0.5, -0.25, 0.,
                  0.25, 0.5, 0.75, 1., 1.5, 2., 2.5, 3., 4., 6.};
  } else {
    for(double bin_edge = -6.; bin_edge <= 6.; bin_edge += 0.25)
      bins_array.push_back(bin_edge);
  }
  double * bins = bins_array.data();
  int nbins = bins_array.size() - 1;
  TH1D* hdata  = new TH1D("hdata" , "Data"  , nbins, bins);
  TH1D* hMC    = new TH1D("hMC"   , "MC"    , nbins, bins);
  TH1D* hdata2 = new TH1D("hdata2", "Data"  , nbins, bins);
  TH1D* hMC2   = new TH1D("hMC2"  , "MC"    , nbins, bins);
  TH1D* hsig_1 = new TH1D("hsig_1", "Signal", nbins, bins);
  TH1D* hsig_2 = new TH1D("hsig_2", "Signal", nbins, bins);

  TRandom3 rnd(90);
  for(int i = 0; i < 1e7; ++i) {
    hdata->Fill(rnd.Gaus());
    hMC->Fill(rnd.Gaus());
  }
  for(int i = 0; i < 1e6; ++i) {
    hdata2->Fill(rnd.Gaus());
    hMC2->Fill(rnd.Gaus());
  }
  for(int i = 0; i < 1e7; ++i) {
    hsig_1->Fill(rnd.Gaus(1.2,0.2));
    hsig_2->Fill(rnd.Gaus(1.2,0.2));
  }

  TCanvas* c = new TCanvas("c_orig", "c_orig", 1200, 600);
  c->Divide(2,1);
  c->cd(1);
  hdata->SetMarkerStyle(20);
  hdata->SetMarkerSize(0.8);
  hdata->Draw("E");
  hMC->SetLineColor(kRed);
  hMC->SetLineWidth(2);
  hMC->Draw("same");
  hsig_1->Scale(1.e-3);
  hsig_1->SetLineColor(kRed);
  hsig_1->SetLineWidth(2);
  hsig_1->Draw("same");
  c->cd(2);
  hdata2->SetMarkerStyle(20);
  hdata2->SetMarkerSize(0.8);
  hdata2->Draw("E");
  hMC2->SetLineColor(kRed);
  hMC2->SetLineWidth(2);
  hMC2->Draw("same");
  hsig_2->Scale(1.e-3);
  hsig_2->SetLineColor(kRed);
  hsig_2->SetLineWidth(2);
  hsig_2->Draw("same");
  c->SaveAs("test_variable_width_bins_orig.png");

  /////////////////////////////////////////////////
  // Create Roo objects for toy Data/MC
  /////////////////////////////////////////////////

  c = new TCanvas("c_res", "c_res", 1000, 700);

  //Define observable variable
  RooRealVar obs("obs", "Observable", -6., 6.);
  RooBinning obs_bin(nbins, bins, "obs_bin");
  obs.setBinning(obs_bin);
  RooRealVar nbkg_1("nbkg_1", "N(bkg)",  1e4, 1e9); nbkg_1.setVal(hdata ->Integral());
  RooRealVar nbkg_2("nbkg_2", "N(bkg)",  1e4, 1e9); nbkg_2.setVal(hdata2->Integral());
  RooRealVar nsig("nsig", "N(sig)", -1e6, 1e6);

  //Define datasets
  RooDataHist data ("data" , "Data"  , RooArgList(obs), hdata );
  RooDataHist mc   ("mc"   , "MC"    , RooArgList(obs), hMC   );
  RooDataHist sig_1("sig_1", "Signal", RooArgList(obs), hsig_1);
  RooDataHist data2("data2", "Data"  , RooArgList(obs), hdata2);
  RooDataHist mc2  ("mc2"  , "MC"    , RooArgList(obs), hMC2  );
  RooDataHist sig_2("sig_2", "Signal", RooArgList(obs), hsig_2);

  //Create PDFs from MC
  RooHistPdf  bkgPDF_1("bkgPDF_1", "Background PDF" , RooArgList(obs), mc   );
  RooHistPdf  bkgPDF_2("bkgPDF_2", "Background PDF" , RooArgList(obs), mc2  );
  RooHistPdf  sigPDF_1("sigPDF_1", "Signal PDF"    , RooArgList(obs) , sig_1);
  RooHistPdf  sigPDF_2("sigPDF_2", "Signal PDF"    , RooArgList(obs) , sig_2);
  RooAddPdf   totPDF_1("totPDF_1", "Total PDF", RooArgList(sigPDF_1, bkgPDF_1), RooArgList(nsig, nbkg_1));
  RooAddPdf   totPDF_2("totPDF_2", "Total PDF", RooArgList(sigPDF_2, bkgPDF_2), RooArgList(nsig, nbkg_2));

  //Create a category object, store results as a category
  RooCategory categories("categories", "categories");
  map<string, RooDataHist*> dataMap;
  categories.defineType("cat_0", 0);
  categories.defineType("cat_1", 1);
  dataMap["cat_0"] = &data;
  dataMap["cat_1"] = &data2;

  //Create a simultaneous PDF and dataset
  RooSimultaneous totPDF("totPDF", "Total PDF", categories);
  totPDF.addPdf(totPDF_1, "cat_0");
  totPDF.addPdf(totPDF_2, "cat_1");
  RooDataHist combined_data("combined_data", "combined_data", obs, categories, dataMap);

  //fit to get N(bkg) correct
  nsig.setVal(0.); nsig.setConstant(1);
  if(Mode % 10 == 1)
    totPDF_1.fitTo(data, RooFit::Extended(1));
  else if(Mode % 10 == 2)
    totPDF_2.fitTo(data2, RooFit::Extended(1));
  else
    totPDF.fitTo(combined_data, RooFit::Extended(1));

  double nbkg_1_nom(nbkg_1.getVal()), nbkg_2_nom(nbkg_2.getVal());

  auto xframe = obs.frame();
  combined_data.plotOn(xframe, RooFit::Cut("categories == 0"), RooFit::LineColor(kRed));
  totPDF       .plotOn(xframe, RooFit::Slice(categories, "cat_0"), RooFit::ProjWData(combined_data), RooFit::LineColor(kRed));
  combined_data.plotOn(xframe, RooFit::Cut("categories == 1"));
  totPDF       .plotOn(xframe, RooFit::Slice(categories, "cat_1"), RooFit::ProjWData(combined_data));
  xframe->Draw();
  c->SaveAs(Form("test_variable_width_bins_%i.png", Mode));
  nsig.setVal(1.e3); nsig.setConstant(0);


  /////////////////////////////////////////////////
  // Perform fits/upper limit
  /////////////////////////////////////////////////

  RooWorkspace ws;
  ws.import(totPDF);
  ws.import(combined_data);

  //Create fit model
  RooArgList poi_list(nsig);
  RooArgList obs_list(obs);
  if(Mode % 10 == 0)
    obs_list.add(categories);
  RooArgList nuisance_params;
  if(Mode % 10 != 2)
    nuisance_params.add(nbkg_1);
  if(Mode % 10 != 1)
    nuisance_params.add(nbkg_2);
  RooStats::ModelConfig model;
  model.SetWorkspace(ws);
  if(Mode % 10 == 1)
    model.SetPdf(totPDF_1);
  else if(Mode % 10 == 2)
    model.SetPdf(totPDF_2);
  else
    model.SetPdf(totPDF);
  model.SetParametersOfInterest(poi_list);
  model.SetObservables(obs_list);
  model.SetNuisanceParameters(nuisance_params);
  model.SetName("S+B Model");
  model.SetProtoData(combined_data);

  auto bModel = model.Clone();
  bModel->SetName("B Model");
  ((RooRealVar*) poi_list.find("nsig"))->setVal(0.);
  bModel->SetSnapshot(poi_list);
  ((RooRealVar*) poi_list.find("nsig"))->setVal(1.e3);

  bool useAsimov = false;
  RooStats::AsymptoticCalculator fc((Mode % 10 == 0) ? combined_data : ((Mode % 10 == 1) ? data : data2),
                                    *bModel, model, useAsimov);
  fc.SetOneSided(1);
  //create a hypotest inverter passing the desired calculator
  RooStats::HypoTestInverter calc(fc);
  calc.SetConfidenceLevel(0.95);

  calc.UseCLs(1);
  calc.SetVerbose(0);
  auto toymc = fc.GetTestStatSampler();
  RooStats::ProfileLikelihoodTestStat profl(*(model.GetPdf()));
  profl.SetOneSided(1);
  toymc->SetTestStatistic(&profl);

  /////////////////////////////////////////////////
  // Perform the scan
  /////////////////////////////////////////////////

  const int    npoints  = 50;
  const double min_scan = 5e2;
  const double max_scan = 5e4;
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


  /////////////////////////////////////////////////
  // Plot the scan results
  /////////////////////////////////////////////////

  RooStats::HypoTestInverterPlot freq_plot("HTI_Result_Plot","Expected CLs",result);
  //xPlot in a new canvas with style
  c = new TCanvas();
  c->cd();
  freq_plot.Draw();
  c->SetLogx();
  //Add upper limit info to the plot
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.04);
  label.SetTextColor(kRed);
  label.SetTextAlign(13);
  label.DrawLatex(0.12, 0.34, Form("Expected 95%% CL = %.2e^{+%.2e}_{-%.2e}",
                                  expectedUL, expectedULP-expectedUL, expectedUL-expectedULM));
  c->SaveAs(Form("test_variable_width_bins_pvals_%i.png", Mode));

  cout << "-------------------------------------------------------------------------------\n"
       <<"Nominal N(bkg): " << nbkg_1_nom << ", " << nbkg_2_nom << " (expected "
       << data.sum(0) << ", " << data2.sum(0) << ")\n"
       << "-------------------------------------------------------------------------------\n"
       << "Observable binning" << obs.getBinning() << endl;

}
