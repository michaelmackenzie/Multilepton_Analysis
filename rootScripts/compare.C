//Compare results of local an eos histogram

int compare(const char* file, const char* hist_dir, int set) {

  TFile* f1 = TFile::Open(file, "READ");
  TFile* f2 = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/histograms/%s/%s", hist_dir, file), "READ");
  if(!f1 || !f2) return 1;
  TH1* h1 = (TH1*) f1->Get(Form("Data/event_%i/lepm", set));
  TH1* h2 = (TH1*) f2->Get(Form("Data/event_%i/lepm", set));
  if(!h1 || !h2) {
    cout << "Histograms not found!\n";
    return 2;
  }
  printf("Entries  : local = %.3f; eos = %.3f\n", h1->GetEntries(), h2->GetEntries());
  printf("Integrals: local = %.3f; eos = %.3f\n", h1->Integral(0,h1->GetNbinsX()+1), h2->Integral(0,h2->GetNbinsX()+1));
  return 0;
}
