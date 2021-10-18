//Script to process the individual and combined category limits and plot them

int make_limit_plot(int set = 8, TString selection = "zmutau",
                    vector<int> years = {2016, 2017, 2018},
                    bool processCards = true,
                    bool doNoSys = false,
                    bool doObs = false) {
  int status(0);
  TString lep = (selection.Contains("mutau")) ? "e" : "mu";
  if(processCards) {
    //Run combine on each category
    gSystem->Exec(Form("combine -d combine_mva_%s_%i%s.txt %s --name _mva_%s_%i%s -t -1 --rMin -20 --rMax 20",
                       selection.Data(), set,
                       (doNoSys) ? "_nosys" : "",
                       (doObs) ? "" : "--run blind", selection.Data(), set,
                       (doNoSys) ? "_nosys" : ""));
    gSystem->Exec(Form("combine -d combine_mva_%s_%s_%i%s.txt %s --name _mva_%s_%s_%i%s -t -1 --rMin -20 --rMax 20",
                       selection.Data(), lep.Data(), set,
                       (doNoSys) ? "_nosys" : "",
                       (doObs) ? "" : "--run blind", selection.Data(), lep.Data(), set,
                       (doNoSys) ? "_nosys" : ""));
    gSystem->Exec(Form("combine -d combine_mva_total_%s_%i%s.txt %s --name _mva_total_%s_%i%s -t -1 --rMin -20 --rMax 20",
                       selection.Data(), set,
                       (doNoSys) ? "_nosys" : "",
                       (doObs) ? "" : "--run blind", selection.Data(), set,
                       (doNoSys) ? "_nosys" : ""));
  }

  //Extract the results for each
  TFile* f1 = TFile::Open(Form("higgsCombine_mva_%s_%i%s.AsymptoticLimits.mH120.root", selection.Data(), set, (doNoSys) ? "_nosys" : ""), "READ");
  TFile* f2 = TFile::Open(Form("higgsCombine_mva_%s_%s_%i%s.AsymptoticLimits.mH120.root", selection.Data(), lep.Data(), set, (doNoSys) ? "_nosys" : ""), "READ");
  TFile* f3 = TFile::Open(Form("higgsCombine_mva_total_%s_%i%s.AsymptoticLimits.mH120.root", selection.Data(), set, (doNoSys) ? "_nosys" : ""), "READ");

  if(!f1 || !f2 || !f3) {
    return 1;
  }

  TTree* t1 = (TTree*) f1->Get("limit");
  if(!t1) {
    cout << "Tree 1 not found!\n";
  }
  t1->SetName("limits_1");

  TTree* t2 = (TTree*) f2->Get("limit");
  if(!t2) {
    cout << "Tree 2 not found!\n";
  }
  t2->SetName("limits_2");

  TTree* t3 = (TTree*) f3->Get("limit");
  if(!t3) {
    cout << "Tree 3 not found!\n";
  }
  t3->SetName("limits_3");

  double expected[3], up_1[3], down_1[3], up_2[3], down_2[3], obs[3], y[3], yerr[3];
  double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6;
  for(int i = 0; i < 3; ++i) {
    TTree* t = t1;
    if     (i == 1) t = t2;
    else if(i == 2) t = t3;
    double val;
    t->SetBranchAddress("limit", &val);
    t->GetEntry(2); expected[i] = scale*val;
    t->GetEntry(0); down_2  [i] = expected[i] - scale*val;
    t->GetEntry(1); down_1  [i] = expected[i] - scale*val;
    t->GetEntry(3); up_1    [i] = scale*val - expected[i];
    t->GetEntry(4); up_2    [i] = scale*val - expected[i];
    t->GetEntry(5); obs     [i] = scale*val;

    y[i] = 3 - i;
    yerr[i] = 0.2;
  }

  TGraphAsymmErrors* expected_1 = new TGraphAsymmErrors(3, expected, y, down_1, up_1, yerr, yerr);
  expected_1->SetName("1_sigma_exp");
  expected_1->SetFillColor(kGreen);
  expected_1->SetMarkerStyle(20);
  expected_1->SetMarkerSize(1.5);
  TGraphAsymmErrors* expected_2 = new TGraphAsymmErrors(3, expected, y, down_2, up_2, yerr, yerr);
  expected_2->SetName("2_sigma_exp");
  expected_2->SetFillColor(kYellow);
  expected_2->SetMarkerStyle(20);
  expected_2->SetMarkerSize(0.8);
  expected_2->SetTitle("CLs Limits");
  expected_2->GetYaxis()->SetRangeUser(0.5, 4.5);
  expected_2->GetXaxis()->SetRangeUser(1.e-6, 1.2*max(expected[0]+up_2[0], expected[1]+up_2[1]));
  expected_2->GetXaxis()->SetTitle("Branching fraction");
  expected_2->GetYaxis()->SetLabelOffset(999);
  expected_2->GetYaxis()->SetLabelSize(0);

  TCanvas* c = new TCanvas("c", "c", 800, 800);
  expected_2->Draw("AE2");
  expected_1->Draw("PE2 SAME");

  TGaxis::SetMaxDigits(3);
  TGraph* g_obs = new TGraph(3, obs, y);
  g_obs->SetName("observed");
  g_obs->SetMarkerStyle(5);
  g_obs->SetMarkerSize(2);
  if(doObs) g_obs->Draw("P SAME");

  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  if(doObs) leg->AddEntry(g_obs, "Observed", "P");
  leg->AddEntry(expected_1, "Expected", "P");
  leg->AddEntry(expected_1, "#pm1#sigma", "F");
  leg->AddEntry(expected_2, "#pm2#sigma", "F");
  leg->Draw();

  //Draw the category labels
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  // label.SetTextColor(1);
  label.SetTextSize(0.05);
  label.SetTextAlign(13);
  label.SetTextAngle(25);
  label.DrawLatex(0.01, 0.55, "Hadronic");
  label.DrawLatex(0.01, 0.38, "Leptonic");
  label.DrawLatex(0.01, 0.18, "Combined");
  label.SetTextSize(0.03);
  label.DrawLatex(0.1, 0.55, Form("%.1e", expected[0]));
  label.DrawLatex(0.1, 0.38, Form("%.1e", expected[1]));
  label.DrawLatex(0.1, 0.18, Form("%.1e", expected[2]));

c->Print(Form("limits_%s_%i%s.png", selection.Data(), set, (doNoSys) ? "_nosys" : ""));
  return status;
}
