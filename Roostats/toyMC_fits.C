//Script to perform toy MC fits using same PDF to generate and fit

bool doConstraints_ = false; //whether or not to use constraint based PDFs

int toyMC_fits(int set, int year, int nfits = 1000) {
  int status(0);
  TFile* fInput = TFile::Open(Form("workspaces/fit_lepm_background_%i_%i.root", year, set), "READ");
  if(!fInput) return 1;
  fInput->cd();

  RooWorkspace* wsIn = (RooWorkspace*) fInput->Get("ws");

  wsIn->Print();

  //PDF to generate data
  auto gener_PDF = wsIn->pdf((doConstraints_) ? "totPDF_constr" : "totPDF");
  if(!gener_PDF) {
    cout << "Generating PDF not found!\n";
    return 1;
  }
  //fit using the standard PDF
  auto fit_PDF = wsIn->pdf((doConstraints_) ? "totPDF_constr" : "totPDF");

  //variables of interest
  auto lepm = wsIn->var("lepm");
  auto br_emu = wsIn->var("br_emu");

  //create the study
  RooMCStudy mcstudy(*gener_PDF,RooArgSet(*lepm),RooFit::FitModel(*fit_PDF),RooFit::Silence(), RooFit::Extended(1),
		     RooFit::FitOptions(RooFit::Save(1), RooFit::PrintEvalErrors(0)));

  //perform the study
  mcstudy.generateAndFit(nfits);

  //Plot the distributions of the fitted parameter, the error and the pull
  auto brval_frame  = mcstudy.plotParam(*br_emu, RooFit::Bins(40));
  auto brerr_frame  = mcstudy.plotError(*br_emu, RooFit::Bins(40));
  auto brpull_frame = mcstudy.plotPull(*br_emu, RooFit::Bins(40), RooFit::FitGauss(1));

  //Plot distribution of minimized likelihood
  auto NLLframe = mcstudy.plotNLL(RooFit::Bins(40));

  //Actually plot
  auto canvas = new TCanvas("c_toy", "c_toy", 1200, 1000);
  canvas->Divide(2,2);
  canvas->cd(1);
  brval_frame->Draw();
  canvas->cd(2);
  brerr_frame->Draw();
  canvas->cd(3);
  brpull_frame->Draw();
  canvas->cd(4);
  NLLframe->Draw();
  canvas->SaveAs(Form("plots/latest_production/%i/plot_toyMC_fits_%i.pdf", year, set));
  return status;
}
