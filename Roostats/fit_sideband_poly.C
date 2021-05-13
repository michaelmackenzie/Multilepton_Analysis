//Script to fit the side band regions with an exponential
bool use_range_ = true; //Whether to fit in a RooFit::Range or not
bool use_blind_ = true; //Whether to remove data from a region or not

void fit_sideband_poly() {

  // POI
  RooRealVar mass("mass", "mass", 75., 120., "GeV/c^{2}");
  mass.setRange("LowSideband" , 75., 85.);
  mass.setRange("HighSideband", 95., 120.);
  mass.setRange("Full", 75., 120.);

  // Make a generator model
  double true_val = -0.05;
  RooRealVar tau("tau", "The exponent", true_val, -10., -0.001);
  RooExponential exp("exp", "A falling exponential function", mass, tau);

  // Generate some toy data
  RooDataHist* data = exp.generateBinned(RooArgSet(mass), 1e5);
  auto data_blinded = data->reduce("mass < 84.9 || mass > 95.7");

  // shift tau away to re-fit later
  tau.setVal(-1.);

  // Make a fit model
  RooRealVar a("a", "a", 2.03, -5., 5.);
  RooRealVar b("b", "b", 0.65, -5., 5.);
  RooRealVar c("c", "c", 0.40, -5., 5.);
  RooRealVar d("d", "d", 0.21, -5., 5.);
  RooBernstein bernPDF("bernPDF", "bernPDF", mass, RooArgSet(a,b,c,d));

  RooRealVar mean("mean", "mean", -500., -1000., 100.);
  RooRealVar sigma("sigma", "sigma", 110., 0.1, 1000.);
  RooGaussian gausPDF("gausPDF", "gausPDF", mass, mean, sigma);

  if(use_range_) {
    exp.fitTo    ((use_blind_) ? *data_blinded : *data, RooFit::Range("LowSideband", "HighSideband"));
    gausPDF.fitTo((use_blind_) ? *data_blinded : *data, RooFit::Range("LowSideband", "HighSideband"));
    bernPDF.fitTo((use_blind_) ? *data_blinded : *data, RooFit::Range("LowSideband", "HighSideband"));
  } else {
    exp.fitTo    ((use_blind_) ? *data_blinded : *data);
    gausPDF.fitTo((use_blind_) ? *data_blinded : *data);
    bernPDF.fitTo((use_blind_) ? *data_blinded : *data);
  }

  auto xframe = mass.frame();
  if(use_blind_) data_blinded->plotOn(xframe);
  else           data->plotOn(xframe);

  double old_val = tau.getVal();
  tau.setVal(true_val);
  exp.plotOn(xframe, RooFit::Range("Full"), RooFit::NormRange("LowSideband,HighSideband"));
  std::cout << "*************************************************************" << std::endl
            << "Chi^2/dof: " << xframe->chiSquare() << std::endl
            << "N(bins) = " << data_blinded->numEntries() << std::endl
            << "--> Chi^2 = " << xframe->chiSquare()*data_blinded->numEntries() << std::endl
            << "--> P(Chi^2)= " << ROOT::Math::chisquared_cdf_c(xframe->chiSquare()*data_blinded->numEntries(), data_blinded->numEntries() - 2)
            << std::endl
            << "*************************************************************"
            << std::endl;
  tau.setVal(old_val);

  gausPDF.plotOn(xframe, RooFit::Range("Full"), RooFit::NormRange("LowSideband,HighSideband"), RooFit::LineColor(kViolet), RooFit::LineStyle(kDashed));
  std::cout << "*************************************************************" << std::endl
            << "Chi^2/dof: " << xframe->chiSquare() << std::endl
            << "N(bins) = " << data_blinded->numEntries() << std::endl //need number of bins not blinded though...
            << "--> Chi^2 = " << xframe->chiSquare()*data_blinded->numEntries() << std::endl
            << "--> P(Chi^2)= " << ROOT::Math::chisquared_cdf_c(xframe->chiSquare()*data_blinded->numEntries(), data_blinded->numEntries() - 5)
            << std::endl
            << "*************************************************************"
            << std::endl;

  bernPDF.plotOn(xframe, RooFit::Range("Full"), RooFit::NormRange("LowSideband,HighSideband"), RooFit::LineColor(kGreen), RooFit::LineStyle(10));
  std::cout << "*************************************************************" << std::endl
            << "Chi^2/dof: " << xframe->chiSquare() << std::endl
            << "N(bins) = " << data_blinded->numEntries() << std::endl //need number of bins not blinded though...
            << "--> Chi^2 = " << xframe->chiSquare()*data_blinded->numEntries() << std::endl
            << "--> P(Chi^2)= " << ROOT::Math::chisquared_cdf_c(xframe->chiSquare()*data_blinded->numEntries(), data_blinded->numEntries() - 5)
            << std::endl
            << "*************************************************************"
            << std::endl;

  exp.plotOn(xframe, RooFit::Range("Full"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::NormRange("LowSideband,HighSideband"));
  std::cout << "*************************************************************" << std::endl
            << "Chi^2/dof: " << xframe->chiSquare() << std::endl
            << "N(bins) = " << data_blinded->numEntries() << std::endl
            << "--> Chi^2 = " << xframe->chiSquare()*data_blinded->numEntries() << std::endl
            << "--> P(Chi^2)= " << ROOT::Math::chisquared_cdf_c(xframe->chiSquare()*data_blinded->numEntries(), data_blinded->numEntries() - 2)
            << std::endl
            << "*************************************************************"
            << std::endl;

  auto c1 = new TCanvas();
  xframe->Draw();
  xframe->Print();
  c1->Print(Form("fit_sideband_poly_blind_%i_ranges_%i.png", use_blind_, use_range_));
  std::cout << "Fit parameter results:\n Gaussian: " << mean.getVal() << ", " << sigma.getVal() << std::endl
            << " Bernstein: " << a.getVal() << ", " << b.getVal() << ", " << c.getVal()
            << ", " << d.getVal() << std::endl
            << " Exp: " << old_val << std::endl;
}
