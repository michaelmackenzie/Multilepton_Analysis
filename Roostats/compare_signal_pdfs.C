//Script to compare signal PDF methods

Int_t compare_signal_pdfs(int set = 8, vector<int> years = {2016, 2017, 2018}, TString base = "../histograms/nanoaods_dev/") {

  TString hist = "lepm";
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  cout << "Opening files for signal PDFs!\n";
  TFile* fSigFit = TFile::Open(Form("workspaces/fit_signal_%s_%s_%i.root", hist.Data(), year_string.Data(), set), "READ");
  TFile* fMorph  = TFile::Open(Form("workspaces/morphed_signal_%s_%i.root", year_string.Data(), set), "READ");
  if(!fSigFit || !fMorph) return 1;
  cout << "Getting workspaces!\n";
  auto wsSigFit = (RooWorkspace*) fSigFit->Get("ws");
  auto wsMorph  = (RooWorkspace*) fMorph ->Get("ws");
  cout << "Getting PDFs!\n";
  auto pdfSigFit = wsSigFit->pdf("sigpdf");
  auto pdfMorph  = wsMorph->pdf("morph_pdf_binned");
  if(!pdfSigFit) {cout << "Fit PDF not found!\n"; wsSigFit->Print(); return 2;}
  if(!pdfMorph) {cout << "Morphed PDF not found!\n"; wsMorph->Print(); return 3;}
  cout << "Getting var!\n";
  auto lepm      = wsSigFit->var(hist.Data());
  if(!pdfMorph) {cout << "Var " << hist.Data() << " not found!\n"; return 4;}
  auto xframe = lepm->frame();
  cout << "Drawing PDFs!\n";
  pdfSigFit->plotOn(xframe);
  pdfMorph->plotOn(xframe, RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));

  auto c1 = new TCanvas();
  xframe->Draw();
  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(xframe->findObject("morph_pdf_binned_Norm[lepm]"), "Morphed PDF", "L");
  leg->AddEntry(xframe->findObject("sigpdf_Norm[lepm]"), "Fit PDF", "L");
  leg->Draw("same");
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  c1->SaveAs(Form("plots/latest_production/%s/compare_signal_pdfs_%s_%i.pdf", year_string.Data(), hist.Data(), set));
  c1->SaveAs(Form("plots/latest_production/%s/compare_signal_pdfs_%s_%i.png", year_string.Data(), hist.Data(), set));
  
  return 0;
}
