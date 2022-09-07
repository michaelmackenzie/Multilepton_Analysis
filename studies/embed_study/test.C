int test() {
  TFile* f = TFile::Open("root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/LFVAnalysis_DY50-amc_2018.root", "READ");
  if(!f) return 1;

  TTree* Events = (TTree*) f->Get("emu");
  if(!f) {
    cout << "Events tree not found!\n";
    return 2;
  }

  TH1* hZEta = new TH1D("hzeta", "#eta(Z)", 50, -10., 10.);
  Events->Draw("GenZll_eta >> hzeta", "");

  TCanvas* c = new TCanvas();
  hZEta->Draw("hist");
  c->SetLogy();
  c->Print("test_zeta.png");
  delete c;
  return 0;

}
