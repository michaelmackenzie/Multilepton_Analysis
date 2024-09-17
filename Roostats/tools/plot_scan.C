//plot a likelihood scan for a single parameter
bool remove_zero_point_ = true; //Plot delta NLL
bool plot_2nll_         = true; //Plot 2*NLL

int plot_scan(const char* file, const char* var = "r", const char* tag = "", const int verbose = 0) {

  //Retrieve the input data
  TFile* f = TFile::Open(file, "READ");
  if(!f) return 1;
  TTree* tree = (TTree*) f->Get("limit");
  if(!tree) return 2;

  if(verbose > 1) tree->Print();

  //read the scan and create a graph of the NLL
  double min_val(1.e20), max_val(-1.e20);
  double r_fit, nll_fit(1.e20); //best fit value
  const int nentries = tree->GetEntries();
  double rvals[nentries-1], nlls[nentries-1];
  double nll0, nll;
  float r, dnll;
  tree->SetBranchAddress(var, &r);
  tree->SetBranchAddress("deltaNLL", &dnll);
  tree->SetBranchAddress("nll0", &nll0);
  tree->SetBranchAddress("nll" , &nll);
  for(int entry = 0; entry < nentries; ++entry) {
    tree->GetEntry(entry);
    double nll_val = nll0 + nll + dnll;
    if(plot_2nll_) nll_val *= 2.;
    min_val = min(min_val, nll_val);
    max_val = max(max_val, nll_val);

    if(verbose > 3) {
      printf(" Point %3i: %s = %7.2f; nll = %12.2f (dNll = %.2f, nll0 = %.2f, nll = %.2f)\n", entry, var, r, nll_val, dnll, nll0, nll);
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
    nlls [index] = (remove_zero_point_) ? nll_val - nll_fit : nll_val;
  }

  if(remove_zero_point_) {
    min_val -= nll_fit;
    max_val -= nll_fit;
    nll_fit = 0.;
  }

  //Create a graph of the scan and the optimal point
  TGraph* g = new TGraph(nentries-1, rvals, nlls);
  g->SetName("gNLL");

  TGraph* g_best = new TGraph(1, &r_fit, &nll_fit);
  g_best->SetName("gNLL_best");

  //decide whether a maximum NLL value is too large
  const double max_allowed = (plot_2nll_) ? 40. : 20;
  max_val = min(max_allowed + min_val, max_val);

  //Create NLL plot with envelope
  TCanvas* c = new TCanvas();
  g->SetLineColor(kBlue);
  g->SetMarkerColor(kBlue);
  g->SetLineWidth(3);
  g->SetMarkerSize(0.8);
  g->SetMarkerStyle(20);
  g->Draw("AL");
  const double buffer = 0.05*(max_val-min_val);
  g->GetYaxis()->SetRangeUser(min_val-buffer, max_val+buffer);

  g_best->SetMarkerColor(kBlue+1);
  g_best->SetMarkerSize(2.);
  g_best->SetMarkerStyle(kFullStar);
  g_best->Draw("P");
  g_best->Print("v");


  printf("Best fit %s = %.3f\n", var, r_fit);
  double s1_lo(rvals[0]), s1_hi(rvals[nentries-2]); //for fitting the NLL distribution

  //Check for the 1/2 sigma crossings
  const double s1_val((plot_2nll_) ? 1. : 0.5), s2_val((plot_2nll_) ? 4. : 2.); //delta NLL values
  int i_s1_left(-1), i_s1_right(-1), i_s2_left(-1), i_s2_right(-1); //indices of the crossing points
  for(int ipoint = 1; ipoint < g->GetN(); ++ipoint) {
    const double y(g->GetY()[ipoint]-min_val), y_prev(g->GetY()[ipoint-1]-min_val);
    const double x((g->GetX()[ipoint] + g->GetX()[ipoint-1])/2.);
    // const double x(g->GetX()[ipoint]);
    //2-sigma crossing point
    if((y < s2_val && y_prev > s2_val) || (y > s2_val && y_prev < s2_val)) {
      TLine* line = new TLine(x, min_val-buffer, x, min_val + ((plot_2nll_) ? 4. : 2.));
      line->SetLineWidth(2);
      line->SetLineStyle(kDashed);
      line->SetLineColor(kBlack);
      line->Draw("same");
      cout << "Found 2 sigma edge at " << var << " = " << x << " ( " << y_prev << " - " << y << ")\n";
      if(y_prev < y) i_s2_right = ipoint;
      else           i_s2_left  = ipoint;
    }
    //1-sigma crossing point
    if((y < s1_val && y_prev > s1_val) || (y > s1_val && y_prev < s1_val)) {
      TLine* line = new TLine(x, min_val-buffer, x, min_val + ((plot_2nll_) ? 1. : 0.5));
      line->SetLineWidth(2);
      line->SetLineStyle(kDashed);
      line->SetLineColor(kBlack);
      line->Draw("same");
      cout << "Found 1 sigma edge at " << var << " = " << x << " ( " << y_prev << " - " << y << ")\n";
      if(x < r_fit) s1_lo = x;
      else          s1_hi = x;
      if(y_prev < y) i_s1_right = ipoint;
      else           i_s1_left  = ipoint;
    }
  }

  TF1* fit_func = new TF1("fit_func", (plot_2nll_) ? "((x - [minimum])/[width])^2 + [offset]" : "0.5*((x - [minimum])/[width])^2 + [offset]",
                          (s1_lo + (r_fit - s1_lo)*0.5), (s1_hi - (s1_hi - r_fit)*0.5));
  fit_func->SetParameters(r_fit, 0., (r_fit - s1_lo));
  g->Fit(fit_func, "R");
  if(verbose > 3)
    fit_func->Print();
  g->SetTitle(Form("Likelihood scan;%s;%s", var, (plot_2nll_) ? "2*#DeltaNLL" : (remove_zero_point_) ? "#DeltaNLL" : "NLL"));
  g->GetXaxis()->SetTitleSize(0.05);
  g->GetXaxis()->SetTitleOffset(0.70);
  g->GetYaxis()->SetTitleSize(0.05);
  g->GetYaxis()->SetTitleOffset(0.76);
  c->SaveAs(Form("scan_%s%s.png", var, tag));
  printf("1-sigma scan results  : %s = %7.4f + %7.4f - %7.4f\n", var, r_fit, (r_fit - s1_lo), (s1_hi - r_fit));
  printf("Fit to the scan result: %s = %7.4f +- %7.4f\n", var, fit_func->GetParameter(0), fit_func->GetParameter(2));

  //Fit the crossings
  if(i_s2_left >= 0) {
    fit_func->SetRange(g->GetX()[max(0, i_s2_left-3)], g->GetX()[min(g->GetN()-1, i_s2_left+3)]);
    float fit_cross = fit_func->GetX((plot_2nll_) ? 4. : 2.);
    printf("Fit left : 2-sigma crossing = %6.3f (+%.3f)\n", fit_cross, r_fit - fit_cross);
  }
  if(i_s2_right >= 0) {
    fit_func->SetRange(g->GetX()[max(0, i_s2_right-3)], g->GetX()[min(g->GetN()-1, i_s2_right+3)]);
    float fit_cross = fit_func->GetX((plot_2nll_) ? 4. : 2.);
    printf("Fit right: 2-sigma crossing = %6.3f (+%.3f)\n", fit_cross, fit_cross - r_fit);
  }
  if(i_s1_left >= 0) {
    fit_func->SetRange(g->GetX()[max(0, i_s1_left-3)], g->GetX()[min(g->GetN()-1, i_s1_left+3)]);
    float fit_cross = fit_func->GetX((plot_2nll_) ? 1. : 0.5);
    printf("Fit left : 1-sigma crossing = %6.3f (-%.3f)\n", fit_cross, r_fit - fit_cross);
  }
  if(i_s1_right >= 0) {
    fit_func->SetRange(g->GetX()[max(0, i_s1_right-3)], g->GetX()[min(g->GetN()-1, i_s1_right+3)]);
    float fit_cross = fit_func->GetX((plot_2nll_) ? 1. : 0.5);
    printf("Fit right: 1-sigma crossing = %6.3f (+%.3f)\n", fit_cross, fit_cross - r_fit);
  }
  return 0;
}
