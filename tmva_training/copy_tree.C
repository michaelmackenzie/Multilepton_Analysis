
int copy_tree(const char* file = "trees/background_ztautau_Z0_nano_mutau_2016_2017_2018_8.tree",
               int nentries = 100) {
  TFile* f = TFile::Open(file, "READ");
  if(!f) return 1;
  TTree* t = (TTree*) f->Get("background_tree");
  if(!t) return 2;
  t->SetName("old_background_tree");
  t->SetBranchStatus("*", 1);

  cout << "Cloning " << nentries << " from " << file << " tree\n";
  TFile* fout = new TFile("trees/test_tree.root", "RECREATE");
  auto tout = t->CloneTree(nentries);
  tout->SetName("background_tree");
  tout->Write();
  fout->Clone();
  f->Close();
  return 0;
}
