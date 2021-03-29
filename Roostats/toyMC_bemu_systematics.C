//Script to perform toy MC fits using either the same PDF to generate and fit
// or a systematically shifted generation PDF and the nominal to fit

int toyMC_bemu_systematics(int set = 8, TString selection = "zemu",
                           vector<int> years = {2016, 2017, 2018},
                           int systematic = 5,
                           int nfits = 1000,
                           bool print = true,
                           bool self_test = false,
                           int seed = 90) {
  int status(0);
  double scale_lum = -1.; //for testing effects from changing the luminosity
  double fit_bias = 0.;
  bool doHiggs = selection.Contains("h");
  if     (selection == "zemu") fit_bias = -1.4e-8;
  else if(selection == "hemu") fit_bias = 0.;

  TRandom3* rnd = new TRandom3(seed);

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }

  //Get the workspace with the background + signal PDFs
  TFile* fInput = TFile::Open(Form("workspaces/fit_%s_lepm_background_binned_%s_%i.root", selection.Data(), year_string.Data(), set), "READ");
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
  auto genPDF = (self_test) ? fit_PDF : ws->pdf(Form("totPDF_sys_%i", systematic));
  if(!genPDF) {
    cout << "Generating PDF not found!\n";
    return 1;
  }


  //variables of interest
  auto lepm = ws->var("lepm");
  auto br_sig = ws->var("br_sig");
  auto lum_var = ws->var("lum_var");

  //turn off systematic constraints for evaluating these uncertainties
  if(ws->var("br_sig_beta")) {
    auto br_sig_beta = ws->var("br_sig_beta");
    br_sig_beta->setVal(0.);
    br_sig_beta->setConstant(1);
    cout << "Setting branching fraction systematic factor to constant!\n";
  }

  //Set an example branching ratio
  double true_br_sig = (selection.Contains("h") ? 1.e-3 : 1.e-6);
  br_sig->setVal(true_br_sig); //set the branching fraction to the true value

  //Store the nominal number of background and signal events per category
  auto n_bkg = ws->var((self_test) ? "n_bkg" : Form("n_bkg_sys_%i", systematic));
  auto n_sig = ws->function((self_test) ? "n_sig" : Form("n_sig_sys_%i", systematic));
  double init_n_bkg = n_bkg->getVal();
  double init_n_sig = n_sig->getVal();
  if(scale_lum > 0.) {init_n_bkg *= scale_lum; init_n_sig *= scale_lum;}
  if(init_n_bkg <= 0. || init_n_sig <= 0) {
    cout << "Systematic has an undefined event numbers! N(bkg) = " << init_n_bkg
         << " and N(sig) = " << init_n_sig << endl;
    return 2;
  }

  cout << "--- Initial background = " << init_n_bkg << " and signal = " << init_n_sig << endl;
  //if scaling the luminosity, update the luminosity variable to propagate to br_sig calculation
  if(scale_lum > 0.) {
    lum_var->setVal(scale_lum*lum_var->getVal());
  }

  //Create histograms for the fit results
  TH1F* hBrSig   = new TH1F("hbrsig"  , "Branching Ratios", 80, min(-2.*true_br_sig, -1.e-6), max(2.*true_br_sig, 1.e-6));
  TH1F* hBrUnc   = new TH1F("hbrunc"  , "Branching Ratio Uncertainty", 80, min(-2.*true_br_sig, -1.e-6), max(2.*true_br_sig, 1.e-6));
  TH1F* hBrDiff  = new TH1F("hbrdiff" , "Branching Ratio Errors", 80, min(-2.*true_br_sig, -1.e-6), max(2.*true_br_sig, 1.e-6));
  TH1F* hBrDiffB = new TH1F("hbrdiffb", "Branching Ratio Errors with bias offset", 80, min(-2.*true_br_sig, -1.e-6), max(2.*true_br_sig, 1.e-6));
  TH1F* hBrPull  = new TH1F("hbrpull" , "Branching Ratio Pulls", 80, -10., 10.);
  TH1F* hNLL     = new TH1F("hnll"    , "Fit NLL", 100, -2.67e7, -2.62e7);
  bool donll = false;

  /////////////////////////
  // Fit Loop
  /////////////////////////

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  RooDataHist* data_last; //save just for plotting
  for(int ifit = 0; ifit < nfits; ++ifit) {
    br_sig->setVal(true_br_sig); //reset the branching fraction to the true value
    //Generate data using the generating PDF --> generate data for each category
    //vary the number of events using a Poisson distribution with expected as the mean
    int n_events = rnd->Poisson(init_n_bkg + init_n_sig);
    if(n_events < 0) {
      cout << "Fit has an undefined event numbers! N = " << n_events << endl;
      return 3;
    }
    data_last = genPDF->generateBinned(RooArgSet(*lepm), n_events);

    if(ifit == 0) {
      data_last->Print();
    }
    //set the branching fraction to a random value near the true value
    br_sig->setVal(true_br_sig * (1.5 - rnd->Uniform()));

    //perform the fit
    if(ifit % 10 == 0) cout << "Performing fit " << ifit << endl;
    fit_PDF->fitTo(*data_last, RooFit::Extended(1), RooFit::PrintLevel(0), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    if(ifit == 0) {
      cout << "Finished the fit!\n";
      br_sig->Print();
      fit_PDF->Print();
      cout << "Fit br_sig - true br_sig = " << br_sig->getVal() << " - " << true_br_sig
           << " = " << br_sig->getVal()-true_br_sig << " - bias = "
           << br_sig->getVal()-true_br_sig - fit_bias << endl;
    }

    //store the fit results
    hBrSig->Fill(br_sig->getVal());
    hBrDiff->Fill(br_sig->getVal() - true_br_sig);
    hBrDiffB->Fill(br_sig->getVal() - true_br_sig - fit_bias);
    hBrUnc->Fill(br_sig->getError());
    hBrPull->Fill((br_sig->getVal() - true_br_sig - fit_bias)/br_sig->getError());
    if(donll) {
      auto nll = fit_PDF->createNLL(*data_last);
      if(nll) hNLL->Fill(nll->getVal());
    }

    //print an example fit
    if(ifit == 0 && print) {
      auto xframe = lepm->frame();
      data_last->plotOn(xframe);
      fit_PDF->plotOn(xframe, RooFit::Components("sigPDF"));
      fit_PDF->plotOn(xframe, RooFit::Components((doHiggs) ? "bkgPDF_bst3" : "bkgPDF_bst4"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      auto c1 = new TCanvas();
      xframe->Draw();
      TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
      leg->AddEntry((TH1*) (c1->GetPrimitive("totPDF_Norm[lepm]_Comp[sigPDF]")), "Signal", "L");
      leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totPDF_Norm[lepm]_Comp[bkgPDF_bst%i]", 4-doHiggs))), "Background", "L");
      leg->Draw();
      if(self_test)
        c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_self_%s_%i.png", year_string.Data(), selection.Data(), set));
      else
        c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_%i_%s_%i.png", year_string.Data(), systematic, selection.Data(), set));
      c1->SetLogy();
      if(self_test)
        c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_self_%s_%i_log.png", year_string.Data(), selection.Data(), set));
      else
        c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_%i_%s_%i_log.png", year_string.Data(), systematic, selection.Data(), set));
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
  if(hBrDiffB->Integral() > 0.) {
    hBrDiffB->Fit("gaus");
    hBrDiffB->Draw();
  } else {
    hBrUnc->Draw();
  }
  c1->cd(4);
  hBrPull->Fit("gaus");
  hBrPull->Draw();
  // hNLL->Draw();
  if(print) {
    if(self_test)
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_results_self_%s_%i.png", year_string.Data(), selection.Data(), set));
    else
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_results_%i_%s_%i.png", year_string.Data(), systematic, selection.Data(), set));
  }

  return status;
}
