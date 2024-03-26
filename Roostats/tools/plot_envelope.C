//plot envelope for Z->e+mu fit

int plot_envelope(const int set = 13) {

  //create a list of scan files for the set
  vector<TString> files;
  int icat = 0;
  while(icat < 10) {
    const char* file = Form("higgsCombine_env_%i_cat_%i.MultiDimFit.mH120.root", set, icat);
    if(!gSystem->AccessPathName(file)) {
      cout << "Adding " << file << endl;
      files.push_back(file);
    }
    ++icat;
  }
  //add the profiled index version
  files.push_back(Form("higgsCombine_env_%i_tot.MultiDimFit.mH120.root", set));

  //read each scan and create a graph of the NLL
  vector<TTree*> trees;
  vector<TGraph*> graphs;
  int index = 0;
  double min_val(1.e20), max_val(-1.e20);
  for(TString file : files) {
    TFile* f = TFile::Open(file.Data(), "READ");
    if(!f) return 1;
    TTree* tree = (TTree*) f->Get("limit");
    if(!tree) return 2;
    trees.push_back(tree);
    const int nentries = tree->GetEntries();
    double rvals[nentries], nlls[nentries];
    double nll0, nll;
    float r, dnll;
    tree->SetBranchAddress("r", &r);
    tree->SetBranchAddress("deltaNLL", &dnll);
    tree->SetBranchAddress("nll0", &nll0);
    tree->SetBranchAddress("nll" , &nll);
    for(int entry = 0; entry < nentries-1; ++entry) {
      tree->GetEntry(entry+1);
      rvals[entry] = r;
      nlls [entry] = nll0 + nll + dnll;
      min_val = min(min_val, nlls[entry]);
      max_val = max(max_val, nlls[entry]);
    }
    TGraph* g = new TGraph(nentries-1, rvals, nlls);
    g->SetName(Form("gNLL_%i", index));
    graphs.push_back(g);
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

  //Add 1 and 2 sigma lines
  for(int ipoint = 1; ipoint < tot->GetN(); ++ipoint) {
    const double y(tot->GetY()[ipoint]-min_val), y_prev(tot->GetY()[ipoint-1]-min_val);
    const double x(tot->GetX()[ipoint]);
    if((y < 2. && y_prev > 2.) || (y > 2. && y_prev < 2.)) {
      TLine* line = new TLine(x, min_val-buffer, x, y+min_val);
      line->SetLineWidth(2);
      line->SetLineStyle(kDashed);
      line->SetLineColor(kBlack);
      line->Draw("same");
      cout << "Found 2 sigma edge at r = " << x << " ( " << y_prev << " - " << y << ")\n";
    }
    if((y < 0.5 && y_prev > 0.5) || (y > 0.5 && y_prev < 0.5)) {
      TLine* line = new TLine(x, min_val-buffer, x, y+min_val);
      line->SetLineWidth(2);
      line->SetLineStyle(kDashed);
      line->SetLineColor(kBlack);
      line->Draw("same");
      cout << "Found 1 sigma edge at r = " << x << " ( " << y_prev << " - " << y << ")\n";
    }
  }

  tot->SetTitle("Envelope;r;NLL");
  c->SaveAs(Form("envelope_%i.png", set));
  return 0;
}
