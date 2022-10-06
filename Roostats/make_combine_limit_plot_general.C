//Script to process limits for channels/years/etc. and plot the limits
double scale_ = 1.;

struct config_t {
  TString name_;
  TString label_;
  vector<int> sets_;
  vector<int> years_;
  double scale_;

  config_t(TString name, TString label, vector<int> sets, vector<int> years, double scale = 1.):
   name_(name), label_(label), sets_(sets), years_(years), scale_(scale) {}
};

int make_combine_limit_plot_general(vector<config_t> configs, //info for each entry
                                    TString tag, //tag for the output figure file name
                                    TString selection = "zemu",
                                    bool processCards = true,
                                    bool doNoSys = false,
                                    bool doObs = false) {

  if(configs.size() == 0) {
    cout << "No configuration cards given!\n";
    return 1;
  }


  //Fit results for each input
  vector<TFile*> file_list;
  vector<TTree*> tree_list;

  //Loop through each input configuration
  for(config_t config : configs) {
    vector<int> years = config.years_;
    vector<int> sets  = config.sets_;
    TString card      = config.name_;

    //Move to the proper directory
    if(years.size() == 0) { cout << "No years given!\n"; return 2; }
    TString year_string = Form("%i", years[0]);
    for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
    TString dir = Form("datacards/%s", year_string.Data());
    gSystem->cd(dir.Data());

    if(sets.size() == 0) { cout << "No sets given!\n"; return 3; }
    TString set_string = Form("%i", sets[0]);
    for(int i = 1; i < sets.size(); ++i) set_string += Form("_%i", sets[i]);

    int status(0);
    if(processCards) {
      //Run combine on each datacard
      gSystem->Exec(Form("combine -d combine_%s.txt %s --name _%s -t -1 --rMin -20 --rMax 20 %s",
                         card.Data(),
                         (doObs) ? "" : "--run blind",
                         card.Data(),
                         (doNoSys) ? "--freezeParameters allConstrainedNuisances" : ""));
    }

    TFile* f = TFile::Open(Form("higgsCombine_%s.AsymptoticLimits.mH120.root", card.Data()), "READ");
    if(!f) return 4;
    file_list.push_back(f);
    TTree* t = (TTree*) f->Get("limit");
    if(!t) {
      cout << "Tree for card name " << card.Data() << " not found\n";
      return 5;
    }
    t->SetName(Form("limits_%s", card.Data()));
    tree_list.push_back(t);
    gSystem->cd("../..");
  }

  const int nfiles = configs.size();
  double expected[nfiles], up_1[nfiles], down_1[nfiles], up_2[nfiles], down_2[nfiles], obs[nfiles], y[nfiles], yerr[nfiles];
  double max_val = -1.e9;
  double min_val =  1.e9;
  for(int itree = 0; itree < nfiles; ++itree) {
    TTree* t = tree_list[itree];
    double val;
    const double scale = configs[itree].scale_;
    t->SetBranchAddress("limit", &val);
    t->GetEntry(2); expected[itree] = scale*val;
    t->GetEntry(0); down_2  [itree] = expected[itree] - scale*val;
    t->GetEntry(1); down_1  [itree] = expected[itree] - scale*val;
    t->GetEntry(3); up_1    [itree] = scale*val       - expected[itree];
    t->GetEntry(4); up_2    [itree] = scale*val       - expected[itree];
    t->GetEntry(5); obs     [itree] = scale*val;

    y[itree] = nfiles - itree;
    yerr[itree] = 0.2;
    max_val = max(max_val, expected[itree] + up_2  [itree]);
    min_val = min(min_val, expected[itree] - down_2[itree]);
  }

  TGraphAsymmErrors* expected_1 = new TGraphAsymmErrors(nfiles, expected, y, down_1, up_1, yerr, yerr);
  expected_1->SetName("1_sigma_exp");
  expected_1->SetFillColor(kGreen);
  expected_1->SetMarkerStyle(20);
  expected_1->SetMarkerSize(1.5);

  TGraphAsymmErrors* expected_2 = new TGraphAsymmErrors(nfiles, expected, y, down_2, up_2, yerr, yerr);
  expected_2->SetName("2_sigma_exp");
  expected_2->SetFillColor(kYellow);
  expected_2->SetMarkerStyle(20);
  expected_2->SetMarkerSize(0.8);
  expected_2->SetTitle("CLs Limits");
  expected_2->GetYaxis()->SetRangeUser(0.5, nfiles*1.3);
  expected_2->GetXaxis()->SetRangeUser(0.8*max(1.e-15, min_val), 1.2*max_val);
  expected_2->GetXaxis()->SetTitle("Branching fraction");
  expected_2->GetYaxis()->SetLabelOffset(999);
  expected_2->GetYaxis()->SetLabelSize(0);

  TCanvas* c = new TCanvas("c", "c", 800, 800);
  expected_2->Draw("AE2");
  expected_1->Draw("PE2 SAME");

  TGaxis::SetMaxDigits(3);
  TGraph* g_obs = new TGraph(nfiles, obs, y);
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
    label.DrawLatex(0.01, yloc, configs[icard].label_.Data());
    label.DrawLatex(0.1 , yloc, Form("%.1e", expected[icard]));
  }
  // label.DrawLatex(0.1, 0.38, Form("%.1e", expected[1]));
  // label.DrawLatex(0.1, 0.18, Form("%.1e", expected[2]));

  c->Print(Form("limits_%s_%s.png", selection.Data(), tag.Data()));
  return 0;
}
