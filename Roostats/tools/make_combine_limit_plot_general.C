//Script to process limits for channels/years/etc. and plot the limits
double scale_ = 1.;
TString dir_ = "";
bool speed_limit_  = true; //use Combine arguments to speed up limit calculation
bool more_precise_ = false; //more precise steps in the minimizations
bool preliminary_  = true;
bool add_values_   = true; //add text values of the limits to the plot
bool log_plot_     = true; //plot the x-axis in log-scale

struct config_t {
  TString name_;
  TString label_;
  vector<int> sets_;
  vector<int> years_;
  double scale_;
  double rmin_;
  double rmax_;

  config_t(TString name, TString label, vector<int> sets, vector<int> years, double scale = 1., double rmin = -20., double rmax = 20.):
    name_(name), label_(label), sets_(sets), years_(years), scale_(scale), rmin_(rmin), rmax_(rmax) {}
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
    TString name      = config.name_;
    const double rmin = config.rmin_;
    const double rmax = config.rmax_;

    //Move to the proper directory
    if(years.size() == 0) { cout << "No years given!\n"; return 2; }
    TString year_string = Form("%i", years[0]);
    for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
    TString dir = (dir_ == "") ? Form("datacards/%s", year_string.Data()) : dir_;
    cout << ">>> Using directory " << dir.Data() << endl;
    gSystem->cd(dir.Data());

    int status(0);
    if(processCards) {
      TString additional_command = "";
      if(doNoSys) {
        if(selection == "zemu") additional_command = "--freezeParameters 'rgx{.*},var{cat_.*},var{bst_.*},var{exp_.*}'";
        else                    additional_command = "--freezeParameters allConstrainedNuisances";
      }
      if(speed_limit_) additional_command += " --cminDefaultMinimizerStrategy 0";
      if(selection == "zemu") {
        additional_command += " --X-rtd MINIMIZER_freezeDisassociatedParams";
        additional_command += " --X-rtd REMOVE_CONSTANT_ZERO_POINT=1";
        additional_command += " --X-rtd MINIMIZER_multiMin_hideConstants";
        additional_command += " --X-rtd MINIMIZER_multiMin_maskConstraints";
      } else {
        additional_command += " --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1";
      }
      if(more_precise_) additional_command += " --rAbsAcc 0.0005 --rRelAcc 0.0005 --cminDefaultMinimizerTolerance 0.001 --cminDiscreteMinTol 0.0001";

      //Run combine on each datacard
      TString card = (name.BeginsWith("datacard")) ? name + ".txt" : "combine_" + name + ".txt";
      printf("Processing combine card %s/%s\n", dir.Data(), card.Data());
      TString command = Form("combine -d %s %s --name _%s --rMin %.1f --rMax %.1f %s",
                             card.Data(),
                             (doObs) ? "" : "-t -1 --run blind",
                             name.Data(),
                             rmin, rmax,
                             additional_command.Data());
      printf(">>> %s\n", command.Data());
      gSystem->Exec(command.Data());
    }

    TFile* f = TFile::Open(Form("higgsCombine_%s.AsymptoticLimits.mH120.root", name.Data()), "READ");
    if(!f) return 4;
    file_list.push_back(f);
    TTree* t = (TTree*) f->Get("limit");
    if(!t) {
      cout << "Tree for card name " << name.Data() << " not found\n";
      return 5;
    }
    t->SetName(Form("limits_%s", name.Data()));
    tree_list.push_back(t);
    gSystem->cd("../..");
  }

  const int nfiles = configs.size();
  double expected[nfiles], up_1[nfiles], down_1[nfiles], up_2[nfiles], down_2[nfiles], obs[nfiles], y[nfiles], yerr[nfiles], zeros[nfiles];
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
    max_val = max(max_val, max(expected[itree] + up_2  [itree], obs[itree]));
    min_val = min(min_val, min(expected[itree] - down_2[itree], obs[itree]));
    zeros[itree] = 0.;
    if(doObs) {
      printf("%s: r < %.5e (%.5e (+%.5e, -%.5e) [%.5e - %.5e])\n", configs[itree].name_.Data(),
             obs[itree],
             expected[itree], up_1[itree], down_1[itree],
             expected[itree] - down_1[itree], expected[itree]  + up_1[itree]);
    } else {
      printf("%s: r < %.5e (+%.5e, -%.5e) [%.5e - %.5e]\n", configs[itree].name_.Data(),
             expected[itree], up_1[itree], down_1[itree],
             expected[itree] - down_1[itree], expected[itree]  + up_1[itree]);
    }
  }

  TGraphAsymmErrors* expected_1 = new TGraphAsymmErrors(nfiles, expected, y, down_1, up_1, yerr, yerr);
  expected_1->SetName("1_sigma_exp");
  expected_1->SetFillColor(kGreen+1);

  TGraphAsymmErrors* expected_line = new TGraphAsymmErrors(nfiles, expected, y, zeros, zeros, yerr, yerr);
  expected_line->SetName("line_exp");
  expected_line->SetLineStyle(kDashed);
  expected_line->SetLineWidth(2);
  expected_line->SetLineColor(kBlack);
  expected_line->SetMarkerStyle(20);
  expected_line->SetMarkerSize(1.5);

  const double ymax = 1.3*nfiles;
  const double ymin = 0.5;
  TGraphAsymmErrors* expected_2 = new TGraphAsymmErrors(nfiles, expected, y, down_2, up_2, yerr, yerr);
  expected_2->SetName("2_sigma_exp");
  expected_2->SetFillColor(kOrange);
  expected_2->SetMarkerStyle(20);
  expected_2->SetMarkerSize(0.8);
  // expected_2->SetTitle("");
  expected_2->SetTitle(Form(";95%% upper limit on BF(Z^{0} #rightarrow %s^{#pm}%s^{#mp});",
                                        selection.BeginsWith("zmu") ? "#mu" : "e", selection.EndsWith("mu") ? "#mu" : "#tau"));

  //calculate x-axis range
  const float scale_size = (log10(max_val) - log10(min_val)); //orders of magnitude spanning the plot
  const float xmax = (log_plot_) ? max_val*pow(2.0,max(0.f, scale_size)) : 1.1*max_val;
  const float xmin = (log_plot_) ? min_val/pow(1.5,max(0.f, scale_size)) : 0.;
  printf("Max val = %.2e, Min val = %.2e --> xrange = [%.2e , %.2e]\n", max_val, min_val, xmin, xmax);

  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TCanvas* c = new TCanvas("c", "c", 800, 800);
  c->SetLeftMargin(0.21);
  //Create a histogram to use as the axis
  TH1* haxis = new TH1F("haxis", "", 1, xmin, xmax);
  haxis->SetTitle(expected_2->GetTitle());
  haxis->Draw();

  //Add the graphs to the plot
  expected_2->Draw("E2");
  expected_1->Draw("E2 SAME");
  expected_line->Draw("PE SAME");
  if(log_plot_) c->SetLogx();
  haxis->GetYaxis()->SetRangeUser(ymin, ymax);
  haxis->GetXaxis()->SetRangeUser(xmin, xmax);
  haxis->GetXaxis()->SetMoreLogLabels(true);
  haxis->GetYaxis()->SetLabelOffset(1e10);
  haxis->GetYaxis()->SetLabelSize(0);
  haxis->GetXaxis()->SetTitleOffset(1.2);

  TGaxis::SetMaxDigits(3);
  TGraph* g_obs = new TGraph(nfiles, obs, y);
  g_obs->SetName("observed");
  g_obs->SetMarkerStyle(70);
  g_obs->SetMarkerSize(3);
  if(doObs) g_obs->Draw("P SAME");

  // c->SetGridx();

  TLegend* leg = new TLegend(0.6, 0.75, 0.89, 0.89);
  if(doObs) leg->AddEntry(g_obs, "Observed", "P");
  leg->AddEntry(expected_line, "Expected", "PL");
  leg->AddEntry(expected_1, "#pm1#sigma", "F");
  leg->AddEntry(expected_2, "#pm2#sigma", "F");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->Draw();

  //Add labels to the plot
  TLatex label;
  label.SetNDC();

  //add CMS label
  label.SetTextFont(61);
  label.SetTextColor(1);
  label.SetTextSize(0.06);
  label.SetTextAlign(11);
  label.SetTextAngle(0);
  label.DrawLatex(0.24, 0.83, "CMS");
  if(preliminary_) {
    label.SetTextFont(52);
    label.SetTextSize(0.76*label.GetTextSize());
    label.SetTextAngle(0);
    label.DrawLatex(0.24, 0.78, "Preliminary");
  }

  //add the luminosity label
  label.SetTextFont(42);
  label.SetTextSize(0.035);
  label.SetTextAlign(31);
  label.SetTextAngle(0);
  const double lum = 137.64;
  label.DrawLatex(0.90, 0.91, Form("%.0f fb^{-1} (13 TeV)",lum));

  //Draw the category labels
  label.SetTextFont(62);
  label.SetTextAlign(32);
  label.SetTextSize(0.035);
  for(int icard = 0; icard < nfiles; ++icard) {
    const double yrange = ymax - ymin;
    const double yloc = 0.1 + 0.8*(y[icard]-ymin)/yrange;
    if(add_values_)
      label.DrawLatex(0.30, yloc, Form("%s: %.2e", configs[icard].label_.Data(), expected[icard]));
    else
      label.DrawLatex(0.20, yloc, Form("%s", configs[icard].label_.Data()));
  }
  // label.DrawLatex(0.1, 0.38, Form("%.1e", expected[1]));
  // label.DrawLatex(0.1, 0.18, Form("%.1e", expected[2]));

  gSystem->Exec("[ ! -d limits ] && mkdir limits");
  c->Print(Form("limits/limits_%s_%s.pdf", selection.Data(), tag.Data()));
  return 0;
}
