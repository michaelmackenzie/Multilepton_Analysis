//Script to perform toy MC fits using alternate background parameterizations
//for the generating PDF
bool use_bernstein_ = false;

int toyMC_bemu_bkg_func(vector<int> sets = {8}, TString selection = "zemu",
                        vector<int> years = {2016, 2017, 2018},
                        int nfits = 1000,
                        bool self_test = false,
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
  auto fitPDF = ws->pdf("totPDF");
  if(!fitPDF) {
    cout << "Fitting PDF not found!\n";
    return 1;
  }

  //PDF to generate data
  TString gen_pdf_name;
  if(self_test) gen_pdf_name = "totPDF";
  else if( doHiggs &&  use_bernstein_) gen_pdf_name = "totPDF_bst4";
  else if( doHiggs && !use_bernstein_) gen_pdf_name = "totPDF_cheb4";
  else if(!doHiggs &&  use_bernstein_) gen_pdf_name = "totPDF_bst5";
  else if(!doHiggs && !use_bernstein_) gen_pdf_name = "totPDF_cheb5";
  RooSimultaneous* genPDF = (RooSimultaneous*) ws->pdf(gen_pdf_name.Data())->clone("genPDF");
  if(!genPDF) {
    cout << "Generating PDF " << gen_pdf_name.Data() << " not found!\n";
    return 1;
  }
  cout << "Using generating PDF " << gen_pdf_name.Data() << endl;

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
  double true_br_sig = (selection.Contains("h") ? 1.e-3 : 1.e-6);
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


  for(int set : sets) {
    auto n_bkg = ws->var(Form("n_bkg_%i", set));
    auto n_sig = ws->function(Form("n_sig_%i", set));
    double init_n_bkg = n_bkg->getVal();
    double init_n_sig = n_sig->getVal();
    if(scale_lum > 0.) {init_n_bkg *= scale_lum; init_n_sig *= scale_lum;}
    if(init_n_bkg <= 0. || init_n_sig <= 0) {
      cout << "Undefined event numbers in set " << set << "! N(bkg) = " << init_n_bkg
           << " and N(sig) = " << init_n_sig << endl;
      return 3;
    }
    init_bkgs.push_back(init_n_bkg);
    init_sigs.push_back(init_n_sig);
    cout << "--- Set " << set << ": Initial background = " << init_n_bkg << " and signal = " << init_n_sig << endl;
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
  // genPDF->printTree(cout);
  RooDataHist* data_last; //save just for plotting
  for(int ifit = 0; ifit < nfits; ++ifit) {
    if(ifit == 0) cout << "--- Beginning first fit...\n";
    br_sig->setVal(true_br_sig); //reset the branching fraction to the true value

    map<string, RooDataHist*> dataCategoryMap;
    for(unsigned index = 0; index < sets.size(); ++index) {
      if(ifit == 0) cout << "--- Beginning generation of data for category " << index << "...\n";
      //Generate data using the generating PDF --> generate data for each category
      //vary the number of events using a Poisson distribution with expected as the mean
      int n_events = rnd->Poisson(init_bkgs[index] + init_sigs[index]);
      if(n_events < 0) {
        cout << "!!! Set " << sets[index] << ": Fit has an undefined event numbers! N = " << n_events << endl;
        return 4;
      }
      dataCategoryMap[Form("%s_%i", selection.Data(), index)] = genPDF->generateBinned(RooArgSet(*lepm), n_events);
    }
    if(ifit == 0) cout << "--- Finished generation of data, creating combinded data...\n";
    data_last = new RooDataHist("combined_toyMC_data", "combined_toyMC_data", *lepm, *categories, dataCategoryMap);
    //set the branching fraction to a random value near the true value
    br_sig->setVal(true_br_sig * (1.5 - rnd->Uniform()));

    //perform the fit
    if(ifit % 10 == 0) cout << "Performing fit " << ifit << endl;
    fitPDF->fitTo(*data_last, RooFit::Extended(1), RooFit::PrintLevel(0), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
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
    // if(ifit == 0 && print) {
    //   auto xframe = lepm->frame();
    //   data_last->plotOn(xframe);
    //   fitPDF->plotOn(xframe, RooFit::Components("sigPDF"));
    //   fitPDF->plotOn(xframe, RooFit::Components((doHiggs) ? "bkgPDF_bst3" : "bkgPDF_bst4"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    //   auto c1 = new TCanvas();
    //   xframe->Draw();
    //   TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
    //   leg->AddEntry((TH1*) (c1->GetPrimitive("totPDF_Norm[lepm]_Comp[sigPDF]")), "Signal", "L");
    //   leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totPDF_Norm[lepm]_Comp[bkgPDF_bst%i]", 4-doHiggs))), "Background", "L");
    //   leg->Draw();
    //   if(self_test)
    //     c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_self_%s_%i.png", year_string.Data(), selection.Data(), set));
    //   else
    //     c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_%i_%s_%i.png", year_string.Data(), systematic, selection.Data(), set));
    //   c1->SetLogy();
    //   if(self_test)
    //     c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_self_%s_%i_log.png", year_string.Data(), selection.Data(), set));
    //   else
    //     c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_%i_%s_%i_log.png", year_string.Data(), systematic, selection.Data(), set));
    // }
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
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_results_self_%s_%s.png", year_string.Data(), selection.Data(), set_str.Data()));
    else
      c1->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_systematics_results_%s_%s.png", year_string.Data(), selection.Data(), set_str.Data()));
  }

  // genPDF->printTree(cout);

  return status;
}
