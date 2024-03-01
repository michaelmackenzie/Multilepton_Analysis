#include "../construct_multidim.C"

void test_chebychev() {
  RooRealVar obs("obs", "obs", 90., 70., 110.);
  obs.setRange("full"        , 70., 110.);
  obs.setRange("LowSideband" , 70.,  86.);
  obs.setRange("HighSideband", 96., 110.);
  obs.setRange("BlindRegion" , 86.,  96.);
  obs.setBins(80);

  // RooRealVar poly_c1("cheb_1", "cheb_1", -0.70, -25, 25.);
  // RooRealVar poly_c2("cheb_2", "cheb_2",  0.30, -25, 25.);
  // RooRealVar poly_c3("cheb_3", "cheb_3", -0.07, -25, 25.);
  // RooAbsPdf* pdf = new RooChebychev("cheb_pdf", "Chebychev PDF", obs, RooArgList(poly_c1, poly_c2, poly_c3));
  RooAbsPdf* pdf = create_chebychev(obs, 3, 11);
  RooDataHist* gen = pdf->generateBinned(obs, 1.e4);

  RooWorkspace ws;
  ws.import(obs);
  ws.import(*pdf);
  ws.import(*gen);

  TCanvas c;
  auto xframe = obs.frame(RooFit::Title("Testing Chebychev PDF"));
  gen->plotOn(xframe);
  pdf->plotOn(xframe);

  //Save a snapshot of the generation
  std::unique_ptr<RooArgSet> params{pdf->getParameters(obs)};
  ws.defineSet("parameters", *params);
  ws.defineSet("observables", obs);
  ws.saveSnapshot("gen_params", *params, true);

  //Set the PDF to the wrong values
  // poly_c1.setVal(-0.6);
  // poly_c2.setVal( 0.2);
  // poly_c3.setVal(-0.1);
  pdf->plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));

  //Attempt to fit in the sidebands
  pdf->fitTo(*gen, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
  int nbins = gen->sumEntries();
  const double chisq = get_chi_squared(obs, pdf, *gen, true, &nbins);
  pdf->plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed), RooFit::NormRange("full"), RooFit::Range("full"));

  printf("Fit chi^2 = %.3f / %i = %.4f\n", chisq, nbins - 4, chisq / (nbins - 4));

  xframe->Draw();
  c.SaveAs("test_chebychev.png");

}
