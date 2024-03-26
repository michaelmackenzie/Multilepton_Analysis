//Script to perform toy MC fits using either the same PDF to generate and fit
// or a systematically shifted generation PDF and the nominal to fit

bool use_bernstein_ = true;

int toyMC_bemu_roomcstudy(vector<int> sets = {8}, TString selection = "zemu",
                          vector<int> years = {2016, 2017, 2018},
                          int nfits = 1000,
                          bool self_test = false,
                          bool print = true,
                          int seed = 90) {
  int status(0);
  bool doHiggs = selection.Contains("h");

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
  auto genPDF = ws->pdf(gen_pdf_name.Data());
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
  if(ws->pdf("br_sig_beta")) {
    auto br_sig_beta = ws->var("br_sig_beta");
    br_sig_beta->setConstant(1);
    br_sig_beta->setVal(0.);
    cout << "Setting branching fraction systematic factor to constant!\n";
  }

  //Set an example branching ratio
  double true_br_sig = (selection.Contains("h") ? 1.e-3 : 1.e-6);
  br_sig->setVal(true_br_sig); //set the branching fraction to the true value

  RooMCStudy mcstudy(*genPDF,RooArgSet(*lepm, *categories),RooFit::FitModel(*fitPDF),RooFit::Silence(), RooFit::Extended(1),
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
    canvas->SaveAs(Form("plots/latest_production/%s/toyMC_bemu_roomcstudy_%s_%s.png", year_string.Data(), selection.Data(), set_str.Data()));
  return status;
}
