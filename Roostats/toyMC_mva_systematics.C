//Script to perform toy MC fits using same PDF to generate and fit

int toyMC_mva_systematics(int set = 8, TString selection = "zmutau",
                          vector<int> years = {2016, 2017, 2018},
                          int systematic = 5,
                          int nfits = 500,
                          bool print = true,
                          bool self_test = false) {
  int status(0);
  int seed = 90;
  TRandom3* rnd = new TRandom3(seed);
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }
  TFile* fInput = TFile::Open(Form("workspaces/hist_background_mva_%s_%s_%i.root", selection.Data(), year_string.Data(), set), "READ");
  if(!fInput) return 1;
  fInput->cd();

  RooWorkspace* ws = (RooWorkspace*) fInput->Get("ws");
  ws->Print();

  //Fit using the standard PDF
  auto fit_PDF = ws->pdf("totPDF");

  //PDF to generate data
  auto gen_PDF = (self_test) ? fit_PDF : ws->pdf(Form("totPDF_sys_%i", systematic));
  if(!gen_PDF) {
    cout << "Generating PDF not found!\n";
    return 1;
  }


  //variables of interest
  auto mva = ws->var("mva");
  auto br_sig = ws->var("br_sig");


  //Set an example branching ratio
  double true_br_sig = (selection.Contains("h") ? 5.e-3 : 5.e-5);

  //Create histograms for the fit results
  TH1F* hBrSig  = new TH1F("hbrsig" , "Branching Ratios", 80, min(-2.*true_br_sig, -3.e-6), max(2.*true_br_sig, 3.e-6));
  TH1F* hBrDiff = new TH1F("hbrdiff", "Branching Ratio Errors", 80, min(-2.*true_br_sig, -3.e-6), max(2.*true_br_sig, 3.e-6));
  TH1F* hBrPull = new TH1F("hbrpull", "Branching Ratio Pulls", 80, -10., 10.);

  //perform each fit
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  RooCategory* categories = (RooCategory*) (ws->obj(selection.Data()));
  for(int ifit = 0; ifit < nfits; ++ifit) {
    br_sig->setVal(true_br_sig); //reset the branching fraction to the true value
    //Generate data using the generating PDF --> generate data for each category
    int index = 0;
    map<string, RooDataHist*> dataCategoryMap;
    while(ws->pdf(Form("bkgMVAPDF_%i_sys_%i", index, systematic))) {
      auto bkgMVAPDF = ws->pdf((self_test) ? Form("bkgMVAPDF_%i", index) : Form("bkgMVAPDF_%i_sys_%i", index, systematic));
      auto n_bkg = ws->var((self_test) ? Form("n_bkg_%i", index) : Form("n_bkg_%i_sys_%i", index, systematic));
      auto n_sig = ws->function((self_test) ? Form("n_sig_%i", index) : Form("n_sig_%i_sys_%i", index, systematic));
      int n_bkg_events = rnd->Poisson(n_bkg->getVal());
      int n_sig_events = rnd->Poisson(n_sig->getVal());
      cout << "Generating background data for index " << index << " with " << n_bkg_events << " events\n";
      RooDataHist* bkg_mva_gen = bkgMVAPDF->generateBinned(RooArgSet(*mva), n_bkg_events);
      cout << "Generating signal data for index " << index << " with " << n_sig_events << " events\n";
      auto sigMVAPDF = ws->pdf((self_test) ? Form("sigMVAPDF_%i", index) : Form("sigMVAPDF_%i_sys_%i", index, systematic));
      RooDataHist* sig_mva_gen = sigMVAPDF->generateBinned(RooArgSet(*mva), n_sig_events);
      if(ifit == 0) sigMVAPDF->Print();
      bkg_mva_gen->add(*sig_mva_gen);
      if(ifit == 0) bkg_mva_gen->Print();
      string category = Form("%s_%i", selection.Data(), index);
      dataCategoryMap[category] = bkg_mva_gen;
      cout << "Category " << index << " has " << n_bkg->getVal() << " background and "
           << n_sig->getVal() << " signal\n";
      ++index;
    }

    RooDataHist combined_data("combined_data", "combined_data", *mva, *categories, dataCategoryMap);
    if(ifit == 0) {
      combined_data.Print();
      for(int i = 0; i < index; ++i) {
        dataCategoryMap[Form("%s_%i", selection.Data(), i)]->Print();
      }
    }
    if(ifit % 10 == 0) cout << "Performing fit " << ifit << endl;
    fit_PDF->fitTo(combined_data, RooFit::Extended(1));
    if(ifit == 0) {
      cout << "Finished the fit!\n";
      br_sig->Print();
      fit_PDF->Print();
      cout << "Fit br_sig - true br_sig = " << br_sig->getVal() << " - " << true_br_sig
           << " = " << br_sig->getVal()-true_br_sig << endl;
      if(ws->var("br_ll")) ws->var("br_ll")->Print();
    }
    hBrSig->Fill(br_sig->getVal());
    hBrDiff->Fill(br_sig->getVal() - true_br_sig);
    hBrPull->Fill((br_sig->getVal() - true_br_sig)/br_sig->getError());

    //print an example fit
    if(ifit == 0) {
      for(unsigned i = 0; i < index; ++i) {
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
    for(int i = 0; i < index; ++i) {
      delete dataCategoryMap[Form("%s_%i", selection.Data(), i)];
    }
  }

  auto c1 = new TCanvas("fit_results", "Fit Results", 1000, 800);
  c1->Divide(2,2);
  c1->cd(1);
  hBrSig->Draw();
  c1->cd(2);
  hBrDiff->Draw();
  c1->cd(3);
  hBrPull->Fit("gaus");
  hBrPull->Draw();
  c1->cd(4);
  auto xframe = mva->frame(50);
  fit_PDF->plotOn(xframe, RooFit::Components("bkgMVAPDF_0"), RooFit::Slice(*categories, Form("%s_0", selection.Data())));
  if(!self_test) gen_PDF->plotOn(xframe, RooFit::Components(Form("bkgMVAPDF_0_sys_%i", systematic)), RooFit::Slice(*categories, Form("%s_0", selection.Data())),
                                 RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  xframe->Draw();
  if(self_test)
    c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_results_self_%s_%i.png", year_string.Data(), selection.Data(), set));
  else
    c1->SaveAs(Form("plots/latest_production/%s/toyMC_mva_systematics_results_%i_%s_%i.png", year_string.Data(), systematic, selection.Data(), set));

  return status;
}