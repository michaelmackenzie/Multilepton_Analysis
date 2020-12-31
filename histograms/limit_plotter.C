//Script to plot limit results from Higgs Combine

int verbose_ = 1;

struct LimitData_t {
  const static int SIZE = 6;
  double limit_[SIZE];
  double limit_error_[SIZE];
  int    syst_[SIZE];
  float  quantile_[SIZE];
};

struct TreeData_t {
  double limit_;
  double limit_error_;
  int    syst_;
  float  quantile_;
};

int plot_limit(TString selection = "zetau", TString path = "datacards/2016_2017_2018/results/") {
  vector<LimitData_t> results;
  int index = 0;
  TString basename = "higgsCombine_counting_MVA_";
  TString endname  = ".AsymptoticLimits.mH120.root";

  //////////////////
  // Read in data //
  //////////////////

  TreeData_t tree_data;
  while(true) {
    TFile* f = TFile::Open(Form("%s%s%s_%i%s", path.Data(), basename.Data(), selection.Data(), index, endname.Data()), "READ");
    if(!f) break;
    TTree* t = (TTree*) f->Get("limit");
    if(!t) return 1;
    LimitData_t data;
    if(t->GetEntriesFast() > data.SIZE) {
      cout << "Too many entries to handle! Quitting...\n";
      return -1;
    }
    t->SetBranchAddress("limit", &tree_data.limit_);
    t->SetBranchAddress("limitErr", &tree_data.limit_error_);
    t->SetBranchAddress("syst", &tree_data.syst_);
    t->SetBranchAddress("quantileExpected", &tree_data.quantile_);
    if(verbose_ > 1) cout << "Printing results for index " << index << endl;
    for(int entry = 0; entry < t->GetEntriesFast(); ++entry) {
      t->GetEntry(entry);
      data.limit_      [entry] = tree_data.limit_;
      data.limit_error_[entry] = tree_data.limit_error_;
      data.syst_       [entry] = tree_data.syst_;
      data.quantile_   [entry] = tree_data.quantile_;
      if(verbose_ > 1) cout << " Entry " << entry << " limit = " << tree_data.limit_
			    << " limitErr = " << tree_data.limit_error_
			    << " syst = " << tree_data.syst_
			    << " quantile = " << tree_data.quantile_
			    << endl;
    }
    results.push_back(data);
    ++index;
  }
  //if no files found, quit
  if(index == 0) return 2;
  
  //get combined information
  TFile* f = TFile::Open(Form("%s%s%s%s", path.Data(), basename.Data(), selection.Data(), endname.Data()), "READ");
  if(!f) return 3;
  TTree* t = (TTree*) f->Get("limit");
  if(!t) return 4;
  LimitData_t data;
  t->SetBranchAddress("limit", &tree_data.limit_);
  t->SetBranchAddress("limitErr", &tree_data.limit_error_);
  t->SetBranchAddress("syst", &tree_data.syst_);
  t->SetBranchAddress("quantileExpected", &tree_data.quantile_);
  if(verbose_ > 1) cout << "Printing results for combined, index " << index << endl;
  for(int entry = 0; entry < t->GetEntriesFast(); ++entry) {
    t->GetEntry(entry);
    data.limit_      [entry] = tree_data.limit_;
    data.limit_error_[entry] = tree_data.limit_error_;
    data.syst_       [entry] = tree_data.syst_;
    data.quantile_   [entry] = tree_data.quantile_;
    if(verbose_ > 1) cout << " Entry " << entry << " limit = " << tree_data.limit_
			  << " limitErr = " << tree_data.limit_error_
			  << " syst = " << tree_data.syst_
			  << " quantile = " << tree_data.quantile_
			  << endl;
  }
  results.push_back(data);
  ++index;
  
    
  ////////////////////
  // Setup the data //
  ////////////////////
  int i_exp = 2, i_m1s = 1, i_p1s = 3, i_m2s = 0, i_p2s = 4; //indices in data struct
  double exp_xs    [index], exp_ys    [index];
  double exp_p1s_xs[index], exp_p1s_ys[index];
  double exp_m1s_xs[index], exp_m1s_ys[index];
  double exp_p2s_xs[index], exp_p2s_ys[index];
  double exp_m2s_xs[index], exp_m2s_ys[index];
  double yval = index; //where to center the values on the plot

  //for plotting
  double xmin(9999.), xmax(-1);
  for(int i_data = 0; i_data < index; ++i_data) {
    exp_ys[i_data]     = yval;
    exp_p1s_ys[i_data] = 0.25;
    exp_m1s_ys[i_data] = 0.25;
    exp_p2s_ys[i_data] = 0.25;
    exp_m2s_ys[i_data] = 0.25;
    exp_xs[i_data]     = results[i_data].limit_[i_exp];
    exp_p1s_xs[i_data] = abs(results[i_data].limit_[i_p1s] - exp_xs[i_data]);
    exp_m1s_xs[i_data] = abs(results[i_data].limit_[i_m1s] - exp_xs[i_data]);
    exp_p2s_xs[i_data] = abs(results[i_data].limit_[i_p2s] - exp_xs[i_data]);
    exp_m2s_xs[i_data] = abs(results[i_data].limit_[i_m2s] - exp_xs[i_data]);
    yval -= 1.;
    xmax = max(xmax, results[i_data].limit_[i_p2s]);
    xmin = min(xmin, results[i_data].limit_[i_m2s]);
  }
  
  ///////////////////
  // Plot the data //
  ///////////////////

  
  TCanvas* c = new TCanvas("c_lim", "c_lim", 800, 800);
  TGraphAsymmErrors* expected = new TGraphAsymmErrors(index, exp_xs, exp_ys, exp_m1s_xs, exp_p1s_xs,
						      exp_p1s_ys, exp_p1s_ys);
  TGraphAsymmErrors* expected_2s = new TGraphAsymmErrors(index, exp_xs, exp_ys, exp_m2s_xs, exp_p2s_xs,
							 exp_p2s_ys, exp_p2s_ys);
  expected_2s->SetName("gExpected2s");
  expected_2s->SetTitle("Cut-and-count limits by category; Limit / Current Limit; Category");
  expected_2s->SetFillColor(kYellow);
  expected_2s->SetLineWidth(0);
  xmax *= 1.2;
  xmin /= 2.0;
  expected_2s->GetYaxis()->SetRangeUser(0.5, index + 0.5);
  expected_2s->GetXaxis()->SetLimits(xmin, xmax);
  // expected_2s->GetHistogram()->
  expected_2s->Draw("A2");
  
  expected->SetName("gExpected");
  expected->SetMarkerStyle(5);
  expected->SetMarkerSize(1.8);
  expected->SetFillColor(kGreen);
  expected->SetLineWidth(0);
  expected->Draw("2");
  expected->Draw("P");


  //add line separating combined category
  TLine* line = new TLine(xmin, 1.5, xmax, 1.5);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");
  
  //add legend
  TLegend *leg = new TLegend(0.6, 0.1, 0.9, 0.3);
  leg->SetTextSize(0.035);
  leg->AddEntry(expected   , "Expected limit", "P");
  leg->AddEntry(expected   , "68% Expected"  , "F");
  leg->AddEntry(expected_2s, "95% Expected"  , "F");
  leg->Draw("same");

  gSystem->Exec(Form("[ ! -d %sfigures ] && mkdir %sfigures", path.Data(), path.Data()));
  c->SaveAs(Form("%sfigures/limits_%s.png", path.Data(), selection.Data()));
  c->SetLogx();
  c->SaveAs(Form("%sfigures/limits_%s_log.png", path.Data(), selection.Data()));
  
  return 0;
}

int print_standard_limits() {
  int status(0);
  status += plot_limit("zmutau");
  status += plot_limit("hmutau");
  status += plot_limit("zetau");
  status += plot_limit("hetau");
  // status += plot_limit("zmutau_e");
  // status += plot_limit("hmutau_e");
  // status += plot_limit("zetau_mu");
  // status += plot_limit("hetau_mu");
  return status;
}
