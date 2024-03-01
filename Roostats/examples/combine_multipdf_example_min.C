//Minimally changed example using RooMultiPdf taken from:
// https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/nonstandard/#roomultipdf-conventional-bias-studies

void combine_multipdf_example_min(){

   // Load the combine Library
   gSystem->Load("libHiggsAnalysisCombinedLimit.so");

   // mass variable
   RooRealVar mass("CMS_hgg_mass","m_{#gamma#gamma}",120,100,180);


   // create 3 background pdfs
   // 1. exponential
   RooRealVar expo_1("expo_1","slope of exponential",-0.02,-100.,-0.00001);
   RooExponential exponential("exponential","exponential pdf",mass,expo_1);

   // 2. polynomial with 3 parameters
   // RooRealVar poly_1("poly_1","T1 of chebychev polynomial",-1.5,-25,25);
   // RooRealVar poly_2("poly_2","T2 of chebychev polynomial", 0.7,-25,25);
   // RooRealVar poly_3("poly_3","T3 of chebychev polynomial",-0.2,-25,25);
   // RooChebychev polynomial("polynomial","polynomial pdf",mass,RooArgList(poly_1,poly_2,poly_3));


   // 2nd order poly
   RooRealVar poly_1("poly_1","T1 of polynomial",1.e4,-1.e5,1.e5);
   RooRealVar poly_2("poly_2","T2 of polynomial", 250.,-1.e4,1.e4);
   RooGenericPdf polynomial("polynomial","@1*(@0-@2)*(@0-@2) + 1",RooArgList(mass,poly_1, poly_2));

   // 3. A power law function
   RooRealVar pow_1("pow_1","exponent of power law",-3,-1.e4,-1.e-10);
   RooGenericPdf powerlaw("powerlaw","TMath::Power(@0,@1)",RooArgList(mass,pow_1));

   // Generate some data (lets use the power lay function for it)
   // Here we are using unbinned data, but binning the data is also fine
   const int ndata = 10000;
   RooDataSet *data = powerlaw.generate(mass,RooFit::NumEvents(1000));
   // RooDataSet *data = polynomial.generate(mass,RooFit::NumEvents(ndata));

   // First we fit the pdfs to the data (gives us a sensible starting value of parameters for, e.g - blind limits)
   exponential.fitTo(*data);   // index 0
   polynomial.fitTo(*data);   // index 1
   powerlaw.fitTo(*data);     // index 2

   // Make a plot (data is a toy dataset)
   RooPlot *plot = mass.frame();   data->plotOn(plot);
   exponential.plotOn(plot,RooFit::LineColor(kGreen));
   polynomial.plotOn(plot,RooFit::LineColor(kBlue));
   powerlaw.plotOn(plot,RooFit::LineColor(kRed));
   plot->SetTitle("PDF fits to toy data");
   TCanvas c;
   plot->Draw();
   c.SaveAs("combine_multipdf_example_min.png");

   // Make a RooCategory object. This will control which of the pdfs is "active"
   RooCategory cat("pdf_index","Index of Pdf which is active");

   // Make a RooMultiPdf object. The order of the pdfs will be the order of their index, ie for below
   // 0 == exponential
   // 1 == polynomial
   // 2 == powerlaw
   RooArgList mypdfs;
   mypdfs.add(exponential);
   mypdfs.add(polynomial);
   mypdfs.add(powerlaw);

   RooMultiPdf multipdf("roomultipdf","All Pdfs",cat,mypdfs);
   // By default the multipdf will tell combine to add 0.5 to the nll for each parameter (this is the penalty for the discrete profiling method)
   // It can be changed with
   //   multipdf.setCorrectionFactor(penalty)
   // For bias-studies, this isn;t relevant however, so lets just leave the default

   // As usual make an extended term for the background with _norm for freely floating yield
   RooRealVar norm("roomultipdf_norm","Number of background events",1000,0,100000);
   cat.setIndex(1);

   // We will also produce a signal model for the bias studies
   RooRealVar sigma("sigma","sigma",1.2); sigma.setConstant(true);
   RooRealVar MH("MH","MH",125); MH.setConstant(true);
   RooGaussian signal("signal","signal",mass,MH,sigma);


   // Save to a new workspace
   TFile *fout = new TFile("workspace.root","RECREATE");
   RooWorkspace wout("workspace","workspace");

   data->SetName("data");
   wout.import(*data);
   wout.import(cat);
   wout.import(norm);
   wout.import(multipdf);
   wout.import(signal);
   wout.Print();
   wout.Write();

   powerlaw.Print("tree");
   exponential.Print("tree");
   polynomial.Print("tree");
}
