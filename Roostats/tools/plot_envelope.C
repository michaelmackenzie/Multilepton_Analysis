//plot envelope for Z->e+mu fit
bool remove_zero_point_ = true;

int plot_envelope(const int set = 13, const char* file = "", const char* tag = "", const int verbose = 0) {

  //create a list of scan files for the set
  vector<TString> files;
  if(!file || TString(file) == "") {
    int icat = 0;
    while(icat < 10) {
      const char* file = Form("higgsCombine_env_%i_cat_%i.MultiDimFit.mH125.123456.root", set, icat);
      if(!gSystem->AccessPathName(file)) {
        cout << "Adding " << file << endl;
        files.push_back(file);
      }
      ++icat;
    }
    //add the profiled index version
    files.push_back(Form("higgsCombine_env_%i_tot.MultiDimFit.mH125.123456.root", set));
  } else { //use the given file
    files.push_back(file);
  }

  //read each scan and create a graph of the NLL
  vector<TTree*> trees;
  vector<TGraph*> graphs;
  int index = 0;
  double min_val(1.e20), max_val(-1.e20);
  double r_fit, nll_fit(1.e20);
  for(TString file : files) {
    if(verbose > 0) cout << "Checking file " << file.Data() << endl;
    TFile* f = TFile::Open(file.Data(), "READ");
    if(!f) return 1;
    TTree* tree = (TTree*) f->Get("limit");
    if(!tree) return 2;
    trees.push_back(tree);
    const int nentries = tree->GetEntries();
    double rvals[nentries-1], nlls[nentries-1];
    double nll0, nll;
    float r, dnll;
    tree->SetBranchAddress("r", &r);
    tree->SetBranchAddress("deltaNLL", &dnll);
    tree->SetBranchAddress("nll0", &nll0);
    tree->SetBranchAddress("nll" , &nll);
    for(int entry = 0; entry < nentries; ++entry) {
      tree->GetEntry(entry);
      const double nll_val = nll0 + nll + dnll;
      min_val = min(min_val, nll_val);
      max_val = max(max_val, nll_val);

      if(verbose > 3) {
        printf(" Point %3i: r = %7.2f; nll = %12.2f\n", entry, r, nll_val);
      }

      //entry 0 is the best fit result for this scan
      if(entry == 0 && nll_val < nll_fit) {
        r_fit   = r;
        nll_fit = nll_val;
        continue;
      }

      //scan results
      const int index = entry-1;
      rvals[index] = r;
      nlls [index] = nll_val;
    }
    TGraph* g = new TGraph(nentries-1, rvals, nlls);
    g->SetName(Form("gNLL_%i", index));
    graphs.push_back(g);
  }

  //remove the minimum NLL from all functions
  if(remove_zero_point_) {
    for(unsigned igraph = 0; igraph < graphs.size(); ++igraph) {
      auto g = graphs[igraph];
      for(int ipoint = 0; ipoint < g->GetN(); ++ipoint) {
        g->SetPoint(ipoint, g->GetX()[ipoint], g->GetY()[ipoint] - min_val);
      }
    }
    max_val = max_val - min_val;
    min_val = 0.;
  }

  //decide whether a maximum NLL value is too large
  const double max_allowed = 20;
  max_val = min(max_allowed + min_val, max_val);

  //Create NLL plot with envelope
  TCanvas* c = new TCanvas();
  TGraph* tot = graphs.back();
  tot->SetLineColor(kBlue);
  tot->SetMarkerColor(kBlue);
  tot->SetLineWidth(3);
  tot->SetMarkerSize(0.8);
  tot->SetMarkerStyle(20);
  tot->Draw("AL");
  const double buffer = 0.05*(max_val-min_val);
  tot->GetYaxis()->SetRangeUser(min_val-buffer, max_val+buffer);

  const int colors[] = {kRed, kGreen, kOrange, kViolet, kYellow-3};
  const int ncolors = sizeof(colors)/sizeof(*colors);
  for(int igraph = 0; igraph < graphs.size() - 1; ++igraph) {
    TGraph* g = graphs[igraph];
    const int color = colors[igraph % ncolors];
    g->SetLineColor(color);
    g->SetMarkerColor(color);
    g->SetLineWidth(2);
    g->SetMarkerSize(0.8);
    g->SetMarkerStyle(20);
    g->Draw("PL");
  }

  tot->Draw("L");

  printf("Best fit r = %.3f\n", r_fit);
  //Add 1 and 2 sigma lines
  const double s1_val(1.), s2_val(4.); //delta NLL values
  for(int ipoint = 1; ipoint < tot->GetN(); ++ipoint) {
    const double y(tot->GetY()[ipoint]-min_val), y_prev(tot->GetY()[ipoint-1]-min_val);
    const double x(tot->GetX()[ipoint]);
    if((y < s2_val && y_prev > s2_val) || (y > s2_val && y_prev < s2_val)) {
      TLine* line = new TLine(x, min_val-buffer, x, y+min_val);
      line->SetLineWidth(2);
      line->SetLineStyle(kDashed);
      line->SetLineColor(kBlack);
      line->Draw("same");
      cout << "Found 2 sigma edge at r = " << x << " ( " << y_prev << " - " << y << ")\n";
    }
    if((y < s1_val && y_prev > s1_val) || (y > s1_val && y_prev < s1_val)) {
      TLine* line = new TLine(x, min_val-buffer, x, y+min_val);
      line->SetLineWidth(2);
      line->SetLineStyle(kDashed);
      line->SetLineColor(kBlack);
      line->Draw("same");
      cout << "Found 1 sigma edge at r = " << x << " ( " << y_prev << " - " << y << ")\n";
    }
  }

  tot->SetTitle("Envelope;r;NLL");
  c->SaveAs(Form("envelope_%i%s.png", set, tag));
  return 0;
}
