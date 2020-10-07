//Script to morph ee and mumu data into e+mu

Int_t morph_signal(int set = 8, int year = 2016, TString base = "../histograms/nanoaods_dev/") {

  // gROOT->SetBatch(kTRUE);

  TString muon_name = "workspaces/fit_lepm_SameSign_Muon_";
  muon_name += year;
  muon_name += "_";
  muon_name += set;
  muon_name += ".root";
  TFile* f_muon = TFile::Open(muon_name.Data(), "READ");
  if(!f_muon) return 1;

  RooWorkspace* ws_muon = (RooWorkspace*) f_muon->Get("ws");
  if(!ws_muon) return -1;
  
  TString electron_name = "workspaces/fit_lepm_SameSign_Electron_";
  electron_name += year;
  electron_name += "_";
  electron_name += set;
  electron_name += ".root";
  TFile* f_electron = TFile::Open(electron_name.Data(), "READ");
  if(!f_electron) return 2;

  RooWorkspace* ws_electron = (RooWorkspace*) f_electron->Get("ws");
  if(!ws_electron) return -2;

  TString zemu_name = base + "ztautau_emu_clfv_";
  zemu_name += year;
  zemu_name += "_ZEMu.hist";

  TFile* f_zemu = TFile::Open(zemu_name.Data(), "READ");
  if(!f_zemu) return 3;

  TTree* t_zemu = (TTree*) f_zemu->Get(Form("Data/tree_%i/tree_%i",
					    set+ZTauTauHistMaker::kEMu,
					    set+ZTauTauHistMaker::kEMu));
  if(!t_zemu) return 4;
  std::cout << "All file elements retrieved!\n";
  RooRealVar lepm("lepm","di-lepton invariant mass", 75., 110.);
  RooDataSet dataset("dataset", "dataset", RooArgSet(lepm), RooFit::Import(*t_zemu));

  std::cout << "Dataset loaded!\n";
  
  auto pdf_muon = ws_muon->pdf("sigpdf");
  auto pdf_electron = ws_electron->pdf("sigpdf");

  auto binned_muon = pdf_muon->generateBinned(RooArgSet(lepm), 1e8);
  auto binned_electron = pdf_electron->generateBinned(RooArgSet(lepm), 1e8);

  RooHistPdf pdf_muon_binned("pdf_muon_binned", "pdf_muon_binned", RooArgSet(lepm), *binned_muon);
  RooHistPdf pdf_electron_binned("pdf_electron_binned", "pdf_electron_binned", RooArgSet(lepm), *binned_electron);

  RooRealVar alpha("alpha", "alpha", 0.5);
  RooIntegralMorph morph_pdf("morph_pdf", "morph_pdf", pdf_muon_binned, pdf_electron_binned, lepm, alpha);

  auto binned_morphed = morph_pdf.generateBinned(RooArgSet(lepm), 1e8);
  RooHistPdf morph_pdf_binned("morph_pdf_binned", "morphed_pdf_binned", RooArgSet(lepm), *binned_morphed, 4);

  auto xframe = lepm.frame(80);
  dataset.plotOn(xframe);
  morph_pdf_binned.plotOn(xframe);
  pdf_muon_binned.plotOn(xframe, RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  pdf_electron_binned.plotOn(xframe, RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));

  auto c1 = new TCanvas();
  xframe->Draw();
  c1->SaveAs(Form("plots/latest_production/%i/compare_morphed_pdf_%i.pdf", year, set));
  TFile* fOut = new TFile(Form("workspaces/morphed_signal_%i_%i.root", year, set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(morph_pdf_binned);
  ws.Write();
  fOut->Close();

  return 0;
}
