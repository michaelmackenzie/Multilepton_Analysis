void split_trees(const char* fin, const char* fout) {
  TFile* fIn = TFile::Open(fin, "READ");
  if(!fIn) return;
  TTree* tree = (TTree*) fIn->Get("background_tree");
  if(!tree) {cout << "Tree not found!\n"; return;}
  TFile* fOut = new TFile(fout, "RECREATE");
  TTree* train = tree->CopyTree("train>0."); train->SetName("train");
  TTree* test  = tree->CopyTree("train>0."); test->SetName("test");
  fOut->cd();
  train->Write();
  test->Write();
  fOut->Close();
  fIn->Close();
}
