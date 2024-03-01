//Modified example using RooMultiPdf taken from:
// https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/nonstandard/#roomultipdf-conventional-bias-studies

void combine_multipdf_example_alt(const bool gen_with_3rd_order = true){

   // mass variable
   RooRealVar mass("CMS_hgg_mass","m_{#gamma#gamma}",120,100,180);
   mass.setRange("full"        , 100., 190.);
   mass.setRange("LowSideband" , 100., 120.);
   mass.setRange("HighSideband", 130., 190.);
   mass.setRange("BlindRegion" , 120., 130.);
   mass.setBins(80);


   // create 3 background pdfs
   // polynomial with 2 parameters
   RooRealVar poly_1_c1("poly_1_c1","T1 of chebychev polynomial", -0.8,-25,25);
   RooRealVar poly_1_c2("poly_1_c2","T2 of chebychev polynomial",  0.2,-25,25);
   RooChebychev poly_1("poly_pdf_1","polynomial pdf (1)",mass,RooArgList(poly_1_c1,poly_1_c2));

   // polynomial with 3 parameters
   RooRealVar poly_2_c1("poly_2_c1","T1 of chebychev polynomial", -1.4,-25,25);
   RooRealVar poly_2_c2("poly_2_c2","T2 of chebychev polynomial",  0.7,-25,25);
   RooRealVar poly_2_c3("poly_2_c3","T3 of chebychev polynomial", -0.2,-25,25);
   RooChebychev poly_2("poly_pdf_2","polynomial pdf (2)",mass,RooArgList(poly_2_c1,poly_2_c2,poly_2_c3));

   // polynomial with 4 parameters
   RooRealVar poly_3_c1("poly_3_c1","T1 of chebychev polynomial",0.,-25,25);
   RooRealVar poly_3_c2("poly_3_c2","T2 of chebychev polynomial",0.,-25,25);
   RooRealVar poly_3_c3("poly_3_c3","T3 of chebychev polynomial",0.,-25,25);
   RooRealVar poly_3_c4("poly_3_c4","T4 of chebychev polynomial",0.,-25,25);
   RooChebychev poly_3("poly_pdf_3","polynomial pdf (3)",mass,RooArgList(poly_3_c1,poly_3_c2,poly_3_c3,poly_3_c4));

   // Generate some data
   const int ndata = 10000;
   RooDataHist *data = (gen_with_3rd_order) ? poly_2.generateBinned(mass,RooFit::NumEvents(ndata)) : poly_1.generateBinned(mass,RooFit::NumEvents(ndata));

   // First we fit the pdfs to the data
   poly_2_c1.setVal( 0.1);
   poly_2_c2.setVal( 0.1);
   poly_2_c3.setVal( 0.01);

   poly_1.fitTo(*data);
   poly_2.fitTo(*data);
   poly_3.fitTo(*data);

   // Make a plot (data is a toy dataset)
   RooPlot *plot = mass.frame();
   data->plotOn(plot);
   poly_1.plotOn(plot,RooFit::LineColor(kBlue));
   poly_2.plotOn(plot,RooFit::LineColor(kRed),RooFit::LineStyle(kDashed));
   poly_3.plotOn(plot,RooFit::LineColor(kGreen),RooFit::LineStyle(kDashed));
   plot->SetTitle("PDF fits to toy data");
   TCanvas c;
   plot->Draw();
   c.SaveAs("combine_multipdf_example_alt.png");

   // Make a RooCategory object. This will control which of the pdfs is "active"
   RooCategory cat("pdf_index","Index of Pdf which is active");

   // Make a RooMultiPdf object. The order of the pdfs will be the order of their index, ie for below
   RooArgList mypdfs;
   mypdfs.add(poly_1);
   mypdfs.add(poly_2);
   mypdfs.add(poly_3);
   RooMultiPdf multipdf("roomultipdf","All Pdfs",cat,mypdfs);

   // We will also produce a signal model for the bias studies
   RooRealVar mean  ("mean"  , "mean"  , 9.06608e+01); mean  .setConstant(true);
   RooRealVar sigma ("sigma" , "sigma" , 2.34308e+00); sigma .setConstant(true);
   RooRealVar alpha1("alpha1", "alpha1", 1.07075e+00); alpha1.setConstant(true);
   RooRealVar alpha2("alpha2", "alpha2", 1.37750e+00); alpha2.setConstant(true);
   RooRealVar enne1 ("enne1" , "enne2" , 3.56827e+00); enne1 .setConstant(true);
   RooRealVar enne2 ("enne2" , "enne2" , 2.92393e+00); enne2 .setConstant(true);
   RooAbsPdf* signal = new RooDoubleCrystalBall("signal", "signal PDF", mass, mean, sigma, alpha1, enne1, alpha2, enne2);


   // Save to a new workspace
   TFile *fout = new TFile("workspace_alt.root","RECREATE");
   RooWorkspace wout("workspace","workspace");

   data->SetName("data");
   wout.import(*data, RooFit::RecycleConflictNodes());
   wout.import(cat, RooFit::RecycleConflictNodes());
   wout.import(multipdf, RooFit::RecycleConflictNodes());
   wout.import(*signal, RooFit::RecycleConflictNodes());
   wout.Print();
   wout.Write();
}
