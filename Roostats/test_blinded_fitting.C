#include "construct_multidim.C"
using namespace CLFV;

//Relevant testing parameters
namespace CLFV {
  bool use_mc_ = false;
  bool use_range_ = false; //whether or not to do sideband fits
  int use_high_side_ = 0;
  bool do_no_fit_ = false;
  int pdf_version_ = 1; //Bernstein poly version, RooBernstein, RooGenericPdf, or RooBernsteinFast
  const bool unblind_data_ = false;
}

//Main testing function
void test_blinded_fitting(int set = 8, TString selection = "zemu",
                          vector<int> years = {2016, 2017, 2018},
                          int seed = 90) {

  ///////////////////////////////////////
  // Initialize useful variables
  ///////////////////////////////////////

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  bool doHiggs = selection.Contains("h");
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  double xlow  = (doHiggs) ? 100. :  70.;
  double xhigh = (doHiggs) ? 150. : 110.;
  double bmass = (doHiggs) ? 125. :  91.; //just rough mass for blinding region definition
  RooRealVar lepm("lepm", "di-lepton invariant mass", (doHiggs) ? 125. : 91., xlow, xhigh, "GeV/c^{2}");

  //initialize ranges
  lepm.setRange("full", xlow, xhigh);
  lepm.setRange("LowSideband", xlow, bmass - 5.);
  lepm.setRange("HighSideband", bmass + 5., xhigh);
  lepm.setRange("blind", bmass - 5., bmass + 5.);

  ///////////////////////////////////////
  // Read in the data
  ///////////////////////////////////////

  TFile* f = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "READ");
  if(!f) return;
  TH1D* hbkg  = (TH1D*) f->Get("hbackground");
  TH1D* hdata = (TH1D*) f->Get("hdata");
  if(!hbkg || !hdata) {
    cout << "Histograms not found!\n";
    return;
  }
  TH1D* hblindData = (TH1D*) hdata->Clone("hblindData");
  int bin_lo = hdata->FindBin(bmass - 5.);
  int bin_hi = hdata->FindBin(bmass + 5.);
  for(int bin = bin_lo; bin <= bin_hi; ++bin) hblindData->SetBinContent(bin, 0.);
  lepm.setBins((lepm.getMax() - lepm.getMin())/hbkg->GetBinWidth(1)); //match binning of histograms

  ///////////////////////////////////////
  // Create RooDataHists
  ///////////////////////////////////////

  RooDataHist bkgData  (Form("bkgData_%i"  , set), "Background Data"  , RooArgList(lepm), hbkg);
  RooDataHist dataData (Form("dataData_%i" , set),  "Data Data"       , RooArgList(lepm), hdata);
  RooDataHist blindData(Form("blindData_%i", set),  "Blind Data"      , RooArgList(lepm), hblindData);

  ///////////////////////////////////////
  // Initialize PDFs
  ///////////////////////////////////////

  //Bernstein polynomial, with different implementations
  RooAbsPdf* bkgPDF;
  if(pdf_version_ == 0) //RooBernstein
    bkgPDF = create_bernstein(lepm, 3, 0);
  else if(pdf_version_ == 1) //RooGenericPdf
    bkgPDF = create_generic_bernstein(lepm, 3, 0);
  else if(pdf_version_ == 2) //RooBernsteinFast
    bkgPDF = create_fast_bernstein(lepm, 3, 0);
  else {
    cout << "Unknown pdf option " << pdf_version_ << endl;
    return;
  }

  //Create an example Exponential PDF for comparison
  RooAbsPdf* exp_pdf = create_exponential(lepm, 1, 0);

  ///////////////////////////////////////
  // Perform the fits
  ///////////////////////////////////////

  //Define the fit range
  const char* range = (use_high_side_ > 0) ? "HighSideband" : (use_high_side_ < 0) ? "LowSideband" : "LowSideband,HighSideband";

  if(!do_no_fit_) {
    if(use_range_) {
      bkgPDF->fitTo((use_mc_) ? bkgData : dataData, RooFit::Range(range), RooFit::Extended(true), RooFit::Minimizer("Minuit2", "migrad"));
      exp_pdf->fitTo((use_mc_) ? bkgData : dataData, RooFit::Range(range), RooFit::Extended(true), RooFit::Minimizer("Minuit2", "migrad"));
    } else {
      bkgPDF->fitTo((use_mc_) ? bkgData : dataData, RooFit::Extended(true)), RooFit::Minimizer("Minuit2", "migrad");
      exp_pdf->fitTo((use_mc_) ? bkgData : dataData, RooFit::Extended(true), RooFit::Minimizer("Minuit2", "migrad"));
    }
  }

  ///////////////////////////////////////
  // Plot the results
  ///////////////////////////////////////

  TCanvas* c = new TCanvas();
  TLegend* leg = new TLegend();
  auto xframe = lepm.frame();

  if(use_mc_)  bkgData.plotOn(xframe);
  else        {
    if(!unblind_data_)
      dataData.plotOn(xframe, RooFit::Invisible());
    else
      dataData.plotOn(xframe);
  }

  double chi_sq[4];
  int dof = bkgData.numEntries();

  //Add Bernstein polynomial
  bkgPDF->plotOn(xframe, RooFit::Name("bernstein"), RooFit::Range("LowSideband,HighSideband"), RooFit::NormRange(range));
  chi_sq[2] = xframe->chiSquare() * dof / (dof - 4);
  bkgPDF->plotOn(xframe, RooFit::Range("blind"), RooFit::NormRange(range), RooFit::LineColor(kViolet));

  //Add exponential
  exp_pdf->plotOn(xframe, RooFit::Name("exponential"), RooFit::LineColor(kRed), RooFit::Range("LowSideband,HighSideband"),
                  RooFit::NormRange(range), RooFit::LineStyle(kDashed));
  chi_sq[3] = xframe->chiSquare() * dof / (dof - 4);
  exp_pdf->plotOn(xframe, RooFit::LineColor(kOrange), RooFit::Range("blind"), RooFit::NormRange(range), RooFit::LineStyle(kDashed));

  //Add visible data if blinding
  if(!use_mc_ && !unblind_data_) blindData.plotOn(xframe);

  //Evaluate goodness of fit
  if(use_range_) {
    dof -= 10./hdata->GetBinWidth(1); //number of blinded bins
  }
  if(use_range_) {
    chi_sq[0]  = get_subrange_chisquare(lepm, bkgPDF , (use_mc_) ? bkgData : dataData, "LowSideband");
    chi_sq[0] += get_subrange_chisquare(lepm, bkgPDF , (use_mc_) ? bkgData : dataData, "HighSideband");
    chi_sq[1]  = get_subrange_chisquare(lepm, exp_pdf, (use_mc_) ? bkgData : dataData, "LowSideband");
    chi_sq[1] += get_subrange_chisquare(lepm, exp_pdf, (use_mc_) ? bkgData : dataData, "HighSideband");
  } else {
    chi_sq[0]  = get_subrange_chisquare(lepm, bkgPDF , (use_mc_) ? bkgData : dataData, "full");
    chi_sq[1]  = get_subrange_chisquare(lepm, exp_pdf, (use_mc_) ? bkgData : dataData, "full");
  }

  cout << "Bkg PDF chi-squared: " << chi_sq[0] << " / " << dof - 4 << " = " << chi_sq[0] / (dof-4) << " (vs " << chi_sq[2] << ")\n";
  cout << "Exp PDF chi-squared: " << chi_sq[1] << " / " << dof - 4 << " = " << chi_sq[1] / (dof-4) << " (vs " << chi_sq[3] << ")\n";

  //Draw the RooPlot and print info/canvas
  xframe->Draw();
  leg->AddEntry("bernstein", Form("Bernstein, #chi^2/DOF = %.2f", chi_sq[0] / (dof-4)), "L");
  leg->AddEntry("exponential", Form("Exponential, #chi^2/DOF = %.2f", chi_sq[1] / (dof-4)), "L");
  leg->Draw();
  bkgPDF->Print("tree");
  exp_pdf->Print("tree");
  c->SaveAs(Form("test_blinded_fitting_v%i_r%i_d%i.png", pdf_version_, use_range_, !use_mc_));
}
