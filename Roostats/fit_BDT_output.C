{
  gSystem->Load("libRooFit");

  //Define the observable
  RooRealVar bdt_out("bdt_out","The BDT output",-1.,1.);

  //Generate a sample, but this is just for the example, in reality you'll have the BDT outputs for signal and background
  RooRealVar sig_land_mean("sig_land_mean","sig_land_mean",0.8);
  RooRealVar sig_land_sigma("sig_land_sigma","sig_land_sigma",0.1);
  RooLandau land_signal("land_signal","Signal",bdt_out,sig_land_mean,sig_land_sigma);

  RooRealVar bkg_land_mean("bkg_land_mean","bkg_land_mean",-0.5);
  RooRealVar bkg_land_sigma("bkg_land_sigma","bkg_land_sigma",0.3);
  RooLandau land_backgr("land_backgr","Background",bdt_out,bkg_land_mean,bkg_land_sigma);

  //So let's say these are the two outputs of the BDT

  /////////////////////
  //FIRST let's say this is for electron-tau
  RooDataHist *sig_bdtout_etau = land_signal.generateBinned(RooArgSet(bdt_out),10000);
  RooDataHist *bkg_bdtout_etau = land_backgr.generateBinned(RooArgSet(bdt_out),10000);

  //Now let's say we have some data to fit. It can also be the MC total dataset. Let's say we have 50 signal events and 1000 background
  RooDataHist *sig_bdtgen_etau = land_signal.generateBinned(RooArgSet(bdt_out),50);
  RooDataHist *bkg_bdtgen_etau = land_backgr.generateBinned(RooArgSet(bdt_out),1000);

  sig_bdtgen_etau->add(*bkg_bdtgen_etau);

  cout << "try   " << sig_bdtgen_etau->numEntries() << endl;

  //SECOND let's say this is for muon-tau
  sig_land_mean.setVal(0.7);
  sig_land_sigma.setVal(0.08);
  bkg_land_mean.setVal(-0.55);
  bkg_land_sigma.setVal(0.35);

  RooDataHist *sig_bdtout_mutau = land_signal.generateBinned(RooArgSet(bdt_out),10000);
  RooDataHist *bkg_bdtout_mutau = land_backgr.generateBinned(RooArgSet(bdt_out),10000);

  //Now let's say we have some data to fit. It can also be the MC total dataset. Let's say we have 40 signal events and 800 background
  RooDataHist *sig_bdtgen_mutau = land_signal.generateBinned(RooArgSet(bdt_out),40);
  RooDataHist *bkg_bdtgen_mutau = land_backgr.generateBinned(RooArgSet(bdt_out),800);

  sig_bdtgen_mutau->add(*bkg_bdtgen_mutau);

  //Create the signal and background PDFs and combine them
  RooRealVar lum("lum","lum",150.);
  RooRealVar xsec("xsec","Z prod xsec",6225.*1000.);
  RooRealVar bratio("bratio","Branching ratio",1.e-6,-0.1,0.1);

  //add an idea of systematic uncertainty
  RooRealVar bratio_eff("bratio_eff", "Branching ratio systematic", 1., 0., 2.);
  // bratio_eff.setConstant(1);
  RooRealVar sys_unc("sys_unc", "sys_unc", 0.1);
  RooRealVar sys_mean("sys_mean", "sys_mean", 1.);
  RooGaussian bratio_constr("bratio_constr", "Branching ratio constraint", bratio_eff, sys_mean, sys_unc);

  ///////////////////////
  //first for electron-tau
  RooHistPdf sig_histpdf_etau("sig_histpdf_etau","The signal histogram PDF",RooArgSet(bdt_out),*sig_bdtout_etau);
  RooHistPdf bkg_histpdf_etau("bkg_histpdf_etau","The signal histogram PDF",RooArgSet(bdt_out),*bkg_bdtout_etau);

  RooRealVar eff_etau("eff_etau","etau efficiency",0.9);

  RooFormulaVar Nsig_etau("Nsig_etau","@0*@1*@2*@3*@4",RooArgList(lum,xsec,bratio,bratio_eff,eff_etau));
  RooRealVar Nbkg_etau("Nbkg_etau","Number of background events in etau",1000.,0.,2000.);

  RooAddPdf totpdf_etau("totpdf_etau","Total PDF for etau",RooArgList(sig_histpdf_etau,bkg_histpdf_etau),RooArgList(Nsig_etau,Nbkg_etau));
  RooProdPdf totpdf_etau_constr("totpdf_etau_constr", "Constrained total PDF for etau", RooArgList(totpdf_etau, bratio_constr));
  ///////////////////////
  //second for muon-tau
  RooHistPdf sig_histpdf_mutau("sig_histpdf_mutau","The signal histogram PDF",RooArgSet(bdt_out),*sig_bdtout_mutau);
  RooHistPdf bkg_histpdf_mutau("bkg_histpdf_mutau","The signal histogram PDF",RooArgSet(bdt_out),*bkg_bdtout_mutau);

  RooRealVar eff_mutau("eff_mutau","mutau efficiency",0.8);

  RooFormulaVar Nsig_mutau("Nsig_mutau","@0*@1*@2*@3*@4",RooArgList(lum,xsec,bratio,bratio_eff,eff_mutau));
  RooRealVar Nbkg_mutau("Nbkg_mutau","Number of background events in mutau",800.,0.,2000.);

  RooAddPdf totpdf_mutau("totpdf_mutau","Total PDF for mutau",RooArgList(sig_histpdf_mutau,bkg_histpdf_mutau),RooArgList(Nsig_mutau,Nbkg_mutau));
  RooProdPdf totpdf_mutau_constr("totpdf_mutau_constr", "Constrained total PDF for mutau", RooArgList(totpdf_mutau, bratio_constr));

  ///////////////////////////////////////
  // Begin category definitions
  ///////////////////////////////////////

  //Now combine the two PDFs by defining categories
  RooCategory lepcat("lepcat","lepcat");
  lepcat.defineType("etau",0); //type defined by name and index
  lepcat.defineType("mutau",1);

  RooSimultaneous totPDF("totPDF","The total PDF",lepcat);
  totPDF.addPdf(totpdf_etau_constr,"etau"); //pass in pdf corresponding to cat name
  totPDF.addPdf(totpdf_mutau_constr,"mutau");

  //Combine the datasets
  // pass in data with category name and data
  RooDataHist combined_data("combined_data","combined_data",bdt_out,RooFit::Index(lepcat),RooFit::Import("etau",*sig_bdtgen_etau),RooFit::Import("mutau",*sig_bdtgen_mutau));

  //Now fit, plot, save, enjoy
  totPDF.fitTo(combined_data,RooFit::Extended(1), RooFit::Constrain(RooArgSet(bratio_eff)));

  //Plot for one category, for example etau
  RooPlot *bdtout_plot = bdt_out.frame(50);
  combined_data.plotOn(bdtout_plot,RooFit::Cut("lepcat==0")); //need to cut to see this category
  totPDF.plotOn(bdtout_plot,RooFit::Slice(lepcat,"etau"),RooFit::ProjWData(combined_data));
  totPDF.plotOn(bdtout_plot,RooFit::Slice(lepcat,"etau"),RooFit::ProjWData(combined_data),
                RooFit::Components("bkg_histpdf_etau"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  totPDF.plotOn(bdtout_plot,RooFit::Slice(lepcat,"etau"),RooFit::ProjWData(combined_data),
                RooFit::Components("sig_histpdf_etau"), RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));

  TCanvas c1("c1", "c1", 1000, 600);
  c1.Divide(2,1);
  c1.cd(1);
  bdtout_plot->Draw();
  c1.cd(2);
  RooPlot *bdtout_plot_2 = bdt_out.frame(50);
  combined_data.plotOn(bdtout_plot_2,RooFit::Cut("lepcat==1")); //need to cut to see this category
  totPDF.plotOn(bdtout_plot_2,RooFit::Slice(lepcat,"mutau"),RooFit::ProjWData(combined_data));
  totPDF.plotOn(bdtout_plot_2,RooFit::Slice(lepcat,"mutau"),RooFit::ProjWData(combined_data),
                RooFit::Components("bkg_histpdf_mutau"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  totPDF.plotOn(bdtout_plot_2,RooFit::Slice(lepcat,"mutau"),RooFit::ProjWData(combined_data),
                RooFit::Components("sig_histpdf_mutau"), RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
  bdtout_plot_2->Draw();
  c1.SaveAs("fit_BDToutput.png");

  RooWorkspace workspace("myworkspace");
  workspace.import(totPDF);
  workspace.import(combined_data);

  TFile fOut("fit_BDToutput.root","RECREATE");
  fOut.cd();
  workspace.Write();
  fOut.Close();

  /////////////////////////////////////////////////
  // Next, profile for the parameter of interest //
  /////////////////////////////////////////////////

   RooArgSet paramOfInterest(bratio);
   RooArgList nuisance_params(bratio_eff);
   RooStats::ModelConfig model;
   model.SetWorkspace(workspace);
   model.SetPdf(totPDF);
   model.SetParametersOfInterest(paramOfInterest);
   model.SetObservables(bdt_out);
   model.SetNuisanceParameters(nuisance_params);
   model.SetName("S+B Model");

   auto bModel = model.Clone();
   bModel->SetName("B Model");
   double oldval = bratio.getVal();
   bratio.setVal(0.);
   bModel->SetSnapshot(paramOfInterest);
   bratio.setVal(oldval);

   RooStats::AsymptoticCalculator fc(combined_data, *bModel, model);
   fc.SetOneSided(1);
   //create a hypotest inverter passing the desired calculator
   RooStats::HypoTestInverter calc(fc);
   calc.SetConfidenceLevel(0.95);

   //use CLs
   calc.UseCLs(1);
   calc.SetVerbose(0);
   //configure toy MC sampler
   auto toymc = fc.GetTestStatSampler();
   //set profile likelihood test statistics
   RooStats::ProfileLikelihoodTestStat profl(*(model.GetPdf()));
   //for CLs (bounded intervals) use one-sided profile likelihood
   profl.SetOneSided(1);
   //est the test statistic to use
   toymc->SetTestStatistic(&profl);

   int npoints = 200; //number of points to scan
   double min_scan = 1.e-10;
   double max_scan = 3.e-7;
   calc.SetFixedScan(npoints, min_scan, max_scan);

   auto result = calc.GetInterval();
   double upperLimit  = result->UpperLimit();
   double expectedUL  = result->GetExpectedUpperLimit( 0);
   double expectedULM = result->GetExpectedUpperLimit(-1);
   double expectedULP = result->GetExpectedUpperLimit( 1);
   std::cout << "##################################################\n"
             << "The fit branching fraction is " << oldval << std::endl
             << "The observed upper limit is " << upperLimit << std::endl;

   //compute the expected limit
   std::cout << "Expected upper limits, using the alternate model: " << std::endl
             << " expected limit (median): " << expectedUL  << std::endl
             << " expected limit (-1 sig): " << expectedULM << std::endl
             << " expected limit (+1 sig): " << expectedULP << std::endl
             << "##################################################\n";


  RooStats::HypoTestInverterPlot freq_plot("HTI_Result_Plot","Expected CLs",result);
  //xPlot in a new canvas with style
  TCanvas* canvas = new TCanvas();
  canvas->cd();
  freq_plot.Draw();
  canvas->SetLogx();
  //Add upper limit info to the plot
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.04);
  label.SetTextColor(kRed);
  label.SetTextAlign(13);
  label.DrawLatex(0.12, 0.34, Form("Expected 95%% CL = %.2e^{+%.2e}_{-%.2e}",
                                   expectedUL, expectedULP-expectedUL, expectedUL-expectedULM));
  label.DrawLatex(0.12, 0.26, Form("Observed 95%% CL = %.2e", upperLimit));

  canvas->SaveAs("fit_BDToutput_limit.png");
}
