//Script to test the effect of randomly setting +/- uncertainties

double test_random_sys_set(int nsys, int seed1, int seed2, double& tot_sys) {
  TRandom3* rnd_sys = new TRandom3(seed1); //for generating systematics
  TRandom3* rnd_est = new TRandom3(seed2); //for random test of +1/-1
  vector<double> sys; //list of systematic uncertainties
  int gen_mode = 0; //how systematic uncertainties are generated
  tot_sys = 0.; //actual total systematic uncertainty
  double est_sys = 0.; //estimated systematic
  for(int isys = 0; isys < nsys; ++isys) {
    double sys_val;
    if(gen_mode == 0) sys_val = 3.*rnd_sys->Uniform(); //flat between 0 - 3
    else {
      cout << "Unkown systematic generation method!\n";
      return -1.;
    }
    tot_sys += sys_val*sys_val;
    est_sys += rnd_est->Uniform() > 0.5 ? sys_val : -1.*sys_val;
    // est_sys += rnd_est->Gaus(0., sys_val);
  }
  tot_sys = sqrt(tot_sys);
  // cout << "Total systematic = " << tot_sys << " vs est = "
  //      << est_sys << " for " << nsys << " systematics with seed = "
  //      << seed2 << endl;
  delete rnd_sys;
  delete rnd_est;
  return abs(est_sys);
}

void test_random_sys_set(int ntests = 10, int Method = 0) {
  int nmin   = 10;
  int nmax   = 1000;
  int nsteps = 990;
  int seed0  = 90;
  double ns[nsteps+1], errs[nsteps+1], tots[nsteps+1], ests[nsteps+1];
  TH1F* herr = new TH1F("herr", "Systematic uncertainty error over estimated value", (int) (5*sqrt(nsteps)), -5., 5.);
  double max_tot(-1.), max_est(-1.);
  for(int istep = 0; istep <= nsteps; ++istep) {
    ns[istep] = nmin + istep*(nmax - nmin)*1./nsteps;
    double tot_sys(0.), est_sys(0.);
    vector<double> estimates;
    for(int itest = 0; itest < ntests; ++itest) {
      double est = test_random_sys_set(ns[istep], seed0 + istep, seed0 + ntests*istep + (itest+1), tot_sys);
      if(Method == 0) { //normal average
        est_sys += est/ntests;
      } else if(Method == 1) { //add in quadature
        est_sys += est*est;
      } else if(Method == 2) { //take the maximum
        est_sys = max(est, est_sys);
      } else if(Method == 3 || Method == 4) { //take the average of the top 50%
        estimates.push_back(est);
      }
    }
    if(Method == 1) {
      est_sys = sqrt(est_sys/ntests);
    } else if(Method == 3 || Method == 4) {
      std::sort(estimates.begin(), estimates.end());
      int nests = estimates.size() - estimates.size()/2;
      for(unsigned index = estimates.size()/2; index < estimates.size(); ++index) {
        if(Method == 3)
          est_sys += estimates[index] / nests;
        else if(Method == 4)
          est_sys += estimates[index]*estimates[index];
      }
      if(Method == 4) est_sys = sqrt(est_sys / nests);
    }
    ests[istep] = est_sys;
    tots[istep] = tot_sys;
    max_tot = max(tot_sys, max_tot);
    max_est = max(est_sys, max_est);
    errs[istep] = (est_sys - tot_sys)/est_sys;
    herr->Fill(errs[istep]);
  }
  TCanvas* c = new TCanvas(Form("c_test_%i", Method), "c_test", 1200, 700);
  c->Divide(2,1);
  c->cd(1);
  // TGraph* g = new TGraph(nsteps+1, ns, errs);
  TGraph* g = new TGraph(nsteps+1, tots, ests);
  g->SetLineColor(kBlue);
  g->SetLineWidth(2);
  g->SetMarkerStyle(20);
  g->SetMarkerSize(0.8);
  TString method = "Average";
  if(Method == 1) method = "Quad Avg.";
  else if(Method == 2) method = "Maximum";
  else if(Method == 3) method = "Avg of top 50%";
  else if(Method == 4) method = "Quad Avg of top 50%";
  g->SetTitle(Form("%s %i tests vs Actual Systematic; True systematic; Estimated systematic", method.Data(), ntests));
  g->Draw("AP");
  g->GetXaxis()->SetRangeUser(0., 1.1*max_tot);
  g->GetYaxis()->SetRangeUser(0., 1.1*max_est);
  g->GetYaxis()->SetTitleOffset(1.4);
  TLine* line = new TLine(0., 0., min(max_tot, max_est), min(max_tot,max_est));
  line->SetLineWidth(2);
  line->SetLineColor(kRed);
  line->Draw("same");
  c->cd(2);
  herr->SetLineWidth(2);
  herr->Draw();
  herr->SetXTitle("(est - true)/est Systematic uncertainty");

  gSystem->Exec("[ ! -d plots/tests ] && mkdir -p plots/tests");
  c->Print(Form("plots/tests/test_random_sys_set_navg_%i_method_%i.png", ntests, Method));
}
