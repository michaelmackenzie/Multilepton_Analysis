//Script to perform toy MC fits using either the same PDF to generate and fit
// or a systematically shifted generation PDF and the nominal to fit

int toyMC_mva_systematics(int set = 8, TString selection = "zmutau",
                          vector<int> years = {2016, 2017, 2018},
                          int systematic = 5,
                          int nfits = 500,
                          bool print = true,
                          bool self_test = false) {
  int status(0);
  int seed = 90;
  double scale_lum = -1.; //for testing effects from changing the luminosity
  TRandom3* rnd = new TRandom3(seed);

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }

  //Get the workspace with the background + signal PDFs
  TFile* fInput = TFile::Open(Form("workspaces/hist_background_mva_%s_%s_%i.root", selection.Data(), year_string.Data(), set), "READ");
  if(!fInput) return 1;
  fInput->cd();

  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  ws->Print();

  //Fit using the standard PDF
  auto fit_PDF = ws->pdf("totPDF");
  if(!fit_PDF) {
    cout << "Fitting PDF not found!\n";
    return 1;
  }

  //PDF to generate data
  auto gen_PDF = (self_test) ? fit_PDF : ws->pdf(Form("totPDF_sys_%i", systematic));
  if(!gen_PDF) {
    cout << "Generating PDF not found!\n";
    return 1;
  }


  //variables of interest
  auto mva = ws->var("mva");
  auto br_sig = ws->var("br_sig");
  auto lum_var = ws->var("lum_var");


  //Set an example branching ratio
  double true_br_sig = (selection.Contains("h") ? 5.e-3 : 5.e-6);
  br_sig->setVal(true_br_sig); //set the branching fraction to the true value

  //Store the nominal number of background and signal events per category
  vector<double> n_bkgs, n_sigs;
  int index = 0;
  while(ws->pdf((self_test) ? Form("bkgMVAPDF_%i", index) : Form("bkgMVAPDF_%i_sys_%i", index, systematic))) {
    auto n_bkg = ws->var((self_test) ? Form("n_bkg_%i", index) : Form("n_bkg_%i_sys_%i", index, systematic));
    auto n_sig = ws->function((self_test) ? Form("n_sig_%i", index) : Form("n_sig_%i_sys_%i", index, systematic));
    n_bkgs.push_back((scale_lum > 0.) ? scale_lum*n_bkg->getVal() : n_bkg->getVal());
    n_sigs.push_back((scale_lum > 0.) ? scale_lum*n_sig->getVal() : n_sig->getVal());
    ++index;
  }

  //if scaling the luminosity, update the luminosity variable to propagate to br_sig calculation
  if(scale_lum > 0.) {
    lum_var->setVal(scale_lum*lum_var->getVal());
  }

  //Create histograms for the fit results
  TH1F* hBrSig  = new TH1F("hbrsig" , "Branching Ratios", 80, min(-2.*true_br_sig, -3.e-6), max(2.*true_br_sig, 3.e-6));
  TH1F* hBrDiff = new TH1F("hbrdiff", "Branching Ratio Errors", 80, min(-2.*true_br_sig, -3.e-6), max(2.*true_br_sig, 3.e-6));
  TH1F* hBrPull = new TH1F("hbrpull", "Branching Ratio Pulls", 80, -10., 10.);
  TH1F* hNLL    = new TH1F("hnll", "Fit NLL", 100, -2.67e7, -2.62e7);
  bool donll = false;

  /////////////////////////
  // Fit Loop
  /////////////////////////

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  RooCategory* categories = (RooCategory*) (ws->obj(selection.Data()));
  RooDataHist* combined_data_last; //save just for plotting
  for(int ifit = 0; ifit < nfits; ++ifit) {
    br_sig->setVal(true_br_sig); //reset the branching fraction to the true value
    //Generate data using the generating PDF --> generate data for each category
    map<string, RooDataHist*> dataCategoryMap;
    for(unsigned icat = 0; icat < n_bkgs.size(); ++icat) {
      //vary the number of events using a Poisson distribution with expected as the mean
      int n_bkg_events = rnd->Poisson(n_bkgs[icat]);
      int n_sig_events = rnd->Poisson(n_sigs[icat]);

      //get the background PDF
      auto bkgMVAPDF = ws->pdf((self_test) ? Form("bkgMVAPDF_%i", icat) : Form("bkgMVAPDF_%i_sys_%i", icat, systematic));
      cout << "Generating background data for index " << icat << " with " << n_bkg_events << " events (mean = "
           << n_bkgs[icat] << ")\n";
      //generate toy data
      RooDataHist* bkg_mva_gen = bkgMVAPDF->generateBinned(RooArgSet(*mva), n_bkg_events);

      //get the signal PDF
      auto sigMVAPDF = ws->pdf((self_test) ? Form("sigMVAPDF_%i", icat) : Form("sigMVAPDF_%i_sys_%i", icat, systematic));
      cout << "Generating signal data for index " << icat << " with " << n_sig_events << " events (mean = "
           << n_sigs[icat] << ")\n";
      //generate toy data
      RooDataHist* sig_mva_gen = sigMVAPDF->generateBinned(RooArgSet(*mva), n_sig_events);
      if(ifit == 0) sigMVAPDF->Print();

      //add the signal to the background data, to fit the total
      bkg_mva_gen->add(*sig_mva_gen);
      if(ifit == 0) bkg_mva_gen->Print();

      //store the resulting data in the category map
      string category = Form("%s_%i", selection.Data(), icat);
      dataCategoryMap[category] = bkg_mva_gen;

      cout << "Category " << icat << " has " << n_bkg_events << " background and "
           << n_sig_events << " signal\n";
    } //end category loop

    //define the total dataset
    RooDataHist combined_data("combined_data", "combined_data", *mva, *categories, dataCategoryMap);
    if(ifit == 0) {
      combined_data.Print();
      for(unsigned i = 0; i < n_bkgs.size(); ++i) {
        dataCategoryMap[Form("%s_%i", selection.Data(), i)]->Print();
      }
    }
    //set the branching fraction to a random value near the true value
    br_sig->setVal(true_br_sig * (1.5 - rnd->Uniform()));

    //perform the fit
    if(ifit % 10 == 0) cout << "Performing fit " << ifit << endl;
    fit_PDF->fitTo(combined_data, RooFit::Extended(1));
    if(ifit == 0) {
      cout << "Finished the fit!\n";
      br_sig->Print();
      fit_PDF->Print();
      cout << "Fit br_sig - true br_sig = " << br_sig->getVal() << " - " << true_br_sig
           << " = " << br_sig->getVal()-true_br_sig << endl;
    }

    //store the fit results
    hBrSig->Fill(br_sig->getVal());
    hBrDiff->Fill(br_sig->getVal() - true_br_sig);
    hBrPull->Fill((br_sig->getVal() - true_br_sig)/br_sig->getError());
    if(donll) {
      auto nll = fit_PDF->createNLL(combined_data);
      if(nll) hNLL->Fill(nll->getVal());
    }

    //print an example fit
    if(ifit == 0 && print) {
      for(unsigned i = 0; i < n_bkgs.size(); ++i) {
        auto xframe = mva->frame(50);
        combined_data.plotOn(xframe, RooFit::Cut(Form("%s==%i",selection.Data(),i)));
        fit_PDF->plotOn(xframe, RooFit::Components(Form("sigMVAPDF_%i", i)),
                        RooFit::Slice(*categories, Form("%s_%i", selection.Data(),i)), RooFit::ProjWData(combined_data));
        fit_PDF->plotOn(xframe, RooFit::Components(Form("bkgMVAPDF_%i", i)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed),
                        RooFit::Slice(*categories, Form("%s_%i", selection.Data(),i)), RooFit::ProjWData(combined_data));
        auto c1 = new TCanvas();
        xframe->Draw();
        TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
        leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totMVAPDF_%i_Norm[mva]_Comp[sigMVAPDF_%i]", i, i))), "Signal", "L");
        leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totMVAPDF_%i_Norm[mva]_Comp[bkgMVAPDF_%i]", i, i))), "Background", "L");
        leg->Draw();
        if(self_test)
          c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_self_cat_%i_%s_%i.png", year_string.Data(), i, selection.Data(), set));
        else
          c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_%i_cat_%i_%s_%i.png", year_string.Data(), systematic, i, selection.Data(), set));
        c1->SetLogy();
        if(self_test)
          c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_self_cat_%i_%s_%i_log.png", year_string.Data(), i, selection.Data(), set));
        else
          c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_%i_cat_%i_%s_%i_log.png", year_string.Data(), systematic, i, selection.Data(), set));
      }
    }

    //cleanup memory
    for(unsigned i = 0; i < n_bkgs.size(); ++i) {
      delete dataCategoryMap[Form("%s_%i", selection.Data(), i)];
    }

  } //end fit loop


  //Create a canvas with the fit results
  auto c1 = new TCanvas("fit_results", "Fit Results", 1000, 800);
  c1->Divide(2,2);
  c1->cd(1);
  hBrSig->Draw();
  c1->cd(2);
  hBrDiff->Fit("gaus");
  hBrDiff->Draw();
  c1->cd(3);
  hBrPull->Fit("gaus");
  hBrPull->Draw();
  c1->cd(4);
  hNLL->Draw();
  if(print) {
    if(self_test)
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_results_self_%s_%i.png", year_string.Data(), selection.Data(), set));
    else
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_results_%i_%s_%i.png", year_string.Data(), systematic, selection.Data(), set));
  }

  return status;
}
