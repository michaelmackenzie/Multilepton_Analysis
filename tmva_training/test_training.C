using namespace CLFV;

#include "TrainTrkQual.C"
#include "copy_tree.C"
int test_training(const char* file = "trees/background_clfv_Z0_nano_mutau_2016_2017_2018_8.tree",
                  int nentries = 1e3, int version = -1) {
  int status = copy_tree(file, nentries);
  if(status) return status;
  TFile* f = TFile::Open("trees/test_tree.root", "READ");
  if(!f) return 10;
  TTree* t = (TTree*) f->Get("background_tree");
  if(!t) return 20;
  TTree* s = (TTree*) t->Clone("signal");
  if(version > -1) trkqual_version_ = version;
  printf("Beginning test training\n");
  return TrainTrkQual(s, t, "test_tmva_training", {});

}
