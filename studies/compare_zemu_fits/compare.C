//Compare background functions from different workspaces

/**
   cat: category index: 11, 12, or 13
 **/
int compare(int cat = 13) {
  TFile* f1 = TFile::Open(Form("rootfiles/workspace_v10_bkg_bin%i.root", cat - 10));
  TFile* f2 = TFile::Open(Form("rootfiles/combine_bemu_zemu_%i_workspace.root", cat));
  if(!f1 || !f2) return 1;

  RooWorkspace* ws1 = (RooWorkspace*) f1->Get("workspace_background");
  RooWorkspace* ws2 = (RooWorkspace*) f2->Get("w");

  if(!ws1 || !ws2) {
    cout << "RooWorkspaces not found!\n";
    return 2;
  }

  RooAbsPdf* pdf1 = ws1->pdf(Form("multipdf_bin%i"      , cat-10));
  RooAbsPdf* pdf2 = ws2->pdf(Form("shapeBkg_bkg_lepm_%i", cat   ));

  if(!pdf1 || !pdf2) {
    cout << "RooAbsPdfs not found!\n";
    return 3;
  }

  RooDataHist* data1 = (RooDataHist*) ws1->data(Form("sim_binned_obs_bin%i", cat-10));
  RooDataHist* data2 = (RooDataHist*) ws2->data("data_obs");

  if(!data1 || !data2) {
    cout << "Data not found!\n";
    return 4;
  }

  RooRealVar* obs1 = (RooRealVar*) ws1->var(Form("lepm_%i", cat));
  RooRealVar* obs2 = (RooRealVar*) ws2->var(Form("lepm_%i", cat));

  if(!obs1 || !obs2) {
    cout << "Observable not found!\n";
    return 5;
  }

  TCanvas* c = new TCanvas();

  //plot each dataset with its corresponding PDF
  auto frame = obs1->frame();
  data1->plotOn(frame);
  pdf1->plotOn(frame);
  frame->Draw();
  c->SaveAs(Form("figures/pdf1_%i.png", cat));

  frame = obs2->frame();
  data2->plotOn(frame);
  pdf2->plotOn(frame);
  frame->Draw();
  c->SaveAs(Form("figures/pdf2_%i.png", cat));

  //plot both PDFs with a single dataset
  frame = obs2->frame();
  data2->plotOn(frame);
  pdf2->plotOn(frame);
  pdf1->plotOn(frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  frame->Draw();
  c->SaveAs(Form("figures/pdfs_%i.png", cat));

  f1->Close();
  f2->Close();
  return 0;
}
