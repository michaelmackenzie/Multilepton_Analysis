//Test fitting DY/WW shapes for the Z->e+mu search
#include "../construct_multidim.C"

////////////////////////////////////////////////
// Plot the raw PDF distribution
TH1* get_pdf(RooAbsPdf* pdf, RooRealVar& obs) {
  TH1* fit = pdf->createHistogram("fit", obs);
  for(int ibin = 1; ibin <= fit->GetNbinsX(); ++ibin) fit->SetBinError(ibin, 0.);
  fit->SetLineColor(kRed);
  fit->SetFillColor(0);
  fit->SetTitle("PDF");
  fit->SetYTitle("PDF");
  return fit;
}

////////////////////////////////////////////////
// Get Data / Fit histogram
TH1* get_data_fit_diff(RooDataHist& data, RooAbsPdf* pdf, RooRealVar& obs) {
  TH1* fit = get_pdf(pdf, obs);
  TH1* diff = data.createHistogram("diff", obs);
  fit->Scale(diff->Integral() / fit->Integral());
  diff->Divide(fit);
  diff->SetMarkerStyle(20);
  diff->SetMarkerSize(0.8);
  diff->SetLineColor(kBlue);
  diff->SetMarkerColor(kBlack);
  diff->SetFillColor(0);
  diff->SetTitle("");
  diff->SetYTitle("Data / Fit");
  delete fit;
  return diff;
}

int test_dy_ww_shape(int set = 13) {
  TFile* f = TFile::Open(Form("histograms/zemu_lepm_%i_2016_2017_2018.hist", set));
  if(!f) return 1;

  THStack* hstack = (THStack*) f->Get("hstack");

  if(!hstack) {
    cout << "Stack not found!\n" << endl;
    return 2;
  }

  TH1* hdy = (TH1*) hstack->GetHists()->FindObject(Form("Z->#tau#tau_lepm_2%i", set));
  TH1* hww = (TH1*) hstack->GetHists()->FindObject(Form("Other VB_lepm_2%i", set));
  if(!hdy) {
    cout << "Drell-Yan not found!\n" << endl;
    hstack->GetHists()->Print();
    return 2;
  }
  if(!hww) {
    cout << "WW not found!\n" << endl;
    hstack->GetHists()->Print();
    return 2;
  }
  hdy->SetDirectory(0);
  hww->SetDirectory(0);
  f->Close();

  for(int ibin = 1; ibin <= hww->GetNbinsX(); ++ibin) {
    if(hww->GetBinContent(ibin) < 0.) hww->SetBinContent(ibin, 0.);
    if(hdy->GetBinContent(ibin) < 0.) hdy->SetBinContent(ibin, 0.);
  }

  RooRealVar obs("obs", "obs", 90., 70., 110.);
  const int bin_lo = hww->FindBin(70.01);
  const int bin_hi = hww->FindBin(109.99);
  obs.setRange("full", 70., 110.);
  obs.setRange("LowSideband", 70., 86.);
  obs.setRange("HighSideband", 96., 110.);
  obs.setRange("BlindRegion", 86., 96.);
  obs.setBins(bin_hi - bin_lo + 1);
  obs.Print();

  const int poly_order = 3;
  const int exp_order  = 0;
  RooAbsPdf* polyPdf = create_fast_bernstein(obs, poly_order, set, "_comb");
  // RooAbsPdf* polyPdf = create_bernstein(obs, poly_order, set, "_comb");
  RooAbsPdf* expPdf  = create_exponential   (obs, exp_order , set, "_comb");
  polyPdf->Print();
  expPdf->Print();

  auto polyVar = RooArgList(*(polyPdf->getVariables()));
  polyVar.Print();
  ((RooRealVar*) polyVar.at(0))->setVal(1.66889);
  ((RooRealVar*) polyVar.at(1))->setVal(1.57434);

  auto expVar = RooArgList(*(expPdf->getVariables()));
  expVar.Print();
  ((RooRealVar*) expVar.at(0))->setVal(-0.135877);

  RooDataHist* wwData = new RooDataHist("ww_data", "WW Data", RooArgList(obs), hww);
  RooDataHist* dyData = new RooDataHist("dy_data", "DY Data", RooArgList(obs), hdy);

  polyPdf->fitTo(*wwData, RooFit::SumW2Error(1));
  expPdf ->fitTo(*dyData, RooFit::SumW2Error(1));
  polyPdf->Print("tree");
  expPdf ->Print("tree");

  const int ndof = bin_hi - bin_lo + 1;

  // useFrameChiSq_  = true;
  // useManualChisq_ = false;
  blind_data_ = false;
  const double poly_chi_sq =  get_manual_subrange_chisquare(obs, polyPdf, *wwData, "full", "full", false);
  printf("Poly PDF: %.2f / %i DOF = %.4f\n", poly_chi_sq, ndof - poly_order  - 2, poly_chi_sq / (ndof - poly_order  - 2));
  blind_data_ = false;
  // const double exp_chi_sq  =  get_manual_subrange_chisquare(obs, expPdf , *dyData, "full", "full", false);
  const double exp_chi_sq  =  get_subrange_chisquare(obs, expPdf , *dyData, "full");
  printf("Exp  PDF: %.2f / %i DOF = %.4f\n", exp_chi_sq , ndof - 2*exp_order - 2, exp_chi_sq  / (ndof - 2*exp_order - 2));

  TCanvas c("c","c",1800,1000);
  TPad* pad1 = new TPad("pad1", "pad1", 0.00, 0.3, 0.33, 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0.00, 0.0, 0.33, 0.3); pad2->Draw();
  TPad* pad3 = new TPad("pad3", "pad3", 0.33, 0.3, 0.66, 1.0); pad3->Draw();
  TPad* pad4 = new TPad("pad4", "pad4", 0.33, 0.0, 0.66, 0.3); pad4->Draw();
  TPad* pad5 = new TPad("pad5", "pad5", 0.66, 0.3, 1.00, 1.0); pad5->Draw();
  TPad* pad6 = new TPad("pad6", "pad6", 0.66, 0.0, 1.00, 0.3); pad6->Draw();
  gStyle->SetOptStat(0);

  pad1->cd();
  auto xframe = obs.frame(RooFit::Title("Drell-Yan MC fit"));
  dyData->plotOn(xframe);
  expPdf->plotOn(xframe);
  xframe->Draw();

  //Data / Fit comparison
  pad2->cd();
  TH1* hdy_diff = get_data_fit_diff(*dyData, expPdf, obs);
  hdy_diff->SetName("dy_diff");
  hdy_diff->Draw("E1");
  hdy_diff->GetXaxis()->SetRangeUser(70.,110.);
  hdy_diff->GetYaxis()->SetRangeUser(0.5,1.5);
  TLine* line = new TLine(70., 1., 110., 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");

  pad3->cd();
  xframe = obs.frame(RooFit::Title("Di-/tri-boson MC fit"));
  wwData->plotOn(xframe);
  polyPdf->plotOn(xframe);
  xframe->Draw();

  //Data / Fit comparison
  pad4->cd();
  TH1* hww_diff = get_data_fit_diff(*wwData, polyPdf, obs);
  hww_diff->SetName("ww_diff");
  hww_diff->Draw("E1");
  hww_diff->GetXaxis()->SetRangeUser(70.,110.);
  hww_diff->GetYaxis()->SetRangeUser(0.5,1.5);
  line->Draw("same");

  //add the PDFs together
  //////////////////////////////////////////////
  // Fit the combination data with both backgrounds

  //add the PDFs together
  RooRealVar* comb_coeff = new RooRealVar("comb_coeff", "comb_coeff", 0.15, 0., 1.);
  RooAddPdf* basePdf     = new RooAddPdf ("comb_pdf" , "comb_pdf",
                                          RooArgList(*polyPdf, *expPdf), RooArgList(*comb_coeff), false);

  RooDataHist* genWW   = polyPdf->generateBinned(obs, hww->Integral(bin_lo, bin_hi));
  RooDataHist* genData = expPdf ->generateBinned(obs, hdy->Integral(bin_lo, bin_hi));
  genData->add(*genWW);
  TH1* data = genData->createHistogram("data", obs);
  basePdf->fitTo(*genData, RooFit::SumW2Error(1));

  pad5->cd();
  xframe = obs.frame(RooFit::Title("Mock data fit"));
  genData->plotOn(xframe);
  basePdf->plotOn(xframe);
  basePdf->plotOn(xframe, RooFit::Components(polyPdf->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  basePdf->plotOn(xframe, RooFit::Components(expPdf->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  xframe->Draw();

  //Data / Fit comparison
  pad6->cd();
  TH1* hdata_diff = get_data_fit_diff(*genData, basePdf, obs);
  hdata_diff->SetName("data_diff");
  hdata_diff->Draw("E1");
  hdata_diff->GetXaxis()->SetRangeUser(70.,110.);
  hdata_diff->GetYaxis()->SetRangeUser(0.5,1.5);
  line->Draw("same");

  c.SaveAs("test_dy_ww_shape.png");

  cout << "N(DY) = " << hdy->Integral(bin_lo, bin_hi) << endl;
  cout << "N(WW) = " << hww->Integral(bin_lo, bin_hi) << endl;
  return 0;
}
