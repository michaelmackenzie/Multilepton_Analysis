void test_bernstein() {
  RooRealVar obs("obs", "obs", 90., 70., 110.);
  obs.setRange("full"        , 70., 110.);
  obs.setRange("LowSideband" , 70.,  86.);
  obs.setRange("HighSideband", 96., 110.);
  obs.setRange("BlindRegion" , 86.,  96.);
  obs.setBins(80);

  RooRealVar poly_c1("bst_1", "bst_1",  1.66944, -1.e2, 1.e2);
  RooRealVar poly_c2("bst_2", "bst_2",  1.57416, -1.e2, 1.e2);
  RooRealVar poly_c3("bst_3", "bst_3", -4.26054, -1.e10, 1.e10);
  RooAbsPdf* pdf = new RooBernsteinFast<2>("bst_pdf", "Bernstein PDF", obs, RooArgList(poly_c1, poly_c2));
  RooDataHist* gen = pdf->generateBinned(obs, 1.e4);

  TCanvas c;
  auto xframe = obs.frame(RooFit::Title("Testing Bernstein PDF"));
  gen->plotOn(xframe);
  pdf->plotOn(xframe);
  poly_c3.setVal(-1.e3);
  pdf->plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  poly_c3.setVal(1.e3);
  pdf->plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
  xframe->Draw();

  c.SaveAs("test_bernstein.png");

}
