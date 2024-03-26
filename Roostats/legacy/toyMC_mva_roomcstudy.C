//Script to perform toy MC fits using either the same PDF to generate and fit
// or a systematically shifted generation PDF and the nominal to fit

int toyMC_mva_roomcstudy(int set = 8, TString selection = "zmutau",
                          vector<int> years = {2016, 2017, 2018},
                          int systematic = 5,
                          int nfits = 1000,
                          bool print = true,
                          int seed = 90) {
  int status(0);

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
  auto gen_PDF = fit_PDF;
  if(!gen_PDF) {
    cout << "Generating PDF not found!\n";
    return 1;
  }


  //variables of interest
  auto mva = ws->var("mva");
  auto br_sig = ws->var("br_sig");
  RooCategory* categories = (RooCategory*) (ws->obj(selection.Data()));

  //turn off systematic constraints for evaluating these uncertainties
  // if(ws->pdf("br_sig_eff")) {
  //   auto br_sig_constr = ws->pdf("br_sig_eff");
  //   br_sig_eff->setConstant(1);
  //   br_sig_eff->setVal(1.);
  // }

  //Set an example branching ratio
  double true_br_sig = (selection.Contains("h") ? 5.e-3 : 5.e-6);
  br_sig->setVal(true_br_sig); //set the branching fraction to the true value

  RooMCStudy mcstudy(*gen_PDF,RooArgSet(*mva, *categories),RooFit::FitModel(*fit_PDF),RooFit::Silence(), RooFit::Extended(1),
                     RooFit::FitOptions(RooFit::Save(1), RooFit::PrintEvalErrors(0)), RooFit::Binned(1));

  //perform the study
  mcstudy.generateAndFit(nfits);

  br_sig->Print();

  auto canvas = new TCanvas("c_toy", "c_toy", 1200, 1000);
  canvas->Divide(2,2);

  //Plot the distributions of the fitted parameter, the error and the pull
  auto brval_frame  = mcstudy.plotParam(*br_sig, RooFit::Bins(40), RooFit::FitGauss(1));
  canvas->cd(1);
  brval_frame->Draw();
  if(print) {
    auto brerr_frame  = mcstudy.plotError(*br_sig, RooFit::Bins(40), RooFit::FitGauss(1));
    auto brpull_frame = mcstudy.plotPull(*br_sig, RooFit::Bins(40), RooFit::FitGauss(1));
    canvas->cd(2);
    brerr_frame->Draw();
    canvas->cd(3);
    brpull_frame->Draw();
    canvas->cd(4);
    //Plot distribution of minimized likelihood
    auto NLLframe = mcstudy.plotNLL(RooFit::Bins(40));
    NLLframe->Draw();
  }

  if(print)
    canvas->SaveAs(Form("plots/latest_production/%s/toyMC_mva_roomcstudy_%s_%i.png", year_string.Data(), selection.Data(),set));
  return status;
}
