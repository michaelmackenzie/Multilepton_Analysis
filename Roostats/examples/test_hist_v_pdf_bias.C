//Use a PDF to define a histogram, test generating with the histogram and fitting with the PDF
int ndata_;

void make_data_card(const char* fname, const bool use_multipdf = false) {

  //Create the data card
  std::ofstream outfile;
  outfile.open(Form("%s.txt", fname));
  outfile << "# -*- mode: tcl -*-\n";
  outfile << "# Example Higgs Combine datacard\n\n";
  outfile << "imax  1 number of channels\n";
  outfile << "jmax  1 number of backgrounds\n";
  outfile << "kmax  * number of nuisance parameters\n\n";
  outfile << "-----------------------------------------------------------------------------------------------------------" << endl
          << Form("shapes * * %s.root $CHANNEL:$PROCESS\n", fname)
          << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

  outfile << Form("%-12s %10s\n", "bin", "mass");
  outfile << Form("%-12s %10i\n\n", "observation", -1);
  outfile << Form("%-12s %10s %10s\n", "bin", "mass", "mass");
  outfile << Form("%-12s %10s %10s\n", "process", "signal", "bkg");
  outfile << Form("%-12s %10i %10i\n\n", "process", 0, 1);
  outfile << Form("%-12s %10.3f %10.1f\n\n", "rate", 40., (double) ndata_);

  //add an example signal uncertainty of 5%
  outfile << "-----------------------------------------------------------------------------------------------------------" << endl
          << Form("%-8s lnN %10.2f %10s\n", "sigNorm", 1.05, "-")
          << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

  if(use_multipdf) outfile << Form("%-12s %10s\n\n", "cat", "discrete");
  outfile << Form("nbkg rateParam %s bkg 1.\n", "mass"); //allow the background to float

}

void test_hist_v_pdf_bias(const bool use_multipdf = true){

   // mass variable
   RooRealVar mass("mass","m_{e#mu}",90,70,110);
   mass.setRange("full"        , 70., 110.);
   mass.setRange("LowSideband" , 70.,  85.);
   mass.setRange("HighSideband", 95., 110.);
   mass.setRange("BlindRegion" , 85.,  95.);
   mass.setBins(80);


   // Create background and signal PDFs

   // // polynomial with 4 parameters
   // RooRealVar poly_3_c1("poly_3_c1","T1 of chebychev polynomial",-1.46,-5,5);
   // RooRealVar poly_3_c2("poly_3_c2","T2 of chebychev polynomial", 0.71,-5,5);
   // RooRealVar poly_3_c3("poly_3_c3","T3 of chebychev polynomial",-0.20,-5,5);
   // RooChebychev bkg_pdf("poly_pdf_3","polynomial pdf (3)",mass,RooArgList(poly_3_c1,poly_3_c2,poly_3_c3));

   //Gaussian + polynomial
   TString formula = "TMath::Gaus(@0, @1, @2) + @3 + @4*@0 + @5*@0*@0";
   RooRealVar bkg_g_0("bkg_g_0", "bkg_g_0", 60.54  , 40., 70.); //mean
   RooRealVar bkg_g_1("bkg_g_1", "bkg_g_1", 11.29  ,  1., 60.); //sigma
   RooRealVar bkg_p_0("bkg_p_0", "bkg_p_0", 0.062  ,-10., 10.); //poly c0
   RooRealVar bkg_p_1("bkg_p_1", "bkg_p_1", 3.44e-5, -1.,  1.); //poly c1
   RooRealVar bkg_p_2("bkg_p_2", "bkg_p_2",-4.05e-6, -1.,  1.); //poly c2
   RooGenericPdf bkg_pdf("bkg_pdf", formula.Data(), RooArgList(mass, bkg_g_0, bkg_g_1, bkg_p_0, bkg_p_1, bkg_p_2));

   // Double sided Crystal Ball for the signal
   RooRealVar mean  ("mean"  , "mean"  , 9.06608e+01); mean  .setConstant(true);
   RooRealVar sigma ("sigma" , "sigma" , 2.34308e+00); sigma .setConstant(true);
   RooRealVar alpha1("alpha1", "alpha1", 1.07075e+00); alpha1.setConstant(true);
   RooRealVar alpha2("alpha2", "alpha2", 1.37750e+00); alpha2.setConstant(true);
   RooRealVar enne1 ("enne1" , "enne2" , 3.56827e+00); enne1 .setConstant(true);
   RooRealVar enne2 ("enne2" , "enne2" , 2.92393e+00); enne2 .setConstant(true);
   RooAbsPdf* signal = new RooDoubleCrystalBall("signal", "signal PDF", mass, mean, sigma, alpha1, enne1, alpha2, enne2);


   // Generate some data
   ndata_ = 7300;
   RooDataHist *data = bkg_pdf.generateBinned(mass,RooFit::NumEvents(ndata_));

   // Fit the background pdf to the data
   bkg_pdf.fitTo(*data);

   // Make a histogram from the background PDF
   TH1* h = bkg_pdf.createHistogram("bkg_hist", mass);
   RooDataHist hData("bkg_data"    , "Bkg Data Hist", RooArgSet(mass), h    );
   RooHistPdf  hPDF ("bkg_hist_pdf", "Bkg Hist PDF" , RooArgSet(mass), hData);

   // Make a plot (data is a toy dataset)
   RooPlot *plot = mass.frame();
   data->plotOn(plot);
   bkg_pdf.plotOn(plot,RooFit::LineColor(kBlue),RooFit::LineStyle(kDashed));
   hPDF   .plotOn(plot,RooFit::LineColor(kRed ),RooFit::LineStyle(kDashed));
   plot->SetTitle("PDF fit to toy data");
   TCanvas c;
   plot->Draw();
   c.SaveAs("test_hist_v_pdf_bias.png");

   // Make a workspace with the PDF
   {
     TFile *fout = new TFile("hist_v_pdf_bias_pdf.root","RECREATE");
     RooWorkspace wout("mass","mass");

     //If requested, wrap the PDF in a RooMultiPdf
     if(use_multipdf) {
       RooCategory categories("cat", "cat");
       RooMultiPdf envelope("bkg", "Background PDF envelope", categories, RooArgList(bkg_pdf));
       categories.setIndex(0);
       wout.import(envelope, RooFit::RecycleConflictNodes());
     } else {
       bkg_pdf.SetName("bkg");
       wout.import(bkg_pdf, RooFit::RecycleConflictNodes());
     }
     data->SetName("data_obs");
     wout.import(*data  , RooFit::RecycleConflictNodes());
     wout.import(*signal, RooFit::RecycleConflictNodes());
     wout.Print();
     wout.Write();
     fout->Close();
     make_data_card("hist_v_pdf_bias_pdf", use_multipdf);
   }

   // Make a workspace with the histogram
   {
     TFile *fout = new TFile("hist_v_pdf_bias_hist.root","RECREATE");
     RooWorkspace wout("mass","mass");

     hPDF.SetName("bkg");
     data->SetName("data_obs");
     wout.import(*data, RooFit::RecycleConflictNodes());
     wout.import(hPDF, RooFit::RecycleConflictNodes());
     wout.import(*signal, RooFit::RecycleConflictNodes());
     wout.Print();
     wout.Write();
     fout->Close();
     make_data_card("hist_v_pdf_bias_hist");
   }
}
