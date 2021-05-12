//Script to fit the side band regions with an exponential

void fit_sideband_poly() {

  // Make a fit model
  RooRealVar mass("mass", "mass", 75., 120., "GeV/c^{2}");
  mass.setRange("LowSideband", 75., 85.);
  mass.setRange("HighSideband", 95., 120.);
  mass.setRange("Full", 75., 120.);
  RooRealVar tau("tau", "The exponent", -0.2, -10., -0.01);
  RooExponential exp("exp", "A falling exponential function", mass, tau);

  // Generate some toy data
  RooDataHist* dataset = exp.generateBinned(RooArgSet(mass), 1e5);
  tau.setVal(-2.); //shift away from the true value
  auto data_blinded = dataset->reduce("mass < 84.9 || mass > 95.7");

  exp.fitTo(/* *dataset */*data_blinded, RooFit::Range("LowSideband", "HighSideband"));

  auto xframe = mass.frame();
  data_blinded->plotOn(xframe);
  exp.plotOn(xframe, RooFit::Range("LowSideband"), RooFit::NormRange("LowSideband,HighSideband"));
  exp.plotOn(xframe, RooFit::Range("HighSideband"), RooFit::NormRange("LowSideband,HighSideband"));
  exp.plotOn(xframe, RooFit::Range("Full"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::NormRange("LowSideband,HighSideband"));

  auto c1 = new TCanvas();
  xframe->Draw();

  std::cout << "*************************************************************" << std::endl
            << "Chi^2/dof: " << xframe->chiSquare() << std::endl
            << "N(bins) = " << data_blinded->numEntries() << std::endl //need number of bins not blinded though...
            << "--> Chi^2 = " << xframe->chiSquare()*data_blinded->numEntries() << std::endl
            << "--> P(Chi^2)= " << ROOT::Math::chisquared_cdf_c(xframe->chiSquare()*data_blinded->numEntries(), data_blinded->numEntries() - 2 /*norm and tau free params*/)
            << std::endl
            << "*************************************************************"
            << std::endl;
}
