//Script to fit the side band regions with a polynomial

Int_t fit_sideband_poly(int set = 8, int year = 2016) {

  TFile* fInput = TFile::Open(Form("background_trees/background_ztautau_data_nano_emu_%i.tree",
				       set+ZTauTauHistMaker::kEMu), "READ");
  if(!fInput) return 1;
  TTree* tree = (TTree*) fInput->Get("background_tree");
  if(!tree) return 2;

  RooRealVar mass("lepm", "lepm", 75., 120., "GeV/c^{2}");
  mass.setRange("LowSideband", 75., 85.);
  mass.setRange("HighSideband", 95., 120.);

  RooRealVar a_bkg("a_bkg", "a_bkg", -0.8 ,  -3.,  3.);
  RooRealVar b_bkg("b_bkg", "b_bkg",  0.3 ,  -3.,  3.);
  RooRealVar c_bkg("c_bkg", "c_bkg", -0.2 ,  -3.,  3.);
  RooRealVar d_bkg("d_bkg", "d_bkg", -0.02,  -3.,  3.);
  RooRealVar e_bkg("e_bkg", "e_bkg",  0.  , -10., 10.);
  RooRealVar f_bkg("f_bkg", "f_bkg",  0.  , -10., 10.);
  RooRealVar g_bkg("g_bkg", "g_bkg",  0.  , -10., 10.);

  RooChebychev bkgPDF("bkgPDF", "bkgPDF", mass, RooArgList(a_bkg, b_bkg, c_bkg));

  RooDataSet dataset("dataset", "dataset", RooArgSet(mass), RooFit::Import(tree));

  bkgPDF.fitTo(dataset, RooFit::Range("LowSideband", "HighSideband"));

  auto data_blinded = dataset.reduce("lepm < 84.9 || lepm > 95.7");
  auto xframe = mass.frame(50);
  data_blinded->plotOn(xframe);
  bkgPDF.plotOn(xframe, RooFit::Range("LowSideband", "HighSideband"));

  auto c1 = new TCanvas();
  xframe->Draw();
  gSystem->Exec(Form("[ ! -d plots/latest_production/%i ] && mkdir -p plots/latest_production/%i", year, year));
  c1->SaveAs(Form("plots/latest_productions/%i/fit_sideband_poly_%i.pdf", year, set));

  std::cout << "Chi^2: " << xframe->chiSquare() << std::endl;

  TFile* fOut = new TFile(Form("workspaces/fit_sideband_poly_%i_%i.root", year, set), "RECREATE");
  fOut->cd();
  RooWorkspace workspace("ws");
  workspace.import(bkgPDF);
  // workspace.Print();
  workspace.Write();
  fOut->Close();
  return 0;
}
