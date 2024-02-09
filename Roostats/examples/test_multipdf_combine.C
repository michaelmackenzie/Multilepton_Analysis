//Test using a RooMultiPdf in Combine

/**
   bkg_mode options:
   0: Nominal Bernstein poly values fit to Z->e+mu sidebands --> Fails in Combine
   1: Nominal Chebychev poly values fit to Z->e+mu sidebands --> Fails in Combine
   2: Approximate Chebychev values from Z->e+mu sideband fit --> Fails in Combine
   3: Higher (Chebychev) background near peak
   4: Simpler (Chebychev) falling spectrum
**/
void test_multipdf_combine(const int bkg_mode = 3) {

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

  //Create background PDFs using polynomials
  RooAbsPdf *poly_1(nullptr), *poly_2(nullptr);
  if(bkg_mode == 0) { //Bernstein polynomials
    RooRealVar* bst_3_c0 = new RooRealVar("bst_3_0", "bst_3_0", -0.206163 , -25., 25.);
    RooRealVar* bst_3_c1 = new RooRealVar("bst_3_1", "bst_3_1",  0.0958628, -25., 25.);
    RooRealVar* bst_3_c2 = new RooRealVar("bst_3_2", "bst_3_2",  0.0160934, -25., 25.);
    poly_1 = new RooBernsteinFast<3>("bst_3_pdf", "Bernstein PDF, order 3", obs, RooArgList(*bst_3_c0, *bst_3_c1, *bst_3_c2));

    RooRealVar* bst_4_c0 = new RooRealVar("bst_4_0", "bst_4_0", 0.0309362, -25., 25.);
    RooRealVar* bst_4_c1 = new RooRealVar("bst_4_1", "bst_4_1", 0.0170719, -25., 25.);
    RooRealVar* bst_4_c2 = new RooRealVar("bst_4_2", "bst_4_2", 0.0434216, -25., 25.);
    RooRealVar* bst_4_c3 = new RooRealVar("bst_4_3", "bst_4_3", 0.0203762, -25., 25.);
    poly_2 = new RooBernsteinFast<4>("bst_4_pdf", "Bernstein PDF, order 4", obs, RooArgList(*bst_4_c0, *bst_4_c1, *bst_4_c2, *bst_4_c3));
  } else if(bkg_mode == 1) { //Chebychev polynomials
    RooRealVar* chb_3_c0 = new RooRealVar("chb_3_0", "chb_3_0", -1.49379, -25., 25.);
    RooRealVar* chb_3_c1 = new RooRealVar("chb_3_1", "chb_3_1", 0.722981, -25., 25.);
    RooRealVar* chb_3_c2 = new RooRealVar("chb_3_2", "chb_3_2",-0.189633, -25., 25.);
    poly_1 = new RooChebychev("chb_3_pdf", "Chebychev PDF, order 3", obs, RooArgList(*chb_3_c0, *chb_3_c1, *chb_3_c2));

    RooRealVar* chb_4_c0 = new RooRealVar("chb_4_0", "chb_4_0",  -1.48095, -25., 25.);
    RooRealVar* chb_4_c1 = new RooRealVar("chb_4_1", "chb_4_1",  0.728222, -25., 25.);
    RooRealVar* chb_4_c2 = new RooRealVar("chb_4_2", "chb_4_2", -0.208949, -25., 25.);
    RooRealVar* chb_4_c3 = new RooRealVar("chb_4_3", "chb_4_3", 0.0191548, -25., 25.);
    poly_2 = new RooChebychev("chb_4_pdf", "Chebychev PDF, order 4", obs, RooArgList(*chb_4_c0, *chb_4_c1, *chb_4_c2, *chb_4_c3));
  } else if(bkg_mode == 2) { //Chebychev polynomials (approx)
    RooRealVar* chb_3_c0 = new RooRealVar("chb_3_0", "chb_3_0", -1.40, -25., 25.);
    RooRealVar* chb_3_c1 = new RooRealVar("chb_3_1", "chb_3_1",  0.70, -25., 25.);
    RooRealVar* chb_3_c2 = new RooRealVar("chb_3_2", "chb_3_2", -0.20, -25., 25.);
    poly_1 = new RooChebychev("chb_3_pdf", "Chebychev PDF, order 3", obs, RooArgList(*chb_3_c0, *chb_3_c1, *chb_3_c2));

    //not used to generate --> fit values later
    RooRealVar* chb_4_c0 = new RooRealVar("chb_4_0", "chb_4_0",  -1.48095, -25., 25.);
    RooRealVar* chb_4_c1 = new RooRealVar("chb_4_1", "chb_4_1",  0.728222, -25., 25.);
    RooRealVar* chb_4_c2 = new RooRealVar("chb_4_2", "chb_4_2", -0.208949, -25., 25.);
    RooRealVar* chb_4_c3 = new RooRealVar("chb_4_3", "chb_4_3", 0.0191548, -25., 25.);
    poly_2 = new RooChebychev("chb_4_pdf", "Chebychev PDF, order 4", obs, RooArgList(*chb_4_c0, *chb_4_c1, *chb_4_c2, *chb_4_c3));
  } else if(bkg_mode == 3) { //Higher background near signal
    RooRealVar* chb_3_c0 = new RooRealVar("chb_3_0", "chb_3_0", -0.80, -25., 25.);
    RooRealVar* chb_3_c1 = new RooRealVar("chb_3_1", "chb_3_1",  0.50, -25., 25.);
    RooRealVar* chb_3_c2 = new RooRealVar("chb_3_2", "chb_3_2", -0.20, -25., 25.);
    poly_1 = new RooChebychev("chb_3_pdf", "Chebychev PDF, order 3", obs, RooArgList(*chb_3_c0, *chb_3_c1, *chb_3_c2));

    //not used to generate --> fit values later
    RooRealVar* chb_4_c0 = new RooRealVar("chb_4_0", "chb_4_0",  -1.48095, -25., 25.);
    RooRealVar* chb_4_c1 = new RooRealVar("chb_4_1", "chb_4_1",  0.728222, -25., 25.);
    RooRealVar* chb_4_c2 = new RooRealVar("chb_4_2", "chb_4_2", -0.208949, -25., 25.);
    RooRealVar* chb_4_c3 = new RooRealVar("chb_4_3", "chb_4_3", 0.0191548, -25., 25.);
    poly_2 = new RooChebychev("chb_4_pdf", "Chebychev PDF, order 4", obs, RooArgList(*chb_4_c0, *chb_4_c1, *chb_4_c2, *chb_4_c3));
  } else if(bkg_mode == 4) { //Simple falling background
    RooRealVar* chb_2_c0 = new RooRealVar("chb_2_0", "chb_2_0", -6.56766e-01, -25., 25.);
    RooRealVar* chb_2_c1 = new RooRealVar("chb_2_1", "chb_2_1",  1.22148e-01, -25., 25.);
    poly_1 = new RooChebychev("chb_2_pdf", "Chebychev PDF, order 2", obs, RooArgList(*chb_2_c0, *chb_2_c1));

    //not used to generate --> fit values later
    RooRealVar* chb_4_c0 = new RooRealVar("chb_4_0", "chb_4_0",  -1.48095, -25., 25.);
    RooRealVar* chb_4_c1 = new RooRealVar("chb_4_1", "chb_4_1",  0.728222, -25., 25.);
    RooRealVar* chb_4_c2 = new RooRealVar("chb_4_2", "chb_4_2", -0.208949, -25., 25.);
    RooRealVar* chb_4_c3 = new RooRealVar("chb_4_3", "chb_4_3", 0.0191548, -25., 25.);
    poly_2 = new RooChebychev("chb_4_pdf", "Chebychev PDF, order 4", obs, RooArgList(*chb_4_c0, *chb_4_c1, *chb_4_c2, *chb_4_c3));
  } else {
    cout << "Unknown background mode " << bkg_mode << endl;
    return;
  }


  //Create a RooMultiPdf with two PDFs
  RooCategory categories("cat", "cat");
  RooArgList pdf_list;
  pdf_list.add(*poly_1);
  pdf_list.add(*poly_2);
  RooMultiPdf* bkgPDF = new RooMultiPdf("bkgPDF", "Background PDF envelope", categories, pdf_list);
  categories.setIndex(0);
  const int npdfs = categories.numTypes();

  //Add the signal and background PDFs to make a total PDF
  RooRealVar* n_sig = new RooRealVar("n_sig", "N(signal)", 0., -1000., 1000.);
  RooRealVar* n_bkg = new RooRealVar("n_bkg", "N(background)", 7500., 0., 15000.);
  RooAbsPdf* totPDF = new RooAddPdf("total_PDF", "total PDF", RooArgList(*bkgPDF, *sigPDF), RooArgList(*n_bkg, *n_sig));

  // Generate data from the background PDF
  const int ndata = 7307; //data in a nominal dataset
  RooDataHist* gen = poly_1->generateBinned(obs, RooFit::NumEvents(ndata));

  //Fit the PDFs to the data
  poly_1->fitTo(*gen);
  poly_2->fitTo(*gen);


  //////////////////////////////////////////////
  // Plot the input PDFs
  //////////////////////////////////////////////

  //Plot the background and signal PDF together
  {
    TCanvas c;
    auto xframe = obs.frame(RooFit::Title("Testing Multi-PDF"));
    gen->plotOn(xframe);
    n_sig->setVal(700.); //make visible
    totPDF->plotOn(xframe, RooFit::LineColor(kRed ));
    totPDF->plotOn(xframe, RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed), RooFit::Components("sigPDF"));
    categories.setIndex(0);
    totPDF->plotOn(xframe, RooFit::LineColor(kRed ), RooFit::LineStyle(kDashed), RooFit::Components("bkgPDF"));
    categories.setIndex(1);
    totPDF->plotOn(xframe, RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed), RooFit::Components("bkgPDF"));
    categories.setIndex(0);
    xframe->Draw();
    c.SaveAs("test_multipdf_combine_pdfs.png");
    n_sig->setVal(0.); //restore to 0 signal
    delete xframe;
  }

  //////////////////////////////////////////////
  // Create the data card and workspace
  //////////////////////////////////////////////

  const char* fname = "combine_test_multipdf";

  //Create the data card
  std::ofstream outfile;
  outfile.open(Form("%s.txt", fname));
  outfile << "# -*- mode: tcl -*-\n";
  outfile << "# Example Higgs Combine datacard using a RooMultiPdf background\n\n";
  outfile << "imax  1 number of channels\n";
  outfile << "jmax  1 number of backgrounds\n";
  outfile << "kmax  * number of nuisance parameters\n\n";
  outfile << "-----------------------------------------------------------------------------------------------------------" << endl
          << Form("shapes * * %s.root $CHANNEL:$PROCESS\n", fname)
          << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

  outfile << Form("%-12s %10s\n", "bin", obs.GetName());
  outfile << Form("%-12s %10i\n\n", "observation", -1);
  outfile << Form("%-12s %10s %10s\n", "bin", obs.GetName(), obs.GetName());
  outfile << Form("%-12s %10s %10s\n", "process", "sig", "bkg");
  outfile << Form("%-12s %10i %10i\n\n", "process", 0, 1);
  outfile << Form("%-12s %10.3f %10.1f\n\n", "rate", 30.256, (double) ndata);

  //add an example signal uncertainty of 5%
  outfile << "-----------------------------------------------------------------------------------------------------------" << endl
          << Form("%-8s lnN %10.2f %10s\n", "sigNorm", 1.05, "-")
          << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

  outfile << Form("%-12s %10s\n\n", categories.GetName(), "discrete");
  outfile << Form("nbkg rateParam %s bkg 1.\n", obs.GetName()); //allow the background to float

  outfile.close();

  //Create the workspace
  TFile* f = new TFile(Form("%s.root", fname), "RECREATE");
  RooWorkspace ws(obs.GetName(), obs.GetName());
  sigPDF->SetName("sig");
  bkgPDF->SetName("bkg");
  gen->SetName("data_obs");
  ws.import(obs);
  ws.import(*sigPDF   , RooFit::RecycleConflictNodes());
  ws.import(*bkgPDF   , RooFit::RecycleConflictNodes());
  ws.import(categories, RooFit::RecycleConflictNodes());
  ws.import(*gen      , RooFit::RecycleConflictNodes());
  ws.Write();
  f->Close();
}
