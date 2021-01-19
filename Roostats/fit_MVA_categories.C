Int_t fit_MVA_categories(int set = 8, TString selection = "zmutau",
			 vector<int> years = {2016/*, 2017, 2018*/},
			 int seed = 90) {
  gSystem->Load("libRooFit");

  int status(0);
  vector<TString> hists;
  if     (selection == "hmutau"  ) hists = {"mva0", "mva6"};
  else if(selection == "zmutau"  ) hists = {"mva1", "mva7"};
  else if(selection == "hetau"   ) hists = {"mva2", "mva8"};
  else if(selection == "zetau"   ) hists = {"mva3", "mva9"};
  else if(selection == "hemu"    ) hists = {"mva4"};
  else if(selection == "zemu"    ) hists = {"mva5"};
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }
  
  //create list of data selections
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  vector<TString> selecs;
  if(selec.Contains("tau")) selecs = {selec, "emu"};
  else selecs = {selec};

  //detemine the signal name
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  vector<int> set_offsets;
  for(TString s : selec) {
    int set_offset = ZTauTauHistMaker::kEMu;
    if     (selec == "mutau") set_offset = ZTauTauHistMaker::kMuTau;
    else if(selec == "etau" ) set_offset = ZTauTauHistMaker::kETau;
    set_offsets.push_back(set_offset);
  }
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  //Define the observable
  RooRealVar bdt_out("bdt_out","The BDT output",-1.,1.);

  //Generate a sample, but this is just for the example, in reality you'll have the BDT outputs for signal and background
  RooRealVar sig_land_mean("sig_land_mean","sig_land_mean",0.8);
  RooRealVar sig_land_sigma("sig_land_sigma","sig_land_sigma",0.3);
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
  sig_land_sigma.setVal(0.25);
  bkg_land_mean.setVal(-0.55);
  bkg_land_sigma.setVal(0.35);

  RooDataHist *sig_bdtout_mutau = land_signal.generateBinned(RooArgSet(bdt_out),10000);
  RooDataHist *bkg_bdtout_mutau = land_backgr.generateBinned(RooArgSet(bdt_out),10000);

  //Now let's say we have some data to fit. It can also be the MC total dataset. Let's say we have 50 signal events and 1000 background
  RooDataHist *sig_bdtgen_mutau = land_signal.generateBinned(RooArgSet(bdt_out),40);
  RooDataHist *bkg_bdtgen_mutau = land_backgr.generateBinned(RooArgSet(bdt_out),800);

  sig_bdtgen_mutau->add(*bkg_bdtgen_mutau);

  //Create the signal and background PDFs and combine them
  RooRealVar lumi("lumi","lumi",150.);
  RooRealVar xsec("xsec","Z prod xsec",6225.*1000.);   
  RooRealVar bratio("bratio","Branching ratio",0.00001,0.,0.001);

  ///////////////////////
  //first for electron-tau
  RooHistPdf sig_histpdf_etau("sig_histpdf_etau","The signal histogram PDF",RooArgSet(bdt_out),*sig_bdtout_etau);
  RooHistPdf bkg_histpdf_etau("bkg_histpdf_etau","The signal histogram PDF",RooArgSet(bdt_out),*bkg_bdtout_etau);

  RooRealVar eff_etau("eff_etau","etau efficiency",0.9);

  RooFormulaVar Nsig_etau("Nsig_etau","@0*@1*@2*@3",RooArgList(lumi,xsec,bratio,eff_etau));
  RooRealVar Nbkg_etau("Nbkg_etau","Number of background events in etau",1000.,0.,2000.);

  RooAddPdf totpdf_etau("totpdf_etau","Total PDF for etau",RooArgList(sig_histpdf_etau,bkg_histpdf_etau),RooArgList(Nsig_etau,Nbkg_etau));

  ///////////////////////
  //second for muon-tau
  RooHistPdf sig_histpdf_mutau("sig_histpdf_mutau","The signal histogram PDF",RooArgSet(bdt_out),*sig_bdtout_mutau);
  RooHistPdf bkg_histpdf_mutau("bkg_histpdf_mutau","The signal histogram PDF",RooArgSet(bdt_out),*bkg_bdtout_mutau);

  RooRealVar eff_mutau("eff_mutau","mutau efficiency",0.8);

  RooFormulaVar Nsig_mutau("Nsig_mutau","@0*@1*@2*@3",RooArgList(lumi,xsec,bratio,eff_mutau));
  RooRealVar Nbkg_mutau("Nbkg_mutau","Number of background events in mutau",800.,0.,2000.);

  RooAddPdf totpdf_mutau("totpdf_mutau","Total PDF for mutau",RooArgList(sig_histpdf_mutau,bkg_histpdf_mutau),RooArgList(Nsig_mutau,Nbkg_mutau));

  //Now combine the two PDFs by defining categories
  RooCategory lepcat("lepcat","lepcat");
  lepcat.defineType("etau",0);
  lepcat.defineType("mutau",1);

  RooSimultaneous totPDF("totPDF","The total PDF",lepcat); 
  totPDF.addPdf(totpdf_etau,"etau");
  totPDF.addPdf(totpdf_mutau,"mutau");

  //Combine the datasets

  RooDataHist combined_data("combined_data","combined_data",bdt_out,RooFit::Index(lepcat),RooFit::Import("etau",*sig_bdtgen_etau),RooFit::Import("mutau",*sig_bdtgen_mutau));

  //Now fit, plot, save, enjoy
  totPDF.fitTo(combined_data,RooFit::Extended(1));

  //Plot for one category, for example etau
  RooPlot *bdtout_plot = bdt_out.frame(50);
  combined_data.plotOn(bdtout_plot,RooFit::Cut("lepcat==0"));
  totPDF.plotOn(bdtout_plot,RooFit::Slice(lepcat,"etau"),RooFit::ProjWData(combined_data));

  TCanvas c1;
  c1.cd();
  bdtout_plot->Draw();
  c1.SaveAs("fit_BDToutput.pdf");

  RooWorkspace workspace("myworkspace");
  workspace.import(totPDF);
  workspace.import(combined_data);

  TFile fOut("fit_BDToutput.root","RECREATE");
  fOut.cd();
  workspace.Write();
  fOut.Close();

}
