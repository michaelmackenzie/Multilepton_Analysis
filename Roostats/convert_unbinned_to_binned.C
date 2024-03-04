//Read in toys with an unbinned dataset, output toys with a binned dataset
int verbose_ = 0;

int convert_unbinned_to_binned(const char* name_in, const char* name_out) {

  //Input data file
  TFile* fIn = TFile::Open(name_in, "READ");
  if(!fIn) return 1;

  //Toy directory to clone
  auto in_dir  = (TDirectoryFile*) fIn->Get("toys");
  if(!in_dir) {
    cout << "Input toy directory not found!\n";
    return 2;
  }

  //Ouput data file and directory
  TFile* fOut = new TFile(name_out, "RECREATE");
  auto out_dir = fOut->mkdir("toys");

  if(verbose_ > 0) {
    fIn->ls();
    in_dir->ls();
  }
  //Add each toy dataset found
  for(int itoy = 1; ;++itoy) {
    RooDataSet* in_toy = (RooDataSet*) in_dir->Get(Form("toy_%i", itoy));
    if(!in_toy) break;
    if(verbose_) {
      cout << "Adding toy " << itoy << endl;
    }
    auto out_toy = in_toy->binnedClone(Form("toy_%i", itoy), Form("toy_%i", itoy));
    out_dir->cd();
    out_toy->Write();
    delete in_toy;
    delete out_toy;
  }

  if(verbose_) cout << "Finished creating binned toy datasets\n";

  //Close the output
  fOut->Close();

  //Close the input
  fIn->Close();

  return 0;
}
