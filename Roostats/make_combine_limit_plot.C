//Script to process limits for channels/years/etc. and plot the limits

int make_combine_limit_plot(vector<TString> cards, //combine datacard names to process
                            vector<TString> labels, //label for each card
                            TString tag, //tag for the output figure file name
                            int set = 8, TString selection = "zmutau",
                            vector<int> years = {2016, 2017, 2018},
                            bool processCards = true,
                            bool doObs = false) {
  //Move to the proper directory
  TString year_string = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
  TString dir = Form("datacards/%s", year_string.Data());

  int status(0);
  if(processCards) {
    //Run combine on each datacard
    for(TString card : cards) {
      gSystem->Exec(Form("combine -d combine_mva_%s.txt %s --name _%s -t -1 --rMin -20 --rMax 20",
                         card.Data(),
                         (doObs) ? "" : "--run blind",
                         card.Data()));
    }
  }

  //Extract the results for each
  vector<TFile*> file_list;
  vector<TTree*> tree_list;
  for(TString card : cards) {
    TFile* f = TFile::Open(Form("higgsCombine_mva_%s.AsymptoticLimits.mH120.root", card.Data()), "READ");
    if(!f) return 1;
    file_list.push_back(f);
    TTree* t = (TTree*) f->Get("limit");
    if(!t) {
      cout << "Tree for card name " << card.Data() << " not found\n";
      return 2;
    }
    t->SetName(Form("limits_%s", card.Data()));
    tree_list.push_back(t);
  }

  const int nfiles = cards.size();
  double expected[nfiles], up_1[nfiles], down_1[nfiles], up_2[nfiles], down_2[nfiles], obs[nfiles], y[nfiles], yerr[nfiles];
  const double scale = (selection.Contains("h")) ? 1.e-4 : 1.e-6; //get the absolute branching fraction from signal strength
  for(int itree = 0; itree < nfiles; ++itree) {
    TTree* t = tree_list[itree];
    double val;
    t->SetBranchAddress("limit", &val);
    t->GetEntry(2); expected[itree] = scale*val;
    t->GetEntry(0); down_2  [itree] = expected[itree] - scale*val;
    t->GetEntry(1); down_1  [itree] = expected[itree] - scale*val;
    t->GetEntry(3); up_1    [itree] = scale*val       - expected[itree];
    t->GetEntry(4); up_2    [itree] = scale*val       - expected[itree];
    t->GetEntry(5); obs     [itree] = scale*val;

    y[itree] = nfiles - itree;
    yerr[itree] = 0.2;
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
  expected_2->GetYaxis()->SetRangeUser(0.5, nfiles*1.3);
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
  label.SetTextSize(0.03);
  for(int icard = 0; icard < nfiles; ++icard) {
    const double yloc = (nfiles == 1) ? 0.5 : 0.68 - (icard)*(0.68-0.2)/(nfiles-1);
    label.DrawLatex(0.01, yloc, labels[icard].Data());
    label.DrawLatex(0.1 , yloc, Form("%.1e", expected[icard]));
  }
  // label.DrawLatex(0.1, 0.38, Form("%.1e", expected[1]));
  // label.DrawLatex(0.1, 0.18, Form("%.1e", expected[2]));

  c->Print(Form("limits_%s_%i_%s.png", selection.Data(), set, tag.Data()));
  return status;
}
