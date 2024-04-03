//Print the results of a single MultiDimFit grid scan

void print_single_scan(const char* file, const char* index_name = nullptr, const char* figure_name = "envelope", int verbose = 0) {

  //check that the file exists
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
  float r, dnll, rErr;
  int index(0);
  tree->SetBranchAddress("r", &r);
  tree->SetBranchAddress("deltaNLL", &dnll);
  tree->SetBranchAddress("nll0", &nll0);
  tree->SetBranchAddress("nll" , &nll);
  if(index_name) tree->SetBranchAddress(index_name , &index);

  //Scan the results
  double r_fit(0.), nll_fit(0.); //fit results
  double r_min(0.), nll_min(1.e20); //scan point with the minimum r
  int index_min(0), index_fit(0);
  double r_vals[nentries-1], nll_vals[nentries-1];
  for(int entry = 0; entry < nentries; ++entry) {
    tree->GetEntry(entry);
    if(verbose > 0)
      printf("Entry %3i: r = %7.3f, deltaNLL = %7.2f, nll = %.2f, nll0 = %.2f, index = %i\n",
             entry, r, dnll, nll, nll0, index);
    const double nll_val = nll0 + nll + dnll;
    if(entry == 0) { //results for the best fit
      r_fit = r;
      nll_fit = nll_val;
      index_fit = index;
      continue;
    }
    r_vals  [entry-1] = r; //save the envelope scan
    nll_vals[entry-1] = nll_val;
    //scanned point with minimum NLL (excluding the fit point)
    if(nll_val < nll_min) {
      r_min = r;
      nll_min = nll_val;;
      index_min = index;
    }
  }

  //Plot the envelope
  TCanvas c;

  //Draw the envelope
  TGraph* env = new TGraph(nentries-1, r_vals, nll_vals);
  env->SetLineWidth(2);
  env->SetMarkerSize(0.8);
  env->SetMarkerStyle(20);
  env->SetMarkerColor(kBlue);
  env->SetTitle("NLL Scan; r; NLL");
  env->Draw("AP");

  //Draw the best fit point
  TGraph* best = new TGraph(1, &r_fit, &nll_fit);
  best->SetLineWidth(2);
  best->SetMarkerSize(2.2);
  best->SetMarkerStyle(kFullStar);
  best->SetMarkerColor(kRed);
  best->Draw("P");

  env->GetYaxis()->SetRangeUser(nll_min-0.2, nll_min + 5.);

  c.SaveAs(Form("%s.png", figure_name));

  //print the summary
  printf("Fit result : r = %.2f, NLL = %.2f, index = %i\n", r_fit, nll_fit, index_fit);
  printf("Scan result: r = %.2f, NLL = %.2f, index = %i\n", r_min, nll_min, index_min);
}
