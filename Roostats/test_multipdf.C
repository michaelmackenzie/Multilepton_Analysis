//Test using a RooMultiPdf to fit pseudo-data

void test_multipdf() {

  //////////////////////////////////////////////
  // Create the inputs
  //////////////////////////////////////////////

  //Define the observable
  RooRealVar obs("obs", "obs", 90., 70., 110.);
  obs.setRange("full"        , 70., 110.);
  obs.setRange("LowSideband" , 70.,  86.);
  obs.setRange("HighSideband", 96., 110.);
  obs.setRange("BlindRegion" , 86.,  96.);
  obs.setBins(80);

  //Create a signal PDF
  RooRealVar mean  ("mean"  , "mean"  , 9.06608e+01); mean  .setConstant(true);
  RooRealVar sigma ("sigma" , "sigma" , 2.34308e+00); sigma .setConstant(true);
  RooRealVar alpha1("alpha1", "alpha1", 1.07075e+00); alpha1.setConstant(true);
  RooRealVar alpha2("alpha2", "alpha2", 1.37750e+00); alpha2.setConstant(true);
  RooRealVar enne1 ("enne1" , "enne2" , 3.56827e+00); enne1 .setConstant(true);
  RooRealVar enne2 ("enne2" , "enne2" , 2.92393e+00); enne2 .setConstant(true);
  RooAbsPdf* sigPDF = new RooDoubleCrystalBall("sigPDF", "signal PDF", obs, mean, sigma, alpha1, enne1, alpha2, enne2);

  //Create background PDFs using Bernstein polynomials
  RooRealVar bst_3_c0("bst_3_0", "bst_3_0", -0.206163 , -25., 25.);
  RooRealVar bst_3_c1("bst_3_1", "bst_3_1",  0.0958628, -25., 25.);
  RooRealVar bst_3_c2("bst_3_2", "bst_3_2",  0.0160934, -25., 25.);
  auto bst_3_pdf = new RooBernsteinFast<3>("bst_3_pdf", "Bernstein PDF, order 3", obs, RooArgList(bst_3_c0, bst_3_c1, bst_3_c2));
  // bst_3_pdf->protectSubRange(true);
  // auto bst_3_pdf = new RooChebychev("bst_3_pdf", "Chebychev PDF, order 3", obs, RooArgList(bst_3_c0, bst_3_c1, bst_3_c2));

  RooRealVar bst_4_c0("bst_4_0", "bst_4_0", 0.0309362, -25., 25.);
  RooRealVar bst_4_c1("bst_4_1", "bst_4_1", 0.0170719, -25., 25.);
  RooRealVar bst_4_c2("bst_4_2", "bst_4_2", 0.0434216, -25., 25.);
  RooRealVar bst_4_c3("bst_4_3", "bst_4_3", 0.0203762, -25., 25.);
  auto bst_4_pdf = new RooBernsteinFast<4>("bst_4_pdf", "Chebychev PDF, order 4", obs, RooArgList(bst_4_c0, bst_4_c1, bst_4_c2, bst_4_c3));
  // bst_4_pdf->protectSubRange(true);
  // auto bst_4_pdf = new RooChebychev("bst_4_pdf", "Bernstein PDF, order 4", obs, RooArgList(bst_4_c0, bst_4_c1, bst_4_c2, bst_4_c3));

  //Create a RooMultiPdf with two PDFs
  RooCategory categories("cat", "cat");
  RooArgList pdf_list;
  pdf_list.add(*bst_3_pdf); //categories.defineType("bst_3", 0);
  pdf_list.add(*bst_4_pdf); //categories.defineType("bst_4", 1);
  RooMultiPdf* bkgPDF = new RooMultiPdf("bkgPDF", "Background PDF envelope", categories, pdf_list);
  categories.setIndex(0);

  //Add the signal and background PDFs to make a total PDF
  RooRealVar* n_sig = new RooRealVar("n_sig", "N(signal)", 0., -1000., 1000.);
  RooRealVar* n_bkg = new RooRealVar("n_bkg", "N(background)", 7500., 0., 15000.);
  RooAbsPdf* totPDF = new RooAddPdf("total_PDF", "total PDF", RooArgList(*bkgPDF, *sigPDF), RooArgList(*n_bkg, *n_sig));

  //////////////////////////////////////////////
  // Plot the input PDFs
  //////////////////////////////////////////////

  //Plot the signal PDF
  {
    TCanvas* c = new TCanvas("c_sig", "c_sig", 1000, 1000);
    auto xframe = obs.frame();
    xframe->SetTitle("");
    xframe->SetXTitle("M_{e#mu}");
    sigPDF->plotOn(xframe);
    xframe->Draw();
    c->SaveAs("test_multipdf_sig_pdf.png");
    delete xframe;
    delete c;
  }

  //Plot the background and signal PDF together
  {
    TCanvas c;
    auto xframe = obs.frame(RooFit::Title("Testing Multi-PDF"));
    n_sig->setVal(700.); //make visible
    totPDF->plotOn(xframe, RooFit::LineColor(kRed ));
    totPDF->plotOn(xframe, RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed), RooFit::Components("sigPDF"));
    totPDF->plotOn(xframe, RooFit::LineColor(kRed ), RooFit::LineStyle(kDashed), RooFit::Components("bkgPDF"));
    xframe->Draw();
    c.SaveAs("test_multipdf_bkg_pdf.png");
    n_sig->setVal(0.); //restore to 0 signal
  }

  //////////////////////////////////////////////
  // Test fitting the multi-PDF
  //////////////////////////////////////////////

  //Generate example data
  const int ndata = 7307;
  RooDataHist* gen = bst_3_pdf->generateBinned(obs, ndata);

  //Fit to the data
  {
    // categories.setConstant(true); // --> doesn't change the result
    // auto fitres = bkgPDF->fitTo(*gen, RooFit::Save(true)); // --> fit succeeds, but only fits background parameters, no signal
    auto fitres = totPDF->fitTo(*gen, RooFit::Save(true));
    if(!fitres || fitres->status() != 0) cout << "!!! Fit Failed!\n";
  }

  //Plot the fit result
  {
    TCanvas c;
    auto xframe = obs.frame(RooFit::Title("Testing Multi-PDF fitting"));
    gen->plotOn(xframe);
    totPDF->plotOn(xframe, RooFit::LineColor(kRed));
    totPDF->plotOn(xframe, RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed), RooFit::Components("sigPDF"));
    totPDF->plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Components("bkgPDF"));
    xframe->Draw();

    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    label.SetTextSize(0.04);
    label.SetTextAlign(13);
    label.DrawLatex(0.65, 0.85, Form("N(data) = %i"  , ndata          ));
    label.DrawLatex(0.65, 0.80, Form("N(bkg)  = %.1f", n_bkg->getVal()));
    label.DrawLatex(0.65, 0.75, Form("N(sig)  = %.1f", n_sig->getVal()));
    c.SaveAs("test_multipdf_fit.png");
  }

  //////////////////////////////////////////////
  // Test fitting normal PDFs
  //////////////////////////////////////////////

  //set to nominal coefficient values
  bst_3_c0.setVal(-0.206163);
  bst_3_c1.setVal(0.0958628);
  bst_3_c2.setVal(0.0160934);
  bst_4_c0.setVal(0.0309362);
  bst_4_c1.setVal(0.0170719);
  bst_4_c2.setVal(0.0434216);
  bst_4_c3.setVal(0.0203762);

  //Fit to the data using only 1 bkg PDF
  delete totPDF;
  totPDF = new RooAddPdf("total_PDF", "total PDF", RooArgList(*bst_3_pdf, *sigPDF), RooArgList(*n_bkg, *n_sig), false);

  {
    auto fitres = totPDF->fitTo(*gen, RooFit::Save(true));
    if(!fitres || fitres->status() != 0) cout << "!!! Fit Failed!\n";
  }

  //Plot the fit result
  {
    TCanvas c;
    auto xframe = obs.frame(RooFit::Title("Testing normal PDF fitting"));
    gen->plotOn(xframe);
    totPDF->plotOn(xframe, RooFit::LineColor(kRed));
    totPDF->plotOn(xframe, RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed), RooFit::Components("sigPDF"));
    totPDF->plotOn(xframe, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Components("bst_3_pdf"));
    xframe->Draw();

    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    label.SetTextSize(0.04);
    label.SetTextAlign(13);
    label.DrawLatex(0.65, 0.85, Form("N(data) = %i"  , ndata          ));
    label.DrawLatex(0.65, 0.80, Form("N(bkg)  = %.1f", n_bkg->getVal()));
    label.DrawLatex(0.65, 0.75, Form("N(sig)  = %.1f", n_sig->getVal()));
    c.SaveAs("test_multipdf_single_fit.png");
  }
}
