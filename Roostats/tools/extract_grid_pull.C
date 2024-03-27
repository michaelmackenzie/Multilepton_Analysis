//extract a pull value from a grid of scanned NLL values

double extract_grid_pull(const char* file, const double r_true) {

  //create a list of scan files for the set
  if(gSystem->AccessPathName(file)) {
    cout << __func__ <<": File " << file << " not found\n";
    return 0.;
  }

  //Get the file with the NLL scan
  TFile* f = TFile::Open(file, "READ");
  if(!f) return 0.;

  //Get the tree with the scan results
  TTree* tree = (TTree*) f->Get("limit");
  if(!tree) {
    cout << __func__ << ": Tree not found in file " << file << endl;
    return 0.;
  }

  //Setup the scan result addresses
  const int nentries = tree->GetEntries();
  double rvals[nentries-1], nlls[nentries-1];
  double nll0, nll;
  float r, dnll;
  tree->SetBranchAddress("r", &r);
  tree->SetBranchAddress("deltaNLL", &dnll);
  tree->SetBranchAddress("nll0", &nll0);
  tree->SetBranchAddress("nll" , &nll);

  //Scan the results to get the NLL for the best fit r and gen-true r values
  double r_fit(0.), nll_fit(0.); //fit results
  double r_closest(1.e20), nll_closest(0.); //closest scan point to r_true
  for(int entry = 0; entry < nentries-1; ++entry) {
    tree->GetEntry(entry);
    const double nll_val = nll0 + nll + dnll;
    if(entry == 0) { //results for the best fit
      r_fit = r;
      nll_fit = nll_val;
      continue;
    }
    //find point closest to r_gen
    if(std::fabs(r - r_true) < std::fabs(r_closest - r_true)) {
      r_closest = r;
      nll_closest = nll_val;
    }
  }

  //approximate the fit pull using the difference in the negative log-likelihood
  const double nll_difference = std::fabs(nll_closest - nll_fit);
  const double sigma = std::sqrt(0.5*nll_difference);
  return sigma;
}
