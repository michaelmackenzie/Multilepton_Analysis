//Test fitting DY/WW shapes for the Z->e+mu search

////////////////////////////////////////////////
// Plot the raw PDF distribution
TH1* get_pdf(RooAbsPdf* pdf, RooRealVar& obs) {
  TH1* fit = pdf->createHistogram("fit", obs);
  for(int ibin = 1; ibin <= fit->GetNbinsX(); ++ibin) fit->SetBinError(ibin, 0.);
  fit->SetLineColor(kRed);
  fit->SetFillColor(0);
  fit->SetTitle("PDF");
  fit->SetYTitle("PDF");
  return fit;
}

////////////////////////////////////////////////
// Get Data / Fit histogram
TH1* get_data_fit_diff(RooDataHist& data, RooAbsPdf* pdf, RooRealVar& obs) {
  TH1* fit = get_pdf(pdf, obs);
  TH1* diff = data.createHistogram("diff", obs);
  fit->Scale(diff->Integral() / fit->Integral());
  diff->Divide(fit);
  diff->SetMarkerStyle(20);
  diff->SetMarkerSize(0.8);
  diff->SetLineColor(kBlue);
  diff->SetMarkerColor(kBlack);
  diff->SetFillColor(0);
  diff->SetTitle("");
  diff->SetYTitle("Data / Fit");
  delete fit;
  return diff;
}


////////////////////////////////////////////////
// Main function
void standalone_dy_ww_shape(bool use_fast_bernstein = true, int range = 0) {

  //////////////////////////////////////////////
  // Define the observable

  RooRealVar obs("obs", "obs", 90., 70., 110.);
  obs.setRange("full"        , 70., 110.);
  obs.setRange("LowSideband" , 70.,  86.);
  obs.setRange("HighSideband", 96., 110.);
  obs.setRange("BlindRegion" , 86.,  96.);
  obs.setBins(80);
  obs.Print();

  //////////////////////////////////////////////
  // Define the background PDFs

  const int poly_order = 2;
  const int exp_order  = 0;

  RooRealVar poly_c1("bst_1", "bst_1",  1.66944, -1.e2, 1.e2);
  RooRealVar poly_c2("bst_2", "bst_2",  1.57416, -1.e2, 1.e2);
  RooAbsPdf* polyPdf = nullptr;
  if(use_fast_bernstein) {
    polyPdf = new RooBernsteinFast<2>("bst_pdf", "Bernstein PDF", obs, RooArgList(poly_c1, poly_c2));
  } else {
    polyPdf = new RooBernstein("bst_pdf", "Bernstein PDF", obs, RooArgList(poly_c1, poly_c2));
  }

  RooRealVar exp_c1("exp_1", "exp_1", -0.13588, -5., 5.);
  RooAbsPdf* expPdf  = new RooExponential("exp_pdf", "Exponential PDF", obs, exp_c1);
  polyPdf->Print();
  expPdf->Print();

  //////////////////////////////////////////////
  // Generate toy data

  const double nww = 1725.23;
  const double ndy = 8307.27;

  RooDataHist* genWW = polyPdf->generateBinned(obs, nww);
  RooDataHist* genDY = expPdf ->generateBinned(obs, ndy);

  RooDataHist* genData = polyPdf->generateBinned(obs, nww);
  genData->add(*(expPdf->generateBinned(obs, ndy)));


  //////////////////////////////////////////////
  // Fit the background PDFs

  if(range == 0) {
    polyPdf->fitTo(*genWW, RooFit::SumW2Error(1));
    expPdf ->fitTo(*genDY, RooFit::SumW2Error(1));
  } else if(range == 1) {
    polyPdf->fitTo(*genWW, RooFit::SumW2Error(1), RooFit::Range("full"));
    expPdf ->fitTo(*genDY, RooFit::SumW2Error(1), RooFit::Range("full"));
  } else if(range == 2) {
    polyPdf->fitTo(*genWW, RooFit::SumW2Error(1), RooFit::Range("LowSideband,HighSideband"));
    expPdf ->fitTo(*genDY, RooFit::SumW2Error(1), RooFit::Range("LowSideband,HighSideband"));
  }
  polyPdf->Print("tree");
  expPdf ->Print("tree");


  //////////////////////////////////////////////
  // Retrieve histograms of the data

  TH1* hww  = genWW  ->createHistogram("hww"  , obs);
  TH1* hdy  = genDY  ->createHistogram("hdy"  , obs);
  TH1* data = genData->createHistogram("hdata", obs);

  //////////////////////////////////////////////
  // Plot the individual background fits

  TCanvas c("c","c",1800,1000);
  TPad* pad1 = new TPad("pad1", "pad1", 0.00, 0.3, 0.33, 1.0); pad1->Draw(); //Drell-Yan plots
  TPad* pad2 = new TPad("pad2", "pad2", 0.00, 0.0, 0.33, 0.3); pad2->Draw();
  TPad* pad3 = new TPad("pad3", "pad3", 0.33, 0.3, 0.66, 1.0); pad3->Draw(); //WW plots
  TPad* pad4 = new TPad("pad4", "pad4", 0.33, 0.0, 0.66, 0.3); pad4->Draw();
  TPad* pad5 = new TPad("pad5", "pad5", 0.66, 0.3, 1.00, 1.0); pad5->Draw(); //Total data plots
  TPad* pad6 = new TPad("pad6", "pad6", 0.66, 0.0, 1.00, 0.3); pad6->Draw();
  gStyle->SetOptStat(0);

  //////////////////////////////////////////////
  // Drell-Yan plot

  pad1->cd();
  auto xframe = obs.frame(RooFit::Title("Drell-Yan MC fit"));
  genDY->plotOn(xframe);
  expPdf->plotOn(xframe);
  xframe->Draw();

  //Data / Fit comparison
  pad2->cd();
  TH1* hdy_diff = get_data_fit_diff(*genDY, expPdf, obs);
  hdy_diff->SetName("dy_diff");
  hdy_diff->Draw("E1");
  hdy_diff->GetXaxis()->SetRangeUser(70.,110.);
  hdy_diff->GetYaxis()->SetRangeUser(0.5,1.5);
  TLine* line = new TLine(70., 1., 110., 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");

  //////////////////////////////////////////////
  // WW plot

  pad3->cd();
  xframe = obs.frame(RooFit::Title("Di-/tri-boson MC fit"));
  genWW->plotOn(xframe);
  polyPdf->plotOn(xframe);
  xframe->Draw();

  //Data / Fit comparison
  pad4->cd();
  TH1* hww_diff = get_data_fit_diff(*genWW, polyPdf, obs);
  hww_diff->SetName("ww_diff");
  hww_diff->Draw("E1");
  hww_diff->GetXaxis()->SetRangeUser(70.,110.);
  hww_diff->GetYaxis()->SetRangeUser(0.5,1.5);
  line->Draw("same");


  //////////////////////////////////////////////
  // Fit the combination data with both backgrounds

  //add the PDFs together
  RooRealVar* comb_coeff = new RooRealVar("comb_coeff", "comb_coeff", 0.15, 0., 1.);
  RooAddPdf* basePdf     = new RooAddPdf ("comb_pdf" , "comb_pdf",
                                          RooArgList(*polyPdf, *expPdf), RooArgList(*comb_coeff), false);
  if(range == 0) {
    basePdf->fitTo(*genData, RooFit::SumW2Error(1));
  } else if(range == 1) {
    basePdf->fitTo(*genData, RooFit::SumW2Error(1), RooFit::Range("full"));
  } else if(range == 2) {
    basePdf->fitTo(*genData, RooFit::SumW2Error(1), RooFit::Range("LowSideband,HighSideband"));
  }

  //////////////////////////////////////////////
  // Plot the data fit results

  pad5->cd();
  xframe = obs.frame(RooFit::Title("Mock data fit"));
  genData->plotOn(xframe);
  basePdf->plotOn(xframe);
  basePdf->plotOn(xframe, RooFit::Components(polyPdf->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  basePdf->plotOn(xframe, RooFit::Components(expPdf->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  xframe->Draw();

  //Data / Fit comparison
  pad6->cd();
  //FIXME: The Data / Fit appears to only use the polyPdf
  //       Perhaps a normalization issue between Bernstein and Exp PDFs?
  TH1* hdata_diff = get_data_fit_diff(*genData, basePdf, obs);
  hdata_diff->SetName("data_diff");
  hdata_diff->Draw("E1");
  hdata_diff->GetXaxis()->SetRangeUser(70.,110.);
  // hdata_diff->GetYaxis()->SetRangeUser(0.5,1.5);
  line->Draw("same");

  c.SaveAs("standalone_dy_ww_shape.png");

  pad1->SetLogy();
  pad3->SetLogy();
  pad5->SetLogy();
  c.SaveAs("standalone_dy_ww_shape_log.png");

  TCanvas c2;

  TH1* hpdf_poly = get_pdf(polyPdf, obs);
  hpdf_poly->SetName("poly_pdf");
  TH1* hpdf_exp  = get_pdf(expPdf, obs);
  hpdf_exp ->SetName("exp_pdf");
  TH1* hpdf_tot = get_pdf(basePdf, obs);
  hpdf_tot ->SetName("tot_pdf");

  hpdf_tot->SetLineColor(kBlue);
  hpdf_tot ->Draw("hist");
  hpdf_poly->Draw("hist same");
  hpdf_exp ->Draw("hist same");
  // hpdf_tot ->GetYaxis()->SetRangeUser(1.e-5,1.);
  c2.Update();
  c2.SetLogy();
  c2.SaveAs("standalone_dy_ww_shape_pdfs.png");
  cout << "Total integral: " << hpdf_tot ->Integral() << endl;
  cout << "Poly  integral: " << hpdf_poly->Integral() << endl;
  cout << "Exp   integral: " << hpdf_exp ->Integral() << endl;
  return 0;
}
