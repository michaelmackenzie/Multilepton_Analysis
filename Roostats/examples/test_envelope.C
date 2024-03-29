//Test creating a RooMultiPdf envelope

void test_envelope() {

  //Create an observable (di-lepton mass)
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
  RooDoubleCrystalBall sigPDF("sig", "signal PDF", obs, mean, sigma, alpha1, enne1, alpha2, enne2);

  //Create an exponential PDF
  RooRealVar expo_1("expo_1","slope of exponential",-0.02,-10,-0.0001);
  RooExponential exponential("exponential","exponential pdf",obs,expo_1);

  //Create a polynomial PDF
  RooRealVar poly_c1("cheb_1", "cheb_1", -0.70, -25, 25.);
  RooRealVar poly_c2("cheb_2", "cheb_2",  0.30, -25, 25.);
  RooRealVar poly_c3("cheb_3", "cheb_3", -0.07, -25, 25.);
  RooChebychev polynomial("cheb_pdf", "Chebychev PDF", obs, RooArgList(poly_c1, poly_c2, poly_c3));

  //Generate toy data
  int ndata = 3000;
  RooDataSet *data = exponential.generate(obs,RooFit::NumEvents(ndata));
  data->SetName("data_obs");
  exponential.fitTo(*data);
  polynomial.fitTo(*data);

  //Plot the data
  RooPlot *plot = obs.frame();
  data->plotOn(plot);
  exponential.plotOn(plot,RooFit::LineColor(kBlue));
  polynomial.plotOn(plot,RooFit::LineColor(kRed));
  plot->SetTitle("PDF fits to toy data");
  TCanvas c;
  plot->Draw();
  c.SaveAs("test_envelope.png");

  //Create an envelope of functions
  RooCategory categories("cat", "cat");
  RooMultiPdf pdf("bkg", "Background PDF envelope", categories, RooArgList(exponential, polynomial));
  categories.setIndex(0);
  const int npdfs = categories.numTypes();

  //Save the PDFs to a workspace
  const char* fname = "test_envelope";
  TFile* f = new TFile(Form("%s.root", fname), "RECREATE");
  RooWorkspace ws(obs.GetName(), obs.GetName());
  ws.import(obs);
  ws.import(sigPDF);
  ws.import(pdf);
  ws.import(*data);
  ws.Write();
  f->Close();

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

}
