//Script to perform toy MC fits using alternate background parameterizations
//for the generating PDF
bool use_bernstein_ = true;

int toyMC_bemu_bkg_func(vector<int> sets = {8}, TString selection = "zemu",
                        vector<int> years = {2016, 2017, 2018},
                        int nfits = 1000,
                        bool self_test = false,
                        bool zero_signal = false,
                        bool print = true,
                        int seed = 90) {
  int status(0);
  double scale_lum = -1.; //for testing effects from changing the luminosity
  double fit_bias = 0.;
  bool doHiggs = selection.Contains("h");
  if     (selection == "zemu") fit_bias = 0.;//-1.4e-8;
  else if(selection == "hemu") fit_bias = 0.;

  TRandom3* rnd = new TRandom3(seed);

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }

  TString set_str = "";
  for(int set : sets) {
    if(set_str == "") set_str += set;
    else {set_str += "_"; set_str += set;}
  }

  //Get the workspace with the background + signal PDFs
  TFile* fInput = TFile::Open(Form("workspaces/fit_%s_lepm_background_binned_%s_%s.root", selection.Data(), year_string.Data(), set_str.Data()), "READ");
  if(!fInput) return 1;
  fInput->cd();

  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  ws->Print();

  //Fit using the standard PDF
  RooSimultaneous* fitPDF = (RooSimultaneous*) ws->pdf("totPDF");
  if(!fitPDF) {
    cout << "Fitting PDF not found!\n";
    return 1;
  }
  cout << "Retrieved the fitting PDF:\n";
  fitPDF->Print();
  for(int icat = 0; icat < sets.size(); ++icat) {
    auto pdf = (RooAddPdf*) fitPDF->getPdf(Form("%s_%i", selection.Data(), icat));
    if(pdf) {
      pdf->Print();
      auto bkg = ws->pdf(Form("bkgPDF_%i", sets[icat]));
      if(bkg) bkg->Print();
    }
    else {
      cout << "Unable to find underylying fit PDF for set " << sets[icat] << "(cat " << icat << ")!\n";
    }
  }
  //PDF to generate data
  TString gen_pdf_name;
  if(self_test)                        gen_pdf_name = "totPDF";
  else if( doHiggs &&  use_bernstein_) gen_pdf_name = "totPDF_bst4";
  else if( doHiggs && !use_bernstein_) gen_pdf_name = "totPDF_cheb4";
  else if(!doHiggs &&  use_bernstein_) gen_pdf_name = "totPDF_bst4";
  else if(!doHiggs && !use_bernstein_) gen_pdf_name = "totPDF_cheb5";
  RooSimultaneous* genPDF = (RooSimultaneous*) ws->pdf(gen_pdf_name.Data());
  if(!genPDF) {
    cout << "Generating PDF " << gen_pdf_name.Data() << " not found!\n";
    return 1;
  }
  cout << "Using generating PDF " << gen_pdf_name.Data() << endl;
  genPDF = (RooSimultaneous*) genPDF->clone("genPDF");
  genPDF->Print();

  //variables of interest
  auto lepm = ws->var("lepm");
  auto br_sig = ws->var("br_sig");
  RooCategory* categories = (RooCategory*) (ws->obj(selection.Data()));
  if(!categories) {
    cout << "Failed to find category definition!\n";
    return 2;
  }

  //turn off systematic constraints for evaluating these uncertainties
  if(ws->var("br_sig_beta")) {
    auto br_sig_beta = ws->var("br_sig_beta");
    br_sig_beta->setVal(0.);
    br_sig_beta->setConstant(1);
    cout << "Setting branching fraction systematic factor to constant!\n";
  }

  //Set an example branching ratio
  bool ishiggs = selection.Contains("h");
  double true_br_sig = (ishiggs ? 2.e-4 : 5.e-7);
  if(zero_signal) true_br_sig = 0.;
  br_sig->setVal(true_br_sig); //set the branching fraction to the true value

  //Store the parameters for the generating PDF, to restore post-fit
  RooArgSet* params = genPDF->getParameters(RooArgSet(*lepm, *categories)) ;

  // Make a snapshot of the parameterlist
  RooArgSet* savedParams = (RooArgSet*) params->snapshot() ;

  //////////////////////////////////////////////////////////////////////////
  // Get the intial estimates for each category
  //////////////////////////////////////////////////////////////////////////

  vector<double> init_bkgs;
  vector<double> init_sigs;

  double ntot_bkg(0.), ntot_sig(0.);
  for(int set : sets) {
    auto n_bkg = ws->var(Form("n_bkg_%i", set));
    auto n_sig = ws->function(Form("n_sig_%i", set));
    double init_n_bkg = n_bkg->getVal();
    double init_n_sig = n_sig->getVal();
    if(scale_lum > 0.) {init_n_bkg *= scale_lum; init_n_sig *= scale_lum;}
    if(init_n_bkg <= 0. || (init_n_sig <= 0 && !zero_signal)) {
      cout << "Undefined event numbers in set " << set << "! N(bkg) = " << init_n_bkg
           << " and N(sig) = " << init_n_sig << endl;
      return 3;
    }
    init_bkgs.push_back(init_n_bkg);
    init_sigs.push_back(init_n_sig);
    cout << "--- Set " << set << ": Initial background = " << init_n_bkg << " and signal = " << init_n_sig << endl;
    ntot_bkg += init_n_bkg;
    ntot_sig += init_n_sig;
  }


  //if scaling the luminosity, update the luminosity variable to propagate to br_sig calculation
  // if(scale_lum > 0.) {
  //   lum_var->setVal(scale_lum*lum_var->getVal());
  // }

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
    if(ifit == 0) cout << "--- Beginning first fit...\n";
    br_sig->setVal(true_br_sig); //reset the branching fraction to the true value

    //Let the genPDF, which is extended, randomly select the category based on the fraction of events in either
    int n_events = rnd->Poisson(ntot_bkg + ntot_sig);
    data_last = genPDF->generateBinned(RooArgSet(*lepm, *categories), n_events);
    if(ifit == 0) cout << "--- Finished generation of data...\n";

    //set the branching fraction to a random value near the true value
    if(zero_signal) {
      br_sig->setVal(((ishiggs) ? 1.e-3 : 1.e-7) * (1.5 - rnd->Uniform()));
    } else {
      br_sig->setVal(true_br_sig * (1.5 - rnd->Uniform()));
    }

    //perform the fit
    if(ifit % 10 == 0) cout << "Performing fit " << ifit << endl;
    fitPDF->fitTo(*data_last, RooFit::PrintLevel(0), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    if(ifit == 0) {
      cout << "Finished the fit!\n";
      br_sig->Print();
      fitPDF->Print();
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
      auto nll = fitPDF->createNLL(*data_last);
      if(nll) hNLL->Fill(nll->getVal());
    }

    //print an example fit
    if(ifit == 0 && print) {
      for(int iplot = 0; iplot < sets.size(); ++iplot) {
        auto xframe = lepm->frame();
        data_last->plotOn(xframe, RooFit::Cut(Form("%s == %i", selection.Data(), iplot)));
        fitPDF->plotOn(xframe, RooFit::Components(Form("sigPDF_%i", sets[iplot])), RooFit::Slice(*categories, Form("%s_%i", selection.Data(), iplot)),
                       RooFit::ProjWData(*data_last));
        fitPDF->plotOn(xframe, RooFit::Components(Form("bkgPDF_%i", sets[iplot])), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed),
                       RooFit::Slice(*categories, Form("%s_%i", selection.Data(), iplot)),
                       RooFit::ProjWData(*data_last));
        // fitPDF->plotOn(xframe, RooFit::Components((doHiggs) ? "bkgPDF_bst3" : "bkgPDF_bst4"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
        auto c1 = new TCanvas();
        xframe->Draw();
        TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
        leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totPDF_%i_Norm[lepm]_Comp[sigPDF_%i]", sets[iplot], sets[iplot]))), "Signal", "L");
        leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totPDF_%i_Norm[lepm]_Comp[bkgPDF_%i]", sets[iplot], sets[iplot]))), "Background", "L");
        leg->Draw();
        if(self_test)
          c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_bkg_func_self%s_%s_%s_cat_%i.png",
                          year_string.Data(), (zero_signal) ? "_nosig" : "", selection.Data(), set_str.Data(), iplot));
        else
          c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_bkg_func%s_%s_%s_cat_%i.png",
                          year_string.Data(), (zero_signal) ? "_nosig" : "", selection.Data(), set_str.Data(), iplot));
        c1->ls();
      }
    }
    if(ifit < nfits - 1) delete data_last;
    //restore the genPDF parameters
    *params = *savedParams;
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
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_results_self%s_%s_%s.png",
                      year_string.Data(), (zero_signal) ? "_nosig" : "", selection.Data(), set_str.Data()));
    else
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_results%s_%s_%s.png",
                      year_string.Data(), (zero_signal) ? "_nosig" : "", selection.Data(), set_str.Data()));
  }

  return status;
}
