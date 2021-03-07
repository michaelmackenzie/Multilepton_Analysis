//Script to perform toy MC fits using same PDF to generate and fit

bool doConstraints_ = false; //whether or not to use constraint based PDFs

int toyMC_fits(int set, vector<int> years, int nfits = 1000, bool print = true) {
  int status(0);
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    if(i > 0) year_string += "_";
    year_string += year;
  }
  TFile* fInput = TFile::Open(Form("workspaces/fit_lepm_background_%s_%i.root", year_string.Data(), set), "READ");
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
  auto n_sig = wsIn->function("n_sig");
  auto eff = wsIn->function("eff");


  //create the study
  RooMCStudy mcstudy(*gener_PDF,RooArgSet(*lepm),RooFit::FitModel(*fit_PDF),RooFit::Silence(), RooFit::Extended(1),
                     RooFit::FitOptions(RooFit::Save(1), RooFit::PrintEvalErrors(0)));

  //perform the study
  mcstudy.generateAndFit(nfits);

  if(!n_sig) cout << "n_sig variable not found!\n";
  else     {
    n_sig->Print();
    br_emu->Print();
  }

  if(!eff) cout << "eff variable not found!\n";
  else eff->Print();
  if(wsIn->var("br_ll")) wsIn->var("br_ll")->Print();
  if(wsIn->function("n_muon_var")) wsIn->function("n_muon_var")->Print();
  if(wsIn->function("n_electron_var")) wsIn->function("n_electron_var")->Print();

  auto canvas = new TCanvas("c_toy", "c_toy", 1200, 1000);
  canvas->Divide(2,2);

  //Plot the distributions of the fitted parameter, the error and the pull
  auto brval_frame  = mcstudy.plotParam(*br_emu, RooFit::Bins(40));
  if(print) {
    auto brerr_frame  = mcstudy.plotError(*br_emu, RooFit::Bins(40));
    auto brpull_frame = mcstudy.plotPull(*br_emu, RooFit::Bins(40), RooFit::FitGauss(1));
    canvas->cd(2);
    brerr_frame->Draw();
    canvas->cd(3);
    brpull_frame->Draw();
    canvas->cd(4);
    //Plot distribution of minimized likelihood
    auto NLLframe = mcstudy.plotNLL(RooFit::Bins(40));
    NLLframe->Draw();
  }

  canvas->cd(1);
  brval_frame->Draw();
  if(print)
    canvas->SaveAs(Form("plots/latest_production/%s/plot_toyMC_fits_%i.pdf", year_string.Data(), set));
  return status;
}
